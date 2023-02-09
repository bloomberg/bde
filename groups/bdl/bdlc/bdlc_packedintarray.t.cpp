// bdlc_packedintarray.t.cpp                                          -*-C++-*-
#include <bdlc_packedintarray.h>

#include <bslalg_swaputil.h>

#include <bslim_testutil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_review.h>
#include <bsls_types.h>

#include <bslx_byteinstream.h>
#include <bslx_byteoutstream.h>
#include <bslx_instreamfunctions.h>
#include <bslx_outstreamfunctions.h>
#include <bslx_testinstream.h>
#include <bslx_testinstreamexception.h>
#include <bslx_testoutstream.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_map.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

#include <bsl_cstdint.h>
#include <bsl_utility.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a (value-semantic) container class, and
// a 'const_iterator' for that container.  The primary manipulators are
// therefore the basic methods for adding ('append') elements and emptying the
// array ('pop_back' and 'removeAll').  The basic accessors are those
// providing access to the data placed within the array (operator[]) and those
// allowing access to primary attributes of the array ('allocator',
// 'bytesPerElement', 'capacity', 'isEmpty', 'length').  After the standard
// 10-cases, the unique functionality of the container, which goes beyond an
// attribute class, is tested.  Effort is made to use only the primary
// manipulators and accessors whenever possible, thus making every test case
// independent.
//
// Primary Manipulators
//: o 'append'
//: o 'pop_back'
//: o 'removeAll'
//
// Basic Accessors
//: o 'operator[]'
//: o 'allocator'
//: o 'bytesPerElement'
//: o 'capacity'
//: o 'isEmpty'
//: o 'length'
//
// Certain standard value-semantic-type test cases are omitted:
//: o [ 8] -- GENERATOR FUNCTION
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o Any allocated memory is always from the object allocator.
//: o An object's value is independent of the allocator used to supply memory.
//: o Injected exceptions are safely propagated during memory allocation.
//: o Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//: o All explicit memory allocations are presumed to use the global, default,
//:   or object allocator.
//: o ACCESSOR methods are 'const' thread-safe.
//: o Individual attribute types are presumed to be *alias-safe*; hence, only
//:   certain methods require the testing of this property:
//:   o copy-assignment
//:   o swap
// ----------------------------------------------------------------------------
// [20] PackedIntArrayConstIterator();
// [20] PackedIntArrayConstIterator(const PIACI& original);
// [20] ~PackedIntArrayConstIterator();
// [20] PIACI& operator=(const PIACI& rhs);
// [20] PIACI& PIACI::operator++();
// [20] PIACI& PIACI::operator--();
// [20] TYPE PIACI::operator*() const;
// [20] TYPE PIACI::operator->() const;
// [20] TYPE PIACI::operator[](bsl::ptrdiff_t offset) const;
// [20] bool operator==(const PIACI& lhs, const PIAI& rhs);
// [20] bool operator!=(const PIACI& lhs, const PIAI& rhs);
// [22] PIACI& PIACI::operator+=(bsl::ptrdiff_t offset);
// [22] PIACI& PIACI::operator-=(bsl::ptrdiff_t offset);
// [22] PIACI PIACI::operator+(bsl::ptrdiff_t offset);
// [22] PIACI PIACI::operator-(bsl::ptrdiff_t offset);
// [22] PIACI PIACI::operator++(PIACI& iter, int);
// [22] PIACI PIACI::operator--(PIACI& iter, int);
// [22] bsl::ptrdiff_t operator-(const PIACI& lhs, const PIACI& rhs);
// [21] bool operator<(const PIACI& lhs, const PIACI& rhs);
// [21] bool operator<=(const PIACI& lhs, const PIACI& rhs);
// [21] bool operator>(const PIACI& lhs, const PIACI& rhs);
// [21] bool operator>=(const PIACI& lhs, const PIACI& rhs);
// [10] int maxSupportedBdexVersion(int serializationVersion);
// [ 2] PackedIntArray(bslma::Allocator *basicAllocator = 0);
// [18] PackedIntArray(numElements, value, basicAllocator);
// [ 7] PackedIntArray(const PackedIntArray& o, *bA = 0);
// [ 2] ~PackedIntArray();
// [ 9] PackedIntArray& operator=(const PackedIntArray& rhs);
// [ 2] void append(TYPE value);
// [12] void append(const PackedIntArray& srcArray);
// [12] void append(const PackedIntArray& srcArray, si, ne);
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
// [13] void insert(di, value);
// [24] PIACI insert(PIACI dst, value);
// [13] void insert(di, const PackedIntArray& srcArray);
// [13] void insert(di, const PackedIntArray& srcArray, si, ne);
// [ 2] void pop_back();
// [12] void push_back(TYPE value);
// [14] void remove(di);
// [14] void remove(di, ne);
// [25] PIACI remove(PIACI dstFirst, PIACI dstLast);
// [ 2] void removeAll();
// [15] void replace(di, value);
// [15] void replace(di, const PackedIntArray& srcArray, si, ne);
// [16] void reserveCapacity(numElements);
// [16] void reserveCapacity(numElements, maxValue);
// [16] void reserveCapacity(numElements, minValue, maxValue);
// [17] void resize(bsl::size_t numElements);
// [11] void swap(PackedIntArray& other);
// [ 4] TYPE operator[](bsl::size_t index) const;
// [ 4] bslma::Allocator *allocator() const;
// [19] TYPE back() const;
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
// [20] PackedIntArrayConstIterator begin() const;
// [ 4] int bytesPerElement() const;
// [ 4] bsl::size_t capacity() const;
// [20] PackedIntArrayConstIterator end() const;
// [19] TYPE front() const;
// [ 4] bool isEmpty() const;
// [ 6] bool isEqual(const PackedIntArray& other) const;
// [ 4] bsl::size_t length() const;
// [ 5] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
// [ 5] ostream& operator<<(ostream& stream, const PackedIntArray& array);
// [ 6] bool operator==(lhs, rhs);
// [ 6] bool operator!=(lhs, rhs);
// [11] void swap(PackedIntArray& a, PackedIntArray& b);
// [26] void hashAppend(HASHALG&, const PackedIntArray&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [27] USAGE EXAMPLE
// [ 3] Obj& gg(Obj *object, const char *spec);
// [ 3] UnsignedObj& gg(UnsignedObj *object, const char *spec);
// [ 3] int ggg(Obj *object, const char *spec);
// [ 3] int ggg(UnsignedObj *object, const char *spec);
// [ 8] PackedIntArray g(const char *spec);
// [23] CONCERN: iterators remain valid over modifications
// ----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlc::PackedIntArray<bsl::int64_t>                  Obj;
typedef bsl::int64_t                                        Element;
typedef bdlc::PackedIntArray<bsl::int64_t>::const_iterator  Iterator;

typedef bdlc::PackedIntArray<bsl::uint64_t>                 UnsignedObj;
typedef bsl::uint64_t                                       UnsignedElement;
typedef bdlc::PackedIntArray<bsl::uint64_t>::const_iterator UnsignedIterator;

const static bsl::int64_t k_INT8_MIN
                                    = bsl::numeric_limits<bsl::int8_t >::min();
const static bsl::int64_t k_INT8_MAX
                                    = bsl::numeric_limits<bsl::int8_t >::max();
const static bsl::int64_t k_INT16_MIN
                                    = bsl::numeric_limits<bsl::int16_t>::min();
const static bsl::int64_t k_INT16_MAX
                                    = bsl::numeric_limits<bsl::int16_t>::max();
const static bsl::int64_t k_INT32_MIN
                                    = bsl::numeric_limits<bsl::int32_t>::min();
const static bsl::int64_t k_INT32_MAX
                                    = bsl::numeric_limits<bsl::int32_t>::max();
const static bsl::int64_t k_INT64_MIN
                                    = bsl::numeric_limits<bsl::int64_t>::min();
const static bsl::int64_t k_INT64_MAX
                                    = bsl::numeric_limits<bsl::int64_t>::max();

const static bsl::uint64_t k_UINT8_MAX
                                   = bsl::numeric_limits<bsl::uint8_t >::max();
const static bsl::uint64_t k_UINT16_MAX
                                   = bsl::numeric_limits<bsl::uint16_t>::max();
const static bsl::uint64_t k_UINT32_MAX
                                   = bsl::numeric_limits<bsl::uint32_t>::max();
const static bsl::uint64_t k_UINT64_MAX
                                   = bsl::numeric_limits<bsl::uint64_t>::max();

typedef bslx::TestInStream  In;
typedef bslx::TestOutStream Out;

#define SERIALIZATION_VERSION 20140601

// ============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
// ----------------------------------------------------------------------------
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: 'Temperature Map'
///- - - - - - - - - - - - - - - -
// There exist many applications in which the range of 'int' data that a
// container will hold is not known at design time.  This means in order to
// build a robust component one must default to 'bsl::vector<int>', which for
// many applications is excessive in its usage of space.
//
// Suppose we are creating a map of temperatures for every city in the United
// States for every day.  This represents a large body of data, most of which
// is easily representable in a 'signed char', and in only rare situations is a
// 'short' required.
//
// To be able to represent all possible values for all areas and times,
// including extremes like Death Valley, a traditional implementation would
// require use of a 'vector<short>' for each day for each area.  This is
// excessive for all but the most extreme values, and therefore wasteful for
// this map as a whole.
//
// We can use 'bdlc::PackedIntArray' to efficiently store this data.
//
// First, we declare and define a 'my_Date' class.  This class is very similar
// to 'bdlt::Date', and therefore is elided for the sake of compactness.
//..
                                // =======
                                // my_Date
                                // =======
    class my_Date {
        // A (value-semantic) attribute class that provides a very simple date.
        signed char d_day;    // the day
        signed char d_month;  // the month
        int         d_year;   // the year

        // FRIENDS
        friend bool operator<(const my_Date&, const my_Date&);

      public:
        // CREATORS
        explicit my_Date(int         year  = 1,
                         signed char month = 1,
                         signed char day   = 1);
            // Create a 'my_Date' object having the optionally specified 'day',
            // 'month', and 'year'.  Each, if unspecified, will default to 1.
    };

    bool operator<(const my_Date& lhs, const my_Date& rhs);
        // Return 'true' if the specified 'lhs' represents an earlier date than
        // the specified 'rhs' object, and 'false' otherwise.

                            // -------
                            // my_Date
                            // -------
    // CREATORS
    inline
    my_Date::my_Date(int year, signed char month , signed char day)
    : d_day(day)
    , d_month(month)
    , d_year(year)
    {
    }

    bool operator<(const my_Date& lhs, const my_Date& rhs)
    {
        return 10000 * lhs.d_year + 100 * lhs.d_month + lhs.d_day <
               10000 * rhs.d_year + 100 * rhs.d_month + rhs.d_day;
    }
//..

// ============================================================================
//                GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
// ----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.
//
// LANGUAGE SPECIFICATION:
// -----------------------
//
// <SPEC> ::= <EMPTY>   | <LIST>       | <ULIST>
//
// <EMPTY> ::=
//
// <LIST> ::= <ITEM>    | <ITEM><LIST>
//
// <ITEM> ::= <ELEMENT> | 'x' | 'p'
//
// <ELEMENT> ::= 'z' | 'o' | 'O' | 'c' | 'C' | 's' | 'S' | 'i' | 'I' | 'l'
//             | 'L' | '^' | '!' | '@' | '$' | '(' | ')'
//
// <ULIST> ::= <UITEM>  | <UITEM><ULIST>
//
// <UITEM> ::= <UELEMENT> | 'x' | 'p'
//
// <UELEMENT> ::= 'z' | 'O' | 'C' | 'S' | 'I' | 'L' | 's' | 'i' | 'l'

int getValue(char specChar, bsl::int64_t *value, int verboseFlag);
    // Place into the specified 'value' the value corresponding to the
    // specified 'specChar' and display errors to 'cerr' if the specified
    // 'verboseFlag' is set.  Return 0 if operation successful, return non-zero
    // otherwise.

//..
///LEGEND
///=======================
// SPEC_CHAR  VALUE
// ---------  ------------
// z                     0
// o                    -1
// O                     1
// c            k_INT8_MIN
// C            k_INT8_MAX
// s           k_INT16_MIN
// S           k_INT16_MAX
// i           k_INT32_MIN
// I           k_INT32_MAX
// l           k_INT64_MIN
// L           k_INT64_MAX
// ^        k_INT8_MIN - 1
// !        k_INT8_MAX + 1
// @       k_INT16_MIN - 1
// $       k_INT16_MAX + 1
// (       k_INT32_MIN - 1
// )       k_INT32_MAX + 1
//..

int getValue(char specChar, bsl::int64_t *value, int verboseFlag)
{
    enum { SUCCESS_APPEND = -1, SUCCESS_REMOVEALL = -2, SUCCESS_POPBACK = -3 };
    switch(specChar) {
      case 'z': {
        *value = 0;
      } break;
      case 'o': {
        *value = -1;
      } break;
      case 'O': {
        *value = 1;
      } break;
      case 'c': {
        *value = k_INT8_MIN;
      } break;
      case 'C': {
        *value = k_INT8_MAX;
      } break;
      case 's': {
        *value = k_INT16_MIN;
      } break;
      case 'S': {
        *value = k_INT16_MAX;
      } break;
      case 'i': {
        *value = k_INT32_MIN;
      } break;
      case 'I': {
        *value = k_INT32_MAX;
      } break;
      case 'l': {
        *value = k_INT64_MIN;
      } break;
      case 'L': {
        *value = k_INT64_MAX;
      } break;
      case '^': {
        *value = k_INT8_MIN - 1;
      } break;
      case '!': {
        *value = k_INT8_MAX + 1;
      } break;
      case '@': {
        *value = k_INT16_MIN - 1;
      } break;
      case '$': {
        *value = k_INT16_MAX + 1;
      } break;
      case '(': {
        *value = k_INT32_MIN - 1;
      } break;
      case ')': {
        *value = k_INT32_MAX + 1;
      } break;
      case 'x': {
        return SUCCESS_REMOVEALL;                                     // RETURN
      } break;
      case 'p': {
        return SUCCESS_POPBACK;                                       // RETURN
      } break;
      default : {
        if (verboseFlag) {
            cerr << "\t\tERROR!" << endl;
            cerr << specChar << " not recognized." << endl;
        }
        return 1;                                                     // RETURN
      }
    }
    return SUCCESS_APPEND;
}

int getValue(char specChar, bsl::uint64_t *value, int verboseFlag);
    // Place into the specified 'value' the value corresponding to the
    // specified 'specChar' and display errors to 'cerr' if the specified
    // 'verboseFlag' is set.  Return 0 if operation successful, return non-zero
    // otherwise.

//..
///LEGEND
///=======================
// SPEC_CHAR  VALUE
// ---------  ------------
// z                     0
// O                     1
// C           k_UINT8_MAX
// S          k_UINT16_MAX
// I          k_UINT32_MAX
// L          k_UINT64_MAX
// s       k_UINT8_MAX + 1
// i      k_UINT16_MAX + 1
// l      k_UINT32_MAX + 1
//..

int getValue(char specChar, bsl::uint64_t *value, int verboseFlag)
{
    enum { SUCCESS_APPEND = -1, SUCCESS_REMOVEALL = -2, SUCCESS_POPBACK = -3 };
    switch(specChar) {
      case 'z': {
        *value = 0;
      } break;
      case 'O': {
        *value = 1;
      } break;
      case 'C': {
        *value = k_UINT8_MAX;
      } break;
      case 'S': {
        *value = k_UINT16_MAX;
      } break;
      case 'I': {
        *value = k_UINT32_MAX;
      } break;
      case 'L': {
        *value = k_UINT64_MAX;
      } break;
      case 's': {
        *value = k_UINT8_MAX + 1;
      } break;
      case 'i': {
        *value = k_UINT16_MAX + 1;
      } break;
      case 'l': {
        *value = k_UINT32_MAX + 1;
      } break;
      case 'x': {
        return SUCCESS_REMOVEALL;                                     // RETURN
      } break;
      case 'p': {
        return SUCCESS_POPBACK;                                       // RETURN
      } break;
      default : {
        if (verboseFlag) {
            cerr << "\t\tERROR!" << endl;
            cerr << specChar << " not recognized." << endl;
        }
        return 1;                                                     // RETURN
      }
    }
    return SUCCESS_APPEND;
}

template <class OBJ>
int ggg(OBJ *object, const char *spec, int verboseFlag = 1)
    // Configure the specified 'object' according to the specified 'spec',
    // using only the primary manipulator function 'append' and white-box
    // manipulators 'pop_back' and 'removeAll'.  Optionally specify a zero
    // 'verboseFlag' to suppress 'spec' syntax error messages.  Return the
    // index of the first invalid character, and a negative value otherwise.
    // Note that this function is used to implement 'gg' as well as allow for
    // verification of syntax error detection.
{
    enum { SUCCESS = -1 };

    typename OBJ::value_type v;

    for (int i = 0; spec[i]; ++i) {
        int rv = getValue(spec[i], &v, verboseFlag);
        if (rv > -1) {
            return i;                                                 // RETURN
        }
        else if (rv == -1) {
            object->append(v);
        }
        else if (rv == -2) {
            object->removeAll();
        }
        else {
            object->pop_back();
        }
    }

    return SUCCESS;
}

template <class OBJ>
OBJ& gg(OBJ *object, const char *spec)
    // Return, by reference, the specified 'object' with its value adjusted
    // according to the specified 'spec'.
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    switch (test) { case 0:
      case 27: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

// Then, we create our 'temperatureMap', which is a map of dates to a map of
// zip codes to a 'PackedIntArray' of temperatures.  Each 'PackedIntArray' has
// entries for each temperature from 12 A.M, to 11 P.M for each city in each
// zip code.  Notice that we use a 'PackedIntArray' to hold the data compactly.
//..
    bsl::map<my_Date, bsl::map<bsl::string, bdlc::PackedIntArray<int> > >
                                                                temperatureMap;
//..
// Next, we add data to the map (provided by the National Weather Service) for
// a normal case, and the extreme.
//..
    bdlc::PackedIntArray<int>& nyc
                           = temperatureMap[my_Date(2013, 9,  6)]["10023"];
    bdlc::PackedIntArray<int>& dValley
                           = temperatureMap[my_Date(1913, 7, 10)]["92328"];
    bdlc::PackedIntArray<int>& boston
                           = temperatureMap[my_Date(2013, 9,  6)]["02202"];

    int nycTemperatures[24]  = { 60,  58, 57,  56,  55,  54,  54,  55,
                                 56,  59, 61,  64,  66,  67,  69,  69,
                                 70,  70, 68,  67,  65,  63,  61,  60};

    int deathValleyTemps[24] = { 65,  55, 50,  47,  62,  75,  77,  89,
                                 91,  92, 95, 110, 113, 121, 134, 126,
                                113,  99, 96,  84,  79,  81,  73,  69};

    int bostonTemps[24]      = { 55,  53, 52,  51,  50,  49,  49,  50,
                                 51,  54, 56,  59,  61,  62,  64,  64,
                                 65,  65, 63,  62,  60,  58,  56,  55};
//..
// Then, since the size of the data set is known at design time, as well as
// extreme values for the areas, we can use the 'reserveCapacity()' method to
// give the container hints about the data to come.
//..
    nyc.reserveCapacity    (24, 54,  70);
    dValley.reserveCapacity(24, 47, 134);
    boston.reserveCapacity (24, 49,  65);
//..
// Now we add the data to the respective containers.
//..
    for (bsl::size_t i= 0; i < 24; ++i) {
        nyc.append(nycTemperatures[i]);
        dValley.append(deathValleyTemps[i]);
        boston.append(bostonTemps[i]);
    }
//..
// Finally, notice that in order to represent these values in a
// 'PackedIntArray', it required '24 * sizeof(signed char)' bytes (24 on most
// systems) of dynamic memory for 'nyc', which represents the normal case for
// this data.  A 'vector<short>' would require '24 * sizeof(short)' bytes (48
// on most systems) of dynamic memory to represent the same data.
//..
    ASSERT(static_cast<int>(sizeof(signed char)) == nyc.bytesPerElement());
    ASSERT(                                   24 == nyc.length());
//..
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING 'hashAppend'
        //   Ensure that equal arrays hash to the sam evalue.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', hash to the same value if they compare
        //:   equal.
        //:
        //: 2 The capacity and storage method of the objects does not affect
        //:   the result of hashing.
        //:
        //: 3 Non-modifiable objects can be hashed (i.e., objects providing
        //:   only non-modifiable access).
        //
        // Plan:
        //: 1 Specify a set of specifications for the 'gg' function that result
        //:   in an empty object but with different capacity and bytes per
        //:   element characteristics.
        //:
        //: 2 Specify a set of specifications for distinct object values.
        //:
        //: 3 For every item in the cross-product of these two sets, verify
        //:   that the hash value is the same when the two items are.  Hope
        //:   that they are different when the two items are.  (C-1..3)
        //
        // Testing:
        //   void hashAppend(HASHALG&, const PackedIntArray&);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING 'hashAppend'"
                          << "\n====================\n";

        typedef ::BloombergLP::bslh::Hash<> Hasher;
        typedef Hasher::result_type         HashType;
        Hasher                              hasher;

        if (verbose) {
            cout << "\nVerify hashing for the signed array.\n";
        }
        {
            static const struct {
                int         d_lineNum;
                const char *d_spec_p;
            } INIT[] = {
                // line spec
                //----  ----
                { L_,   ""          },
                { L_,   "Lp"        },
                { L_,   "LLLLLLx"   },
                { L_,   "LLLLLLxLp" },
            };
            const int NUM_INIT = static_cast<int>(sizeof INIT / sizeof *INIT);

            static const struct {
                int         d_lineNum;
                const char *d_spec_p;
            } DATA[] = {
                // line spec
                //----  ----
                { L_,   ""          },
                { L_,   "zoO"       },
                { L_,   "cC"        },
                { L_,   "sS"        },
                { L_,   "iI"        },
                { L_,   "zoOcC"     },
                { L_,   "zoOsS"     },
                { L_,   "zoOcCsSiI" },
                { L_,   "IiSsCcOoz" },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                for (int si = 0; si < NUM_INIT ; ++si) {
                    const int   LINE1 = DATA[ti].d_lineNum;
                    bsl::string SPEC1 = (bsl::string()
                                         + INIT[si].d_spec_p
                                         + DATA[ti].d_spec_p);

                    Obj mX;  const Obj& X = gg(&mX, SPEC1.c_str());

                    for (int tj = 0; tj < NUM_DATA; ++tj) {
                        for (int sj = 0; sj < NUM_INIT ; ++sj) {
                            const int   LINE2 = DATA[tj].d_lineNum;
                            bsl::string SPEC2 = (bsl::string()
                                                 + INIT[sj].d_spec_p
                                                 + DATA[tj].d_spec_p);

                            if (veryVerbose) { P_(SPEC1) P(SPEC2) }

                            Obj mY;  const Obj& Y = gg(&mY, SPEC2.c_str());

                            bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                            HashType hX = hasher(X);
                            HashType hY = hasher(Y);

                            LOOP2_ASSERT(LINE1,
                                         LINE2,
                                         (hX == hY) == (ti == tj));
                            LOOP2_ASSERT(LINE1,
                                         LINE2,
                                         defaultAllocator.numAllocations()
                                                               == allocations);
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\nVerify hashing for the unsigned array."
                          << endl;

        {
            static const struct {
                int         d_lineNum;
                const char *d_spec_p;
            } INIT[] = {
                // line spec
                //----  ----
                { L_,   ""          },
                { L_,   "Lp"        },
                { L_,   "LLLLLLx"   },
                { L_,   "LLLLLLxLp" },
            };
            const int NUM_INIT = static_cast<int>(sizeof INIT / sizeof *INIT);

            static const struct {
                int         d_lineNum;
                const char *d_spec_p;
            } DATA[] = {
                // line spec
                //----  ----
                { L_,   ""          },
                { L_,   "zO"        },
                { L_,   "C"         },
                { L_,   "sS"        },
                { L_,   "iI"        },
                { L_,   "zOC"       },
                { L_,   "zOsS"      },
                { L_,   "zOCsSiI"   },
                { L_,   "IiSsCOOz"  },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                for (int si = 0; si < NUM_INIT ; ++si) {
                    const int   LINE1 = DATA[ti].d_lineNum;
                    bsl::string SPEC1 = (bsl::string()
                                         + INIT[si].d_spec_p
                                         + DATA[ti].d_spec_p);

                    UnsignedObj        mX;
                    const UnsignedObj& X = gg(&mX, SPEC1.c_str());

                    for (int tj = 0; tj < NUM_DATA; ++tj) {
                        for (int sj = 0; sj < NUM_INIT ; ++sj) {
                            const int   LINE2 = DATA[tj].d_lineNum;
                            bsl::string SPEC2 = (bsl::string()
                                                 + INIT[sj].d_spec_p
                                                 + DATA[tj].d_spec_p);

                            if (veryVerbose) { P_(SPEC1) P(SPEC2) }

                            UnsignedObj        mY;
                            const UnsignedObj& Y = gg(&mY, SPEC2.c_str());

                            bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                            HashType hX = hasher(X);
                            HashType hY = hasher(Y);

                            LOOP2_ASSERT(LINE1,
                                         LINE2,
                                         (hX == hY) == (ti == tj));

                            LOOP2_ASSERT(LINE1,
                                         LINE2,
                                         defaultAllocator.numAllocations()
                                                               == allocations);
                        }
                    }
                }
            }
        }
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING 'remove(PIACI dstFirst, PIACI dstLast)'
        //   The 'remove' method operates as expected.
        //
        // Concerns:
        //: 1 The methods produce the expected value.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Specify a set of specifications for the 'gg' function that result
        //:   in an empty object but with different capacity and bytes per
        //:   element characteristics.
        //:
        //: 2 Use the table-based approach to specify a set of test vectors.
        //:
        //: 3 For every item in the cross-product of these two sets, verify
        //:   the result for the method (the set from 2 is applied to only
        //:   the initial object).  (C-1)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   PIACI remove(PIACI dstFirst, PIACI dstLast);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'remove(PIACI dstFirst, PIACI dstLast)'" << endl
                 << "===============================================" << endl;
        }

        static const struct {
            int         d_lineNum;
            const char *d_spec_p;
        } INIT[] = {
            // line spec
            //----  ----
            { L_,   ""              },
            { L_,   "Lp"            },
            { L_,   "LLLLLLLLLLx"   },
            { L_,   "LLLLLLLLLLxLp" },
        };
        const int NUM_INIT = static_cast<int>(sizeof INIT / sizeof *INIT);

        if (verbose) cout << "\nSub-array remove." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec_p;       // specification
                int         d_di;           // destination index
                int         d_ne;           // number of elements
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec      di  ne  exp
                //----  --------  --  --  ----------------------
                { L_,   "C",       0,  0, "C"                    },
                { L_,   "CS",      0,  0, "CS"                   },

                { L_,   "CSIL",    0,  1, "SIL"                  },
                { L_,   "CSIL",    1,  1, "CIL"                  },
                { L_,   "CSIL",    2,  1, "CSL"                  },
                { L_,   "CSIL",    3,  1, "CSI"                  },

                { L_,   "CSIL",    0,  2, "IL"                   },
                { L_,   "CSIL",    1,  2, "CL"                   },
                { L_,   "CSIL",    2,  2, "CS"                   },

                { L_,   "CSIL",    0,  3, "L"                    },
                { L_,   "CSIL",    1,  3, "C"                    },

                { L_,   "CSIL",    0,  4, ""                     },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const int         DI      = DATA[i].d_di;
                const int         NE      = DATA[i].d_ne;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj mEXP;  gg(&mEXP, EXPSPEC);  const Obj& EXP = mEXP;

                UnsignedObj        mUEXP;  gg(&mUEXP, EXPSPEC);
                const UnsignedObj& UEXP = mUEXP;

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC = (bsl::string()
                                        + INIT[j].d_spec_p
                                        + DATA[i].d_spec_p);

                    if (veryVerbose) { P(SPEC) }

                    Obj        mX;  gg(&mX, SPEC.c_str());
                    const Obj& X = mX;
                    Iterator   F = X.begin() + DI;
                    Iterator   L = F + NE;
                    LOOP_ASSERT(LINE, F == mX.remove(F, L));
                    LOOP_ASSERT(LINE, X == EXP);

                    UnsignedObj        mUX;  gg(&mUX, SPEC.c_str());
                    const UnsignedObj& UX = mUX;
                    UnsignedIterator   UF = UX.begin() + DI;
                    UnsignedIterator   UL = UF + NE;
                    LOOP_ASSERT(LINE, UF == mUX.remove(UF, UL));
                    LOOP_ASSERT(LINE, UX == UEXP);
                }
            }
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj         mX;   gg(&mX,  "CSI");
            UnsignedObj mUX;  gg(&mUX, "CSI");

            Obj        mT;
            const Obj& T = mT;

            UnsignedObj        mUT;
            const UnsignedObj& UT = mUT;

            mT = mX;  ASSERT_SAFE_PASS(mT.remove(T.begin(), T.end()));
            mT = mX;  ASSERT_SAFE_FAIL(mT.remove(T.end(),   T.begin()));

            mUT = mUX;  ASSERT_SAFE_PASS(mUT.remove(UT.begin(), UT.end()));
            mUT = mUX;  ASSERT_SAFE_FAIL(mUT.remove(UT.end(),   UT.begin()));
        }
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING 'insert(PIACI dst, value)'
        //   The 'insert' method operates as expected.
        //
        // Concerns:
        //: 1 The method is exception neutral with respect to memory
        //:   allocation.
        //:
        //: 2 The method produces the expected value.
        //:
        //: 3 When there is initially sufficient storage in the result variable
        //:   to store the result, the methods produce the expected value.
        //
        // Plan:
        //: 1 Wrap all tests with the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
        //:   macros to ensure exception neutrality with respect to memory
        //:   allocation.  (C-1)
        //:
        //: 2 Specify a set of specifications for the 'gg' function that result
        //:   in an empty object but with different capacity and bytes per
        //:   element characteristics.
        //:
        //: 3 Use the table-based approach to specify a set of test vectors.
        //:
        //: 4 For every item in the cross-product of these two sets, verify
        //:   the result for the method (the set from 2 is applied to only
        //:   the initial object).  (C-2..3)
        //
        // Testing:
        //   PIACI insert(PIACI dst, value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'insert(PIACI dst, value)'" << endl
                          << "==================================" << endl;

        static const struct {
            int         d_lineNum;
            const char *d_spec_p;
        } INIT[] = {
            // line spec
            //----  ----
            { L_,   ""              },
            { L_,   "Lp"            },
            { L_,   "LLLLLLLLLLx"   },
            { L_,   "LLLLLLLLLLxLp" },
        };
        const int NUM_INIT = static_cast<int>(sizeof INIT / sizeof *INIT);

        if (verbose) cout << "\nValue insert with iterator." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec1_p;      // specification 1
                int         d_di;           // destination index
                const char  d_spec2_p;      // specification 2
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec1     di  spec2     exp
                //----  --------  --  --------  ----------------------------
                { L_,   "",        0, 'C',      "C"                          },

                { L_,   "C",       0, 'S',      "SC"                         },
                { L_,   "C",       1, 'S',      "CS"                         },

                { L_,   "CS",      0, 'I',      "ICS"                        },
                { L_,   "CS",      1, 'I',      "CIS"                        },
                { L_,   "CS",      2, 'I',      "CSI"                        },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const int         DI      = DATA[i].d_di;
                const char        SPEC2   = DATA[i].d_spec2_p;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Element    y;     getValue(SPEC2, &y, 0);
                Obj        mEXP;  gg(&mEXP, EXPSPEC);
                const Obj& EXP =  mEXP;

                UnsignedElement    uy;     getValue(SPEC2, &uy, 0);
                UnsignedObj        mUEXP;  gg(&mUEXP, EXPSPEC);
                const UnsignedObj& UEXP =  mUEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC1 = (bsl::string()
                                         + INIT[j].d_spec_p
                                         + DATA[i].d_spec1_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC1.c_str());
                        const Obj& X = mX;
                        Iterator   DST = X.begin() + DI;
                        LOOP_ASSERT(LINE, DST == mX.insert(DST, y));
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        UnsignedObj        mUX(&sa);  gg(&mUX, SPEC1.c_str());
                        const UnsignedObj& UX = mUX;
                        UnsignedIterator   DST = UX.begin() + DI;
                        LOOP_ASSERT(LINE, DST == mUX.insert(DST, uy));
                        LOOP_ASSERT(LINE, UX == UEXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // CONST ITERATOR REMAINS VALID OVER MODIFICATIONS
        //   Ensure the methods work correctly.
        //
        // Concerns:
        //: 1 The iterator remains valid as the data in the underlying array
        //:   is modified; including modifications that modify the size of the
        //:   array.
        //
        // Plan:
        //: 1 Use the table-based approach to specify a set of test vectors
        //:   'S'.
        //:
        //: 2 For every pair of items 'si' and 'sj' chosen from 'S', use the
        //:   generator function to produce an array from 'si', exhaustively
        //:   create an iterator at each position in the array, append an
        //:   array generated from 'sj', and verify the iterators remain
        //:   valid.  Then 'pop_back' as many elements as possible and ensure
        //:   the iterators remain valid as long as possible.
        //:
        //: 3 For every pair of items 'si' and 'sj' chosen from 'S', use the
        //:   generator function to produce an array from 'si', exhaustively
        //:   create an iterator at each position in the array, assign the
        //:   value of this array to an array generated from 'sj', and verify
        //:   the iterators remain valid if the new length of the array is
        //:   sufficient for the iterator to remain valid.  (C-1)
        //
        // Testing:
        //   CONCERN: iterators remain valid over modifications
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONST ITERATOR REMAINS VALID OVER MODIFICATIONS"
                          << endl
                          << "==============================================="
                          << endl;
        {
            static const struct {
                int          d_lineNum;          // source line number
                const char  *d_spec_p;           // specification string
                bsl::size_t  d_length;           // expected length
            } DATA[] = {
                //line  spec      len
                //----  --------  ---
                { L_,   "z",        1 },
                { L_,   "o",        1 },
                { L_,   "O",        1 },
                { L_,   "c",        1 },
                { L_,   "C",        1 },
                { L_,   "s",        1 },
                { L_,   "S",        1 },
                { L_,   "i",        1 },
                { L_,   "I",        1 },
                { L_,   "l",        1 },
                { L_,   "L",        1 },
                { L_,   "^",        1 },
                { L_,   "!",        1 },
                { L_,   "@",        1 },
                { L_,   "$",        1 },
                { L_,   "(",        1 },
                { L_,   ")",        1 },
                { L_,   "Lx",       0 },
                { L_,   "LxS",      1 },
                { L_,   "LxSx",     0 },
                { L_,   "CSI",      3 },
                { L_,   "CSIx",     0 },
                { L_,   "zOC",      3 },
                { L_,   "zSOsC",    5 },
                { L_,   "zSOIsC",   6 },
                { L_,   "LzSOIsC",  7 }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         ILINE = DATA[ti].d_lineNum;
                const char *const ISPEC = DATA[ti].d_spec_p;
                const bsl::size_t ILEN  = DATA[ti].d_length;

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int         JLINE = DATA[tj].d_lineNum;
                    const char *const JSPEC = DATA[tj].d_spec_p;
                    const bsl::size_t JLEN  = DATA[tj].d_length;

                    for (unsigned int index = 0; index < ILEN; ++index) {
                        Obj mX;  const Obj& X = gg(&mX, ISPEC);

                        // Create an iterator at a valid position.

                        Iterator iter(X.begin());
                        iter += index;
                        ASSERTV(ILINE, JLINE, *iter == X[index]);

                        Obj mY;
                        gg(&mY, JSPEC);

                        // Verify 'append' does not invalidate the iterator.

                        mX.append(mY);
                        ASSERTV(ILINE, JLINE, *iter == X[index]);

                        // Verify reducing the size does not invalidate the
                        // iterator.

                        mX.pop_back();
                        while (mX.length() > index) {
                            ASSERTV(ILINE, JLINE, *iter == X[index]);
                            mX.pop_back();
                        }
                    }

                    for (unsigned int index = 0;
                         index < ILEN && index < JLEN;
                         ++index) {
                        Obj mX;  const Obj& X = gg(&mX, ISPEC);

                        // Create an iterator at a valid position.

                        Iterator iter(X.begin());
                        iter += index;
                        ASSERTV(ILINE, JLINE, *iter == X[index]);

                        Obj mY;  const Obj& Y = gg(&mY, JSPEC);

                        // Verify assignment does not invalidate the iterator.

                        mX = Y;
                        ASSERTV(ILINE, JLINE, *iter == X[index]);
                    }
                }
            }
        }

        {
            static const struct {
                int          d_lineNum;          // source line number
                const char  *d_spec_p;           // specification string
                bsl::size_t  d_length;           // expected length
            } DATA[] = {
                //line  spec      len
                //----  --------  ---
                { L_,   "z",        1 },
                { L_,   "O",        1 },
                { L_,   "C",        1 },
                { L_,   "S",        1 },
                { L_,   "I",        1 },
                { L_,   "L",        1 },
                { L_,   "s",        1 },
                { L_,   "i",        1 },
                { L_,   "l",        1 },
                { L_,   "Lx",       0 },
                { L_,   "LxS",      1 },
                { L_,   "LxSx",     0 },
                { L_,   "CSI",      3 },
                { L_,   "CSIx",     0 },
                { L_,   "zOC",      3 },
                { L_,   "zSOsC",    5 },
                { L_,   "zSOIsC",   6 },
                { L_,   "LzSOIsC",  7 }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         ILINE = DATA[ti].d_lineNum;
                const char *const ISPEC = DATA[ti].d_spec_p;
                const bsl::size_t ILEN  = DATA[ti].d_length;

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int         JLINE = DATA[tj].d_lineNum;
                    const char *const JSPEC = DATA[tj].d_spec_p;
                    const bsl::size_t JLEN  = DATA[tj].d_length;

                    for (unsigned int index = 0; index < ILEN; ++index) {
                        UnsignedObj mX;  const UnsignedObj& X = gg(&mX, ISPEC);

                        // Create an iterator at a valid position.

                        UnsignedIterator iter(X.begin());
                        iter += index;
                        ASSERTV(ILINE, JLINE, *iter == X[index]);

                        UnsignedObj mY;
                        gg(&mY, JSPEC);

                        // Verify 'append' does not invalidate the iterator.

                        mX.append(mY);
                        ASSERTV(ILINE, JLINE, *iter == X[index]);

                        // Verify reducing the size does not invalidate the
                        // iterator.

                        mX.pop_back();
                        while (mX.length() > index) {
                            ASSERTV(ILINE, JLINE, *iter == X[index]);
                            mX.pop_back();
                        }
                    }

                    for (unsigned int index = 0;
                         index < ILEN && index < JLEN;
                         ++index) {
                        UnsignedObj mX;  const UnsignedObj& X = gg(&mX, ISPEC);

                        // Create an iterator at a valid position.

                        UnsignedIterator iter(X.begin());
                        iter += index;
                        ASSERTV(ILINE, JLINE, *iter == X[index]);

                        UnsignedObj mY;  const UnsignedObj& Y = gg(&mY, JSPEC);

                        // Verify assignment does not invalidate the iterator.

                        mX = Y;
                        ASSERTV(ILINE, JLINE, *iter == X[index]);
                    }
                }
            }
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // CONST ITERATOR OTHER METHODS
        //   Ensure the methods work correctly.
        //
        // Concerns:
        //: 1 The methods work as expected; including with negative deltas.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the generator function to produce objects of arbitrary state.
        //:   Exhaustively verify the methods with all valid starting
        //:   positions for an iterator and all valid deltas, positive and
        //:   negative, with the array's 'operator[]' as an oracle.  (C-1)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   PIACI& PIACI::operator+=(bsl::ptrdiff_t offset);
        //   PIACI& PIACI::operator-=(bsl::ptrdiff_t offset);
        //   PIACI PIACI::operator+(bsl::ptrdiff_t offset);
        //   PIACI PIACI::operator-(bsl::ptrdiff_t offset);
        //   PIACI PIACI::operator++(PIACI& iter, int);
        //   PIACI PIACI::operator--(PIACI& iter, int);
        //   bsl::ptrdiff_t operator-(const PIACI& lhs, const PIACI& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONST ITERATOR OTHER METHODS" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting other methods."
                          << endl;
        {
            static const struct {
                int          d_lineNum;          // source line number
                const char  *d_spec_p;           // specification string
                bsl::size_t  d_length;           // expected length
            } DATA[] = {
                //line  spec      len
                //----  --------  ---
                { L_,   "",         0 },
                { L_,   "z",        1 },
                { L_,   "o",        1 },
                { L_,   "O",        1 },
                { L_,   "c",        1 },
                { L_,   "C",        1 },
                { L_,   "s",        1 },
                { L_,   "S",        1 },
                { L_,   "i",        1 },
                { L_,   "I",        1 },
                { L_,   "l",        1 },
                { L_,   "L",        1 },
                { L_,   "^",        1 },
                { L_,   "!",        1 },
                { L_,   "@",        1 },
                { L_,   "$",        1 },
                { L_,   "(",        1 },
                { L_,   ")",        1 },
                { L_,   "Lx",       0 },
                { L_,   "LxS",      1 },
                { L_,   "LxSx",     0 },
                { L_,   "CSI",      3 },
                { L_,   "CSIx",     0 },
                { L_,   "zOC",      3 },
                { L_,   "zSOsC",    5 },
                { L_,   "zSOIsC",   6 },
                { L_,   "LzSOIsC",  7 }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const bsl::size_t LEN  = DATA[ti].d_length;

                Obj mX;  const Obj& X = gg(&mX, SPEC);   // original spec

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                LOOP_ASSERT(LINE, LEN == X.length());

                Iterator iter(X.begin());

                for (int i = 0; i < static_cast<int>(LEN); ++i) {
                    for (int j = 0; j < static_cast<int>(LEN); ++j) {
                        if (i + j < static_cast<int>(LEN)) {
                            Iterator iter2 = iter + j;
                            LOOP2_ASSERT(i, j, *iter2 == X[i + j]);

                            Iterator iter3 = iter;
                            iter3 += j;
                            LOOP2_ASSERT(i, j, *iter3 == X[i + j]);

                            LOOP2_ASSERT(i, j, iter3 - iter ==  j);
                            LOOP2_ASSERT(i, j, iter - iter3 == -j);

                            Iterator iter4 = iter - (-j);
                            LOOP2_ASSERT(i, j, *iter4 == X[i + j]);

                            Iterator iter5 = iter;
                            iter5 -= (-j);
                            LOOP2_ASSERT(i, j, *iter5 == X[i + j]);
                        }
                        if (i - j >= 0) {
                            Iterator iter2 = iter - j;
                            LOOP2_ASSERT(i, j, *iter2 == X[i - j]);

                            Iterator iter3 = iter;
                            iter3 -= j;
                            LOOP2_ASSERT(i, j, *iter3 == X[i - j]);

                            LOOP2_ASSERT(i, j, iter3 - iter == -j);
                            LOOP2_ASSERT(i, j, iter - iter3 ==  j);

                            Iterator iter4 = iter + (-j);
                            LOOP2_ASSERT(i, j, *iter4 == X[i - j]);

                            Iterator iter5 = iter;
                            iter5 += (-j);
                            LOOP2_ASSERT(i, j, *iter5 == X[i - j]);
                        }
                    }

                    // Verify 'operator++(int)' and 'operator--(int)'.

                    Iterator iterOrig(iter);

                    Iterator iter2(iter++);
                    ASSERT(iter2 == iterOrig);

                    iter2++;
                    Iterator iter3(iter2--);
                    ASSERT(iter2 == iterOrig);
                    ASSERT(iter3 == iter);
                }

                ASSERT(iter == X.end());

                LOOP_ASSERT(LINE,
                            defaultAllocator.numAllocations() == allocations);
            }
        }
        {
            static const struct {
                int              d_lineNum;          // source line number
                const char      *d_spec_p;           // specification string
                bsl::size_t      d_length;           // expected length
            } DATA[] = {
                //line  spec      len
                //----  --------  ---
                { L_,   "",         0 },
                { L_,   "z",        1 },
                { L_,   "O",        1 },
                { L_,   "C",        1 },
                { L_,   "S",        1 },
                { L_,   "I",        1 },
                { L_,   "L",        1 },
                { L_,   "s",        1 },
                { L_,   "i",        1 },
                { L_,   "l",        1 },
                { L_,   "Lx",       0 },
                { L_,   "LxS",      1 },
                { L_,   "LxSx",     0 },
                { L_,   "CSI",      3 },
                { L_,   "CSIx",     0 },
                { L_,   "zOC",      3 },
                { L_,   "zSOsC",    5 },
                { L_,   "zSOIsC",   6 },
                { L_,   "LzSOIsC",  7 }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const bsl::size_t LEN  = DATA[ti].d_length;

                UnsignedObj        mX;
                const UnsignedObj& X = gg(&mX, SPEC);   // original spec

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                LOOP_ASSERT(LINE, LEN == X.length());

                UnsignedIterator iter(X.begin());

                for (int i = 0; i < static_cast<int>(LEN); ++i) {
                    for (int j = 0; j < static_cast<int>(LEN); ++j) {
                        if (i + j < static_cast<int>(LEN)) {
                            UnsignedIterator iter2 = iter + j;
                            LOOP2_ASSERT(i, j, *iter2 == X[i + j]);

                            UnsignedIterator iter3 = iter;
                            iter3 += j;
                            LOOP2_ASSERT(i, j, *iter3 == X[i + j]);

                            LOOP2_ASSERT(i, j, iter3 - iter ==  j);
                            LOOP2_ASSERT(i, j, iter - iter3 == -j);

                            UnsignedIterator iter4 = iter - (-j);
                            LOOP2_ASSERT(i, j, *iter4 == X[i + j]);

                            UnsignedIterator iter5 = iter;
                            iter5 -= (-j);
                            LOOP2_ASSERT(i, j, *iter5 == X[i + j]);
                        }
                        if (i - j >= 0) {
                            UnsignedIterator iter2 = iter - j;
                            LOOP2_ASSERT(i, j, *iter2 == X[i - j]);

                            UnsignedIterator iter3 = iter;
                            iter3 -= j;
                            LOOP2_ASSERT(i, j, *iter3 == X[i - j]);

                            LOOP2_ASSERT(i, j, iter3 - iter == -j);
                            LOOP2_ASSERT(i, j, iter - iter3 ==  j);

                            UnsignedIterator iter4 = iter + (-j);
                            LOOP2_ASSERT(i, j, *iter4 == X[i - j]);

                            UnsignedIterator iter5 = iter;
                            iter5 += (-j);
                            LOOP2_ASSERT(i, j, *iter5 == X[i - j]);
                        }
                    }

                    // Verify 'operator++(int)' and 'operator--(int)'.

                    UnsignedIterator iterOrig(iter);

                    UnsignedIterator iter2(iter++);
                    ASSERT(iter2 == iterOrig);

                    iter2++;
                    UnsignedIterator iter3(iter2--);
                    ASSERT(iter2 == iterOrig);
                    ASSERT(iter3 == iter);
                }

                ASSERT(iter == X.end());

                LOOP_ASSERT(LINE,
                            defaultAllocator.numAllocations() == allocations);
            }
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj mX;  const Obj& X = mX;
            mX.append(k_INT8_MAX);

            Obj mY;  const Obj& Y = mY;
            mY.append(k_INT8_MAX);

            Iterator iter;

            ASSERT_SAFE_FAIL(iter++);
            ASSERT_SAFE_FAIL(iter--);
            ASSERT_SAFE_FAIL(iter += 1);
            ASSERT_SAFE_FAIL(iter -= 1);
            ASSERT_SAFE_FAIL(iter + 1);
            ASSERT_SAFE_FAIL(iter - 1);

            iter = X.begin();
            ASSERT_SAFE_PASS(iter += 1);
            ASSERT_SAFE_FAIL(iter += 1);

            iter = X.begin();
            ASSERT_SAFE_FAIL(iter -= 1);

            iter = X.end();
            ASSERT_SAFE_PASS(iter -= 1);
            ASSERT_SAFE_FAIL(iter -= 1);

            iter = X.end();
            ASSERT_SAFE_FAIL(iter += 1);

            iter = X.begin();
            ASSERT_SAFE_PASS(iter++);
            ASSERT_SAFE_PASS(iter--);
            ASSERT_SAFE_FAIL(iter--);

            iter = X.end();
            ASSERT_SAFE_PASS(iter--);
            ASSERT_SAFE_PASS(iter++);
            ASSERT_SAFE_FAIL(iter++);

            iter = X.begin();
            ASSERT_SAFE_FAIL(iter - 1);
            ASSERT_SAFE_PASS(iter + 1);
            ASSERT_SAFE_FAIL(iter + 2);

            iter = X.end();
            ASSERT_SAFE_FAIL(iter + 1);
            ASSERT_SAFE_PASS(iter - 1);
            ASSERT_SAFE_FAIL(iter - 2);

            ASSERT_SAFE_PASS(X.end() - X.begin());
            ASSERT_SAFE_PASS(X.begin() - X.end());
            ASSERT_SAFE_FAIL(X.begin() - Y.end());

            mX.append(k_INT8_MAX);

            ASSERT_SAFE_PASS(X.end() - X.begin());
            ASSERT_SAFE_PASS(X.begin() - X.end());
            ASSERT_SAFE_FAIL(X.begin() - Y.end());

            (void)Y;
        }
        {
            bsls::AssertTestHandlerGuard hG;

            UnsignedObj mX;  const UnsignedObj& X = mX;
            mX.append(k_INT8_MAX);

            UnsignedObj mY;  const UnsignedObj& Y = mY;
            mY.append(k_INT8_MAX);

            UnsignedIterator iter;

            ASSERT_SAFE_FAIL(iter++);
            ASSERT_SAFE_FAIL(iter--);
            ASSERT_SAFE_FAIL(iter += 1);
            ASSERT_SAFE_FAIL(iter -= 1);
            ASSERT_SAFE_FAIL(iter + 1);
            ASSERT_SAFE_FAIL(iter - 1);

            iter = X.begin();
            ASSERT_SAFE_PASS(iter += 1);
            ASSERT_SAFE_FAIL(iter += 1);

            iter = X.begin();
            ASSERT_SAFE_FAIL(iter -= 1);

            iter = X.end();
            ASSERT_SAFE_PASS(iter -= 1);
            ASSERT_SAFE_FAIL(iter -= 1);

            iter = X.end();
            ASSERT_SAFE_FAIL(iter += 1);

            iter = X.begin();
            ASSERT_SAFE_PASS(iter++);
            ASSERT_SAFE_PASS(iter--);
            ASSERT_SAFE_FAIL(iter--);

            iter = X.end();
            ASSERT_SAFE_PASS(iter--);
            ASSERT_SAFE_PASS(iter++);
            ASSERT_SAFE_FAIL(iter++);

            iter = X.begin();
            ASSERT_SAFE_FAIL(iter - 1);
            ASSERT_SAFE_PASS(iter + 1);
            ASSERT_SAFE_FAIL(iter + 2);

            iter = X.end();
            ASSERT_SAFE_FAIL(iter + 1);
            ASSERT_SAFE_PASS(iter - 1);
            ASSERT_SAFE_FAIL(iter - 2);

            ASSERT_SAFE_PASS(X.end() - X.begin());
            ASSERT_SAFE_PASS(X.begin() - X.end());
            ASSERT_SAFE_FAIL(X.begin() - Y.end());

            mX.append(k_INT8_MAX);

            ASSERT_SAFE_PASS(X.end() - X.begin());
            ASSERT_SAFE_PASS(X.begin() - X.end());
            ASSERT_SAFE_FAIL(X.begin() - Y.end());

            (void)Y;
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // CONST ITERATOR RELATIONAL OPERATORS
        //   Ensure the relational operators work as expected.
        //
        // Concerns:
        //: 1 The methods work as expected.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Construct a sorted array and verify the results of an operator by
        //:   comparison with the result of the operator on the values at the
        //:   iterators.  (C-1)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   bool operator<(const PIACI& lhs, const PIACI& rhs);
        //   bool operator<=(const PIACI& lhs, const PIACI& rhs);
        //   bool operator>(const PIACI& lhs, const PIACI& rhs);
        //   bool operator>=(const PIACI& lhs, const PIACI& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONST ITERATOR RELATIONAL OPERATORS" << endl
                          << "===================================" << endl;

        if (verbose) cout << "\nTesting relational operators."
                          << endl;
        {
            Obj mX;  const Obj& X = mX;
            mX.append(0);  mX.append(1);  mX.append(2);
            mX.append(3);  mX.append(4);  mX.append(5);
            mX.append(6);  mX.append(7);  mX.append(8);

            for (Iterator iter1 = X.begin(); iter1 != X.end(); ++iter1) {
                for (Iterator iter2 = X.begin(); iter2 != X.end(); ++iter2) {
                    if (veryVerbose) { P_(*iter1) P(*iter2) }

                    ASSERT((iter1 <  iter2) == (*iter1 <  *iter2));
                    ASSERT((iter1 <= iter2) == (*iter1 <= *iter2));
                    ASSERT((iter1 >  iter2) == (*iter1 >  *iter2));
                    ASSERT((iter1 >= iter2) == (*iter1 >= *iter2));
                }
            }
        }
        {
            UnsignedObj mX;  const UnsignedObj& X = mX;
            mX.append(0);  mX.append(1);  mX.append(2);
            mX.append(3);  mX.append(4);  mX.append(5);
            mX.append(6);  mX.append(7);  mX.append(8);

            for (UnsignedIterator iter1 = X.begin();
                 iter1 != X.end();
                 ++iter1) {
                for (UnsignedIterator iter2 = X.begin();
                     iter2 != X.end();
                     ++iter2) {
                    if (veryVerbose) { P_(*iter1) P(*iter2) }

                    ASSERT((iter1 <  iter2) == (*iter1 <  *iter2));
                    ASSERT((iter1 <= iter2) == (*iter1 <= *iter2));
                    ASSERT((iter1 >  iter2) == (*iter1 >  *iter2));
                    ASSERT((iter1 >= iter2) == (*iter1 >= *iter2));
                }
            }
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj mX;  const Obj& X = mX;
            Obj mY;  const Obj& Y = mY;

            ASSERT_SAFE_PASS((void)(X.begin() <  X.end()));
            ASSERT_SAFE_PASS((void)(X.begin() <  X.end()));
            ASSERT_SAFE_PASS((void)(X.begin() <= X.end()));
            ASSERT_SAFE_PASS((void)(X.begin() >  X.end()));
            ASSERT_SAFE_PASS((void)(X.begin() >= X.end()));
            ASSERT_SAFE_FAIL((void)(X.begin() <  Y.end()));
            ASSERT_SAFE_FAIL((void)(X.begin() <= Y.end()));
            ASSERT_SAFE_FAIL((void)(X.begin() >  Y.end()));
            ASSERT_SAFE_FAIL((void)(X.begin() >= Y.end()));

            (void)X;
            (void)Y;
        }
        {
            bsls::AssertTestHandlerGuard hG;

            UnsignedObj mX;  const UnsignedObj& X = mX;
            UnsignedObj mY;  const UnsignedObj& Y = mY;

            ASSERT_SAFE_PASS((void)(X.begin() <  X.end()));
            ASSERT_SAFE_PASS((void)(X.begin() <= X.end()));
            ASSERT_SAFE_PASS((void)(X.begin() >  X.end()));
            ASSERT_SAFE_PASS((void)(X.begin() >= X.end()));
            ASSERT_SAFE_FAIL((void)(X.begin() <  Y.end()));
            ASSERT_SAFE_FAIL((void)(X.begin() <= Y.end()));
            ASSERT_SAFE_FAIL((void)(X.begin() >  Y.end()));
            ASSERT_SAFE_FAIL((void)(X.begin() >= Y.end()));

            (void)X;
            (void)Y;
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // CONST ITERATOR BASIC METHODS
        //   Ensure we can create a 'const' iterator, inspect the value at
        //   the iterator, and destroy the iterator.
        //
        // Concerns:
        //: 1 The equality, assignment, and copy construct methods work as
        //:   expected.
        //:
        //: 2 The other basic methods work correctly, including the use of
        //:   negative offsets in 'operator[]'.
        //:
        //: 3 The destructor functions properly.
        //:
        //: 4 The iterator returned by 'begin()' is valid and constant
        //:   regardless of the length of the array, and is equal to the
        //:   iterator returned by 'end()' if the array is empty.
        //:
        //: 5 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Construct arrays and verify the results of the equality,
        //:   assignment, and copy construct methods.  (C-1)
        //:
        //: 2 Use the generator function to produce objects of arbitrary state.
        //:   Verify the basic methods - excluding 'operator[]' - against the
        //:   expected values.  Exhaustively verify 'operator[]' with all valid
        //:   starting positions for an iterator and all valid offsets,
        //:   positive and negative, with the array's 'operator[]' as an
        //:   oracle.  (C-2)
        //:
        //: 3 Allow the iterators to go out-of-scope; there is nothing to
        //:   explicitly verify.  (C-3)
        //:
        //: 4 Create an empty array and verify 'begin() == end()'.  Use the
        //:   generator function to produce objects of arbitrary state and
        //:   assign the empty array to this array.  Verify the iterator
        //:   returned by 'begin()' on this array has the same value as the
        //:   one obtained while the array was empty.  (C-4)
        //:
        //: 5 Verify defensive checks are triggered for invalid values.  (C-5)
        //
        // Testing:
        //   PackedIntArrayConstIterator begin() const;
        //   PackedIntArrayConstIterator end() const;
        //   PackedIntArrayConstIterator();
        //   PackedIntArrayConstIterator(const PIACI& original);
        //   ~PackedIntArrayConstIterator();
        //   PIACI& operator=(const PIACI& rhs);
        //   PIACI& PIACI::operator++();
        //   PIACI& PIACI::operator--();
        //   TYPE PIACI::operator*() const;
        //   TYPE PIACI::operator->() const;
        //   TYPE PIACI::operator[](bsl::ptrdiff_t offset) const;
        //   bool operator==(const PIACI& lhs, const PIAI& rhs);
        //   bool operator!=(const PIACI& lhs, const PIAI& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONST ITERATOR BASIC METHODS" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting equality operators."
                          << endl;
        {
            Obj mX;  const Obj& X = mX;
            mX.append(k_INT8_MAX);

            Obj mY;  const Obj& Y = mY;
            mY.append(k_INT8_MAX);

            Iterator iter1 = X.begin();
            Iterator iter2 = X.end();
            Iterator iter3 = Y.begin();
            Iterator iter4 = Y.end();

            ASSERT(iter1 == iter1);
            ASSERT(iter1 != iter2);
            ASSERT(iter1 != iter3);
            ASSERT(iter1 != iter4);

            ASSERT(iter2 != iter1);
            ASSERT(iter2 == iter2);
            ASSERT(iter2 != iter3);
            ASSERT(iter2 != iter4);

            ASSERT(iter3 != iter1);
            ASSERT(iter3 != iter2);
            ASSERT(iter3 == iter3);
            ASSERT(iter3 != iter4);

            ASSERT(iter4 != iter1);
            ASSERT(iter4 != iter2);
            ASSERT(iter4 != iter3);
            ASSERT(iter4 == iter4);

            mX.append(k_INT8_MAX);
            iter2 = mX.end();
            ASSERT(iter1 != iter2);
            ++iter1;
            ASSERT(iter1 != iter2);
            --iter2;
            ASSERT(iter1 == iter2);
        }
        {
            UnsignedObj mX;  const UnsignedObj& X = mX;
            mX.append(k_INT8_MAX);

            UnsignedObj mY;  const UnsignedObj& Y = mY;
            mY.append(k_INT8_MAX);

            UnsignedIterator iter1 = X.begin();
            UnsignedIterator iter2 = X.end();
            UnsignedIterator iter3 = Y.begin();
            UnsignedIterator iter4 = Y.end();

            ASSERT(iter1 == iter1);
            ASSERT(iter1 != iter2);
            ASSERT(iter1 != iter3);
            ASSERT(iter1 != iter4);

            ASSERT(iter2 != iter1);
            ASSERT(iter2 == iter2);
            ASSERT(iter2 != iter3);
            ASSERT(iter2 != iter4);

            ASSERT(iter3 != iter1);
            ASSERT(iter3 != iter2);
            ASSERT(iter3 == iter3);
            ASSERT(iter3 != iter4);

            ASSERT(iter4 != iter1);
            ASSERT(iter4 != iter2);
            ASSERT(iter4 != iter3);
            ASSERT(iter4 == iter4);

            mX.append(k_INT8_MAX);
            iter2 = mX.end();
            ASSERT(iter1 != iter2);
            ++iter1;
            ASSERT(iter1 != iter2);
            --iter2;
            ASSERT(iter1 == iter2);
        }


        if (verbose) cout << "\nTesting assignment and copy construct."
                          << endl;
        {
            Obj mX;  const Obj& X = mX;
            mX.append(k_INT8_MAX);

            Iterator iter1 = X.begin();
            Iterator iter2 = X.end();
            ASSERT(iter1 != iter2);

            {
                Iterator iter;

                ASSERT(iter != iter1);
                ASSERT(iter != iter2);

                iter = iter1;

                ASSERT(iter == iter1);
                ASSERT(iter != iter2);

                iter = iter2;

                ASSERT(iter != iter1);
                ASSERT(iter == iter2);
            }
            {
                Iterator iter(iter1);

                ASSERT(iter == iter1);
                ASSERT(iter != iter2);
            }
            {
                Iterator iter(iter2);

                ASSERT(iter != iter1);
                ASSERT(iter == iter2);
            }
        }
        {
            UnsignedObj mX;  const UnsignedObj& X = mX;
            mX.append(k_INT8_MAX);

            UnsignedIterator iter1 = X.begin();
            UnsignedIterator iter2 = X.end();
            ASSERT(iter1 != iter2);

            {
                UnsignedIterator iter;

                ASSERT(iter != iter1);
                ASSERT(iter != iter2);

                iter = iter1;

                ASSERT(iter == iter1);
                ASSERT(iter != iter2);

                iter = iter2;

                ASSERT(iter != iter1);
                ASSERT(iter == iter2);
            }
            {
                UnsignedIterator iter(iter1);

                ASSERT(iter == iter1);
                ASSERT(iter != iter2);
            }
            {
                UnsignedIterator iter(iter2);

                ASSERT(iter != iter1);
                ASSERT(iter == iter2);
            }
        }

        if (verbose) cout << "\nTesting basic methods."
                          << endl;
        {
            static const struct {
                int          d_lineNum;          // source line number
                const char  *d_spec_p;           // specification string
                bsl::size_t  d_length;           // expected length
            } DATA[] = {
                //line  spec      len
                //----  --------  ---
                { L_,   "",         0 },
                { L_,   "z",        1 },
                { L_,   "o",        1 },
                { L_,   "O",        1 },
                { L_,   "c",        1 },
                { L_,   "C",        1 },
                { L_,   "s",        1 },
                { L_,   "S",        1 },
                { L_,   "i",        1 },
                { L_,   "I",        1 },
                { L_,   "l",        1 },
                { L_,   "L",        1 },
                { L_,   "^",        1 },
                { L_,   "!",        1 },
                { L_,   "@",        1 },
                { L_,   "$",        1 },
                { L_,   "(",        1 },
                { L_,   ")",        1 },
                { L_,   "Lx",       0 },
                { L_,   "LxS",      1 },
                { L_,   "LxSx",     0 },
                { L_,   "CSI",      3 },
                { L_,   "CSIx",     0 },
                { L_,   "zOC",      3 },
                { L_,   "zSOsC",    5 },
                { L_,   "zSOIsC",   6 },
                { L_,   "LzSOIsC",  7 }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const bsl::size_t LEN  = DATA[ti].d_length;

                Obj mX;  const Obj& X = gg(&mX, SPEC);   // original spec

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                LOOP_ASSERT(LINE, LEN == X.length());

                // Verify assignment return value.

                Iterator iter;
                ASSERT(&iter == &(iter = X.begin()));

                for (int i = 0; i < static_cast<int>(LEN); ++i) {
                    // Verify 'operator*' and 'operator->'.

                    LOOP_ASSERT(i, *iter == X[i]);
                    LOOP_ASSERT(i, iter.operator->() == X[i]);

                    // Verify 'operator[]' including with negative index.

                    for (int j = 0; j < static_cast<int>(LEN); ++j) {
                        if (i + j < static_cast<int>(LEN)) {
                            LOOP2_ASSERT(i, j, iter[ j] == X[i + j]);
                        }
                        if (i - j >= 0) {
                            LOOP2_ASSERT(i, j, iter[-j] == X[i - j]);
                        }
                    }

                    // Verify 'operator++' and 'operator--'.

                    Iterator iterOrig(iter);

                    Iterator iter2(++iter);
                    ASSERT(iter2 == iter);

                    Iterator iter3(--iter2);
                    ASSERT(iter2 == iterOrig);
                    ASSERT(iter3 == iterOrig);
                }

                ASSERT(iter == X.end());

                LOOP_ASSERT(LINE,
                            defaultAllocator.numAllocations() == allocations);

                // Verify 'begin()'.

                Obj mY;  const Obj Y = mY;

                Iterator initialBegin = Y.begin();

                iter = Y.begin();
                ASSERT(iter == Y.end());
                ASSERT(iter == initialBegin);

                mY = X;
                iter = Y.begin();
                ASSERT(iter == initialBegin);
                ASSERT(Y.isEmpty() || *iter == Y[0]);
            }
        }
        {
            static const struct {
                int              d_lineNum;          // source line number
                const char      *d_spec_p;           // specification string
                bsl::size_t      d_length;           // expected length
            } DATA[] = {
                //line  spec      len
                //----  --------  ---
                { L_,   "",         0 },
                { L_,   "z",        1 },
                { L_,   "O",        1 },
                { L_,   "C",        1 },
                { L_,   "S",        1 },
                { L_,   "I",        1 },
                { L_,   "L",        1 },
                { L_,   "s",        1 },
                { L_,   "i",        1 },
                { L_,   "l",        1 },
                { L_,   "Lx",       0 },
                { L_,   "LxS",      1 },
                { L_,   "LxSx",     0 },
                { L_,   "CSI",      3 },
                { L_,   "CSIx",     0 },
                { L_,   "zOC",      3 },
                { L_,   "zSOsC",    5 },
                { L_,   "zSOIsC",   6 },
                { L_,   "LzSOIsC",  7 }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const bsl::size_t LEN  = DATA[ti].d_length;

                UnsignedObj        mX;
                const UnsignedObj& X = gg(&mX, SPEC);   // original spec

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                LOOP_ASSERT(LINE, LEN == X.length());

                // Verify assignment return value.

                UnsignedIterator iter;
                ASSERT(&iter == &(iter = X.begin()));

                for (int i = 0; i < static_cast<int>(LEN); ++i) {
                    // Verify 'operator*' and 'operator->'.

                    LOOP_ASSERT(i, *iter == X[i]);
                    LOOP_ASSERT(i, iter.operator->() == X[i]);

                    // Verify 'operator[]' including with negative index.

                    for (int j = 0; j < static_cast<int>(LEN); ++j) {
                        if (i + j < static_cast<int>(LEN)) {
                            LOOP2_ASSERT(i, j, iter[ j] == X[i + j]);
                        }
                        if (i - j >= 0) {
                            LOOP2_ASSERT(i, j, iter[-j] == X[i - j]);
                        }
                    }

                    // Verify 'operator++' and 'operator--'.

                    UnsignedIterator iterOrig(iter);

                    UnsignedIterator iter2(++iter);
                    ASSERT(iter2 == iter);

                    UnsignedIterator iter3(--iter2);
                    ASSERT(iter2 == iterOrig);
                    ASSERT(iter3 == iterOrig);
                }

                ASSERT(iter == X.end());

                LOOP_ASSERT(LINE,
                            defaultAllocator.numAllocations() == allocations);

                // Verify 'begin()'.

                UnsignedObj mY;  const UnsignedObj Y = mY;

                UnsignedIterator initialBegin = Y.begin();

                iter = Y.begin();
                ASSERT(iter == Y.end());
                ASSERT(iter == initialBegin);

                mY = X;
                iter = Y.begin();
                ASSERT(iter == initialBegin);
                ASSERT(Y.isEmpty() || *iter == Y[0]);
            }
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj mX;  const Obj& X = mX;
            mX.append(k_INT8_MAX);

            Iterator iter;
            ASSERT_SAFE_FAIL(*iter);
            ASSERT_SAFE_FAIL(iter.operator->());
            ASSERT_SAFE_FAIL(iter[0]);

            iter = X.begin();
            ASSERT_SAFE_FAIL(iter[-1]);
            ASSERT_SAFE_PASS(iter[ 0]);
            ASSERT_SAFE_FAIL(iter[ 1]);

            iter = X.begin();
            ASSERT_SAFE_PASS(++iter);
            ASSERT_SAFE_PASS(--iter);
            ASSERT_SAFE_FAIL(--iter);

            iter = X.end();
            ASSERT_SAFE_PASS(--iter);
            ASSERT_SAFE_PASS(++iter);
            ASSERT_SAFE_FAIL(++iter);

            iter = X.begin();
            ASSERT_SAFE_PASS(*iter);
            ASSERT_SAFE_PASS(iter.operator->());
            mX.removeAll();
            ASSERT_SAFE_FAIL(*iter);
            ASSERT_SAFE_FAIL(iter.operator->());
        }
        {
            bsls::AssertTestHandlerGuard hG;

            UnsignedObj mX;  const UnsignedObj& X = mX;
            mX.append(k_INT8_MAX);

            UnsignedIterator iter;
            ASSERT_SAFE_FAIL(*iter);
            ASSERT_SAFE_FAIL(iter.operator->());
            ASSERT_SAFE_FAIL(iter[0]);

            iter = X.begin();
            ASSERT_SAFE_FAIL(iter[-1]);
            ASSERT_SAFE_PASS(iter[ 0]);
            ASSERT_SAFE_FAIL(iter[ 1]);

            iter = X.begin();
            ASSERT_SAFE_PASS(++iter);
            ASSERT_SAFE_PASS(--iter);
            ASSERT_SAFE_FAIL(--iter);

            iter = X.end();
            ASSERT_SAFE_PASS(--iter);
            ASSERT_SAFE_PASS(++iter);
            ASSERT_SAFE_FAIL(++iter);

            iter = X.begin();
            ASSERT_SAFE_PASS(*iter);
            ASSERT_SAFE_PASS(iter.operator->());
            mX.removeAll();
            ASSERT_SAFE_FAIL(*iter);
            ASSERT_SAFE_FAIL(iter.operator->());
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING 'front' AND 'back'
        //   Ensure these accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding attribute of
        //:   the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //:
        //: 3 No accessor allocates any memory.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the generator function to produce objects of arbitrary state
        //:   and verify the accessors' return values against expected values.
        //:   (C-1)
        //:
        //: 2 The accessors will only be accessed from a 'const' reference to
        //:   the created object.  (C-2)
        //:
        //: 3 The default allocator will be used for all created objects
        //:   (excluding those used to test 'allocator') and the number of
        //:   allocation will be verified to ensure that no memory was
        //:   allocated during use of the accessors.  (C-3)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   TYPE back() const;
        //   TYPE front() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'front' AND 'back'" << endl
                          << "==========================" << endl;

        {
            static const struct {
                int          d_lineNum;  // source line number
                const char  *d_spec_p;   // specification string
                Element      d_front;    // expected 'front' value
                Element      d_back;     // expected 'back' value
            } DATA[] = {
                //line    spec        front             back
                //----  --------  ---------------  ---------------
                { L_,   "z",                    0,               0 },
                { L_,   "o",                   -1,              -1 },
                { L_,   "O",                    1,               1 },
                { L_,   "c",           k_INT8_MIN,      k_INT8_MIN },
                { L_,   "C",           k_INT8_MAX,      k_INT8_MAX },
                { L_,   "s",          k_INT16_MIN,     k_INT16_MIN },
                { L_,   "S",          k_INT16_MAX,     k_INT16_MAX },
                { L_,   "i",          k_INT32_MIN,     k_INT32_MIN },
                { L_,   "I",          k_INT32_MAX,     k_INT32_MAX },
                { L_,   "l",          k_INT64_MIN,     k_INT64_MIN },
                { L_,   "L",          k_INT64_MAX,     k_INT64_MAX },
                { L_,   "^",       k_INT8_MIN - 1,  k_INT8_MIN - 1 },
                { L_,   "!",       k_INT8_MAX + 1,  k_INT8_MAX + 1 },
                { L_,   "@",      k_INT16_MIN - 1, k_INT16_MIN - 1 },
                { L_,   "$",      k_INT16_MAX + 1, k_INT16_MAX + 1 },
                { L_,   "(",      k_INT32_MIN - 1, k_INT32_MIN - 1 },
                { L_,   ")",      k_INT32_MAX + 1, k_INT32_MAX + 1 },
                { L_,   "LxS",        k_INT16_MAX,     k_INT16_MAX },
                { L_,   "CS",          k_INT8_MAX,     k_INT16_MAX },
                { L_,   "SC",         k_INT16_MAX,      k_INT8_MAX },
                { L_,   "CSI",         k_INT8_MAX,     k_INT32_MAX },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec_p;
                const Element     FRONT = DATA[ti].d_front;
                const Element     BACK  = DATA[ti].d_back;

                Obj mX;  const Obj& X = gg(&mX, SPEC);   // original spec

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                LOOP_ASSERT(LINE, FRONT == X.front());
                LOOP_ASSERT(LINE, BACK  == X.back());

                LOOP_ASSERT(LINE,
                            defaultAllocator.numAllocations() == allocations);
            }
        }
        {
            static const struct {
                int              d_lineNum;  // source line number
                const char      *d_spec_p;   // specification string
                UnsignedElement  d_front;    // expected 'front' value
                UnsignedElement  d_back;     // expected 'back' value
            } DATA[] = {
                //line    spec        front              back
                //----  --------  ----------------  ----------------
                { L_,   "z",                     0,                0 },
                { L_,   "O",                     1,                1 },
                { L_,   "C",           k_UINT8_MAX,      k_UINT8_MAX },
                { L_,   "S",          k_UINT16_MAX,     k_UINT16_MAX },
                { L_,   "I",          k_UINT32_MAX,     k_UINT32_MAX },
                { L_,   "L",          k_UINT64_MAX,     k_UINT64_MAX },
                { L_,   "LxS",        k_UINT16_MAX,     k_UINT16_MAX },
                { L_,   "CS",          k_UINT8_MAX,     k_UINT16_MAX },
                { L_,   "SC",         k_UINT16_MAX,      k_UINT8_MAX },
                { L_,   "CSI",         k_UINT8_MAX,     k_UINT32_MAX },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int             LINE  = DATA[ti].d_lineNum;
                const char *const     SPEC  = DATA[ti].d_spec_p;
                const UnsignedElement FRONT = DATA[ti].d_front;
                const UnsignedElement BACK = DATA[ti].d_back;

                UnsignedObj        mX;
                const UnsignedObj& X = gg(&mX, SPEC);   // original spec

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                LOOP_ASSERT(LINE, FRONT == X.front());
                LOOP_ASSERT(LINE, BACK  == X.back());

                LOOP_ASSERT(LINE,
                            defaultAllocator.numAllocations() == allocations);
            }
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // VALUE CONSTRUCTOR
        //   Ensure that we can create an array with a specified length and
        //   initial value.  Ensure that all attributes of the array are set
        //   appropriately for these parameters.
        //
        // Concerns:
        //: 1 The value constructor creates the correct initial value and has
        //:   the internal memory management system hooked up properly so that
        //:   *all* internally allocated memory draws from the same
        //:   user-supplied allocator whenever one is specified.
        //:
        //: 2 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 3 There is no temporary memory allocation from any allocator.
        //:
        //: 4 Any memory allocation is exception neutral.
        //:
        //: 5 The value constructor (with or without a supplied allocator)
        //:   creates an object having the specified numElements of the
        //:   specified initial value.
        //:
        //: 6 If an initial value is NOT supplied to the value constructor, the
        //:   default value of 0 is used.
        //:
        //: 7 All allocated memory is returned to the allocator.
        //
        // Plan:
        //: 1 Create an object using the value constructor with and without
        //:   passing in an allocator, verify the allocator is stored using the
        //:   'allocator' accessor, and verifying all allocations are done from
        //:   the allocator in future tests.  (C-1,2)
        //:
        //: 2 Monitor memory usage from the default allocator to ensure all
        //:   memory is obtained from the supplied allocator.  (C-3)
        //:
        //: 3 Use the loop-based approach to create objects (both signed and
        //:   unsigned) with various values for 'numElements' and 'value' with
        //:   a 'bslma::TestAllocator' and compare all aspects of this object
        //:   with the oracle value obtained from a default constructed object
        //:   with the 'value' appended the appropriate number of times.  Also
        //:   vary the test allocator's allocation limit to verify behavior in
        //:   the presence of exceptions.  (C-4..6)
        //:
        //: 4 Use a supplied 'bslma::TestAllocator' that goes out-of-scope
        //:   at the conclusion of each test to ensure all memory is returned
        //:   to the allocator.  (C-7)
        //
        // Testing:
        //   PackedIntArray(numElements, value, basicAllocator);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VALUE CONSTRUCTOR" << endl
                          << "=================" << endl;

        if (verbose) cout << "\nTesting with various allocator configurations."
                          << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            Obj mX(3);  const Obj& X = mX;
            ASSERT(&defaultAllocator == X.allocator());
            ASSERT(3 == X.length());
            ASSERT(3 == X.capacity());
            ASSERT(allocations + 1 == defaultAllocator.numAllocations());
        }
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            Obj        mX(3, 0, reinterpret_cast<bslma::TestAllocator *>(0));
            const Obj& X = mX;
            ASSERT(&defaultAllocator == X.allocator());
            ASSERT(3 == X.length());
            ASSERT(3 == X.capacity());
            ASSERT(allocations + 1 == defaultAllocator.numAllocations());
        }
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(3, 0, &sa);  const Obj& X = mX;
            ASSERT(&sa == X.allocator());
            ASSERT(3 == X.length());
            ASSERT(3 == X.capacity());
            ASSERT(allocations == defaultAllocator.numAllocations());
            ASSERT(1 == sa.numAllocations());
        }

        if (verbose) cout << "\nTesting with 'numElements'."
                          << endl;
        {
            bslma::TestAllocator         sa(veryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&sa);

            for (bsl::size_t ne = 0; ne < 50; ++ne) {
                Obj                mEXP;
                const Obj&         EXP  = mEXP;
                UnsignedObj        mUEXP;
                const UnsignedObj& UEXP = mUEXP;
                for (bsl::size_t i = 0; i < ne; ++i) {
                    if (veryVerbose) { P_(ne) P(i) }

                    mEXP.append(0);
                    mUEXP.append(0);
                }

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    Obj mX(ne);  const Obj& X = mX;

                    LOOP_ASSERT(ne, X == EXP);
                    LOOP_ASSERT(ne,
                                X.bytesPerElement() == EXP.bytesPerElement());
                    LOOP_ASSERT(ne, X.capacity() == EXP.capacity());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    UnsignedObj mX(ne);  const UnsignedObj& X = mX;

                    LOOP_ASSERT(ne, X == UEXP);
                    LOOP_ASSERT(ne,
                                X.bytesPerElement() == UEXP.bytesPerElement());
                    LOOP_ASSERT(ne, X.capacity() == UEXP.capacity());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP_ASSERT(ne,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nTesting with 'numElements' and 'value'."
                          << endl;
        {
            const char        *VALUES = "cCsSiIlL";
            const bsl::size_t  NUM_VALUES = strlen(VALUES);

            for (bsl::size_t ne = 0; ne < 10; ++ne) {
                for (bsl::size_t vi = 0; vi < NUM_VALUES; ++vi) {
                    Element         VALUE;   getValue(VALUES[vi], &VALUE,  0);
                    UnsignedElement UVALUE;  getValue(VALUES[vi], &UVALUE, 0);

                    Obj                mEXP;
                    const Obj&         EXP  = mEXP;
                    UnsignedObj        mUEXP;
                    const UnsignedObj& UEXP = mUEXP;
                    for (bsl::size_t i = 0; i < ne; ++i) {
                        if (veryVerbose) { P_(ne) P_(vi) P(i) }

                        mEXP.append(VALUE);
                        mUEXP.append(UVALUE);
                    }

                    bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                    bslma::TestAllocator sa(veryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj mX(ne, VALUE, &sa);  const Obj& X = mX;

                        LOOP_ASSERT(ne, X == EXP);
                        LOOP_ASSERT(ne,
                                    X.bytesPerElement()
                                                     == EXP.bytesPerElement());
                        LOOP_ASSERT(ne, X.capacity() == EXP.capacity());
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        UnsignedObj        mX(ne, UVALUE, &sa);
                        const UnsignedObj& X = mX;

                        LOOP_ASSERT(ne, X == UEXP);
                        LOOP_ASSERT(ne,
                                    X.bytesPerElement()
                                                    == UEXP.bytesPerElement());
                        LOOP_ASSERT(ne, X.capacity() == UEXP.capacity());
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    LOOP_ASSERT(ne,
                                allocations
                                         == defaultAllocator.numAllocations());
                }
            }
        }

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // RESIZE
        //   The 'resize' method operates as expected.
        //
        // Concerns:
        //: 1 The method is exception neutral with respect to memory
        //:   allocation.
        //:
        //: 2 The method produces the expected value.
        //
        // Plan:
        //: 1 Wrap all tests with the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
        //:   macros to ensure exception neutrality with respect to memory
        //:   allocation.  (C-1)
        //:
        //: 2 Specify a set of specifications for the 'gg' function that result
        //:   in an empty object but with different capacity and bytes per
        //:   element characteristics.
        //:
        //: 3 Use the table-based approach to specify a set of test vectors.
        //:
        //: 4 For every item in the cross-product of these two sets, verify
        //:   the result for the method (the set from 2 is applied to only
        //:   the initial object).  (C-2)
        //
        // Testing:
        //   void resize(bsl::size_t numElements);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "RESIZE" << endl
                          << "======" << endl;

        static const struct {
            int         d_lineNum;
            const char *d_spec_p;
        } INIT[] = {
            // line spec
            //----  ----
            { L_,   ""              },
            { L_,   "Lp"            },
            { L_,   "LLLLLLLLLLx"   },
            { L_,   "LLLLLLLLLLxLp" },
        };
        const int NUM_INIT = static_cast<int>(sizeof INIT / sizeof *INIT);

        if (verbose) cout << "\nTesting resize." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec_p;       // specification
                bsl::size_t d_ne;           // number of elements
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec      ne  exp
                //----  --------  --  ----------------------------
                { L_,   "",        0, ""                           },
                { L_,   "",        1, "z"                          },
                { L_,   "",        2, "zz"                         },
                { L_,   "",        3, "zzz"                        },
                { L_,   "",        4, "zzzz"                       },
                { L_,   "",        5, "zzzzz"                      },

                { L_,   "I",       0, ""                           },
                { L_,   "I",       1, "I"                          },
                { L_,   "I",       2, "Iz"                         },
                { L_,   "I",       3, "Izz"                        },
                { L_,   "I",       4, "Izzz"                       },
                { L_,   "I",       5, "Izzzz"                      },

                { L_,   "CS",      0, ""                           },
                { L_,   "CS",      1, "C"                          },
                { L_,   "CS",      2, "CS"                         },
                { L_,   "CS",      3, "CSz"                        },
                { L_,   "CS",      4, "CSzz"                       },
                { L_,   "CS",      5, "CSzzz"                      },

                { L_,   "CSI",     0, ""                           },
                { L_,   "CSI",     1, "C"                          },
                { L_,   "CSI",     2, "CS"                         },
                { L_,   "CSI",     3, "CSI"                        },
                { L_,   "CSI",     4, "CSIz"                       },
                { L_,   "CSI",     5, "CSIzz"                      },

                { L_,   "CSIL",    0, ""                           },
                { L_,   "CSIL",    1, "C"                          },
                { L_,   "CSIL",    2, "CS"                         },
                { L_,   "CSIL",    3, "CSI"                        },
                { L_,   "CSIL",    4, "CSIL"                       },
                { L_,   "CSIL",    5, "CSILz"                      },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const bsl::size_t NE      = DATA[i].d_ne;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj mEXP;  gg(&mEXP, EXPSPEC);  const Obj& EXP = mEXP;

                UnsignedObj        mUEXP;  gg(&mUEXP, EXPSPEC);
                const UnsignedObj& UEXP = mUEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC = (bsl::string()
                                        + INIT[j].d_spec_p
                                        + DATA[i].d_spec_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC.c_str());
                        const Obj& X = mX;
                        mX.resize(NE);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        UnsignedObj        mUX(&sa);  gg(&mUX, SPEC.c_str());
                        const UnsignedObj& UX = mUX;
                        mUX.resize(NE);
                        LOOP_ASSERT(LINE, UX == UEXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // RESERVE CAPACITY
        //   The 'reserveCapacity' methods operate as expected.
        //
        // Concerns:
        //: 1 The methods are exception neutral with respect to memory
        //:   allocation.
        //:
        //: 2 The methods convert the extrema to the correct number of required
        //    bytes per element.
        //:
        //: 3 The methods produce the expected state without affecting the
        //:   value.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Wrap all tests with the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
        //:   macros to ensure exception neutrality with respect to memory
        //:   allocation.  (C-1)
        //:
        //: 2 Use the 'reserve' methods to verify the conversion to required
        //:   bytes is correct.  (C-2)
        //:
        //: 3 Use the table-based approach to specify a set of test vectors.
        //:   After direct validation of expected results, use 'removeAll' and
        //:   verify the number of bytes of capacity.  (C-3)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   void reserveCapacity(numElements);
        //   void reserveCapacity(numElements, maxValue);
        //   void reserveCapacity(numElements, minValue, maxValue);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "RESERVE CAPACITY" << endl
                          << "================" << endl;

        if (verbose) {
            cout << "\nVerify computation of required bytes per element."
                 << endl;
        }
        {
            { // Test the signed object.
                static const struct {
                    int         d_lineNum;      // source line number
                    int         d_ne;           // number of elements
                    const char  d_minSpec;      // spec for min value
                    const char  d_maxSpec;      // spec for max value
                    bsl::size_t d_ec;           // expected capacity
                } DATA[] = {
                    //line  ne  min  max  ec
                    //----  --  ---  ---  --
                    { L_,   10, 'c', 'z', 10 },
                    { L_,   10, 'z', 'C', 10 },
                    { L_,    3, 's', 'z',  6 },
                    { L_,    3, 'z', 'S',  6 },
                    { L_,    3, '^', 'z',  6 },
                    { L_,    3, 'z', '!',  6 },
                    { L_,    4, 'i', 'z', 16 },
                    { L_,    4, 'z', 'I', 16 },
                    { L_,    4, '@', 'z', 16 },
                    { L_,    4, 'z', '$', 16 },
                    { L_,    2, 'l', 'z', 16 },
                    { L_,    2, 'z', 'L', 16 },
                    { L_,    2, '(', 'z', 16 },
                    { L_,    2, 'z', ')', 16 },
                };
                const int NUM_DATA = static_cast<int>(sizeof DATA
                                                      / sizeof *DATA);

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int         LINE = DATA[i].d_lineNum;
                    const int         NE   = DATA[i].d_ne;
                    const char        MIN  = DATA[i].d_minSpec;
                    const char        MAX  = DATA[i].d_maxSpec;
                    const bsl::size_t EC   = DATA[i].d_ec;

                    bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    Element minValue;  getValue(MIN, &minValue, 0);
                    Element maxValue;  getValue(MAX, &maxValue, 0);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj mX(&sa);  const Obj& X = mX;

                        mX.reserveCapacity(NE, minValue, maxValue);
                        LOOP_ASSERT(LINE, X.capacity() == EC);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
                }
            }
            { // Test the unsigned object.
                static const struct {
                    int         d_lineNum;      // source line number
                    int         d_ne;           // number of elements
                    const char  d_maxSpec;      // spec for max value
                    bsl::size_t d_ec;           // expected capacity
                } DATA[] = {
                    //line  ne  max  ec
                    //----  --  ---  --
                    { L_,   10, 'C', 10 },
                    { L_,    3, 's',  6 },
                    { L_,    3, 'S',  6 },
                    { L_,    4, 'i', 16 },
                    { L_,    4, 'I', 16 },
                    { L_,    2, 'l', 16 },
                    { L_,    2, 'L', 16 },
                };
                const int NUM_DATA = static_cast<int>(sizeof DATA
                                                      / sizeof *DATA);

                for (int i = 0; i < NUM_DATA; ++i) {
                    const int         LINE = DATA[i].d_lineNum;
                    const int         NE   = DATA[i].d_ne;
                    const char        MAX  = DATA[i].d_maxSpec;
                    const bsl::size_t EC   = DATA[i].d_ec;

                    bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    UnsignedElement maxValue;  getValue(MAX, &maxValue, 0);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        UnsignedObj mX(&sa);  const UnsignedObj& X = mX;

                        mX.reserveCapacity(NE, maxValue);
                        LOOP_ASSERT(LINE, X.capacity() == EC);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
                }
            }
        }

        if (verbose) cout << "\nReserve capacity without values." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec_p;       // specification
                int         d_ne;           // number of elements
                bsl::size_t d_ec;           // expected capacity
                bsl::size_t d_eb;           // expected number of bytes
            } DATA[] = {
                //line  spec      ne  ec  eb
                //----  --------  --  --  ---
                { L_,   "",        0,  0,   0 },
                { L_,   "",        1, 10,  10 },
                { L_,   "",        2, 16,  16 },
                { L_,   "",        3, 25,  25 },
                { L_,   "",        4, 39,  39 },
                { L_,   "",        5, 60,  60 },

                { L_,   "I",       0,  1,   6 },
                { L_,   "I",       1,  2,  10 },
                { L_,   "I",       2,  4,  16 },
                { L_,   "I",       3,  6,  25 },
                { L_,   "I",       4,  9,  39 },
                { L_,   "I",       5, 15,  60 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE = DATA[i].d_lineNum;
                const char *const SPEC = DATA[i].d_spec_p;
                const int         NE   = DATA[i].d_ne;
                const bsl::size_t EC   = DATA[i].d_ec;
                const bsl::size_t EB   = DATA[i].d_eb;

                Obj mEXP;           gg(&mEXP, SPEC);   const Obj& EXP = mEXP;

                UnsignedObj        mUEXP;  gg(&mUEXP, SPEC);
                const UnsignedObj& UEXP = mUEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    Obj mX(&sa);  gg(&mX, SPEC);  const Obj& X = mX;

                    mX.reserveCapacity(NE);
                    LOOP_ASSERT(LINE, X.capacity() == EC);
                    LOOP_ASSERT(LINE, X == EXP);

                    mX.removeAll();
                    LOOP_ASSERT(LINE, X.capacity() == EB);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    UnsignedObj        mX(&sa);  gg(&mX, SPEC);
                    const UnsignedObj& X = mX;

                    mX.reserveCapacity(NE);
                    LOOP_ASSERT(LINE, X.capacity() == EC);
                    LOOP_ASSERT(LINE, X == UEXP);

                    mX.removeAll();
                    LOOP_ASSERT(LINE, X.capacity() == EB);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nReserve capacity with max value." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec_p;       // specification
                int         d_ne;           // number of elements
                const char  d_maxSpec;      // spec for max value
                bsl::size_t d_ec;           // expected capacity
                bsl::size_t d_eb;           // expected number of bytes
            } DATA[] = {
                //line  spec      ne  max  ec  eb
                //----  --------  --  ---  --  ---
                { L_,   "",        0, 'C',  0,   0 },
                { L_,   "",        1, 'C',  1,   1 },
                { L_,   "",        2, 'C',  3,   3 },
                { L_,   "",        3, 'C',  3,   3 },
                { L_,   "",        4, 'C',  6,   6 },
                { L_,   "",        5, 'C',  6,   6 },

                { L_,   "",        0, 'S',  0,   0 },
                { L_,   "",        1, 'S',  3,   3 },
                { L_,   "",        2, 'S',  6,   6 },
                { L_,   "",        3, 'S',  6,   6 },
                { L_,   "",        4, 'S', 10,  10 },
                { L_,   "",        5, 'S', 10,  10 },

                { L_,   "S",       0, 'C',  1,   3 },
                { L_,   "S",       1, 'C',  1,   3 },
                { L_,   "S",       2, 'C',  3,   6 },
                { L_,   "S",       3, 'C',  3,   6 },
                { L_,   "S",       4, 'C',  5,  10 },
                { L_,   "S",       5, 'C',  5,  10 },

                { L_,   "S",       0, 'I',  1,   3 },
                { L_,   "S",       1, 'I',  3,   6 },
                { L_,   "S",       2, 'I',  5,  10 },
                { L_,   "S",       3, 'I',  8,  16 },
                { L_,   "S",       4, 'I',  8,  16 },
                { L_,   "S",       5, 'I', 12,  25 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE = DATA[i].d_lineNum;
                const char *const SPEC = DATA[i].d_spec_p;
                const int         NE   = DATA[i].d_ne;
                const char        MAX  = DATA[i].d_maxSpec;
                const bsl::size_t EC   = DATA[i].d_ec;
                const bsl::size_t EB   = DATA[i].d_eb;

                Obj mEXP;           gg(&mEXP, SPEC);   const Obj& EXP = mEXP;

                UnsignedObj        mUEXP;  gg(&mUEXP, SPEC);
                const UnsignedObj& UEXP = mUEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    Obj mX(&sa);  gg(&mX, SPEC);  const Obj& X = mX;

                    Element maxValue;  getValue(MAX, &maxValue, 0);

                    mX.reserveCapacity(NE, maxValue);
                    LOOP_ASSERT(LINE, X.capacity() == EC);
                    LOOP_ASSERT(LINE, X == EXP);

                    mX.removeAll();
                    LOOP_ASSERT(LINE, X.capacity() == EB);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    UnsignedObj        mX(&sa);  gg(&mX, SPEC);
                    const UnsignedObj& X = mX;
                    UnsignedElement    maxValue;  getValue(MAX, &maxValue, 0);

                    mX.reserveCapacity(NE, maxValue);
                    LOOP_ASSERT(LINE, X.capacity() == EC);
                    LOOP_ASSERT(LINE, X == UEXP);

                    mX.removeAll();
                    LOOP_ASSERT(LINE, X.capacity() == EB);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nReserve capacity with min and max values."
                          << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec_p;       // specification
                int         d_ne;           // number of elements
                const char  d_minSpec;      // spec for min value
                const char  d_maxSpec;      // spec for max value
                bsl::size_t d_ec;           // expected capacity
                bsl::size_t d_eb;           // expected number of bytes
            } DATA[] = {
                //line  spec      ne  min  max  ec  eb
                //----  --------  --  ---  ---  --  ---
                { L_,   "",        0, 'C', 'C',  0,   0 },
                { L_,   "",        1, 'C', 'C',  1,   1 },
                { L_,   "",        2, 'C', 'C',  3,   3 },
                { L_,   "",        3, 'C', 'C',  3,   3 },
                { L_,   "",        4, 'C', 'C',  6,   6 },
                { L_,   "",        5, 'C', 'C',  6,   6 },

                { L_,   "",        0, 'C', 'S',  0,   0 },
                { L_,   "",        1, 'C', 'S',  3,   3 },
                { L_,   "",        2, 'C', 'S',  6,   6 },
                { L_,   "",        3, 'C', 'S',  6,   6 },
                { L_,   "",        4, 'C', 'S', 10,  10 },
                { L_,   "",        5, 'C', 'S', 10,  10 },

                { L_,   "S",       0, 'C', 'C',  1,   3 },
                { L_,   "S",       1, 'C', 'C',  1,   3 },
                { L_,   "S",       2, 'C', 'C',  3,   6 },
                { L_,   "S",       3, 'C', 'C',  3,   6 },
                { L_,   "S",       4, 'C', 'C',  5,  10 },
                { L_,   "S",       5, 'C', 'C',  5,  10 },

                { L_,   "S",       0, 'C', 'I',  1,   3 },
                { L_,   "S",       1, 'C', 'I',  3,   6 },
                { L_,   "S",       2, 'C', 'I',  5,  10 },
                { L_,   "S",       3, 'C', 'I',  8,  16 },
                { L_,   "S",       4, 'C', 'I',  8,  16 },
                { L_,   "S",       5, 'C', 'I', 12,  25 },

                { L_,   "I",       1, 'c', 'S',  1,   6 },
                { L_,   "I",       1, 's', 'S',  1,   6 },
                { L_,   "I",       1, 'i', 'S',  1,   6 },
                { L_,   "I",       1, 'l', 'S',  2,  10 },

                { L_,   "I",       2, 'c', 'S',  2,  10 },
                { L_,   "I",       2, 's', 'S',  2,  10 },
                { L_,   "I",       2, 'i', 'S',  2,  10 },
                { L_,   "I",       2, 'l', 'S',  4,  16 },

                { L_,   "I",       3, 'c', 'S',  4,  16 },
                { L_,   "I",       3, 's', 'S',  4,  16 },
                { L_,   "I",       3, 'i', 'S',  4,  16 },
                { L_,   "I",       3, 'l', 'S',  6,  25 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE = DATA[i].d_lineNum;
                const char *const SPEC = DATA[i].d_spec_p;
                const int         NE   = DATA[i].d_ne;
                const char        MIN  = DATA[i].d_minSpec;
                const char        MAX  = DATA[i].d_maxSpec;
                const bsl::size_t EC   = DATA[i].d_ec;
                const bsl::size_t EB   = DATA[i].d_eb;

                Obj mEXP;           gg(&mEXP, SPEC);   const Obj& EXP = mEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    Obj mX(&sa);  gg(&mX, SPEC);  const Obj& X = mX;

                    Element minValue;  getValue(MIN, &minValue, 0);
                    Element maxValue;  getValue(MAX, &maxValue, 0);

                    mX.reserveCapacity(NE, minValue, maxValue);
                    LOOP_ASSERT(LINE, X.capacity() == EC);
                    LOOP_ASSERT(LINE, X == EXP);

                    mX.removeAll();
                    LOOP_ASSERT(LINE, X.capacity() == EB);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj mX;
            ASSERT_SAFE_PASS(mX.reserveCapacity(10, 0, 0));
            ASSERT_SAFE_FAIL(mX.reserveCapacity(10, 1, 0));

            UnsignedObj mUX;
            ASSERT_SAFE_PASS(mUX.reserveCapacity(10, 0, 0));
            ASSERT_SAFE_FAIL(mUX.reserveCapacity(10, 1, 0));
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // REPLACE
        //   The 'replace' methods operate as expected.
        //
        // Concerns:
        //: 1 The methods are exception neutral with respect to memory
        //:   allocation.
        //:
        //: 2 The methods produce the expected value.
        //:
        //: 3 The methods are alias safe.
        //:
        //: 4 When there is initially sufficient storage in the result variable
        //:   to store the result, the methods produce the expected value.
        //:
        //: 5 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Wrap all tests with the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
        //:   macros to ensure exception neutrality with respect to memory
        //:   allocation.  (C-1)
        //:
        //: 2 Specify a set of specifications for the 'gg' function that result
        //:   in an empty object but with different capacity and bytes per
        //:   element characteristics.
        //:
        //: 3 Use the table-based approach to specify a set of test vectors.
        //:
        //: 4 For every item in the cross-product of these two sets, verify
        //:   the result for the method (the set from 2 is applied to only
        //:   the initial object).  (C-2..4)
        //:
        //: 5 Verify defensive checks are triggered for invalid values.  (C-5)
        //
        // Testing:
        //   void replace(di, value);
        //   void replace(di, const PackedIntArray& srcArray, si, ne);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "REPLACE" << endl
                          << "=======" << endl;

        static const struct {
            int         d_lineNum;
            const char *d_spec_p;
        } INIT[] = {
            // line spec
            //----  ----
            { L_,   ""              },
            { L_,   "Lp"            },
            { L_,   "LLLLLLLLLLx"   },
            { L_,   "LLLLLLLLLLxLp" },
        };
        const int NUM_INIT = static_cast<int>(sizeof INIT / sizeof *INIT);

        if (verbose) cout << "\nValue replace." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec1_p;      // specification 1
                int         d_di;           // destination index
                const char  d_spec2_p;      // specification 2
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec1     di  spec2     exp
                //----  --------  --  --------  ----------------------------
                { L_,   "C",       0, 'S',      "S"                          },

                { L_,   "CS",      0, 'I',      "IS"                         },
                { L_,   "CS",      1, 'I',      "CI"                         },

                { L_,   "CSIL",    0, 'I',      "ISIL"                       },
                { L_,   "CSIL",    1, 'I',      "CIIL"                       },
                { L_,   "CSIL",    2, 'I',      "CSIL"                       },
                { L_,   "CSIL",    3, 'I',      "CSII"                       },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const int         DI      = DATA[i].d_di;
                const char        SPEC2   = DATA[i].d_spec2_p;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Element    y;  getValue(SPEC2, &y, 0);
                Obj        mEXP;  gg(&mEXP, EXPSPEC);
                const Obj& EXP = mEXP;

                UnsignedElement    uy;  getValue(SPEC2, &uy, 0);
                UnsignedObj        mUEXP;  gg(&mUEXP, EXPSPEC);
                const UnsignedObj& UEXP = mUEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC1 = (bsl::string()
                                         + INIT[j].d_spec_p
                                         + DATA[i].d_spec1_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC1.c_str());
                        const Obj& X = mX;
                        mX.replace(DI, y);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        UnsignedObj        mUX(&sa);  gg(&mUX, SPEC1.c_str());
                        const UnsignedObj& UX = mUX;
                        mUX.replace(DI, uy);
                        LOOP_ASSERT(LINE, UX == UEXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nSub-array replace." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec1_p;      // specification 1
                int         d_di;           // destination index
                const char *d_spec2_p;      // specification 2
                int         d_si;           // source index
                int         d_ne;           // number of elements
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec1     di  spec2     si  ne  exp
                //----  --------  --  --------  --  --  --------------------
                { L_,   "C",       0, "",        0,  0, "C"                  },

                { L_,   "CS",      0, "CSIL",    0,  1, "CS"                 },
                { L_,   "CS",      0, "CSIL",    1,  1, "SS"                 },
                { L_,   "CS",      0, "CSIL",    2,  1, "IS"                 },
                { L_,   "CS",      0, "CSIL",    3,  1, "LS"                 },

                { L_,   "CS",      0, "CSIL",    0,  2, "CS"                 },
                { L_,   "CS",      0, "CSIL",    1,  2, "SI"                 },
                { L_,   "CS",      0, "CSIL",    2,  2, "IL"                 },

                { L_,   "CS",      1, "CSIL",    0,  1, "CC"                 },
                { L_,   "CS",      1, "CSIL",    1,  1, "CS"                 },
                { L_,   "CS",      1, "CSIL",    2,  1, "CI"                 },
                { L_,   "CS",      1, "CSIL",    3,  1, "CL"                 },

                { L_,   "CSIL",    0, "CSIL",    0,  1, "CSIL"               },
                { L_,   "CSIL",    0, "CSIL",    1,  1, "SSIL"               },
                { L_,   "CSIL",    0, "CSIL",    2,  1, "ISIL"               },
                { L_,   "CSIL",    0, "CSIL",    3,  1, "LSIL"               },

                { L_,   "CSIL",    0, "CSIL",    0,  3, "CSIL"               },
                { L_,   "CSIL",    0, "CSIL",    1,  3, "SILL"               },

                { L_,   "CSIL",    1, "CSIL",    0,  3, "CCSI"               },
                { L_,   "CSIL",    1, "CSIL",    1,  3, "CSIL"               },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const char *const SPEC2   = DATA[i].d_spec2_p;
                const int         DI      = DATA[i].d_di;
                const int         SI      = DATA[i].d_si;
                const int         NE      = DATA[i].d_ne;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj        mY;    gg(&mY,   SPEC2);
                const Obj& Y = mY;
                Obj        mEXP;  gg(&mEXP, EXPSPEC);
                const Obj& EXP = mEXP;

                UnsignedObj        mUY;    gg(&mUY,   SPEC2);
                const UnsignedObj& UY = mUY;
                UnsignedObj        mUEXP;  gg(&mUEXP, EXPSPEC);
                const UnsignedObj& UEXP = mUEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC1 = (bsl::string()
                                         + INIT[j].d_spec_p
                                         + DATA[i].d_spec1_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC1.c_str());
                        const Obj& X = mX;
                        mX.replace(DI, Y, SI, NE);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        UnsignedObj        mUX(&sa);  gg(&mUX, SPEC1.c_str());
                        const UnsignedObj& UX = mUX;
                        mUX.replace(DI, UY, SI, NE);
                        LOOP_ASSERT(LINE, UX == UEXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nAliased sub-array replace." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec_p;       // specification
                int         d_di;           // destination index
                int         d_si;           // source index
                int         d_ne;           // number of elements
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec      di  si  ne  exp
                //----  --------  --  --  --  ----------------------
                { L_,   "C",       0,  0,  0, "C"                    },
                { L_,   "CS",      0,  0,  0, "CS"                   },

                { L_,   "CSIL",    0,  0,  2, "CSIL"                 },
                { L_,   "CSIL",    0,  1,  2, "SIIL"                 },
                { L_,   "CSIL",    0,  2,  2, "ILIL"                 },

                { L_,   "CSIL",    2,  0,  2, "CSCS"                 },
                { L_,   "CSIL",    2,  1,  2, "CSSI"                 },
                { L_,   "CSIL",    2,  2,  2, "CSIL"                 },

                { L_,   "CSIL",    3,  0,  1, "CSIC"                 },
                { L_,   "CSIL",    3,  1,  1, "CSIS"                 },
                { L_,   "CSIL",    3,  2,  1, "CSII"                 },
                { L_,   "CSIL",    3,  3,  1, "CSIL"                 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const int         DI      = DATA[i].d_di;
                const int         SI      = DATA[i].d_si;
                const int         NE      = DATA[i].d_ne;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj mEXP;  gg(&mEXP, EXPSPEC);  const Obj& EXP = mEXP;

                UnsignedObj        mUEXP;  gg(&mUEXP, EXPSPEC);
                const UnsignedObj& UEXP = mUEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC = (bsl::string()
                                        + INIT[j].d_spec_p
                                        + DATA[i].d_spec_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC.c_str());
                        const Obj& X = mX;
                        mX.replace(DI, X, SI, NE);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        UnsignedObj        mUX(&sa);  gg(&mUX, SPEC.c_str());
                        const UnsignedObj& UX = mUX;
                        mUX.replace(DI, UX, SI, NE);
                        LOOP_ASSERT(LINE, UX == UEXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj        mX;  gg(&mX, "CSIL");
            Obj        mY;  gg(&mY, "CC");
            const Obj& Y = mY;

            UnsignedObj        mUX;  gg(&mUX, "CSIL");
            UnsignedObj        mUY;  gg(&mUY, "CC");
            const UnsignedObj& UY = mUY;

            ASSERT_SAFE_FAIL(mX.replace(  4,  0));

            ASSERT_SAFE_FAIL(mX.replace(  4,  Y, 0, 1));

            ASSERT_SAFE_PASS(mX.replace(  0,  0));

            ASSERT_SAFE_PASS(mX.replace(  0,  Y, 0, 0));
            ASSERT_SAFE_PASS(mX.replace(  0,  Y, 0, 1));
            ASSERT_SAFE_PASS(mX.replace(  0,  Y, 0, 2));
            ASSERT_SAFE_FAIL(mX.replace(  0,  Y, 0, 3));
            ASSERT_SAFE_PASS(mX.replace(  0,  Y, 1, 0));
            ASSERT_SAFE_PASS(mX.replace(  0,  Y, 1, 1));
            ASSERT_SAFE_FAIL(mX.replace(  0,  Y, 1, 2));
            ASSERT_SAFE_PASS(mX.replace(  0,  Y, 2, 0));
            ASSERT_SAFE_FAIL(mX.replace(  0,  Y, 2, 1));
            ASSERT_SAFE_FAIL(mX.replace(  0,  Y, 3, 0));

            ASSERT_SAFE_FAIL(mUX.replace( 4, 0));

            ASSERT_SAFE_FAIL(mUX.replace( 4, UY, 0, 1));

            ASSERT_SAFE_PASS(mUX.replace( 0, 0));

            ASSERT_SAFE_PASS(mUX.replace( 0, UY, 0, 0));
            ASSERT_SAFE_PASS(mUX.replace( 0, UY, 0, 1));
            ASSERT_SAFE_PASS(mUX.replace( 0, UY, 0, 2));
            ASSERT_SAFE_FAIL(mUX.replace( 0, UY, 0, 3));
            ASSERT_SAFE_PASS(mUX.replace( 0, UY, 1, 0));
            ASSERT_SAFE_PASS(mUX.replace( 0, UY, 1, 1));
            ASSERT_SAFE_FAIL(mUX.replace( 0, UY, 1, 2));
            ASSERT_SAFE_PASS(mUX.replace( 0, UY, 2, 0));
            ASSERT_SAFE_FAIL(mUX.replace( 0, UY, 2, 1));
            ASSERT_SAFE_FAIL(mUX.replace( 0, UY, 3, 0));
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // REMOVE
        //   The 'remove' methods operate as expected.
        //
        // Concerns:
        //: 1 The methods produce the expected value.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Specify a set of specifications for the 'gg' function that result
        //:   in an empty object but with different capacity and bytes per
        //:   element characteristics.
        //:
        //: 2 Use the table-based approach to specify a set of test vectors.
        //:
        //: 3 For every item in the cross-product of these two sets, verify
        //:   the result for the method (the set from 2 is applied to only
        //:   the initial object).  (C-1)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   void remove(di);
        //   void remove(di, ne);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "REMOVE" << endl
                          << "======" << endl;

        static const struct {
            int         d_lineNum;
            const char *d_spec_p;
        } INIT[] = {
            // line spec
            //----  ----
            { L_,   ""              },
            { L_,   "Lp"            },
            { L_,   "LLLLLLLLLLx"   },
            { L_,   "LLLLLLLLLLxLp" },
        };
        const int NUM_INIT = static_cast<int>(sizeof INIT / sizeof *INIT);

        if (verbose) cout << "\nValue remove." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec_p;       // specification
                int         d_di;           // destination index
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec      di  exp
                //----  --------  --  ----------------------------
                { L_,   "C",       0, ""                            },

                { L_,   "CS",      0, "S"                           },
                { L_,   "CS",      1, "C"                           },

                { L_,   "CSIL",    0, "SIL"                         },
                { L_,   "CSIL",    1, "CIL"                         },
                { L_,   "CSIL",    2, "CSL"                         },
                { L_,   "CSIL",    3, "CSI"                         },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const int         DI      = DATA[i].d_di;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj mEXP;  gg(&mEXP, EXPSPEC);  const Obj& EXP = mEXP;

                UnsignedObj        mUEXP;  gg(&mUEXP, EXPSPEC);
                const UnsignedObj& UEXP = mUEXP;

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC = (bsl::string()
                                        + INIT[j].d_spec_p
                                        + DATA[i].d_spec_p);

                    if (veryVerbose) { P(SPEC) }

                    Obj        mX;           gg(&mX, SPEC.c_str());
                    const Obj& X = mX;
                    mX.remove(DI);
                    LOOP_ASSERT(LINE, X == EXP);

                    UnsignedObj        mUX;  gg(&mUX, SPEC.c_str());
                    const UnsignedObj& UX = mUX;
                    mUX.remove(DI);
                    LOOP_ASSERT(LINE, UX == UEXP);
                }
            }
        }

        if (verbose) cout << "\nSub-array remove." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec_p;       // specification
                int         d_di;           // destination index
                int         d_ne;           // number of elements
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec      di  ne  exp
                //----  --------  --  --  ----------------------
                { L_,   "C",       0,  0, "C"                    },
                { L_,   "CS",      0,  0, "CS"                   },

                { L_,   "CSIL",    0,  1, "SIL"                  },
                { L_,   "CSIL",    1,  1, "CIL"                  },
                { L_,   "CSIL",    2,  1, "CSL"                  },
                { L_,   "CSIL",    3,  1, "CSI"                  },

                { L_,   "CSIL",    0,  2, "IL"                   },
                { L_,   "CSIL",    1,  2, "CL"                   },
                { L_,   "CSIL",    2,  2, "CS"                   },

                { L_,   "CSIL",    0,  3, "L"                    },
                { L_,   "CSIL",    1,  3, "C"                    },

                { L_,   "CSIL",    0,  4, ""                     },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const int         DI      = DATA[i].d_di;
                const int         NE      = DATA[i].d_ne;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj mEXP;  gg(&mEXP, EXPSPEC);  const Obj& EXP = mEXP;

                UnsignedObj        mUEXP;  gg(&mUEXP, EXPSPEC);
                const UnsignedObj& UEXP = mUEXP;

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC = (bsl::string()
                                        + INIT[j].d_spec_p
                                        + DATA[i].d_spec_p);

                    if (veryVerbose) { P(SPEC) }

                    Obj        mX;           gg(&mX, SPEC.c_str());
                    const Obj& X = mX;
                    mX.remove(DI, NE);
                    LOOP_ASSERT(LINE, X == EXP);

                    UnsignedObj        mUX;  gg(&mUX, SPEC.c_str());
                    const UnsignedObj& UX = mUX;
                    mUX.remove(DI, NE);
                    LOOP_ASSERT(LINE, UX == UEXP);
                }
            }
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj         mX;  gg(&mX, "CSI");
            UnsignedObj mUX;  gg(&mUX, "CSI");

            Obj         mT;
            UnsignedObj mUT;

            mT = mX;  ASSERT_FAIL(mT.remove(4));

            mT = mX;  ASSERT_SAFE_PASS(mT.remove(0, 0));
            mT = mX;  ASSERT_SAFE_PASS(mT.remove(0, 1));
            mT = mX;  ASSERT_SAFE_PASS(mT.remove(0, 2));
            mT = mX;  ASSERT_SAFE_PASS(mT.remove(0, 3));
            mT = mX;  ASSERT_SAFE_FAIL(mT.remove(0, 4));
            mT = mX;  ASSERT_SAFE_PASS(mT.remove(1, 0));
            mT = mX;  ASSERT_SAFE_PASS(mT.remove(1, 1));
            mT = mX;  ASSERT_SAFE_PASS(mT.remove(1, 2));
            mT = mX;  ASSERT_SAFE_FAIL(mT.remove(1, 3));
            mT = mX;  ASSERT_SAFE_PASS(mT.remove(2, 0));
            mT = mX;  ASSERT_SAFE_PASS(mT.remove(2, 1));
            mT = mX;  ASSERT_SAFE_FAIL(mT.remove(2, 2));
            mT = mX;  ASSERT_SAFE_PASS(mT.remove(3, 0));
            mT = mX;  ASSERT_SAFE_FAIL(mT.remove(3, 1));
            mT = mX;  ASSERT_SAFE_FAIL(mT.remove(4, 0));

            mUT = mUX;  ASSERT_FAIL(mUT.remove(4));

            mUT = mUX;  ASSERT_SAFE_PASS(mUT.remove(0, 0));
            mUT = mUX;  ASSERT_SAFE_PASS(mUT.remove(0, 1));
            mUT = mUX;  ASSERT_SAFE_PASS(mUT.remove(0, 2));
            mUT = mUX;  ASSERT_SAFE_PASS(mUT.remove(0, 3));
            mUT = mUX;  ASSERT_SAFE_FAIL(mUT.remove(0, 4));
            mUT = mUX;  ASSERT_SAFE_PASS(mUT.remove(1, 0));
            mUT = mUX;  ASSERT_SAFE_PASS(mUT.remove(1, 1));
            mUT = mUX;  ASSERT_SAFE_PASS(mUT.remove(1, 2));
            mUT = mUX;  ASSERT_SAFE_FAIL(mUT.remove(1, 3));
            mUT = mUX;  ASSERT_SAFE_PASS(mUT.remove(2, 0));
            mUT = mUX;  ASSERT_SAFE_PASS(mUT.remove(2, 1));
            mUT = mUX;  ASSERT_SAFE_FAIL(mUT.remove(2, 2));
            mUT = mUX;  ASSERT_SAFE_PASS(mUT.remove(3, 0));
            mUT = mUX;  ASSERT_SAFE_FAIL(mUT.remove(3, 1));
            mUT = mUX;  ASSERT_SAFE_FAIL(mUT.remove(4, 0));
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // INSERT
        //   The 'insert' methods operate as expected.
        //
        // Concerns:
        //: 1 The methods are exception neutral with respect to memory
        //:   allocation.
        //:
        //: 2 The methods produce the expected value.
        //:
        //: 3 The methods are alias safe.
        //:
        //: 4 When there is initially sufficient storage in the result variable
        //:   to store the result, the methods produce the expected value.
        //:
        //: 5 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Wrap all tests with the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
        //:   macros to ensure exception neutrality with respect to memory
        //:   allocation.  (C-1)
        //:
        //: 2 Specify a set of specifications for the 'gg' function that result
        //:   in an empty object but with different capacity and bytes per
        //:   element characteristics.
        //:
        //: 3 Use the table-based approach to specify a set of test vectors.
        //:
        //: 4 For every item in the cross-product of these two sets, verify
        //:   the result for the method (the set from 2 is applied to only
        //:   the initial object).  (C-2..4)
        //:
        //: 5 Verify defensive checks are triggered for invalid values.  (C-5)
        //
        // Testing:
        //   void insert(di, value);
        //   void insert(di, const PackedIntArray& srcArray);
        //   void insert(di, const PackedIntArray& srcArray, si, ne);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "INSERT" << endl
                          << "======" << endl;

        static const struct {
            int         d_lineNum;
            const char *d_spec_p;
        } INIT[] = {
            // line spec
            //----  ----
            { L_,   ""              },
            { L_,   "Lp"            },
            { L_,   "LLLLLLLLLLx"   },
            { L_,   "LLLLLLLLLLxLp" },
        };
        const int NUM_INIT = static_cast<int>(sizeof INIT / sizeof *INIT);

        if (verbose) cout << "\nValue insert." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec1_p;      // specification 1
                int         d_di;           // destination index
                const char  d_spec2_p;      // specification 2
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec1     di  spec2     exp
                //----  --------  --  --------  ----------------------------
                { L_,   "",        0, 'C',      "C"                          },

                { L_,   "C",       0, 'S',      "SC"                         },
                { L_,   "C",       1, 'S',      "CS"                         },

                { L_,   "CS",      0, 'I',      "ICS"                        },
                { L_,   "CS",      1, 'I',      "CIS"                        },
                { L_,   "CS",      2, 'I',      "CSI"                        },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const int         DI      = DATA[i].d_di;
                const char        SPEC2   = DATA[i].d_spec2_p;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Element    y;     getValue(SPEC2, &y, 0);
                Obj        mEXP;  gg(&mEXP, EXPSPEC);
                const Obj& EXP =  mEXP;

                UnsignedElement    uy;     getValue(SPEC2, &uy, 0);
                UnsignedObj        mUEXP;  gg(&mUEXP, EXPSPEC);
                const UnsignedObj& UEXP =  mUEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC1 = (bsl::string()
                                         + INIT[j].d_spec_p
                                         + DATA[i].d_spec1_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC1.c_str());
                        const Obj& X = mX;
                        mX.insert(DI, y);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        UnsignedObj        mUX(&sa);  gg(&mUX, SPEC1.c_str());
                        const UnsignedObj& UX = mUX;
                        mUX.insert(DI, uy);
                        LOOP_ASSERT(LINE, UX == UEXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nFull array insert." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec1_p;      // specification 1
                int         d_di;           // destination index
                const char *d_spec2_p;      // specification 2
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec1     di  spec2     exp
                //----  --------  --  --------  ----------------------------
                { L_,   "",        0, "",       ""                           },

                { L_,   "",        0, "C",      "C"                          },
                { L_,   "C",       1, "",       "C"                          },

                { L_,   "",        0, "CS",     "CS"                         },
                { L_,   "C",       1, "S",      "CS"                         },
                { L_,   "CS",      2, "",       "CS"                         },

                { L_,   "",        0, "CSI",    "CSI"                        },
                { L_,   "C",       1, "SI",     "CSI"                        },
                { L_,   "CS",      2, "I",      "CSI"                        },
                { L_,   "CSI",     3, "",       "CSI"                        },

                { L_,   "",        0, "CSIL",   "CSIL"                       },
                { L_,   "C",       1, "SIL",    "CSIL"                       },
                { L_,   "CS",      2, "IL",     "CSIL"                       },
                { L_,   "CSI",     3, "L",      "CSIL"                       },
                { L_,   "CSIL",    4, "",       "CSIL"                       },

                { L_,   "CS",      0, "CSIL",   "CSILCS"                     },
                { L_,   "CS",      1, "CSIL",   "CCSILS"                     },
                { L_,   "CS",      2, "CSIL",   "CSCSIL"                     },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const int         DI      = DATA[i].d_di;
                const char *const SPEC2   = DATA[i].d_spec2_p;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj        mY;    gg(&mY,   SPEC2);
                const Obj& Y = mY;
                Obj        mEXP;  gg(&mEXP, EXPSPEC);
                const Obj& EXP = mEXP;

                UnsignedObj        mUY;    gg(&mUY,   SPEC2);
                const UnsignedObj& UY = mUY;
                UnsignedObj        mUEXP;  gg(&mUEXP, EXPSPEC);
                const UnsignedObj& UEXP = mUEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC1 = (bsl::string()
                                         + INIT[j].d_spec_p
                                         + DATA[i].d_spec1_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC1.c_str());
                        const Obj& X = mX;
                        mX.insert(DI, Y);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        UnsignedObj        mUX(&sa);  gg(&mUX, SPEC1.c_str());
                        const UnsignedObj& UX = mUX;
                        mUX.insert(DI, UY);
                        LOOP_ASSERT(LINE, UX == UEXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nSub-array insert." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec1_p;      // specification 1
                int         d_di;           // destination index
                const char *d_spec2_p;      // specification 2
                int         d_si;           // source index
                int         d_ne;           // number of elements
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec1     di  spec2     si  ne  exp
                //----  --------  --  --------  --  --  --------------------
                { L_,   "",        0, "",        0,  0, ""                   },
                { L_,   "C",       1, "",        0,  0, "C"                  },
                { L_,   "CS",      2, "",        0,  0, "CS"                 },

                { L_,   "",        0, "CSIL",    0,  0, ""                   },
                { L_,   "",        0, "CSIL",    0,  1, "C"                  },
                { L_,   "",        0, "CSIL",    0,  2, "CS"                 },
                { L_,   "",        0, "CSIL",    0,  3, "CSI"                },
                { L_,   "",        0, "CSIL",    0,  4, "CSIL"               },

                { L_,   "CS",      0, "CSIL",    1,  2, "SICS"               },
                { L_,   "CS",      0, "CSIL",    2,  2, "ILCS"               },

                { L_,   "CS",      1, "CSIL",    1,  2, "CSIS"               },
                { L_,   "CS",      1, "CSIL",    2,  2, "CILS"               },

                { L_,   "CS",      2, "CSIL",    1,  2, "CSSI"               },
                { L_,   "CS",      2, "CSIL",    2,  2, "CSIL"               },

                { L_,   "CS",      2, "CSIL",    3,  0, "CS"                 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const char *const SPEC2   = DATA[i].d_spec2_p;
                const int         DI      = DATA[i].d_di;
                const int         SI      = DATA[i].d_si;
                const int         NE      = DATA[i].d_ne;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj        mY;    gg(&mY,   SPEC2);
                const Obj& Y = mY;
                Obj        mEXP;  gg(&mEXP, EXPSPEC);
                const Obj& EXP = mEXP;

                UnsignedObj        mUY;    gg(&mUY,   SPEC2);
                const UnsignedObj& UY = mUY;
                UnsignedObj        mUEXP;  gg(&mUEXP, EXPSPEC);
                const UnsignedObj& UEXP = mUEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC1 = (bsl::string()
                                         + INIT[j].d_spec_p
                                         + DATA[i].d_spec1_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC1.c_str());
                        const Obj& X = mX;
                        mX.insert(DI, Y, SI, NE);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        UnsignedObj        mUX(&sa);  gg(&mUX, SPEC1.c_str());
                        const UnsignedObj& UX = mUX;
                        mUX.insert(DI, UY, SI, NE);
                        LOOP_ASSERT(LINE, UX == UEXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nAliased full array insert." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec_p;       // specification
                int         d_di;           // destination index
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec      di  exp
                //----  --------  --  ------------------------------
                { L_,   "",        0, ""                              },

                { L_,   "CSIL",    0, "CSILCSIL"                      },
                { L_,   "CSIL",    1, "CCSILSIL"                      },
                { L_,   "CSIL",    2, "CSCSILIL"                      },
                { L_,   "CSIL",    3, "CSICSILL"                      },
                { L_,   "CSIL",    4, "CSILCSIL"                      },

                { L_,   "C",       1, "CC"                            },
                { L_,   "CS",      2, "CSCS"                          },
                { L_,   "CSI",     3, "CSICSI"                        },
                { L_,   "CSIL",    4, "CSILCSIL"                      },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const int         DI      = DATA[i].d_di;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj mEXP;  gg(&mEXP, EXPSPEC);  const Obj& EXP = mEXP;

                UnsignedObj        mUEXP;  gg(&mUEXP, EXPSPEC);
                const UnsignedObj& UEXP = mUEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC = (bsl::string()
                                        + INIT[j].d_spec_p
                                        + DATA[i].d_spec_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC.c_str());
                        const Obj& X = mX;
                        mX.insert(DI, X);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        UnsignedObj        mUX(&sa);  gg(&mUX, SPEC.c_str());
                        const UnsignedObj& UX = mUX;
                        mUX.insert(DI, UX);
                        LOOP_ASSERT(LINE, UX == UEXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nAliased sub-array insert." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec_p;       // specification
                int         d_di;           // destination index
                int         d_si;           // source index
                int         d_ne;           // number of elements
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec      di  si  ne  exp
                //----  --------  --  --  --  ----------------------
                { L_,   "",        0,  0,  0, ""                     },
                { L_,   "C",       0,  0,  0, "C"                    },
                { L_,   "CS",      0,  0,  0, "CS"                   },

                { L_,   "CSIL",    0,  0,  2, "CSCSIL"               },
                { L_,   "CSIL",    0,  1,  2, "SICSIL"               },
                { L_,   "CSIL",    0,  2,  2, "ILCSIL"               },

                { L_,   "CSIL",    2,  0,  2, "CSCSIL"               },
                { L_,   "CSIL",    2,  1,  2, "CSSIIL"               },
                { L_,   "CSIL",    2,  2,  2, "CSILIL"               },

                { L_,   "CSIL",    4,  0,  1, "CSILC"                },
                { L_,   "CSIL",    4,  1,  1, "CSILS"                },
                { L_,   "CSIL",    4,  2,  1, "CSILI"                },
                { L_,   "CSIL",    4,  3,  1, "CSILL"                },

                { L_,   "CSIL",    4,  0,  2, "CSILCS"               },
                { L_,   "CSIL",    4,  1,  2, "CSILSI"               },
                { L_,   "CSIL",    4,  2,  2, "CSILIL"               },

                { L_,   "CSIL",    4,  0,  3, "CSILCSI"              },
                { L_,   "CSIL",    4,  1,  3, "CSILSIL"              },

                { L_,   "CSIL",    4,  0,  4, "CSILCSIL"             },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const int         DI      = DATA[i].d_di;
                const int         SI      = DATA[i].d_si;
                const int         NE      = DATA[i].d_ne;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj mEXP;  gg(&mEXP, EXPSPEC);  const Obj& EXP = mEXP;

                UnsignedObj        mUEXP;  gg(&mUEXP, EXPSPEC);
                const UnsignedObj& UEXP = mUEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC = (bsl::string()
                                        + INIT[j].d_spec_p
                                        + DATA[i].d_spec_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC.c_str());
                        const Obj& X = mX;
                        mX.insert(DI, X, SI, NE);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        UnsignedObj        mUX(&sa);  gg(&mUX, SPEC.c_str());
                        const UnsignedObj& UX = mUX;
                        mUX.insert(DI, UX, SI, NE);
                        LOOP_ASSERT(LINE, UX == UEXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj        mX;
            Obj        mY;  gg(&mY, "CC");
            const Obj& Y =  mY;

            UnsignedObj        mUX;
            UnsignedObj        mUY;  gg(&mUY, "CC");
            const UnsignedObj& UY = mUY;

            ASSERT_SAFE_FAIL(mX.insert(  1,  0));

            ASSERT_SAFE_FAIL(mX.insert(  1,  Y));

            ASSERT_SAFE_FAIL(mX.insert(  1,  Y, 0, 0));

            ASSERT_SAFE_PASS(mX.insert(  0,  0));

            ASSERT_SAFE_PASS(mX.insert(  0,  Y));

            ASSERT_SAFE_PASS(mX.insert(  0,  Y, 0, 0));
            ASSERT_SAFE_PASS(mX.insert(  0,  Y, 0, 1));
            ASSERT_SAFE_PASS(mX.insert(  0,  Y, 0, 2));
            ASSERT_SAFE_FAIL(mX.insert(  0,  Y, 0, 3));
            ASSERT_SAFE_PASS(mX.insert(  0,  Y, 1, 0));
            ASSERT_SAFE_PASS(mX.insert(  0,  Y, 1, 1));
            ASSERT_SAFE_FAIL(mX.insert(  0,  Y, 1, 2));
            ASSERT_SAFE_PASS(mX.insert(  0,  Y, 2, 0));
            ASSERT_SAFE_FAIL(mX.insert(  0,  Y, 2, 1));
            ASSERT_SAFE_FAIL(mX.insert(  0,  Y, 3, 0));

            ASSERT_SAFE_FAIL(mUX.insert( 1, 0));

            ASSERT_SAFE_FAIL(mUX.insert( 1, UY));

            ASSERT_SAFE_FAIL(mUX.insert( 1, UY, 0, 0));

            ASSERT_SAFE_PASS(mUX.insert( 0, 0));

            ASSERT_SAFE_PASS(mUX.insert( 0, UY));

            ASSERT_SAFE_PASS(mUX.insert( 0, UY, 0, 0));
            ASSERT_SAFE_PASS(mUX.insert( 0, UY, 0, 1));
            ASSERT_SAFE_PASS(mUX.insert( 0, UY, 0, 2));
            ASSERT_SAFE_FAIL(mUX.insert( 0, UY, 0, 3));
            ASSERT_SAFE_PASS(mUX.insert( 0, UY, 1, 0));
            ASSERT_SAFE_PASS(mUX.insert( 0, UY, 1, 1));
            ASSERT_SAFE_FAIL(mUX.insert( 0, UY, 1, 2));
            ASSERT_SAFE_PASS(mUX.insert( 0, UY, 2, 0));
            ASSERT_SAFE_FAIL(mUX.insert( 0, UY, 2, 1));
            ASSERT_SAFE_FAIL(mUX.insert( 0, UY, 3, 0));
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // APPEND
        //   The 'append' methods operate as expected.
        //
        // Concerns:
        //: 1 The methods are exception neutral with respect to memory
        //:   allocation.
        //:
        //: 2 The methods produce the expected value.
        //:
        //: 3 The methods are alias safe.
        //:
        //: 4 When there is initially sufficient storage in the result variable
        //:   to store the result, the methods produce the expected value.
        //:
        //: 5 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Wrap all tests with the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
        //:   macros to ensure exception neutrality with respect to memory
        //:   allocation.  (C-1)
        //:
        //: 2 Specify a set of specifications for the 'gg' function that result
        //:   in an empty object but with different capacity and bytes per
        //:   element characteristics.
        //:
        //: 3 Use the table-based approach to specify a set of test vectors.
        //:
        //: 4 For every item in the cross-product of these two sets, verify
        //:   the result for the method (the set from 2 is applied to only
        //:   the initial object).  (C-2..4)
        //:
        //: 5 Verify defensive checks are triggered for invalid values.  (C-5)
        //
        // Testing:
        //   void push_back(TYPE value);
        //   void append(const PackedIntArray& srcArray);
        //   void append(const PackedIntArray& srcArray, si, ne);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "APPEND" << endl
                          << "======" << endl;

        static const struct {
            int         d_lineNum;
            const char *d_spec_p;
        } INIT[] = {
            // line spec
            //----  ----
            { L_,   ""              },
            { L_,   "Lp"            },
            { L_,   "LLLLLLLLLLx"   },
            { L_,   "LLLLLLLLLLxLp" },
        };
        const int NUM_INIT = static_cast<int>(sizeof INIT / sizeof *INIT);

        if (verbose) cout << "\nValue append ('push_back')." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec1_p;      // specification 1
                const char  d_spec2_p;      // specification 2
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec1     spec2     exp
                //----  --------  --------  ----------------------------
                { L_,   "",       'C',      "C"                           },
                { L_,   "",       'S',      "S"                           },
                { L_,   "",       'I',      "I"                           },
                { L_,   "",       'L',      "L"                           },

                { L_,   "C",      'C',      "CC"                          },
                { L_,   "C",      'S',      "CS"                          },
                { L_,   "C",      'I',      "CI"                          },
                { L_,   "C",      'L',      "CL"                          },

                { L_,   "I",      'C',      "IC"                          },
                { L_,   "I",      'S',      "IS"                          },
                { L_,   "I",      'I',      "II"                          },
                { L_,   "I",      'L',      "IL"                          },

                { L_,   "CS",     'C',      "CSC"                         },
                { L_,   "CS",     'S',      "CSS"                         },
                { L_,   "CS",     'I',      "CSI"                         },
                { L_,   "CS",     'L',      "CSL"                         },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const char        SPEC2   = DATA[i].d_spec2_p;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Element y;  getValue(SPEC2, &y, 0);

                Obj mEXP;  gg(&mEXP, EXPSPEC);  const Obj& EXP = mEXP;

                UnsignedElement    uy;  getValue(SPEC2, &uy, 0);
                UnsignedObj        mUEXP;  gg(&mUEXP, EXPSPEC);
                const UnsignedObj& UEXP = mUEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC1 = (bsl::string()
                                         + INIT[j].d_spec_p
                                         + DATA[i].d_spec1_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC1.c_str());
                        const Obj& X = mX;
                        mX.push_back(y);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        UnsignedObj        mUX(&sa);  gg(&mUX, SPEC1.c_str());
                        const UnsignedObj& UX = mUX;
                        mUX.push_back(uy);
                        LOOP_ASSERT(LINE, UX == UEXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nFull array append." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec1_p;      // specification 1
                const char *d_spec2_p;      // specification 2
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec1     spec2     exp
                //----  --------  --------  ------------------------------
                { L_,   "",       "",       ""                              },

                { L_,   "",       "C",      "C"                             },
                { L_,   "C",      "",       "C"                             },

                { L_,   "",       "CS",     "CS"                            },
                { L_,   "C",      "S",      "CS"                            },
                { L_,   "CS",     "",       "CS"                            },

                { L_,   "",       "CSI",    "CSI"                           },
                { L_,   "C",      "SI",     "CSI"                           },
                { L_,   "CS",     "I",      "CSI"                           },
                { L_,   "CSI",    "",       "CSI"                           },

                { L_,   "",       "CSIL",   "CSIL"                          },
                { L_,   "C",      "SIL",    "CSIL"                          },
                { L_,   "CS",     "IL",     "CSIL"                          },
                { L_,   "CSI",    "L",      "CSIL"                          },
                { L_,   "CSIL",   "",       "CSIL"                          },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const char *const SPEC2   = DATA[i].d_spec2_p;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj        mY;    gg(&mY,   SPEC2);
                const Obj& Y = mY;
                Obj        mEXP;  gg(&mEXP, EXPSPEC);
                const Obj& EXP = mEXP;

                UnsignedObj        mUY;    gg(&mUY,   SPEC2);
                const UnsignedObj& UY = mUY;
                UnsignedObj        mUEXP;  gg(&mUEXP, EXPSPEC);
                const UnsignedObj& UEXP = mUEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC1 = (bsl::string()
                                         + INIT[j].d_spec_p
                                         + DATA[i].d_spec1_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC1.c_str());
                        const Obj& X = mX;

                        mX.append(Y);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END


                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        UnsignedObj        mUX(&sa);  gg(&mUX, SPEC1.c_str());
                        const UnsignedObj& UX = mUX;
                        mUX.append(UY);
                        LOOP_ASSERT(LINE, UX == UEXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nSub-array append." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec1_p;      // specification 1
                const char *d_spec2_p;      // specification 2
                int         d_si;           // source index
                int         d_ne;           // number of elements
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec1     spec2     si  ne  exp
                //----  --------  --------  --  --  ----------------------
                { L_,   "",       "",        0,  0, ""                     },
                { L_,   "C",      "",        0,  0, "C"                    },
                { L_,   "CS",     "",        0,  0, "CS"                   },

                { L_,   "",       "CSIL",    0,  0, ""                     },
                { L_,   "",       "CSIL",    0,  1, "C"                    },
                { L_,   "",       "CSIL",    0,  2, "CS"                   },
                { L_,   "",       "CSIL",    0,  3, "CSI"                  },
                { L_,   "",       "CSIL",    0,  4, "CSIL"                 },

                { L_,   "CS",     "CSIL",    1,  2, "CSSI"                 },
                { L_,   "CS",     "CSIL",    2,  2, "CSIL"                 },

                { L_,   "CS",     "CSIL",    3,  0, "CS"                   },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const char *const SPEC2   = DATA[i].d_spec2_p;
                const int         SI      = DATA[i].d_si;
                const int         NE      = DATA[i].d_ne;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj        mY;    gg(&mY,   SPEC2);
                const Obj& Y = mY;
                Obj        mEXP;  gg(&mEXP, EXPSPEC);
                const Obj& EXP = mEXP;

                UnsignedObj        mUY;    gg(&mUY,   SPEC2);
                const UnsignedObj& UY = mUY;
                UnsignedObj        mUEXP;  gg(&mUEXP, EXPSPEC);
                const UnsignedObj& UEXP = mUEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC1 = (bsl::string()
                                         + INIT[j].d_spec_p
                                         + DATA[i].d_spec1_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC1.c_str());
                        const Obj& X = mX;
                        mX.append(Y, SI, NE);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        UnsignedObj        mUX(&sa);  gg(&mUX, SPEC1.c_str());
                        const UnsignedObj& UX = mUX;
                        mUX.append(UY, SI, NE);
                        LOOP_ASSERT(LINE, UX == UEXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nAliased full array append." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec_p;       // specification
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec      exp
                //----  --------  ------------------------------
                { L_,   "",       ""                              },

                { L_,   "C",      "CC"                            },
                { L_,   "CS",     "CSCS"                          },
                { L_,   "CSI",    "CSICSI"                        },
                { L_,   "CSIL",   "CSILCSIL"                      },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj mEXP;  gg(&mEXP, EXPSPEC);  const Obj& EXP = mEXP;

                UnsignedObj        mUEXP;  gg(&mUEXP, EXPSPEC);
                const UnsignedObj& UEXP = mUEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC = (bsl::string()
                                        + INIT[j].d_spec_p
                                        + DATA[i].d_spec_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC.c_str());
                        const Obj& X = mX;
                        mX.append(X);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        UnsignedObj        mUX(&sa);  gg(&mUX, SPEC.c_str());
                        const UnsignedObj& UX = mUX;
                        mUX.append(UX);
                        LOOP_ASSERT(LINE, UX == UEXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nAliased sub-array append." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec_p;       // specification
                int         d_si;           // source index
                int         d_ne;           // number of elements
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec      si  ne  exp
                //----  --------  --  --  ----------------------
                { L_,   "",       0,  0, ""                     },
                { L_,   "C",      0,  0, "C"                    },
                { L_,   "CS",     0,  0, "CS"                   },

                { L_,   "CSIL",   0,  1, "CSILC"                },
                { L_,   "CSIL",   1,  1, "CSILS"                },
                { L_,   "CSIL",   2,  1, "CSILI"                },
                { L_,   "CSIL",   3,  1, "CSILL"                },

                { L_,   "CSIL",   0,  2, "CSILCS"               },
                { L_,   "CSIL",   1,  2, "CSILSI"               },
                { L_,   "CSIL",   2,  2, "CSILIL"               },

                { L_,   "CSIL",   0,  3, "CSILCSI"              },
                { L_,   "CSIL",   1,  3, "CSILSIL"              },

                { L_,   "CSIL",   0,  4, "CSILCSIL"             },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const int         SI      = DATA[i].d_si;
                const int         NE      = DATA[i].d_ne;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj mEXP;  gg(&mEXP, EXPSPEC);  const Obj& EXP = mEXP;

                UnsignedObj        mUEXP;  gg(&mUEXP, EXPSPEC);
                const UnsignedObj& UEXP = mUEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC = (bsl::string()
                                        + INIT[j].d_spec_p
                                        + DATA[i].d_spec_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC.c_str());
                        const Obj& X = mX;
                        mX.append(X, SI, NE);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        UnsignedObj        mUX(&sa);  gg(&mUX, SPEC.c_str());
                        const UnsignedObj& UX = mUX;
                        mUX.append(UX, SI, NE);
                        LOOP_ASSERT(LINE, UX == UEXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj        mX;
            Obj        mY;
            gg(&mY, "CC");
            const Obj& Y = mY;

            UnsignedObj        mUX;
            UnsignedObj        mUY;
            gg(&mUY, "CC");
            const UnsignedObj& UY = mUY;

            ASSERT_SAFE_PASS(mX.append(  Y, 0, 0));
            ASSERT_SAFE_PASS(mX.append(  Y, 0, 1));
            ASSERT_SAFE_PASS(mX.append(  Y, 0, 2));
            ASSERT_SAFE_FAIL(mX.append(  Y, 0, 3));
            ASSERT_SAFE_PASS(mX.append(  Y, 1, 0));
            ASSERT_SAFE_PASS(mX.append(  Y, 1, 1));
            ASSERT_SAFE_FAIL(mX.append(  Y, 1, 2));
            ASSERT_SAFE_PASS(mX.append(  Y, 2, 0));
            ASSERT_SAFE_FAIL(mX.append(  Y, 2, 1));
            ASSERT_SAFE_FAIL(mX.append(  Y, 3, 0));

            ASSERT_SAFE_PASS(mUX.append(UY, 0, 0));
            ASSERT_SAFE_PASS(mUX.append(UY, 0, 1));
            ASSERT_SAFE_PASS(mUX.append(UY, 0, 2));
            ASSERT_SAFE_FAIL(mUX.append(UY, 0, 3));
            ASSERT_SAFE_PASS(mUX.append(UY, 1, 0));
            ASSERT_SAFE_PASS(mUX.append(UY, 1, 1));
            ASSERT_SAFE_FAIL(mUX.append(UY, 1, 2));
            ASSERT_SAFE_PASS(mUX.append(UY, 2, 0));
            ASSERT_SAFE_FAIL(mUX.append(UY, 2, 1));
            ASSERT_SAFE_FAIL(mUX.append(UY, 3, 0));
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTION
        //   Ensure that, when member and free 'swap' are implemented, we can
        //   exchange the values of any two objects that use the same
        //   allocator.
        //
        // Concerns:
        //: 1 Both functions exchange the values of the (two) supplied objects.
        //:
        //: 2 The common object allocator address held by both objects is
        //:   unchanged.
        //:
        //: 3 The member function does not allocate memory from any allocator;
        //:   nor does the free function when the two objects being swapped use
        //:   the same allocator.
        //:
        //: 4 The free function can be called with two objects that use
        //:   different allocators.
        //:
        //: 5 Both functions have standard signatures and return types.
        //:
        //: 6 Using either function to swap an object with itself does not
        //:   affect the value of the object (alias-safety).
        //:
        //: 7 The free 'swap' function is discoverable through ADL (Argument
        //:   Dependent Lookup).
        //:
        //: 8 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the addresses of the 'swap' member and free functions defined
        //:   in this component to initialize, respectively, member-function
        //:   and free-function pointers having the appropriate signatures and
        //:   return types.  (C-5)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 4 For each row 'R1' in the table of P-3:  (C-1..2, 6)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor and 'oa' to create a modifiable
        //:     'Obj', 'mW', having the value described by 'R1'; also use the
        //:     copy constructor and a "scratch" allocator to create a 'const'
        //:     'Obj' 'XX' from 'mW'.
        //:
        //:   3 Use the member and free 'swap' functions to swap the value of
        //:     'mW' with itself; verify, after each swap, that:  (C-6)
        //:
        //:     1 The value is unchanged.  (C-6)
        //:
        //:     2 The allocator address held by the object is unchanged.
        //:
        //:     3 There was no additional object memory allocation.
        //:
        //:   4 For each row 'R2' in the table of P-3:  (C-1..2)
        //:
        //:     1 Use the copy constructor and 'oa' to create a modifiable
        //:       'Obj', 'mX', from 'XX' (P-4.2).
        //:
        //:     2 Use the value constructor and 'oa' to create a modifiable
        //:       'Obj', 'mY', and having the value described by 'R2'; also use
        //:       the copy constructor to create, using a "scratch" allocator,
        //:       a 'const' 'Obj', 'YY', from 'Y'.
        //:
        //:     3 Use, in turn, the member and free 'swap' functions to swap
        //:       the values of 'mX' and 'mY'; verify, after each swap, that:
        //:       (C-1..2)
        //:
        //:       1 The values have been exchanged.  (C-1)
        //:
        //:       2 The common object allocator address held by 'mX' and 'mY'
        //:         is unchanged in both objects.  (C-2)
        //:
        //:       3 There was no additional object memory allocation.
        //:
        //: 5 Verify that the free 'swap' function is discoverable through ADL:
        //:   (C-7)
        //:
        //:   1 Create a set of attribute values, 'A', distinct from the values
        //:     corresponding to the default-constructed object, choosing
        //:     values that allocate memory if possible.
        //:
        //:   2 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   3 Use the default constructor and 'oa' to create a modifiable
        //:     'Obj' 'mX' (having default attribute values); also use the copy
        //:     constructor and a "scratch" allocator to create a 'const' 'Obj'
        //:     'XX' from 'mX'.
        //:
        //:   4 Use the value constructor and 'oa' to create a modifiable 'Obj'
        //:     'mY' having the value described by the 'Ai' attributes; also
        //:     use the copy constructor and a "scratch" allocator to create a
        //:     'const' 'Obj' 'YY' from 'mY'.
        //:
        //:   5 Use the 'bslalg::SwapUtil' helper function template to swap the
        //:     values of 'mX' and 'mY', using the free 'swap' function defined
        //:     in this component, then verify that:  (C-7)
        //:
        //:     1 The values have been exchanged.
        //:
        //:     2 There was no additional object memory allocation.  (C-7)
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory was
        //:   allocated from the default allocator.  (C-3)
        //:
        //: 7 Verify that free 'swap' exchanges the values of any two objects
        //:   that use different allocators.  (C-4)
        //:
        //: 8 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when, using the member 'swap' function, an attempt is
        //:   made to swap objects that do not refer to the same allocator, but
        //:   not when the allocators are the same (using the
        //:   'BSLS_ASSERTTEST_*' macros).  (C-8)
        //
        // Testing:
        //   void swap(PackedIntArray& other);
        //   void swap(PackedIntArray& a, PackedIntArray& b);
        // --------------------------------------------------------------------


        if (verbose) cout << endl
                          << "SWAP MEMBER AND FREE FUNCTION" << endl
                          << "=============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each function to a variable." << endl;
        {
            using namespace bdlc;

            typedef void (Obj::*funcPtr)(Obj&);

            typedef void (*freeFuncPtr)(Obj&, Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     memberSwap = &Obj::swap;
            freeFuncPtr freeSwap   = swap;

            // Quash unused variable warnings.

            Obj a;
            Obj b;
            (a.*memberSwap)(b);
            freeSwap(a, b);
        }
        {
            using namespace bdlc;

            typedef void (UnsignedObj::*funcPtr)(UnsignedObj&);

            typedef void (*freeFuncPtr)(UnsignedObj&, UnsignedObj&);

            // Verify that the signatures and return types are standard.

            funcPtr     memberSwap = &UnsignedObj::swap;
            freeFuncPtr freeSwap   = swap;

            // Quash unused variable warnings.

            UnsignedObj a;
            UnsignedObj b;
            (a.*memberSwap)(b);
            freeSwap(a, b);
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "\nCreate a table of distinct object values."
                                                                       << endl;

        static const struct {
            int         d_line;           // source line number
            const char *d_spec;
        } DATA[] = {
          // LINE SPEC
          // ----  ----
            { L_,   ""             },
            { L_,   "z"            },
            { L_,   "zoO"          },
            { L_,   "cC"           },
            { L_,   "sS"           },
            { L_,   "iI"           },
            // Duplicates
            { L_,   "zoOzoO"       },
            { L_,   "cCcC"         },
            { L_,   "iIiI"         },
            { L_,   "zczCszSzizIz" },
            { L_,   "zoOcCsSiI"    },
            { L_,   "IiSsCcOoz"    },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const char *const SPEC1   = DATA[ti].d_spec;

            bslma::TestAllocator oa("object",       veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj        mW(&oa);
            const Obj& W = gg(&mW, SPEC1);
            const Obj  XX(W, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P_(W) P_(XX) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE1, W, Obj(&scratch), W == Obj(&scratch));
                firstFlag = false;
            }

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                mW.swap(mW);

                LOOP3_ASSERT(LINE1, W, XX, W == XX);
                LOOP_ASSERT(LINE1, &oa == W.allocator());
                LOOP_ASSERT(LINE1, oam.isTotalSame());
            }

            // free function 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                swap(mW, mW);

                LOOP3_ASSERT(LINE1, W, XX, W == XX);
                LOOP_ASSERT(LINE1, &oa == W.allocator());
                LOOP_ASSERT(LINE1, oam.isTotalSame());
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const char *const SPEC2   = DATA[tj].d_spec;

                Obj        mX(XX, &oa);
                const Obj& X = mX;

                Obj        mY(&oa);
                const Obj& Y = gg(&mY, SPEC2);
                const Obj  YY(Y, &scratch);

                if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P_(YY) }

                // member 'swap'
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    mX.swap(mY);

                    LOOP4_ASSERT(LINE1, LINE2, X, YY, X == YY);
                    LOOP4_ASSERT(LINE1, LINE2, Y, XX, Y == XX);
                    LOOP2_ASSERT(LINE1, LINE2, &oa == X.allocator());
                    LOOP2_ASSERT(LINE1, LINE2, &oa == Y.allocator());
                    LOOP2_ASSERT(LINE1, LINE2, oam.isTotalSame());
                }

                // free function 'swap', same allocator
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    swap(mX, mY);

                    LOOP4_ASSERT(LINE1, LINE2, X, XX, X == XX);
                    LOOP4_ASSERT(LINE1, LINE2, Y, YY, Y == YY);
                    LOOP2_ASSERT(LINE1, LINE2, &oa == X.allocator());
                    LOOP2_ASSERT(LINE1, LINE2, &oa == Y.allocator());
                    LOOP2_ASSERT(LINE1, LINE2, oam.isTotalSame());
                }
            }
        }

        if (verbose) cout <<
                "\nInvoke free 'swap' function in a context where ADL is used."
                                                                       << endl;
        {
            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                  Obj mX(&oa);  const Obj& X = mX;
            const Obj XX(X, &scratch);

                  Obj mY(&oa);  const Obj& Y = gg(&mY, "zoOcCsSiI");
            const Obj YY(Y, &scratch);

            if (veryVeryVerbose) { T_ P_(X) P(Y) }

            bslma::TestAllocatorMonitor oam(&oa);

            bslalg::SwapUtil::swap(&mX, &mY);

            LOOP2_ASSERT(YY, X, YY == X);
            LOOP2_ASSERT(XX, Y, XX == Y);
            ASSERT(oam.isTotalSame());

            if (veryVeryVerbose) { T_ P_(X) P(Y) }
        }

        // Verify no memory is allocated from the default allocator.

        LOOP_ASSERT(da.numBlocksTotal(), !da.numBlocksTotal());

        if (verbose) cout <<
                   "\nFree 'swap' function with different allocators." << endl;
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1 = DATA[ti].d_line;
            const char *const SPEC1 = DATA[ti].d_spec;

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator     oa2("object2", veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mXX(&scratch);  const Obj& XX = gg(&mXX, SPEC1);

            if (veryVerbose) { T_ P_(LINE1) P(XX) }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2 = DATA[tj].d_line;
                const char *const SPEC2 = DATA[tj].d_spec;

                Obj mX(XX, &oa);  const Obj& X = mX;
                Obj mY(&oa2);     const Obj& Y = gg(&mY, SPEC2);

                const Obj YY(Y, &scratch);

                if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

                // free function 'swap', different allocators
                {
                    swap(mX, mY);

                    ASSERTV(LINE1, LINE2, YY, X, YY == X);
                    ASSERTV(LINE1, LINE2, XX, Y, XX == Y);
                    ASSERTV(LINE1, LINE2, &oa  == X.allocator());
                    ASSERTV(LINE1, LINE2, &oa2 == Y.allocator());
                }
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            if (veryVerbose) cout << "\t'swap' member function" << endl;
            {
                bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
                bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                Obj mA(&oa1);
                Obj mB(&oa1);
                Obj mC(&oa1);
                Obj mZ(&oa2);

                ASSERT_SAFE_PASS(mA.swap(mB));
                ASSERT_SAFE_FAIL(mC.swap(mZ));
            }
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING BDEX STREAMING
        //   The BDEX streaming concerns for this component are standard.  We
        //   first test the class method 'maxSupportedBdexVersion' and then
        //   use that method to probe the member functions 'outStream' and
        //   'inStream' in the manner of a "breathing test" to verify basic
        //   functionality.  We then thoroughly test streaming functionality
        //   using the provided BDEX functions, which forward appropriate calls
        //   to the member functions of this component.  We next step through
        //   the sequence of possible stream states (valid, empty, invalid,
        //   incomplete, and corrupted), appropriately selecting data sets as
        //   described below.  In all cases, exception neutrality is confirmed
        //   using the specially instrumented 'bslx::TestInStream' and a pair
        //   of standard macros, 'BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN' and
        //   'BSLX_TESTINSTREAM_EXCEPTION_TEST_END', which configure the
        //   'bslx::TestInStream' object appropriately in a loop.
        //
        // Concerns:
        //: 1 The class method 'maxSupportedBdexVersion' correctly returns the
        //:   version to be used for the specified 'serializationVersion'.
        //:
        //: 2 For valid streams, externalization and unexternalization are
        //:   inverse operations.
        //:
        //: 3 For invalid streams, externalization and unexternalization
        //:   do not alter the value of the object and leave the stream
        //:   invalid.
        //:
        //: 4 Unexternalizing incomplete data does not modify the object's
        //:   value.
        //:
        //: 5 Unexternalization of corrupted data results in a valid object
        //:   of unspecified value and an invalidated stream.
        //:
        //: 6 The wire format of the object is as expected.
        //:
        //: 7 All methods are exception neutral.
        //
        // Plan:
        //: 1 Test 'maxSupportedBdexVersion' explicitly.  (C-1)
        //:
        //: 2 Perform a direct test of the 'outStream' and 'inStream'
        //:   methods (the rest of the testing will use the stream operators).
        //:
        //: 3 For the set S of globally-defined test values, use all
        //:   combinations (u, v) in the cross product S X S, stream the
        //:   value of v into (a temporary copy of) u and assert u == v.  (C-2)
        //:
        //: 4 For each u in S, create a copy and attempt to stream into it
        //:   from an invalid stream.  Verify after each try that the object is
        //:   unchanged and that the stream is invalid.  (C-3)
        //:
        //: 5 Write 3 distinct objects to an output stream buffer of total
        //:   length N.  For each partial stream length from 0 to N - 1,
        //:   construct an input stream and attempt to read into objects
        //:   initialized with distinct values.  Verify values of objects
        //:   that are either successfully modified or left entirely
        //:   unmodified, and that the stream became invalid immediately after
        //:   the first incomplete read.  Finally ensure that each object
        //:   streamed into is in some valid state by assigning it a distinct
        //:   new value and testing for equality.  (C-4)
        //:
        //: 6 Use the underlying stream package to simulate of a typical valid
        //:   (control) stream and verify that it can be streamed in
        //:   successfully.  Then for each data field in the stream (beginning
        //:   with the version number), provide one or more similar tests with
        //:   that data field corrupted.  After each test, verify that the
        //:   object is in some valid state after streaming, and that the
        //:   input stream has gone invalid.  (C-5)
        //:
        //: 7 Explicitly test the wire format.  (C-6)
        //:
        //: 8 Use the exception testing macros to ensure all methods are
        //:   exception neutral.  (C-7)
        //
        // Testing:
        //   int maxSupportedBdexVersion(int serializationVersion);
        //   STREAM& bdexStreamIn(STREAM& stream, int version);
        //   STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING BDEX STREAMING" << endl
                          << "======================" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // scalar and array object values for various stream tests
        Obj mVA;  gg(&mVA, "");         const Obj& VA = mVA;

        Obj mVB;  gg(&mVB, "C");        const Obj& VB = mVB;

        Obj mVC;  gg(&mVC, "CS");       const Obj& VC = mVC;

        Obj mVD;  gg(&mVD, "CSI");      const Obj& VD = mVD;

        Obj mVE;  gg(&mVE, "CSIL");     const Obj& VE = mVE;

        Obj mVF;  gg(&mVF, "CsSiI");    const Obj& VF = mVF;

        Obj mVG;  gg(&mVG, "zOLLLIs");  const Obj& VG = mVG;

        UnsignedObj mUVA;  gg(&mUVA, "");        const UnsignedObj& UVA = mUVA;

        UnsignedObj mUVB;  gg(&mUVB, "C");       const UnsignedObj& UVB = mUVB;

        UnsignedObj mUVC;  gg(&mUVC, "CS");      const UnsignedObj& UVC = mUVC;

        UnsignedObj mUVD;  gg(&mUVD, "CSI");     const UnsignedObj& UVD = mUVD;

        UnsignedObj mUVE;  gg(&mUVE, "CSIL");    const UnsignedObj& UVE = mUVE;

        UnsignedObj mUVF;  gg(&mUVF, "CsSiI");   const UnsignedObj& UVF = mUVF;

        UnsignedObj mUVG;  gg(&mUVG, "zOLLLIs"); const UnsignedObj& UVG = mUVG;

        const int NUM_VALUES = 7;

        const Obj         VALUES[NUM_VALUES] = { VA, VB, VC, VD, VE, VF, VG };
        const UnsignedObj UVALUES[NUM_VALUES] =
                                         { UVA, UVB, UVC, UVD, UVE, UVF, UVG };
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'." << endl;
        {
            if (verbose) cout << "\tUsing object syntax." << endl;
            const Obj         X;
            const UnsignedObj Y;
            ASSERT(1 == X.maxSupportedBdexVersion(0));
            ASSERT(1 == Y.maxSupportedBdexVersion(0));
            if (verbose) cout << "\tUsing class method syntax." << endl;
            ASSERT(1 == Obj::maxSupportedBdexVersion(0));
            ASSERT(1 == UnsignedObj::maxSupportedBdexVersion(0));
        }

        if (verbose) {
            cout << "\nDirect initial trial of 'streamOut' and (valid) "
                 << "'streamIn' functionality." << endl;
        }
        {
            const Obj X(VC);
            Out       out(SERIALIZATION_VERSION);
            const int VERSION = Obj::maxSupportedBdexVersion(0);

            ASSERT(&out == &X.bdexStreamOut(out, VERSION));

            const char *const OD  = out.data();
            const bsl::size_t LOD = out.length();
            In                in(OD, LOD);
            ASSERT(in);
            ASSERT(!in.isEmpty());
            Obj               t(VA);
            ASSERT(X != t);

            ASSERT(&in == &t.bdexStreamIn(in, VERSION)); ASSERT(X == t);
            ASSERT(in);                                  ASSERT(in.isEmpty());
        }
        {
            const UnsignedObj X(UVC);
            Out               out(SERIALIZATION_VERSION);

            const int VERSION = UnsignedObj::maxSupportedBdexVersion(0);

            ASSERT(&out == &X.bdexStreamOut(out, VERSION));

            const char *const OD  = out.data();
            const bsl::size_t LOD = out.length();
            In                in(OD, LOD);
            ASSERT(in);
            ASSERT(!in.isEmpty());
            UnsignedObj       t(UVA);
            ASSERT(X != t);

            ASSERT(&in == &t.bdexStreamIn(in, VERSION)); ASSERT(X == t);
            ASSERT(in);                                  ASSERT(in.isEmpty());
        }

        const int VERSION = 1;

        if (verbose) {
            cout << "\nThorough test using stream operators ('<<' and '>>')."
                 << endl;
        }
        {
            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X(VALUES[i]);

                Out out(SERIALIZATION_VERSION);

                using bslx::OutStreamFunctions::bdexStreamOut;
                using bslx::InStreamFunctions::bdexStreamIn;

                bdexStreamOut(out, X, VERSION);
                const char *const OD  = out.data();
                const bsl::size_t LOD = out.length();

                // Verify that each new value overwrites every old value and
                // that the input stream is emptied, but remains valid.

                for (int j = 0; j < NUM_VALUES; ++j) {
                    In in(OD, LOD);
                    LOOP2_ASSERT(i, j, in);  LOOP2_ASSERT(i, j, !in.isEmpty());
                    BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                        Obj t(VALUES[j]);
                        in.reset();
                        LOOP2_ASSERT(i, j, (X == t) == (i == j));
                        bdexStreamIn(in, t, VERSION);
                        LOOP2_ASSERT(i, j, X == t);
                    } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
                    LOOP2_ASSERT(i, j, in);  LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }
        {
            for (int i = 0; i < NUM_VALUES; ++i) {
                const UnsignedObj X(UVALUES[i]);

                Out out(SERIALIZATION_VERSION);

                using bslx::OutStreamFunctions::bdexStreamOut;
                using bslx::InStreamFunctions::bdexStreamIn;

                bdexStreamOut(out, X, VERSION);
                const char *const OD  = out.data();
                const bsl::size_t LOD = out.length();

                // Verify that each new value overwrites every old value and
                // that the input stream is emptied, but remains valid.

                for (int j = 0; j < NUM_VALUES; ++j) {
                    In in(OD, LOD);
                    LOOP2_ASSERT(i, j, in);  LOOP2_ASSERT(i, j, !in.isEmpty());
                    BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                        UnsignedObj t(UVALUES[j]);
                        in.reset();
                        LOOP2_ASSERT(i, j, (X == t) == (i == j));
                        bdexStreamIn(in, t, VERSION);
                        LOOP2_ASSERT(i, j, X == t);
                    } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
                    LOOP2_ASSERT(i, j, in);  LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }

        if (verbose) {
            cout << "\nTesting streamIn functionality via operator ('>>')."
                 << endl;
        }

        if (verbose) cout << "\tOn empty and invalid streams." << endl;
        {
            Out out(SERIALIZATION_VERSION);

            const char *const OD  = out.data();
            const bsl::size_t LOD = out.length();
            ASSERT(0 == LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                In in(OD, LOD);
                LOOP_ASSERT(i, in);           LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input stream
                // leaves the stream invalid and the target object unchanged.

                using bslx::InStreamFunctions::bdexStreamIn;

                const Obj X(VALUES[i]);  Obj t(X);  LOOP_ASSERT(i, X == t);
                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                    in.reset();
                    bdexStreamIn(in, t, VERSION);
                    LOOP_ASSERT(i, !in);     LOOP_ASSERT(i, X == t);
                    bdexStreamIn(in, t, VERSION);
                    LOOP_ASSERT(i, !in);     LOOP_ASSERT(i, X == t);
                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }
        {
            Out out(SERIALIZATION_VERSION);

            const char *const OD  = out.data();
            const bsl::size_t LOD = out.length();
            ASSERT(0 == LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                In in(OD, LOD);
                LOOP_ASSERT(i, in);           LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input stream
                // leaves the stream invalid and the target object unchanged.

                using bslx::InStreamFunctions::bdexStreamIn;

                const UnsignedObj X(UVALUES[i]);
                UnsignedObj       t(X);
                LOOP_ASSERT(i, X == t);
                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                    in.reset();
                    bdexStreamIn(in, t, VERSION);
                    LOOP_ASSERT(i, !in);     LOOP_ASSERT(i, X == t);
                    bdexStreamIn(in, t, VERSION);
                    LOOP_ASSERT(i, !in);     LOOP_ASSERT(i, X == t);
                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }

        if (verbose) {
            cout << "\tOn incomplete (but otherwise valid) data."
                 << endl;
        }
        {
            const Obj W1 = VA, X1 = VB, Y1 = VC;
            const Obj W2 = VB, X2 = VC, Y2 = VD;
            const Obj W3 = VC, X3 = VD, Y3 = VE;

            using bslx::OutStreamFunctions::bdexStreamOut;
            using bslx::InStreamFunctions::bdexStreamIn;

            Out out(SERIALIZATION_VERSION);
            bdexStreamOut(out, X1, VERSION);
            const bsl::size_t LOD1 = out.length();
            bdexStreamOut(out, X2, VERSION);
            const bsl::size_t LOD2 = out.length();
            bdexStreamOut(out, X3, VERSION);
            const bsl::size_t LOD  = out.length();

            const char *const OD = out.data();

            for (bsl::size_t i = 0; i < LOD; ++i) {
                In in(OD, i);
                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                    in.reset();
                    LOOP_ASSERT(i, in); LOOP_ASSERT(i, !i == in.isEmpty());
                    Obj t1(W1), t2(W2), t3(W3);

                    if (i < LOD1) {
                        bdexStreamIn(in, t1, VERSION);
                        LOOP_ASSERT(i, !in);
                        if (0 == i) LOOP_ASSERT(i, W1 == t1);
                        bdexStreamIn(in, t2, VERSION);
                        LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W2 == t2);
                        bdexStreamIn(in, t3, VERSION);
                        LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W3 == t3);
                    }
                    else if (i < LOD2) {
                        bdexStreamIn(in, t1, VERSION);
                        LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X1 == t1);
                        bdexStreamIn(in, t2, VERSION);
                        LOOP_ASSERT(i, !in);
                        if (LOD1 == i) LOOP_ASSERT(i, W2 == t2);
                        bdexStreamIn(in, t3, VERSION);
                        LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W3 == t3);
                    }
                    else {
                        bdexStreamIn(in, t1, VERSION);
                        LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X1 == t1);
                        bdexStreamIn(in, t2, VERSION);
                        LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X2 == t2);
                        bdexStreamIn(in, t3, VERSION);
                        LOOP_ASSERT(i, !in);
                        if (LOD2 == i) LOOP_ASSERT(i, W3 == t3);
                    }

                    LOOP_ASSERT(i, Y1 != t1);
                    t1 = Y1;
                    LOOP_ASSERT(i, Y1 == t1);

                    LOOP_ASSERT(i, Y2 != t2);
                    t2 = Y2;
                    LOOP_ASSERT(i, Y2 == t2);

                    LOOP_ASSERT(i, Y3 != t3);
                    t3 = Y3;
                    LOOP_ASSERT(i, Y3 == t3);
                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }
        {
            const UnsignedObj W1 = UVA, X1 = UVB, Y1 = UVC;
            const UnsignedObj W2 = UVB, X2 = UVC, Y2 = UVD;
            const UnsignedObj W3 = UVC, X3 = UVD, Y3 = UVE;

            using bslx::OutStreamFunctions::bdexStreamOut;
            using bslx::InStreamFunctions::bdexStreamIn;

            Out out(SERIALIZATION_VERSION);
            bdexStreamOut(out, X1, VERSION);
            const bsl::size_t LOD1 = out.length();
            bdexStreamOut(out, X2, VERSION);
            const bsl::size_t LOD2 = out.length();
            bdexStreamOut(out, X3, VERSION);
            const bsl::size_t LOD  = out.length();

            const char *const OD = out.data();

            for (bsl::size_t i = 0; i < LOD; ++i) {
                In in(OD, i);
                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                    in.reset();
                    LOOP_ASSERT(i, in); LOOP_ASSERT(i, !i == in.isEmpty());
                    UnsignedObj t1(W1), t2(W2), t3(W3);

                    if (i < LOD1) {
                        bdexStreamIn(in, t1, VERSION);
                        LOOP_ASSERT(i, !in);
                        if (0 == i) LOOP_ASSERT(i, W1 == t1);
                        bdexStreamIn(in, t2, VERSION);
                        LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W2 == t2);
                        bdexStreamIn(in, t3, VERSION);
                        LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W3 == t3);
                    }
                    else if (i < LOD2) {
                        bdexStreamIn(in, t1, VERSION);
                        LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X1 == t1);
                        bdexStreamIn(in, t2, VERSION);
                        LOOP_ASSERT(i, !in);
                        if (LOD1 == i) LOOP_ASSERT(i, W2 == t2);
                        bdexStreamIn(in, t3, VERSION);
                        LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W3 == t3);
                    }
                    else {
                        bdexStreamIn(in, t1, VERSION);
                        LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X1 == t1);
                        bdexStreamIn(in, t2, VERSION);
                        LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X2 == t2);
                        bdexStreamIn(in, t3, VERSION);
                        LOOP_ASSERT(i, !in);
                        if (LOD2 == i) LOOP_ASSERT(i, W3 == t3);
                    }

                    LOOP_ASSERT(i, Y1 != t1);
                    t1 = Y1;
                    LOOP_ASSERT(i, Y1 == t1);

                    LOOP_ASSERT(i, Y2 != t2);
                    t2 = Y2;
                    LOOP_ASSERT(i, Y2 == t2);

                    LOOP_ASSERT(i, Y3 != t3);
                    t3 = Y3;
                    LOOP_ASSERT(i, Y3 == t3);
                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }

        if (verbose) cout << "\tOn corrupted data." << endl;

        const Obj W;                   // default value
        const Obj X(VC);               // original (control)
        const Obj Y(VB);               // new (streamed-out)

        const UnsignedObj UW;          // default value
        const UnsignedObj UX(UVC);     // original (control)
        const UnsignedObj UY(UVB);     // new (streamed-out)

        if (verbose) cout << "\t\tGood stream (for control)." << endl;
        {
            Out out(SERIALIZATION_VERSION);

            // Stream out "new" value.
            out.putInt8(1);
            out.putLength(1);
            out.putInt8(127);

            const char *const OD  = out.data();
            const bsl::size_t LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);

            using bslx::InStreamFunctions::bdexStreamIn;

            bdexStreamIn(in, t, VERSION);  ASSERT(in);
            ASSERT(W != t);  ASSERT(X != t);  ASSERT(Y == t);
        }
        {
            Out out(SERIALIZATION_VERSION);

            // Stream out "new" value.
            out.putInt8(1);
            out.putLength(1);
            out.putUint8(255);

            const char *const OD  = out.data();
            const bsl::size_t LOD = out.length();

            UnsignedObj t(UX);
            ASSERT(UW != t);  ASSERT(UX == t);  ASSERT(UY != t);
            In in(OD, LOD);  ASSERT(in);

            using bslx::InStreamFunctions::bdexStreamIn;

            bdexStreamIn(in, t, VERSION);  ASSERT(in);
            ASSERT(UW != t);  ASSERT(UX != t);  ASSERT(UY == t);
        }

        if (verbose) cout << "\t\tBad version." << endl;
        {
            const char version = 0; // too small ('version' must be >= 1)

            Out out(SERIALIZATION_VERSION);

            // Stream out "new" value.
            out.putInt8(1);
            out.putLength(1);
            out.putInt8(127);

            const char *const OD  = out.data();
            const bsl::size_t LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            bdexStreamIn(in, t, version);  ASSERT(!in);
            ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        {
            const char version = 0; // too small ('version' must be >= 1)

            Out out(SERIALIZATION_VERSION);

            // Stream out "new" value.
            out.putInt8(1);
            out.putLength(1);
            out.putUint8(255);

            const char *const OD  = out.data();
            const bsl::size_t LOD = out.length();

            UnsignedObj t(UX);
            ASSERT(UW != t);  ASSERT(UX == t);  ASSERT(UY != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            bdexStreamIn(in, t, version);  ASSERT(!in);
            ASSERT(UW != t);  ASSERT(UX == t);  ASSERT(UY != t);
        }
        {
            const char version = 5 ; // too large (current version is 1)

            Out out(SERIALIZATION_VERSION);

            // Stream out "new" value.
            out.putInt8(1);
            out.putLength(1);
            out.putInt8(127);

            const char *const OD  = out.data();
            const bsl::size_t LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            bdexStreamIn(in, t, version);  ASSERT(!in);
            ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        {
            const char version = 5 ; // too large (current version is 1)

            Out out(SERIALIZATION_VERSION);

            // Stream out "new" value.
            out.putInt8(1);
            out.putLength(1);
            out.putUint8(255);

            const char *const OD  = out.data();
            const bsl::size_t LOD = out.length();

            UnsignedObj t(UX);
            ASSERT(UW != t);  ASSERT(UX == t);  ASSERT(UY != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            bdexStreamIn(in, t, version);  ASSERT(!in);
            ASSERT(UW != t);  ASSERT(UX == t);  ASSERT(UY != t);
        }

        if (verbose) cout << "\t\tInvalid bytes per element." << endl;
        {
            Out out(SERIALIZATION_VERSION);

            // Stream out "new" value.
            out.putInt8(3);
            out.putLength(1);
            out.putInt8(127);

            const char *const OD  = out.data();
            const bsl::size_t LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            bdexStreamIn(in, t, VERSION);  ASSERT(!in);
            ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        {
            Out out(SERIALIZATION_VERSION);

            // Stream out "new" value.
            out.putInt8(3);
            out.putLength(1);
            out.putUint8(255);

            const char *const OD  = out.data();
            const bsl::size_t LOD = out.length();

            UnsignedObj t(UX);
            ASSERT(UW != t);  ASSERT(UX == t);  ASSERT(UY != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            bdexStreamIn(in, t, VERSION);  ASSERT(!in);
            ASSERT(UW != t);  ASSERT(UX == t);  ASSERT(UY != t);
        }

        if (verbose) cout << "\nWire format direct tests." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec_p;       // specification
                int         d_version;      // version to stream with
                int         d_length;       // expect output length
                const char *d_fmt_p;        // expected output format
            } DATA[] = {
                //line  spec       ver  len  format
                //----  ---------  ---  ---  ------------------------------
                { L_,   "C",         0,   0, ""                             },
                { L_,   "CS",        0,   0, ""                             },

                { L_,   "C",         2,   0, ""                             },
                { L_,   "CS",        2,   0, ""                             },

                { L_,   "C",         1,   3, "\x01\x01\x7F"                 },
                { L_,   "CS",        1,   6, "\x02\x02\x00\x7F\x7f\xFF"     }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const char *const SPEC    = DATA[i].d_spec_p;
                const int         VERSION = DATA[i].d_version;
                const int         LEN     = DATA[i].d_length;
                const char *const FMT     = DATA[i].d_fmt_p;

                if (veryVerbose) { P_(SPEC) P_(VERSION) P(LEN) }

                Obj mX;  gg(&mX, SPEC);  const Obj& X = mX;

                bslx::ByteOutStream out(SERIALIZATION_VERSION);
                X.bdexStreamOut(out, VERSION);

                LOOP_ASSERT(LINE, LEN == static_cast<int>(out.length()));
                LOOP_ASSERT(LINE, 0 == memcmp(out.data(),
                                              FMT,
                                              LEN));

                if (memcmp(out.data(), FMT, LEN)) {
                    if (veryVerbose) {
                        const char *hex = "0123456789abcdef";
                        P_(LINE);
                        for (bsl::size_t j = 0; j < out.length(); ++j) {
                            cout << "\\x"
                                 << hex[*(out.data() + j) >> 4]
                                 << hex[*(out.data() + j) & 0x0f];
                        }
                        cout << endl;
                    }
                }

                Obj mY;  const Obj& Y = mY;
                if (LEN) { // version is supported
                    bslx::ByteInStream in(out.data(), out.length());
                    mY.bdexStreamIn(in, VERSION);
                }
                else { // version is not supported
                    mY = X;
                    bslx::ByteInStream in;
                    mY.bdexStreamIn(in, VERSION);
                    LOOP_ASSERT(LINE, !in);
                }
                LOOP_ASSERT(LINE, X == Y);
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // COPY ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any target object
        //:   to that of any source object.
        //:
        //: 2 The address of the target object's allocator is unchanged.
        //:
        //: 3 Any memory allocation is from the target object's allocator.
        //:
        //: 4 The signature and return type are standard.
        //:
        //: 5 The reference returned is to the target object (i.e., '*this').
        //:
        //: 6 The value of the source object is not modified.
        //:
        //: 7 The allocator of the source object is not modified.
        //:
        //: 8 Any memory allocation is exception neutral.
        //:
        //: 9 Assigning an object to itself behaves as expected (alias-safety).
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a function pointer
        //:   having the appropriate structure for the copy assignment operator
        //:   defined in this component.  (C-4)
        //:
        //: 2 Construct a 'bslma::TestAllocator' object and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of valid object values (one per row) in terms of
        //:     their individual attributes, including (a) first, the default
        //:     value, (b) boundary values corresponding to every range of
        //:     values that each individual attribute can independently attain,
        //:     and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //: 4 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3: (C-1..3, 5..9)
        //:
        //:   1 Use the value constructor to create two 'const' 'Obj', 'Z' and
        //:     'ZZ', each having the value 'V' and using a "scratch"
        //:     allocator.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2):
        //:
        //:     1 Construct a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:     2 Use the value constructor to create a modifiable 'Obj', 'mX',
        //:       using 'oa' and having the value 'W'.
        //:
        //:     3 Use the equality comparison operators to verify that the two
        //:       objects, 'mX' and 'Z', are initially equal if and only if the
        //:       table indices from P-4 and P-4.2 are the same.
        //:
        //:     4 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:       (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:
        //:     5 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     6 Use the equality comparison operators to verify that: (C-1,
        //:       6)
        //:
        //:       1 The target object, 'mX', now has the same value as 'Z'.
        //:
        //:       2 'Z' still has the same value as 'ZZ'.
        //:
        //:     7 Use the 'allocator' accessor of both 'mX' and 'Z' to verify
        //:       that both object allocators are unchanged.  (C-2, 7)
        //:
        //:     8 Use appropriate test allocators to verify that: (C-3, 8)
        //:
        //:       1 No additional memory is allocated by the source object.
        //:
        //:       2 All object memory is released when the object is destroyed.
        //:
        //: 5 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-4.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-3: (C-1..3, 5..10)
        //:
        //:   1 Construct a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor to create a modifiable 'Obj' 'mX'
        //:     using 'oa' and a 'const' 'Obj' 'ZZ' (using a distinct "scratch"
        //:     allocator).
        //:
        //:   3 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   4 Use the equality comparison operators to verify that the target
        //:     object, 'mX', has the same value as 'ZZ'.
        //:
        //:   5 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:     (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:     (C-9)
        //:
        //:   6 Verify that the address of the return value is the same as that
        //:     of 'mX'.  (C-5)
        //:
        //:   7 Use the equality comparison operators to verify that the target
        //:     object, 'mX', still has the same value as 'ZZ'.  (C-10)
        //:
        //:   8 Use the 'allocator' accessor of 'mX' to verify that it is still
        //:     the object allocator.  (C-2)
        //:
        //:   9 Use appropriate test allocators to verify that: (C-3, 8)
        //:
        //:     1 Any memory that is allocated is from the object allocator.
        //:
        //:     2 No additional (e.g., temporary) object memory is allocated
        //:       when assigning an object value that did NOT initially require
        //:       allocated memory.
        //:
        //:     3 All object memory is released when the object is destroyed.
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   PackedIntArray& operator=(const PackedIntArray& rhs);
        // --------------------------------------------------------------------


        if (verbose) cout << endl
                          << "COPY ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            // Quash unused variable warning.

            Obj a;
            Obj b;
            (a.*operatorAssignment)(b);
        }
        {
            typedef UnsignedObj&
                               (UnsignedObj::*operatorPtr)(const UnsignedObj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &UnsignedObj::operator=;

            // Quash unused variable warning.

            UnsignedObj a;
            UnsignedObj b;
            (a.*operatorAssignment)(b);
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
        "\nCreate a table of distinct object values and expected memory usage."
                                                                       << endl;

        static const struct {
            int         d_line;           // source line number
            const char *d_spec;
        } DATA[] = {
          // LINE SPEC
          // ----  ----
            { L_,   ""             },
            { L_,   "z"            },
            { L_,   "zO"           },
            { L_,   "CS"           },
            { L_,   "sS"           },
            { L_,   "iI"           },
            // Duplicates
            { L_,   "zOzO"         },
            { L_,   "CSCS"         },
            { L_,   "iIiI"         },
            { L_,   "zSzCszSzizIz" },
            { L_,   "zOOSCsSiI"    },
            { L_,   "IiSsCSOOz"    },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const char *const SPEC1   = DATA[ti].d_spec;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mZ(&scratch);   const Obj& Z = gg(&mZ,  SPEC1);

            Obj mZZ(&scratch);  const Obj& ZZ = gg(&mZZ, SPEC1);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P_(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE1, Z, Obj(&scratch), Z == Obj(&scratch));
                firstFlag = false;
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const char *const SPEC2   = DATA[tj].d_spec;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(&oa);  const Obj& X = gg(&mX, SPEC2);

                    if (veryVerbose) { T_ P_(LINE2) P_(X) }

                    LOOP4_ASSERT(LINE1, LINE2, X, Z,
                                 (X == Z) == (LINE1 == LINE2));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    Obj *mR = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        mR = &(mX = Z);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    // Verify the address of the return value.

                    LOOP4_ASSERT(LINE1, LINE2, mR, &mX, mR == &mX);

                    LOOP4_ASSERT(LINE1, LINE2,  X, Z,  X == Z);
                    LOOP4_ASSERT(LINE1, LINE2, ZZ, Z, ZZ == Z);

                    LOOP4_ASSERT(LINE1, LINE2, &oa, X.allocator(),
                                 &oa == X.allocator());
                    LOOP4_ASSERT(LINE1, LINE2, &scratch, Z.allocator(),
                                 &scratch == Z.allocator());

                    LOOP2_ASSERT(LINE1, LINE2, sam.isInUseSame());

                    LOOP2_ASSERT(LINE1, LINE2, !da.numBlocksTotal());
                }

                // Verify all memory is released on object destruction.

                LOOP3_ASSERT(LINE1, LINE2, da.numBlocksInUse(),
                             0 == da.numBlocksInUse());
                LOOP3_ASSERT(LINE1, LINE2, oa.numBlocksInUse(),
                             0 == oa.numBlocksInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(&oa);   const Obj& X = gg(&mX, SPEC1);

                Obj mZZ(&oa);  const Obj& ZZ = gg(&mZZ, SPEC1);

                const Obj& Z = mX;

                LOOP3_ASSERT(LINE1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                Obj *mR = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    mR = &(mX = Z);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                // Verify the address of the return value.

                LOOP3_ASSERT(LINE1, mR, &mX, mR == &mX);

                LOOP3_ASSERT(LINE1, mR, &X, mR == &X);

                LOOP3_ASSERT(LINE1, Z, ZZ, ZZ == Z);

                LOOP3_ASSERT(LINE1, &oa, Z.allocator(), &oa == Z.allocator());

                if (0 == ti) {  // Empty, no allocation.
                    LOOP_ASSERT(LINE1, oam.isInUseSame());
                }

                LOOP_ASSERT(LINE1, sam.isInUseSame());

                LOOP_ASSERT(LINE1, !da.numBlocksTotal());
            }

            // Verify all memory is released on object destruction.

            LOOP2_ASSERT(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
            LOOP2_ASSERT(LINE1, da.numBlocksInUse(), 0 == da.numBlocksInUse());
        }

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const char *const SPEC1   = DATA[ti].d_spec;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            UnsignedObj        mZ(&scratch);
            const UnsignedObj& Z  = gg(&mZ,  SPEC1);
            UnsignedObj        mZZ(&scratch);
            const UnsignedObj& ZZ = gg(&mZZ, SPEC1);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P_(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE1,
                             Z,
                             UnsignedObj(&scratch),
                             Z == UnsignedObj(&scratch));
                firstFlag = false;
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const char *const SPEC2   = DATA[tj].d_spec;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    UnsignedObj        mX(&oa);
                    const UnsignedObj& X = gg(&mX, SPEC2);

                    if (veryVerbose) { T_ P_(LINE2) P_(X) }

                    LOOP4_ASSERT(LINE1, LINE2, X, Z,
                                 (X == Z) == (LINE1 == LINE2));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    UnsignedObj *mR = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        mR = &(mX = Z);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    // Verify the address of the return value.

                    LOOP4_ASSERT(LINE1, LINE2, mR, &mX, mR == &mX);

                    LOOP4_ASSERT(LINE1, LINE2,  X, Z,  X == Z);
                    LOOP4_ASSERT(LINE1, LINE2, ZZ, Z, ZZ == Z);

                    LOOP4_ASSERT(LINE1, LINE2, &oa, X.allocator(),
                                 &oa == X.allocator());
                    LOOP4_ASSERT(LINE1, LINE2, &scratch, Z.allocator(),
                                 &scratch == Z.allocator());

                    LOOP2_ASSERT(LINE1, LINE2, sam.isInUseSame());

                    LOOP2_ASSERT(LINE1, LINE2, !da.numBlocksTotal());
                }

                // Verify all memory is released on object destruction.

                LOOP3_ASSERT(LINE1, LINE2, da.numBlocksInUse(),
                             0 == da.numBlocksInUse());
                LOOP3_ASSERT(LINE1, LINE2, oa.numBlocksInUse(),
                             0 == oa.numBlocksInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                UnsignedObj mX(&oa);   const UnsignedObj& X = gg(&mX, SPEC1);

                UnsignedObj mZZ(&oa);  const UnsignedObj& ZZ = gg(&mX, SPEC1);

                const UnsignedObj& Z = mX;

                LOOP3_ASSERT(LINE1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                UnsignedObj *mR = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    mR = &(mX = Z);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                // Verify the address of the return value.

                LOOP3_ASSERT(LINE1, mR, &mX, mR == &mX);

                LOOP3_ASSERT(LINE1, mR, &X, mR == &X);

                LOOP3_ASSERT(LINE1, Z, ZZ, ZZ == Z);

                LOOP3_ASSERT(LINE1, &oa, Z.allocator(), &oa == Z.allocator());

                if (0 == ti) {  // Empty, no allocation.
                    LOOP_ASSERT(LINE1, oam.isInUseSame());
                }

                LOOP_ASSERT(LINE1, sam.isInUseSame());

                LOOP_ASSERT(LINE1, !da.numBlocksTotal());
            }

            // Verify all memory is released on object destruction.

            LOOP2_ASSERT(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
            LOOP2_ASSERT(LINE1, da.numBlocksInUse(), 0 == da.numBlocksInUse());
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTION
        //   Verify the 'g' functions work properly.
        //
        // Since there are two result types needed, the function was omitted.
        //
        // Testing:
        //   PackedIntArray g(const char *spec);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GENERATOR FUNCTION" << endl
                          << "==================" << endl;
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor (with or without a supplied allocator)
        //:   creates an object having the same value as the supplied original
        //:   object.
        //:
        //: 2 If an allocator is *not* supplied to the copy constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object (i.e., the
        //:   allocator of the original object is never copied).
        //:
        //: 3 If an allocator is supplied to the copy constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the copy constructor has no effect on
        //:   subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator, which is
        //:   returned by the 'allocator' accessor method.
        //:
        //: 7 There is no temporary memory allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 The original object is passed as a reference providing
        //:   non-modifiable access to the object.
        //:
        //:10 The value of the original object is unchanged.
        //:
        //:11 The allocator address of the original object is unchanged.
        //:
        //:12 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of widely varying object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1: (C-1..12)
        //:
        //:   1 Use the value constructor to create two 'const' 'Obj', 'Z' and
        //:     'ZZ', each having the value 'V' and using a "scratch"
        //:     allocator.
        //:
        //:   2 Execute an inner loop creating three distinct objects in turn,
        //:     each using the copy constructor on 'Z' from P-2.1, but
        //:     configured differently: (a) without passing an allocator, (b)
        //:     passing a null allocator address explicitly, and (c) passing
        //:     the address of a test allocator distinct from the default.
        //:
        //:   3 For each of these three iterations (P-2.2): (C-1..12)
        //:
        //:     1 Construct three 'bslma::TestAllocator' objects and install
        //:       one as the current default allocator (note that a ubiquitous
        //:       test allocator is already installed as the global allocator).
        //:
        //:     2 Use the copy constructor to dynamically create an object 'X',
        //:       supplying it the 'const' object 'Z' (see P-2.1), configured
        //:       appropriately (see P-2.2) using a distinct test allocator for
        //:       the object's footprint.  (C-9)
        //:
        //:     3 Use the equality comparison operators to verify that:
        //:
        //:       1 The newly constructed object, 'X', has the same value as
        //:         'Z'.  (C-1, 5)
        //:
        //:       2 'Z' still has the same value as 'ZZ'.  (C-10)
        //:
        //:     4 Use the 'allocator' accessor of each underlying attribute
        //:       capable of allocating memory to ensure that its object
        //:       allocator is properly installed; also apply the object's
        //:       'allocator' accessor, as well as that of 'Z'.  (C-6, 11)
        //:
        //:     5 Use appropriate test allocators to verify that: (C-2..4, 7,
        //:       8, 12)
        //:
        //:       1 Memory is always allocated, and comes from the object
        //:         allocator only (irrespective of the specific number of
        //:         allocations or the total amount of memory allocated).
        //:         (C-2, 4)
        //:
        //:       2 If an allocator was supplied at construction (P-2.1c), the
        //:         current default allocator doesn't allocate any memory.
        //:         (C-3)
        //:
        //:       3 No temporary memory is allocated from the object allocator.
        //:         (C-7)
        //:
        //:       4 All object memory is released when the object is destroyed.
        //:         (C-8)
        //:
        //: 3 Create an object as an automatic variable in the presence of
        //:   injected exceptions (using the
        //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros) and verify that no
        //:   memory is leaked.  (C-12)
        //
        // Testing:
        //   PackedIntArray(const PackedIntArray& o, *bA = 0);
        // --------------------------------------------------------------------


        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        if (verbose) cout << "\nTesting signed arrays." << endl;
        {
            static const struct {
                int         d_line;           // source line number
                const char *d_spec;
            } DATA[] = {
                // LINE SPEC
                // ----  ----
                { L_,   ""             },
                { L_,   "z"            },
                { L_,   "zoO"          },
                { L_,   "cC"           },
                { L_,   "sS"           },
                { L_,   "iI"           },
                { L_,   "zczCszSzizIz" },
                { L_,   "zoOcCsSiI"    }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory.

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const char *const SPEC   = DATA[ti].d_spec;

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mZ(&scratch);   const Obj& Z = gg(&mZ, SPEC);

                Obj mZZ(&scratch);  const Obj& ZZ = gg(&mZZ, SPEC);

                if (veryVerbose) { T_ P_(Z) P(ZZ) }

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj                  *objPtr;
                    bslma::TestAllocator *objAllocatorPtr;

                    switch (CONFIG) {
                      case 'a': {
                        objPtr = new (fa) Obj(Z);
                        objAllocatorPtr = &da;
                      } break;
                      case 'b': {
                        objPtr = new (fa) Obj(Z, 0);
                        objAllocatorPtr = &da;
                      } break;
                      case 'c': {
                        objPtr = new (fa) Obj(Z, &sa);
                        objAllocatorPtr = &sa;
                      } break;
                      default: {
                        LOOP_ASSERT(CONFIG, !"Bad allocator config.");
                      } break;
                    }
                    LOOP2_ASSERT(LINE, CONFIG,
                                 sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                    // Ensure the first row of the table contains the
                    // default-constructed value.

                    static bool firstFlag = true;
                    if (firstFlag) {
                        LOOP4_ASSERT(LINE, CONFIG, Obj(), *objPtr,
                                     Obj() == *objPtr)
                        firstFlag = false;
                    }

                    // Verify the value of the object.

                    LOOP4_ASSERT(LINE, CONFIG,  X, Z,  X == Z);

                    // Verify that the value of 'Z' has not changed.

                    LOOP4_ASSERT(LINE, CONFIG, ZZ, Z, ZZ == Z);

                    // Also apply the object's 'allocator' accessor, as well as
                    // that of 'Z'.

                    LOOP4_ASSERT(LINE, CONFIG, &oa, X.allocator(),
                                 &oa == X.allocator());

                    LOOP4_ASSERT(LINE, CONFIG, &scratch, Z.allocator(),
                                 &scratch == Z.allocator());

                    // Verify no allocation from the non-object allocator.

                    LOOP3_ASSERT(LINE, CONFIG, noa.numBlocksTotal(),
                                 0 == noa.numBlocksTotal());

                    // Verify memory is always allocated (because of bsl::set)
                    // except when default constructed (SPEC == "").

                    if (0 == ti) {
                        LOOP3_ASSERT(LINE, CONFIG, oa.numBlocksInUse(),
                                     0 == oa.numBlocksInUse());
                    }
                    else {
                        LOOP3_ASSERT(LINE, CONFIG, oa.numBlocksInUse(),
                                     0 != oa.numBlocksInUse());
                    }

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    LOOP3_ASSERT(LINE, CONFIG, fa.numBlocksInUse(),
                                 0 == fa.numBlocksInUse());
                    LOOP3_ASSERT(LINE, CONFIG, da.numBlocksInUse(),
                                 0 == da.numBlocksInUse());
                    LOOP3_ASSERT(LINE, CONFIG, sa.numBlocksInUse(),
                                 0 == sa.numBlocksInUse());
                }  // end foreach configuration

            }  // end foreach row

            // Double check that at least some object memory got allocated.

            ASSERT(anyObjectMemoryAllocatedFlag);

            // Note that memory should be independently allocated for each
            // attribute capable of allocating memory.

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const char *const SPEC   = DATA[ti].d_spec;

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mZ(&scratch);  const Obj& Z = gg(&mZ, SPEC);

                if (veryVerbose) { T_ P_(LINE) P(Z) }

                bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj obj(Z, &oa);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP2_ASSERT(LINE, da.numBlocksInUse(), !da.numBlocksInUse());
                LOOP2_ASSERT(LINE, oa.numBlocksInUse(), !oa.numBlocksInUse());
            }
        }

        if (verbose) cout << "\nTesting unsigned arrays." << endl;
        {
            static const struct {
                int         d_line;           // source line number
                const char *d_spec;
            } DATA[] = {
                // LINE SPEC
                // ----  ----
                { L_,   ""             },
                { L_,   "z"            },
                { L_,   "zO"           },
                { L_,   "C"            },
                { L_,   "sS"           },
                { L_,   "iI"           },
                { L_,   "zzCszSzizIz"  },
                { L_,   "zOCsSiI"      }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory.

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const char *const SPEC   = DATA[ti].d_spec;

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                UnsignedObj        mZ(&scratch);
                const UnsignedObj& Z  = gg(&mZ, SPEC);
                UnsignedObj        mZZ(&scratch);
                const UnsignedObj& ZZ = gg(&mZZ, SPEC);

                if (veryVerbose) { T_ P_(Z) P(ZZ) }

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    UnsignedObj          *objPtr;
                    bslma::TestAllocator *objAllocatorPtr;

                    switch (CONFIG) {
                      case 'a': {
                        objPtr = new (fa) UnsignedObj(Z);
                        objAllocatorPtr = &da;
                      } break;
                      case 'b': {
                        objPtr = new (fa) UnsignedObj(Z, 0);
                        objAllocatorPtr = &da;
                      } break;
                      case 'c': {
                        objPtr = new (fa) UnsignedObj(Z, &sa);
                        objAllocatorPtr = &sa;
                      } break;
                      default: {
                        LOOP_ASSERT(CONFIG, !"Bad allocator config.");
                      } break;
                    }
                    LOOP2_ASSERT(LINE, CONFIG,
                                 sizeof(UnsignedObj) == fa.numBytesInUse());

                    UnsignedObj& mX = *objPtr;  const UnsignedObj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                    // Ensure the first row of the table contains the
                    // default-constructed value.

                    static bool firstFlag = true;
                    if (firstFlag) {
                        LOOP4_ASSERT(LINE, CONFIG, UnsignedObj(), *objPtr,
                                     UnsignedObj() == *objPtr)
                        firstFlag = false;
                    }

                    // Verify the value of the object.

                    LOOP4_ASSERT(LINE, CONFIG,  X, Z,  X == Z);

                    // Verify that the value of 'Z' has not changed.

                    LOOP4_ASSERT(LINE, CONFIG, ZZ, Z, ZZ == Z);

                    // Also apply the object's 'allocator' accessor, as well as
                    // that of 'Z'.

                    LOOP4_ASSERT(LINE, CONFIG, &oa, X.allocator(),
                                 &oa == X.allocator());

                    LOOP4_ASSERT(LINE, CONFIG, &scratch, Z.allocator(),
                                 &scratch == Z.allocator());

                    // Verify no allocation from the non-object allocator.

                    LOOP3_ASSERT(LINE, CONFIG, noa.numBlocksTotal(),
                                 0 == noa.numBlocksTotal());

                    // Verify memory is always allocated (because of bsl::set)
                    // except when default constructed (SPEC == "").

                    if (0 == ti) {
                        LOOP3_ASSERT(LINE, CONFIG, oa.numBlocksInUse(),
                                     0 == oa.numBlocksInUse());
                    }
                    else {
                        LOOP3_ASSERT(LINE, CONFIG, oa.numBlocksInUse(),
                                     0 != oa.numBlocksInUse());
                    }

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    LOOP3_ASSERT(LINE, CONFIG, fa.numBlocksInUse(),
                                 0 == fa.numBlocksInUse());
                    LOOP3_ASSERT(LINE, CONFIG, da.numBlocksInUse(),
                                 0 == da.numBlocksInUse());
                    LOOP3_ASSERT(LINE, CONFIG, sa.numBlocksInUse(),
                                 0 == sa.numBlocksInUse());
                }  // end foreach configuration

            }  // end foreach row

            // Double check that at least some object memory got allocated.

            ASSERT(anyObjectMemoryAllocatedFlag);

            // Note that memory should be independently allocated for each
            // attribute capable of allocating memory.

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const char *const SPEC   = DATA[ti].d_spec;

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                UnsignedObj        mZ(&scratch);
                const UnsignedObj& Z = gg(&mZ, SPEC);

                if (veryVerbose) { T_ P_(LINE) P(Z) }

                bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    UnsignedObj obj(Z, &oa);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP2_ASSERT(LINE, da.numBlocksInUse(), !da.numBlocksInUse());
                LOOP2_ASSERT(LINE, oa.numBlocksInUse(), !oa.numBlocksInUse());
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:   their corresponding salient attributes respectively compare
        //:   equal.
        //:
        //: 2 The capacity and storage method of the objects does not affect
        //:   the result of the equality comparison.
        //:
        //: 3 Non-modifiable objects can be compared (i.e., objects providing
        //:   only non-modifiable access).
        //:
        //: 4 No memory allocation occurs as a result of comparison (e.g., the
        //:   arguments are not passed by value).
        //:
        //: 5 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 6 The return type of the equality comparison operators is 'bool'.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate structure for
        //:   the two homogeneous, free equality comparison operators defined
        //:   in this component.  (C-5,6)
        //:
        //: 2 Use the default allocator for all object creation and verify
        //:   the comparison operators do not allocate memory.  (C-4)
        //:
        //: 3 Specify a set of specifications for the 'gg' function that result
        //:   in an empty object but with different capacity and bytes per
        //:   element characteristics.
        //:
        //: 4 Specify a set of specifications for distinct object values.
        //:
        //: 5 For every item in the cross-product of these two sets, verify
        //:   the result of the operators on the item to itself and the item to
        //:   every other item.  (C-1..3)
        //
        // Testing:
        //   bool isEqual(const PackedIntArray& other) const;
        //   bool operator==(lhs, rhs);
        //   bool operator!=(lhs, rhs);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                         << "EQUALITY OPERATORS" << endl
                         << "==================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            using namespace bdlc;
            typedef bool (*operatorPtr)(const Obj&, const Obj&);
            // Verify that the signature and return type is standard.

            operatorPtr operatorEq = operator==;
            operatorPtr operatorNe = operator!=;

            // Quash unused variable warning.

            Obj a;
            Obj b;
            ASSERT(true == operatorEq(a, b));
            ASSERT(false == operatorNe(a, b));
        }
        {
            using namespace bdlc;
            typedef bool (*operatorPtr)(const UnsignedObj&,
                                        const UnsignedObj&);
            // Verify that the signature and return type is standard.

            operatorPtr operatorEq = operator==;
            operatorPtr operatorNe = operator!=;

            // Quash unused variable warning.

            UnsignedObj a;
            UnsignedObj b;
            ASSERT(true == operatorEq(a, b));
            ASSERT(false == operatorNe(a, b));
        }

        if (verbose) cout << "\nVerify the operators for the signed array."
                          << endl;

        {
            static const struct {
                int         d_lineNum;
                const char *d_spec_p;
            } INIT[] = {
                // line spec
                //----  ----
                { L_,   ""          },
                { L_,   "Lp"        },
                { L_,   "LLLLLLx"   },
                { L_,   "LLLLLLxLp" },
            };
            const int NUM_INIT = static_cast<int>(sizeof INIT / sizeof *INIT);

            static const struct {
                int         d_lineNum;
                const char *d_spec_p;
            } DATA[] = {
                // line spec
                //----  ----
                { L_,   ""          },
                { L_,   "zoO"       },
                { L_,   "cC"        },
                { L_,   "sS"        },
                { L_,   "iI"        },
                { L_,   "zoOcC"     },
                { L_,   "zoOsS"     },
                { L_,   "zoOcCsSiI" },
                { L_,   "IiSsCcOoz" },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                for (int si = 0; si < NUM_INIT ; ++si) {
                    const int   LINE1 = DATA[ti].d_lineNum;
                    bsl::string SPEC1 = (bsl::string()
                                         + INIT[si].d_spec_p
                                         + DATA[ti].d_spec_p);

                    Obj mX;  const Obj& X = gg(&mX, SPEC1.c_str());

                    for (int tj = 0; tj < NUM_DATA; ++tj) {
                        for (int sj = 0; sj < NUM_INIT ; ++sj) {
                            const int   LINE2 = DATA[tj].d_lineNum;
                            bsl::string SPEC2 = (bsl::string()
                                                 + INIT[sj].d_spec_p
                                                 + DATA[tj].d_spec_p);

                            if (veryVerbose) { P_(SPEC1) P(SPEC2) }

                            Obj mY;  const Obj& Y = gg(&mY, SPEC2.c_str());

                            bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                            LOOP2_ASSERT(LINE1,
                                         LINE2,
                                         X.isEqual(Y) == (ti == tj));
                            LOOP2_ASSERT(LINE1, LINE2, (X == Y) == (ti == tj));
                            LOOP2_ASSERT(LINE1, LINE2, (X != Y) == (ti != tj));

                            LOOP2_ASSERT(LINE1,
                                         LINE2,
                                         defaultAllocator.numAllocations()
                                                               == allocations);
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\nVerify the operators for the unsigned array."
                          << endl;

        {
            static const struct {
                int         d_lineNum;
                const char *d_spec_p;
            } INIT[] = {
                // line spec
                //----  ----
                { L_,   ""          },
                { L_,   "Lp"        },
                { L_,   "LLLLLLx"   },
                { L_,   "LLLLLLxLp" },
            };
            const int NUM_INIT = static_cast<int>(sizeof INIT / sizeof *INIT);

            static const struct {
                int         d_lineNum;
                const char *d_spec_p;
            } DATA[] = {
                // line spec
                //----  ----
                { L_,   ""          },
                { L_,   "zO"        },
                { L_,   "C"         },
                { L_,   "sS"        },
                { L_,   "iI"        },
                { L_,   "zOC"       },
                { L_,   "zOsS"      },
                { L_,   "zOCsSiI"   },
                { L_,   "IiSsCOOz"  },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                for (int si = 0; si < NUM_INIT ; ++si) {
                    const int   LINE1 = DATA[ti].d_lineNum;
                    bsl::string SPEC1 = (bsl::string()
                                         + INIT[si].d_spec_p
                                         + DATA[ti].d_spec_p);

                    UnsignedObj        mX;
                    const UnsignedObj& X = gg(&mX, SPEC1.c_str());

                    for (int tj = 0; tj < NUM_DATA; ++tj) {
                        for (int sj = 0; sj < NUM_INIT ; ++sj) {
                            const int   LINE2 = DATA[tj].d_lineNum;
                            bsl::string SPEC2 = (bsl::string()
                                                 + INIT[sj].d_spec_p
                                                 + DATA[tj].d_spec_p);

                            if (veryVerbose) { P_(SPEC1) P(SPEC2) }

                            UnsignedObj        mY;
                            const UnsignedObj& Y = gg(&mY, SPEC2.c_str());

                            bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                            LOOP2_ASSERT(LINE1,
                                         LINE2,
                                         X.isEqual(Y) == (ti == tj));
                            LOOP2_ASSERT(LINE1, LINE2, (X == Y) == (ti == tj));
                            LOOP2_ASSERT(LINE1, LINE2, (X != Y) == (ti != tj));

                            LOOP2_ASSERT(LINE1,
                                         LINE2,
                                         defaultAllocator.numAllocations()
                                                               == allocations);
                        }
                    }
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.
        //:
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)',
        //:   but with each "attributeName = " elided.
        //:
        //: 4 The 'print' method signature and return type are standard.
        //:
        //: 5 The 'print' method returns the supplied 'ostream'.
        //:
        //: 6 The optional 'level' and 'spacesPerLevel' parameters have the
        //:   correct default values.
        //:
        //: 7 The output 'operator<<' signature and return type are standard.
        //:
        //: 8 The output 'operator<<' returns the supplied 'ostream'.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-4,7)
        //:
        //: 2 Using the table-driven technique: (C-1..3,5..6,8)
        //:
        //:   1 Define fourteen carefully selected combinations of (two) object
        //:     values ('A' and 'B'), having distinct values for each
        //:     corresponding salient attribute, and various values for the two
        //:     formatting parameters, along with the expected output ( 'value'
        //:     x 'level' x 'spacesPerLevel' ):
        //:     1 { A } x { 0 } x { 0, 1, -1, -8 } --> 3 expected o/ps
        //:     2 { A } x { 3, -3 } x { 0, 2, -2, -8 } --> 8 expected o/ps
        //:     3 { B } x { 2 } x { 3 } --> 1 expected op
        //:     4 { A B } x { -8 } x { -8 } --> 2 expected o/ps
        //:     4 { A B } x { -9 } x { -9 } --> 2 expected o/ps
        //:
        //:   2 For each row in the table defined in P-2.1: (C-1..3,5,7)
        //:
        //:     1 Using a 'const' 'Obj', supply each object value and pair of
        //:       formatting parameters to 'print', omitting the 'level' or
        //:       'spacesPerLevel' parameter if the value of that argument is
        //:       '-8'.  If the parameters are, arbitrarily, (-9, -9), then
        //:       invoke the 'operator<<' instead.
        //:
        //:     2 Use a standard 'ostringstream' to capture the actual output.
        //:
        //:     3 Verify the address of what is returned is that of the
        //:       supplied stream.  (C-5,8)
        //:
        //:     4 Compare the contents captured in P-2.2.2 with what is
        //:       expected.  (C-1..3,6)
        //
        // Testing:
        //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
        //   ostream& operator<<(ostream& stream, const PackedIntArray& array);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR" << endl
                          << "=========================" << endl;

        if (verbose) cout << "\nAssign the addresses of 'print' and "
                             "the output 'operator<<' to variables." << endl;
        {
            using namespace bdlc;

            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;

            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOp  = operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOp;
        }
        {
            using namespace bdlc;

            typedef ostream& (UnsignedObj::*funcPtr)(ostream&, int, int) const;

            typedef ostream& (*operatorPtr)(ostream&, const UnsignedObj&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &UnsignedObj::print;
            operatorPtr operatorOp  = operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOp;
        }

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations." << endl;
        static const struct {
            int         d_line;           // source line number
            int         d_level;
            int         d_spacesPerLevel;
            bsl::size_t d_numElems;
            const int   d_array [5];

            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"
#define SP " "

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1, -8 } -->  4 expected o/ps
        // ------------------------------------------------------------------

        //LINE  L  SPL   NE   ARRAY       EXP
        //----  -  ---   --   -----       ---

        { L_,   0,   0,   0,  {},         "["                                NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   1,   0,  {},         "["                                NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   -1,  0,  {},         "["                                SP
                                          "]"                                },
        { L_,   0,   -8,  0,  {},         "["                                NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   0,   1,  {0},        "["                                NL
                                          "0"                                NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   1,   1,  {0},        "["                                NL
                                          " 0"                               NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   -1,  1,  {0},        "["                                SP
                                          "0"                                SP
                                          "]"                                },
        { L_,   0,   -8,  1,  {0},        "["                                NL
                                          "    0"                            NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   0,   3,  {0,1,-1},   "["                                NL
                                          "0"                                NL
                                          "1"                                NL
                                          "-1"                               NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   1,   3,  {0,1,-1},   "["                                NL
                                          " 0"                               NL
                                          " 1"                               NL
                                          " -1"                              NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   -1,  3,  {0,1,-1},   "["                                SP
                                          "0"                                SP
                                          "1"                                SP
                                          "-1"                               SP
                                          "]"                                },
        { L_,   0,   -8,  3,  {0,1,-1},   "["                                NL
                                          "    0"                            NL
                                          "    1"                            NL
                                          "    -1"                           NL
                                          "]"                                NL
                                                                             },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2, -8 } -->  6 expected o/ps
        // ------------------------------------------------------------------

        //LINE  L  SPL   NE   ARRAY       EXP
        //----  -  ---   --   -----       ---

        { L_,   3,   0,   0,  {},         "["                                NL
                                          "]"                                NL
                                                                             },
        { L_,   3,   2,   0,  {},         "      ["                          NL
                                          "      ]"                          NL
                                                                             },
        { L_,   3,   -2,  0,  {},         "      ["                          SP
                                          "]"                                },
        { L_,   3,   -8,  0,  {},         "            ["                    NL
                                          "            ]"                    NL
                                                                             },
        { L_,   3,   0,   1,  {0},        "["                                NL
                                          "0"                                NL
                                          "]"                                NL
                                                                             },
        { L_,   3,   2,   1,  {0},        "      ["                          NL
                                          "        0"                        NL
                                          "      ]"                          NL
                                                                             },
        { L_,   3,   -2,  1,  {0},        "      ["                          SP
                                          "0"                                SP
                                          "]"                                },
        { L_,   3,   -8,  1,  {0},        "            ["                    NL
                                          "                0"                NL
                                          "            ]"                    NL
                                                                             },
        { L_,   3,   0,   3,  {0,1,-1},   "["                                NL
                                          "0"                                NL
                                          "1"                                NL
                                          "-1"                               NL
                                          "]"                                NL
                                                                             },
        { L_,   3,   2,   3,  {0,1,-1},   "      ["                          NL
                                          "        0"                        NL
                                          "        1"                        NL
                                          "        -1"                       NL
                                          "      ]"                          NL
                                                                             },
        { L_,   3,   -2,  3,  {0,1,-1},   "      ["                          SP
                                          "0"                                SP
                                          "1"                                SP
                                          "-1"                               SP
                                          "]"                                },
        { L_,   3,   -8,  3,  {0,1,-1},   "            ["                    NL
                                          "                0"                NL
                                          "                1"                NL
                                          "                -1"               NL
                                          "            ]"                    NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 }     x { 3 }            -->  1 expected o/p
        // -----------------------------------------------------------------

        //LINE  L  SPL   NE   ARRAY       EXP
        //----  -  ---   --   -----       ---

        { L_,  2,  3,   3,  {0,1,-1},     "      ["                          NL
                                          "         0"                       NL
                                          "         1"                       NL
                                          "         -1"                      NL
                                          "      ]"                          NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -8 }   x { -8 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE  L  SPL   NE   ARRAY       EXP
        //----  -  ---   --   -----       ---

        { L_,  -8,  -8,  1,   {0},        "["                                NL
                                          "    0"                            NL
                                          "]"                                NL
                                                                             },
        { L_,  -8,  -8,  3,   {0,1,-1},   "["                                NL
                                          "    0"                            NL
                                          "    1"                            NL
                                          "    -1"                           NL
                                          "]"                                NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.5: { A B } x { -9 }   x { -9 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL  OFF   FLAG  DESC  EXP
        //---- - ---  ---   ----  ----  ---

        // { L_,  -9,  -9,  1,   {0},        "[ 0 ]"                         },
        // { L_,  -9,  -9,  3,   {0,1,-1},   "[ 0 1 -1 ]"                    },

#undef NL
#undef SP

    };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE      = DATA[ti].d_line;
                const int         L         = DATA[ti].d_level;
                const int         SPL       = DATA[ti].d_spacesPerLevel;
                const bsl::size_t NUM_ELEMS = DATA[ti].d_numElems;
                const int  *const ARRAY    =  DATA[ti].d_array;
                const char *const EXP       = DATA[ti].d_expected_p;

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                Obj mX;  const Obj& X = mX;
                for (bsl::size_t i = 0; i < NUM_ELEMS; ++i) {
                    if (veryVerbose) { P_(LINE) P(i) }

                    mX.append(ARRAY[i]);
                }

                ostringstream os;

                // Verify supplied stream is returned by reference.

                if (-9 == L && -9 == SPL) {
                    LOOP_ASSERT(LINE, &os == &(os << X));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {
                    LOOP_ASSERT(LINE, -8 == SPL || -8 != L);

                    if (-8 != SPL) {
                        LOOP_ASSERT(LINE, &os == &X.print(os, L, SPL));
                    }
                    else if (-8 != L) {
                        LOOP_ASSERT(LINE, &os == &X.print(os, L));
                    }
                    else {
                        LOOP_ASSERT(LINE, &os == &X.print(os));
                    }

                    if (veryVeryVerbose) { T_ T_ Q(print) }
                }

                // Verify output is formatted as expected.

                if (veryVerbose) {
                    cout << "\t\t" << LINE << endl;
                    cout << "PRINT STRING" << endl;
                    for ( bsl::size_t i = 0; i < os.str().size(); ++i) {
                        cout << static_cast<int>(os.str()[i]) << " ";
                    }
                    cout << "\nPRINT EXP" << endl;
                    for ( bsl::size_t i = 0; i < strlen(EXP); ++i) {
                        cout << static_cast<int>(EXP[i]) << " ";
                    }
                    cout << "\n----------------------------------------\n\n";
                }
                LOOP3_ASSERT(LINE, EXP, os.str(), EXP == os.str());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding attribute of
        //:   the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //:
        //: 3 No accessor allocates any memory.
        //
        // Plan:
        //: 1 To test 'allocator', create object with various allocators and
        //:   ensure the returned value matches the supplied allocator.
        //:
        //: 2 Use the generator function to produce objects of arbitrary state
        //:   and verify the accessors' return values against expected values.
        //:   (C-1)
        //:
        //: 3 The accessors will only be accessed from a 'const' reference to
        //:   the created object.  (C-2)
        //:
        //: 4 The default allocator will be used for all created objects
        //:   (excluding those used to test 'allocator') and the number of
        //:   allocation will be verified to ensure that no memory was
        //:   allocated during use of the accessors.  (C-3)
        //
        // Testing:
        //   TYPE operator[](bsl::size_t index) const;
        //   bslma::Allocator *allocator() const;
        //   int bytesPerElement() const;
        //   bsl::size_t capacity() const;
        //   bool isEmpty() const;
        //   bsl::size_t length() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout << "\nTesting 'allocator'." << endl;
        {
            Obj mX;  const Obj& X = mX;
            ASSERT(&defaultAllocator == X.allocator());
        }
        {
            Obj        mX(reinterpret_cast<bslma::TestAllocator *>(0));
            const Obj& X = mX;
            ASSERT(&defaultAllocator == X.allocator());
        }
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;
            ASSERT(&sa == X.allocator());
        }

        if (verbose) cout << "\nTesting residual basic accessors." << endl;
        {
            const int SZ = 10;

            static const struct {
                int          d_lineNum;          // source line number
                const char  *d_spec_p;           // specification string
                bsl::size_t  d_length;           // expected length
                bsl::size_t  d_capacity;         // expected capacity
                int          d_bytesPerElement;  // expected bytes per element
                Element      d_elements[SZ];     // expected element values
            } DATA[] = {
                //line  spec      len  cap  bpe  elements
                //----  --------  ---  ---  ---  ----------------
                { L_,   "",         0,   0,   1, {}                          },
                { L_,   "z",        1,   1,   1, { 0 }                       },
                { L_,   "o",        1,   1,   1, { -1 }                      },
                { L_,   "O",        1,   1,   1, { 1 }                       },
                { L_,   "c",        1,   1,   1, { k_INT8_MIN }              },
                { L_,   "C",        1,   1,   1, { k_INT8_MAX }              },
                { L_,   "s",        1,   1,   2, { k_INT16_MIN }             },
                { L_,   "S",        1,   1,   2, { k_INT16_MAX }             },
                { L_,   "i",        1,   1,   4, { k_INT32_MIN }             },
                { L_,   "I",        1,   1,   4, { k_INT32_MAX }             },
                { L_,   "l",        1,   1,   8, { k_INT64_MIN }             },
                { L_,   "L",        1,   1,   8, { k_INT64_MAX }             },
                { L_,   "^",        1,   1,   2, { k_INT8_MIN - 1 }          },
                { L_,   "!",        1,   1,   2, { k_INT8_MAX + 1 }          },
                { L_,   "@",        1,   1,   4, { k_INT16_MIN - 1 }         },
                { L_,   "$",        1,   1,   4, { k_INT16_MAX + 1 }         },
                { L_,   "(",        1,   1,   8, { k_INT32_MIN - 1 }         },
                { L_,   ")",        1,   1,   8, { k_INT32_MAX + 1 }         },
                { L_,   "Lx",       0,  10,   1, {}                          },
                { L_,   "LxS",      1,   5,   2, { k_INT16_MAX }             },
                { L_,   "LxSx",     0,  10,   1, {}                          },
                { L_,   "CSI",      3,   4,   4,
                                    { k_INT8_MAX, k_INT16_MAX, k_INT32_MAX } },
                { L_,   "CSIx",     0,  16,   1, {}                          }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int            LINE = DATA[ti].d_lineNum;
                const char *const    SPEC = DATA[ti].d_spec_p;
                const bsl::size_t    LEN  = DATA[ti].d_length;
                const bsl::size_t    CAP  = DATA[ti].d_capacity;
                const int            BPE  = DATA[ti].d_bytesPerElement;
                const Element *const EXP  = DATA[ti].d_elements;

                Obj mX;  const Obj& X = gg(&mX, SPEC);   // original spec

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                LOOP_ASSERT(LINE, LEN == X.length());
                LOOP_ASSERT(LINE, (0 == LEN) == X.isEmpty());
                LOOP_ASSERT(LINE, CAP == X.capacity());
                LOOP_ASSERT(LINE, BPE == X.bytesPerElement());
                for (bsl::size_t i = 0; i < LEN; i++) {
                    if (veryVerbose) { P_(EXP[i]) P(X[i]) }

                    LOOP_ASSERT(LINE, EXP[i] == X[i]);
                }

                LOOP_ASSERT(LINE,
                            defaultAllocator.numAllocations() == allocations);
            }
        }
        {
            const int SZ = 10;

            static const struct {
                int              d_lineNum;          // source line number
                const char      *d_spec_p;           // specification string
                bsl::size_t      d_length;           // expected length
                bsl::size_t      d_capacity;         // expected capacity
                int              d_bytesPerElement;  // expected bytes per elem
                UnsignedElement  d_elements[SZ];     // expected element values
            } DATA[] = {
                //line  spec      len  cap  bpe  elements
                //----  --------  ---  ---  ---  ----------------
                { L_,   "",         0,   0,   1, {}                          },
                { L_,   "z",        1,   1,   1, { 0 }                       },
                { L_,   "O",        1,   1,   1, { 1 }                       },
                { L_,   "C",        1,   1,   1, { k_UINT8_MAX }             },
                { L_,   "S",        1,   1,   2, { k_UINT16_MAX }            },
                { L_,   "I",        1,   1,   4, { k_UINT32_MAX }            },
                { L_,   "L",        1,   1,   8, { k_UINT64_MAX }            },
                { L_,   "s",        1,   1,   2, { k_UINT8_MAX + 1 }         },
                { L_,   "i",        1,   1,   4, { k_UINT16_MAX + 1 }        },
                { L_,   "l",        1,   1,   8, { k_UINT32_MAX + 1 }        },
                { L_,   "Lx",       0,  10,   1, {}                          },
                { L_,   "LxS",      1,   5,   2, { k_UINT16_MAX }            },
                { L_,   "LxSx",     0,  10,   1, {}                          },
                { L_,   "CSI",      3,   4,   4,
                                 { k_UINT8_MAX, k_UINT16_MAX, k_UINT32_MAX } },
                { L_,   "CSIx",     0,  16,   1, {}                          }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int                    LINE = DATA[ti].d_lineNum;
                const char *const            SPEC = DATA[ti].d_spec_p;
                const bsl::size_t            LEN  = DATA[ti].d_length;
                const bsl::size_t            CAP  = DATA[ti].d_capacity;
                const int                    BPE  = DATA[ti].d_bytesPerElement;
                const UnsignedElement *const EXP  = DATA[ti].d_elements;

                UnsignedObj        mX;
                const UnsignedObj& X = gg(&mX, SPEC);   // original spec

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                LOOP_ASSERT(LINE, LEN == X.length());
                LOOP_ASSERT(LINE, (0 == LEN) == X.isEmpty());
                LOOP_ASSERT(LINE, CAP == X.capacity());
                LOOP_ASSERT(LINE, BPE == X.bytesPerElement());
                for (bsl::size_t i = 0; i < LEN; i++) {
                    if (veryVerbose) { P_(EXP[i]) P(X[i]) }

                    LOOP_ASSERT(LINE, EXP[i] == X[i]);
                }

                LOOP_ASSERT(LINE,
                            defaultAllocator.numAllocations() == allocations);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PRIMITIVE GENERATOR FUNCTIONS
        //   Ensure that the generator functions are able to create an object
        //   in any state.
        //
        // Concerns:
        //: 1 Valid syntax produces the expected results.
        //:
        //: 2 Invalid syntax is detected and reported.
        //
        // Plan:
        //: 1 Evaluate a series of test strings of increasing complexity to
        //:   set the state of a newly created object and verify the returned
        //:   object using basic accessors.  (C-1)
        //:
        //: 2 Evaluate the 'ggg' function with a series of test strings of
        //:   increasing complexity and verify its return value.  (C-2)
        //
        // Testing:
        //    Obj& gg(Obj *object, const char *spec);
        //    UnsignedObj& gg(UnsignedObj *object, const char *spec);
        //    int ggg(Obj *object, const char *spec);
        //    int ggg(UnsignedObj *object, const char *spec);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMITIVE GENERATOR FUNCTIONS" << endl
                          << "=============================" << endl;

        if (verbose) cout << "\nTesting generator on valid specs." << endl;
        {
            const int SZ = 10;

            static const struct {
                int          d_lineNum;          // source line number
                const char  *d_spec_p;           // specification string
                bsl::size_t  d_length;           // expected length
                bsl::size_t  d_capacity;         // expected capacity
                int          d_bytesPerElement;  // expected bytes per element
                Element      d_elements[SZ];     // expected element values
            } DATA[] = {
                //line  spec      len  cap  bpe  elements
                //----  --------  ---  ---  ---  ----------------
                { L_,   "",         0,   0,   1, {}                          },
                { L_,   "z",        1,   1,   1, { 0 }                       },
                { L_,   "o",        1,   1,   1, { -1 }                      },
                { L_,   "O",        1,   1,   1, { 1 }                       },
                { L_,   "c",        1,   1,   1, { k_INT8_MIN }              },
                { L_,   "C",        1,   1,   1, { k_INT8_MAX }              },
                { L_,   "s",        1,   1,   2, { k_INT16_MIN }             },
                { L_,   "S",        1,   1,   2, { k_INT16_MAX }             },
                { L_,   "i",        1,   1,   4, { k_INT32_MIN }             },
                { L_,   "I",        1,   1,   4, { k_INT32_MAX }             },
                { L_,   "l",        1,   1,   8, { k_INT64_MIN }             },
                { L_,   "L",        1,   1,   8, { k_INT64_MAX }             },
                { L_,   "^",        1,   1,   2, { k_INT8_MIN - 1 }          },
                { L_,   "!",        1,   1,   2, { k_INT8_MAX + 1 }          },
                { L_,   "@",        1,   1,   4, { k_INT16_MIN - 1 }         },
                { L_,   "$",        1,   1,   4, { k_INT16_MAX + 1 }         },
                { L_,   "(",        1,   1,   8, { k_INT32_MIN - 1 }         },
                { L_,   ")",        1,   1,   8, { k_INT32_MAX + 1 }         },
                { L_,   "Lx",       0,  10,   1, {}                          },
                { L_,   "LxS",      1,   5,   2, { k_INT16_MAX }             },
                { L_,   "LxSx",     0,  10,   1, {}                          },
                { L_,   "CSI",      3,   4,   4,
                                    { k_INT8_MAX, k_INT16_MAX, k_INT32_MAX } },
                { L_,   "CSIx",     0,  16,   1, {}                          },
                { L_,   "Lp",       0,   1,   8, {}                          },
                { L_,   "LpS",      1,   1,   8, { k_INT16_MAX }             },
                { L_,   "LpSp",     0,   1,   8, {}                          },
                { L_,   "CSIp",     2,   4,   4, { k_INT8_MAX, k_INT16_MAX } }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int            LINE = DATA[ti].d_lineNum;
                const char *const    SPEC = DATA[ti].d_spec_p;
                const bsl::size_t    LEN  = DATA[ti].d_length;
                const bsl::size_t    CAP  = DATA[ti].d_capacity;
                const int            BPE  = DATA[ti].d_bytesPerElement;
                const Element *const EXP  = DATA[ti].d_elements;

                Obj mX;  const Obj& X = gg(&mX, SPEC);   // original spec

                LOOP_ASSERT(LINE, LEN == X.length());
                LOOP_ASSERT(LINE, CAP == X.capacity());
                LOOP_ASSERT(LINE, BPE == X.bytesPerElement());
                for (bsl::size_t i = 0; i < LEN; i++) {
                    if (veryVerbose) { P_(EXP[i]) P(X[i]) }

                    LOOP_ASSERT(LINE, EXP[i] == X[i]);
                }
            }
        }
        {
            const int SZ = 10;

            static const struct {
                int              d_lineNum;          // source line number
                const char      *d_spec_p;           // specification string
                bsl::size_t      d_length;           // expected length
                bsl::size_t      d_capacity;         // expected capacity
                int              d_bytesPerElement;  // expected bytes per elem
                UnsignedElement  d_elements[SZ];     // expected element values
            } DATA[] = {
                //line  spec      len  cap  bpe  elements
                //----  --------  ---  ---  ---  ----------------
                { L_,   "",         0,   0,   1, {}                          },
                { L_,   "z",        1,   1,   1, { 0 }                       },
                { L_,   "O",        1,   1,   1, { 1 }                       },
                { L_,   "C",        1,   1,   1, { k_UINT8_MAX }             },
                { L_,   "S",        1,   1,   2, { k_UINT16_MAX }            },
                { L_,   "I",        1,   1,   4, { k_UINT32_MAX }            },
                { L_,   "L",        1,   1,   8, { k_UINT64_MAX }            },
                { L_,   "s",        1,   1,   2, { k_UINT8_MAX + 1 }         },
                { L_,   "i",        1,   1,   4, { k_UINT16_MAX + 1 }        },
                { L_,   "l",        1,   1,   8, { k_UINT32_MAX + 1 }        },
                { L_,   "Lx",       0,  10,   1, {}                          },
                { L_,   "LxS",      1,   5,   2, { k_UINT16_MAX }            },
                { L_,   "LxSx",     0,  10,   1, {}                          },
                { L_,   "CSI",      3,   4,   4,
                                 { k_UINT8_MAX, k_UINT16_MAX, k_UINT32_MAX } },
                { L_,   "CSIx",     0,  16,   1, {}                          },
                { L_,   "Lp",       0,   1,   8, {}                          },
                { L_,   "LpS",      1,   1,   8, { k_UINT16_MAX }            },
                { L_,   "LpSp",     0,   1,   8, {}                          },
                { L_,   "CSIp",     2,   4,   4,
                                               { k_UINT8_MAX, k_UINT16_MAX } }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int                    LINE = DATA[ti].d_lineNum;
                const char *const            SPEC = DATA[ti].d_spec_p;
                const bsl::size_t            LEN  = DATA[ti].d_length;
                const bsl::size_t            CAP  = DATA[ti].d_capacity;
                const int                    BPE  = DATA[ti].d_bytesPerElement;
                const UnsignedElement *const EXP  = DATA[ti].d_elements;

                UnsignedObj        mX;
                const UnsignedObj& X = gg(&mX, SPEC);   // original spec

                LOOP_ASSERT(LINE, LEN == X.length());
                LOOP_ASSERT(LINE, CAP == X.capacity());
                LOOP_ASSERT(LINE, BPE == X.bytesPerElement());
                for (bsl::size_t i = 0; i < LEN; i++) {
                    if (veryVerbose) { P_(EXP[i]) P(X[i]) }

                    LOOP_ASSERT(LINE, EXP[i] == X[i]);
                }
            }
        }

        if (verbose) cout << "\nTesting generator on invalid specs." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_index;    // offending character index
            } DATA[] = {
                //line  spec            index
                //----  -------------   -----
                { L_,   "",             -1,     }, // control

                { L_,   "C",            -1,     }, // control
                { L_,   " ",             0,     },
                { L_,   ".",             0,     },
                { L_,   "2",             0,     },

                { L_,   "CS",           -1,     }, // control
                { L_,   " C",            0,     },
                { L_,   ".C",            0,     },
                { L_,   "2C",            0,     },
                { L_,   "C ",            1,     },
                { L_,   "C.",            1,     },
                { L_,   "C2",            1,     },

                { L_,   "CSI",          -1,     }, // control
                { L_,   " CS",           0,     },
                { L_,   ".CS",           0,     },
                { L_,   "2CS",           0,     },
                { L_,   "C S",           1,     },
                { L_,   "C.S",           1,     },
                { L_,   "C2S",           1,     },
                { L_,   "CS ",           2,     },
                { L_,   "CS.",           2,     },
                { L_,   "CS2",           2,     },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          LINE  = DATA[ti].d_lineNum;
                const char *const  SPEC  = DATA[ti].d_spec_p;
                const int          INDEX = DATA[ti].d_index;

                if (veryVerbose) { P_(SPEC) P(INDEX) }

                {
                    Obj mX;

                    int result = ggg(&mX, SPEC, veryVerbose);
                    LOOP_ASSERT(LINE, INDEX == result);
                }
                {
                    UnsignedObj mX;

                    int result = ggg(&mX, SPEC, veryVerbose);
                    LOOP_ASSERT(LINE, INDEX == result);
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS TEST
        //   The basic concern is that the default constructor, the destructor,
        //   and, under normal conditions (i.e., no aliasing), the primary
        //   manipulators:
        //      - append
        //      - pop_back
        //      - removeAll
        //   operate as expected.
        //
        // Concerns:
        //: 1 The default constructor creates the correct initial value and has
        //:   the internal memory management system hooked up properly so that
        //:   *all* internally allocated memory draws from the same
        //:   user-supplied allocator whenever one is specified.
        //:
        //: 2 The method 'append' produces the expected value, increases
        //:   capacity as needed, and is exception neutral with respect to
        //:   memory allocation.
        //:
        //: 3 The method 'removeAll' produces the expected value (empty) and
        //:   does not affect allocated memory.
        //:
        //: 4 The method 'pop_back' produces the expected value and does not
        //:   affect allocated memory or bytes per element.
        //:
        //: 5 Memory is not leaked by any method and the destructor properly
        //:   deallocates the residual allocated memory.
        //
        // Plan:
        //: 1 Create an object using the default constructor with and without
        //:   passing in an allocator, verify the allocator is stored using the
        //:   (untested) 'allocator' accessor, and verifying all allocations
        //:   are done from the allocator in future tests.
        //:
        //: 2 Create objects (both signed and unsigned) using the
        //:   'bslma::TestAllocator', use the 'append' method with various
        //:   values, and the (untested) accessors to verify the value of the
        //:   object and that allocation occurred when expected.  Also vary the
        //:   test allocator's allocation limit to verify behavior in the
        //:   presence of exceptions.  (C-1,2)
        //:
        //: 3 Create objects using the 'bslma::TestAllocator', use the 'append'
        //:   to obtain various states, use 'removeAll', verify the objects are
        //:   empty, then repopulate the objects and ensure no allocation
        //:   occurs.  (C-3)
        //:
        //: 4 Create objects using the 'bslma::TestAllocator', use the 'append'
        //:   to obtain various states, use 'pop_all', verify the objects have
        //:   a reduced length but unchanged capacity.  (C-4)
        //:
        //: 5 Use a supplied 'bslma::TestAllocator' that goes out-of-scope
        //:   at the conclusion of each test to ensure all memory is returned
        //:   to the allocator.  (C-5)
        //
        // Testing:
        //   PackedIntArray(bslma::Allocator *basicAllocator = 0);
        //   ~PackedIntArray();
        //   void append(TYPE value);
        //   void pop_back();
        //   void removeAll();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS TEST" << endl
                          << "=========================" << endl;

        if (verbose) cout << "\nTesting with various allocator configurations."
                          << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            Obj mX;  const Obj& X = mX;
            ASSERT(&defaultAllocator == X.allocator());
            ASSERT(0 == X.length());
            ASSERT(0 == X.capacity());
            ASSERT(allocations == defaultAllocator.numAllocations());

            mX.append(0);
            ASSERT(1 == X.length());
            ASSERT(1 == X.capacity());
            ASSERT(allocations + 1 == defaultAllocator.numAllocations());
        }
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            Obj        mX(reinterpret_cast<bslma::TestAllocator *>(0));
            const Obj& X = mX;
            ASSERT(&defaultAllocator == X.allocator());
            ASSERT(0 == X.length());
            ASSERT(0 == X.capacity());
            ASSERT(allocations == defaultAllocator.numAllocations());

            mX.append(0);
            ASSERT(1 == X.length());
            ASSERT(1 == X.capacity());
            ASSERT(allocations + 1 == defaultAllocator.numAllocations());
        }
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;
            ASSERT(&sa == X.allocator());
            ASSERT(0 == X.length());
            ASSERT(0 == X.capacity());
            ASSERT(allocations == defaultAllocator.numAllocations());
            ASSERT(0 == sa.numAllocations());

            mX.append(0);
            ASSERT(1 == X.length());
            ASSERT(1 == X.capacity());
            ASSERT(allocations == defaultAllocator.numAllocations());
            ASSERT(1 == sa.numAllocations());
        }

        if (verbose) cout << "\nTesting 'append'." << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                Obj mX(&sa);  const Obj& X = mX;
                ASSERT( 0 == X.length());
                ASSERT( 0 == X.capacity());
                ASSERT( 1 == X.bytesPerElement());

                mX.append(0);
                ASSERT( 1 == X.length());
                ASSERT( 1 == X.capacity());
                ASSERT( 1 == X.bytesPerElement());
                ASSERT(        0 == X[0]);

                mX.append(k_INT8_MIN);
                ASSERT( 2 == X.length());
                ASSERT( 3 == X.capacity());
                ASSERT( 1 == X.bytesPerElement());
                ASSERT(        0 == X[0]);
                ASSERT( k_INT8_MIN == X[1]);

                mX.append(k_INT8_MAX);
                ASSERT( 3 == X.length());
                ASSERT( 3 == X.capacity());
                ASSERT( 1 == X.bytesPerElement());
                ASSERT(        0 == X[0]);
                ASSERT( k_INT8_MIN == X[1]);
                ASSERT( k_INT8_MAX == X[2]);

                mX.append(k_INT16_MIN);
                ASSERT( 4 == X.length());
                ASSERT( 5 == X.capacity());
                ASSERT( 2 == X.bytesPerElement());
                ASSERT(        0 == X[0]);
                ASSERT( k_INT8_MIN == X[1]);
                ASSERT( k_INT8_MAX == X[2]);
                ASSERT(k_INT16_MIN == X[3]);

                mX.append(k_INT16_MAX);
                ASSERT( 5 == X.length());
                ASSERT( 5 == X.capacity());
                ASSERT( 2 == X.bytesPerElement());
                ASSERT(        0 == X[0]);
                ASSERT( k_INT8_MIN == X[1]);
                ASSERT( k_INT8_MAX == X[2]);
                ASSERT(k_INT16_MIN == X[3]);
                ASSERT(k_INT16_MAX == X[4]);

                mX.append(k_INT32_MIN);
                ASSERT( 6 == X.length());
                ASSERT( 6 == X.capacity());
                ASSERT( 4 == X.bytesPerElement());
                ASSERT(        0 == X[0]);
                ASSERT( k_INT8_MIN == X[1]);
                ASSERT( k_INT8_MAX == X[2]);
                ASSERT(k_INT16_MIN == X[3]);
                ASSERT(k_INT16_MAX == X[4]);
                ASSERT(k_INT32_MIN == X[5]);

                mX.append(k_INT32_MAX);
                ASSERT( 7 == X.length());
                ASSERT( 9 == X.capacity());
                ASSERT( 4 == X.bytesPerElement());
                ASSERT(        0 == X[0]);
                ASSERT( k_INT8_MIN == X[1]);
                ASSERT( k_INT8_MAX == X[2]);
                ASSERT(k_INT16_MIN == X[3]);
                ASSERT(k_INT16_MAX == X[4]);
                ASSERT(k_INT32_MIN == X[5]);
                ASSERT(k_INT32_MAX == X[6]);

                mX.append(k_INT64_MIN);
                ASSERT( 8 == X.length());
                ASSERT(11 == X.capacity());
                ASSERT( 8 == X.bytesPerElement());
                ASSERT(        0 == X[0]);
                ASSERT( k_INT8_MIN == X[1]);
                ASSERT( k_INT8_MAX == X[2]);
                ASSERT(k_INT16_MIN == X[3]);
                ASSERT(k_INT16_MAX == X[4]);
                ASSERT(k_INT32_MIN == X[5]);
                ASSERT(k_INT32_MAX == X[6]);
                ASSERT(k_INT64_MIN == X[7]);

                mX.append(k_INT64_MAX);
                ASSERT( 9 == X.length());
                ASSERT(11 == X.capacity());
                ASSERT( 8 == X.bytesPerElement());
                ASSERT(        0 == X[0]);
                ASSERT( k_INT8_MIN == X[1]);
                ASSERT( k_INT8_MAX == X[2]);
                ASSERT(k_INT16_MIN == X[3]);
                ASSERT(k_INT16_MAX == X[4]);
                ASSERT(k_INT32_MIN == X[5]);
                ASSERT(k_INT32_MAX == X[6]);
                ASSERT(k_INT64_MIN == X[7]);
                ASSERT(k_INT64_MAX == X[8]);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(allocations == defaultAllocator.numAllocations());
        }

        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                UnsignedObj mX(&sa);  const UnsignedObj& X = mX;
                ASSERT(0 == X.length());
                ASSERT(0 == X.capacity());
                ASSERT(1 == X.bytesPerElement());

                mX.append(0);
                ASSERT(1 == X.length());
                ASSERT(1 == X.capacity());
                ASSERT(1 == X.bytesPerElement());
                ASSERT(         0 == X[0]);

                mX.append(k_UINT8_MAX);
                ASSERT(2 == X.length());
                ASSERT(3 == X.capacity());
                ASSERT(1 == X.bytesPerElement());
                ASSERT(         0 == X[0]);
                ASSERT( k_UINT8_MAX == X[1]);

                mX.append(k_UINT16_MAX);
                ASSERT(3 == X.length());
                ASSERT(3 == X.capacity());
                ASSERT(2 == X.bytesPerElement());
                ASSERT(         0 == X[0]);
                ASSERT( k_UINT8_MAX == X[1]);
                ASSERT(k_UINT16_MAX == X[2]);

                mX.append(k_UINT32_MAX);
                ASSERT(4 == X.length());
                ASSERT(4 == X.capacity());
                ASSERT(4 == X.bytesPerElement());
                ASSERT(         0 == X[0]);
                ASSERT( k_UINT8_MAX == X[1]);
                ASSERT(k_UINT16_MAX == X[2]);
                ASSERT(k_UINT32_MAX == X[3]);

                mX.append(k_UINT64_MAX);
                ASSERT(5 == X.length());
                ASSERT(7 == X.capacity());
                ASSERT(8 == X.bytesPerElement());
                ASSERT(         0 == X[0]);
                ASSERT( k_UINT8_MAX == X[1]);
                ASSERT(k_UINT16_MAX == X[2]);
                ASSERT(k_UINT32_MAX == X[3]);
                ASSERT(k_UINT64_MAX == X[4]);

            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(allocations == defaultAllocator.numAllocations());
        }

        if (verbose) cout << "\nTesting 'removeAll'." << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;
            mX.append(0);
            mX.append(k_INT8_MIN);
            mX.append(k_INT8_MAX);
            mX.append(k_INT16_MIN);
            mX.append(k_INT16_MAX);
            mX.append(k_INT32_MIN);
            mX.append(k_INT32_MAX);
            mX.append(k_INT64_MIN);
            mX.append(k_INT64_MAX);
            ASSERT( 9 == X.length());
            ASSERT(11 == X.capacity());
            ASSERT( 8 == X.bytesPerElement());
            ASSERT(        0 == X[0]);
            ASSERT( k_INT8_MIN == X[1]);
            ASSERT( k_INT8_MAX == X[2]);
            ASSERT(k_INT16_MIN == X[3]);
            ASSERT(k_INT16_MAX == X[4]);
            ASSERT(k_INT32_MIN == X[5]);
            ASSERT(k_INT32_MAX == X[6]);
            ASSERT(k_INT64_MIN == X[7]);
            ASSERT(k_INT64_MAX == X[8]);

            bsls::Types::Int64 na = sa.numAllocations();
            bsls::Types::Int64 nd = sa.numDeallocations();

            mX.removeAll();
            ASSERT( 0 == X.length());
            ASSERT(91 == X.capacity());
            ASSERT( 1 == X.bytesPerElement());
            ASSERT(na == sa.numAllocations());
            ASSERT(nd == sa.numDeallocations());

            mX.append(0);
            mX.append(k_INT8_MIN);
            mX.append(k_INT8_MAX);
            mX.append(k_INT16_MIN);
            mX.append(k_INT16_MAX);
            mX.append(k_INT32_MIN);
            mX.append(k_INT32_MAX);
            mX.append(k_INT64_MIN);
            mX.append(k_INT64_MAX);
            ASSERT( 9 == X.length());
            ASSERT(11 == X.capacity());
            ASSERT( 8 == X.bytesPerElement());
            ASSERT(        0 == X[0]);
            ASSERT( k_INT8_MIN == X[1]);
            ASSERT( k_INT8_MAX == X[2]);
            ASSERT(k_INT16_MIN == X[3]);
            ASSERT(k_INT16_MAX == X[4]);
            ASSERT(k_INT32_MIN == X[5]);
            ASSERT(k_INT32_MAX == X[6]);
            ASSERT(k_INT64_MIN == X[7]);
            ASSERT(k_INT64_MAX == X[8]);
            ASSERT(na == sa.numAllocations());
            ASSERT(nd == sa.numDeallocations());

            ASSERT(allocations == defaultAllocator.numAllocations());
        }

        if (verbose) cout << "\nTesting 'pop_back'." << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;
            mX.append(0);
            mX.append(k_INT8_MIN);
            mX.append(k_INT8_MAX);
            mX.append(k_INT16_MIN);
            mX.append(k_INT16_MAX);
            mX.append(k_INT32_MIN);
            mX.append(k_INT32_MAX);
            mX.append(k_INT64_MIN);
            mX.append(k_INT64_MAX);
            ASSERT( 9 == X.length());
            ASSERT(11 == X.capacity());
            ASSERT( 8 == X.bytesPerElement());
            ASSERT(        0 == X[0]);
            ASSERT( k_INT8_MIN == X[1]);
            ASSERT( k_INT8_MAX == X[2]);
            ASSERT(k_INT16_MIN == X[3]);
            ASSERT(k_INT16_MAX == X[4]);
            ASSERT(k_INT32_MIN == X[5]);
            ASSERT(k_INT32_MAX == X[6]);
            ASSERT(k_INT64_MIN == X[7]);
            ASSERT(k_INT64_MAX == X[8]);

            bsls::Types::Int64 na = sa.numAllocations();
            bsls::Types::Int64 nd = sa.numDeallocations();

            mX.pop_back();
            ASSERT( 8 == X.length());
            ASSERT(11 == X.capacity());
            ASSERT( 8 == X.bytesPerElement());
            ASSERT(        0 == X[0]);
            ASSERT( k_INT8_MIN == X[1]);
            ASSERT( k_INT8_MAX == X[2]);
            ASSERT(k_INT16_MIN == X[3]);
            ASSERT(k_INT16_MAX == X[4]);
            ASSERT(k_INT32_MIN == X[5]);
            ASSERT(k_INT32_MAX == X[6]);
            ASSERT(k_INT64_MIN == X[7]);
            ASSERT(na == sa.numAllocations());
            ASSERT(nd == sa.numDeallocations());

            mX.pop_back();
            ASSERT( 7 == X.length());
            ASSERT(11 == X.capacity());
            ASSERT( 8 == X.bytesPerElement());
            ASSERT(        0 == X[0]);
            ASSERT( k_INT8_MIN == X[1]);
            ASSERT( k_INT8_MAX == X[2]);
            ASSERT(k_INT16_MIN == X[3]);
            ASSERT(k_INT16_MAX == X[4]);
            ASSERT(k_INT32_MIN == X[5]);
            ASSERT(k_INT32_MAX == X[6]);
            ASSERT(na == sa.numAllocations());
            ASSERT(nd == sa.numDeallocations());

            mX.pop_back();
            ASSERT( 6 == X.length());
            ASSERT(11 == X.capacity());
            ASSERT( 8 == X.bytesPerElement());
            ASSERT(        0 == X[0]);
            ASSERT( k_INT8_MIN == X[1]);
            ASSERT( k_INT8_MAX == X[2]);
            ASSERT(k_INT16_MIN == X[3]);
            ASSERT(k_INT16_MAX == X[4]);
            ASSERT(k_INT32_MIN == X[5]);
            ASSERT(na == sa.numAllocations());
            ASSERT(nd == sa.numDeallocations());

            ASSERT(allocations == defaultAllocator.numAllocations());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create an object 'w' (default ctor).       { w:D             }
        //: 2 Create an object 'x' (copy from 'w').      { w:D x:D         }
        //: 3 Set 'x' to 'A' (value distinct from 'D').  { w:D x:A         }
        //: 4 Create an object 'y' (init. to 'A').       { w:D x:A y:A     }
        //: 5 Create an object 'z' (copy from 'y').      { w:D x:A y:A z:A }
        //: 6 Set 'z' to 'D' (the default value).        { w:D x:A y:A z:D }
        //: 7 Assign 'w' from 'x'.                       { w:A x:A y:A z:D }
        //: 8 Assign 'w' from 'z'.                       { w:D x:A y:A z:D }
        //: 9 Assign 'x' from 'x' (aliasing).            { w:D x:A y:A z:D }
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bslma::TestAllocator testAllocator("test", veryVeryVeryVerbose);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1) Create an object x1 (default ctor)."
                             "\t\t\t{ x1: }" << endl;
        Obj mX1(&testAllocator);  const Obj& X1 = mX1;
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout <<
            "\ta) Check initial state of x1." << endl;
        ASSERT(0 == X1.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x1 <op> x1." << endl;

        ASSERT((X1 == X1) == 1);          ASSERT((X1 != X1) == 0)   ;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2) Create a second object x2 (copy from x1)."
                             "\t\t{ x1: x2: }" << endl;
        Obj mX2(X1, &testAllocator);  const Obj& X2 = mX2;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta) Check the initial state of x2." << endl;
        ASSERT(0 == X2.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(X2 == X1);          ASSERT((X2 != X1) == 0);
        ASSERT((X2 == X2) == 1);          ASSERT((X2 != X2) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3) Append an element value 0 to x1)."
                             "\t\t\t{ x1:0 x2: }" << endl;
        mX1.append(0);
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout <<
            "\ta) Check new state of x1." << endl;
        ASSERT(1 == X1.length());
        ASSERT(0 == X1[0]);

        if (verbose) cout <<
            "\tb) Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT((X1 == X1) == 1);        ASSERT((X1 != X1) == 0);
        ASSERT((X1 == X2) == 0);        ASSERT((X1 != X2) == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4) Append the same element value 0 to x2)."
                             "\t\t{ x1:0 x2:0 }" << endl;
        mX2.append(0);
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta) Check new state of x2." << endl;
        ASSERT(1 == X2.length());
        ASSERT(0 == X2[0]);

        if (verbose) cout <<
            "\tb) Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT((X2 == X1) == 1);          ASSERT((X2 != X1) == 0);
        ASSERT((X2 == X2) == 1);          ASSERT((X2 != X2) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5) Append another element value 1 to x2)."
                             "\t\t{ x1:0 x2:01 }" << endl;
        mX2.append(1);
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta) Check new state of x2." << endl;
        ASSERT(2 == X2.length());
        ASSERT(0 == X2[0]);
        ASSERT(1 == X2[1]);

        if (verbose) cout <<
            "\tb) Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT((X2 == X1) == 0);          ASSERT((X2 != X1) == 1);
        ASSERT((X2 == X2) == 1);          ASSERT((X2 != X2) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6) Remove all elements from x1."
                             "\t\t\t{ x1: x2:01 }" << endl;
        mX1.removeAll();
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout <<
            "\ta) Check new state of x1." << endl;
        ASSERT(0 == X1.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT((X1 == X1) == 1);          ASSERT((X1 != X1) == 0);
        ASSERT((X1 == X2) == 0);          ASSERT((X1 != X2) == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7) Create a third object x3 (default ctor)."
                             "\t\t{ x1: x2:01 x3: }" << endl;

        Obj mX3(&testAllocator);  const Obj& X3 = mX3;
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout <<
            "\ta) Check new state of x3." << endl;
        ASSERT(0 == X3.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT((X3 == X1) == 1);          ASSERT((X3 != X1) == 0);
        ASSERT((X3 == X2) == 0);          ASSERT((X3 != X2) == 1);
        ASSERT((X3 == X3) == 1);          ASSERT((X3 != X3) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8) Create a fourth object x4 (copy of x2)."
                             "\t\t{ x1: x2:01 x3: x4:01 }" << endl;

        Obj mX4(X2, &testAllocator);  const Obj& X4 = mX4;
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout <<
            "\ta) Check new state of x4." << endl;

        ASSERT(2 == X4.length());
        ASSERT(0 == X4[0]);
        ASSERT(1 == X4[1]);

        if (verbose) cout <<
            "\tb) Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT((X4 == X1) == 0);          ASSERT((X4 != X1) == 1);
        ASSERT((X4 == X2) == 1);          ASSERT((X4 != X2) == 0);
        ASSERT((X4 == X3) == 0);          ASSERT((X4 != X3) == 1);
        ASSERT((X4 == X4) == 1);          ASSERT((X4 != X4) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9) Assign x2 = x1 (non-empty becomes empty)."
                             "\t\t{ x1: x2: x3: x4:01 }" << endl;

        mX2 = X1;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta) Check new state of x2." << endl;
        ASSERT(0 == X2.length());

        if (verbose) cout <<
            "\tb) Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT((X2 == X1) == 1);          ASSERT((X2 != X1) == 0);
        ASSERT((X2 == X2) == 1);          ASSERT((X2 != X2) == 0);
        ASSERT((X2 == X3) == 1);          ASSERT((X2 != X3) == 0);
        ASSERT((X2 == X4) == 0);          ASSERT((X2 != X4) == 1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n10) Assign x3 = x4 (empty becomes non-empty)."
                             "\t\t{ x1: x2: x3:01 x4:01 }" << endl;

        mX3 = X4;
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout <<
            "\ta) Check new state of x3." << endl;
        ASSERT(2 == X3.length());
        ASSERT(0 == X3[0]);
        ASSERT(1 == X3[1]);

        if (verbose) cout <<
            "\tb) Try equality operators: x3 <op> x1, x2, x3, x4." << endl;
        ASSERT((X3 == X1) == 0);          ASSERT((X3 != X1) == 1);
        ASSERT((X3 == X2) == 0);          ASSERT((X3 != X2) == 1);
        ASSERT((X3 == X3) == 1);          ASSERT((X3 != X3) == 0);
        ASSERT((X3 == X4) == 1);          ASSERT((X3 != X4) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n11) Assign x4 = x4 (aliasing)."
                             "\t\t\t\t{ x1: x2: x3:01 x4:01 }" << endl;

        mX4 = X4;
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout <<
            "\ta) Check new state of x4." << endl;
        ASSERT(2 == X4.length());
        ASSERT(0 == X4[0]);
        ASSERT(1 == X4[1]);

        if (verbose) cout <<
            "\tb) Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT((X4 == X1) == 0);          ASSERT((X4 != X1) == 1);
        ASSERT((X4 == X2) == 0);          ASSERT((X4 != X2) == 1);
        ASSERT((X4 == X3) == 1);          ASSERT((X4 != X3) == 0);
        ASSERT((X4 == X4) == 1);          ASSERT((X4 != X4) == 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout <<
                "\n 1) Create an object X5 (default ctor, default allocator)."
                                                       "\t\t\t{ X5: }" << endl;
        {
            bslma::TestAllocator         ga(veryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&ga);

            Obj mX5;  const Obj& X5 = mX5;
            if (verbose) { cout << '\t';  P(X5); }

            if (verbose) cout << "\ta) Check initial state of X5." << endl;
            ASSERT(0 == X5.length());

            if (verbose) cout <<
                            "\tb) Try equality operators: X5 <op> X5." << endl;
            ASSERT((X5 == X5) == 1);          ASSERT((X5 != X5) == 0);

            mX5.append(0);
            if (verbose) { cout << '\t';  P(X5); }

            ASSERT(ga.numBytesInUse() > 0);
        }
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout <<
                "\n 1) Create an object X6 with ctor(int, bool, ...\n"
                                                       "\t\t\t{ X6: }" << endl;
        {
            Obj mX6(8, false, &testAllocator);  const Obj& X6 = mX6;
            if (verbose) { cout << '\t';  P(X6); }

            if (verbose) cout << "\ta) Check initial state of X6." << endl;
            ASSERT(8 == X6.length());

            if (verbose) cout <<
                            "\tb) Try equality operators: X6 <op> X6." << endl;
            ASSERT((X6 == X6) == 1);        ASSERT((X6 != X6) == 0);

            ASSERT(X6.length() == 8);
            ASSERT(false == X6[0]);         ASSERT(false == X6[4]);
            ASSERT(false == X6[5]);         ASSERT(false == X6[7]);
        }
        {
            bslma::TestAllocator         ga(veryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&ga);

            // Obj mX6(8, 0); const Obj& X6 = mX6; <-- note: causes error
            Obj mX6(8, false);  const Obj& X6 = mX6;
            if (verbose) { cout << '\t';  P(X6); }

            ASSERT(ga.numBytesInUse() > 0);

            if (verbose) cout << "\ta) Check initial state of X6." << endl;
            ASSERT(8 == X6.length());

            if (verbose) cout <<
                            "\tb) Try equality operators: X6 <op> X6." << endl;
            ASSERT((X6 == X6) == 1);        ASSERT((X6 != X6) == 0);

            ASSERT(X6.length() == 8);
            ASSERT(false == X6[0]);         ASSERT(false == X6[4]);
            ASSERT(false == X6[5]);         ASSERT(false == X6[7]);
        }
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      } break;
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus
                  << "." << bsl::endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
