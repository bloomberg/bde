// bdem_row.t.cpp                                                     -*-C++-*-

#include <bdem_row.h>

#include <bdem_aggregateoption.h>
#include <bdem_choicearrayimp.h>
#include <bdem_choiceimp.h>
#include <bdem_elemref.h>
#include <bdem_listimp.h>
#include <bdem_properties.h>
#include <bdem_rowdata.h>
#include <bdem_tableimp.h>

#include <bdex_testinstream.h>
#include <bdex_testoutstream.h>

#include <bdet_datetime.h>
#include <bdet_datetimetz.h>
#include <bdet_date.h>
#include <bdet_datetz.h>
#include <bdet_time.h>
#include <bdet_timetz.h>

#include <bdetu_unset.h>

#include <bslma_default.h>
#include <bsls_platform.h>

#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_strstream.h>
#include <bsl_vector.h>

#include <bsl_cstdlib.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// A 'bdem_Row' is not really an independent object, but merely another
// view on a 'bdem_RowData' defined in 'bdem_rowdata.h'.  All of the methods
// perform simple delegation, hence there is no need for a full-blown test.
// It will be sufficient to verify that each method is "hooked-up" properly.
// We can accomplish this goal with a single, comprehensive "breathing" test
// that exercises each function at least twice.
//
// TBD: these tests have not been extended to include elements that of type
// bdem_Choice or bdem_ChoiceArray.  Since bdetu_unset doesn't support these
// types, this will probably never be worth doing.
//-----------------------------------------------------------------------------
// [ 2] bdem_ElemRef operator[](int index);
// [ 2] bdem_Row& replaceValues(const bdem_Row& other);
// [ 2] void makeAllNull();
// [ 2] void makeNull(int index);
// [ 2] bool& theBool(int index);
// [ 2] char& theChar(int index);
// [ 2] short& theShort(int index);
// [ 2] int& theInt(int index);
// [ 2] bsls_Types::Int64& theInt64(int index);
// [ 2] float& theFloat(int index);
// [ 2] double& theDouble(int index);
// [ 2] bsl::string& theString(int index);
// [ 2] bdet_Datetime& theDatetime(int index);
// [ 2] bdet_DatetimeTz& theDatetimeTz(int index);
// [ 2] bdet_Date& theDate(int index);
// [ 2] bdet_DateTz& theDateTz(int index);
// [ 2] bdet_Time& theTime(int index);
// [ 2] bdet_TimeTz& theTimeTz(int index);
// [ 2] bsl::vector<bool>& theBoolArray(int index);
// [ 2] bsl::vector<char>& theCharArray(int index);
// [ 2] bsl::vector<short>& theShortArray(int index);
// [ 2] bsl::vector<int>& theIntArray(int index);
// [ 2] bsl::vector<bsls_Types::Int64>& theInt64Array(int index);
// [ 2] bsl::vector<float>& theFloatArray(int index);
// [ 2] bsl::vector<double>& theDoubleArray(int index);
// [ 2] bsl::vector<bsl::string>& theStringArray(int index);
// [ 2] bsl::vector<bdet_Datetime>& theDatetimeArray(int index);
// [ 2] bsl::vector<bdet_DatetimeTz>& theDatetimeTzArray(int index);
// [ 2] bsl::vector<bdet_Date>& theDateArray(int index);
// [ 2] bsl::vector<bdet_DateTz>& theDateTzArray(int index);
// [ 2] bsl::vector<bdet_Time>& theTimeArray(int index);
// [ 2] bsl::vector<bdet_TimeTz>& theTimeTzArray(int index);
// [ 2] bdem_Choice& theChoice(int index);
// [ 2] bdem_ChoiceArray& theChoiceArray(int index);
// [ 2] bdem_List& theList(int index);
// [ 2] bdem_Table& theTable(int index);
//
// [ 2] theModifiableBool(int index, value);
// [ 2] theModifiableChar(int index, value);
// [ 2] theModifiableShort(int index, value);
// [ 2] theModifiableInt(int index, value);
// [ 2] theModifiableInt64(int index, value);
// [ 2] theModifiableFloat(int index, value);
// [ 2] theModifiableDouble(int index, value);
// [ 2] theModifiableString(int index, value);
// [ 2] theModifiableDatetime(int index, value);
// [ 2] theModifiableDatetimeTz(int index, value);
// [ 2] theModifiableDate(int index, value);
// [ 2] theModifiableDateTz(int index, value);
// [ 2] theModifiableTime(int index, value);
// [ 2] theModifiableTimeTz(int index, value);
// [ 2] theModifiableBoolArray(int index, value);
// [ 2] theModifiableCharArray(int index, value);
// [ 2] theModifiableShortArray(int index, value);
// [ 2] theModifiableIntArray(int index, value);
// [ 2] theModifiableInt64Array(int index, value);
// [ 2] theModifiableFloatArray(int index, value);
// [ 2] theModifiableDoubleArray(int index, value);
// [ 2] theModifiableStringArray(int index, value);
// [ 2] theModifiableDatetimeArray(int index, value);
// [ 2] theModifiableDatetimeTzArray(int index, value);
// [ 2] theModifiableDateArray(int index, value);
// [ 2] theModifiableDateTzArray(int index, value);
// [ 2] theModifiableTimeArray(int index, value);
// [ 2] theModifiableTimeTzArray(int index, value);
// [ 2] theModifiableChoice(int index, value);
// [ 2] theModifiableChoiceArray(int index, value);
// [ 2] theModifiableList(int index, value);
// [ 2] theModifiableTable(int index, value);
// [  ] bdexStreamIn(STREAM& stream, int version);
//
// [ 2] int length() const;
// [ 2] bdem_ElemType::Type elemType(int index) const;
// [ 2] bdem_ConstElemRef operator[](int index) const;
// [ 2] const bool& theBool(int index) const;
// [ 2] const char& theChar(int index) const;
// [ 2] const short& theShort(int index) const;
// [ 2] const int& theInt(int index) const;
// [ 2] const bsls_Types::Int64& theInt64(int index) const;
// [ 2] const float& theFloat(int index) const;
// [ 2] const double& theDouble(int index) const;
// [ 2] const bsl::string& theString(int index) const;
// [ 2] const bdet_Datetime& theDatetime(int index) const;
// [ 2] const bdet_DatetimeTz& theDatetimeTz(int index) const;
// [ 2] const bdet_Date& theDate(int index) const;
// [ 2] const bdet_DateTz& theDateTz(int index) const;
// [ 2] const bdet_Time& theTime(int index) const;
// [ 2] const bdet_TimeTz& theTimeTz(int index) const;
// [ 2] const bsl::vector<bool>& theBoolArray(int index) const;
// [ 2] const bsl::vector<char>& theCharArray(int index) const;
// [ 2] const bsl::vector<short>& theShortArray(int index) const;
// [ 2] const bsl::vector<int>& theIntArray(int index) const;
// [ 2] const bsl::vector<Int64>& theInt64Array(int index) const;
// [ 2] const bsl::vector<float>& theFloatArray(int index) const;
// [ 2] const bsl::vector<double>& theDoubleArray(int index) const;
// [ 2] const bsl::vector<bsl::string>& theStringArray(int index) const;
// [ 2] const bsl::vector<bdet_Datetime>& theDatetimeArray(int i) const;
// [ 2] const bsl::vector<bdet_DatetimeTz>& theDatetimeTzArray(int i) const;
// [ 2] const bsl::vector<bdet_Date>& theDateArray(int index) const;
// [ 2] const bsl::vector<bdet_DateTz>& theDateTzArray(int index) const;
// [ 2] const bsl::vector<bdet_Time>& theTimeArray(int index) const;
// [ 2] const bsl::vector<bdet_TimeTz>& theTimeTzArray(int index) const;
// [ 2] const bdem_Choice& theChoice(int index) const;
// [ 2] const bdem_ChoiceArray& theChoiceArray(int index) const;
// [ 2] const bdem_List& theList(int index) const;
// [ 2] const bdem_Table& theTable(int index) const;
// [  ] bdexStreamOut(STREAM& stream, int version) const;
// [ 2] print(bsl::ostream& stream, int level, int spl const;
//
// [ 2] bool operator==(const bdem_Row& lhs, const bdem_Row& rhs);
// [ 2] bool operator!=(const bdem_Row& lhs, const bdem_Row& rhs);
// [  ] ostream& operator<<(ostream& stream, const bdem_Row& row);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE
// ----------------------------------------------------------------------------

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
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bsls_Types::Int64 Int64;
typedef bdem_RowData      RowData;
typedef bdem_RowLayout    RowLayout;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Overloaded Operators needed for Implicit Conversion of List& to ListImp&

bool operator==(const bdem_ListImp& lhs, const bdem_List& rhs) {
    return lhs == *reinterpret_cast<const bdem_ListImp *>(&rhs);
}
bool operator==(const bdem_List& lhs, const bdem_ListImp& rhs) {
    return *reinterpret_cast<const bdem_ListImp *>(&lhs) == rhs;
}
bool operator==(const bdem_List& lhs, const bdem_List& rhs) {
    return *reinterpret_cast<const bdem_ListImp *>(&lhs)
        == *reinterpret_cast<const bdem_ListImp *>(&rhs);
}

// Overloaded Operators needed for Implicit Conversion of Table& to TableImp&

bool operator==(const bdem_TableImp& lhs, const bdem_Table& rhs) {
    return lhs == *reinterpret_cast<const bdem_TableImp *>(&rhs);
}
bool operator==(const bdem_Table& lhs, const bdem_TableImp& rhs) {
    return *reinterpret_cast<const bdem_TableImp *>(&lhs) == rhs;
}
bool operator==(const bdem_Table& lhs, const bdem_Table& rhs) {
    return *reinterpret_cast<const bdem_TableImp *>(&lhs)
        == *reinterpret_cast<const bdem_TableImp *>(&rhs);
}

template <typename TYPE>
bool isAnySet(const TYPE& object)
    // Return 'true' if any element in the specified 'object' is "set", that is
    // it does not have the "unset" value, and 'false' otherwise.
{
    const int len = object.length();
    for (int i = 0; i < len; ++i) {
        const bdem_ConstElemRef& elemRef = object[i];
        if (!elemRef.descriptor()->isUnset(elemRef.data())) {
            return true;
        }
    }
    return false;
}

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Demonstrate how to use this component.
        //
        // Concerns:
        //   That the usage example is syntactically an otherwise correct.
        //
        // Plan:
        //   Repeat it, converting "assert" to "ASSERT"
        //
        // Tactics:
        //   - Ad Hoc Data Selection
        //   - Brute Force Implementation
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

///Usage
///-----
// A row is not typically a first-class object, but is instead created and
// owned by a parent object such as a 'bdem_List' or 'bdem_Table'.  In this
// example we first illustrate how to create a row directly (most users will
// not generally need to follow this example, but rather will obtain rows from
// higher-level 'bdem' components).  Then we will demonstrate more typical row
// usage by getting and setting member elements.
//
// In order to create a row directly, we will need an array of descriptors
// (of type 'bdem_Descriptor'), which is in turn used to characterize the
// salient behavior of each element we plan to instantiate.  For this example,
// we propose to create a row containing a sequence of elements corresponding
// to the 'bdem_ElemType' enumerators 'BDEM_DOUBLE', 'BDEM_INT', 'BDEM_STRING',
// and 'BDEM_STRING_ARRAY'.  First, we create an array, 'ETA', of enumerators:
//..
    static const bdem_ElemType::Type ETA[] = {
        bdem_ElemType::BDEM_DOUBLE,
        bdem_ElemType::BDEM_INT,
        bdem_ElemType::BDEM_STRING,
        bdem_ElemType::BDEM_STRING_ARRAY
    };
//..
// Next, we create an array, 'EDA', of pointers to 'bdem_Descriptor' values.
// We need one appropriate pointer for each *distinct* element type in 'ETA'.
// The array index of that descriptor pointer must match the 'bdem_ElemType'
// enumerator value corresponding to its type:
//..
    static const bdem_Descriptor *EDA[] = {
        0, 0,
        &bdem_Properties::d_intAttr,
        0, 0,
        &bdem_Properties::d_doubleAttr,
        &bdem_Properties::d_stringAttr,
        0, 0, 0, /* arrays: */ 0, 0, 0, 0, 0, 0,
        &bdem_Properties::d_stringArrayAttr
    };
//..
// Next we create a 'bdem_RowLayout' object, 'mRL', that defines the layout of
// the 'bdem' elements in the rows that are created later:
//..
    bdem_RowLayout mRL(ETA,         // array of element type enumerators
                       4,           // length of row
                       EDA,         // (sparse) array of element descriptors
                       bslma_Default::defaultAllocator());
//..
// Given 'mRL', we now instantiate two (modifiable) row definitions, 'mRD1' and
// 'mRD2':
//..
    bdem_RowData mRD1(&mRL,
                      bdem_AggregateOption::BDEM_PASS_THROUGH,
                      bslma_Default::defaultAllocator());

    bdem_RowData mRD2(&mRL,
                      bdem_AggregateOption::BDEM_PASS_THROUGH,
                      bslma_Default::defaultAllocator());

//..
// By design, a 'bdem_Row' contains exactly one data member of type
// 'bdem_RowData'.  Casting the address of a 'bdem_RowData' to a 'bdem_Row *'
// provides a limited "view" of the row data that is appropriate for typical
// general users (such as clients of lists and tables):
//..
    bdem_Row &row1 = *reinterpret_cast<bdem_Row *>(&mRD1);
    bdem_Row &row2 = *reinterpret_cast<bdem_Row *>(&mRD2);
//..
// We observe that these two rows, each of length 4, are initially identical:
//..
    ASSERT(4    == row1.length());
    ASSERT(4    == row2.length());
    ASSERT(row1 == row2);
//..
// Each element of both rows has been initialized to its null value.  Printing
// either row, say, 'row1':
//..
if (verbose)
     bsl::cout << row1 << bsl::endl;
//..
// yields the following output:
//..
//  { DOUBLE NULL INT NULL STRING NULL STRING_ARRAY NULL }
//..
// The next thing we'll do is assign values to the individual elements of the
// rows.  There are two distinct styles of accessing elements.  The first style
// identifies a reference to the specific element directly:
//..
    double& doubleRef           = row1.theModifiableDouble(0);
    doubleRef                   = 2.5; ASSERT(    2.5 == row1.theDouble(0));
    row1.theModifiableInt(1)    = 34;  ASSERT(     34 == row1.theInt(1));
    row1.theModifiableString(2) = "hello";
                                       ASSERT("hello" == row1.theString(2));
//..
// Note that the two rows still have the same length, but not the same value:
//..
    ASSERT(row1.length() == row2.length());  ASSERT(row1 != row2);
//..
// The second style of access returns a 'bdem_ElemRef' by value.  This "proxy"
// can then be used to access the element:
//..
    bdem_ElemRef elemRef0          = row2[0];
    elemRef0.theModifiableDouble() = 2.5;

    bdem_ElemRef elemRef1          = row2[1];
    elemRef1.theModifiableInt()    = 34;

    bdem_ElemRef elemRef2          = row2[2];
    elemRef2.theModifiableString() = "hello";
//..
// Note that 'row1' again has the same value as 'row2':
//..
    ASSERT(row1.length() == row2.length());  ASSERT(row1 == row2);
//..
// which when printed:
//..
if (verbose)
    bsl::cout << row2 << bsl::endl;
//..
// yields:
//..
//  { DOUBLE 2.5 INT 34 STRING hello STRING_ARRAY NULL }
//..
// As another example, we might choose to append a few strings to the empty
// string array at the end of 'row2':
//..
    row2.theModifiableStringArray(3).push_back("alpha");
    row2[3].theModifiableStringArray().push_back("beta");
//..
// which when printed:
//..
if (verbose)
    bsl::cout << row2 << bsl::endl;
//..
// yields:
//..
//  { DOUBLE 2.5 INT 34 STRING hello STRING_ARRAY [ alpha beta ] }
//..
// Note that we can also assign directly to the element of an array:
//..
    row2.theModifiableStringArray(3)[1] = "gamma";
//..
// which when printed:
//..
if (verbose)
    bsl::cout << row2 << bsl::endl;
//..
// yields:
//..
//  { DOUBLE 2.5 INT 34 STRING hello STRING_ARRAY [ alpha gamma ] }
//..
// As a final example, we reset the second element in 'row2' back to null:
//..
    row2.makeNull(1);
//..
// which when printed:
//..
if (verbose)
    bsl::cout << row2 << bsl::endl;
//..
// now yields:
//..
//  { DOUBLE 2.5 INT NULL STRING hello STRING_ARRAY [ alpha gamma ] }
//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DELEGATION TEST
        //   This test verifies that the "delegation functions" are "hooked up"
        //   properly, but does not attempt to test the underlying behavior.
        //   Note that no attempt is made at checking for alias safety or
        //   exception neutrality"
        //
        // Concerns:
        //   1. Each method in this "Ghost" class delegates properly.
        //   2. That all accessors work on const and non-const instances.
        //
        // Plan:
        //   1. Create a (const) empty row and verify all relevant accessors
        //      work.
        //
        //   2. Create a modifiable row with one integer element and verify
        //      that all relevant accessors and manipulators work.
        //
        //   3. Create a couple of modifiable rows, each containing all 32
        //      types, but with the second in reverse order.  Create two more
        //      rows precisely the same way (to be used as controls).  Verify
        //      that each function performs as expected.
        //
        // Tactics:
        //   - Ad Hoc Data Selection
        //   - Brute Force Implementation
        //
        // Testing:
        //   int length() const;
        //   print(bsl::ostream& stream, int level, int spl const;
        //
        //   bool operator==(const bdem_Row& lhs, const bdem_Row& rhs);
        //   bool operator!=(const bdem_Row& lhs, const bdem_Row& rhs);
        //
        //   bdem_ConstElemRef operator[](int index) const;
        //
        //   const bool& theBool(int index) const;
        //   const char& theChar(int index) const;
        //   const short& theShort(int index) const;
        //   const int& theInt(int index) const;
        //   const bsls_Types::Int64& theInt64(int index) const;
        //   const float& theFloat(int index) const;
        //   const double& theDouble(int index) const;
        //   const bsl::string& theString(int index) const;
        //   const bdet_Datetime& theDatetime(int index) const;
        //   const bdet_DatetimeTz& theDatetimeTz(int index) const;
        //   const bdet_Date& theDate(int index) const;
        //   const bdet_DateTz& theDateTz(int index) const;
        //   const bdet_Time& theTime(int index) const;
        //   const bdet_TimeTz& theTimeTz(int index) const;
        //   const bsl::vector<bool>& theBoolArray(int index) const;
        //   const bsl::vector<char>& theCharArray(int index) const;
        //   const bsl::vector<short>& theShortArray(int index) const;
        //   const bsl::vector<int>& theIntArray(int index) const;
        //   const bsl::vector<Int64>& theInt64Array(int index) const;
        //   const bsl::vector<float>& theFloatArray(int index) const;
        //   const bsl::vector<double>& theDoubleArray(int index) const;
        //   const bsl::vector<bsl::string>& theStringArray(int index) const;
        //   const bsl::vector<bdet_Datetime>& theDatetimeArray(int i) const;
        //   const bsl::vector<bdet_DatetimeTz>& theDatetimeTzArray(int i)
        //                                                              const;
        //   const bsl::vector<bdet_Date>& theDateArray(int index) const;
        //   const bsl::vector<bdet_DateTz>& theDateTzArray(int index) const;
        //   const bsl::vector<bdet_Time>& theTimeArray(int index) const;
        //   const bsl::vector<bdet_TimeTz>& theTimeTzArray(int index) const;
        //   const bdem_Choice& theChoice(int index) const;
        //   const bdem_ChoiceArray& theChoiceArray(int index) const;
        //   const bdem_List& theList(int index) const;
        //   const bdem_Table& theTable(int index) const;
        //
        //   ? bdexStreamOut(STREAM& stream, int version) const;
        //   ?? ostream& operator<<(ostream& stream, const bdem_Row& row);
        //
        //   bdem_ElemRef operator[](int index);
        //
        //   bool& theModifiableBool(int index);
        //   char& theModifiableChar(int index);
        //   short& theModifiableShort(int index);
        //   int& theModifiableInt(int index);
        //   bsls_Types::Int64& theModifiableInt64(int index);
        //   float& theModifiableFloat(int index);
        //   double& theModifiableDouble(int index);
        //   bsl::string& theModifiableString(int index);
        //   bdet_Datetime& theModifiableDatetime(int index);
        //   bdet_DatetimeTz& theModifiableDatetimeTz(int index);
        //   bdet_Date& theModifiableDate(int index);
        //   bdet_DateTz& theModifiableDateTz(int index);
        //   bdet_Time& theModifiableTime(int index);
        //   bdet_TimeTz& theModifiableTimeTz(int index);
        //   bsl::vector<bool>& theModifiableBoolArray(int index);
        //   bsl::vector<char>& theModifiableCharArray(int index);
        //   bsl::vector<short>& theModifiableShortArray(int index);
        //   bsl::vector<int>& theModifiableIntArray(int index);
        //   bsl::vector<bsls_Types::Int64>&
        //                                  theModifiableInt64Array(int index);
        //   bsl::vector<float>& theModifiableFloatArray(int index);
        //   bsl::vector<double>& theModifiableDoubleArray(int index);
        //   bsl::vector<bsl::string>& theModifiableStringArray(int index);
        //   bsl::vector<bdet_Datetime>& theModifiableDatetimeArray(int index);
        //   bsl::vector<bdet_DatetimeTz>&
        //                             theModifiableDatetimeTzArray(int index);
        //   bsl::vector<bdet_Date>& theModifiableDateArray(int index);
        //   bsl::vector<bdet_DateTz>& theModifiableDateTzArray(int index);
        //   bsl::vector<bdet_Time>& theModifiableTimeArray(int index);
        //   bsl::vector<bdet_TimeTz>& theModifiableTimeTzArray(int index);
        //   bdem_Choice& theModifiableChoice(int index);
        //   bdem_ChoiceArray& theModifiableChoiceArray(int index);
        //   bdem_List& theModifiableList(int index);
        //   bdem_Table& theModifiableTable(int index);
        //
        //   bdem_ElemType::Type elemType(int index) const;
        //
        //   ?bdexStreamIn(STREAM& stream, int version);
        //
        //   bdem_Row& replaceValues(const bdem_Row& other);
        //   void makeAllNull();
        //   void makeNull(int index);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "DELEGATION TEST" << endl
                          << "===============" << endl;

        const bdem_AggregateOption::AllocationStrategy S =
            bdem_AggregateOption::BDEM_PASS_THROUGH;

        bslma_Allocator *const NDA = bslma_Default::defaultAllocator();

        if (verbose) cout << "\nCreate an empty Row" << endl;
        {
            if (verbose) cout << "\tCreate an empty RowDef" << endl;
            RowLayout mRL; const RowLayout& RL = mRL;
            RowData mX(&mRL, S, NDA); const RowData& X = mX;
            if (veryVerbose) { cout << "\t\t"; P(X.length()); }
            ASSERT(0 == X.length());

            if (verbose) cout << "\tTry various accessor functions" << endl;

            bdem_Row& mR = *reinterpret_cast<bdem_Row *>(&mX);
            const bdem_Row& R = mR;

            if (verbose) cout << "\t\ttrivial accessors" << endl;
            {
                LOOP_ASSERT(R.length(), 0 == R.length());

                LOOP_ASSERT(R.isAnyNonNull(), 0 == R.isAnyNonNull());

                ASSERT(0 == isAnySet(R));

                LOOP_ASSERT(R.isAnyNull(), 0 == R.isAnyNull());
            }

            if (verbose) cout << "\t\t bdex stream out (and back in)" << endl;
            {
                bdex_TestOutStream out;

                int version = bdem_Row::maxSupportedBdexVersion();

                bdex_TestOutStream& out2 = R.bdexStreamOut(out, version);
                ASSERT(&out == &out2);

                bdex_TestInStream in(out.data(), out.length());

                bdem_RowLayout mRD2(bdem_RowLayout::InitialCapacity(0), NDA);
                bdem_RowData   header2(
                                      &mRD2,
                                      bdem_AggregateOption::BDEM_PASS_THROUGH);
                bdem_Row& mR2 = *reinterpret_cast<bdem_Row *>(&header2);
                const bdem_Row& R2 = mR2;

                bdex_TestInStream& in2 = mR2.bdexStreamIn(in, version);
                ASSERT(&in2 == &in);
                ASSERT(R == R2);
            }

            if (verbose) cout << "\t\tstd::ostream print" << endl;
            {
                char buffer[1000];
                ostrstream out(buffer, sizeof buffer);
                R.print(out, -2, 1); out << ends;
                if (veryVerbose) {
                    cout << "\t\t\tbuffer: ``";
                    for (const char *p = buffer; *p; ++p) {
                        if ('\n' == *p)     cout << 'N';
                        else if (' ' == *p) cout << '_';
                        else if ('{' == *p) cout << *p;
                        else if ('}' == *p) cout << *p;
                        else                cout << "{0x" << unsigned(*p)<<'}';
                    }
                    cout << "''" << endl;
                }
                ASSERT(0 == strcmp("{\n  }\n", buffer));
            }
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nCreate tables needed for elements" << endl;

        if (verbose) cout << "\tSet up THE element discriptor array" << endl;

        typedef bdem_Properties Prop;
        typedef bdem_ChoiceImp  CImp;
        typedef bdem_ListImp    Limp;
        typedef bdem_TableImp   Timp;

        static const bdem_Descriptor *EDA[] = {
            &Prop::d_charAttr,          &Prop::d_shortAttr,
            &Prop::d_intAttr,           &Prop::d_int64Attr,
            &Prop::d_floatAttr,         &Prop::d_doubleAttr,
            &Prop::d_stringAttr,        &Prop::d_datetimeAttr,
            &Prop::d_dateAttr,          &Prop::d_timeAttr,
            &Prop::d_charArrayAttr,     &Prop::d_shortArrayAttr,
            &Prop::d_intArrayAttr,      &Prop::d_int64ArrayAttr,
            &Prop::d_floatArrayAttr,    &Prop::d_doubleArrayAttr,
            &Prop::d_stringArrayAttr,   &Prop::d_datetimeArrayAttr,
            &Prop::d_dateArrayAttr,     &Prop::d_timeArrayAttr,
            &Limp::d_listAttr,          &Timp::d_tableAttr,

            &Prop::d_boolAttr,          &Prop::d_datetimeTzAttr,
            &Prop::d_dateTzAttr,        &Prop::d_timeTzAttr,
            &Prop::d_boolArrayAttr,     &Prop::d_datetimeTzArrayAttr,
            &Prop::d_dateTzArrayAttr,   &Prop::d_timeTzArrayAttr
              // TBD
//             &CImp::d_choiceDescriptor
        };
        ASSERT(30 == sizeof EDA/sizeof *EDA);

        if (verbose) cout << "\tCreate all 31 unset element values" << endl;

        // unset scalar values
        const bool     UNSET_BDEM_BOOL   = bdetu_Unset<bool>::unsetValue();
        const char     UNSET_BDEM_CHAR   = bdetu_Unset<char>::unsetValue();
        const short    UNSET_BDEM_SHORT  = bdetu_Unset<short>::unsetValue();
        const int      UNSET_BDEM_INT    = bdetu_Unset<int>::unsetValue();
        const Int64    UNSET_BDEM_INT64  = bdetu_Unset<Int64>::unsetValue();
        const float    UNSET_BDEM_FLOAT  = bdetu_Unset<float>::unsetValue();
        const double   UNSET_BDEM_DOUBLE = bdetu_Unset<double>::unsetValue();
        const bsl::string
                       UNSET_BDEM_STRING =
                                        bdetu_Unset<bsl::string>::unsetValue();
        const bdet_Datetime
                       UNSET_BDEM_DATETIME =
                                      bdetu_Unset<bdet_Datetime>::unsetValue();
        const bdet_DatetimeTz
                       UNSET_BDEM_DATETIMETZ =
                                    bdetu_Unset<bdet_DatetimeTz>::unsetValue();
        const bdet_Date
                       UNSET_BDEM_DATE =
                                          bdetu_Unset<bdet_Date>::unsetValue();
        const bdet_DateTz
                       UNSET_BDEM_DATETZ =
                                        bdetu_Unset<bdet_DateTz>::unsetValue();
        const bdet_Time
                       UNSET_BDEM_TIME = bdetu_Unset<bdet_Time>::unsetValue();
        const bdet_TimeTz
                       UNSET_BDEM_TIMETZ =
                                        bdetu_Unset<bdet_TimeTz>::unsetValue();

        // unset array values
        const bsl::vector<bool>          UNSET_BDEM_BOOL_ARRAY;
        const bsl::vector<char>          UNSET_BDEM_CHAR_ARRAY;
        const bsl::vector<short>         UNSET_BDEM_SHORT_ARRAY;
        const bsl::vector<int>           UNSET_BDEM_INT_ARRAY;
        const bsl::vector<Int64>         UNSET_BDEM_INT64_ARRAY;
        const bsl::vector<float>         UNSET_BDEM_FLOAT_ARRAY;
        const bsl::vector<double>        UNSET_BDEM_DOUBLE_ARRAY;
        const bsl::vector<bsl::string>   UNSET_BDEM_STRING_ARRAY;
        const bsl::vector<bdet_Datetime> UNSET_BDEM_DATETIME_ARRAY;
        const bsl::vector<bdet_DatetimeTz> UNSET_BDEM_DATETIMETZ_ARRAY;
        const bsl::vector<bdet_Date>     UNSET_BDEM_DATE_ARRAY;
        const bsl::vector<bdet_DateTz>     UNSET_BDEM_DATETZ_ARRAY;
        const bsl::vector<bdet_Time>     UNSET_BDEM_TIME_ARRAY;
        const bsl::vector<bdet_TimeTz>     UNSET_BDEM_TIMETZ_ARRAY;

        // unset aggregate values
        // TBD
//         const bdem_ChoiceImp            UNSET_BDEM_CHOICE(S, 0);
        const bdem_ListImp              UNSET_BDEM_LIST(S);
        const bdem_TableImp             UNSET_BDEM_TABLE(S);

        if (verbose) cout << "\tCreate a non-null value for each type" << endl;

        // non-null scalar values
        const bool          BDEM_BOOL_VALUE   = true;
        const char          BDEM_CHAR_VALUE   = 'A';
        const short         BDEM_SHORT_VALUE  = 1;
        const int           BDEM_INT_VALUE    = 2;
        const Int64         BDEM_INT64_VALUE  = 3;
        const float         BDEM_FLOAT_VALUE  = 4.5;
        const double        BDEM_DOUBLE_VALUE = 5.5;
        const bsl::string   BDEM_STRING_VALUE = "six";
        const bdet_Date     BDEM_DATE_VALUE(9876, 12, 30);
        const bdet_DateTz   BDEM_DATETZ_VALUE(BDEM_DATE_VALUE, 3);
        const bdet_Time     BDEM_TIME_VALUE(12, 34, 56, 789);
        const bdet_TimeTz   BDEM_TIMETZ_VALUE(BDEM_TIME_VALUE, 12);
        const bdet_Datetime BDEM_DATETIME_VALUE(BDEM_DATE_VALUE,
                                                BDEM_TIME_VALUE);
        const bdet_DatetimeTz BDEM_DATETIMETZ_VALUE(BDEM_DATETIME_VALUE, -5);

        // setup for array values
        bsl::vector<bool>          tmpBoolA;
        tmpBoolA.push_back(BDEM_BOOL_VALUE);
        bsl::vector<char>          tmpCharA;
        tmpCharA.push_back(BDEM_CHAR_VALUE);
        bsl::vector<short>         tmpShortA;
        tmpShortA.push_back(BDEM_SHORT_VALUE);
        bsl::vector<int>           tmpIntA;
        tmpIntA.push_back(BDEM_INT_VALUE);
        bsl::vector<Int64>         tmpInt64A;
        tmpInt64A.push_back(BDEM_INT64_VALUE);
        bsl::vector<float>         tmpFloatA;
        tmpFloatA.push_back(BDEM_FLOAT_VALUE);
        bsl::vector<double>        tmpDoubleA;
        tmpDoubleA.push_back(BDEM_DOUBLE_VALUE);
        bsl::vector<bsl::string>   tmpStringA;
        tmpStringA.push_back(BDEM_STRING_VALUE);
        bsl::vector<bdet_Datetime> tmpDTA;
        tmpDTA.push_back(BDEM_DATETIME_VALUE);
        bsl::vector<bdet_DatetimeTz> tmpDTZA;
        tmpDTZA.push_back(BDEM_DATETIMETZ_VALUE);
        bsl::vector<bdet_Date>     tmpDateA;
        tmpDateA.push_back(BDEM_DATE_VALUE);
        bsl::vector<bdet_DateTz>   tmpDateTzA;
        tmpDateTzA.push_back(BDEM_DATETZ_VALUE);
        bsl::vector<bdet_Time>     tmpTimeA;
        tmpTimeA.push_back(BDEM_TIME_VALUE);
        bsl::vector<bdet_TimeTz>   tmpTimeTzA;
        tmpTimeTzA.push_back(BDEM_TIMETZ_VALUE);

        // non-null array values
        const bsl::vector<bool>&          BDEM_BOOL_ARRAY_VALUE   = tmpBoolA;
        const bsl::vector<char>&          BDEM_CHAR_ARRAY_VALUE   = tmpCharA;
        const bsl::vector<short>&         BDEM_SHORT_ARRAY_VALUE  = tmpShortA;
        const bsl::vector<int>&           BDEM_INT_ARRAY_VALUE    = tmpIntA;
        const bsl::vector<Int64>&         BDEM_INT64_ARRAY_VALUE  = tmpInt64A;
        const bsl::vector<float>&         BDEM_FLOAT_ARRAY_VALUE  = tmpFloatA;
        const bsl::vector<double>&        BDEM_DOUBLE_ARRAY_VALUE = tmpDoubleA;
        const bsl::vector<bsl::string>&   BDEM_STRING_ARRAY_VALUE = tmpStringA;
        const bsl::vector<bdet_Datetime>& BDEM_DATETIME_ARRAY_VALUE
                                                                  = tmpDTA;
        const bsl::vector<bdet_DatetimeTz>&
                                          BDEM_DATETIMETZ_ARRAY_VALUE
                                                                  = tmpDTZA;
        const bsl::vector<bdet_Date>&     BDEM_DATE_ARRAY_VALUE   = tmpDateA;
        const bsl::vector<bdet_DateTz>&   BDEM_DATETZ_ARRAY_VALUE = tmpDateTzA;
        const bsl::vector<bdet_Time>&     BDEM_TIME_ARRAY_VALUE   = tmpTimeA;
        const bsl::vector<bdet_TimeTz>&   BDEM_TIMETZ_ARRAY_VALUE = tmpTimeTzA;

        // setup for aggregate values
        // TBD
//         bdem_ChoiceImp tmpChoice(S, 0);
//         tmpChoice.addSelection(&Prop::d_intAttr);
//         int tmpChoiceValue = 100;
//         tmpChoice[0].theModifiableInt() = tmpChoiceValue;

        bdem_ListImp  tmpList(S);
        int tmpListValue = 100;
        tmpList.insertElement(0, &tmpListValue, EDA[bdem_ElemType::BDEM_INT]);

        bdem_TableImp tmpTable(S);
        tmpTable.insertNullRows(0, 1); // table with zero columns, one row

        // non-null aggregate values: LIST { INT 100 } and TABLE { [ ] { } }
        const bdem_List& LIST_VALUE = *reinterpret_cast<bdem_List *>(&tmpList);
        const bdem_Table& TABLE_VALUE =
            *reinterpret_cast<bdem_Table*>(&tmpTable);

        //cout << "FIX ME: I notice that the output for table is all "
        //    "        [ ]'s\n and I believe that arrays should be [ ]'s and "
        //    "        list/table\n should be { }'s -- please advise" << endl;

        // --------------------------------------------------------------------

        if (verbose) cout << "\nCreate a Row with a single Int" << endl;
        {
            if (verbose) cout << "\tSet up the element type array" << endl;
            static const bdem_ElemType::Type ETA[] = {
                bdem_ElemType::BDEM_INT
            };
            ASSERT(1 == sizeof ETA/sizeof *ETA);

            if (verbose) cout << "\tCreate a RowLayout" << endl;
            RowLayout mRL(ETA, 1, EDA, NDA);
            const RowLayout& RL = mRL;

            if (verbose) cout << "\tCreate a RowData" << endl;
            RowData mX(&RL, S, NDA); const RowData& X = mX;
            if (veryVerbose) { cout << "\t\t"; P(X.length()); }
            ASSERT(1 == X.length());

            if (verbose) cout << "\tCast it to a (non-empty) Row" << endl;
            bdem_Row& mR = *reinterpret_cast<bdem_Row *>(&mX);
            const bdem_Row& R = mR;

            if (verbose) cout << "\tTry various functions" << endl;

            if (verbose) cout << "\t\taccessors" << endl;
            {
                LOOP_ASSERT(R.length(), 1 == R.length());

                LOOP_ASSERT(R.elemType(0),
                            bdem_ElemType::BDEM_INT == R.elemType(0));

                LOOP_ASSERT(R[0].theInt(), UNSET_BDEM_INT == R[0].theInt());

                LOOP_ASSERT(R.isAnyNonNull(), 0 == R.isAnyNonNull());

                ASSERT(0 == isAnySet(R));

                LOOP_ASSERT(R.isAnyNull(), 1 == R.isAnyNull());

                LOOP_ASSERT(R.theInt(0), UNSET_BDEM_INT == R.theInt(0));
            }

            if (verbose) cout << "\t\tmanipulators" << endl;
            {
                mR.theModifiableInt(0) = 1;
                LOOP_ASSERT(R[0].theInt(), 1 == R[0].theInt());
                LOOP_ASSERT(R.isAnyNonNull(), 1 == R.isAnyNonNull());
                ASSERT(1 == isAnySet(R));
                LOOP_ASSERT(R.isAnyNull(), 0 == R.isAnyNull());

                mR[0].theModifiableInt() = 2;
                LOOP_ASSERT(R[0].theInt(), 2 == R.theInt(0));
                LOOP_ASSERT(R.isAnyNonNull(), 1 == R.isAnyNonNull());
                ASSERT(1 == isAnySet(R));
                LOOP_ASSERT(R.isAnyNull(), 0 == R.isAnyNull());

                mR.replaceValues(mR); // alias test
                LOOP_ASSERT(R[0].theInt(), 2 == R.theInt(0));
                LOOP_ASSERT(R.isAnyNonNull(), 1 == R.isAnyNonNull());
                ASSERT(1 == isAnySet(R));
                LOOP_ASSERT(R.isAnyNull(), 0 == R.isAnyNull());

                mR.makeAllNull();
                LOOP_ASSERT(R[0].theInt(), UNSET_BDEM_INT == R[0].theInt());
                LOOP_ASSERT(R.theInt(0),   UNSET_BDEM_INT == R.theInt(0));
                LOOP_ASSERT(R.isAnyNonNull(), 0 == R.isAnyNonNull());
                ASSERT(0 == isAnySet(R));
                LOOP_ASSERT(R.isAnyNull(), 1 == R.isAnyNull());

                mR.replaceValues(mR); // alias test
                LOOP_ASSERT(R[0].theInt(), UNSET_BDEM_INT == R[0].theInt());
                LOOP_ASSERT(R.theInt(0),   UNSET_BDEM_INT == R.theInt(0));
                LOOP_ASSERT(R.isAnyNonNull(), 0 == R.isAnyNonNull());
                ASSERT(0 == isAnySet(R));
                LOOP_ASSERT(R.isAnyNull(), 1 == R.isAnyNull());
            }

            if (verbose) cout << "\t\t bdex stream out (and back in)" << endl;
            {
                bdex_TestOutStream out;
                ASSERT(out);

                int version = bdem_Row::maxSupportedBdexVersion();

                LOOP_ASSERT(R.elemType(0),
                            bdem_ElemType::BDEM_INT == R.elemType(0));

                bdex_TestOutStream& out2 = R.bdexStreamOut(out, version);
                ASSERT(out);
                ASSERT(&out == &out2);

                bdex_TestInStream in(out.data(), out.length());
                in.setSuppressVersionCheck(1);
                ASSERT(in);    ASSERT(!in.isEmpty());

                bdem_RowLayout mL(ETA, 1, EDA, NDA);
                bdem_RowData mRD2(&mL, S, NDA);
                bdem_Row& mR2 = *reinterpret_cast<bdem_Row *>(&mRD2);
                const bdem_Row& R2 = mR2;
                LOOP_ASSERT(R2.elemType(0),
                            bdem_ElemType::BDEM_INT == R2.elemType(0));
                mR2[0].theModifiableInt() = 90;

                bdex_TestInStream& in2 = mR2.bdexStreamIn(in, version);
                ASSERT(in);
                ASSERT(&in2 == &in);
                ASSERT(R2.isAnyNull());        ASSERT(!R2.isAnyNonNull());
                ASSERT(1 == R2.length());
                LOOP2_ASSERT(R.length(), R2.length(),
                                                    R.length() == R2.length());
                ASSERT(R == R2);

                // do it with a non-null value
                out.removeAll();
                mR[0].theModifiableInt() = 22;
                R.bdexStreamOut(out, version);
                in.reload(out.data(), out.length());
                mR2.bdexStreamIn(in, version);
                ASSERT(in);
                ASSERT(!R2.isAnyNull());        ASSERT(R2.isAnyNonNull());
                ASSERT(1 == R2.length());
                LOOP2_ASSERT(R.length(), R2.length(),
                                                    R.length() == R2.length());
                ASSERT(22 == R2[0].theInt());
                ASSERT(R == R2);
            }

            if (verbose) cout << "\t\tstd::ostream print" << endl;
            {
                mR[0].theModifiableInt() = 5;
                char buffer[1000];
                ostrstream out(buffer, sizeof buffer);
                R.print(out, 1, 2); out << ends;
                if (veryVerbose) {
                    cout << "\t\t\tbuffer: ``";
                    for (const char *p = buffer; *p; ++p) {
                        if ('\n' == *p)     cout << 'N';
                        else if (' ' == *p) cout << '_';
                        else if ('{' == *p) cout << *p;
                        else if ('}' == *p) cout << *p;
                        else if ('I' == *p) cout << *p;
                        else if ('N' == *p) cout << *p;
                        else if ('T' == *p) cout << *p;
                        else if ('5' == *p) cout << *p;
                        else                cout << "{0x" << unsigned(*p)<<'}';
                    }
                    cout << "''" << endl;
                }
                ASSERT(0 == strcmp("  {\n    INT 5\n  }\n", buffer));
            }
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nCreate Rows with all 31 elem types" << endl;

        if (verbose) cout << "\tCreate 2 arrays of bdem_elemType" << endl;

        static const bdem_ElemType::Type ETA1[] = {
            bdem_ElemType::BDEM_CHAR,         bdem_ElemType::BDEM_SHORT,
            bdem_ElemType::BDEM_INT,          bdem_ElemType::BDEM_INT64,
            bdem_ElemType::BDEM_FLOAT,        bdem_ElemType::BDEM_DOUBLE,
            bdem_ElemType::BDEM_STRING,       bdem_ElemType::BDEM_DATETIME,
            bdem_ElemType::BDEM_DATE,         bdem_ElemType::BDEM_TIME,
            bdem_ElemType::BDEM_CHAR_ARRAY,   bdem_ElemType::BDEM_SHORT_ARRAY,
            bdem_ElemType::BDEM_INT_ARRAY,    bdem_ElemType::BDEM_INT64_ARRAY,
            bdem_ElemType::BDEM_FLOAT_ARRAY,  bdem_ElemType::BDEM_DOUBLE_ARRAY,
            bdem_ElemType::BDEM_STRING_ARRAY,
            bdem_ElemType::BDEM_DATETIME_ARRAY,
            bdem_ElemType::BDEM_DATE_ARRAY,   bdem_ElemType::BDEM_TIME_ARRAY,
            bdem_ElemType::BDEM_LIST,         bdem_ElemType::BDEM_TABLE,

            bdem_ElemType::BDEM_BOOL,         bdem_ElemType::BDEM_DATETIMETZ,
            bdem_ElemType::BDEM_DATETZ,       bdem_ElemType::BDEM_TIMETZ,
            bdem_ElemType::BDEM_BOOL_ARRAY,
            bdem_ElemType::BDEM_DATETIMETZ_ARRAY,
            bdem_ElemType::BDEM_DATETZ_ARRAY, bdem_ElemType::BDEM_TIMETZ_ARRAY
            // TBD
            // bdem_ElemType::BDEM_CHOICE
        };
        ASSERT(30 == sizeof ETA1/sizeof *ETA1);

        static const bdem_ElemType::Type ETA2[] = {
            // TBD
            // bdem_ElemType::BDEM_CHOICE,
            bdem_ElemType::BDEM_TIMETZ_ARRAY,
            bdem_ElemType::BDEM_DATETZ_ARRAY,
            bdem_ElemType::BDEM_DATETIMETZ_ARRAY,
            bdem_ElemType::BDEM_BOOL_ARRAY,
            bdem_ElemType::BDEM_TIMETZ,
            bdem_ElemType::BDEM_DATETZ,
            bdem_ElemType::BDEM_DATETIMETZ,
            bdem_ElemType::BDEM_BOOL,

            bdem_ElemType::BDEM_TABLE,
            bdem_ElemType::BDEM_LIST,
            bdem_ElemType::BDEM_TIME_ARRAY,
            bdem_ElemType::BDEM_DATE_ARRAY,
            bdem_ElemType::BDEM_DATETIME_ARRAY,
            bdem_ElemType::BDEM_STRING_ARRAY,
            bdem_ElemType::BDEM_DOUBLE_ARRAY,
            bdem_ElemType::BDEM_FLOAT_ARRAY,
            bdem_ElemType::BDEM_INT64_ARRAY,
            bdem_ElemType::BDEM_INT_ARRAY,
            bdem_ElemType::BDEM_SHORT_ARRAY,
            bdem_ElemType::BDEM_CHAR_ARRAY,
            bdem_ElemType::BDEM_TIME,             bdem_ElemType::BDEM_DATE,
            bdem_ElemType::BDEM_DATETIME,         bdem_ElemType::BDEM_STRING,
            bdem_ElemType::BDEM_DOUBLE,           bdem_ElemType::BDEM_FLOAT,
            bdem_ElemType::BDEM_INT64,            bdem_ElemType::BDEM_INT,
            bdem_ElemType::BDEM_SHORT,            bdem_ElemType::BDEM_CHAR
        };
        ASSERT(30 == sizeof ETA2/sizeof *ETA2);

        if (verbose) cout << "\tSet up two RowLayouts" << endl;

        RowLayout mRL1(ETA1, 30, EDA, NDA);
        RowLayout mRL2(ETA2, 30, EDA, NDA);
        const RowLayout& RL1 = mRL1;
        const RowLayout& RL2 = mRL2;

        if (verbose) cout << "\tCreate two RowDatas" << endl;
        RowData mX1(&RL1, S, NDA); const RowData& X1 = mX1;
        RowData mC1(&RL1, S, NDA); const RowData& C1 = mC1;
        RowData mX2(&RL2, S, NDA); const RowData& X2 = mX2;
        RowData mC2(&RL2, S, NDA); const RowData& C2 = mC2;
        RowData mX3(&RL1, S, NDA); const RowData& X3 = mX3;

        if (verbose) cout << "\tCast them to Row objects" << endl;
        bdem_Row& mR1 = *reinterpret_cast<bdem_Row *>(&mX1);
        bdem_Row& mR2 = *reinterpret_cast<bdem_Row *>(&mX2);
        bdem_Row& mR3 = *reinterpret_cast<bdem_Row *>(&mX3);
        const bdem_Row &R1 = mR1, &R2 = mR2, &R3 = mR3;
        const bdem_Row& CR1 = *reinterpret_cast<bdem_Row *>(&mC1);
        const bdem_Row& CR2 = *reinterpret_cast<bdem_Row *>(&mC2);

        if (verbose) cout << "\tTest accessors" << endl;

        if (verbose) cout << "\t\tForward Type Order" << endl;
        ASSERT(30 == R1.length());
        ASSERT( 0 == R1.isAnyNonNull());
        ASSERT( 0 == isAnySet(R1));
        ASSERT( 1 == R1.isAnyNull());

        if (verbose) cout << "\t\tReverse Type Order" << endl;
        ASSERT(30 == R2.length());
        ASSERT( 0 == R2.isAnyNonNull());
        ASSERT( 0 == isAnySet(R2));
        ASSERT( 1 == R2.isAnyNull());

        #define TEST_ACCESSOR(ROW, TYPE, ETYPE, IDX) {                     \
          if (veryVerbose) cout << "\t\t\t" << #ETYPE << endl;             \
          {                                                                \
            ASSERT(bdem_ElemType::ETYPE == ROW.elemType(IDX));             \
            ASSERT(UNSET_ ## ETYPE == ROW.the ## TYPE(IDX));               \
            ASSERT(UNSET_ ## ETYPE == ROW[IDX].the ## TYPE());             \
          }                                                                \
        }

        // Scalar values - forward order - R1
        TEST_ACCESSOR(R1, Char,            BDEM_CHAR ,            0 );
        TEST_ACCESSOR(R1, Short,           BDEM_SHORT,            1 );
        TEST_ACCESSOR(R1, Int,             BDEM_INT,              2 );
        TEST_ACCESSOR(R1, Int64,           BDEM_INT64,            3 );
        TEST_ACCESSOR(R1, Float,           BDEM_FLOAT,            4 );
        TEST_ACCESSOR(R1, Double,          BDEM_DOUBLE,           5 );
        TEST_ACCESSOR(R1, String,          BDEM_STRING,           6 );
        TEST_ACCESSOR(R1, Datetime,        BDEM_DATETIME,         7 );
        TEST_ACCESSOR(R1, Date,            BDEM_DATE,             8 );
        TEST_ACCESSOR(R1, Time,            BDEM_TIME,             9 );
        TEST_ACCESSOR(R1, CharArray,       BDEM_CHAR_ARRAY,       10);
        TEST_ACCESSOR(R1, ShortArray,      BDEM_SHORT_ARRAY,      11);
        TEST_ACCESSOR(R1, IntArray,        BDEM_INT_ARRAY,        12);
        TEST_ACCESSOR(R1, Int64Array,      BDEM_INT64_ARRAY,      13);
        TEST_ACCESSOR(R1, FloatArray,      BDEM_FLOAT_ARRAY,      14);
        TEST_ACCESSOR(R1, DoubleArray,     BDEM_DOUBLE_ARRAY,     15);
        TEST_ACCESSOR(R1, StringArray,     BDEM_STRING_ARRAY,     16);
        TEST_ACCESSOR(R1, DatetimeArray,   BDEM_DATETIME_ARRAY,   17);
        TEST_ACCESSOR(R1, DateArray,       BDEM_DATE_ARRAY,       18);
        TEST_ACCESSOR(R1, TimeArray,       BDEM_TIME_ARRAY,       19);
        TEST_ACCESSOR(R1, List,            BDEM_LIST,             20);
        TEST_ACCESSOR(R1, Table,           BDEM_TABLE,            21);
        TEST_ACCESSOR(R1, Bool,            BDEM_BOOL,             22);
        TEST_ACCESSOR(R1, DatetimeTz,      BDEM_DATETIMETZ,       23);
        TEST_ACCESSOR(R1, DateTz,          BDEM_DATETZ,           24);
        TEST_ACCESSOR(R1, TimeTz,          BDEM_TIMETZ,           25);
        TEST_ACCESSOR(R1, BoolArray,       BDEM_BOOL_ARRAY,       26);
        TEST_ACCESSOR(R1, DatetimeTzArray, BDEM_DATETIMETZ_ARRAY, 27);
        TEST_ACCESSOR(R1, DateTzArray,     BDEM_DATETZ_ARRAY,     28);
        TEST_ACCESSOR(R1, TimeTzArray,     BDEM_TIMETZ_ARRAY,     29);
// TBD  TEST_ACCESSOR(R1, Choice,          BDEM_CHOICE,           30);

        // Aggregate values - reverse order - R2
        TEST_ACCESSOR(R2, Char,            BDEM_CHAR ,            29);
        TEST_ACCESSOR(R2, Short,           BDEM_SHORT,            28);
        TEST_ACCESSOR(R2, Int,             BDEM_INT,              27);
        TEST_ACCESSOR(R2, Int64,           BDEM_INT64,            26);
        TEST_ACCESSOR(R2, Float,           BDEM_FLOAT,            25);
        TEST_ACCESSOR(R2, Double,          BDEM_DOUBLE,           24);
        TEST_ACCESSOR(R2, String,          BDEM_STRING,           23);
        TEST_ACCESSOR(R2, Datetime,        BDEM_DATETIME,         22);
        TEST_ACCESSOR(R2, Date,            BDEM_DATE,             21);
        TEST_ACCESSOR(R2, Time,            BDEM_TIME,             20);
        TEST_ACCESSOR(R2, CharArray,       BDEM_CHAR_ARRAY,       19);
        TEST_ACCESSOR(R2, ShortArray,      BDEM_SHORT_ARRAY,      18);
        TEST_ACCESSOR(R2, IntArray,        BDEM_INT_ARRAY,        17);
        TEST_ACCESSOR(R2, Int64Array,      BDEM_INT64_ARRAY,      16);
        TEST_ACCESSOR(R2, FloatArray,      BDEM_FLOAT_ARRAY,      15);
        TEST_ACCESSOR(R2, DoubleArray,     BDEM_DOUBLE_ARRAY,     14);
        TEST_ACCESSOR(R2, StringArray,     BDEM_STRING_ARRAY,     13);
        TEST_ACCESSOR(R2, DatetimeArray,   BDEM_DATETIME_ARRAY,   12);
        TEST_ACCESSOR(R2, DateArray,       BDEM_DATE_ARRAY,       11);
        TEST_ACCESSOR(R2, TimeArray,       BDEM_TIME_ARRAY,       10);
        TEST_ACCESSOR(R2, List,            BDEM_LIST,             9 );
        TEST_ACCESSOR(R2, Table,           BDEM_TABLE,            8 );
        TEST_ACCESSOR(R2, Bool,            BDEM_BOOL,             7 );
        TEST_ACCESSOR(R2, DatetimeTz,      BDEM_DATETIMETZ,       6 );
        TEST_ACCESSOR(R2, DateTz,          BDEM_DATETZ,           5 );
        TEST_ACCESSOR(R2, TimeTz,          BDEM_TIMETZ,           4 );
        TEST_ACCESSOR(R2, BoolArray,       BDEM_BOOL_ARRAY,       3 );
        TEST_ACCESSOR(R2, DatetimeTzArray, BDEM_DATETIMETZ_ARRAY, 2 );
        TEST_ACCESSOR(R2, DateTzArray,     BDEM_DATETZ_ARRAY,     1 );
        TEST_ACCESSOR(R2, TimeTzArray,     BDEM_TIMETZ_ARRAY,     0 );
// TBD  TEST_ACCESSOR(R1, Choice,          BDEM_CHOICE,           -1);

        if (verbose) cout << "\t\tEquality and Gross Inequality" << endl;

        ASSERT(1 == (R1  == CR1));    ASSERT(0 == (R1 != CR1));
        ASSERT(1 == (CR1 == R1));     ASSERT(0 == (CR1 != R1));

        ASSERT(1 == (R2  == CR2));    ASSERT(0 == (R2 != CR2));
        ASSERT(1 == (CR2 == R2));     ASSERT(0 == (CR2 != R2));

        ASSERT(0 == (R1 == R2));      ASSERT(1 == (R1 != R2));
        ASSERT(0 == (R2 == R1));      ASSERT(1 == (R2 != R1));

        ASSERT(0 == (R1 == CR2));     ASSERT(1 == (R1 != CR2));
        ASSERT(0 == (R2 == CR1));     ASSERT(1 == (R2 != CR1));

        ASSERT(0 == (CR1 == R2));     ASSERT(1 == (CR1 != R2));
        ASSERT(0 == (CR2 == R1));     ASSERT(1 == (CR2 != R1));

        ASSERT(0 == (CR1 == CR2));    ASSERT(1 == (CR1 != CR2));
        ASSERT(0 == (CR2 == CR1));    ASSERT(1 == (CR2 != CR1));

        if (verbose) cout << "\tManipulators and Equality Operators" << endl;

        if (verbose) cout << "\t\tField By Field Manipulation" << endl;

        // We are going to change both the forward and the reverse formats.
        // We will leave the forward format, but undo the reverse one.
        // When this section is complete 'R1' will contain a non-null value
        // in each respective position; we will verify this property with the
        // 'areAllSet' method and then use that instance to test the
        // 'replaceValues' and 'makeAllNull' methods.

        #define TEST_UNSET_NULL(TYPE, ETYPE, NUM) {                   \
          if (veryVerbose) cout << "\t\t\t" << #ETYPE << endl;        \
          {                                                           \
            const int I = NUM, J = 29 - I;                            \
            ASSERT(R2 == CR2);                                        \
            ASSERT(0  == R2.isAnyNonNull());                          \
            ASSERT(1  == R2.isAnyNull());                             \
            ASSERT(1  == R2[J].isNull());                             \
                                                                      \
            mR2.theModifiable ## TYPE(J) = ETYPE ## _VALUE;           \
            ASSERT(ETYPE ## _VALUE == mR2.the ## TYPE(J));            \
            ASSERT(ETYPE ## _VALUE == mR2.theModifiable ## TYPE(J));  \
            ASSERT(R2 != CR2);                                        \
            ASSERT(1  == R2.isAnyNonNull());                          \
            ASSERT(1  == R2.isAnyNull());                             \
            ASSERT(0  == R2[J].isNull());                             \
                                                                      \
            ASSERT(UNSET_ ## ETYPE == R1[I].the ## TYPE());           \
            mR1[I].theModifiable ## TYPE() = R2.the ## TYPE(J);       \
            ASSERT(ETYPE ## _VALUE == R1[I].the ## TYPE());           \
            ASSERT(1 == R2.isAnyNull());                              \
                                                                      \
            if (veryVerbose) { P(R1[I]) P(R2[J]) P(CR1[I]) }          \
                                                                      \
            ASSERT(0  == R2[J].isNull());                             \
            mR2.makeNull(J);                                          \
            ASSERT(R2 == CR2);                                        \
            ASSERT(0  == R2.isAnyNonNull());                          \
            ASSERT(1  == R2.isAnyNull());                             \
            ASSERT(1  == R2[J].isNull());                             \
                                                                      \
            mR2.theModifiable ## TYPE(J) = ETYPE ## _VALUE;           \
            ASSERT(ETYPE ## _VALUE == mR2.the ## TYPE(J));            \
            ASSERT(ETYPE ## _VALUE == mR2.theModifiable ## TYPE(J));  \
            ASSERT(R2 != CR2);                                        \
            ASSERT(1  == R2.isAnyNonNull());                          \
            ASSERT(1  == R2.isAnyNull());                             \
            ASSERT(0  == R2[J].isNull());                             \
                                                                      \
            mR2.theModifiable ## TYPE(J) = UNSET_ ## ETYPE;           \
            ASSERT(UNSET_ ## ETYPE  == mR2.the ## TYPE(J));           \
            ASSERT(UNSET_ ## ETYPE  == mR2.theModifiable ## TYPE(J)); \
            ASSERT(R2 != CR2);                                        \
            ASSERT(1  == R2.isAnyNonNull());                          \
            ASSERT(1  == R2.isAnyNull());                             \
            ASSERT(0  == R2[J].isNull());                             \
            mR2.makeNull(J);                                          \
            ASSERT(R2 == CR2);                                        \
          }                                                           \
        }

        {
          TEST_UNSET_NULL(Char,            BDEM_CHAR,              0);
          TEST_UNSET_NULL(Short,           BDEM_SHORT,             1);
          TEST_UNSET_NULL(Int,             BDEM_INT,               2);
          TEST_UNSET_NULL(Int64,           BDEM_INT64,             3);
          TEST_UNSET_NULL(Float,           BDEM_FLOAT,             4);
          TEST_UNSET_NULL(Double,          BDEM_DOUBLE,            5);
          TEST_UNSET_NULL(String,          BDEM_STRING,            6);
          TEST_UNSET_NULL(Datetime,        BDEM_DATETIME,          7);
          TEST_UNSET_NULL(Date,            BDEM_DATE,              8);
          TEST_UNSET_NULL(Time,            BDEM_TIME,              9);
          TEST_UNSET_NULL(CharArray,       BDEM_CHAR_ARRAY,       10);
          TEST_UNSET_NULL(ShortArray,      BDEM_SHORT_ARRAY,      11);
          TEST_UNSET_NULL(IntArray,        BDEM_INT_ARRAY,        12);
          TEST_UNSET_NULL(Int64Array,      BDEM_INT64_ARRAY,      13);
          TEST_UNSET_NULL(FloatArray,      BDEM_FLOAT_ARRAY,      14);
          TEST_UNSET_NULL(DoubleArray,     BDEM_DOUBLE_ARRAY,     15);
          TEST_UNSET_NULL(StringArray,     BDEM_STRING_ARRAY,     16);
          TEST_UNSET_NULL(DatetimeArray,   BDEM_DATETIME_ARRAY,   17);
          TEST_UNSET_NULL(DateArray,       BDEM_DATE_ARRAY,       18);
          TEST_UNSET_NULL(TimeArray,       BDEM_TIME_ARRAY,       19);
          // list                                                 20
          TEST_UNSET_NULL(Bool,            BDEM_BOOL,             22);
          TEST_UNSET_NULL(DatetimeTz,      BDEM_DATETIMETZ,       23);
          TEST_UNSET_NULL(DateTz,          BDEM_DATETZ,           24);
          TEST_UNSET_NULL(TimeTz,          BDEM_TIMETZ,           25);
          TEST_UNSET_NULL(BoolArray,       BDEM_BOOL_ARRAY,       26);
          TEST_UNSET_NULL(DatetimeTzArray, BDEM_DATETIMETZ_ARRAY, 27);
          TEST_UNSET_NULL(DateTzArray,     BDEM_DATETZ_ARRAY,     28);
          TEST_UNSET_NULL(TimeTzArray,     BDEM_TIMETZ_ARRAY,     29);
        }

        if (veryVerbose) cout << "\t\t\tLIST" << endl;
        {
            const int I = 20, J = 29 - I;
            ASSERT(R2 == CR2);
            ASSERT(0 == R2.isAnyNonNull());
            ASSERT(0 == isAnySet(R2));

            // Note: must test this aspect of 'bdem_Row' in 'bdem_list.t.cpp'

            bdem_ListImp *elemPtrI =
                 reinterpret_cast<bdem_ListImp *>(&mR1.theModifiableList(I));
            ASSERT(elemPtrI);  // use
            bdem_ListImp& elemIRef =
                *reinterpret_cast<bdem_ListImp *>(&mR1[I].theModifiableList());

            bdem_ListImp& elemRefJ =
                *reinterpret_cast<bdem_ListImp *>(&mR2.theModifiableList(J));
            bdem_ListImp *elemJPtr =
                 reinterpret_cast<bdem_ListImp *>(&mR2[J].theModifiableList());
            ASSERT(elemJPtr);  // use

            const bdem_ListImp& UNSET_LIST_IMP =
                *reinterpret_cast<const bdem_ListImp *>(&UNSET_BDEM_LIST);
            const bdem_ListImp& LIST_IMP_VALUE =
                *reinterpret_cast<const bdem_ListImp *>(&LIST_VALUE);

            // mR2.theModifiableList(J) = LIST_VALUE;
            elemRefJ = LIST_IMP_VALUE;

            ASSERT(R2 != CR2);
            ASSERT(1 == R2.isAnyNonNull());
            ASSERT(1 == isAnySet(R2));

            ASSERT(1 == R1.isAnyNull());  // EXTRA CHECK FOR LAST ELEMENT SET

            ASSERT(UNSET_BDEM_LIST == R1[I].theList());

            // mR1[I].theModifiableList() = R2.theList(J);
            elemIRef = elemRefJ;

            ASSERT(LIST_VALUE == R1[I].theList());

            ASSERT(1 == R1.isAnyNull());  // EXTRA CHECK FOR LAST ELEMENT SET

            if (veryVerbose) { P(R1[I]) P(R2[J]) P(CR1[I]) }

            // mR2.theModifiableList(J) = UNSET_LIST;
            elemRefJ = UNSET_LIST_IMP;

            ASSERT(R2 != CR2);
            ASSERT(1 == R2.isAnyNonNull());
            ASSERT(0 == isAnySet(R2));

            mR2.makeNull(J);
            ASSERT(R2 == CR2);
            ASSERT(0 == R2.isAnyNonNull());
            ASSERT(0 == isAnySet(R2));
            ASSERT(1 == R2.isAnyNull());

#if !defined(BSLS_PLATFORM__CMP_MSVC)
            mR2.theModifiableList(J);      // clear nullness bit
#else
            &mR2[J].theModifiableList();   // clear nullness bit
#endif
            elemRefJ = UNSET_LIST_IMP;

            ASSERT(R2 != CR2);
            ASSERT(1 == R2.isAnyNonNull());
            ASSERT(0 == isAnySet(R2));
            ASSERT(1 == R2.isAnyNull());
        }

        if (veryVerbose) cout << "\t\t\tTABLE" << endl;
        {
            const int I = 21, J = 29 - I;
            ASSERT(R2 != CR2);
            ASSERT(1 == R2.isAnyNonNull());
            ASSERT(0 == isAnySet(R2));

            // Note: must test this aspect of 'bdem_Row' in 'bdem_table.t.cpp'

            bdem_TableImp *elemPtrI =
              reinterpret_cast<bdem_TableImp *>(&mR1.theModifiableTable(I));
            ASSERT(elemPtrI);  // use
            bdem_TableImp& elemIRef =
              *reinterpret_cast<bdem_TableImp *>(&mR1[I].theModifiableTable());

            bdem_TableImp& elemRefJ =
              *reinterpret_cast<bdem_TableImp *>(&mR2.theModifiableTable(J));
            bdem_TableImp *elemJPtr =
              reinterpret_cast<bdem_TableImp *>(&mR2[J].theModifiableTable());
            ASSERT(elemJPtr);  // use

            const bdem_TableImp& UNSET_TABLE_IMP =
                *reinterpret_cast<const bdem_TableImp *>(&UNSET_BDEM_TABLE);
            const bdem_TableImp& TABLE_IMP_VALUE =
                *reinterpret_cast<const bdem_TableImp *>(&TABLE_VALUE);

            // mR2.theModifiableList(J) = TABLE_VALUE;
            elemRefJ = TABLE_IMP_VALUE;

            ASSERT(R2 != CR2);
            ASSERT(1 == R2.isAnyNonNull());
            ASSERT(1 == isAnySet(R2));

            ASSERT(0 == R1.isAnyNull());  // EXTRA CHECK FOR LAST ELEMENT SET

            ASSERT(UNSET_BDEM_TABLE == R1[I].theTable());

            // mR1[I].theModifiableTable() = R2.theTable(J);
            elemIRef = elemRefJ;

            ASSERT(TABLE_VALUE == R1[I].theTable());

            if (veryVerbose) { P(R1[I]) P(R2[J]) P(CR1[I]) }

            // mR2.theModifiableTable(J) = UNSET_TABLE;
            elemRefJ = UNSET_TABLE_IMP;

            ASSERT(R2 != CR2);
            ASSERT(1 == R2.isAnyNonNull());
            ASSERT(0 == isAnySet(R2));
        }

        // New bdem types

        // TBD add test for bdem_Choice and bdem_ChoiceArray
        if (veryVerbose) {
            cout << "Visual Verification:" << endl;
            P(R1); cout << endl;
            P(R2); cout << endl;
            P(R3); cout << endl;
        }

        if (verbose) cout << "\t\tCopying Values Wholesale" << endl;

        // At this point we have R1 completely set and R3 of the same
        // layout completely unset.  We will verify 'replacesValues'
        // and then verify 'makeAllUnset' to conclude this test.

        LOOP2_ASSERT(CR1, R3, CR1 == R3);
        ASSERT(R1 != R3);
        ASSERT(0 == R3.isAnyNonNull());  ASSERT(1 == R3.isAnyNull());
        ASSERT(0 == isAnySet(R3));

        mR3.replaceValues(R1);

        ASSERT(CR1 != R3);               ASSERT(R1 == R3);
        ASSERT(1 == R3.isAnyNonNull());  ASSERT(0 == R3.isAnyNull());
        ASSERT(1 == isAnySet(R3));

        mR3.makeAllNull();

        ASSERT(CR1 == R3);               ASSERT(R1 != R3);
        ASSERT(0 == R3.isAnyNonNull());  ASSERT(1 == R3.isAnyNull());
        ASSERT(0 == isAnySet(R3));

        mR1.replaceValues(R3);

        ASSERT(CR1 == R3);               ASSERT(R1 == R3);
        ASSERT(0 == R3.isAnyNonNull());  ASSERT(1 == R3.isAnyNull());
        ASSERT(0 == isAnySet(R3));
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This test case is provided for the use of the developer.
        //
        // Tactics:
        //   - Ad Hoc Data Selection
        //   - Brute Force Implementation
        //
        // Testing:
        //   This test case *exercises* whatever, but tests nothing!
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        // Anything goes....

      } break;
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
