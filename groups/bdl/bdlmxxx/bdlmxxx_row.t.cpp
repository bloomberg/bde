// bdlmxxx_row.t.cpp                                                  -*-C++-*-

#include <bdlmxxx_row.h>

#include <bdlmxxx_aggregateoption.h>
#include <bdlmxxx_choicearrayimp.h>
#include <bdlmxxx_choiceimp.h>
#include <bdlmxxx_elemref.h>
#include <bdlmxxx_listimp.h>
#include <bdlmxxx_properties.h>
#include <bdlmxxx_rowdata.h>
#include <bdlmxxx_tableimp.h>

#include <bdlxxxx_testinstream.h>
#include <bdlxxxx_testoutstream.h>

#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_date.h>
#include <bdlt_datetz.h>
#include <bdlt_time.h>
#include <bdlt_timetz.h>

#include <bdltuxxx_unset.h>

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
// A 'bdlmxxx::Row' is not really an independent object, but merely another
// view on a 'bdlmxxx::RowData' defined in 'bdlmxxx_rowdata.h'.  All of the methods
// perform simple delegation, hence there is no need for a full-blown test.
// It will be sufficient to verify that each method is "hooked-up" properly.
// We can accomplish this goal with a single, comprehensive "breathing" test
// that exercises each function at least twice.
//
// TBD: these tests have not been extended to include elements that of type
// bdlmxxx::Choice or bdem_ChoiceArray.  Since bdltuxxx_unset doesn't support these
// types, this will probably never be worth doing.
//-----------------------------------------------------------------------------
// [ 2] bdlmxxx::ElemRef operator[](int index);
// [ 2] bdlmxxx::Row& replaceValues(const bdlmxxx::Row& other);
// [ 2] void makeAllNull();
// [ 2] void makeNull(int index);
// [ 2] bool& theBool(int index);
// [ 2] char& theChar(int index);
// [ 2] short& theShort(int index);
// [ 2] int& theInt(int index);
// [ 2] bsls::Types::Int64& theInt64(int index);
// [ 2] float& theFloat(int index);
// [ 2] double& theDouble(int index);
// [ 2] bsl::string& theString(int index);
// [ 2] bdlt::Datetime& theDatetime(int index);
// [ 2] bdlt::DatetimeTz& theDatetimeTz(int index);
// [ 2] bdlt::Date& theDate(int index);
// [ 2] bdlt::DateTz& theDateTz(int index);
// [ 2] bdlt::Time& theTime(int index);
// [ 2] bdlt::TimeTz& theTimeTz(int index);
// [ 2] bsl::vector<bool>& theBoolArray(int index);
// [ 2] bsl::vector<char>& theCharArray(int index);
// [ 2] bsl::vector<short>& theShortArray(int index);
// [ 2] bsl::vector<int>& theIntArray(int index);
// [ 2] bsl::vector<bsls::Types::Int64>& theInt64Array(int index);
// [ 2] bsl::vector<float>& theFloatArray(int index);
// [ 2] bsl::vector<double>& theDoubleArray(int index);
// [ 2] bsl::vector<bsl::string>& theStringArray(int index);
// [ 2] bsl::vector<bdlt::Datetime>& theDatetimeArray(int index);
// [ 2] bsl::vector<bdlt::DatetimeTz>& theDatetimeTzArray(int index);
// [ 2] bsl::vector<bdlt::Date>& theDateArray(int index);
// [ 2] bsl::vector<bdlt::DateTz>& theDateTzArray(int index);
// [ 2] bsl::vector<bdlt::Time>& theTimeArray(int index);
// [ 2] bsl::vector<bdlt::TimeTz>& theTimeTzArray(int index);
// [ 2] bdlmxxx::Choice& theChoice(int index);
// [ 2] bdlmxxx::ChoiceArray& theChoiceArray(int index);
// [ 2] bdlmxxx::List& theList(int index);
// [ 2] bdlmxxx::Table& theTable(int index);
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
// [ 2] bdlmxxx::ElemType::Type elemType(int index) const;
// [ 2] bdlmxxx::ConstElemRef operator[](int index) const;
// [ 2] const bool& theBool(int index) const;
// [ 2] const char& theChar(int index) const;
// [ 2] const short& theShort(int index) const;
// [ 2] const int& theInt(int index) const;
// [ 2] const bsls::Types::Int64& theInt64(int index) const;
// [ 2] const float& theFloat(int index) const;
// [ 2] const double& theDouble(int index) const;
// [ 2] const bsl::string& theString(int index) const;
// [ 2] const bdlt::Datetime& theDatetime(int index) const;
// [ 2] const bdlt::DatetimeTz& theDatetimeTz(int index) const;
// [ 2] const bdlt::Date& theDate(int index) const;
// [ 2] const bdlt::DateTz& theDateTz(int index) const;
// [ 2] const bdlt::Time& theTime(int index) const;
// [ 2] const bdlt::TimeTz& theTimeTz(int index) const;
// [ 2] const bsl::vector<bool>& theBoolArray(int index) const;
// [ 2] const bsl::vector<char>& theCharArray(int index) const;
// [ 2] const bsl::vector<short>& theShortArray(int index) const;
// [ 2] const bsl::vector<int>& theIntArray(int index) const;
// [ 2] const bsl::vector<Int64>& theInt64Array(int index) const;
// [ 2] const bsl::vector<float>& theFloatArray(int index) const;
// [ 2] const bsl::vector<double>& theDoubleArray(int index) const;
// [ 2] const bsl::vector<bsl::string>& theStringArray(int index) const;
// [ 2] const bsl::vector<bdlt::Datetime>& theDatetimeArray(int i) const;
// [ 2] const bsl::vector<bdlt::DatetimeTz>& theDatetimeTzArray(int i) const;
// [ 2] const bsl::vector<bdlt::Date>& theDateArray(int index) const;
// [ 2] const bsl::vector<bdlt::DateTz>& theDateTzArray(int index) const;
// [ 2] const bsl::vector<bdlt::Time>& theTimeArray(int index) const;
// [ 2] const bsl::vector<bdlt::TimeTz>& theTimeTzArray(int index) const;
// [ 2] const bdlmxxx::Choice& theChoice(int index) const;
// [ 2] const bdlmxxx::ChoiceArray& theChoiceArray(int index) const;
// [ 2] const bdlmxxx::List& theList(int index) const;
// [ 2] const bdlmxxx::Table& theTable(int index) const;
// [  ] bdexStreamOut(STREAM& stream, int version) const;
// [ 2] print(bsl::ostream& stream, int level, int spl const;
//
// [ 2] bool operator==(const bdlmxxx::Row& lhs, const bdlmxxx::Row& rhs);
// [ 2] bool operator!=(const bdlmxxx::Row& lhs, const bdlmxxx::Row& rhs);
// [  ] ostream& operator<<(ostream& stream, const bdlmxxx::Row& row);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE
// ----------------------------------------------------------------------------

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
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

typedef bsls::Types::Int64 Int64;
typedef bdlmxxx::RowData       RowData;
typedef bdlmxxx::RowLayout     RowLayout;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Overloaded Operators needed for Implicit Conversion of List& to ListImp&

bool operator==(const bdlmxxx::ListImp& lhs, const bdlmxxx::List& rhs) {
    return lhs == *reinterpret_cast<const bdlmxxx::ListImp *>(&rhs);
}
bool operator==(const bdlmxxx::List& lhs, const bdlmxxx::ListImp& rhs) {
    return *reinterpret_cast<const bdlmxxx::ListImp *>(&lhs) == rhs;
}
bool operator==(const bdlmxxx::List& lhs, const bdlmxxx::List& rhs) {
    return *reinterpret_cast<const bdlmxxx::ListImp *>(&lhs)
        == *reinterpret_cast<const bdlmxxx::ListImp *>(&rhs);
}

// Overloaded Operators needed for Implicit Conversion of Table& to TableImp&

bool operator==(const bdlmxxx::TableImp& lhs, const bdlmxxx::Table& rhs) {
    return lhs == *reinterpret_cast<const bdlmxxx::TableImp *>(&rhs);
}
bool operator==(const bdlmxxx::Table& lhs, const bdlmxxx::TableImp& rhs) {
    return *reinterpret_cast<const bdlmxxx::TableImp *>(&lhs) == rhs;
}
bool operator==(const bdlmxxx::Table& lhs, const bdlmxxx::Table& rhs) {
    return *reinterpret_cast<const bdlmxxx::TableImp *>(&lhs)
        == *reinterpret_cast<const bdlmxxx::TableImp *>(&rhs);
}

template <class TYPE>
bool isAnySet(const TYPE& object)
    // Return 'true' if any element in the specified 'object' is "set", that is
    // it does not have the "unset" value, and 'false' otherwise.
{
    const int len = object.length();
    for (int i = 0; i < len; ++i) {
        const bdlmxxx::ConstElemRef& elemRef = object[i];
        if (!elemRef.descriptor()->isUnset(elemRef.data())) {
            return true;                                              // RETURN
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
// owned by a parent object such as a 'bdlmxxx::List' or 'bdlmxxx::Table'.  In this
// example we first illustrate how to create a row directly (most users will
// not generally need to follow this example, but rather will obtain rows from
// higher-level 'bdem' components).  Then we will demonstrate more typical row
// usage by getting and setting member elements.
//
// In order to create a row directly, we will need an array of descriptors
// (of type 'bdlmxxx::Descriptor'), which is in turn used to characterize the
// salient behavior of each element we plan to instantiate.  For this example,
// we propose to create a row containing a sequence of elements corresponding
// to the 'bdlmxxx::ElemType' enumerators 'BDEM_DOUBLE', 'BDEM_INT', 'BDEM_STRING',
// and 'BDEM_STRING_ARRAY'.  First, we create an array, 'ETA', of enumerators:
//..
    static const bdlmxxx::ElemType::Type ETA[] = {
        bdlmxxx::ElemType::BDEM_DOUBLE,
        bdlmxxx::ElemType::BDEM_INT,
        bdlmxxx::ElemType::BDEM_STRING,
        bdlmxxx::ElemType::BDEM_STRING_ARRAY
    };
//..
// Next, we create an array, 'EDA', of pointers to 'bdlmxxx::Descriptor' values.
// We need one appropriate pointer for each *distinct* element type in 'ETA'.
// The array index of that descriptor pointer must match the 'bdlmxxx::ElemType'
// enumerator value corresponding to its type:
//..
    static const bdlmxxx::Descriptor *EDA[] = {
        0, 0,
        &bdlmxxx::Properties::s_intAttr,
        0, 0,
        &bdlmxxx::Properties::s_doubleAttr,
        &bdlmxxx::Properties::s_stringAttr,
        0, 0, 0, /* arrays: */ 0, 0, 0, 0, 0, 0,
        &bdlmxxx::Properties::s_stringArrayAttr
    };
//..
// Next we create a 'bdlmxxx::RowLayout' object, 'mRL', that defines the layout of
// the 'bdem' elements in the rows that are created later:
//..
    bdlmxxx::RowLayout mRL(ETA,         // array of element type enumerators
                       4,           // length of row
                       EDA,         // (sparse) array of element descriptors
                       bslma::Default::defaultAllocator());
//..
// Given 'mRL', we now instantiate two (modifiable) row definitions, 'mRD1' and
// 'mRD2':
//..
    bdlmxxx::RowData mRD1(&mRL,
                      bdlmxxx::AggregateOption::BDEM_PASS_THROUGH,
                      bslma::Default::defaultAllocator());

    bdlmxxx::RowData mRD2(&mRL,
                      bdlmxxx::AggregateOption::BDEM_PASS_THROUGH,
                      bslma::Default::defaultAllocator());

//..
// By design, a 'bdlmxxx::Row' contains exactly one data member of type
// 'bdlmxxx::RowData'.  Casting the address of a 'bdlmxxx::RowData' to a 'bdlmxxx::Row *'
// provides a limited "view" of the row data that is appropriate for typical
// general users (such as clients of lists and tables):
//..
    bdlmxxx::Row &row1 = *reinterpret_cast<bdlmxxx::Row *>(&mRD1);
    bdlmxxx::Row &row2 = *reinterpret_cast<bdlmxxx::Row *>(&mRD2);
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
// The second style of access returns a 'bdlmxxx::ElemRef' by value.  This "proxy"
// can then be used to access the element:
//..
    bdlmxxx::ElemRef elemRef0          = row2[0];
    elemRef0.theModifiableDouble() = 2.5;

    bdlmxxx::ElemRef elemRef1          = row2[1];
    elemRef1.theModifiableInt()    = 34;

    bdlmxxx::ElemRef elemRef2          = row2[2];
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
        //   bool operator==(const bdlmxxx::Row& lhs, const bdlmxxx::Row& rhs);
        //   bool operator!=(const bdlmxxx::Row& lhs, const bdlmxxx::Row& rhs);
        //
        //   bdlmxxx::ConstElemRef operator[](int index) const;
        //
        //   const bool& theBool(int index) const;
        //   const char& theChar(int index) const;
        //   const short& theShort(int index) const;
        //   const int& theInt(int index) const;
        //   const bsls::Types::Int64& theInt64(int index) const;
        //   const float& theFloat(int index) const;
        //   const double& theDouble(int index) const;
        //   const bsl::string& theString(int index) const;
        //   const bdlt::Datetime& theDatetime(int index) const;
        //   const bdlt::DatetimeTz& theDatetimeTz(int index) const;
        //   const bdlt::Date& theDate(int index) const;
        //   const bdlt::DateTz& theDateTz(int index) const;
        //   const bdlt::Time& theTime(int index) const;
        //   const bdlt::TimeTz& theTimeTz(int index) const;
        //   const bsl::vector<bool>& theBoolArray(int index) const;
        //   const bsl::vector<char>& theCharArray(int index) const;
        //   const bsl::vector<short>& theShortArray(int index) const;
        //   const bsl::vector<int>& theIntArray(int index) const;
        //   const bsl::vector<Int64>& theInt64Array(int index) const;
        //   const bsl::vector<float>& theFloatArray(int index) const;
        //   const bsl::vector<double>& theDoubleArray(int index) const;
        //   const bsl::vector<bsl::string>& theStringArray(int index) const;
        //   const bsl::vector<bdlt::Datetime>& theDatetimeArray(int i) const;
        //   const bsl::vector<bdlt::DatetimeTz>& theDatetimeTzArray(int i)
        //                                                              const;
        //   const bsl::vector<bdlt::Date>& theDateArray(int index) const;
        //   const bsl::vector<bdlt::DateTz>& theDateTzArray(int index) const;
        //   const bsl::vector<bdlt::Time>& theTimeArray(int index) const;
        //   const bsl::vector<bdlt::TimeTz>& theTimeTzArray(int index) const;
        //   const bdlmxxx::Choice& theChoice(int index) const;
        //   const bdlmxxx::ChoiceArray& theChoiceArray(int index) const;
        //   const bdlmxxx::List& theList(int index) const;
        //   const bdlmxxx::Table& theTable(int index) const;
        //
        //   ? bdexStreamOut(STREAM& stream, int version) const;
        //   ?? ostream& operator<<(ostream& stream, const bdlmxxx::Row& row);
        //
        //   bdlmxxx::ElemRef operator[](int index);
        //
        //   bool& theModifiableBool(int index);
        //   char& theModifiableChar(int index);
        //   short& theModifiableShort(int index);
        //   int& theModifiableInt(int index);
        //   bsls::Types::Int64& theModifiableInt64(int index);
        //   float& theModifiableFloat(int index);
        //   double& theModifiableDouble(int index);
        //   bsl::string& theModifiableString(int index);
        //   bdlt::Datetime& theModifiableDatetime(int index);
        //   bdlt::DatetimeTz& theModifiableDatetimeTz(int index);
        //   bdlt::Date& theModifiableDate(int index);
        //   bdlt::DateTz& theModifiableDateTz(int index);
        //   bdlt::Time& theModifiableTime(int index);
        //   bdlt::TimeTz& theModifiableTimeTz(int index);
        //   bsl::vector<bool>& theModifiableBoolArray(int index);
        //   bsl::vector<char>& theModifiableCharArray(int index);
        //   bsl::vector<short>& theModifiableShortArray(int index);
        //   bsl::vector<int>& theModifiableIntArray(int index);
        //   bsl::vector<bsls::Types::Int64>&
        //                                  theModifiableInt64Array(int index);
        //   bsl::vector<float>& theModifiableFloatArray(int index);
        //   bsl::vector<double>& theModifiableDoubleArray(int index);
        //   bsl::vector<bsl::string>& theModifiableStringArray(int index);
        //   bsl::vector<bdlt::Datetime>& theModifiableDatetimeArray(int index);
        //   bsl::vector<bdlt::DatetimeTz>&
        //                             theModifiableDatetimeTzArray(int index);
        //   bsl::vector<bdlt::Date>& theModifiableDateArray(int index);
        //   bsl::vector<bdlt::DateTz>& theModifiableDateTzArray(int index);
        //   bsl::vector<bdlt::Time>& theModifiableTimeArray(int index);
        //   bsl::vector<bdlt::TimeTz>& theModifiableTimeTzArray(int index);
        //   bdlmxxx::Choice& theModifiableChoice(int index);
        //   bdlmxxx::ChoiceArray& theModifiableChoiceArray(int index);
        //   bdlmxxx::List& theModifiableList(int index);
        //   bdlmxxx::Table& theModifiableTable(int index);
        //
        //   bdlmxxx::ElemType::Type elemType(int index) const;
        //
        //   ?bdexStreamIn(STREAM& stream, int version);
        //
        //   bdlmxxx::Row& replaceValues(const bdlmxxx::Row& other);
        //   void makeAllNull();
        //   void makeNull(int index);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "DELEGATION TEST" << endl
                          << "===============" << endl;

        const bdlmxxx::AggregateOption::AllocationStrategy S =
            bdlmxxx::AggregateOption::BDEM_PASS_THROUGH;

        bslma::Allocator *const NDA = bslma::Default::defaultAllocator();

        if (verbose) cout << "\nCreate an empty Row" << endl;
        {
            if (verbose) cout << "\tCreate an empty RowDef" << endl;
            RowLayout mRL; const RowLayout& RL = mRL;
            RowData mX(&mRL, S, NDA); const RowData& X = mX;
            if (veryVerbose) { cout << "\t\t"; P(X.length()); }
            ASSERT(0 == X.length());

            if (verbose) cout << "\tTry various accessor functions" << endl;

            bdlmxxx::Row& mR = *reinterpret_cast<bdlmxxx::Row *>(&mX);
            const bdlmxxx::Row& R = mR;

            if (verbose) cout << "\t\ttrivial accessors" << endl;
            {
                LOOP_ASSERT(R.length(), 0 == R.length());

                LOOP_ASSERT(R.isAnyNonNull(), 0 == R.isAnyNonNull());

                ASSERT(0 == isAnySet(R));

                LOOP_ASSERT(R.isAnyNull(), 0 == R.isAnyNull());
            }

            if (verbose) cout << "\t\t bdex stream out (and back in)" << endl;
            {
                bdlxxxx::TestOutStream out;

                int version = bdlmxxx::Row::maxSupportedBdexVersion();

                bdlxxxx::TestOutStream& out2 = R.bdexStreamOut(out, version);
                ASSERT(&out == &out2);

                bdlxxxx::TestInStream in(out.data(), out.length());

                bdlmxxx::RowLayout mRD2(bdlmxxx::RowLayout::InitialCapacity(0), NDA);
                bdlmxxx::RowData   header2(
                                      &mRD2,
                                      bdlmxxx::AggregateOption::BDEM_PASS_THROUGH);
                bdlmxxx::Row& mR2 = *reinterpret_cast<bdlmxxx::Row *>(&header2);
                const bdlmxxx::Row& R2 = mR2;

                bdlxxxx::TestInStream& in2 = mR2.bdexStreamIn(in, version);
                ASSERT(&in2 == &in);
                ASSERT(R == R2);
            }

            if (verbose) cout << "\t\tbsl::ostream print" << endl;
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

        typedef bdlmxxx::Properties Prop;
        typedef bdlmxxx::ChoiceImp  CImp;
        typedef bdlmxxx::ListImp    Limp;
        typedef bdlmxxx::TableImp   Timp;

        static const bdlmxxx::Descriptor *EDA[] = {
            &Prop::s_charAttr,          &Prop::s_shortAttr,
            &Prop::s_intAttr,           &Prop::s_int64Attr,
            &Prop::s_floatAttr,         &Prop::s_doubleAttr,
            &Prop::s_stringAttr,        &Prop::s_datetimeAttr,
            &Prop::s_dateAttr,          &Prop::s_timeAttr,
            &Prop::s_charArrayAttr,     &Prop::s_shortArrayAttr,
            &Prop::s_intArrayAttr,      &Prop::s_int64ArrayAttr,
            &Prop::s_floatArrayAttr,    &Prop::s_doubleArrayAttr,
            &Prop::s_stringArrayAttr,   &Prop::s_datetimeArrayAttr,
            &Prop::s_dateArrayAttr,     &Prop::s_timeArrayAttr,
            &Limp::s_listAttr,          &Timp::s_tableAttr,

            &Prop::s_boolAttr,          &Prop::s_datetimeTzAttr,
            &Prop::s_dateTzAttr,        &Prop::s_timeTzAttr,
            &Prop::s_boolArrayAttr,     &Prop::s_datetimeTzArrayAttr,
            &Prop::s_dateTzArrayAttr,   &Prop::s_timeTzArrayAttr
              // TBD
//             &CImp::s_choiceDescriptor
        };
        ASSERT(30 == sizeof EDA/sizeof *EDA);

        if (verbose) cout << "\tCreate all 31 unset element values" << endl;

        // unset scalar values
        const bool     UNSET_BDEM_BOOL   = bdltuxxx::Unset<bool>::unsetValue();
        const char     UNSET_BDEM_CHAR   = bdltuxxx::Unset<char>::unsetValue();
        const short    UNSET_BDEM_SHORT  = bdltuxxx::Unset<short>::unsetValue();
        const int      UNSET_BDEM_INT    = bdltuxxx::Unset<int>::unsetValue();
        const Int64    UNSET_BDEM_INT64  = bdltuxxx::Unset<Int64>::unsetValue();
        const float    UNSET_BDEM_FLOAT  = bdltuxxx::Unset<float>::unsetValue();
        const double   UNSET_BDEM_DOUBLE = bdltuxxx::Unset<double>::unsetValue();
        const bsl::string
                       UNSET_BDEM_STRING =
                                        bdltuxxx::Unset<bsl::string>::unsetValue();
        const bdlt::Datetime
                       UNSET_BDEM_DATETIME =
                                      bdltuxxx::Unset<bdlt::Datetime>::unsetValue();
        const bdlt::DatetimeTz
                       UNSET_BDEM_DATETIMETZ =
                                    bdltuxxx::Unset<bdlt::DatetimeTz>::unsetValue();
        const bdlt::Date
                       UNSET_BDEM_DATE =
                                          bdltuxxx::Unset<bdlt::Date>::unsetValue();
        const bdlt::DateTz
                       UNSET_BDEM_DATETZ =
                                        bdltuxxx::Unset<bdlt::DateTz>::unsetValue();
        const bdlt::Time
                       UNSET_BDEM_TIME = bdltuxxx::Unset<bdlt::Time>::unsetValue();
        const bdlt::TimeTz
                       UNSET_BDEM_TIMETZ =
                                        bdltuxxx::Unset<bdlt::TimeTz>::unsetValue();

        // unset array values
        const bsl::vector<bool>          UNSET_BDEM_BOOL_ARRAY;
        const bsl::vector<char>          UNSET_BDEM_CHAR_ARRAY;
        const bsl::vector<short>         UNSET_BDEM_SHORT_ARRAY;
        const bsl::vector<int>           UNSET_BDEM_INT_ARRAY;
        const bsl::vector<Int64>         UNSET_BDEM_INT64_ARRAY;
        const bsl::vector<float>         UNSET_BDEM_FLOAT_ARRAY;
        const bsl::vector<double>        UNSET_BDEM_DOUBLE_ARRAY;
        const bsl::vector<bsl::string>   UNSET_BDEM_STRING_ARRAY;
        const bsl::vector<bdlt::Datetime> UNSET_BDEM_DATETIME_ARRAY;
        const bsl::vector<bdlt::DatetimeTz> UNSET_BDEM_DATETIMETZ_ARRAY;
        const bsl::vector<bdlt::Date>     UNSET_BDEM_DATE_ARRAY;
        const bsl::vector<bdlt::DateTz>     UNSET_BDEM_DATETZ_ARRAY;
        const bsl::vector<bdlt::Time>     UNSET_BDEM_TIME_ARRAY;
        const bsl::vector<bdlt::TimeTz>     UNSET_BDEM_TIMETZ_ARRAY;

        // unset aggregate values
        // TBD
//         const bdlmxxx::ChoiceImp            UNSET_BDEM_CHOICE(S, 0);
        const bdlmxxx::ListImp              UNSET_BDEM_LIST(S);
        const bdlmxxx::TableImp             UNSET_BDEM_TABLE(S);

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
        const bdlt::Date     BDEM_DATE_VALUE(9876, 12, 30);
        const bdlt::DateTz   BDEM_DATETZ_VALUE(BDEM_DATE_VALUE, 3);
        const bdlt::Time     BDEM_TIME_VALUE(12, 34, 56, 789);
        const bdlt::TimeTz   BDEM_TIMETZ_VALUE(BDEM_TIME_VALUE, 12);
        const bdlt::Datetime BDEM_DATETIME_VALUE(BDEM_DATE_VALUE,
                                                BDEM_TIME_VALUE);
        const bdlt::DatetimeTz BDEM_DATETIMETZ_VALUE(BDEM_DATETIME_VALUE, -5);

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
        bsl::vector<bdlt::Datetime> tmpDTA;
        tmpDTA.push_back(BDEM_DATETIME_VALUE);
        bsl::vector<bdlt::DatetimeTz> tmpDTZA;
        tmpDTZA.push_back(BDEM_DATETIMETZ_VALUE);
        bsl::vector<bdlt::Date>     tmpDateA;
        tmpDateA.push_back(BDEM_DATE_VALUE);
        bsl::vector<bdlt::DateTz>   tmpDateTzA;
        tmpDateTzA.push_back(BDEM_DATETZ_VALUE);
        bsl::vector<bdlt::Time>     tmpTimeA;
        tmpTimeA.push_back(BDEM_TIME_VALUE);
        bsl::vector<bdlt::TimeTz>   tmpTimeTzA;
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
        const bsl::vector<bdlt::Datetime>& BDEM_DATETIME_ARRAY_VALUE
                                                                  = tmpDTA;
        const bsl::vector<bdlt::DatetimeTz>&
                                          BDEM_DATETIMETZ_ARRAY_VALUE
                                                                  = tmpDTZA;
        const bsl::vector<bdlt::Date>&     BDEM_DATE_ARRAY_VALUE   = tmpDateA;
        const bsl::vector<bdlt::DateTz>&   BDEM_DATETZ_ARRAY_VALUE = tmpDateTzA;
        const bsl::vector<bdlt::Time>&     BDEM_TIME_ARRAY_VALUE   = tmpTimeA;
        const bsl::vector<bdlt::TimeTz>&   BDEM_TIMETZ_ARRAY_VALUE = tmpTimeTzA;

        // setup for aggregate values
        // TBD
//         bdlmxxx::ChoiceImp tmpChoice(S, 0);
//         tmpChoice.addSelection(&Prop::s_intAttr);
//         int tmpChoiceValue = 100;
//         tmpChoice[0].theModifiableInt() = tmpChoiceValue;

        bdlmxxx::ListImp  tmpList(S);
        int tmpListValue = 100;
        tmpList.insertElement(0, &tmpListValue, EDA[bdlmxxx::ElemType::BDEM_INT]);

        bdlmxxx::TableImp tmpTable(S);
        tmpTable.insertNullRows(0, 1); // table with zero columns, one row

        // non-null aggregate values: LIST { INT 100 } and TABLE { [ ] { } }
        const bdlmxxx::List& LIST_VALUE = *reinterpret_cast<bdlmxxx::List *>(&tmpList);
        const bdlmxxx::Table& TABLE_VALUE =
            *reinterpret_cast<bdlmxxx::Table*>(&tmpTable);

        //cout << "FIX ME: I notice that the output for table is all "
        //    "        [ ]'s\n and I believe that arrays should be [ ]'s and "
        //    "        list/table\n should be { }'s -- please advise" << endl;

        // --------------------------------------------------------------------

        if (verbose) cout << "\nCreate a Row with a single Int" << endl;
        {
            if (verbose) cout << "\tSet up the element type array" << endl;
            static const bdlmxxx::ElemType::Type ETA[] = {
                bdlmxxx::ElemType::BDEM_INT
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
            bdlmxxx::Row& mR = *reinterpret_cast<bdlmxxx::Row *>(&mX);
            const bdlmxxx::Row& R = mR;

            if (verbose) cout << "\tTry various functions" << endl;

            if (verbose) cout << "\t\taccessors" << endl;
            {
                LOOP_ASSERT(R.length(), 1 == R.length());

                LOOP_ASSERT(R.elemType(0),
                            bdlmxxx::ElemType::BDEM_INT == R.elemType(0));

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
                bdlxxxx::TestOutStream out;
                ASSERT(out);

                int version = bdlmxxx::Row::maxSupportedBdexVersion();

                LOOP_ASSERT(R.elemType(0),
                            bdlmxxx::ElemType::BDEM_INT == R.elemType(0));

                bdlxxxx::TestOutStream& out2 = R.bdexStreamOut(out, version);
                ASSERT(out);
                ASSERT(&out == &out2);

                bdlxxxx::TestInStream in(out.data(), out.length());
                in.setSuppressVersionCheck(1);
                ASSERT(in);    ASSERT(!in.isEmpty());

                bdlmxxx::RowLayout mL(ETA, 1, EDA, NDA);
                bdlmxxx::RowData mRD2(&mL, S, NDA);
                bdlmxxx::Row& mR2 = *reinterpret_cast<bdlmxxx::Row *>(&mRD2);
                const bdlmxxx::Row& R2 = mR2;
                LOOP_ASSERT(R2.elemType(0),
                            bdlmxxx::ElemType::BDEM_INT == R2.elemType(0));
                mR2[0].theModifiableInt() = 90;

                bdlxxxx::TestInStream& in2 = mR2.bdexStreamIn(in, version);
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

            if (verbose) cout << "\t\tbsl::ostream print" << endl;
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

        if (verbose) cout << "\tCreate 2 arrays of bdem::elemType" << endl;

        static const bdlmxxx::ElemType::Type ETA1[] = {
            bdlmxxx::ElemType::BDEM_CHAR,         bdlmxxx::ElemType::BDEM_SHORT,
            bdlmxxx::ElemType::BDEM_INT,          bdlmxxx::ElemType::BDEM_INT64,
            bdlmxxx::ElemType::BDEM_FLOAT,        bdlmxxx::ElemType::BDEM_DOUBLE,
            bdlmxxx::ElemType::BDEM_STRING,       bdlmxxx::ElemType::BDEM_DATETIME,
            bdlmxxx::ElemType::BDEM_DATE,         bdlmxxx::ElemType::BDEM_TIME,
            bdlmxxx::ElemType::BDEM_CHAR_ARRAY,   bdlmxxx::ElemType::BDEM_SHORT_ARRAY,
            bdlmxxx::ElemType::BDEM_INT_ARRAY,    bdlmxxx::ElemType::BDEM_INT64_ARRAY,
            bdlmxxx::ElemType::BDEM_FLOAT_ARRAY,  bdlmxxx::ElemType::BDEM_DOUBLE_ARRAY,
            bdlmxxx::ElemType::BDEM_STRING_ARRAY,
            bdlmxxx::ElemType::BDEM_DATETIME_ARRAY,
            bdlmxxx::ElemType::BDEM_DATE_ARRAY,   bdlmxxx::ElemType::BDEM_TIME_ARRAY,
            bdlmxxx::ElemType::BDEM_LIST,         bdlmxxx::ElemType::BDEM_TABLE,

            bdlmxxx::ElemType::BDEM_BOOL,         bdlmxxx::ElemType::BDEM_DATETIMETZ,
            bdlmxxx::ElemType::BDEM_DATETZ,       bdlmxxx::ElemType::BDEM_TIMETZ,
            bdlmxxx::ElemType::BDEM_BOOL_ARRAY,
            bdlmxxx::ElemType::BDEM_DATETIMETZ_ARRAY,
            bdlmxxx::ElemType::BDEM_DATETZ_ARRAY, bdlmxxx::ElemType::BDEM_TIMETZ_ARRAY
            // TBD
            // bdlmxxx::ElemType::BDEM_CHOICE
        };
        ASSERT(30 == sizeof ETA1/sizeof *ETA1);

        static const bdlmxxx::ElemType::Type ETA2[] = {
            // TBD
            // bdlmxxx::ElemType::BDEM_CHOICE,
            bdlmxxx::ElemType::BDEM_TIMETZ_ARRAY,
            bdlmxxx::ElemType::BDEM_DATETZ_ARRAY,
            bdlmxxx::ElemType::BDEM_DATETIMETZ_ARRAY,
            bdlmxxx::ElemType::BDEM_BOOL_ARRAY,
            bdlmxxx::ElemType::BDEM_TIMETZ,
            bdlmxxx::ElemType::BDEM_DATETZ,
            bdlmxxx::ElemType::BDEM_DATETIMETZ,
            bdlmxxx::ElemType::BDEM_BOOL,

            bdlmxxx::ElemType::BDEM_TABLE,
            bdlmxxx::ElemType::BDEM_LIST,
            bdlmxxx::ElemType::BDEM_TIME_ARRAY,
            bdlmxxx::ElemType::BDEM_DATE_ARRAY,
            bdlmxxx::ElemType::BDEM_DATETIME_ARRAY,
            bdlmxxx::ElemType::BDEM_STRING_ARRAY,
            bdlmxxx::ElemType::BDEM_DOUBLE_ARRAY,
            bdlmxxx::ElemType::BDEM_FLOAT_ARRAY,
            bdlmxxx::ElemType::BDEM_INT64_ARRAY,
            bdlmxxx::ElemType::BDEM_INT_ARRAY,
            bdlmxxx::ElemType::BDEM_SHORT_ARRAY,
            bdlmxxx::ElemType::BDEM_CHAR_ARRAY,
            bdlmxxx::ElemType::BDEM_TIME,             bdlmxxx::ElemType::BDEM_DATE,
            bdlmxxx::ElemType::BDEM_DATETIME,         bdlmxxx::ElemType::BDEM_STRING,
            bdlmxxx::ElemType::BDEM_DOUBLE,           bdlmxxx::ElemType::BDEM_FLOAT,
            bdlmxxx::ElemType::BDEM_INT64,            bdlmxxx::ElemType::BDEM_INT,
            bdlmxxx::ElemType::BDEM_SHORT,            bdlmxxx::ElemType::BDEM_CHAR
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
        bdlmxxx::Row& mR1 = *reinterpret_cast<bdlmxxx::Row *>(&mX1);
        bdlmxxx::Row& mR2 = *reinterpret_cast<bdlmxxx::Row *>(&mX2);
        bdlmxxx::Row& mR3 = *reinterpret_cast<bdlmxxx::Row *>(&mX3);
        const bdlmxxx::Row &R1 = mR1, &R2 = mR2, &R3 = mR3;
        const bdlmxxx::Row& CR1 = *reinterpret_cast<bdlmxxx::Row *>(&mC1);
        const bdlmxxx::Row& CR2 = *reinterpret_cast<bdlmxxx::Row *>(&mC2);

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
            ASSERT(bdlmxxx::ElemType::ETYPE == ROW.elemType(IDX));             \
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

            // Note: must test this aspect of 'bdlmxxx::Row' in 'bdlmxxx_list.t.cpp'

            bdlmxxx::ListImp *elemPtrI =
                 reinterpret_cast<bdlmxxx::ListImp *>(&mR1.theModifiableList(I));
            ASSERT(elemPtrI);  // use
            bdlmxxx::ListImp& elemIRef =
                *reinterpret_cast<bdlmxxx::ListImp *>(&mR1[I].theModifiableList());

            bdlmxxx::ListImp& elemRefJ =
                *reinterpret_cast<bdlmxxx::ListImp *>(&mR2.theModifiableList(J));
            bdlmxxx::ListImp *elemJPtr =
                 reinterpret_cast<bdlmxxx::ListImp *>(&mR2[J].theModifiableList());
            ASSERT(elemJPtr);  // use

            const bdlmxxx::ListImp& UNSET_LIST_IMP =
                *reinterpret_cast<const bdlmxxx::ListImp *>(&UNSET_BDEM_LIST);
            const bdlmxxx::ListImp& LIST_IMP_VALUE =
                *reinterpret_cast<const bdlmxxx::ListImp *>(&LIST_VALUE);

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

#if !defined(BSLS_PLATFORM_CMP_MSVC)
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

            // Note: must test this aspect of 'bdlmxxx::Row' in 'bdlmxxx_table.t.cpp'

            bdlmxxx::TableImp *elemPtrI =
              reinterpret_cast<bdlmxxx::TableImp *>(&mR1.theModifiableTable(I));
            ASSERT(elemPtrI);  // use
            bdlmxxx::TableImp& elemIRef =
              *reinterpret_cast<bdlmxxx::TableImp *>(&mR1[I].theModifiableTable());

            bdlmxxx::TableImp& elemRefJ =
              *reinterpret_cast<bdlmxxx::TableImp *>(&mR2.theModifiableTable(J));
            bdlmxxx::TableImp *elemJPtr =
              reinterpret_cast<bdlmxxx::TableImp *>(&mR2[J].theModifiableTable());
            ASSERT(elemJPtr);  // use

            const bdlmxxx::TableImp& UNSET_TABLE_IMP =
                *reinterpret_cast<const bdlmxxx::TableImp *>(&UNSET_BDEM_TABLE);
            const bdlmxxx::TableImp& TABLE_IMP_VALUE =
                *reinterpret_cast<const bdlmxxx::TableImp *>(&TABLE_VALUE);

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

        // TBD add test for bdlmxxx::Choice and bdlmxxx::ChoiceArray
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

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
