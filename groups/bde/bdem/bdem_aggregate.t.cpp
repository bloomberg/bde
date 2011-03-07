// bdem_aggregate.t.cpp                                               -*-C++-*-
#include <bdem_aggregate.h>

#include <bdem_descriptor.h>
#include <bdem_elemattrlookup.h>
#include <bdem_elemref.h>

#include <bdetu_unset.h>

#include <bdex_byteinstream.h>
#include <bdex_byteoutstream.h>
#include <bdex_testinstream.h>                  // for testing only
#include <bdex_testinstreamexception.h>         // for testing only
#include <bdex_testoutstream.h>                 // for testing only

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>       // for testing only
#include <bsls_platform.h>                      // for testing only
#include <bsls_types.h>                         // for testing only

#include <bsl_iostream.h>
#include <bsl_strstream.h>
#include <bsl_vector.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>                // memset(), memcpy()
#include <bsl_c_ctype.h>                // isspace()

#ifdef BSLS_PLATFORM__OS_WINDOWS
#include <crtdbg.h>  // _CrtSetReportMode, to suppress popups
#endif

#ifdef BSLS_PLATFORM__OS_LINUX
    #define hex bsl::hex
    #define dec bsl::dec
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// This component expose four classes, 'bdem_RowData', 'bdem_Row', 'bdem_List'
// and 'bdem_Table'.  Since 'bdem_RowData' does not have public constructors it
// is not tested directly.  The majority of the manipulators and accessors
// of 'bdem_Row' are used by 'bdem_List' for forwarding.  These functions of
// 'bdem_Row' are tested as part of 'bdem_List' tests.
//
// Like other bde components, this component is alias-safe.  The simple
// case is when the parameter to a member function is a reference to the object
// itself.  For aggregate objects, more complex situations are possible.  These
// include when the parameter is part of the object (including aggregates
// within other aggregates) and when the object itself is part of the
// parameter.  Whenever these circumstances could occur specific tests are
// included in individual test cases.
//
//-----------------------------------------------------------------------------
//
// 'bdem_Row' public interface
// [19]  bdem_Row& operator=(const bdem_Row& rhs);
// [10]  void makeAllNull();
// [10]  void makeValueNull(int index);
// [10]  void makeRangeNull(int si, int ne);
// [25]  void replaceValue(int di, const bdem_Row& sr, int si);
// [25]  void replaceValues(int di, const bdem_Row& sr, int si, int ne);
// [29]  char& theChar(int index);
// [29]  short& theShort(int index);
// [29]  int& theInt(int index);
// [29]  bsls_Types::Int64& theInt64(int index);
// [29]  float& theFloat(int index);
// [29]  double& theDouble(int index);
// [29]  bsl::string& theString(int index);
// [29]  bdet_Datetime& theDatetime(int index);
// [29]  bdet_Date& theDate(int index);
// [29]  bdet_Time& theTime(int index);
// [29]  vector<char>& theCharArray(int index);
// [29]  vector<short>& theShortArray(int index);
// [29]  vector<int>& theIntArray(int index);
// [29]  vector<Int64>& theInt64Array(int index);
// [29]  vector<float>& theFloatArray(int index);
// [29]  vector<double>& theDoubleArray(int index);
// [29]  vector<bsl::string>& theStringArray(int index);
// [29]  vector<bdet_Datetime>& theDatetimeArray(int index);
// [29]  vector<bdet_Date>& theDateArray(int index);
// [29]  vector<bdet_Time>& theTimeArray(int index);
// [29]  bdem_List& theList(int index);
// [29]  bdem_Table& theTable(int index);
// [  ]  bdex_InStream& bdexStreamIn(bdex_InStream& stream);
// [31]  int length() const;
// [31]  bdem_ElemType::Type elemType(int index) const;
// [29]  const char& theChar(int index) const;
// [29]  const short& theShort(int index) const;
// [29]  const int& theInt(int index) const;
// [29]  const Int64& theInt64(int index) const;
// [29]  const float& theFloat(int index) const;
// [29]  const double& theDouble(int index) const;
// [29]  const bsl::string& theString(int index) const;
// [29]  const bdet_Datetime& theDatetime(int index) const;
// [29]  const bdet_Date& theDate(int index) const;
// [29]  const bdet_Time& theTime(int index) const;
// [29]  const vector<char>& theCharArray(int index) const;
// [29]  const vector<short>& theShortArray(int index) const;
// [29]  const vector<int>& theIntArray(int index) const;
// [29]  const vector<Int64>& theInt64Array(int index) const;
// [29]  const vector<float>& theFloatArray(int index) const;
// [29]  const vector<double>& theDoubleArray(int index) const;
// [29]  const vector<bsl::string>& theStringArray(int index) const;
// [29]  const vector<bdet_Datetime>& theDatetimeArray(int index) const;
// [29]  const vector<bdet_Date>& theDateArray(int index) const;
// [29]  const vector<bdet_Time>& theTimeArray(int index) const;
// [29]  const bdem_List& theList(int index) const;
// [29]  const bdem_Table& theTable(int index) const;
// [  ]  bdex_OutStream& bdexStreamOut(bdex_OutStream& stream) const;
// [30]  ostream& print(ostream& stream, int level, int spl) const;
//
// [ 6]  bool operator==(const bdem_Row& lhs, const bdem_Row& rhs);
// [ 6]  bool operator!=(const bdem_Row& lhs, const bdem_Row& rhs);
// [ 5]  ostream& operator<<(ostream& stream, const bdem_Row& rhs);
//
// 'bdem_List' public interface
// [11]  bdem_List(bslma_Allocator *ba = 0);
// [11]  bdem_List(bdem_AggregateOption::AllocationStrategy allocMode, *ba=0);
// [11]  bdem_List(const ExplicitRow& original, *ba = 0);
// [11]  bdem_List(const bdem_List& original, *ba = 0);
// [11]  bdem_List(const ExplicitRow& original, allocationMode, *ba = 0);
// [11]  bdem_List(const bdem_List& original, allocationMode, *ba = 0);
// [ 1]  ~bdem_List();
// [19]  bdem_List& operator=(const bdem_Row& rhs);
// [19]  bdem_List& operator=(const bdem_List& rhs);
// [29]  bdem_Row& theRow();
// [29]  char& theChar(int index);
// [29]  short& theShort(int index);
// [29]  int& theInt(int index);
// [29]  Int64& theInt64(int index);
// [29]  float& theFloat(int index);
// [29]  double& theDouble(int index);
// [29]  bsl::string& theString(int index);
// [29]  bdet_Datetime& theDatetime(int index);
// [29]  bdet_Date& theDate(int index);
// [29]  bdet_Time& theTime(int index);
// [29]  vector<char>& theCharArray(int index);
// [29]  vector<short>& theShortArray(int index);
// [29]  vector<int>& theIntArray(int index);
// [29]  vector<Int64>& theInt64Array(int index);
// [29]  vector<float>& theFloatArray(int index);
// [29]  vector<double>& theDoubleArray(int index);
// [29]  vector<bsl::string>& theStringArray(int index);
// [29]  vector<bdet_Datetime>& theDatetimeArray(int index);
// [29]  vector<bdet_Date>& theDateArray(int index);
// [29]  vector<bdet_Time>& theTimeArray(int index);
// [29]  bdem_List& theList(int index);
// [29]  bdem_Table& theTable(int index);
// [16]  char& appendChar(char v);
// [16]  short& appendShort(short v);
// [16]  int& appendInt(int v);
// [16]  Int64& appendInt64(Int64 v);
// [16]  float& appendFloat(float v);
// [16]  double& appendDouble(double v);
// [16]  bsl::string& appendString(const char *v);
// [16]  bsl::string& appendString(const bsl::string& v);
// [16]  bdet_Datetime& appendDatetime(const bdet_Datetime& v);
// [16]  bdet_Date& appendDate(const bdet_Date& v);
// [16]  bdet_Time& appendTime(const bdet_Time& v);
// [16]  vector<char>& appendCharArray(const vector<char>& v);
// [16]  vector<short>& appendShortArray(const vector<short>& v);
// [16]  vector<int>& appendIntArray(const vector<int>& v);
// [16]  vector<Int64>& appendInt64Array(const vector<Int64>& v);
// [16]  vector<float>& appendFloatArray(const vector<float>& v);
// [16]  vector<double>& appendDoubleArray(const vector<double>& v);
// [16]  vector<bsl::string>& appendStringArray(const vector<bsl::string>& v);
// [16]  vector<bdet_Datetime>&
//               appendDatetimeArray(const const vector<bdet_Datetime>& v);
// [16]  vector<bdet_Date>& appendDateArray(const vector<bdet_Date>& v);
// [16]  vector<bdet_Time>& appendTimeArray(const vector<bdet_Time>& v);
// [16]  bdem_List& appendList(const bdem_List& list);
// [16]  bdem_List& appendList(const bdem_Row& row);
// [16]  bdem_Table& appendTable(const bdem_Table& table);
// [16]  int appendElement(const bdem_Row& sr, int si);
// [16]  int appendElement(const bdem_List& sl, int si);
// [16]  void appendElements(const bdem_Row& sr);
// [16]  void appendElements(const bdem_List& sl);
// [16]  void appendElements(const bdem_Row& sr, int si, int ne);
// [16]  void appendElements(const bdem_List& sl, int si, int ne);
// [16]  char& insertChar(int di, char v);
// [16]  short& insertShort(int di, short v);
// [16]  int& insertInt(int di, int v);
// [16]  Int64& insertInt64(int di, Int64 v);
// [16]  float& insertFloat(int di, float v);
// [16]  double& insertDouble(int di, double v);
// [16]  bsl::string& insertString(int di, const char *v);
// [16]  bsl::string& insertString(int di, const bsl::string& v);
// [16]  bdet_Datetime& insertDatetime(int di, const bdet_Datetime& v);
// [16]  bdet_Date& insertDate(int di, const bdet_Date& v);
// [16]  bdet_Time& insertTime(int di, const bdet_Time& v);
// [16]  vector<char>& insertCharArray(int di, const vector<char>& v);
// [16]  vector<short>& insertShortArray(int di, const vector<short>& v);
// [16]  vector<int>& insertIntArray(int di, const vector<int>& v);
// [16]  vector<Int64>& insertInt64Array(int di, const vector<Int64>& v);
// [16]  vector<float>& insertFloatArray(int di, const vector<float>& v);
// [16]  vector<double>& insertDoubleArray(int di, const vector<double>& v);
// [16]  vector<bsl::string>&
//                     insertStringArray(int di, const vector<bsl::string>& v);
// [16]  vector<bdet_Datetime>&
//             insertDatetimeArray(int di, const vector<bdet_Datetime>& v);
// [16]  vector<bdet_Date>&
//                     insertDateArray(int di, const vector<bdet_Date>& v);
// [16]  vector<bdet_Time>&
//                     insertTimeArray(int di, const vector<bdet_Time>& v);
// [16]  bdem_List& insertList(int di, const bdem_List& sl);
// [16]  bdem_List& insertList(int di, const bdem_Row& sr);
// [16]  bdem_Table& insertTable(int di, const bdem_Table& st);
// [16]  void insertElement(int di, const bdem_Row& sr, int si);
// [16]  void insertElement(int di, const bdem_List& sl, int si);
// [16]  void insertElements(int di, const bdem_Row& sr);
// [16]  void insertElements(int di, const bdem_List& sl);
// [16]  void insertElements(int di, const bdem_Row& sr, int si, int ne);
// [16]  void insertElements(int di, const bdem_List& sl, int si, int ne);
// [10]  void makeAllNull();
// [10]  void makeValueNull(int index);
// [10]  void makeRangeNull(int si, int ne);
// [16]  void remove(int index);
// [16]  void remove(int si, int ne);
// [16]  void removeAll();
// [17]  void replace(int di, const bdem_Row& sr, int si);
// [17]  void replace(int di, const bdem_List& sl, int si);
// [17]  void replace(int di, const bdem_Row& sr, int si, int ne);
// [17]  void replace(int di, const bdem_List& sl, int si, int ne);
// [15]  void replaceType(int di, bdem_ElemType::Type elemType);
// [15]  void compact();
// [ 7]  void swap(int index1, int index2);
// [13]  bdex_InStream& bdexStreamIn(bdex_InStream& stream);
// [31]  int length() const;
// [31]  bdem_ElemType::Type elemType(int index) const;
// [29]  const bdem_Row& theRow() const;
// [29]  const char& theChar(int index) const;
// [29]  const short& theShort(int index) const;
// [29]  const int& theInt(int index) const;
// [29]  const Int64& theInt64(int index) const;
// [29]  const float& theFloat(int index) const;
// [29]  const double& theDouble(int index) const;
// [29]  const bsl::string& theString(int index) const;
// [29]  const bdet_Datetime& theDatetime(int index) const;
// [29]  const bdet_Date& theDate(int index) const;
// [29]  const bdet_Time& theTime(int index) const;
// [29]  const vector<char>& theCharArray(int index) const;
// [29]  const vector<short>& theShortArray(int index) const;
// [29]  const vector<int>& theIntArray(int index) const;
// [29]  const vector<Int64>& theInt64Array(int index) const;
// [29]  const vector<float>& theFloatArray(int index) const;
// [29]  const vector<double>& theDoubleArray(int index) const;
// [29]  const vector<bsl::string>& theStringArray(int index) const;
// [29]  const vector<bdet_Datetime>& theDatetimeArray(int index) const;
// [29]  const vector<bdet_Date>& theDateArray(int index) const;
// [29]  const vector<bdet_Time>& theTimeArray(int index) const;
// [29]  const bdem_List& theList(int index) const;
// [29]  const bdem_Table& theTable(int index) const;
// [13]  bdex_OutStream& bdexStreamOut(bdex_OutStream& stream) const;
// [30]  ostream& print(ostream& stream, int level, int spl) const;
//
// [ 6]  bool operator==(const bdem_List& lhs, const bdem_List& rhs);
// [ 6]  bool operator!=(const bdem_List& lhs, const bdem_List& rhs);
// [ 5]  ostream& operator<<(ostream& stream, const bdem_List& rhs);
//
// 'bdem_Table' public interface
// [12]  bdem_Table(bslma_Allocator *ba = 0);
// [12]  bdem_Table(bdem_AggregateAllocationMode::AllocationStrategy, *ba = 0);
// [12]  bdem_Table(const bdem_Table& original, *ba = 0);
// [12]  bdem_Table(const bdem_Table& original, allocationMode, *ba = 0);
// [ 2]  ~bdem_Table();
// [20]  bdem_Table& operator=(const bdem_Table& rhs);
// [29]  bdem_Row& theRow(int ri);
// [29]  char& theChar(int ri, int ci);
// [29]  short& theShort(int ri, int ci);
// [29]  int& theInt(int ri, int ci);
// [29]  Int64& theInt64(int ri, int ci);
// [29]  float& theFloat(int ri, int ci);
// [29]  double& theDouble(int ri, int ci);
// [29]  bsl::string& theString(int ri, int ci);
// [29]  bdet_Datetime& theDatetime(int ri, int ci);
// [29]  bdet_Date& theDate(int ri, int ci);
// [29]  bdet_Time& theTime(int ri, int ci);
// [29]  vector<char>& theCharArray(int ri, int ci);
// [29]  vector<short>& theShortArray(int ri, int ci);
// [29]  vector<int>& theIntArray(int ri, int ci);
// [29]  vector<Int64>& theInt64Array(int ri, int ci);
// [29]  vector<float>& theFloatArray(int ri, int ci);
// [29]  vector<double>& theDoubleArray(int ri, int ci);
// [29]  vector<bsl::string>& theStringArray(int ri, int ci);
// [29]  vector<bdet_Datetime>& theDatetimeArray(int ri, int ci);
// [29]  vector<bdet_Date>& theDateArray(int ri, int ci);
// [29]  vector<bdet_Time>& theTimeArray(int ri, int ci);
// [29]  bdem_List& theList(int ri, int ci);
// [29]  bdem_Table& theTable(int ri, int ci);
// [23]  void makeValueNull(int ri, int ci);
// [23]  void makeColumnNull(int index);
// [23]  void makeRowsNull(int si, int rc);
// [23]  void makeAllNull();
// [18]  bdem_Row& appendRow(const bdem_List& sl);
// [18]  bdem_Row& appendRow(const bdem_Table& st, int si);
// [18]  void appendRows(const bdem_Table& st);
// [18]  void appendRows(const bdem_Table& st, int si, int rc);
// [18]  bdem_Row& insertRow(int di, const bdem_List& sl);
// [18]  bdem_Row& insertRow(int di, const bdem_Table& st, int si);
// [18]  void insertRows(int di, const bdem_Table& st);
// [18]  void insertRows(int di, const bdem_Table& st, int si, int rc);
// [21]  void replaceRow(int di, const bdem_Row& sr);
// [21]  void replaceRow(int di, const bdem_List& sl);
// [21]  void replaceRow(int di, const bdem_Table& st, int si);
// [21]  void replaceRows(int di, const bdem_Table& st, int si, int rc);
// [27]  void swapRows(int ri1, int ri2);
// [18]  void removeRow(int index);
// [18]  void removeRows(int index, int rc);
// [18]  void removeAllRows();
// [18]  void removeAll();
// [14]  bdex_InStream& bdexStreamIn(bdex_InStream& stream);
// [31]  int numRows() const;
// [31]  int numColumns() const;
// [31]  bdem_ElemType::Type columnType(int index) const;
// [29]  const bdem_Row& theRow(int ri) const;
// [29]  const char& theChar(int ri, int ci) const;
// [29]  const short& theShort(int ri, int ci) const;
// [29]  const int& theInt(int ri, int ci) const;
// [29]  const Int64& theInt64(int ri, int ci) const;
// [29]  const float& theFloat(int ri, int ci) const;
// [29]  const double& theDouble(int ri, int ci) const;
// [29]  const bsl::string& theString(int ri, int ci) const;
// [29]  const bdet_Datetime& theDatetime(int ri, int ci) const;
// [29]  const bdet_Date& theDate(int ri, int ci) const;
// [29]  const bdet_Time& theTime(int ri, int ci) const;
// [29]  const vector<char>& theCharArray(int ri, int ci) const;
// [29]  const vector<short>& theShortArray(int ri, int ci) const;
// [29]  const vector<int>& theIntArray(int ri, int ci) const;
// [29]  const vector<Int64>& theInt64Array(int ri, int ci) const;
// [29]  const vector<float>& theFloatArray(int ri, int ci) const;
// [29]  const vector<double>& theDoubleArray(int ri, int ci) const;
// [29]  const vector<bsl::string>& theStringArray(int ri, int ci) const;
// [29]  const vector<bdet_Datetime>&
//                                      theDatetimeArray(int ri, int ci) const;
// [29]  const vector<bdet_Date>& theDateArray(int ri, int ci) const;
// [29]  const vector<bdet_Time>& theTimeArray(int ri, int ci) const;
// [29]  const bdem_List& theList(int ri, int ci) const;
// [29]  const bdem_Table& theTable(int ri, int ci) const;
// [14]  bdex_OutStream& bdexStreamOut(bdex_OutStream& stream) const;
// [30]  ostream& print(ostream& stream, int level, int spl) const;
//
// [24]  bool operator==(const bdem_Table& lhs, const bdem_Table& rhs);
// [24]  bool operator!=(const bdem_Table& lhs, const bdem_Table& rhs);
// [ 5]  ostream& operator<<(ostream& stream, const bdem_Table& rhs);
//-----------------------------------------------------------------------------
// [ 1]  'bdem_List' Breathing Test
// [ 2]  'bdem_Table' Breathing Test
// [ 3]  bdem_List& gg(bdem_List *address, const char *spec);
// [ 3]  int ggg(bdem_List *address, const char *spec, int showErrorFlag);
// [ 4]  int hhh(bdem_Table *address, const char *spec, int showErrorFlag);
// [ 4]  bdem_Table& hh(bdem_Table *object, const char *spec);
// [ 8]  void reverseList(bdem_List *list); helper function
// [ 8]  void reverseSpec(char *spec); helper function
// [40]  USAGE Example

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

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
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define PR(X) cout << #X " = "; X.print(cout); cout << endl; // Print id+value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define PR_(X) cout << #X " = "; X.print(cout); cout << flush; // no '\n'
#define L_ __LINE__                           // current Line number
#define PS(X) cout << #X " = \n" << (X) << endl; // Print identifier and value.
#define T_()  cout << "\t" << flush;          // Print a tab (w/o newline)

#define NUM_ARRYELEMS(a) (sizeof(a) / sizeof(a[0]))

//==========================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//--------------------------------------------------------------------------
typedef bdem_Row   Row;
typedef bdem_List  List;
typedef bdem_Table Table;
typedef bdex_TestInStream  In;
typedef bdex_TestOutStream Out;

typedef bsls_Types::Int64  Int64;

const int NUM_TYPES = bdem_ElemType::BDEM_NUM_TYPES;

#if defined(BDE_BUILD_TARGET_EXC) && !defined(BSLS_PLATFORM__CMP_MSVC)

static bool EXCEPTIONS_ON = true;

#else

static bool EXCEPTIONS_ON = false;

#endif

//--------------------------------------------------------------------------

typedef bsl::vector<bdem_ElemType::Type> my_ElemTypeArray;

//==========================================================================
//                  FUNCTIONS AND OPERATORS FOR TESTING
//--------------------------------------------------------------------------

// Print out a vector of any type that already knows how to print itself.
template <class TYPE>
ostream& operator<<(ostream& stream, vector<TYPE> vec)
{
    stream << "[ ";
    int len = vec.size();
    for (int i = 0; i < len; ++i) {
        if (i) stream << " ";
        stream << vec[i];
    }
    stream << " ]\n";

    return stream;
}

void getElemTypes(my_ElemTypeArray *types, const bdem_Row& row)
    // Fill types with element types from row
{
    int numTypes = row.length();
    types->clear();
    types->reserve(numTypes);
    for (int i = 0; i < numTypes; ++i)
    {
        types->push_back(row.elemType(i));
    }
}

inline
void getElemTypes(my_ElemTypeArray *types, const bdem_List& list)
    // Fill types with element types from list
{
    getElemTypes(types, list.row());
}

void getElemTypes(my_ElemTypeArray *types, const bdem_Table& table)
    // Fill types with element types from table
{
    int numTypes = table.numColumns();
    types->clear();
    types->reserve(numTypes);
    for (int i = 0; i < numTypes; ++i)
    {
        types->push_back(table.columnType(i));
    }
}

void replaceListElements(bdem_List&       list,
                         int              dstIndex,
                         const bdem_Row&  srcRow,
                         int              srcIndex,
                         int              numElements)
{
    bdem_List tempList(srcRow);
    for(int i = 0; i < numElements; ++i) {
        list.replaceElement(dstIndex + i, tempList[srcIndex + i]);
    }
}

void resetListElements(bdem_List&       list,
                       int              dstIndex,
                       const my_ElemTypeArray& srcTypes,
                       int              srcIndex,
                       int              numElements)
{
    for(int i=0; i<numElements; i++) {
        list.resetElement(dstIndex+i, srcTypes[srcIndex+i]);
    }
}

void makeRangeNull(bdem_Row&  row, int startIndex, int numElements)
{
    for(int i = 0; i < numElements; ++i) {
        row[startIndex + i].makeNull();
    }
}

void replaceRows(bdem_Table&       table,
                 int               dstIndex,
                 const bdem_Table& src,
                 int               srcIndex,
                 int               numElems)
{
    table.insertRows(dstIndex, src, srcIndex, numElems);
    table.removeRows(dstIndex + numElems, numElems);
}

void hexdump(const void *memory_arg, int length) {
    const char *memory = (const char *) memory_arg;

    int i = 0;
    printf("\"");
    for (; i < length; ++i) {
        if (5 == i % 15 || 10 == i % 15) {
            printf("\"  \"");
        }
        if (i && 0 == i % 15) {
            printf("\"\n\"");
        }
        printf("\\x%02x", (int) (unsigned char) *memory++);
    }
    printf("\"\n");
}

const char *charToTypeString(char typeChar)
   // return a string representing the supported type indicated by the
   // specified 'typeChar'.
{
    const char *typeStrings[] = {
        "Char",           // A
        "Short",          // B
        "Int",            // C
        "Int64",          // D
        "Float",          // E
        "Double",         // F
        "String",         // G
        "Datetime",       // H
        "Date",           // I
        "Time",           // J
        "CharArray",      // K
        "ShortArray",     // L
        "IntArray",       // M
        "Int64Array",     // N
        "FloatArray",     // O
        "DoubleArray",    // P
        "StringArray",    // Q
        "DatetimeArray",  // R
        "DateArray",      // S
        "TimeArray",      // T
        "List",           // U
        "Table",          // V

        "Bool",           // W
        "DateTimeTz",     // X
        "DateTz",         // Y
        "TimeTz",         // Z
        "BoolArray",      // a
        "DateTimeTzArray",// b
        "DateTzArray",    // c
        "TimeTzArray",    // d
        "Choice",         // e
        "ChoiseArray",    // f
    };
    const char *allTypes = "ABCDEFGHIJKLMNOPQRSTUV";
    const char *position = strchr(allTypes, typeChar);
    if(position == 0) return "";

    return typeStrings[position - allTypes];
}

struct Assertions {
    char assert1[0 == bdem_AggregateOption::BDEM_PASS_THROUGH];
    char assert2[1 == bdem_AggregateOption::BDEM_WRITE_MANY];
    char assert3[2 == bdem_AggregateOption::BDEM_SUBORDINATE];
    char assert4[3 == bdem_AggregateOption::BDEM_WRITE_ONCE];
};

const char *optionToString(bdem_AggregateOption::AllocationStrategy optval)
   // return a string representing the pool option indicated by the
   // specified 'optval'.
{
    const char *typeStrings[] = {
        "BDEM_PASS_THROUGH",
        "BDEM_WRITE_MANY",
        "BDEM_SUBORDINATE",
        "BDEM_WRITE_ONCE",
    };

    if ((int)optval < 0
     || (int)optval >= (int)(sizeof(typeStrings) / sizeof(const char))) {
        return "";
    }
    return typeStrings[(int) optval];
}

enum CompareEnum { COMPARE_LT = -2, COMPARE_LE = -1, COMPARE_EQ = 0,
    COMPARE_GE = 1, COMPARE_GT = 2 };

static int compareValues (int lhs, int rhs, CompareEnum expected)
    // compare two specified signed integer values 'lhs' and 'rhs'
    // and return true if the comparison is as expected.
{
    if (lhs < rhs) {
        return expected < 0;
    }
    else if (lhs > rhs) {
        return expected > 0;
    }
    else {
        return expected != COMPARE_LT && expected != COMPARE_GT;
    }
}

//=============================================================================
//          HELPER FUNCTIONS OPERATING ON 'g' AND 'h' 'spec' CHARACTERS
//
// These functions identify the 'type' and 'value' characters of the 'g' and
// 'h' languages, and convert to and from 'char' (string) and 'int' (index)
// representations.  Some of these are used by 'ggg' and/or 'hhh' directly, but
// the complete set is made available for general use.
//-----------------------------------------------------------------------------

static bool isTypeChar(char c)
    // Return 1 if the specified character 'c' is in "ABCDEFGHIJKLMNOPQRSTUV",
    // and 0 otherwise.
{
    static const char TYPE_CHARS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef";
    return 0 != c && 0 != strchr(TYPE_CHARS, c);
}

static bool isValChar(char c)
    // Return 1 if the specified character, 'c', is in "xyu", and 0 otherwise.
{
    return 'x' == c || 'y' == c || 'u' == c || 'n' == c;
}

static int typeCharToIndex(char type)
    // Return the integer index of the specified 'type' character.  The
    // behavior is undefined unless 'type' is one of "ABCDEFGHIJKLMNOPQRSTUV".
{
    ASSERT(isTypeChar(type));
    static const char TYPE_CHARS[] = "ABCDEFGHIJKLMNOPQRSTUV";
    return strchr(TYPE_CHARS, type) - TYPE_CHARS;
}

static int valCharToIndex(char val)
    // Return the integer index of the specified 'val' character.  The behavior
    // is undefined unless 'val' is one of "xyu".
{
    ASSERT(isValChar(val));
    if ('n' == val) {
        val = 'u';
    }
    static const char VAL_CHARS[] = "xyu";
    return strchr(VAL_CHARS, val) - VAL_CHARS;
}

static char indexToTypeChar(int index)
    // Return the type character that corresponds to the specified 'index'.
    // The behavior is undefined unless 0 <= index < 22.
{
    ASSERT(0 <= index && index < 22);
    static const char TYPE_CHARS[] = "ABCDEFGHIJKLMNOPQRSTUV";
    return TYPE_CHARS[index];
}

//=============================================================================
//                        TEST DATA POINTER ARRAYS
//
// This set of 66 pointers (22 array-of-3 types) are defined here at global
// scope because they are used by the 'ggg' helper 'appendElement'.  They are
// populated in 'main' after the definition of the specific test values, but
// are available as needed to various additional helper functions.
//-----------------------------------------------------------------------------

static const char                      *G_VALUES_A[3];  // { &XA, &YA, &UA }
static const short                     *G_VALUES_B[3];  // { &XB, &YB, &UB }
static const int                       *G_VALUES_C[3];  // { &XC, &YC, &UC }
static const Int64                     *G_VALUES_D[3];  // ...
static const float                     *G_VALUES_E[3];
static const double                    *G_VALUES_F[3];
static const bsl::string               *G_VALUES_G[3];
static const bdet_Datetime             *G_VALUES_H[3];
static const bdet_Date                 *G_VALUES_I[3];
static const bdet_Time                 *G_VALUES_J[3];
static const bsl::vector<char>          *G_VALUES_K[3];
static const bsl::vector<short>         *G_VALUES_L[3];
static const bsl::vector<int>           *G_VALUES_M[3];
static const bsl::vector<Int64>         *G_VALUES_N[3];
static const bsl::vector<float>         *G_VALUES_O[3];
static const bsl::vector<double>        *G_VALUES_P[3];
static const bsl::vector<bsl::string>   *G_VALUES_Q[3];
static const bsl::vector<bdet_Datetime> *G_VALUES_R[3];
static const bsl::vector<bdet_Date>     *G_VALUES_S[3];
static const bsl::vector<bdet_Time>     *G_VALUES_T[3];
static const bdem_List                 *G_VALUES_U[3];
static const bdem_Table                *G_VALUES_V[3];  // { &XV, &YV, &UV }

static const bool                      *G_VALUES_W[3];
static const bdet_DatetimeTz           *G_VALUES_X[3];
static const bdet_DateTz               *G_VALUES_Y[3];
static const bdet_TimeTz               *G_VALUES_Z[3];
static const bsl::vector<bool>         *G_VALUES_a[3];
static const bsl::vector<bdet_DatetimeTz> *G_VALUES_b[3];
static const bsl::vector<bdet_DateTz>  *G_VALUES_c[3];
static const bsl::vector<bdet_TimeTz>  *G_VALUES_d[3];
static const bdem_Choice               *G_VALUES_e[3];
static const bdem_ChoiceArray          *G_VALUES_f[3];

//=============================================================================
//                     HELPER FUNCTIONS FOR 'ggg', 'hhh'
//
// These functions are used by 'ggg' and/or 'hhh' directly.  They are also
// available as general-purpose helpers.
//-----------------------------------------------------------------------------

template <typename TYPE>
const TYPE *vectorData(const bsl::vector<TYPE>& v)
    // Return a pointer to the first element of 'v' or null if 'v' is empty.
{
    return (v.empty() ? 0 : &v.front());
}

static bool
listInvariants(const List& list) {
    bool ret = true;
    int len = list.length();
    bool isOk;
    for (int i = 0; i < len; ++i) {
        bdem_ConstElemRef elemRef(list[i]);
        LOOP_ASSERT(i, isOk = (elemRef.isNonNull()
                            || elemRef.descriptor()->isUnset(elemRef.data())));
        ret &= isOk;
    }
    return ret;
}

static void
bug(const char *spec, int position, const char *description, int showErrorFlag)
    // Write the specified 'spec' and the specified error 'description' to
    // 'cout', identifying the specified 'position' of the errant character in
    // 'spec', only if the specified 'showErrorFlag' is non-zero.
{
    if (showErrorFlag) {
        cout << "spec : " << spec << endl;
        cout << description << " at position " << position;
        if ('\0' != spec[position]) {
            cout << " ('" << spec[position] << "')." << endl;
        }
        else {
            cout << " ('')." << endl;
        }
    }
    return;
}

static void appendElement(List *address, char val, char type)
    // Append an element of the 'bdem_ElemType::Type' corresponding to the
    // specified 'type' character and having the value corresponding to the
    // specified 'val' character to the 'bdem_List' at the specified 'address'.
{
    ASSERT('x' == val ||'y' == val ||'u' == val ||'n' == val);
    int i = 'n' == val ? valCharToIndex('u') : valCharToIndex(val);

    switch (type) {
      case 'A': address->appendChar(*G_VALUES_A[i]);             break;
      case 'B': address->appendShort(*G_VALUES_B[i]);            break;
      case 'C': address->appendInt(*G_VALUES_C[i]);              break;
      case 'D': address->appendInt64(*G_VALUES_D[i]);            break;
      case 'E': address->appendFloat(*G_VALUES_E[i]);            break;
      case 'F': address->appendDouble(*G_VALUES_F[i]);           break;
      case 'G': address->appendString(*G_VALUES_G[i]);           break;
      case 'H': address->appendDatetime(*G_VALUES_H[i]);         break;
      case 'I': address->appendDate(*G_VALUES_I[i]);             break;
      case 'J': address->appendTime(*G_VALUES_J[i]);             break;

      case 'K': address->appendCharArray(*G_VALUES_K[i]);        break;
      case 'L': address->appendShortArray(*G_VALUES_L[i]);       break;
      case 'M': address->appendIntArray(*G_VALUES_M[i]);         break;
      case 'N': address->appendInt64Array(*G_VALUES_N[i]);       break;
      case 'O': address->appendFloatArray(*G_VALUES_O[i]);       break;
      case 'P': address->appendDoubleArray(*G_VALUES_P[i]);      break;
      case 'Q': address->appendStringArray(*G_VALUES_Q[i]);      break;
      case 'R': address->appendDatetimeArray(*G_VALUES_R[i]);    break;
      case 'S': address->appendDateArray(*G_VALUES_S[i]);        break;
      case 'T': address->appendTimeArray(*G_VALUES_T[i]);        break;

      case 'U': address->appendList(*G_VALUES_U[i]);             break;
      case 'V': address->appendTable(*G_VALUES_V[i]);            break;

      case 'W': address->appendBool(*G_VALUES_W[i]);             break;
      case 'X': address->appendDatetimeTz(*G_VALUES_X[i]);       break;
      case 'Y': address->appendDateTz(*G_VALUES_Y[i]);           break;
      case 'Z': address->appendTimeTz(*G_VALUES_Z[i]);           break;
      case 'a': address->appendBoolArray(*G_VALUES_a[i]);        break;
      case 'b': address->appendDatetimeTzArray(*G_VALUES_b[i]);  break;
      case 'c': address->appendDateTzArray(*G_VALUES_c[i]);      break;
      case 'd': address->appendTimeTzArray(*G_VALUES_d[i]);      break;
      case 'e': address->appendChoice(*G_VALUES_e[i]);           break;
      case 'f': address->appendChoiceArray(*G_VALUES_f[i]);      break;

      default:  P(val);  P(type);  ASSERT(!"Bad type character");
    }
    if ('n' == val) {
        (*address)[address->length() - 1].makeNull();
    }
    return;
}

static void appendToElemTypeArray(my_ElemTypeArray *e, const char *spec)
    // Append to the specified 'bdet_ElemTypeArray' 'e' the sequence of types
    // in the specified 'spec'.
{
    for (; *spec; ++spec) {
        e->push_back(bdem_ElemType::Type(typeCharToIndex(*spec)));
    }
    return;
}

//-----------------------------------------------------------------------------
//       ISUNSET FUNCTION
//-----------------------------------------------------------------------------

bool isUnset(const bdem_ConstElemRef& e)
{
    switch (e.type()) {
      case bdem_ElemType::BDEM_CHAR:
        return *G_VALUES_A[2] == e.theChar();
      case bdem_ElemType::BDEM_SHORT:
        return *G_VALUES_B[2] == e.theShort();
      case bdem_ElemType::BDEM_INT:
        return *G_VALUES_C[2] == e.theInt();
      case bdem_ElemType::BDEM_INT64:
        return *G_VALUES_D[2] == e.theInt64();
      case bdem_ElemType::BDEM_FLOAT:
        return *G_VALUES_E[2] == e.theFloat();
      case bdem_ElemType::BDEM_DOUBLE:
        return *G_VALUES_F[2] == e.theDouble();
      case bdem_ElemType::BDEM_STRING:
        return *G_VALUES_G[2] == e.theString();
      case bdem_ElemType::BDEM_DATETIME:
        return *G_VALUES_H[2] == e.theDatetime();
      case bdem_ElemType::BDEM_DATE:
        return *G_VALUES_I[2] == e.theDate();
      case bdem_ElemType::BDEM_TIME:
        return *G_VALUES_J[2] == e.theTime();
      case bdem_ElemType::BDEM_CHAR_ARRAY:
        return *G_VALUES_K[2] == e.theCharArray();
      case bdem_ElemType::BDEM_SHORT_ARRAY:
        return *G_VALUES_L[2] == e.theShortArray();
      case bdem_ElemType::BDEM_INT_ARRAY:
        return *G_VALUES_M[2] == e.theIntArray();
      case bdem_ElemType::BDEM_INT64_ARRAY:
        return *G_VALUES_N[2] == e.theInt64Array();
      case bdem_ElemType::BDEM_FLOAT_ARRAY:
        return *G_VALUES_O[2] == e.theFloatArray();
      case bdem_ElemType::BDEM_DOUBLE_ARRAY:
        return *G_VALUES_P[2] == e.theDoubleArray();
      case bdem_ElemType::BDEM_STRING_ARRAY:
        return *G_VALUES_Q[2] == e.theStringArray();
      case bdem_ElemType::BDEM_DATETIME_ARRAY:
        return *G_VALUES_R[2] == e.theDatetimeArray();
      case bdem_ElemType::BDEM_DATE_ARRAY:
        return *G_VALUES_S[2] == e.theDateArray();
      case bdem_ElemType::BDEM_TIME_ARRAY:
        return *G_VALUES_T[2] == e.theTimeArray();
      case bdem_ElemType::BDEM_LIST:
        return *G_VALUES_U[2] == e.theList();
      case bdem_ElemType::BDEM_TABLE:
        return *G_VALUES_V[2] == e.theTable();
      case bdem_ElemType::BDEM_BOOL:
        return *G_VALUES_W[2] == e.theBool();
      case bdem_ElemType::BDEM_DATETIMETZ:
        return *G_VALUES_X[2] == e.theDatetimeTz();
      case bdem_ElemType::BDEM_DATETZ:
        return *G_VALUES_Y[2] == e.theDateTz();
      case bdem_ElemType::BDEM_TIMETZ:
        return *G_VALUES_Z[2] == e.theTimeTz();
      case bdem_ElemType::BDEM_BOOL_ARRAY:
        return *G_VALUES_a[2] == e.theBoolArray();
      case bdem_ElemType::BDEM_DATETIMETZ_ARRAY:
        return *G_VALUES_b[2] == e.theDatetimeTzArray();
      case bdem_ElemType::BDEM_DATETZ_ARRAY:
        return *G_VALUES_c[2] == e.theDateTzArray();
      case bdem_ElemType::BDEM_TIMETZ_ARRAY:
        return *G_VALUES_d[2] == e.theTimeTzArray();
      case bdem_ElemType::BDEM_CHOICE:
        return *G_VALUES_e[2] == e.theChoice();
      case bdem_ElemType::BDEM_CHOICE_ARRAY:
        return *G_VALUES_f[2] == e.theChoiceArray();
    }

    return false;
}

//=============================================================================
//       GENERATOR FUNCTIONS 'g', 'gg', AND 'ggg' FOR TESTING LISTS
//-----------------------------------------------------------------------------
// The 'g' family of functions generate a 'bdem_List' object for testing.
// They, along with their helpers, interpret a given 'spec' (from left to
// right) to configure the list according to a custom language.  The 22
// characters [A-V] represent the 22 'bdem_ElemType' types.  'x', 'y', and
// 'u' respectively represent two arbitrary but unique values and the 'unset'
// value (of the appropriate type).  Value and type characters must occur in
// pairs and generate the appropriate 'append'.  A tilde ('~') removes all
// elements, leaving the list in the logical (empty and unset) default creation
// state.  The full language specification follows.
//
// LANGUAGE SPECIFICATION:
// -----------------------
//
// <SPEC>        ::= <INSTRUCTION> | <INSTRUCTION><SPEC>
//
// <INSTRUCTION> ::= <VALUE><TYPE> | <REMOVE_ALL> | <EMPTY>
//
// <VALUE>       ::= = 'x' | 'y' | 'u'  // two arbitrary values plus "unset"
//
// <TYPE>        ::= 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' |
//                   'J' | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' |
//                   'S' | 'T' | 'U' | 'V' |    //the 22 'bdem_ElemType' types
//
// <REMOVE_ALL>  ::= '~'
//
// Spec String  Description
// -----------  ---------------------------------------------------------------
// ""           Has no effect; leaves the object unaltered.
// "xC"         Produces: LIST { INT 103 }
// "xCyF"       Produces: LIST { INT 103 DOUBLE 206.006 }
// "xCyFuG"     Produces: LIST { INT 103 DOUBLE 206.006 STRING NULL }
// "xCyFuG~"    Produces: LIST { INT 103 DOUBLE 206.006 STRING NULL }, and then
//              removes all elements.
//-----------------------------------------------------------------------------

enum {
    // 'enum' of parse errors for 'ggg'.
    GGG_SUCCESS = 0,
    GGG_EXPECTED_TYPE_CHARACTER,
    GGG_INAPPROPRIATE_CHARACTER
};

static int ggg(List *address, const char *spec, int showErrorFlag)
    // Configure the modifiable 'bdem_List' at the specified 'address'
    // according to the specified 'spec'.  Report errors to 'cout' only if the
    // specified 'showErrorFlag' is non-zero.  Return 0 on success, and a
    // unique non-zero global error code otherwise.  Note that this function
    // uses only the primary manipulators of the test plan.  Note also that
    // neither pointer argument may be null.
{
    for (const char *p = spec; *p; ++p) {
        if ('~' == *p) {
            address->removeAll();
            continue;
        }
        if (isValChar(*p)) {
            char valChar = *p++;
            if (isTypeChar(*p)) {
                appendElement(address, valChar, *p);
                continue;
            }
            bug(spec, p - spec, "Expected type character", showErrorFlag);
            return GGG_EXPECTED_TYPE_CHARACTER;
        }
        bug(spec, p - spec, "Inappropriate character", showErrorFlag);
        return GGG_INAPPROPRIATE_CHARACTER;
    }
    return GGG_SUCCESS;
}

List& gg(List *address, const char *spec)
    // Configure the modifiable 'bdem_List' object at the specified 'address'
    // according to the specified 'spec', and return a reference to the
    // modifiable list.  Note that this function uses only the primary
    // manipulators of the test plan.  Note also that neither argument may be
    // null.
{
    enum { SHOW_ERROR_FLAG = 1 };
    int status = ggg(address, spec, SHOW_ERROR_FLAG);
    ASSERT(GGG_SUCCESS == status);
    return *address;
}

List g(const char *spec)
    // Return by value a new 'bdem_List' object corresponding to the specified
    // 'spec'.
{
    List object;
    return gg(&object, spec);
}

//=============================================================================
//              GENERATOR FUNCTIONS 'h', 'hh', AND 'hhh' FOR TESTING
//-----------------------------------------------------------------------------
// The 'h' family of functions generate a 'bdem_Table' object for testing.
// They, along with their helpers, interpret a given 'spec' (from left to
// right) to configure the table according to a custom language.  The 22
// characters [A-V] represent the 22 'bdem_ElemType' types.  'x', 'y', and
// 'u' respectively represent two arbitrary but unique values and the 'unset'
// value (of the appropriate type).  A semicolon (';') terminates each type
// field and each value field.  Any value field must have the same number of
// entries as the current type field.  The initial type field corresponds to
// the 'columnTypes' of the input 'bdem_Table'.  When a complete type field is
// recognized, the 'columnTypes' of the table are reset to the appropriate
// value.  When a complete, valid value field is recognized, an appropriate row
// is appended to the table.  A tilde ('~') resets the table to its empty (and
// unset) logical state.  A bang ('!') removes all rows (but does not alter the
// 'columnTypes').  Note that '~' and '!' may not occur within (incomplete)
// type or value fields.  The full language specification follows.
//
// LANGUAGE SPECIFICATION:
// -----------------------
//
// <SPEC>        ::= <SEQ> | <EMPTY>
//
// <SEQ>         ::= <ROWDEF><ROWS> | <REMOVE>
//
// <ROWDEF>      ::= <TYPE><DELIM>
//
// <TYPE>        ::= <T> | <T><TYPE>
//
// <T>           ::= 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' |
//                   'J' | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' |
//                   'S' | 'T' | 'U' | 'V' |    // the 22 'bdem_ElemType' types
//
// <DELIM>       ::= ';'
//
// <ROWS>        ::= <ROW> | <ROW><ROWS> | ^
//
// <ROW>         ::= <VAL><DELIM>
//
// <VAL>         ::= <V> | <V><VAL>
//
// <V>           ::= = 'x' | 'y' | 'u'  // two arbitrary values plus "unset"
//
// <REMOVE>      ::= <REMOVE_ROWS> | <REMOVE_ALL>
//
// <REMOVE_ROWS> ::= '!'
//
// <REMOVE_ALL>  ::= '~'
//
// Spec String  Description
// -----------  ---------------------------------------------------------------
// ""           Has no effect; leaves the object empty.
// "CFG;"       Sets the columnTypes to "INT DOUBLE STRING"
// "CF;xx;"     Sets the columnTypes to "INT DOUBLE" and appends a row of
//              appropriate 'x' values
// "CF;xx;yy;"  Sets the columnTypes to "INT DOUBLE", appends a row of
//              appropriate 'x' values, and then appends a row of appropriate
//              'y' values.
// "CF;xx;yy;!" Sets the columnTypes to "INT DOUBLE", appends a row of
//              appropriate 'x' values, appends a row of appropriate 'y'
//              values, and then removes the rows (leaving the column types
//              intact).
// "CF;xx;yy;~" Sets the columnTypes to "INT DOUBLE", appends a row of
//              appropriate 'x' values, appends a row of appropriate 'y'
//              values, and then removes the rows and resets column types
//              to the initial empty state.
//
//-----------------------------------------------------------------------------

enum {
    // 'enum' of parse errors for 'hhh'.
    HHH_SUCCESS = 0,
    HHH_TYPEFIELD_UNTERMINATED,
    HHH_VALFIELD_UNTERMINATED,
    HHH_MISSING_EXPECTED_VALUE,
    HHH_ILLEGAL_CHARACTER
};

static int hhh(Table *address, const char *spec, int showErrorFlag)
    // Configure the modifiable 'bdem_Table' at the specified 'address'
    // according to the specified 'spec'.  Report errors to 'cout' only if the
    // specified 'showErrorFlag' is non-zero.  Return 0 on success, and a
    // unique non-zero global error code otherwise.  Note that this function
    // uses only the primary manipulators of the test plan.  Note also that
    // neither pointer argument may be null.
{
    ASSERT(address);
    ASSERT(spec);

    const int MAX     = 1000;     // maximum number of columns permitted
    char      buf[MAX * 2 + 1];   // buffer to accumulate types and values
    int       numCols;            // count of types (columns); 0 = "empty"

    // Note that this implementation parses the 'hhh' spec to build up a
    // sequence of 'ggg' specs, each of which is in turn used with 'gg' to
    // configure a temporary list from which the row is extracted to
    // configure the specified table.

    // Note that 'numCols' and the null-terminated 'buf' are 'for'-loop
    // invariants that preserve column-type information.  'buf[2 * i + 1]'
    // holds the column type character for each column index 'i' such that
    // 0 < i < numCols, and buf[2 * numCols] = '\0'.

    my_ElemTypeArray e;
    getElemTypes(&e, *address);
    numCols = e.size();
    for (int i = 0; i < numCols; ++i) {
        buf[2 * i + 1] = indexToTypeChar(e[i]);
    }
    buf[2 * numCols] = '\0';

    for (const char *p = spec; *p; ++p) {
        if ('!' == *p) {
            address->removeAllRows();
        }
        else if ('~' == *p) {
            address->removeAll();
            numCols = 0;  buf[0] = '\0';     // reset column state to "empty"
        }
        else if (isTypeChar(*p)) {
            my_ElemTypeArray tempElems;
            for (numCols = 0; isTypeChar(*p) && numCols < MAX; ++numCols) {
                tempElems.push_back(bdem_ElemType::Type(typeCharToIndex(*p)));
                buf[2 * numCols + 1] = *p++; // rewrite "stateful" 'buf'
            }
            buf[2 * numCols] = '\0';
            if (';' != *p) {
                bug(spec, p - spec, "Missing ';' after last valid type",
                    showErrorFlag);
                return HHH_TYPEFIELD_UNTERMINATED;
            }
            address->reset(tempElems);
        }
        else if (isValChar(*p) || ';' == *p) {
            int numVals;
            for (numVals = 0; isValChar(*p) && numVals < numCols; ++numVals) {
                buf[2 * numVals] = *p++;    // relies on type chars from above
            }
            if (';' != *p) {
                bug(spec, p - spec, "Missing ';' after last valid value",
                    showErrorFlag);
                return HHH_VALFIELD_UNTERMINATED;
            }
            if (numVals < numCols) {
                bug(spec, p - spec, "Missing expected value", showErrorFlag);
                return HHH_MISSING_EXPECTED_VALUE;
            }
            List tempList;
            address->appendRow(gg(&tempList, buf));
        }
        else {
            bug(spec, p - spec, "Illegal character", showErrorFlag);
            return HHH_ILLEGAL_CHARACTER;
        }
    }
    return HHH_SUCCESS;
}

Table& hh(Table *address, const char *spec)
    // Configure the modifiable 'bdem_Table' at the specified 'address'
    // according to the specified 'spec'.  Return a reference to '*address'.
    // Note that this function uses only the primary manipulators of the test
    // plan.  Note also that neither argument may be null.
{
    enum { SHOW_ERROR_FLAG = 1 };
    int status = hhh(address, spec, SHOW_ERROR_FLAG);
    ASSERT(HHH_SUCCESS == status);
    return *address;
}

Table h(const char *spec)
    // Return by value a new 'bdem_Table' object corresponding to the specified
    // 'spec'.
{
    Table object;
    return hh(&object, spec);
}

static int getNumRows(const char *spec)
    // return the number of rows in the 'bdem_Table' created by the specified
    // 'spec'.
{
    int size = 0;
    for (; *spec; ++spec) {
        if (*spec == ';') ++size;
        if (isTypeChar(*spec)) {
            ++spec;
            for (; *spec; ++spec) {
                 if (*spec == ';') break;
            }
            if (*spec == 0) break;
            size = 0;
        }
        if (*spec == '~' || *spec == '!') size = 0;
    }

    return size;
}

static int hhhx(Table *address, const char *colspec,
                const char *rowspec, int showErrorFlag)
{
    // rowspec may contain a-e or n.
    // Example:
    // hhhx(address, "ABU", "abedc", false);
    // generates a table with the spec:
    // ABU;xxx;yyy;xyx;uyu;xux;
    const int   MAX     = 1000;     // maximum number of columns permitted
    char        buf[MAX * 2 + 1];   // buffer to accumulate types and values
    const char *specvalues[] = {
         "xxxxx", "yyyyy", "xnxnx", "nynyn", "xyxyx", "nnnnn"
    };

    const int NUM_VALUES = 5;

    const int COLS = strlen(colspec);
    strcpy(buf, colspec);
    strcat(buf, ";");
    char *p = buf + strlen(buf);

    for (;*rowspec; ++rowspec) {
        if ((*rowspec < 'a' || *rowspec > 'e') && *rowspec != 'n')
            return HHH_ILLEGAL_CHARACTER;
        for (int i=0; i < COLS; ++i) {
             int index = *rowspec == 'n' ? 5 : *rowspec - 'a';
             *(p++) = specvalues[index][i % NUM_VALUES];
        }
        *(p++) = ';';
    }
    *p = '\0';

    return hhh(address, buf, showErrorFlag);
}

Table& hhx(Table *address, const char *colspec, const char *rowspec)
    // Configure the modifiable 'bdem_Table' at the specified 'address'
    // according to the specified 'spec'.  Return a reference to '*address'.
    // Note that this function uses only the primary manipulators of the test
    // plan.  Note also that neither argument may be null.
{
    enum { SHOW_ERROR_FLAG = 1 };
    int status = hhhx(address, colspec, rowspec, SHOW_ERROR_FLAG);
    ASSERT(HHH_SUCCESS == status);
    return *address;
}

Table hx(const char *colspec, const char *rowspec)
    // Return by value a new 'bdem_Table' object corresponding to the specified
    // 'colspec' and 'rowspec'.
{
    Table object;
    return hhx(&object, colspec, rowspec);
}

//-----------------------------------------------------------------------------
//                Useful 'bdem' Object-Manipulation Helpers
//             (May come before the 'G_VALUES_i[]' definitions)
//-----------------------------------------------------------------------------

static void stretchList(List *list, int numElements)
    // Append the specified 'numElements' of unset values of type 'int' to the
    // specified 'list'.
{
    for (int i = 0; i < numElements; ++i) {
        list->appendNullInt();  // arbitrary element; can add diversity here
    }
    return;
}

static void stretchRemoveAll(List *list, int numElements)
{
    stretchList(list, numElements);
    list->removeAll();
}

//-----------------------------------------------------------------------------
//                Useful 'bdem' Object-Manipulation Helpers
//                   (Must come after the 'ggg' helpers)
//-----------------------------------------------------------------------------

static void replaceValues(List        *list,
                          int          dstIndex,
                          const List&  srcList,
                          int          srcIndex,
                          int          numElements)
{
    list->insertElements(dstIndex, srcList, srcIndex, numElements);
    list->removeElements(dstIndex + numElements, numElements);
}

//-----------------------------------------------------------------------------
//   'setEqual' helpers for non-'const' tables, lists, and rows,
//       overloaded to accept 'int' or 'char' type and value arguments.
//             (Must come after the 'G_VALUES_i[]' definitions)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//   'setEqual' helpers for non-'const' tables, lists, and rows,
//       overloaded to accept 'int' or 'char' type and value arguments.
//             (Must come after the 'G_VALUES_i[]' definitions)
//-----------------------------------------------------------------------------

static void assignElemRef(const bdem_ElemRef *lhs,
                          const bdem_ConstElemRef& rhs)
    // Return 1 if the element in the specified 'row' at the specified 'rowIdx'
    // of the type specified by 'typeIdx' has the value specified by 'valIdx'.
    // The behavior is undefined unless 0 <= rowIdx < length(),
    // 0 <= typeIndex <= 13, and 0 <= valueIndex <= 2.
{
    int typeIdx = lhs->type();

    ASSERT(typeIdx >= 0 && typeIdx < 22);
    ASSERT(rhs.type() == typeIdx);

    switch(typeIdx) {
      case 0:  lhs->theModifiableChar()          = rhs.theChar();
      break;
      case 1:  lhs->theModifiableShort()         = rhs.theShort();
      break;
      case 2:  lhs->theModifiableInt()           = rhs.theInt();
      break;
      case 3:  lhs->theModifiableInt64()         = rhs.theInt64();
      break;
      case 4:  lhs->theModifiableFloat()         = rhs.theFloat();
      break;
      case 5:  lhs->theModifiableDouble()        = rhs.theDouble();
      break;
      case 6:  lhs->theModifiableString()        = rhs.theString();
      break;
      case 7:  lhs->theModifiableDatetime()      = rhs.theDatetime();
      break;
      case 8:  lhs->theModifiableDate()          = rhs.theDate();
      break;
      case 9:  lhs->theModifiableTime()          = rhs.theTime();
      break;
      case 10: lhs->theModifiableCharArray()     = rhs.theCharArray();
      break;
      case 11: lhs->theModifiableShortArray()    = rhs.theShortArray();
      break;
      case 12: lhs->theModifiableIntArray()      = rhs.theIntArray();
      break;
      case 13: lhs->theModifiableInt64Array()    = rhs.theInt64Array();
      break;
      case 14: lhs->theModifiableFloatArray()    = rhs.theFloatArray();
      break;
      case 15: lhs->theModifiableDoubleArray()   = rhs.theDoubleArray();
      break;
      case 16: lhs->theModifiableStringArray()   = rhs.theStringArray();
      break;
      case 17: lhs->theModifiableDatetimeArray() = rhs.theDatetimeArray();
      break;
      case 18: lhs->theModifiableDateArray()     = rhs.theDateArray();
      break;
      case 19: lhs->theModifiableTimeArray()     = rhs.theTimeArray();
      break;
      case 20: lhs->theModifiableList()          = rhs.theList();
      break;
      case 21: lhs->theModifiableTable()         = rhs.theTable();
      break;
      default: ASSERT(0);
    }
}

static void setEqual(const List *list, int index, char type, char val)
{
    setEqual(list, index, typeCharToIndex(type), valCharToIndex(val));
}

//=============================================================================
//              Miscellaneous helpers following the general pattern
//-----------------------------------------------------------------------------

static bool areEqual(const Row& a, int i, const Row& b, int j)
    // Return 1 if the two elements in the two specified rows 'a' and 'b' at
    // the specified indices 'i, and 'j', respectively, have the same type and
    // value, and zero otherwise.
{
    ASSERT(0 <= i);
    ASSERT(i < a.length());
    ASSERT(0 <= j);
    ASSERT(j < b.length());

    if (a.elemType(i) != b.elemType(j)) {
        return false;                                                 // RETURN
    }

    typedef bdem_ElemType T;

    switch(a.elemType(i)) {
    case T::BDEM_CHAR:
      return a.theChar(i)          == b.theChar(j);
    case T::BDEM_SHORT:
      return a.theShort(i)         == b.theShort(j);
    case T::BDEM_INT:
      return a.theInt(i)           == b.theInt(j);
    case T::BDEM_INT64:
      return a.theInt64(i)         == b.theInt64(j);
    case T::BDEM_FLOAT:
      return a.theFloat(i)         == b.theFloat(j);
    case T::BDEM_DOUBLE:
      return a.theDouble(i)        == b.theDouble(j);
    case T::BDEM_STRING:
      return a.theString(i)        == b.theString(j);
    case T::BDEM_DATETIME:
      return a.theDatetime(i)      == b.theDatetime(j);
    case T::BDEM_DATE:
      return a.theDate(i)          == b.theDate(j);
    case T::BDEM_TIME:
      return a.theTime(i)          == b.theTime(j);
    case T::BDEM_CHAR_ARRAY:
      return a.theCharArray(i)     == b.theCharArray(j);
    case T::BDEM_SHORT_ARRAY:
      return a.theShortArray(i)    == b.theShortArray(j);
    case T::BDEM_INT_ARRAY:
      return a.theIntArray(i)      == b.theIntArray(j);
    case T::BDEM_INT64_ARRAY:
      return a.theInt64Array(i)    == b.theInt64Array(j);
    case T::BDEM_FLOAT_ARRAY:
      return a.theFloatArray(i)    == b.theFloatArray(j);
    case T::BDEM_DOUBLE_ARRAY:
      return a.theDoubleArray(i)   == b.theDoubleArray(j);
    case T::BDEM_STRING_ARRAY:
      return a.theStringArray(i)   == b.theStringArray(j);
    case T::BDEM_DATETIME_ARRAY:
      return a.theDatetimeArray(i) == b.theDatetimeArray(j);
    case T::BDEM_DATE_ARRAY:
      return a.theDateArray(i)     == b.theDateArray(j);
    case T::BDEM_TIME_ARRAY:
      return a.theTimeArray(i)     == b.theTimeArray(j);
    case T::BDEM_LIST:
      return a.theList(i)          == b.theList(j);
    case T::BDEM_TABLE:
      return a.theTable(i)         == b.theTable(j);

    default: ASSERT(!"ERROR");
      return false;                                                   // RETURN
    }
}

void reverseSpec(char *spec)
    // Reverse the order of the specified null terminated string 'spec' as used
    // by the 'bdem_List' 'g' generator functions.  The behavior is undefined
    // unless the string length is a multiple of 2.
{
    int len = strlen(spec);
    int items = len/2;
    ASSERT(len % 2 == 0);

    for (int item = 0; item < items/2; ++item) {
        int pos = item * 2;
        int epos = len - pos - 2;

        char t = spec[pos];
        spec[pos] = spec[epos];
        spec[epos] = t;

        t = spec[pos + 1];
        spec[pos + 1] = spec[epos + 1];
        spec[epos + 1] = t;
    }
}

void reverseList(List *list)
    // reverse the order of the specified 'list'.
{
    int len = list->length();
    for (int item = 0; item < len/2; ++item) {
        list->swapElements(item, len - item - 1);
    }
}

const int NUM_PERMUTATIONS = 4;

void makePermutation(List *list, const char *SPEC,
                     int reverse, int permutation)
{
    const int MAX_SPEC_LEN = 64;
    char uspec[MAX_SPEC_LEN];
    int i;
    strcpy(uspec, SPEC);
    if (reverse) {
        reverseSpec(uspec);
    }
    for (i=0; i < permutation; ++i) {
        list->appendChar(0);
    }
    for (i=0; i < permutation; ++i) {
        list->removeElement(0);
    }

    gg(list, uspec);
    if (reverse) {
        reverseList(list);
    }
}

int getMaxOffset(const Row& srcRow)
    // Returns the maximum offset used within a row.  If the row is empty,
    // zero is returned.  The absolute value is meaningless from the
    // perspective of the test driver and is only used to compare values from
    // rows having the same element types but different internal
    // representations.
{
    int len = srcRow.length();
    int maxOffset = 0;
    for (int i = 0; i < len; ++i) {
        const char *item = &srcRow.theChar(i);
        int offset = ((const char *) item) - ((const char *) &srcRow);
        if (offset > maxOffset)
        {
            int len = bdem_ElemAttrLookup::lookupTable()[srcRow.elemType(i)]->
                                                                        d_size;
            maxOffset = offset + len - 1;
        }
    }

    return maxOffset;
}

void dumpOffsets(const Row& srcRow)
{
    int len = srcRow.length();
    for (int i = 0; i < len; ++i) {
        const char *item = &srcRow.theChar(i);
        int offset = ((const char *) item) - ((const char *) &srcRow);
        cout << srcRow.elemType(i) << ' ' << offset << endl;
    }
}

//=============================================================================
//  MACROS TO DEFINE TEST SUPPORT VARIABLES POTENTIALLY USED IN EACH TEST CASE
//-----------------------------------------------------------------------------

// Many test cases will not need the full set of variables exported out of main
// so it is suggested that this one overly large macro is broken down into a
// few more specific declaration macros, creating only the variables needed for
// specific testing scenarios.

#define DECLARE_TEST_ALLOCATOR                                                \
    bslma_TestAllocator  testAllocator(veryVeryVerbose);                      \
    bslma_Allocator     *Z = &testAllocator;

#define DECLARE_MAIN_VARIABLES                                                \
    /* -----------------------------------------------------------------------\
    // The following "global constants for testing" are in 'main' because the \
    // test allocator (defined in 'main') is used in the constructors of the  \
    // test lists and tables.                                                 \
    //                                                                        \
    // Make three sets of useful constants for testing: Ui, Xi, Yi            \
    // --------------------------------------------------------------------*/ \
                                                                              \
    const char                       UA = bdetu_Unset<char>::unsetValue();    \
    const short                      UB = bdetu_Unset<short>::unsetValue();   \
    const int                        UC = bdetu_Unset<int>::unsetValue();     \
    const Int64                      UD = bdetu_Unset<Int64>::unsetValue();   \
    const float                      UE = bdetu_Unset<float>::unsetValue();   \
    const double                     UF = bdetu_Unset<double>::unsetValue();  \
    const bsl::string                UG;                                      \
    const bdet_Datetime              UH;                                      \
    const bdet_Date                  UI;                                      \
    const bdet_Time                  UJ;                                      \
                                                                              \
    /* Empty arrays, lists, and tables make fine null values, so no special   \
       provisions are necessary. */                                           \
    const bsl::vector<char>           UK;                                     \
    const bsl::vector<short>          UL;                                     \
    const bsl::vector<int>            UM;                                     \
    const bsl::vector<Int64>          UN;                                     \
    const bsl::vector<float>          UO;                                     \
    const bsl::vector<double>         UP;                                     \
    const bsl::vector<bsl::string>    UQ;                                     \
    const bsl::vector<bdet_Datetime>  UR;                                     \
    const bsl::vector<bdet_Date>      US;                                     \
    const bsl::vector<bdet_Time>      UT;                                     \
                                                                              \
    const bdem_List                  UU;                                      \
    const bdem_Table                 UV;                                      \
                                                                              \
    const bool                       UW = bdetu_Unset<bool>::unsetValue();    \
    const bdet_DatetimeTz            UX;                                      \
    const bdet_DateTz                UY;                                      \
    const bdet_TimeTz                UZ;                                      \
    const bsl::vector<bool>          Ua;                                      \
    const bsl::vector<bdet_DatetimeTz> Ub;                                    \
    const bsl::vector<bdet_DateTz>   Uc;                                      \
    const bsl::vector<bdet_TimeTz>   Ud;                                      \
    const bdem_Choice                Ue;                                      \
    const bdem_ChoiceArray           Uf;                                      \
                                                                              \
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ \
                                                                              \
    const char                       XA = 'x'; /* = 120 decimal (ascii only)*/\
    const short                      XB = 12;                                 \
    const int                        XC = 103;                                \
    const Int64                      XD = 10004;                              \
    const float                      XE = 105.5;                              \
    const double                     XF = 106.006;                            \
    const bsl::string                XG = "one-seven---";  /* not in-place */ \
    const bdet_Datetime              XH(bdet_Date(108, 8, 8),                 \
                                        bdet_Time(8, 8, 8, 108));             \
    const bdet_Date                  XI(109, 9, 9);                           \
    const bdet_Time                  XJ(10, 10, 10, 110);                     \
                                                                              \
    bsl::vector<char>                 XK_;                                    \
    const bsl::vector<char>&          XK = XK_;                               \
    XK_.push_back(XA);                                                        \
    XK_.push_back('N');                                                       \
    XK_.push_back(XA);                                                        \
    XK_.push_back('N');                                                       \
    XK_.push_back(XA);                                                        \
                                                                              \
    bsl::vector<short>                XL_;                                    \
    const bsl::vector<short>&         XL = XL_;                               \
    XL_.push_back(XB);                                                        \
    XL_.push_back(UB);                                                        \
                                                                              \
    bsl::vector<int>                  XM_;                                    \
    const bsl::vector<int>&           XM = XM_;                               \
    XM_.push_back(XC);                                                        \
    XM_.push_back(UC);                                                        \
                                                                              \
    bsl::vector<Int64>                XN_;                                    \
    const bsl::vector<Int64>&         XN = XN_;                               \
    XN_.push_back(XD);                                                        \
    XN_.push_back(UD);                                                        \
                                                                              \
    bsl::vector<float>                XO_;                                    \
    const bsl::vector<float>&         XO = XO_;                               \
    XO_.push_back(XE);                                                        \
    XO_.push_back(UE);                                                        \
                                                                              \
    bsl::vector<double>               XP_;                                    \
    const bsl::vector<double>&        XP = XP_;                               \
    XP_.push_back(XF);                                                        \
    XP_.push_back(UF);                                                        \
                                                                              \
    bsl::vector<bsl::string>          XQ_;                                    \
    const bsl::vector<bsl::string>&   XQ = XQ_;                               \
    XQ_.push_back(XG);                                                        \
    XQ_.push_back(UG);                                                        \
                                                                              \
    bsl::vector<bdet_Datetime>        XR_;                                    \
    const bsl::vector<bdet_Datetime>& XR = XR_;                               \
    XR_.push_back(XH);                                                        \
    XR_.push_back(UH);                                                        \
                                                                              \
    bsl::vector<bdet_Date>            XS_;                                    \
    const bsl::vector<bdet_Date>&     XS = XS_;                               \
    XS_.push_back(XI);                                                        \
    XS_.push_back(UI);                                                        \
                                                                              \
    bsl::vector<bdet_Time>            XT_;                                    \
    const bsl::vector<bdet_Time>&     XT = XT_;                               \
    XT_.push_back(XJ);                                                        \
    XT_.push_back(UJ);                                                        \
                                                                              \
    bdem_List                        XU_(Z);                                  \
    const bdem_List&                 XU = XU_;                                \
    XU_.appendInt(XC);                                                        \
    XU_.appendString(XG);                                                     \
                                                                              \
    my_ElemTypeArray e;                                                       \
    getElemTypes(&e, XU);                                                     \
    bdem_Table                       XV_(e, Z);                               \
    const bdem_Table&                XV = XV_;                                \
    XV_.appendNullRow();                                                      \
    XV_.theModifiableRow(0)[0].theModifiableInt()    = XC;                    \
    XV_.theModifiableRow(0)[1].theModifiableString() = XG;                    \
    XV_.appendNullRow();                                                      \
    XV_.theModifiableRow(1)[0].theModifiableInt()    = UC;                    \
    XV_.theModifiableRow(1)[1].theModifiableString() = UG;                    \
                                                                              \
    const bool                       XW = true;                               \
    const bdet_DatetimeTz            XX(XH, 120);                             \
    const bdet_DateTz                XY(XI, 120);                             \
    const bdet_TimeTz                XZ(XJ, 120);                             \
                                                                              \
    bsl::vector<bool>                Xa_;                                     \
    const bsl::vector<bool>&         Xa = Xa_;                                \
    Xa_.push_back(true);                                                      \
    Xa_.push_back(false);                                                     \
    Xa_.push_back(true);                                                      \
    Xa_.push_back(true);                                                      \
                                                                              \
    bsl::vector<bdet_DatetimeTz>    Xb_;                                      \
    const bsl::vector<bdet_DatetimeTz>& Xb = Xb_;                             \
    Xb_.push_back(XX);                                                        \
    Xb_.push_back(UX);                                                        \
                                                                              \
    bsl::vector<bdet_DateTz>        Xc_;                                      \
    const bsl::vector<bdet_DateTz>& Xc = Xc_;                                 \
    Xc_.push_back(XY);                                                        \
    Xc_.push_back(UY);                                                        \
                                                                              \
    bsl::vector<bdet_TimeTz>        Xd_;                                      \
    const bsl::vector<bdet_TimeTz>& Xd = Xd_;                                 \
    Xd_.push_back(XZ);                                                        \
    Xd_.push_back(UZ);                                                        \
                                                                              \
    bdem_Choice                     Xe_;                                      \
    const bdem_Choice&              Xe = Xe_;                                 \
    Xe_.addSelection(bdem_ElemType::BDEM_INT);                                \
    Xe_.addSelection(bdem_ElemType::BDEM_DOUBLE);                             \
    Xe_.makeSelection(0).theModifiableInt() = 5;                              \
                                                                              \
    bdem_ChoiceArray                Xf_;                                      \
    const bdem_ChoiceArray&         Xf = Xf_;                                 \
    {                                                                         \
        bdem_ElemType::Type catalog[] = { bdem_ElemType::BDEM_INT,            \
                                          bdem_ElemType::BDEM_DOUBLE };       \
        Xf_.reset(catalog, 2);                                                \
        bdem_Choice choice(catalog, 2, Z);                                    \
        choice.makeSelection(0).theModifiableInt() = -6;                      \
        Xf_.appendItem(Xe);                                                   \
        Xf_.appendItem(choice);                                               \
    }                                                                         \
                                                                              \
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ \
                                                                              \
    const char                       YA = 'y'; /* = 121 decimal (ascii only)*/\
    const short                      YB = 22;                                 \
    const int                        YC = 203;                                \
    const Int64                      YD = 20004;                              \
    const float                      YE = 205.5;                              \
    const double                     YF = 206.006;                            \
    const bsl::string                YG = "two-seven";                        \
    const bdet_Datetime              YH(bdet_Date(208, 8, 8),                 \
                                        bdet_Time(8, 8, 8, 208));             \
    const bdet_Date                  YI(209, 9, 9);                           \
    const bdet_Time                  YJ(10, 10, 10, 210);                     \
                                                                              \
    bsl::vector<char>                 YK_;                                    \
    const bsl::vector<char>&          YK = YK_;                               \
    YK_.push_back(YA);                                                        \
    YK_.push_back('n');                                                       \
                                                                              \
    bsl::vector<short>                YL_;                                    \
    const bsl::vector<short>&         YL = YL_;                               \
    YL_.push_back(YB);                                                        \
    YL_.push_back(UB);                                                        \
                                                                              \
    bsl::vector<int>                  YM_;                                    \
    const bsl::vector<int>&           YM = YM_;                               \
    YM_.push_back(YC);                                                        \
    YM_.push_back(UC);                                                        \
                                                                              \
    bsl::vector<Int64>                YN_;                                    \
    const bsl::vector<Int64>&         YN = YN_;                               \
    YN_.push_back(YD);                                                        \
    YN_.push_back(UD);                                                        \
                                                                              \
    bsl::vector<float>                YO_;                                    \
    const bsl::vector<float>&         YO = YO_;                               \
    YO_.push_back(YE);                                                        \
    YO_.push_back(UE);                                                        \
                                                                              \
    bsl::vector<double>               YP_;                                    \
    const bsl::vector<double>&        YP = YP_;                               \
    YP_.push_back(YF);                                                        \
    YP_.push_back(UF);                                                        \
                                                                              \
    bsl::vector<bsl::string>          YQ_;                                    \
    const bsl::vector<bsl::string>&   YQ = YQ_;                               \
    YQ_.push_back(YG);                                                        \
    YQ_.push_back(UG);                                                        \
                                                                              \
    bsl::vector<bdet_Datetime>        YR_;                                    \
    const bsl::vector<bdet_Datetime>& YR = YR_;                               \
    YR_.push_back(YH);                                                        \
    YR_.push_back(UH);                                                        \
                                                                              \
    bsl::vector<bdet_Date>            YS_;                                    \
    const bsl::vector<bdet_Date>&     YS = YS_;                               \
    YS_.push_back(YI);                                                        \
    YS_.push_back(UI);                                                        \
                                                                              \
    bsl::vector<bdet_Time>            YT_;                                    \
    const bsl::vector<bdet_Time>&     YT = YT_;                               \
    YT_.push_back(YJ);                                                        \
    YT_.push_back(UJ);                                                        \
                                                                              \
    bdem_List                        YU_(Z);                                  \
    const bdem_List&                 YU = YU_;                                \
    YU_.appendInt(YC);                                                        \
    YU_.appendString(YG);                                                     \
                                                                              \
    getElemTypes(&e, YU);                                                     \
    bdem_Table                       YV_(vectorData(e), e.size(), Z);         \
    const bdem_Table&                YV = YV_;                                \
    YV_.appendNullRow();                                                      \
    YV_.theModifiableRow(0)[0].theModifiableInt()    = YC;                    \
    YV_.theModifiableRow(0)[1].theModifiableString() = YG;                    \
    YV_.appendNullRow();                                                      \
    YV_.theModifiableRow(1)[0].theModifiableInt()    = UC;                    \
    YV_.theModifiableRow(1)[1].theModifiableString() = UG;                    \
                                                                              \
    const bool                       YW = true;                               \
    const bdet_DatetimeTz            YX(YH, 180);                             \
    const bdet_DateTz                YY(YI, 180);                             \
    const bdet_TimeTz                YZ(YJ, 180);                             \
                                                                              \
    bsl::vector<bool>                Ya_;                                     \
    const bsl::vector<bool>&         Ya = Ya_;                                \
    Ya_.push_back(true);                                                      \
    Ya_.push_back(false);                                                     \
    Ya_.push_back(true);                                                      \
    Ya_.push_back(true);                                                      \
                                                                              \
    bsl::vector<bdet_DatetimeTz>    Yb_;                                      \
    const bsl::vector<bdet_DatetimeTz>& Yb = Yb_;                             \
    Yb_.push_back(YX);                                                        \
    Yb_.push_back(UX);                                                        \
                                                                              \
    bsl::vector<bdet_DateTz>        Yc_;                                      \
    const bsl::vector<bdet_DateTz>& Yc = Yc_;                                 \
    Yc_.push_back(YY);                                                        \
    Yc_.push_back(UY);                                                        \
                                                                              \
    bsl::vector<bdet_TimeTz>        Yd_;                                      \
    const bsl::vector<bdet_TimeTz>& Yd = Yd_;                                 \
    Yd_.push_back(YZ);                                                        \
    Yd_.push_back(UZ);                                                        \
                                                                              \
    bdem_Choice                     Ye_;                                      \
    const bdem_Choice&              Ye = Ye_;                                 \
    Ye_.addSelection(bdem_ElemType::BDEM_INT);                                \
    Ye_.addSelection(bdem_ElemType::BDEM_DOUBLE);                             \
    Ye_.makeSelection(1).theModifiableDouble() = 7.5;                         \
                                                                              \
    bdem_ChoiceArray                Yf_;                                      \
    const bdem_ChoiceArray&         Yf = Yf_;                                 \
    {                                                                         \
        bdem_ElemType::Type catalog[] = { bdem_ElemType::BDEM_INT,            \
                                          bdem_ElemType::BDEM_DOUBLE };       \
        Yf_.reset(catalog, 2);                                                \
        bdem_Choice choice(catalog, 2, Z);                                    \
        choice.makeSelection(1).theModifiableDouble() = -10.25;               \
        Yf_.appendItem(Ye);                                                   \
        Yf_.appendItem(choice);                                               \
    }                                                                         \
                                                                              \
    /*------------------------------------------------------------------------\
    // Two more convenient test objects populated with the 22 bdem types      \
    //----------------------------------------------------------------------*/\
                                                                              \
    List LX_(Z);  const List& LX = LX_;                                       \
    LX_.appendChar(XA);                                                       \
    LX_.appendShort(XB);                                                      \
    LX_.appendInt(XC);                                                        \
    LX_.appendInt64(XD);                                                      \
    LX_.appendFloat(XE);                                                      \
    LX_.appendDouble(XF);                                                     \
    LX_.appendString(XG);                                                     \
    LX_.appendDatetime(XH);                                                   \
    LX_.appendDate(XI);                                                       \
    LX_.appendTime(XJ);                                                       \
    LX_.appendCharArray(XK);                                                  \
    LX_.appendShortArray(XL);                                                 \
    LX_.appendIntArray(XM);                                                   \
    LX_.appendInt64Array(XN);                                                 \
    LX_.appendFloatArray(XO);                                                 \
    LX_.appendDoubleArray(XP);                                                \
    LX_.appendStringArray(XQ);                                                \
    LX_.appendDatetimeArray(XR);                                              \
    LX_.appendDateArray(XS);                                                  \
    LX_.appendTimeArray(XT);                                                  \
    LX_.appendList(XU);                                                       \
    LX_.appendTable(XV);                                                      \
                                                                              \
    LX_.appendBool(XW);                                                       \
    LX_.appendDatetimeTz(XX);                                                 \
    LX_.appendDateTz(XY);                                                     \
    LX_.appendTimeTz(XZ);                                                     \
    LX_.appendBoolArray(Xa);                                                  \
    LX_.appendDatetimeTzArray(Xb);                                            \
    LX_.appendDateTzArray(Xc);                                                \
    LX_.appendTimeTzArray(Xd);                                                \
    LX_.appendChoice(Xe);                                                     \
    LX_.appendChoiceArray(Xf);                                                \
                                                                              \
    getElemTypes(&e, LX.row());                                               \
    const my_ElemTypeArray BDEM_TYPES = e;                                    \
                                                                              \
    /* -----------------------------------------------------------------------\
    // populate global element pointers for generator function g("...") */    \
                                                                              \
    G_VALUES_A[0] = &XA;    G_VALUES_A[1] = &YA;    G_VALUES_A[2] = &UA;      \
    G_VALUES_B[0] = &XB;    G_VALUES_B[1] = &YB;    G_VALUES_B[2] = &UB;      \
    G_VALUES_C[0] = &XC;    G_VALUES_C[1] = &YC;    G_VALUES_C[2] = &UC;      \
    G_VALUES_D[0] = &XD;    G_VALUES_D[1] = &YD;    G_VALUES_D[2] = &UD;      \
    G_VALUES_E[0] = &XE;    G_VALUES_E[1] = &YE;    G_VALUES_E[2] = &UE;      \
    G_VALUES_F[0] = &XF;    G_VALUES_F[1] = &YF;    G_VALUES_F[2] = &UF;      \
    G_VALUES_G[0] = &XG;    G_VALUES_G[1] = &YG;    G_VALUES_G[2] = &UG;      \
    G_VALUES_H[0] = &XH;    G_VALUES_H[1] = &YH;    G_VALUES_H[2] = &UH;      \
    G_VALUES_I[0] = &XI;    G_VALUES_I[1] = &YI;    G_VALUES_I[2] = &UI;      \
    G_VALUES_J[0] = &XJ;    G_VALUES_J[1] = &YJ;    G_VALUES_J[2] = &UJ;      \
    G_VALUES_K[0] = &XK;    G_VALUES_K[1] = &YK;    G_VALUES_K[2] = &UK;      \
    G_VALUES_L[0] = &XL;    G_VALUES_L[1] = &YL;    G_VALUES_L[2] = &UL;      \
    G_VALUES_M[0] = &XM;    G_VALUES_M[1] = &YM;    G_VALUES_M[2] = &UM;      \
    G_VALUES_N[0] = &XN;    G_VALUES_N[1] = &YN;    G_VALUES_N[2] = &UN;      \
    G_VALUES_O[0] = &XO;    G_VALUES_O[1] = &YO;    G_VALUES_O[2] = &UO;      \
    G_VALUES_P[0] = &XP;    G_VALUES_P[1] = &YP;    G_VALUES_P[2] = &UP;      \
    G_VALUES_Q[0] = &XQ;    G_VALUES_Q[1] = &YQ;    G_VALUES_Q[2] = &UQ;      \
    G_VALUES_R[0] = &XR;    G_VALUES_R[1] = &YR;    G_VALUES_R[2] = &UR;      \
    G_VALUES_S[0] = &XS;    G_VALUES_S[1] = &YS;    G_VALUES_S[2] = &US;      \
    G_VALUES_T[0] = &XT;    G_VALUES_T[1] = &YT;    G_VALUES_T[2] = &UT;      \
    G_VALUES_U[0] = &XU;    G_VALUES_U[1] = &YU;    G_VALUES_U[2] = &UU;      \
    G_VALUES_V[0] = &XV;    G_VALUES_V[1] = &YV;    G_VALUES_V[2] = &UV;      \
                                                                              \
    G_VALUES_W[0] = &XW;    G_VALUES_W[1] = &YW;    G_VALUES_W[2] = &UW;      \
    G_VALUES_X[0] = &XX;    G_VALUES_X[1] = &YX;    G_VALUES_X[2] = &UX;      \
    G_VALUES_Y[0] = &XY;    G_VALUES_Y[1] = &YY;    G_VALUES_Y[2] = &UY;      \
    G_VALUES_Z[0] = &XZ;    G_VALUES_Z[1] = &YZ;    G_VALUES_Z[2] = &UZ;      \
    G_VALUES_a[0] = &Xa;    G_VALUES_a[1] = &Ya;    G_VALUES_a[2] = &Ua;      \
    G_VALUES_b[0] = &Xb;    G_VALUES_b[1] = &Yb;    G_VALUES_b[2] = &Ub;      \
    G_VALUES_c[0] = &Xc;    G_VALUES_c[1] = &Yc;    G_VALUES_c[2] = &Uc;      \
    G_VALUES_d[0] = &Xd;    G_VALUES_d[1] = &Yd;    G_VALUES_d[2] = &Ud;      \
    G_VALUES_e[0] = &Xe;    G_VALUES_e[1] = &Ye;    G_VALUES_e[2] = &Ue;      \
    G_VALUES_f[0] = &Xf;    G_VALUES_f[1] = &Yf;    G_VALUES_f[2] = &Uf;      \

//=============================================================================
//                              TEST CASES
//-----------------------------------------------------------------------------
#define DEFINE_TEST_CASE(NUMBER)                                              \
void testCase##NUMBER(bool verbose, bool veryVerbose, bool veryVeryVerbose,   \
                                                                  char *argv[])

DEFINE_TEST_CASE(41) {
        // ------------------------------------------------------------------
        // TESTING setColumnXXX FUNCTIONS IN TABLES
        //
        // Create a table containing columns of all 22 types.
        // Create up to 3 rows.  Test that each setColumn function does
        // What is intended.

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl
                          << "Testing setColumn functions" << endl
                          << "===========================" << endl;

        if (verbose) cout << endl
                          << "Testing setColumnString()" << endl;
        {
            static const struct {
                int         d_lineNum;     // source line number
                const char *d_spec;        // Initial spec for Table object
                int         d_testValIdx;  // Col val0 = X, 1 = Y, 2 = U
                const char *d_result;      // Result spec for Table object
            } DATA[] = {
                //line  t-spec             valIdx t-result
                //----  ------             ------ --------
                { L_,   "GUV;",             1,     "GUV;"                    },
                { L_,   "GUV;uuu;",         0,     "GUV;xuu;"                },
                { L_,   "GUV;uuu;",         1,     "GUV;yuu;"                },
                { L_,   "GUV;uuu;",         2,     "GUV;uuu;"                },
                { L_,   "GUV;uuu;xxx;",     0,     "GUV;xuu;xxx;"            },
                { L_,   "GUV;uuu;xxx;",     1,     "GUV;yuu;yxx;"            },
                { L_,   "GUV;uuu;xxx;",     2,     "GUV;uuu;uxx;"            },
                { L_,   "GUV;uuu;xxx;yyy;", 0,     "GUV;xuu;xxx;xyy;"        },
                { L_,   "GUV;uuu;xxx;yyy;", 1,     "GUV;yuu;yxx;yyy;"        },
                { L_,   "GUV;uuu;xxx;yyy;", 2,     "GUV;uuu;uxx;uyy;"        }
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int           LINE     = DATA[ti].d_lineNum;
                const char         *T_SPEC   = DATA[ti].d_spec;
                const bsl::string&  TEST_VAL =
                                            *G_VALUES_G[DATA[ti].d_testValIdx];
                const char         *T_RESULT = DATA[ti].d_result;

                Table mT(&testAllocator);  const Table& T = hh(&mT, T_SPEC);
                Table mT2(T, &testAllocator); const Table& T2 = mT2;
                Table mR(&testAllocator);  const Table& R = hh(&mR, T_RESULT);

                if (veryVerbose) {
                    cout << "Before: "; P(T);
                    cout << "        "; P(R);
                }

                mT.setColumnString(0, TEST_VAL);
                const char* testValCstr = TEST_VAL.data();
                mT2.setColumnString(0, testValCstr);
                if (veryVerbose) {
                    cout << "After:  "; P(T);
                    cout << "        "; P(T2);
                }
                LOOP_ASSERT(LINE, T == R);
                LOOP_ASSERT(LINE, T2 == R);
            }
        }

        if (verbose) cout << endl
                          << "Testing setColumnTable()" << endl;
        {
            static const struct {
                int         d_lineNum;     // source line number
                const char *d_spec;        // Initial spec for Table object
                int         d_testValIdx;  // Col val0 = X, 1 = Y, 2 = U
                const char *d_result;      // Result spec for Table object
            } DATA[] = {
                //line  t-spec             valIdx t-result
                //----  ------             ------ --------
                { L_,   "GUV;",             1,     "GUV;"                    },
                { L_,   "GUV;uuu;",         0,     "GUV;uux;"                },
                { L_,   "GUV;uuu;",         1,     "GUV;uuy;"                },
                { L_,   "GUV;uuu;",         2,     "GUV;uuu;"                },
                { L_,   "GUV;uuu;xxx;",     0,     "GUV;uux;xxx;"            },
                { L_,   "GUV;uuu;xxx;",     1,     "GUV;uuy;xxy;"            },
                { L_,   "GUV;uuu;xxx;",     2,     "GUV;uuu;xxu;"            },
                { L_,   "GUV;uuu;xxx;yyy;", 0,     "GUV;uux;xxx;yyx;"        },
                { L_,   "GUV;uuu;xxx;yyy;", 1,     "GUV;uuy;xxy;yyy;"        },
                { L_,   "GUV;uuu;xxx;yyy;", 2,     "GUV;uuu;xxu;yyu;"        }
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int     LINE      = DATA[ti].d_lineNum;
                const char   *T_SPEC    = DATA[ti].d_spec;
                const Table&  TEST_VAL = *G_VALUES_V[DATA[ti].d_testValIdx];
                const char   *T_RESULT  = DATA[ti].d_result;

                Table mT(&testAllocator);  const Table& T = hh(&mT, T_SPEC);
                Table mR(&testAllocator);  const Table& R = hh(&mR, T_RESULT);

                if (veryVerbose) {
                    cout << "Before: "; P(T);
                    cout << "        "; P(R);
                }

                mT.setColumnTable(2, TEST_VAL);
                if (veryVerbose) { cout << "After:  "; P(T); }
                LOOP_ASSERT(LINE, T == R);
            }
        }

        if (verbose) cout << endl
                          << "Testing setColumnString()" << endl;
        {
            static const struct {
                int         d_lineNum;     // source line number
                const char *d_spec;        // Initial spec for Table object
                const char *d_result;      // Result spec for Table object
            } DATA[] = {
                //line  t-spec              t-result
                //----  ------              --------
                { L_,   "GUV;",             "GUV;"                    },
                { L_,   "GUV;uuu;",         "GUV;xuu;"                },
                { L_,   "GUV;uuu;xxx;",     "GUV;xuu;xxx;"            },
                { L_,   "GUV;uuu;xxx;yyy;", "GUV;xuu;xxx;xyy;"        }
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int           LINE     = DATA[ti].d_lineNum;
                const char         *T_SPEC   = DATA[ti].d_spec;
                const char         *T_RESULT = DATA[ti].d_result;

                const bdem_ConstElemRef
                                      TEST_VAL(LX[bdem_ElemType::BDEM_STRING]);

                Table mT(&testAllocator);  const Table& T = hh(&mT, T_SPEC);
                Table mR(&testAllocator);  const Table& R = hh(&mR, T_RESULT);

                if (veryVerbose) {
                    cout << "Before: "; P(T);
                    cout << "        "; P(R);
                }

                mT.setColumnValue(0, TEST_VAL);
                if (veryVerbose) { cout << "After:  "; P(T); }
                LOOP_ASSERT(LINE, T == R);
            }
        }
      }

DEFINE_TEST_CASE(40) {
        // ------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Create a 'bdem_List' holding the data that would normally be
        //   injected into a "Std Equity Trade (Local)" message.
        //
        //   Representation of the data:
        //
        //   Equity Trade {
        //     Tick Type                      ET
        //     Time {
        //       datetime                     2002/11/20/9/30/0/0
        //       GMT Offset                   -5
        //     }
        //     Sequence Number                1
        //     Source/Contributor             NASDAQ
        //     Market Center                  NASDAQ
        //     Security Identifier {
        //       Security Identifier          INTC
        //       Security Identifier Type     EXCH
        //     }
        //     Sale Condition {
        //       Sale Condition               aa
        //       Update Action                None
        //     }
        //     Trade Price {
        //       Price                        18.13
        //       Price Denominator            2
        //     }
        //     Trade Size                     2000
        //     Currency Code                  USD
        //   }
        // ------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

        bdex_ByteOutStream outStream;

        {
            // Std Time (GMT) record

            const bdem_ElemType::Type timeElemTypes[] = {
                bdem_ElemType::BDEM_DATETIME,
                bdem_ElemType::BDEM_FLOAT
            };
            const int timeNumElemTypes = NUM_ARRYELEMS(timeElemTypes);

            const int datetimeIndex = 0;
            const int offsetIndex = 1;

            // Std Security ID record

            const bdem_ElemType::Type securityElemTypes[] = {
                bdem_ElemType::BDEM_STRING,
                bdem_ElemType::BDEM_STRING
            };
            const int securityNumElemTypes = NUM_ARRYELEMS(securityElemTypes);

            const int secIDIndex = 0;
            const int secIDTypeIndex = 1;

            // Std Equity Sale record

            const bdem_ElemType::Type equitySaleElemTypes[] = {
                bdem_ElemType::BDEM_STRING,
                bdem_ElemType::BDEM_STRING
            };
            const int equitySaleNumElemTypes =
                NUM_ARRYELEMS(equitySaleElemTypes);

            const int saleCondIndex = 0;
            const int updateActionIndex = 1;

            // Std Price record

            const bdem_ElemType::Type priceElemTypes[] = {
                bdem_ElemType::BDEM_FLOAT,
                bdem_ElemType::BDEM_STRING
            };
            const int priceNumElemTypes = NUM_ARRYELEMS(priceElemTypes);

            const int priceIndex = 0;
            const int priceDenomIndex = 1;

            // Equity Trade (Local) record

            const bdem_ElemType::Type equityTradeElemTypes[] = {
                bdem_ElemType::BDEM_STRING,
                bdem_ElemType::BDEM_LIST,
                bdem_ElemType::BDEM_INT,
                bdem_ElemType::BDEM_STRING,
                bdem_ElemType::BDEM_STRING,
                bdem_ElemType::BDEM_LIST,
                bdem_ElemType::BDEM_LIST,
                bdem_ElemType::BDEM_LIST,
                bdem_ElemType::BDEM_INT,
                bdem_ElemType::BDEM_STRING
            };
            const int equityTradeNumElemTypes =
                NUM_ARRYELEMS(equityTradeElemTypes);

            const int tickTypeIndex = 0;
            const int timeListIndex = 1;
            const int seqNumIndex = 2;
            const int sourceConIndex = 3;
            const int marketCenterIndex = 4;
            const int securityListIndex = 5;
            const int saleCondListIndex = 6;
            const int tradePriceListIndex = 7;
            const int tradeSizeIndex = 8;
            const int curCodeIndex = 9;

            // This code must be repeated for each message sent.
            // It creates the actual message sent.

            bdem_List equityTradeList(equityTradeElemTypes,
                                      equityTradeNumElemTypes);
            bdem_List *timeListPtr =
                             &equityTradeList.theModifiableList(timeListIndex);
            bdem_List *securityListPtr =
                         &equityTradeList.theModifiableList(securityListIndex);
            bdem_List *equitySaleListPtr =
                         &equityTradeList.theModifiableList(saleCondListIndex);
            bdem_List *priceListPtr =
                       &equityTradeList.theModifiableList(tradePriceListIndex);
            timeListPtr->reset(timeElemTypes, timeNumElemTypes);
            securityListPtr->reset(securityElemTypes, securityNumElemTypes);
            equitySaleListPtr->reset(equitySaleElemTypes,
                                     equitySaleNumElemTypes);
            priceListPtr->reset(priceElemTypes, priceNumElemTypes);

            // Populate some of the "additional" information.

            equityTradeList.theModifiableString(tickTypeIndex) = "ET";

            // Obtain pointer to the time list and populate the list

            bdet_Datetime dateTime;
            dateTime.date().setYearMonthDay(2002, 11, 20);
            dateTime.setTime(9, 30, 0, 0);
            timeListPtr->theModifiableDatetime(datetimeIndex) = dateTime;
            timeListPtr->theModifiableFloat(offsetIndex) = -5.0;

            // Populate some of the "additional" information.

            equityTradeList.theModifiableInt(seqNumIndex) = 1;
            equityTradeList.theModifiableString(sourceConIndex)    = "NASDAQ";
            equityTradeList.theModifiableString(marketCenterIndex) = "NASDAQ";

            // Obtain pointer to the security list and populate the list

            securityListPtr->theModifiableString(secIDIndex)     = "INTC";
            securityListPtr->theModifiableString(secIDTypeIndex) = "EXCH";

            // Obtain pointer to the sale condition list and populate the list.

            equitySaleListPtr->theModifiableString(saleCondIndex)     = "aa";
            equitySaleListPtr->theModifiableString(updateActionIndex) = "None";

            // Obtain pointer to the equity trade list and populate the list.

            priceListPtr->reset(priceElemTypes, priceNumElemTypes);
            priceListPtr->theModifiableFloat(priceIndex)       = 18.13F;
            priceListPtr->theModifiableString(priceDenomIndex) = "2";

            // Populate some of the "additional" information.

            equityTradeList.theModifiableInt(tradeSizeIndex)  = 2000;
            equityTradeList.theModifiableString(curCodeIndex) = "USD";

            // Send message

            if (veryVerbose) equityTradeList.print(cout);

            outStream.removeAll();
            bdex_OutStreamFunctions::streamOut(
                                    outStream,
                                    equityTradeList,
                                    equityTradeList.maxSupportedBdexVersion());
        }

        // Receive message
        const char *PD  = outStream.data();
        const int   NPB = outStream.length();

        bdex_ByteInStream inStream(PD, NPB);

        {
            bdem_List equityTradeList;
            bdex_InStreamFunctions::streamIn(
                                    inStream,
                                    equityTradeList,
                                    equityTradeList.maxSupportedBdexVersion());

            const int datetimeIndex = 0;
            const int offsetIndex = 1;

            const int secIDIndex = 0;
            const int secIDTypeIndex = 1;

            const int saleCondIndex = 0;
            const int updateActionIndex = 1;

            const int priceIndex = 0;
            const int priceDenomIndex = 1;

            const int tickTypeIndex = 0;
            const int timeListIndex = 1;
            const int seqNumIndex = 2;
            const int sourceConIndex = 3;
            const int marketCenterIndex = 4;
            const int securityListIndex = 5;
            const int saleCondListIndex = 6;
            const int tradePriceListIndex = 7;
            const int tradeSizeIndex = 8;
            const int curCodeIndex = 9;

            // Process message

            // Note that the "someVal*" variables are simply placeholders to
            // indicate how to access the information in a message.  In a
            // functioning program the message would either be used in the
            // form that it came in or the values would be accessed in the
            // manner shown below and used for some purpose.  Constructing an
            // aggregate which perfectly matches the receiver's schema is
            // possible and may produce a smaller aggregate than the message
            // received, though this extra copying is often not necessary
            // since the message provides a superset of the information
            // described by the receiver schema and this information is
            // easily accessible by the preprocessed index values.

            // Extract some of the "additional" information.

            bsl::string someval1 = equityTradeList.theModifiableString(
                                                                tickTypeIndex);
            ASSERT(someval1 == "ET");

            // Obtain pointer to the time list and extract the list.

            bdem_List *timeListPtr = &equityTradeList.theModifiableList(
                                                                timeListIndex);
            bdet_Datetime someval2 = timeListPtr->theModifiableDatetime(
                                                                datetimeIndex);
            float someval3 = timeListPtr->theModifiableFloat(
                                                                offsetIndex);
            //ASSERT(someval2 ==
            ASSERT(someval3 == -5.0);

            // Extract some of the "additional" information.

            int someval4 = equityTradeList.theInt(seqNumIndex);
            bsl::string someval5 = equityTradeList.theModifiableString(
                                                               sourceConIndex);
            bsl::string someval6 = equityTradeList.theModifiableString(
                                                            marketCenterIndex);

            ASSERT(someval4 == 1);
            ASSERT(someval5 == "NASDAQ");
            ASSERT(someval6 == "NASDAQ");

            // Obtain pointer to the security list and extract the list.

            bdem_List *securityListPtr =
                                   &equityTradeList.theModifiableList(
                                                            securityListIndex);
            bsl::string someval7 = securityListPtr->theModifiableString(
                                                            secIDIndex);
            bsl::string someval8 = securityListPtr->theModifiableString(
                                                            secIDTypeIndex);

            ASSERT(someval7 == "INTC");
            ASSERT(someval8 == "EXCH");

            // Obtain pointer to the equity sale condition list and extract
            // the list.

            bdem_List *equitySaleListPtr =
                                   &equityTradeList.theModifiableList(
                                                            saleCondListIndex);
            bsl::string someval9 = equitySaleListPtr->theModifiableString(
                                                            saleCondIndex);
            bsl::string someval10 =
                               equitySaleListPtr->theModifiableString(
                                                            updateActionIndex);

            ASSERT(someval9 == "aa");
            ASSERT(someval10 == "None");

            // Obtain pointer to the price list and extract the list.

            bdem_List *priceListPtr =
                    &equityTradeList.theModifiableList(tradePriceListIndex);
            float someval11 = priceListPtr->theModifiableFloat(priceIndex);
            bsl::string someval12 = priceListPtr->theModifiableString(
                                                              priceDenomIndex);

            ASSERT(someval11 == 18.13F);
            ASSERT(someval12 == "2");

            // Extract some of the "additional" information.

            int someval13 = equityTradeList.theModifiableInt(tradeSizeIndex);
            bsl::string someval14 = equityTradeList.theModifiableString(
                                                                 curCodeIndex);

            ASSERT(someval13 == 2000);
            ASSERT(someval14 == "USD");
        }

        if (verbose) cout << endl
                          << "Usage from component header file." << endl;
        {
///Usage
///-----
// The aggregate types that constitute this component are ideal for packaging
// complex results from one process and transmitting them to another process
// where they can be used either directly or after unpacking.  For example,
// consider a hierarchical list structure containing two elements, each of
// which is a list.  The first sublist element contains an 'INT' element
// followed by a 'DOUBLE' element.  The second sublist contains two 'INT'
// elements, the first of which is unset:
//..
//  LIST {
//     LIST {
//        INT 100
//        DOUBLE 200.0
//     }
//     LIST {
//        INT
//        INT 400
//     }
//  }
//..
// The above data structure can be created programmatically as follows:
//..
    bdem_List list;
//
    list.appendNullList();
    bdem_List *subListPtr = &list.theModifiableList(0);
    subListPtr->appendInt(100);
    subListPtr->appendDouble(200);
//
    list.appendNullList();
    subListPtr = &list.theModifiableList(1);
    subListPtr->appendNullInt();
    subListPtr->appendInt(400);
//..
// And the contents of the list can be verified as follows:
//..
    ASSERT(  2 == list.length());
//
    const bdem_List& firstList = list.theList(0);
    ASSERT(  2 == firstList.length());
    ASSERT(100 == firstList.theInt(0));
    ASSERT(200 == firstList.theDouble(1));
//
    const bdem_List& secondList = list.theList(1);
    ASSERT(  2 == secondList.length());
    ASSERT(       secondList[0].isNull());
    ASSERT(400 == secondList.theInt(1));
//..
        }
      }

DEFINE_TEST_CASE(39) {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //
        // Plan:
        //   Since the 'bdem_List' and 'bdem_Table' basic accessors
        //   forward to the 'bdem_Row' basic accessors, it is sufficient
        //   to verify that the 'bdem_Row' methods operate for a single
        //   test value and then thoroughly test the 'bdem_List' and
        //   'bdem_Table' methods.
        //
        //   1. Independent of any object, verify that each of the basic
        //      accessor methods under test has the expected signature.
        //   2. For each object X in a set of objects ordered by increasing
        //      length L:
        //      a. Verify that the 'length', 'numRows' and 'numColumns' methods
        //         return the expected value.
        //      b. Verify that the element type in each valid position within
        //         the my_ElemTypeArray returned by the 'elemTypes' and
        //         and 'columnTypes' methods is of the correct type.
        //      c. Verify that the element type returned by the 'elemType' and
        //         'columnType' methods is correct for every valid index.
        //
        //   Test with values from all 22 element types.  Create identical
        //   lists having potentially different internal representations.
        //
        // Testing:
        //   int                       bdem_Row::length() const;
        //   const my_ElemTypeArray& bdem_Row::elemTypes() const;
        //   bdem_ElemType::Type       bdem_Row::elemType(int index) const;
        //
        //   int                       bdem_List::length() const;
        //   const my_ElemTypeArray& bdem_List::elemTypes() const;
        //   bdem_ElemType::Type       bdem_List::elemType(int index) const;
        //
        //   int                       bdem_Table::numRows() const;
        //   int                       bdem_Table::numColumns() const;
        //   const my_ElemTypeArray& bdem_Table::columnTypes() const;
        //   bdem_ElemType::Type       bdem_Table::columnType(int index) const;
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl
                          << "Testing Basic Accessors" << endl
                          << "=======================" << endl;

        if (verbose) cout << endl
                          << "Testing 'bdem_Row' basic accessors." << endl;
        {
            const char *const SPEC     = "xBxC";
            const int         NUM_ELTS = (int)strlen(SPEC) / 2;

            List mL(&testAllocator);  const List& L = gg(&mL, SPEC);
            const Row& R = L.row();

            if (veryVerbose) { P_(SPEC); P(R); }

            ASSERT(R.length() == NUM_ELTS);

            my_ElemTypeArray e1;
            getElemTypes(&e1, R);
            const my_ElemTypeArray& ETA = e1;
            for (int ei = 0; ei < NUM_ELTS; ++ei) {
                const bdem_ElemType::Type ET = bdem_ElemType::Type
                                           (typeCharToIndex(SPEC[ei * 2 + 1]));
                ASSERT(ET == ETA[ei]);
                ASSERT(ET == R.elemType(ei));
            }
        }

        if (verbose) cout << endl
                          << "Testing 'bdem_List' basic accessors." << endl;
        {
            static const struct {
                int         d_lineNum;   // source line number
                const char *d_spec;      // spec for List object
            } DATA[] = {
                //line  l-spec
                //----  ------
                { L_,   "",                                              },

                { L_,   "xA",                                            },
                { L_,   "xB",                                            },
                { L_,   "xC",                                            },
                { L_,   "xD",                                            },
                { L_,   "xE",                                            },
                { L_,   "xF",                                            },
                { L_,   "xG",                                            },
                { L_,   "xH",                                            },
                { L_,   "xI",                                            },
                { L_,   "xJ",                                            },
                { L_,   "xK",                                            },
                { L_,   "xL",                                            },
                { L_,   "xM",                                            },
                { L_,   "xN",                                            },
                { L_,   "xO",                                            },
                { L_,   "xP",                                            },
                { L_,   "xQ",                                            },
                { L_,   "xR",                                            },
                { L_,   "xS",                                            },
                { L_,   "xT",                                            },
                { L_,   "xU",                                            },
                { L_,   "xV",                                            },

                { L_,   "uAuA",                                          },
                { L_,   "uLuB",                                          },
                { L_,   "uBuC",                                          },
                { L_,   "uMuD",                                          },
                { L_,   "uCuE",                                          },
                { L_,   "uNuF",                                          },
                { L_,   "xDxG",                                          },
                { L_,   "xOxH",                                          },
                { L_,   "xEyI",                                          },
                { L_,   "xPxJ",                                          },
                { L_,   "xFxK",                                          },
                { L_,   "xQxL",                                          },
                { L_,   "xGxM",                                          },
                { L_,   "xRxN",                                          },
                { L_,   "yHyO",                                          },
                { L_,   "ySyP",                                          },
                { L_,   "yIxQ",                                          },
                { L_,   "yTyR",                                          },
                { L_,   "yJyS",                                          },
                { L_,   "yUyT",                                          },
                { L_,   "yKyU",                                          },
                { L_,   "yVyV",                                          },

                { L_,   "uAuBuC",                                        },
                { L_,   "uDuEuF",                                        },
                { L_,   "uGuHuI",                                        },
                { L_,   "uJuKuL",                                        },
                { L_,   "xMxNxO",                                        },
                { L_,   "xPxQxR",                                        },
                { L_,   "xSxTyU",                                        },
                { L_,   "xVxAxB",                                        },
                { L_,   "xCyDxE",                                        },
                { L_,   "xFxGxH",                                        },
                { L_,   "yIyJyK",                                        },
                { L_,   "yLyMyN",                                        },
                { L_,   "yOyPyQ",                                        },
                { L_,   "yRxSxT",                                        },
                { L_,   "yUyVyV",                                        },

                { L_,   "xAxAxBxA",                                      },
                { L_,   "xBxCxBxB",                                      },
                { L_,   "xDxCxCxC",                                      },
                { L_,   "xDxDxDxDxD",                                    },
                { L_,   "xExExFxExE",                                    },
                { L_,   "xFxGxFxFxF",                                    },
                { L_,   "xHxGxGxGxG",                                    },
                { L_,   "xHxHxHxHxHxH",                                  },
                { L_,   "xIxIxIxJxIxI",                                  },
                { L_,   "xJxJxKxJxJxJ",                                  },
                { L_,   "xLxKxKxKxKxK",                                  },
                { L_,   "xLxLxLxLxLxLxL",                                },
                { L_,   "xMxMxMxMxMxNxM",                                },
                { L_,   "xOxNxNxNxNxNxN",                                },
                { L_,   "xPxPxPxPxPxPxPxP",                              },
                { L_,   "xQxQxQxQxQxRxQxQ",                              },
                { L_,   "xRxSxRxRxRxRxRxR",                              },
                { L_,   "xSxSxSxSxSxSxSxSxS",                            },
                { L_,   "xTxTxTxTxTxUxTxTxT",                            },
                { L_,   "xUxUxVxUxUxUxUxUxU",                            },
                { L_,   "xVxVxVxVxVxVxVxVxVxV",                          },
                { L_,   "xAxBxCxDxExFxGxHxIxJxKxLxMxNxOxPxQxRxSxTxUxV",  },
                { L_,   "yLyMyNyOyPyQyRySyTyUyVyAyByCyDyEyFyGyHyIyJyK",  },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE     = DATA[ti].d_lineNum;
                const char *L_SPEC   = DATA[ti].d_spec;
                const int   curLen   = (int)strlen(L_SPEC);
                const int   NUM_ELTS = curLen / 2;

                List mL(&testAllocator);  const List& L = gg(&mL, L_SPEC);
                LOOP_ASSERT(LINE, curLen == 2 * L.length()); // same lengths

                if (curLen != oldLen) {  // non-decreasing
                    if (verbose) cout << "\tUsing objects of length "
                                      << NUM_ELTS << '.' << endl;
                    LOOP2_ASSERT(LINE, L_SPEC, oldLen <= curLen);
                    oldLen = curLen;
                }
                if (veryVerbose) { P_(LINE); P_(L_SPEC); P(L); }

                LOOP_ASSERT(LINE, L.length() == NUM_ELTS);

                my_ElemTypeArray e1;
                getElemTypes(&e1, L);
                const my_ElemTypeArray& ETA = e1;
                for (int ei = 0; ei < NUM_ELTS; ++ei) {
                    const bdem_ElemType::Type ET = bdem_ElemType::Type
                                         (typeCharToIndex(L_SPEC[ei * 2 + 1]));
                    LOOP2_ASSERT(LINE, ei, ET == ETA[ei]);
                    LOOP2_ASSERT(LINE, ei, ET == L.elemType(ei));
                }
            }
        }

        if (verbose) cout << "\nTest objects having potentially different "
                             "internal representations." << endl;
        {
            static const struct {
                int         d_lineNum;   // source line number
                const char *d_spec;      // spec for List object
            } DATA[] = {
                //line  l-spec
                //----  ------
                { L_,   "",                                             },
                { L_,   "xA",                                           },
                { L_,   "xAxB",                                         },
                { L_,   "xAxBxC",                                       },
                { L_,   "xAxBxCxD",                                     },
                { L_,   "xAxBxCxDxE",                                   },
                { L_,   "xAxBxCxDxExAxB",                               },
                { L_,   "xAxBxCxDxExAxBxC",                             },
                { L_,   "xAxBxCxDxExAxBxCxD",                           },
                { L_,   "xAxBxCxDxExAxBxCxDxExAxBxCxDxE",               },
                { L_,   "xAxBxCxDxExAxBxCxDxExAxBxCxDxExA",             },
                { L_,   "xAxBxCxDxExAxBxCxDxExAxBxCxDxExAxB",           },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE     = DATA[ti].d_lineNum;
                const char *L_SPEC   = DATA[ti].d_spec;
                const int   curLen   = (int)strlen(L_SPEC);
                const int   NUM_ELTS = curLen / 2;

                if (curLen != oldLen) {  // strictly increasing
                    if (verbose) {
                        cout << "\tUsing objects having (logical) "
                                "length " << NUM_ELTS << '.' << endl;
                    }
                    LOOP2_ASSERT(LINE, L_SPEC, oldLen < curLen);
                    oldLen = curLen;
                }

                for (int v = 0; v < NUM_PERMUTATIONS; ++v) {
                    List mV(&testAllocator);
                    makePermutation(&mV, L_SPEC, v%2, v/2);
                    const List& V = mV;

                    LOOP2_ASSERT(LINE, v, V.length() == NUM_ELTS);

                    my_ElemTypeArray e1;
                    getElemTypes(&e1, V);
                    const my_ElemTypeArray& ETA = e1;
                    for (int ei = 0; ei < NUM_ELTS; ++ei) {
                        const bdem_ElemType::Type ET = bdem_ElemType::Type
                                         (typeCharToIndex(L_SPEC[ei * 2 + 1]));
                        LOOP3_ASSERT(LINE, v, ei, ET == ETA[ei]);
                        LOOP3_ASSERT(LINE, v, ei, ET == V.elemType(ei));
                    }
                }
            }
        }

        if (verbose) cout << endl
                          << "Testing 'bdem_Table' basic accessors." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec;     // spec for Table object
                int         d_nRows;    // number of rows in Table
                int         d_nCols;    // number of columns in Table
            } DATA[] = {
                //line  t-spec                               nrows    ncols
                //----  ------                               -----    -----
                { L_,   "",                                  0,       0,     },
                { L_,   ";",                                 1,       0,     },
                { L_,   ";;",                                2,       0,     },
                { L_,   ";;;",                               3,       0,     },
                { L_,   ";;;;",                              4,       0,     },
                { L_,   ";;;;;",                             5,       0,     },

                { L_,   "A;",                                0,       1,     },
                { L_,   "B;x;",                              1,       1,     },
                { L_,   "C;x;y;",                            2,       1,     },
                { L_,   "D;x;y;u;",                          3,       1,     },
                { L_,   "E;x;y;u;x;",                        4,       1,     },
                { L_,   "F;x;y;u;x;y;",                      5,       1,     },

                { L_,   "GH;",                               0,       2,     },
                { L_,   "IJ;xy;",                            1,       2,     },
                { L_,   "KL;xy;yx;",                         2,       2,     },
                { L_,   "MN;xy;yx;ux;",                      3,       2,     },
                { L_,   "OP;xy;yx;ux;xx;",                   4,       2,     },
                { L_,   "QR;xy;yx;ux;xx;yy;",                5,       2,     },

                { L_,   "STU;",                              0,       3,     },
                { L_,   "VBA;xyu;",                          1,       3,     },
                { L_,   "EDC;xyu;uyx;",                      2,       3,     },
                { L_,   "HGF;xyu;uyx;uxy;",                  3,       3,     },
                { L_,   "KJI;xyu;uyx;uxy;xxx;",              4,       3,     },
                { L_,   "NML;xyu;uyx;uxy;xxx;yyy;",          5,       3,     },

                { L_,   "RQPO;",                             0,       4,     },
                { L_,   "VUTS;xyux;",                        1,       4,     },
                { L_,   "DCBA;xyux;uyxu;",                   2,       4,     },
                { L_,   "HGFE;xyux;uyxu;uxyu;",              3,       4,     },
                { L_,   "LKJI;xyux;uyxu;uxyu;xxxx;",         4,       4,     },
                { L_,   "PONM;xyux;uyxu;uxyu;xxxx;yyyy;",    5,       4,     },

                { L_,   "UTSRQ;",                            0,       5,     },
                { L_,   "VABCD;xyuxy;",                      1,       5,     },
                { L_,   "EDFGI;xyuxy;uyxuy;",                2,       5,     },
                { L_,   "JKLMN;xyuxy;uyxuy;uxyux;",          3,       5,     },
                { L_,   "OPQRS;xyuxy;uyxuy;uxyux;xxxxx;",    4,       5,     },
                { L_,   "TUVAB;xyuxy;uyxuy;uxyux;xxxxx;yyyyy;", 5,    5,     },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *T_SPEC = DATA[ti].d_spec;
                const int   NROWS  = DATA[ti].d_nRows;
                const int   NCOLS  = DATA[ti].d_nCols;

                Table mT(&testAllocator);  const Table& T = hh(&mT, T_SPEC);

                if (veryVerbose) {
                    P_(LINE); P_(T_SPEC); P_(NROWS); P_(NCOLS); P(T);
                }

                LOOP_ASSERT(LINE, T.numRows()    == NROWS);
                LOOP_ASSERT(LINE, T.numColumns() == NCOLS);

                my_ElemTypeArray e1;
                getElemTypes(&e1, T);
                const my_ElemTypeArray& ETA = e1;
                for (int ei = 0; ei < NCOLS; ++ei) {
                    const bdem_ElemType::Type ET = bdem_ElemType::Type
                                                 (typeCharToIndex(T_SPEC[ei]));
                    LOOP2_ASSERT(LINE, ei, ET == ETA[ei]);
                    LOOP2_ASSERT(LINE, ei, ET == T.columnType(ei));
                }
            }
        }
      }

DEFINE_TEST_CASE(38) {
        // --------------------------------------------------------------------
        // TESTING 'bdem_Row', 'bdem_List', 'bdem_Table' PRINT METHODS
        //
        //   The 'print' methods format the value of the object directly from
        //   the underlying state information according to supplied arguments.
        //   Ensure that the methods format properly for:
        //     - unset and set values.
        //     - negative, 0, and positive levels.
        //     - 0 and non-zero spaces per level.
        // Plan:
        //   Since the 'bdem_List' and 'bdem_Table' 'print' methods both
        //   forward to the 'bdem_Row' 'print' method, it is sufficient
        //   to verify that the 'bdem_Row' method operates for a single
        //   test value and then thoroughly test the 'bdem_List' and
        //   'bdem_Table' methods.
        //
        //   Values from all of the 22 element types are used in the tests
        //   of the 'bdem_List' and 'bdem_Table' 'print' methods.  For each
        //   of an enumerated set of objects, 'level', and 'spacesPerLevel'
        //   values, ordered by increasing object length, use 'ostrstream' to
        //   'print' that object's value, using the tabulated parameters, to
        //   two separate character buffers each with different initial values.
        //   Compare the contents of these buffers with the literal expected
        //   output format and verify that the characters beyond the null
        //   characters are unaffected in both buffers.
        //
        // Testing:
        //   ostream& bdem_Row::print(ostream& os, int level, int spl) const;
        //   ostream& bdem_List::print(ostream& os, int level, int spl) const;
        //   ostream& bdem_Table::print(ostream& os, int level, int spl) const;
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl
                          << "Testing 'print' methods" << endl
                          << "=======================" << endl;

        const int  SIZE = 2048;  // Must be big enough to hold output string.
        const char Z1   = (char) 0xFF;  // Used to represent an unset char.
        const char Z2   = 0x00;  // Value 2 used to represent an unset char.

        char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
        char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
        const char *CTRL_BUF1 = mCtrlBuf1;
        const char *CTRL_BUF2 = mCtrlBuf2;

        if (verbose) cout << "\nTesting 'bdem_Row' 'print' method" << endl;
        {
            char buf1[SIZE], buf2[SIZE];
            memcpy(buf1, CTRL_BUF1, SIZE);  // Preset buf1 to Z1 values.
            memcpy(buf2, CTRL_BUF2, SIZE);  // Preset buf2 to Z2 values.

            const int IND          = 0;
            const int SPL          = 1;
            const char *const SPEC = "xBxC";
            const char *const FMT  = "{\n SHORT 12\n INT 103\n}\n";

            List mL(&testAllocator);  const List& L = gg(&mL, SPEC);
            const Row& X = L.row();

            if (verbose) {
                cout << "\t\tSpec = \"" << SPEC << "\", ";
                P_(IND); P(SPL);
            }
            if (veryVerbose) cout << "EXPECTED FORMAT:" << endl << FMT << endl;
            ostrstream out1(buf1, SIZE);  X.print(out1, IND, SPL) << ends;
            ostrstream out2(buf2, SIZE);  X.print(out2, IND, SPL) << ends;
            if (veryVerbose) cout << "ACTUAL FORMAT:" << endl << buf1 << endl;

            const int SZ = (int)strlen(FMT) + 1;
            const int REST = SIZE - SZ;
            ASSERT(SZ < SIZE);  // Check buffer is large enough.
            ASSERT(Z1 == buf1[SIZE - 1]);  // Check for overrun.
            ASSERT(Z2 == buf2[SIZE - 1]);  // Check for overrun.
            ASSERT(0 == memcmp(buf1, FMT, SZ));
            ASSERT(0 == memcmp(buf2, FMT, SZ));
            ASSERT(0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
            ASSERT(0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ, REST));
        }

        if (verbose) cout << "\nTesting 'bdem_List' 'print' method" << endl;
#define NL "\n"
        {
            static const struct {
                int         d_lineNum;   // source line number
                const char *d_spec_p;    // specification string
                int         d_indent;    // indentation level
                int         d_spaces;    // spaces per indentation level
                const char *d_fmt_p;     // expected output format
            } DATA[] = {
                //line  spec    indent +/-  spaces      expected      // ADJUST
                //----  ----    ----------  ------      --------
                { L_,   "",            0,         0,    "{"             NL
                                                        "}"             NL   },

                { L_,   "",            0,         2,    "{"             NL
                                                        "}"             NL   },

                { L_,   "",            1,         1,    " {"            NL
                                                        " }"            NL   },

                { L_,   "",            1,         2,    "  {"           NL
                                                        "  }"           NL   },

                { L_,   "",           -1,         2,    "{"             NL
                                                        "  }"           NL   },

                { L_,   "xA",         -2,         1,    "{"             NL
                                                        "   CHAR x"     NL
                                                        "  }"           NL   },

                { L_,   "xA",          0,         1,    "{"             NL
                                                        " CHAR x"       NL
                                                        "}"             NL   },

                { L_,   "xB",          0,         1,    "{"             NL
                                                        " SHORT 12"     NL
                                                        "}"             NL   },

                { L_,   "uB",          0,         1,    "{"             NL
                                                        " SHORT -32768" NL
                                                        "}"             NL   },

                { L_,   "xC",          0,         1,    "{"             NL
                                                        " INT 103"      NL
                                                        "}"             NL   },

                { L_,   "uC",          0,         1,    "{"             NL
                                                     " INT -2147483648" NL
                                                        "}"             NL   },

                { L_,   "xD",          0,         1,    "{"             NL
                                                        " INT64 10004"  NL
                                                        "}"             NL   },

                { L_,   "uD",          0,         1,    "{"             NL
                                          " INT64 -9223372036854775808" NL
                                                        "}"             NL   },

                { L_,   "xE",          0,         1,    "{"             NL
                                                        " FLOAT 105.5"  NL
                                                        "}"             NL   },
#ifdef BSLS_PLATFORM__CMP_MSVC
                { L_,   "uE",          0,         1,    "{"             NL
                                                  " FLOAT -6.56419e-015" NL
                                                        "}"             NL   },
#else
                { L_,   "uE",          0,         1,    "{"             NL
                                                  " FLOAT -6.56419e-15" NL
                                                        "}"             NL   },
#endif
                { L_,   "xF",          0,         1,    "{"             NL
                                                      " DOUBLE 106.006" NL
                                                        "}"             NL   },
#ifdef BSLS_PLATFORM__CMP_MSVC
                { L_,   "uF",          0,         1,    "{"             NL
                                                 " DOUBLE -2.42454e-014" NL
                                                        "}"             NL   },
#else
                { L_,   "uF",          0,         1,    "{"             NL
                                                 " DOUBLE -2.42454e-14" NL
                                                        "}"             NL   },
#endif
                { L_,   "xG",          0,         1,    "{"             NL
                                                 " STRING one-seven---" NL
                                                        "}"             NL   },

                { L_,   "uG",          0,         1,    "{"             NL
                                                        " STRING "      NL
                                                        "}"             NL   },

                { L_,   "xH",          0,         1,    "{"             NL
                                     " DATETIME 08AUG0108_08:08:08.108" NL
                                                        "}"             NL   },

                { L_,   "uH",          0,         1,    "{"             NL
                                     " DATETIME 01JAN0001_24:00:00.000" NL
                                                        "}"             NL   },

                { L_,   "xI",          0,         1,    "{"             NL
                                                      " DATE 09SEP0109" NL
                                                        "}"             NL   },

                { L_,   "uI",          0,         1,    "{"             NL
                                                      " DATE 01JAN0001" NL
                                                        "}"             NL   },

                { L_,   "xJ",          0,         1,    "{"             NL
                                                   " TIME 10:10:10.110" NL
                                                        "}"             NL   },

                { L_,   "uJ",          0,         1,    "{"             NL
                                                   " TIME 24:00:00.000" NL
                                                        "}"             NL   },

                { L_,   "xK",          0,         1,    "{"             NL
                                                        " CHAR_ARRAY "
                                                        "\"xNxNx\""     NL
                                                        "}"             NL   },

                { L_,   "uK",          0,         1,    "{"             NL
                                                        " CHAR_ARRAY \"\"" NL
                                                        "}"             NL   },

                { L_,   "xL",          0,         1,    "{"             NL
                                                       " SHORT_ARRAY [" NL
                                                       "  12"           NL
                                                       "  -32768"       NL
                                                       " ]"             NL
                                                        "}"             NL   },

                { L_,   "uL",          0,         1,    "{"             NL
                                                       " SHORT_ARRAY [" NL
                                                       " ]"             NL
                                                        "}"             NL   },

                { L_,   "xM",          0,         1,    "{"             NL
                                                        " INT_ARRAY ["  NL
                                                        "  103"         NL
                                                        "  -2147483648" NL
                                                        " ]"            NL
                                                        "}"             NL   },

                { L_,   "uM",          0,         1,    "{"             NL
                                                        " INT_ARRAY ["  NL
                                                        " ]"            NL
                                                        "}"             NL   },

                { L_,   "xN",          0,         1,    "{"             NL
                                               " INT64_ARRAY ["         NL
                                               "  10004"                NL
                                               "  -9223372036854775808" NL
                                               " ]"                     NL
                                                        "}"             NL   },

                { L_,   "uN",          0,         1,    "{"             NL
                                                       " INT64_ARRAY [" NL
                                                       " ]"             NL
                                                        "}"             NL   },
#ifdef BSLS_PLATFORM__CMP_MSVC
                { L_,   "xO",          0,         1,    "{"             NL
                                                       " FLOAT_ARRAY [" NL
                                                       "  105.5"        NL
                                                       "  -6.56419e-015" NL
                                                       " ]"             NL
                                                        "}"             NL   },
#else
                { L_,   "xO",          0,         1,    "{"             NL
                                                       " FLOAT_ARRAY [" NL
                                                       "  105.5"        NL
                                                       "  -6.56419e-15" NL
                                                       " ]"             NL
                                                        "}"             NL   },
#endif

                { L_,   "uO",          0,         1,    "{"             NL
                                                       " FLOAT_ARRAY [" NL
                                                       " ]"             NL
                                                        "}"             NL   },

#ifdef BSLS_PLATFORM__CMP_MSVC
                { L_,   "xP",          0,         1,    "{"             NL
                                                      " DOUBLE_ARRAY [" NL
                                                      "  106.006"       NL
                                                      "  -2.42454e-014"  NL
                                                      " ]"              NL
                                                        "}"             NL   },
#else
                { L_,   "xP",          0,         1,    "{"             NL
                                                      " DOUBLE_ARRAY [" NL
                                                      "  106.006"       NL
                                                      "  -2.42454e-14"  NL
                                                      " ]"              NL
                                                        "}"             NL   },
#endif
                { L_,   "uP",          0,         1,    "{"             NL
                                                      " DOUBLE_ARRAY [" NL
                                                      " ]"              NL
                                                        "}"             NL   },

                { L_,   "xQ",          0,         1,    "{"             NL
                                                      " STRING_ARRAY [" NL
                                                      "  one-seven---"  NL
                                                      "  "              NL
                                                      " ]"              NL
                                                        "}"             NL   },

                { L_,   "uQ",          0,         1,    "{"             NL
                                                      " STRING_ARRAY [" NL
                                                      " ]"              NL
                                                        "}"             NL   },

                { L_,   "xR",          0,         1,    "{"             NL
                                             " DATETIME_ARRAY ["        NL
                                             "  08AUG0108_08:08:08.108" NL
                                             "  01JAN0001_24:00:00.000" NL
                                             " ]"                       NL
                                                        "}"             NL   },

                { L_,   "uR",          0,         1,    "{"             NL
                                                    " DATETIME_ARRAY [" NL
                                                    " ]"                NL
                                                        "}"             NL   },

                { L_,   "xS",          0,         1,    "{"             NL
                                                        " DATE_ARRAY [" NL
                                                        "  09SEP0109"   NL
                                                        "  01JAN0001"   NL
                                                        " ]"            NL
                                                        "}"             NL   },

                { L_,   "uS",          0,         1,    "{"             NL
                                                        " DATE_ARRAY [" NL
                                                        " ]"            NL
                                                        "}"             NL   },

                { L_,   "xT",          0,         1,    "{"             NL
                                                       " TIME_ARRAY ["  NL
                                                       "  10:10:10.110" NL
                                                       "  24:00:00.000" NL
                                                       " ]"             NL
                                                        "}"             NL   },

                { L_,   "uT",          0,         1,    "{"             NL
                                                        " TIME_ARRAY [" NL
                                                        " ]"            NL
                                                        "}"             NL   },

                { L_,   "xU",          0,         1,    "{"             NL
                                                " LIST {"               NL
                                                "  INT 103"             NL
                                                "  STRING one-seven---" NL
                                                " }"                    NL
                                                        "}"             NL   },

                { L_,   "uU",          0,         1,    "{"             NL
                                                        " LIST {"       NL
                                                        " }"            NL
                                                        "}"             NL   },

                { L_,   "xV",          0,         1,    "{"             NL
                                               " TABLE {"               NL
                                               "  Column Types: ["      NL
                                               "   INT"                 NL
                                               "   STRING"              NL
                                               "  ]"                    NL
                                               "  Row 0: {"             NL
                                               "   103"                 NL
                                               "   one-seven---"        NL
                                               "  }"                    NL
                                               "  Row 1: {"             NL
                                               "   -2147483648"         NL
                                               "   "                    NL
                                               "  }"                    NL
                                               " }"                     NL
                                                        "}"             NL   },

                { L_,   "uV",          0,         1,    "{"             NL
                                                    " TABLE {"          NL
                                                    "  Column Types: [" NL
                                                    "  ]"               NL
                                                    " }"                NL
                                                        "}"             NL   },

                { L_,   "xBxC",        1,         2,    "  {"           NL
                                                        "    SHORT 12"  NL
                                                        "    INT 103"   NL
                                                        "  }"           NL   },

                { L_,   "xBxC",        2,         1,    "  {"           NL
                                                        "   SHORT 12"   NL
                                                        "   INT 103"    NL
                                                        "  }"           NL   },

                { L_,   "xAxBxCxDxE",  1,         3,    "   {"          NL
                                                    "      CHAR x"      NL
                                                    "      SHORT 12"    NL
                                                    "      INT 103"     NL
                                                    "      INT64 10004" NL
                                                    "      FLOAT 105.5" NL
                                                        "   }"          NL   },
            };
#undef NL

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const int         IND    = DATA[ti].d_indent;
                const int         SPL    = DATA[ti].d_spaces;
                const char *const FMT    = DATA[ti].d_fmt_p;
                const int         curLen = (int)strlen(SPEC);

                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE);  // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE);  // Preset buf2 to Z2 values.

                List mX(&testAllocator);  const List& X = gg(&mX, SPEC);
                LOOP_ASSERT(LINE, curLen == 2 * X.length());  // same lengths

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton objects of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen < curLen);  // strictly increasing
                    oldLen = curLen;
                }

                if (verbose) {
                    cout << "\t\tSpec = \"" << SPEC << "\", ";
                    P_(IND); P(SPL);
                }
                if (veryVerbose) cout << "EXPECTED FORMAT:"
                                      << endl << FMT << endl;
                ostrstream out1(buf1, SIZE);  X.print(out1, IND, SPL) << ends;
                ostrstream out2(buf2, SIZE);  X.print(out2, IND, SPL) << ends;
                if (veryVerbose) cout << "ACTUAL FORMAT:"
                                      << endl << buf1 << endl;

                const int SZ = (int)strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LINE, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP3_ASSERT(LINE, buf1, FMT, 0 == memcmp(buf1, FMT, SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(buf2, FMT, SZ));
                LOOP_ASSERT(LINE,0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
                LOOP_ASSERT(LINE,0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ, REST));
            }
        }

        if (verbose) cout << "\nTesting 'bdem_Table' 'print' method" << endl;
        {
            static const struct {
                int         d_lineNum;   // source line number
                const char *d_spec_p;    // specification string
                int         d_indent;    // indentation level
                int         d_spaces;    // spaces per indentation level
                const char *d_fmt_p;     // expected output format
            } DATA[] = {
                //line  spec    indent +/-  spaces      expected      // ADJUST
                //----  ----    ----------  ------      --------
                { L_,   "",            0,         0,    "{\n"
                                                        "Column Types: [\n"
                                                        "]\n"
                                                        "}\n"                },

                { L_,   "",            0,         2,    "{\n"
                                                        "  Column Types: [\n"
                                                        "  ]\n"
                                                        "}\n"                },

                { L_,   "",            1,         1,    " {\n"
                                                        "  Column Types: [\n"
                                                        "  ]\n"
                                                        " }\n"               },

                { L_,   "",            1,         2,    "  {\n"
                                                        "    Column Types: [\n"
                                                        "    ]\n"
                                                        "  }\n"              },

                { L_,   "",           -1,         2,    "{\n"
                                                        "    Column Types: [\n"
                                                        "    ]\n"
                                                        "  }\n"              },

                { L_,   "A;x;",         -2,         1,  "{\n"
                                                        "   Column Types: [\n"
                                                        "    CHAR\n"
                                                        "   ]\n"
                                                        "   Row 0: {\n"
                                                        "    x\n"
                                                        "   }\n"
                                                        "  }\n"              },

                { L_,   "A;x;",          0,         1,  "{\n"
                                                        " Column Types: [\n"
                                                        "  CHAR\n"
                                                        " ]\n"
                                                        " Row 0: {\n"
                                                        "  x\n"
                                                        " }\n"
                                                        "}\n"                },

                { L_,   "B;x;",          0,         1,  "{\n"
                                                        " Column Types: [\n"
                                                        "  SHORT\n"
                                                        " ]\n"
                                                        " Row 0: {\n"
                                                        "  12\n"
                                                        " }\n"
                                                        "}\n"                },

                { L_,   "C;x;",          0,         1,  "{\n"
                                                        " Column Types: [\n"
                                                        "  INT\n"
                                                        " ]\n"
                                                        " Row 0: {\n"
                                                        "  103\n"
                                                        " }\n"
                                                        "}\n"                },

                { L_,   "D;x;",          0,         1,  "{\n"
                                                        " Column Types: [\n"
                                                        "  INT64\n"
                                                        " ]\n"
                                                        " Row 0: {\n"
                                                        "  10004\n"
                                                        " }\n"
                                                        "}\n"                },

                { L_,   "E;x;",          0,         1,  "{\n"
                                                        " Column Types: [\n"
                                                        "  FLOAT\n"
                                                        " ]\n"
                                                        " Row 0: {\n"
                                                        "  105.5\n"
                                                        " }\n"
                                                        "}\n"                },

                { L_,   "F;x;",          0,         1,  "{\n"
                                                        " Column Types: [\n"
                                                        "  DOUBLE\n"
                                                        " ]\n"
                                                        " Row 0: {\n"
                                                        "  106.006\n"
                                                        " }\n"
                                                        "}\n"                },

                { L_,   "G;x;",          0,         1,  "{\n"
                                                        " Column Types: [\n"
                                                        "  STRING\n"
                                                        " ]\n"
                                                        " Row 0: {\n"
                                                        "  one-seven---\n"
                                                        " }\n"
                                                        "}\n"                },

                { L_,   "H;x;",          0,         1,
                                                  "{\n"
                                                  " Column Types: [\n"
                                                  "  DATETIME\n"
                                                  " ]\n"
                                                  " Row 0: {\n"
                                                  "  08AUG0108_08:08:08.108\n"
                                                  " }\n"
                                                  "}\n"                      },

                { L_,   "I;x;",          0,         1,  "{\n"
                                                        " Column Types: [\n"
                                                        "  DATE\n"
                                                        " ]\n"
                                                        " Row 0: {\n"
                                                        "  09SEP0109\n"
                                                        " }\n"
                                                        "}\n"                },

                { L_,   "J;x;",          0,         1,  "{\n"
                                                        " Column Types: [\n"
                                                        "  TIME\n"
                                                        " ]\n"
                                                        " Row 0: {\n"
                                                        "  10:10:10.110\n"
                                                        " }\n"
                                                        "}\n"                },

                { L_,   "K;x;",          0,         1,  "{\n"
                                                        " Column Types: [\n"
                                                        "  CHAR_ARRAY\n"
                                                        " ]\n"
                                                        " Row 0: {\n"
                                                        "  \"xNxNx\"\n"
                                                        " }\n"
                                                        "}\n"                },

                { L_,   "L;x;",          0,         1,  "{\n"
                                                        " Column Types: [\n"
                                                        "  SHORT_ARRAY\n"
                                                        " ]\n"
                                                        " Row 0: {\n"
                                                        "  [\n"
                                                        "   12\n"
                                                        "   -32768\n"
                                                        "  ]\n"
                                                        " }\n"
                                                        "}\n"                },

                { L_,   "M;x;",          0,         1,  "{\n"
                                                        " Column Types: [\n"
                                                        "  INT_ARRAY\n"
                                                        " ]\n"
                                                        " Row 0: {\n"
                                                        "  [\n"
                                                        "   103\n"
                                                        "   -2147483648\n"
                                                        "  ]\n"
                                                        " }\n"
                                                        "}\n"                },

                { L_,   "N;x;",          0,         1,
                                                    "{\n"
                                                    " Column Types: [\n"
                                                    "  INT64_ARRAY\n"
                                                    " ]\n"
                                                    " Row 0: {\n"
                                                    "  [\n"
                                                    "   10004\n"
                                                    "   -9223372036854775808\n"
                                                    "  ]\n"
                                                    " }\n"
                                                    "}\n"                    },
#ifdef BSLS_PLATFORM__CMP_MSVC
                { L_,   "O;x;",          0,         1,  "{\n"
                                                        " Column Types: [\n"
                                                        "  FLOAT_ARRAY\n"
                                                        " ]\n"
                                                        " Row 0: {\n"
                                                        "  [\n"
                                                        "   105.5\n"
                                                        "   -6.56419e-015\n"
                                                        "  ]\n"
                                                        " }\n"
                                                        "}\n"                },
                { L_,   "P;x;",          0,         1,  "{\n"
                                                        " Column Types: [\n"
                                                        "  DOUBLE_ARRAY\n"
                                                        " ]\n"
                                                        " Row 0: {\n"
                                                        "  [\n"
                                                        "   106.006\n"
                                                        "   -2.42454e-014\n"
                                                        "  ]\n"
                                                        " }\n"
                                                        "}\n"                },
#else
                { L_,   "O;x;",          0,         1,  "{\n"
                                                        " Column Types: [\n"
                                                        "  FLOAT_ARRAY\n"
                                                        " ]\n"
                                                        " Row 0: {\n"
                                                        "  [\n"
                                                        "   105.5\n"
                                                        "   -6.56419e-15\n"
                                                        "  ]\n"
                                                        " }\n"
                                                        "}\n"                },
                { L_,   "P;x;",          0,         1,  "{\n"
                                                        " Column Types: [\n"
                                                        "  DOUBLE_ARRAY\n"
                                                        " ]\n"
                                                        " Row 0: {\n"
                                                        "  [\n"
                                                        "   106.006\n"
                                                        "   -2.42454e-14\n"
                                                        "  ]\n"
                                                        " }\n"
                                                        "}\n"                },
#endif
                { L_,   "Q;x;",          0,         1,  "{\n"
                                                        " Column Types: [\n"
                                                        "  STRING_ARRAY\n"
                                                        " ]\n"
                                                        " Row 0: {\n"
                                                        "  [\n"
                                                        "   one-seven---\n"
                                                        "   \n"
                                                        "  ]\n"
                                                        " }\n"
                                                        "}\n"                },

                { L_,   "R;x;",          0,         1,
                                                  "{\n"
                                                  " Column Types: [\n"
                                                  "  DATETIME_ARRAY\n"
                                                  " ]\n"
                                                  " Row 0: {\n"
                                                  "  [\n"
                                                  "   08AUG0108_08:08:08.108\n"
                                                  "   01JAN0001_24:00:00.000\n"
                                                  "  ]\n"
                                                  " }\n"
                                                  "}\n"                      },

                { L_,   "S;x;",          0,         1,  "{\n"
                                                        " Column Types: [\n"
                                                        "  DATE_ARRAY\n"
                                                        " ]\n"
                                                        " Row 0: {\n"
                                                        "  [\n"
                                                        "   09SEP0109\n"
                                                        "   01JAN0001\n"
                                                        "  ]\n"
                                                        " }\n"
                                                        "}\n"                },

                { L_,   "T;x;",          0,         1,  "{\n"
                                                        " Column Types: [\n"
                                                        "  TIME_ARRAY\n"
                                                        " ]\n"
                                                        " Row 0: {\n"
                                                        "  [\n"
                                                        "   10:10:10.110\n"
                                                        "   24:00:00.000\n"
                                                        "  ]\n"
                                                        " }\n"
                                                        "}\n"                },

                { L_,   "U;x;",          0,         1,
                                                     "{\n"
                                                     " Column Types: [\n"
                                                     "  LIST\n"
                                                     " ]\n"
                                                     " Row 0: {\n"
                                                     "  {\n"
                                                     "   INT 103\n"
                                                     "   STRING one-seven---\n"
                                                     "  }\n"
                                                     " }\n"
                                                     "}\n"                 },

                { L_,   "V;x;",          0,         1,  "{\n"
                                                        " Column Types: [\n"
                                                        "  TABLE\n"
                                                        " ]\n"
                                                        " Row 0: {\n"
                                                        "  {\n"
                                                        "   Column Types: [\n"
                                                        "    INT\n"
                                                        "    STRING\n"
                                                        "   ]\n"
                                                        "   Row 0: {\n"
                                                        "    103\n"
                                                        "    one-seven---\n"
                                                        "   }\n"
                                                        "   Row 1: {\n"
                                                        "    -2147483648\n"
                                                        "    \n"
                                                        "   }\n"
                                                        "  }\n"
                                                        " }\n"
                                                        "}\n"                },

                { L_,   "BC;xx;",        1,         2,  "  {\n"
                                                        "    Column Types: [\n"
                                                        "      SHORT\n"
                                                        "      INT\n"
                                                        "    ]\n"
                                                        "    Row 0: {\n"
                                                        "      12\n"
                                                        "      103\n"
                                                        "    }\n"
                                                        "  }\n"              },

                { L_,   "BC;xx;",        2,         1,  "  {\n"
                                                        "   Column Types: [\n"
                                                        "    SHORT\n"
                                                        "    INT\n"
                                                        "   ]\n"
                                                        "   Row 0: {\n"
                                                        "    12\n"
                                                        "    103\n"
                                                        "   }\n"
                                                        "  }\n"              },

                { L_,   "VBUVQ;xxxxx;yyyyy;",  1,   3,
                                            "   {\n"
                                            "      Column Types: [\n"
                                            "         TABLE\n"
                                            "         SHORT\n"
                                            "         LIST\n"
                                            "         TABLE\n"
                                            "         STRING_ARRAY\n"
                                            "      ]\n"
                                            "      Row 0: {\n"
                                            "         {\n"
                                            "            Column Types: [\n"
                                            "               INT\n"
                                            "               STRING\n"
                                            "            ]\n"
                                            "            Row 0: {\n"
                                            "               103\n"
                                            "               one-seven---\n"
                                            "            }\n"
                                            "            Row 1: {\n"
                                            "               -2147483648\n"
                                            "               \n"
                                            "            }\n"
                                            "         }\n"
                                            "         12\n"
                                            "         {\n"
                                            "            INT 103\n"
                                            "            STRING one-seven---\n"
                                            "         }\n"
                                            "         {\n"
                                            "            Column Types: [\n"
                                            "               INT\n"
                                            "               STRING\n"
                                            "            ]\n"
                                            "            Row 0: {\n"
                                            "               103\n"
                                            "               one-seven---\n"
                                            "            }\n"
                                            "            Row 1: {\n"
                                            "               -2147483648\n"
                                            "               \n"
                                            "            }\n"
                                            "         }\n"
                                            "         [\n"
                                            "            one-seven---\n"
                                            "            \n"
                                            "         ]\n"
                                            "      }\n"
                                            "      Row 1: {\n"
                                            "         {\n"
                                            "            Column Types: [\n"
                                            "               INT\n"
                                            "               STRING\n"
                                            "            ]\n"
                                            "            Row 0: {\n"
                                            "               203\n"
                                            "               two-seven\n"
                                            "            }\n"
                                            "            Row 1: {\n"
                                            "               -2147483648\n"
                                            "               \n"
                                            "            }\n"
                                            "         }\n"
                                            "         22\n"
                                            "         {\n"
                                            "            INT 203\n"
                                            "            STRING two-seven\n"
                                            "         }\n"
                                            "         {\n"
                                            "            Column Types: [\n"
                                            "               INT\n"
                                            "               STRING\n"
                                            "            ]\n"
                                            "            Row 0: {\n"
                                            "               203\n"
                                            "               two-seven\n"
                                            "            }\n"
                                            "            Row 1: {\n"
                                            "               -2147483648\n"
                                            "               \n"
                                            "            }\n"
                                            "         }\n"
                                            "         [\n"
                                            "            two-seven\n"
                                            "            \n"
                                            "         ]\n"
                                            "      }\n"
                                            "   }\n"                         },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const int         IND    = DATA[ti].d_indent;
                const int         SPL    = DATA[ti].d_spaces;
                const char *const FMT    = DATA[ti].d_fmt_p;
                const int         curLen = (int)strlen(SPEC);

                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE);  // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE);  // Preset buf2 to Z2 values.

                Table mX(&testAllocator);  const Table& X = hh(&mX, SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton objects of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen < curLen);  // strictly increasing
                    oldLen = curLen;
                }

                if (verbose) {
                    cout << "\t\tSpec = \"" << SPEC << "\", ";
                    P_(IND); P(SPL);
                }
                if (veryVerbose) cout << "EXPECTED FORMAT:"
                                      << endl << FMT << endl;
                ostrstream out1(buf1, SIZE);  X.print(out1, IND, SPL) << ends;
                ostrstream out2(buf2, SIZE);  X.print(out2, IND, SPL) << ends;
                if (veryVerbose) cout << "ACTUAL FORMAT:"
                                      << endl << buf1 << endl;

                const int SZ = (int)strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LINE, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, 0 == memcmp(buf1, FMT, SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(buf2, FMT, SZ));
                LOOP_ASSERT(LINE,0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
                LOOP_ASSERT(LINE,0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ, REST));
            }
        }
      }

DEFINE_TEST_CASE(37) {
        // --------------------------------------------------------------------
        // TESTING BDEM_TABLE'S 'THE' MANIPULATOR AND ACCESSOR METHODS
        //
        // Plan:
        //   Since the 'bdem_List' and 'bdem_Table' methods forward to
        //   the 'bdem_Row' methods, it is sufficient to verify that the
        //   'bdem_Row' methods operate for a single test value and then
        //   thoroughly test the 'bdem_List' and 'bdem_Table' methods.
        //   Values from all of the 22 element types are used in the tests.
        //
        //   Apart from thoroughness, the strategy for testing the 'the'
        //   methods of 'bdem_Row', 'bdem_List' and 'bdem_Table' is similar.
        //   For each of the 22 types:
        //     * Construct non-const and const objects.
        //     * Access the elements of each of the objects using the 'the'
        //       methods.
        //     * Verify that non-const objects can indeed be modified
        //       through references returned by non-const 'the' methods.
        //     * Verify that the const and non-const 'the' methods
        //       have the proper signatures.  (This latter test is done
        //       independent of any object.)
        //
        // Testing:
        //   'bdem_Table' public interface
        //
        //   bdem_Row&.row(int ri);
        //   char& theChar(int ri, int ci);
        //   short& theShort(int ri, int ci);
        //   int& theInt(int ri, int ci);
        //   Int64& theInt64(int ri, int ci);
        //   float& theFloat(int ri, int ci);
        //   double& theDouble(int ri, int ci);
        //   bsl::string& theString(int ri, int ci);
        //   bdet_Datetime& theDatetime(int ri, int ci);
        //   bdet_Date& theDate(int ri, int ci);
        //   bdet_Time& theTime(int ri, int ci);
        //   bsl::vector<char>& theCharArray(int ri, int ci);
        //   bsl::vector<short>& theShortArray(int ri, int ci);
        //   bsl::vector<int>& theIntArray(int ri, int ci);
        //   bsl::vector<Int64>& theInt64Array(int ri, int ci);
        //   bsl::vector<float>& theFloatArray(int ri, int ci);
        //   bsl::vector<double>& theDoubleArray(int ri, int ci);
        //   bsl::vector<bsl::string>& theStringArray(int ri, int ci);
        //   bsl::vector<bdet_Datetime>& theDatetimeArray(int ri, int ci);
        //   bsl::vector<bdet_Date>& theDateArray(int ri, int ci);
        //   bsl::vector<bdet_Time>& theTimeArray(int ri, int ci);
        //   bdem_List& theList(int ri, int ci);
        //   bdem_Table& theTable(int ri, int ci);
        //   const bdem_Row&.row(int ri) const;
        //   const char& theChar(int ri, int ci) const;
        //   const short& theShort(int ri, int ci) const;
        //   const int& theInt(int ri, int ci) const;
        //   const Int64& theInt64(int ri, int ci) const;
        //   const float& theFloat(int ri, int ci) const;
        //   const double& theDouble(int ri, int ci) const;
        //   const bsl::string& theString(int ri, int ci) const;
        //   const bdet_Datetime& theDatetime(int ri, int ci) const;
        //   const bdet_Date& theDate(int ri, int ci) const;
        //   const bdet_Time& theTime(int ri, int ci) const;
        //   const bsl::vector<char>& theCharArray(int ri, int ci) const;
        //   const bsl::vector<short>& theShortArray(int ri, int ci) const;
        //   const bsl::vector<int>& theIntArray(int ri, int ci) const;
        //   const bsl::vector<Int64>& theInt64Array(int ri, int ci) const;
        //   const bsl::vector<float>& theFloatArray(int ri, int ci) const;
        //   const bsl::vector<double>& theDoubleArray(int ri, int ci) const;
        //   const bsl::vector<bsl::string>&
        //                                theStringArray(int ri, int ci) const;
        //   const bsl::vector<bdet_Datetime>&
        //                              theDatetimeArray(int ri, int ci) const;
        //   const bsl::vector<bdet_Date>& theDateArray(int ri, int ci) const;
        //   const bsl::vector<bdet_Time>& theTimeArray(int ri, int ci) const;
        //   const bdem_List& theList(int ri, int ci) const;
        //   const bdem_Table& theTable(int ri, int ci) const;
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl
                          << "Testing bdem_Table 'the' Methods" << endl
                          << "================================" << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_RWspec;   // spec for read/write object
                const char *d_ROspec;   // spec for read-only object
            } DATA[] = {
                //line  rw-table                      ro-table
                //----  --------                      --------
                { L_,   "A;x;",                       "A;y;"           },
                { L_,   "B;x;",                       "B;y;"           },
                { L_,   "C;x;",                       "C;y;"           },
                { L_,   "D;x;",                       "D;y;"           },
                { L_,   "E;x;",                       "E;y;"           },
                { L_,   "F;x;",                       "F;y;"           },
                { L_,   "G;x;",                       "G;y;"           },
                { L_,   "H;x;",                       "H;y;"           },
                { L_,   "I;x;",                       "I;y;"           },
                { L_,   "J;x;",                       "J;y;"           },
                { L_,   "K;x;",                       "K;y;"           },
                { L_,   "L;x;",                       "L;y;"           },
                { L_,   "M;x;",                       "M;y;"           },
                { L_,   "N;x;",                       "N;y;"           },
                { L_,   "O;x;",                       "O;y;"           },
                { L_,   "P;x;",                       "P;y;"           },
                { L_,   "Q;x;",                       "Q;y;"           },
                { L_,   "R;x;",                       "R;y;"           },
                { L_,   "S;x;",                       "S;y;"           },
                { L_,   "T;x;",                       "T;y;"           },
                { L_,   "U;x;",                       "U;y;"           },
                { L_,   "V;x;",                       "V;y;"           },

                { L_,   "ABC;xyu;",                   "ABC;yux;"       },
                { L_,   "ABC;xyx;",                   "ABC;yux;"       },
                { L_,   "ABC;uyx;",                   "ABC;xyx;"       },
                { L_,   "ABC;xyx;",                   "ABC;xyy;"       },
                { L_,   "D;x;y;u;",                   "D;y;u;x;"       },
                { L_,   "D;x;y;u;",                   "D;y;y;u;"       },
                { L_,   "D;x;y;u;",                   "D;x;y;x;"       },
                { L_,   "D;x;x;x;",                   "D;x;y;x;"       },

                { L_,   "HKUV;xyux;yuxy;uxyu;xyux;",
                                     "HKUV;yuxy;uxyu;xyux;yuxy;"       },
                { L_,   "HKUV;xyux;yuxy;uxyu;xyux;",
                                     "HKUV;yyuy;yuyy;uyyu;yyuy;",      },
                { L_,   "HKUV;xyux;yuxy;uxyu;xyux;",
                                     "HKUV;xxux;xuxx;uxxu;xxux;",      },
                { L_,   "HKUV;xyux;yuxy;uxyu;xyux;",
                                     "HKUV;xyxx;yyxy;xxyy;xyxx;",      },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE    = DATA[ti].d_lineNum;
                const char *RW_SPEC = DATA[ti].d_RWspec;
                const char *RO_SPEC = DATA[ti].d_ROspec;

                const Table W(h(RW_SPEC));       // control for writable
                const Table R(h(RO_SPEC));       // control for read-only
                LOOP_ASSERT(LINE, W != R);

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(LINE); P_(RW_SPEC); P(RO_SPEC);
                    cout << "\t\t"; P(W);
                    cout << "\t\t"; P(R);
                }

                BEGIN_BSLMA_EXCEPTION_TEST {
                  Table x(W, &testAllocator);  Table &X = x;
                  Table z(W, &testAllocator);  Table &Z = z;
                  {
                      const Table y(R, &testAllocator);  const Table &Y = y;
                      LOOP_ASSERT(LINE, X == W);
                      LOOP_ASSERT(LINE, Z == W);
                      LOOP_ASSERT(LINE, Y == R);
                      if (veryVerbose) {
                          cout << "\t\t\tBEFORE: "; P_(X); P_(Z); P(Y);
                      }
                      const int NROWS = X.numRows();
                      const int NCOLS = X.numColumns();
                      for (int ri = 0; ri < NROWS; ++ri) {
                          Row& rX = X.theModifiableRow(ri);
                          Row& rZ = Z.theModifiableRow(ri);
                          const Row& crY = Y[ri];
                          for (int ci = 0; ci < NCOLS; ++ci) {
                              switch (X.columnType(ci)) {
                                case bdem_ElemType::BDEM_CHAR: {
                                  typedef char T;
                                  T& t        = rX[ci].theModifiableChar();
                                  const T& ct = Y[ri][ci].theChar();
                                  t = ct;
                                  rZ.theModifiableChar(ci) = crY.theChar(ci);
                                } break;
                                case bdem_ElemType::BDEM_SHORT: {
                                  typedef short T;
                                  T& t        = rX[ci].theModifiableShort();
                                  const T& ct = Y[ri][ci].theShort();
                                  t = ct;
                                  rZ.theModifiableShort(ci) = crY.theShort(ci);
                                } break;
                                case bdem_ElemType::BDEM_INT: {
                                  typedef int T;
                                  T& t        = rX[ci].theModifiableInt();
                                  const T& ct = Y[ri][ci].theInt();
                                  t = ct;
                                  rZ.theModifiableInt(ci) = crY.theInt(ci);
                                } break;
                                case bdem_ElemType::BDEM_INT64: {
                                  typedef Int64 T;
                                  T& t        = rX[ci].theModifiableInt64();
                                  const T& ct = Y[ri][ci].theInt64();
                                  t = ct;
                                  rZ.theModifiableInt64(ci) = crY.theInt64(ci);
                                } break;
                                case bdem_ElemType::BDEM_FLOAT: {
                                  typedef float T;
                                  T& t        = rX[ci].theModifiableFloat();
                                  const T& ct = Y[ri][ci].theFloat();
                                  t = ct;
                                  rZ.theModifiableFloat(ci) = crY.theFloat(ci);
                                } break;
                                case bdem_ElemType::BDEM_DOUBLE: {
                                  typedef double T;
                                  T& t       = rX[ci].theModifiableDouble();
                                  const T& ct = Y[ri][ci].theDouble();
                                  t = ct;
                                  rZ.theModifiableDouble(ci) =
                                                             crY.theDouble(ci);
                                } break;
                                case bdem_ElemType::BDEM_STRING: {
                                  typedef bsl::string T;
                                  T& t       = rX[ci].theModifiableString();
                                  const T& ct = Y[ri][ci].theString();
                                  t = ct;
                                  rZ.theModifiableString(ci) =
                                                             crY.theString(ci);
                                } break;
                                case bdem_ElemType::BDEM_DATETIME: {
                                  typedef bdet_Datetime T;
                                  T& t     = rX[ci].theModifiableDatetime();
                                  const T& ct = Y[ri][ci].theDatetime();
                                  t = ct;
                                  rZ.theModifiableDatetime(ci) =
                                                           crY.theDatetime(ci);
                                } break;
                                case bdem_ElemType::BDEM_DATE: {
                                  typedef bdet_Date T;
                                  T& t        = rX[ci].theModifiableDate();
                                  const T& ct = Y[ri][ci].theDate();
                                  t = ct;
                                  rZ.theModifiableDate(ci) = crY.theDate(ci);
                                } break;
                                case bdem_ElemType::BDEM_TIME: {
                                  typedef bdet_Time T;
                                  T& t        = rX[ci].theModifiableTime();
                                  const T& ct = Y[ri][ci].theTime();
                                  t = ct;
                                  rZ.theModifiableTime(ci) = crY.theTime(ci);
                                } break;
                                case bdem_ElemType::BDEM_CHAR_ARRAY: {
                                  typedef bsl::vector<char> T;
                                  T& t    = rX[ci].theModifiableCharArray();
                                  const T& ct = Y[ri][ci].theCharArray();
                                  t = ct;
                                  rZ.theModifiableCharArray(ci) =
                                                          crY.theCharArray(ci);
                                } break;
                                case bdem_ElemType::BDEM_SHORT_ARRAY: {
                                  typedef bsl::vector<short> T;
                                  T& t   = rX[ci].theModifiableShortArray();
                                  const T& ct = Y[ri][ci].theShortArray();
                                  t = ct;
                                  rZ.theModifiableShortArray(ci) =
                                                         crY.theShortArray(ci);
                                } break;
                                case bdem_ElemType::BDEM_INT_ARRAY: {
                                  typedef bsl::vector<int> T;
                                  T& t     = rX[ci].theModifiableIntArray();
                                  const T& ct = Y[ri][ci].theIntArray();
                                  t = ct;
                                  rZ.theModifiableIntArray(ci) =
                                                           crY.theIntArray(ci);
                                } break;
                                case bdem_ElemType::BDEM_INT64_ARRAY: {
                                  typedef bsl::vector<Int64> T;
                                  T& t   = rX[ci].theModifiableInt64Array();
                                  const T& ct = Y[ri][ci].theInt64Array();
                                  t = ct;
                                  rZ.theModifiableInt64Array(ci) =
                                                         crY.theInt64Array(ci);
                                } break;
                                case bdem_ElemType::BDEM_FLOAT_ARRAY: {
                                  typedef bsl::vector<float> T;
                                  T& t   = rX[ci].theModifiableFloatArray();
                                  const T& ct = Y[ri][ci].theFloatArray();
                                  t = ct;
                                  rZ.theModifiableFloatArray(ci) =
                                                         crY.theFloatArray(ci);
                                } break;
                                case bdem_ElemType::BDEM_DOUBLE_ARRAY: {
                                  typedef bsl::vector<double> T;
                                  T& t  = rX[ci].theModifiableDoubleArray();
                                  const T& ct = Y[ri][ci].theDoubleArray();
                                  t = ct;
                                  rZ.theModifiableDoubleArray(ci) =
                                                        crY.theDoubleArray(ci);
                                } break;
                                case bdem_ElemType::BDEM_STRING_ARRAY: {
                                  typedef bsl::vector<bsl::string> T;
                                  T& t  = rX[ci].theModifiableStringArray();
                                  const T& ct = Y[ri][ci].theStringArray();
                                  t = ct;
                                  rZ.theModifiableStringArray(ci) =
                                                        crY.theStringArray(ci);
                                } break;
                                case bdem_ElemType::BDEM_DATETIME_ARRAY: {
                                  typedef bsl::vector<bdet_Datetime> T;
                                  T& t= rX[ci].theModifiableDatetimeArray();
                                  const T& ct = Y[ri][ci].theDatetimeArray();
                                  t = ct;
                                  rZ.theModifiableDatetimeArray(ci) =
                                                      crY.theDatetimeArray(ci);
                                } break;
                                case bdem_ElemType::BDEM_DATE_ARRAY: {
                                  typedef bsl::vector<bdet_Date> T;
                                  T& t    = rX[ci].theModifiableDateArray();
                                  const T& ct = Y[ri][ci].theDateArray();
                                  t = ct;
                                  rZ.theModifiableDateArray(ci) =
                                                          crY.theDateArray(ci);
                                } break;
                                case bdem_ElemType::BDEM_TIME_ARRAY: {
                                  typedef bsl::vector<bdet_Time> T;
                                  T& t    = rX[ci].theModifiableTimeArray();
                                  const T& ct = Y[ri][ci].theTimeArray();
                                  t = ct;
                                  rZ.theModifiableTimeArray(ci) =
                                                          crY.theTimeArray(ci);
                                } break;
                                case bdem_ElemType::BDEM_LIST: {
                                  typedef bdem_List T;
                                  T& t        = rX[ci].theModifiableList();
                                  const T& ct = Y[ri][ci].theList();
                                  t = ct;
                                  rZ.theModifiableList(ci) = crY.theList(ci);
                                } break;
                                case bdem_ElemType::BDEM_TABLE: {
                                  typedef bdem_Table T;
                                  T& t        = rX[ci].theModifiableTable();
                                  const T& ct = Y[ri][ci].theTable();
                                  t = ct;
                                  rZ.theModifiableTable(ci) = crY.theTable(ci);
                                } break;
                                default: {
                                  LOOP_ASSERT(LINE, 0 == 1);
                                } break;
                              }
                          }
                      }
                      if (veryVerbose) {
                          cout << "\t\t\tAFTER: "; P_(X); P_(Z); P(Y);
                      }
                      LOOP_ASSERT(LINE, X != W);
                      LOOP_ASSERT(LINE, Z != W);
                      LOOP_ASSERT(LINE, X == R);
                      LOOP_ASSERT(LINE, Z == R);
                      LOOP_ASSERT(LINE, Y == R);  // readonly unchanged?
                  }
                  LOOP_ASSERT(LINE, X == R);      // readonly is out of scope
                  LOOP_ASSERT(LINE, Z == R);      // readonly is out of scope
                } END_BSLMA_EXCEPTION_TEST
            }
        }
      }

DEFINE_TEST_CASE(36) {
        // --------------------------------------------------------------------
        // TESTING BDEM_LIST'S 'THE' MANIPULATOR AND ACCESSOR METHODS
        //
        // Plan:
        //   Since the 'bdem_List' and 'bdem_Table' methods forward to
        //   the 'bdem_Row' methods, it is sufficient to verify that the
        //   'bdem_Row' methods operate for a single test value and then
        //   thoroughly test the 'bdem_List' and 'bdem_Table' methods.
        //   Values from all of the 22 element types are used in the tests.
        //
        //   Apart from thoroughness, the strategy for testing the 'the'
        //   methods of 'bdem_Row', 'bdem_List' and 'bdem_Table' is similar.
        //   For each of the 22 types:
        //     * Construct non-const and const objects.
        //     * Access the elements of each of the objects using the 'the'
        //       methods.
        //     * Verify that non-const objects can indeed be modified
        //       through references returned by non-const 'the' methods.
        //     * Verify that the const and non-const 'the' methods
        //       have the proper signatures.  (This latter test is done
        //       independent of any object.)
        //
        // Testing:
        //   'bdem_List' public interface
        //
        //   bdem_Row&.row();
        //   char& theChar(int index);
        //   short& theShort(int index);
        //   int& theInt(int index);
        //   Int64& theInt64(int index);
        //   float& theFloat(int index);
        //   double& theDouble(int index);
        //   bsl::string& theString(int index);
        //   bdet_Datetime& theDatetime(int index);
        //   bdet_Date& theDate(int index);
        //   bdet_Time& theTime(int index);
        //   bsl::vector<char>& theCharArray(int index);
        //   bsl::vector<short>& theShortArray(int index);
        //   bsl::vector<int>& theIntArray(int index);
        //   bsl::vector<Int64>& theInt64Array(int index);
        //   bsl::vector<float>& theFloatArray(int index);
        //   bsl::vector<double>& theDoubleArray(int index);
        //   bsl::vector<bsl::string>& theStringArray(int index);
        //   bsl::vector<bdet_Datetime>& theDatetimeArray(int index);
        //   bsl::vector<bdet_Date>& theDateArray(int index);
        //   bsl::vector<bdet_Time>& theTimeArray(int index);
        //   bdem_List& theList(int index);
        //   bdem_Table& theTable(int index);
        //   const bdem_Row&.row() const;
        //   const char& theChar(int index) const;
        //   const short& theShort(int index) const;
        //   const int& theInt(int index) const;
        //   const Int64& theInt64(int index) const;
        //   const float& theFloat(int index) const;
        //   const double& theDouble(int index) const;
        //   const bsl::string& theString(int index) const;
        //   const bdet_Datetime& theDatetime(int index) const;
        //   const bdet_Date& theDate(int index) const;
        //   const bdet_Time& theTime(int index) const;
        //   const bsl::vector<char>& theCharArray(int index) const;
        //   const bsl::vector<short>& theShortArray(int index) const;
        //   const bsl::vector<int>& theIntArray(int index) const;
        //   const bsl::vector<Int64>& theInt64Array(int index) const;
        //   const bsl::vector<float>& theFloatArray(int index) const;
        //   const bsl::vector<double>& theDoubleArray(int index) const;
        //   const bsl::vector<bsl::string>& theStringArray(int index) const;
        //   const bsl::vector<bdet_Datetime>&
        //                                   theDatetimeArray(int index) const;
        //   const bsl::vector<bdet_Date>& theDateArray(int index) const;
        //   const bsl::vector<bdet_Time>& theTimeArray(int index) const;
        //   const bdem_List& theList(int index) const;
        //   const bdem_Table& theTable(int index) const;
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl
                          << "Testing bdem_List 'the' Methods" << endl
                          << "===============================" << endl;

        if (verbose) cout << endl << "\tVerify method signatures." << endl;
        {
            {
                Row&       (List::*f)()        = &List::row;
                const Row& (List::*cf)() const = &List::row;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef char T;
                T&       (List::*f)(int)        = &List::theModifiableChar;
                const T& (List::*cf)(int) const = &List::theChar;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef short T;
                T&       (List::*f)(int)        = &List::theModifiableShort;
                const T& (List::*cf)(int) const = &List::theShort;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef int T;
                T&       (List::*f)(int)        = &List::theModifiableInt;
                const T& (List::*cf)(int) const = &List::theInt;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef Int64 T;
                T&       (List::*f)(int)        = &List::theModifiableInt64;
                const T& (List::*cf)(int) const = &List::theInt64;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef float T;
                T&       (List::*f)(int)        = &List::theModifiableFloat;
                const T& (List::*cf)(int) const = &List::theFloat;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef double T;
                T&       (List::*f)(int)        = &List::theModifiableDouble;
                const T& (List::*cf)(int) const = &List::theDouble;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bsl::string T;
                T&       (List::*f)(int)        = &List::theModifiableString;
                const T& (List::*cf)(int) const = &List::theString;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bdet_Datetime T;
                T&       (List::*f)(int)        = &List::theModifiableDatetime;
                const T& (List::*cf)(int) const = &List::theDatetime;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bdet_Date T;
                T&       (List::*f)(int)        = &List::theModifiableDate;
                const T& (List::*cf)(int) const = &List::theDate;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bdet_Time T;
                T&       (List::*f)(int)        = &List::theModifiableTime;
                const T& (List::*cf)(int) const = &List::theTime;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bsl::vector<char> T;
                T&       (List::*f)(int)       = &List::theModifiableCharArray;
                const T& (List::*cf)(int) const = &List::theCharArray;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bsl::vector<short> T;
                T&       (List::*f)(int)      = &List::theModifiableShortArray;
                const T& (List::*cf)(int) const = &List::theShortArray;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bsl::vector<int> T;
                T&       (List::*f)(int)        = &List::theModifiableIntArray;
                const T& (List::*cf)(int) const = &List::theIntArray;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bsl::vector<Int64> T;
                T&       (List::*f)(int)      = &List::theModifiableInt64Array;
                const T& (List::*cf)(int) const = &List::theInt64Array;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bsl::vector<float> T;
                T&       (List::*f)(int)      = &List::theModifiableFloatArray;
                const T& (List::*cf)(int) const = &List::theFloatArray;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bsl::vector<double> T;
                T&       (List::*f)(int)     = &List::theModifiableDoubleArray;
                const T& (List::*cf)(int) const = &List::theDoubleArray;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bsl::vector<bsl::string> T;
                T&       (List::*f)(int)     = &List::theModifiableStringArray;
                const T& (List::*cf)(int) const = &List::theStringArray;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bsl::vector<bdet_Datetime> T;
                T&       (List::*f)(int)   = &List::theModifiableDatetimeArray;
                const T& (List::*cf)(int) const = &List::theDatetimeArray;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bsl::vector<bdet_Date> T;
                T&       (List::*f)(int)       = &List::theModifiableDateArray;
                const T& (List::*cf)(int) const = &List::theDateArray;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bsl::vector<bdet_Time> T;
                T&       (List::*f)(int)       = &List::theModifiableTimeArray;
                const T& (List::*cf)(int) const = &List::theTimeArray;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bdem_List T;
                T&       (List::*f)(int)        = &List::theModifiableList;
                const T& (List::*cf)(int) const = &List::theList;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bdem_Table T;
                T&       (List::*f)(int)        = &List::theModifiableTable;
                const T& (List::*cf)(int) const = &List::theTable;
                ASSERT(f && cf);  // use c, cf
            }
        }
        {
            static const struct {
                int         d_lineNum;    // source line number
                const char *d_RWspec;     // spec for read/write object
                const char *d_ROspec;     // spec for read-only object
            } DATA[] = {
                //line  rw-list                  ro-list
                //----  -------                  -------
                { L_,   "xA",                    "yA"                      },
                { L_,   "xB",                    "yB"                      },
                { L_,   "xC",                    "yC"                      },
                { L_,   "xD",                    "yD"                      },
                { L_,   "xE",                    "yE"                      },
                { L_,   "xF",                    "yF"                      },
                { L_,   "xG",                    "yG"                      },
                { L_,   "xH",                    "yH"                      },
                { L_,   "xI",                    "yI"                      },
                { L_,   "xJ",                    "yJ"                      },
                { L_,   "xK",                    "yK"                      },
                { L_,   "xL",                    "yL"                      },
                { L_,   "xM",                    "yM"                      },
                { L_,   "xN",                    "yN"                      },
                { L_,   "xO",                    "yO"                      },
                { L_,   "xP",                    "yP"                      },
                { L_,   "xQ",                    "yQ"                      },
                { L_,   "xR",                    "yR"                      },
                { L_,   "xS",                    "yS"                      },
                { L_,   "xT",                    "yT"                      },
                { L_,   "xU",                    "yU"                      },
                { L_,   "xV",                    "yV"                      },

                { L_,   "xK",                    "uK"                      },
                { L_,   "uK",                    "xK"                      },
                { L_,   "xAxA",                  "yAyA"                    },
                { L_,   "xAxB",                  "yAyB"                    },
                { L_,   "xAxC",                  "yAyC"                    },
                { L_,   "xAxD",                  "yAyD"                    },
                { L_,   "xAxE",                  "yAyE"                    },
                { L_,   "xAxF",                  "yAyF"                    },
                { L_,   "xAxG",                  "yAyG"                    },
                { L_,   "xAxH",                  "yAyH"                    },
                { L_,   "xAxI",                  "yAyI"                    },
                { L_,   "xAxJ",                  "yAyJ"                    },
                { L_,   "xAxK",                  "yAyK"                    },
                { L_,   "xAxL",                  "yAyL"                    },
                { L_,   "xAxM",                  "yAyM"                    },
                { L_,   "xAxN",                  "yAyN"                    },
                { L_,   "xAxO",                  "yAyO"                    },
                { L_,   "xAxP",                  "yAyP"                    },
                { L_,   "xAxQ",                  "yAyQ"                    },
                { L_,   "xAxR",                  "yAyR"                    },
                { L_,   "xAxS",                  "yAyS"                    },
                { L_,   "xAxT",                  "yAyT"                    },
                { L_,   "xAxU",                  "yAyU"                    },
                { L_,   "xAxV",                  "yAyV"                    },
                { L_,   "uAuAuA",                "uAxAyA"                  },
                { L_,   "uDuEuF",                "uDxEyF"                  },
                { L_,   "uDuEuF",                "xDyEuF"                  },
                { L_,   "xDuExF",                "xDyExF"                  },
                { L_,   "xDyExFyG",              "xDyEuFyG",               },
                { L_,   "xDyExFyG",              "uDyExFuG",               },
                { L_,   "xDxExFxG",              "xDyEuFxG",               },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE    = DATA[ti].d_lineNum;
                const char *RW_SPEC = DATA[ti].d_RWspec;
                const char *RO_SPEC = DATA[ti].d_ROspec;

                const List W(g(RW_SPEC));       // control for writable
                const List R(g(RO_SPEC));       // control for read-only
                LOOP_ASSERT(LINE, W != R);

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(LINE); P_(RW_SPEC); P(RO_SPEC);
                    cout << "\t\t"; P(W);
                    cout << "\t\t"; P(R);
                }

                BEGIN_BSLMA_EXCEPTION_TEST {
                  List x(W, &testAllocator);  List &X = x;
                  List z(W, &testAllocator);  List &Z = z;
                  Row& rZ = Z.row();
                  {
                      const List y(R, &testAllocator);  const List &Y = y;
                      const Row& crY = Y.row();
                      LOOP_ASSERT(LINE, X == W);
                      LOOP_ASSERT(LINE, Z == W);
                      LOOP_ASSERT(LINE, Y == R);
                      if (veryVerbose) {
                          cout << "\t\t\tBEFORE: "; P_(X); P_(Z); P(Y);
                      }
                      const int LEN = X.length();
                      for (int ei = 0; ei < LEN; ++ei) {
                          switch (X.elemType(ei)) {
                            case bdem_ElemType::BDEM_CHAR: {
                              typedef char T;
                              T& t        = X.theModifiableChar(ei);
                              const T& ct = Y.theChar(ei);
                              t = ct;
                              rZ.theModifiableChar(ei) = crY.theChar(ei);
                            } break;
                            case bdem_ElemType::BDEM_SHORT: {
                              typedef short T;
                              T& t        = X.theModifiableShort(ei);
                              const T& ct = Y.theShort(ei);
                              t = ct;
                              rZ.theModifiableShort(ei) = crY.theShort(ei);
                            } break;
                            case bdem_ElemType::BDEM_INT: {
                              typedef int T;
                              T& t        = X.theModifiableInt(ei);
                              const T& ct = Y.theInt(ei);
                              t = ct;
                              rZ.theModifiableInt(ei) = crY.theInt(ei);
                            } break;
                            case bdem_ElemType::BDEM_INT64: {
                              typedef Int64 T;
                              T& t        = X.theModifiableInt64(ei);
                              const T& ct = Y.theInt64(ei);
                              t = ct;
                              rZ.theModifiableInt64(ei) = crY.theInt64(ei);
                            } break;
                            case bdem_ElemType::BDEM_FLOAT: {
                              typedef float T;
                              T& t        = X.theModifiableFloat(ei);
                              const T& ct = Y.theFloat(ei);
                              t = ct;
                              rZ.theModifiableFloat(ei) = crY.theFloat(ei);
                            } break;
                            case bdem_ElemType::BDEM_DOUBLE: {
                              typedef double T;
                              T& t        = X.theModifiableDouble(ei);
                              const T& ct = Y.theDouble(ei);
                              t = ct;
                              rZ.theModifiableDouble(ei) = crY.theDouble(ei);
                            } break;
                            case bdem_ElemType::BDEM_STRING: {
                              typedef bsl::string T;
                              T& t        = X.theModifiableString(ei);
                              const T& ct = Y.theString(ei);
                              t = ct;
                              rZ.theModifiableString(ei) = crY.theString(ei);
                            } break;
                            case bdem_ElemType::BDEM_DATETIME: {
                              typedef bdet_Datetime T;
                              T& t        = X.theModifiableDatetime(ei);
                              const T& ct = Y.theDatetime(ei);
                              t = ct;
                              rZ.theModifiableDatetime(ei) =
                                                           crY.theDatetime(ei);
                            } break;
                            case bdem_ElemType::BDEM_DATE: {
                              typedef bdet_Date T;
                              T& t        = X.theModifiableDate(ei);
                              const T& ct = Y.theDate(ei);
                              t = ct;
                              rZ.theModifiableDate(ei) = crY.theDate(ei);
                            } break;
                            case bdem_ElemType::BDEM_TIME: {
                              typedef bdet_Time T;
                              T& t        = X.theModifiableTime(ei);
                              const T& ct = Y.theTime(ei);
                              t = ct;
                              rZ.theModifiableTime(ei) = crY.theTime(ei);
                            } break;
                            case bdem_ElemType::BDEM_CHAR_ARRAY: {
                              typedef bsl::vector<char> T;
                              T& t        = X.theModifiableCharArray(ei);
                              const T& ct = Y.theCharArray(ei);
                              t = ct;
                              rZ.theModifiableCharArray(ei) =
                                                          crY.theCharArray(ei);
                            } break;
                            case bdem_ElemType::BDEM_SHORT_ARRAY: {
                              typedef bsl::vector<short> T;
                              T& t        = X.theModifiableShortArray(ei);
                              const T& ct = Y.theShortArray(ei);
                              t = ct;
                              rZ.theModifiableShortArray(ei) =
                                                         crY.theShortArray(ei);
                            } break;
                            case bdem_ElemType::BDEM_INT_ARRAY: {
                              typedef bsl::vector<int> T;
                              T& t        = X.theModifiableIntArray(ei);
                              const T& ct = Y.theIntArray(ei);
                              t = ct;
                              rZ.theModifiableIntArray(ei) =
                                                           crY.theIntArray(ei);
                            } break;
                            case bdem_ElemType::BDEM_INT64_ARRAY: {
                              typedef bsl::vector<Int64> T;
                              T& t        = X.theModifiableInt64Array(ei);
                              const T& ct = Y.theInt64Array(ei);
                              t = ct;
                              rZ.theModifiableInt64Array(ei) =
                                                         crY.theInt64Array(ei);
                            } break;
                            case bdem_ElemType::BDEM_FLOAT_ARRAY: {
                              typedef bsl::vector<float> T;
                              T& t        = X.theModifiableFloatArray(ei);
                              const T& ct = Y.theFloatArray(ei);
                              t = ct;
                              rZ.theModifiableFloatArray(ei) =
                                                         crY.theFloatArray(ei);
                            } break;
                            case bdem_ElemType::BDEM_DOUBLE_ARRAY: {
                              typedef bsl::vector<double> T;
                              T& t        = X.theModifiableDoubleArray(ei);
                              const T& ct = Y.theDoubleArray(ei);
                              t = ct;
                              rZ.theModifiableDoubleArray(ei) =
                                                        crY.theDoubleArray(ei);
                            } break;
                            case bdem_ElemType::BDEM_STRING_ARRAY: {
                              typedef bsl::vector<bsl::string> T;
                              T& t        = X.theModifiableStringArray(ei);
                              const T& ct = Y.theStringArray(ei);
                              t = ct;
                              rZ.theModifiableStringArray(ei) =
                                                        crY.theStringArray(ei);
                            } break;
                            case bdem_ElemType::BDEM_DATETIME_ARRAY: {
                              typedef bsl::vector<bdet_Datetime> T;
                              T& t        = X.theModifiableDatetimeArray(ei);
                              const T& ct = Y.theDatetimeArray(ei);
                              t = ct;
                              rZ.theModifiableDatetimeArray(ei) =
                                                      crY.theDatetimeArray(ei);
                            } break;
                            case bdem_ElemType::BDEM_DATE_ARRAY: {
                              typedef bsl::vector<bdet_Date> T;
                              T& t        = X.theModifiableDateArray(ei);
                              const T& ct = Y.theDateArray(ei);
                              t = ct;
                              rZ.theModifiableDateArray(ei) =
                                                          crY.theDateArray(ei);
                            } break;
                            case bdem_ElemType::BDEM_TIME_ARRAY: {
                              typedef bsl::vector<bdet_Time> T;
                              T& t        = X.theModifiableTimeArray(ei);
                              const T& ct = Y.theTimeArray(ei);
                              t = ct;
                              rZ.theModifiableTimeArray(ei) =
                                                          crY.theTimeArray(ei);
                            } break;
                            case bdem_ElemType::BDEM_LIST: {
                              typedef bdem_List T;
                              T& t        = X.theModifiableList(ei);
                              const T& ct = Y.theList(ei);
                              t = ct;
                              rZ.theModifiableList(ei) = crY.theList(ei);
                            } break;
                            case bdem_ElemType::BDEM_TABLE: {
                              typedef bdem_Table T;
                              T& t        = X.theModifiableTable(ei);
                              const T& ct = Y.theTable(ei);
                              t = ct;
                              rZ.theModifiableTable(ei) = crY.theTable(ei);
                            } break;
                            default: {
                              LOOP_ASSERT(LINE, 0 == 1);
                            } break;
                          }
                      }
                      if (veryVerbose) {
                          cout << "\t\t\tAFTER: "; P_(X); P_(Z); P(Y);
                      }
                      LOOP_ASSERT(LINE, X != W);
                      LOOP_ASSERT(LINE, Z != W);
                      LOOP_ASSERT(LINE, X == R);
                      LOOP_ASSERT(LINE, Z == R);
                      LOOP_ASSERT(LINE, Y == R);  // readonly unchanged?
                  }
                  LOOP_ASSERT(LINE, X == R);      // readonly is out of scope
                  LOOP_ASSERT(LINE, Z == R);      // readonly is out of scope
                } END_BSLMA_EXCEPTION_TEST
            }
        }
      }

DEFINE_TEST_CASE(35) {
        // --------------------------------------------------------------------
        // TESTING BDEM_ROW'S 'THE' MANIPULATOR AND ACCESSOR METHODS
        //
        // Plan:
        //   Since the 'bdem_List' and 'bdem_Table' methods forward to
        //   the 'bdem_Row' methods, it is sufficient to verify that the
        //   'bdem_Row' methods operate for a single test value and then
        //   thoroughly test the 'bdem_List' and 'bdem_Table' methods.
        //   Values from all of the 22 element types are used in the tests.
        //
        //   Apart from thoroughness, the strategy for testing the 'the'
        //   methods of 'bdem_Row', 'bdem_List' and 'bdem_Table' is similar.
        //   For each of the 22 types:
        //     * Construct non-const and const objects.
        //     * Access the elements of each of the objects using the 'the'
        //       methods.
        //     * Verify that non-const objects can indeed be modified
        //       through references returned by non-const 'the' methods.
        //     * Verify that the const and non-const 'the' methods
        //       have the proper signatures.  (This latter test is done
        //       independent of any object.)
        //
        // Testing:
        //   'bdem_Row' public interface
        //
        //   char& theChar(int index);
        //   short& theShort(int index);
        //   int& theInt(int index);
        //   Int64& theInt64(int index);
        //   float& theFloat(int index);
        //   double& theDouble(int index);
        //   bsl::string& theString(int index);
        //   bdet_Datetime& theDatetime(int index);
        //   bdet_Date& theDate(int index);
        //   bdet_Time& theTime(int index);
        //   bsl::vector<char>& theCharArray(int index);
        //   bsl::vector<short>& theShortArray(int index);
        //   bsl::vector<int>& theIntArray(int index);
        //   bsl::vector<Int64>& theInt64Array(int index);
        //   bsl::vector<float>& theFloatArray(int index);
        //   bsl::vector<double>& theDoubleArray(int index);
        //   bsl::vector<bsl::string>& theStringArray(int index);
        //   bsl::vector<bdet_Datetime>& theDatetimeArray(int index);
        //   bsl::vector<bdet_Date>& theDateArray(int index);
        //   bsl::vector<bdet_Time>& theTimeArray(int index);
        //   bdem_List& theList(int index);
        //   bdem_Table& theTable(int index);
        //   const char& theChar(int index) const;
        //   const short& theShort(int index) const;
        //   const int& theInt(int index) const;
        //   const Int64& theInt64(int index) const;
        //   const float& theFloat(int index) const;
        //   const double& theDouble(int index) const;
        //   const bsl::string& theString(int index) const;
        //   const bdet_Datetime& theDatetime(int index) const;
        //   const bdet_Date& theDate(int index) const;
        //   const bdet_Time& theTime(int index) const;
        //   const bsl::vector<char>& theCharArray(int index) const;
        //   const bsl::vector<short>& theShortArray(int index) const;
        //   const bsl::vector<int>& theIntArray(int index) const;
        //   const bsl::vector<Int64>& theInt64Array(int index) const;
        //   const bsl::vector<float>& theFloatArray(int index) const;
        //   const bsl::vector<double>& theDoubleArray(int index) const;
        //   const bsl::vector<bsl::string>& theStringArray(int index) const;
        //   const bsl::vector<bdet_Datetime>&
        //                                   theDatetimeArray(int index) const;
        //   const bsl::vector<bdet_Date>& theDateArray(int index) const;
        //   const bsl::vector<bdet_Time>& theTimeArray(int index) const;
        //   const bdem_List& theList(int index) const;
        //   const bdem_Table& theTable(int index) const;
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl
                          << "Testing bdem_Row 'the' Methods" << endl
                          << "==============================" << endl;

        if (verbose) cout << endl << "\tVerify method signatures." << endl;
        {
            {
                typedef char T;
                T&       (Row::*f)(int)        = &Row::theModifiableChar;
                const T& (Row::*cf)(int) const = &Row::theChar;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef short T;
                T&       (Row::*f)(int)        = &Row::theModifiableShort;
                const T& (Row::*cf)(int) const = &Row::theShort;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef int T;
                T&       (Row::*f)(int)        = &Row::theModifiableInt;
                const T& (Row::*cf)(int) const = &Row::theInt;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef Int64 T;
                T&       (Row::*f)(int)        = &Row::theModifiableInt64;
                const T& (Row::*cf)(int) const = &Row::theInt64;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef float T;
                T&       (Row::*f)(int)        = &Row::theModifiableFloat;
                const T& (Row::*cf)(int) const = &Row::theFloat;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef double T;
                T&       (Row::*f)(int)        = &Row::theModifiableDouble;
                const T& (Row::*cf)(int) const = &Row::theDouble;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bsl::string T;
                T&       (Row::*f)(int)        = &Row::theModifiableString;
                const T& (Row::*cf)(int) const = &Row::theString;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bdet_Datetime T;
                T&       (Row::*f)(int)        = &Row::theModifiableDatetime;
                const T& (Row::*cf)(int) const = &Row::theDatetime;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bdet_Date T;
                T&       (Row::*f)(int)        = &Row::theModifiableDate;
                const T& (Row::*cf)(int) const = &Row::theDate;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bdet_Time T;
                T&       (Row::*f)(int)        = &Row::theModifiableTime;
                const T& (Row::*cf)(int) const = &Row::theTime;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bsl::vector<char> T;
                T&       (Row::*f)(int)        = &Row::theModifiableCharArray;
                const T& (Row::*cf)(int) const = &Row::theCharArray;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bsl::vector<short> T;
                T&       (Row::*f)(int)        = &Row::theModifiableShortArray;
                const T& (Row::*cf)(int) const = &Row::theShortArray;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bsl::vector<int> T;
                T&       (Row::*f)(int)        = &Row::theModifiableIntArray;
                const T& (Row::*cf)(int) const = &Row::theIntArray;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bsl::vector<Int64> T;
                T&       (Row::*f)(int)        = &Row::theModifiableInt64Array;
                const T& (Row::*cf)(int) const = &Row::theInt64Array;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bsl::vector<float> T;
                T&       (Row::*f)(int)        = &Row::theModifiableFloatArray;
                const T& (Row::*cf)(int) const = &Row::theFloatArray;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bsl::vector<double> T;
                T&       (Row::*f)(int)       = &Row::theModifiableDoubleArray;
                const T& (Row::*cf)(int) const = &Row::theDoubleArray;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bsl::vector<bsl::string> T;
                T&       (Row::*f)(int)       = &Row::theModifiableStringArray;
                const T& (Row::*cf)(int) const = &Row::theStringArray;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bsl::vector<bdet_Datetime> T;
                T&       (Row::*f)(int)     = &Row::theModifiableDatetimeArray;
                const T& (Row::*cf)(int) const = &Row::theDatetimeArray;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bsl::vector<bdet_Date> T;
                T&       (Row::*f)(int)        = &Row::theModifiableDateArray;
                const T& (Row::*cf)(int) const = &Row::theDateArray;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bsl::vector<bdet_Time> T;
                T&       (Row::*f)(int)        = &Row::theModifiableTimeArray;
                const T& (Row::*cf)(int) const = &Row::theTimeArray;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bdem_List T;
                T&       (Row::*f)(int)        = &Row::theModifiableList;
                const T& (Row::*cf)(int) const = &Row::theList;
                ASSERT(f && cf);  // use c, cf
            }
            {
                typedef bdem_Table T;
                T&       (Row::*f)(int)        = &Row::theModifiableTable;
                const T& (Row::*cf)(int) const = &Row::theTable;
                ASSERT(f && cf);  // use c, cf
            }
        }
        {
            static const struct {
                int         d_lineNum;    // source line number
                const char *d_RWspec;     // spec for read/write object
                const char *d_ROspec;     // spec for read-only object
            } DATA[] = {
                //line  rw-list                  ro-list
                //----  -------                  -------
                { L_,   "xA",                    "yA"                      },
                { L_,   "xB",                    "yB"                      },
                { L_,   "xC",                    "yC"                      },
                { L_,   "xD",                    "yD"                      },
                { L_,   "xE",                    "yE"                      },
                { L_,   "xF",                    "yF"                      },
                { L_,   "xG",                    "yG"                      },
                { L_,   "xH",                    "yH"                      },
                { L_,   "xI",                    "yI"                      },
                { L_,   "xJ",                    "yJ"                      },
                { L_,   "xK",                    "yK"                      },
                { L_,   "xL",                    "yL"                      },
                { L_,   "xM",                    "yM"                      },
                { L_,   "xN",                    "yN"                      },
                { L_,   "xO",                    "yO"                      },
                { L_,   "xP",                    "yP"                      },
                { L_,   "xQ",                    "yQ"                      },
                { L_,   "xR",                    "yR"                      },
                { L_,   "xS",                    "yS"                      },
                { L_,   "xT",                    "yT"                      },
                { L_,   "xU",                    "yU"                      },
                { L_,   "xV",                    "yV"                      },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE    = DATA[ti].d_lineNum;
                const char *RW_SPEC = DATA[ti].d_RWspec;
                const char *RO_SPEC = DATA[ti].d_ROspec;

                const List W(g(RW_SPEC));       // control for writable
                const List R(g(RO_SPEC));       // control for read-only
                LOOP_ASSERT(LINE, W != R);

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(LINE); P_(RW_SPEC); P(RO_SPEC);
                    cout << "\t\t"; P(W);
                    cout << "\t\t"; P(R);
                }

                BEGIN_BSLMA_EXCEPTION_TEST {
                  List x(W, &testAllocator);  List &X = x;
                  Row& RX = X.row();
                  {
                      const List y(R, &testAllocator);  const List &Y = y;
                      const Row& RY = Y.row();
                      LOOP_ASSERT(LINE, X  == W);
                      LOOP_ASSERT(LINE, Y  == R);
                      LOOP_ASSERT(LINE, RX != RY);
                      if (veryVerbose) {
                          cout << "\t\t\tBEFORE: "; P_(X); P(Y);
                      }
                      switch (RY.elemType(0)) {
                        case bdem_ElemType::BDEM_CHAR: {
                          typedef char T;
                          T& t        = RX.theModifiableChar(0);
                          const T& ct = RY.theChar(0);
                          t = ct;
                        } break;
                        case bdem_ElemType::BDEM_SHORT: {
                          typedef short T;
                          T& t        = RX.theModifiableShort(0);
                          const T& ct = RY.theShort(0);
                          t = ct;
                        } break;
                        case bdem_ElemType::BDEM_INT: {
                          typedef int T;
                          T& t        = RX.theModifiableInt(0);
                          const T& ct = RY.theInt(0);
                          t = ct;
                        } break;
                        case bdem_ElemType::BDEM_INT64: {
                          typedef Int64 T;
                          T& t        = RX.theModifiableInt64(0);
                          const T& ct = RY.theInt64(0);
                          t = ct;
                        } break;
                        case bdem_ElemType::BDEM_FLOAT: {
                          typedef float T;
                          T& t        = RX.theModifiableFloat(0);
                          const T& ct = RY.theFloat(0);
                          t = ct;
                        } break;
                        case bdem_ElemType::BDEM_DOUBLE: {
                          typedef double T;
                          T& t        = RX.theModifiableDouble(0);
                          const T& ct = RY.theDouble(0);
                          t = ct;
                        } break;
                        case bdem_ElemType::BDEM_STRING: {
                          typedef bsl::string T;
                          T& t        = RX.theModifiableString(0);
                          const T& ct = RY.theString(0);
                          t = ct;
                        } break;
                        case bdem_ElemType::BDEM_DATETIME: {
                          typedef bdet_Datetime T;
                          T& t        = RX.theModifiableDatetime(0);
                          const T& ct = RY.theDatetime(0);
                          t = ct;
                        } break;
                        case bdem_ElemType::BDEM_DATE: {
                          typedef bdet_Date T;
                          T& t        = RX.theModifiableDate(0);
                          const T& ct = RY.theDate(0);
                          t = ct;
                        } break;
                        case bdem_ElemType::BDEM_TIME: {
                          typedef bdet_Time T;
                          T& t        = RX.theModifiableTime(0);
                          const T& ct = RY.theTime(0);
                          t = ct;
                        } break;
                        case bdem_ElemType::BDEM_CHAR_ARRAY: {
                          typedef bsl::vector<char> T;
                          T& t        = RX.theModifiableCharArray(0);
                          const T& ct = RY.theCharArray(0);
                          t = ct;
                        } break;
                        case bdem_ElemType::BDEM_SHORT_ARRAY: {
                          typedef bsl::vector<short> T;
                          T& t        = RX.theModifiableShortArray(0);
                          const T& ct = RY.theShortArray(0);
                          t = ct;
                        } break;
                        case bdem_ElemType::BDEM_INT_ARRAY: {
                          typedef bsl::vector<int> T;
                          T& t        = RX.theModifiableIntArray(0);
                          const T& ct = RY.theIntArray(0);
                          t = ct;
                        } break;
                        case bdem_ElemType::BDEM_INT64_ARRAY: {
                          typedef bsl::vector<Int64> T;
                          T& t        = RX.theModifiableInt64Array(0);
                          const T& ct = RY.theInt64Array(0);
                          t = ct;
                        } break;
                        case bdem_ElemType::BDEM_FLOAT_ARRAY: {
                          typedef bsl::vector<float> T;
                          T& t        = RX.theModifiableFloatArray(0);
                          const T& ct = RY.theFloatArray(0);
                          t = ct;
                        } break;
                        case bdem_ElemType::BDEM_DOUBLE_ARRAY: {
                          typedef bsl::vector<double> T;
                          T& t        = RX.theModifiableDoubleArray(0);
                          const T& ct = RY.theDoubleArray(0);
                          t = ct;
                        } break;
                        case bdem_ElemType::BDEM_STRING_ARRAY: {
                          typedef bsl::vector<bsl::string> T;
                          T& t        = RX.theModifiableStringArray(0);
                          const T& ct = RY.theStringArray(0);
                          t = ct;
                        } break;
                        case bdem_ElemType::BDEM_DATETIME_ARRAY: {
                          typedef bsl::vector<bdet_Datetime> T;
                          T& t        = RX.theModifiableDatetimeArray(0);
                          const T& ct = RY.theDatetimeArray(0);
                          t = ct;
                        } break;
                        case bdem_ElemType::BDEM_DATE_ARRAY: {
                          typedef bsl::vector<bdet_Date> T;
                          T& t        = RX.theModifiableDateArray(0);
                          const T& ct = RY.theDateArray(0);
                          t = ct;
                        } break;
                        case bdem_ElemType::BDEM_TIME_ARRAY: {
                          typedef bsl::vector<bdet_Time> T;
                          T& t        = RX.theModifiableTimeArray(0);
                          const T& ct = RY.theTimeArray(0);
                          t = ct;
                        } break;
                        case bdem_ElemType::BDEM_LIST: {
                          typedef bdem_List T;
                          T& t        = RX.theModifiableList(0);
                          const T& ct = RY.theList(0);
                          t = ct;
                        } break;
                        case bdem_ElemType::BDEM_TABLE: {
                          typedef bdem_Table T;
                          T& t        = RX.theModifiableTable(0);
                          const T& ct = RY.theTable(0);
                          t = ct;
                        } break;
                        default: {
                          LOOP_ASSERT(LINE, 0 == 1);
                        } break;
                      }
                      if (veryVerbose) {
                          cout << "\t\t\tAFTER: "; P_(X); P(Y);
                      }
                      LOOP_ASSERT(LINE, X  != W);
                      LOOP_ASSERT(LINE, X  == R);
                      LOOP_ASSERT(LINE, Y  == R);  // readonly unchanged?
                      LOOP_ASSERT(LINE, RX == RY);
                  }
                  LOOP_ASSERT(LINE, X == R);      // readonly is out of scope
               } END_BSLMA_EXCEPTION_TEST
            }
        }
      }

DEFINE_TEST_CASE(34) {
        // --------------------------------------------------------------------
        // TESTING 'bdem_Table' reset METHOD
        //
        // Concerns:
        //   The function operates correctly when the source types object is
        //   within the table (within a 'bdem_Table' or 'bdem_List'.
        //   The function operates correctly when the source types object is
        //   the table's current column types object.
        //   The table remains in a consistent state.
        //
        // Plan:
        //   'reset' is always followed by 'appendNullRow' to
        //    verify the internal state is correct.
        //   The 'reset' function must be tested with aliasing.
        //   In this case, aliasing means that the 'my_ElemTypeArray'
        //   parameter to the function is contained within the 'bdem_List'
        //   itself, i.e., a list with these types or a table with these types.
        //   The test is performed with a 'bdem_List' using an allocator that
        //   scribbles over memory after deallocation 'bslma_TestAllocator' and
        //   using the no pool option.
        //
        //  Testing:
        //    void reset(const my_ElemTypeArray& srcTypes);
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl
                          << "Testing bdem_Table 'reset' method"
                          << endl
                          << "============================================"
                          << endl;

        if (verbose) cout <<
            "\nTesting x.reset(types) no aliasing" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_dtSpec;   // initial (destination) table
                const char *d_seSpec;   // source elemtypes
                int         d_numRows;  // number of rows to add
                const char *d_expSpec;  // expected table value
            } DATA[] = {
                //line  d-table       s-elems   numrows expected
                //----  -------       -------   ------- --------   Depth = 0
                { L_,   "",           "",       0,      ""            },
                { L_,   "",           "",       1,      ";"           },
                { L_,   ";",          "",       0,      ""            },
                { L_,   ";",          "",       1,      ";"           },

                //line  d-table       s-elems   numrows expected
                //----  -------       -------   ------- --------   Depth = 1
                { L_,   "C;",         "",       0,      ""            },
                { L_,   "C;",         "",       1,      ";"           },
                { L_,   "",           "C",      0,      "C;"          },
                { L_,   "",           "C",      1,      "C;n;"        },

                //line  d-table       s-elems   numrows expected
                //----  -------       -------   ------- --------   Depth = 2
                { L_,   "T;",         "A",      0,      "A;"          },
                { L_,   "T;",         "B",      0,      "B;"          },
                { L_,   "T;",         "C",      0,      "C;"          },
                { L_,   "T;",         "D",      0,      "D;"          },
                { L_,   "T;",         "E",      0,      "E;"          },
                { L_,   "T;",         "F",      0,      "F;"          },
                { L_,   "T;",         "G",      0,      "G;"          },
                { L_,   "T;",         "H",      0,      "H;"          },
                { L_,   "T;",         "I",      0,      "I;"          },
                { L_,   "T;",         "J",      0,      "J;"          },
                { L_,   "T;",         "K",      0,      "K;"          },
                { L_,   "T;",         "L",      0,      "L;"          },
                { L_,   "T;",         "M",      0,      "M;"          },
                { L_,   "T;",         "N",      0,      "N;"          },
                { L_,   "T;",         "O",      0,      "O;"          },
                { L_,   "T;",         "P",      0,      "P;"          },
                { L_,   "T;",         "Q",      0,      "Q;"          },
                { L_,   "T;",         "R",      0,      "R;"          },
                { L_,   "T;",         "S",      0,      "S;"          },
                { L_,   "T;",         "T",      0,      "T;"          },
                { L_,   "T;",         "U",      0,      "U;"          },
                { L_,   "T;",         "V",      0,      "V;"          },
                { L_,   "T;",         "A",      1,      "A;n;"        },
                { L_,   "T;",         "B",      1,      "B;n;"        },
                { L_,   "T;",         "C",      1,      "C;n;"        },
                { L_,   "T;",         "D",      1,      "D;n;"        },
                { L_,   "T;",         "E",      1,      "E;n;"        },
                { L_,   "T;",         "F",      1,      "F;n;"        },
                { L_,   "T;",         "G",      1,      "G;n;"        },
                { L_,   "T;",         "H",      1,      "H;n;"        },
                { L_,   "T;",         "I",      1,      "I;n;"        },
                { L_,   "T;",         "J",      1,      "J;n;"        },
                { L_,   "T;",         "K",      1,      "K;n;"        },
                { L_,   "T;",         "L",      1,      "L;n;"        },
                { L_,   "T;",         "M",      1,      "M;n;"        },
                { L_,   "T;",         "N",      1,      "N;n;"        },
                { L_,   "T;",         "O",      1,      "O;n;"        },
                { L_,   "T;",         "P",      1,      "P;n;"        },
                { L_,   "T;",         "Q",      1,      "Q;n;"        },
                { L_,   "T;",         "R",      1,      "R;n;"        },
                { L_,   "T;",         "S",      1,      "S;n;"        },
                { L_,   "T;",         "T",      1,      "T;n;"        },
                { L_,   "T;",         "U",      1,      "U;n;"        },
                { L_,   "T;",         "V",      1,      "V;n;"        },
                { L_,   "T;x;",       "A",      1,      "A;n;"        },
                { L_,   "T;x;",       "B",      1,      "B;n;"        },
                { L_,   "T;x;",       "C",      1,      "C;n;"        },
                { L_,   "T;x;",       "D",      1,      "D;n;"        },
                { L_,   "T;x;",       "E",      1,      "E;n;"        },
                { L_,   "T;x;",       "F",      1,      "F;n;"        },
                { L_,   "T;x;",       "G",      1,      "G;n;"        },
                { L_,   "T;x;",       "H",      1,      "H;n;"        },
                { L_,   "T;x;",       "I",      1,      "I;n;"        },
                { L_,   "T;x;",       "J",      1,      "J;n;"        },
                { L_,   "T;x;",       "K",      1,      "K;n;"        },
                { L_,   "T;x;",       "L",      1,      "L;n;"        },
                { L_,   "T;x;",       "M",      1,      "M;n;"        },
                { L_,   "T;x;",       "N",      1,      "N;n;"        },
                { L_,   "T;x;",       "O",      1,      "O;n;"        },
                { L_,   "T;x;",       "P",      1,      "P;n;"        },
                { L_,   "T;x;",       "Q",      1,      "Q;n;"        },
                { L_,   "T;x;",       "R",      1,      "R;n;"        },
                { L_,   "T;x;",       "S",      1,      "S;n;"        },
                { L_,   "T;x;",       "T",      1,      "T;n;"        },
                { L_,   "T;x;",       "U",      1,      "U;n;"        },
                { L_,   "T;x;",       "V",      1,      "V;n;"        },

                //line  d-table       s-elems   numrows expected
                //----  -------       -------   ------- --------   Depth = 3
                { L_,   "TB;",        "A",      0,      "A;"          },
                { L_,   "TB;xy;",     "A",      0,      "A;"          },
                { L_,   "TB;xy;",     "B",      0,      "B;"          },
                { L_,   "TB;xy;",     "C",      0,      "C;"          },
                { L_,   "TB;xy;",     "D",      0,      "D;"          },
                { L_,   "TB;xy;",     "E",      0,      "E;"          },
                { L_,   "TB;xy;",     "F",      0,      "F;"          },
                { L_,   "TB;xy;",     "G",      0,      "G;"          },
                { L_,   "TB;xy;",     "H",      0,      "H;"          },
                { L_,   "TB;xy;",     "I",      0,      "I;"          },
                { L_,   "TB;xy;",     "J",      0,      "J;"          },
                { L_,   "TB;xy;",     "K",      0,      "K;"          },
                { L_,   "TB;xy;",     "L",      0,      "L;"          },
                { L_,   "TB;xy;",     "M",      0,      "M;"          },
                { L_,   "TB;xy;",     "N",      0,      "N;"          },
                { L_,   "TB;xy;",     "O",      0,      "O;"          },
                { L_,   "TB;xy;",     "P",      0,      "P;"          },
                { L_,   "TB;xy;",     "Q",      0,      "Q;"          },
                { L_,   "TB;xy;",     "R",      0,      "R;"          },
                { L_,   "TB;xy;",     "S",      0,      "S;"          },
                { L_,   "TB;xy;",     "T",      0,      "T;"          },
                { L_,   "TB;xy;",     "U",      0,      "U;"          },
                { L_,   "TB;xy;",     "V",      0,      "V;"          },
                { L_,   "TB;xy;",     "T",      1,      "T;n;"        },
                { L_,   "TB;xy;nx;",  "V",      0,      "V;"          },
                { L_,   "B;",         "AT",     1,      "AT;nn;"      },
                { L_,   "B;x;",       "AT",     1,      "AT;nn;"      },
                { L_,   "B;x;",       "BT",     1,      "BT;nn;"      },
                { L_,   "B;x;",       "CT",     1,      "CT;nn;"      },
                { L_,   "B;x;",       "DT",     1,      "DT;nn;"      },
                { L_,   "B;x;",       "ET",     1,      "ET;nn;"      },
                { L_,   "B;x;",       "FT",     1,      "FT;nn;"      },
                { L_,   "B;x;",       "GT",     1,      "GT;nn;"      },
                { L_,   "B;x;",       "HT",     1,      "HT;nn;"      },
                { L_,   "B;x;",       "IT",     1,      "IT;nn;"      },
                { L_,   "B;x;",       "JT",     1,      "JT;nn;"      },
                { L_,   "B;x;",       "KT",     1,      "KT;nn;"      },
                { L_,   "B;x;",       "LT",     1,      "LT;nn;"      },
                { L_,   "B;x;",       "MT",     1,      "MT;nn;"      },
                { L_,   "B;x;",       "NT",     1,      "NT;nn;"      },
                { L_,   "B;x;",       "OT",     1,      "OT;nn;"      },
                { L_,   "B;x;",       "PT",     1,      "PT;nn;"      },
                { L_,   "B;x;",       "QT",     1,      "QT;nn;"      },
                { L_,   "B;x;",       "RT",     1,      "RT;nn;"      },
                { L_,   "B;x;",       "ST",     1,      "ST;nn;"      },
                { L_,   "B;x;",       "TT",     1,      "TT;nn;"      },
                { L_,   "B;x;",       "UT",     1,      "UT;nn;"      },
                { L_,   "B;x;",       "VT",     1,      "VT;nn;"      },
                { L_,   "B;x;y;",     "AT",     1,      "AT;nn;"      },

                //line  d-table       s-elems   numrows expected
                //----  -------       -------   ------- --------   Depth = 4
                { L_,   "TC;",        "AB",     0,      "AB;"         },
                { L_,   "TC;",        "AB",     1,      "AB;nn;"      },
                { L_,   "TC;xx;",     "AB",     0,      "AB;"         },
                { L_,   "TC;xx;",     "AB",     1,      "AB;nn;"      },
                { L_,   "TC;",        "GB",     0,      "GB;"         },
                { L_,   "TC;",        "GB",     1,      "GB;nn;"      },
                { L_,   "TC;xx;",     "GB",     0,      "GB;"         },
                { L_,   "TC;xx;",     "GB",     1,      "GB;nn;"      },
                { L_,   "T;",         "ABC",    0,      "ABC;"        },
                { L_,   "T;",         "ABC",    1,      "ABC;nnn;"    },
                { L_,   "T;x;",       "ABC",    0,      "ABC;"        },
                { L_,   "T;x;",       "ABC",    1,      "ABC;nnn;"    },
                { L_,   "T;",         "AGC",    0,      "AGC;"        },
                { L_,   "T;",         "AGC",    1,      "AGC;nnn;"    },
                { L_,   "T;x;",       "AGC",    0,      "AGC;"        },
                { L_,   "T;x;",       "AGC",    1,      "AGC;nnn;"    },
                { L_,   "TCI;",       "B",      0,      "B;"          },
                { L_,   "TCI;",       "B",      1,      "B;n;"        },
                { L_,   "TCI;xxx;",   "B",      0,      "B;"          },
                { L_,   "TCI;xxx;",   "B",      1,      "B;n;"        },
                { L_,   "",           "ABCD",   0,      "ABCD;"       },
                { L_,   "",           "ABCD",   1,      "ABCD;nnnn;"  },
                { L_,   "ABCD;xyxy;", "",       0,      ""            },

                //line  d-table       s-elems   numrows expected
                //----  -------       -------   ------- --------   Depth = 5
                { L_,   "TCF;",       "TC",     0,      "TC;"         },
                { L_,   "TCF;",       "TC",     1,      "TC;nn;"      },
                { L_,   "TCF;xyx;",   "TC",     0,      "TC;"         },
                { L_,   "TCF;xyx;",   "TC",     1,      "TC;nn;"      },
                { L_,   "CF;",        "TCG",    0,      "TCG;"        },
                { L_,   "CF;xx;",     "TCG",    0,      "TCG;"        },
                { L_,   "CF;xx;",     "TCG",    1,      "TCG;nnn;"    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *D_SPEC = DATA[ti].d_dtSpec;
                const char *S_SPEC = DATA[ti].d_seSpec;
                int        NUMROWS = DATA[ti].d_numRows;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = ((int) strlen(D_SPEC))/2 + strlen(S_SPEC);
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                Table DD(h(D_SPEC));            // control for destination
                my_ElemTypeArray SS;          // control for source elemtypes
                appendToElemTypeArray(&SS, S_SPEC);
                Table EE(h(E_SPEC));            // control for expected value
                const int EXP_LEN = bsl::strlen(E_SPEC);

                if (1 == EE.numRows() && ';' == E_SPEC[0]) {
                    EE.makeRowsNull(0, 1);
                }

                for (int i = 0; i < EXP_LEN; ++i) {
                    if ('n' == E_SPEC[i]) {
                        EE.makeRowsNull(0, 1);
                        break;
                    }
                }

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(D_SPEC); P_(S_SPEC); P_(NUMROWS);
                                                P(E_SPEC);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(SS);
                    cout << "\t\t"; P(EE); P(LINE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                if (veryVerbose) cout << "\t\treset(se)" << endl;
                {
                  bslma_TestAllocator ta;
                  bslma_TestAllocator& testAllocator = ta;

                  BEGIN_BSLMA_EXCEPTION_TEST {

                    Table x(DD, bdem_AggregateOption::BDEM_PASS_THROUGH,
                            &ta);
                    const Table &X = x;
                    {
                        my_ElemTypeArray s(SS, &ta);
                        const my_ElemTypeArray &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.reset(vectorData(s), s.size()); // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        ASSERT(x.numRows() == 0);

                        x.appendNullRows(NUMROWS);

                        LOOP3_ASSERT(LINE, EE, X, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }

        const char *DATA[] = { "", "A",   "T",    "GT",    "GHT", "BGHT"   };
        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        if (verbose) cout << endl << "Testing alias cases" << endl;

        // A table is created with a single

        for (int ii = 0; ii < NUM_DATA; ++ii) {

            const char *P_SPEC = DATA[ii];
            if (veryVerbose) P(P_SPEC);

            my_ElemTypeArray PS;
            appendToElemTypeArray(&PS, P_SPEC);

            if (veryVeryVerbose) P(PS);

            Table R(PS, Z);
            if (veryVeryVerbose) P(R);
            BEGIN_BSLMA_EXCEPTION_TEST {
              const int AL = testAllocator.allocationLimit();
              testAllocator.setAllocationLimit(-1);
              Table U(bdem_AggregateOption::BDEM_PASS_THROUGH,
                      Z);

              U = R;
              if (veryVeryVerbose) P(U);

              testAllocator.setAllocationLimit(AL);
              my_ElemTypeArray e1;
              getElemTypes(&e1, U);
              U.reset(e1);
              my_ElemTypeArray e2;
              getElemTypes(&e2, U);
              if (veryVeryVerbose) P(e2);
              testAllocator.setAllocationLimit(-1);

              getElemTypes(&e1, U);
              ASSERT(e1 == PS);
              ASSERT(U.numRows() == 0);

              U.appendNullRow();
              ASSERT(U.numRows() == 1);
            } END_BSLMA_EXCEPTION_TEST
        }

        if (verbose) cout << endl
                          << "Testing overlap cases with 'bdem_Table'"
                          << endl;

        // A table is created with a single

        for (int ii = 0; ii < NUM_DATA; ++ii) {

            const char *P_SPEC = DATA[ii];
            if (veryVerbose) P(P_SPEC);

            for (int jj = 0; jj < NUM_DATA; ++jj) {

                const char *S_SPEC = DATA[jj];
                if (veryVerbose) P(S_SPEC);
                my_ElemTypeArray PS;
                PS.push_back(bdem_ElemType::BDEM_TABLE);
                appendToElemTypeArray(&PS, P_SPEC);

                if (veryVeryVerbose) P(PS);

                Table R(PS, Z);
                R.appendNullRow();
                my_ElemTypeArray SS;        // control for source elemtypes
                appendToElemTypeArray(&SS, S_SPEC);
                R.theModifiableRow(0).theModifiableTable(0).reset(SS);
                if (veryVeryVerbose) P(R);
                BEGIN_BSLMA_EXCEPTION_TEST {
                  const int AL = testAllocator.allocationLimit();
                  testAllocator.setAllocationLimit(-1);
                  Table U(bdem_AggregateOption::BDEM_PASS_THROUGH, Z);

                  U = R;
                  if (veryVeryVerbose) P(U);

                  testAllocator.setAllocationLimit(AL);
                  my_ElemTypeArray e1;
                  getElemTypes(&e1, U[0].theTable(0));
                  if (veryVeryVerbose) P(e1);
                  getElemTypes(&e1, U[0].theTable(0));
                  U.reset(e1);
                  getElemTypes(&e1, U);
                  if (veryVeryVerbose) P(e1);
                  testAllocator.setAllocationLimit(-1);

                  getElemTypes(&e1, U);
                  ASSERT(e1 == SS);
                  ASSERT(U.numRows() == 0);

                  U.appendNullRow();
                  ASSERT(U.numRows() == 1);
                } END_BSLMA_EXCEPTION_TEST
            }
        }

        if (verbose)
            cout << endl << "Testing overlap cases with 'bdem_List'" << endl;

        for (int ii = 0; ii < NUM_DATA; ++ii) {

            const char *P_SPEC = DATA[ii];
            if (veryVerbose) P(P_SPEC);

            for (int jj = 0; jj < NUM_DATA; ++jj) {

                const char *S_SPEC = DATA[jj];
                if (veryVerbose) P(S_SPEC);
                my_ElemTypeArray PS;
                PS.push_back(bdem_ElemType::BDEM_LIST);
                appendToElemTypeArray(&PS, P_SPEC);

                if (veryVeryVerbose) P(PS);

                Table R(PS, Z);
                R.appendNullRow();
                my_ElemTypeArray SS;          // control for source elemtypes
                appendToElemTypeArray(&SS, S_SPEC);
                R.theModifiableRow(0).theModifiableList(0).reset(SS);
                if (veryVeryVerbose) P(R);
                BEGIN_BSLMA_EXCEPTION_TEST {
                  const int AL = testAllocator.allocationLimit();
                  testAllocator.setAllocationLimit(-1);
                  Table U(bdem_AggregateOption::BDEM_PASS_THROUGH, Z);

                  U = R;
                  if (veryVeryVerbose) P(U);

                  testAllocator.setAllocationLimit(AL);
                  my_ElemTypeArray e1;
                  getElemTypes(&e1, U[0].theList(0));
                  if (veryVeryVerbose) P(e1);
                  getElemTypes(&e1, U[0].theList(0));
                  U.reset(e1);
                  getElemTypes(&e1, U);
                  if (veryVeryVerbose) P(e1);
                  testAllocator.setAllocationLimit(-1);

                  getElemTypes(&e1, U);
                  ASSERT(e1 == SS);
                  ASSERT(U.numRows() == 0);

                  U.appendNullRow();
                  ASSERT(U.numRows() == 1);
                } END_BSLMA_EXCEPTION_TEST
            }
        }
      }

DEFINE_TEST_CASE(33) {
        // --------------------------------------------------------------------
        // TESTING 'bdem_Table' SWAPROWS METHOD
        //
        // Concerns:
        //   We are concerned that, for an object of any length, 'swapRows'
        //   must exchange the values at any valid pair of index positions
        //   while leaving all other elements unaffected.
        //
        // Plan:
        //   For each object X in a set of objects ordered by increasing length
        //   L containing uniform values, V0.
        //     For each valid index position P1 in [0 .. L-1]:
        //       For each valid index position P2 in [0 .. L-1]:
        //         1.  Create a test object T from X using the copy ctor.
        //         2.  Replace the element at P1 with V1 and at P2 with V2.
        //         3.  Swap these elements in T.
        //         4.  Verify that
        //               (i)     V2 == T[P1]     always
        //         5.  Verify that
        //               (ii)    V1 == T[P2]     if (P1 != P2)
        //                       V2 == T[P2]     if (P1 == P2)
        //         6.  For each index position, i, in [0 .. L-1] verify that:
        //               (iii)   V0 == T[i]      if (P1 != i && P2 != i)
        //
        // Testing:
        //   void swapRows(int index1, int index2);
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl
                          << "Testing bdem_Table 'swapRows' Method" << endl
                          << "====================================" << endl;

        if (verbose) cout << "\nTesting swapRows(index1, index2)" << endl;

        const char *SPECS[] = {
           ";",   "A;x;",   "ABC;xxx;",   ";;",   "ABC;xxx;yyy;",
           ";;;", "ABCD;xyxy;yxxy;yxuu;",
           "ABCD;xxxx;yyyy;uuuu;xuxu;",   "ABCD;xuuu;yuuu;xyxy;yxyx;uxux;"
        };
        const int NUM_SPECS = sizeof(SPECS) / sizeof(*SPECS);

        Table mX(&testAllocator);                            // control

        for (int ii = 0; ii < NUM_SPECS; ++ii) {
            Table mX(&testAllocator);  const Table& X = mX;  // control
            hh(&mX, SPECS[ii]);

            int iLen = mX.numRows();

            if (verbose) { cout << "\t"; P_(SPECS[ii]); P(X); }
            for (int pos1 = 0; pos1 < iLen; ++pos1) { // for each position
                for (int pos2 = 0; pos2 < iLen; ++pos2) { // for each position
                    Table mT(X, &testAllocator);
                    const Table& T = mT;                  // object under test
                    LOOP3_ASSERT(iLen, pos1, pos2, X == T);

                    if (veryVerbose) { cout << "\t\t";  P_(pos1);
                                       P_(pos2);        P(T); }
                    mT.swapRows(pos1, pos2);
                    if (veryVerbose) { cout << "\t\t";  P_(pos1);
                                       P_(pos2);        P(T); }

                    LOOP3_ASSERT(iLen, pos1, pos2, T[pos1] == X[pos2]);
                    LOOP3_ASSERT(iLen, pos1, pos2, T[pos2] == X[pos1]);

                    for (int i = 0; i < iLen; ++i) {
                        if (i == pos1 || i == pos2) continue;
                        LOOP4_ASSERT(iLen, pos1, pos2, i, T[i] == X[i]);
                    }
                }
            }
        }
      }

DEFINE_TEST_CASE(32) {
        // --------------------------------------------------------------------
        // TESTING 'bdem_Table' ISNULL, ISNONNULL, ETC. METHODS
        //
        // Concerns:
        //   That table & row accessors regarding nullness of elements work.
        //
        // Plan:
        //   Create tables with 'hh', do table-driven access of them and
        //   confirm the results.
        //
        // Testing:
        //   bool isNonNull(int ri, int ci) const;
        //   bool isNull(int ri, int ci) const;
        //   bool isAnyInRowNonNull(int ri) const;
        //   bool isAnyInRowNull(int ri) const;
        //   bool isAnyInColumnNonNull(int ci) const;
        //   bool isAnyInColumnNull(int ci) const;
        //   bool isAnyNonNull() const;
        //   bool isAnyNull() const;
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl
                          << "Testing bdem_Table 'IsNull' Accessors" << endl
                          << "=====================================" << endl;

        {
            static const struct {
                int         d_lineNum;          // source line number
                const char *d_spec;             // table spec
                int         d_ri;               // row index (ignore if -1)
                int         d_ci;               // column index (ignore if -1)
                int         d_isAnyInRowNonNull;// isAnyInRowNonNull() result
                int         d_isAnyInRowNull;   // isAnyInRowNull() result
                int         d_isAnyInColumnNonNull;
                                                // isAnyInColumnNonNull() rslt.
                int         d_isAnyInColumnNull;// isAnyInColumnNull() rslt.
                int         d_isNonNull;        // isNonNull() result
                int         d_isAnyNonNull;     // isAnyNonNull() result
                int         d_isAnyNull;        // isAnyNull() result
            } DATA[] = {
                //                           rnn rn  cnn cn  nn  anynn anyn
                //line d-spec       ri  ci   |   |   |   |   |   |     /
                //---- ------       --  --   --  --  --  --  --  --  --
                { L_,  "",          -1, -1,  0,  0,  0,  0,  0,  0,  0,   },
                { L_,  ";",          0, -1,  0,  0,  0,  0,  0,  0,  0,   },
                { L_,  "A;",        -1,  0,  0,  0,  0,  0,  0,  0,  0,   },
                { L_,  "A;x;",       0,  0,  1,  0,  1,  0,  1,  1,  0,   },
                { L_,  "A;u;",       0,  0,  0,  1,  0,  1,  0,  0,  1,   },
                { L_,  "AB;",       -1,  0,  0,  0,  0,  0,  0,  0,  0,   },
                { L_,  "AB;",       -1,  1,  0,  0,  0,  0,  0,  0,  0,   },

                { L_,  "AB;xx;",     0,  0,  1,  0,  1,  0,  1,  1,  0,   },
                { L_,  "AB;uu;",     0,  0,  0,  1,  0,  1,  0,  0,  1,   },
                { L_,  "AB;xu;",     0,  0,  1,  1,  1,  0,  1,  1,  1,   },
                { L_,  "AB;ux;",     0,  0,  1,  1,  0,  1,  0,  1,  1,   },

                { L_,  "AB;xx;",     0,  1,  1,  0,  1,  0,  1,  1,  0,   },
                { L_,  "AB;uu;",     0,  1,  0,  1,  0,  1,  0,  0,  1,   },
                { L_,  "AB;xu;",     0,  1,  1,  1,  0,  1,  0,  1,  1,   },
                { L_,  "AB;ux;",     0,  1,  1,  1,  1,  0,  1,  1,  1,   },

                { L_,  "AB;xx;xx;",  0,  0,  1,  0,  1,  0,  1,  1,  0,   },
                { L_,  "AB;uu;xx;",  0,  0,  0,  1,  1,  1,  0,  1,  1,   },
                { L_,  "AB;xu;xx;",  0,  0,  1,  1,  1,  0,  1,  1,  1,   },
                { L_,  "AB;ux;xx;",  0,  0,  1,  1,  1,  1,  0,  1,  1,   },

                { L_,  "AB;uu;uu;",  0,  0,  0,  1,  0,  1,  0,  0,  1,   },
                { L_,  "AB;xu;xu;",  0,  0,  1,  1,  1,  0,  1,  1,  1,   },
                { L_,  "AB;xx;xx;",  0,  1,  1,  0,  1,  0,  1,  1,  0,   },
                { L_,  "AB;uu;xx;",  0,  1,  0,  1,  1,  1,  0,  1,  1,   },

                { L_,  "AB;xu;xx;",  0,  1,  1,  1,  1,  1,  0,  1,  1,   },
                { L_,  "AB;ux;xx;",  0,  1,  1,  1,  1,  0,  1,  1,  1,   },
                { L_,  "AB;uu;uu;",  0,  1,  0,  1,  0,  1,  0,  0,  1,   },
                { L_,  "AB;xu;xu;",  0,  1,  1,  1,  0,  1,  0,  1,  1,   },

                { L_,  "AB;xx;xx;",  1,  0,  1,  0,  1,  0,  1,  1,  0,   },
                { L_,  "AB;uu;xx;",  1,  0,  1,  0,  1,  1,  1,  1,  1,   },
                { L_,  "AB;xu;xx;",  1,  0,  1,  0,  1,  0,  1,  1,  1,   },
                { L_,  "AB;ux;xx;",  1,  0,  1,  0,  1,  1,  1,  1,  1,   },

                { L_,  "AB;uu;uu;",  1,  0,  0,  1,  0,  1,  0,  0,  1,   },
                { L_,  "AB;xu;xu;",  1,  0,  1,  1,  1,  0,  1,  1,  1,   },
                { L_,  "AB;xx;xx;",  1,  1,  1,  0,  1,  0,  1,  1,  0,   },
                { L_,  "AB;uu;xx;",  1,  1,  1,  0,  1,  1,  1,  1,  1,   },

                { L_,  "AB;xu;xx;",  1,  1,  1,  0,  1,  1,  1,  1,  1,   },
                { L_,  "AB;ux;xx;",  1,  1,  1,  0,  1,  0,  1,  1,  1,   },
                { L_,  "AB;uu;uu;",  1,  1,  0,  1,  0,  1,  0,  0,  1,   },
                { L_,  "AB;xu;xu;",  1,  1,  1,  1,  0,  1,  0,  1,  1,   },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE         = DATA[ti].d_lineNum;
                const char *SPEC         = DATA[ti].d_spec;
                const int   RI           = DATA[ti].d_ri;
                const int   CI           = DATA[ti].d_ci;
                const int   ISANYINROWNONNULL =
                                           DATA[ti].d_isAnyInRowNonNull;
                const int   ISANYINROWNULL =
                                           DATA[ti].d_isAnyInRowNull;
                const int   ISANYINCOLUMNNONNULL =
                                           DATA[ti].d_isAnyInColumnNonNull;
                const int   ISANYINCOLUMNNULL =
                                           DATA[ti].d_isAnyInColumnNull;
                const int   ISANYNONNULL = DATA[ti].d_isAnyNonNull;
                const int   ISANYNULL    = DATA[ti].d_isAnyNull;
                const int   ISNONNULL    = DATA[ti].d_isNonNull;

                Table mU(&testAllocator); hh(&mU, SPEC);
                const Table& U = mU;

                // make unsets null
                for (int row = 0; row < U.numRows(); ++row) {
                    for (int col = 0; col < U.numColumns(); ++col) {
                        bdem_ElemRef ref = mU.theModifiableRow(row)[col];
                        if ((bdem_ElemType::BDEM_CHAR == ref.type() &&
                                                        UA == ref.theChar()) ||
                           (bdem_ElemType::BDEM_SHORT == ref.type() &&
                                                       UB == ref.theShort())) {
                            ref.makeNull();
                        }
                    }
                }

                if (veryVerbose) { P(SPEC); P_(RI); P_(CI); P(U); }

                LOOP_ASSERT(LINE, ISANYNONNULL == U.isAnyNonNull());
                LOOP_ASSERT(LINE, ISANYNULL    == U.isAnyNull());
                if (RI >= 0) {
                    LOOP_ASSERT(LINE,
                             ISANYINROWNONNULL == U.theRow(RI).isAnyNonNull());
                    LOOP_ASSERT(LINE,
                                   ISANYINROWNULL == U.theRow(RI).isAnyNull());
                }
                if (CI >= 0) {
                    LOOP_ASSERT(LINE,
                           ISANYINCOLUMNNONNULL == U.isAnyInColumnNonNull(CI));
                    LOOP_ASSERT(LINE,
                                 ISANYINCOLUMNNULL == U.isAnyInColumnNull(CI));
                }
                if (RI >= 0 && CI >= 0)
                    LOOP_ASSERT(LINE, ISNONNULL == U[RI][CI].isNonNull());
                if (RI >= 0 && CI >= 0)
                    LOOP_ASSERT(LINE, (1 - ISNONNULL) == U[RI][CI].isNull());
            }
        }
      }

DEFINE_TEST_CASE(31) {
        // --------------------------------------------------------------------
        // TESTING 'bdem_Row' REPLACEVALUE METHOD
        // Concerns:
        //   For the 'bdem_Row' 'replaceValue' method, the following
        //   properties must hold:
        //     1. The source is left unaffected (apart from aliasing).
        //     2. The subsequent existence of the source has no effect on the
        //        result object (apart from aliasing).
        //     3. The function is alias safe including when
        //        a) the source and destination rows are the same
        //        b) the source includes the destination.
        //        c) the destination includes the source.
        //     4. The function preserves object invariants.
        //     5. The implementations for the overlap and alias cases are
        //        different.  Tests must include:
        //        a) source rows containing aggregates and NOT containing
        //           aggregates.
        //        b) destination rows containing aggregates and NOT
        //           containing aggregates.
        //        c) source rows overlapping the destination row.
        //        d) source rows the same as the destination row.
        //
        // Plan:
        //   Use the enumeration technique to a depth of 5 for both the normal
        //   and alias cases.  Data is tabulated explicitly for the 'replace'
        //   method that takes a range from a source row (or itself, for the
        //   aliasing test); the "scalar" 'replace' test selects a subset of
        //   the table by testing 'if (1 == NE)' where 'NE' is the tabulated
        //   number of elements to replace.
        //     - In the "canonical state" (black-box) tests, we confirm that
        //       the source is unmodified by the method call, and that its
        //       subsequent destruction has no effect on the destination
        //       object.
        //     - In all cases we want to make sure that after the application
        //       of the operation, the object is allowed to go out of scope
        //       directly to enable the destructor to assert object invariants.
        //     - Each object constructed should be wrapped in separate
        //       BSLMA test assert macros and use gg as an optimization.
        //
        // Testing:
        //   void replaceValues(int di, const bdem_Row& sr, int si, int ne);
        //   void replaceValue(int di, const bdem_Row& sr, int si);
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl
                          << "Testing 'bdem_Row' 'replaceValue'" << endl
                          << "=================================" << endl;

        if (verbose) cout <<
            "\nTesting replaceValues(di, sr, si, ne) et al. (no aliasing)"
            << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_drSpec;   // initial (destination) row
                int         d_di;       // index at which to replace into dr
                const char *d_slSpec;   // source list
                int         d_si;       // index at which to replace from sl
                int         d_ne;       // number of elements to replace
                const char *d_expSpec;  // expected list value
            } DATA[] = {
                //line  d-list       di  s-list      si   ne  expected
                //----  ------       --  ------      --   --  -------- Depth=0
                { L_,   "",          0,  "",         0,   0,  ""             },

                //line  d-list       di  s-list      si   ne  expected
                //----  ------       --  ------      --   --  -------- Depth=1
                { L_,   "xA",        0,  "",         0,   0,  "xA"           },
                { L_,   "xA",        1,  "",         0,   0,  "xA"           },
                { L_,   "xB",        1,  "",         0,   0,  "xB"           },
                { L_,   "xC",        1,  "",         0,   0,  "xC"           },
                { L_,   "xD",        1,  "",         0,   0,  "xD"           },
                { L_,   "xE",        1,  "",         0,   0,  "xE"           },
                { L_,   "xF",        1,  "",         0,   0,  "xF"           },
                { L_,   "xG",        1,  "",         0,   0,  "xG"           },
                { L_,   "xH",        1,  "",         0,   0,  "xH"           },
                { L_,   "xI",        1,  "",         0,   0,  "xI"           },
                { L_,   "xJ",        1,  "",         0,   0,  "xJ"           },
                { L_,   "xK",        1,  "",         0,   0,  "xK"           },
                { L_,   "xL",        1,  "",         0,   0,  "xL"           },
                { L_,   "xM",        1,  "",         0,   0,  "xM"           },
                { L_,   "xN",        1,  "",         0,   0,  "xN"           },
                { L_,   "xO",        1,  "",         0,   0,  "xO"           },
                { L_,   "xP",        1,  "",         0,   0,  "xP"           },
                { L_,   "xQ",        1,  "",         0,   0,  "xQ"           },
                { L_,   "xR",        1,  "",         0,   0,  "xR"           },
                { L_,   "xS",        1,  "",         0,   0,  "xS"           },
                { L_,   "xT",        1,  "",         0,   0,  "xT"           },
                { L_,   "xU",        1,  "",         0,   0,  "xU"           },

                { L_,   "",          0,  "xB",       0,   0,  ""             },
                { L_,   "",          0,  "xB",       1,   0,  ""             },

                //line  d-list       di  s-list      si   ne  expected
                //----  ------       --  ------      --   --  -------- Depth=2
                { L_,   "xAxB",      0,  "",         0,   0,  "xAxB"         },
                { L_,   "xAxB",      1,  "",         0,   0,  "xAxB"         },
                { L_,   "xAxB",      2,  "",         0,   0,  "xAxB"         },
                { L_,   "xAxT",      2,  "",         0,   0,  "xAxT"         },
                { L_,   "xAxU",      2,  "",         0,   0,  "xAxU"         },

                { L_,   "xA",        0,  "yB",       0,   0,  "xA"           },
                { L_,   "yA",        0,  "xA",       0,   1,  "xA"           },
                { L_,   "yB",        0,  "xB",       0,   1,  "xB"           },
                { L_,   "yC",        0,  "xC",       0,   1,  "xC"           },
                { L_,   "yD",        0,  "xD",       0,   1,  "xD"           },
                { L_,   "yE",        0,  "xE",       0,   1,  "xE"           },
                { L_,   "yF",        0,  "xF",       0,   1,  "xF"           },
                { L_,   "yG",        0,  "yG",       0,   1,  "yG"           },
                { L_,   "xG",        0,  "xG",       0,   1,  "xG"           },
                { L_,   "xG",        0,  "yG",       0,   1,  "yG"           },
                { L_,   "yG",        0,  "xG",       0,   1,  "xG"           },
                { L_,   "yH",        0,  "xH",       0,   1,  "xH"           },
                { L_,   "yI",        0,  "xI",       0,   1,  "xI"           },
                { L_,   "yJ",        0,  "xJ",       0,   1,  "xJ"           },
                { L_,   "yK",        0,  "xK",       0,   1,  "xK"           },
                { L_,   "yL",        0,  "xL",       0,   1,  "xL"           },
                { L_,   "yM",        0,  "xM",       0,   1,  "xM"           },
                { L_,   "yN",        0,  "xN",       0,   1,  "xN"           },
                { L_,   "yO",        0,  "xO",       0,   1,  "xO"           },
                { L_,   "yP",        0,  "xP",       0,   1,  "xP"           },
                { L_,   "yQ",        0,  "xQ",       0,   1,  "xQ"           },
                { L_,   "yR",        0,  "xR",       0,   1,  "xR"           },
                { L_,   "yS",        0,  "xS",       0,   1,  "xS"           },
                { L_,   "yT",        0,  "xT",       0,   1,  "xT"           },
                { L_,   "yU",        0,  "xU",       0,   1,  "xU"           },
                { L_,   "yV",        0,  "xV",       0,   1,  "xV"           },

                { L_,   "",          0,  "xAxB",     0,   0,  ""             },
                { L_,   "",          0,  "xAxB",     1,   0,  ""             },
                { L_,   "",          0,  "xAxB",     2,   0,  ""             },

                //line  d-list       di  s-list      si   ne  expected
                //----  ------       --  ------      --   --  -------- Depth=3
                { L_,   "xAxBxC",    0,  "",         0,   0,  "xAxBxC"       },
                { L_,   "xAxBxC",    1,  "",         0,   0,  "xAxBxC"       },
                { L_,   "xAxBxC",    2,  "",         0,   0,  "xAxBxC"       },
                { L_,   "xAxBxC",    3,  "",         0,   0,  "xAxBxC"       },
                { L_,   "xAxBxU",    3,  "",         0,   0,  "xAxBxU"       },

                { L_,   "xAxB",      0,  "xC",       0,   0,  "xAxB"         },
                { L_,   "xAxB",      0,  "yA",       0,   1,  "yAxB"         },
                { L_,   "yUxB",      0,  "xU",       0,   1,  "xUxB"         },
                { L_,   "yVxB",      0,  "xV",       0,   1,  "xVxB"         },
                { L_,   "xAxB",      1,  "yB",       0,   1,  "xAyB"         },
                { L_,   "xAxT",      1,  "yT",       0,   1,  "xAyT"         },
                { L_,   "xByU",      1,  "xU",       0,   1,  "xBxU"         },
                { L_,   "xAyU",      1,  "xU",       0,   1,  "xAxU"         },
                { L_,   "xAyV",      1,  "xV",       0,   1,  "xAxV"         },

                { L_,   "xA",        0,  "yAuA",     0,   0,  "xA"           },
                { L_,   "xA",        0,  "yAuA",     0,   1,  "yA"           },
                { L_,   "xA",        0,  "yAuA",     1,   0,  "xA"           },
                { L_,   "xA",        0,  "yAuA",     1,   1,  "uA"           },
                { L_,   "xA",        0,  "yAuA",     2,   0,  "xA"           },
                { L_,   "xA",        1,  "yAuA",     0,   0,  "xA"           },
                { L_,   "xA",        1,  "yAuA",     1,   0,  "xA"           },
                { L_,   "xA",        1,  "yAuA",     2,   0,  "xA"           },

                { L_,   "",          0,  "xAxBxC",   0,   0,  ""             },
                { L_,   "",          0,  "xAxBxC",   1,   0,  ""             },
                { L_,   "",          0,  "xAxBxC",   2,   0,  ""             },
                { L_,   "",          0,  "xAxBxC",   3,   0,  ""             },

                //line  d-list       di  s-list      si   ne  expected
                //----  ------       --  ------      --   --  -------- Depth=4
                { L_,   "xAyBuCuD",  0,  "",         0,   0,  "xAyBuCuD"     },
                { L_,   "xAyBuCuD",  1,  "",         0,   0,  "xAyBuCuD"     },
                { L_,   "xAyBuCuD",  2,  "",         0,   0,  "xAyBuCuD"     },
                { L_,   "xAyBuCuD",  3,  "",         0,   0,  "xAyBuCuD"     },
                { L_,   "xAyBuCuD",  4,  "",         0,   0,  "xAyBuCuD"     },

                { L_,   "xAyBuC",    0,  "uA",       0,   0,  "xAyBuC"       },
                { L_,   "xAyBuC",    0,  "uA",       0,   1,  "uAyBuC"       },
                { L_,   "xAyBuC",    0,  "uA",       1,   0,  "xAyBuC"       },
                { L_,   "xAyBuC",    1,  "uB",       0,   0,  "xAyBuC"       },
                { L_,   "xAyBuC",    1,  "uB",       0,   1,  "xAuBuC"       },
                { L_,   "xAyBuC",    1,  "uB",       1,   0,  "xAyBuC"       },
                { L_,   "xAyBuC",    2,  "uC",       0,   0,  "xAyBuC"       },
                { L_,   "xAyBuC",    2,  "uC",       0,   1,  "xAyBuC"       },
                { L_,   "xAyBuC",    2,  "uC",       1,   0,  "xAyBuC"       },
                { L_,   "xAyBuC",    3,  "uC",       0,   0,  "xAyBuC"       },
                { L_,   "xAyAuA",    3,  "uC",       1,   0,  "xAyAuA"       },

                { L_,   "xAyB",      0,  "uAuB",     0,   0,  "xAyB"         },
                { L_,   "xAyB",      0,  "uAuB",     0,   1,  "uAyB"         },
                { L_,   "xAyB",      0,  "uAuB",     0,   2,  "uAuB"         },
                { L_,   "xAyB",      0,  "uBuA",     1,   0,  "xAyB"         },
                { L_,   "xAyB",      0,  "uBuA",     1,   1,  "uAyB"         },
                { L_,   "xAyB",      0,  "uBuA",     2,   0,  "xAyB"         },
                { L_,   "xAyB",      1,  "uBuA",     0,   0,  "xAyB"         },
                { L_,   "xAyB",      1,  "uBuA",     0,   1,  "xAuB"         },
                { L_,   "xAyB",      1,  "xBuB",     1,   0,  "xAyB"         },
                { L_,   "xAyB",      1,  "xBuB",     1,   1,  "xAuB"         },
                { L_,   "xAyB",      1,  "xBuB",     2,   0,  "xAyB"         },
                { L_,   "xAyB",      2,  "uAuA",     0,   0,  "xAyB"         },
                { L_,   "xAyB",      2,  "uBuA",     1,   0,  "xAyB"         },
                { L_,   "xAyB",      2,  "uBuA",     2,   0,  "xAyB"         },

                { L_,   "xA",        0,  "yAuAuA",   0,   0,  "xA"           },
                { L_,   "xA",        0,  "yAuAuA",   0,   1,  "yA"           },
                { L_,   "xA",        0,  "yAuAuA",   1,   0,  "xA"           },
                { L_,   "xA",        0,  "yAuAuA",   1,   1,  "uA"           },
                { L_,   "xA",        0,  "yAuAuA",   2,   0,  "xA"           },
                { L_,   "xA",        0,  "yAuAuA",   2,   1,  "uA"           },
                { L_,   "xA",        0,  "yAuAuA",   3,   0,  "xA"           },
                { L_,   "xA",        1,  "yAuAuA",   0,   0,  "xA"           },
                { L_,   "xA",        1,  "yAuAuA",   1,   0,  "xA"           },
                { L_,   "xA",        1,  "yAuAuA",   2,   0,  "xA"           },
                { L_,   "xA",        1,  "yAuAuA",   3,   0,  "xA"           },

                { L_,   "",          0,  "xAyAuAuA", 0,   0,  ""             },
                { L_,   "",          0,  "xAyAuAuA", 1,   0,  ""             },
                { L_,   "",          0,  "xAyAuAuA", 2,   0,  ""             },
                { L_,   "",          0,  "xAyAuAuA", 3,   0,  ""             },
                { L_,   "",          0,  "xAyAuAuA", 4,   0,  ""             },

                //line  d-list         di s-list     si   ne  expected
                //----  ------         -- ------     --   --  -------- Depth=5
                { L_,   "xAxBxCxDxE",  0, "",        0,   0,  "xAxBxCxDxE"   },
                { L_,   "xAxBxCxDxE",  1, "",        0,   0,  "xAxBxCxDxE"   },
                { L_,   "xAxBxCxDxE",  2, "",        0,   0,  "xAxBxCxDxE"   },
                { L_,   "xAxBxCxDxE",  3, "",        0,   0,  "xAxBxCxDxE"   },
                { L_,   "xAxBxCxDxE",  4, "",        0,   0,  "xAxBxCxDxE"   },
                { L_,   "xAxBxCxDxE",  5, "",        0,   0,  "xAxBxCxDxE"   },

                { L_,   "xSxTxUxV",    0, "yS",      0,   1,  "ySxTxUxV"     },
                { L_,   "xSxTxUxV",    1, "yT",      0,   1,  "xSyTxUxV"     },
                { L_,   "xSxTxUxV",    2, "yU",      0,   1,  "xSxTyUxV"     },
                { L_,   "xSxTxUxV",    3, "yV",      0,   1,  "xSxTxUyV"     },
                { L_,   "xAxBxCxD",    0, "yA",      0,   0,  "xAxBxCxD"     },
                { L_,   "xAxBxCxD",    0, "yA",      0,   1,  "yAxBxCxD"     },
                { L_,   "xAxBxCxD",    0, "yA",      1,   0,  "xAxBxCxD"     },
                { L_,   "xAxBxCxD",    1, "yB",      0,   0,  "xAxBxCxD"     },
                { L_,   "xAxBxCxD",    1, "yB",      0,   1,  "xAyBxCxD"     },
                { L_,   "xAxBxCxD",    1, "yB",      1,   0,  "xAxBxCxD"     },
                { L_,   "xAxBxCxD",    2, "yC",      0,   0,  "xAxBxCxD"     },
                { L_,   "xAxBxCxD",    2, "yC",      0,   1,  "xAxByCxD"     },
                { L_,   "xAxBxCxD",    2, "yC",      1,   0,  "xAxBxCxD"     },
                { L_,   "xAxBxCxD",    3, "yD",      0,   0,  "xAxBxCxD"     },
                { L_,   "xAxBxCxD",    3, "yD",      0,   1,  "xAxBxCyD"     },
                { L_,   "xAxBxCxD",    3, "yD",      1,   0,  "xAxBxCxD"     },
                { L_,   "xAxBxCxD",    4, "yE",      0,   0,  "xAxBxCxD"     },
                { L_,   "xAxBxCxD",    4, "yE",      1,   0,  "xAxBxCxD"     },

                //line  d-list      di s-list      si   ne  expected
                //----  ------      -- ------      --   --  -------- Depth = 5
                { L_,   "xAxBxC",   0, "yDyE",     0,   0,  "xAxBxC"       },
                { L_,   "xAxBxC",   0, "yAyE",     0,   1,  "yAxBxC"       },
                { L_,   "xAxBxC",   0, "yAyB",     0,   2,  "yAyBxC"       },
                { L_,   "xAxBxC",   0, "yDyE",     1,   0,  "xAxBxC"       },
                { L_,   "xAxBxC",   0, "yDyA",     1,   1,  "yAxBxC"       },
                { L_,   "xAxBxC",   0, "yDyE",     2,   0,  "xAxBxC"       },
                { L_,   "xAxBxC",   1, "yDyE",     0,   0,  "xAxBxC"       },
                { L_,   "xAxBxC",   1, "yByE",     0,   1,  "xAyBxC"       },
                { L_,   "xAxBxC",   1, "yByC",     0,   2,  "xAyByC"       },
                { L_,   "xAxBxC",   1, "yDyB",     1,   0,  "xAxBxC"       },
                { L_,   "xAxBxC",   1, "yDyB",     1,   1,  "xAyBxC"       },
                { L_,   "xAxBxC",   1, "yDyE",     2,   0,  "xAxBxC"       },
                { L_,   "xAxBxC",   2, "yDyE",     0,   0,  "xAxBxC"       },
                { L_,   "xAxBxC",   2, "yCyE",     0,   1,  "xAxByC"       },
                { L_,   "xAxBxC",   2, "yDyC",     1,   0,  "xAxBxC"       },
                { L_,   "xAxBxC",   2, "yDyC",     1,   1,  "xAxByC"       },
                { L_,   "xAxBxC",   2, "yDyE",     2,   0,  "xAxBxC"       },
                { L_,   "xAxBxC",   3, "yDyE",     0,   0,  "xAxBxC"       },
                { L_,   "xAxBxC",   3, "yDyE",     1,   0,  "xAxBxC"       },
                { L_,   "xAxBxC",   3, "yDyE",     2,   0,  "xAxBxC"       },

                { L_,   "xAxB",     0, "xCxDxE",   0,   0,  "xAxB"         },
                { L_,   "xAxB",     0, "yAxDxE",   0,   1,  "yAxB"         },
                { L_,   "xAxB",     0, "yAyBxE",   0,   2,  "yAyB"         },
                { L_,   "xAxB",     0, "xCxDxE",   1,   0,  "xAxB"         },
                { L_,   "xAxB",     0, "xCyAxE",   1,   1,  "yAxB"         },
                { L_,   "xAxB",     0, "xCxDxE",   2,   0,  "xAxB"         },
                { L_,   "xAxB",     0, "xCxDyA",   2,   1,  "yAxB"         },
                { L_,   "xAxB",     0, "xCxDxE",   3,   0,  "xAxB"         },
                { L_,   "xAxB",     1, "xCxDxE",   0,   0,  "xAxB"         },
                { L_,   "xAxB",     1, "yBxDxE",   0,   1,  "xAyB"         },
                { L_,   "xAxB",     1, "xCxDxE",   1,   0,  "xAxB"         },
                { L_,   "xAxB",     1, "xCyBxE",   1,   1,  "xAyB"         },
                { L_,   "xAxB",     1, "xCxDxE",   2,   0,  "xAxB"         },
                { L_,   "xAxB",     1, "xCxDyB",   2,   1,  "xAyB"         },
                { L_,   "xAxB",     1, "xCxDxE",   3,   0,  "xAxB"         },
                { L_,   "xAxB",     2, "xCxDxE",   0,   0,  "xAxB"         },
                { L_,   "xAxB",     2, "xCxDxE",   1,   0,  "xAxB"         },
                { L_,   "xAxB",     2, "xCxDxE",   2,   0,  "xAxB"         },
                { L_,   "xAxB",     2, "xCxDxE",   3,   0,  "xAxB"         },

                { L_,   "xA",       0, "xBxCxDxE", 0,   0,  "xA"           },
                { L_,   "xA",       0, "yAxCxDxE", 0,   1,  "yA"           },
                { L_,   "xA",       0, "xBxCxDxE", 1,   0,  "xA"           },
                { L_,   "xA",       0, "xByAxDxE", 1,   1,  "yA"           },
                { L_,   "xA",       0, "xBxCxDxE", 2,   0,  "xA"           },
                { L_,   "xA",       0, "xBxCyAxE", 2,   1,  "yA"           },
                { L_,   "xA",       0, "xBxCxDxE", 3,   0,  "xA"           },
                { L_,   "xA",       0, "xBxCxDyA", 3,   1,  "yA"           },
                { L_,   "xA",       0, "xBxCxDxE", 4,   0,  "xA"           },
                { L_,   "xA",       1, "xBxCxDxE", 0,   0,  "xA"           },
                { L_,   "xA",       1, "xBxCxDxE", 1,   0,  "xA"           },
                { L_,   "xA",       1, "xBxCxDxE", 2,   0,  "xA"           },
                { L_,   "xA",       1, "xBxCxDxE", 3,   0,  "xA"           },
                { L_,   "xA",       1, "xBxCxDxE", 4,   0,  "xA"           },

                { L_,   "",         0, "xAxBxCxDxE", 0, 0,  ""             },
                { L_,   "",         0, "xAxBxCxDxE", 1, 0,  ""             },
                { L_,   "",         0, "xAxBxCxDxE", 2, 0,  ""             },
                { L_,   "",         0, "xAxBxCxDxE", 3, 0,  ""             },
                { L_,   "",         0, "xAxBxCxDxE", 4, 0,  ""             },
                { L_,   "",         0, "xAxBxCxDxE", 5, 0,  ""             },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *D_SPEC = DATA[ti].d_drSpec;
                const int   DI     = DATA[ti].d_di;
                const char *S_SPEC = DATA[ti].d_slSpec;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = ((int) strlen(D_SPEC) + strlen(S_SPEC))/2;
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                List DD(g(D_SPEC));    // control for destination
                List SS(g(S_SPEC));    // control for source
                List EE(g(E_SPEC));    // control for expected value

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(LINE); P_(D_SPEC); P_(DI); P_(S_SPEC);
                                    P_(SI); P_(NE); P(E_SPEC);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(SS);
                    cout << "\t\t"; P(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                for (int option = 0; option < 3; ++option) {
                bdem_AggregateOption::AllocationStrategy optval =
                      option == 0 ? bdem_AggregateOption::BDEM_PASS_THROUGH
                    : option == 1 ? bdem_AggregateOption::BDEM_WRITE_MANY
                    : bdem_AggregateOption::BDEM_WRITE_ONCE;

                if (veryVerbose)
                    cout << "\t\toption =" << optionToString(optval);
                if (veryVerbose) cout << "\t\treplaceValues(di, sr, si, ne)"
                                      << endl;
                {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    List x(DD, optval, &testAllocator);
                    const List &X = x;
                    {
                        List s(SS, &testAllocator);  const List &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        // source non-'const'
                        replaceValues(&x, DI, s, SI, NE);
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose)
                    cout << "\t\treplaceValue(di, src, si)" << endl;
                if (1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    List x(DD, optval,
                           &testAllocator);
                    const List &X = x;
                    {
                        List s(SS, &testAllocator);  const List &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }

                        x.row()[DI].replaceValue(S.row()[SI]);

                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BSLMA_EXCEPTION_TEST
                }
                }
            }
        }
      }

DEFINE_TEST_CASE(30) {
        //---------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout <<
            "\nTesting x.replaceValues(di, sr, si, ne) et al. (aliasing)"
            << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_xSpec;    // initial list (= srcList)
                int         d_di;       // index at which to insert into x
                int         d_si;       // index at which to insert from x
                int         d_ne;       // number of elements to insert
                const char *d_expSpec;  // expected list value
            } DATA[] = {
                //line  x-list        di  si  ne  expected
                //----  ------        --  --  --  --------   Depth = 0
                { L_,   "",           0,  0,  0,  ""      },

                //line  x-list        di  si  ne  expected
                //----  ------        --  --  --  --------   Depth = 1
                { L_,   "xA",         0,  0,  0,  "xA"     },
                { L_,   "xA",         0,  0,  1,  "xA"     },
                { L_,   "xB",         0,  0,  1,  "xB"     },
                { L_,   "xC",         0,  0,  1,  "xC"     },
                { L_,   "xD",         0,  0,  1,  "xD"     },
                { L_,   "xE",         0,  0,  1,  "xE"     },
                { L_,   "xF",         0,  0,  1,  "xF"     },
                { L_,   "xG",         0,  0,  1,  "xG"     },
                { L_,   "xH",         0,  0,  1,  "xH"     },
                { L_,   "xI",         0,  0,  1,  "xI"     },
                { L_,   "xJ",         0,  0,  1,  "xJ"     },
                { L_,   "xK",         0,  0,  1,  "xK"     },
                { L_,   "xL",         0,  0,  1,  "xL"     },
                { L_,   "xM",         0,  0,  1,  "xM"     },
                { L_,   "xN",         0,  0,  1,  "xN"     },
                { L_,   "xO",         0,  0,  1,  "xO"     },
                { L_,   "xP",         0,  0,  1,  "xP"     },
                { L_,   "xQ",         0,  0,  1,  "xQ"     },
                { L_,   "xR",         0,  0,  1,  "xR"     },
                { L_,   "xS",         0,  0,  1,  "xS"     },
                { L_,   "xT",         0,  0,  1,  "xT"     },
                { L_,   "xU",         0,  0,  1,  "xU"     },
                { L_,   "xV",         0,  0,  1,  "xV"     },
                { L_,   "xA",         0,  1,  0,  "xA"     },

                { L_,   "xA",         1,  0,  0,  "xA"     },
                { L_,   "xA",         1,  1,  0,  "xA"     },

                //line  x-list        di  si  ne  expected
                //----  ------        --  --  --  --------   Depth = 2
                { L_,   "xAyA",       0,  0,  0,  "xAyA"    },
                { L_,   "xAyA",       0,  0,  1,  "xAyA"    },
                { L_,   "xAyA",       0,  0,  2,  "xAyA"    },
                { L_,   "xAyA",       0,  1,  0,  "xAyA"    },
                { L_,   "xAyA",       0,  1,  1,  "yAyA"    },
                { L_,   "xAyA",       0,  2,  0,  "xAyA"    },

                { L_,   "xAyA",       1,  0,  0,  "xAyA"    },
                { L_,   "xAyA",       1,  0,  1,  "xAxA"    },
                { L_,   "xAyA",       1,  1,  0,  "xAyA"    },
                { L_,   "xAyA",       1,  1,  1,  "xAyA"    },
                { L_,   "xAyA",       1,  2,  0,  "xAyA"    },

                { L_,   "xAyA",       2,  0,  0,  "xAyA"    },
                { L_,   "xAyA",       2,  1,  0,  "xAyA"    },
                { L_,   "xAyA",       2,  2,  0,  "xAyA"    },
                { L_,   "xUyU",       1,  0,  1,  "xUxU"    },
                { L_,   "xUyU",       0,  1,  1,  "yUyU"    },
                { L_,   "xTyT",       1,  0,  1,  "xTxT"    },
                { L_,   "xTyT",       0,  1,  1,  "yTyT"    },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *X_SPEC = DATA[ti].d_xSpec;
                const int   DI     = DATA[ti].d_di;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = strlen(X_SPEC)/2;
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                List DD(g(X_SPEC));     // control for destination
                List EE(g(E_SPEC));     // control for expected value

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(LINE); P_(X_SPEC); P_(DI);
                                              P_(SI); P_(NE); P(E_SPEC);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                // CONTROL: ensure same table result as non-alias case.
                {
                    List x(DD, &testAllocator);  const List &X = x;
                    List x2(DD, &testAllocator); // control
                    const List &X2 = x2;
                    replaceValues(&x2, DI, X, SI, NE);
                    LOOP_ASSERT(LINE, EE == X2);
                }

                if (veryVerbose)
                    cout << "\t\treplaceValues(di, sr, si, ne)" << endl;
                {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    List x(DD, &testAllocator);  const List &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    replaceValues(&x, DI, X, SI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose)
                    cout << "\t\treplaceValue(di, sr, si)" << endl;
                if (1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    List x(DD, &testAllocator);  const List &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }

                    x.row()[DI].replaceValue(X.row()[SI]);

                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }
      }

DEFINE_TEST_CASE(29) {
        //---------------------------------------------------------------------
        // TEST SOME MANIPULATIONS OF ROWS
        //
        // Concerns:
        //   That certain manipulations of rows work properly, particularly
        //   when aliasing.
        //---------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout <<
            "\nTesting x.replaceValues(di, sr, si, ne) et al. (overlapping)"
            << endl;
        {
            // Destination contains source list
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_xSpec;    // initial list 1 (= srcList)
                const char *d_ySpec;    // initial list 2
                int         d_yi;       // index at which to insert y into x
                int         d_di;       // index at which to insert into x
                int         d_si;       // index at which to insert from x
                int         d_ne;       // number of elements to insert
            } DATA[] = {
                //line  x-list     y-list     yi   di   si  ne
                //----  ------     ------     --   --   --  --
                 { L_,   "",        "",        0,   0,   0,  0,      },

                //line  x-list     y-list     yi   di   si  ne
                //----  ------     ------     --   --   --  --
                 { L_,   "xA",      "",        0,   0,   0,  0,      },
                 { L_,   "xU",      "",        0,   0,   0,  0,      },
                 { L_,   "",        "xA",      0,   0,   0,  0,      },
                 { L_,   "",        "xU",      0,   0,   0,  0,      },
                 { L_,   "",        "xU",      0,   0,   0,  1,      },

                //line  x-list     y-list     yi   di   si  ne
                //----  ------     ------     --   --   --  --
                 { L_,   "xA",      "xU",      0,   0,   0,  1,      },
                 { L_,   "xA",      "yA",      0,   1,   0,  1,      },
                 { L_,   "xA",      "xU",      0,   0,   0,  1,      },
                 { L_,   "xA",      "yA",      0,   1,   0,  1,      },
                 { L_,   "xU",      "yU",      0,   0,   0,  1,      },
                 { L_,   "xU",      "yU",      0,   1,   0,  1,      },
                 { L_,   "xU",      "yU",      0,   0,   0,  1,      },
                 { L_,   "xU",      "yU",      0,   1,   0,  1,      },
                 { L_,   "xA",      "yA",      1,   0,   0,  1,      },
                 { L_,   "xA",      "xU",      1,   1,   0,  1,      },
                 { L_,   "xA",      "yA",      1,   0,   0,  1,      },
                 { L_,   "xA",      "xU",      1,   1,   0,  1,      },
                 { L_,   "xU",      "yU",      1,   0,   0,  1,      },
                 { L_,   "xU",      "yU",      1,   1,   0,  1,      },
                 { L_,   "xU",      "yU",      1,   0,   0,  1,      },
                 { L_,   "xU",      "yU",      1,   1,   0,  1,      },

                 //line  x-list     y-list     yi   di   si  ne
                 //----  ------     ------     --   --   --  --
                 { L_,   "xAxBxC",  "xU",      0,   0,   0,  1,      },
                 { L_,   "xAxBxC",  "xUyA",    0,   0,   0,  2,      },
                 { L_,   "xAxBxC",  "yAxU",    0,   0,   1,  1,      },
                 { L_,   "xAxBxC",  "yA",      0,   1,   0,  1,      },
                 { L_,   "xAxBxC",  "yAyB",    0,   1,   0,  2,      },
                 { L_,   "xAxBxC",  "xTyA",    0,   1,   1,  1,      },
                 { L_,   "xAxBxC",  "yB",      0,   2,   0,  1,      },
                 { L_,   "xAxBxC",  "yByC",    0,   2,   0,  2,      },
                 { L_,   "xAxBxC",  "xTyB",    0,   2,   1,  1,      },
                 { L_,   "xAxBxC",  "yC",      0,   3,   0,  1,      },
                 { L_,   "xAxBxC",  "xTyC",    0,   3,   1,  1,      },
                 { L_,   "xAxBxC",  "yA",      1,   0,   0,  1,      },
                 { L_,   "xAxBxC",  "yAxU",    1,   0,   0,  2,      },
                 { L_,   "xAxBxC",  "xTyA",    1,   0,   1,  1,      },
                 { L_,   "xAxBxC",  "xU",      1,   1,   0,  1,      },
                 { L_,   "xAxBxC",  "xUyB",    1,   1,   0,  2,      },
                 { L_,   "xAxBxC",  "xTxU",    1,   1,   1,  1,      },
                 { L_,   "xAxBxC",  "yB",      1,   2,   0,  1,      },
                 { L_,   "xAxBxC",  "yByC",    1,   2,   0,  2,      },
                 { L_,   "xAxBxC",  "xTyB",    1,   2,   1,  1,      },
                 { L_,   "xAxBxC",  "yC",      1,   3,   0,  1,      },
                 { L_,   "xAxBxC",  "xTyC",    1,   3,   1,  1,      },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *X_SPEC = DATA[ti].d_xSpec;
                const char *Y_SPEC = DATA[ti].d_ySpec;
                const int   YI     = DATA[ti].d_yi;
                const int   DI     = DATA[ti].d_di;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;

                const int   DEPTH  = (strlen(X_SPEC) + strlen(Y_SPEC))/2;
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                List DD(g(X_SPEC));       // control for destination
                List YY(g(Y_SPEC));       // list inserted into DD
                DD.insertList(YI, YY);

                // The insert function has already been tested for the
                // non overlap case.  Generate the expected result using
                // the same data but no overlapping.

                List EE(DD);              // copy of destination.
                List SS(DD);              // copy for source.
                LOOP_ASSERT(X_SPEC, EE.length() > YI);
                replaceValues(&EE, DI, SS.theList(YI), SI, NE);

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(LINE); P_(X_SPEC); P_(Y_SPEC); P_(YI);
                                    P_(DI); P_(SI); P_(NE);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                if (veryVerbose)
                    cout << "\t\treplaceValues(di, sr, si, ne)" << endl;

                BEGIN_BSLMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    List x(DD, &testAllocator);  const List &X = x;
                    testAllocator.setAllocationLimit(AL);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    replaceValues(&x, DI, X.theList(YI), SI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                } END_BSLMA_EXCEPTION_TEST

                if (veryVerbose)
                    cout << "\t\tassignment within row" << endl;
                if (1 == NE) {
                    BEGIN_BSLMA_EXCEPTION_TEST {
                        const int AL = testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);
                        List x(DD, &testAllocator);  const List &X = x;
                        testAllocator.setAllocationLimit(AL);
                        int type = X.row()[DI].type();
                        if (type == X.theList(YI).row()[SI].type() &&
                                                                   type < 22) {
                            if (veryVerbose) {
                                cout << "\t\t\tBEFORE ASSIGN: ";
                                P_(type); P(X);
                            }
                            const bdem_ElemRef& e = x[DI];
//                             assignElemRef(&e, X.theList(YI).row()[SI]);
                            if (veryVerbose) {
                                cout << "\t\t\t AFTER ASSIGN: ";
                                P_(type); P(X);
                            }
//                             LOOP_ASSERT(LINE, EE == X);
                        }
                    } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose)
                    cout << "\t\treplaceValue(di, sr, si)" << endl;
                if (1 == NE) {
                    BEGIN_BSLMA_EXCEPTION_TEST {
                        const int AL = testAllocator.allocationLimit();
                        testAllocator.setAllocationLimit(-1);
                        List x(DD, &testAllocator);  const List &X = x;
                        testAllocator.setAllocationLimit(AL);
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
//                         x.row()[DI].replaceValue(X.theList(YI).row()[SI]);

                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
//                         LOOP_ASSERT(LINE, EE == X);
                    } END_BSLMA_EXCEPTION_TEST
                }
            }
        }
      }

DEFINE_TEST_CASE(28) {

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout <<
            "\nTesting x.replaceValues(di, sr, si, ne) et al. (overlapping)"
            << endl;
        {
            // Source contains destination list
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_xSpec;    // initial list 1 (= srcList)
                const char *d_ySpec;    // initial list 2
                int         d_yi;       // index at which to insert y into x
                int         d_di;       // index at which to insert into x
                int         d_si;       // index at which to insert from x
                int         d_ne;       // number of elements to insert
            } DATA[] = {
                //line  x-list    y-list   yi   di   si  ne
                //----  ------    ------   --   --   --  --
                { L_,   "",       "",      0,   0,   0,  0,      },

                //line  x-list    y-list   yi   di   si  ne
                //----  ------    ------   --   --   --  --
                { L_,   "",       "xA",    0,   0,   0,  0,      },
                { L_,   "",       "xU",    0,   0,   0,  0,      },
                { L_,   "",       "xT",    0,   0,   0,  0,      },
                { L_,   "",       "xU",    0,   0,   0,  1,      },
                { L_,   "xA",     "",      0,   0,   0,  0,      },
                { L_,   "xU",     "",      0,   0,   0,  0,      },
                { L_,   "xT",     "",      0,   0,   0,  0,      },

                //line  x-list    y-list   yi   di   si  ne
                //----  ------    ------   --   --   --  --
                { L_,   "",       "xUxT",  0,   0,   0,  1,      },
                { L_,   "",       "xTxU",  0,   1,   0,  1,      },
                { L_,   "xA",     "xU",    0,   0,   0,  1,      },
                { L_,   "xT",     "xU",    0,   0,   0,  1,      },
                { L_,   "xU",     "yU",    0,   0,   0,  1,      },
                { L_,   "xA",     "yA",    1,   0,   0,  1,      },
                { L_,   "xT",     "yT",    1,   0,   0,  1,      },
                { L_,   "xU",     "yU",    1,   0,   0,  1,      },

                //line  x-list    y-list   yi   di   si  ne
                //----  ------    ------   --   --   --  --
                { L_,   "xA",     "xUyB",  0,   0,   0,  1,      },
                { L_,   "xU",     "xUyB",  0,   0,   0,  1,      },
                { L_,   "xU",     "yUyB",  0,   0,   0,  1,      },
                { L_,   "xA",     "yAxT",  1,   0,   0,  1,      },
                { L_,   "xA",     "yAxU",  1,   0,   0,  1,      },
                { L_,   "xU",     "xUyB",  1,   0,   0,  1,      },
                { L_,   "xU",     "yUyB",  1,   0,   0,  1,      },
                { L_,   "xA",     "xTxU",  0,   1,   0,  1,      },
                { L_,   "xA",     "xUxU",  0,   1,   0,  1,      },
                { L_,   "xU",     "xTyU",  0,   1,   0,  1,      },
                { L_,   "xU",     "yUyU",  0,   1,   0,  1,      },
                { L_,   "xA",     "xTyA",  1,   1,   0,  1,      },
                { L_,   "xA",     "xUyA",  1,   1,   0,  1,      },
                { L_,   "xU",     "xTyU",  1,   1,   0,  1,      },
                { L_,   "xU",     "yUyU",  1,   1,   0,  1,      },
                { L_,   "xA",     "yAyB",  0,   0,   1,  1,      },
                { L_,   "xU",     "yUyB",  0,   0,   1,  1,      },
                { L_,   "xA",     "xUyB",  1,   0,   1,  1,      },
                { L_,   "xU",     "xUyB",  1,   0,   1,  1,      },
                { L_,   "xA",     "xUyA",  0,   0,   0,  2,      },
                { L_,   "xU",     "yUxU",  0,   0,   0,  2,      },
                { L_,   "xA",     "yAxU",  1,   0,   0,  2,      },
                { L_,   "xU",     "yUxU",  1,   0,   0,  2,      },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *X_SPEC = DATA[ti].d_xSpec;
                const char *Y_SPEC = DATA[ti].d_ySpec;
                const int   YI     = DATA[ti].d_yi;
                const int   DI     = DATA[ti].d_di;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;

                const int   DEPTH  = (strlen(X_SPEC) + strlen(Y_SPEC))/2;
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                // Insert one list into destination list to create test data.

                List DD(g(X_SPEC));        // control for destination
                List YY(g(Y_SPEC));        // list inserted into DD
                DD.insertList(YI, YY);

                // The insert function has already been tested for the
                // non overlap case.  Generate the expected result using
                // the same data but no overlapping.

                List EE(DD);               // copy of destination.
                List SS(DD);               // copy for source.
                LOOP_ASSERT(X_SPEC, EE.length() > YI);
                replaceValues(&EE.theModifiableList(YI), DI, SS, SI, NE);

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(LINE); P_(X_SPEC); P_(Y_SPEC); P_(YI);
                                    P_(DI); P_(SI); P_(NE);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                if (veryVerbose)
                    cout << "\t\treplaceValues(di, sr, si, ne)" << endl;
                {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    List x(DD, &testAllocator);  const List &X = x;
                    testAllocator.setAllocationLimit(AL);
                    if (veryVerbose) {
                        cout << "\t\t\tBEFORE: "; P(YI) P(DI) P(SI) P(NE) }
                    replaceValues(&x.theModifiableList(YI), DI, X, SI, NE);
                    if (veryVerbose) {
                        cout << "\t\t\t AFTER: "; P(YI) P(DI) P(SI) P(NE) }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose)
                    cout << "\t\treplaceValue(di, sr, si)" << endl;
                if (1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    List x(DD, &testAllocator);  const List &X = x;
                    testAllocator.setAllocationLimit(AL);
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.theModifiableList(YI).row()[DI].
                                                     replaceValue(X.row()[SI]);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }
      }

DEFINE_TEST_CASE(27) {
        // --------------------------------------------------------------------
        // TESTING 'bdem_Table' EQUALITY OPERATORS
        // Concerns:
        //   a) Tables with different dimensions are unequal.
        //   b) Tables with the same number of columns and rows with different
        //      element types but identical values (e.g., Integer 0 and Short
        //      0) are unequal.
        //   c) Tables with the same element types but different element values
        //      are unequal.
        //   d) Tables with the same element types and the same values at each
        //      index position, but with different internal representations,
        //      are equal.
        //   f) Empty Tables are equal.
        //   g) Equality is commutative.
        //
        // Plan:
        //   Test with values from all 22 types.
        //   Create identical tables with different internal representations.
        //
        // Testing:
        //   bool operator==(const bdem_Table& lhs, const bdem_Table& rhs);
        //   bool operator!=(const bdem_Table& lhs, const bdem_Table& rhs);
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl
                          << "Testing Table Equality Operators" << endl
                          << "================================" << endl;

        if (verbose) cout <<
            "\nCompare each pair of similar values (u, v) in S X S." << endl;
        {
            static const char *SPECS[] = {
                "A;",          "B;",           "C;",
                "",            ";",
                "A;x;",        "B;x;",         "C;x;",
                "A;u;",        "B;u;",
                "ABCDEFGHIJKLMNOPQRSTUV;xxxxxxxxxxxxxxxxxxxxxx;",
                 ";;",
                "A;x;y;",      "B;x;y;",       "C;x;y;",
                "ABCDE;xxxxx;yxxxx;",          "DEFGHI;xxxxxx;yyyyyy;",
                "FGHITUV;xyxyxyx;yxyxyxy;",
                "AB;xy;xy;xy;yx;",
                "AB;xy;xy;xy;yy;",
            0}; // Null string required as last element.

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const U_SPEC = SPECS[ti];
                const int curLen = (int)getNumRows(U_SPEC);

                Table mU(&testAllocator); hh(&mU, U_SPEC); const Table& U = mU;
                LOOP_ASSERT(ti, curLen == U.numRows()); // same lengths

                if (curLen != oldLen) {
                    if (verbose) cout << "\tUsing lhs objects of length "
                                      << curLen << '.' << endl;
                    LOOP_ASSERT(U_SPEC, oldLen <= curLen); // non-decreasing
                    oldLen = curLen;
                }

                if (veryVerbose) { P_(ti); P_(U_SPEC); P(U); }

                for (int tj = 0; SPECS[tj]; ++tj) {
                    const char *const V_SPEC = SPECS[tj];
                    Table mV(&testAllocator); hh(&mV, V_SPEC);
                    const Table& V = mV;

                    if (veryVerbose) {
                        cout << "  "; P_(tj); P_(V_SPEC); P(V);
                    }
                    const bool isSame = ti == tj;
                    LOOP2_ASSERT(ti, tj,  isSame == (U == V));
                    LOOP2_ASSERT(ti, tj, !isSame == (U != V));
                }
            }
        }
      }

DEFINE_TEST_CASE(26) {
        // --------------------------------------------------------------------
        // TESTING 'bdem_Table' makeValueNull MANIPULATORS
        // Concerns: BUG
        //
        // Plan: BUG
        //
        // Testing:
        //  void makeValueNull(int ri, int ci);
        //  void makeColumnNull(int index);
        //  void makeRowsNull(int si, int rc);
        //  void makeAllNull();
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose)
            cout << endl
                 << "Testing bdem_Table makeValueNull Manipulators" << endl
                 << "=============================================" << endl;

        if (verbose) cout << "\ttesting makeRowsNull() et al.\n" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec;     // source table
                int         d_si;       // row at which to set
                int         d_ne;       // number of rows to set
                const char *d_espec;    // expected result
            } DATA[] = {
                //line    d-table               si   ne  e-table
                //----    -------               --   --  -------      Depth = 0
                { L_,     "",                   0,   0,  ""            },
                { L_,     "a",                  0,   0,  "a"           },
                { L_,     "a",                  0,   1,  "n"           },
                { L_,     "ab",                 0,   0,  "ab"          },
                { L_,     "ab",                 0,   1,  "nb"          },
                { L_,     "ab",                 1,   1,  "an"          },
                { L_,     "ab",                 0,   2,  "nn"          },
                { L_,     "abc",                0,   0,  "abc"         },
                { L_,     "abc",                0,   1,  "nbc"         },
                { L_,     "abc",                0,   2,  "nnc"         },
                { L_,     "abc",                0,   3,  "nnn"         },
                { L_,     "abc",                1,   0,  "abc"         },
                { L_,     "abc",                1,   1,  "anc"         },
                { L_,     "abc",                1,   2,  "ann"         },
                { L_,     "abc",                2,   0,  "abc"         },
                { L_,     "abc",                2,   1,  "abn"         },
                { L_,     "abcd",               0,   0,  "abcd"        },
                { L_,     "abcd",               0,   1,  "nbcd"        },
                { L_,     "abcd",               0,   2,  "nncd"        },
                { L_,     "abcd",               0,   3,  "nnnd"        },
                { L_,     "abcd",               0,   4,  "nnnn"        },
                { L_,     "abcd",               1,   0,  "abcd"        },
                { L_,     "abcd",               1,   1,  "ancd"        },
                { L_,     "abcd",               1,   2,  "annd"        },
                { L_,     "abcd",               1,   3,  "annn"        },
                { L_,     "abcd",               2,   0,  "abcd"        },
                { L_,     "abcd",               2,   1,  "abnd"        },
                { L_,     "abcd",               2,   2,  "abnn"        },
                { L_,     "abcd",               3,   0,  "abcd"        },
                { L_,     "abcd",               3,   1,  "abcn"        },
            };

            const int NUM_DATA  = sizeof DATA / sizeof *DATA;
            const char *COLSPEC = "UCTE";

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *SPEC   = DATA[ti].d_spec;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_espec;

                const int curLen = (int)strlen(SPEC);
                if (curLen != oldLen) {
                    if (verbose) cout << "\tUsing objects of length "
                                      << curLen << '.' << endl;
                    LOOP_ASSERT(SPEC, oldLen <= curLen); // non-decreasing
                    oldLen = curLen;
                }

                Table NN(hx(COLSPEC, E_SPEC));    // control for expected value
                Table EE(hx(COLSPEC, E_SPEC));    // control for expected value

                const int EXP_LEN = bsl::strlen(E_SPEC);

                for (int i = 0; i < EXP_LEN; ++i) {
                    if ('n' == E_SPEC[i]) {
                        NN.makeRowsNull(i, 1);
                    }
                }

                {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Table mU(&testAllocator);
                    testAllocator.setAllocationLimit(AL);
                    hhx(&mU, COLSPEC, SPEC);

                    if (veryVerbose) {
                        P_(SPEC); P_(E_SPEC); P_(SI); P(NE);
                        PR(mU);
                    }

                    mU.makeRowsNull(SI, NE);

                    LOOP_ASSERT(LINE, mU == NN);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if ((int)strlen(SPEC) == NE && SI == 0) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Table mU(&testAllocator);
                    hhx(&mU, COLSPEC, SPEC);

                    if (veryVerbose) {
                        P_(SPEC); P_(E_SPEC); P_(SI); P(NE);
                        PR(mU);
                    }

                    mU.makeAllNull();

                    LOOP3_ASSERT(LINE, mU, NN, mU == NN);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (NE == 1) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Table mU(&testAllocator);
                    hhx(&mU, COLSPEC, SPEC);
                    if (veryVerbose) {
                        P_(SPEC); P_(E_SPEC); P_(SI); P(NE);
                        PR(mU);
                    }

                    mU.theModifiableRow(SI).makeAllNull();

                    LOOP3_ASSERT(LINE, mU, EE, mU == EE);
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }

        if (verbose) cout << "\ttesting makeColumnNull() et al.\n" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec;     // source table
                int         d_si;       // column at which to set
                int         d_ne;       // number of columns to set
                const char *d_espec;    // expected result
            } DATA[] = {
                //line    d-table               si   ne  e-table
                //----    -------               --   --  -------      Depth = 0
                { L_,     "",                   0,   0,  ""             },
                { L_,     "A;",                 0,   0,  "A;"           },
                { L_,     "A;",                 0,   1,  "A;"           },
                { L_,     "AB;",                0,   0,  "AB;"          },
                { L_,     "AB;",                0,   1,  "AB;"          },
                { L_,     "AB;",                0,   2,  "AB;"          },
                { L_,     "ABC;",               0,   0,  "ABC;"         },
                { L_,     "A;x;",               0,   0,  "A;x;"         },
                { L_,     "A;x;",               0,   1,  "A;n;"         },
                { L_,     "AB;xy;",             0,   0,  "AB;xy;"       },
                { L_,     "AB;xy;",             0,   1,  "AB;ny;"       },
                { L_,     "AB;xy;",             0,   2,  "AB;nn;"       },
                { L_,     "AB;xy;",             1,   0,  "AB;xy;"       },
                { L_,     "AB;xy;",             1,   1,  "AB;xn;"       },
                { L_,     "ABC;xyx;",           0,   0,  "ABC;xyx;"     },
                { L_,     "ABC;xyx;",           0,   1,  "ABC;nyx;"     },
                { L_,     "ABC;xyx;",           0,   2,  "ABC;nnx;"     },
                { L_,     "ABC;xyx;",           0,   3,  "ABC;nnn;"     },
                { L_,     "ABCD;xyxx;",         0,   0,  "ABCD;xyxx;"   },
                { L_,     "ABCD;xyxx;",         0,   1,  "ABCD;nyxx;"   },
                { L_,     "ABCD;xyxx;",         0,   2,  "ABCD;nnxx;"   },
                { L_,     "ABCD;xyxx;",         0,   3,  "ABCD;nnnx;"   },
                { L_,     "ABCD;xyxx;",         0,   4,  "ABCD;nnnn;"   },
                { L_,     "ABCD;xyxx;",         1,   2,  "ABCD;xnnx;"   },
                { L_,     "ABCD;xyxx;",         1,   3,  "ABCD;xnnn;"   },
                { L_,     "A;x;y;",             0,   0,  "A;x;y;"       },
                { L_,     "A;x;y;",             0,   1,  "A;n;n;"       },
                { L_,     "AB;xy;yx;",          0,   0,  "AB;xy;yx;"    },
                { L_,     "AB;xy;yx;",          0,   1,  "AB;ny;nx;"    },
                { L_,     "AB;xy;yx;",          0,   2,  "AB;nn;nn;"    },
                { L_,     "AB;xy;yx;",          1,   0,  "AB;xy;yx;"    },
                { L_,     "AB;xy;yx;",          1,   1,  "AB;xn;yn;"    },
                { L_,     "AB;xy;yx;yy;",       0,   0,  "AB;xy;yx;yy;" },
                { L_,     "AB;xy;yx;yy;",       0,   1,  "AB;ny;nx;ny;" },
                { L_,     "AB;xy;yx;yy;",       0,   2,  "AB;nn;nn;nn;" },
                { L_,     "AB;xy;yx;yy;",       1,   1,  "AB;xn;yn;yn;" },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *SPEC   = DATA[ti].d_spec;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_espec;

                const int curLen = (int)getNumRows(SPEC);
                if (curLen != oldLen) {
                    if (verbose) cout << "\tUsing objects of length "
                                      << curLen << '.' << endl;
                    LOOP_ASSERT(SPEC, oldLen <= curLen); // non-decreasing
                    oldLen = curLen;
                }

                Table EE(h(E_SPEC));     // control for expected value

                {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Table mU(&testAllocator);
                    hh(&mU, SPEC);

                    if (veryVerbose) {
                        P_(SPEC); P_(E_SPEC); P_(SI); P(NE);
                        PR(mU);
                    }

                    for (int sii = 0; sii < NE; ++sii) {
                        mU.makeColumnNull(SI + sii);
                    }

                    LOOP_ASSERT(LINE, mU == EE);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (NE == 1) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Table mU(&testAllocator);
                    hh(&mU, SPEC);

                    if (veryVerbose) {
                        P_(SPEC); P_(E_SPEC); P_(SI); P(NE);
                        PR(mU);
                    }

                    mU.makeColumnNull(SI);

                    LOOP_ASSERT(LINE, mU == EE);
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }

        if (verbose) cout << "\ttesting makeValueNull()\n" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec;     // source table
                int         d_ri;       // row at which to set
                int         d_ci;       // columns at which to set
                const char *d_espec;    // expected result
            } DATA[] = {
                //line    d-table               ri   ci  e-table
                //----    -------               --   --  -------      Depth = 0
                { L_,     "A;x;",               0,   0,  "A;n;"         },
                { L_,     "AB;xy;",             0,   0,  "AB;ny;"       },
                { L_,     "AB;xy;",             0,   1,  "AB;xn;"       },
                { L_,     "ABC;xyx;",           0,   0,  "ABC;nyx;"     },
                { L_,     "ABC;xyx;",           0,   1,  "ABC;xnx;"     },
                { L_,     "ABC;xyx;",           0,   2,  "ABC;xyn;"     },
                { L_,     "A;x;y;",             0,   0,  "A;n;y;"       },
                { L_,     "A;x;y;",             1,   0,  "A;x;n;"       },
                { L_,     "AB;xy;yx;",          0,   0,  "AB;ny;yx;"    },
                { L_,     "AB;xy;yx;",          0,   1,  "AB;xn;yx;"    },
                { L_,     "AB;xy;yx;",          1,   0,  "AB;xy;nx;"    },
                { L_,     "AB;xy;yx;",          1,   1,  "AB;xy;yn;"    },
                { L_,     "AB;xy;yx;yy;",       0,   0,  "AB;ny;yx;yy;" },
                { L_,     "AB;xy;yx;yy;",       0,   1,  "AB;xn;yx;yy;" },
                { L_,     "AB;xy;yx;yy;",       1,   0,  "AB;xy;nx;yy;" },
                { L_,     "AB;xy;yx;yy;",       1,   1,  "AB;xy;yn;yy;" },
                { L_,     "AB;xy;yx;yy;",       2,   0,  "AB;xy;yx;ny;" },
                { L_,     "AB;xy;yx;yy;",       2,   1,  "AB;xy;yx;yn;" },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *SPEC   = DATA[ti].d_spec;
                const int   RI     = DATA[ti].d_ri;
                const int   CI     = DATA[ti].d_ci;
                const char *E_SPEC = DATA[ti].d_espec;

                const int curLen = (int)getNumRows(SPEC);
                if (curLen != oldLen) {
                    if (verbose) cout << "\tUsing objects of length "
                                      << curLen << '.' << endl;
                    LOOP_ASSERT(SPEC, oldLen <= curLen); // non-decreasing
                    oldLen = curLen;
                }

                Table EE(h(E_SPEC));     // control for expected value

                {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Table mU(&testAllocator);
                    hh(&mU, SPEC);

                    if (veryVerbose) {
                        P_(SPEC); P_(E_SPEC); P_(RI); P(CI);
                        PR(mU);
                    }

                    mU.theModifiableRow(RI)[CI].makeNull();

                    LOOP3_ASSERT(LINE, mU, EE, mU == EE);
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }
      }

DEFINE_TEST_CASE(25) {
        // --------------------------------------------------------------------
        // TESTING 'bdem_Table' APPENDNULLROW, INSERTNULLROW METHODS
        // Concerns:
        //   For the 'append' and 'insert' methods, the following properties
        //   must hold:
        //     1. The function is exception neutral (w.r.t. allocation).
        //     2. The function preserves object invariants.
        //     3. The function is independent of internal representation.
        //
        // Plan:
        //   * Enumerate Tables containing from 0 to 5 Rows.
        //   * Insert from 0 to 5 unset Rows in each of the various positions
        //     in the Tables.
        //   * Similarly append from 0 to 5 unset Rows to the Tables.
        //   * Verify that the Row& returned by 'appendNullRow' and
        //     'insertNullRow' indeed refers to the new Row.
        //
        // Testing:
        //   bdem_Row& appendNullRow();
        //   void appendNullRows(int ne);
        //
        //   bdem_Row& insertNullRow(int di);
        //   void insertNullRows(int di, int ne);
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl
            << "Testing 'appendNullRow', 'insertNullRow'" << endl
            << "========================================" << endl;

        if (verbose) cout <<
            "\nTesting x.insertNullRows(di, ne) et al. " << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_dtSpec;   // initial (destination) table
                int         d_di;       // index at which to insert into da
                int         d_ne;       // number of elements to insert
                const char *d_expSpec;  // expected table value
            } DATA[] = {
                //line d-table              di  ne              expected
                //---- -------              --  --              --------
                { L_,  "",                  0,  0,              ""         },
                { L_,  "",                  0,  1,              "n"        },
                { L_,  "",                  0,  2,              "nn"       },
                { L_,  "",                  0,  3,              "nnn"      },
                { L_,  "",                  0,  4,              "nnnn"     },
                { L_,  "",                  0,  5,              "nnnnn"    },

                //line d-table              di  ne              expected
                //---- -------              --  --              --------
                { L_,  "a",                 0,  0,              "a"        },
                { L_,  "a",                 0,  1,              "na"       },
                { L_,  "a",                 0,  2,              "nna"      },
                { L_,  "a",                 0,  3,              "nnna"     },
                { L_,  "a",                 1,  0,              "a"        },
                { L_,  "a",                 1,  1,              "an"       },
                { L_,  "a",                 1,  2,              "ann"      },
                { L_,  "a",                 1,  3,              "annn"     },

                //line d-table              di  ne              expected
                //---- -------              --  --              --------
                { L_,  "ab",                0,  0,              "ab"       },
                { L_,  "ab",                0,  1,              "nab"      },
                { L_,  "ab",                0,  2,              "nnab"     },
                { L_,  "ab",                0,  3,              "nnnab"    },
                { L_,  "ab",                1,  0,              "ab"       },
                { L_,  "ab",                1,  1,              "anb"      },
                { L_,  "ab",                1,  2,              "annb"     },
                { L_,  "ab",                1,  3,              "annnb"    },
                { L_,  "ab",                2,  0,              "ab"       },
                { L_,  "ab",                2,  1,              "abn"      },
                { L_,  "ab",                2,  2,              "abnn"     },
                { L_,  "ab",                2,  3,              "abnnn"    },

                //line d-table              di  ne              expected
                //---- -------              --  --              --------
                { L_,  "abc",               0,  0,              "abc"      },
                { L_,  "abc",               0,  1,              "nabc"     },
                { L_,  "abc",               0,  2,              "nnabc"    },
                { L_,  "abc",               1,  0,              "abc"      },
                { L_,  "abc",               1,  1,              "anbc"     },
                { L_,  "abc",               1,  2,              "annbc"    },
                { L_,  "abc",               2,  0,              "abc"      },
                { L_,  "abc",               2,  1,              "abnc"     },
                { L_,  "abc",               2,  2,              "abnnc"    },
                { L_,  "abc",               3,  0,              "abc"      },
                { L_,  "abc",               3,  1,              "abcn"     },
                { L_,  "abc",               3,  2,              "abcnn"    },

                //line d-table              di  ne              expected
                //---- -------              --  --              --------
                { L_,  "abcd",              0,  0,              "abcd"     },
                { L_,  "abcd",              0,  1,              "nabcd"    },
                { L_,  "abcd",              1,  0,              "abcd"     },
                { L_,  "abcd",              1,  1,              "anbcd"    },
                { L_,  "abcd",              2,  0,              "abcd"     },
                { L_,  "abcd",              2,  1,              "abncd"    },
                { L_,  "abcd",              3,  0,              "abcd"     },
                { L_,  "abcd",              3,  1,              "abcnd"    },
                { L_,  "abcd",              4,  0,              "abcd"     },
                { L_,  "abcd",              4,  1,              "abcdn"    },

                //line d-table              di  ne              expected
                //---- -------              --  --              --------
                { L_,  "abcde",             0,  0,              "abcde"    },
                { L_,  "abcde",             1,  0,              "abcde"    },
                { L_,  "abcde",             2,  0,              "abcde"    },
                { L_,  "abcde",             3,  0,              "abcde"    },
                { L_,  "abcde",             4,  0,              "abcde"    },
                { L_,  "abcde",             5,  0,              "abcde"    },
            };

            const int NUM_DATA  = sizeof DATA / sizeof *DATA;
            const char *COLSPEC = "UCTE";

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *D_SPEC = DATA[ti].d_dtSpec;
                const int   DI     = DATA[ti].d_di;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = (int) strlen(D_SPEC);
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                Table DD(hx(COLSPEC, D_SPEC));    // control for destination
                Table EE(hx(COLSPEC, E_SPEC));    // control for expected value
                const int EXP_LEN = bsl::strlen(E_SPEC);

                for (int i = 0; i < EXP_LEN; ++i) {
                    if ('n' == E_SPEC[i]) {
                        EE.makeRowsNull(i, 1);
                    }
                }

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(D_SPEC); P_(DI); P_(NE); P(E_SPEC);
                    cout << "\t\t"; PR(DD);
                    cout << "\t\t"; PR(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                if (veryVerbose) cout << "\t\tinsertNullRows(di, ne)" << endl;
                {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Table x(DD, &testAllocator);  const Table &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; PR(X); }
                    x.insertNullRows(DI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; PR(X); }
                    LOOP3_ASSERT(LINE, X, EE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tinsertNullRow(di)" << endl;
                if (1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Table x(DD, &testAllocator);  const Table &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; PR(X); }
                    x.insertNullRow(DI);
                    const Row& NR = X[DI];
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; PR(X); }
                    LOOP3_ASSERT(LINE, EE, X, EE  == X);
                    LOOP_ASSERT(LINE, &NR == &X[DI]);
                  } END_BSLMA_EXCEPTION_TEST
                }

                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose) cout << "\t\tappendNullRows(ne)" << endl;
                if ((int) strlen(D_SPEC) == DI) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Table x(DD, &testAllocator);  const Table &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; PR(X); }
                    x.appendNullRows(NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; PR(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tappendNullRow()" << endl;
                if ((int) strlen(D_SPEC) == DI && 1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Table x(DD, &testAllocator);  const Table &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; PR(X); }
                    x.appendNullRow();
                    const Row& NR = X[x.numRows() - 1];
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; PR(X); }
                    LOOP_ASSERT(LINE, EE  == X);
                    LOOP_ASSERT(LINE, &NR == &X[X.numRows() - 1]);
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }
      }

DEFINE_TEST_CASE(24) {
        // --------------------------------------------------------------------
        // TESTING 'bdem_Table' REPLACEROW METHODS
        // Concerns:
        //   For the 'replace' method, the following properties must hold:
        //     1. The source is left unaffected (apart from aliasing).
        //     2. The subsequent existence of the source has no effect on the
        //        result object (apart from aliasing).
        //     3. The function is alias safe.
        //     4. The function preserves object invariants.
        //
        // Plan:
        //   Use the enumeration technique to a depth of 5 for both the normal
        //   and alias cases.  Data is tabulated explicitly for the 'replace'
        //   method that takes a range from a source array (or itself, for the
        //   aliasing test); the "scalar" 'replace' test selects a subset of
        //   the table by testing 'if (1 == NE)' where 'NE' is the tabulated
        //   number of elements to replace.  'operator[]' is also tested using
        //   the scalar 'replace' data, but using explicit assignment to
        //   achieve the "expected" result.
        //     - In the "canonical state" (black-box) tests, we confirm that
        //       the source is unmodified by the method call, and that its
        //       subsequent destruction has no effect on the destination
        //       object.
        //     - In all cases we want to make sure that after the application
        //       of the operation, the object is allowed to go out of scope
        //       directly to enable the destructor to assert object invariants.
        //     - Each object constructed should be wrapped in separate
        //       BSLMA test assert macros and use gg as an optimization.
        //
        // Testing:
        //   void replaceRow(int di, const bdem_Row& sr);
        //   void replaceRow(int di, const bdem_List& sl);
        //   void replaceRow(int di, const bdem_Table& st, int si);
        //   void replaceRows(int di, const bdem_Table& st, int si, int ne);
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl
                << "Testing 'replaceRow' and 'replaceRows" << endl
                << "=====================================" << endl;

        if (verbose) cout <<
            "\nTesting replaceRows(di, sa, si, ne) et al. (no aliasing)" <<
            endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_dtSpec;   // initial (destination) table
                int         d_di;       // index at which to replace into dt
                const char *d_stSpec;   // source table
                int         d_si;       // index at which to replace from sa
                int         d_ne;       // number of elements to replace
                const char *d_expSpec;  // expected table value
            } DATA[] = {
                //line  d-table  di  s-table  si  ne  expected
                //----  -------  --  -------  --  --  --------   Depth = 0
                { L_,   "",      0,  "",      0,  0, ""      },

                //line  d-table  di  s-table  si  ne  expected
                //----  -------  --  -------  --  --  --------   Depth = 1
                { L_,   "a",     0,  "",      0,  0,  "a"     },
                { L_,   "a",     1,  "",      0,  0,  "a"     },

                { L_,   "",      0,  "b",     0,  0,  ""      },
                { L_,   "",      0,  "b",     1,  0,  ""      },

                //line  d-table  di  s-table  si  ne  expected
                //----  -------  --  -------  --  --  --------   Depth = 2
                { L_,   "ab",    0,  "",      0,  0,  "ab"    },
                { L_,   "ab",    1,  "",      0,  0,  "ab"    },
                { L_,   "ab",    2,  "",      0,  0,  "ab"    },

                { L_,   "a",     0,  "b",     0,  0,  "a"     },
                { L_,   "a",     0,  "b",     0,  1,  "b"     },
                { L_,   "a",     0,  "b",     1,  0,  "a"     },
                { L_,   "a",     1,  "b",     0,  0,  "a"     },
                { L_,   "a",     1,  "b",     1,  0,  "a"     },

                { L_,   "",      0,  "ab",    0,  0,  ""      },
                { L_,   "",      0,  "ab",    1,  0,  ""      },
                { L_,   "",      0,  "ab",    2,  0,  ""      },

                //line  d-table  di  s-table  si  ne  expected
                //----  -------  --  -------  --  --  --------   Depth = 3
                { L_,   "abc",   0,  "",      0,  0,  "abc"   },
                { L_,   "abc",   1,  "",      0,  0,  "abc"   },
                { L_,   "abc",   2,  "",      0,  0,  "abc"   },
                { L_,   "abc",   3,  "",      0,  0,  "abc"   },

                { L_,   "ab",    0,  "c",     0,  0,  "ab"    },
                { L_,   "ab",    0,  "c",     0,  1,  "cb"    },
                { L_,   "ab",    0,  "c",     1,  0,  "ab"    },
                { L_,   "ab",    1,  "c",     0,  0,  "ab"    },
                { L_,   "ab",    1,  "c",     0,  1,  "ac"    },
                { L_,   "ab",    1,  "c",     1,  0,  "ab"    },
                { L_,   "ab",    2,  "c",     0,  0,  "ab"    },
                { L_,   "ab",    2,  "c",     1,  0,  "ab"    },

                { L_,   "a",     0,  "bc",    0,  0,  "a"     },
                { L_,   "a",     0,  "bc",    0,  1,  "b"     },
                { L_,   "a",     0,  "bc",    1,  0,  "a"     },
                { L_,   "a",     0,  "bc",    1,  1,  "c"     },
                { L_,   "a",     0,  "bc",    2,  0,  "a"     },
                { L_,   "a",     1,  "bc",    0,  0,  "a"     },
                { L_,   "a",     1,  "bc",    1,  0,  "a"     },
                { L_,   "a",     1,  "bc",    2,  0,  "a"     },

                { L_,   "",      0,  "abc",   0,  0,  ""      },
                { L_,   "",      0,  "abc",   1,  0,  ""      },
                { L_,   "",      0,  "abc",   2,  0,  ""      },
                { L_,   "",      0,  "abc",   3,  0,  ""      },

                //line  d-table  di  s-table  si  ne  expected
                //----  -------  --  -------  --  --  --------   Depth = 4
                { L_,   "abcd",  0,  "",      0,  0,  "abcd"  },
                { L_,   "abcd",  1,  "",      0,  0,  "abcd"  },
                { L_,   "abcd",  2,  "",      0,  0,  "abcd"  },
                { L_,   "abcd",  3,  "",      0,  0,  "abcd"  },
                { L_,   "abcd",  4,  "",      0,  0,  "abcd"  },

                { L_,   "abc",   0,  "d",     0,  0,  "abc"   },
                { L_,   "abc",   0,  "d",     0,  1,  "dbc"   },
                { L_,   "abc",   0,  "d",     1,  0,  "abc"   },
                { L_,   "abc",   1,  "d",     0,  0,  "abc"   },
                { L_,   "abc",   1,  "d",     0,  1,  "adc"   },
                { L_,   "abc",   1,  "d",     1,  0,  "abc"   },
                { L_,   "abc",   2,  "d",     0,  0,  "abc"   },
                { L_,   "abc",   2,  "d",     0,  1,  "abd"   },
                { L_,   "abc",   2,  "d",     1,  0,  "abc"   },
                { L_,   "abc",   3,  "d",     0,  0,  "abc"   },
                { L_,   "abc",   3,  "d",     1,  0,  "abc"   },

                { L_,   "ab",    0,  "cd",    0,  0,  "ab"    },
                { L_,   "ab",    0,  "cd",    0,  1,  "cb"    },
                { L_,   "ab",    0,  "cd",    0,  2,  "cd"    },
                { L_,   "ab",    0,  "cd",    1,  0,  "ab"    },
                { L_,   "ab",    0,  "cd",    1,  1,  "db"    },
                { L_,   "ab",    0,  "cd",    2,  0,  "ab"    },
                { L_,   "ab",    1,  "cd",    0,  0,  "ab"    },
                { L_,   "ab",    1,  "cd",    0,  1,  "ac"    },
                { L_,   "ab",    1,  "cd",    1,  0,  "ab"    },
                { L_,   "ab",    1,  "cd",    1,  1,  "ad"    },
                { L_,   "ab",    1,  "cd",    2,  0,  "ab"    },
                { L_,   "ab",    2,  "cd",    0,  0,  "ab"    },
                { L_,   "ab",    2,  "cd",    1,  0,  "ab"    },
                { L_,   "ab",    2,  "cd",    2,  0,  "ab"    },

                { L_,   "a",     0,  "bcd",   0,  0,  "a"     },
                { L_,   "a",     0,  "bcd",   0,  1,  "b"     },
                { L_,   "a",     0,  "bcd",   1,  0,  "a"     },
                { L_,   "a",     0,  "bcd",   1,  1,  "c"     },
                { L_,   "a",     0,  "bcd",   2,  0,  "a"     },
                { L_,   "a",     0,  "bcd",   2,  1,  "d"     },
                { L_,   "a",     0,  "bcd",   3,  0,  "a"     },
                { L_,   "a",     1,  "bcd",   0,  0,  "a"     },
                { L_,   "a",     1,  "bcd",   1,  0,  "a"     },
                { L_,   "a",     1,  "bcd",   2,  0,  "a"     },
                { L_,   "a",     1,  "bcd",   3,  0,  "a"     },

                { L_,   "",      0,  "abcd",  0,  0,  ""      },
                { L_,   "",      0,  "abcd",  1,  0,  ""      },
                { L_,   "",      0,  "abcd",  2,  0,  ""      },
                { L_,   "",      0,  "abcd",  3,  0,  ""      },
                { L_,   "",      0,  "abcd",  4,  0,  ""      },

                //line  d-table  di  s-table  si  ne  expected
                //----  -------  --  -------  --  --  --------   Depth = 5
                { L_,   "abcde", 0,  "",      0,  0,  "abcde" },
                { L_,   "abcde", 1,  "",      0,  0,  "abcde" },
                { L_,   "abcde", 2,  "",      0,  0,  "abcde" },
                { L_,   "abcde", 3,  "",      0,  0,  "abcde" },
                { L_,   "abcde", 4,  "",      0,  0,  "abcde" },
                { L_,   "abcde", 5,  "",      0,  0,  "abcde" },

                { L_,   "abcd",  0,  "e",     0,  0,  "abcd"  },
                { L_,   "abcd",  0,  "e",     0,  1,  "ebcd"  },
                { L_,   "abcd",  0,  "e",     1,  0,  "abcd"  },
                { L_,   "abcd",  1,  "e",     0,  0,  "abcd"  },
                { L_,   "abcd",  1,  "e",     0,  1,  "aecd"  },
                { L_,   "abcd",  1,  "e",     1,  0,  "abcd"  },
                { L_,   "abcd",  2,  "e",     0,  0,  "abcd"  },
                { L_,   "abcd",  2,  "e",     0,  1,  "abed"  },
                { L_,   "abcd",  2,  "e",     1,  0,  "abcd"  },
                { L_,   "abcd",  3,  "e",     0,  0,  "abcd"  },
                { L_,   "abcd",  3,  "e",     0,  1,  "abce"  },
                { L_,   "abcd",  3,  "e",     1,  0,  "abcd"  },
                { L_,   "abcd",  4,  "e",     0,  0,  "abcd"  },
                { L_,   "abcd",  4,  "e",     1,  0,  "abcd"  },

                { L_,   "abc",   0,  "de",    0,  0,  "abc"   },
                { L_,   "abc",   0,  "de",    0,  1,  "dbc"   },
                { L_,   "abc",   0,  "de",    0,  2,  "dec"   },
                { L_,   "abc",   0,  "de",    1,  0,  "abc"   },
                { L_,   "abc",   0,  "de",    1,  1,  "ebc"   },
                { L_,   "abc",   0,  "de",    2,  0,  "abc"   },
                { L_,   "abc",   1,  "de",    0,  0,  "abc"   },
                { L_,   "abc",   1,  "de",    0,  1,  "adc"   },
                { L_,   "abc",   1,  "de",    0,  2,  "ade"   },
                { L_,   "abc",   1,  "de",    1,  0,  "abc"   },
                { L_,   "abc",   1,  "de",    1,  1,  "aec"   },
                { L_,   "abc",   1,  "de",    2,  0,  "abc"   },
                { L_,   "abc",   2,  "de",    0,  0,  "abc"   },
                { L_,   "abc",   2,  "de",    0,  1,  "abd"   },
                { L_,   "abc",   2,  "de",    1,  0,  "abc"   },
                { L_,   "abc",   2,  "de",    1,  1,  "abe"   },
                { L_,   "abc",   2,  "de",    2,  0,  "abc"   },
                { L_,   "abc",   3,  "de",    0,  0,  "abc"   },
                { L_,   "abc",   3,  "de",    1,  0,  "abc"   },
                { L_,   "abc",   3,  "de",    2,  0,  "abc"   },

                { L_,   "ab",    0,  "cde",   0,  0,  "ab"    },
                { L_,   "ab",    0,  "cde",   0,  1,  "cb"    },
                { L_,   "ab",    0,  "cde",   0,  2,  "cd"    },
                { L_,   "ab",    0,  "cde",   1,  0,  "ab"    },
                { L_,   "ab",    0,  "cde",   1,  1,  "db"    },
                { L_,   "ab",    0,  "cde",   2,  0,  "ab"    },
                { L_,   "ab",    0,  "cde",   2,  1,  "eb"    },
                { L_,   "ab",    0,  "cde",   3,  0,  "ab"    },
                { L_,   "ab",    1,  "cde",   0,  0,  "ab"    },
                { L_,   "ab",    1,  "cde",   0,  1,  "ac"    },
                { L_,   "ab",    1,  "cde",   1,  0,  "ab"    },
                { L_,   "ab",    1,  "cde",   1,  1,  "ad"    },
                { L_,   "ab",    1,  "cde",   2,  0,  "ab"    },
                { L_,   "ab",    1,  "cde",   2,  1,  "ae"    },
                { L_,   "ab",    1,  "cde",   3,  0,  "ab"    },
                { L_,   "ab",    2,  "cde",   0,  0,  "ab"    },
                { L_,   "ab",    2,  "cde",   1,  0,  "ab"    },
                { L_,   "ab",    2,  "cde",   2,  0,  "ab"    },
                { L_,   "ab",    2,  "cde",   3,  0,  "ab"    },

                { L_,   "a",     0,  "bcde",  0,  0,  "a"     },
                { L_,   "a",     0,  "bcde",  0,  1,  "b"     },
                { L_,   "a",     0,  "bcde",  1,  0,  "a"     },
                { L_,   "a",     0,  "bcde",  1,  1,  "c"     },
                { L_,   "a",     0,  "bcde",  2,  0,  "a"     },
                { L_,   "a",     0,  "bcde",  2,  1,  "d"     },
                { L_,   "a",     0,  "bcde",  3,  0,  "a"     },
                { L_,   "a",     0,  "bcde",  3,  1,  "e"     },
                { L_,   "a",     0,  "bcde",  4,  0,  "a"     },
                { L_,   "a",     1,  "bcde",  0,  0,  "a"     },
                { L_,   "a",     1,  "bcde",  1,  0,  "a"     },
                { L_,   "a",     1,  "bcde",  2,  0,  "a"     },
                { L_,   "a",     1,  "bcde",  3,  0,  "a"     },
                { L_,   "a",     1,  "bcde",  4,  0,  "a"     },

                { L_,   "",      0,  "abcde", 0,  0,  ""      },
                { L_,   "",      0,  "abcde", 1,  0,  ""      },
                { L_,   "",      0,  "abcde", 2,  0,  ""      },
                { L_,   "",      0,  "abcde", 3,  0,  ""      },
                { L_,   "",      0,  "abcde", 4,  0,  ""      },
                { L_,   "",      0,  "abcde", 5,  0,  ""      },
            };

            const char *COLSPEC = "TQ";

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *D_SPEC = DATA[ti].d_dtSpec;
                const int   DI     = DATA[ti].d_di;
                const char *S_SPEC = DATA[ti].d_stSpec;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = (int) strlen(D_SPEC) + strlen(S_SPEC);
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                Table DD(hx(COLSPEC, D_SPEC));    // control for destination
                Table SS(hx(COLSPEC, S_SPEC));    // control for source
                Table EE(hx(COLSPEC, E_SPEC));    // control for expected value

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(D_SPEC); P_(DI); P_(S_SPEC);
                                                P_(SI); P_(NE); P(E_SPEC);
                    cout << "\t\t"; PR(DD);
                    cout << "\t\t"; PR(SS);
                    cout << "\t\t"; PR(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                if (veryVerbose)
                    cout << "\t\treplaceRows(di, sa, si, ne)" << endl;
                {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Table x(DD, &testAllocator);  const Table &X = x;
                    {
                        Table s(SS, &testAllocator);  const Table &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; PR(X); }
                        replaceRows(x, DI, s, SI, NE);   // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; PR(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\treplaceRow(di, st, si)" << endl;
                if (1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Table x(DD, &testAllocator);  const Table &X = x;
                    {
                        Table s(SS, &testAllocator);  const Table &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; PR(X); }

                        replaceRows(x, DI, S, SI, NE);

                        if (veryVerbose) { cout << "\t\t\t AFTER: "; PR(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\treplaceRow(di, row)" << endl;
                if (1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Table x(DD, &testAllocator);  const Table &X = x;
                    {
                        Table s(SS, &testAllocator);  const Table &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; PR(X); }

                        replaceRows(x, DI, S, SI, NE);   // source non-'const'

                        if (veryVerbose) { cout << "\t\t\t AFTER: "; PR(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }

        //---------------------------------------------------------------------

        if (verbose) cout <<
            "\nTesting x.replaceRows(di, sa, si, ne) et al. (aliasing)"
            << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_xSpec;    // initial table (= srcArray)
                int         d_di;       // index at which to insert into x
                int         d_si;       // index at which to insert from x
                int         d_ne;       // number of elements to insert
                const char *d_expSpec;  // expected table value
            } DATA[] = {
                //line  x-table  di  si  ne  expected
                //----  -------  --  --  --  --------   Depth = 0
                { L_,   "",      0,  0,  0,  ""      },

                //line  x-table  di  si  ne  expected
                //----  -------  --  --  --  --------   Depth = 1
                { L_,   "a",     0,  0,  0,  "a"     },
                { L_,   "a",     0,  0,  1,  "a"     },
                { L_,   "a",     0,  1,  0,  "a"     },

                { L_,   "a",     1,  0,  0,  "a"     },
                { L_,   "a",     1,  1,  0,  "a"     },

                //line  x-table  di  si  ne  expected
                //----  -------  --  --  --  --------   Depth = 2
                { L_,   "ab",    0,  0,  0,  "ab"    },
                { L_,   "ab",    0,  0,  1,  "ab"    },
                { L_,   "ab",    0,  0,  2,  "ab"    },
                { L_,   "ab",    0,  1,  0,  "ab"    },
                { L_,   "ab",    0,  1,  1,  "bb"    },
                { L_,   "ab",    0,  2,  0,  "ab"    },

                { L_,   "ab",    1,  0,  0,  "ab"    },
                { L_,   "ab",    1,  0,  1,  "aa"    },
                { L_,   "ab",    1,  1,  0,  "ab"    },
                { L_,   "ab",    1,  1,  1,  "ab"    },
                { L_,   "ab",    1,  2,  0,  "ab"    },

                { L_,   "ab",    2,  0,  0,  "ab"    },
                { L_,   "ab",    2,  1,  0,  "ab"    },
                { L_,   "ab",    2,  2,  0,  "ab"    },

                //line  x-table  di  si  ne  expected
                //----  -------  --  --  --  --------   Depth = 3
                { L_,   "abc",   0,  0,  0,  "abc"   },
                { L_,   "abc",   0,  0,  1,  "abc"   },
                { L_,   "abc",   0,  0,  2,  "abc"   },
                { L_,   "abc",   0,  0,  3,  "abc"   },
                { L_,   "abc",   0,  1,  0,  "abc"   },
                { L_,   "abc",   0,  1,  1,  "bbc"   },
                { L_,   "abc",   0,  1,  2,  "bcc"   },
                { L_,   "abc",   0,  2,  0,  "abc"   },
                { L_,   "abc",   0,  2,  1,  "cbc"   },
                { L_,   "abc",   0,  3,  0,  "abc"   },

                { L_,   "abc",   1,  0,  0,  "abc"   },
                { L_,   "abc",   1,  0,  1,  "aac"   },
                { L_,   "abc",   1,  0,  2,  "aab"   },
                { L_,   "abc",   1,  1,  0,  "abc"   },
                { L_,   "abc",   1,  1,  1,  "abc"   },
                { L_,   "abc",   1,  1,  2,  "abc"   },
                { L_,   "abc",   1,  2,  0,  "abc"   },
                { L_,   "abc",   1,  2,  1,  "acc"   },
                { L_,   "abc",   1,  3,  0,  "abc"   },

                { L_,   "abc",   2,  0,  0,  "abc"   },
                { L_,   "abc",   2,  0,  1,  "aba"   },
                { L_,   "abc",   2,  1,  0,  "abc"   },
                { L_,   "abc",   2,  1,  1,  "abb"   },
                { L_,   "abc",   2,  2,  0,  "abc"   },
                { L_,   "abc",   2,  2,  1,  "abc"   },
                { L_,   "abc",   2,  3,  0,  "abc"   },

                { L_,   "abc",   3,  0,  0,  "abc"   },
                { L_,   "abc",   3,  1,  0,  "abc"   },
                { L_,   "abc",   3,  2,  0,  "abc"   },
                { L_,   "abc",   3,  3,  0,  "abc"   },

                //line  x-table  di  si  ne  expected
                //----  -------  --  --  --  --------   Depth = 4
                { L_,   "abcd",  0,  0,  0,  "abcd"  },
                { L_,   "abcd",  0,  0,  1,  "abcd"  },
                { L_,   "abcd",  0,  0,  2,  "abcd"  },
                { L_,   "abcd",  0,  0,  3,  "abcd"  },
                { L_,   "abcd",  0,  0,  4,  "abcd"  },
                { L_,   "abcd",  0,  1,  0,  "abcd"  },
                { L_,   "abcd",  0,  1,  1,  "bbcd"  },
                { L_,   "abcd",  0,  1,  2,  "bccd"  },
                { L_,   "abcd",  0,  1,  3,  "bcdd"  },
                { L_,   "abcd",  0,  2,  0,  "abcd"  },
                { L_,   "abcd",  0,  2,  1,  "cbcd"  },
                { L_,   "abcd",  0,  2,  2,  "cdcd"  },
                { L_,   "abcd",  0,  3,  0,  "abcd"  },
                { L_,   "abcd",  0,  3,  1,  "dbcd"  },
                { L_,   "abcd",  0,  4,  0,  "abcd"  },

                { L_,   "abcd",  1,  0,  0,  "abcd"  },
                { L_,   "abcd",  1,  0,  1,  "aacd"  },
                { L_,   "abcd",  1,  0,  2,  "aabd"  },
                { L_,   "abcd",  1,  0,  3,  "aabc"  },
                { L_,   "abcd",  1,  1,  0,  "abcd"  },
                { L_,   "abcd",  1,  1,  1,  "abcd"  },
                { L_,   "abcd",  1,  1,  2,  "abcd"  },
                { L_,   "abcd",  1,  1,  3,  "abcd"  },
                { L_,   "abcd",  1,  2,  0,  "abcd"  },
                { L_,   "abcd",  1,  2,  1,  "accd"  },
                { L_,   "abcd",  1,  2,  2,  "acdd"  },
                { L_,   "abcd",  1,  3,  0,  "abcd"  },
                { L_,   "abcd",  1,  3,  1,  "adcd"  },
                { L_,   "abcd",  1,  4,  0,  "abcd"  },

                { L_,   "abcd",  2,  0,  0,  "abcd"  },
                { L_,   "abcd",  2,  0,  1,  "abad"  },
                { L_,   "abcd",  2,  0,  2,  "abab"  },
                { L_,   "abcd",  2,  1,  0,  "abcd"  },
                { L_,   "abcd",  2,  1,  1,  "abbd"  },
                { L_,   "abcd",  2,  1,  2,  "abbc"  },
                { L_,   "abcd",  2,  2,  0,  "abcd"  },
                { L_,   "abcd",  2,  2,  1,  "abcd"  },
                { L_,   "abcd",  2,  2,  2,  "abcd"  },
                { L_,   "abcd",  2,  3,  0,  "abcd"  },
                { L_,   "abcd",  2,  3,  1,  "abdd"  },
                { L_,   "abcd",  2,  4,  0,  "abcd"  },

                { L_,   "abcd",  3,  0,  0,  "abcd"  },
                { L_,   "abcd",  3,  0,  1,  "abca"  },
                { L_,   "abcd",  3,  1,  0,  "abcd"  },
                { L_,   "abcd",  3,  1,  1,  "abcb"  },
                { L_,   "abcd",  3,  2,  0,  "abcd"  },
                { L_,   "abcd",  3,  2,  1,  "abcc"  },
                { L_,   "abcd",  3,  3,  0,  "abcd"  },
                { L_,   "abcd",  3,  3,  1,  "abcd"  },
                { L_,   "abcd",  3,  4,  0,  "abcd"  },

                { L_,   "abcd",  4,  0,  0,  "abcd"  },
                { L_,   "abcd",  4,  1,  0,  "abcd"  },
                { L_,   "abcd",  4,  2,  0,  "abcd"  },
                { L_,   "abcd",  4,  3,  0,  "abcd"  },
                { L_,   "abcd",  4,  4,  0,  "abcd"  },

                //line  x-table  di  si  ne  expected
                //----  -------  --  --  --  --------   Depth = 5
                { L_,   "abcde", 0,  0,  0,  "abcde" },
                { L_,   "abcde", 0,  0,  1,  "abcde" },
                { L_,   "abcde", 0,  0,  2,  "abcde" },
                { L_,   "abcde", 0,  0,  3,  "abcde" },
                { L_,   "abcde", 0,  0,  4,  "abcde" },
                { L_,   "abcde", 0,  0,  5,  "abcde" },
                { L_,   "abcde", 0,  1,  0,  "abcde" },
                { L_,   "abcde", 0,  1,  1,  "bbcde" },
                { L_,   "abcde", 0,  1,  2,  "bccde" },
                { L_,   "abcde", 0,  1,  3,  "bcdde" },
                { L_,   "abcde", 0,  1,  4,  "bcdee" },
                { L_,   "abcde", 0,  2,  0,  "abcde" },
                { L_,   "abcde", 0,  2,  1,  "cbcde" },
                { L_,   "abcde", 0,  2,  2,  "cdcde" },
                { L_,   "abcde", 0,  2,  3,  "cdede" },
                { L_,   "abcde", 0,  3,  0,  "abcde" },
                { L_,   "abcde", 0,  3,  1,  "dbcde" },
                { L_,   "abcde", 0,  3,  2,  "decde" },
                { L_,   "abcde", 0,  4,  0,  "abcde" },
                { L_,   "abcde", 0,  4,  1,  "ebcde" },
                { L_,   "abcde", 0,  5,  0,  "abcde" },

                { L_,   "abcde", 1,  0,  0,  "abcde" },
                { L_,   "abcde", 1,  0,  1,  "aacde" },
                { L_,   "abcde", 1,  0,  2,  "aabde" },
                { L_,   "abcde", 1,  0,  3,  "aabce" },
                { L_,   "abcde", 1,  0,  4,  "aabcd" },
                { L_,   "abcde", 1,  1,  0,  "abcde" },
                { L_,   "abcde", 1,  1,  1,  "abcde" },
                { L_,   "abcde", 1,  1,  2,  "abcde" },
                { L_,   "abcde", 1,  1,  3,  "abcde" },
                { L_,   "abcde", 1,  1,  4,  "abcde" },
                { L_,   "abcde", 1,  2,  0,  "abcde" },
                { L_,   "abcde", 1,  2,  1,  "accde" },
                { L_,   "abcde", 1,  2,  2,  "acdde" },
                { L_,   "abcde", 1,  2,  3,  "acdee" },
                { L_,   "abcde", 1,  3,  0,  "abcde" },
                { L_,   "abcde", 1,  3,  1,  "adcde" },
                { L_,   "abcde", 1,  3,  2,  "adede" },
                { L_,   "abcde", 1,  4,  0,  "abcde" },
                { L_,   "abcde", 1,  4,  1,  "aecde" },
                { L_,   "abcde", 1,  5,  0,  "abcde" },

                { L_,   "abcde", 2,  0,  0,  "abcde" },
                { L_,   "abcde", 2,  0,  1,  "abade" },
                { L_,   "abcde", 2,  0,  2,  "ababe" },
                { L_,   "abcde", 2,  0,  3,  "ababc" },
                { L_,   "abcde", 2,  1,  0,  "abcde" },
                { L_,   "abcde", 2,  1,  1,  "abbde" },
                { L_,   "abcde", 2,  1,  2,  "abbce" },
                { L_,   "abcde", 2,  1,  3,  "abbcd" },
                { L_,   "abcde", 2,  2,  0,  "abcde" },
                { L_,   "abcde", 2,  2,  1,  "abcde" },
                { L_,   "abcde", 2,  2,  2,  "abcde" },
                { L_,   "abcde", 2,  2,  3,  "abcde" },
                { L_,   "abcde", 2,  3,  0,  "abcde" },
                { L_,   "abcde", 2,  3,  1,  "abdde" },
                { L_,   "abcde", 2,  3,  2,  "abdee" },
                { L_,   "abcde", 2,  4,  0,  "abcde" },
                { L_,   "abcde", 2,  4,  1,  "abede" },
                { L_,   "abcde", 2,  5,  0,  "abcde" },

                { L_,   "abcde", 3,  0,  0,  "abcde" },
                { L_,   "abcde", 3,  0,  1,  "abcae" },
                { L_,   "abcde", 3,  0,  2,  "abcab" },
                { L_,   "abcde", 3,  1,  0,  "abcde" },
                { L_,   "abcde", 3,  1,  1,  "abcbe" },
                { L_,   "abcde", 3,  1,  2,  "abcbc" },
                { L_,   "abcde", 3,  2,  0,  "abcde" },
                { L_,   "abcde", 3,  2,  1,  "abcce" },
                { L_,   "abcde", 3,  2,  2,  "abccd" },
                { L_,   "abcde", 3,  3,  0,  "abcde" },
                { L_,   "abcde", 3,  3,  1,  "abcde" },
                { L_,   "abcde", 3,  3,  2,  "abcde" },
                { L_,   "abcde", 3,  4,  0,  "abcde" },
                { L_,   "abcde", 3,  4,  1,  "abcee" },
                { L_,   "abcde", 3,  5,  0,  "abcde" },

                { L_,   "abcde", 4,  0,  0,  "abcde" },
                { L_,   "abcde", 4,  0,  1,  "abcda" },
                { L_,   "abcde", 4,  1,  0,  "abcde" },
                { L_,   "abcde", 4,  1,  1,  "abcdb" },
                { L_,   "abcde", 4,  2,  0,  "abcde" },
                { L_,   "abcde", 4,  2,  1,  "abcdc" },
                { L_,   "abcde", 4,  3,  0,  "abcde" },
                { L_,   "abcde", 4,  3,  1,  "abcdd" },
                { L_,   "abcde", 4,  4,  0,  "abcde" },
                { L_,   "abcde", 4,  4,  1,  "abcde" },
                { L_,   "abcde", 4,  5,  0,  "abcde" },

                { L_,   "abcde", 5,  0,  0,  "abcde" },
                { L_,   "abcde", 5,  1,  0,  "abcde" },
                { L_,   "abcde", 5,  2,  0,  "abcde" },
                { L_,   "abcde", 5,  3,  0,  "abcde" },
                { L_,   "abcde", 5,  4,  0,  "abcde" },
                { L_,   "abcde", 5,  5,  0,  "abcde" },
            };

            const char *COLSPEC = "TQ";
            const int NUM_DATA  = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *X_SPEC = DATA[ti].d_xSpec;
                const int   DI     = DATA[ti].d_di;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = strlen(X_SPEC);
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                Table DD(hx(COLSPEC, X_SPEC));    // control for destination
                Table EE(hx(COLSPEC, E_SPEC));    // control for expected value

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(X_SPEC); P_(DI);
                                                P_(SI); P_(NE); P(E_SPEC);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                // CONTROL: ensure same table result as non-alias case.
                {
                    Table x(DD, &testAllocator);  const Table &X = x;
                    Table x2(DD, &testAllocator); // control
                    const Table &X2 = x2; // control
                    replaceRows(x2, DI, X, SI, NE);
                    LOOP_ASSERT(LINE, EE == X2);
                }

                if (veryVerbose)
                    cout << "\t\treplaceRows(di, sa, si, ne)" << endl;
                {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Table x(DD, &testAllocator);  const Table &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }

                    replaceRows(x, DI, X, SI, NE);

                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\treplaceRow(di, st, si)" << endl;
                if (1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    Table x(DD, &testAllocator);  const Table &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }

                    replaceRows(x, DI, X, SI, NE);

                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }
      }

DEFINE_TEST_CASE(23) {
        // --------------------------------------------------------------------
        // TESTING 'bdem_Table' ASSIGNMENT OPERATOR
        // We have the following concerns:
        //   1.  The value represented by any instance can be assigned to any
        //         other instance regardless of how either value is represented
        //         internally.
        //   2.  The 'rhs' value must not be affected by the operation.
        //   3.  'rhs' going out of scope has no effect on the value of 'lhs'
        //       after the assignment.
        //   4.  Aliasing (x = x): The assignment operator must always work --
        //         even when the lhs and rhs are identically the same object.
        //   5.  The assignment operator must be neutral with respect to memory
        //       allocation exceptions.
        // Plan:
        //   Specify a set S of unique object values with substantial and
        //   varied differences, ordered by increasing length.  For each value
        //   in S, construct an object x along with a sequence of similarly
        //   constructed duplicates x1, x2, ..., xN.  Attempt to affect every
        //   aspect of white-box state by altering each xi in a unique way.
        //   Let the union of all such objects be the set T.
        //
        //   To address concerns 1, 2, and 5, construct tests u = v for all
        //   (u, v) in T X T.  Using canonical controls UU and VV, assert
        //   before the assignment that UU == u, VV == v, and v == u iff
        //   VV == UU.  After the assignment, assert that VV == u, VV == v,
        //   and, for grins, that v == u.  Let v go out of scope and confirm
        //   that VV == u.  All of these tests are performed within the 'bdema'
        //   exception testing apparatus.
        //
        //   As a separate exercise, we address 4 and 5 by constructing tests
        //   y = y for all y in T.  Using a canonical control X, we will verify
        //   that X == y before and after the assignment, again within
        //   the bdema exception testing apparatus.
        //
        // Testing:
        //   bdem_Table& operator=(const bdem_Table& rhs);
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl
                          << "Testing bdem_Table Assignment Operator" << endl
                          << "======================================" << endl;

        if (verbose) cout <<
            "\nAssign cross product of values with varied representations."
            << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9,
                ";",               "A;x;",    "A;y;x;",    "BC;xx;yy;",
                "DE;yy;xx;yy;",    "DEA;xxy;xxx;uuu;yyy;",
                "EAB;xyx;xxx;uuu;xyy;yyx;",
                "QTC;xyx;xxx;yyy;xxx;uyu;xxx;yyu;",
// exception handling is *extremely* slow on HP
#if !defined(BSLS_PLATFORM__CMP_HP) || !defined(BDE_BUILD_TARGET_EXC)
                "TUH;xxy;yyy;xxx;uuu;xyx;yyy;xxx;yxx;",
                "BA;xx;yy;uu;xy;yx;xu;yu;ux;uy;",
#endif
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9,
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int uLen = (int)getNumRows(U_SPEC);

                if (verbose) {
                    cout << "\tFor lhs objects of length " << uLen << ":\t";
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen <= uLen);   // strictly increasing
                uOldLen = uLen;

                const Table UU = h(U_SPEC);             // control
                LOOP_ASSERT(ui, uLen == UU.numRows());  // same lengths

                // int vOldLen = -1;
                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const int vLen = (int)getNumRows(V_SPEC);

                    if (veryVerbose) {
                        cout << "\t\tFor rhs objects of length " << vLen
                             << ":\t";
                        P(V_SPEC);
                    }

                    const Table VV = h(V_SPEC);  // control
                    const int Z    = ui == vi;   // flag indicating same values

                    for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                        const int U_N = EXTEND[uj];
                        for (int vj = 0; vj < NUM_EXTEND; ++vj) {
                          const int V_N = EXTEND[vj];

                          BEGIN_BSLMA_EXCEPTION_TEST {
                            const int AL = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);
                            Table mU(&testAllocator);
                            // stretchRemoveAll(&mU, U_N); *****************88
                            const Table& U = mU; hh(&mU, U_SPEC);
                            {
                            //--^
                            Table mV(&testAllocator);
                            // stretchRemoveAll(&mV, V_N); *******************
                            const Table& V = mV; hh(&mV, V_SPEC);

                            static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                            if (veryVeryVerbose||veryVerbose && firstFew > 0) {
                                cout << "\t| "; P_(U_N); P_(V_N); PR(U); PR(V);
                                --firstFew;
                            }

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, UU == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, Z==(V==U));

                            testAllocator.setAllocationLimit(AL);
                            mU = V; // test assignment here

                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == V);
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N,  V == U);
                            //--v
                            }
                            // 'mV' (and therefore 'V') now out of scope
                            LOOP4_ASSERT(U_SPEC, U_N, V_SPEC, V_N, VV == U);
                          } END_BSLMA_EXCEPTION_TEST
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting self assignment (Aliasing)." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9, 15, 16, 17
                "",      ";", "A;x;",      "BC;xx;",     "CDE;xxx;",
                "DEAB;xxxx;",   "EABCD;xxxxx;", "ABCDEAB;xxxxxxx;",
                "ABCDEABC;xxxxxxxx;",
                //"ABCDEABCD;xxxxxxxx;", "TUCDEABCDEABCDE;",
                //"ABCDEHIJKLMNOPQS;",
                //"ABCDEABCDEABCDEAB;",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = getNumRows(SPEC);

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }
                LOOP_ASSERT(SPEC, oldLen <= curLen);     // increasing
                oldLen = curLen;

                const Table X = h(SPEC);                 // control
                LOOP_ASSERT(ti, curLen == X.numRows());  // same lengths

                for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    const int N = EXTEND[tj];
                    Table mY(&testAllocator);
                    hh(&mY, SPEC);
                    const Table& Y = mY;

                    if (veryVerbose) { cout << "\t\t"; P_(N); PR(Y); }

                    LOOP2_ASSERT(SPEC, N, Y == Y);
                    LOOP2_ASSERT(SPEC, N, X == Y);

                    testAllocator.setAllocationLimit(AL);
                    mY = Y; // test assignment here

                    LOOP2_ASSERT(SPEC, N, Y == Y);
                    LOOP2_ASSERT(SPEC, N, X == Y);

                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }
      }

DEFINE_TEST_CASE(22) {
        // --------------------------------------------------------------------
        // TESTING 'bdem_List' ASSIGNMENT OPERATOR
        // We have the following concerns:
        //   1.  The value represented by any instance can be assigned to any
        //       other instance regardless of how either value is represented
        //       internally.
        //   2.  The 'rhs' value must not be affected by the operation.
        //   3.  'rhs' going out of scope has no effect on the value of 'lhs'
        //       after the assignment.
        //   4.  Aliasing (x = x): The assignment operator must always work --
        //       even when the lhs and rhs are identically the same object.
        //   5.  The assignment operator must be neutral with respect to memory
        //       allocation exceptions.
        // Plan:
        //   Specify a set S of unique object values with substantial and
        //   varied differences, ordered by increasing length.  For each value
        //   in S, construct an object x along with a sequence of similarly
        //   constructed duplicates x1, x2, ..., xN.  Attempt to affect every
        //   aspect of white-box state by altering each xi in a unique way.
        //   Let the union of all such objects be the set T.
        //
        //   To address concerns 1, 2, and 5, construct tests u = v for all
        //   (u, v) in T X T.  Using canonical controls UU and VV, assert
        //   before the assignment that UU == u, VV == v, and v == u iff
        //   VV == UU.  After the assignment, assert that VV == u, VV == v,
        //   and, for grins, that v == u.  Let v go out of scope and confirm
        //   that VV == u.  All of these tests are performed within the 'bdema'
        //   exception testing apparatus.
        //
        //   As a separate exercise, we address 4 and 5 by constructing tests
        //   y = y for all y in T.  Using a canonical control X, we will verify
        //   that X == y before and after the assignment, again within
        //   the bdema exception testing apparatus.
        //
        // Testing:
        //   bdem_List& bdem_List::operator=(const bdem_Row& rhs);
        //   bdem_List& bdem_List::operator=(const bdem_List& rhs);
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl
                          << "Testing Assignment Operator" << endl
                          << "===========================" << endl;

        if (verbose) cout <<
            "\nAssign cross product of values with varied representations."
            << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9,
                "",
                "xA",    "yA",
                "xBxC",  "yByC",
                "xCxDxE",  "yCyDyE",
                "xDxExAxB", "yDyEyAyB",
                "xExAxBxCxD", "yEyAyByCyD",
                "xQxSxTxCxBxAxE", "yQySyTyCyByAyE",
                "xTxUxHxIxJxKxLxM", "yTyUyHyIyJyKyLyM",
                "xExDxCxBxAxExDxCxB", "yEyDyCyByAyEyDyCyB",
            0}; // Null string required as last element.

            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int uLen = (int)strlen(U_SPEC)/2;

                if (verbose) {
                    cout << "\tFor lhs objects of length " << uLen << ":\t";
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen <= uLen);  // increasing

                if (uLen != uOldLen) uOldLen = uLen;

                const List UU = g(U_SPEC);             // control
                LOOP_ASSERT(ui, uLen == UU.length());  // same lengths

                // int vOldLen = -1;
                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const int vLen = (int)strlen(V_SPEC)/2;

                    if (veryVerbose) {
                        cout << "\t\tFor rhs objects of length " << vLen
                             << ":\t";
                        P(V_SPEC);
                    }

                    const List VV = g(V_SPEC);  // control
                    const int Z   = ui == vi;   // flag indicating same values

                    for (int u = 0; u < 2; ++u) {

                        for (int v = 0; v < NUM_PERMUTATIONS; ++v) {

                          BEGIN_BSLMA_EXCEPTION_TEST {
                            const int AL = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);
                            List mU(&testAllocator);
                            makePermutation(&mU, U_SPEC, u, 0);
                            const List& U = mU;

                            {
                            //--^
                            List mV(&testAllocator);
                            makePermutation(&mV, V_SPEC, v%2, v/2);
                            const List& V = mV;

                            static int firstFew =
                                       2 * NUM_PERMUTATIONS * NUM_PERMUTATIONS;
                            if (veryVeryVerbose||veryVerbose && firstFew > 0) {
                                cout << "\t| "; P_(u); P_(v); P_(U); P(V);
                                --firstFew;
                            }

                            LOOP4_ASSERT(U_SPEC, u, V_SPEC, v, UU == U);
                            LOOP4_ASSERT(U_SPEC, u, V_SPEC, v, VV == V);
                            LOOP4_ASSERT(U_SPEC, u, V_SPEC, v, Z==(V==U));

                            testAllocator.setAllocationLimit(AL);
                            mU = V; // test assignment here

                            LOOP4_ASSERT(U_SPEC, u, V_SPEC, v, VV == U);
                            LOOP4_ASSERT(U_SPEC, u, V_SPEC, v, VV == V);
                            LOOP4_ASSERT(U_SPEC, u, V_SPEC, v,  V == U);
                            //--v
                            }
                            // 'mV' (and therefore 'V') now out of scope
                            LOOP4_ASSERT(U_SPEC, u, V_SPEC, v, VV == U);
                          } END_BSLMA_EXCEPTION_TEST

                          BEGIN_BSLMA_EXCEPTION_TEST {
                            const int AL = testAllocator.allocationLimit();
                            testAllocator.setAllocationLimit(-1);
                            List mW(&testAllocator);
                            makePermutation(&mW, U_SPEC, u%2, u/2);
                            const List& W = mW;
                            {
                            //--^
                            List mV(&testAllocator);
                            makePermutation(&mV, V_SPEC, v%2, v/2);
                            const List& V = mV;

                            static int firstFew =
                                       2 * NUM_PERMUTATIONS * NUM_PERMUTATIONS;
                            if (veryVeryVerbose||veryVerbose && firstFew > 0) {
                                cout << "\t| "; P_(u); P_(v); P_(W); P(V);
                                --firstFew;
                            }

                            LOOP4_ASSERT(U_SPEC, u, V_SPEC, v, UU == W);
                            LOOP4_ASSERT(U_SPEC, u, V_SPEC, v, Z==(W==V));

                            testAllocator.setAllocationLimit(AL);
                            mW = V.row(); // test with input row

                            LOOP4_ASSERT(U_SPEC, u, V_SPEC, v, VV == W);
                            LOOP4_ASSERT(U_SPEC, u, V_SPEC, v, VV == V);
                            LOOP4_ASSERT(U_SPEC, u, V_SPEC, v,  V == W);
                            //--v
                            }
                            // 'mV' (and therefore 'V') now out of scope
                            LOOP4_ASSERT(U_SPEC, u, V_SPEC, v, VV == W);
                          } END_BSLMA_EXCEPTION_TEST
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\nTesting self assignment (Aliasing)." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9, 15, 16, 17
                "",      "xA",      "xBxC",     "xCxDxE",    "xDxExAxB",
                "xExAxBxCxD", "xAxBxCxDxExAxB",         "xAxBxCxDxExAxBxC",
                "xAxBxCxDxExAxBxCxD", "xTxUxCxDxExAxBxCxDxExAxBxCxDxE",
                "xAxBxCxDxExHxIxJxKxLxMxNxOxPxQxS",
                "xAxBxCxDxExAxBxCxDxExAxBxCxDxExAxB",
            0}; // Null string required as last element.

            static const int EXTEND[] = {
                0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC)/2;

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }
                LOOP_ASSERT(SPEC, oldLen < curLen);  // strictly increasing
                oldLen = curLen;

                const List X = g(SPEC);                  // control
                LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                for (int tj = 0; tj < NUM_EXTEND; ++tj) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);

                    const int N = EXTEND[tj];
                    List mY(&testAllocator);
                    makePermutation(&mY, SPEC, N%2, N/2);
                    const List& Y = mY;

                    if (veryVerbose) { cout << "\t\t"; P_(N); P(Y); }

                    LOOP2_ASSERT(SPEC, N, Y == Y);
                    LOOP2_ASSERT(SPEC, N, X == Y);

                    testAllocator.setAllocationLimit(AL);
                    mY = Y; // test assignment here

                    LOOP2_ASSERT(SPEC, N, Y == Y);
                    LOOP2_ASSERT(SPEC, N, X == Y);

                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }
      }

DEFINE_TEST_CASE(21) {
        // --------------------------------------------------------------------
        // TESTING 'bdem_Table' APPENDROW, INSERTROW, REMOVEROW METHODS
        // Concerns:
        //   For the 'append' and 'insert' methods, the following properties
        //   must hold:
        //    1. The source is left unaffected (apart from aliasing).
        //    2. The subsequent existence of the source has no effect on the
        //       result object (apart from aliasing).
        //    3. The function is alias safe.
        //    4. The function is exception neutral (w.r.t. allocation).
        //    5. The function preserves object invariants.
        //    6. The function is independent of internal representation.
        //   Note that all (contingent) reallocations occur strictly before
        //   the essential implementation of each method.  Therefore,
        //   concerns 1, 2, and 4 above are valid for objects in the
        //   "canonical state", but need not be repeated when concern 6
        //   ("white-box test") is addressed.
        //
        //   For the 'remove' methods, the concerns are simply to cover the
        //   full range of possible indices and numbers of elements.
        //
        // Plan:
        //   Use the enumeration technique to a depth of 5 for both the normal
        //   and alias cases.  Data is tabulated explicitly for the 'insert'
        //   method that takes a range from a source table (or itself, for the
        //   aliasing test); other methods are tested using a subset of the
        //   full test vector table.  In particular, the 'append' methods use
        //   data where the destination index equals the destination length
        //   (strlen(D_SPEC) == DI).  All methods using the entire source
        //   object use test data where the source length equals the number of
        //   elements (strlen(S_SPEC) == NE), while the "scalar" methods use
        //   data where the number of elements equals 1 (1 == NE).  In
        //   addition, the 'remove' methods switch the "d-table" and "expected"
        //   values from the 'insert' table.
        //     - In the "canonical state" (black-box) tests, we confirm that
        //       the source is unmodified by the method call, and that its
        //       subsequent destruction has no effect on the destination
        //       object.
        //     - In all cases we want to make sure that after the application
        //       of the operation, the object is allowed to go out of scope
        //       directly to enable the destructor to assert object invariants.
        //     - Each object constructed should be wrapped in separate
        //       BSLMA test assert macros and use hh as an optimization.
        //
        // Testing:
        //   bdem_Row& appendRow(const bdem_List& sl);
        //   bdem_Row& appendRow(const bdem_Table& st, int si);
        //   void appendRows(const bdem_Table& st);
        //   void appendRows(const bdem_Table& st, int si, int ne);
        //
        //   bdem_Row& insertRow(int di, const bdem_List& sl);
        //   bdem_Row& insertRow(int di, const bdem_Table& st, int si);
        //   void insertRows(int di, const bdem_Table& st);
        //   void insertRows(int di, const bdem_Table& st, int si, int ne);
        //
        //   void removeRow(int index);
        //   void removeRows(int index, int rc);
        //   void removeAllRows();
        //   void removeAll();
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl
            << "Testing 'appendRow', 'insertRow', and 'removeRow'" << endl
            << "=================================================" << endl;

        if (verbose) cout <<
            "\nTesting x.insertRows(di, sa, si, ne) et al. (no aliasing)"
            << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_dtSpec;   // initial (destination) table
                int         d_di;       // index at which to insert into dt
                const char *d_stSpec;   // source table
                int         d_si;       // index at which to insert from st
                int         d_ne;       // number of elements to insert
                const char *d_expSpec;  // expected table value
            } DATA[] = {
                //line  d-table    di  s-table    si  ne  expected
                //----  -------    --  -------    --  --  --------
                { L_,   "",        0,  "",        0,  0,  ""         },
                { L_,   "",        0,  "",        0,  0,  ""         },
                { L_,   "a",       0,  "",        0,  0,  "a"        },
                { L_,   "a",       1,  "",        0,  0,  "a"        },
                { L_,   "",        0,  "a",       0,  0,  ""         },
                { L_,   "",        0,  "a",       0,  1,  "a"        },
                { L_,   "",        0,  "a",       1,  0,  ""         },
                { L_,   "ab",      0,  "",        0,  0,  "ab"       },
                { L_,   "ab",      1,  "",        0,  0,  "ab"       },
                { L_,   "ab",      2,  "",        0,  0,  "ab"       },
                { L_,   "a",       0,  "b",       0,  0,  "a"        },
                { L_,   "a",       0,  "b",       0,  1,  "ba"       },
                { L_,   "a",       0,  "b",       1,  0,  "a"        },
                { L_,   "a",       1,  "b",       0,  0,  "a"        },
                { L_,   "a",       1,  "b",       0,  1,  "ab"       },
                { L_,   "a",       1,  "b",       1,  0,  "a"        },
                { L_,   "",        0,  "ab",      0,  0,  ""         },
                { L_,   "",        0,  "ab",      0,  1,  "a"        },
                { L_,   "",        0,  "ab",      0,  2,  "ab"       },
                { L_,   "",        0,  "ab",      1,  0,  ""         },
                { L_,   "",        0,  "ab",      1,  1,  "b"        },
                { L_,   "",        0,  "ab",      2,  0,  ""         },
                { L_,   "abc",     0,  "",        0,  0,  "abc"      },
                { L_,   "abc",     1,  "",        0,  0,  "abc"      },
                { L_,   "abc",     2,  "",        0,  0,  "abc"      },
                { L_,   "abc",     3,  "",        0,  0,  "abc"      },
                { L_,   "ab",      0,  "c",       0,  0,  "ab"       },
                { L_,   "ab",      0,  "c",       0,  1,  "cab"      },
                { L_,   "ab",      0,  "c",       1,  0,  "ab"       },
                { L_,   "ab",      1,  "c",       0,  0,  "ab"       },
                { L_,   "ab",      1,  "c",       0,  1,  "acb"      },
                { L_,   "ab",      1,  "c",       1,  0,  "ab"       },
                { L_,   "ab",      2,  "c",       0,  0,  "ab"       },
                { L_,   "ab",      2,  "c",       0,  1,  "abc"      },
                { L_,   "ab",      2,  "c",       1,  0,  "ab"       },
                { L_,   "a",       0,  "bc",      0,  0,  "a"        },
                { L_,   "a",       0,  "bc",      0,  1,  "ba"       },
                { L_,   "a",       0,  "bc",      0,  2,  "bca"      },
                { L_,   "a",       0,  "bc",      1,  0,  "a"        },
                { L_,   "a",       0,  "bc",      1,  1,  "ca"       },
                { L_,   "a",       0,  "bc",      2,  0,  "a"        },
                { L_,   "a",       1,  "bc",      0,  0,  "a"        },
                { L_,   "a",       1,  "bc",      0,  1,  "ab"       },
                { L_,   "a",       1,  "bc",      0,  2,  "abc"      },
                { L_,   "a",       1,  "bc",      1,  0,  "a"        },
                { L_,   "a",       1,  "bc",      1,  1,  "ac"       },
                { L_,   "a",       1,  "bc",      2,  0,  "a"        },
                { L_,   "",        0,  "abc",     0,  0,  ""         },
                { L_,   "",        0,  "abc",     0,  1,  "a"        },
                { L_,   "",        0,  "abc",     0,  2,  "ab"       },
                { L_,   "",        0,  "abc",     0,  3,  "abc"      },
                { L_,   "",        0,  "abc",     1,  0,  ""         },
                { L_,   "",        0,  "abc",     1,  1,  "b"        },
                { L_,   "",        0,  "abc",     1,  2,  "bc"       },
                { L_,   "",        0,  "abc",     2,  0,  ""         },
                { L_,   "",        0,  "abc",     2,  1,  "c"        },
                { L_,   "",        0,  "abc",     3,  0,  ""         },
                { L_,   "abcd",    0,  "",        0,  0,  "abcd"     },
                { L_,   "abcd",    1,  "",        0,  0,  "abcd"     },
                { L_,   "abcd",    2,  "",        0,  0,  "abcd"     },
                { L_,   "abcd",    3,  "",        0,  0,  "abcd"     },
                { L_,   "abcd",    4,  "",        0,  0,  "abcd"     },
                { L_,   "abc",     0,  "d",       0,  0,  "abc"      },
                { L_,   "abc",     0,  "d",       0,  1,  "dabc"     },
                { L_,   "abc",     0,  "d",       1,  0,  "abc"      },
                { L_,   "abc",     1,  "d",       0,  0,  "abc"      },
                { L_,   "abc",     1,  "d",       0,  1,  "adbc"     },
                { L_,   "abc",     1,  "d",       1,  0,  "abc"      },
                { L_,   "abc",     2,  "d",       0,  0,  "abc"      },
                { L_,   "abc",     2,  "d",       0,  1,  "abdc"     },
                { L_,   "abc",     2,  "d",       1,  0,  "abc"      },
                { L_,   "abc",     3,  "d",       0,  0,  "abc"      },
                { L_,   "abc",     3,  "d",       0,  1,  "abcd"     },
                { L_,   "abc",     3,  "d",       1,  0,  "abc"      },
                { L_,   "ab",      0,  "cd",      0,  0,  "ab"       },
                { L_,   "ab",      0,  "cd",      0,  1,  "cab"      },
                { L_,   "ab",      0,  "cd",      0,  2,  "cdab"     },
                { L_,   "ab",      0,  "cd",      1,  0,  "ab"       },
                { L_,   "ab",      0,  "cd",      1,  1,  "dab"      },
                { L_,   "ab",      0,  "cd",      2,  0,  "ab"       },
                { L_,   "ab",      1,  "cd",      0,  0,  "ab"       },
                { L_,   "ab",      1,  "cd",      0,  1,  "acb"      },
                { L_,   "ab",      1,  "cd",      0,  2,  "acdb"     },
                { L_,   "ab",      1,  "cd",      1,  0,  "ab"       },
                { L_,   "ab",      1,  "cd",      1,  1,  "adb"      },
                { L_,   "ab",      1,  "cd",      2,  0,  "ab"       },
                { L_,   "ab",      2,  "cd",      0,  0,  "ab"       },
                { L_,   "ab",      2,  "cd",      0,  1,  "abc"      },
                { L_,   "ab",      2,  "cd",      0,  2,  "abcd"     },
                { L_,   "ab",      2,  "cd",      1,  0,  "ab"       },
                { L_,   "ab",      2,  "cd",      1,  1,  "abd"      },
                { L_,   "ab",      2,  "cd",      2,  0,  "ab"       },
                { L_,   "a",       0,  "bcd",     0,  0,  "a"        },
                { L_,   "a",       0,  "bcd",     0,  1,  "ba"       },
                { L_,   "a",       0,  "bcd",     0,  2,  "bca"      },
                { L_,   "a",       0,  "bcd",     0,  3,  "bcda"     },
                { L_,   "a",       0,  "bcd",     1,  0,  "a"        },
                { L_,   "a",       0,  "bcd",     1,  1,  "ca"       },
                { L_,   "a",       0,  "bcd",     1,  2,  "cda"      },
                { L_,   "a",       0,  "bcd",     2,  0,  "a"        },
                { L_,   "a",       0,  "bcd",     2,  1,  "da"       },
                { L_,   "a",       0,  "bcd",     3,  0,  "a"        },
                { L_,   "a",       1,  "bcd",     0,  0,  "a"        },
                { L_,   "a",       1,  "bcd",     0,  1,  "ab"       },
                { L_,   "a",       1,  "bcd",     0,  2,  "abc"      },
                { L_,   "a",       1,  "bcd",     0,  3,  "abcd"     },
                { L_,   "a",       1,  "bcd",     1,  0,  "a"        },
                { L_,   "a",       1,  "bcd",     1,  1,  "ac"       },
                { L_,   "a",       1,  "bcd",     1,  2,  "acd"      },
                { L_,   "a",       1,  "bcd",     2,  0,  "a"        },
                { L_,   "a",       1,  "bcd",     2,  1,  "ad"       },
                { L_,   "a",       1,  "bcd",     3,  0,  "a"        },
                { L_,   "",        0,  "abcd",    0,  0,  ""         },
                { L_,   "",        0,  "abcd",    0,  1,  "a"        },
                { L_,   "",        0,  "abcd",    0,  2,  "ab"       },
                { L_,   "",        0,  "abcd",    0,  3,  "abc"      },
                { L_,   "",        0,  "abcd",    0,  4,  "abcd"     },
                { L_,   "",        0,  "abcd",    1,  0,  ""         },
                { L_,   "",        0,  "abcd",    1,  1,  "b"        },
                { L_,   "",        0,  "abcd",    1,  2,  "bc"       },
                { L_,   "",        0,  "abcd",    1,  3,  "bcd"      },
                { L_,   "",        0,  "abcd",    2,  0,  ""         },
                { L_,   "",        0,  "abcd",    2,  1,  "c"        },
                { L_,   "",        0,  "abcd",    2,  2,  "cd"       },
                { L_,   "",        0,  "abcd",    3,  0,  ""         },
                { L_,   "",        0,  "abcd",    3,  1,  "d"        },
                { L_,   "",        0,  "abcd",    4,  0,  ""         },
                { L_,   "abcde",   0,  "",        0,  0,  "abcde"    },
                { L_,   "abcde",   1,  "",        0,  0,  "abcde"    },
                { L_,   "abcde",   2,  "",        0,  0,  "abcde"    },
                { L_,   "abcde",   3,  "",        0,  0,  "abcde"    },
                { L_,   "abcde",   4,  "",        0,  0,  "abcde"    },
                { L_,   "abcde",   5,  "",        0,  0,  "abcde"    },
                { L_,   "abcd",    0,  "e",       0,  0,  "abcd"     },
                { L_,   "abcd",    0,  "e",       0,  1,  "eabcd"    },
                { L_,   "abcd",    0,  "e",       1,  0,  "abcd"     },
                { L_,   "abcd",    1,  "e",       0,  0,  "abcd"     },
                { L_,   "abcd",    1,  "e",       0,  1,  "aebcd"    },
                { L_,   "abcd",    1,  "e",       1,  0,  "abcd"     },
                { L_,   "abcd",    2,  "e",       0,  0,  "abcd"     },
                { L_,   "abcd",    2,  "e",       0,  1,  "abecd"    },
                { L_,   "abcd",    2,  "e",       1,  0,  "abcd"     },
                { L_,   "abcd",    3,  "e",       0,  0,  "abcd"     },
                { L_,   "abcd",    3,  "e",       0,  1,  "abced"    },
                { L_,   "abcd",    3,  "e",       1,  0,  "abcd"     },
                { L_,   "abcd",    4,  "e",       0,  0,  "abcd"     },
                { L_,   "abcd",    4,  "e",       0,  1,  "abcde"    },
                { L_,   "abcd",    4,  "e",       1,  0,  "abcd"     },
                { L_,   "abc",     0,  "de",      0,  0,  "abc"      },
                { L_,   "abc",     0,  "de",      0,  1,  "dabc"     },
                { L_,   "abc",     0,  "de",      0,  2,  "deabc"    },
                { L_,   "abc",     0,  "de",      1,  0,  "abc"      },
                { L_,   "abc",     0,  "de",      1,  1,  "eabc"     },
                { L_,   "abc",     0,  "de",      2,  0,  "abc"      },
                { L_,   "abc",     1,  "de",      0,  0,  "abc"      },
                { L_,   "abc",     1,  "de",      0,  1,  "adbc"     },
                { L_,   "abc",     1,  "de",      0,  2,  "adebc"    },
                { L_,   "abc",     1,  "de",      1,  0,  "abc"      },
                { L_,   "abc",     1,  "de",      1,  1,  "aebc"     },
                { L_,   "abc",     1,  "de",      2,  0,  "abc"      },
                { L_,   "abc",     2,  "de",      0,  0,  "abc"      },
                { L_,   "abc",     2,  "de",      0,  1,  "abdc"     },
                { L_,   "abc",     2,  "de",      0,  2,  "abdec"    },
                { L_,   "abc",     2,  "de",      1,  0,  "abc"      },
                { L_,   "abc",     2,  "de",      1,  1,  "abec"     },
                { L_,   "abc",     2,  "de",      2,  0,  "abc"      },
                { L_,   "abc",     3,  "de",      0,  0,  "abc"      },
                { L_,   "abc",     3,  "de",      0,  1,  "abcd"     },
                { L_,   "abc",     3,  "de",      0,  2,  "abcde"    },
                { L_,   "abc",     3,  "de",      1,  0,  "abc"      },
                { L_,   "abc",     3,  "de",      1,  1,  "abce"     },
                { L_,   "abc",     3,  "de",      2,  0,  "abc"      },
                { L_,   "ab",      0,  "cde",     0,  0,  "ab"       },
                { L_,   "ab",      0,  "cde",     0,  1,  "cab"      },
                { L_,   "ab",      0,  "cde",     0,  2,  "cdab"     },
                { L_,   "ab",      0,  "cde",     0,  3,  "cdeab"    },
                { L_,   "ab",      0,  "cde",     1,  0,  "ab"       },
                { L_,   "ab",      0,  "cde",     1,  1,  "dab"      },
                { L_,   "ab",      0,  "cde",     1,  2,  "deab"     },
                { L_,   "ab",      0,  "cde",     2,  0,  "ab"       },
                { L_,   "ab",      0,  "cde",     2,  1,  "eab"      },
                { L_,   "ab",      0,  "cde",     3,  0,  "ab"       },
                { L_,   "ab",      1,  "cde",     0,  0,  "ab"       },
                { L_,   "ab",      1,  "cde",     0,  1,  "acb"      },
                { L_,   "ab",      1,  "cde",     0,  2,  "acdb"     },
                { L_,   "ab",      1,  "cde",     0,  3,  "acdeb"    },
                { L_,   "ab",      1,  "cde",     1,  0,  "ab"       },
                { L_,   "ab",      1,  "cde",     1,  1,  "adb"      },
                { L_,   "ab",      1,  "cde",     1,  2,  "adeb"     },
                { L_,   "ab",      1,  "cde",     2,  0,  "ab"       },
                { L_,   "ab",      1,  "cde",     2,  1,  "aeb"      },
                { L_,   "ab",      1,  "cde",     3,  0,  "ab"       },
                { L_,   "ab",      2,  "cde",     0,  0,  "ab"       },
                { L_,   "ab",      2,  "cde",     0,  1,  "abc"      },
                { L_,   "ab",      2,  "cde",     0,  2,  "abcd"     },
                { L_,   "ab",      2,  "cde",     0,  3,  "abcde"    },
                { L_,   "ab",      2,  "cde",     1,  0,  "ab"       },
                { L_,   "ab",      2,  "cde",     1,  1,  "abd"      },
                { L_,   "ab",      2,  "cde",     1,  2,  "abde"     },
                { L_,   "ab",      2,  "cde",     2,  0,  "ab"       },
                { L_,   "ab",      2,  "cde",     2,  1,  "abe"      },
                { L_,   "ab",      2,  "cde",     3,  0,  "ab"       },
                { L_,   "a",       0,  "bcde",    0,  0,  "a"        },
                { L_,   "a",       0,  "bcde",    0,  1,  "ba"       },
                { L_,   "a",       0,  "bcde",    0,  2,  "bca"      },
                { L_,   "a",       0,  "bcde",    0,  3,  "bcda"     },
                { L_,   "a",       0,  "bcde",    1,  0,  "a"        },
                { L_,   "a",       0,  "bcde",    1,  1,  "ca"       },
                { L_,   "a",       0,  "bcde",    1,  2,  "cda"      },
                { L_,   "a",       0,  "bcde",    2,  0,  "a"        },
                { L_,   "a",       0,  "bcde",    2,  1,  "da"       },
                { L_,   "a",       0,  "bcde",    3,  0,  "a"        },
                { L_,   "a",       1,  "bcde",    0,  0,  "a"        },
                { L_,   "a",       1,  "bcde",    0,  1,  "ab"       },
                { L_,   "a",       1,  "bcde",    0,  2,  "abc"      },
                { L_,   "a",       1,  "bcde",    0,  3,  "abcd"     },
                { L_,   "a",       1,  "bcde",    1,  0,  "a"        },
                { L_,   "a",       1,  "bcde",    1,  1,  "ac"       },
                { L_,   "a",       1,  "bcde",    1,  2,  "acd"      },
                { L_,   "a",       1,  "bcde",    2,  0,  "a"        },
                { L_,   "a",       1,  "bcde",    2,  1,  "ad"       },
                { L_,   "a",       1,  "bcde",    3,  0,  "a"        },
                { L_,   "",        0,  "abcde",   0,  0,  ""         },
                { L_,   "",        0,  "abcde",   0,  1,  "a"        },
                { L_,   "",        0,  "abcde",   0,  2,  "ab"       },
                { L_,   "",        0,  "abcde",   0,  3,  "abc"      },
                { L_,   "",        0,  "abcde",   0,  4,  "abcd"     },
                { L_,   "",        0,  "abcde",   0,  5,  "abcde"    },
                { L_,   "",        0,  "abcde",   1,  0,  ""         },
                { L_,   "",        0,  "abcde",   1,  1,  "b"        },
                { L_,   "",        0,  "abcde",   1,  2,  "bc"       },
                { L_,   "",        0,  "abcde",   1,  3,  "bcd"      },
                { L_,   "",        0,  "abcde",   1,  4,  "bcde"     },
                { L_,   "",        0,  "abcde",   2,  0,  ""         },
                { L_,   "",        0,  "abcde",   2,  1,  "c"        },
                { L_,   "",        0,  "abcde",   2,  2,  "cd"       },
                { L_,   "",        0,  "abcde",   2,  3,  "cde"      },
                { L_,   "",        0,  "abcde",   3,  0,  ""         },
                { L_,   "",        0,  "abcde",   3,  1,  "d"        },
                { L_,   "",        0,  "abcde",   3,  2,  "de"       },
                { L_,   "",        0,  "abcde",   4,  0,  ""         },
                { L_,   "",        0,  "abcde",   4,  1,  "e"        },
                { L_,   "",        0,  "abcde",   5,  0,  ""         },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            const char *COLSPEC = "UCTE";

            const Table MT;    // empty Table; for removeAll() tests
            ASSERT(0 == MT.numRows() && 0 == MT.numColumns());

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *D_SPEC = DATA[ti].d_dtSpec;
                const int   DI     = DATA[ti].d_di;
                const char *S_SPEC = DATA[ti].d_stSpec;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = (int)strlen(D_SPEC) + (int)strlen(S_SPEC);
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                Table DD(hx(COLSPEC, D_SPEC));    // control for destination
                Table SS(hx(COLSPEC, S_SPEC));    // control for source
                Table EE(hx(COLSPEC, E_SPEC));    // control for expected value

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(D_SPEC); P_(DI); P_(S_SPEC);
                                                P_(SI); P_(NE); P(E_SPEC);
                    cout << "\t\t"; PR(DD);
                    cout << "\t\t"; PR(SS);
                    cout << "\t\t"; PR(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                if (veryVerbose)
                    cout << "\t\tinsertRows(di, sa, si, ne)" << endl;
                {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Table x(DD, &testAllocator);  const Table &X = x;
                    {
                        Table s(SS, &testAllocator);  const Table &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; PR(X); }
                        testAllocator.setAllocationLimit(AL);
                        x.insertRows(DI, s, SI, NE);    // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; PR(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tinsertRows(di, sa)" << endl;
                if ((int) strlen(S_SPEC) == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Table x(DD, &testAllocator);  const Table &X = x;
                    {
                        Table s(SS, &testAllocator);  const Table &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; PR(X); }
                        testAllocator.setAllocationLimit(AL);
                        x.insertRows(DI, s);          // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; PR(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tinsertRow(di, list)" << endl;
                if (1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Table x(DD, &testAllocator);  const Table &X = x;
                    {
                        List s(SS[SI], &testAllocator);
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; PR(X); }
                        testAllocator.setAllocationLimit(AL);
                        x.insertRow(DI, s);  // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; PR(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        //LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BSLMA_EXCEPTION_TEST
                }

                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose) cout << "\t\tappendRows(sa, si, ne)" << endl;
                if ((int) strlen(D_SPEC) == DI) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Table x(DD, &testAllocator);  const Table& X = x;
                    {
                        Table s(SS, &testAllocator);  const Table &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; PR(X); }
                        testAllocator.setAllocationLimit(AL);
                        x.appendRows(s, SI, NE);        // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; PR(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tappendRows(sa)" << endl;
                if ((int) strlen(D_SPEC) == DI && (int) strlen(S_SPEC) == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Table x(DD, &testAllocator);  const Table &X = x;
                    {
                        Table s(SS, &testAllocator);  const Table &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; PR(X); }
                        testAllocator.setAllocationLimit(AL);
                        x.appendRows(s);                // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; PR(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tappendRow(table, idx)" << endl;
                if ((int) strlen(D_SPEC) == DI && 1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Table x(DD, &testAllocator);  const Table &X = x;
                    {
                        Table s(SS, &testAllocator);  const Table &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; PR(X); }
                        testAllocator.setAllocationLimit(AL);
                        x.appendRow(s, SI);      // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; PR(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tappendRow(list)" << endl;
                if ((int) strlen(D_SPEC) == DI && 1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Table x(DD, &testAllocator);  const Table &X = x;
                    {
                        List s(SS[SI], &testAllocator);
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; PR(X); }
                        testAllocator.setAllocationLimit(AL);
                        x.appendRow(s);           // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; PR(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        //LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BSLMA_EXCEPTION_TEST
                }
                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose) cout << "\t\tremoveRows(index, ne)" << endl;
                {
                  BEGIN_BSLMA_EXCEPTION_TEST {  // Note specs are switched.
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Table x(EE, &testAllocator);  const Table &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; PR(X); }
                    testAllocator.setAllocationLimit(AL);
                    x.removeRows(DI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; PR(X); }
                    LOOP_ASSERT(LINE, DD == X);

                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tremoveAllRows()" << endl;
                if (DI == 0 && strlen(D_SPEC) == 0)
                {
                  BEGIN_BSLMA_EXCEPTION_TEST {  // Note specs are switched.
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Table x(EE, &testAllocator);  const Table &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; PR(X); }
                    testAllocator.setAllocationLimit(AL);
                    x.removeAllRows();
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; PR(X); }
                    LOOP_ASSERT(LINE, DD == X);
                  } END_BSLMA_EXCEPTION_TEST
                }

                // verify that table can be used after removing all rows
                if (veryVerbose) cout << "\t\tremoveAllRows() use row" << endl;
                if (DI == 0 && strlen(D_SPEC) == 0)
                {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Table x(EE, &testAllocator);  const Table &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; PR(X); }
                    testAllocator.setAllocationLimit(AL);
                    x.removeAllRows();
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; PR(X); }
                    LOOP_ASSERT(LINE, DD == X);

                    x.appendNullRow();
                }

                if (veryVerbose) cout << "\t\tremoveRow(index)" << endl;
                if (1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {  // Note specs are switched
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Table x(EE, &testAllocator);  const Table &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; PR(X); }
                    testAllocator.setAllocationLimit(AL);
                    x.removeRow(DI);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; PR(X); }
                    LOOP_ASSERT(LINE, DD == X);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tremoveAll()" << endl;
                {
                  BEGIN_BSLMA_EXCEPTION_TEST {  // Note specs are switched
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Table x(EE, &testAllocator);  const Table &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; PR(X); }
                    testAllocator.setAllocationLimit(AL);
                    x.removeAll();
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; PR(X); }
                    LOOP_ASSERT(LINE, MT == X);
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }

        //---------------------------------------------------------------------

        if (verbose) cout <<
            "\nTesting x.insertRows(di, sa, si, ne) et al. (aliasing)"
            << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_xSpec;    // initial table (= srcTable)
                int         d_di;       // index at which to insert into x
                int         d_si;       // index at which to insert from x
                int         d_ne;       // number of elements to insert
                const char *d_expSpec;  // expected table value
            } DATA[] = {
                //line  x-table   di   si  ne  expected
                //----  -------   --   --  --  --------   Depth = 0
                { L_,   "",       0,   0,  0,  ""           },

                //line  x-table   di   si  ne  expected
                //----  -------   --   --  --  --------   Depth = 1
                { L_,   "a",      0,   0,  0,  "a"          },
                { L_,   "a",      0,   0,  1,  "aa"         },
                { L_,   "a",      0,   1,  0,  "a"          },

                { L_,   "a",      1,   0,  0,  "a"          },
                { L_,   "a",      1,   0,  1,  "aa"         },
                { L_,   "a",      1,   1,  0,  "a"          },

                //line  x-table   di   si  ne  expected
                //----  -------   --   --  --  --------   Depth = 2
                { L_,   "ab",     0,   0,  0,  "ab"         },
                { L_,   "ab",     0,   0,  1,  "aab"        },
                { L_,   "ab",     0,   0,  2,  "abab"       },
                { L_,   "ab",     0,   1,  0,  "ab"         },
                { L_,   "ab",     0,   1,  1,  "bab"        },
                { L_,   "ab",     0,   2,  0,  "ab"         },

                { L_,   "ab",     1,   0,  0,  "ab"         },
                { L_,   "ab",     1,   0,  1,  "aab"        },
                { L_,   "ab",     1,   0,  2,  "aabb"       },
                { L_,   "ab",     1,   1,  0,  "ab"         },
                { L_,   "ab",     1,   1,  1,  "abb"        },
                { L_,   "ab",     1,   2,  0,  "ab"         },

                { L_,   "ab",     2,   0,  0,  "ab"         },
                { L_,   "ab",     2,   0,  1,  "aba"        },
                { L_,   "ab",     2,   0,  2,  "abab"       },
                { L_,   "ab",     2,   1,  0,  "ab"         },
                { L_,   "ab",     2,   1,  1,  "abb"        },
                { L_,   "ab",     2,   2,  0,  "ab"         },

                //line  x-table   di   si  ne  expected
                //----  -------   --   --  --  --------   Depth = 3
                { L_,   "abc",    0,   0,  0,  "abc"        },
                { L_,   "abc",    0,   0,  1,  "aabc"       },
                { L_,   "abc",    0,   0,  2,  "ababc"      },
                { L_,   "abc",    0,   0,  3,  "abcabc"     },
                { L_,   "abc",    0,   1,  0,  "abc"        },
                { L_,   "abc",    0,   1,  1,  "babc"       },
                { L_,   "abc",    0,   1,  2,  "bcabc"      },
                { L_,   "abc",    0,   2,  0,  "abc"        },
                { L_,   "abc",    0,   2,  1,  "cabc"       },
                { L_,   "abc",    0,   3,  0,  "abc"        },

                { L_,   "abc",    1,   0,  0,  "abc"        },
                { L_,   "abc",    1,   0,  1,  "aabc"       },
                { L_,   "abc",    1,   0,  2,  "aabbc"      },
                { L_,   "abc",    1,   0,  3,  "aabcbc"     },
                { L_,   "abc",    1,   1,  0,  "abc"        },
                { L_,   "abc",    1,   1,  1,  "abbc"       },
                { L_,   "abc",    1,   1,  2,  "abcbc"      },
                { L_,   "abc",    1,   2,  0,  "abc"        },
                { L_,   "abc",    1,   2,  1,  "acbc"       },
                { L_,   "abc",    1,   3,  0,  "abc"        },

                { L_,   "abc",    2,   0,  0,  "abc"        },
                { L_,   "abc",    2,   0,  1,  "abac"       },
                { L_,   "abc",    2,   0,  2,  "ababc"      },
                { L_,   "abc",    2,   0,  3,  "ababcc"     },
                { L_,   "abc",    2,   1,  0,  "abc"        },
                { L_,   "abc",    2,   1,  1,  "abbc"       },
                { L_,   "abc",    2,   1,  2,  "abbcc"      },
                { L_,   "abc",    2,   2,  0,  "abc"        },
                { L_,   "abc",    2,   2,  1,  "abcc"       },
                { L_,   "abc",    2,   3,  0,  "abc"        },

                { L_,   "abc",    3,   0,  0,  "abc"        },
                { L_,   "abc",    3,   0,  1,  "abca"       },
                { L_,   "abc",    3,   0,  2,  "abcab"      },
                { L_,   "abc",    3,   0,  3,  "abcabc"     },
                { L_,   "abc",    3,   1,  0,  "abc"        },
                { L_,   "abc",    3,   1,  1,  "abcb"       },
                { L_,   "abc",    3,   1,  2,  "abcbc"      },
                { L_,   "abc",    3,   2,  0,  "abc"        },
                { L_,   "abc",    3,   2,  1,  "abcc"       },
                { L_,   "abc",    3,   3,  0,  "abc"        },

                //line  x-table   di   si  ne  expected
                //----  -------   --   --  --  --------   Depth = 4
                { L_,   "abcd",   0,   0,  0,  "abcd"       },
                { L_,   "abcd",   0,   0,  1,  "aabcd"      },
                { L_,   "abcd",   0,   0,  2,  "ababcd"     },
                { L_,   "abcd",   0,   0,  3,  "abcabcd"    },
                { L_,   "abcd",   0,   0,  4,  "abcdabcd"   },
                { L_,   "abcd",   0,   1,  0,  "abcd"       },
                { L_,   "abcd",   0,   1,  1,  "babcd"      },
                { L_,   "abcd",   0,   1,  2,  "bcabcd"     },
                { L_,   "abcd",   0,   1,  3,  "bcdabcd"    },
                { L_,   "abcd",   0,   2,  0,  "abcd"       },
                { L_,   "abcd",   0,   2,  1,  "cabcd"      },
                { L_,   "abcd",   0,   2,  2,  "cdabcd"     },
                { L_,   "abcd",   0,   3,  0,  "abcd"       },
                { L_,   "abcd",   0,   3,  1,  "dabcd"      },
                { L_,   "abcd",   0,   4,  0,  "abcd"       },

                { L_,   "abcd",   1,   0,  0,  "abcd"       },
                { L_,   "abcd",   1,   0,  1,  "aabcd"      },
                { L_,   "abcd",   1,   0,  2,  "aabbcd"     },
                { L_,   "abcd",   1,   0,  3,  "aabcbcd"    },
                { L_,   "abcd",   1,   0,  4,  "aabcdbcd"   },
                { L_,   "abcd",   1,   1,  0,  "abcd"       },
                { L_,   "abcd",   1,   1,  1,  "abbcd"      },
                { L_,   "abcd",   1,   1,  2,  "abcbcd"     },
                { L_,   "abcd",   1,   1,  3,  "abcdbcd"    },
                { L_,   "abcd",   1,   2,  0,  "abcd"       },
                { L_,   "abcd",   1,   2,  1,  "acbcd"      },
                { L_,   "abcd",   1,   2,  2,  "acdbcd"     },
                { L_,   "abcd",   1,   3,  0,  "abcd"       },
                { L_,   "abcd",   1,   3,  1,  "adbcd"      },
                { L_,   "abcd",   1,   4,  0,  "abcd"       },

                { L_,   "abcd",   2,   0,  0,  "abcd"       },
                { L_,   "abcd",   2,   0,  1,  "abacd"      },
                { L_,   "abcd",   2,   0,  2,  "ababcd"     },
                { L_,   "abcd",   2,   0,  3,  "ababccd"    },
                { L_,   "abcd",   2,   0,  4,  "ababcdcd"   },
                { L_,   "abcd",   2,   1,  0,  "abcd"       },
                { L_,   "abcd",   2,   1,  1,  "abbcd"      },
                { L_,   "abcd",   2,   1,  2,  "abbccd"     },
                { L_,   "abcd",   2,   1,  3,  "abbcdcd"    },
                { L_,   "abcd",   2,   2,  0,  "abcd"       },
                { L_,   "abcd",   2,   2,  1,  "abccd"      },
                { L_,   "abcd",   2,   2,  2,  "abcdcd"     },
                { L_,   "abcd",   2,   3,  0,  "abcd"       },
                { L_,   "abcd",   2,   3,  1,  "abdcd"      },
                { L_,   "abcd",   2,   4,  0,  "abcd"       },

                { L_,   "abcd",   3,   0,  0,  "abcd"       },
                { L_,   "abcd",   3,   0,  1,  "abcad"      },
                { L_,   "abcd",   3,   0,  2,  "abcabd"     },
                { L_,   "abcd",   3,   0,  3,  "abcabcd"    },
                { L_,   "abcd",   3,   0,  4,  "abcabcdd"   },
                { L_,   "abcd",   3,   1,  0,  "abcd"       },
                { L_,   "abcd",   3,   1,  1,  "abcbd"      },
                { L_,   "abcd",   3,   1,  2,  "abcbcd"     },
                { L_,   "abcd",   3,   1,  3,  "abcbcdd"    },
                { L_,   "abcd",   3,   2,  0,  "abcd"       },
                { L_,   "abcd",   3,   2,  1,  "abccd"      },
                { L_,   "abcd",   3,   2,  2,  "abccdd"     },
                { L_,   "abcd",   3,   3,  0,  "abcd"       },
                { L_,   "abcd",   3,   3,  1,  "abcdd"      },
                { L_,   "abcd",   3,   4,  0,  "abcd"       },

                { L_,   "abcd",   4,   0,  0,  "abcd"       },
                { L_,   "abcd",   4,   0,  1,  "abcda"      },
                { L_,   "abcd",   4,   0,  2,  "abcdab"     },
                { L_,   "abcd",   4,   0,  3,  "abcdabc"    },
                { L_,   "abcd",   4,   0,  4,  "abcdabcd"   },
                { L_,   "abcd",   4,   1,  0,  "abcd"       },
                { L_,   "abcd",   4,   1,  1,  "abcdb"      },
                { L_,   "abcd",   4,   1,  2,  "abcdbc"     },
                { L_,   "abcd",   4,   1,  3,  "abcdbcd"    },
                { L_,   "abcd",   4,   2,  0,  "abcd"       },
                { L_,   "abcd",   4,   2,  1,  "abcdc"      },
                { L_,   "abcd",   4,   2,  2,  "abcdcd"     },
                { L_,   "abcd",   4,   3,  0,  "abcd"       },
                { L_,   "abcd",   4,   3,  1,  "abcdd"      },
                { L_,   "abcd",   4,   4,  0,  "abcd"       },

                //line  x-table   di   si  ne  expected
                //----  -------   --   --  --  --------   Depth = 5
                { L_,   "abcde",  0,   0,  0,  "abcde"      },
                { L_,   "abcde",  0,   0,  1,  "aabcde"     },
                { L_,   "abcde",  0,   0,  2,  "ababcde"    },
                { L_,   "abcde",  0,   0,  3,  "abcabcde"   },
                { L_,   "abcde",  0,   0,  4,  "abcdabcde"  },
                { L_,   "abcde",  0,   0,  5,  "abcdeabcde" },
                { L_,   "abcde",  0,   1,  0,  "abcde"      },
                { L_,   "abcde",  0,   1,  1,  "babcde"     },
                { L_,   "abcde",  0,   1,  2,  "bcabcde"    },
                { L_,   "abcde",  0,   1,  3,  "bcdabcde"   },
                { L_,   "abcde",  0,   1,  4,  "bcdeabcde"  },
                { L_,   "abcde",  0,   2,  0,  "abcde"      },
                { L_,   "abcde",  0,   2,  1,  "cabcde"     },
                { L_,   "abcde",  0,   2,  2,  "cdabcde"    },
                { L_,   "abcde",  0,   2,  3,  "cdeabcde"   },
                { L_,   "abcde",  0,   3,  0,  "abcde"      },
                { L_,   "abcde",  0,   3,  1,  "dabcde"     },
                { L_,   "abcde",  0,   3,  2,  "deabcde"    },
                { L_,   "abcde",  0,   4,  0,  "abcde"      },
                { L_,   "abcde",  0,   4,  1,  "eabcde"     },
                { L_,   "abcde",  0,   5,  0,  "abcde"      },

                { L_,   "abcde",  1,   0,  0,  "abcde"      },
                { L_,   "abcde",  1,   0,  1,  "aabcde"     },
                { L_,   "abcde",  1,   0,  2,  "aabbcde"    },
                { L_,   "abcde",  1,   0,  3,  "aabcbcde"   },
                { L_,   "abcde",  1,   0,  4,  "aabcdbcde"  },
                { L_,   "abcde",  1,   0,  5,  "aabcdebcde" },
                { L_,   "abcde",  1,   1,  0,  "abcde"      },
                { L_,   "abcde",  1,   1,  1,  "abbcde"     },
                { L_,   "abcde",  1,   1,  2,  "abcbcde"    },
                { L_,   "abcde",  1,   1,  3,  "abcdbcde"   },
                { L_,   "abcde",  1,   1,  4,  "abcdebcde"  },
                { L_,   "abcde",  1,   2,  0,  "abcde"      },
                { L_,   "abcde",  1,   2,  1,  "acbcde"     },
                { L_,   "abcde",  1,   2,  2,  "acdbcde"    },
                { L_,   "abcde",  1,   2,  3,  "acdebcde"   },
                { L_,   "abcde",  1,   3,  0,  "abcde"      },
                { L_,   "abcde",  1,   3,  1,  "adbcde"     },
                { L_,   "abcde",  1,   3,  2,  "adebcde"    },
                { L_,   "abcde",  1,   4,  0,  "abcde"      },
                { L_,   "abcde",  1,   4,  1,  "aebcde"     },
                { L_,   "abcde",  1,   5,  0,  "abcde"      },

                { L_,   "abcde",  2,   0,  0,  "abcde"      },
                { L_,   "abcde",  2,   0,  1,  "abacde"     },
                { L_,   "abcde",  2,   0,  2,  "ababcde"    },
                { L_,   "abcde",  2,   0,  3,  "ababccde"   },
                { L_,   "abcde",  2,   0,  4,  "ababcdcde"  },
                { L_,   "abcde",  2,   0,  5,  "ababcdecde" },
                { L_,   "abcde",  2,   1,  0,  "abcde"      },
                { L_,   "abcde",  2,   1,  1,  "abbcde"     },
                { L_,   "abcde",  2,   1,  2,  "abbccde"    },
                { L_,   "abcde",  2,   1,  3,  "abbcdcde"   },
                { L_,   "abcde",  2,   1,  4,  "abbcdecde"  },
                { L_,   "abcde",  2,   2,  0,  "abcde"      },
                { L_,   "abcde",  2,   2,  1,  "abccde"     },
                { L_,   "abcde",  2,   2,  2,  "abcdcde"    },
                { L_,   "abcde",  2,   2,  3,  "abcdecde"   },
                { L_,   "abcde",  2,   3,  0,  "abcde"      },
                { L_,   "abcde",  2,   3,  1,  "abdcde"     },
                { L_,   "abcde",  2,   3,  2,  "abdecde"    },
                { L_,   "abcde",  2,   4,  0,  "abcde"      },
                { L_,   "abcde",  2,   4,  1,  "abecde"     },
                { L_,   "abcde",  2,   5,  0,  "abcde"      },

                { L_,   "abcde",  3,   0,  0,  "abcde"      },
                { L_,   "abcde",  3,   0,  1,  "abcade"     },
                { L_,   "abcde",  3,   0,  2,  "abcabde"    },
                { L_,   "abcde",  3,   0,  3,  "abcabcde"   },
                { L_,   "abcde",  3,   0,  4,  "abcabcdde"  },
                { L_,   "abcde",  3,   0,  5,  "abcabcdede" },
                { L_,   "abcde",  3,   1,  0,  "abcde"      },
                { L_,   "abcde",  3,   1,  1,  "abcbde"     },
                { L_,   "abcde",  3,   1,  2,  "abcbcde"    },
                { L_,   "abcde",  3,   1,  3,  "abcbcdde"   },
                { L_,   "abcde",  3,   1,  4,  "abcbcdede"  },
                { L_,   "abcde",  3,   2,  0,  "abcde"      },
                { L_,   "abcde",  3,   2,  1,  "abccde"     },
                { L_,   "abcde",  3,   2,  2,  "abccdde"    },
                { L_,   "abcde",  3,   2,  3,  "abccdede"   },
                { L_,   "abcde",  3,   3,  0,  "abcde"      },
                { L_,   "abcde",  3,   3,  1,  "abcdde"     },
                { L_,   "abcde",  3,   3,  2,  "abcdede"    },
                { L_,   "abcde",  3,   4,  0,  "abcde"      },
                { L_,   "abcde",  3,   4,  1,  "abcede"     },
                { L_,   "abcde",  3,   5,  0,  "abcde"      },

                { L_,   "abcde",  4,   0,  0,  "abcde"      },
                { L_,   "abcde",  4,   0,  1,  "abcdae"     },
                { L_,   "abcde",  4,   0,  2,  "abcdabe"    },
                { L_,   "abcde",  4,   0,  3,  "abcdabce"   },
                { L_,   "abcde",  4,   0,  4,  "abcdabcde"  },
                { L_,   "abcde",  4,   0,  5,  "abcdabcdee" },
                { L_,   "abcde",  4,   1,  0,  "abcde"      },
                { L_,   "abcde",  4,   1,  1,  "abcdbe"     },
                { L_,   "abcde",  4,   1,  2,  "abcdbce"    },
                { L_,   "abcde",  4,   1,  3,  "abcdbcde"   },
                { L_,   "abcde",  4,   1,  4,  "abcdbcdee"  },
                { L_,   "abcde",  4,   2,  0,  "abcde"      },
                { L_,   "abcde",  4,   2,  1,  "abcdce"     },
                { L_,   "abcde",  4,   2,  2,  "abcdcde"    },
                { L_,   "abcde",  4,   2,  3,  "abcdcdee"   },
                { L_,   "abcde",  4,   3,  0,  "abcde"      },
                { L_,   "abcde",  4,   3,  1,  "abcdde"     },
                { L_,   "abcde",  4,   3,  2,  "abcddee"    },
                { L_,   "abcde",  4,   4,  0,  "abcde"      },
                { L_,   "abcde",  4,   4,  1,  "abcdee"     },
                { L_,   "abcde",  4,   5,  0,  "abcde"      },

                { L_,   "abcde",  5,   0,  0,  "abcde"      },
                { L_,   "abcde",  5,   0,  1,  "abcdea"     },
                { L_,   "abcde",  5,   0,  2,  "abcdeab"    },
                { L_,   "abcde",  5,   0,  3,  "abcdeabc"   },
                { L_,   "abcde",  5,   0,  4,  "abcdeabcd"  },
                { L_,   "abcde",  5,   0,  5,  "abcdeabcde" },
                { L_,   "abcde",  5,   1,  0,  "abcde"      },
                { L_,   "abcde",  5,   1,  1,  "abcdeb"     },
                { L_,   "abcde",  5,   1,  2,  "abcdebc"    },
                { L_,   "abcde",  5,   1,  3,  "abcdebcd"   },
                { L_,   "abcde",  5,   1,  4,  "abcdebcde"  },
                { L_,   "abcde",  5,   2,  0,  "abcde"      },
                { L_,   "abcde",  5,   2,  1,  "abcdec"     },
                { L_,   "abcde",  5,   2,  2,  "abcdecd"    },
                { L_,   "abcde",  5,   2,  3,  "abcdecde"   },
                { L_,   "abcde",  5,   3,  0,  "abcde"      },
                { L_,   "abcde",  5,   3,  1,  "abcded"     },
                { L_,   "abcde",  5,   3,  2,  "abcdede"    },
                { L_,   "abcde",  5,   4,  0,  "abcde"      },
                { L_,   "abcde",  5,   4,  1,  "abcdee"     },
                { L_,   "abcde",  5,   5,  0,  "abcde"      },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            const char *COLSPEC = "UCTE";

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *X_SPEC = DATA[ti].d_xSpec;
                const int   DI     = DATA[ti].d_di;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = (int) strlen(X_SPEC);
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                Table DD(hx(COLSPEC, X_SPEC));    // control for destination
                Table EE(hx(COLSPEC, E_SPEC));    // control for expected value

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(X_SPEC); P_(DI);
                                                P_(SI); P_(NE); P(E_SPEC);
                    cout << "\t\t"; PR(DD);
                    cout << "\t\t"; PR(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                // CONTROL: ensure same table result as non-alias case.
                {
                    Table x(DD, &testAllocator);  const Table &X = x;
                    Table x2(DD, &testAllocator); // control
                    const Table &X2 = x2;
                    x2.insertRows(DI, X, SI, NE);
                    LOOP_ASSERT(LINE, EE == X2);
                }

                if (veryVerbose) cout << "\t\tinsertRows(di, sa, si, ne)"
                                      << endl;
                {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Table x(DD, &testAllocator);  const Table &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; PR(X); }
                    testAllocator.setAllocationLimit(AL);
                    x.insertRows(DI, X, SI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; PR(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tinsertRows(di, sa)" << endl;
                if ((int) strlen(X_SPEC) == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Table x(DD, &testAllocator);  const Table &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; PR(X); }
                    testAllocator.setAllocationLimit(AL);
                    x.insertRows(DI, X);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; PR(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tinsertRow(di, table, si)" <<endl;
                if (1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Table x(DD, &testAllocator);  const Table &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; PR(X); }
                    testAllocator.setAllocationLimit(AL);
                    x.insertRow(DI, X, SI);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; PR(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }

                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose) cout << "\t\tappendRows(sa, si, ne)" << endl;
                if ((int) strlen(X_SPEC) == DI) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Table x(DD, &testAllocator);  const Table &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; PR(X); }
                    testAllocator.setAllocationLimit(AL);
                    x.appendRows(X, SI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; PR(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tappendRows(sa)" << endl;
                if ((int) strlen(X_SPEC) == DI && (int) strlen(X_SPEC) == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Table x(DD, &testAllocator);  const Table &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; PR(X); }
                    testAllocator.setAllocationLimit(AL);
                    x.appendRows(X);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; PR(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\tappend(table, idx)" << endl;
                if ((int) strlen(X_SPEC) == DI && 1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    const int AL = testAllocator.allocationLimit();
                    testAllocator.setAllocationLimit(-1);
                    Table x(DD, &testAllocator);  const Table &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; PR(X); }
                    testAllocator.setAllocationLimit(AL);
                    x.appendRow(X, SI);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; PR(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }
      }

DEFINE_TEST_CASE(20) {
        // --------------------------------------------------------------------
        // TESTING 'bdem_List' REPLACE METHODS
        // Concerns:
        //   For the 'bdem_List' 'replace' method, the following properties
        //   must hold:
        //    1. The source is left unaffected (apart from aliasing).
        //    2. The subsequent existence of the source has no effect on the
        //       result object (apart from aliasing).
        //    3. The function is alias safe including when
        //       a) the source and destination lists are the same
        //       b) the source includes the destination.
        //       c) the destination includes the source.
        //    4. The function preserves object invariants.
        //    5. The implementations for the overlap and alias cases are
        //       different.  Tests must include:
        //       a) source rows containing aggregates and NOT containing
        //          aggregates.
        //       b) destination lists containing aggregates and NOT
        //          containing aggregates.
        //       c) source rows overlapping the destination list.
        //       d) source rows the same as the destination list.
        //
        // Plan:
        //   Use the enumeration technique to a depth of 5 for both the normal
        //   and alias cases.  Data is tabulated explicitly for the 'replace'
        //   method that takes a range from a source list (or itself, for the
        //   aliasing test); the "scalar" 'replace' test selects a subset of
        //   the table by testing 'if (1 == NE)' where 'NE' is the tabulated
        //   number of elements to replace.
        //     - In the "canonical state" (black-box) tests, we confirm that
        //       the source is unmodified by the method call, and that its
        //       subsequent destruction has no effect on the destination
        //       object.
        //     - In all cases we want to make sure that after the application
        //       of the operation, the object is allowed to go out of scope
        //       directly to enable the destructor to assert object invariants.
        //     - Each object constructed should be wrapped in separate
        //       BSLMA test assert macros and use gg as an optimization.
        //
        // Testing:
        //   void replace(int di, const bdem_Row& sr, int si);
        //   void replace(int di, const bdem_List& sl, int si);
        //   void replace(int di, const bdem_Row& sr, int si, int ne);
        //   void replace(int di, const bdem_List& sl, int si, int ne);
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl
                          << "Testing 'bdem_List' 'replace'" << endl
                          << "=============================" << endl;

        if (verbose) cout <<
            "\nTesting replace(di, sl, si, ne) et al. (no aliasing)" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_dlSpec;   // initial (destination) list
                int         d_di;       // index at which to replace into dl
                const char *d_slSpec;   // source list
                int         d_si;       // index at which to replace from sl
                int         d_ne;       // number of elements to replace
                const char *d_expSpec;  // expected list value
            } DATA[] = {
             //line  d-list        di  s-list     si  ne  expected
             //----  ------        --  ------     --  --  --------   Depth = 0
             { L_,   "",           0,  "",        0,  0, ""                  },

             //line  d-list        di  s-list     si  ne  expected
             //----  ------        --  ------     --  --  --------   Depth = 1
             { L_,   "xA",         0,  "",        0,  0,  "xA"               },
             { L_,   "xA",         1,  "",        0,  0,  "xA"               },
             { L_,   "xB",         1,  "",        0,  0,  "xB"               },
             { L_,   "xC",         1,  "",        0,  0,  "xC"               },
             { L_,   "xD",         1,  "",        0,  0,  "xD"               },
             { L_,   "xE",         1,  "",        0,  0,  "xE"               },
             { L_,   "xF",         1,  "",        0,  0,  "xF"               },
             { L_,   "xG",         1,  "",        0,  0,  "xG"               },
             { L_,   "xH",         1,  "",        0,  0,  "xH"               },
             { L_,   "xI",         1,  "",        0,  0,  "xI"               },
             { L_,   "xJ",         1,  "",        0,  0,  "xJ"               },
             { L_,   "xK",         1,  "",        0,  0,  "xK"               },
             { L_,   "xL",         1,  "",        0,  0,  "xL"               },
             { L_,   "xM",         1,  "",        0,  0,  "xM"               },
             { L_,   "xN",         1,  "",        0,  0,  "xN"               },
             { L_,   "xO",         1,  "",        0,  0,  "xO"               },
             { L_,   "xP",         1,  "",        0,  0,  "xP"               },
             { L_,   "xQ",         1,  "",        0,  0,  "xQ"               },
             { L_,   "xR",         1,  "",        0,  0,  "xR"               },
             { L_,   "xS",         1,  "",        0,  0,  "xS"               },
             { L_,   "xT",         1,  "",        0,  0,  "xT"               },
             { L_,   "xU",         1,  "",        0,  0,  "xU"               },

             { L_,   "",           0,  "xB",      0,  0,  ""                 },
             { L_,   "",           0,  "xB",      1,  0,  ""                 },

             //line  d-list        di  s-list     si  ne  expected
             //----  -------       --  ------     --  --  --------   Depth = 2
             { L_,   "xAxB",       0,  "",        0,  0,  "xAxB"             },
             { L_,   "xAxB",       1,  "",        0,  0,  "xAxB"             },
             { L_,   "xAxB",       2,  "",        0,  0,  "xAxB"             },
             { L_,   "xAxT",       2,  "",        0,  0,  "xAxT"             },
             { L_,   "xAxU",       2,  "",        0,  0,  "xAxU"             },

             { L_,   "xA",         0,  "xB",      0,  0,  "xA"               },
             { L_,   "yA",         0,  "xA",      0,  1,  "xA"               },
             { L_,   "xA",         0,  "xB",      0,  1,  "xB"               },
             { L_,   "xA",         0,  "xC",      0,  1,  "xC"               },
             { L_,   "xA",         0,  "xD",      0,  1,  "xD"               },
             { L_,   "xA",         0,  "xE",      0,  1,  "xE"               },
             { L_,   "xA",         0,  "xF",      0,  1,  "xF"               },
             { L_,   "xA",         0,  "xG",      0,  1,  "xG"               },
             { L_,   "xA",         0,  "xH",      0,  1,  "xH"               },
             { L_,   "xA",         0,  "xI",      0,  1,  "xI"               },
             { L_,   "xA",         0,  "xJ",      0,  1,  "xJ"               },
             { L_,   "xA",         0,  "xK",      0,  1,  "xK"               },
             { L_,   "xA",         0,  "xL",      0,  1,  "xL"               },
             { L_,   "xA",         0,  "xM",      0,  1,  "xM"               },
             { L_,   "xA",         0,  "xN",      0,  1,  "xN"               },
             { L_,   "xA",         0,  "xO",      0,  1,  "xO"               },
             { L_,   "xA",         0,  "xP",      0,  1,  "xP"               },
             { L_,   "xA",         0,  "xQ",      0,  1,  "xQ"               },
             { L_,   "xA",         0,  "xR",      0,  1,  "xR"               },
             { L_,   "xA",         0,  "xS",      0,  1,  "xS"               },
             { L_,   "xA",         0,  "xT",      0,  1,  "xT"               },
             { L_,   "xA",         0,  "xU",      0,  1,  "xU"               },
             { L_,   "xA",         0,  "xV",      0,  1,  "xV"               },
             { L_,   "xA",         0,  "xB",      1,  0,  "xA"               },
             { L_,   "xA",         1,  "xB",      0,  0,  "xA"               },
             { L_,   "xA",         1,  "xB",      1,  0,  "xA"               },

             { L_,   "",           0,  "xAxB",    0,  0,  ""                 },
             { L_,   "",           0,  "xAxB",    1,  0,  ""                 },
             { L_,   "",           0,  "xAxB",    2,  0,  ""                 },

             //line  d-list        di  s-list     si  ne  expected
             //----  ------        --  ------     --  --  --------   Depth = 3
             { L_,   "xAxBxC",     0,  "",        0,  0,  "xAxBxC"           },
             { L_,   "xAxBxC",     1,  "",        0,  0,  "xAxBxC"           },
             { L_,   "xAxBxC",     2,  "",        0,  0,  "xAxBxC"           },
             { L_,   "xAxBxC",     3,  "",        0,  0,  "xAxBxC"           },
             { L_,   "xAxBxU",     3,  "",        0,  0,  "xAxBxU"           },

             { L_,   "xAxB",       0,  "xC",      0,  0,  "xAxB"             },
             { L_,   "xAxB",       0,  "xC",      0,  1,  "xCxB"             },
             { L_,   "xAxB",       0,  "xC",      1,  0,  "xAxB"             },
             { L_,   "xAxB",       1,  "xC",      0,  0,  "xAxB"             },
             { L_,   "xAxB",       1,  "xC",      0,  1,  "xAxC"             },
             { L_,   "xAxB",       1,  "xC",      1,  0,  "xAxB"             },
             { L_,   "xAxB",       2,  "xC",      0,  0,  "xAxB"             },
             { L_,   "xAxB",       2,  "xC",      1,  0,  "xAxB"             },
             { L_,   "xAxU",       1,  "xC",      0,  1,  "xAxC"             },
             { L_,   "xAxB",       1,  "xT",      0,  1,  "xAxT"             },
             { L_,   "xAxB",       1,  "xU",      0,  1,  "xAxU"             },

             { L_,   "xA",         0,  "xBxC",    0,  0,  "xA"               },
             { L_,   "xA",         0,  "xBxC",    0,  1,  "xB"               },
             { L_,   "xA",         0,  "xBxC",    1,  0,  "xA"               },
             { L_,   "xA",         0,  "xBxC",    1,  1,  "xC"               },
             { L_,   "xA",         0,  "xBxC",    2,  0,  "xA"               },
             { L_,   "xA",         1,  "xBxC",    0,  0,  "xA"               },
             { L_,   "xA",         1,  "xBxC",    1,  0,  "xA"               },
             { L_,   "xA",         1,  "xBxC",    2,  0,  "xA"               },
             { L_,   "xA",         0,  "xTxC",    0,  0,  "xA"               },
             { L_,   "xA",         0,  "xTxC",    0,  1,  "xT"               },
             { L_,   "xA",         0,  "xTxC",    1,  0,  "xA"               },
             { L_,   "xA",         0,  "xTxC",    1,  1,  "xC"               },
             { L_,   "xA",         0,  "xTxC",    2,  0,  "xA"               },
             { L_,   "xA",         1,  "xTxC",    0,  0,  "xA"               },
             { L_,   "xA",         1,  "xTxC",    1,  0,  "xA"               },
             { L_,   "xA",         1,  "xTxC",    2,  0,  "xA"               },
             { L_,   "xA",         0,  "xUxC",    0,  0,  "xA"               },
             { L_,   "xA",         0,  "xUxC",    0,  1,  "xU"               },
             { L_,   "xA",         0,  "xUxC",    1,  0,  "xA"               },
             { L_,   "xA",         0,  "xUxC",    1,  1,  "xC"               },
             { L_,   "xA",         0,  "xUxC",    2,  0,  "xA"               },
             { L_,   "xA",         1,  "xUxC",    0,  0,  "xA"               },
             { L_,   "xA",         1,  "xUxC",    1,  0,  "xA"               },
             { L_,   "xA",         1,  "xUxC",    2,  0,  "xA"               },

             { L_,   "",           0,  "xAxBxC",  0,  0,  ""                 },
             { L_,   "",           0,  "xAxBxC",  1,  0,  ""                 },
             { L_,   "",           0,  "xAxBxC",  2,  0,  ""                 },
             { L_,   "",           0,  "xAxBxC",  3,  0,  ""                 },

             //line  d-list        di  s-list     si  ne  expected
             //----  ------        --  ------     --  --  --------   Depth = 4
             { L_,   "xAxBxCxD",   0,  "",        0,  0,  "xAxBxCxD"         },
             { L_,   "xAxBxCxD",   1,  "",        0,  0,  "xAxBxCxD"         },
             { L_,   "xAxBxCxD",   2,  "",        0,  0,  "xAxBxCxD"         },
             { L_,   "xAxBxCxD",   3,  "",        0,  0,  "xAxBxCxD"         },
             { L_,   "xAxBxCxD",   4,  "",        0,  0,  "xAxBxCxD"         },

             { L_,   "xAxBxC",     0,  "xD",      0,  0,  "xAxBxC"           },
             { L_,   "xAxBxC",     0,  "xD",      0,  1,  "xDxBxC"           },
             { L_,   "xAxBxC",     0,  "xD",      1,  0,  "xAxBxC"           },
             { L_,   "xAxBxC",     1,  "xD",      0,  0,  "xAxBxC"           },
             { L_,   "xAxBxC",     1,  "xD",      0,  1,  "xAxDxC"           },
             { L_,   "xAxBxC",     1,  "xD",      1,  0,  "xAxBxC"           },
             { L_,   "xAxBxC",     2,  "xD",      0,  0,  "xAxBxC"           },
             { L_,   "xAxBxC",     2,  "xD",      0,  1,  "xAxBxD"           },
             { L_,   "xAxBxC",     2,  "xD",      1,  0,  "xAxBxC"           },
             { L_,   "xAxBxC",     3,  "xD",      0,  0,  "xAxBxC"           },
             { L_,   "xAxBxC",     3,  "xD",      1,  0,  "xAxBxC"           },

             { L_,   "xAxB",       0,  "xCxD",    0,  0,  "xAxB"             },
             { L_,   "xAxB",       0,  "xCxD",    0,  1,  "xCxB"             },
             { L_,   "xAxB",       0,  "xCxD",    0,  2,  "xCxD"             },
             { L_,   "xAxB",       0,  "xCxD",    1,  0,  "xAxB"             },
             { L_,   "xAxB",       0,  "xCxD",    1,  1,  "xDxB"             },
             { L_,   "xAxB",       0,  "xCxD",    2,  0,  "xAxB"             },
             { L_,   "xAxB",       1,  "xCxD",    0,  0,  "xAxB"             },
             { L_,   "xAxB",       1,  "xCxD",    0,  1,  "xAxC"             },
             { L_,   "xAxB",       1,  "xCxD",    1,  0,  "xAxB"             },
             { L_,   "xAxB",       1,  "xCxD",    1,  1,  "xAxD"             },
             { L_,   "xAxB",       1,  "xCxD",    2,  0,  "xAxB"             },
             { L_,   "xAxB",       2,  "xCxD",    0,  0,  "xAxB"             },
             { L_,   "xAxB",       2,  "xCxD",    1,  0,  "xAxB"             },
             { L_,   "xAxB",       2,  "xCxD",    2,  0,  "xAxB"             },

             { L_,   "xA",         0,  "xBxCxD",  0,  0,  "xA"               },
             { L_,   "xA",         0,  "xBxCxD",  0,  1,  "xB"               },
             { L_,   "xA",         0,  "xBxCxD",  1,  0,  "xA"               },
             { L_,   "xA",         0,  "xBxCxD",  1,  1,  "xC"               },
             { L_,   "xA",         0,  "xBxCxD",  2,  0,  "xA"               },
             { L_,   "xA",         0,  "xBxCxD",  2,  1,  "xD"               },
             { L_,   "xA",         0,  "xBxCxD",  3,  0,  "xA"               },
             { L_,   "xA",         1,  "xBxCxD",  0,  0,  "xA"               },
             { L_,   "xA",         1,  "xBxCxD",  1,  0,  "xA"               },
             { L_,   "xA",         1,  "xBxCxD",  2,  0,  "xA"               },
             { L_,   "xA",         1,  "xBxCxD",  3,  0,  "xA"               },

             { L_,   "",           0,  "xAxBxCxD", 0, 0,  ""                 },
             { L_,   "",           0,  "xAxBxCxD", 1, 0,  ""                 },
             { L_,   "",           0,  "xAxBxCxD", 2, 0,  ""                 },
             { L_,   "",           0,  "xAxBxCxD", 3, 0,  ""                 },
             { L_,   "",           0,  "xAxBxCxD", 4, 0,  ""                 },

             //line  d-list        di  s-list     si  ne  expected
             //----  ------        --  ------     --  --  --------   Depth = 5
             { L_,   "xAxBxCxDxE", 0,  "",        0,  0,  "xAxBxCxDxE"       },
             { L_,   "xAxBxCxDxE", 1,  "",        0,  0,  "xAxBxCxDxE"       },
             { L_,   "xAxBxCxDxE", 2,  "",        0,  0,  "xAxBxCxDxE"       },
             { L_,   "xAxBxCxDxE", 3,  "",        0,  0,  "xAxBxCxDxE"       },
             { L_,   "xAxBxCxDxE", 4,  "",        0,  0,  "xAxBxCxDxE"       },
             { L_,   "xAxBxCxDxE", 5,  "",        0,  0,  "xAxBxCxDxE"       },

             { L_,   "xAxBxCxD",   0,  "xE",      0,  0,  "xAxBxCxD"         },
             { L_,   "xAxBxCxD",   0,  "xE",      0,  1,  "xExBxCxD"         },
             { L_,   "xAxBxCxD",   0,  "xE",      1,  0,  "xAxBxCxD"         },
             { L_,   "xAxBxCxD",   1,  "xE",      0,  0,  "xAxBxCxD"         },
             { L_,   "xAxBxCxD",   1,  "xE",      0,  1,  "xAxExCxD"         },
             { L_,   "xAxBxCxD",   1,  "xE",      1,  0,  "xAxBxCxD"         },
             { L_,   "xAxBxCxD",   2,  "xE",      0,  0,  "xAxBxCxD"         },
             { L_,   "xAxBxCxD",   2,  "xE",      0,  1,  "xAxBxExD"         },
             { L_,   "xAxBxCxD",   2,  "xE",      1,  0,  "xAxBxCxD"         },
             { L_,   "xAxBxCxD",   3,  "xE",      0,  0,  "xAxBxCxD"         },
             { L_,   "xAxBxCxD",   3,  "xE",      0,  1,  "xAxBxCxE"         },
             { L_,   "xAxBxCxD",   3,  "xE",      1,  0,  "xAxBxCxD"         },
             { L_,   "xAxBxCxD",   4,  "xE",      0,  0,  "xAxBxCxD"         },
             { L_,   "xAxBxCxD",   4,  "xE",      1,  0,  "xAxBxCxD"         },

             { L_,   "xAxBxC",     0,  "xDxE",    0,  0,  "xAxBxC"           },
             { L_,   "xAxBxC",     0,  "xDxE",    0,  1,  "xDxBxC"           },
             { L_,   "xAxBxC",     0,  "xDxE",    0,  2,  "xDxExC"           },
             { L_,   "xAxBxC",     0,  "xDxE",    1,  0,  "xAxBxC"           },
             { L_,   "xAxBxC",     0,  "xDxE",    1,  1,  "xExBxC"           },
             { L_,   "xAxBxC",     0,  "xDxE",    2,  0,  "xAxBxC"           },
             { L_,   "xAxBxC",     1,  "xDxE",    0,  0,  "xAxBxC"           },
             { L_,   "xAxBxC",     1,  "xDxE",    0,  1,  "xAxDxC"           },
             { L_,   "xAxBxC",     1,  "xDxE",    0,  2,  "xAxDxE"           },
             { L_,   "xAxBxC",     1,  "xDxE",    1,  0,  "xAxBxC"           },
             { L_,   "xAxBxC",     1,  "xDxE",    1,  1,  "xAxExC"           },
             { L_,   "xAxBxC",     1,  "xDxE",    2,  0,  "xAxBxC"           },
             { L_,   "xAxBxC",     2,  "xDxE",    0,  0,  "xAxBxC"           },
             { L_,   "xAxBxC",     2,  "xDxE",    0,  1,  "xAxBxD"           },
             { L_,   "xAxBxC",     2,  "xDxE",    1,  0,  "xAxBxC"           },
             { L_,   "xAxBxC",     2,  "xDxE",    1,  1,  "xAxBxE"           },
             { L_,   "xAxBxC",     2,  "xDxE",    2,  0,  "xAxBxC"           },
             { L_,   "xAxBxC",     3,  "xDxE",    0,  0,  "xAxBxC"           },
             { L_,   "xAxBxC",     3,  "xDxE",    1,  0,  "xAxBxC"           },
             { L_,   "xAxBxC",     3,  "xDxE",    2,  0,  "xAxBxC"           },

             { L_,   "xAxB",       0,  "xCxDxE",  0,  0,  "xAxB"             },
             { L_,   "xAxB",       0,  "xCxDxE",  0,  1,  "xCxB"             },
             { L_,   "xAxB",       0,  "xCxDxE",  0,  2,  "xCxD"             },
             { L_,   "xAxB",       0,  "xCxDxE",  1,  0,  "xAxB"             },
             { L_,   "xAxB",       0,  "xCxDxE",  1,  1,  "xDxB"             },
             { L_,   "xAxB",       0,  "xCxDxE",  2,  0,  "xAxB"             },
             { L_,   "xAxB",       0,  "xCxDxE",  2,  1,  "xExB"             },
             { L_,   "xAxB",       0,  "xCxDxE",  3,  0,  "xAxB"             },
             { L_,   "xAxB",       1,  "xCxDxE",  0,  0,  "xAxB"             },
             { L_,   "xAxB",       1,  "xCxDxE",  0,  1,  "xAxC"             },
             { L_,   "xAxB",       1,  "xCxDxE",  1,  0,  "xAxB"             },
             { L_,   "xAxB",       1,  "xCxDxE",  1,  1,  "xAxD"             },
             { L_,   "xAxB",       1,  "xCxDxE",  2,  0,  "xAxB"             },
             { L_,   "xAxB",       1,  "xCxDxE",  2,  1,  "xAxE"             },
             { L_,   "xAxB",       1,  "xCxDxE",  3,  0,  "xAxB"             },
             { L_,   "xAxB",       2,  "xCxDxE",  0,  0,  "xAxB"             },
             { L_,   "xAxB",       2,  "xCxDxE",  1,  0,  "xAxB"             },
             { L_,   "xAxB",       2,  "xCxDxE",  2,  0,  "xAxB"             },
             { L_,   "xAxB",       2,  "xCxDxE",  3,  0,  "xAxB"             },

             { L_,   "xA",         0,  "xBxCxDxE", 0, 0,  "xA"               },
             { L_,   "xA",         0,  "xBxCxDxE", 0, 1,  "xB"               },
             { L_,   "xA",         0,  "xBxCxDxE", 1, 0,  "xA"               },
             { L_,   "xA",         0,  "xBxCxDxE", 1, 1,  "xC"               },
             { L_,   "xA",         0,  "xBxCxDxE", 2, 0,  "xA"               },
             { L_,   "xA",         0,  "xBxCxDxE", 2, 1,  "xD"               },
             { L_,   "xA",         0,  "xBxCxDxE", 3, 0,  "xA"               },
             { L_,   "xA",         0,  "xBxCxDxE", 3, 1,  "xE"               },
             { L_,   "xA",         0,  "xBxCxDxE", 4, 0,  "xA"               },
             { L_,   "xA",         1,  "xBxCxDxE", 0, 0,  "xA"               },
             { L_,   "xA",         1,  "xBxCxDxE", 1, 0,  "xA"               },
             { L_,   "xA",         1,  "xBxCxDxE", 2, 0,  "xA"               },
             { L_,   "xA",         1,  "xBxCxDxE", 3, 0,  "xA"               },
             { L_,   "xA",         1,  "xBxCxDxE", 4, 0,  "xA"               },

             { L_,   "",           0,  "xAxBxCxDxE", 0, 0, ""                },
             { L_,   "",           0,  "xAxBxCxDxE", 1, 0, ""                },
             { L_,   "",           0,  "xAxBxCxDxE", 2, 0, ""                },
             { L_,   "",           0,  "xAxBxCxDxE", 3, 0, ""                },
             { L_,   "",           0,  "xAxBxCxDxE", 4, 0, ""                },
             { L_,   "",           0,  "xAxBxCxDxE", 5, 0, ""                },
             { L_,   "xA",         0,  "xU",      0, 1, "xU"                 },
             { L_,   "xAxA",       0,  "xU",      0, 1, "xUxA"               },
             { L_,   "xAxAxA",     0,  "xU",      0, 1, "xUxAxA"             },
             { L_,   "xAxAxAxA",   0,  "xU",      0, 1, "xUxAxAxA"           },
             { L_,   "xAxAxAxAxA", 0,  "xU",      0, 1, "xUxAxAxAxA"         },
             { L_,   "xAxAxAxAxAxA", 0, "xU",     0, 1, "xUxAxAxAxAxA"       },
             { L_,   "xAxAxAxAxAxAxA", 0, "xU",   0, 1, "xUxAxAxAxAxAxA"     },
             { L_,   "xAxAxAxAxAxAxAxA", 0, "xU", 0, 1, "xUxAxAxAxAxAxAxA"   },
             { L_,   "xG",         0,  "xU",      0, 1, "xU"                 },
             { L_,   "xGxA",       0,  "xU",      0, 1, "xUxA"               },
             { L_,   "xGxAxA",     0,  "xU",      0, 1, "xUxAxA"             },
             { L_,   "xGxAxAxA",   0,  "xU",      0, 1, "xUxAxAxA"           },
             { L_,   "xGxAxAxAxA", 0,  "xU",      0, 1, "xUxAxAxAxA"         },
             { L_,   "xGxAxAxAxAxA", 0, "xU",     0, 1, "xUxAxAxAxAxA"       },
             { L_,   "xGxAxAxAxAxAxA", 0, "xU",   0, 1, "xUxAxAxAxAxAxA"     },
             { L_,   "xGxAxAxAxAxAxAxA", 0, "xU", 0, 1, "xUxAxAxAxAxAxAxA"   },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *D_SPEC = DATA[ti].d_dlSpec;
                const int   DI     = DATA[ti].d_di;
                const char *S_SPEC = DATA[ti].d_slSpec;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = ((int) strlen(D_SPEC) + strlen(S_SPEC))/2;
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                List DD(g(D_SPEC));      // control for destination
                List SS(g(S_SPEC));      // control for source
                List EE(g(E_SPEC));      // control for expected value

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(D_SPEC); P_(DI); P_(S_SPEC);
                                                P_(SI); P_(NE); P(E_SPEC);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(SS);
                    cout << "\t\t"; P(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                for (int option = 0; option < 3; ++option) {
                    bdem_AggregateOption::AllocationStrategy optval =
                          option == 0 ? bdem_AggregateOption::BDEM_PASS_THROUGH
                        : option == 1 ? bdem_AggregateOption::BDEM_WRITE_MANY
                        : bdem_AggregateOption::BDEM_WRITE_ONCE;

                    if (veryVerbose)
                        cout << "\t\toption =" << optionToString(optval);
                    if (veryVerbose) cout << "\t\treplace(di, sl, si, ne)"
                                          << endl;
                    {
                      BEGIN_BSLMA_EXCEPTION_TEST {
                        List x(DD, optval,
                               &testAllocator);
                        const List &X = x;
                        {
                            List s(SS, &testAllocator);  const List &S = s;
                            if (veryVerbose) {
                                cout << "\t\t\tBEFORE: "; P(X);
                            }

                            for (int sii = 0; sii < NE; ++sii)
                            {
                                x.replaceElement(DI + sii, s[SI + sii]);
                            }

                            if (veryVerbose) {
                                cout << "\t\t\t AFTER: "; P(X);
                            }
                            LOOP_ASSERT(LINE, EE == X);
                            LOOP_ASSERT(LINE, SS == S);   // source unchanged?
                        }
                        LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                      } END_BSLMA_EXCEPTION_TEST
                    }

                    if (veryVerbose) cout << "\t\treplace(di, src, si)"
                                          << endl;
                    if (1 == NE) {
                      BEGIN_BSLMA_EXCEPTION_TEST {
                        List x(DD, optval,
                               &testAllocator);
                        const List &X = x;
                        {
                            List s(SS, &testAllocator);  const List &S = s;
                            if (veryVerbose) {
                                cout << "\t\t\tBEFORE: "; P(X);
                            }
                            x.replaceElement(DI, S[SI]);   // source 'const'
                            if (veryVerbose) {
                                cout << "\t\t\t AFTER: "; P(X);
                            }
                            LOOP_ASSERT(LINE, EE == X);
                            LOOP_ASSERT(LINE, SS == S);    // source unchanged?
                        }
                        LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                      } END_BSLMA_EXCEPTION_TEST
                    }
                }
            }
        }
      }

DEFINE_TEST_CASE(19) {

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        //---------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'bdem_List' 'replace'" << endl
                          << "=============================" << endl;

        if (verbose) cout <<
            "\nTesting x.replaceElements(di, sl, si, ne) et al. (aliasing)"
                          << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_xSpec;    // initial list (= srcList)
                int         d_di;       // index at which to insert into x
                int         d_si;       // index at which to insert from x
                int         d_ne;       // number of elements to insert
                const char *d_expSpec;  // expected list value
            } DATA[] = {
                //line  x-list        di  si  ne  expected
                //----  ------        --  --  --  --------   Depth = 0
                { L_,   "",           0,  0,  0,  ""            },

                //line  x-list        di  si  ne  expected
                //----  ------        --  --  --  --------   Depth = 1
                { L_,   "xA",         0,  0,  0,  "xA"          },
                { L_,   "xA",         0,  0,  1,  "xA"          },
                { L_,   "xB",         0,  0,  1,  "xB"          },
                { L_,   "xC",         0,  0,  1,  "xC"          },
                { L_,   "xD",         0,  0,  1,  "xD"          },
                { L_,   "xE",         0,  0,  1,  "xE"          },
                { L_,   "xF",         0,  0,  1,  "xF"          },
                { L_,   "xG",         0,  0,  1,  "xG"          },
                { L_,   "xH",         0,  0,  1,  "xH"          },
                { L_,   "xI",         0,  0,  1,  "xI"          },
                { L_,   "xJ",         0,  0,  1,  "xJ"          },
                { L_,   "xK",         0,  0,  1,  "xK"          },
                { L_,   "xL",         0,  0,  1,  "xL"          },
                { L_,   "xM",         0,  0,  1,  "xM"          },
                { L_,   "xN",         0,  0,  1,  "xN"          },
                { L_,   "xO",         0,  0,  1,  "xO"          },
                { L_,   "xP",         0,  0,  1,  "xP"          },
                { L_,   "xQ",         0,  0,  1,  "xQ"          },
                { L_,   "xR",         0,  0,  1,  "xR"          },
                { L_,   "xS",         0,  0,  1,  "xS"          },
                { L_,   "xT",         0,  0,  1,  "xT"          },
                { L_,   "xU",         0,  0,  1,  "xU"          },
                { L_,   "xV",         0,  0,  1,  "xV"          },
                { L_,   "xA",         0,  1,  0,  "xA"          },

                { L_,   "xA",         1,  0,  0,  "xA"          },
                { L_,   "xA",         1,  1,  0,  "xA"          },

                //line  x-list        di  si  ne  expected
                //----  ------        --  --  --  --------   Depth = 2
                { L_,   "xAxB",       0,  0,  0,  "xAxB"        },
                { L_,   "xAxB",       0,  0,  1,  "xAxB"        },
                { L_,   "xAxB",       0,  0,  2,  "xAxB"        },
                { L_,   "xAxB",       0,  1,  0,  "xAxB"        },
                { L_,   "xAxB",       0,  1,  1,  "xBxB"        },
                { L_,   "xAxB",       0,  2,  0,  "xAxB"        },

                { L_,   "xAxB",       1,  0,  0,  "xAxB"        },
                { L_,   "xAxB",       1,  0,  1,  "xAxA"        },
                { L_,   "xAxB",       1,  1,  0,  "xAxB"        },
                { L_,   "xAxB",       1,  1,  1,  "xAxB"        },
                { L_,   "xAxB",       1,  2,  0,  "xAxB"        },

                { L_,   "xAxB",       2,  0,  0,  "xAxB"        },
                { L_,   "xAxB",       2,  1,  0,  "xAxB"        },
                { L_,   "xAxB",       2,  2,  0,  "xAxB"        },

                { L_,   "xUxB",       0,  0,  1,  "xUxB"        },
                { L_,   "xUxB",       0,  0,  2,  "xUxB"        },
                { L_,   "xUxB",       0,  1,  1,  "xBxB"        },

                { L_,   "xUxB",       1,  0,  1,  "xUxU"        },
                { L_,   "xUxB",       1,  1,  1,  "xUxB"        },

                //line  x-list        di  si  ne  expected
                //----  ------        --  --  --  --------   Depth = 3
                { L_,   "xAxBxC",     0,  0,  0,  "xAxBxC"      },
                { L_,   "xAxBxC",     0,  0,  1,  "xAxBxC"      },
                { L_,   "xAxBxC",     0,  0,  2,  "xAxBxC"      },
                { L_,   "xAxBxC",     0,  0,  3,  "xAxBxC"      },
                { L_,   "xAxBxC",     0,  1,  0,  "xAxBxC"      },
                { L_,   "xAxBxC",     0,  1,  1,  "xBxBxC"      },
                { L_,   "xAxBxC",     0,  1,  2,  "xBxCxC"      },
                { L_,   "xAxBxC",     0,  2,  0,  "xAxBxC"      },
                { L_,   "xAxBxC",     0,  2,  1,  "xCxBxC"      },
                { L_,   "xAxBxC",     0,  3,  0,  "xAxBxC"      },

                { L_,   "xAxBxC",     1,  0,  0,  "xAxBxC"      },
                { L_,   "xAxBxC",     1,  0,  1,  "xAxAxC"      },
                { L_,   "xAxBxC",     1,  0,  2,  "xAxAxB"      },
                { L_,   "xAxBxC",     1,  1,  0,  "xAxBxC"      },
                { L_,   "xAxBxC",     1,  1,  1,  "xAxBxC"      },
                { L_,   "xAxBxC",     1,  1,  2,  "xAxBxC"      },
                { L_,   "xAxBxC",     1,  2,  0,  "xAxBxC"      },
                { L_,   "xAxBxC",     1,  2,  1,  "xAxCxC"      },
                { L_,   "xAxBxC",     1,  3,  0,  "xAxBxC"      },

                { L_,   "xAxBxC",     2,  0,  0,  "xAxBxC"      },
                { L_,   "xAxBxC",     2,  0,  1,  "xAxBxA"      },
                { L_,   "xAxBxC",     2,  1,  0,  "xAxBxC"      },
                { L_,   "xAxBxC",     2,  1,  1,  "xAxBxB"      },
                { L_,   "xAxBxC",     2,  2,  0,  "xAxBxC"      },
                { L_,   "xAxBxC",     2,  2,  1,  "xAxBxC"      },
                { L_,   "xAxBxC",     2,  3,  0,  "xAxBxC"      },

                { L_,   "xAxBxC",     3,  0,  0,  "xAxBxC"      },
                { L_,   "xAxBxC",     3,  1,  0,  "xAxBxC"      },
                { L_,   "xAxBxC",     3,  2,  0,  "xAxBxC"      },
                { L_,   "xAxBxC",     3,  3,  0,  "xAxBxC"      },

                { L_,   "xAxUxC",     0,  0,  1,  "xAxUxC"      },
                { L_,   "xAxUxC",     0,  0,  2,  "xAxUxC"      },
                { L_,   "xAxUxC",     0,  0,  3,  "xAxUxC"      },
                { L_,   "xAxUxC",     0,  1,  1,  "xUxUxC"      },
                { L_,   "xAxUxC",     0,  1,  2,  "xUxCxC"      },
                { L_,   "xAxUxC",     0,  2,  1,  "xCxUxC"      },

                { L_,   "xAxUxC",     1,  0,  1,  "xAxAxC"      },
                { L_,   "xAxUxC",     1,  0,  2,  "xAxAxU"      },
                { L_,   "xAxUxC",     1,  1,  1,  "xAxUxC"      },
                { L_,   "xAxUxC",     1,  1,  2,  "xAxUxC"      },
                { L_,   "xAxUxC",     1,  2,  1,  "xAxCxC"      },

                { L_,   "xAxUxC",     2,  0,  1,  "xAxUxA"      },
                { L_,   "xAxUxC",     2,  1,  1,  "xAxUxU"      },
                { L_,   "xAxUxC",     2,  2,  1,  "xAxUxC"      },

                //line  x-list        di  si  ne  expected
                //----  ------        --  --  --  --------   Depth = 4
                { L_,   "xAxBxCxD",   0,  0,  0,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   0,  0,  1,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   0,  0,  2,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   0,  0,  3,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   0,  0,  4,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   0,  1,  0,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   0,  1,  1,  "xBxBxCxD"    },
                { L_,   "xAxBxCxD",   0,  1,  2,  "xBxCxCxD"    },
                { L_,   "xAxBxCxD",   0,  1,  3,  "xBxCxDxD"    },
                { L_,   "xAxBxCxD",   0,  2,  0,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   0,  2,  1,  "xCxBxCxD"    },
                { L_,   "xAxBxCxD",   0,  2,  2,  "xCxDxCxD"    },
                { L_,   "xAxBxCxD",   0,  3,  0,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   0,  3,  1,  "xDxBxCxD"    },
                { L_,   "xAxBxCxD",   0,  4,  0,  "xAxBxCxD"    },

                { L_,   "xAxBxCxD",   1,  0,  0,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   1,  0,  1,  "xAxAxCxD"    },
                { L_,   "xAxBxCxD",   1,  0,  2,  "xAxAxBxD"    },
                { L_,   "xAxBxCxD",   1,  0,  3,  "xAxAxBxC"    },
                { L_,   "xAxBxCxD",   1,  1,  0,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   1,  1,  1,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   1,  1,  2,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   1,  1,  3,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   1,  2,  0,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   1,  2,  1,  "xAxCxCxD"    },
                { L_,   "xAxBxCxD",   1,  2,  2,  "xAxCxDxD"    },
                { L_,   "xAxBxCxD",   1,  3,  0,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   1,  3,  1,  "xAxDxCxD"    },
                { L_,   "xAxBxCxD",   1,  4,  0,  "xAxBxCxD"    },

                { L_,   "xAxBxCxD",   2,  0,  0,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   2,  0,  1,  "xAxBxAxD"    },
                { L_,   "xAxBxCxD",   2,  0,  2,  "xAxBxAxB"    },
                { L_,   "xAxBxCxD",   2,  1,  0,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   2,  1,  1,  "xAxBxBxD"    },
                { L_,   "xAxBxCxD",   2,  1,  2,  "xAxBxBxC"    },
                { L_,   "xAxBxCxD",   2,  2,  0,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   2,  2,  1,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   2,  2,  2,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   2,  3,  0,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   2,  3,  1,  "xAxBxDxD"    },
                { L_,   "xAxBxCxD",   2,  4,  0,  "xAxBxCxD"    },

                { L_,   "xAxBxCxD",   3,  0,  0,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   3,  0,  1,  "xAxBxCxA"    },
                { L_,   "xAxBxCxD",   3,  1,  0,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   3,  1,  1,  "xAxBxCxB"    },
                { L_,   "xAxBxCxD",   3,  2,  0,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   3,  2,  1,  "xAxBxCxC"    },
                { L_,   "xAxBxCxD",   3,  3,  0,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   3,  3,  1,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   3,  4,  0,  "xAxBxCxD"    },

                { L_,   "xAxBxCxD",   4,  0,  0,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   4,  1,  0,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   4,  2,  0,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   4,  3,  0,  "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   4,  4,  0,  "xAxBxCxD"    },

                //line  x-list        di  si  ne  expected
                //----  ------        --  --  --  --------   Depth = 5
                { L_,   "xAxBxCxDxE", 0,  0,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 0,  0,  1,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 0,  0,  2,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 0,  0,  3,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 0,  0,  4,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 0,  0,  5,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 0,  1,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 0,  1,  1,  "xBxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 0,  1,  2,  "xBxCxCxDxE"  },
                { L_,   "xAxBxCxDxE", 0,  1,  3,  "xBxCxDxDxE"  },
                { L_,   "xAxBxCxDxE", 0,  1,  4,  "xBxCxDxExE"  },
                { L_,   "xAxBxCxDxE", 0,  2,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 0,  2,  1,  "xCxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 0,  2,  2,  "xCxDxCxDxE"  },
                { L_,   "xAxBxCxDxE", 0,  2,  3,  "xCxDxExDxE"  },
                { L_,   "xAxBxCxDxE", 0,  3,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 0,  3,  1,  "xDxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 0,  3,  2,  "xDxExCxDxE"  },
                { L_,   "xAxBxCxDxE", 0,  4,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 0,  4,  1,  "xExBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 0,  5,  0,  "xAxBxCxDxE"  },

                { L_,   "xAxBxCxDxE", 1,  0,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 1,  0,  1,  "xAxAxCxDxE"  },
                { L_,   "xAxBxCxDxE", 1,  0,  2,  "xAxAxBxDxE"  },
                { L_,   "xAxBxCxDxE", 1,  0,  3,  "xAxAxBxCxE"  },
                { L_,   "xAxBxCxDxE", 1,  0,  4,  "xAxAxBxCxD"  },
                { L_,   "xAxBxCxDxE", 1,  1,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 1,  1,  1,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 1,  1,  2,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 1,  1,  3,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 1,  1,  4,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 1,  2,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 1,  2,  1,  "xAxCxCxDxE"  },
                { L_,   "xAxBxCxDxE", 1,  2,  2,  "xAxCxDxDxE"  },
                { L_,   "xAxBxCxDxE", 1,  2,  3,  "xAxCxDxExE"  },
                { L_,   "xAxBxCxDxE", 1,  3,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 1,  3,  1,  "xAxDxCxDxE"  },
                { L_,   "xAxBxCxDxE", 1,  3,  2,  "xAxDxExDxE"  },
                { L_,   "xAxBxCxDxE", 1,  4,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 1,  4,  1,  "xAxExCxDxE"  },
                { L_,   "xAxBxCxDxE", 1,  5,  0,  "xAxBxCxDxE"  },

                { L_,   "xAxBxCxDxE", 2,  0,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 2,  0,  1,  "xAxBxAxDxE"  },
                { L_,   "xAxBxCxDxE", 2,  0,  2,  "xAxBxAxBxE"  },
                { L_,   "xAxBxCxDxE", 2,  0,  3,  "xAxBxAxBxC"  },
                { L_,   "xAxBxCxDxE", 2,  1,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 2,  1,  1,  "xAxBxBxDxE"  },
                { L_,   "xAxBxCxDxE", 2,  1,  2,  "xAxBxBxCxE"  },
                { L_,   "xAxBxCxDxE", 2,  1,  3,  "xAxBxBxCxD"  },
                { L_,   "xAxBxCxDxE", 2,  2,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 2,  2,  1,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 2,  2,  2,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 2,  2,  3,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 2,  3,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 2,  3,  1,  "xAxBxDxDxE"  },
                { L_,   "xAxBxCxDxE", 2,  3,  2,  "xAxBxDxExE"  },
                { L_,   "xAxBxCxDxE", 2,  4,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 2,  4,  1,  "xAxBxExDxE"  },
                { L_,   "xAxBxCxDxE", 2,  5,  0,  "xAxBxCxDxE"  },

                { L_,   "xAxBxCxDxE", 3,  0,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 3,  0,  1,  "xAxBxCxAxE"  },
                { L_,   "xAxBxCxDxE", 3,  0,  2,  "xAxBxCxAxB"  },
                { L_,   "xAxBxCxDxE", 3,  1,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 3,  1,  1,  "xAxBxCxBxE"  },
                { L_,   "xAxBxCxDxE", 3,  1,  2,  "xAxBxCxBxC"  },
                { L_,   "xAxBxCxDxE", 3,  2,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 3,  2,  1,  "xAxBxCxCxE"  },
                { L_,   "xAxBxCxDxE", 3,  2,  2,  "xAxBxCxCxD"  },
                { L_,   "xAxBxCxDxE", 3,  3,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 3,  3,  1,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 3,  3,  2,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 3,  4,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 3,  4,  1,  "xAxBxCxExE"  },
                { L_,   "xAxBxCxDxE", 3,  5,  0,  "xAxBxCxDxE"  },

                { L_,   "xAxBxCxDxE", 4,  0,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 4,  0,  1,  "xAxBxCxDxA"  },
                { L_,   "xAxBxCxDxE", 4,  1,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 4,  1,  1,  "xAxBxCxDxB"  },
                { L_,   "xAxBxCxDxE", 4,  2,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 4,  2,  1,  "xAxBxCxDxC"  },
                { L_,   "xAxBxCxDxE", 4,  3,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 4,  3,  1,  "xAxBxCxDxD"  },
                { L_,   "xAxBxCxDxE", 4,  4,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 4,  4,  1,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 4,  5,  0,  "xAxBxCxDxE"  },

                { L_,   "xAxBxCxDxE", 5,  0,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 5,  1,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 5,  2,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 5,  3,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 5,  4,  0,  "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 5,  5,  0,  "xAxBxCxDxE"  },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *X_SPEC = DATA[ti].d_xSpec;
                const int   DI     = DATA[ti].d_di;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = strlen(X_SPEC)/2;
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                List DD(g(X_SPEC));    // control for destination
                List EE(g(E_SPEC));    // control for expected value

#if !defined(BSLS_PLATFORM__CMP_MSVC)
                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(X_SPEC); P_(DI);
                                                P_(SI); P_(NE); P(E_SPEC);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }
#endif

                // CONTROL: ensure same table result as non-alias case.
                {
                    List x(DD, &testAllocator);  const List &X = x;
                    List x2(DD, &testAllocator); // control
                    const List &X2 = x2;
                    for (int sii = 0; sii < NE; ++sii)
                    {
                        x2.replaceElement(DI + sii, X[SI + sii]);
                    }

                    LOOP_ASSERT(LINE, EE == X2);
                }

                if (veryVerbose) cout << "\t\treplace(di, sl, si, ne)" << endl;
                {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    List x(DD, &testAllocator);  const List &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    replaceListElements(x, DI, X.row(), SI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\treplace(di, sl, si)" << endl;
                if (1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    List x(DD, &testAllocator);  const List &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    x.replaceElement(DI, X[SI]);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }
      }

DEFINE_TEST_CASE(18) {

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'bdem_List' 'replace'" << endl
                          << "=============================" << endl;

        if (verbose) cout <<
            "\nTesting x.replace(di, sl, si, ne) et al. (overlapping)"
            << endl;
        {
            // Destination contains source list
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_xSpec;    // initial list 1 (= srcList)
                const char *d_ySpec;    // initial list 2
                int         d_yi;       // index at which to insert y into x
                int         d_di;       // index at which to insert into x
                int         d_si;       // index at which to insert from x
                int         d_ne;       // number of elements to insert
            } DATA[] = {
                //line  x-list     y-list     yi   di   si  ne
                //----  ------     ------     --   --   --  --
                { L_,   "",        "",        0,   0,   0,  0,      },
                { L_,   "xA",      "",        0,   0,   0,  0,      },

                { L_,   "",        "xA",      0,   0,   0,  0,      },
                { L_,   "",        "xA",      0,   0,   0,  1,      },

                { L_,   "xA",      "xT",      0,   0,   0,  1,      },
                { L_,   "xA",      "xT",      0,   1,   0,  1,      },
                { L_,   "xA",      "xU",      0,   0,   0,  1,      },
                { L_,   "xA",      "xU",      0,   1,   0,  1,      },
                { L_,   "xU",      "xA",      0,   0,   0,  1,      },
                { L_,   "xU",      "xA",      0,   1,   0,  1,      },
                { L_,   "xU",      "yU",      0,   0,   0,  1,      },
                { L_,   "xU",      "yU",      0,   1,   0,  1,      },
                { L_,   "xA",      "xT",      1,   0,   0,  1,      },
                { L_,   "xA",      "xT",      1,   1,   0,  1,      },
                { L_,   "xA",      "xU",      1,   0,   0,  1,      },
                { L_,   "xA",      "xU",      1,   1,   0,  1,      },
                { L_,   "xU",      "xA",      1,   0,   0,  1,      },
                { L_,   "xU",      "xA",      1,   1,   0,  1,      },
                { L_,   "xU",      "yU",      1,   0,   0,  1,      },
                { L_,   "xU",      "yU",      1,   1,   0,  1,      },
                { L_,   "xAxBxC",  "yAyB",    0,   0,   0,  1,      },
                { L_,   "xAxBxC",  "yAyB",    0,   0,   0,  2,      },
                { L_,   "xAxBxC",  "yAyB",    0,   0,   1,  1,      },
                { L_,   "xAxBxC",  "yAyB",    0,   1,   0,  1,      },
                { L_,   "xAxBxC",  "yAyB",    0,   1,   0,  2,      },
                { L_,   "xAxBxC",  "yAyB",    0,   1,   1,  1,      },
                { L_,   "xAxBxC",  "yAyB",    0,   2,   0,  1,      },
                { L_,   "xAxBxC",  "yAyB",    0,   2,   0,  2,      },
                { L_,   "xAxBxC",  "yAyB",    0,   2,   1,  1,      },
                { L_,   "xAxBxC",  "yAyB",    0,   3,   0,  1,      },
                { L_,   "xAxBxC",  "yAyB",    0,   3,   1,  1,      },
                { L_,   "xAxBxC",  "yAyB",    1,   0,   0,  1,      },
                { L_,   "xAxBxC",  "yAyB",    1,   0,   0,  2,      },
                { L_,   "xAxBxC",  "yAyB",    1,   0,   1,  1,      },
                { L_,   "xAxBxC",  "yAyB",    1,   1,   0,  1,      },
                { L_,   "xAxBxC",  "yAyB",    1,   1,   0,  2,      },
                { L_,   "xAxBxC",  "yAyB",    1,   1,   1,  1,      },
                { L_,   "xAxBxC",  "yAyB",    1,   2,   0,  1,      },
                { L_,   "xAxBxC",  "yAyB",    1,   2,   0,  2,      },
                { L_,   "xAxBxC",  "yAyB",    1,   2,   1,  1,      },
                { L_,   "xAxBxC",  "yAyB",    1,   3,   0,  1,      },
                { L_,   "xAxBxC",  "yAyB",    1,   3,   1,  1,      },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *X_SPEC = DATA[ti].d_xSpec;
                const char *Y_SPEC = DATA[ti].d_ySpec;
                const int   YI     = DATA[ti].d_yi;
                const int   DI     = DATA[ti].d_di;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;

                const int   DEPTH  = (strlen(X_SPEC) + strlen(Y_SPEC))/2;
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                List DD(g(X_SPEC));        // control for destination
                List YY(g(Y_SPEC));        // list inserted into DD
                DD.insertList(YI, YY);

                // The insert function has already been tested for the
                // non overlap case.  Generate the expected result using
                // the same data but no overlapping.

                List EE(DD);               // copy of destination.
                List SS(DD);               // copy for source.
                LOOP_ASSERT(X_SPEC, EE.length() > YI);
                replaceListElements(EE, DI, SS.theList(YI).row(), SI, NE);

#if !defined(BSLS_PLATFORM__CMP_MSVC)
                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(X_SPEC); P_(Y_SPEC); P_(YI); P_(DI);
                                                P_(SI);     P_(NE);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }
#endif

                if (veryVerbose) cout << "\t\treplace(di, sl, si, ne)" << endl;
                {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    List x(DD, &testAllocator);  const List &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    replaceListElements(x, DI, X.theList(YI).row(), SI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\treplace(di, sl, si)" << endl;
                if (1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    List x(DD, &testAllocator);  const List &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X);
                                       P(DI) P(YI) P(SI) }
                    x.replaceElement(DI, X.theList(YI)[SI]);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }
      }

DEFINE_TEST_CASE(17) {

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout <<
            "\nTesting x.replace(di, sl, si, ne) et al. (overlapping)"
            << endl;
        {
            // Source contains destination list
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_xSpec;    // initial list 1 (= srcList)
                const char *d_ySpec;    // initial list 2
                int         d_yi;       // index at which to insert y into x
                int         d_di;       // index at which to insert into x
                int         d_si;       // index at which to insert from x
                int         d_ne;       // number of elements to insert
            } DATA[] = {
                //line  x-list     y-list     yi   di   si  ne
                //----  ------     ------     --   --   --  --
                { L_,   "",        "",        0,   0,   0,  0,      },

                //line  x-list     y-list     yi   di   si  ne
                //----  ------     ------     --   --   --  --
                { L_,   "",        "xA",      0,   0,   0,  1,      },

                //line  x-list     y-list     yi   di   si  ne
                //----  ------     ------     --   --   --  --
                { L_,   "",        "xAxB",    0,   0,   0,  1,      },
                { L_,   "",        "xAxB",    0,   1,   0,  1,      },
                { L_,   "xA",      "xT",      0,   0,   0,  1,      },
                { L_,   "xA",      "xU",      0,   0,   0,  1,      },
                { L_,   "xU",      "xA",      0,   0,   0,  1,      },
                { L_,   "xU",      "yU",      0,   0,   0,  1,      },
                { L_,   "xA",      "xT",      1,   0,   0,  1,      },
                { L_,   "xA",      "xU",      1,   0,   0,  1,      },
                { L_,   "xU",      "xA",      1,   0,   0,  1,      },
                { L_,   "xU",      "yU",      1,   0,   0,  1,      },

                //line  x-list     y-list     yi   di   si  ne
                //----  ------     ------     --   --   --  --
                { L_,   "xA",      "xTyB",    0,   0,   0,  1,      },
                { L_,   "xA",      "xUyB",    0,   0,   0,  1,      },
                { L_,   "xU",      "xAyB",    0,   0,   0,  1,      },
                { L_,   "xU",      "yUyB",    0,   0,   0,  1,      },
                { L_,   "xA",      "xTyB",    1,   0,   0,  1,      },
                { L_,   "xA",      "xUyB",    1,   0,   0,  1,      },
                { L_,   "xU",      "xAyB",    1,   0,   0,  1,      },
                { L_,   "xU",      "yUyB",    1,   0,   0,  1,      },
                { L_,   "xA",      "xTyB",    0,   1,   0,  1,      },
                { L_,   "xA",      "xUyB",    0,   1,   0,  1,      },
                { L_,   "xU",      "xAyB",    0,   1,   0,  1,      },
                { L_,   "xU",      "yUyB",    0,   1,   0,  1,      },
                { L_,   "xA",      "xTyB",    1,   1,   0,  1,      },
                { L_,   "xA",      "xUyB",    1,   1,   0,  1,      },
                { L_,   "xU",      "xAyB",    1,   1,   0,  1,      },
                { L_,   "xU",      "yUyB",    1,   1,   0,  1,      },
                { L_,   "xA",      "xTyB",    0,   0,   1,  1,      },
                { L_,   "xA",      "xUyB",    0,   0,   1,  1,      },
                { L_,   "xU",      "xAyB",    0,   0,   1,  1,      },
                { L_,   "xU",      "yUyB",    0,   0,   1,  1,      },
                { L_,   "xA",      "xTyB",    1,   0,   1,  1,      },
                { L_,   "xA",      "xUyB",    1,   0,   1,  1,      },
                { L_,   "xU",      "xAyB",    1,   0,   1,  1,      },
                { L_,   "xU",      "yUyB",    1,   0,   1,  1,      },
                { L_,   "xA",      "xTyB",    0,   0,   0,  2,      },
                { L_,   "xA",      "xUyB",    0,   0,   0,  2,      },
                { L_,   "xU",      "xAyB",    0,   0,   0,  2,      },
                { L_,   "xU",      "yUyB",    0,   0,   0,  2,      },
                { L_,   "xA",      "xTyB",    1,   0,   0,  2,      },
                { L_,   "xA",      "xUyB",    1,   0,   0,  2,      },
                { L_,   "xU",      "xAyB",    1,   0,   0,  2,      },
                { L_,   "xU",      "yUyB",    1,   0,   0,  2,      },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *X_SPEC = DATA[ti].d_xSpec;
                const char *Y_SPEC = DATA[ti].d_ySpec;
                const int   YI     = DATA[ti].d_yi;
                const int   DI     = DATA[ti].d_di;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;

                const int   DEPTH  = (strlen(X_SPEC) + strlen(Y_SPEC))/2;
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                // Insert one list into destination list to create test data.

                List DD(g(X_SPEC));        // control for destination
                List YY(g(Y_SPEC));        // list inserted into DD
                DD.insertList(YI, YY);

                // The insert function has already been tested for the
                // non overlap case.  Generate the expected result using
                // the same data but no overlapping.

                List EE(DD);               // copy of destination.
                List SS(DD);               // copy for source.
                LOOP_ASSERT(X_SPEC, EE.length() > YI);
                replaceListElements(EE.theModifiableList(YI), DI, SS.row(),
                                                                       SI, NE);

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(X_SPEC); P_(Y_SPEC); P_(YI); P_(DI);
                                                P_(SI);     P_(NE);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                if (veryVerbose) cout << "\t\treplace(di, sl, si, ne)" << endl;
                {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    List x(DD, &testAllocator);  const List &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                    replaceListElements(x.theModifiableList(YI), DI, X.row(),
                                                                       SI, NE);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\treplace(di, sl, si)" << endl;
                if (1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    List x(DD, &testAllocator);  const List &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); P(SI)
                                                                       P(YI) }
                    x.theModifiableList(YI).replaceElement(DI, X[SI]);
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, EE == X);
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }
      }

DEFINE_TEST_CASE(16) {
        // --------------------------------------------------------------------
        // TESTING 'bdem_List' APPEND, INSERT, REMOVE METHODS
        // Concerns:
        //   For the 'append' and 'insert' methods, the following properties
        //   must hold:
        //    1. The source is left unaffected (apart from aliasing).
        //    2. The subsequent existence of the source has no effect on the
        //       result object (apart from aliasing).
        //    3. The function is alias and overlap safe.
        //    4. The function is exception neutral (w.r.t. allocation).
        //    5. The function preserves object invariants.
        //    6. The function is independent of internal representation.
        //
        // Plan:
        //    The loop in this case sets up 6 variables:
        //    'D_SPEC': "Destination Spec", the spec of 'DD', the list that is
        //              the list to be manipulated
        //    'DI':     the destination index, that is, where in 'DD' the
        //              operation is to target
        //    'S_SPEC': the "Source Spec", the spec of 'SS', the source list,
        //              the 2nd list passed to the manipulator
        //    'SI':     "Source Index", the index into 'SS',
        //    'NE':     the number of elements the manipulator is to process
        //    'E_SPEC': "Expected Spec", the spec of 'EE', the list that should
        //              result from doing
        //              'destinationList.insertElements(DI, sourceList, SI)'.
        //    In the loop a very large number of subtests are specified, and
        //    are molded into tests of different manipulators.  Many of the
        //    manipulator tests only work on a suitable subset of all the tests
        //    in the loop.
        //    All test start with 'List x(DD, &testAllocator)' to create
        //    a modifiable copy of 'DD'.  At the end of the test, 'x' is
        //    usually verified to equal 'EE'.  Some tests, where the value of
        //    'x' after manipulation will not equal 'EE', create a copy 'ee' of
        //    'EE' and modify it with other manipulators to be the value we
        //    expect x to equal.
        //    As for the source list 'SS', we take a modifiable copy 's' of
        //    it and use that for the manipulation of 'x', then verify that
        //    's' was unchanged by the operation.  We have 's' go out of scope
        //    and verify that 'x' was unchanged by 's' going out of scope.
        //
        //    Subtests:
        //    - all cases: x.insertElements(DI, s, SI, NE)
        //    - all cases: x.insertElements(DI, s.row(), SI, NE)
        //    - only if NE == length(SS): x.insertElements(DI, SS)
        //    - only if NE == length(SS): x.insertElements(DI, SS.row())
        //    - only if NE == 1: x.insertElement(DI, SS[SI])
        //    - only if NE == 1: x.insert{TYPE(SS[SI])}(DI, {object at SS[SI]})
        //    - only if NE == 1 && SS(SI) is unset: x.insertNullTYPE(DI)
        //    - only if NE == 1: x.insertNull{TYPE(SS[SI]}(DI)
        //    - only if NE == 1: x.insertNull{TYPE(SS[SI]}(DI)
        //    - only if DI == length(DD): x.appendElements(SS, SI, NE)
        //    - only if DI == length(DD) && NE == length(SS):
        //                                                 x.appendElements(SS)
        //    - only if DI == length(DD) && NE == 1: x.appendElement(SS[SI])
        //    - only if DI == length(DD) && NE == 1:
        //                             x.append{TYPE(SS[SI])}(object at SS[SI])
        //    - only if DI == length(DD) && NE == 1:
        //                                         x.appendNull{TYPE(SS[SI]}()
        //    - only if DI == length(DD) && NE == 1:x.appendNull{TYPE(SS[SI]}()
        //    - all cases: x = EE; x.removeElements(DI, NE); ASSERT(X == DD)
        //    - only if NE == 1: x = EE; x.removeElement(DI); ASSERT(X == DD)
        //    - all cases: x = EE; x.removeAll(); ASSERT(X == empty)
        //
        // Testing:
        //   void appendNullChar();
        //   char& appendChar(char v);
        //   void appendNullShort();
        //   short& appendShort(short v);
        //   void appendNullInt();
        //   int& appendInt(int v);
        //   void appendNullInt64();
        //   Int64& appendInt64(Int64 v);
        //   void appendNullFloat();
        //   float& appendFloat(float v);
        //   void appendNullDouble();
        //   double& appendDouble(double v);
        //   void appendNullString();
        //   bsl::string& appendString(const char *v);
        //   bsl::string& appendString(const bsl::string& v);
        //   void appendNullDatetime();
        //   bdet_Datetime& appendDatetime(const bdet_Datetime& v);
        //   void appendNullDate();
        //   bdet_Date& appendDate(const bdet_Date& v);
        //   void appendNullTime();
        //   bdet_Time& appendTime(const bdet_Time& v);
        //   void appendNullCharArray();
        //   bsl::vector<char>& appendCharArray(const bsl::vector<char>& v);
        //   void appendNullShortArray();
        //   bsl::vector<short>& appendShortArray(const bsl::vector<short>& v);
        //   void appendNullIntArray();
        //   bsl::vector<int>& appendIntArray(const bsl::vector<int>& v);
        //   void appendNullInt64Array();
        //   bsl::vector<Int64>& appendInt64Array(const bsl::vector<Int64>& v);
        //   void appendNullFloatArray();
        //   bsl::vector<float>& appendFloatArray(const bsl::vector<float>& v);
        //   void appendNullDoubleArray();
        //   bsl::vector<double>&
        //                     appendDoubleArray(const bsl::vector<double>& v);
        //   void appendNullStringArray();
        //   bsl::vector<bsl::string>&
        //                appendStringArray(const bsl::vector<bsl::string>& v);
        //   void appendNullDatetimeArray();
        //   bsl::vector<bdet_Datetime>&
        //            appendDatetimeArray(const bsl::vector<bdet_Datetime>& v);
        //   void appendNullDateArray();
        //   bsl::vector<bdet_Date>&
        //                    appendDateArray(const bsl::vector<bdet_Date>& v);
        //   void appendNullTimeArray();
        //   bsl::vector<bdet_Time>&
        //                    appendTimeArray(const bsl::vector<bdet_Time>& v);
        //   void appendNullList();
        //   bdem_List& appendList(const bdem_List& list);
        //   bdem_List& appendList(const bdem_Row& row);
        //   void appendNullTable();
        //   bdem_Table& appendTable(const bdem_Table& table);
        //
        //   void appendNullBool();
        //   bool& appendBool();
        //   void appendNullDatetimeTz();
        //   bdem_DatetimeTz& appendDatetimeTz();
        //   void appendNullDateTz();
        //   bdem_DateTz& appendDateTz();
        //   void appendNullTimeTz();
        //   bdem_TimeTz& appendTimeTz();
        //   void appendNullBoolArray();
        //   bsl::vector<bool>& appendBoolArray();
        //   void appendNullDatetimeTz();
        //   bsl::vector<bdem_DatetimeTz>& appendDatetimeTz();
        //   void appendNullDateTz();
        //   bsl::vector<bdem_DateTz>& appendDateTz();
        //   void appendNullTimeTz();
        //   bsl::vector<bdem_TimeTz>& appendTimeTz();
        //   void appendNullChoice();
        //   bdem_Choice& appendChoice();
        //   void appendNullChoiceArray();
        //   bdem_ChoiceArray& appendChoiceArray();
        //
        //   int appendElement(const bdem_Row& sr, int si);
        //   int appendElement(const bdem_List& sl, int si);
        //   void appendElements(const bdem_Row& sr);
        //   void appendElements(const bdem_List& sl);
        //   void appendElements(const bdem_Row& sr, int si, int ne);
        //   void appendElements(const bdem_List& sl, int si, int ne);
        //
        //   char& insertChar(int di, char v);
        //   short& insertShort(int di, short v);
        //   int& insertInt(int di, int v);
        //   Int64& insertInt64(int di, Int64 v);
        //   float& insertFloat(int di, float v);
        //   double& insertDouble(int di, double v);
        //   bsl::string& insertString(int di, const char *v);
        //   bsl::string& insertString(int di, const bsl::string& v);
        //   bdet_Datetime& insertDatetime(int di, const bdet_Datetime& v);
        //   bdet_Date& insertDate(int di, const bdet_Date& v);
        //   bdet_Time& insertTime(int di, const bdet_Time& v);
        //   bsl::vector<char>&
        //                 insertCharArray(int di, const bsl::vector<char>& v);
        //   bsl::vector<short>&
        //               insertShortArray(int di, const bsl::vector<short>& v);
        //   bsl::vector<int>& insertIntArray(int di, const bsl::vector<int>&);
        //   bsl::vector<Int64>&
        //               insertInt64Array(int di, const bsl::vector<Int64>& v);
        //   bsl::vector<float>&
        //               insertFloatArray(int di, const bsl::vector<float>& v);
        //   bsl::vector<double>&
        //             insertDoubleArray(int di, const bsl::vector<double>& v);
        //   bsl::vector<bsl::string>&
        //        insertStringArray(int di, const bsl::vector<bsl::string>& v);
        //   bsl::vector<bdet_Datetime>&
        //    insertDatetimeArray(int di, const bsl::vector<bdet_Datetime>& v);
        //   bsl::vector<bdet_Date>&
        //            insertDateArray(int di, const bsl::vector<bdet_Date>& v);
        //   bsl::vector<bdet_Time>&
        //            insertTimeArray(int di, const bsl::vector<bdet_Time>& v);
        //   bdem_List& insertList(int di, const bdem_List& sl);
        //   bdem_List& insertList(int di, const bdem_Row& sr);
        //   bdem_Table& insertTable(int di, const bdem_Table& st);
        //
        //   void insertElement(int di, const bdem_Row& sr, int si);
        //   void insertElement(int di, const bdem_List& sl, int si);
        //   void insertElements(int di, const bdem_Row& sr);
        //   void insertElements(int di, const bdem_List& sl);
        //   void insertElements(int di, const bdem_Row& sr, int si, int ne);
        //   void insertElements(int di, const bdem_List& sl, int si, int ne);
        //
        //   void removeElement(int index);
        //   void removeElement(int si, int ne);
        //   void removeAll();
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl
            << "Testing 'append', 'insert', and 'remove'" << endl
            << "========================================" << endl;

        int passCount;

        if (verbose) cout <<
            "\nTesting x.insertElements(di, sl, si, ne) et al. (no aliasing)"
            << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_dlSpec;   // initial (destination) list
                int         d_di;       // index at which to insert into da
                const char *d_slSpec;   // source list
                int         d_si;       // index at which to insert from sl
                int         d_ne;       // number of elements to insert
                const char *d_expSpec;  // expected list value
            } DATA[] = {
                //line  d-list    di   s-list    si  ne  expected
                //----  ------    --   ------    --  --  --------   Depth = 0
                { L_,   "",       0,   "",       0,  0,  ""                },

                //line  d-list    di   s-list    si  ne  expected
                //----  ------    --   ------    --  --  --------   Depth = 1
                { L_,   "xC",     0,   "",       0,  0,  "xC"              },
                { L_,   "xC",     1,   "",       0,  0,  "xC"              },

                { L_,   "",       0,  "xC",      0,  0,  ""                },
                { L_,   "",       0,  "xA",      0,  1,  "xA"              },
                { L_,   "",       0,  "xB",      0,  1,  "xB"              },
                { L_,   "",       0,  "xC",      0,  1,  "xC"              },
                { L_,   "",       0,  "xD",      0,  1,  "xD"              },
                { L_,   "",       0,  "xE",      0,  1,  "xE"              },
                { L_,   "",       0,  "xF",      0,  1,  "xF"              },
                { L_,   "",       0,  "xG",      0,  1,  "xG"              },
                { L_,   "",       0,  "xH",      0,  1,  "xH"              },
                { L_,   "",       0,  "xI",      0,  1,  "xI"              },
                { L_,   "",       0,  "xJ",      0,  1,  "xJ"              },
                { L_,   "",       0,  "xK",      0,  1,  "xK"              },
                { L_,   "",       0,  "xL",      0,  1,  "xL"              },
                { L_,   "",       0,  "xM",      0,  1,  "xM"              },
                { L_,   "",       0,  "xN",      0,  1,  "xN"              },
                { L_,   "",       0,  "xO",      0,  1,  "xO"              },
                { L_,   "",       0,  "xP",      0,  1,  "xP"              },
                { L_,   "",       0,  "xQ",      0,  1,  "xQ"              },
                { L_,   "",       0,  "xR",      0,  1,  "xR"              },
                { L_,   "",       0,  "xS",      0,  1,  "xS"              },
                { L_,   "",       0,  "xT",      0,  1,  "xT"              },
                { L_,   "",       0,  "xU",      0,  1,  "xU"              },
                { L_,   "",       0,  "xV",      0,  1,  "xV"              },
                { L_,   "",       0,  "xW",      0,  1,  "xW"              },
                { L_,   "",       0,  "xX",      0,  1,  "xX"              },
                { L_,   "",       0,  "xY",      0,  1,  "xY"              },
                { L_,   "",       0,  "xZ",      0,  1,  "xZ"              },
                { L_,   "",       0,  "xa",      0,  1,  "xa"              },
                { L_,   "",       0,  "xb",      0,  1,  "xb"              },
                { L_,   "",       0,  "xc",      0,  1,  "xc"              },
                { L_,   "",       0,  "xd",      0,  1,  "xd"              },
                { L_,   "",       0,  "xe",      0,  1,  "xe"              },
                { L_,   "",       0,  "xf",      0,  1,  "xf"              },

                { L_,   "",       0,  "nA",      0,  1,  "nA"              },
                { L_,   "",       0,  "nB",      0,  1,  "nB"              },
                { L_,   "",       0,  "nC",      0,  1,  "nC"              },
                { L_,   "",       0,  "nD",      0,  1,  "nD"              },
                { L_,   "",       0,  "nE",      0,  1,  "nE"              },
                { L_,   "",       0,  "nF",      0,  1,  "nF"              },
                { L_,   "",       0,  "nG",      0,  1,  "nG"              },
                { L_,   "",       0,  "nH",      0,  1,  "nH"              },
                { L_,   "",       0,  "nI",      0,  1,  "nI"              },
                { L_,   "",       0,  "nJ",      0,  1,  "nJ"              },
                { L_,   "",       0,  "nK",      0,  1,  "nK"              },
                { L_,   "",       0,  "nL",      0,  1,  "nL"              },
                { L_,   "",       0,  "nM",      0,  1,  "nM"              },
                { L_,   "",       0,  "nN",      0,  1,  "nN"              },
                { L_,   "",       0,  "nO",      0,  1,  "nO"              },
                { L_,   "",       0,  "nP",      0,  1,  "nP"              },
                { L_,   "",       0,  "nR",      0,  1,  "nR"              },
                { L_,   "",       0,  "nQ",      0,  1,  "nQ"              },
                { L_,   "",       0,  "nS",      0,  1,  "nS"              },
                { L_,   "",       0,  "nT",      0,  1,  "nT"              },
                { L_,   "",       0,  "nU",      0,  1,  "nU"              },
                { L_,   "",       0,  "nV",      0,  1,  "nV"              },
                { L_,   "",       0,  "nW",      0,  1,  "nW"              },
                { L_,   "",       0,  "nX",      0,  1,  "nX"              },
                { L_,   "",       0,  "nY",      0,  1,  "nY"              },
                { L_,   "",       0,  "nZ",      0,  1,  "nZ"              },
                { L_,   "",       0,  "na",      0,  1,  "na"              },
                { L_,   "",       0,  "nb",      0,  1,  "nb"              },
                { L_,   "",       0,  "nc",      0,  1,  "nc"              },
                { L_,   "",       0,  "nd",      0,  1,  "nd"              },
                { L_,   "",       0,  "ne",      0,  1,  "ne"              },
                { L_,   "",       0,  "nf",      0,  1,  "nf"              },
                { L_,   "",       0,  "xC",      1,  0,  ""                },

                //line  d-list    di  s-list     si  ne  expected
                //----  ------    --  ------     --  --  --------   Depth = 2
                { L_,   "yT",     0,  "xA",      0,  1,  "xAyT"            },
                { L_,   "yT",     0,  "xB",      0,  1,  "xByT"            },
                { L_,   "yT",     0,  "xC",      0,  1,  "xCyT"            },
                { L_,   "yT",     0,  "xD",      0,  1,  "xDyT"            },
                { L_,   "yT",     0,  "xE",      0,  1,  "xEyT"            },
                { L_,   "yT",     0,  "xF",      0,  1,  "xFyT"            },
                { L_,   "yT",     0,  "xG",      0,  1,  "xGyT"            },
                { L_,   "yT",     0,  "xH",      0,  1,  "xHyT"            },
                { L_,   "yT",     0,  "xI",      0,  1,  "xIyT"            },
                { L_,   "yT",     0,  "xJ",      0,  1,  "xJyT"            },
                { L_,   "yT",     0,  "xK",      0,  1,  "xKyT"            },
                { L_,   "yT",     0,  "xL",      0,  1,  "xLyT"            },
                { L_,   "yT",     0,  "xM",      0,  1,  "xMyT"            },
                { L_,   "yT",     0,  "xN",      0,  1,  "xNyT"            },
                { L_,   "yT",     0,  "xO",      0,  1,  "xOyT"            },
                { L_,   "yT",     0,  "xP",      0,  1,  "xPyT"            },
                { L_,   "yT",     0,  "xQ",      0,  1,  "xQyT"            },
                { L_,   "yT",     0,  "xR",      0,  1,  "xRyT"            },
                { L_,   "yT",     0,  "xS",      0,  1,  "xSyT"            },
                { L_,   "yT",     0,  "xT",      0,  1,  "xTyT"            },
                { L_,   "yT",     0,  "xU",      0,  1,  "xUyT"            },
                { L_,   "yT",     0,  "xV",      0,  1,  "xVyT"            },
                { L_,   "yT",     0,  "xW",      0,  1,  "xWyT"            },
                { L_,   "yT",     0,  "xX",      0,  1,  "xXyT"            },
                { L_,   "yT",     0,  "xY",      0,  1,  "xYyT"            },
                { L_,   "yT",     0,  "xZ",      0,  1,  "xZyT"            },
                { L_,   "yT",     0,  "xa",      0,  1,  "xayT"            },
                { L_,   "yT",     0,  "xb",      0,  1,  "xbyT"            },
                { L_,   "yT",     0,  "xc",      0,  1,  "xcyT"            },
                { L_,   "yT",     0,  "xd",      0,  1,  "xdyT"            },
                { L_,   "yT",     0,  "xe",      0,  1,  "xeyT"            },
                { L_,   "yT",     0,  "xf",      0,  1,  "xfyT"            },

                { L_,   "yT",     0,  "nTxA",    1,  1,  "xAyT"            },
                { L_,   "yT",     0,  "nTxB",    1,  1,  "xByT"            },
                { L_,   "yT",     0,  "nTxC",    1,  1,  "xCyT"            },
                { L_,   "yT",     0,  "nTxD",    1,  1,  "xDyT"            },
                { L_,   "yT",     0,  "nTxE",    1,  1,  "xEyT"            },
                { L_,   "yT",     0,  "nTxF",    1,  1,  "xFyT"            },
                { L_,   "yT",     0,  "nTxG",    1,  1,  "xGyT"            },
                { L_,   "yT",     0,  "nTxH",    1,  1,  "xHyT"            },
                { L_,   "yT",     0,  "nTxI",    1,  1,  "xIyT"            },
                { L_,   "yT",     0,  "nTxJ",    1,  1,  "xJyT"            },
                { L_,   "yT",     0,  "nTxK",    1,  1,  "xKyT"            },
                { L_,   "yT",     0,  "nTxL",    1,  1,  "xLyT"            },
                { L_,   "yT",     0,  "nTxM",    1,  1,  "xMyT"            },
                { L_,   "yT",     0,  "nTxN",    1,  1,  "xNyT"            },
                { L_,   "yT",     0,  "nTxO",    1,  1,  "xOyT"            },
                { L_,   "yT",     0,  "nTxP",    1,  1,  "xPyT"            },
                { L_,   "yT",     0,  "nTxQ",    1,  1,  "xQyT"            },
                { L_,   "yT",     0,  "nTxR",    1,  1,  "xRyT"            },
                { L_,   "yT",     0,  "nTxS",    1,  1,  "xSyT"            },
                { L_,   "yT",     0,  "nTxT",    1,  1,  "xTyT"            },
                { L_,   "yT",     0,  "nTxU",    1,  1,  "xUyT"            },
                { L_,   "yT",     0,  "nTxV",    1,  1,  "xVyT"            },
                { L_,   "yT",     0,  "nTxW",    1,  1,  "xWyT"            },
                { L_,   "yT",     0,  "nTxX",    1,  1,  "xXyT"            },
                { L_,   "yT",     0,  "nTxY",    1,  1,  "xYyT"            },
                { L_,   "yT",     0,  "nTxZ",    1,  1,  "xZyT"            },
                { L_,   "yT",     0,  "nTxa",    1,  1,  "xayT"            },
                { L_,   "yT",     0,  "nTxb",    1,  1,  "xbyT"            },
                { L_,   "yT",     0,  "nTxc",    1,  1,  "xcyT"            },
                { L_,   "yT",     0,  "nTxd",    1,  1,  "xdyT"            },
                { L_,   "yT",     0,  "nTxe",    1,  1,  "xeyT"            },
                { L_,   "yT",     0,  "nTxf",    1,  1,  "xfyT"            },

                { L_,   "yT",     1,  "xA",      0,  1,  "yTxA"            },
                { L_,   "yT",     1,  "xB",      0,  1,  "yTxB"            },
                { L_,   "yT",     1,  "xC",      0,  1,  "yTxC"            },
                { L_,   "yT",     1,  "xD",      0,  1,  "yTxD"            },
                { L_,   "yT",     1,  "xE",      0,  1,  "yTxE"            },
                { L_,   "yT",     1,  "xF",      0,  1,  "yTxF"            },
                { L_,   "yT",     1,  "xG",      0,  1,  "yTxG"            },
                { L_,   "yT",     1,  "xH",      0,  1,  "yTxH"            },
                { L_,   "yT",     1,  "xI",      0,  1,  "yTxI"            },
                { L_,   "yT",     1,  "xJ",      0,  1,  "yTxJ"            },
                { L_,   "yT",     1,  "xK",      0,  1,  "yTxK"            },
                { L_,   "yT",     1,  "xL",      0,  1,  "yTxL"            },
                { L_,   "yT",     1,  "xM",      0,  1,  "yTxM"            },
                { L_,   "yT",     1,  "xN",      0,  1,  "yTxN"            },
                { L_,   "yT",     1,  "xO",      0,  1,  "yTxO"            },
                { L_,   "yT",     1,  "xP",      0,  1,  "yTxP"            },
                { L_,   "yT",     1,  "xQ",      0,  1,  "yTxQ"            },
                { L_,   "yT",     1,  "xR",      0,  1,  "yTxR"            },
                { L_,   "yT",     1,  "xS",      0,  1,  "yTxS"            },
                { L_,   "yT",     1,  "xT",      0,  1,  "yTxT"            },
                { L_,   "yT",     1,  "xU",      0,  1,  "yTxU"            },
                { L_,   "yT",     1,  "xV",      0,  1,  "yTxV"            },
                { L_,   "yT",     1,  "xW",      0,  1,  "yTxW"            },
                { L_,   "yT",     1,  "xX",      0,  1,  "yTxX"            },
                { L_,   "yT",     1,  "xY",      0,  1,  "yTxY"            },
                { L_,   "yT",     1,  "xZ",      0,  1,  "yTxZ"            },
                { L_,   "yT",     1,  "xa",      0,  1,  "yTxa"            },
                { L_,   "yT",     1,  "xb",      0,  1,  "yTxb"            },
                { L_,   "yT",     1,  "xc",      0,  1,  "yTxc"            },
                { L_,   "yT",     1,  "xd",      0,  1,  "yTxd"            },
                { L_,   "yT",     1,  "xe",      0,  1,  "yTxe"            },
                { L_,   "yT",     1,  "xf",      0,  1,  "yTxf"            },

                { L_,   "yT",     1,  "nAxA",    1,  1,  "yTxA"            },
                { L_,   "yT",     1,  "nAxB",    1,  1,  "yTxB"            },
                { L_,   "yT",     1,  "nAxC",    1,  1,  "yTxC"            },
                { L_,   "yT",     1,  "nAxD",    1,  1,  "yTxD"            },
                { L_,   "yT",     1,  "nAxE",    1,  1,  "yTxE"            },
                { L_,   "yT",     1,  "nAxF",    1,  1,  "yTxF"            },
                { L_,   "yT",     1,  "nAxG",    1,  1,  "yTxG"            },
                { L_,   "yT",     1,  "nAxH",    1,  1,  "yTxH"            },
                { L_,   "yT",     1,  "nAxI",    1,  1,  "yTxI"            },
                { L_,   "yT",     1,  "nAxJ",    1,  1,  "yTxJ"            },
                { L_,   "yT",     1,  "nAxK",    1,  1,  "yTxK"            },
                { L_,   "yT",     1,  "nAxL",    1,  1,  "yTxL"            },
                { L_,   "yT",     1,  "nAxM",    1,  1,  "yTxM"            },
                { L_,   "yT",     1,  "nAxN",    1,  1,  "yTxN"            },
                { L_,   "yT",     1,  "nAxO",    1,  1,  "yTxO"            },
                { L_,   "yT",     1,  "nAxP",    1,  1,  "yTxP"            },
                { L_,   "yT",     1,  "nAxQ",    1,  1,  "yTxQ"            },
                { L_,   "yT",     1,  "nAxR",    1,  1,  "yTxR"            },
                { L_,   "yT",     1,  "nAxS",    1,  1,  "yTxS"            },
                { L_,   "yT",     1,  "nAxT",    1,  1,  "yTxT"            },
                { L_,   "yT",     1,  "nAxU",    1,  1,  "yTxU"            },
                { L_,   "yT",     1,  "nAxV",    1,  1,  "yTxV"            },
                { L_,   "yT",     1,  "nAxW",    1,  1,  "yTxW"            },
                { L_,   "yT",     1,  "nAxX",    1,  1,  "yTxX"            },
                { L_,   "yT",     1,  "nAxY",    1,  1,  "yTxY"            },
                { L_,   "yT",     1,  "nAxZ",    1,  1,  "yTxZ"            },
                { L_,   "yT",     1,  "nAxa",    1,  1,  "yTxa"            },
                { L_,   "yT",     1,  "nAxb",    1,  1,  "yTxb"            },
                { L_,   "yT",     1,  "nAxc",    1,  1,  "yTxc"            },
                { L_,   "yT",     1,  "nAxd",    1,  1,  "yTxd"            },
                { L_,   "yT",     1,  "nAxe",    1,  1,  "yTxe"            },
                { L_,   "yT",     1,  "nAxf",    1,  1,  "yTxf"            },

                { L_,   "yT",     1,  "nA",      0,  1,  "yTnA"            },
                { L_,   "yT",     1,  "nB",      0,  1,  "yTnB"            },
                { L_,   "yT",     1,  "nC",      0,  1,  "yTnC"            },
                { L_,   "yT",     1,  "nD",      0,  1,  "yTnD"            },
                { L_,   "yT",     1,  "nE",      0,  1,  "yTnE"            },
                { L_,   "yT",     1,  "nF",      0,  1,  "yTnF"            },
                { L_,   "yT",     1,  "nG",      0,  1,  "yTnG"            },
                { L_,   "yT",     1,  "nH",      0,  1,  "yTnH"            },
                { L_,   "yT",     1,  "nI",      0,  1,  "yTnI"            },
                { L_,   "yT",     1,  "nJ",      0,  1,  "yTnJ"            },
                { L_,   "yT",     1,  "nK",      0,  1,  "yTnK"            },
                { L_,   "yT",     1,  "nL",      0,  1,  "yTnL"            },
                { L_,   "yT",     1,  "nM",      0,  1,  "yTnM"            },
                { L_,   "yT",     1,  "nN",      0,  1,  "yTnN"            },
                { L_,   "yT",     1,  "nO",      0,  1,  "yTnO"            },
                { L_,   "yT",     1,  "nP",      0,  1,  "yTnP"            },
                { L_,   "yT",     1,  "nQ",      0,  1,  "yTnQ"            },
                { L_,   "yT",     1,  "nR",      0,  1,  "yTnR"            },
                { L_,   "yT",     1,  "nS",      0,  1,  "yTnS"            },
                { L_,   "yT",     1,  "nT",      0,  1,  "yTnT"            },
                { L_,   "yT",     1,  "nU",      0,  1,  "yTnU"            },
                { L_,   "yT",     1,  "nV",      0,  1,  "yTnV"            },
                { L_,   "yT",     1,  "nW",      0,  1,  "yTnW"            },
                { L_,   "yT",     1,  "nX",      0,  1,  "yTnX"            },
                { L_,   "yT",     1,  "nY",      0,  1,  "yTnY"            },
                { L_,   "yT",     1,  "nZ",      0,  1,  "yTnZ"            },
                { L_,   "yT",     1,  "na",      0,  1,  "yTna"            },
                { L_,   "yT",     1,  "nb",      0,  1,  "yTnb"            },
                { L_,   "yT",     1,  "nc",      0,  1,  "yTnc"            },
                { L_,   "yT",     1,  "nd",      0,  1,  "yTnd"            },
                { L_,   "yT",     1,  "ne",      0,  1,  "yTne"            },
                { L_,   "yT",     1,  "nf",      0,  1,  "yTnf"            },

                { L_,   "yU",     0,  "xA",      0,  1,  "xAyU"            },
                { L_,   "yU",     0,  "xB",      0,  1,  "xByU"            },
                { L_,   "yU",     0,  "xC",      0,  1,  "xCyU"            },
                { L_,   "yU",     0,  "xD",      0,  1,  "xDyU"            },
                { L_,   "yU",     0,  "xE",      0,  1,  "xEyU"            },
                { L_,   "yU",     0,  "xF",      0,  1,  "xFyU"            },
                { L_,   "yU",     0,  "xG",      0,  1,  "xGyU"            },
                { L_,   "yU",     0,  "xH",      0,  1,  "xHyU"            },
                { L_,   "yU",     0,  "xI",      0,  1,  "xIyU"            },
                { L_,   "yU",     0,  "xJ",      0,  1,  "xJyU"            },
                { L_,   "yU",     0,  "xK",      0,  1,  "xKyU"            },
                { L_,   "yU",     0,  "xL",      0,  1,  "xLyU"            },
                { L_,   "yU",     0,  "xM",      0,  1,  "xMyU"            },
                { L_,   "yU",     0,  "xN",      0,  1,  "xNyU"            },
                { L_,   "yU",     0,  "xO",      0,  1,  "xOyU"            },
                { L_,   "yU",     0,  "xP",      0,  1,  "xPyU"            },
                { L_,   "yU",     0,  "xQ",      0,  1,  "xQyU"            },
                { L_,   "yU",     0,  "xR",      0,  1,  "xRyU"            },
                { L_,   "yU",     0,  "xS",      0,  1,  "xSyU"            },
                { L_,   "yU",     0,  "xU",      0,  1,  "xUyU"            },
                { L_,   "yU",     0,  "xU",      0,  1,  "xUyU"            },
                { L_,   "yU",     0,  "xV",      0,  1,  "xVyU"            },
                { L_,   "yU",     0,  "xW",      0,  1,  "xWyU"            },
                { L_,   "yU",     0,  "xX",      0,  1,  "xXyU"            },
                { L_,   "yU",     0,  "xY",      0,  1,  "xYyU"            },
                { L_,   "yU",     0,  "xZ",      0,  1,  "xZyU"            },
                { L_,   "yU",     0,  "xa",      0,  1,  "xayU"            },
                { L_,   "yU",     0,  "xb",      0,  1,  "xbyU"            },
                { L_,   "yU",     0,  "xc",      0,  1,  "xcyU"            },
                { L_,   "yU",     0,  "xd",      0,  1,  "xdyU"            },
                { L_,   "yU",     0,  "xe",      0,  1,  "xeyU"            },
                { L_,   "yU",     0,  "xf",      0,  1,  "xfyU"            },

                { L_,   "yU",     1,  "xA",      0,  1,  "yUxA"            },
                { L_,   "yU",     1,  "xB",      0,  1,  "yUxB"            },
                { L_,   "yU",     1,  "xC",      0,  1,  "yUxC"            },
                { L_,   "yU",     1,  "xD",      0,  1,  "yUxD"            },
                { L_,   "yU",     1,  "xE",      0,  1,  "yUxE"            },
                { L_,   "yU",     1,  "xF",      0,  1,  "yUxF"            },
                { L_,   "yU",     1,  "xG",      0,  1,  "yUxG"            },
                { L_,   "yU",     1,  "xH",      0,  1,  "yUxH"            },
                { L_,   "yU",     1,  "xI",      0,  1,  "yUxI"            },
                { L_,   "yU",     1,  "xJ",      0,  1,  "yUxJ"            },
                { L_,   "yU",     1,  "xK",      0,  1,  "yUxK"            },
                { L_,   "yU",     1,  "xL",      0,  1,  "yUxL"            },
                { L_,   "yU",     1,  "xM",      0,  1,  "yUxM"            },
                { L_,   "yU",     1,  "xN",      0,  1,  "yUxN"            },
                { L_,   "yU",     1,  "xO",      0,  1,  "yUxO"            },
                { L_,   "yU",     1,  "xP",      0,  1,  "yUxP"            },
                { L_,   "yU",     1,  "xQ",      0,  1,  "yUxQ"            },
                { L_,   "yU",     1,  "xR",      0,  1,  "yUxR"            },
                { L_,   "yU",     1,  "xS",      0,  1,  "yUxS"            },
                { L_,   "yU",     1,  "xU",      0,  1,  "yUxU"            },
                { L_,   "yU",     1,  "xV",      0,  1,  "yUxV"            },
                { L_,   "yU",     1,  "xW",      0,  1,  "yUxW"            },
                { L_,   "yU",     1,  "xX",      0,  1,  "yUxX"            },
                { L_,   "yU",     1,  "xY",      0,  1,  "yUxY"            },
                { L_,   "yU",     1,  "xZ",      0,  1,  "yUxZ"            },
                { L_,   "yU",     1,  "xa",      0,  1,  "yUxa"            },
                { L_,   "yU",     1,  "xb",      0,  1,  "yUxb"            },
                { L_,   "yU",     1,  "xc",      0,  1,  "yUxc"            },
                { L_,   "yU",     1,  "xd",      0,  1,  "yUxd"            },
                { L_,   "yU",     1,  "xe",      0,  1,  "yUxe"            },
                { L_,   "yU",     1,  "xf",      0,  1,  "yUxf"            },

                { L_,   "xCyF",   0,  "",        0,  0,  "xCyF"            },
                { L_,   "xCyF",   1,  "",        0,  0,  "xCyF"            },
                { L_,   "xCyF",   2,  "",        0,  0,  "xCyF"            },

                { L_,   "xC",     0,  "yF",      0,  0,  "xC"              },
                { L_,   "xC",     0,  "yF",      0,  1,  "yFxC"            },
                { L_,   "xC",     0,  "yF",      1,  0,  "xC"              },
                { L_,   "xC",     1,  "yF",      0,  0,  "xC"              },
                { L_,   "xC",     1,  "yF",      0,  1,  "xCyF"            },
                { L_,   "xC",     1,  "yF",      1,  0,  "xC"              },

                { L_,   "",       0,  "xCyF",    0,  0,  ""                },
                { L_,   "",       0,  "xCyF",    0,  1,  "xC"              },
                { L_,   "",       0,  "xCyF",    0,  2,  "xCyF"            },
                { L_,   "",       0,  "xCyF",    1,  0,  ""                },
                { L_,   "",       0,  "xCyF",    1,  1,  "yF"              },
                { L_,   "",       0,  "xCyF",    2,  0,  ""                },

                //line  d-list    di  s-list     si  ne  expected
                //----  ------    --  ------     --  --  --------   Depth = 3
                { L_,   "xCxUyB", 0,  "",        0,  0,  "xCxUyB"          },
                { L_,   "xCxUyB", 1,  "",        0,  0,  "xCxUyB"          },
                { L_,   "xCxUyB", 2,  "",        0,  0,  "xCxUyB"          },
                { L_,   "xCxUyB", 3,  "",        0,  0,  "xCxUyB"          },

                { L_,   "xCxU",   0,  "yB",      0,  0,  "xCxU"            },
                { L_,   "xCxU",   0,  "yB",      0,  1,  "yBxCxU"          },
                { L_,   "xCxU",   0,  "yB",      1,  0,  "xCxU"            },
                { L_,   "xCxU",   1,  "yB",      0,  0,  "xCxU"            },
                { L_,   "xCxU",   1,  "yB",      0,  1,  "xCyBxU"          },
                { L_,   "xCxU",   1,  "yB",      1,  0,  "xCxU"            },
                { L_,   "xCxU",   2,  "yB",      0,  0,  "xCxU"            },
                { L_,   "xCxU",   2,  "yB",      0,  1,  "xCxUyB"          },
                { L_,   "xCxU",   2,  "yB",      1,  0,  "xCxU"            },

                { L_,   "xC",     0,  "xUyB",    0,  0,  "xC"              },
                { L_,   "xC",     0,  "xUyB",    0,  1,  "xUxC"            },
                { L_,   "xC",     0,  "xUyB",    0,  2,  "xUyBxC"          },
                { L_,   "xC",     0,  "xUyB",    1,  0,  "xC"              },
                { L_,   "xC",     0,  "xUyB",    1,  1,  "yBxC"            },
                { L_,   "xC",     0,  "xUyB",    2,  0,  "xC"              },
                { L_,   "xC",     1,  "xUyB",    0,  0,  "xC"              },
                { L_,   "xC",     1,  "xUyB",    0,  1,  "xCxU"            },
                { L_,   "xC",     1,  "xUyB",    0,  2,  "xCxUyB"          },
                { L_,   "xC",     1,  "xUyB",    1,  0,  "xC"              },
                { L_,   "xC",     1,  "xUyB",    1,  1,  "xCyB"            },
                { L_,   "xC",     1,  "xUyB",    2,  0,  "xC"              },

                { L_,   "",       0,  "xCxUyB",  0,  0,  ""                },
                { L_,   "",       0,  "xCxUyB",  0,  1,  "xC"              },
                { L_,   "",       0,  "xCxUyB",  0,  2,  "xCxU"            },
                { L_,   "",       0,  "xCxUyB",  0,  3,  "xCxUyB"          },
                { L_,   "",       0,  "xCxUyB",  1,  0,  ""                },
                { L_,   "",       0,  "xCxUyB",  1,  1,  "xU"              },
                { L_,   "",       0,  "xCxUyB",  1,  2,  "xUyB"            },
                { L_,   "",       0,  "xCxUyB",  2,  0,  ""                },
                { L_,   "",       0,  "xCxUyB",  2,  1,  "yB"              },
                { L_,   "",       0,  "xCxUyB",  3,  0,  ""                },

                //line  d-list      di   s-list     si  ne expected
                //----  ------      --   ------     --  -- --------   Depth = 4
                { L_,   "xAxBxCxD", 0,   "",        0,  0, "xAxBxCxD"      },
                { L_,   "xAxBxCxD", 1,   "",        0,  0, "xAxBxCxD"      },
                { L_,   "xAxBxCxD", 2,   "",        0,  0, "xAxBxCxD"      },
                { L_,   "xAxBxCxD", 3,   "",        0,  0, "xAxBxCxD"      },
                { L_,   "xAxBxCxD", 4,   "",        0,  0, "xAxBxCxD"      },

                { L_,   "xAxBxC",   0,  "xD",       0,  0, "xAxBxC"        },
                { L_,   "xAxBxC",   0,  "xD",       0,  1, "xDxAxBxC"      },
                { L_,   "xAxBxC",   0,  "xD",       1,  0, "xAxBxC"        },
                { L_,   "xAxBxC",   1,  "xD",       0,  0, "xAxBxC"        },
                { L_,   "xAxBxC",   1,  "xD",       0,  1, "xAxDxBxC"      },
                { L_,   "xAxBxC",   1,  "xD",       1,  0, "xAxBxC"        },
                { L_,   "xAxBxC",   2,  "xD",       0,  0, "xAxBxC"        },
                { L_,   "xAxBxC",   2,  "xD",       0,  1, "xAxBxDxC"      },
                { L_,   "xAxBxC",   2,  "xD",       1,  0, "xAxBxC"        },
                { L_,   "xAxBxC",   3,  "xD",       0,  0, "xAxBxC"        },
                { L_,   "xAxBxC",   3,  "xD",       0,  1, "xAxBxCxD"      },
                { L_,   "xAxBxC",   3,  "xD",       1,  0, "xAxBxC"        },

                { L_,   "xAxB",     0,  "xCxD",     0,  0, "xAxB"          },
                { L_,   "xAxB",     0,  "xCxD",     0,  1, "xCxAxB"        },
                { L_,   "xAxB",     0,  "xCxD",     0,  2, "xCxDxAxB"      },
                { L_,   "xAxB",     0,  "xCxD",     1,  0, "xAxB"          },
                { L_,   "xAxB",     0,  "xCxD",     1,  1, "xDxAxB"        },
                { L_,   "xAxB",     0,  "xCxD",     2,  0, "xAxB"          },
                { L_,   "xAxB",     1,  "xCxD",     0,  0, "xAxB"          },
                { L_,   "xAxB",     1,  "xCxD",     0,  1, "xAxCxB"        },
                { L_,   "xAxB",     1,  "xCxD",     0,  2, "xAxCxDxB"      },
                { L_,   "xAxB",     1,  "xCxD",     1,  0, "xAxB"          },
                { L_,   "xAxB",     1,  "xCxD",     1,  1, "xAxDxB"        },
                { L_,   "xAxB",     1,  "xCxD",     2,  0, "xAxB"          },
                { L_,   "xAxB",     2,  "xCxD",     0,  0, "xAxB"          },
                { L_,   "xAxB",     2,  "xCxD",     0,  1, "xAxBxC"        },
                { L_,   "xAxB",     2,  "xCxD",     0,  2, "xAxBxCxD"      },
                { L_,   "xAxB",     2,  "xCxD",     1,  0, "xAxB"          },
                { L_,   "xAxB",     2,  "xCxD",     1,  1, "xAxBxD"        },
                { L_,   "xAxB",     2,  "xCxD",     2,  0, "xAxB"          },

                { L_,   "xA",       0,  "xBxCxD",   0,  0, "xA"            },
                { L_,   "xA",       0,  "xBxCxD",   0,  1, "xBxA"          },
                { L_,   "xA",       0,  "xBxCxD",   0,  2, "xBxCxA"        },
                { L_,   "xA",       0,  "xBxCxD",   0,  3, "xBxCxDxA"      },
                { L_,   "xA",       0,  "xBxCxD",   1,  0, "xA"            },
                { L_,   "xA",       0,  "xBxCxD",   1,  1, "xCxA"          },
                { L_,   "xA",       0,  "xBxCxD",   1,  2, "xCxDxA"        },
                { L_,   "xA",       0,  "xBxCxD",   2,  0, "xA"            },
                { L_,   "xA",       0,  "xBxCxD",   2,  1, "xDxA"          },
                { L_,   "xA",       0,  "xBxCxD",   3,  0, "xA"            },
                { L_,   "xA",       1,  "xBxCxD",   0,  0, "xA"            },
                { L_,   "xA",       1,  "xBxCxD",   0,  1, "xAxB"          },
                { L_,   "xA",       1,  "xBxCxD",   0,  2, "xAxBxC"        },
                { L_,   "xA",       1,  "xBxCxD",   0,  3, "xAxBxCxD"      },
                { L_,   "xA",       1,  "xBxCxD",   1,  0, "xA"            },
                { L_,   "xA",       1,  "xBxCxD",   1,  1, "xAxC"          },
                { L_,   "xA",       1,  "xBxCxD",   1,  2, "xAxCxD"        },
                { L_,   "xA",       1,  "xBxCxD",   2,  0, "xA"            },
                { L_,   "xA",       1,  "xBxCxD",   2,  1, "xAxD"          },
                { L_,   "xA",       1,  "xBxCxD",   3,  0, "xA"            },

                { L_,   "",         0,  "xAxBxCxD", 0,  0, ""              },
                { L_,   "",         0,  "xAxBxCxD", 0,  1, "xA"            },
                { L_,   "",         0,  "xAxBxCxD", 0,  2, "xAxB"          },
                { L_,   "",         0,  "xAxBxCxD", 0,  3, "xAxBxC"        },
                { L_,   "",         0,  "xAxBxCxD", 0,  4, "xAxBxCxD"      },
                { L_,   "",         0,  "xAxBxCxD", 1,  0, ""              },
                { L_,   "",         0,  "xAxBxCxD", 1,  1, "xB"            },
                { L_,   "",         0,  "xAxBxCxD", 1,  2, "xBxC"          },
                { L_,   "",         0,  "xAxBxCxD", 1,  3, "xBxCxD"        },
                { L_,   "",         0,  "xAxBxCxD", 2,  0, ""              },
                { L_,   "",         0,  "xAxBxCxD", 2,  1, "xC"            },
                { L_,   "",         0,  "xAxBxCxD", 2,  2, "xCxD"          },
                { L_,   "",         0,  "xAxBxCxD", 3,  0, ""              },
                { L_,   "",         0,  "xAxBxCxD", 3,  1, "xD"            },
                { L_,   "",         0,  "xAxBxCxD", 4,  0, ""              },

                //line  d-list        di s-list     si  ne expected
                //----  ------        -- ------     --  -- --------   Depth = 5
                { L_,   "xAxBxCxDxE", 0, "",        0,  0, "xAxBxCxDxE"    },
                { L_,   "xAxBxCxDxE", 1, "",        0,  0, "xAxBxCxDxE"    },
                { L_,   "xAxBxCxDxE", 2, "",        0,  0, "xAxBxCxDxE"    },
                { L_,   "xAxBxCxDxE", 3, "",        0,  0, "xAxBxCxDxE"    },
                { L_,   "xAxBxCxDxE", 4, "",        0,  0, "xAxBxCxDxE"    },
                { L_,   "xAxBxCxDxE", 5, "",        0,  0, "xAxBxCxDxE"    },

                { L_,   "xAxBxCxD", 0, "xE",        0,  0, "xAxBxCxD"      },
                { L_,   "xAxBxCxD", 0, "xE",        0,  1, "xExAxBxCxD"    },
                { L_,   "xAxBxCxD", 0, "xE",        1,  0, "xAxBxCxD"      },
                { L_,   "xAxBxCxD", 1, "xE",        0,  0, "xAxBxCxD"      },
                { L_,   "xAxBxCxD", 1, "xE",        0,  1, "xAxExBxCxD"    },
                { L_,   "xAxBxCxD", 1, "xE",        1,  0, "xAxBxCxD"      },
                { L_,   "xAxBxCxD", 2, "xE",        0,  0, "xAxBxCxD"      },
                { L_,   "xAxBxCxD", 2, "xE",        0,  1, "xAxBxExCxD"    },
                { L_,   "xAxBxCxD", 2, "xE",        1,  0, "xAxBxCxD"      },
                { L_,   "xAxBxCxD", 3, "xE",        0,  0, "xAxBxCxD"      },
                { L_,   "xAxBxCxD", 3, "xE",        0,  1, "xAxBxCxExD"    },
                { L_,   "xAxBxCxD", 3, "xE",        1,  0, "xAxBxCxD"      },
                { L_,   "xAxBxCxD", 4, "xE",        0,  0, "xAxBxCxD"      },
                { L_,   "xAxBxCxD", 4, "xE",        0,  1, "xAxBxCxDxE"    },
                { L_,   "xAxBxCxD", 4, "xE",        1,  0, "xAxBxCxD"      },

                { L_,   "xAxBxC", 0,  "xDxE",       0,  0, "xAxBxC"        },
                { L_,   "xAxBxC", 0,  "xDxE",       0,  1, "xDxAxBxC"      },
                { L_,   "xAxBxC", 0,  "xDxE",       0,  2, "xDxExAxBxC"    },
                { L_,   "xAxBxC", 0,  "xDxE",       1,  0, "xAxBxC"        },
                { L_,   "xAxBxC", 0,  "xDxE",       1,  1, "xExAxBxC"      },
                { L_,   "xAxBxC", 0,  "xDxE",       2,  0, "xAxBxC"        },
                { L_,   "xAxBxC", 1,  "xDxE",       0,  0, "xAxBxC"        },
                { L_,   "xAxBxC", 1,  "xDxE",       0,  1, "xAxDxBxC"      },
                { L_,   "xAxBxC", 1,  "xDxE",       0,  2, "xAxDxExBxC"    },
                { L_,   "xAxBxC", 1,  "xDxE",       1,  0, "xAxBxC"        },
                { L_,   "xAxBxC", 1,  "xDxE",       1,  1, "xAxExBxC"      },
                { L_,   "xAxBxC", 1,  "xDxE",       2,  0, "xAxBxC"        },
                { L_,   "xAxBxC", 2,  "xDxE",       0,  0, "xAxBxC"        },
                { L_,   "xAxBxC", 2,  "xDxE",       0,  1, "xAxBxDxC"      },
                { L_,   "xAxBxC", 2,  "xDxE",       0,  2, "xAxBxDxExC"    },
                { L_,   "xAxBxC", 2,  "xDxE",       1,  0, "xAxBxC"        },
                { L_,   "xAxBxC", 2,  "xDxE",       1,  1, "xAxBxExC"      },
                { L_,   "xAxBxC", 2,  "xDxE",       2,  0, "xAxBxC"        },
                { L_,   "xAxBxC", 3,  "xDxE",       0,  0, "xAxBxC"        },
                { L_,   "xAxBxC", 3,  "xDxE",       0,  1, "xAxBxCxD"      },
                { L_,   "xAxBxC", 3,  "xDxE",       0,  2, "xAxBxCxDxE"    },
                { L_,   "xAxBxC", 3,  "xDxE",       1,  0, "xAxBxC"        },
                { L_,   "xAxBxC", 3,  "xDxE",       1,  1, "xAxBxCxE"      },
                { L_,   "xAxBxC", 3,  "xDxE",       2,  0, "xAxBxC"        },

                { L_,   "xAxB",   0,  "xCxDxE",     0,  0, "xAxB"          },
                { L_,   "xAxB",   0,  "xCxDxE",     0,  1, "xCxAxB"        },
                { L_,   "xAxB",   0,  "xCxDxE",     0,  2, "xCxDxAxB"      },
                { L_,   "xAxB",   0,  "xCxDxE",     0,  3, "xCxDxExAxB"    },
                { L_,   "xAxB",   0,  "xCxDxE",     1,  0, "xAxB"          },
                { L_,   "xAxB",   0,  "xCxDxE",     1,  1, "xDxAxB"        },
                { L_,   "xAxB",   0,  "xCxDxE",     1,  2, "xDxExAxB"      },
                { L_,   "xAxB",   0,  "xCxDxE",     2,  0, "xAxB"          },
                { L_,   "xAxB",   0,  "xCxDxE",     2,  1, "xExAxB"        },
                { L_,   "xAxB",   0,  "xCxDxE",     3,  0, "xAxB"          },
                { L_,   "xAxB",   1,  "xCxDxE",     0,  0, "xAxB"          },
                { L_,   "xAxB",   1,  "xCxDxE",     0,  1, "xAxCxB"        },
                { L_,   "xAxB",   1,  "xCxDxE",     0,  2, "xAxCxDxB"      },
                { L_,   "xAxB",   1,  "xCxDxE",     0,  3, "xAxCxDxExB"    },
                { L_,   "xAxB",   1,  "xCxDxE",     1,  0, "xAxB"          },
                { L_,   "xAxB",   1,  "xCxDxE",     1,  1, "xAxDxB"        },
                { L_,   "xAxB",   1,  "xCxDxE",     1,  2, "xAxDxExB"      },
                { L_,   "xAxB",   1,  "xCxDxE",     2,  0, "xAxB"          },
                { L_,   "xAxB",   1,  "xCxDxE",     2,  1, "xAxExB"        },
                { L_,   "xAxB",   1,  "xCxDxE",     3,  0, "xAxB"          },
                { L_,   "xAxB",   2,  "xCxDxE",     0,  0, "xAxB"          },
                { L_,   "xAxB",   2,  "xCxDxE",     0,  1, "xAxBxC"        },
                { L_,   "xAxB",   2,  "xCxDxE",     0,  2, "xAxBxCxD"      },
                { L_,   "xAxB",   2,  "xCxDxE",     0,  3, "xAxBxCxDxE"    },
                { L_,   "xAxB",   2,  "xCxDxE",     1,  0, "xAxB"          },
                { L_,   "xAxB",   2,  "xCxDxE",     1,  1, "xAxBxD"        },
                { L_,   "xAxB",   2,  "xCxDxE",     1,  2, "xAxBxDxE"      },
                { L_,   "xAxB",   2,  "xCxDxE",     2,  0, "xAxB"          },
                { L_,   "xAxB",   2,  "xCxDxE",     2,  1, "xAxBxE"        },
                { L_,   "xAxB",   2,  "xCxDxE",     3,  0, "xAxB"          },

                { L_,   "xA",     0,  "xBxCxDxE",   0,  0, "xA"            },
                { L_,   "xA",     0,  "xBxCxDxE",   0,  1, "xBxA"          },
                { L_,   "xA",     0,  "xBxCxDxE",   0,  2, "xBxCxA"        },
                { L_,   "xA",     0,  "xBxCxDxE",   0,  3, "xBxCxDxA"      },
                { L_,   "xA",     0,  "xBxCxDxE",   1,  0, "xA"            },
                { L_,   "xA",     0,  "xBxCxDxE",   1,  1, "xCxA"          },
                { L_,   "xA",     0,  "xBxCxDxE",   1,  2, "xCxDxA"        },
                { L_,   "xA",     0,  "xBxCxDxE",   2,  0, "xA"            },
                { L_,   "xA",     0,  "xBxCxDxE",   2,  1, "xDxA"          },
                { L_,   "xA",     0,  "xBxCxDxE",   3,  0, "xA"            },
                { L_,   "xA",     1,  "xBxCxDxE",   0,  0, "xA"            },
                { L_,   "xA",     1,  "xBxCxDxE",   0,  1, "xAxB"          },
                { L_,   "xA",     1,  "xBxCxDxE",   0,  2, "xAxBxC"        },
                { L_,   "xA",     1,  "xBxCxDxE",   0,  3, "xAxBxCxD"      },
                { L_,   "xA",     1,  "xBxCxDxE",   1,  0, "xA"            },
                { L_,   "xA",     1,  "xBxCxDxE",   1,  1, "xAxC"          },
                { L_,   "xA",     1,  "xBxCxDxE",   1,  2, "xAxCxD"        },
                { L_,   "xA",     1,  "xBxCxDxE",   2,  0, "xA"            },
                { L_,   "xA",     1,  "xBxCxDxE",   2,  1, "xAxD"          },
                { L_,   "xA",     1,  "xBxCxDxE",   3,  0, "xA"            },

                { L_,   "",       0,  "xAxBxCxDxE", 0,  0, ""              },
                { L_,   "",       0,  "xAxBxCxDxE", 0,  1, "xA"            },
                { L_,   "",       0,  "xAxBxCxDxE", 0,  2, "xAxB"          },
                { L_,   "",       0,  "xAxBxCxDxE", 0,  3, "xAxBxC"        },
                { L_,   "",       0,  "xAxBxCxDxE", 0,  4, "xAxBxCxD"      },
                { L_,   "",       0,  "xAxBxCxDxE", 0,  5, "xAxBxCxDxE"    },
                { L_,   "",       0,  "xAxBxCxDxE", 1,  0, ""              },
                { L_,   "",       0,  "xAxBxCxDxE", 1,  1, "xB"            },
                { L_,   "",       0,  "xAxBxCxDxE", 1,  2, "xBxC"          },
                { L_,   "",       0,  "xAxBxCxDxE", 1,  3, "xBxCxD"        },
                { L_,   "",       0,  "xAxBxCxDxE", 1,  4, "xBxCxDxE"      },
                { L_,   "",       0,  "xAxBxCxDxE", 2,  0, ""              },
                { L_,   "",       0,  "xAxBxCxDxE", 2,  1, "xC"            },
                { L_,   "",       0,  "xAxBxCxDxE", 2,  2, "xCxD"          },
                { L_,   "",       0,  "xAxBxCxDxE", 2,  3, "xCxDxE"        },
                { L_,   "",       0,  "xAxBxCxDxE", 3,  0, ""              },
                { L_,   "",       0,  "xAxBxCxDxE", 3,  1, "xD"            },
                { L_,   "",       0,  "xAxBxCxDxE", 3,  2, "xDxE"          },
                { L_,   "",       0,  "xAxBxCxDxE", 4,  0, ""              },
                { L_,   "",       0,  "xAxBxCxDxE", 4,  1, "xE"            },
                { L_,   "",       0,  "xAxBxCxDxE", 5,  0, ""              },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const List MT;    // empty List; for removeAll() tests
            ASSERT(0 == MT.length());

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *D_SPEC = DATA[ti].d_dlSpec;
                const int   DI     = DATA[ti].d_di;
                const char *S_SPEC = DATA[ti].d_slSpec;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = ((int) strlen(D_SPEC) + strlen(S_SPEC))/2;
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                const List DD(g(D_SPEC));    // control for destination
                const List SS(g(S_SPEC));    // control for source
                const List EE(g(E_SPEC));    // control for expected value

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(D_SPEC); P_(DI); P_(S_SPEC);
                                                P_(SI); P_(NE); P(E_SPEC);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(SS);
                    cout << "\t\t"; P(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                if (veryVerbose)
                    cout << "\t\tinsertElements(di, sl, si, ne)" << endl;
                {
                  passCount = 0;

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    ++passCount;

                    List x(DD, &testAllocator);  const List &X = x;
                    {
                        List s(SS, &testAllocator);  const List &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }

                        x.insertElements(DI, s, SI, NE);  // source non-'const'

                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);       // source unchanged?
                        LOOP_ASSERT(LINE, listInvariants(X));
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                    LOOP_ASSERT(LINE, listInvariants(X));
                  } END_BSLMA_EXCEPTION_TEST

                  if (EXCEPTIONS_ON) ASSERT(passCount > 1);
                }

                if (veryVerbose)
                    cout << "\t\tinsertElements(di, sl, si.row(), ne)" << endl;
                {
                  passCount = 0;

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    ++passCount;

                    List x(DD, &testAllocator);  const List &X = x;
                    {
                        List s(SS, &testAllocator);  const List &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }

                        x.insertElements(DI, s.row(), SI, NE);
                                                          // source non-'const'

                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);       // source unchanged?
                        LOOP_ASSERT(LINE, listInvariants(X));
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                    LOOP_ASSERT(LINE, listInvariants(X));
                  } END_BSLMA_EXCEPTION_TEST

                  if (EXCEPTIONS_ON) ASSERT(passCount > 1);
                }

                if (veryVerbose) cout << "\t\tinsertElements(di, sl)" << endl;
                if ((int)strlen(S_SPEC)/2 == NE) {
                  passCount = 0;

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    ++passCount;

                    List x(DD, &testAllocator);  const List &X = x;
                    {
                        List s(SS, &testAllocator);  const List &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }

                        x.insertElements(DI, s);       // source non-'const'

                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);    // source unchanged?
                        LOOP_ASSERT(LINE, listInvariants(X));
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                    LOOP_ASSERT(LINE, listInvariants(X));
                  } END_BSLMA_EXCEPTION_TEST

                  if (EXCEPTIONS_ON) ASSERT(passCount > 1);
                }

                if (veryVerbose) cout << "\t\tinsertElements(di, sl)" << endl;
                if ((int)strlen(S_SPEC)/2 == NE) {
                  passCount = 0;

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    ++passCount;

                    List x(DD, &testAllocator);  const List &X = x;
                    {
                        List s(SS, &testAllocator);  const List &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }

                        x.insertElements(DI, s.row());    // source non-'const'

                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);    // source unchanged?
                        LOOP_ASSERT(LINE, listInvariants(X));
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                    LOOP_ASSERT(LINE, listInvariants(X));
                  } END_BSLMA_EXCEPTION_TEST

                  if (EXCEPTIONS_ON) ASSERT(passCount > 1);
                }

                if (veryVerbose) cout << "\t\tinsert(di, item)" << endl;

                if (1 == NE) {
                  if (veryVerbose)
                      cout << "\t\tinsertElement(di, sl, si)" << endl;
                  passCount = 0;

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    ++passCount;

                    List x(DD, &testAllocator);  const List &X = x;
                    {
                        List s(SS, &testAllocator);  const List &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }

                        x.insertElement(DI, s[SI]);     // source non-'const'

                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                        LOOP_ASSERT(LINE, listInvariants(X));
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                    LOOP_ASSERT(LINE, listInvariants(X));
                  } END_BSLMA_EXCEPTION_TEST

                  if (EXCEPTIONS_ON) ASSERT(passCount > 1);
                }

                if (1 == NE) {
                  passCount = 0;

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    ++passCount;

                    List x(DD, &testAllocator);  const List &X = x;
                    {
                        List s(SS, &testAllocator);  const List &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        char valChar = S_SPEC[2 * SI];
                        int i = valCharToIndex(valChar);

                        switch (S_SPEC[2 * SI + 1]) {
                          case 'A':
                            x.insertChar(DI, *G_VALUES_A[i]);
                            break;
                          case 'B':
                            x.insertShort(DI, *G_VALUES_B[i]);
                            break;
                          case 'C':
                            x.insertInt(DI, *G_VALUES_C[i]);
                            break;
                          case 'D':
                            x.insertInt64(DI, *G_VALUES_D[i]);
                            break;
                          case 'E':
                            x.insertFloat(DI, *G_VALUES_E[i]);
                            break;
                          case 'F':
                            x.insertDouble(DI, *G_VALUES_F[i]);
                            break;
                          case 'G':
                            x.insertString(DI, *G_VALUES_G[i]);
                            break;
                          case 'H':
                            x.insertDatetime(DI, *G_VALUES_H[i]);
                            break;
                          case 'I':
                            x.insertDate(DI, *G_VALUES_I[i]);
                            break;
                          case 'J':
                            x.insertTime(DI, *G_VALUES_J[i]);
                            break;
                          case 'K':
                            x.insertCharArray(DI, *G_VALUES_K[i]);
                            break;
                          case 'L':
                            x.insertShortArray(DI, *G_VALUES_L[i]);
                            break;
                          case 'M':
                            x.insertIntArray(DI, *G_VALUES_M[i]);
                            break;
                          case 'N':
                            x.insertInt64Array(DI, *G_VALUES_N[i]);
                            break;
                          case 'O':
                            x.insertFloatArray(DI, *G_VALUES_O[i]);
                            break;
                          case 'P':
                            x.insertDoubleArray(DI, *G_VALUES_P[i]);
                            break;
                          case 'Q':
                            x.insertStringArray(DI, *G_VALUES_Q[i]);
                            break;
                          case 'R':
                            x.insertDatetimeArray(DI, *G_VALUES_R[i]);
                            break;
                          case 'S':
                            x.insertDateArray(DI, *G_VALUES_S[i]);
                            break;
                          case 'T':
                            x.insertTimeArray(DI, *G_VALUES_T[i]);
                            break;
                          case 'U':
                            x.insertList(DI, *G_VALUES_U[i]);
                            break;
                          case 'V':
                            x.insertTable(DI, *G_VALUES_V[i]);
                            break;
                          case 'W':
                            x.insertBool(DI, *G_VALUES_W[i]);
                            break;
                          case 'X':
                            x.insertDatetimeTz(DI, *G_VALUES_X[i]);
                            break;
                          case 'Y':
                            x.insertDateTz(DI, *G_VALUES_Y[i]);
                            break;
                          case 'Z':
                            x.insertTimeTz(DI, *G_VALUES_Z[i]);
                            break;
                          case 'a':
                            x.insertBoolArray(DI, *G_VALUES_a[i]);
                            break;
                          case 'b':
                            x.insertDatetimeTzArray(DI, *G_VALUES_b[i]);
                            break;
                          case 'c':
                            x.insertDateTzArray(DI, *G_VALUES_c[i]);
                            break;
                          case 'd':
                            x.insertTimeTzArray(DI, *G_VALUES_d[i]);
                            break;
                          case 'e':
                            x.insertChoice(DI, *G_VALUES_e[i]);
                            break;
                          case 'f':
                            x.insertChoiceArray(DI, *G_VALUES_f[i]);
                            break;
                          default:
                            P(S_SPEC[1]); ASSERT(!"Bad type character");
                        }
                        if ('n' == valChar) {
                            x[DI].makeNull();
                        }

                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                        LOOP_ASSERT(LINE, listInvariants(X));
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                    LOOP_ASSERT(LINE, listInvariants(X));
                  } END_BSLMA_EXCEPTION_TEST

                  if (EXCEPTIONS_ON) ASSERT(passCount > 1);
                }

                if (1 == NE && S_SPEC[2 * SI] == 'u') {
                    passCount = 0;

                    BEGIN_BSLMA_EXCEPTION_TEST {
                      ++passCount;

                      List x(DD, &testAllocator);  const List &X = x;
                      {
                        List s(SS, &testAllocator);  const List &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }

                        switch (S_SPEC[2 * SI + 1]) {
                          case 'A':
                            x.insertNullChar(DI);
                            break;
                          case 'B':
                            x.insertNullShort(DI);
                            break;
                          case 'C':
                            x.insertNullInt(DI);
                            break;
                          case 'D':
                            x.insertNullInt64(DI);
                            break;
                          case 'E':
                            x.insertNullFloat(DI);
                            break;
                          case 'F':
                            x.insertNullDouble(DI);
                            break;
                          case 'G':
                            x.insertNullString(DI);
                            break;
                          case 'H':
                            x.insertNullDatetime(DI);
                            break;
                          case 'I':
                            x.insertNullDate(DI);
                            break;
                          case 'J':
                            x.insertNullTime(DI);
                            break;
                          case 'K':
                            x.insertNullCharArray(DI);
                            break;
                          case 'L':
                            x.insertNullShortArray(DI);
                            break;
                          case 'M':
                            x.insertNullIntArray(DI);
                            break;
                          case 'N':
                            x.insertNullInt64Array(DI);
                            break;
                          case 'O':
                            x.insertNullFloatArray(DI);
                            break;
                          case 'P':
                            x.insertNullDoubleArray(DI);
                            break;
                          case 'Q':
                            x.insertNullStringArray(DI);
                            break;
                          case 'R':
                            x.insertNullDatetimeArray(DI);
                            break;
                          case 'S':
                            x.insertNullDateArray(DI);
                            break;
                          case 'T':
                            x.insertNullTimeArray(DI);
                            break;
                          case 'U':
                            x.insertNullList(DI);
                            break;
                          case 'V':
                            x.insertNullTable(DI);
                            break;
                          case 'W':
                            x.insertNullBool(DI);
                            break;
                          case 'X':
                            x.insertNullDatetimeTz(DI);
                            break;
                          case 'Y':
                            x.insertNullDateTz(DI);
                            break;
                          case 'Z':
                            x.insertNullTimeTz(DI);
                            break;
                          case 'a':
                            x.insertNullBoolArray(DI);
                            break;
                          case 'b':
                            x.insertNullDatetimeTzArray(DI);
                            break;
                          case 'c':
                            x.insertNullDateTzArray(DI);
                            break;
                          case 'd':
                            x.insertNullTimeTzArray(DI);
                            break;
                          case 'e':
                            x.insertNullChoice(DI);
                            break;
                          case 'f':
                            x.insertNullChoiceArray(DI);
                            break;
                          default:
                            P(S_SPEC[1]); ASSERT(!"Bad type character");
                        }
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                        LOOP_ASSERT(LINE, listInvariants(X));
                      }
                      LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                      LOOP_ASSERT(LINE, listInvariants(X));
                    } END_BSLMA_EXCEPTION_TEST

                    if (EXCEPTIONS_ON) ASSERT(passCount > 1);
                }

                if (1 == NE) {
                    passCount = 0;

                    BEGIN_BSLMA_EXCEPTION_TEST {
                      ++passCount;

                      List x(DD, &testAllocator);  const List &X = x;
                      List mExp(EE, &testAllocator);
                      mExp[DI].makeNull();
                      {
                        List s(SS, &testAllocator);  const List &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }

                        switch (S_SPEC[2 * SI + 1]) {
                          case 'A':
                            x.insertNullChar(DI);
                            break;
                          case 'B':
                            x.insertNullShort(DI);
                            break;
                          case 'C':
                            x.insertNullInt(DI);
                            break;
                          case 'D':
                            x.insertNullInt64(DI);
                            break;
                          case 'E':
                            x.insertNullFloat(DI);
                            break;
                          case 'F':
                            x.insertNullDouble(DI);
                            break;
                          case 'G':
                            x.insertNullString(DI);
                            break;
                          case 'H':
                            x.insertNullDatetime(DI);
                            break;
                          case 'I':
                            x.insertNullDate(DI);
                            break;
                          case 'J':
                            x.insertNullTime(DI);
                            break;
                          case 'K':
                            x.insertNullCharArray(DI);
                            break;
                          case 'L':
                            x.insertNullShortArray(DI);
                            break;
                          case 'M':
                            x.insertNullIntArray(DI);
                            break;
                          case 'N':
                            x.insertNullInt64Array(DI);
                            break;
                          case 'O':
                            x.insertNullFloatArray(DI);
                            break;
                          case 'P':
                            x.insertNullDoubleArray(DI);
                            break;
                          case 'Q':
                            x.insertNullStringArray(DI);
                            break;
                          case 'R':
                            x.insertNullDatetimeArray(DI);
                            break;
                          case 'S':
                            x.insertNullDateArray(DI);
                            break;
                          case 'T':
                            x.insertNullTimeArray(DI);
                            break;
                          case 'U':
                            x.insertNullList(DI);
                            break;
                          case 'V':
                            x.insertNullTable(DI);
                            break;
                          case 'W':
                            x.insertNullBool(DI);
                            break;
                          case 'X':
                            x.insertNullDatetimeTz(DI);
                            break;
                          case 'Y':
                            x.insertNullDateTz(DI);
                            break;
                          case 'Z':
                            x.insertNullTimeTz(DI);
                            break;
                          case 'a':
                            x.insertNullBoolArray(DI);
                            break;
                          case 'b':
                            x.insertNullDatetimeTzArray(DI);
                            break;
                          case 'c':
                            x.insertNullDateTzArray(DI);
                            break;
                          case 'd':
                            x.insertNullTimeTzArray(DI);
                            break;
                          case 'e':
                            x.insertNullChoice(DI);
                            break;
                          case 'f':
                            x.insertNullChoiceArray(DI);
                            break;
                          default:
                            P(S_SPEC[1]); ASSERT(!"Bad type character");
                        }
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP3_ASSERT(LINE, mExp, X, mExp == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                        LOOP_ASSERT(LINE, listInvariants(X));
                      }
                      LOOP_ASSERT(LINE, mExp == X);  // source is out of scope
                      LOOP_ASSERT(LINE, listInvariants(X));
                    } END_BSLMA_EXCEPTION_TEST

                    if (EXCEPTIONS_ON) ASSERT(passCount > 1);
                }

                if (1 == NE) {
                    passCount = 0;

                    BEGIN_BSLMA_EXCEPTION_TEST {
                      ++passCount;

                      List x(DD, &testAllocator);  const List &X = x;
                      List mExp(EE, &testAllocator);
                      mExp[DI].makeNull();
                      {
                        List s(SS, &testAllocator);  const List &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }

                        switch (S_SPEC[2 * SI + 1]) {
                          case 'A':
                            x.insertNullChar(DI);
                            break;
                          case 'B':
                            x.insertNullShort(DI);
                            break;
                          case 'C':
                            x.insertNullInt(DI);
                            break;
                          case 'D':
                            x.insertNullInt64(DI);
                            break;
                          case 'E':
                            x.insertNullFloat(DI);
                            break;
                          case 'F':
                            x.insertNullDouble(DI);
                            break;
                          case 'G':
                            x.insertNullString(DI);
                            break;
                          case 'H':
                            x.insertNullDatetime(DI);
                            break;
                          case 'I':
                            x.insertNullDate(DI);
                            break;
                          case 'J':
                            x.insertNullTime(DI);
                            break;
                          case 'K':
                            x.insertNullCharArray(DI);
                            break;
                          case 'L':
                            x.insertNullShortArray(DI);
                            break;
                          case 'M':
                            x.insertNullIntArray(DI);
                            break;
                          case 'N':
                            x.insertNullInt64Array(DI);
                            break;
                          case 'O':
                            x.insertNullFloatArray(DI);
                            break;
                          case 'P':
                            x.insertNullDoubleArray(DI);
                            break;
                          case 'Q':
                            x.insertNullStringArray(DI);
                            break;
                          case 'R':
                            x.insertNullDatetimeArray(DI);
                            break;
                          case 'S':
                            x.insertNullDateArray(DI);
                            break;
                          case 'T':
                            x.insertNullTimeArray(DI);
                            break;
                          case 'U':
                            x.insertNullList(DI);
                            break;
                          case 'V':
                            x.insertNullTable(DI);
                            break;
                          case 'W':
                            x.insertNullBool(DI);
                            break;
                          case 'X':
                            x.insertNullDatetimeTz(DI);
                            break;
                          case 'Y':
                            x.insertNullDateTz(DI);
                            break;
                          case 'Z':
                            x.insertNullTimeTz(DI);
                            break;
                          case 'a':
                            x.insertNullBoolArray(DI);
                            break;
                          case 'b':
                            x.insertNullDatetimeTzArray(DI);
                            break;
                          case 'c':
                            x.insertNullDateTzArray(DI);
                            break;
                          case 'd':
                            x.insertNullTimeTzArray(DI);
                            break;
                          case 'e':
                            x.insertNullChoice(DI);
                            break;
                          case 'f':
                            x.insertNullChoiceArray(DI);
                            break;
                          default:
                            P(S_SPEC[1]); ASSERT(!"Bad type character");
                        }
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }

                        LOOP_ASSERT(LINE, X[DI].isNull());

                        LOOP_ASSERT(LINE, mExp == X);
                        LOOP_ASSERT(LINE, listInvariants(X));

                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                      }
                      LOOP_ASSERT(LINE, mExp == X);    // source out of scope
                      LOOP_ASSERT(LINE, listInvariants(X));
                    } END_BSLMA_EXCEPTION_TEST

                    if (EXCEPTIONS_ON) ASSERT(passCount > 1);
                }

                // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                if (veryVerbose)
                    cout << "\t\tappendElements(sl, si, ne)" << endl;
                if ((int)strlen(D_SPEC)/2 == DI) {
                  passCount = 0;

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    ++passCount;

                    List x(DD, &testAllocator);  const List &X = x;
                    {
                        List s(SS, &testAllocator);  const List &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }

                        x.appendElements(s, SI, NE);    // source non-'const'

                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                        LOOP_ASSERT(LINE, listInvariants(X));
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                    LOOP_ASSERT(LINE, listInvariants(X));
                  } END_BSLMA_EXCEPTION_TEST

                  if (EXCEPTIONS_ON) ASSERT(passCount > 1);
                }

                if (veryVerbose) cout << "\t\tappendElements(sl)" << endl;
                if ((int)strlen(D_SPEC)/2 == DI
                    && (int)strlen(S_SPEC)/2 == NE) {
                  passCount = 0;

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    ++passCount;

                    ASSERT(0 == SI);
                    List x(DD, &testAllocator);  const List &X = x;
                    {
                        List s(SS, &testAllocator);  const List &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }

                        x.appendElements(s);            // source non-'const'

                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                        LOOP_ASSERT(LINE, listInvariants(X));
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                    LOOP_ASSERT(LINE, listInvariants(X));
                  } END_BSLMA_EXCEPTION_TEST

                  if (EXCEPTIONS_ON) ASSERT(passCount > 1);
                }

                if (veryVerbose) cout << "\t\tappend(item)" << endl;
                if ((int)strlen(D_SPEC)/2 == DI && 1 == NE) {
                  if (veryVerbose) cout << "\t\tappendElement(sl[si])" << endl;
                  passCount = 0;

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    ++passCount;

                    List x(DD, &testAllocator);  const List &X = x;
                    {
                        List s(SS, &testAllocator);  const List &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }

                        x.appendElement(s[SI]);         // source non-'const'

                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                        LOOP_ASSERT(LINE, listInvariants(X));
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                    LOOP_ASSERT(LINE, listInvariants(X));
                  } END_BSLMA_EXCEPTION_TEST

                  if (EXCEPTIONS_ON) ASSERT(passCount > 1);
                }

                if ((int)strlen(D_SPEC)/2 == DI && 1 == NE) {
                  passCount = 0;

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    ++passCount;

                    List x(DD, &testAllocator);  const List &X = x;
                    {
                        List s(SS, &testAllocator);  const List &S = s;
                        if (veryVerbose) {
                            cout << "\t\t\ttype: "
                                 << charToTypeString(S_SPEC[1]) << endl;
                        }
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        char valChar = S_SPEC[2 * SI];
                        int i = valCharToIndex(valChar);

                        switch (S_SPEC[2 * SI + 1]) {
                          case 'A':
                            x.appendChar(*G_VALUES_A[i]);
                            break;
                          case 'B':
                            x.appendShort(*G_VALUES_B[i]);
                            break;
                          case 'C':
                            x.appendInt(*G_VALUES_C[i]);
                            break;
                          case 'D':
                            x.appendInt64(*G_VALUES_D[i]);
                            break;
                          case 'E':
                            x.appendFloat(*G_VALUES_E[i]);
                            break;
                          case 'F':
                            x.appendDouble(*G_VALUES_F[i]);
                            break;
                          case 'G':
                            x.appendString(*G_VALUES_G[i]);
                            break;
                          case 'H':
                            x.appendDatetime(*G_VALUES_H[i]);
                            break;
                          case 'I':
                            x.appendDate(*G_VALUES_I[i]);
                            break;
                          case 'J':
                            x.appendTime(*G_VALUES_J[i]);
                            break;
                          case 'K':
                            x.appendCharArray(*G_VALUES_K[i]);
                            break;
                          case 'L':
                            x.appendShortArray(*G_VALUES_L[i]);
                            break;
                          case 'M':
                            x.appendIntArray(*G_VALUES_M[i]);
                            break;
                          case 'N':
                            x.appendInt64Array(*G_VALUES_N[i]);
                            break;
                          case 'O':
                            x.appendFloatArray(*G_VALUES_O[i]);
                            break;
                          case 'P':
                            x.appendDoubleArray(*G_VALUES_P[i]);
                            break;
                          case 'Q':
                            x.appendStringArray(*G_VALUES_Q[i]);
                            break;
                          case 'R':
                            x.appendDatetimeArray(*G_VALUES_R[i]);
                            break;
                          case 'S':
                            x.appendDateArray(*G_VALUES_S[i]);
                            break;
                          case 'T':
                            x.appendTimeArray(*G_VALUES_T[i]);
                            break;
                          case 'U':
                            x.appendList(*G_VALUES_U[i]);
                            break;
                          case 'V':
                            x.appendTable(*G_VALUES_V[i]);
                            break;
                          case 'W':
                            x.appendBool(*G_VALUES_W[i]);
                            break;
                          case 'X':
                            x.appendDatetimeTz(*G_VALUES_X[i]);
                            break;
                          case 'Y':
                            x.appendDateTz(*G_VALUES_Y[i]);
                            break;
                          case 'Z':
                            x.appendTimeTz(*G_VALUES_Z[i]);
                            break;
                          case 'a':
                            x.appendBoolArray(*G_VALUES_a[i]);
                            break;
                          case 'b':
                            x.appendDatetimeTzArray(*G_VALUES_b[i]);
                            break;
                          case 'c':
                            x.appendDateTzArray(*G_VALUES_c[i]);
                            break;
                          case 'd':
                            x.appendTimeTzArray(*G_VALUES_d[i]);
                            break;
                          case 'e':
                            x.appendChoice(*G_VALUES_e[i]);
                            break;
                          case 'f':
                            x.appendChoiceArray(*G_VALUES_f[i]);
                            break;
                          default:
                             P(S_SPEC[1]); ASSERT(!"Bad type character");
                        }
                        if ('n' == valChar) {
                            x[X.length() - 1].makeNull();
                        }

                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                        LOOP_ASSERT(LINE, listInvariants(X));
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                    LOOP_ASSERT(LINE, listInvariants(X));
                  } END_BSLMA_EXCEPTION_TEST

                  if (EXCEPTIONS_ON) LOOP_ASSERT(passCount, passCount > 1);
                }

                if ((int)strlen(D_SPEC)/2 == DI && 1 == NE) {
                  passCount = 0;

                  BEGIN_BSLMA_EXCEPTION_TEST {
                      ++passCount;

                      List x(DD, &testAllocator);  const List &X = x;
                      bdem_List mExp(EE, &testAllocator);
                      mExp[mExp.length() - 1].makeNull();
                      {
                        List s(SS, &testAllocator);  const List &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }

                        switch (S_SPEC[2 * SI + 1]) {
                          case 'A':
                            x.appendNullChar();
                            break;
                          case 'B':
                            x.appendNullShort();
                            break;
                          case 'C':
                            x.appendNullInt();
                            break;
                          case 'D':
                            x.appendNullInt64();
                            break;
                          case 'E':
                            x.appendNullFloat();
                            break;
                          case 'F':
                            x.appendNullDouble();
                            break;
                          case 'G':
                            x.appendNullString();
                            break;
                          case 'H':
                            x.appendNullDatetime();
                            break;
                          case 'I':
                            x.appendNullDate();
                            break;
                          case 'J':
                            x.appendNullTime();
                            break;
                          case 'K':
                            x.appendNullCharArray();
                            break;
                          case 'L':
                            x.appendNullShortArray();
                            break;
                          case 'M':
                            x.appendNullIntArray();
                            break;
                          case 'N':
                            x.appendNullInt64Array();
                            break;
                          case 'O':
                            x.appendNullFloatArray();
                            break;
                          case 'P':
                            x.appendNullDoubleArray();
                            break;
                          case 'Q':
                            x.appendNullStringArray();
                            break;
                          case 'R':
                            x.appendNullDatetimeArray();
                            break;
                          case 'S':
                            x.appendNullDateArray();
                            break;
                          case 'T':
                            x.appendNullTimeArray();
                            break;
                          case 'U':
                            x.appendNullList();
                            break;
                          case 'V':
                            x.appendNullTable();
                            break;
                          case 'W':
                            x.appendNullBool();
                            break;
                          case 'X':
                            x.appendNullDatetimeTz();
                            break;
                          case 'Y':
                            x.appendNullDateTz();
                            break;
                          case 'Z':
                            x.appendNullTimeTz();
                            break;
                          case 'a':
                            x.appendNullBoolArray();
                            break;
                          case 'b':
                            x.appendNullDatetimeTzArray();
                            break;
                          case 'c':
                            x.appendNullDateTzArray();
                            break;
                          case 'd':
                            x.appendNullTimeTzArray();
                            break;
                          case 'e':
                            x.appendNullChoice();
                            break;
                          case 'f':
                            x.appendNullChoiceArray();
                            break;
                          default:
                            P(S_SPEC[1]); ASSERT(!"Bad type character");
                        }
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }

                        LOOP_ASSERT(LINE, mExp == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                        LOOP_ASSERT(LINE, listInvariants(X));
                      }
                      LOOP_ASSERT(LINE, mExp == X);
                      LOOP_ASSERT(LINE, listInvariants(X));
                  } END_BSLMA_EXCEPTION_TEST

                  if (EXCEPTIONS_ON) ASSERT(passCount > 1);
                }

                if ((int)strlen(D_SPEC)/2 == DI && 1 == NE) {
                  passCount = 0;

                  BEGIN_BSLMA_EXCEPTION_TEST {
                      ++passCount;

                      List x(DD, &testAllocator);  const List &X = x;
                      bdem_List mExp(EE, &testAllocator);
                      mExp[mExp.length() - 1].makeNull();
                      {
                        List s(SS, &testAllocator);  const List &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }

                        switch (S_SPEC[2 * SI + 1]) {
                          case 'A':
                            x.appendNullChar();
                            break;
                          case 'B':
                            x.appendNullShort();
                            break;
                          case 'C':
                            x.appendNullInt();
                            break;
                          case 'D':
                            x.appendNullInt64();
                            break;
                          case 'E':
                            x.appendNullFloat();
                            break;
                          case 'F':
                            x.appendNullDouble();
                            break;
                          case 'G':
                            x.appendNullString();
                            break;
                          case 'H':
                            x.appendNullDatetime();
                            break;
                          case 'I':
                            x.appendNullDate();
                            break;
                          case 'J':
                            x.appendNullTime();
                            break;
                          case 'K':
                            x.appendNullCharArray();
                            break;
                          case 'L':
                            x.appendNullShortArray();
                            break;
                          case 'M':
                            x.appendNullIntArray();
                            break;
                          case 'N':
                            x.appendNullInt64Array();
                            break;
                          case 'O':
                            x.appendNullFloatArray();
                            break;
                          case 'P':
                            x.appendNullDoubleArray();
                            break;
                          case 'Q':
                            x.appendNullStringArray();
                            break;
                          case 'R':
                            x.appendNullDatetimeArray();
                            break;
                          case 'S':
                            x.appendNullDateArray();
                            break;
                          case 'T':
                            x.appendNullTimeArray();
                            break;
                          case 'U':
                            x.appendNullList();
                            break;
                          case 'V':
                            x.appendNullTable();
                            break;
                          case 'W':
                            x.appendNullBool();
                            break;
                          case 'X':
                            x.appendNullDatetimeTz();
                            break;
                          case 'Y':
                            x.appendNullDateTz();
                            break;
                          case 'Z':
                            x.appendNullTimeTz();
                            break;
                          case 'a':
                            x.appendNullBoolArray();
                            break;
                          case 'b':
                            x.appendNullDatetimeTzArray();
                            break;
                          case 'c':
                            x.appendNullDateTzArray();
                            break;
                          case 'd':
                            x.appendNullTimeTzArray();
                            break;
                          case 'e':
                            x.appendNullChoice();
                            break;
                          case 'f':
                            x.appendNullChoiceArray();
                            break;
                          default:
                            P(S_SPEC[1]); ASSERT(!"Bad type character");
                        }
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }

                        LOOP_ASSERT(LINE, X[X.length() - 1].isNull());

                        LOOP_ASSERT(LINE, mExp == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                        LOOP_ASSERT(LINE, listInvariants(X));
                      }
                      LOOP_ASSERT(LINE, mExp == X);
                      LOOP_ASSERT(LINE, listInvariants(X));
                  } END_BSLMA_EXCEPTION_TEST

                  if (EXCEPTIONS_ON) ASSERT(passCount > 1);
                }

                if (veryVerbose) cout << "\t\tremoveElements(index, ne)"
                                      << endl;
                {
                  passCount = 0;

                  BEGIN_BSLMA_EXCEPTION_TEST {  // Note specs are switched.
                    ++passCount;

                    List x(EE, &testAllocator);  const List &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }

                    x.removeElements(DI, NE);

                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, DD == X);
                    LOOP_ASSERT(LINE, listInvariants(X));
                  } END_BSLMA_EXCEPTION_TEST

                  if (EXCEPTIONS_ON) ASSERT(passCount > 1);
                }

                if (veryVerbose) cout << "\t\tremoveElement(index)" << endl;
                if (1 == NE) {
                  passCount = 0;

                  BEGIN_BSLMA_EXCEPTION_TEST {  // Note specs are switched
                    ++passCount;

                    List x(EE, &testAllocator);  const List &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }

                    x.removeElement(DI);

                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, DD == X);
                    LOOP_ASSERT(LINE, listInvariants(X));
                  } END_BSLMA_EXCEPTION_TEST

                  if (EXCEPTIONS_ON) ASSERT(passCount > 1);
                }

                if (veryVerbose) cout << "\t\tremoveAll()" << endl;
                {
                  passCount = 0;

                  BEGIN_BSLMA_EXCEPTION_TEST {  // Note specs are switched.
                    ++passCount;

                    List x(EE, &testAllocator);  const List &X = x;
                    if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }

                    x.removeAll();
                    if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                    LOOP_ASSERT(LINE, MT == X);
                    LOOP_ASSERT(LINE, listInvariants(X));
                  } END_BSLMA_EXCEPTION_TEST

                  if (EXCEPTIONS_ON) ASSERT(passCount > 1);
                }
            }
        }
      }

DEFINE_TEST_CASE(15) {
        // --------------------------------------------------------------------
        // TESTING 'bdem_List' APPENDUNSETELEMENT, INSERTUNSETELEMENT METHODS
        // Concerns:
        //   For the 'append' and 'insert' methods, the following properties
        //   must hold:
        //    1. The source is left unaffected (apart from aliasing).
        //    2. The subsequent existence of the source has no effect on the
        //       result object (apart from aliasing).
        //    3. The function is alias safe.
        //    4. The function is exception neutral (w.r.t. allocation).
        //    5. The function preserves object invariants.
        //    6. The function is independent of internal representation.
        //   The elemtype array within the 'bdem_Row', 'bdem_List' and
        //   'bdem_Table' can be accessed using accessors.  Since this array
        //   can be used as a parameter to the functions being tested,
        //   overlaps are an issue.
        //
        // Plan: BUG
        //
        // Testing:
        //   void appendNullElement(bdem_ElemType::Type elemType);
        //   void appendNullElements(const my_ElemTypeArray&);
        //
        //   void insertNullElement(int di, bdem_ElemType::Type elemType);
        //   void insertNullElements(int di, const my_ElemTypeArray&);
        //
        //   void replaceType(int di, bdem_ElemType::Type elemType);
        //   void replaceTypes(int di, const my_ElemTypeArray&, si, ne);
        //
        //   void reset(const my_ElemTypeArray&);
        //
        //   void compact();
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl <<
              "Testing 'appendNullElement(s)', 'insertNullElement(s)'\n"
              "======================================================\n";
        if (verbose) cout <<
            "\nTesting x.insertNullElements(di, se) et al. (no aliasing)"
            << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_dlSpec;   // initial (destination) list
                int         d_di;       // index at which to insert into da
                const char *d_seSpec;   // source elemtypes
                const char *d_expSpec;  // expected list value
            } DATA[] = {
                //line  d-list        di   s-elems expected
                //----  ------        --   ------- --------    Depth = 0
                { L_,   "",           0,   "",      ""            },

                //line  d-list        di   s-elems expected
                //----  ------        --   ------- --------    Depth = 1
                { L_,   "xC",         0,   "",      "xC"          },
                { L_,   "xC",         1,   "",      "xC"          },

                { L_,   "",           0,   "A",     "nA"          },
                { L_,   "",           0,   "B",     "nB"          },
                { L_,   "",           0,   "C",     "nC"          },
                { L_,   "",           0,   "D",     "nD"          },
                { L_,   "",           0,   "E",     "nE"          },
                { L_,   "",           0,   "F",     "nF"          },
                { L_,   "",           0,   "G",     "nG"          },
                { L_,   "",           0,   "H",     "nH"          },
                { L_,   "",           0,   "I",     "nI"          },
                { L_,   "",           0,   "J",     "nJ"          },
                { L_,   "",           0,   "K",     "nK"          },
                { L_,   "",           0,   "L",     "nL"          },
                { L_,   "",           0,   "M",     "nM"          },
                { L_,   "",           0,   "N",     "nN"          },
                { L_,   "",           0,   "O",     "nO"          },
                { L_,   "",           0,   "P",     "nP"          },
                { L_,   "",           0,   "R",     "nR"          },
                { L_,   "",           0,   "Q",     "nQ"          },
                { L_,   "",           0,   "S",     "nS"          },
                { L_,   "",           0,   "T",     "nT"          },
                { L_,   "",           0,   "U",     "nU"          },
                { L_,   "",           0,   "V",     "nV"          },

                //line  d-list        di   s-elems expected
                //----  -------       --   ------- --------    Depth = 1
                { L_,   "yT",         0,   "A",     "nAyT"        },
                { L_,   "yT",         0,   "B",     "nByT"        },
                { L_,   "yT",         0,   "C",     "nCyT"        },
                { L_,   "yT",         0,   "D",     "nDyT"        },
                { L_,   "yT",         0,   "E",     "nEyT"        },
                { L_,   "yT",         0,   "F",     "nFyT"        },
                { L_,   "yT",         0,   "G",     "nGyT"        },
                { L_,   "yT",         0,   "H",     "nHyT"        },
                { L_,   "yT",         0,   "I",     "nIyT"        },
                { L_,   "yT",         0,   "J",     "nJyT"        },
                { L_,   "yT",         0,   "K",     "nKyT"        },
                { L_,   "yT",         0,   "L",     "nLyT"        },
                { L_,   "yT",         0,   "M",     "nMyT"        },
                { L_,   "yT",         0,   "N",     "nNyT"        },
                { L_,   "yT",         0,   "O",     "nOyT"        },
                { L_,   "yT",         0,   "P",     "nPyT"        },
                { L_,   "yT",         0,   "Q",     "nQyT"        },
                { L_,   "yT",         0,   "R",     "nRyT"        },
                { L_,   "yT",         0,   "S",     "nSyT"        },
                { L_,   "yT",         0,   "T",     "nTyT"        },
                { L_,   "yT",         0,   "U",     "nUyT"        },
                { L_,   "yT",         0,   "V",     "nVyT"        },
                { L_,   "yT",         1,   "A",     "yTnA"        },
                { L_,   "yT",         1,   "B",     "yTnB"        },
                { L_,   "yT",         1,   "C",     "yTnC"        },
                { L_,   "yT",         1,   "D",     "yTnD"        },
                { L_,   "yT",         1,   "E",     "yTnE"        },
                { L_,   "yT",         1,   "F",     "yTnF"        },
                { L_,   "yT",         1,   "G",     "yTnG"        },
                { L_,   "yT",         1,   "H",     "yTnH"        },
                { L_,   "yT",         1,   "I",     "yTnI"        },
                { L_,   "yT",         1,   "J",     "yTnJ"        },
                { L_,   "yT",         1,   "K",     "yTnK"        },
                { L_,   "yT",         1,   "L",     "yTnL"        },
                { L_,   "yT",         1,   "M",     "yTnM"        },
                { L_,   "yT",         1,   "N",     "yTnN"        },
                { L_,   "yT",         1,   "O",     "yTnO"        },
                { L_,   "yT",         1,   "P",     "yTnP"        },
                { L_,   "yT",         1,   "Q",     "yTnQ"        },
                { L_,   "yT",         1,   "R",     "yTnR"        },
                { L_,   "yT",         1,   "S",     "yTnS"        },
                { L_,   "yT",         1,   "T",     "yTnT"        },
                { L_,   "yT",         1,   "U",     "yTnU"        },
                { L_,   "yT",         1,   "V",     "yTnV"        },
                { L_,   "yU",         0,   "A",     "nAyU"        },
                { L_,   "yU",         0,   "B",     "nByU"        },
                { L_,   "yU",         0,   "C",     "nCyU"        },
                { L_,   "yU",         0,   "D",     "nDyU"        },
                { L_,   "yU",         0,   "E",     "nEyU"        },
                { L_,   "yU",         0,   "F",     "nFyU"        },
                { L_,   "yU",         0,   "G",     "nGyU"        },
                { L_,   "yU",         0,   "H",     "nHyU"        },
                { L_,   "yU",         0,   "I",     "nIyU"        },
                { L_,   "yU",         0,   "J",     "nJyU"        },
                { L_,   "yU",         0,   "K",     "nKyU"        },
                { L_,   "yU",         0,   "L",     "nLyU"        },
                { L_,   "yU",         0,   "M",     "nMyU"        },
                { L_,   "yU",         0,   "N",     "nNyU"        },
                { L_,   "yU",         0,   "O",     "nOyU"        },
                { L_,   "yU",         0,   "P",     "nPyU"        },
                { L_,   "yU",         0,   "Q",     "nQyU"        },
                { L_,   "yU",         0,   "R",     "nRyU"        },
                { L_,   "yU",         0,   "S",     "nSyU"        },
                { L_,   "yU",         0,   "U",     "nUyU"        },
                { L_,   "yU",         0,   "U",     "nUyU"        },
                { L_,   "yU",         0,   "V",     "nVyU"        },
                { L_,   "yU",         1,   "A",     "yUnA"        },
                { L_,   "yU",         1,   "B",     "yUnB"        },
                { L_,   "yU",         1,   "C",     "yUnC"        },
                { L_,   "yU",         1,   "D",     "yUnD"        },
                { L_,   "yU",         1,   "E",     "yUnE"        },
                { L_,   "yU",         1,   "F",     "yUnF"        },
                { L_,   "yU",         1,   "G",     "yUnG"        },
                { L_,   "yU",         1,   "H",     "yUnH"        },
                { L_,   "yU",         1,   "I",     "yUnI"        },
                { L_,   "yU",         1,   "J",     "yUnJ"        },
                { L_,   "yU",         1,   "K",     "yUnK"        },
                { L_,   "yU",         1,   "L",     "yUnL"        },
                { L_,   "yU",         1,   "M",     "yUnM"        },
                { L_,   "yU",         1,   "N",     "yUnN"        },
                { L_,   "yU",         1,   "O",     "yUnO"        },
                { L_,   "yU",         1,   "P",     "yUnP"        },
                { L_,   "yU",         1,   "Q",     "yUnQ"        },
                { L_,   "yU",         1,   "R",     "yUnR"        },
                { L_,   "yU",         1,   "S",     "yUnS"        },
                { L_,   "yU",         1,   "U",     "yUnU"        },
                { L_,   "yU",         1,   "U",     "yUnU"        },
                { L_,   "yU",         1,   "V",     "yUnV"        },
                { L_,   "xCyF",       0,   "",      "xCyF"        },

                { L_,   "",           0,   "CF",    "nCnF"        },
                { L_,   "",           0,   "CU",    "nCnU"        },

                //line  d-list        di   s-elems expected
                //----  ------        --   ------- --------    Depth = 1
                { L_,   "xCxUyB",     0,   "",      "xCxUyB"      },
                { L_,   "xCxUyB",     1,   "",      "xCxUyB"      },
                { L_,   "xCxUyB",     2,   "",      "xCxUyB"      },
                { L_,   "xCxUyB",     3,   "",      "xCxUyB"      },

                { L_,   "xCxU",       0,   "B",     "nBxCxU"      },
                { L_,   "xCxU",       1,   "B",     "xCnBxU"      },
                { L_,   "xCxU",       2,   "B",     "xCxUnB"      },

                { L_,   "xC",         0,   "UB",    "nUnBxC"      },
                { L_,   "xC",         1,   "UB",    "xCnUnB"      },

                { L_,   "",           0,   "CUB",   "nCnUnB"      },

                //line  d-list        di   s-elems  expected
                //----  ------        --   -------  --------   Depth = 4
                { L_,   "xAxBxCxD",   0,   "",      "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   1,   "",      "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   2,   "",      "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   3,   "",      "xAxBxCxD"    },
                { L_,   "xAxBxCxD",   4,   "",      "xAxBxCxD"    },
                { L_,   "xVxUxTxS",   0,   "",      "xVxUxTxS"    },
                { L_,   "xVxUxTxS",   1,   "",      "xVxUxTxS"    },
                { L_,   "xVxUxTxS",   2,   "",      "xVxUxTxS"    },
                { L_,   "xVxUxTxS",   3,   "",      "xVxUxTxS"    },
                { L_,   "xVxUxTxS",   4,   "",      "xVxUxTxS"    },

                { L_,   "xAxBxC",     0,   "D",     "nDxAxBxC"    },
                { L_,   "xAxBxC",     1,   "D",     "xAnDxBxC"    },
                { L_,   "xAxBxC",     2,   "D",     "xAxBnDxC"    },
                { L_,   "xAxBxC",     3,   "D",     "xAxBxCnD"    },
                { L_,   "xVxUxT",     0,   "S",     "nSxVxUxT"    },
                { L_,   "xVxUxT",     1,   "S",     "xVnSxUxT"    },
                { L_,   "xVxUxT",     2,   "S",     "xVxUnSxT"    },
                { L_,   "xVxUxT",     3,   "S",     "xVxUxTnS"    },

                { L_,   "xAxB",       0,   "CD",    "nCnDxAxB"    },
                { L_,   "xAxB",       1,   "CD",    "xAnCnDxB"    },
                { L_,   "xAxB",       2,   "CD",    "xAxBnCnD"    },
                { L_,   "xVxU",       0,   "TS",    "nTnSxVxU"    },
                { L_,   "xVxU",       1,   "TS",    "xVnTnSxU"    },
                { L_,   "xVxU",       2,   "TS",    "xVxUnTnS"    },

                { L_,   "xA",         0,   "BCD",   "nBnCnDxA"    },
                { L_,   "xA",         1,   "BCD",   "xAnBnCnD"    },
                { L_,   "xV",         0,   "UTS",   "nUnTnSxV"    },
                { L_,   "xV",         1,   "UTS",   "xVnUnTnS"    },

                { L_,   "",           0,   "ABCD",  "nAnBnCnD"    },
                { L_,   "",           0,   "VUTS",  "nVnUnTnS"    },

                //line  d-list        di   s-elems  expected
                //----  -------       --   -------  --------   Depth = 5
                { L_,   "xAxBxCxDxE", 0,   "",      "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 1,   "",      "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 2,   "",      "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 3,   "",      "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 4,   "",      "xAxBxCxDxE"  },
                { L_,   "xAxBxCxDxE", 5,   "",      "xAxBxCxDxE"  },

                { L_,   "xAxBxCxD",   0,   "E",     "nExAxBxCxD"  },
                { L_,   "xAxBxCxD",   1,   "E",     "xAnExBxCxD"  },
                { L_,   "xAxBxCxD",   2,   "E",     "xAxBnExCxD"  },
                { L_,   "xAxBxCxD",   3,   "E",     "xAxBxCnExD"  },
                { L_,   "xAxBxCxD",   4,   "E",     "xAxBxCxDnE"  },

                { L_,   "xAxBxC",     0,   "DE",    "nDnExAxBxC"  },
                { L_,   "xAxBxC",     1,   "DE",    "xAnDnExBxC"  },
                { L_,   "xAxBxC",     2,   "DE",    "xAxBnDnExC"  },
                { L_,   "xAxBxC",     3,   "DE",    "xAxBxCnDnE"  },

                { L_,   "xAxB",       0,   "CDE",   "nCnDnExAxB"  },
                { L_,   "xAxB",       1,   "CDE",   "xAnCnDnExB"  },
                { L_,   "xAxB",       2,   "CDE",   "xAxBnCnDnE"  },

                { L_,   "",           0,   "ABCDE", "nAnBnCnDnE"  },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *D_SPEC = DATA[ti].d_dlSpec;
                const int   DI     = DATA[ti].d_di;
                const char *S_SPEC = DATA[ti].d_seSpec;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   NE     = strlen(S_SPEC);
                const int   DEPTH  = ((int) strlen(D_SPEC))/2 + strlen(S_SPEC);
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                List DD(g(D_SPEC));             // control for destination
                my_ElemTypeArray SS;          // control for source elemtypes
                appendToElemTypeArray(&SS, S_SPEC);
                List EE(g(E_SPEC));             // control for expected value

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(D_SPEC); P_(DI); P_(S_SPEC);
                    P(E_SPEC); P(LINE)
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(SS);
                    cout << "\t\t"; P(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                if (veryVerbose)
                    cout << "\t\tinsertNullElements(di, se)" << endl;
                {
                    bslma_TestAllocator ta;
                    bslma_TestAllocator& testAllocator = ta;

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    List x(DD, &testAllocator);  const List &X = x;
                    {
                        my_ElemTypeArray s(SS, &testAllocator);
                        const my_ElemTypeArray &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.insertNullElements(DI, vectorData(s), s.size());
                                                          // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose)
                    cout << "\t\tinsertNullElements(di, se)" << endl;
                {
                    bslma_TestAllocator ta;
                    bslma_TestAllocator& testAllocator = ta;

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    List x(DD, &testAllocator);  const List &X = x;
                    List mExp(EE, &testAllocator); const List& Exp = mExp;
                    for (int i = 0; i < SS.size(); ++i) {
                        mExp[DI + i].makeNull();
                    }
                    {
                        my_ElemTypeArray s(SS, &testAllocator);
                        const my_ElemTypeArray &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.insertNullElements(DI, vectorData(s), s.size());
                                                          // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, Exp == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, Exp == X);  // source is out of scope
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose)
                    cout << "\t\tinsertNullElement(di, item)" << endl;
                if (1 == NE) {
                    bslma_TestAllocator ta;
                    bslma_TestAllocator& testAllocator = ta;

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    List x(DD, &testAllocator);  const List &X = x;
                    {
                        bdem_ElemType::Type elemType = SS[0];
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.insertNullElement(DI, elemType);
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                    }
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose)
                    cout << "\t\tinsertNullElement(di, item)" << endl;
                if (1 == NE) {
                    bslma_TestAllocator ta;
                    bslma_TestAllocator& testAllocator = ta;

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    List x(DD, &testAllocator);  const List &X = x;
                    List mExp(EE, &testAllocator); const List& Exp = mExp;
                    mExp[DI].makeNull();
                    {
                        bdem_ElemType::Type elemType = SS[0];
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.insertNullElement(DI, elemType);
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, Exp == X);
                    }
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose)
                    cout << "\t\tappendNullElements(se)" << endl;
                if ((int)strlen(D_SPEC)/2 == DI) {
                    bslma_TestAllocator ta;
                    bslma_TestAllocator& testAllocator = ta;

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    List x(DD, &testAllocator);  const List &X = x;
                    {
                        my_ElemTypeArray s(SS, &testAllocator);
                        const my_ElemTypeArray &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.appendNullElements(vectorData(s), s.size());
                                                          // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);    // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose)
                    cout << "\t\tappendNullElements(se)" << endl;
                if ((int)strlen(D_SPEC)/2 == DI) {
                    bslma_TestAllocator ta;
                    bslma_TestAllocator& testAllocator = ta;

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    List mExp(EE, &testAllocator); const List& Exp = mExp;
                    for (int i = 0; i < SS.size(); ++i) {
                        mExp[DI + i].makeNull();
                    }
                    List x(DD, &testAllocator);  const List &X = x;
                    {
                        my_ElemTypeArray s(SS, &testAllocator);
                        const my_ElemTypeArray &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.appendNullElements(vectorData(s), s.size());
                                                          // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, Exp == X);
                        LOOP_ASSERT(LINE, SS == S);    // source unchanged?
                    }
                    LOOP_ASSERT(LINE, Exp == X);  // source is out of scope
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose)
                    cout << "\t\tappendNullElement(item)" << endl;
                if ((int)strlen(D_SPEC)/2 == DI && 1 == NE) {
                    bslma_TestAllocator ta;
                    bslma_TestAllocator& testAllocator = ta;

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    List x(DD, &testAllocator);  const List &X = x;
                    {
                        bdem_ElemType::Type elemType = SS[0];
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.appendNullElement(elemType);
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                    }
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose)
                    cout << "\t\tappendNullElement(item)" << endl;
                if ((int)strlen(D_SPEC)/2 == DI && 1 == NE) {
                    bslma_TestAllocator ta;
                    bslma_TestAllocator& testAllocator = ta;

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    List x(DD, &testAllocator);  const List &X = x;
                    List mExp(EE, &testAllocator); const List& Exp = mExp;
                    mExp[DI].makeNull();
                    {
                        bdem_ElemType::Type elemType = SS[0];
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.appendNullElement(elemType);
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, Exp == X);
                    }
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }

        if (verbose) cout <<
            "\nTesting x.replaceTypes(et) et al. (no aliasing)" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_dlSpec;   // initial (destination) list
                int         d_di;       // index at which to insert into da
                const char *d_seSpec;   // source elemtypes
                int         d_si;       //
                int         d_ne;       // number of elements to replace
                const char *d_expSpec;  // expected list value
            } DATA[] = {
                //line  d-list    di   s-elems   si   ne   expected
                //----  ------    --   -------   --   --   --------   Depth = 0
                { L_,   "",       0,   "",       0,   0,   ""            },

                //line  d-list    di   s-elems   si   ne   expected
                //----  ------    --   -------   --   --   --------   Depth = 1
                { L_,   "xC",     0,   "",       0,   0,   "xC"          },

                //line  d-list    di   s-elems   si   ne   expected
                //----  ------    --   -------   --   --   --------   Depth = 2
                { L_,   "xC",     0,   "A",      0,   0,   "xC"          },
                { L_,   "xC",     0,   "A",      0,   1,   "nA"          },
                { L_,   "xC",     0,   "B",      0,   1,   "nB"          },
                { L_,   "xC",     0,   "C",      0,   1,   "nC"          },
                { L_,   "xC",     0,   "D",      0,   1,   "nD"          },
                { L_,   "xC",     0,   "E",      0,   1,   "nE"          },
                { L_,   "xC",     0,   "F",      0,   1,   "nF"          },
                { L_,   "xC",     0,   "G",      0,   1,   "nG"          },
                { L_,   "xC",     0,   "H",      0,   1,   "nH"          },
                { L_,   "xC",     0,   "I",      0,   1,   "nI"          },
                { L_,   "xC",     0,   "J",      0,   1,   "nJ"          },
                { L_,   "xC",     0,   "K",      0,   1,   "nK"          },
                { L_,   "xC",     0,   "L",      0,   1,   "nL"          },
                { L_,   "xC",     0,   "M",      0,   1,   "nM"          },
                { L_,   "xC",     0,   "N",      0,   1,   "nN"          },
                { L_,   "xC",     0,   "O",      0,   1,   "nO"          },
                { L_,   "xC",     0,   "P",      0,   1,   "nP"          },
                { L_,   "xC",     0,   "Q",      0,   1,   "nQ"          },
                { L_,   "xC",     0,   "R",      0,   1,   "nR"          },
                { L_,   "xC",     0,   "S",      0,   1,   "nS"          },
                { L_,   "xC",     0,   "T",      0,   1,   "nT"          },
                { L_,   "xC",     0,   "U",      0,   1,   "nU"          },
                { L_,   "xC",     0,   "V",      0,   1,   "nV"          },
                { L_,   "xT",     0,   "A",      0,   1,   "nA"          },
                { L_,   "xT",     0,   "B",      0,   1,   "nB"          },
                { L_,   "xT",     0,   "C",      0,   1,   "nC"          },
                { L_,   "xT",     0,   "D",      0,   1,   "nD"          },
                { L_,   "xT",     0,   "E",      0,   1,   "nE"          },
                { L_,   "xT",     0,   "F",      0,   1,   "nF"          },
                { L_,   "xT",     0,   "G",      0,   1,   "nG"          },
                { L_,   "xT",     0,   "H",      0,   1,   "nH"          },
                { L_,   "xT",     0,   "I",      0,   1,   "nI"          },
                { L_,   "xT",     0,   "J",      0,   1,   "nJ"          },
                { L_,   "xT",     0,   "K",      0,   1,   "nK"          },
                { L_,   "xT",     0,   "L",      0,   1,   "nL"          },
                { L_,   "xT",     0,   "M",      0,   1,   "nM"          },
                { L_,   "xT",     0,   "N",      0,   1,   "nN"          },
                { L_,   "xT",     0,   "O",      0,   1,   "nO"          },
                { L_,   "xT",     0,   "P",      0,   1,   "nP"          },
                { L_,   "xT",     0,   "Q",      0,   1,   "nQ"          },
                { L_,   "xT",     0,   "R",      0,   1,   "nR"          },
                { L_,   "xT",     0,   "S",      0,   1,   "nS"          },
                { L_,   "xT",     0,   "T",      0,   1,   "nT"          },
                { L_,   "xT",     0,   "U",      0,   1,   "nU"          },
                { L_,   "xT",     0,   "V",      0,   1,   "nV"          },

                //line  d-list    di   s-elems   si   ne   expected
                //----  ------    --   -------   --   --   --------   Depth = 3
                { L_,   "xTyB",   0,   "A",      0,   1,   "nAyB"        },
                { L_,   "xTyB",   0,   "B",      0,   1,   "nByB"        },
                { L_,   "xTyB",   0,   "C",      0,   1,   "nCyB"        },
                { L_,   "xTyB",   0,   "D",      0,   1,   "nDyB"        },
                { L_,   "xTyB",   0,   "E",      0,   1,   "nEyB"        },
                { L_,   "xTyB",   0,   "F",      0,   1,   "nFyB"        },
                { L_,   "xTyB",   0,   "G",      0,   1,   "nGyB"        },
                { L_,   "xTyB",   0,   "H",      0,   1,   "nHyB"        },
                { L_,   "xTyB",   0,   "I",      0,   1,   "nIyB"        },
                { L_,   "xTyB",   0,   "J",      0,   1,   "nJyB"        },
                { L_,   "xTyB",   0,   "K",      0,   1,   "nKyB"        },
                { L_,   "xTyB",   0,   "L",      0,   1,   "nLyB"        },
                { L_,   "xTyB",   0,   "M",      0,   1,   "nMyB"        },
                { L_,   "xTyB",   0,   "N",      0,   1,   "nNyB"        },
                { L_,   "xTyB",   0,   "O",      0,   1,   "nOyB"        },
                { L_,   "xTyB",   0,   "P",      0,   1,   "nPyB"        },
                { L_,   "xTyB",   0,   "Q",      0,   1,   "nQyB"        },
                { L_,   "xTyB",   0,   "R",      0,   1,   "nRyB"        },
                { L_,   "xTyB",   0,   "S",      0,   1,   "nSyB"        },
                { L_,   "xTyB",   0,   "T",      0,   1,   "nTyB"        },
                { L_,   "xTyB",   0,   "U",      0,   1,   "nUyB"        },
                { L_,   "xTyB",   0,   "V",      0,   1,   "nVyB"        },
                { L_,   "xTyB",   1,   "A",      0,   1,   "xTnA"        },
                { L_,   "xTyB",   1,   "B",      0,   1,   "xTnB"        },
                { L_,   "xTyB",   1,   "C",      0,   1,   "xTnC"        },
                { L_,   "xTyB",   1,   "D",      0,   1,   "xTnD"        },
                { L_,   "xTyB",   1,   "E",      0,   1,   "xTnE"        },
                { L_,   "xTyB",   1,   "F",      0,   1,   "xTnF"        },
                { L_,   "xTyB",   1,   "G",      0,   1,   "xTnG"        },
                { L_,   "xTyB",   1,   "H",      0,   1,   "xTnH"        },
                { L_,   "xTyB",   1,   "I",      0,   1,   "xTnI"        },
                { L_,   "xTyB",   1,   "J",      0,   1,   "xTnJ"        },
                { L_,   "xTyB",   1,   "K",      0,   1,   "xTnK"        },
                { L_,   "xTyB",   1,   "L",      0,   1,   "xTnL"        },
                { L_,   "xTyB",   1,   "M",      0,   1,   "xTnM"        },
                { L_,   "xTyB",   1,   "N",      0,   1,   "xTnN"        },
                { L_,   "xTyB",   1,   "O",      0,   1,   "xTnO"        },
                { L_,   "xTyB",   1,   "P",      0,   1,   "xTnP"        },
                { L_,   "xTyB",   1,   "Q",      0,   1,   "xTnQ"        },
                { L_,   "xTyB",   1,   "R",      0,   1,   "xTnR"        },
                { L_,   "xTyB",   1,   "S",      0,   1,   "xTnS"        },
                { L_,   "xTyB",   1,   "T",      0,   1,   "xTnT"        },
                { L_,   "xTyB",   1,   "U",      0,   1,   "xTnU"        },
                { L_,   "xTyB",   1,   "V",      0,   1,   "xTnV"        },

                { L_,   "xTyC",   0,   "AB",     0,   0,   "xTyC"        },
                { L_,   "xTyC",   1,   "AB",     0,   0,   "xTyC"        },
                { L_,   "xTyC",   0,   "AB",     0,   1,   "nAyC"        },
                { L_,   "xTyC",   1,   "AB",     0,   1,   "xTnA"        },
                { L_,   "xTyC",   0,   "AB",     1,   1,   "nByC"        },
                { L_,   "xTyC",   1,   "AB",     1,   1,   "xTnB"        },
                { L_,   "xTyC",   0,   "AB",     0,   2,   "nAnB"        },
                { L_,   "xTyC",   0,   "TC",     0,   2,   "nTnC"        },

                { L_,   "xTyCxF", 0,   "TC",     0,   1,   "nTyCxF"      },
                { L_,   "xTyCxF", 1,   "TC",     0,   1,   "xTnTxF"      },
                { L_,   "xTyCxF", 2,   "TC",     0,   1,   "xTyCnT"      },
                { L_,   "xTyCxF", 0,   "TC",     1,   1,   "nCyCxF"      },
                { L_,   "xTyCxF", 1,   "TC",     1,   1,   "xTnCxF"      },
                { L_,   "xTyCxF", 2,   "TC",     1,   1,   "xTyCnC"      },
                { L_,   "xTyCxF", 0,   "TC",     0,   2,   "nTnCxF"      },
                { L_,   "xTyCxF", 1,   "TC",     0,   2,   "xTnTnC"      },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *D_SPEC = DATA[ti].d_dlSpec;
                const int   DI     = DATA[ti].d_di;
                const char *S_SPEC = DATA[ti].d_seSpec;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = ((int) strlen(D_SPEC))/2 + strlen(S_SPEC);
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                List DD(g(D_SPEC));             // control for destination
                my_ElemTypeArray SS;          // control for source elemtypes
                appendToElemTypeArray(&SS, S_SPEC);
                List EE(g(E_SPEC));             // control for expected value

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(D_SPEC); P_(DI); P_(S_SPEC);
                                                P_(SI); P_(NE); P(E_SPEC);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(SS);
                    cout << "\t\t"; P(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                if (veryVerbose)
                    cout << "\t\treplaceTypes(di, se, si, ne)" << endl;
                {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    List x(DD, &testAllocator);  const List &X = x;
                    {
                        my_ElemTypeArray s(SS, &testAllocator);
                        const my_ElemTypeArray &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        resetListElements(x, DI, s, SI, NE);
                                                          // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP3_ASSERT(LINE, EE, X, EE == X);
                        LOOP_ASSERT(LINE, SS == S);      // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BSLMA_EXCEPTION_TEST
                }

                if (veryVerbose) cout << "\t\treplaceType(di, item)" << endl;
                if (1 == NE) {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    List x(DD, &testAllocator);  const List &X = x;
                    {
                        bdem_ElemType::Type elemType = SS[SI];
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.resetElement(DI, elemType);
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP3_ASSERT(LINE, EE, X, EE == X);
                    }
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }

        if (verbose) cout << "\nTesting x.reset(et) (no aliasing)"
                          << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_dlSpec;   // initial (destination) list
                const char *d_seSpec;   // source elemtypes
                const char *d_expSpec;  // expected list value
            } DATA[] = {
                //line  d-list      s-elems    expected
                //----  ------      -------    --------   Depth = 0
                { L_,   "",         "",        ""           },

                //line  d-list      s-elems    expected
                //----  ------      -------    --------   Depth = 1
                { L_,   "xC",       "",        ""           },
                { L_,   "",         "C",       "nC"         },
                { L_,   "",         "T",       "nT"         },

                //line  d-list      s-elems    expected
                //----  ------      -------    --------   Depth = 2
                { L_,   "xT",       "A",       "nA"         },
                { L_,   "xT",       "B",       "nB"         },
                { L_,   "xT",       "C",       "nC"         },
                { L_,   "xT",       "D",       "nD"         },
                { L_,   "xT",       "E",       "nE"         },
                { L_,   "xT",       "F",       "nF"         },
                { L_,   "xT",       "G",       "nG"         },
                { L_,   "xT",       "H",       "nH"         },
                { L_,   "xT",       "I",       "nI"         },
                { L_,   "xT",       "J",       "nJ"         },
                { L_,   "xT",       "K",       "nK"         },
                { L_,   "xT",       "L",       "nL"         },
                { L_,   "xT",       "M",       "nM"         },
                { L_,   "xT",       "N",       "nN"         },
                { L_,   "xT",       "O",       "nO"         },
                { L_,   "xT",       "P",       "nP"         },
                { L_,   "xT",       "Q",       "nQ"         },
                { L_,   "xT",       "R",       "nR"         },
                { L_,   "xT",       "S",       "nS"         },
                { L_,   "xT",       "T",       "nT"         },
                { L_,   "xT",       "U",       "nU"         },
                { L_,   "xT",       "V",       "nV"         },

                //line  d-list      s-elems    expected
                //----  ------      -------    --------   Depth = 3
                { L_,   "xTyB",     "A",       "nA"         },
                { L_,   "xTyB",     "B",       "nB"         },
                { L_,   "xTyB",     "C",       "nC"         },
                { L_,   "xTyB",     "D",       "nD"         },
                { L_,   "xTyB",     "E",       "nE"         },
                { L_,   "xTyB",     "F",       "nF"         },
                { L_,   "xTyB",     "G",       "nG"         },
                { L_,   "xTyB",     "H",       "nH"         },
                { L_,   "xTyB",     "I",       "nI"         },
                { L_,   "xTyB",     "J",       "nJ"         },
                { L_,   "xTyB",     "K",       "nK"         },
                { L_,   "xTyB",     "L",       "nL"         },
                { L_,   "xTyB",     "M",       "nM"         },
                { L_,   "xTyB",     "N",       "nN"         },
                { L_,   "xTyB",     "O",       "nO"         },
                { L_,   "xTyB",     "P",       "nP"         },
                { L_,   "xTyB",     "Q",       "nQ"         },
                { L_,   "xTyB",     "R",       "nR"         },
                { L_,   "xTyB",     "S",       "nS"         },
                { L_,   "xTyB",     "T",       "nT"         },
                { L_,   "xTyB",     "U",       "nU"         },
                { L_,   "xTyB",     "V",       "nV"         },
                { L_,   "yB",       "AT",      "nAnT"       },
                { L_,   "yB",       "BT",      "nBnT"       },
                { L_,   "yB",       "CT",      "nCnT"       },
                { L_,   "yB",       "DT",      "nDnT"       },
                { L_,   "yB",       "ET",      "nEnT"       },
                { L_,   "yB",       "FT",      "nFnT"       },
                { L_,   "yB",       "GT",      "nGnT"       },
                { L_,   "yB",       "HT",      "nHnT"       },
                { L_,   "yB",       "IT",      "nInT"       },
                { L_,   "yB",       "JT",      "nJnT"       },
                { L_,   "yB",       "KT",      "nKnT"       },
                { L_,   "yB",       "LT",      "nLnT"       },
                { L_,   "yB",       "MT",      "nMnT"       },
                { L_,   "yB",       "NT",      "nNnT"       },
                { L_,   "yB",       "OT",      "nOnT"       },
                { L_,   "yB",       "PT",      "nPnT"       },
                { L_,   "yB",       "QT",      "nQnT"       },
                { L_,   "yB",       "RT",      "nRnT"       },
                { L_,   "yB",       "ST",      "nSnT"       },
                { L_,   "yB",       "TT",      "nTnT"       },
                { L_,   "yB",       "UT",      "nUnT"       },
                { L_,   "yB",       "VT",      "nVnT"       },

                //line  d-list      s-elems    expected
                //----  ------      -------    --------   Depth = 4
                { L_,   "xTyC",     "AB",      "nAnB"       },
                { L_,   "xT",       "ABC",     "nAnBnC"     },
                { L_,   "xTyCxI",   "B",       "nB"         },
                { L_,   "",         "ABCD",    "nAnBnCnD"   },
                { L_,   "xAxBxCxD", "",        ""           },

                //line  d-list      s-elems    expected
                //----  -------     -------    --------   Depth = 5
                { L_,   "xTyCxF",   "TC",      "nTnC"       },
                { L_,   "yCxF",     "TCG",     "nTnCnG"     },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *D_SPEC = DATA[ti].d_dlSpec;
                const char *S_SPEC = DATA[ti].d_seSpec;
                const char *E_SPEC = DATA[ti].d_expSpec;

                const int   DEPTH  = ((int) strlen(D_SPEC))/2 + strlen(S_SPEC);
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                List DD(g(D_SPEC));             // control for destination
                my_ElemTypeArray SS;          // control for source elemtypes
                appendToElemTypeArray(&SS, S_SPEC);
                List EE(g(E_SPEC));             // control for expected value

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(LINE); P_(D_SPEC); P_(S_SPEC);
                                                P(E_SPEC);
                    cout << "\t\t"; P(DD);
                    cout << "\t\t"; P(SS);
                    cout << "\t\t"; P(EE);

                    cout << "\t\t\t---------- BLACK BOX ----------" << endl;
                }

                if (veryVerbose) cout << "\t\treset(se)" << endl;
                {
                  BEGIN_BSLMA_EXCEPTION_TEST {
                    List x(DD, &testAllocator);  const List &X = x;
                    {
                        my_ElemTypeArray s(SS, &testAllocator);
                        if (veryVerbose) { P(X) P(s) }
                        const my_ElemTypeArray &S = s;
                        if (veryVerbose) { cout << "\t\t\tBEFORE: "; P(X); }
                        x.reset(vectorData(s), s.size()); // source non-'const'
                        if (veryVerbose) { cout << "\t\t\t AFTER: "; P(X); }
                        LOOP_ASSERT(LINE, EE == X);
                        LOOP_ASSERT(LINE, SS == S);     // source unchanged?
                    }
                    LOOP_ASSERT(LINE, EE == X);  // source is out of scope
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }

        int baseMaxOffset;
        {
            List R;
            R.appendNullChar();
            baseMaxOffset = getMaxOffset(R.row());
        }

        // Verify the operation of getMaxOffset helper function.
        // Test only with similar types so alignment is not an issue.
        if (verbose) cout <<
            "\nTesting getMaxOffset() helper function" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec;     // spec
                int         d_offset;   // the expected offset
            } DATA[] = {
                { L_, "xA",           1,                 },
                { L_, "xAxA",         2,                 },
                { L_, "xAxAxA",       3,                 },
                { L_, "xB",           sizeof(short)      },
                { L_, "xByB",         2 * sizeof(short)  },
                { L_, "yBxBuB",       3 * sizeof(short)  },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE = DATA[ti].d_lineNum;
                const char *SPEC = DATA[ti].d_spec;
                int       OFFSET = DATA[ti].d_offset;

                List T(g(SPEC));

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(SPEC);  P(OFFSET); P(T);
                }

                int offset = getMaxOffset(T.row()) - baseMaxOffset + 1;
                if (veryVerbose) P(offset);
                // TBD:
//                 LOOP_ASSERT(LINE, offset == OFFSET);

                reverseList(&T);

                int reverseOffset =
                                  getMaxOffset(T.row()) - baseMaxOffset + 1;
                if (veryVerbose) P(reverseOffset);
                // TBD:
//                 LOOP_ASSERT(LINE, reverseOffset == OFFSET);
            }
        }

        if (verbose) cout << "\nTesting x.compact()" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec;     // source list
                const char *d_espec;    // expected result
            } DATA[] = {
                { L_, "",             "",             },
                { L_, "xA",           "uA",           },
                { L_, "xTyU",         "uTuU",         },
                { L_, "xDxExF",       "uFuEuD",       },
                { L_, "xBxTxUxE",     "uUuTuEuB",     },
                { L_, "xAxTxAxT",     "uTuTuAuA",     },
                { L_, "xTxTxAxA",     "uTuTuAuA",     },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const char *SPEC   = DATA[ti].d_spec;
                const char *E_SPEC = DATA[ti].d_espec;
                const int   DEPTH  = (int) strlen(SPEC);
                if (DEPTH > oldDepth) {
                    oldDepth = DEPTH;
                    if (verbose) { cout << '\t';  P(DEPTH); }
                }

                if (veryVerbose) cout << "\t\tcompact()" << endl;
                const int NUM_PERMUTATIONS = 16;

                bslma_TestAllocator ga;

                bslma_TestAllocator da;
                bslma_DefaultAllocatorGuard dag(&da);

                List EE(g(SPEC), &ga);         // control for expected value
                List RR(g(E_SPEC), &ga);       // control for reference value
                List CC(EE, &ga);

                int bestMaxOffset = getMaxOffset(CC.row());
                int refMaxOffset = getMaxOffset(RR.row());

                if (veryVerbose) {
                    cout << "\t  =================================="
                                "==================================" << endl;
                    cout << "\t  "; P_(SPEC);  P(EE);
                }

                for (int perm = 0; perm < NUM_PERMUTATIONS; ++perm) {
                    bslma_TestAllocator ta;
                    bslma_TestAllocator& testAllocator = ta;

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    List mU(&testAllocator);
                    makePermutation(&mU, SPEC, perm%2, perm/2);
                    const List& U = mU;
                    int initialMaxOffset = getMaxOffset(U.row());

                    mU.compact();
                    int newMaxOffset = getMaxOffset(U.row());
                    if (veryVerbose) {
                        P_(perm); P_(newMaxOffset); P_(initialMaxOffset);
                        P(refMaxOffset);
                    }
                    // TBD:
//                     LOOP_ASSERT(SPEC, bestMaxOffset == newMaxOffset);
//                     LOOP_ASSERT(SPEC, refMaxOffset == newMaxOffset);
//                     LOOP_ASSERT(SPEC, initialMaxOffset >= newMaxOffset);
                    LOOP_ASSERT(SPEC, U == EE);
                  } END_BSLMA_EXCEPTION_TEST
                }
            }
        }
      }

DEFINE_TEST_CASE(14) {
        // --------------------------------------------------------------------
        // TESTING 'bdem_Table' STREAMING FUNCTIONALITY
        //   1. The bdex stream functions are implemented using the respective
        //      member functions 'bdexStreamOut' and 'bdexStreamIn'.
        //   2. Streaming must be neutral to exceptions thrown as a result of
        //      either allocating memory or streaming in values.
        //   3. Ensure that streaming works under the following conditions:
        //       VALID - may contain any sequence of valid values.
        //       EMPTY - valid, but contains no data.
        //       INVALID - may or may not be empty.
        //       INCOMPLETE - the stream is truncated, but otherwise valid.
        //       CORRUPTED - the data contains explicitly inconsistent fields.
        //
        // Plan:
        //   First perform a trivial direct (breathing) test of the
        //   'outStream' and 'inStream' methods (to address concern 1).  Note
        //   that the rest of the testing will use the stream functions.
        //
        //   Next, specify a set S of unique object values with substantial
        //   and varied differences, ordered by increasing length.  For each
        //   value in S, construct an object x along with a sequence of
        //   similarly constructed duplicates x1, x2, ..., xN.  Attempt to
        //   affect every aspect of white-box state by altering each xi in
        //   a unique way.  Let the union of all such objects be the set T.
        //
        //   VALID STREAMS (and exceptions)
        //      Using all combinations of (u, v) in T X T, stream-out the value
        //      of u into a buffer and stream it back into (an independent
        //      instance of) v, and assert that u == v.
        //
        //   EMPTY AND INVALID STREAMS
        //     For each x in T, attempt to stream into (a temporary copy of) x
        //     from an empty and then invalid stream.  Verify after each try
        //     that the object is unchanged and that the stream is invalid.
        //
        //   INCOMPLETE (BUT OTHERWISE VALID) DATA
        //     Write 3 distinct objects to an output stream buffer of total
        //     length N.  For each partial stream length from 0 to N - 1,
        //     construct a truncated input stream and attempt to read into
        //     objects initialized with distinct values.  Verify values of
        //     objects that are either successfully modified or left entirely
        //     unmodified,  and that the stream became invalid immediately
        //     after the first incomplete read.  Finally ensure that each
        //     object streamed into is in some valid state by creating a copy
        //     and then assigning a known value to that copy; allow the
        //     original instance to leave scope without further modification,
        //     so that the destructor asserts internal object invariants
        //     appropriately.
        //
        //   CORRUPTED DATA
        //     We will assume that the incomplete test fail every field,
        //     including a char (multi-byte representation) hence we need
        //     only to produce values that are inconsistent with a valid
        //     value and verify that they are detected.  Use the underlying
        //     stream package to simulate an instance of a typical valid
        //     (control) stream and verify that it can be streamed in
        //     successfully.  Then for each data field in the stream (beginning
        //     with the version number), provide one or more similar tests with
        //     that data field corrupted.  After each test, verify that the
        //     object is in some valid state after streaming, and that the
        //     input stream has gone invalid.
        //
        // Testing:
        //   bdex_InStream& bdem_Table::bdexStreamIn(bdex_InStream& is);
        //   bdex_OutStream& bdem_Table::bdexStreamOut(bdex_OutStream&) const;
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl
                          << "Testing Streaming Functionality" << endl
                          << "===============================" << endl;

        const int MAX_VERSION = Table::maxSupportedBdexVersion();

        if (verbose) cout << "\nDirect initial trial of 'bdexStreamOut' and"
                             " (valid) 'bdexStreamIn' functionality." << endl;
        {
            const Table X(h("ABC;xyx;yyx;"), &testAllocator);
            if (veryVerbose) { cout << "\t   Value being streamed: "; PR(X); }

            bdex_TestOutStream out;
            X.bdexStreamOut(out, MAX_VERSION);

            const char *const OD  = out.data();
            const int         LOD = out.length();
            ASSERT(MAX_VERSION == OD[1]);

            bdex_TestInStream in(OD, LOD);  ASSERT(in);  ASSERT(!in.isEmpty());

            Table t(h("DE;xx;"), &testAllocator);

            if (veryVerbose) { cout << "\tValue being overwritten: "; PR(t); }
            ASSERT(X != t);

            t.bdexStreamIn(in, MAX_VERSION);
            ASSERT(in);  ASSERT(in.isEmpty());

            if (veryVerbose) { cout << "\t  Value after overwrite: "; PR(t); }
            ASSERT(X == t);
        }

        if (verbose) cout << "\nTesting stream functions." << endl;

        if (verbose) cout << "\tOn valid, non-empty stream data." << endl;
        {
            static const char *SPECS[] = { // rows: 0 - 5,
                "",
                "A;",
                "AB;",
                ";",
                "A;x;",
                "AB;xx;",
                "ABCDEFGHIJKLMNOPQRSTUV;xxxxxxxxxxxxxxxxxxxxxx;",
                "BCDEFGHIJKLMNOPQRSTUVA;yyyyyyyyyyyyyyyyyyyyyy;",
                ";;",
                "AB;xx;yy;",
                "ABCDEFGHIJKLMNOPQRSTUV;xxxxxxxxxxxxxxxxxxxxxx;"  // continues
                     "yyyyyyyyyyyyyyyyyyyyyy;",
                "ABCGHITUV;xxxxxxxxx;nnnnnnnnn;yyyyyyyyy;",
                "ABCGHITUV;xyxxxxxxx;nnnxnnnnn;yyyyyyyyy;",
                "ABCGHITUV;xyxxxxxxx;nnnxnnnnn;yyyyyyyyy;xxxxxxxxx;",
                0 }; // Null string required as last element.

            static const char *STARTSPECS[] = {
                "", ";",
                "ABCGHITUV;xxxxxxxxx;nnnnnnnnn;yyyyyyyyy;",
                "ABCGHITUV;xyxxxxxxx;nnnxnnnnn;yyyyyyyyy;",
                "ABCGHITUV;xyxxxxxxx;nnnxnnnnn;yyyyyyyyy;xxxxxxxxx;",
                0 };

            enum { NUM_SPECS = sizeof(SPECS) / sizeof(*SPECS) - 1 };

            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int uLen = getNumRows(U_SPEC);

                if (verbose) {
                    cout << "\t\tFor source objects of length "
                         << uLen << ":\t";
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen <= uLen);   // increasing
                uOldLen = uLen;

                const Table UU = h(U_SPEC);             // control
                LOOP_ASSERT(ui, uLen == UU.numRows());  // same lengths

                Table mU(&testAllocator);
                const Table& U = mU;          hh(&mU, U_SPEC);

                bdex_TestOutStream out;

                // testing stream-out function here
                bdex_OutStreamFunctions::streamOut(out, U, MAX_VERSION);

                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Must reset stream for each iteration of inner loop.
                bdex_TestInStream testInStream(OD, LOD);
                testInStream.setSuppressVersionCheck(1);
                LOOP_ASSERT(U_SPEC, testInStream);
                LOOP_ASSERT(U_SPEC, !testInStream.isEmpty());

                for (int svi = 0; STARTSPECS[svi]; ++svi) {
                    const char *const STARTSPEC = STARTSPECS[svi];

                    Table mVS;
                    hh(&mVS, STARTSPEC);
                    const Table& VS = mVS;

                    BEGIN_BSLMA_EXCEPTION_TEST {
                      testInStream.reset();
                      const int AL = testAllocator.allocationLimit();
                      testAllocator.setAllocationLimit(-1);
            //--------------^
            LOOP2_ASSERT(U_SPEC, STARTSPEC, testInStream);
            LOOP2_ASSERT(U_SPEC, STARTSPEC, !testInStream.isEmpty());
            //--------------v
                      Table mV(mVS, &testAllocator);
                      const Table& V = mV;

                      if (veryVerbose) {
                          cout << "\t |"; PR(U); PR(V);
                      }

                      LOOP2_ASSERT(U_SPEC, STARTSPEC, UU == U);
                      LOOP2_ASSERT(U_SPEC, STARTSPEC, VS == V);

                      testAllocator.setAllocationLimit(AL);
                      // test stream-in function here
                      bdex_InStreamFunctions::streamIn(testInStream,
                                                       mV,
                                                       MAX_VERSION);

                      LOOP2_ASSERT(UU, U, UU == U);
                      LOOP2_ASSERT(UU, V, UU == V);
                      LOOP2_ASSERT(U, V, U == V);
                   } END_BSLMA_EXCEPTION_TEST

                   BEGIN_BDEX_EXCEPTION_TEST {
                     testInStream.reset();
            //--------------^
            LOOP2_ASSERT(U_SPEC, STARTSPEC, testInStream);
            LOOP2_ASSERT(U_SPEC, STARTSPEC, !testInStream.isEmpty());
            //--------------v
                      Table mV(mVS, &testAllocator);
                      const Table& V = mV;

                      if (veryVerbose) {
                          cout << "\t |"; PR(U); PR(V);
                      }

                      LOOP2_ASSERT(U_SPEC, STARTSPEC, UU == U);
                      LOOP2_ASSERT(U_SPEC, STARTSPEC, VS == V);

                      // test stream-in operator here
                      bdex_InStreamFunctions::streamIn(testInStream,
                                                       mV,
                                                       MAX_VERSION);

                      LOOP2_ASSERT(U_SPEC, STARTSPEC, UU == U);
                      LOOP2_ASSERT(U_SPEC, STARTSPEC, UU == V);
                      LOOP2_ASSERT(U_SPEC, STARTSPEC, U == V);
                    } END_BDEX_EXCEPTION_TEST
                }

                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];
                    const int vLen = getNumRows(V_SPEC);

                    const Table VV = h(V_SPEC);               // control

                    if (veryVerbose) {
                        cout << "\t\t\tFor destination objects of length "
                             << vLen << ":\t";
                        P(V_SPEC);
                    }

                    const int Z = ui == vi; // flag indicating same values

                    Table mVR;
                    hh(&mVR, V_SPEC);

                    {
                        testInStream.reset();
            //--------------^
            LOOP2_ASSERT(U_SPEC, V_SPEC, testInStream);
            LOOP2_ASSERT(U_SPEC, V_SPEC, !testInStream.isEmpty());
            //--------------v
                        Table mV(mVR, &testAllocator);
                        const Table& V = mV;

                        if (veryVerbose) {
                            cout << "\t |"; PR(U); PR(V);
                        }

                        LOOP2_ASSERT(U_SPEC, V_SPEC, UU == U);
                        LOOP2_ASSERT(U_SPEC, V_SPEC, VV == V);
                        LOOP2_ASSERT(U_SPEC, V_SPEC, Z==(U==V));

                        // test stream-in operator here
                        bdex_InStreamFunctions::streamIn(testInStream,
                                                         mV,
                                                         MAX_VERSION);

                        LOOP2_ASSERT(UU, U, UU == U);
                        LOOP2_ASSERT(UU, V, UU == V);
                        LOOP2_ASSERT(U, V, U == V);
                    }
                }
            }

            if (verbose) cout <<
                               "Testing incomplete bdex streaming of tables\n";

            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];

                Table mU(&testAllocator);
                const Table& U = mU;          hh(&mU, U_SPEC);

                bdex_TestOutStream out;

                // testing stream-out operator here
                bdex_OutStreamFunctions::streamOut(out,
                                                   U,
                                                   MAX_VERSION);

                const char *const OD  = out.data();
                const int         LOD = out.length();

                Table mX;                const Table& X = mX;
                hh(&mX, SPECS[(ui + 4) % NUM_SPECS]);

                for (int lod = LOD-1; lod >= 0; --lod) {
                    // Must reset stream for each iteration of inner loop.
                    bdex_TestInStream testInStream(OD, lod);
                    testInStream.setSuppressVersionCheck(1);
                    LOOP_ASSERT(U_SPEC, testInStream);
                    LOOP_ASSERT(U_SPEC, !lod == testInStream.isEmpty());

                    BEGIN_BDEX_EXCEPTION_TEST {
                        testInStream.reset();

                        Table mVS(X);     const Table& VS = mVS;

                        bdex_InStreamFunctions::streamIn(testInStream,
                                                         mVS,
                                                         MAX_VERSION);

                        LOOP_ASSERT(U_SPEC, !testInStream);

                        if (0 == lod) LOOP_ASSERT(U_SPEC, X == VS);

                        // destruction of mVS will assert table invariants
                    } END_BDEX_EXCEPTION_TEST
                }
            }
        }

        if (verbose) cout
            << "\tVerify binary data is identical across platforms." << endl;
        static const struct {
            int         d_lineNum;      // source line number
            int         d_version;      // version to stream
            const char *d_spec;         // the spec
            int         d_length;       // length of binary
            char        d_binary[128];  // binary representation
        } DATA[] = {
            // version 2
            { L_, 2, "", 3,
              { 0x2, 0x0, 0x0,  },
            },
            { L_, 2, ";", 3,
              { 0x2, 0x0, 0x1,  },
            },
            { L_, 2, "AB;xx;", 12,
              { 0x2, 0x2, 0x0, 0x1, 0x1, 0xc0, 0x0, 0x0, 0x0,
                0x78, 0x0, 0xc, },
            },
            { L_, 2, "ABC;xxx;", 17,
              { 0x2, 0x3, 0x0, 0x1, 0x2, 0x1, 0xe0, 0x0, 0x0, 0x0,
                0x78, 0x0, 0xc, 0x0, 0x0, 0x0, 0x67,  },
            },
            { L_, 2, "ABCDE;xxxxx;", 31,
              { 0x2, 0x5, 0x0, 0x1, 0x2, 0x3, 0x4, 0x1, 0xf8, 0x0, 0x0, 0x0,
                0x78, 0x0, 0xc, 0x0,
                0x0, 0x0, 0x67, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x27, 0x14, 0x42,
                0xd3, 0x0, 0x0,  },
            },
            { L_, 2, "FGHIJ;xxxxx;", 47,
              { 0x2, 0x5, 0x5, 0x6, 0x7, 0x8, 0x9, 0x1, 0xf8, 0x0, 0x0, 0x0,
                0x40, 0x5a, 0x80, 0x62,
                0x4d, 0xd2, 0xf1, 0xaa, 0xc, 0x6f, 0x6e, 0x65, 0x2d, 0x73,
                0x65, 0x76, 0x65, 0x6e, 0x2d, 0x2d, 0x2d, 0x0, 0x99, 0x86, 0x1,
                0xbe, 0xe6, 0xac, 0x0, 0x9b, 0x13, 0x2, 0x2e, 0xa0, 0x3e,  },
            },
            { L_, 2, "KLMNO;xxxxx;", 58,
              { 0x2, 0x5, 0xa, 0xb, 0xc, 0xd, 0xe, 0x1, 0xf8, 0x0, 0x0, 0x0,
                0x5, 0x78, 0x4e, 0x78,
                0x4e, 0x78, 0x2, 0x0, 0xc, 0x80, 0x0, 0x2, 0x0, 0x0, 0x0, 0x67,
                0x80, 0x0, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x27,
                0x14, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x42, 0xd3,
                0x0, 0x0, 0xa7, 0xec, 0x80, 0x0,  },
            },
            { L_, 2, "PQSTUV;xxxxxx;", 107,
              { 0x2, 0x6, 0xf, 0x10, 0x12, 0x13, 0x14, 0x15, 0x1,
                0xfc, 0x0, 0x0, 0x0, 0x2, 0x40,
                0x5a, 0x80, 0x62, 0x4d, 0xd2, 0xf1, 0xaa, 0xbd, 0x1b, 0x4c,
                0x40, 0x0, 0x0, 0x0, 0x0, 0x2, 0xc, 0x6f, 0x6e, 0x65, 0x2d,
                0x73, 0x65, 0x76, 0x65, 0x6e, 0x2d, 0x2d, 0x2d, 0x0, 0x2, 0x0,
                0x9b, 0x13, 0x0, 0x0, 0x1, 0x2, 0x2, 0x2e, 0xa0, 0x3e, 0x5,
                0x26, 0x5c, 0x0, 0x2, 0x2, 0x6, 0x0, 0x0, 0x0, 0x67, 0xc, 0x6f,
                0x6e, 0x65, 0x2d, 0x73, 0x65, 0x76, 0x65, 0x6e, 0x2d, 0x2d,
                0x2d, 0x2, 0x2, 0x6, 0x2, 0x0, 0x0, 0x0, 0x67, 0xc, 0x6f, 0x6e,
                0x65, 0x2d, 0x73, 0x65, 0x76, 0x65, 0x6e, 0x2d, 0x2d, 0x2d,
                0x80, 0x0, 0x0, 0x0, 0x0,  },
            },

            // version 1
            { L_, 1, "", 3,
              { 0x1, 0x0, 0x0,  },
            },
            { L_, 1, ";", 3,
              { 0x1, 0x0, 0x1,  },
            },
            { L_, 1, "AB;xx;", 8,
              { 0x1, 0x2, 0x0, 0x1, 0x1, 0x78, 0x0, 0xc,  },
            },
            { L_, 1, "ABC;xxx;", 13,
              { 0x1, 0x3, 0x0, 0x1, 0x2, 0x1, 0x78, 0x0, 0xc, 0x0, 0x0, 0x0,
                0x67,  },
            },
            { L_, 1, "ABCDE;xxxxx;", 27,
              { 0x1, 0x5, 0x0, 0x1, 0x2, 0x3, 0x4, 0x1, 0x78, 0x0, 0xc, 0x0,
                0x0, 0x0, 0x67, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x27, 0x14, 0x42,
                0xd3, 0x0, 0x0,  },
            },
            { L_, 1, "FGHIJ;xxxxx;", 43,
              { 0x1, 0x5, 0x5, 0x6, 0x7, 0x8, 0x9, 0x1, 0x40, 0x5a, 0x80, 0x62,
                0x4d, 0xd2, 0xf1, 0xaa, 0xc, 0x6f, 0x6e, 0x65, 0x2d, 0x73,
                0x65, 0x76, 0x65, 0x6e, 0x2d, 0x2d, 0x2d, 0x0, 0x99, 0x86, 0x1,
                0xbe, 0xe6, 0xac, 0x0, 0x9b, 0x13, 0x2, 0x2e, 0xa0, 0x3e,  },
            },
            { L_, 1, "KLMNO;xxxxx;", 54,
              { 0x1, 0x5, 0xa, 0xb, 0xc, 0xd, 0xe, 0x1, 0x5, 0x78, 0x4e, 0x78,
                0x4e, 0x78, 0x2, 0x0, 0xc, 0x80, 0x0, 0x2, 0x0, 0x0, 0x0, 0x67,
                0x80, 0x0, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x27,
                0x14, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x42, 0xd3,
                0x0, 0x0, 0xa7, 0xec, 0x80, 0x0,  },
            },
            { L_, 1, "PQSTUV;xxxxxx;", 103,
              { 0x1, 0x6, 0xf, 0x10, 0x12, 0x13, 0x14, 0x15, 0x1, 0x2, 0x40,
                0x5a, 0x80, 0x62, 0x4d, 0xd2, 0xf1, 0xaa, 0xbd, 0x1b, 0x4c,
                0x40, 0x0, 0x0, 0x0, 0x0, 0x2, 0xc, 0x6f, 0x6e, 0x65, 0x2d,
                0x73, 0x65, 0x76, 0x65, 0x6e, 0x2d, 0x2d, 0x2d, 0x0, 0x2, 0x0,
                0x9b, 0x13, 0x0, 0x0, 0x1, 0x2, 0x2, 0x2e, 0xa0, 0x3e, 0x5,
                0x26, 0x5c, 0x0, 0x2, 0x2, 0x6, 0x0, 0x0, 0x0, 0x67, 0xc, 0x6f,
                0x6e, 0x65, 0x2d, 0x73, 0x65, 0x76, 0x65, 0x6e, 0x2d, 0x2d,
                0x2d, 0x2, 0x2, 0x6, 0x2, 0x0, 0x0, 0x0, 0x67, 0xc, 0x6f, 0x6e,
                0x65, 0x2d, 0x73, 0x65, 0x76, 0x65, 0x6e, 0x2d, 0x2d, 0x2d,
                0x80, 0x0, 0x0, 0x0, 0x0,  },
            },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   VERSION = DATA[ti].d_version;
            const char *SPEC    = DATA[ti].d_spec;
            const char *BINARY  = DATA[ti].d_binary;
            int         LENGTH  = DATA[ti].d_length;

            Table mX(&testAllocator); hh(&mX, SPEC); const Table& X = mX;
            bdex_ByteInStream testInStream(BINARY, LENGTH);

            Table U;
            int version;
            testInStream.getVersion(version);
            ASSERT(VERSION == version);
            bdex_InStreamFunctions::streamIn(testInStream,
                                             U,
                                             VERSION);
            LOOP_ASSERT(SPEC, X == U);

            bdex_ByteOutStream out;

            out.putVersion(VERSION);
            X.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            LOOP_ASSERT(SPEC, LOD == LENGTH);
            LOOP_ASSERT(SPEC, memcmp(OD, BINARY, LENGTH) == 0);

            // dump the hex values *only* when 'd' is program parameter 2.
            if (verbose && argv[2][0] == 'd') {
                cout << '"' << VERSION << ' ' << SPEC << "\", " << LENGTH
                     << ", \n";
                  cout << hex;
                int ij = 0;
                while (ij < LENGTH) {
                    for (int k = 0; k < 8 && ij < LENGTH ; ++k, ++ij) {
                        cout << "0x" << (0xff & (int)BINARY[ij]) << ", ";
                    }
                    cout << endl;
                }
                cout << dec << endl;

                cout << '"' << VERSION << ' ' << SPEC << "\", " << LOD
                     << ", \n";
                  cout << hex;
                ij = 0;
                while (ij < LOD) {
                    for (int k = 0; k < 8 && ij < LOD ; ++k, ++ij) {
                        cout << "0x" << (0xff & (int)OD[ij]) << ", ";
                    }
                    cout << endl;
                }
                cout << dec << endl;

                cout << endl;
            }
        }
      }

DEFINE_TEST_CASE(13) {
        // --------------------------------------------------------------------
        // TESTING 'bdem_Row' AND 'bdem_List' STREAMING FUNCTIONALITY
        //   1. The available stream functions are implemented using the
        //      respective member functions 'bdexStreamOut' and 'bdexStreamIn'.
        //   2. Streaming must be neutral to exceptions thrown as a result of
        //      either allocating memory or streaming in values.
        //   3. Ensure that streaming works under the following conditions:
        //       VALID - may contain any sequence of valid values.
        //       EMPTY - valid, but contains no data.
        //       INVALID - may or may not be empty.
        //       INCOMPLETE - the stream is truncated, but otherwise valid.
        //       CORRUPTED - the data contains explicitly inconsistent fields.
        //
        // Concerns:
        //   a) Streaming works for all 22 types with different internal
        //      representations.
        //
        // Plan:
        //   First perform a trivial direct (breathing) test of the
        //   'outStream' and 'inStream' methods (to address concern 1).  Note
        //   that the rest of the testing will use the stream functions.
        //
        //   Next, specify a set S of unique object values with substantial
        //   and varied differences, ordered by increasing length.  For each
        //   value in S, construct an object x along with a sequence of
        //   similarly constructed duplicates x1, x2, ..., xN.  Attempt to
        //   affect every aspect of white-box state by altering each xi in
        //   a unique way.  Let the union of all such objects be the set T.
        //
        //   VALID STREAMS (and exceptions)
        //      Using all combinations of (u, v) in T X subset of T, stream-out
        //      the value
        //      of u into a buffer and stream it back into (an independent
        //      instance of) v, and assert that u == v.
        //
        //   EMPTY AND INVALID STREAMS
        //     For each x in T, attempt to stream into (a temporary copy of) x
        //     from an empty and then invalid stream.  Verify after each try
        //     that the object is unchanged and that the stream is invalid.
        //
        //   INCOMPLETE (BUT OTHERWISE VALID) DATA
        //     Write 3 distinct objects to an output stream buffer of total
        //     length N.  For each partial stream length from 0 to N - 1,
        //     construct a truncated input stream and attempt to read into
        //     objects initialized with distinct values.  Verify values of
        //     objects that are either successfully modified or left entirely
        //     unmodified,  and that the stream became invalid immediately
        //     after the first incomplete read.  Finally ensure that each
        //     object streamed into is in some valid state by creating a copy
        //     and then assigning a known value to that copy; allow the
        //     original instance to leave scope without further modification,
        //     so that the destructor asserts internal object invariants
        //     appropriately.
        //
        //   TBD: Testing streaming of corrupted streams is very difficult --
        //     some corrupted data will cause too large an array to be
        //     allocated, exhausting memory and causing a segfault, other
        //     corrupted data won't be detected as such.  It would be
        //     prohibitively difficult to make the bdex streaming pass the
        //     test.
        //   CORRUPTED DATA
        //     We will assume that the incomplete test fail every field,
        //     including a char (multi-byte representation) hence we need
        //     only to produce values that are inconsistent with a valid
        //     value and verify that they are detected.  Use the underlying
        //     stream package to simulate an instance of a typical valid
        //     (control) stream and verify that it can be streamed in
        //     successfully.  Then for each data field in the stream (beginning
        //     with the version number), provide one or more similar tests with
        //     that data field corrupted.  After each test, verify that the
        //     object is in some valid state after streaming, and that the
        //     input stream has gone invalid.
        //
        // Testing:
        //   bdex_InStream& bdexStreamIn(bdex_InStream& stream); (indirectly)
        //   bdex_OutStream& bdexStreamOut(bdex_OutStream& stream) const;
        //                                                       (indirectly)
        //   bdex_InStream& bdem_List::bdexStreamIn(bdex_InStream& stream);
        //                                                       (indirectly)
        //   bdex_OutStream& bdem_List::bdexStreamOut(bdex_OutStream& stream)
        //                                                 const (indirectly)
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl
                          << "Testing Streaming Functionality" << endl
                          << "===============================" << endl;

        const int LIST_MAX_VERSION = List::maxSupportedBdexVersion();
        const int ROW_MAX_VERSION  = Row::maxSupportedBdexVersion();
        if (verbose) cout << "\nDirect initial trial of 'bdexStreamOut' and"
                             " (valid) 'bdexStreamIn' functionality." << endl;
        {
            const List X(g("xAxBxC"), &testAllocator);
            if (veryVerbose) { cout << "\t   Value being streamed: "; P(X); }

            bdex_TestOutStream out;
            X.bdexStreamOut(out, LIST_MAX_VERSION);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            bdex_TestInStream in(OD, LOD);  ASSERT(in);  ASSERT(!in.isEmpty());

            List t(g("xDxE"), &testAllocator);

            if (veryVerbose) { cout << "\tValue being overwritten: "; P(t); }
            ASSERT(X != t);

            t.bdexStreamIn(in, LIST_MAX_VERSION);
            ASSERT(in);  ASSERT(in.isEmpty());

            if (veryVerbose) { cout << "\t  Value after overwrite: "; P(t); }
            ASSERT(X == t);
        }

        if (verbose) cout << "\nTesting stream functions." << endl;

        if (verbose) cout << "\tOn valid, non-empty stream data." << endl;
        {
            static const char *SPECS[] = {
                "",
                "xA", "xB", "xC", "xD", "xE", "xF", "xG",
                "xH", "xI", "xJ", "xK", "xL", "xM", "xN",
                "xO", "xP", "xQ", "xR", "xS", "xT", "xU", "xV",
                "xAxB",
                "yAxAyB", "yAxByB", "yAxCyB", "yAxDyB", "yAxEyB", "yAxFyB",
                "yAxGyB", "yAxHyB", "yAxIyB", "yAxJyB", "yAxKyB", "yAxLyB",
                "yAxMyB", "yAxNyB", "yAxOyB", "yAxPyB", "yAxQyB", "yAxRyB",
                "yAxSyB", "yAxTyB", "yAxUyB", "yAxVyB",
                "xDxExAxB",   "xExAxBxCxD", "yVyUyTySyByA",
            0}; // Null string required as last element.

            enum { NUM_SPECS = sizeof(SPECS) / sizeof(*SPECS) - 1 };

            static const int EXTEND[] = {
                0, 1, 5,
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            if (verbose) cout <<
                          "\tStreamout list, streamin, compare for equality\n";

            int uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int uLen = (int)strlen(U_SPEC)/2;

                if (verbose) {
                    cout << "\t\tFor source objects of length "
                         << uLen << ":\t";
                    P(U_SPEC);
                }

                LOOP_ASSERT(U_SPEC, uOldLen <= uLen);   // increasing
                uOldLen = uLen;

                const List UU = g(U_SPEC);              // control
                LOOP_ASSERT(ui, uLen == UU.length());   // same lengths

                for (int uj = 0; uj < NUM_EXTEND; ++uj) {
                    const int U_N = EXTEND[uj];

                    List mU(&testAllocator);     stretchRemoveAll(&mU, U_N);
                    const List& U = mU;          gg(&mU, U_SPEC);

                    bdex_TestOutStream out;

                    // testing stream-out function here
                    bdex_OutStreamFunctions::streamOut(out,
                                                       U,
                                                       LIST_MAX_VERSION);

                    const char *const OD  = out.data();
                    const int         LOD = out.length();

                    // Must reset stream for each iteration of inner loop.
                    bdex_TestInStream testInStream(OD, LOD);
                    testInStream.setSuppressVersionCheck(1);
                    LOOP2_ASSERT(U_SPEC, U_N, testInStream);
                    LOOP2_ASSERT(U_SPEC, U_N, !testInStream.isEmpty());

                    int prevLen = -1;

                    for (int vi = 0; SPECS[vi]; ++vi) {
                        const char *const V_SPEC = SPECS[vi];
                        const int vLen = (int)strlen(V_SPEC)/2;

                        // only test with different lengths
                        if (vLen == prevLen) continue;
                        prevLen = vLen;

                        const List VV = g(V_SPEC);               // control

                        if (0 == uj && veryVerbose || veryVeryVerbose) {
                            cout << "\t\t\tFor destination objects of length "
                                 << vLen << ":\t";
                            P(V_SPEC);
                        }

                        const int Z = ui == vi; // flag indicating same values

                        BEGIN_BSLMA_EXCEPTION_TEST {
                          testInStream.reset();
                          const int AL = testAllocator.allocationLimit();
                          testAllocator.setAllocationLimit(-1);
            //--------------^
            LOOP3_ASSERT(U_SPEC, U_N, V_SPEC, testInStream);
            LOOP3_ASSERT(U_SPEC, U_N, V_SPEC, !testInStream.isEmpty());
            //--------------v
                          List mV(&testAllocator);
                          const List& V = mV;      gg(&mV, V_SPEC);

                          static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                          if (veryVeryVerbose||veryVerbose && firstFew > 0) {
                              cout << "\t |"; P_(U_N); P_(U); P(V);
                              --firstFew;
                          }

                          LOOP3_ASSERT(U_SPEC, U_N, V_SPEC, UU == U);
                          LOOP3_ASSERT(U_SPEC, U_N, V_SPEC, VV == V);
                          LOOP3_ASSERT(U_SPEC, U_N, V_SPEC, Z==(U==V));

                          testAllocator.setAllocationLimit(AL);
                          bdex_InStreamFunctions::streamIn(testInStream,
                                                           mV,
                                                           LIST_MAX_VERSION);

                          LOOP3_ASSERT(U_SPEC, U_N, V_SPEC, testInStream);
                          LOOP3_ASSERT(U_SPEC, U_N, V_SPEC, UU == U);
                          LOOP3_ASSERT(U_SPEC, U_N, V_SPEC, U == V);

                          if (!(U==V)) {
                               P(U);
                               P(V);
                          }

                        } END_BSLMA_EXCEPTION_TEST

                        BEGIN_BDEX_EXCEPTION_TEST {
                          testInStream.reset();
            //--------------^
            LOOP3_ASSERT(U_SPEC, U_N, V_SPEC, testInStream);
            LOOP3_ASSERT(U_SPEC, U_N, V_SPEC, !testInStream.isEmpty());
            //--------------v
                          List mV(&testAllocator);
                          const List& V = mV;      gg(&mV, V_SPEC);

                          static int firstFew = 2 * NUM_EXTEND * NUM_EXTEND;
                          if (veryVeryVerbose||veryVerbose && firstFew > 0) {
                              cout << "\t |"; P_(U_N); P_(U); P(V);
                              --firstFew;
                          }

                          LOOP3_ASSERT(U_SPEC, U_N, V_SPEC, UU == U);
                          LOOP3_ASSERT(U_SPEC, U_N, V_SPEC, VV == V);
                          LOOP3_ASSERT(U_SPEC, U_N, V_SPEC, Z==(U==V));

                          bdex_InStreamFunctions::streamIn(testInStream,
                                                           mV,
                                                           LIST_MAX_VERSION);

                          LOOP3_ASSERT(U_SPEC, U_N, V_SPEC, testInStream);
                          LOOP3_ASSERT(U_SPEC, U_N, V_SPEC, UU == U);
                          LOOP3_ASSERT(U_SPEC, U_N, V_SPEC, U == V);

                        } END_BDEX_EXCEPTION_TEST
                    }
                }
            }

            if (verbose) cout <<
                           "\tStreamout, streamin row, compare for equality\n";

            uOldLen = -1;
            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int uLen = (int)strlen(U_SPEC)/2;

                LOOP_ASSERT(U_SPEC, uOldLen <= uLen);   // increasing
                uOldLen = uLen;

                List mU;
                const List& U = mU;          gg(&mU, U_SPEC);

                my_ElemTypeArray elemTypes;
                getElemTypes(&elemTypes, U);

                BEGIN_BSLMA_EXCEPTION_TEST {
                    bdex_TestOutStream out;

                    // testing stream-out operator here
                    bdex_OutStreamFunctions::streamOut(out,
                                                       U.row(),
                                                       ROW_MAX_VERSION);

                    const char *const OD  = out.data();
                    const int         LOD = out.length();

                    if (verbose) {
                        cout << "\t\tFor source objects of length "
                             << uLen << ":\t";
                        P_(U_SPEC); P(LOD);
                    }

                    // Must reset stream for each iteration of inner loop.
                    bdex_TestInStream testInStream(OD, LOD);
                    testInStream.setSuppressVersionCheck(1);
                    LOOP_ASSERT(U_SPEC, testInStream);
                    if (LOD) {
                        LOOP_ASSERT(U_SPEC, !testInStream.isEmpty());
                    }

                    BEGIN_BDEX_EXCEPTION_TEST {
                        testInStream.reset();

                        List mW;    const List& W = mW;

                        // Bdex streamin for row assumes row already has right
                        // number of elements.
                        mW.appendNullElements(elemTypes.begin(),
                                                             elemTypes.size());

                        bdex_InStreamFunctions::streamIn(testInStream,
                                                         mW.row(),
                                                         ROW_MAX_VERSION);

                        LOOP3_ASSERT(ui, U, W, U == W);
                        LOOP_ASSERT(ui, testInStream);
                        LOOP_ASSERT(ui, testInStream.isEmpty());
                    } END_BDEX_EXCEPTION_TEST
                } END_BSLMA_EXCEPTION_TEST
            }

            if (verbose) cout <<
                               "\tStreaming in List from empty valid stream\n";

            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int uLen = (int)strlen(U_SPEC)/2;

                List mU;
                const List& U = mU;          gg(&mU, U_SPEC);
                List mV(U);                  const List& V = mV;

                // Must reset stream for each iteration of inner loop.
                bdex_TestInStream testInStream("", 0);
                LOOP_ASSERT(U_SPEC, testInStream);
                LOOP_ASSERT(U_SPEC, testInStream.isEmpty());

                bdex_InStreamFunctions::streamIn(testInStream,
                                                 mU,
                                                 LIST_MAX_VERSION);

                LOOP_ASSERT(U_SPEC, !testInStream);
                LOOP_ASSERT(U_SPEC, U == V);
            }

            if (verbose) cout <<
                               "\tStreaming in Row from empty valid stream\n";

            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int uLen = (int)strlen(U_SPEC)/2;

                List mU;
                const List& U = mU;          gg(&mU, U_SPEC);
                List mV(U);                  const List& V = mV;

                // Must reset stream for each iteration of inner loop.
                bdex_TestInStream testInStream("", 0);
                LOOP_ASSERT(U_SPEC, testInStream);
                LOOP_ASSERT(U_SPEC, testInStream.isEmpty());

                bdex_InStreamFunctions::streamIn(testInStream,
                                                 mU.row(),
                                                 ROW_MAX_VERSION);

                if (uLen) {
                    LOOP_ASSERT(U_SPEC, !testInStream);
                }
                LOOP3_ASSERT(U_SPEC, U, V, U == V);
            }

            if (verbose) cout <<
                     "\tStreaming in List from invalid but not empty stream\n";

            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int uLen = (int)strlen(U_SPEC)/2;

                List mU;
                const List& U = mU;          gg(&mU, U_SPEC);
                List mV(U);                  const List& V = mV;

                // Must reset stream for each iteration of inner loop.
                bdex_TestInStream testInStream("abcdefghijkl", 12);
                LOOP_ASSERT(U_SPEC, testInStream);
                LOOP_ASSERT(U_SPEC, !testInStream.isEmpty());

                testInStream.invalidate();

                LOOP_ASSERT(U_SPEC, !testInStream);

                bdex_InStreamFunctions::streamIn(testInStream,
                                                 mU,
                                                 LIST_MAX_VERSION);

                LOOP_ASSERT(U_SPEC, !testInStream);
                LOOP_ASSERT(U_SPEC, U == V);
            }

            if (verbose) cout <<
                      "\tStreaming in Row from invalid but not empty stream\n";

            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];
                const int uLen = (int)strlen(U_SPEC)/2;

                List mU;
                const List& U = mU;          gg(&mU, U_SPEC);
                List mV(U);                  const List& V = mV;

                // Must reset stream for each iteration of inner loop.
                bdex_TestInStream testInStream("abcdefghijkl", 12);
                LOOP_ASSERT(U_SPEC, testInStream);
                LOOP_ASSERT(U_SPEC, !testInStream.isEmpty());

                testInStream.invalidate();

                LOOP_ASSERT(U_SPEC, !testInStream);

                bdex_InStreamFunctions::streamIn(testInStream,
                                                 mU.row(),
                                                 ROW_MAX_VERSION);

                LOOP_ASSERT(U_SPEC, !testInStream);
                LOOP_ASSERT(U_SPEC, U == V);
            }

            {
                if (verbose) cout <<
                                 "\tTesting streaming in incomplete streams\n";

                List mV;
                const List& V = mV;          gg(&mV, "xDxE");

                List mW;
                const List& W = mW;          gg(&mW, "yVyU");

                List mX;
                const List& X = mX;          gg(&mX, "xAyB");

                for (int ui = 0; ui < NUM_SPECS; ++ui) {
                    List mU;
                    const List& U = mU;          gg(&mU, SPECS[ui]);

                    bdex_TestOutStream out;

                    bdex_OutStreamFunctions::streamOut(out,
                                                       U,
                                                       LIST_MAX_VERSION);
                    const int uStreamLen = out.length();

                    bdex_OutStreamFunctions::streamOut(out,
                                                       V,
                                                       LIST_MAX_VERSION);
                    const int vStreamLen = out.length();

                    bdex_OutStreamFunctions::streamOut(out,
                                                       W,
                                                       LIST_MAX_VERSION);
                    const int wStreamLen = out.length();

                    for (int lod = wStreamLen-1; lod >= 0; --lod) {
                        List mU2(mX);        const List& U2 = mU2;

                        const char *const OD  = out.data();

                        // Must reset stream for each iteration of inner loop.
                        bdex_TestInStream testInStream(OD, lod);
                        testInStream.setSuppressVersionCheck(1);
                        LOOP_ASSERT(lod, testInStream);
                        LOOP_ASSERT(lod, (lod > 0) == !testInStream.isEmpty());

                        bdex_InStreamFunctions::streamIn(testInStream,
                                                         mU2,
                                                         LIST_MAX_VERSION);
                        if (lod >= uStreamLen) {
                            LOOP_ASSERT(lod, testInStream);
                            LOOP3_ASSERT(lod, U, U2, U == U2);
                            LOOP2_ASSERT(lod, uStreamLen,
                                testInStream.isEmpty() == (lod == uStreamLen));
                        }
                        else {
                            LOOP_ASSERT(lod, !testInStream);
                        }

                        List mV2(X);            const List& V2 = mV2;
                        LOOP_ASSERT(lod, X == V2);
                            // if assignment went OK, mY was in valid state

                        bdex_InStreamFunctions::streamIn(testInStream,
                                                         mV2,
                                                         LIST_MAX_VERSION);

                        if (lod >= vStreamLen) {
                            LOOP_ASSERT(lod, testInStream);
                            LOOP_ASSERT(lod, V == V2);
                            LOOP2_ASSERT(lod, vStreamLen,
                                testInStream.isEmpty() == (lod == vStreamLen));
                        }
                        else {
                            LOOP_ASSERT(lod, !testInStream);
                            if (lod <= uStreamLen) {
                                LOOP_ASSERT(lod, X == V2);
                            }
                        }

                        List mW2(X);            const List& W2 = mW2;

                        LOOP_ASSERT(lod, X == W2);
                            // if assignment went OK, mY was in valid state

                        bdex_InStreamFunctions::streamIn(testInStream,
                                                         mW2,
                                                         LIST_MAX_VERSION);

                        LOOP_ASSERT(lod, !testInStream);
                        if (lod <= vStreamLen) {
                            LOOP_ASSERT(lod, X == W2);
                        }

                        // destructors of U2, V2 & W2 will assert they're in
                        // a valid state
                    }
                }
            }
        }

        {
            if (verbose) cout <<
              "\tVerify binary data is identical across platforms for lists\n";
            static const struct {
                int         d_lineNum;                // source line number
                const char *d_spec;                   // the spec
                int         d_length;                 // length of binary
                const char  d_binary[128];            // binary representation
            } DATA[] = {
                { L_, "", 2,
                  { "\x02\x00" },
                },
                { L_, "xA", 8,
                  { "\x02\x01\x00\x80\x00\x00\x00\x78" },
                },
                { L_, "xAxB", 11,
                  { "\x02\x02\x00\x01\xc0\x00\x00\x00\x78\x00\x0c" },
                },
                { L_, "xAxBxC", 16,
                  { "\x02\x03\x00\x01\x02"  "\xe0\x00\x00\x00\x78"
                    "\x00\x0c\x00\x00\x00"  "\x67" },
                },
                { L_, "xAxBxCxDxE", 30,
                  { "\x02\x05\x00\x01\x02"  "\x03\x04\xf8\x00\x00"
                    "\x00\x78\x00\x0c\x00"  "\x00\x00\x67\x00\x00"
                    "\x00\x00\x00\x00\x27"  "\x14\x42\xd3\x00\x00" },
                },
                { L_, "xFxGxHxIxJ", 46,
                  { "\x02\x05\x05\x06\x07"  "\x08\x09\xf8\x00\x00"
                    "\x00\x40\x5a\x80\x62"  "\x4d\xd2\xf1\xaa\x0c"
                    "\x6f\x6e\x65\x2d\x73"  "\x65\x76\x65\x6e\x2d"
                    "\x2d\x2d\x00\x99\x86"  "\x01\xbe\xe6\xac\x00"
                    "\x9b\x13\x02\x2e\xa0"  "\x3e" },
                },
                { L_, "xKxLxMxNxO", 57,
                  { "\x02\x05\x0a\x0b\x0c"  "\x0d\x0e\xf8\x00\x00"
                    "\x00\x05\x78\x4e\x78"  "\x4e\x78\x02\x00\x0c"
                    "\x80\x00\x02\x00\x00"  "\x00\x67\x80\x00\x00"
                    "\x00\x02\x00\x00\x00"  "\x00\x00\x00\x27\x14"
                    "\x80\x00\x00\x00\x00"  "\x00\x00\x00\x02\x42"
                    "\xd3\x00\x00\xa7\xec"  "\x80\x00" },
                },
                // TBD: Uncomment
//                 { L_, "uAuBuCuDuEuF", 12,
//                   { "\x02\x06\x00\x01\x02"  "\x03\x04\x05\x00\x00"
//                     "\x00\x00\x00" },
//                 },
//                 { L_, "xPxQxSxTxUxV", 106,
//                   { "\x02\x06\x0f\x10\x12"  "\x13\x14\x15\xf8\x00"
//                     "\x00\x00\x02\x40\x5a"  "\x80\x62\x4d\xd2\xf1"
//                     "\xaa\xbd\x1b\x4c\x40"  "\x00\x00\x00\x00\x02"
//                     "\x0c\x6f\x6e\x65\x2d"  "\x73\x65\x76\x65\x6e"
//                     "\x2d\x2d\x2d\x00\x02"  "\x00\x9b\x13\x00\x00"
//                     "\x01\x02\x02\x2e\xa0"  "\x3e\x05\x26\x5c\x00"
//                     "\x02\x02\x06\x00\x00"  "\x00\x67\x0c\x6f\x6e"
//                     "\x65\x2d\x73\x65\x76"  "\x65\x6e\x2d\x2d\x2d"
//                     "\x02\x02\x06\x02\x00"  "\x00\x00\x67\x0c\x6f"
//                     "\x6e\x65\x2d\x73\x65"  "\x76\x65\x6e\x2d\x2d"
//                     "\x2d\x80\x00\x00\x00"  "\x00" },
//                 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int VERSION = 2;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *SPEC   = DATA[ti].d_spec;
                const char *BINARY = DATA[ti].d_binary;
                int         LENGTH = DATA[ti].d_length;

                if (veryVerbose) P(SPEC);

                List mX(&testAllocator); gg(&mX, SPEC); const List& X = mX;
                if (veryVerbose) P(X);

                bdex_ByteInStream testInStream(BINARY, LENGTH);

                List U;
                int version;
                testInStream.getVersion(version);
                ASSERT(VERSION == version);
                U.bdexStreamIn(testInStream, VERSION);
                LOOP3_ASSERT(SPEC, U, X, U == X);

                bdex_ByteOutStream out;

                out.putVersion(VERSION);
                // testing stream-out operator here
                X.bdexStreamOut(out, VERSION);
                const char *const OD  = out.data();
                const int         LOD = out.length();

                LOOP4_ASSERT(SPEC, LENGTH, LOD, out, LOD == LENGTH);
                LOOP_ASSERT(SPEC, memcmp(OD, BINARY, LENGTH) == 0);

                if (LOD != LENGTH || memcmp(OD, BINARY, LENGTH)) {
                    cout << "LINE: " << LINE << ", SPEC: " << SPEC << endl;
                    cout << "EXPECTED:\n";
                    hexdump(BINARY, LENGTH);
                    cout << "OD:\n";
                    hexdump(OD, LOD);
                }

                // dump the hex values *only* when 'd' is program parameter 2.
                if (verbose && argv[2][0] == 'd') {
                    cout << '"' << SPEC << "\", " << LOD << ", \n{ ";
                    for (int ij = 0; ij < LOD; ++ij) {
                        cout << hex << "0x" << (0xff & (int) OD[ij]) << ", ";
                    }
                    cout << " }, " << dec << endl;
                }
            }
        }

        {
            if (verbose) cout <<
               "\tVerify binary data is identical across platforms for rows\n";
            static const struct {
                int         d_lineNum;                // source line number
                const char *d_spec;                   // the spec
                int         d_length;                 // length of binary
                const char  d_binary[128];            // binary representation
            } DATA[] = {
                { L_, "", 1,
                  "\x02" },
                { L_, "xA", 6,
                  "\x02\x80\x00\x00\x00"  "\x78" },
                { L_, "xAxB", 8,
                  "\x02\xc0\x00\x00\x00"  "\x78\x00\x0c" },
                { L_, "xAxBxC", 12,
                  "\x02\xe0\x00\x00\x00"  "\x78\x00\x0c\x00\x00"
                  "\x00\x67" },
                { L_, "xAxBxCxDxE", 24,
                  "\x02\xf8\x00\x00\x00"  "\x78\x00\x0c\x00\x00"
                  "\x00\x67\x00\x00\x00"  "\x00\x00\x00\x27\x14"
                  "\x42\xd3\x00\x00" },
                { L_, "xFxGxHxIxJ", 40,
                  "\x02\xf8\x00\x00\x00"  "\x40\x5a\x80\x62\x4d"
                  "\xd2\xf1\xaa\x0c\x6f"  "\x6e\x65\x2d\x73\x65"
                  "\x76\x65\x6e\x2d\x2d"  "\x2d\x00\x99\x86\x01"
                  "\xbe\xe6\xac\x00\x9b"  "\x13\x02\x2e\xa0\x3e" },
                { L_, "xKxLxMxNxO", 51,
                  "\x02\xf8\x00\x00\x00"  "\x05\x78\x4e\x78\x4e"
                  "\x78\x02\x00\x0c\x80"  "\x00\x02\x00\x00\x00"
                  "\x67\x80\x00\x00\x00"  "\x02\x00\x00\x00\x00"
                  "\x00\x00\x27\x14\x80"  "\x00\x00\x00\x00\x00"
                  "\x00\x00\x02\x42\xd3"  "\x00\x00\xa7\xec\x80"
                  "\x00" },
                { L_, "nAnBnCnDnEnF", 5,
                  "\x02\x00\x00\x00\x00" },
                { L_, "xPxQxSxTxUxV", 99,
                  "\x02\xfc\x00\x00\x00"  "\x02\x40\x5a\x80\x62"
                  "\x4d\xd2\xf1\xaa\xbd"  "\x1b\x4c\x40\x00\x00"
                  "\x00\x00\x02\x0c\x6f"  "\x6e\x65\x2d\x73\x65"
                  "\x76\x65\x6e\x2d\x2d"  "\x2d\x00\x02\x00\x9b"
                  "\x13\x00\x00\x01\x02"  "\x02\x2e\xa0\x3e\x05"
                  "\x26\x5c\x00\x02\x02"  "\x06\x00\x00\x00\x67"
                  "\x0c\x6f\x6e\x65\x2d"  "\x73\x65\x76\x65\x6e"
                  "\x2d\x2d\x2d\x02\x02"  "\x06\x02\x00\x00\x00"
                  "\x67\x0c\x6f\x6e\x65"  "\x2d\x73\x65\x76\x65"
                  "\x6e\x2d\x2d\x2d\x80"  "\x00\x00\x00\x00" } };

            enum { NUM_DATA = sizeof(DATA) / sizeof(*DATA) };

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *SPEC   = DATA[ti].d_spec;
                const char *BINARY = DATA[ti].d_binary;
                int         LENGTH = DATA[ti].d_length;

                if (veryVerbose) P(SPEC);

                List mX(&testAllocator); gg(&mX, SPEC); const List& X = mX;
                if (veryVerbose) P(X);
                const int VERSION = 2;
                {
                    bdex_ByteOutStream out;
                    out.putVersion(VERSION);
                    bdex_OutStreamFunctions::streamOut(out,
                                                       X.row(),
                                                       VERSION);

                    const char *const OD  = out.data();
                    const int         LOD = out.length();

                    if (veryVerbose) {
                        cout << "Spec: \"" << SPEC << "\", Length: " <<
                                                                   LOD << endl;
                        cout << "Hex:\n";
                        hexdump(OD, LOD);
                    }

                    LOOP3_ASSERT(SPEC, LENGTH, LOD, LOD == LENGTH);
                    LOOP_ASSERT(SPEC, memcmp(OD, BINARY, LENGTH) == 0);

                    if (LOD != LENGTH || memcmp(OD, BINARY, LENGTH)) {
                        cout << "LINE: " << LINE << ", SPEC: " << SPEC << endl;
                        cout << "EXPECTED:\n";
                        hexdump(BINARY, LENGTH);
                        cout << "OD:\n";
                        hexdump(OD, LOD);
                    }
                }

                my_ElemTypeArray elemTypes;
                getElemTypes(&elemTypes, X);

                {
                    bdex_ByteInStream testInStream(BINARY, LENGTH);

                    List mU;    const List& U = mU;
                    mU.appendNullElements(elemTypes.begin(),
                                          elemTypes.size());
                    int version;
                    testInStream.getVersion(version);
                    bdex_InStreamFunctions::streamIn(testInStream,
                                                     mU.row(),
                                                     version);
                    LOOP3_ASSERT(LINE, U, X, U == X);
                    LOOP_ASSERT(LINE, testInStream);
                    LOOP_ASSERT(LINE, testInStream.isEmpty());
                }
            }
        }

        {
            if (verbose) cout << "\tOn empty and invalid streams." << endl;
            {
                static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9, 15, 16
                    "",               "xA",             "xBxC",     "xCxDxE",
                    "xDxExAxB",       "xExAxBxCxD",    "xAxBxCxDxExAxB",
                    "xAxBxCxDxExAxBxC",
                    "xAxBxCxDxExAxBxCxD",
                    "xAxBxCxDxExAxBxCxDxExAxBxCxDxE",
                    "xAxBxCxDxExAxBxCxDxExAxBxCxDxExA",
                0}; // Null string required as last element.

                static const int EXTEND[] = {
                    0, 1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17
                };
                const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

                bdex_TestInStream testInStream("", 0);
                testInStream.setSuppressVersionCheck(1);
                int oldLen = -1;
                for (int ti = 0; SPECS[ti]; ++ti) {
                    const char *const SPEC = SPECS[ti];
                    const int curLen = (int)strlen(SPEC)/2;

                    if (verbose) cout << "\t\tFor objects of length "
                                      << curLen << '.' << endl;
                    LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
                    oldLen = curLen;

                    if (veryVerbose) { cout << "\t\t\t"; P(SPEC); }

                    // Create control object X.

                    List mX(&testAllocator); gg(&mX, SPEC); const List& X = mX;
                    LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                    for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                      BEGIN_BSLMA_EXCEPTION_TEST {
                      BEGIN_BDEX_EXCEPTION_TEST {
                        testInStream.reset();

                        const int N = EXTEND[ei];

                        if (veryVerbose) { cout << "\t\t\t\t"; P(N); }

                        List t(&testAllocator);      gg(&t, SPEC);
                        stretchRemoveAll(&t, N);    gg(&t, SPEC);

                        // Ensure that reading from an empty or invalid input
                        // stream leaves the stream invalid and the target
                        // object unchanged.

                        LOOP2_ASSERT(ti, ei, testInStream);
                        LOOP2_ASSERT(ti, ei, X == t);

                        bdex_InStreamFunctions::streamIn(testInStream,
                                                         t,
                                                         LIST_MAX_VERSION);
                        LOOP2_ASSERT(ti, ei,!testInStream);
                        LOOP2_ASSERT(ti, ei, X == t);

                        bdex_InStreamFunctions::streamIn(testInStream,
                                                         t,
                                                         LIST_MAX_VERSION);
                        LOOP2_ASSERT(ti, ei,!testInStream);
                        LOOP2_ASSERT(ti, ei, X == t);

                        testInStream.reset();

                        LOOP2_ASSERT(ti, ei, testInStream);
                        LOOP2_ASSERT(ti, ei, X.row() == t.row());

                        bdex_InStreamFunctions::streamIn(testInStream,
                                                         t.row(),
                                                         ROW_MAX_VERSION);
                        if (t.length()) {
                            LOOP2_ASSERT(ti, ei, !testInStream);
                        }
                        LOOP2_ASSERT(ti, ei, X.row() == t.row());

                        bdex_InStreamFunctions::streamIn(testInStream,
                                                         t.row(),
                                                         ROW_MAX_VERSION);
                        if (t.length()) {
                            LOOP2_ASSERT(ti, ei, !testInStream);
                        }
                        LOOP2_ASSERT(ti, ei, X.row() == t.row());

                      } END_BDEX_EXCEPTION_TEST
                      } END_BSLMA_EXCEPTION_TEST
                    }
                }
            }
        }
      }

DEFINE_TEST_CASE(12) {
        // --------------------------------------------------------------------
        // TESTING 'bdem_Table' CONSTRUCTORS
        // Concerns:
        //   a. The Pool option must be used as specified.
        //   b. The internal state is valid for all possible options
        //      settings, and for different source elements.
        //      Internal state validity includes:
        //      1) Correct element types that have been initialized correctly
        //         with the correct constructor.
        //      2) The memory allocated each row is sufficient.
        //      3) The memory size recorded is correct.
        //
        //      Checking internal state tests could include: adding new
        //      elements, deleting elements and setting values for elements.
        //
        //   c. Constructors taking element types should correctly initialize
        //      elements to unset values.
        //
        // Plan:
        //
        //   For the constructors that do not take the pool option, the
        //   default is BDEM_PASS_THROUGH.  For these bdem_Table objects, a
        //   string array is created.  When a long string is deleted from the
        //   array, the string memory will be freed to the test allocator. The
        //   string is now inserted into the array.  Memory will now be
        //   re-allocated from the test allocator.
        //
        //   Verify AllocationMode.
        //     Construct objects with the 3 different options using the test
        //     allocator.  The default is BDEM_PASS_THROUGH.  Verify that the
        //     correct option is used.  For each object, an array is appended.
        //     The array is then removed.  The identical array is then
        //     appended to the Table again.
        //   a) For bdem_Table objects with BDEM_PASS_THROUGH, adding and
        //      removing the array causes memory allocation and deallocation.
        //   b) For bdem_Table objects with BDEM_WRITE_MANY, adding causes
        //      memory allocation.  However, removing and re-adding does not
        //      cause any memory allocation or deallocation.
        //   c) For bdem_Table objects with BDEM_WRITE_ONCE.  Adding may cause
        //      memory allocation from the testAllocator, but usually not.
        //      Removing the array does not cause any deallocation.
        //
        // Testing:
        //   bdem_Table(bslma_Allocator *ba = 0);
        //   bdem_Table(bdem_AggregateOption::AllocationStrategy aM, *ba = 0);
        //   bdem_Table(const my_ElemTypeArray& et, *ba = 0);
        //   bdem_Table(const my_ElemTypeArray& et, allocationMode, *ba = 0);
        //   bdem_Table(const bdem_Table& original, *ba = 0);
        //   bdem_Table(const bdem_Table& original, allocationMode, *ba = 0);
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << "bdem_Table(alloc)" << endl;

        my_ElemTypeArray ETA;

        BEGIN_BSLMA_EXCEPTION_TEST {
            Table T1(Z);
            ASSERT(T1.numRows() == 0 && T1.numColumns() == 0);
            const bdem_ElemType::Type ETA[] = {
                bdem_ElemType::BDEM_CHAR,
                bdem_ElemType::BDEM_CHAR_ARRAY,
                bdem_ElemType::BDEM_STRING_ARRAY
            };
            T1.reset(ETA, NUM_ARRYELEMS(ETA));
            ASSERT(T1.numRows() == 0 && T1.numColumns() == 3);
            T1.appendNullRow();
            ASSERT(T1.numRows() == 1 && T1.numColumns() == 3);
            Row& mR = T1.theModifiableRow(0);
            mR[0].theModifiableChar() = ' ';
            mR[1].theModifiableCharArray()   = XK;
            mR[2].theModifiableStringArray() = XQ;
            ASSERT(T1.numRows() == 1 && T1.numColumns() == 3);
        } END_BSLMA_EXCEPTION_TEST

        if (verbose) cout << "verify BDEM_PASS_THROUGH default" << endl;
        {
            // BDEM_PASS_THROUGH is the default.
            Table T2(Z);

            ASSERT(T2.numRows() == 0 && T2.numColumns() == 0);

            // verify the BDEM_PASS_THROUGH
            const bdem_ElemType::Type ETA[] = {
                bdem_ElemType::BDEM_CHAR_ARRAY
            };
            T2.reset(ETA, NUM_ARRYELEMS(ETA));

            T2.appendNullRow();
            T2.removeAllRows();

            int numSmall = testAllocator.numBytesInUse();
            T2.appendNullRow();
            T2.theModifiableRow(0)[0].theModifiableCharArray() = XK;
            int numBig   = testAllocator.numBytesInUse();
            ASSERT(numSmall < numBig);

            for (int i = 0; i < 10; ++i) {
                if (veryVerbose) {
                    int IN_USE = testAllocator.numBytesInUse();
                    cout << "\t\t"; P_(i); P(IN_USE);
                }
                T2.removeRow(0);

                LOOP_ASSERT(i, numSmall == testAllocator.numBytesInUse());
                T2.appendNullRow();
                T2.theModifiableRow(0)[0].theModifiableCharArray() = XK;
                LOOP_ASSERT(i, numBig   == testAllocator.numBytesInUse());
            }

            T2.removeAll();
            ASSERT(T2.numRows() == 0 && T2.numColumns() == 0);

            // verify the NO_HINT allocationMode
            const bdem_ElemType::Type ETA2[] = {
                bdem_ElemType::BDEM_STRING_ARRAY
            };
            T2.reset(ETA2, NUM_ARRYELEMS(ETA2));

            T2.appendNullRow();
            T2.theModifiableRow(0)[0].theModifiableStringArray() = UQ;
            T2.theModifiableRow(0)[0].theModifiableStringArray().push_back(XG);
                                                               // "long" string
            T2.removeRow(0);
            numSmall = testAllocator.numBytesInUse();
            T2.appendNullRow();
            T2.theModifiableRow(0)[0].theModifiableStringArray() = UQ;
            T2.theModifiableRow(0)[0].theModifiableStringArray().push_back(XG);
                                                               // "long" string
            ASSERT(T2.numRows() == 1 && T2.numColumns() == 1);
            numBig   = testAllocator.numBytesInUse();

            for (int j = 0; j < 10; ++j) {
                if (veryVerbose) {
                    int IN_USE = testAllocator.numBytesInUse();
                    cout << "\t\t"; P_(j); P(IN_USE);
                }

                T2.removeRow(0);
                LOOP_ASSERT(j, numSmall == testAllocator.numBytesInUse());

                T2.appendNullRow();
                T2.theModifiableRow(0)[0].theModifiableStringArray() = UQ;
                // "long" string
                T2.theModifiableRow(0)[0].theModifiableStringArray().
                                                                 push_back(XG);
                LOOP_ASSERT(j, 1        == T2[0][0].theStringArray().size());
                LOOP_ASSERT(j, numBig   == testAllocator.numBytesInUse());
            }
        }

        if (verbose) cout << "bdem_Table(option, alloc)" << endl;

        static const struct {
            int                        d_lineNum; // source line number
            bdem_AggregateOption::AllocationStrategy d_option;
                                                  // value for pool option
            int                        d_ni;      // number of iterations
            CompareEnum                d_rbefore; // comparison result before
                                                  // append
            CompareEnum                d_rafter;  // comparison result after
                                                  // append
            CompareEnum                d_rfinal;  // comparison result after
                                                  // all iterations
        } OPTDATA[] = {
           { L_, bdem_AggregateOption::BDEM_PASS_THROUGH,         10,
                 COMPARE_GT, COMPARE_EQ, COMPARE_EQ },
           { L_, bdem_AggregateOption::BDEM_WRITE_MANY,       10,
                 COMPARE_EQ, COMPARE_EQ, COMPARE_EQ },
           { L_, bdem_AggregateOption::BDEM_WRITE_ONCE, 10,
                 COMPARE_LE, COMPARE_LE, COMPARE_LE },
        };

        const int NUM_OPTDATA = sizeof OPTDATA / sizeof *OPTDATA;

        for (int ti = 0; ti < NUM_OPTDATA; ++ti) {
            const int                        LINE    = OPTDATA[ti].d_lineNum;
            const bdem_AggregateOption::AllocationStrategy
                                             OPTVAL  = OPTDATA[ti].d_option;
            const int                        NI      = OPTDATA[ti].d_ni;
            const CompareEnum                RBEFORE = OPTDATA[ti].d_rbefore;
            const CompareEnum                RAFTER  = OPTDATA[ti].d_rafter;
            const CompareEnum                RFINAL  = OPTDATA[ti].d_rfinal;

            if (veryVerbose)
                cout << "bdem_Table(" << optionToString(OPTVAL)
                     << ", alloc)" << endl;

            BEGIN_BSLMA_EXCEPTION_TEST {
                Table T2(OPTVAL, Z);
                ASSERT(T2.numRows() == 0 && T2.numColumns() == 0);
                const bdem_ElemType::Type ETA[] = {
                    bdem_ElemType::BDEM_CHAR,
                    bdem_ElemType::BDEM_CHAR_ARRAY,
                    bdem_ElemType::BDEM_STRING_ARRAY
                };
                T2.reset(ETA, NUM_ARRYELEMS(ETA));
                ASSERT(T2.numRows() == 0 && T2.numColumns() == 3);
                T2.appendNullRow();
                ASSERT(T2.numRows() == 1 && T2.numColumns() == 3);
                T2.theModifiableRow(0)[0].theModifiableChar() = ' ';
                T2.theModifiableRow(0)[1].theModifiableCharArray()   = XK;
                T2.theModifiableRow(0)[2].theModifiableStringArray() = XQ;
                ASSERT(T2.numRows() == 1 && T2.numColumns() == 3);
            } END_BSLMA_EXCEPTION_TEST

            if (verbose) cout << "\t\tverify pool option "
                              << optionToString(OPTVAL) << endl;
            Table T2(OPTVAL, Z);

            ASSERT(T2.numRows() == 0 && T2.numColumns() == 0);

            const bdem_ElemType::Type ETA3[] = {
                bdem_ElemType::BDEM_CHAR_ARRAY
            };
            T2.reset(ETA3, NUM_ARRYELEMS(ETA3));
            T2.appendNullRow();
            T2.theModifiableRow(0)[0].theModifiableCharArray() = XK;
            int NUM_BYTES = testAllocator.numBytesInUse();

            for (int i = 0; i < NI; ++i) {
                if (veryVerbose) {
                    int IN_USE = testAllocator.numBytesInUse();
                    cout << "\t\t"; P_(i); P(IN_USE);
                }
                T2.removeRow(0);

                LOOP_ASSERT(i,
                        compareValues(NUM_BYTES, testAllocator.numBytesInUse(),
                                      RBEFORE));
                T2.appendNullRow();
                T2.theModifiableRow(0)[0].theModifiableCharArray() = XK;
                LOOP_ASSERT(i,
                        compareValues(NUM_BYTES, testAllocator.numBytesInUse(),
                                      RAFTER));
            }
            ASSERT(compareValues(NUM_BYTES, testAllocator.numBytesInUse(),
                                 RFINAL));
        }

        if (verbose) cout << "verify NO_HINT option" << endl;
        {
            Table T2(bdem_AggregateOption::BDEM_PASS_THROUGH, Z);

            ASSERT(T2.numRows() == 0 && T2.numColumns() == 0);
            const bdem_ElemType::Type ETA[] = {
                bdem_ElemType::BDEM_STRING_ARRAY
            };
            T2.reset(ETA, NUM_ARRYELEMS(ETA));
            T2.appendNullRow();
            Row& mR = T2.theModifiableRow(0);
            mR[0].theModifiableStringArray() = UQ;
            // "long" string
            mR[0].theModifiableStringArray().push_back(XG);
            mR[0].theModifiableStringArray().erase(
                                               mR[0].theStringArray().begin());
            const int NUM_SMALL = testAllocator.numBytesInUse();
            mR[0].theModifiableStringArray().push_back(XG);
            ASSERT(T2.numRows() == 1 && T2.numColumns() == 1);
            const int NUM_BIG   = testAllocator.numBytesInUse();
            ASSERT(NUM_BIG >= NUM_SMALL);
            for (int i = 0; i < 10; ++i) {
                if (veryVerbose) {
                    int IN_USE = testAllocator.numBytesInUse();
                    cout << "\t\t"; P_(i); P_(IN_USE);
                }
                mR[0].theModifiableStringArray().erase(
                                               mR[0].theStringArray().begin());
                if (veryVerbose) {
                    int IN_USE_AFTER_REMOVE = testAllocator.numBytesInUse();
                    P(IN_USE_AFTER_REMOVE);
                }
                LOOP_ASSERT(i, 0 == T2[0][0].theStringArray().size());

                LOOP_ASSERT(i, NUM_SMALL == testAllocator.numBytesInUse());
                // "long" string
                mR[0].theModifiableStringArray().push_back(XG);
                LOOP_ASSERT(i, 1 == T2[0][0].theStringArray().size());
                LOOP_ASSERT(i, NUM_BIG   == testAllocator.numBytesInUse());
            }
        }

        const bdem_ElemType::Type elems[] = {
            bdem_ElemType::BDEM_CHAR,
            bdem_ElemType::BDEM_LIST
        };

        if (verbose) cout << "bdem_Table(elems, alloc)" << endl;
        BEGIN_BSLMA_EXCEPTION_TEST {
            Table T11(elems, NUM_ARRYELEMS(elems), Z);

            T11.appendNullRow();
            Row& mR = T11.theModifiableRow(0);
            mR[0].theModifiableChar() = ' ';
            mR[1].theModifiableList() = XU;
            ASSERT(T11.numRows() == 1 && T11.numColumns() == 2);
        } END_BSLMA_EXCEPTION_TEST

        if (verbose) cout << "verify BDEM_PASS_THROUGH default" << endl;
        {
            // BDEM_WRITE_MANY is the default.
            Table T2(elems, NUM_ARRYELEMS(elems), Z);

            ASSERT(T2.numRows() == 0 && T2.numColumns() == 2);

            T2.removeAll();

            // verify the BDEM_WRITE_MANY
            const bdem_ElemType::Type ETA[] = {
                bdem_ElemType::BDEM_CHAR_ARRAY,
                bdem_ElemType::BDEM_STRING_ARRAY
            };
            T2.reset(ETA, NUM_ARRYELEMS(ETA));

            // cycle first
            T2.appendNullRow();
            T2.theModifiableRow(0)[0].theModifiableCharArray() = XK;
            T2.removeRow(0);

            T2.appendNullRow();
            T2.theModifiableRow(0)[0].theModifiableCharArray() = XK;
            T2.removeRow(0);
            int numSmall = testAllocator.numBytesInUse();
            T2.appendNullRow();
            T2.theModifiableRow(0)[0].theModifiableCharArray() = XK;
            int numBig   = testAllocator.numBytesInUse();
            ASSERT(numSmall < numBig);

            for (int i = 0; i < 10; ++i) {
                if (veryVerbose) {
                    int IN_USE = testAllocator.numBytesInUse();
                    cout << "\t\t"; P_(i); P(IN_USE);
                }
                T2.removeRow(0);

                LOOP_ASSERT(i, numSmall == testAllocator.numBytesInUse());
                T2.appendNullRow();
                T2.theModifiableRow(0)[0].theModifiableCharArray() = XK;
                LOOP_ASSERT(i, numBig   == testAllocator.numBytesInUse());
            }

            T2.removeRow(0);
            ASSERT(T2.numRows() == 0 && T2.numColumns() == 2);

            // verify the NO_HINT option
            T2.appendNullRow();
            T2.theModifiableRow(0)[1].theModifiableStringArray() = UQ;
            T2.theModifiableRow(0)[1].theModifiableStringArray().push_back(XG);
                                                               // "long" string
            T2.theModifiableRow(0)[1].theModifiableStringArray().clear();
            numSmall = testAllocator.numBytesInUse();

            T2.theModifiableRow(0)[1].theModifiableStringArray() = UQ;
            T2.theModifiableRow(0)[1].theModifiableStringArray().push_back(XG);
                                                               // "long" string
            numBig   = testAllocator.numBytesInUse();

            ASSERT(T2.numRows() == 1 && T2.numColumns() == 2);

            for (int j = 0; j < 10; ++j) {
                if (veryVerbose) {
                    int IN_USE = testAllocator.numBytesInUse();
                    cout << "\t\t"; P_(j); P(IN_USE);
                }
                T2.theModifiableRow(0)[1].theModifiableStringArray().clear();
                LOOP_ASSERT(j, T2[0][1].theStringArray().size() == 0);

                LOOP_ASSERT(j, numSmall == testAllocator.numBytesInUse());
                // "long" string
                T2.theModifiableRow(0)[1].theModifiableStringArray().
                                                                 push_back(XG);
                LOOP_ASSERT(j, T2[0][1].theStringArray().size() == 1);
                LOOP_ASSERT(j, numBig   == testAllocator.numBytesInUse());
            }
        }

        for (int ti = 0; ti < NUM_OPTDATA; ++ti) {
            const int                        LINE    = OPTDATA[ti].d_lineNum;
            const bdem_AggregateOption::AllocationStrategy
                                             OPTVAL  = OPTDATA[ti].d_option;
            const int                        NI      = OPTDATA[ti].d_ni;
            const CompareEnum                RBEFORE = OPTDATA[ti].d_rbefore;
            const CompareEnum                RAFTER  = OPTDATA[ti].d_rafter;
            const CompareEnum                RFINAL  = OPTDATA[ti].d_rfinal;

            if (veryVerbose)
                cout << "bdem_Table(elems, " << optionToString(OPTVAL)
                     << ", alloc)" << endl;

            BEGIN_BSLMA_EXCEPTION_TEST {
                Table T3(elems, NUM_ARRYELEMS(elems), OPTVAL, Z);
                ASSERT(T3.numRows() == 0 && T3.numColumns() == 2);
                T3.removeAll();
                ASSERT(T3.numRows() == 0 && T3.numColumns() == 0);
                const bdem_ElemType::Type ETA[] = {
                    bdem_ElemType::BDEM_CHAR,
                    bdem_ElemType::BDEM_CHAR_ARRAY,
                    bdem_ElemType::BDEM_STRING_ARRAY
                };
                T3.reset(ETA, NUM_ARRYELEMS(ETA));
                T3.appendNullRow();
                T3.theModifiableRow(0)[0].theModifiableChar() = ' ';
                T3.theModifiableRow(0)[1].theModifiableCharArray()   = XK;
                T3.theModifiableRow(0)[2].theModifiableStringArray() = XQ;
                ASSERT(T3.numRows() == 1 && T3.numColumns() == 3);
            } END_BSLMA_EXCEPTION_TEST

            if (verbose) cout << "\tverify pool option "
                              << optionToString(OPTVAL) << endl;
            Table T2(elems, NUM_ARRYELEMS(elems), OPTVAL, Z);

            ASSERT(T2.numRows() == 0 && T2.numColumns() == 2);
            T2.removeAll();
            ASSERT(T2.numRows() == 0 && T2.numColumns() == 0);

            const bdem_ElemType::Type ETA[] = {
                bdem_ElemType::BDEM_CHAR_ARRAY
            };
            T2.reset(ETA, NUM_ARRYELEMS(ETA));
            T2.appendNullRow();
            T2.theModifiableRow(0)[0].theModifiableCharArray() = XK;
            ASSERT(T2.numRows() == 1 && T2.numColumns() == 1);
            int NUM_BYTES  = testAllocator.numBytesInUse();

            for (int i = 0; i < NI; ++i) {
                if (veryVerbose) {
                    int IN_USE = testAllocator.numBytesInUse();
                    cout << "\t\t"; P_(i); P(IN_USE);
                }
                T2.removeRow(0);
                ASSERT(T2.numRows() == 0 && T2.numColumns() == 1);

                LOOP_ASSERT(i,
                        compareValues(NUM_BYTES, testAllocator.numBytesInUse(),
                                      RBEFORE));
                T2.appendNullRow();
                T2.theModifiableRow(0)[0].theModifiableCharArray() = XK;
                ASSERT(T2.numRows() == 1 && T2.numColumns() == 1);
                LOOP_ASSERT(i,
                        compareValues(NUM_BYTES, testAllocator.numBytesInUse(),
                                      RAFTER));
            }
            ASSERT(compareValues(NUM_BYTES, testAllocator.numBytesInUse(),
                                 RFINAL));
        }

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec;     // spec for Table object
            int         d_nRows;    // number of rows in Table
            int         d_nCols;    // number of columns in Table
        } DATA[] = {
            //line  t-spec                               nrows    ncols
            //----  ------                               -----    -----
            { L_,   "",                                  0,       0,     },
            { L_,   ";",                                 1,       0,     },
            { L_,   ";;",                                2,       0,     },
            { L_,   ";;;",                               3,       0,     },
            { L_,   ";;;;",                              4,       0,     },
            { L_,   ";;;;;",                             5,       0,     },

            { L_,   "A;",                                0,       1,     },
            { L_,   "B;x;",                              1,       1,     },
            { L_,   "C;x;y;",                            2,       1,     },
            { L_,   "D;x;y;u;",                          3,       1,     },
            { L_,   "E;x;y;u;x;",                        4,       1,     },
            { L_,   "F;x;y;u;x;y;",                      5,       1,     },

            { L_,   "GH;",                               0,       2,     },
            { L_,   "IJ;xy;",                            1,       2,     },
            { L_,   "KL;xy;yx;",                         2,       2,     },
            { L_,   "MN;xy;yx;ux;",                      3,       2,     },
            { L_,   "OP;xy;yx;ux;xx;",                   4,       2,     },
            { L_,   "QR;xy;yx;ux;xx;yy;",                5,       2,     },

            { L_,   "STU;",                              0,       3,     },
            { L_,   "VBA;xyu;",                          1,       3,     },
            { L_,   "EDC;xyu;uyx;",                      2,       3,     },
            { L_,   "HGF;xyu;uyx;uxy;",                  3,       3,     },
            { L_,   "KJI;xyu;uyx;uxy;xxx;",              4,       3,     },
            { L_,   "NML;xyu;uyx;uxy;xxx;yyy;",          5,       3,     },

            { L_,   "RQPO;",                             0,       4,     },
            { L_,   "VUTS;xyux;",                        1,       4,     },
            { L_,   "DCBA;xyux;uyxu;",                   2,       4,     },
            { L_,   "HGFE;xyux;uyxu;uxyu;",              3,       4,     },
            { L_,   "LKJI;xyux;uyxu;uxyu;xxxx;",         4,       4,     },
            { L_,   "PONM;xyux;uyxu;uxyu;xxxx;yyyy;",    5,       4,     },

            { L_,   "UTSRQ;",                            0,       5,     },
            { L_,   "VABCD;xyuxy;",                      1,       5,     },
            { L_,   "EDFGI;xyuxy;uyxuy;",                2,       5,     },
            { L_,   "JKLMN;xyuxy;uyxuy;uxyux;",          3,       5,     },
            { L_,   "OPQRS;xyuxy;uyxuy;uxyux;xxxxx;",    4,       5,     },
            { L_,   "TUVAB;xyuxy;uyxuy;uxyux;xxxxx;yyyyy;", 5,    5,     },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE  = DATA[ti].d_lineNum;
                const char *SPEC  = DATA[ti].d_spec;
                const int   NROWS = DATA[ti].d_nRows;
                const int   NCOLS = DATA[ti].d_nCols;

                if (veryVerbose) { P_(LINE); P_(SPEC); P_(NROWS); P(NCOLS); }

                Table mW(&testAllocator);      hh(&mW, SPEC);  // control
                const Table& W = mW;
                LOOP_ASSERT(LINE, W.numRows() == NROWS
                            && W.numColumns() == NCOLS);
                if (veryVerbose) { cout << "\t"; P(W); }

                my_ElemTypeArray e1;
                my_ElemTypeArray e2;
                getElemTypes(&e1, W);
                Table T3(vectorData(e1), e1.size(),
                         bdem_AggregateOption::BDEM_WRITE_MANY, Z);

                LOOP_ASSERT(LINE, W.numRows() == NROWS
                            && W.numColumns() == NCOLS);
                LOOP_ASSERT(LINE, T3.numRows() == 0
                            && T3.numColumns() == NCOLS);
                getElemTypes(&e1, T3);
                getElemTypes(&e2, W);
                LOOP_ASSERT(LINE, e1 == e2);
                if (veryVerbose) { cout << "\t"; P(T3); }

                Table T4(vectorData(e2), e2.size(), Z);

                if (veryVerbose) { cout << "\t"; P(T4); }
                LOOP_ASSERT(LINE, W.numRows() == NROWS
                            && W.numColumns() == NCOLS);
                LOOP_ASSERT(LINE, T4.numRows() == 0
                            && T4.numColumns() == NCOLS);
                getElemTypes(&e1, T4);
                getElemTypes(&e2, W);
                LOOP_ASSERT(LINE, e1 == e2);
            }
        }

        if (verbose) cout << "\nCopy construct values." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE  = DATA[ti].d_lineNum;
                const char *SPEC  = DATA[ti].d_spec;
                const int   NROWS = DATA[ti].d_nRows;
                const int   NCOLS = DATA[ti].d_nCols;

                if (veryVerbose) { P_(LINE); P_(SPEC); P_(NROWS); P(NCOLS); }

                // Create control objects w, w0.
                Table mW(&testAllocator);     hh(&mW, SPEC);
                const Table& W = mW;
                LOOP_ASSERT(ti, W.numRows() == NROWS &&
                             W.numColumns() == NCOLS);
                Table mW0(&testAllocator);    hh(&mW0, SPEC);
                const Table& W0 = mW0;
                mW0.removeAllRows();
                LOOP_ASSERT(ti, W0.numRows() == 0 &&
                             W0.numColumns() == NCOLS);
                if (veryVerbose) { cout << "\t"; P_(W); P(W0); }

                Table mX(&testAllocator);
                const Table &X = mX;          hh(&mX, SPEC);
                Table mX0(&testAllocator);
                const Table &X0 = mX0;        hh(&mX0, SPEC);
                mX0.removeAllRows();
                if (veryVerbose) { cout << "\t\t"; P_(X); P(X0); }

                {                                   // Table, no Allocator
                    const Table Y(X);
                    if (veryVerbose) { cout << "\t\t\t"; P(Y); }
                    LOOP_ASSERT(SPEC, W == Y);
                    LOOP_ASSERT(SPEC, W == X);
                }

                {                                   // ??, no Allocator
                    my_ElemTypeArray e1;
                    getElemTypes(&e1, X0);
                    const Table Y(vectorData(e1), e1.size());
                    if (veryVerbose) { cout << "\t\t\t"; P(Y); }
                    LOOP_ASSERT(SPEC, W0 == Y);
                    LOOP_ASSERT(SPEC, W0 == X0);
                }

                {                                   // Table, Allocator
                    const Table Y(X, Z);
                    if (veryVerbose) { cout << "\t\t\t"; P(Y); }
                    LOOP_ASSERT(SPEC, W == Y);
                    LOOP_ASSERT(SPEC, W == X);
                }

                {                                   // ??, Allocator
                    my_ElemTypeArray e1;
                    getElemTypes(&e1, X0);
                    const Table Y(vectorData(e1), e1.size(), Z);
                    if (veryVerbose) { cout << "\t\t\t"; P(Y); }
                    LOOP_ASSERT(SPEC, W0 == Y);
                    LOOP_ASSERT(SPEC, W0 == X0);
                }

                {                           // Table, Option, Allocator
                    const Table Y(X, bdem_AggregateOption::BDEM_WRITE_MANY, Z);
                    if (veryVerbose) { cout << "\t\t\t"; P(Y); }
                    LOOP_ASSERT(SPEC, W == Y);
                    LOOP_ASSERT(SPEC, W == X);
                }

                {                           // ??, Option, Allocator
                    my_ElemTypeArray e1;
                    getElemTypes(&e1, X0);
                    const Table Y(vectorData(e1), e1.size(),
                                  bdem_AggregateOption::BDEM_WRITE_MANY,
                                  Z);
                    if (veryVerbose) { cout << "\t\t\t"; P(Y); }
                    LOOP_ASSERT(SPEC, W0 == Y);
                    LOOP_ASSERT(SPEC, W0 == X0);
                }

            }
        }

        Table CC(Z);

        for (int ti = 0; ti < NUM_OPTDATA; ++ti) {
            const int                        LINE    = OPTDATA[ti].d_lineNum;
            const bdem_AggregateOption::AllocationStrategy
                                             OPTVAL  = OPTDATA[ti].d_option;
            const int                        NI      = OPTDATA[ti].d_ni;
            const CompareEnum                RBEFORE = OPTDATA[ti].d_rbefore;
            const CompareEnum                RAFTER  = OPTDATA[ti].d_rafter;
            const CompareEnum                RFINAL  = OPTDATA[ti].d_rfinal;

            if (veryVerbose)
                cout << "bdem_Table(Table, " << optionToString(OPTVAL)
                     << ", alloc)" << endl;

            BEGIN_BSLMA_EXCEPTION_TEST {
                Table T3(CC, OPTVAL, Z);
                ASSERT(T3.numRows() == 0 && T3.numColumns() == 0);
                ETA.clear();
                ETA.push_back(bdem_ElemType::BDEM_CHAR);
                ETA.push_back(bdem_ElemType::BDEM_CHAR_ARRAY);
                ETA.push_back(bdem_ElemType::BDEM_STRING_ARRAY);
                T3.reset(vectorData(ETA), ETA.size());
                T3.appendNullRow();
                T3.theModifiableRow(0)[0].theModifiableChar() = ' ';
                T3.theModifiableRow(0)[1].theModifiableCharArray()   = XK;
                T3.theModifiableRow(0)[2].theModifiableStringArray() = XQ;
                ASSERT(T3.numRows() == 1 && T3.numColumns() == 3);
            } END_BSLMA_EXCEPTION_TEST

            if (verbose) cout << "\tverify pool option "
                              << optionToString(OPTVAL) << endl;
            Table T2(CC, OPTVAL, Z);
            ASSERT(T2.numRows() == 0 && T2.numColumns() == 0);

            ETA.clear();
            ETA.push_back(bdem_ElemType::BDEM_CHAR_ARRAY);
            T2.reset(vectorData(ETA), ETA.size());
            T2.appendNullRow();
            T2.theModifiableRow(0)[0].theModifiableCharArray() = XK;
            int NUM_BYTES = testAllocator.numBytesInUse();

            for (int i = 0; i < NI; ++i) {
                if (veryVerbose) {
                    int IN_USE = testAllocator.numBytesInUse();
                    cout << "\t\t"; P_(i); P(IN_USE);
                }
                T2.removeRow(0);

                LOOP_ASSERT(i,
                        compareValues(NUM_BYTES, testAllocator.numBytesInUse(),
                                      RBEFORE));
                T2.appendNullRow();
                T2.theModifiableRow(0)[0].theModifiableCharArray() = XK;
                LOOP_ASSERT(i,
                        compareValues(NUM_BYTES, testAllocator.numBytesInUse(),
                                      RAFTER));
            }
            ASSERT(compareValues(NUM_BYTES, testAllocator.numBytesInUse(),
                                 RFINAL));
        }
      }

DEFINE_TEST_CASE(11) {
        // --------------------------------------------------------------------
        // TESTING 'bdem_List' CONSTRUCTORS
        // Concerns:
        //   a. The Pool option must be used as specified.
        //   b. The internal state is valid for all possible options
        //      settings, and for different source elements.
        //      Internal state validity includes:
        //      1) Correct element types that have been initialized correctly
        //         with the correct constructor.
        //      2) The memory allocated for the row is sufficient.
        //      3) The memory size recorded is correct.
        //
        //      Checking internal state tests could include: adding new
        //      elements, deleting elements and setting values for elements.
        //
        //   c. Constructors taking element types should correctly initialize
        //      elements to unset values.
        //
        // Plan:
        //
        //   For the constructors that do not take the pool option, the
        //   default is BDEM_PASS_THROUGH.  Verify this.
        //   For these bdem_List objects, a string array is created.
        //   When a long string is deleted from the array, the string memory
        //   will be freed to the test allocator.  The allocator will show
        //   that the memory has been freed.  The same string is now inserted
        //   into the array.  Memory will now be allocated from the test
        //   allocator.
        //
        //   Verify AllocationMode.
        //     Construct objects with the 3 different options using the test
        //     allocator.  The default is BDEM_PASS_THROUGH.  Verify that the
        //     correct option is used.  For each object, an array is appended.
        //     The array is then removed.
        //     The identical array is then appended to the list again.
        //   a) For bdem_List object with BDEM_PASS_THROUGH.  Adding and
        //      removing the array causes memory allocation and deallocation.
        //   b) For bdem_List object with BDEM_WRITE_MANY.  Adding causes
        //      memory allocation.  However, removing and re-adding does not
        //      cause any memory allocation or deallocation.
        //   c) For bdem_List object with BDEM_WRITE_ONCE.  Adding causes
        //      memory allocation.  Removing the array does not cause any
        //      deallocation.  Re-adding the array causes memory allocation.
        //
        // Testing:
        //   bdem_List(bslma_Allocator *ba = 0);
        //   bdem_List(bdem_AggregateOption::AllocationStrategy, *ba = 0);
        //   bdem_List(const my_ElemTypeArray& et, *ba = 0);
        //   bdem_List(const my_ElemTypeArray& et, option, *ba = 0);
        //   bdem_List(const ExplicitRow& original, *ba = 0);
        //   bdem_List(const bdem_List& original, *ba = 0);
        //   bdem_List(const ExplicitRow& original, option, *ba = 0);
        //   bdem_List(const bdem_List& original, option, *ba = 0);
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << "bdem_List(alloc)" << endl;

        BEGIN_BSLMA_EXCEPTION_TEST {
            List L1(Z);
            ASSERT (L1.length() == 0);
            L1.appendChar(' ');
            L1.appendCharArray(XK);
            L1.appendStringArray(XQ);
            ASSERT (L1.length() == 3);
        } END_BSLMA_EXCEPTION_TEST

        if (verbose) cout << "verify BDEM_PASS_THROUGH default" << endl;
        {
            // BDEM_PASS_THROUGH is the default
            bslma_TestAllocator ta(veryVeryVerbose);
            List L2(&ta);

            ASSERT (L2.length() == 0);

            // verify the BDEM_PASS_THROUGH option

            // exercise the allocate through a cycle before we start
            L2.appendCharArray(XK);
            L2.removeAll();
            L2.appendCharArray(XK);
            L2.removeAll();

            int numSmall = ta.numBytesInUse();
            L2.appendCharArray(XK);
            int numBig   = ta.numBytesInUse();
            ASSERT(numBig > numSmall);

            for (int i = 0; i < 10; ++i) {
                if (veryVerbose) {
                    int IN_USE = ta.numBytesInUse();
                    cout << "\t\t"; P_(i); P(IN_USE);
                }
                L2.removeAll();

                LOOP3_ASSERT(i,
                             numSmall,
                             ta.numBytesInUse(),
                             numSmall == ta.numBytesInUse());
                L2.appendCharArray(XK);
                LOOP3_ASSERT(i,
                             numBig,
                             ta.numBytesInUse(),
                             numBig == ta.numBytesInUse());
            }

            L2.removeAll();
            ASSERT (L2.length() == 0);

            // verify the NO_HINT option
            L2.appendStringArray(UQ);
            // "long" string
            L2.theModifiableStringArray(0).push_back(XG);
            L2.theModifiableStringArray(0).erase(L2.theStringArray(0).begin());
            numSmall = ta.numBytesInUse();
            L2.theModifiableStringArray(0).push_back(XG);
            ASSERT (L2.length() == 1);
            numBig   = ta.numBytesInUse();
            ASSERT(numBig >= numSmall);
            for (int j = 0; j < 10; ++j) {
                if (veryVerbose) {
                    int IN_USE = ta.numBytesInUse();
                    cout << "\t\t"; P_(j); P(IN_USE);
                }
                L2.theModifiableStringArray(0).erase(
                                                 L2.theStringArray(0).begin());
                LOOP_ASSERT(j, L2.theStringArray(0).size() == 0);

                LOOP_ASSERT(j, numSmall == ta.numBytesInUse());
                // "long" string
                L2.theModifiableStringArray(0).push_back(XG);
                LOOP_ASSERT(j, L2.theStringArray(0).size() == 1);
                LOOP_ASSERT(j, numBig   == ta.numBytesInUse());
            }
        }

        if (verbose) cout << "bdem_List(option, alloc)" << endl;

        static const struct {
            int                        d_lineNum; // source line number
            bdem_AggregateOption::AllocationStrategy d_option;
                                                  // value for pool option
            int                        d_ni;      // number of iterations
            CompareEnum                d_rbefore; // comparison result before
                                                  // append
            CompareEnum                d_rafter;  // comparison result after
                                                  // append
            CompareEnum                d_rfinal;  // comparison result after
                                                  // all iterations
        } DATA[] = {
           { L_, bdem_AggregateOption::BDEM_PASS_THROUGH,         10,
                 COMPARE_GT, COMPARE_EQ, COMPARE_EQ },
           { L_, bdem_AggregateOption::BDEM_WRITE_MANY,       10,
                 COMPARE_EQ, COMPARE_EQ, COMPARE_EQ },
           { L_, bdem_AggregateOption::BDEM_WRITE_ONCE, 10,
                 COMPARE_LE, COMPARE_LE, COMPARE_LE },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                        LINE    = DATA[ti].d_lineNum;
            const bdem_AggregateOption::AllocationStrategy
                                             OPTVAL  = DATA[ti].d_option;
            const int                        NI      = DATA[ti].d_ni;
            const CompareEnum                RBEFORE = DATA[ti].d_rbefore;
            const CompareEnum                RAFTER  = DATA[ti].d_rafter;
            const CompareEnum                RFINAL  = DATA[ti].d_rfinal;

            if (veryVerbose)
                cout << "bdem_List(" << optionToString(OPTVAL)
                     << ", alloc)" << endl;

            BEGIN_BSLMA_EXCEPTION_TEST {
                List L2(OPTVAL, Z);
                ASSERT (L2.length() == 0);
                L2.appendChar(' ');
                L2.appendCharArray(XK);
                L2.appendStringArray(XQ);
                ASSERT (L2.length() == 3);
            } END_BSLMA_EXCEPTION_TEST

            if (verbose) cout << "\t\tverify pool option "
                              << optionToString(OPTVAL) << endl;
            List L2(OPTVAL, Z);

            ASSERT (L2.length() == 0);

            // for some strange reason this does not work unless you go through
            // a full cycle first
            L2.appendCharArray(XK);
            L2.removeAll();
            L2.appendCharArray(XK);
            int NUM_BYTES = testAllocator.numBytesInUse();

            for (int i = 0; i < NI; ++i) {
                if (veryVerbose) {
                    int IN_USE = testAllocator.numBytesInUse();
                    cout << "\t\t"; P_(i); P(IN_USE);
                }
                L2.removeAll();

                LOOP_ASSERT(i,
                        compareValues(NUM_BYTES, testAllocator.numBytesInUse(),
                                      RBEFORE));
                L2.appendCharArray(XK);
                LOOP4_ASSERT(i, LINE, NUM_BYTES, testAllocator.numBytesInUse(),
                        compareValues(NUM_BYTES, testAllocator.numBytesInUse(),
                                      RAFTER));
            }
            ASSERT(compareValues(NUM_BYTES, testAllocator.numBytesInUse(),
                                 RFINAL));
        }

        {
            // better test of BDEM_WRITE_ONCE

            if (verbose) cout << "\t\tverify BDEM_WRITE_ONCE option" << endl;

            List L2(bdem_AggregateOption::BDEM_WRITE_ONCE, Z);

            ASSERT (L2.length() == 0);

            L2.appendCharArray(XK);
            int numBytes = testAllocator.numBytesInUse();

            for (int i = 0; i < 10; ++i) {
                if (veryVerbose) {
                    int IN_USE = testAllocator.numBytesInUse();
                    cout << "\t\t"; P_(i); P(IN_USE);
                }
                L2.removeAll();

                LOOP_ASSERT(i, testAllocator.numBytesInUse() == numBytes);

                numBytes = testAllocator.numBytesInUse();

                L2.appendCharArray(XK);

                LOOP_ASSERT(i, testAllocator.numBytesInUse() >= numBytes);

                numBytes = testAllocator.numBytesInUse();
            }
        }

        if (verbose) cout << "verify NO_HINT option" << endl;
        {
            List L2(bdem_AggregateOption::BDEM_PASS_THROUGH, Z);

            ASSERT (L2.length() == 0);
            L2.appendStringArray(UQ);
            L2.theModifiableStringArray(0).push_back(XG);   // "long" string
            ASSERT (L2.length() == 1);
            const int NUM_BYTES  = testAllocator.numBytesInUse();
            for (int i = 0; i < 10; ++i) {
                if (veryVerbose) {
                    int IN_USE = testAllocator.numBytesInUse();
                    cout << "\t\t"; P_(i); P(IN_USE);
                }
                L2.theModifiableStringArray(0).erase(
                                                 L2.theStringArray(0).begin());
                LOOP_ASSERT(i, L2.theStringArray(0).size() == 0);

                LOOP_ASSERT(i, NUM_BYTES >= testAllocator.numBytesInUse());
                L2.theModifiableStringArray(0).push_back(XG);  // "long" string
                LOOP_ASSERT(i, L2.theStringArray(0).size() == 1);
                LOOP_ASSERT(i, NUM_BYTES == testAllocator.numBytesInUse());
            }
        }

        my_ElemTypeArray elems(Z);
        elems.push_back(bdem_ElemType::BDEM_CHAR);
        elems.push_back(bdem_ElemType::BDEM_LIST);

        if (verbose) cout << "bdem_List(elems, alloc)" << endl;
        BEGIN_BSLMA_EXCEPTION_TEST {
            List L11(vectorData(elems), elems.size(), Z);

            L11.appendChar(' ');
            L11.appendStringArray(XQ);
            ASSERT(L11.length() == 4);
        } END_BSLMA_EXCEPTION_TEST

        if (verbose) cout << "verify BDEM_WRITE_MANY default" << endl;
        {
            List L2(vectorData(elems),
                    elems.size(),
                    Z);

            ASSERT (L2.length() == 2);

            // verify the BDEM_WRITE_MANY

            // cycle us first
            L2.appendCharArray(XK);
            L2.removeAll();
            L2.appendCharArray(XK);
            L2.removeAll();
            int numSmall = testAllocator.numBytesInUse();
            L2.appendCharArray(XK);
            int numBig   = testAllocator.numBytesInUse();
            ASSERT(numBig > numSmall);

            for (int i = 0; i < 10; ++i) {
                if (veryVerbose) {
                    int IN_USE = testAllocator.numBytesInUse();
                    cout << "\t\t"; P_(i); P(IN_USE);
                }
                L2.removeAll();

                LOOP_ASSERT(i, numSmall == testAllocator.numBytesInUse());
                L2.appendCharArray(XK);
                LOOP_ASSERT(i, numBig   == testAllocator.numBytesInUse());
            }

            L2.removeAll();
            ASSERT (L2.length() == 0);

            // verify the NO_HINT option
            L2.appendStringArray(UQ);
            ASSERT (L2.length() == 1);

            // cycle first
            L2.theModifiableStringArray(0).push_back(XG);   // "long" string
            L2.theModifiableStringArray(0).erase(L2.theStringArray(0).begin());
            L2.theModifiableStringArray(0).push_back(XG);   // "long" string
            L2.theModifiableStringArray(0).erase(L2.theStringArray(0).begin());

            numSmall = testAllocator.numBytesInUse();
            L2.theModifiableStringArray(0).push_back(XG);   // "long" string
            numBig   = testAllocator.numBytesInUse();
            ASSERT(numBig >= numSmall);

            for (int j = 0; j < 10; ++j) {
                if (veryVerbose) {
                    int IN_USE = testAllocator.numBytesInUse();
                    cout << "\t\t"; P_(j); P(IN_USE);
                }
                L2.theModifiableStringArray(0).erase(
                                                 L2.theStringArray(0).begin());
                LOOP_ASSERT(j, 0         == L2.theStringArray(0).size());
                LOOP_ASSERT(j, numSmall == testAllocator.numBytesInUse());
                // "long" string
                L2.theModifiableStringArray(0).push_back(XG);
                LOOP_ASSERT(j, 1         == L2.theStringArray(0).size());
                LOOP_ASSERT(j, numBig   == testAllocator.numBytesInUse());
            }
        }

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                        LINE    = DATA[ti].d_lineNum;
            const bdem_AggregateOption::AllocationStrategy
                                             OPTVAL  = DATA[ti].d_option;
            const int                        NI      = DATA[ti].d_ni;
            const CompareEnum                RBEFORE = DATA[ti].d_rbefore;
            const CompareEnum                RAFTER  = DATA[ti].d_rafter;
            const CompareEnum                RFINAL  = DATA[ti].d_rfinal;

            if (veryVerbose)
                cout << "bdem_List(elems, " << optionToString(OPTVAL)
                     << ", alloc)" << endl;

            BEGIN_BSLMA_EXCEPTION_TEST {
                List L3(vectorData(elems), elems.size(), OPTVAL, Z);
                L3.removeAll();
                ASSERT (L3.length() == 0);
                L3.appendChar(' ');
                L3.appendCharArray(XK);
                    L3.appendStringArray(XQ);
                    ASSERT (L3.length() == 3);
            } END_BSLMA_EXCEPTION_TEST

            if (verbose) cout << "\tverify pool option "
                              << optionToString(OPTVAL) << endl;
            List L2(vectorData(elems), elems.size(), OPTVAL, Z);

            L2.removeAll();

            ASSERT (L2.length() == 0);

            L2.appendCharArray(XK);
            int NUM_BYTES  = testAllocator.numBytesInUse();

            for (int i = 0; i < NI; ++i) {
                if (veryVerbose) {
                    int IN_USE = testAllocator.numBytesInUse();
                    cout << "\t\t"; P_(i); P(IN_USE);
                }
                L2.removeAll();
                ASSERT (L2.length() == 0);

                LOOP_ASSERT(i,
                        compareValues(NUM_BYTES, testAllocator.numBytesInUse(),
                                      RBEFORE));
                L2.appendCharArray(XK);
                ASSERT (L2.length() == 1);
                LOOP_ASSERT(i,
                        compareValues(NUM_BYTES, testAllocator.numBytesInUse(),
                                      RAFTER));
            }
            ASSERT(compareValues(NUM_BYTES, testAllocator.numBytesInUse(),
                                 RFINAL));
        }

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec;     // expect 'spec' string
            const char *d_types;    // input element types (encoded)
            int         d_length;   // expected length of returned list
        } TYPEDATA[] = {
            // line          spec        elemType  length
            // ----          ----        --------  ------
            {   L_,          "nA",       "A",      1,   },
            {   L_,          "nB",       "B",      1,   },
            {   L_,          "nC",       "C",      1,   },
            {   L_,          "nD",       "D",      1,   },
            {   L_,          "nE",       "E",      1,   },
            {   L_,          "nF",       "F",      1,   },
            {   L_,          "nG",       "G",      1,   },
            {   L_,          "nH",       "H",      1,   },
            {   L_,          "nI",       "I",      1,   },
            {   L_,          "nJ",       "J",      1,   },
            {   L_,          "nK",       "K",      1,   },
            {   L_,          "nL",       "L",      1,   },
            {   L_,          "nM",       "M",      1,   },
            {   L_,          "nN",       "N",      1,   },
            {   L_,          "nO",       "O",      1,   },
            {   L_,          "nP",       "P",      1,   },
            {   L_,          "nQ",       "Q",      1,   },
            {   L_,          "nR",       "R",      1,   },
            {   L_,          "nS",       "S",      1,   },
            {   L_,          "nT",       "T",      1,   },
            {   L_,          "nU",       "U",      1,   },
            {   L_,          "nV",       "V",      1,   },
            {   L_,          "nAnA",     "AA",     2,   },
            {   L_,          "nBnA",     "BA",     2,   },
            {   L_,          "nGnH",     "GH",     2,   },
            {   L_,          "nUnV",     "UV",     2,   },
            {   L_,          "nAnBnC",   "ABC",    3,   },
            {   L_,          "nAnBnCnD", "ABCD",   4,   },
        };

        {
            const int NUM_DATA = sizeof TYPEDATA / sizeof *TYPEDATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = TYPEDATA[ti].d_lineNum;
                const char *SPEC   = TYPEDATA[ti].d_spec;
                const char *TYPES  = TYPEDATA[ti].d_types;
                const int   LENGTH = TYPEDATA[ti].d_length;

                if (veryVerbose) { cout << "\n\t"; P_(ti); P(SPEC); }

                List control(&testAllocator);      gg(&control, SPEC);

                my_ElemTypeArray e;  appendToElemTypeArray(&e, TYPES);

                List L3(vectorData(e), e.size(),
                        bdem_AggregateOption::BDEM_WRITE_MANY, Z);
                L3.makeAllNull();

                LOOP_ASSERT(LINE, L3.length() == LENGTH);
                LOOP_ASSERT(LINE, L3 == control);
                if (veryVerbose) { cout << "\t"; P(L3); }
                if (veryVerbose) { cout << "\t"; P(control); }

                List L4(vectorData(e), e.size(), Z);
                L4.makeAllNull();
                if (veryVerbose) { cout << "\t"; P(L4); }
                LOOP_ASSERT(LINE, L4.length() == LENGTH);
                LOOP3_ASSERT(LINE, L4, control, L4 == control);
            }
        }

        if (verbose) cout << "\nCopy construct values." << endl;
        {
            static const char *SPECS[] = {
                "",      "xA",      "xBxC",     "xCxDxE",
                "xDxExAxB",   "xExAxBxCxD",
            0}; // Null string required as last element.

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC)/2;

                if (verbose) {
                    cout << "\tFor an object of length " << curLen << ":\t";
                    P(SPEC);
                }

                LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
                oldLen = curLen;

                // Create control object w.
                List mW(&testAllocator); gg(&mW, SPEC);
                const List& W = mW;
                LOOP_ASSERT(ti, curLen == W.length()); // same lengths
                if (veryVerbose) { cout << "\t"; P(W); }

                List mX(&testAllocator);
                const List &X = mX;              gg(&mX, SPEC);
                if (veryVerbose) { cout << "\t\t"; P(X); }

                {                                   // No allocator.  List
                    const List Y0(X);
                    if (veryVerbose) { cout << "\t\t\t"; P(Y0); }
                    LOOP_ASSERT(SPEC, W == Y0);
                    LOOP_ASSERT(SPEC, W == X);
                }

                {                                   // No allocator.  Row
                    const List Y1(X.row());
                    if (veryVerbose) { cout << "\t\t\t"; P(Y1); }
                    LOOP_ASSERT(SPEC, W == Y1);
                    LOOP_ASSERT(SPEC, W == X);
                }

                {                                   // Allocator.  List
                    const List Y2(X, Z);
                    if (veryVerbose) { cout << "\t\t\t"; P(Y2); }
                    LOOP_ASSERT(SPEC, W == Y2);
                    LOOP_ASSERT(SPEC, W == X);
                }

                {                                   // Allocator.  Row
                    const List Y3(X.row(), Z);
                    if (veryVerbose) { cout << "\t\t\t"; P(Y3); }
                    LOOP_ASSERT(SPEC, W == Y3);
                    LOOP_ASSERT(SPEC, W == X);
                }

                {                           // Allocator.  Option, List
                    const List Y4(X, bdem_AggregateOption::BDEM_WRITE_MANY,
                                     Z);
                    if (veryVerbose) { cout << "\t\t\t"; P(Y4); }
                    LOOP_ASSERT(SPEC, W == Y4);
                    LOOP_ASSERT(SPEC, W == X);
                }

                {                           // Allocator.  Option, List
                    const List Y5(X.row(),
                                  bdem_AggregateOption::BDEM_WRITE_MANY,
                                  Z);
                    if (veryVerbose) { cout << "\t\t\t"; P(Y5); }
                    LOOP_ASSERT(SPEC, W == Y5);
                    LOOP_ASSERT(SPEC, W == X);
                }

            }
        }

        List CC(Z);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int                        LINE    = DATA[ti].d_lineNum;
            const bdem_AggregateOption::AllocationStrategy
                                             OPTVAL  = DATA[ti].d_option;
            const int                        NI      = DATA[ti].d_ni;
            const CompareEnum                RBEFORE = DATA[ti].d_rbefore;
            const CompareEnum                RAFTER  = DATA[ti].d_rafter;
            const CompareEnum                RFINAL  = DATA[ti].d_rfinal;

            if (veryVerbose)
                cout << "bdem_List(List, " << optionToString(OPTVAL)
                     << ", " << ", alloc)" << endl;

            BEGIN_BSLMA_EXCEPTION_TEST {
                List L3(CC, OPTVAL, Z);
                L3.removeAll();
                ASSERT (L3.length() == 0);
                L3.appendChar(' ');
                L3.appendCharArray(XK);
                L3.appendStringArray(XQ);
                ASSERT (L3.length() == 3);
            } END_BSLMA_EXCEPTION_TEST

            if (verbose) cout << "\tverify pool option "
                              << optionToString(OPTVAL) << endl;
            List L2(CC, OPTVAL, Z);

            L2.removeAll();

            ASSERT (L2.length() == 0);

            L2.appendCharArray(XK);
            int NUM_BYTES = testAllocator.numBytesInUse();

            for (int i = 0; i < NI; ++i) {
                if (veryVerbose) {
                    int IN_USE = testAllocator.numBytesInUse();
                    cout << "\t\t"; P_(i); P(IN_USE);
                }
                L2.removeAll();

                LOOP_ASSERT(i,
                        compareValues(NUM_BYTES, testAllocator.numBytesInUse(),
                                      RBEFORE));
                L2.appendCharArray(XK);
                LOOP_ASSERT(i,
                        compareValues(NUM_BYTES, testAllocator.numBytesInUse(),
                                      RAFTER));
            }
            ASSERT(compareValues(NUM_BYTES, testAllocator.numBytesInUse(),
                                 RFINAL));
        }
      }

DEFINE_TEST_CASE(10) {
        // --------------------------------------------------------------------
        // TESTING 'bdem_Row' AND 'bdem_List' makeValueNull, ETC. MANIPULATORS
        // Concerns:
        //   a) 'makeValueNull', etc. must be tested for each of the 22 types.
        //   b) The function could operate differently with different internal
        //      representations.
        //
        // Plan:
        //   The 'bdem_List' methods forward to the 'bdem_Row' functions.
        //   Therefore, after verifying the 'bdem_Row' functions for a single
        //   value, the complete testing can be performed with the 'bdem_List'
        //   functions.
        //    a) Test with values from all 22 types.
        //    b) Verify exception neutrality.
        //
        // Testing:
        //   void bdem_Row::makeAllNull();
        //   void bdem_Row::makeValueNull(int index);
        //   void bdem_Row::makeRangeNull(int si, int ne);
        //
        //   void bdem_List::makeAllNull();
        //   void bdem_List::makeValueNull(int index);
        //   void bdem_List::makeRangeNull(int si, int ne);
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) {
            cout << endl
                 << "Testing bdem_Row and bdem_List 'makeValueNull' Manip."
                 << endl
                 << "======================================================"
                 << endl;
        }

        if (verbose) cout << "\tTesting 'bdem_Row' functions." << endl;
        {
            if (verbose) cout << "\t\tTesting 'bdem_Row::makeAllNull'."
                              << endl;

            List L(g("xAxBxC"), &testAllocator);
            List C(g("nAnBnC"), &testAllocator);
            ASSERT((L.row() == C.row()) == 0);

            L.row().makeAllNull();
            ASSERT((L.row() == C.row()) == 1);

            C.row().makeAllNull();
            ASSERT((L.row() == C.row()) == 1);
        }
        {
            if (verbose) cout << "\t\tTesting 'bdem_Row::makeValueNull'."
                              << endl;

            List L(g("xAxBxC"), &testAllocator);
            List C(g("xAnBxC"), &testAllocator);
            ASSERT ((L.row() == C.row()) == 0);

            L.row()[1].makeNull();
            ASSERT ((L.row() == C.row()) == 1);

            C.row()[1].makeNull();
            ASSERT ((L.row() == C.row()) == 1);
        }
        {
            if (verbose) cout << "\t\tTesting 'bdem_Row::makeRangeNull'."
                              << endl;

            List L(g("xAxBxC"), &testAllocator);
            List C(g("xAnBnC"), &testAllocator);
            ASSERT ((L.row() == C.row()) == 0);

            makeRangeNull(L.row(), 1, 2);
            ASSERT ((L.row() == C.row()) == 1);

            makeRangeNull(C.row(), 1, 2);
            ASSERT ((L.row() == C.row()) == 1);
        }

        if (verbose) cout << "Testing bdem_List functions." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec;     // source list
                int         d_si;       // index at which to test
                int         d_ne;       // number of elements to test
                const char *d_espec;    // expected result
            } DATA[] = {
                //line    d-list                      si   ne  e-list
                //----    ------                      --   --  ------ Depth = 0
                { L_,     "",                         0,   0,  ""            },

                //line    d-list                      si   ne  e-list
                //----    ------                      --   --  ------ Depth = 1
                { L_,     "xA",                       0,   0,  "xA"          },
                { L_,     "xA",                       0,   1,  "nA"          },
                { L_,     "xB",                       0,   1,  "nB"          },
                { L_,     "xC",                       0,   1,  "nC"          },
                { L_,     "xD",                       0,   1,  "nD"          },
                { L_,     "xE",                       0,   1,  "nE"          },
                { L_,     "xF",                       0,   1,  "nF"          },
                { L_,     "xG",                       0,   1,  "nG"          },
                { L_,     "xH",                       0,   1,  "nH"          },
                { L_,     "xI",                       0,   1,  "nI"          },
                { L_,     "xJ",                       0,   1,  "nJ"          },
                { L_,     "xK",                       0,   1,  "nK"          },
                { L_,     "xL",                       0,   1,  "nL"          },
                { L_,     "xM",                       0,   1,  "nM"          },
                { L_,     "xN",                       0,   1,  "nN"          },
                { L_,     "xO",                       0,   1,  "nO"          },
                { L_,     "xP",                       0,   1,  "nP"          },
                { L_,     "xQ",                       0,   1,  "nQ"          },
                { L_,     "xR",                       0,   1,  "nR"          },
                { L_,     "xS",                       0,   1,  "nS"          },
                { L_,     "xT",                       0,   1,  "nT"          },
                { L_,     "xU",                       0,   1,  "nU"          },
                { L_,     "xV",                       0,   1,  "nV"          },
                { L_,     "nA",                       0,   0,  "nA"          },
                { L_,     "nA",                       0,   1,  "nA"          },
                { L_,     "nB",                       0,   1,  "nB"          },
                { L_,     "nC",                       0,   1,  "nC"          },
                { L_,     "nD",                       0,   1,  "nD"          },
                { L_,     "nE",                       0,   1,  "nE"          },
                { L_,     "nF",                       0,   1,  "nF"          },
                { L_,     "nG",                       0,   1,  "nG"          },
                { L_,     "nH",                       0,   1,  "nH"          },
                { L_,     "nI",                       0,   1,  "nI"          },
                { L_,     "nJ",                       0,   1,  "nJ"          },
                { L_,     "nK",                       0,   1,  "nK"          },
                { L_,     "nL",                       0,   1,  "nL"          },
                { L_,     "nM",                       0,   1,  "nM"          },
                { L_,     "nN",                       0,   1,  "nN"          },
                { L_,     "nO",                       0,   1,  "nO"          },
                { L_,     "nP",                       0,   1,  "nP"          },
                { L_,     "nQ",                       0,   1,  "nQ"          },
                { L_,     "nR",                       0,   1,  "nR"          },
                { L_,     "nS",                       0,   1,  "nS"          },
                { L_,     "nT",                       0,   1,  "nT"          },
                { L_,     "nU",                       0,   1,  "nU"          },
                { L_,     "nV",                       0,   1,  "nV"          },

                //line    d-list                      si   ne  e-list
                //----    ------                      --   --  ------ Depth = 2
                { L_,     "xAxA",                     0,   2,  "nAnA"        },
                { L_,     "xAxB",                     0,   2,  "nAnB"        },
                { L_,     "xAxC",                     0,   2,  "nAnC"        },
                { L_,     "xAxD",                     0,   2,  "nAnD"        },
                { L_,     "xAxE",                     0,   2,  "nAnE"        },
                { L_,     "xAxF",                     0,   2,  "nAnF"        },
                { L_,     "xAxG",                     0,   2,  "nAnG"        },
                { L_,     "xAxH",                     0,   2,  "nAnH"        },
                { L_,     "xAxI",                     0,   2,  "nAnI"        },
                { L_,     "xAxJ",                     0,   2,  "nAnJ"        },
                { L_,     "xAxK",                     0,   2,  "nAnK"        },
                { L_,     "xAxL",                     0,   2,  "nAnL"        },
                { L_,     "xAxM",                     0,   2,  "nAnM"        },
                { L_,     "xAxN",                     0,   2,  "nAnN"        },
                { L_,     "xAxO",                     0,   2,  "nAnO"        },
                { L_,     "xAxP",                     0,   2,  "nAnP"        },
                { L_,     "xAxQ",                     0,   2,  "nAnQ"        },
                { L_,     "xAxR",                     0,   2,  "nAnR"        },
                { L_,     "xAxS",                     0,   2,  "nAnS"        },
                { L_,     "xAxT",                     0,   2,  "nAnT"        },
                { L_,     "xAxU",                     0,   2,  "nAnU"        },
                { L_,     "xAxV",                     0,   2,  "nAnV"        },

                { L_,     "xAxA",                     0,   0,  "xAxA"        },
                { L_,     "xAxA",                     0,   1,  "nAxA"        },

                { L_,     "xAxA",                     1,   1,  "xAnA"        },
                { L_,     "xAxB",                     1,   1,  "xAnB"        },
                { L_,     "xAxC",                     1,   1,  "xAnC"        },
                { L_,     "xAxD",                     1,   1,  "xAnD"        },
                { L_,     "xAxE",                     1,   1,  "xAnE"        },
                { L_,     "xAxF",                     1,   1,  "xAnF"        },
                { L_,     "xAxG",                     1,   1,  "xAnG"        },
                { L_,     "xAxH",                     1,   1,  "xAnH"        },
                { L_,     "xAxI",                     1,   1,  "xAnI"        },
                { L_,     "xAxJ",                     1,   1,  "xAnJ"        },
                { L_,     "xAxK",                     1,   1,  "xAnK"        },
                { L_,     "xAxL",                     1,   1,  "xAnL"        },
                { L_,     "xAxM",                     1,   1,  "xAnM"        },
                { L_,     "xAxN",                     1,   1,  "xAnN"        },
                { L_,     "xAxO",                     1,   1,  "xAnO"        },
                { L_,     "xAxP",                     1,   1,  "xAnP"        },
                { L_,     "xAxQ",                     1,   1,  "xAnQ"        },
                { L_,     "xAxR",                     1,   1,  "xAnR"        },
                { L_,     "xAxS",                     1,   1,  "xAnS"        },
                { L_,     "xAxT",                     1,   1,  "xAnT"        },
                { L_,     "xAxU",                     1,   1,  "xAnU"        },
                { L_,     "xAxV",                     1,   1,  "xAnV"        },

                { L_,     "xAxBxC",                   0,   0,  "xAxBxC"      },
                { L_,     "xAxBxC",                   0,   1,  "nAxBxC"      },
                { L_,     "xAxBxC",                   0,   2,  "nAnBxC"      },
                { L_,     "xAxBxC",                   0,   3,  "nAnBnC"      },
                { L_,     "xAxBxC",                   1,   0,  "xAxBxC"      },
                { L_,     "xAxBxC",                   1,   1,  "xAnBxC"      },
                { L_,     "xAxBxC",                   1,   2,  "xAnBnC"      },
                { L_,     "xAxBxC",                   2,   0,  "xAxBxC"      },
                { L_,     "xAxBxC",                   2,   1,  "xAxBnC"      },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *SPEC   = DATA[ti].d_spec;
                const int   SI     = DATA[ti].d_si;
                const int   NE     = DATA[ti].d_ne;
                const char *E_SPEC = DATA[ti].d_espec;

                const int curLen = (int)strlen(SPEC);
                if (curLen != oldLen) {
                    if (verbose) cout << "\tUsing objects of length "
                                      << (curLen/2) << '.' << endl;
                    LOOP_ASSERT(SPEC, oldLen <= curLen); // non-decreasing
                    oldLen = curLen;
                }

                List EE(g(E_SPEC));              // control for expected value

                const int NUM_PERMUTATIONS = 4;
                for (int perm = 0; perm < NUM_PERMUTATIONS; ++perm) {
                  bslma_TestAllocator ta;
                  bslma_TestAllocator& testAllocator = ta;

                  BEGIN_BSLMA_EXCEPTION_TEST {
                    List mU(&testAllocator);
                    makePermutation(&mU, SPEC, perm%2, perm/2);

                    if (veryVerbose) {
                        P_(LINE) P_(perm); P_(SPEC); P_(E_SPEC); P_(SI); P(NE);
                        P(mU);
                    }

                    makeRangeNull(mU.row(),SI, NE);

                    LOOP3_ASSERT(LINE, mU, EE, mU == EE);
                  } END_BSLMA_EXCEPTION_TEST
                }

                if ((int)(strlen(SPEC) / 2) == NE && 0 == SI) {
                    for (int perm = 0; perm < NUM_PERMUTATIONS; ++perm) {
                        bslma_TestAllocator ta;
                        bslma_TestAllocator& testAllocator = ta;

                      BEGIN_BSLMA_EXCEPTION_TEST {
                        List mU(&testAllocator);
                        makePermutation(&mU, SPEC, perm%2, perm/2);

                        if (veryVerbose) {
                            P_(perm); P_(SPEC); P_(E_SPEC); P_(SI); P(NE);
                            P_(LINE) P(mU);
                        }

                        mU.makeAllNull();

                        LOOP3_ASSERT(LINE, mU, EE, mU == EE);
                      } END_BSLMA_EXCEPTION_TEST
                    }
                }

                if (NE == 1) {
                    for (int perm = 0; perm < NUM_PERMUTATIONS; ++perm) {
                        bslma_TestAllocator ta;
                        bslma_TestAllocator& testAllocator = ta;

                      BEGIN_BSLMA_EXCEPTION_TEST {
                        List mU(&testAllocator);
                        makePermutation(&mU, SPEC, perm%2, perm/2);

                        if (veryVerbose) {
                            P_(LINE) P_(perm); P_(SPEC); P_(E_SPEC); P_(SI);
                            P(NE); P(mU);
                        }

                        mU[SI].makeNull();

                        LOOP3_ASSERT(LINE, mU, EE, mU == EE);
                      } END_BSLMA_EXCEPTION_TEST
                    }
                }
            }
        }
      }

DEFINE_TEST_CASE(9) {
        // --------------------------------------------------------------------
        // TESTING 'bdem_Row' and 'bdem_List' ISNULL, ETC. ACCESSORS
        // Concerns:
        //   a) The 'isnull' family of functions must be tested for each of the
        //      22 types.
        //   b) The functions could operate differently with different internal
        //      representations.
        //   A Specific concern is with Lists containing aggregates.
        //   The standard test values do not have all the permutations.
        //
        // Plan:
        //   * Test with values from all 22 types.
        //   * Test with different internal representations.
        //   * Fully enumerate Lists of length 0 and 1.
        //   * For Lists of length 2, fully enumerate the type at index 1.
        //   * For Lists of length 3
        //     These representative types are chosen to exercise the variants
        //     of the proctor templates defined in the '.cpp' more
        //     thoroughly.
        //
        // Testing:
        //   int bdem_Row::isAnyNonNull() const;
        //   int bdem_Row::isAnyNull() const;
        //   int bdem_Row::isNonNull(int index) const;
        //   int bdem_Row::operator[](int index) const;
        //
        //   int bdem_List::isAnyNonNull() const;
        //   int bdem_List::isAnyNull() const;
        //   int bdem_List::operator[](int index) const;
        //
        //   int bdem_Elemref::isNull(int index) const;
        //   int bdem_Elemref::isNonNull(int index) const;
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl
                          << "Testing bdem_List IsSet Accessors" << endl
                          << "=================================" << endl;

        {
            static const struct {
                int         d_lineNum;    // source line number
                const char *d_spec;       // list
                int         d_si;         // index at which to test
                int         d_isNonNull;  // value for 'isNonNull' result
                int         d_isAnyNonNull;
                int         d_isAnyNull;
            } DATA[] = {
                //line  d-list             si   isnn isann isan
                //----  ------             --   ---- ----- ----
                { L_,     "",              -1,  0,   0,    0  },
                { L_,     "xA",            0,   1,   1,    0  },
                { L_,     "xA",            0,   1,   1,    0  },
                { L_,     "xB",            0,   1,   1,    0  },

                { L_,     "xC",            0,   1,   1,    0  },
                { L_,     "xD",            0,   1,   1,    0  },
                { L_,     "xE",            0,   1,   1,    0  },
                { L_,     "xF",            0,   1,   1,    0  },

                { L_,     "xG",            0,   1,   1,    0  },
                { L_,     "xG",            0,   1,   1,    0  },
                { L_,     "xH",            0,   1,   1,    0  },
                { L_,     "xH",            0,   1,   1,    0  },

                { L_,     "xI",            0,   1,   1,    0  },
                { L_,     "xJ",            0,   1,   1,    0  },
                { L_,     "xK",            0,   1,   1,    0  },
                { L_,     "xK",            0,   1,   1,    0  },

                { L_,     "xL",            0,   1,   1,    0  },
                { L_,     "xM",            0,   1,   1,    0  },
                { L_,     "xN",            0,   1,   1,    0  },
                { L_,     "xO",            0,   1,   1,    0  },

                { L_,     "xP",            0,   1,   1,    0  },
                { L_,     "xQ",            0,   1,   1,    0  },
                { L_,     "xR",            0,   1,   1,    0  },
                { L_,     "xS",            0,   1,   1,    0  },

                { L_,     "xT",            0,   1,   1,    0  },
                { L_,     "xU",            0,   1,   1,    0  },
                { L_,     "xU",            0,   1,   1,    0  },
                { L_,     "xV",            0,   1,   1,    0  },

                { L_,     "xV",            0,   1,   1,    0  },
                { L_,     "uA",            0,   0,   0,    1  },
                { L_,     "uA",            0,   0,   0,    1  },
                { L_,     "uB",            0,   0,   0,    1  },

                { L_,     "uC",            0,   0,   0,    1  },
                { L_,     "uD",            0,   0,   0,    1  },
                { L_,     "uE",            0,   0,   0,    1  },
                { L_,     "uF",            0,   0,   0,    1  },

                { L_,     "uG",            0,   0,   0,    1  },
                { L_,     "uG",            0,   0,   0,    1  },
                { L_,     "uH",            0,   0,   0,    1  },
                { L_,     "uH",            0,   0,   0,    1  },

                { L_,     "uI",            0,   0,   0,    1  },
                { L_,     "uJ",            0,   0,   0,    1  },
                { L_,     "uK",            0,   0,   0,    1  },
                { L_,     "uK",            0,   0,   0,    1  },

                { L_,     "uL",            0,   0,   0,    1  },
                { L_,     "uM",            0,   0,   0,    1  },
                { L_,     "uN",            0,   0,   0,    1  },
                { L_,     "uO",            0,   0,   0,    1  },

                { L_,     "uP",            0,   0,   0,    1  },
                { L_,     "uQ",            0,   0,   0,    1  },
                { L_,     "uR",            0,   0,   0,    1  },
                { L_,     "uS",            0,   0,   0,    1  },

                { L_,     "uT",            0,   0,   0,    1  },
                { L_,     "uU",            0,   0,   0,    1  },
                { L_,     "uU",            0,   0,   0,    1  },
                { L_,     "uV",            0,   0,   0,    1  },

                { L_,     "uV",            0,   0,   0,    1  },
                { L_,     "xAxA",          0,   1,   1,    0  },
                { L_,     "xAxA",          0,   1,   1,    0  },
                { L_,     "xAxA",          1,   1,   1,    0  },

                { L_,     "xAxB",          0,   1,   1,    0  },
                { L_,     "xAxB",          0,   1,   1,    0  },
                { L_,     "xAxB",          1,   1,   1,    0  },
                { L_,     "xAxC",          0,   1,   1,    0  },

                { L_,     "xAxC",          0,   1,   1,    0  },
                { L_,     "xAxC",          1,   1,   1,    0  },
                { L_,     "xAxD",          0,   1,   1,    0  },
                { L_,     "xAxD",          0,   1,   1,    0  },

                { L_,     "xAxD",          1,   1,   1,    0  },
                { L_,     "xAxE",          0,   1,   1,    0  },
                { L_,     "xAxE",          0,   1,   1,    0  },
                { L_,     "xAxE",          1,   1,   1,    0  },

                { L_,     "xAxF",          0,   1,   1,    0  },
                { L_,     "xAxF",          0,   1,   1,    0  },
                { L_,     "xAxF",          1,   1,   1,    0  },
                { L_,     "xAxG",          0,   1,   1,    0  },

                { L_,     "xAxG",          0,   1,   1,    0  },
                { L_,     "xAxG",          1,   1,   1,    0  },
                { L_,     "xAxH",          0,   1,   1,    0  },
                { L_,     "xAxH",          0,   1,   1,    0  },

                { L_,     "xAxH",          1,   1,   1,    0  },
                { L_,     "xAxI",          0,   1,   1,    0  },
                { L_,     "xAxI",          0,   1,   1,    0  },
                { L_,     "xAxI",          1,   1,   1,    0  },

                { L_,     "xAxJ",          0,   1,   1,    0  },
                { L_,     "xAxJ",          0,   1,   1,    0  },
                { L_,     "xAxJ",          1,   1,   1,    0  },
                { L_,     "xAxK",          0,   1,   1,    0  },

                { L_,     "xAxK",          0,   1,   1,    0  },
                { L_,     "xAxK",          1,   1,   1,    0  },
                { L_,     "xAxL",          0,   1,   1,    0  },
                { L_,     "xAxL",          0,   1,   1,    0  },

                { L_,     "xAxL",          1,   1,   1,    0  },
                { L_,     "xAxM",          0,   1,   1,    0  },
                { L_,     "xAxM",          0,   1,   1,    0  },
                { L_,     "xAxM",          1,   1,   1,    0  },

                { L_,     "xAxN",          0,   1,   1,    0  },
                { L_,     "xAxN",          0,   1,   1,    0  },
                { L_,     "xAxN",          1,   1,   1,    0  },
                { L_,     "xAxO",          0,   1,   1,    0  },

                { L_,     "xAxO",          0,   1,   1,    0  },
                { L_,     "xAxO",          1,   1,   1,    0  },
                { L_,     "xAxP",          0,   1,   1,    0  },
                { L_,     "xAxP",          0,   1,   1,    0  },

                { L_,     "xAxP",          1,   1,   1,    0  },
                { L_,     "xAxQ",          0,   1,   1,    0  },
                { L_,     "xAxQ",          0,   1,   1,    0  },
                { L_,     "xAxQ",          1,   1,   1,    0  },

                { L_,     "xAxR",          0,   1,   1,    0  },
                { L_,     "xAxR",          0,   1,   1,    0  },
                { L_,     "xAxR",          1,   1,   1,    0  },
                { L_,     "xAxS",          0,   1,   1,    0  },

                { L_,     "xAxS",          0,   1,   1,    0  },
                { L_,     "xAxS",          1,   1,   1,    0  },
                { L_,     "xAxT",          0,   1,   1,    0  },
                { L_,     "xAxT",          0,   1,   1,    0  },

                { L_,     "xAxT",          1,   1,   1,    0  },
                { L_,     "xAxU",          0,   1,   1,    0  },
                { L_,     "xAxU",          0,   1,   1,    0  },
                { L_,     "xAxU",          1,   1,   1,    0  },

                { L_,     "xAxV",          0,   1,   1,    0  },
                { L_,     "xAxV",          0,   1,   1,    0  },
                { L_,     "xAxV",          1,   1,   1,    0  },
                { L_,     "xAuA",          0,   1,   1,    1  },

                { L_,     "xAuA",          0,   1,   1,    1  },
                { L_,     "xAuA",          1,   0,   1,    1  },
                { L_,     "xAuB",          0,   1,   1,    1  },
                { L_,     "xAuB",          0,   1,   1,    1  },

                { L_,     "xAuB",          1,   0,   1,    1  },
                { L_,     "xAuC",          0,   1,   1,    1  },
                { L_,     "xAuC",          0,   1,   1,    1  },
                { L_,     "xAuC",          1,   0,   1,    1  },

                { L_,     "xAuD",          0,   1,   1,    1  },
                { L_,     "xAuD",          0,   1,   1,    1  },
                { L_,     "xAuD",          1,   0,   1,    1  },
                { L_,     "xAuE",          0,   1,   1,    1  },

                { L_,     "xAuE",          0,   1,   1,    1  },
                { L_,     "xAuE",          1,   0,   1,    1  },
                { L_,     "xAuF",          0,   1,   1,    1  },
                { L_,     "xAuF",          0,   1,   1,    1  },

                { L_,     "xAuF",          1,   0,   1,    1  },
                { L_,     "xAuG",          0,   1,   1,    1  },
                { L_,     "xAuG",          0,   1,   1,    1  },
                { L_,     "xAuG",          1,   0,   1,    1  },

                { L_,     "xAuH",          0,   1,   1,    1  },
                { L_,     "xAuH",          0,   1,   1,    1  },
                { L_,     "xAuH",          1,   0,   1,    1  },
                { L_,     "xAuI",          0,   1,   1,    1  },

                { L_,     "xAuI",          0,   1,   1,    1  },
                { L_,     "xAuI",          1,   0,   1,    1  },
                { L_,     "xAuJ",          0,   1,   1,    1  },
                { L_,     "xAuJ",          0,   1,   1,    1  },

                { L_,     "xAuJ",          1,   0,   1,    1  },
                { L_,     "xAuK",          0,   1,   1,    1  },
                { L_,     "xAuK",          0,   1,   1,    1  },
                { L_,     "xAuK",          1,   0,   1,    1  },

                { L_,     "xAuL",          0,   1,   1,    1  },
                { L_,     "xAuL",          0,   1,   1,    1  },
                { L_,     "xAuL",          1,   0,   1,    1  },
                { L_,     "xAuM",          0,   1,   1,    1  },

                { L_,     "xAuM",          0,   1,   1,    1  },
                { L_,     "xAuM",          1,   0,   1,    1  },
                { L_,     "xAuN",          0,   1,   1,    1  },
                { L_,     "xAuN",          0,   1,   1,    1  },

                { L_,     "xAuN",          1,   0,   1,    1  },
                { L_,     "xAuO",          0,   1,   1,    1  },
                { L_,     "xAuO",          0,   1,   1,    1  },
                { L_,     "xAuO",          1,   0,   1,    1  },

                { L_,     "xAuP",          0,   1,   1,    1  },
                { L_,     "xAuP",          0,   1,   1,    1  },
                { L_,     "xAuP",          1,   0,   1,    1  },
                { L_,     "xAuQ",          0,   1,   1,    1  },

                { L_,     "xAuQ",          0,   1,   1,    1  },
                { L_,     "xAuQ",          1,   0,   1,    1  },
                { L_,     "xAuR",          0,   1,   1,    1  },
                { L_,     "xAuR",          0,   1,   1,    1  },

                { L_,     "xAuR",          1,   0,   1,    1  },
                { L_,     "xAuS",          0,   1,   1,    1  },
                { L_,     "xAuS",          0,   1,   1,    1  },
                { L_,     "xAuS",          1,   0,   1,    1  },

                { L_,     "xAuT",          0,   1,   1,    1  },
                { L_,     "xAuT",          0,   1,   1,    1  },
                { L_,     "xAuT",          1,   0,   1,    1  },
                { L_,     "xAuU",          0,   1,   1,    1  },

                { L_,     "xAuU",          0,   1,   1,    1  },
                { L_,     "xAuU",          1,   0,   1,    1  },
                { L_,     "xAuV",          0,   1,   1,    1  },
                { L_,     "xAuV",          0,   1,   1,    1  },

                { L_,     "xAuV",          1,   0,   1,    1  },
                { L_,     "xAxKxU",        0,   1,   1,    0  },
                { L_,     "xGxAxH",        0,   1,   1,    0  },
                { L_,     "xHxUxA",        0,   1,   1,    0  },

                { L_,     "xKxGxV",        0,   1,   1,    0  },
                { L_,     "xUxVxK",        1,   1,   1,    0  },
                { L_,     "xVxHxG",        1,   1,   1,    0  },
                { L_,     "xAxAxA",        1,   1,   1,    0  },

                { L_,     "xGxKxU",        2,   1,   1,    0  },
                { L_,     "xHxGxH",        2,   1,   1,    0  },
                { L_,     "uKxUxG",        0,   0,   1,    1  },
                { L_,     "uUxHxV",        0,   0,   1,    1  },

                { L_,     "uVxVxK",        0,   0,   1,    1  },
                { L_,     "uAxKxH",        0,   0,   1,    1  },
                { L_,     "uGxAxA",        1,   1,   1,    1  },
                { L_,     "uHxUxU",        1,   1,   1,    1  },

                { L_,     "uKxGxK",        1,   1,   1,    1  },
                { L_,     "uUxVxG",        2,   1,   1,    1  },
                { L_,     "uVxHxV",        2,   1,   1,    1  },
                { L_,     "xAuAxU",        0,   1,   1,    1  },

                { L_,     "xGuKxH",        0,   1,   1,    1  },
                { L_,     "xHuGxA",        0,   1,   1,    1  },
                { L_,     "xKuUxV",        0,   1,   1,    1  },
                { L_,     "xUuHxK",        1,   0,   1,    1  },

                { L_,     "xVuVxG",        1,   0,   1,    1  },
                { L_,     "xAuKxA",        1,   0,   1,    1  },
                { L_,     "xGuAxU",        2,   1,   1,    1  },
                { L_,     "xHuUxH",        2,   1,   1,    1  },

                { L_,     "uKuGxG",        0,   0,   1,    1  },
                { L_,     "uUuVxV",        0,   0,   1,    1  },
                { L_,     "uVuHxK",        0,   0,   1,    1  },
                { L_,     "uAuAxH",        0,   0,   1,    1  },

                { L_,     "uGuKxA",        1,   0,   1,    1  },
                { L_,     "uHuGxU",        1,   0,   1,    1  },
                { L_,     "uKuUxK",        1,   0,   1,    1  },
                { L_,     "uUuHxG",        2,   1,   1,    1  },

                { L_,     "uVuVxV",        2,   1,   1,    1  },
                { L_,     "xAxKuU",        0,   1,   1,    1  },
                { L_,     "xGxAuH",        0,   1,   1,    1  },
                { L_,     "xHxUuA",        0,   1,   1,    1  },

                { L_,     "xKxGuV",        0,   1,   1,    1  },
                { L_,     "xUxVuK",        1,   1,   1,    1  },
                { L_,     "xVxHuG",        1,   1,   1,    1  },
                { L_,     "xAxAuA",        1,   1,   1,    1  },

                { L_,     "xGxKuU",        2,   0,   1,    1  },
                { L_,     "xHxGuH",        2,   0,   1,    1  },
                { L_,     "uKxUuG",        0,   0,   1,    1  },
                { L_,     "uUxHuV",        0,   0,   1,    1  },

                { L_,     "uVxVuK",        0,   0,   1,    1  },
                { L_,     "uAxKuH",        0,   0,   1,    1  },
                { L_,     "uGxAuA",        1,   1,   1,    1  },
                { L_,     "uHxUuU",        1,   1,   1,    1  },

                { L_,     "uKxGuK",        1,   1,   1,    1  },
                { L_,     "uUxVuG",        2,   0,   1,    1  },
                { L_,     "uVxHuV",        2,   0,   1,    1  },
                { L_,     "xAuAuU",        0,   1,   1,    1  },

                { L_,     "xGuKuH",        0,   1,   1,    1  },
                { L_,     "xHuGuA",        0,   1,   1,    1  },
                { L_,     "xKuUuV",        0,   1,   1,    1  },
                { L_,     "xUuHuK",        1,   0,   1,    1  },

                { L_,     "xVuVuG",        1,   0,   1,    1  },
                { L_,     "xAuKuA",        1,   0,   1,    1  },
                { L_,     "xGuAuU",        2,   0,   1,    1  },
                { L_,     "xHuUuH",        2,   0,   1,    1  },

                { L_,     "uKuGuG",        0,   0,   0,    1  },
                { L_,     "uUuVuV",        0,   0,   0,    1  },
                { L_,     "uVuHuK",        0,   0,   0,    1  },
                { L_,     "uAuAuH",        0,   0,   0,    1  },

                { L_,     "uGuKuA",        1,   0,   0,    1  },
                { L_,     "uHuGuU",        1,   0,   0,    1  },
                { L_,     "uKuUuK",        1,   0,   0,    1  },
                { L_,     "uUuHuG",        2,   0,   0,    1  },

                { L_,     "uVuVuV",        2,   0,   0,    1  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE         = DATA[ti].d_lineNum;
                const char *SPEC         = DATA[ti].d_spec;
                const int   SI           = DATA[ti].d_si;
                const int   ISNONNULL    = DATA[ti].d_isNonNull;
                const int   ISANYNONNULL = DATA[ti].d_isAnyNonNull;
                const int   ISANYNULL    = DATA[ti].d_isAnyNull;

                const int curLen = (int)strlen(SPEC);
                if (curLen != oldLen) {
                    if (verbose) cout << "\tUsing objects of length "
                                      << (curLen/2) << '.' << endl;
                    LOOP_ASSERT(SPEC, oldLen < curLen); // non-decreasing
                    oldLen = curLen;
                }

                List mU(&testAllocator);    const List& U = mU;
                gg(&mU, SPEC);

                // convert unsets to nulls
                for (int i = 0; i < U.length(); ++i) {
                    if (isUnset(U[i])) {
                        mU[i].makeNull();
                    }
                }

                Row& mR = mU.row();         const Row& R = U.row();

                if (veryVerbose) { P_(LINE); P_(SPEC); P_(SI); P(U); }

                if (SI >= 0) {
                    LOOP_ASSERT(LINE,   ISNONNULL ==  R[SI].isNonNull());
                    LOOP_ASSERT(LINE, ! ISNONNULL ==  R[SI].isNull());
                    LOOP_ASSERT(LINE,   ISNONNULL == mR[SI].isNonNull());
                    LOOP_ASSERT(LINE, ! ISNONNULL == mR[SI].isNull());

                    LOOP_ASSERT(LINE,    ISNONNULL ==  U[SI].isNonNull());
                    LOOP_ASSERT(LINE,  ! ISNONNULL ==  U[SI].isNull());
                    LOOP3_ASSERT(LINE,   ISNONNULL,   mU[SI].isNonNull(),
                                         ISNONNULL == mU[SI].isNonNull());
                    LOOP3_ASSERT(LINE, ! ISNONNULL,   mU[SI].isNull(),
                                       ! ISNONNULL == mU[SI].isNull());
                }

                LOOP_ASSERT(LINE,  ISANYNONNULL ==  R.isAnyNonNull());
                LOOP_ASSERT(LINE,  ISANYNONNULL == mR.isAnyNonNull());
                LOOP_ASSERT(LINE,  ISANYNONNULL ==  U.isAnyNonNull());
                LOOP3_ASSERT(LINE, ISANYNONNULL,   mU.isAnyNonNull(),
                                   ISANYNONNULL == mU.isAnyNonNull());

                LOOP_ASSERT(LINE,  ISANYNULL    ==  R.isAnyNull());
                LOOP_ASSERT(LINE,  ISANYNULL    == mR.isAnyNull());
                LOOP_ASSERT(LINE,  ISANYNULL    ==  U.isAnyNull());
                LOOP3_ASSERT(LINE, ISANYNULL,      mU.isAnyNull(),
                                   ISANYNULL    == mU.isAnyNull());
            }

            // Specific test for bdem_List
            {
                List mU(&testAllocator);
                mU.appendNullList();

                ASSERT(mU.theList(0).length() == 0);
                ASSERT(mU[0].isNonNull() == 0);

                const Row& R = mU.theList(0).row();
                ASSERT(R.isAnyNonNull() == 0);
                ASSERT(R.isAnyNull()    == 0);

                mU.theModifiableList(0).appendNullChar();

                ASSERT(mU.theList(0).length() == 1);
                ASSERT(mU[0].isNonNull() != 0);

                const Row& R2 = mU.theModifiableList(0).row();
                ASSERT(R2.isAnyNonNull() == 0);
                ASSERT(R2.isAnyNull()    == 1);

                ASSERT(R2[0].isNonNull() == 0);
                ASSERT(R2[0].isNull()    == 1);
            }

            // Specific test for bdem_Table
            {
                List mU(&testAllocator);
                mU.appendNullTable();

                ASSERT(!mU[0].isNonNull());

                // test with a single row with no columns

                mU.theModifiableTable(0).appendNullRow();

                ASSERT(1 == mU.theTable(0).numRows());
                ASSERT(0 == mU.theTable(0).numColumns());
                ASSERT(mU[0].isNonNull());

                mU.theModifiableTable(0).removeAll();

                ASSERT(0 == mU.theTable(0).numRows());
                ASSERT(0 == mU.theTable(0).numColumns());
                ASSERT(mU[0].isNonNull());

                // test with no rows and one column

                my_ElemTypeArray elems(Z);
                elems.push_back(bdem_ElemType::BDEM_CHAR);

                mU.theModifiableTable(0).reset(vectorData(elems),
                                                                 elems.size());
                ASSERT(0 == mU.theTable(0).numRows());
                ASSERT(1 == mU.theTable(0).numColumns());
                ASSERT(mU[0].isNonNull());
            }
        }
      }

DEFINE_TEST_CASE(8) {
        // --------------------------------------------------------------------
        // TESTING REVERSELIST, REVERSESPEC HELPER FUNCTIONS
        // Plan:
        //   Verify the operation of the reverse helper functions.
        //   The 'reverseList' function uses the 'swap' function.
        //
        // Testing
        //   void reverseList(bdem_List *list);
        //   void reverseSpec(char *spec);
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << "\nTesting 'reverseSpec' and "
                             "'reverseList' helper functions:" << endl;

        static const struct {
            int         d_lineNum;   // source line number
            const char *d_spec;      // input 'spec' string for 'gg'
            const char *d_espec;     // expected reversed spec.
        } DATA[] = {
            // line    spec              expected reversed
            // ----    ----              -----------------
            {   L_,    "",               "",                          },
            {   L_,    "xA",             "xA",                        },
            {   L_,    "xAyB",           "yBxA",                      },
            {   L_,    "xAyBuC",         "uCyBxA",                    },
            {   L_,    "xAyBuCxF",       "xFuCyBxA",                  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int REV_SPEC_SIZE = 10;
        char testSpec[REV_SPEC_SIZE];
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE   = DATA[ti].d_lineNum;
            const char *O_SPEC = DATA[ti].d_spec;   // Original spec
            const char *R_SPEC = DATA[ti].d_espec;  // Reversed spec

            strcpy(testSpec, O_SPEC);

            // reverse the test spec string
            reverseSpec(testSpec);
            LOOP_ASSERT(LINE, strcmp(testSpec, R_SPEC) == 0);

            // reverse the list
            List mL(Z);
            gg(&mL, O_SPEC);
            reverseList(&mL);

            List mR(Z);
            gg(&mR, R_SPEC);

            LOOP_ASSERT(LINE, (mL == mR) == 1);
        }
      }

DEFINE_TEST_CASE(7) {
        // --------------------------------------------------------------------
        // TESTING 'bdem_List' SWAP METHOD
        // Concerns:
        //   For an object of any length, 'swap' must exchange the values
        //   at any valid pair of index positions while leaving all other
        //   elements unaffected.  For any two types t1 and t2 and values
        //   v1 and v2 of those respective types, 'swap' must exchange
        //   v1 and v2.
        //
        // Plan:
        //   For each object X in a set of objects ordered by increasing length
        //   L containing uniform values, V0:
        //     For each valid index position P1 in [0 .. L-1]:
        //       For each valid index position P2 in [0 .. L-1]:
        //         1.  Create a test object T from X using the copy ctor.
        //         2.  Replace the element at P1 with V1 and at P2 with V2.
        //         3.  Swap these elements in T.
        //         4.  Verify that
        //               (i)     V2 == T[P1]     always
        //         5.  Verify that
        //               (ii)    V1 == T[P2]     if (P1 != P2)
        //                       V2 == T[P2]     if (P1 == P2)
        //         6.  For each index position, i, in [0 .. L-1] verify that:
        //               (iii)   V0 == T[i]      if (P1 != i && P2 != i)
        //
        // Testing:
        //   void swapElements(int index1, int index2);
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl
                          << "Testing bdem_List 'swap' Method" << endl
                          << "===============================" << endl;

        if (verbose) cout << "\nTesting swapElements(index1, index2)" << endl;

        const char *SPECS[]  = {
            "xA", "xAyA", "xAxB", "xAxU", "xUxA", "xAxBxC", "xGxHxI",
            "xAxBxCxD", "xAxDxCxBxE", "xAxBxCxDxExF",
            "xMxNxOxPxQxRxSxTxUxVxAxBxCxDxExFxGxHxIxJxKxL"
        };
        const int NUM_SPECS  = sizeof(SPECS) / sizeof(*SPECS);

        for (int ii = 0; ii < NUM_SPECS; ++ii) {
            List mX(&testAllocator);  const List& X = mX;  // control
            gg(&mX, SPECS[ii]);

            int iLen = mX.length();

            if (verbose) { cout << "\t"; P_(SPECS[ii]); P(X); }
            for (int pos1 = 0; pos1 < iLen; ++pos1) {     // for each position
                for (int pos2 = 0; pos2 < iLen; ++pos2) { // for each position
                    List mT(X, &testAllocator);
                    const List& T = mT;                   // object under test
                    LOOP3_ASSERT(iLen, pos1, pos2, X == T);

                    if (veryVerbose) { cout << "\t\t";  P_(pos1);
                                       P_(pos2);        P(T); }
                    mT.swapElements(pos1, pos2);
                    if (veryVerbose) { cout << "\t\t";  P_(pos1);
                                       P_(pos2);        P(T); }

                    LOOP3_ASSERT(iLen, pos1, pos2,
                                 areEqual(T.row(), pos1, X.row(), pos2));
                    LOOP3_ASSERT(iLen, pos1, pos2,
                                 areEqual(T.row(), pos2, X.row(), pos1));

                    for (int i = 0; i < iLen; ++i) {
                        if (i == pos1 || i == pos2) continue;
                        LOOP3_ASSERT(iLen, pos1, pos2,
                                     areEqual(T.row(), i, X.row(), i));
                    }
                }
            }
        }
      }

DEFINE_TEST_CASE(6) {
        // --------------------------------------------------------------------
        // TESTING 'bdem_Row' AND 'bdem_List' EQUALITY OPERATORS
        // Concerns:
        //   a) Equality must be tested for each of the 22 types.
        //   b) Lists of different lengths are unequal.
        //   c) Lists of the same length with different element types but
        //      identical values (e.g., Integer 0 and Short 0) are unequal.
        //   d) Lists with the same element types but different element values
        //      are unequal.
        //   e) Lists with the same element types and the same values at each
        //      index position but with different internal representations are
        //      equal.
        //   f) Empty Lists are equal.
        //   g) Equality is commutative.
        //
        // Plan:
        //   Test with values from all 22 types.
        //   Create identical lists with different internal representations.
        //
        // Testing:
        //   bool operator==(const bdem_Row& lhs, const bdem_Row& rhs);
        //   bool operator!=(const bdem_Row& lhs, const bdem_Row& rhs);
        //
        //   bool operator==(const bdem_List&, const bdem_List&);
        //   bool operator!=(const bdem_List&, const bdem_List&);
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl
                          << "Testing Equality Operators" << endl
                          << "==========================" << endl;

        if (verbose) cout <<
            "\nCompare each pair of similar values (u, v) in S X S." << endl;
        {
            static const char *SPECS[] = {
                "",
                "xA", "xB", "xC", "xD", "xE", "xF", "xG", "xH", "xI", "xJ",
                "xK", "xL", "xM", "xN", "xO", "xP", "xQ", "xR", "xS", "xT",
                "xU", "xV",
                "yA", "yB", "yC", "yD", "yE", "yF", "yG", "yH", "yI", "yJ",
                "yK", "yL", "yM", "yN", "yO", "yP", "yQ", "yR", "yS", "yT",
                "yU", "yV",
                "uA", "uB", "uC", "uD", "uE", "uF", "uG", "uH", "uI", "uJ",
                "uK", "uL", "uM", "uN", "uO", "uP", "uQ", "uR", "uS", "uT",
                "uU", "uV",
                "uAuB",                  "uBuA",
                "uBuB",                  "uBuU",
                "uUuB",                  "uUuU",
                "xAxA",                  "xAxB",
                "xAyB",                  "xBxA",
                "xBxB",                  "xBxU",
                "xUxB",                  "xUxU",
                "yAyA",                  "yAyB",
                "yBxA",                  "yByA",
                "yByB",                  "yByU",
                "yUyB",                  "yUyU",
                "uAuAuA",                "uAuAuB",
                "uAuBuA",                "uBuAuA",
                "xAxAxA",                "xAxAxB",
                "xAxAyB",                "xAxBxA",
                "xAyBxA",                "xBxAxA",
                "yAyAyA",                "yAyAyB",
                "yAyByA",                "yBxAxA",
                "yByAyA",                "xAxAxAxA",
                "xAxAxBxA",              "xAxBxAxA",
                "xBxAxAxA",              "xAxAxAxAxA",
                "xAxAxBxAxA",            "xAxBxAxAxA",
                "xBxAxAxAxA",            "xAxAxAxAxAxA",
                "xAxAxAxBxAxA",          "xAxAxBxAxAxA",
                "xBxAxAxAxAxA",          "xAxAxAxAxAxAxA",
                "xAxAxAxAxAxBxA",        "xBxAxAxAxAxAxA",
                "xAxAxAxAxAxAxAxA",      "xAxAxAxAxAxBxAxA",
                "xAxBxAxAxAxAxAxA",      "xAxAxAxAxAxAxAxAxA",
                "xAxAxAxAxAxBxAxAxA",    "xAxAxBxAxAxAxAxAxA",
                "xAxAxAxAxAxAxAxAxAxA",  "xAxAxAxAxAxBxAxAxAxA",
                "xAxBxCxDxExFxGxHxIxJxKxLxMxNxOxPxQxRxSxTxUxV",
                "yAyByCyDyEyFyGyHyIyJyKyLyMyNyOyPyQyRySyTyUyV",
            0}; // Null string required as last element.

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const U_SPEC = SPECS[ti];
                const int curLen = (int)strlen(U_SPEC);

                List mU(&testAllocator); gg(&mU, U_SPEC); const List& U = mU;
                LOOP_ASSERT(ti, curLen == 2 * U.length()); // same lengths

                if (curLen != oldLen) {
                    if (verbose) cout << "\tUsing lhs objects of length "
                                      << curLen << '.' << endl;
                    LOOP_ASSERT(U_SPEC, oldLen <= curLen); // non-decreasing
                    oldLen = curLen;
                }

                if (veryVerbose) { P_(ti); P_(U_SPEC); P(U); }

                for (int tj = 0; SPECS[tj]; ++tj) {
                    const char *const V_SPEC = SPECS[tj];
                    List mV(&testAllocator); gg(&mV, V_SPEC);
                    const List& V = mV;

                    if (veryVerbose) { cout << "  "; P_(tj); P_(V_SPEC); P(V);}
                    const bool isSame = ti == tj;
                    LOOP2_ASSERT(ti, tj,  isSame == (U == V));
                    LOOP2_ASSERT(ti, tj,  isSame == (V == U));
                    LOOP2_ASSERT(ti, tj,  isSame == (U.row() == V.row()));
                    LOOP2_ASSERT(ti, tj,  isSame == (V.row() == U.row()));
                    LOOP2_ASSERT(ti, tj, !isSame == (U != V));
                    LOOP2_ASSERT(ti, tj, !isSame == (V != U));
                    LOOP2_ASSERT(ti, tj, !isSame == (U.row() != V.row()));
                    LOOP2_ASSERT(ti, tj, !isSame == (V.row() != U.row()));
                }
            }
        }

        if (verbose) cout << "\nCompare objects of equal value having "
                             "potentially different internal state." << endl;
        {
            static const char *SPECS[] = { // len: 0 - 5, 7, 8, 9, 15, 16, 17
                "",      "xA",      "xAxB",     "xAxBxC",    "xAxBxCxD",
                "xAxBxCxDxE", "xAxBxCxDxExAxB",         "xAxBxCxDxExAxBxC",
                "xAxBxCxDxExAxBxCxD", "xAxBxCxDxExAxBxCxDxExAxBxCxDxE",
                "xAxBxCxDxExAxBxCxDxExAxBxCxDxExA",
                "xAxBxCxDxExAxBxCxDxExAxBxCxDxExAxB",
            0}; // Null string required as last element.

            const int NUM_PERMUTATIONS = 4;

            int oldLen = -1;
            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const int curLen = (int)strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) {
                        cout << "\tUsing objects having (logical) "
                                "length " << (curLen/2) << '.' << endl;
                    }
                    LOOP_ASSERT(SPEC, oldLen < curLen); // strictly increasing
                    oldLen = curLen;
                }

                List mX(&testAllocator); gg(&mX, SPEC); const List& X = mX;
                LOOP_ASSERT(ti, curLen == 2 * X.length()); // same lengths
                if (veryVerbose) { cout << "\t\t"; P_(ti); P_(SPEC); P(X) }

                for (int u = 0; u < NUM_PERMUTATIONS; ++u) {
                    List mU(&testAllocator);
                    makePermutation(&mU, SPEC, u%2, u/2);
                    const List& U = mU;

                    // compare canonical representation with every variation

                    LOOP2_ASSERT(SPEC, u, 1 == (U == X));
                    LOOP2_ASSERT(SPEC, u, 1 == (X == U));
                    LOOP2_ASSERT(SPEC, u, 0 == (U != X));
                    LOOP2_ASSERT(SPEC, u, 0 == (X != U));

                    for (int v = 0; v < NUM_PERMUTATIONS; ++v) {
                        List mV(&testAllocator);
                        makePermutation(&mV, SPEC, v%2, v/2);
                        const List& V = mV;

                        // compare every variation with every other one

                        LOOP3_ASSERT(SPEC, u, v, 1 == (U == V));
                        LOOP3_ASSERT(SPEC, u, v, 1 == (V == U));
                        LOOP3_ASSERT(SPEC, u, v, 0 == (U != V));
                        LOOP3_ASSERT(SPEC, u, v, 0 == (V != U));
                    }
                }
            }
        }
      }

DEFINE_TEST_CASE(5) {
        // --------------------------------------------------------------------
        // TESTING 'bdem_Row', 'bdem_List', 'bdem_Table' OUTPUT (<<) OPERATORS
        //   Since the output operators are layered on basic accessors, it is
        //   sufficient to test only the output *format* (and to ensure that no
        //   additional characters are written past the terminating null).
        //
        // Concerns:
        //   1) The output operators perform correctly for all 22 types with
        //      non-null values.
        //   2) The output operators perform correctly for all 22 types with
        //      null (unset) values.
        //
        // Plan:
        //   For each of a small representative set of object values, ordered
        //   by increasing length, use 'ostrstream' to write that object's
        //   value to two separate character buffers each with different
        //   initial values.  Compare the contents of these buffers with the
        //   literal expected output format and verify that the characters
        //   beyond the null characters are unaffected in both buffers.
        //
        //   Since the 'bdem_List' and 'bdem_Table' functions both forward to
        //   the 'bdem_Row' function, it is sufficient to verify that the
        //   'bdem_Row' function operates for a single test value and then
        //   thoroughly test the 'bdem_List' and 'bdem_Table' functions.
        //
        // Testing:
        //   ostream& operator<<(ostream& stream, const bdem_Row& rhs);
        //   ostream& operator<<(ostream& stream, const bdem_List& rhs);
        //   ostream& operator<<(ostream& stream, const bdem_Table& rhs);
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl
                          << "Testing Output (<<) Operators" << endl
                          << "=============================" << endl;

        if (verbose)
            cout << "\nTesting 'operator<<' (ostream, bdem_Row&)." << endl;
        {
            const int SIZE = 4000; // Must be big enough to hold output string.
            const char Z1  = (char) 0xFF;  // Used to represent an unset char.
            const char Z2  = 0x00;  // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            const char *const SPEC = "xBxC";
            const char *const FMT  = "{ SHORT 12 INT 103 }";

            char buf1[SIZE], buf2[SIZE];
            memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
            memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

            List mL(&testAllocator);  const List& L = gg(&mL, SPEC);
            const Row& X = L.row();

            if (verbose) cout << "\t\tSpec = \"" << SPEC << '"' << endl;
            if (veryVerbose) cout << "EXPECTED FORMAT:" << endl<<FMT<<endl;
            ostrstream out1(buf1, SIZE);  out1 << X << ends;
            ostrstream out2(buf2, SIZE);  out2 << X << ends;
            if (veryVerbose) cout << "ACTUAL FORMAT:" << endl<<buf1<<endl;

            const int SZ = (int)strlen(FMT) + 1;
            const int REST = SIZE - SZ;
            ASSERT(SZ < SIZE);  // Check buffer is large enough.
            ASSERT(Z1 == buf1[SIZE - 1]);  // Check for overrun.
            ASSERT(Z2 == buf2[SIZE - 1]);  // Check for overrun.
            ASSERT(0 == memcmp(buf1, FMT, SZ));
            ASSERT(0 == memcmp(buf2, FMT, SZ));
            ASSERT(0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
            ASSERT(0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ, REST));
        }

        if (verbose)
            cout << "\nTesting 'operator<<' (ostream, bdem_List&)." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //line  spec           output format                // ADJUST
                //----  ----           -------------
                { L_,   "",            "{ }"                                 },
                { L_,   "xA",          "{ CHAR x }"                          },
                { L_,   "xB",          "{ SHORT 12 }"                        },
                { L_,   "uB",          "{ SHORT -32768 }"                    },
                { L_,   "xC",          "{ INT 103 }"                         },
                { L_,   "uC",          "{ INT -2147483648 }"                 },
                { L_,   "xD",          "{ INT64 10004 }"                     },
                { L_,   "uD",          "{ INT64 -9223372036854775808 }"      },
                { L_,   "xE",          "{ FLOAT 105.5 }"                     },
#ifdef BSLS_PLATFORM__CMP_MSVC
                { L_,   "uE",          "{ FLOAT -6.56419e-015 }"             },
#else
                { L_,   "uE",          "{ FLOAT -6.56419e-15 }"              },
#endif
                { L_,   "xF",          "{ DOUBLE 106.006 }"                  },
#ifdef BSLS_PLATFORM__CMP_MSVC
                { L_,   "uF",          "{ DOUBLE -2.42454e-014 }"            },
#else
                { L_,   "uF",          "{ DOUBLE -2.42454e-14 }"             },
#endif
                { L_,   "xG",          "{ STRING one-seven--- }"             },
                { L_,   "uG",          "{ STRING  }"                         },
                { L_,   "xH",          "{ DATETIME 08AUG0108_08:08:08.108 }" },
                { L_,   "uH",          "{ DATETIME 01JAN0001_24:00:00.000 }" },
                { L_,   "xI",          "{ DATE 09SEP0109 }"                  },
                { L_,   "uI",          "{ DATE 01JAN0001 }"                  },
                { L_,   "xJ",          "{ TIME 10:10:10.110 }"               },
                { L_,   "uJ",          "{ TIME 24:00:00.000 }"               },
                { L_,   "xK",          "{ CHAR_ARRAY \"xNxNx\" }"            },
                { L_,   "uK",          "{ CHAR_ARRAY \"\" }"                 },
                { L_,   "xL",          "{ SHORT_ARRAY [ 12 -32768 ] }"       },
                { L_,   "uL",          "{ SHORT_ARRAY [ ] }"                 },
                { L_,   "xM",          "{ INT_ARRAY [ 103 -2147483648 ] }"   },
                { L_,   "uM",          "{ INT_ARRAY [ ] }"                   },
                { L_,   "xN",          "{ INT64_ARRAY [ 10004 "
                                       "-9223372036854775808 ] }"            },
                { L_,   "uN",          "{ INT64_ARRAY [ ] }"                 },
#ifdef BSLS_PLATFORM__CMP_MSVC
                { L_,   "xO",          "{ FLOAT_ARRAY [ 105.5 "
                                       "-6.56419e-015 ] }"                   },
#else
                { L_,   "xO",          "{ FLOAT_ARRAY [ 105.5 "
                                       "-6.56419e-15 ] }"                    },
#endif
                { L_,   "uO",          "{ FLOAT_ARRAY [ ] }"                 },
#ifdef BSLS_PLATFORM__CMP_MSVC
                { L_,   "xP",          "{ DOUBLE_ARRAY [ 106.006 "
                                       "-2.42454e-014 ] }"                   },
#else
                { L_,   "xP",          "{ DOUBLE_ARRAY [ 106.006 "
                                       "-2.42454e-14 ] }"                    },
#endif
                { L_,   "uP",          "{ DOUBLE_ARRAY [ ] }"                },
                { L_,   "xQ",          "{ STRING_ARRAY [ one-seven---  ] }"  },
                { L_,   "uQ",          "{ STRING_ARRAY [ ] }"                },
                { L_,   "xR",          "{ DATETIME_ARRAY [ "
                                       "08AUG0108_08:08:08.108 "
                                       "01JAN0001_24:00:00.000 ] }"          },
                { L_,   "uR",          "{ DATETIME_ARRAY [ ] }"              },
                { L_,   "xS",          "{ DATE_ARRAY [ "
                                       "09SEP0109 01JAN0001 ] }"             },
                { L_,   "uS",          "{ DATE_ARRAY [ ] }"                  },
                { L_,   "xT",          "{ TIME_ARRAY [ "
                                       "10:10:10.110 24:00:00.000 ] }"       },
                { L_,   "uT",          "{ TIME_ARRAY [ ] }"                  },
                { L_,   "xU",          "{ LIST { "
                                       "INT 103 STRING one-seven--- } }"     },
                { L_,   "uU",          "{ LIST { } }"                        },
                { L_,   "xV",          "{ TABLE { "
                                       "Column Types: [ INT STRING ] "
                                       "Row 0: { 103 one-seven--- } "
                                       "Row 1: { -2147483648  } } }"         },
                { L_,   "uV",          "{ TABLE { Column Types: [ ] } }"     },
                { L_,   "xBxC",        "{ SHORT 12 INT 103 }"                },
                { L_,   "xAxBxCxDxE",  "{ CHAR x SHORT 12 INT 103 "
                                       "INT64 10004 FLOAT 105.5 }"           },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 4000; // Must be big enough to hold output string.
            const char Z1  = (char) 0xFF;  // Used to represent an unset char.
            const char Z2  = 0x00;  // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const char *const FMT  = DATA[ti].d_fmt_p;
                const int curLen = (int)strlen(SPEC);

                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                List mX(&testAllocator);  const List& X = gg(&mX, SPEC);
                LOOP_ASSERT(LINE, curLen == 2 * X.length()); // same lengths

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton objects of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen < curLen); // strictly increasing
                    oldLen = curLen;
                }

                if (verbose) cout << "\t\tSpec = \"" << SPEC << '"' << endl;
                ostrstream out1(buf1, SIZE);  out1 << X << ends;
                ostrstream out2(buf2, SIZE);  out2 << X << ends;

                bool failure = 0 != memcmp(buf1, FMT, strlen(FMT)) ||
                                                   strlen(buf1) != strlen(FMT);
                if (veryVerbose || failure) {
                    if (failure) cout << "Error: LINE = " << LINE << endl;
                    cout << "EXPECTED FORMAT(" << strlen(FMT)  << "): ``" <<
                                                                FMT  << "''\n";
                    cout << "  ACTUAL FORMAT(" << strlen(buf1) << "): ``" <<
                                                                buf1 << "''\n";
                }

                const int SZ = (int)strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LINE, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, 0 == memcmp(buf1, FMT, SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(buf2, FMT, SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(buf1 + SZ, CTRL_BUF1+SZ, REST));
                LOOP_ASSERT(LINE, 0 == memcmp(buf2 + SZ, CTRL_BUF2+SZ, REST));
            }
        }

        if (verbose)
            cout << "\nTesting 'operator<<' (ostream, bdem_Table&)." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //line  spec           output format                  // ADJUST
                //----  ----           -------------
                { L_,   "",            "{ Column Types: [ ]}"                },
                { L_,   "A;x;",        "{ Column Types: [ CHAR ] "
                                       "Row 0: { x }}"                       },
                { L_,   "B;x;",        "{ Column Types: [ SHORT ] "
                                       "Row 0: { 12 }}"                      },
                { L_,   "C;x;",        "{ Column Types: [ INT ] "
                                       "Row 0: { 103 }}"                     },
                { L_,   "D;x;",        "{ Column Types: [ INT64 ] "
                                       "Row 0: { 10004 }}"                   },
                { L_,   "E;x;",        "{ Column Types: [ FLOAT ] "
                                       "Row 0: { 105.5 }}"                   },
                { L_,   "F;x;",        "{ Column Types: [ DOUBLE ] "
                                       "Row 0: { 106.006 }}"                 },
                { L_,   "G;x;",        "{ Column Types: [ STRING ] "
                                       "Row 0: { one-seven--- }}"            },
                { L_,   "H;x;",        "{ Column Types: [ DATETIME ] "
                                       "Row 0: { 08AUG0108_08:08:08.108 }}"  },
                { L_,   "I;x;",        "{ Column Types: [ DATE ] "
                                       "Row 0: { 09SEP0109 }}"               },
                { L_,   "J;x;",        "{ Column Types: [ TIME ] "
                                       "Row 0: { 10:10:10.110 }}"            },
                { L_,   "K;x;",        "{ Column Types: [ CHAR_ARRAY ] "
                                       "Row 0: { \"xNxNx\" }}"           },
                { L_,   "L;x;",        "{ Column Types: [ SHORT_ARRAY ] "
                                       "Row 0: { [ 12 -32768 ] }}"           },
                { L_,   "M;x;",        "{ Column Types: [ INT_ARRAY ] "
                                       "Row 0: { [ 103 -2147483648 ] }}"     },
                { L_,   "N;x;",        "{ Column Types: [ INT64_ARRAY ] "
                                       "Row 0: { [ 10004 "
                                       "-9223372036854775808 ] }}"           },
#ifdef BSLS_PLATFORM__CMP_MSVC
                { L_,   "O;x;",        "{ Column Types: [ FLOAT_ARRAY ] "
                                       "Row 0: { [ 105.5 -6.56419e-015 ] }}" },
                { L_,   "P;x;",        "{ Column Types: [ DOUBLE_ARRAY ] "
                                      "Row 0: { [ 106.006 -2.42454e-014 ] }}"},
#else
                { L_,   "O;x;",        "{ Column Types: [ FLOAT_ARRAY ] "
                                       "Row 0: { [ 105.5 -6.56419e-15 ] }}"  },
                { L_,   "P;x;",        "{ Column Types: [ DOUBLE_ARRAY ] "
                                       "Row 0: { [ 106.006 -2.42454e-14 ] }}"},
#endif
                { L_,   "Q;x;",        "{ Column Types: [ STRING_ARRAY ] "
                                       "Row 0: { [ one-seven---  ] }}"       },
                { L_,   "R;x;",        "{ Column Types: [ DATETIME_ARRAY ] "
                                       "Row 0: { [ 08AUG0108_08:08:08.108 "
                                       "01JAN0001_24:00:00.000 ] }}"         },
                { L_,   "S;x;",        "{ Column Types: [ DATE_ARRAY ] "
                                       "Row 0: { [ 09SEP0109 01JAN0001 ] }}" },
                { L_,   "T;x;",        "{ Column Types: [ TIME_ARRAY ] "
                                       "Row 0: { [ 10:10:10.110 "
                                       "24:00:00.000 ] }}"                   },
                { L_,   "U;x;",        "{ Column Types: [ LIST ] "
                                       "Row 0: { { INT 103 "
                                       "STRING one-seven--- } }}"            },
                { L_,   "V;x;",        "{ Column Types: [ TABLE ] "
                                       "Row 0: { "
                                       "{ Column Types: [ INT STRING ] "
                                       "Row 0: { 103 one-seven--- } "
                                       "Row 1: { -2147483648  } } }}"        },
                { L_,   "BC;xx;",      "{ Column Types: [ SHORT INT ] "
                                       "Row 0: { 12 103 }}"                  },
                { L_,   "VBUVQ;xxxxx;yyyyy;",
                                       "{ Column Types: "
                                       "[ TABLE SHORT LIST TABLE "
                                       "STRING_ARRAY ] "
                                       "Row 0: { { "
                                       "Column Types: [ INT STRING ] "
                                       "Row 0: { 103 one-seven--- } "
                                       "Row 1: { -2147483648  } } 12 "
                                       "{ INT 103 STRING one-seven--- } { "
                                       "Column Types: [ INT STRING ] "
                                       "Row 0: { 103 one-seven--- } "
                                       "Row 1: { -2147483648  } } "
                                       "[ one-seven---  ] } "
                                       "Row 1: { { "
                                       "Column Types: [ INT STRING ] "
                                       "Row 0: { 203 two-seven } "
                                       "Row 1: { -2147483648  } } 22 "
                                       "{ INT 203 STRING two-seven } { "
                                       "Column Types: [ INT STRING ] "
                                       "Row 0: { 203 two-seven } "
                                       "Row 1: { -2147483648  } } "
                                       "[ two-seven  ] }}"                   },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 4000; // Must be big enough to hold output string.
            const char Z1  = (char) 0xFF;  // Used to represent an unset char.
            const char Z2  = 0x00;  // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const char *const FMT  = DATA[ti].d_fmt_p;
                const int curLen = (int)strlen(SPEC);

                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                Table mX(&testAllocator);  const Table& X = hh(&mX, SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton objects of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen < curLen); // strictly increasing
                    oldLen = curLen;
                }

                if (verbose) cout << "\t\tSpec = \"" << SPEC << '"' << endl;
                if (veryVerbose) cout << "EXPECTED FORMAT:" << endl<<FMT<<endl;
                ostrstream out1(buf1, SIZE);  out1 << X << ends;
                ostrstream out2(buf2, SIZE);  out2 << X << ends;
                if (veryVerbose) cout << "ACTUAL FORMAT:" << endl<<buf1<<endl;

                if (strlen(FMT) != strlen(buf1) || strcmp(FMT, buf1)) {
                    cout << "Error: LINE = " << LINE << endl;
                    cout << "Expected result: ``" << FMT  << "''\n";
                    cout << "  Actual result: ``" << buf1 << "''\n";
                }

                const int SZ = (int)strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LINE, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, 0 == memcmp(buf1, FMT, SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(buf2, FMT, SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(buf1 + SZ, CTRL_BUF1+SZ, REST));
                LOOP_ASSERT(LINE, 0 == memcmp(buf2 + SZ, CTRL_BUF2+SZ, REST));
            }
        }
      }

DEFINE_TEST_CASE(4) {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTIONS 'HH' AND 'HHH' ('bdem_Table')
        // Plan:
        //   Test the behavior of 'hh' and 'hhh'.
        //   * Show 'hh' returns a correct reference to its first argument.
        //   * Enumerate representative input, ordered by length.
        //   * Show the correct accessing of all 66 unique values (of 22
        //     unique types) that are a part of the 'hh' language.
        //   * Test large legal input strings.
        //   * Test 'hhh' syntax checking with syntactically illegal inputs.
        //
        // Testing:
        //   int hhh(bdem_Table *address, const char *spec, int showErrorFlag);
        //   bdem_Table& hh(bdem_Table *object, const char *spec);
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl
            << "Testing 'hh' and 'hhh' generator functions" << endl
            << "==========================================" << endl;

        if (verbose) cout <<
            "\nTesting 'hh' and 'hhh' (table) generator functions." << endl;

        // Strategy: Enumerate legal 'spec' strings, ordered by length,
        // with the following constraints:
        //  * Alphabet: Punctuation: ';', '~', '!'; Type/Value: 'A', 'x'.
        //  * All strings of length 0, 1, and 2.
        //  * Length >= 3: suppress combinations of 3 punctuations.
        //  * Length >= 4: suppress repeated punctuations and permutations
        //    of pairs of punctuations.
        //  * Length > 4: sample a few representative strings.

        static const struct {
            int         d_lineNum;   // source line number
            const char *d_spec;      // input 'spec' string for 'gg'
            const char *d_types;     // expected element types (encoded)
            int         d_numRows;   // expected number of rows
        } DATA[] = {
            // line    spec            elemType   numRows
            // ----    ----            --------   -------
            {   L_,    ""            ,    "",       0,  },  // length 0

            {   L_,    ";"           ,    "",       1,  },  // length 1
            {   L_,    "~"           ,    "",       0,  },
            {   L_,    "!"           ,    "",       0,  },

            {   L_,    ";;"          ,    "",       2,  },  // length 2
            {   L_,    ";~"          ,    "",       0,  },
            {   L_,    ";!"          ,    "",       0,  },
            {   L_,    "~;"          ,    "",       1,  },
            {   L_,    "~~"          ,    "",       0,  },
            {   L_,    "~!"          ,    "",       0,  },
            {   L_,    "!;"          ,    "",       1,  },
            {   L_,    "!~"          ,    "",       0,  },
            {   L_,    "!!"          ,    "",       0,  },
            {   L_,    "A;"          ,   "A",       0,  },

            {   L_,    ";A;"         ,   "A",       0,  },  // length 3
            {   L_,    "~A;"         ,   "A",       0,  },
            {   L_,    "!A;"         ,   "A",       0,  },
            {   L_,    "A;~"         ,    "",       0,  },
            {   L_,    "A;!"         ,   "A",       0,  },
            {   L_,    "AA;"         ,  "AA",       0,  },

            {   L_,    ";~A;"        ,   "A",       0,  },  // length 4
            {   L_,    ";!A;"        ,   "A",       0,  },
            {   L_,    ";A;~"        ,    "",       0,  },
            {   L_,    ";A;!"        ,   "A",       0,  },
            {   L_,    ";AA;"        ,  "AA",       0,  },
            {   L_,    "~;A;"        ,   "A",       0,  },
            {   L_,    "~!A;"        ,   "A",       0,  },
            {   L_,    "~A;~"        ,    "",       0,  },
            {   L_,    "~A;!"        ,   "A",       0,  },
            {   L_,    "~AA;"        ,  "AA",       0,  },
            {   L_,    "!;A;"        ,   "A",       0,  },
            {   L_,    "!~A;"        ,   "A",       0,  },
            {   L_,    "!A;~"        ,    "",       0,  },
            {   L_,    "!A;!"        ,   "A",       0,  },
            {   L_,    "!AA;"        ,  "AA",       0,  },
            {   L_,    "A;~!"        ,    "",       0,  },
            {   L_,    "A;!~"        ,    "",       0,  },
            {   L_,    "A;A;"        ,   "A",       0,  },
            {   L_,    "A;x;"        ,   "A",       1,  },

            {   L_,    "A;x;x;"      ,   "A",       2,  },  // miscellaneous
            {   L_,    "AA;xx;xx;"   ,  "AA",       2,  },
            {   L_,    "AA;xx;xx;~"  ,    "",       0,  },
            {   L_,    "AA;xx;xx;!"  ,  "AA",       0,  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\tEnumerated 'spec' ordered by length." << endl;
        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE = DATA[i].d_lineNum;
            Table mT(Z);
            const Table& RT = hh(&mT, DATA[i].d_spec);
            my_ElemTypeArray e;  appendToElemTypeArray(&e, DATA[i].d_types);

            LOOP_ASSERT(LINE, &RT == &mT);    // Test 'hh' return value
            my_ElemTypeArray e1;
            getElemTypes(&e1, mT);
            LOOP_ASSERT(LINE, e == e1);
            LOOP_ASSERT(LINE, DATA[i].d_numRows == mT.numRows());
            LOOP_ASSERT(LINE, getNumRows(DATA[i].d_spec) == mT.numRows());
        }

        // Testing all possible one-element tables (that 'hh' can generate)
        if (verbose) cout << "\tAll available one-element tables." << endl;

        Table mT(Z);
        ASSERT( hh(&mT, "~A;x;")[0].theChar(0) == XA );
        ASSERT( hh(&mT, "~A;y;")[0].theChar(0) == YA );
        ASSERT( hh(&mT, "~A;u;")[0].theChar(0) == UA );

        ASSERT( hh(&mT, "~B;x;")[0].theShort(0) == XB );
        ASSERT( hh(&mT, "~B;y;")[0].theShort(0) == YB );
        ASSERT( hh(&mT, "~B;u;")[0].theShort(0) == UB );

        ASSERT( hh(&mT, "~C;x;")[0].theInt(0) == XC );
        ASSERT( hh(&mT, "~C;y;")[0].theInt(0) == YC );
        ASSERT( hh(&mT, "~C;u;")[0].theInt(0) == UC );

        ASSERT( hh(&mT, "~D;x;")[0].theInt64(0) == XD );
        ASSERT( hh(&mT, "~D;y;")[0].theInt64(0) == YD );
        ASSERT( hh(&mT, "~D;u;")[0].theInt64(0) == UD );

        ASSERT( hh(&mT, "~E;x;")[0].theFloat(0) == XE );
        ASSERT( hh(&mT, "~E;y;")[0].theFloat(0) == YE );
        ASSERT( hh(&mT, "~E;u;")[0].theFloat(0) == UE );

        ASSERT( hh(&mT, "~F;x;")[0].theDouble(0) == XF );
        ASSERT( hh(&mT, "~F;y;")[0].theDouble(0) == YF );
        ASSERT( hh(&mT, "~F;u;")[0].theDouble(0) == UF );

        ASSERT( hh(&mT, "~G;x;")[0].theString(0) == XG );
        ASSERT( hh(&mT, "~G;y;")[0].theString(0) == YG );
        ASSERT( hh(&mT, "~G;u;")[0].theString(0) == UG );

        ASSERT( hh(&mT, "~H;x;")[0].theDatetime(0) == XH );
        ASSERT( hh(&mT, "~H;y;")[0].theDatetime(0) == YH );
        ASSERT( hh(&mT, "~H;u;")[0].theDatetime(0) == UH );

        ASSERT( hh(&mT, "~I;x;")[0].theDate(0) == XI );
        ASSERT( hh(&mT, "~I;y;")[0].theDate(0) == YI );
        ASSERT( hh(&mT, "~I;u;")[0].theDate(0) == UI );

        ASSERT( hh(&mT, "~J;x;")[0].theTime(0) == XJ );
        ASSERT( hh(&mT, "~J;y;")[0].theTime(0) == YJ );
        ASSERT( hh(&mT, "~J;u;")[0].theTime(0) == UJ );

        ASSERT( hh(&mT, "~K;x;")[0].theCharArray(0) == XK );
        ASSERT( hh(&mT, "~K;y;")[0].theCharArray(0) == YK );
        ASSERT( hh(&mT, "~K;u;")[0].theCharArray(0) == UK );

        ASSERT( hh(&mT, "~L;x;")[0].theShortArray(0) == XL );
        ASSERT( hh(&mT, "~L;y;")[0].theShortArray(0) == YL );
        ASSERT( hh(&mT, "~L;u;")[0].theShortArray(0) == UL );

        ASSERT( hh(&mT, "~M;x;")[0].theIntArray(0) == XM );
        ASSERT( hh(&mT, "~M;y;")[0].theIntArray(0) == YM );
        ASSERT( hh(&mT, "~M;u;")[0].theIntArray(0) == UM );

        ASSERT( hh(&mT, "~N;x;")[0].theInt64Array(0) == XN );
        ASSERT( hh(&mT, "~N;y;")[0].theInt64Array(0) == YN );
        ASSERT( hh(&mT, "~N;u;")[0].theInt64Array(0) == UN );

        ASSERT( hh(&mT, "~O;x;")[0].theFloatArray(0) == XO );
        ASSERT( hh(&mT, "~O;y;")[0].theFloatArray(0) == YO );
        ASSERT( hh(&mT, "~O;u;")[0].theFloatArray(0) == UO );

        ASSERT( hh(&mT, "~P;x;")[0].theDoubleArray(0) == XP );
        ASSERT( hh(&mT, "~P;y;")[0].theDoubleArray(0) == YP );
        ASSERT( hh(&mT, "~P;u;")[0].theDoubleArray(0) == UP );

        ASSERT( hh(&mT, "~Q;x;")[0].theStringArray(0) == XQ );
        ASSERT( hh(&mT, "~Q;y;")[0].theStringArray(0) == YQ );
        ASSERT( hh(&mT, "~Q;u;")[0].theStringArray(0) == UQ );

        ASSERT( hh(&mT, "~R;x;")[0].theDatetimeArray(0) == XR );
        ASSERT( hh(&mT, "~R;y;")[0].theDatetimeArray(0) == YR );
        ASSERT( hh(&mT, "~R;u;")[0].theDatetimeArray(0) == UR );

        ASSERT( hh(&mT, "~S;x;")[0].theDateArray(0) == XS );
        ASSERT( hh(&mT, "~S;y;")[0].theDateArray(0) == YS );
        ASSERT( hh(&mT, "~S;u;")[0].theDateArray(0) == US );

        ASSERT( hh(&mT, "~T;x;")[0].theTimeArray(0) == XT );
        ASSERT( hh(&mT, "~T;y;")[0].theTimeArray(0) == YT );
        ASSERT( hh(&mT, "~T;u;")[0].theTimeArray(0) == UT );

        ASSERT( hh(&mT, "~U;x;")[0].theList(0) == XU );
        ASSERT( hh(&mT, "~U;y;")[0].theList(0) == YU );
        ASSERT( hh(&mT, "~U;u;")[0].theList(0) == UU );

        ASSERT( hh(&mT, "~V;x;")[0].theTable(0) == XV );
        ASSERT( hh(&mT, "~V;y;")[0].theTable(0) == YV );
        ASSERT( hh(&mT, "~V;u;")[0].theTable(0) == UV );

        if (verbose) cout << "\tLarge 'spec'." << endl;
        const int NUM_PASSES = 10;
        hh(&mT, "~ED;~!~A;~ABCDEFGHIJKLMNOPQRSTUV;");
        for (int j = 0; j < NUM_PASSES; ++j) {
            hh(&mT, "xyuxyuxyuxyuxyuxyuxyux;");
            LOOP_ASSERT( j, 22    == mT.numColumns() );
            LOOP_ASSERT( j, j + 1 == mT.numRows() );
            LOOP_ASSERT( j, XA    == mT[j].theChar(0) );
            LOOP_ASSERT( j, XV    == mT[j].theTable(21) );
        }

        if (verbose) cout << "\nTesting error reporting in 'hhh'." << endl;
        static const struct {
            int         d_lineNum;   // source line number
            const char *d_spec;      // input 'spec' string for 'gg'
            int         d_errorCode; // expected 'enum' error code
        } E_DATA[] = {
            // line    spec                  error code
            // ----    ----                  ----------
            {   L_,    "."              ,    HHH_ILLEGAL_CHARACTER,      },
            {   L_,    "z"              ,    HHH_ILLEGAL_CHARACTER,      },

            {   L_,    "A;x:"           ,    HHH_VALFIELD_UNTERMINATED,  },
            {   L_,    "AB;xy~"         ,    HHH_VALFIELD_UNTERMINATED,  },
            {   L_,    "AB;xy;xy!"      ,    HHH_VALFIELD_UNTERMINATED,  },
            {   L_,    "AB;xy;xy;x"     ,    HHH_VALFIELD_UNTERMINATED,  },
            {   L_,    "AB;xy;xy;xy"    ,    HHH_VALFIELD_UNTERMINATED,  },
            {   L_,    "AB;xy;xy;xyu"   ,    HHH_VALFIELD_UNTERMINATED,  },
            {   L_,    "AB;xy;xy;xyz"   ,    HHH_VALFIELD_UNTERMINATED,  },

            {   L_,    "Ag"             ,    HHH_TYPEFIELD_UNTERMINATED, },
            {   L_,    "Ag;"            ,    HHH_TYPEFIELD_UNTERMINATED, },
            {   L_,    "ABC"            ,    HHH_TYPEFIELD_UNTERMINATED, },
            {   L_,    "ABC~"           ,    HHH_TYPEFIELD_UNTERMINATED, },
            {   L_,    "AB!;"           ,    HHH_TYPEFIELD_UNTERMINATED, },
            {   L_,    "ABx;"           ,    HHH_TYPEFIELD_UNTERMINATED, },
            {   L_,    "ABz;"           ,    HHH_TYPEFIELD_UNTERMINATED, },

            {   L_,    "A;;"            ,    HHH_MISSING_EXPECTED_VALUE, },
            {   L_,    "AB;x;"          ,    HHH_MISSING_EXPECTED_VALUE, },
            {   L_,    "AB;xy;x;"       ,    HHH_MISSING_EXPECTED_VALUE, },

            {   L_,    ""               ,    HHH_SUCCESS,                },
            {   L_,    "~ABC;xyu;~;;"   ,    HHH_SUCCESS,                }
        };
        int numEData = sizeof E_DATA / sizeof E_DATA[0]; // E_DATA[] length
        for (int k = 0; k < numEData; ++k) {
            const int LINE = E_DATA[k].d_lineNum;
            Table mT(Z);
            const int SHOW_ERRORS = veryVerbose;
            const int ERROR_CODE = hhh(&mT, E_DATA[k].d_spec, SHOW_ERRORS);
            LOOP_ASSERT(LINE, ERROR_CODE == E_DATA[k].d_errorCode);
        }
      }

DEFINE_TEST_CASE(3) {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTIONS 'GG' AND 'GGG' ('bdem_List')
        // Plan:
        //   Test the behavior of 'gg' and 'ggg'.
        //   * Verify that all test values are distinct.
        //   * Show 'gg' returns a correct reference to its first argument.
        //   * Enumerate representative input, ordered by length.
        //   * Show the correct accessing of all 66 unique values (of 22
        //     unique types) that are a part of the 'gg' language.
        //   * Test large legal input strings.
        //   * Test 'ggg' syntax checking with syntactically illegal inputs.
        //
        // Testing:
        //   bdem_List& gg(bdem_List *address, const char *spec);
        //   int ggg(bdem_List *address, const char *spec, int showErrorFlag);
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << endl
            << "Testing 'gg' and 'ggg' generator functions" << endl
            << "==========================================" << endl;

        if (verbose) cout <<
            "\nTesting that all test values are distinct." << endl;

        ASSERT(0 == (XA == UA));
        ASSERT(0 == (XA == YA));
        ASSERT(0 == (UA == YA));
        ASSERT(0 == (XB == UB));
        ASSERT(0 == (XB == YB));
        ASSERT(0 == (UB == YB));
        ASSERT(0 == (XC == UC));
        ASSERT(0 == (XC == YC));
        ASSERT(0 == (UC == YC));
        ASSERT(0 == (XD == UD));
        ASSERT(0 == (XD == YD));
        ASSERT(0 == (UD == YD));
        ASSERT(0 == (XE == UE));
        ASSERT(0 == (XE == YE));
        ASSERT(0 == (UE == YE));
        ASSERT(0 == (XF == UF));
        ASSERT(0 == (XF == YF));
        ASSERT(0 == (UF == YF));
        ASSERT(0 == (XG == UG));
        ASSERT(0 == (XG == YG));
        ASSERT(0 == (UG == YG));
        ASSERT(0 == (XH == UH));
        ASSERT(0 == (XH == YH));
        ASSERT(0 == (UH == YH));
        ASSERT(0 == (XI == UI));
        ASSERT(0 == (XI == YI));
        ASSERT(0 == (UI == YI));
        ASSERT(0 == (XJ == UJ));
        ASSERT(0 == (XJ == YJ));
        ASSERT(0 == (UJ == YJ));
        ASSERT(0 == (XK == UK));
        ASSERT(0 == (XK == YK));
        ASSERT(0 == (UK == YK));
        ASSERT(0 == (XL == UL));
        ASSERT(0 == (XL == YL));
        ASSERT(0 == (UL == YL));
        ASSERT(0 == (XM == UM));
        ASSERT(0 == (XM == YM));
        ASSERT(0 == (UM == YM));
        ASSERT(0 == (XN == UN));
        ASSERT(0 == (XN == YN));
        ASSERT(0 == (UN == YN));
        ASSERT(0 == (XO == UO));
        ASSERT(0 == (XO == YO));
        ASSERT(0 == (UO == YO));
        ASSERT(0 == (XP == UP));
        ASSERT(0 == (XP == YP));
        ASSERT(0 == (UP == YP));
        ASSERT(0 == (XQ == UQ));
        ASSERT(0 == (XQ == YQ));
        ASSERT(0 == (UQ == YQ));
        ASSERT(0 == (XR == UR));
        ASSERT(0 == (XR == YR));
        ASSERT(0 == (UR == YR));
        ASSERT(0 == (XS == US));
        ASSERT(0 == (XS == YS));
        ASSERT(0 == (US == YS));
        ASSERT(0 == (XT == UT));
        ASSERT(0 == (XT == YT));
        ASSERT(0 == (UT == YT));
        ASSERT(0 == (XU == UU));
        ASSERT(0 == (XU == YU));
        ASSERT(0 == (UU == YU));
        ASSERT(0 == (XV == UV));
        ASSERT(0 == (XV == YV));
        ASSERT(0 == (UV == YV));

        if (verbose) cout <<
            "\nTesting 'gg' (list) generator function:" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec;     // input 'spec' string for 'gg'
            const char *d_types;    // expected element types (encoded)
            int         d_length;   // expected length of returned list
        } DATA[] = {
            // line          spec    elemType  length
            // ----          ----    --------  ------
            {   L_,            "",      "",    0, },
            {   L_,           "~",      "",    0, },
            {   L_,          "~~",      "",    0, },

            {   L_,          "xA",     "A",    1, },
            {   L_,          "yB",     "B",    1, },
            {   L_,          "uC",     "C",    1, },
            {   L_,          "xD",     "D",    1, },
            {   L_,          "yE",     "E",    1, },
            {   L_,          "uF",     "F",    1, },
            {   L_,          "xG",     "G",    1, },
            {   L_,          "yH",     "H",    1, },
            {   L_,          "uI",     "I",    1, },
            {   L_,          "xJ",     "J",    1, },
            {   L_,          "yK",     "K",    1, },
            {   L_,          "uL",     "L",    1, },
            {   L_,          "xM",     "M",    1, },
            {   L_,          "yN",     "N",    1, },
            {   L_,          "uO",     "O",    1, },
            {   L_,          "xP",     "P",    1, },
            {   L_,          "yQ",     "Q",    1, },
            {   L_,          "uR",     "R",    1, },
            {   L_,          "xS",     "S",    1, },
            {   L_,          "yT",     "T",    1, },
            {   L_,          "uU",     "U",    1, },
            {   L_,          "xV",     "V",    1, },

            {   L_,         "xB~",      "",    0, },
            {   L_,         "~uC",     "C",    1, },
            {   L_,         "~xD",     "D",    1, },

            {   L_,        "~~yF",     "F",    1, },
            {   L_,        "uG~~",      "",    0, },
            {   L_,        "yHuI",    "HI",    2, },

            {   L_,       "~yUuV",    "UV",    2, },
            {   L_,       "xB~uA",     "A",    1, },
            {   L_,       "yBuC~",      "",    0, },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\tEnumerated 'spec' ordered by length." << endl;
        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE = DATA[i].d_lineNum;
            List mL(Z);
            const List& RL = gg(&mL, DATA[i].d_spec);
            my_ElemTypeArray e;  appendToElemTypeArray(&e, DATA[i].d_types);
            LOOP_ASSERT(LINE, &RL == &mL);    // Test 'gg' return value
            my_ElemTypeArray e1;
            getElemTypes(&e1, mL);
            LOOP_ASSERT(LINE, e == e1);
            LOOP_ASSERT(LINE, mL.length() == DATA[i].d_length);
        }

        // Testing all possible one-element lists (that 'gg' can generate),
        // Note that this tests type as well as value.
        if (verbose) cout << "\tAll available one-element lists." << endl;
        List mL(Z);
        ASSERT( gg(&mL, "~xA").theChar(0) == XA );
        ASSERT( gg(&mL, "~yA").theChar(0) == YA );
        ASSERT( gg(&mL, "~uA").theChar(0) == UA );

        ASSERT( gg(&mL, "~xB").theShort(0) == XB );
        ASSERT( gg(&mL, "~yB").theShort(0) == YB );
        ASSERT( gg(&mL, "~uB").theShort(0) == UB );

        ASSERT( gg(&mL, "~xC").theInt(0) == XC );
        ASSERT( gg(&mL, "~yC").theInt(0) == YC );
        ASSERT( gg(&mL, "~uC").theInt(0) == UC );

        ASSERT( gg(&mL, "~xD").theInt64(0) == XD );
        ASSERT( gg(&mL, "~yD").theInt64(0) == YD );
        ASSERT( gg(&mL, "~uD").theInt64(0) == UD );

        ASSERT( gg(&mL, "~xE").theFloat(0) == XE );
        ASSERT( gg(&mL, "~yE").theFloat(0) == YE );
        ASSERT( gg(&mL, "~uE").theFloat(0) == UE );

        ASSERT( gg(&mL, "~xF").theDouble(0) == XF );
        ASSERT( gg(&mL, "~yF").theDouble(0) == YF );
        ASSERT( gg(&mL, "~uF").theDouble(0) == UF );

        ASSERT( gg(&mL, "~xG").theString(0) == XG );
        ASSERT( gg(&mL, "~yG").theString(0) == YG );
        ASSERT( gg(&mL, "~uG").theString(0) == UG );

        ASSERT( gg(&mL, "~xH").theDatetime(0) == XH );
        ASSERT( gg(&mL, "~yH").theDatetime(0) == YH );
        ASSERT( gg(&mL, "~uH").theDatetime(0) == UH );

        ASSERT( gg(&mL, "~xI").theDate(0) == XI );
        ASSERT( gg(&mL, "~yI").theDate(0) == YI );
        ASSERT( gg(&mL, "~uI").theDate(0) == UI );

        ASSERT( gg(&mL, "~xJ").theTime(0) == XJ );
        ASSERT( gg(&mL, "~yJ").theTime(0) == YJ );
        ASSERT( gg(&mL, "~uJ").theTime(0) == UJ );

        ASSERT( gg(&mL, "~xK").theCharArray(0) == XK );
        ASSERT( gg(&mL, "~yK").theCharArray(0) == YK );
        ASSERT( gg(&mL, "~uK").theCharArray(0) == UK );

        ASSERT( gg(&mL, "~xL").theShortArray(0) == XL );
        ASSERT( gg(&mL, "~yL").theShortArray(0) == YL );
        ASSERT( gg(&mL, "~uL").theShortArray(0) == UL );

        ASSERT( gg(&mL, "~xM").theIntArray(0) == XM );
        ASSERT( gg(&mL, "~yM").theIntArray(0) == YM );
        ASSERT( gg(&mL, "~uM").theIntArray(0) == UM );

        ASSERT( gg(&mL, "~xN").theInt64Array(0) == XN );
        ASSERT( gg(&mL, "~yN").theInt64Array(0) == YN );
        ASSERT( gg(&mL, "~uN").theInt64Array(0) == UN );

        ASSERT( gg(&mL, "~xO").theFloatArray(0) == XO );
        ASSERT( gg(&mL, "~yO").theFloatArray(0) == YO );
        ASSERT( gg(&mL, "~uO").theFloatArray(0) == UO );

        ASSERT( gg(&mL, "~xP").theDoubleArray(0) == XP );
        ASSERT( gg(&mL, "~yP").theDoubleArray(0) == YP );
        ASSERT( gg(&mL, "~uP").theDoubleArray(0) == UP );

        ASSERT( gg(&mL, "~xQ").theStringArray(0) == XQ );
        ASSERT( gg(&mL, "~yQ").theStringArray(0) == YQ );
        ASSERT( gg(&mL, "~uQ").theStringArray(0) == UQ );

        ASSERT( gg(&mL, "~xR").theDatetimeArray(0) == XR );
        ASSERT( gg(&mL, "~yR").theDatetimeArray(0) == YR );
        ASSERT( gg(&mL, "~uR").theDatetimeArray(0) == UR );

        ASSERT( gg(&mL, "~xS").theDateArray(0) == XS );
        ASSERT( gg(&mL, "~yS").theDateArray(0) == YS );
        ASSERT( gg(&mL, "~uS").theDateArray(0) == US );

        ASSERT( gg(&mL, "~xT").theTimeArray(0) == XT );
        ASSERT( gg(&mL, "~yT").theTimeArray(0) == YT );
        ASSERT( gg(&mL, "~uT").theTimeArray(0) == UT );

        ASSERT( gg(&mL, "~xU").theList(0) == XU );
        ASSERT( gg(&mL, "~yU").theList(0) == YU );
        ASSERT( gg(&mL, "~uU").theList(0) == UU );

        ASSERT( gg(&mL, "~xV").theTable(0) == XV );
        ASSERT( gg(&mL, "~yV").theTable(0) == YV );
        ASSERT( gg(&mL, "~uV").theTable(0) == UV );

        if (verbose) cout << "\tLarge 'spec'." << endl;
        const int NUM_PASSES = 10;
        const int SPEC_SIZE  = 44 * NUM_PASSES + 31;
        char bigSpec[SPEC_SIZE] = "~~xDxCxBxA~~yDyCyByA~uDuCuBuA~";
        for (int j = 0; j < NUM_PASSES; ++j) {
            strcat(bigSpec, "xAxBxCxDxExFxGxHxIxJxKxLxMxNxOxPxQxRxSxTxUxV");
            gg(&mL, bigSpec);
            LOOP_ASSERT( j, 22 * (j + 1) == mL.length() );
            LOOP_ASSERT( j, XA  == mL.theChar(22 * j) );
            LOOP_ASSERT( j, XV  == mL.theTable(22 * (j + 1) - 1) );
        }

        if (verbose) cout << "\nTesting error reporting in 'ggg'." << endl;
        static const struct {
            int         d_lineNum;   // source line number
            const char *d_spec;      // input 'spec' string for 'gg'
            int         d_errorCode; // expected 'enum' error code
        } E_DATA[] = {
            // line    spec                error code
            // ----    ----                ----------
            {   L_,    "."             ,   GGG_INAPPROPRIATE_CHARACTER, },
            {   L_,    ","             ,   GGG_INAPPROPRIATE_CHARACTER, },
            {   L_,    "z"             ,   GGG_INAPPROPRIATE_CHARACTER, },
            {   L_,    ";"             ,   GGG_INAPPROPRIATE_CHARACTER, },
            {   L_,    "A"             ,   GGG_INAPPROPRIATE_CHARACTER, },

            {   L_,    "Ax"            ,   GGG_INAPPROPRIATE_CHARACTER, },
            {   L_,    "A~"            ,   GGG_INAPPROPRIATE_CHARACTER, },

            {   L_,    "Ax;"           ,   GGG_INAPPROPRIATE_CHARACTER, },
            {   L_,    "Axx"           ,   GGG_INAPPROPRIATE_CHARACTER, },

            {   L_,    "x~"            ,   GGG_EXPECTED_TYPE_CHARACTER, },
            {   L_,    "x;"            ,   GGG_EXPECTED_TYPE_CHARACTER, },
            {   L_,    "xx"            ,   GGG_EXPECTED_TYPE_CHARACTER, },

            {   L_,    "xAx~"          ,   GGG_EXPECTED_TYPE_CHARACTER, },
            {   L_,    "xBxx"          ,   GGG_EXPECTED_TYPE_CHARACTER, },
            {   L_,    "xxxC"          ,   GGG_EXPECTED_TYPE_CHARACTER, },
            {   L_,    "x;xD"          ,   GGG_EXPECTED_TYPE_CHARACTER, },

            {   L_,    ""              ,   GGG_SUCCESS,                 },
            {   L_,    "~xAyB~xCyDuE~" ,   GGG_SUCCESS,                 }
        };
        int numEData = sizeof E_DATA / sizeof E_DATA[0]; // E_DATA[] length
        for (int k = 0; k < numEData; ++k) {
            const int LINE = E_DATA[k].d_lineNum;
            List mL(Z);
            const int SHOW_ERRORS = veryVerbose;
            const int ERROR_CODE  = ggg(&mL, E_DATA[k].d_spec, SHOW_ERRORS);
            LOOP_ASSERT(LINE, ERROR_CODE == E_DATA[k].d_errorCode);
        }

        // verify the operation of the areEqual helper function.

        if (verbose) cout << "\nTesting 'areEqual' helper function:" << endl;
        static const struct {
            int          d_lineNum;  // source line number
            const char  *d_lspec;    // lhs spec
            int          d_lpos;     // lhs position
            const char  *d_rspec;    // rhs spec
            int          d_rpos;     // rhs position
            int          d_result;   // result
        } Q_DATA[] = {
            {  L_,  "xA",         0,     "xA",          0,     1     },
            {  L_,  "xA",         0,     "yA",          0,     0     },
            {  L_,  "xA",         0,     "uA",          0,     0     },
            {  L_,  "uA",         0,     "xA",          0,     0     },
            {  L_,  "xA",         0,     "xB",          0,     0     },
            {  L_,  "xB",         0,     "xB",          0,     1     },
            {  L_,  "xC",         0,     "xC",          0,     1     },
            {  L_,  "xD",         0,     "xD",          0,     1     },
            {  L_,  "xE",         0,     "xE",          0,     1     },
            {  L_,  "xAxA",       0,     "xAxB",        0,     1     },
            {  L_,  "xAxA",       0,     "xAxB",        1,     0     },
            {  L_,  "xAxA",       1,     "xAxB",        0,     1     },
            {  L_,  "xAxA",       1,     "xAxB",        1,     0     },
            {  L_,  "xBxA",       0,     "xAxB",        0,     0     },
            {  L_,  "xBxA",       0,     "xAxB",        1,     1     },
            {  L_,  "xBxA",       1,     "xAxB",        0,     1     },
            {  L_,  "xBxA",       1,     "xAxB",        1,     0     },
            {  L_,  "xAxA",       0,     "xAyA",        0,     1     },
            {  L_,  "xAxA",       0,     "xAyA",        1,     0     },
            {  L_,  "xAxA",       1,     "xAyA",        0,     1     },
            {  L_,  "xAxA",       1,     "xAyA",        1,     0     },
            {  L_,  "yAxA",       0,     "xAyA",        0,     0     },
            {  L_,  "yAxA",       0,     "xAyA",        1,     1     },
            {  L_,  "yAxA",       1,     "xAyA",        0,     1     },
            {  L_,  "yAxA",       1,     "xAyA",        1,     0     },
            {  L_,  "xAxBxC",     0,     "xCxAxB",      0,     0     },
            {  L_,  "xAxBxC",     0,     "xCxAxB",      1,     1     },
            {  L_,  "xAxBxC",     2,     "xCxAxB",      0,     1     },
            {  L_,  "xAxBxC",     1,     "xCxAxB",      2,     1     },
            {  L_,  "xAxBxC",     2,     "xCxAxB",      2,     0     },
        };

        int numQData = sizeof Q_DATA / sizeof Q_DATA[0]; // Q_DATA[] length
        for (int m = 0; m < numQData; ++m) {
            const int LINE = Q_DATA[m].d_lineNum;

            List mL(Z);
            gg(&mL, Q_DATA[m].d_lspec);
            List mR(Z);
            gg(&mR, Q_DATA[m].d_rspec);

            LOOP_ASSERT(LINE, areEqual (mL.row(), Q_DATA[m].d_lpos,
                                        mR.row(), Q_DATA[m].d_rpos) ==
                                        Q_DATA[m].d_result);
        }
      }

DEFINE_TEST_CASE(2) {
        // --------------------------------------------------------------------
        // BREATHING TEST FOR 'bdem_Table'
        // Concerns:
        //   The Breathing Test concerns for 'bdem_Table' are similar to those
        //   for 'bdem_List'.
        //
        // Plan:
        //   See the 12-step Test Plan for 'bdem_List' in case 1 below,
        //   substituting "T" for "L" and the two accessors 'numRows' and
        //   'numColumns' for 'length' as appropriate.
        //
        // Testing:
        //   This breathing test exercises basic functionality as a preliminary
        //   to the thorough testing done in subsequent cases.
        //
        //   bdem_Table::~bdem_Table()
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << "\nBreathing Test -- 'bdem_Table':"
                          << "\n===============================" << endl;

        // **************** 12-step breathing test for a Table ****************

        //  1-T) Create (empty) mT1 using default constructor
        //          a) 0   == mT1.numRows()  &&  0 == mT1.numColumns()
        //          b) mT1 == mT1
        if (verbose) cout << "\nTesting creating an empty table 'mT1'"
                             " using default constructor:" << endl;

        Table mT1(Z);  const Table& T1 = mT1;

        ASSERT(0 == T1.numRows());  ASSERT(0 == T1.numColumns());
        ASSERT(1 == (T1 == T1));    ASSERT(0 == (T1 != T1));

        //  2-T) Create (empty) mT2 using copy c'tor from mT1
        //          a) 0   == mT1.numRows()  &&  0 == mT1.numColumns()
        //          b) mT2 == mT1
        if (verbose) cout <<
            "\nTesting copy c'tor from empty table, 't2(mT1)':" << endl;

        Table mT2(T1, Z);  const Table& T2 = mT2;

        ASSERT(0 == T2.numRows());  ASSERT(0 == T2.numColumns());
        ASSERT(1 == (T2 == T2));    ASSERT(0 == (T2 != T2));
        ASSERT(1 == (T2 == T1));    ASSERT(0 == (T2 != T1));

        //  3-T) Append 22 columns to mT1, then append a row
        //          a)   0 == mT1.numRows() && 22 == mT1.numColumns()
        //          b) mT1 != mT2
        //          c)   1 == mT1.numRows() && 22 == mT1.numColumns()
        if (verbose) cout <<
            "\nTesting appending some columns and rows to 'mT1':" << endl;

        const bdem_ElemType::Type t1Types[] = {
            bdem_ElemType::BDEM_CHAR,
            bdem_ElemType::BDEM_SHORT,
            bdem_ElemType::BDEM_INT,
            bdem_ElemType::BDEM_INT64,
            bdem_ElemType::BDEM_FLOAT,
            bdem_ElemType::BDEM_DOUBLE,
            bdem_ElemType::BDEM_STRING,
            bdem_ElemType::BDEM_DATETIME,
            bdem_ElemType::BDEM_DATE,
            bdem_ElemType::BDEM_TIME,
            bdem_ElemType::BDEM_CHAR_ARRAY,
            bdem_ElemType::BDEM_SHORT_ARRAY,
            bdem_ElemType::BDEM_INT_ARRAY,
            bdem_ElemType::BDEM_INT64_ARRAY,
            bdem_ElemType::BDEM_FLOAT_ARRAY,
            bdem_ElemType::BDEM_DOUBLE_ARRAY,
            bdem_ElemType::BDEM_STRING_ARRAY,
            bdem_ElemType::BDEM_DATETIME_ARRAY,
            bdem_ElemType::BDEM_DATE_ARRAY,
            bdem_ElemType::BDEM_TIME_ARRAY,
            bdem_ElemType::BDEM_LIST,
            bdem_ElemType::BDEM_TABLE,

            bdem_ElemType::BDEM_BOOL,
            bdem_ElemType::BDEM_DATETIMETZ,
            bdem_ElemType::BDEM_DATETZ,
            bdem_ElemType::BDEM_TIMETZ,
            bdem_ElemType::BDEM_BOOL_ARRAY,
            bdem_ElemType::BDEM_DATETIMETZ_ARRAY,
            bdem_ElemType::BDEM_DATETZ_ARRAY,
            bdem_ElemType::BDEM_TIMETZ_ARRAY,
            bdem_ElemType::BDEM_CHOICE,
            bdem_ElemType::BDEM_CHOICE_ARRAY
        };

        const int numt1Types = NUM_ARRYELEMS(t1Types);

        mT1.reset(t1Types, numt1Types);

        ASSERT(0 == T1.numRows());  ASSERT(NUM_TYPES == T1.numColumns());
        ASSERT(1 == (T1 == T1));    ASSERT( 0 == (T1 != T1));
        ASSERT(0 == (T1 == T2));    ASSERT( 1 == (T1 != T2));

        my_ElemTypeArray e1;
        getElemTypes(&e1, LX);
        my_ElemTypeArray e2;
        getElemTypes(&e2, T1);
        ASSERT(e1 == e2);

        mT1.appendRow(LX);

        ASSERT(1 == T1.numRows());  ASSERT(NUM_TYPES == T1.numColumns());
        ASSERT(1 == (T1 == T1));    ASSERT( 0 == (T1 != T1));
        ASSERT(0 == (T1 == T2));    ASSERT( 1 == (T1 != T2));

        ASSERT(LX.row() == T1[0]);

        ASSERT(XA == T1[0][0].theChar());
        ASSERT(XB == T1[0][1].theShort());
        ASSERT(XC == T1[0][2].theInt());
        ASSERT(XD == T1[0][3].theInt64());
        ASSERT(XE == T1[0][4].theFloat());
        ASSERT(XF == T1[0][5].theDouble());
        ASSERT(XG == T1[0][6].theString());
        ASSERT(XH == T1[0][7].theDatetime());
        ASSERT(XI == T1[0][8].theDate());
        ASSERT(XJ == T1[0][9].theTime());
        ASSERT(XK == T1[0][10].theCharArray());
        ASSERT(XL == T1[0][11].theShortArray());
        ASSERT(XM == T1[0][12].theIntArray());
        ASSERT(XN == T1[0][13].theInt64Array());
        ASSERT(XO == T1[0][14].theFloatArray());
        ASSERT(XP == T1[0][15].theDoubleArray());
        ASSERT(XQ == T1[0][16].theStringArray());
        ASSERT(XR == T1[0][17].theDatetimeArray());
        ASSERT(XS == T1[0][18].theDateArray());
        ASSERT(XT == T1[0][19].theTimeArray());
        ASSERT(XU == T1[0][20].theList());
        ASSERT(XV == T1[0][21].theTable());

        ASSERT(XW == T1[0][22].theBool());
        ASSERT(XX == T1[0][23].theDatetimeTz());
        ASSERT(XY == T1[0][24].theDateTz());
        ASSERT(XZ == T1[0][25].theTimeTz());
        ASSERT(Xa == T1[0][26].theBoolArray());
        ASSERT(Xb == T1[0][27].theDatetimeTzArray());
        ASSERT(Xc == T1[0][28].theDateTzArray());
        ASSERT(Xd == T1[0][29].theTimeTzArray());
        ASSERT(Xe == T1[0][30].theChoice());
        ASSERT(Xf == T1[0][31].theChoiceArray());

        //  4-T) Append the same 22 columns to mT2
        //          a)   0 == mT2.numRows() && 22 == mT2.numColumns()
        //          b) mT1 == mT2
        if (verbose) cout <<
            "\nTesting appending the same columns and rows to 'mT2':" << endl;

        mT2.reset(t1Types, numt1Types);

        ASSERT(0 == T2.numRows());  ASSERT(NUM_TYPES == T2.numColumns());
        ASSERT(1 == (T2 == T2));    ASSERT( 0 == (T2 != T2));
        ASSERT(0 == (T2 == T1));    ASSERT( 1 == (T2 != T1));

        mT2.appendRow(LX);

        ASSERT(1 == T2.numRows());  ASSERT(NUM_TYPES == T2.numColumns());
        ASSERT(1 == (T2 == T2));    ASSERT( 0 == (T2 != T2));
        ASSERT(1 == (T2 == T1));    ASSERT( 0 == (T2 != T1));

        ASSERT(LX.row() == T2[0]);

        ASSERT(XA == T2[0][0].theChar());
        ASSERT(XB == T2[0][1].theShort());
        ASSERT(XC == T2[0][2].theInt());
        ASSERT(XD == T2[0][3].theInt64());
        ASSERT(XE == T2[0][4].theFloat());
        ASSERT(XF == T2[0][5].theDouble());
        ASSERT(XG == T2[0][6].theString());
        ASSERT(XH == T2[0][7].theDatetime());
        ASSERT(XI == T2[0][8].theDate());
        ASSERT(XJ == T2[0][9].theTime());
        ASSERT(XK == T2[0][10].theCharArray());
        ASSERT(XL == T2[0][11].theShortArray());
        ASSERT(XM == T2[0][12].theIntArray());
        ASSERT(XN == T2[0][13].theInt64Array());
        ASSERT(XO == T2[0][14].theFloatArray());
        ASSERT(XP == T2[0][15].theDoubleArray());
        ASSERT(XQ == T2[0][16].theStringArray());
        ASSERT(XR == T2[0][17].theDatetimeArray());
        ASSERT(XS == T2[0][18].theDateArray());
        ASSERT(XT == T2[0][19].theTimeArray());
        ASSERT(XU == T2[0][20].theList());
        ASSERT(XV == T2[0][21].theTable());

        ASSERT(XW == T2[0][22].theBool());
        ASSERT(XX == T2[0][23].theDatetimeTz());
        ASSERT(XY == T2[0][24].theDateTz());
        ASSERT(XZ == T2[0][25].theTimeTz());
        ASSERT(Xa == T2[0][26].theBoolArray());
        ASSERT(Xb == T2[0][27].theDatetimeTzArray());
        ASSERT(Xc == T2[0][28].theDateTzArray());
        ASSERT(Xd == T2[0][29].theTimeTzArray());
        ASSERT(Xe == T2[0][30].theChoice());
        ASSERT(Xf == T2[0][31].theChoiceArray());

        //  5-T) Empty mT1 with mT1.removeAll()
        //          a)   0 == mT1.numRows() && 0 == mT1.numColumns()
        //          b) mT1 != mT2
        if (verbose) cout <<
            "\nTesting emptying table 'mT1' with 'mT1.removeAll()':" << endl;

        mT1.removeAll();

        ASSERT(0 == T1.numRows());  ASSERT(0 == T1.numColumns());
        ASSERT(1 == (T1 == T1));    ASSERT(0 == (T1 != T1));
        ASSERT(0 == (T1 == T2));    ASSERT(1 == (T1 != T2));

        //  6-T) Re-create mT1 as in Test 3-T, then empty again
        //          a)   1 == mT1.numRows() && 22 == mT1.numColumns()
        //          b) mT1 == mT2
        //          c)   0 == mT1.numRows() && 22 == mT1.numColumns()
        //          d) mT1 != mT2

        if (verbose) cout <<  "\nTesting refilling table 'mT1' as before "
                              "and then emptying it again:" << endl;

        mT1.reset(t1Types, numt1Types);

        ASSERT(0 == T1.numRows());  ASSERT(NUM_TYPES == T1.numColumns());
        ASSERT(1 == (T1 == T1));    ASSERT( 0 == (T1 != T1));
        ASSERT(0 == (T1 == T2));    ASSERT( 1 == (T1 != T2));

        mT1.appendRow(LX);

        ASSERT(1 == T1.numRows());  ASSERT(NUM_TYPES == T1.numColumns());
        ASSERT(1 == (T1 == T1));    ASSERT( 0 == (T1 != T1));
        ASSERT(1 == (T1 == T2));    ASSERT( 0 == (T1 != T2));

        ASSERT(LX.row() == T1[0]);

        ASSERT(XA == T1[0][0].theChar());
        ASSERT(XB == T1[0][1].theShort());
        ASSERT(XC == T1[0][2].theInt());
        ASSERT(XD == T1[0][3].theInt64());
        ASSERT(XE == T1[0][4].theFloat());
        ASSERT(XF == T1[0][5].theDouble());
        ASSERT(XG == T1[0][6].theString());
        ASSERT(XH == T1[0][7].theDatetime());
        ASSERT(XI == T1[0][8].theDate());
        ASSERT(XJ == T1[0][9].theTime());
        ASSERT(XK == T1[0][10].theCharArray());
        ASSERT(XL == T1[0][11].theShortArray());
        ASSERT(XM == T1[0][12].theIntArray());
        ASSERT(XN == T1[0][13].theInt64Array());
        ASSERT(XO == T1[0][14].theFloatArray());
        ASSERT(XP == T1[0][15].theDoubleArray());
        ASSERT(XQ == T1[0][16].theStringArray());
        ASSERT(XR == T1[0][17].theDatetimeArray());
        ASSERT(XS == T1[0][18].theDateArray());
        ASSERT(XT == T1[0][19].theTimeArray());
        ASSERT(XU == T1[0][20].theList());
        ASSERT(XV == T1[0][21].theTable());

        ASSERT(XW == T1[0][22].theBool());
        ASSERT(XX == T1[0][23].theDatetimeTz());
        ASSERT(XY == T1[0][24].theDateTz());
        ASSERT(XZ == T1[0][25].theTimeTz());
        ASSERT(Xa == T1[0][26].theBoolArray());
        ASSERT(Xb == T1[0][27].theDatetimeTzArray());
        ASSERT(Xc == T1[0][28].theDateTzArray());
        ASSERT(Xd == T1[0][29].theTimeTzArray());
        ASSERT(Xe == T1[0][30].theChoice());
        ASSERT(Xf == T1[0][31].theChoiceArray());

        mT1.removeAll();

        ASSERT(0 == T1.numRows());  ASSERT(0 == T1.numColumns());
        ASSERT(1 == (T1 == T1));    ASSERT(0 == (T1 != T1));
        ASSERT(0 == (T1 == T2));    ASSERT(1 == (T1 != T2));

        //  7-T) Create empty mT3 using default constructor
        //          a)   0 == mT3.numRows() && 0 == mT3.numColumns()
        //          b) mT3 == mT1
        //             mT3 != mT2
        if (verbose) cout <<  "\nCreating another empty table, 'mT3', "
                          << "using default c'tor:" << endl;

        Table mT3(Z);  const Table& T3 = mT3;

        ASSERT(0 == T3.numRows());  ASSERT(0 == T3.numColumns());
        ASSERT(1 == (T3 == T3));    ASSERT(0 == (T3 != T3));
        ASSERT(1 == (T3 == T1));    ASSERT(0 == (T3 != T1));
        ASSERT(0 == (T3 == T2));    ASSERT(1 == (T3 != T2));

        //  8-T) Create Table mT4(mT2)
        //          a)   1 == mT4.numRows() && 22 == mT4.numColumns()
        //          b) mT4 == mT2
        //             mT4 != mT1
        //             mT4 != mT3
        if (verbose) cout <<
            "\nTesting copy c'tor from non-empty table 'mT4(mT2)':" << endl;

        Table mT4(T2, Z);  const Table& T4 = mT4;

        ASSERT(1 == T4.numRows());  ASSERT(NUM_TYPES == T4.numColumns());
        ASSERT(1 == (T4 == T4));    ASSERT( 0 == (T4 != T4));
        ASSERT(1 == (T4 == T2));    ASSERT( 0 == (T4 != T2));
        ASSERT(0 == (T4 == T1));    ASSERT( 1 == (T4 != T1));
        ASSERT(0 == (T4 == T3));    ASSERT( 1 == (T4 != T3));

        ASSERT(LX.row() == T4[0]);

        ASSERT(XA == T4[0][0].theChar());
        ASSERT(XB == T4[0][1].theShort());
        ASSERT(XC == T4[0][2].theInt());
        ASSERT(XD == T4[0][3].theInt64());
        ASSERT(XE == T4[0][4].theFloat());
        ASSERT(XF == T4[0][5].theDouble());
        ASSERT(XG == T4[0][6].theString());
        ASSERT(XH == T4[0][7].theDatetime());
        ASSERT(XI == T4[0][8].theDate());
        ASSERT(XJ == T4[0][9].theTime());
        ASSERT(XK == T4[0][10].theCharArray());
        ASSERT(XL == T4[0][11].theShortArray());
        ASSERT(XM == T4[0][12].theIntArray());
        ASSERT(XN == T4[0][13].theInt64Array());
        ASSERT(XO == T4[0][14].theFloatArray());
        ASSERT(XP == T4[0][15].theDoubleArray());
        ASSERT(XQ == T4[0][16].theStringArray());
        ASSERT(XR == T4[0][17].theDatetimeArray());
        ASSERT(XS == T4[0][18].theDateArray());
        ASSERT(XT == T4[0][19].theTimeArray());
        ASSERT(XU == T4[0][20].theList());
        ASSERT(XV == T4[0][21].theTable());

        ASSERT(XW == T4[0][22].theBool());
        ASSERT(XX == T4[0][23].theDatetimeTz());
        ASSERT(XY == T4[0][24].theDateTz());
        ASSERT(XZ == T4[0][25].theTimeTz());
        ASSERT(Xa == T4[0][26].theBoolArray());
        ASSERT(Xb == T4[0][27].theDatetimeTzArray());
        ASSERT(Xc == T4[0][28].theDateTzArray());
        ASSERT(Xd == T4[0][29].theTimeTzArray());
        ASSERT(Xe == T4[0][30].theChoice());
        ASSERT(Xf == T4[0][31].theChoiceArray());

        //  9-T) mT1 = mT2, testing assignment of longer table to shorter
        //          a)   1 == mT1.numRows() && 22 == mT1.numColumns()
        //          b) mT1 == mT2
        //             mT1 == mT4
        //             mT1 != mT3

        if (verbose) cout <<
            "\nTesting assignment, longer to shorter, 'mT1 = mT2':" << endl;

        mT1 = T2;

        ASSERT(1 == T1.numRows());  ASSERT(NUM_TYPES == T1.numColumns());
        ASSERT(1 == (T1 == T1));    ASSERT( 0 == (T1 != T1));
        ASSERT(1 == (T1 == T2));    ASSERT( 0 == (T1 != T2));
        ASSERT(1 == (T1 == T4));    ASSERT( 0 == (T1 != T4));
        ASSERT(0 == (T1 == T3));    ASSERT( 1 == (T1 != T3));

        ASSERT(LX.row() == T1[0]);

        ASSERT(XA == T1[0][0].theChar());
        ASSERT(XB == T1[0][1].theShort());
        ASSERT(XC == T1[0][2].theInt());
        ASSERT(XD == T1[0][3].theInt64());
        ASSERT(XE == T1[0][4].theFloat());
        ASSERT(XF == T1[0][5].theDouble());
        ASSERT(XG == T1[0][6].theString());
        ASSERT(XH == T1[0][7].theDatetime());
        ASSERT(XI == T1[0][8].theDate());
        ASSERT(XJ == T1[0][9].theTime());
        ASSERT(XK == T1[0][10].theCharArray());
        ASSERT(XL == T1[0][11].theShortArray());
        ASSERT(XM == T1[0][12].theIntArray());
        ASSERT(XN == T1[0][13].theInt64Array());
        ASSERT(XO == T1[0][14].theFloatArray());
        ASSERT(XP == T1[0][15].theDoubleArray());
        ASSERT(XQ == T1[0][16].theStringArray());
        ASSERT(XR == T1[0][17].theDatetimeArray());
        ASSERT(XS == T1[0][18].theDateArray());
        ASSERT(XT == T1[0][19].theTimeArray());
        ASSERT(XU == T1[0][20].theList());
        ASSERT(XV == T1[0][21].theTable());

        ASSERT(XW == T1[0][22].theBool());
        ASSERT(XX == T1[0][23].theDatetimeTz());
        ASSERT(XY == T1[0][24].theDateTz());
        ASSERT(XZ == T1[0][25].theTimeTz());
        ASSERT(Xa == T1[0][26].theBoolArray());
        ASSERT(Xb == T1[0][27].theDatetimeTzArray());
        ASSERT(Xc == T1[0][28].theDateTzArray());
        ASSERT(Xd == T1[0][29].theTimeTzArray());
        ASSERT(Xe == T1[0][30].theChoice());
        ASSERT(Xf == T1[0][31].theChoiceArray());

        //    10-T) mT4 = mT3, testing assignment of shorter table to longer
        //          a)   0 == mT4.numRows() && 0 == mT4.numColumns()
        //          b) mT4 == mT3
        //             mT4 != mT1
        //             mT4 != mT2

        if (verbose) cout <<
            "\nTesting assignment, shorter to longer, 'mT4 = mT3':" << endl;

        mT4 = T3;

        ASSERT(0 == T4.numRows());  ASSERT(0 == T4.numColumns());
        ASSERT(1 == (T4 == T4));    ASSERT(0 == (T4 != T4));
        ASSERT(1 == (T4 == T3));    ASSERT(0 == (T4 != T3));
        ASSERT(0 == (T4 == T1));    ASSERT(1 == (T4 != T1));
        ASSERT(0 == (T4 == T2));    ASSERT(1 == (T4 != T2));

        //  11-T) mT2 = mT2, testing self-assignment
        //          a)   1 == mT2.numRows() && 22 == mT2.numColumns()
        //          b) mT2 == mT2
        //             mT2 == mT1
        //             mT2 != mT3
        //             mT2 != mT4

        if (verbose) cout << "\nTesting self-assignment, 'mT2 = mT2':" << endl;

        mT2 = T2;

        ASSERT(1 == T2.numRows());  ASSERT(NUM_TYPES == T2.numColumns());
        ASSERT(1 == (T2 == T2));    ASSERT( 0 == (T2 != T2));
        ASSERT(1 == (T2 == T1));    ASSERT( 0 == (T2 != T1));
        ASSERT(0 == (T2 == T3));    ASSERT( 1 == (T2 != T3));
        ASSERT(0 == (T2 == T4));    ASSERT( 1 == (T2 != T4));

        ASSERT(LX.row() == T2[0]);

        ASSERT(XA == T2[0][0].theChar());
        ASSERT(XB == T2[0][1].theShort());
        ASSERT(XC == T2[0][2].theInt());
        ASSERT(XD == T2[0][3].theInt64());
        ASSERT(XE == T2[0][4].theFloat());
        ASSERT(XF == T2[0][5].theDouble());
        ASSERT(XG == T2[0][6].theString());
        ASSERT(XH == T2[0][7].theDatetime());
        ASSERT(XI == T2[0][8].theDate());
        ASSERT(XJ == T2[0][9].theTime());
        ASSERT(XK == T2[0][10].theCharArray());
        ASSERT(XL == T2[0][11].theShortArray());
        ASSERT(XM == T2[0][12].theIntArray());
        ASSERT(XN == T2[0][13].theInt64Array());
        ASSERT(XO == T2[0][14].theFloatArray());
        ASSERT(XP == T2[0][15].theDoubleArray());
        ASSERT(XQ == T2[0][16].theStringArray());
        ASSERT(XR == T2[0][17].theDatetimeArray());
        ASSERT(XS == T2[0][18].theDateArray());
        ASSERT(XT == T2[0][19].theTimeArray());
        ASSERT(XU == T2[0][20].theList());
        ASSERT(XV == T2[0][21].theTable());

        ASSERT(XW == T2[0][22].theBool());
        ASSERT(XX == T2[0][23].theDatetimeTz());
        ASSERT(XY == T2[0][24].theDateTz());
        ASSERT(XZ == T2[0][25].theTimeTz());
        ASSERT(Xa == T2[0][26].theBoolArray());
        ASSERT(Xb == T2[0][27].theDatetimeTzArray());
        ASSERT(Xc == T2[0][28].theDateTzArray());
        ASSERT(Xd == T2[0][29].theTimeTzArray());
        ASSERT(Xe == T2[0][30].theChoice());
        ASSERT(Xf == T2[0][31].theChoiceArray());

        //  12-T) stream out mT2, and then stream same data back in to mT4
        //          a)   1 == mT4.numRows() && 22 == mT4.numColumns()
        //          b) mT4 == mT2
        //             mT4 == mT1
        //             mT4 != mT3
        //             mT4 == mT4

        if (verbose) cout << "\nTesting basic streaming functionality: "
                          << endl;

        bdex_TestOutStream out;
        const int TABLE_MAX_VERSION = Table::maxSupportedBdexVersion();
        bdex_OutStreamFunctions::streamOut(out,
                                           T2,
                                           TABLE_MAX_VERSION);

        const char *OD  = out.data();
        const int   LOD = out.length();

        bdex_TestInStream in(OD, LOD);
        ASSERT(in);

        bdex_InStreamFunctions::streamIn(in,
                                         mT4,
                                         TABLE_MAX_VERSION);
        ASSERT(in);

        ASSERT(1 == T4.numRows());  ASSERT(NUM_TYPES == T4.numColumns());
        ASSERT(1 == (T4 == T2));    ASSERT( 0 == (T4 != T2));
        ASSERT(1 == (T4 == T1));    ASSERT( 0 == (T4 != T1));
        ASSERT(0 == (T4 == T3));    ASSERT( 1 == (T4 != T3));
        ASSERT(1 == (T4 == T4));    ASSERT( 0 == (T4 != T4));

        ASSERT(LX.row() == T4[0]);

        ASSERT(XA == T4[0][0].theChar());
        ASSERT(XB == T4[0][1].theShort());
        ASSERT(XC == T4[0][2].theInt());
        ASSERT(XD == T4[0][3].theInt64());
        ASSERT(XE == T4[0][4].theFloat());
        ASSERT(XF == T4[0][5].theDouble());
        ASSERT(XG == T4[0][6].theString());
        ASSERT(XH == T4[0][7].theDatetime());
        ASSERT(XI == T4[0][8].theDate());
        ASSERT(XJ == T4[0][9].theTime());
        ASSERT(XK == T4[0][10].theCharArray());
        ASSERT(XL == T4[0][11].theShortArray());
        ASSERT(XM == T4[0][12].theIntArray());
        ASSERT(XN == T4[0][13].theInt64Array());
        ASSERT(XO == T4[0][14].theFloatArray());
        ASSERT(XP == T4[0][15].theDoubleArray());
        ASSERT(XQ == T4[0][16].theStringArray());
        ASSERT(XR == T4[0][17].theDatetimeArray());
        ASSERT(XS == T4[0][18].theDateArray());
        ASSERT(XT == T4[0][19].theTimeArray());
        ASSERT(XU == T4[0][20].theList());
        ASSERT(XV == T4[0][21].theTable());

        ASSERT(XW == T4[0][22].theBool());
        ASSERT(XX == T4[0][23].theDatetimeTz());
        ASSERT(XY == T4[0][24].theDateTz());
        ASSERT(XZ == T4[0][25].theTimeTz());
        ASSERT(Xa == T4[0][26].theBoolArray());
        ASSERT(Xb == T4[0][27].theDatetimeTzArray());
        ASSERT(Xc == T4[0][28].theDateTzArray());
        ASSERT(Xd == T4[0][29].theTimeTzArray());
        ASSERT(Xe == T4[0][30].theChoice());
        ASSERT(Xf == T4[0][31].theChoiceArray());
      }

DEFINE_TEST_CASE(1) {
        // --------------------------------------------------------------------
        // BREATHING TEST FOR 'bdem_List'
        // Concerns:
        //   We are concerned that the basic (value-semantic) functionality of
        //   'bdem_List' works properly.  We are also concerned that the
        //   somewhat customized proctor objects implemented in the '.cpp' of
        //   this component do their job.  So we perform 'bdema' exception
        //   testing in this Breathing Test.  //## TM: THIS IS AN ASSUMPTION!
        //                                    //##     MODIFY AS NEEDED.
        //
        // Plan:
        //   We test basic functionality in 12 steps, inspired by the
        //   'bdem_Schema' breathing test, which is in turn inspired by
        //   'bsl::vector<double>'.
        //
        //   We will use 4 objects for our tests: mL1, mL2, mL3, and mL4
        //
        //   After each step, we will validate that
        //      a) the list we just modified is in the expected state, by
        //         checking bdem_List::length(), and validating using the
        //         accessors.
        //      b) the object is equal/not equal to the objects already built
        //         at that step, and equal to itself, as we would expect.
        //
        //   The tests:
        //      1) Create empty mL1 using default constructor
        //              a)   0 == mL1.length()
        //              b) mL1 == mL1
        //      2) Create mL2 using copy c'tor from mL1
        //              a)   0 == mL2.length()
        //              b) mL2 == mL1
        //      3) Append 22 appropriate items to mL1
        //              a)  22 == mL1.length()
        //              b) mL1 != mL2
        //      4) Append same items to mL2
        //              a)  22 == mL2.length()
        //              b) mL1 == mL2
        //      5) mL1.removeAll()
        //              a)   0 == mL1.length()
        //              b) mL1 != mL2
        //      6) Re-append same 22 items to mL1 and re-remove them
        //              a)  22 == mL1.length()
        //                 mL1 == mL2
        //              b)   0 == mL1.length()
        //                 mL1 != mL2
        //      7) Create empty mL3 using default c'tor
        //              a)   0 == mL3.length()
        //              b) mL3 == mL1
        //                 mL3 != mL2
        //      8) Create mL4(mL2)
        //              a)  22 == mL4.length()
        //              b) mL4 == mL2
        //                 mL4 != mL1
        //                 mL4 != mL3
        //      9) mL1 = mL2, testing assignment of longer object to shorter
        //              a)  22 == mL1.length()
        //              b) mL1 == mL2
        //                 mL1 == mL4
        //                 mL1 != mL3
        //     10) mL4 = mL3, testing assignment of shorter object to longer
        //              a)   0 == mL4.length()
        //              b) mL4 == mL3
        //                 mL4 != mL1
        //                 mL4 != mL2
        //     11) mL2 = mL2, testing self-assignment
        //              a)  22 == mL2.length()
        //              b) mL2 == mL2
        //                 mL2 == mL1
        //                 mL2 != mL3
        //                 mL2 != mL4
        //     12) stream mL2 out, and then stream same data back in to mL4
        //              a)  22 == mL4.length()
        //              b) mL4 == mL2
        //                 mL4 == mL1
        //                 mL4 != mL3
        //                 mL4 == mL4
        //
        // Testing:
        //   This breathing test exercises basic functionality as a preliminary
        //   to the thorough testing done in subsequent cases.
        //
        //   bdem_List::~bdem_List()
        // --------------------------------------------------------------------

        DECLARE_TEST_ALLOCATOR
        DECLARE_MAIN_VARIABLES

        if (verbose) cout << "\nBreathing Test -- 'bdem_List':"
                          << "\n==============================" << endl;

        // ***************** 12-step breathing test for a List ****************

        //  1-L) Create (empty) mL1 using default constructor
        //          a) 0   == mL2.length()
        //          b) mL1 == mL1
        if (verbose) cout <<  "\nTesting creating an empty list 'L1' "
                              "using default constructor:" << endl;

        List mL1(Z);  const List& L1 = mL1;

        ASSERT(0 == L1.length());
        ASSERT(1 == (L1 == L1));   ASSERT(0 == (L1 != L1));

        //  2-L) Create (empty) mL2 using copy c'tor from mL1
        //          a) 0   == mL2.length()
        //          b) mL2 == mL1
        if (verbose) cout <<
            "\nTesting copy c'tor from empty list, 'mL2(mL1)':" << endl;

        List mL2(mL1, Z);  const List& L2 = mL2;

        ASSERT(0 == L2.length());
        ASSERT(1 == (L2 == L2));   ASSERT(0 == (L2 != L2));
        ASSERT(1 == (L2 == L1));   ASSERT(0 == (L2 != L1));

        //  3-L) Append 22 elements to mL1
        //          a) 22  == mL1.length()
        //          b) mL1 != mL2
        if (verbose) cout <<
            "\nTesting appending the 22 elements to 'mL1' directly:" << endl;

        mL1.appendChar(XA);
        mL1.appendShort(XB);
        mL1.appendInt(XC);
        mL1.appendInt64(XD);
        mL1.appendFloat(XE);
        mL1.appendDouble(XF);
        mL1.appendString(XG);
        mL1.appendDatetime(XH);
        mL1.appendDate(XI);
        mL1.appendTime(XJ);
        mL1.appendCharArray(XK);
        mL1.appendShortArray(XL);
        mL1.appendIntArray(XM);
        mL1.appendInt64Array(XN);
        mL1.appendFloatArray(XO);
        mL1.appendDoubleArray(XP);
        mL1.appendStringArray(XQ);
        mL1.appendDatetimeArray(XR);
        mL1.appendDateArray(XS);
        mL1.appendTimeArray(XT);
        mL1.appendList(XU);
        mL1.appendTable(XV);

        mL1.appendBool(XW);
        mL1.appendDatetimeTz(XX);
        mL1.appendDateTz(XY);
        mL1.appendTimeTz(XZ);
        mL1.appendBoolArray(Xa);
        mL1.appendDatetimeTzArray(Xb);
        mL1.appendDateTzArray(Xc);
        mL1.appendTimeTzArray(Xd);
        mL1.appendChoice(Xe);
        mL1.appendChoiceArray(Xf);

        ASSERT(NUM_TYPES == L1.length());
        ASSERT( 1 == (L1 == L1));   ASSERT(0 == (L1 != L1));
        ASSERT( 0 == (L1 == L2));   ASSERT(1 == (L1 != L2));

        ASSERT(LX == L1);

        ASSERT(XA == L1.theChar(0));
        ASSERT(XB == L1.theShort(1));
        ASSERT(XC == L1.theInt(2));
        ASSERT(XD == L1.theInt64(3));
        ASSERT(XE == L1.theFloat(4));
        ASSERT(XF == L1.theDouble(5));
        ASSERT(XG == L1.theString(6));
        ASSERT(XH == L1.theDatetime(7));
        ASSERT(XI == L1.theDate(8));
        ASSERT(XJ == L1.theTime(9));
        ASSERT(XK == L1.theCharArray(10));
        ASSERT(XL == L1.theShortArray(11));
        ASSERT(XM == L1.theIntArray(12));
        ASSERT(XN == L1.theInt64Array(13));
        ASSERT(XO == L1.theFloatArray(14));
        ASSERT(XP == L1.theDoubleArray(15));
        ASSERT(XQ == L1.theStringArray(16));
        ASSERT(XR == L1.theDatetimeArray(17));
        ASSERT(XS == L1.theDateArray(18));
        ASSERT(XT == L1.theTimeArray(19));
        ASSERT(XU == L1.theList(20));
        ASSERT(XV == L1.theTable(21));

        ASSERT(XW == L1.theBool(22));
        ASSERT(XX == L1.theDatetimeTz(23));
        ASSERT(XY == L1.theDateTz(24));
        ASSERT(XZ == L1.theTimeTz(25));
        ASSERT(Xa == L1.theBoolArray(26));
        ASSERT(Xb == L1.theDatetimeTzArray(27));
        ASSERT(Xc == L1.theDateTzArray(28));
        ASSERT(Xd == L1.theTimeTzArray(29));
        ASSERT(Xe == L1.theChoice(30));
        ASSERT(Xf == L1.theChoiceArray(31));

        //  4-L) Append same 22 elements to mL2
        //          a) 3   == mL2.length()
        //          b) mL1 == mL2
        if (verbose) cout << "\nTesting appending the same elements to 'mL2' "
                             "using 'appendElement':" << endl;

        for (int i = 0; i < NUM_TYPES; ++i) {
            mL2.appendElement(LX[i]);
        }

        ASSERT(NUM_TYPES == L2.length());
        ASSERT( 1 == (L2 == L2));   ASSERT(0 == (L2 != L2));
        ASSERT( 1 == (L2 == L1));   ASSERT(0 == (L2 != L1));

        ASSERT(LX == L2);

        ASSERT(XA == L1.theChar(0));
        ASSERT(XB == L1.theShort(1));
        ASSERT(XC == L1.theInt(2));
        ASSERT(XD == L1.theInt64(3));
        ASSERT(XE == L1.theFloat(4));
        ASSERT(XF == L1.theDouble(5));
        ASSERT(XG == L1.theString(6));
        ASSERT(XH == L1.theDatetime(7));
        ASSERT(XI == L1.theDate(8));
        ASSERT(XJ == L1.theTime(9));
        ASSERT(XK == L1.theCharArray(10));
        ASSERT(XL == L1.theShortArray(11));
        ASSERT(XM == L1.theIntArray(12));
        ASSERT(XN == L1.theInt64Array(13));
        ASSERT(XO == L1.theFloatArray(14));
        ASSERT(XP == L1.theDoubleArray(15));
        ASSERT(XQ == L1.theStringArray(16));
        ASSERT(XR == L1.theDatetimeArray(17));
        ASSERT(XS == L1.theDateArray(18));
        ASSERT(XT == L1.theTimeArray(19));
        ASSERT(XU == L1.theList(20));
        ASSERT(XV == L1.theTable(21));

        ASSERT(XW == L1.theBool(22));
        ASSERT(XX == L1.theDatetimeTz(23));
        ASSERT(XY == L1.theDateTz(24));
        ASSERT(XZ == L1.theTimeTz(25));
        ASSERT(Xa == L1.theBoolArray(26));
        ASSERT(Xb == L1.theDatetimeTzArray(27));
        ASSERT(Xc == L1.theDateTzArray(28));
        ASSERT(Xd == L1.theTimeTzArray(29));
        ASSERT(Xe == L1.theChoice(30));
        ASSERT(Xf == L1.theChoiceArray(31));

        //  5-L) Empty mL1 with mL1.removeAll()
        //          a) 0  == mL1.length()
        //          b) 11 != 12
        if (verbose) cout <<
            "\nTesting emptying list 'mL1' with 'mL1.removeAll()':" << endl;

        mL1.removeAll();

        ASSERT(0 == L1.length());
        ASSERT(1 == (L1 == L1));   ASSERT(0 == (L1 != L1));
        ASSERT(0 == (L1 == L2));   ASSERT(1 == (L1 != L2));

        //  6-L) Re-create mL1 as in Test 3-L, then empty again
        //          a) 1   == mL1.length()
        //          b) mL1 == mL2
        //          c) 0   == mL1.length()
        //          d) mL1 != mL2
        if (verbose) cout <<  "\nTesting refilling list 'mL1' as before "
                              "and then emptying it again:" << endl;

        for (int i = 0; i < NUM_TYPES; ++i) {
            mL1.appendElement(LX[i]);
        }

        ASSERT(NUM_TYPES == L1.length());
        ASSERT( 1 == (L1 == L1));   ASSERT(0 == (L1 != L1));
        ASSERT( 1 == (L1 == L2));   ASSERT(0 == (L1 != L2));

        ASSERT(LX == L1);

        ASSERT(XA == L1.theChar(0));
        ASSERT(XB == L1.theShort(1));
        ASSERT(XC == L1.theInt(2));
        ASSERT(XD == L1.theInt64(3));
        ASSERT(XE == L1.theFloat(4));
        ASSERT(XF == L1.theDouble(5));
        ASSERT(XG == L1.theString(6));
        ASSERT(XH == L1.theDatetime(7));
        ASSERT(XI == L1.theDate(8));
        ASSERT(XJ == L1.theTime(9));
        ASSERT(XK == L1.theCharArray(10));
        ASSERT(XL == L1.theShortArray(11));
        ASSERT(XM == L1.theIntArray(12));
        ASSERT(XN == L1.theInt64Array(13));
        ASSERT(XO == L1.theFloatArray(14));
        ASSERT(XP == L1.theDoubleArray(15));
        ASSERT(XQ == L1.theStringArray(16));
        ASSERT(XR == L1.theDatetimeArray(17));
        ASSERT(XS == L1.theDateArray(18));
        ASSERT(XT == L1.theTimeArray(19));
        ASSERT(XU == L1.theList(20));
        ASSERT(XV == L1.theTable(21));

        ASSERT(XW == L1.theBool(22));
        ASSERT(XX == L1.theDatetimeTz(23));
        ASSERT(XY == L1.theDateTz(24));
        ASSERT(XZ == L1.theTimeTz(25));
        ASSERT(Xa == L1.theBoolArray(26));
        ASSERT(Xb == L1.theDatetimeTzArray(27));
        ASSERT(Xc == L1.theDateTzArray(28));
        ASSERT(Xd == L1.theTimeTzArray(29));
        ASSERT(Xe == L1.theChoice(30));
        ASSERT(Xf == L1.theChoiceArray(31));

        mL1.removeAll();

        ASSERT(0 == L1.length());
        ASSERT(1 == (L1 == L1));   ASSERT(0 == (L1 != L1));
        ASSERT(0 == (L1 == L2));   ASSERT(1 == (L1 != L2));

        //  7-L) Create empty mL3 using default constructor
        //          a) 0   == mL3.length()
        //          b) mL3 == mL1
        //             mL3 != mL2
        if (verbose) cout <<  "\nCreating another empty list, 'mL3', "
                          << "using default c'tor:" << endl;

        List mL3(Z);  const List& L3 = mL3;

        ASSERT(0 == L3.length());
        ASSERT(1 == (L3 == L3));   ASSERT(0 == (L3 != L3));
        ASSERT(1 == (L3 == L1));   ASSERT(0 == (L3 != L1));
        ASSERT(0 == (L3 == L2));   ASSERT(1 == (L3 != L2));

        //  8-L) Create List mL4(mL2)
        //          a) 22  == mL4.length()
        //          b) mL4 == mL2
        //             mL4 != mL1
        //             mL4 != mL3
        if (verbose) cout <<
            "\nTesting copy c'tor from non-empty list 'mL4(mL2)':" << endl;

        List mL4(L2, Z);  const List& L4 = mL4;

        ASSERT(NUM_TYPES == L4.length());
        ASSERT( 1 == (L4 == L4));   ASSERT(0 == (L4 != L4));
        ASSERT( 1 == (L4 == L2));   ASSERT(0 == (L4 != L2));
        ASSERT( 0 == (L4 == L1));   ASSERT(1 == (L4 != L1));
        ASSERT( 0 == (L4 == L3));   ASSERT(1 == (L4 != L3));

        ASSERT(LX == L4);

        ASSERT(XA == L4.theChar(0));
        ASSERT(XB == L4.theShort(1));
        ASSERT(XC == L4.theInt(2));
        ASSERT(XD == L4.theInt64(3));
        ASSERT(XE == L4.theFloat(4));
        ASSERT(XF == L4.theDouble(5));
        ASSERT(XG == L4.theString(6));
        ASSERT(XH == L4.theDatetime(7));
        ASSERT(XI == L4.theDate(8));
        ASSERT(XJ == L4.theTime(9));
        ASSERT(XK == L4.theCharArray(10));
        ASSERT(XL == L4.theShortArray(11));
        ASSERT(XM == L4.theIntArray(12));
        ASSERT(XN == L4.theInt64Array(13));
        ASSERT(XO == L4.theFloatArray(14));
        ASSERT(XP == L4.theDoubleArray(15));
        ASSERT(XQ == L4.theStringArray(16));
        ASSERT(XR == L4.theDatetimeArray(17));
        ASSERT(XS == L4.theDateArray(18));
        ASSERT(XT == L4.theTimeArray(19));
        ASSERT(XU == L4.theList(20));
        ASSERT(XV == L4.theTable(21));

        ASSERT(XW == L4.theBool(22));
        ASSERT(XX == L4.theDatetimeTz(23));
        ASSERT(XY == L4.theDateTz(24));
        ASSERT(XZ == L4.theTimeTz(25));
        ASSERT(Xa == L4.theBoolArray(26));
        ASSERT(Xb == L4.theDatetimeTzArray(27));
        ASSERT(Xc == L4.theDateTzArray(28));
        ASSERT(Xd == L4.theTimeTzArray(29));
        ASSERT(Xe == L4.theChoice(30));
        ASSERT(Xf == L4.theChoiceArray(31));

        //  9-L) mL1 = mL2, testing assignment of longer list to shorter
        //          a) 22  == mL1.length()
        //          b) mL1 == mL2
        //             mL1 == mL4
        //             mL1 != mL3

        if (verbose) cout <<
            "\nTesting assignment, longer to shorter, 'mL1 = mL2':" << endl;

        mL1 = L2;

        if (veryVerbose) P(mL1);
        if (veryVerbose) P(L2);
        ASSERT(NUM_TYPES == L1.length());
        ASSERT( 1 == (L1 == L1));   ASSERT(0 == (L1 != L1));
        ASSERT( 1 == (L1 == L2));   ASSERT(0 == (L1 != L2));
        ASSERT( 1 == (L1 == L4));   ASSERT(0 == (L1 != L4));
        ASSERT( 0 == (L1 == L3));   ASSERT(1 == (L1 != L3));

        ASSERT(LX == L1);

        ASSERT(XA == L1.theChar(0));
        ASSERT(XB == L1.theShort(1));
        ASSERT(XC == L1.theInt(2));
        ASSERT(XD == L1.theInt64(3));
        ASSERT(XE == L1.theFloat(4));
        ASSERT(XF == L1.theDouble(5));
        ASSERT(XG == L1.theString(6));
        ASSERT(XH == L1.theDatetime(7));
        ASSERT(XI == L1.theDate(8));
        ASSERT(XJ == L1.theTime(9));
        ASSERT(XK == L1.theCharArray(10));
        ASSERT(XL == L1.theShortArray(11));
        ASSERT(XM == L1.theIntArray(12));
        ASSERT(XN == L1.theInt64Array(13));
        ASSERT(XO == L1.theFloatArray(14));
        ASSERT(XP == L1.theDoubleArray(15));
        ASSERT(XQ == L1.theStringArray(16));
        ASSERT(XR == L1.theDatetimeArray(17));
        ASSERT(XS == L1.theDateArray(18));
        ASSERT(XT == L1.theTimeArray(19));
        ASSERT(XU == L1.theList(20));
        ASSERT(XV == L1.theTable(21));

        ASSERT(XW == L1.theBool(22));
        ASSERT(XX == L1.theDatetimeTz(23));
        ASSERT(XY == L1.theDateTz(24));
        ASSERT(XZ == L1.theTimeTz(25));
        ASSERT(Xa == L1.theBoolArray(26));
        ASSERT(Xb == L1.theDatetimeTzArray(27));
        ASSERT(Xc == L1.theDateTzArray(28));
        ASSERT(Xd == L1.theTimeTzArray(29));
        ASSERT(Xe == L1.theChoice(30));
        ASSERT(Xf == L1.theChoiceArray(31));

        //  10-L) mL4 = mL3, testing assignment of shorter list to longer
        //          a) 0   == t4.numRows() && 0 == t4.numColumns()
        //          b) mL4 == mL3
        //             mL4 != mL1
        //             mL4 != tl2

        if (verbose) cout <<
            "\nTesting assignment, shorter to longer, 'mL4 = mL3':" << endl;

        mL4 = L3;

        ASSERT(0 == L4.length());
        ASSERT(1 == (L4 == L4));   ASSERT(0 == (L4 != L4));
        ASSERT(1 == (L4 == L3));   ASSERT(0 == (L4 != L3));
        ASSERT(0 == (L4 == L1));   ASSERT(1 == (L4 != L1));
        ASSERT(0 == (L4 == L2));   ASSERT(1 == (L4 != L2));

        //  11-L) mL2 = mL2, testing self-assignment
        //          a) 22  == mL2.length()
        //          b) mL2 == mL2
        //             mL2 == mL1
        //             mL2 != mL3
        //             mL2 != mL4

        if (verbose) cout << "\nTesting self-assignment, 'mL2 = mL2':" << endl;

        mL2 = L2;

        ASSERT(NUM_TYPES == L2.length());
        ASSERT( 1 == (L2 == L2));   ASSERT(0 == (L2 != L2));
        ASSERT( 1 == (L2 == L1));   ASSERT(0 == (L2 != L1));
        ASSERT( 0 == (L2 == L3));   ASSERT(1 == (L2 != L3));
        ASSERT( 0 == (L2 == L4));   ASSERT(1 == (L2 != L4));

        ASSERT(LX == L2);

        ASSERT(XA == L2.theChar(0));
        ASSERT(XB == L2.theShort(1));
        ASSERT(XC == L2.theInt(2));
        ASSERT(XD == L2.theInt64(3));
        ASSERT(XE == L2.theFloat(4));
        ASSERT(XF == L2.theDouble(5));
        ASSERT(XG == L2.theString(6));
        ASSERT(XH == L2.theDatetime(7));
        ASSERT(XI == L2.theDate(8));
        ASSERT(XJ == L2.theTime(9));
        ASSERT(XK == L2.theCharArray(10));
        ASSERT(XL == L2.theShortArray(11));
        ASSERT(XM == L2.theIntArray(12));
        ASSERT(XN == L2.theInt64Array(13));
        ASSERT(XO == L2.theFloatArray(14));
        ASSERT(XP == L2.theDoubleArray(15));
        ASSERT(XQ == L2.theStringArray(16));
        ASSERT(XR == L2.theDatetimeArray(17));
        ASSERT(XS == L2.theDateArray(18));
        ASSERT(XT == L2.theTimeArray(19));
        ASSERT(XU == L2.theList(20));
        ASSERT(XV == L2.theTable(21));

        ASSERT(XW == L2.theBool(22));
        ASSERT(XX == L2.theDatetimeTz(23));
        ASSERT(XY == L2.theDateTz(24));
        ASSERT(XZ == L2.theTimeTz(25));
        ASSERT(Xa == L2.theBoolArray(26));
        ASSERT(Xb == L2.theDatetimeTzArray(27));
        ASSERT(Xc == L2.theDateTzArray(28));
        ASSERT(Xd == L2.theTimeTzArray(29));
        ASSERT(Xe == L2.theChoice(30));
        ASSERT(Xf == L2.theChoiceArray(31));

        //  12-L) stream mL2 out, and then stream same data back in to mL4
        //          a) 22  == mL4.length()
        //          b) mL4 == mL2
        //             mL4 == mL1
        //             mL4 != mL3
        //             l4  == mL4

        if (verbose) cout << "\nTesting basic streaming functionality: "
                          << endl;

        bdex_TestOutStream out;
        const int LIST_MAX_VERSION = List::maxSupportedBdexVersion();
        bdex_OutStreamFunctions::streamOut(out,
                                           L2,
                                           LIST_MAX_VERSION);

        const char *PD  = out.data();
        const int   NPB = out.length();

        bdex_TestInStream in(PD,NPB);
        in.setSuppressVersionCheck(1);
        if (veryVerbose) cout << "\nStream in list" << endl;
        ASSERT(in);

        bdex_InStreamFunctions::streamIn(in,
                                         mL4,
                                         LIST_MAX_VERSION);
        ASSERT(in);

        ASSERT(NUM_TYPES == L2.length());
        ASSERT( 1 == (L4 == L2));   ASSERT(0 == (L4 != L2));
        ASSERT( 1 == (L4 == L1));   ASSERT(0 == (L4 != L1));
        ASSERT( 0 == (L4 == L3));   ASSERT(1 == (L4 != L3));
        ASSERT( 1 == (L4 == L4));   ASSERT(0 == (L4 != L4));

        ASSERT(LX == L4);

        ASSERT(XA == L4.theChar(0));
        ASSERT(XB == L4.theShort(1));
        ASSERT(XC == L4.theInt(2));
        ASSERT(XD == L4.theInt64(3));
        ASSERT(XE == L4.theFloat(4));
        ASSERT(XF == L4.theDouble(5));
        ASSERT(XG == L4.theString(6));
        ASSERT(XH == L4.theDatetime(7));
        ASSERT(XI == L4.theDate(8));
        ASSERT(XJ == L4.theTime(9));
        ASSERT(XK == L4.theCharArray(10));
        ASSERT(XL == L4.theShortArray(11));
        ASSERT(XM == L4.theIntArray(12));
        ASSERT(XN == L4.theInt64Array(13));
        ASSERT(XO == L4.theFloatArray(14));
        ASSERT(XP == L4.theDoubleArray(15));
        ASSERT(XQ == L4.theStringArray(16));
        ASSERT(XR == L4.theDatetimeArray(17));
        ASSERT(XS == L4.theDateArray(18));
        ASSERT(XT == L4.theTimeArray(19));
        ASSERT(XU == L4.theList(20));
        ASSERT(XV == L4.theTable(21));

        ASSERT(XW == L4.theBool(22));
        ASSERT(XX == L4.theDatetimeTz(23));
        ASSERT(XY == L4.theDateTz(24));
        ASSERT(XZ == L4.theTimeTz(25));
        ASSERT(Xa == L4.theBoolArray(26));
        ASSERT(Xb == L4.theDatetimeTzArray(27));
        ASSERT(Xc == L4.theDateTzArray(28));
        ASSERT(Xd == L4.theTimeTzArray(29));
        ASSERT(Xe == L4.theChoice(30));
        ASSERT(Xf == L4.theChoiceArray(31));
      }

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

#ifdef BSLS_PLATFORM__OS_WINDOWS
    // Suppress all windows debugging popups
    _CrtSetReportMode(_CRT_ASSERT, 0);
    _CrtSetReportMode(_CRT_ERROR,  0);
    _CrtSetReportMode(_CRT_WARN,   0);
#endif

    switch (test) { case 0:  // Zero is always the leading case.
#define CASE(NUMBER)                                                     \
  case NUMBER: testCase##NUMBER(verbose, veryVerbose, veryVeryVerbose,   \
                                                           argv ); break
        CASE(41);
        CASE(40);
        CASE(39);
        CASE(38);
        CASE(37);
        CASE(36);
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
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
