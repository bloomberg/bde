// bdld_datum.t.cpp                                                   -*-C++-*-
#include <bdld_datum.h>

#include <bdldfp_decimalconvertutil.h>
#include <bdldfp_decimalutil.h>

#include <bdlma_bufferedsequentialallocator.h>  // for testing only

#include <bdlsb_memoutstreambuf.h>

#include <bdlt_datetime.h>
#include <bdlt_datetimeinterval.h>
#include <bdlt_currenttime.h>

#include <bslim_testutil.h>

#include <bslma_default.h>                      // for testing only
#include <bslma_defaultallocatorguard.h>        // for testing only
#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatormonitor.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_review.h>
#include <bsls_stopwatch.h>
#include <bsls_timeutil.h>

#include <bsl_algorithm.h>
#include <bsl_cstddef.h>                        // 'size_t'
#include <bsl_cstdlib.h>                        // 'atoi'
#include <bsl_ctime.h>                          // Stopwatch: CLOCK_PROCESS_...
#include <bsl_iostream.h>
#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_memory.h>
#include <bsl_sstream.h>
#include <bsl_unordered_set.h>
#include <bsl_vector.h>

#include <time.h>
#if defined(_WIN32)
#include <Windows.h>
#ifdef ERROR
#undef ERROR
#endif
#elif defined(__unix__) || defined(__unix) || defined(unix) \
    || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#include <sys/resource.h>
#include <sys/times.h>
#else
#error "Unable to define getCPUTime( ) for an unknown OS."
#endif

using namespace BloombergLP;
using namespace bsl;
using namespace bslstl;
using namespace BloombergLP::bdld;
using bdlt::Date;
using bdlt::Datetime;
using bdlt::DatetimeInterval;
using bdlt::Time;
using bdldfp::Decimal64;

// ============================================================================
//                                TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under the test is a mechanism, 'Datum', that provides a
// space-efficient discriminated union (i.e., a variant) that holds the value
// of one of the supported data types.
//
// First, we test internal utility struct, 'Datum_Helpers32', that simplifies
// some manipulation of 64-bit integers on 32-bit platforms.  Then, we test
// portion of class 'Datum' that create/validate/compare non-aggregate value.
// Then, we test class 'DatumMapEntry', that implements a single element in a
// datum maps.  Then, we test classes 'DatumMutableArrayRef',
// 'DatumMutableMapRef' and 'DatumMutableMapOwningKeysRef' that are uses to
// build 'Datum' objects that contain the arrays and maps respectively and owns
// all memory occupied by array and map elements.  Then, we test classes
// 'DatumArrayRef' and 'DatumMapRef', that provide a read-only view to the
// array and map held by a 'Datum' object and act as a return values for
// corresponding 'Datum' accessors.  Finally, we complete testing of the class
// 'Datum' for aggregate values (arrays and maps) and remaining methods.
// Because the 'Datum' is implemented as a POD-type, our primary testing
// concerns will be limited to creating 'Datum' object with different types,
// retrieving the type and value from constructed 'Datum' objects, validating
// the compiler generated copy construction and assignment operator and
// finally verifying that all the memory allocated by 'Datum' objects is
// deallocated when the 'Datum' object is destroyed (except references to
// external data).
//
///Concerns
///--------
//:  1 All primary manipulator use the supplied allocator to allocate memory or
//:    do not allocate memory (when the allocator is not required).
//:
//:  2 Copy constructor and assignment operators do not copy any of the storage
//:    an original 'Datum' is pointing to, and only copy the address to which
//:    the original 'Datum' is pointing or the value it contains.
//:
//:  3 Destroying the 'Datum' that contains aggregate types -- i.e. arrays and
//:    maps -- will recursively destroy the 'Datum' objects that compose the
//:    aggregate, except cases where the aggregate references external memory
//:    ( see 'References to External Strings and Arrays' section in bdld_datum
//:    component documentation ).  For all non-aggregated type, all memory is
//:    released then the 'Datum' object is destroyed.
//:
//:  4 Comparison operators compare two 'Datum' objects by values they refer
//:    to.
//
//: o Primary Manipulators for non-aggregate data types:
//:   - createBoolean(bool);
//:   - createDate(const bdlt::Date&);
//:   - createDatetime(const bdlt::Datetime&, bslma::Allocator *);
//:   - createDatetimeInterval(const DatetimeInterval&, bslma::Allocator *);
//:   - createDecimal64(Decimal);
//:   - createDouble(double);
//:   - createError(int);
//:   - createError(int, const bslstl::StringRef&, bslma::Allocator *);
//:   - createInteger(int);
//:   - createInteger64(bsls::Types::Int64, bslma::Allocator *);
//:   - createNull();
//:   - createStringRef(const char *, bslma::Allocator *);
//:   - createStringRef(const char *, SizeType, bslma::Allocator *);
//:   - createStringRef(const bslstl::StringRef&, bslma::Allocator *);
//:   - createTime(const bdlt::Time&);
//:   - createUdt(void *data, int type);
//:   - copyBinary(void *, int);
//:   - copyString(const char *, bslma::Allocator *);
//:   - copyString(const char *, SizeType, bslma::Allocator *);
//:   - copyString(const bslstl::StringRef&, bslma::Allocator *);
//
//: o Primary Manipulators for aggregate data types:
//:   - createArrayReference(const Datum *, SizeType, bslma::Allocator *);
//:   - adoptArray(const DatumMutableArrayRef&);
//:   - adoptIntMap(const DatumMutableMapRef&);
//:   - adoptMap(const DatumMutableMapRef&);
//:   - adoptMap(const DatumMutableMapOwningKeysRef&);
//
//: o Basic Accessors:
//:   - isTYPE();
//:   - theTYPE();
//-----------------------------------------------------------------------------
//                         // ----------------------
//                         // struct Datum_Helpers32
//                         // ----------------------
//
// CLASS METHODS
// [ 2] Int64 loadSmallInt64(short hight16, int low32);
// [ 2] bool storeSmallInt64(Int64 value, short *phigh16, int *plow32);
//
//                              // -----------
//                              // class Datum
//                              // -----------
// TYPES
// [26] enum DataType { ... };
// [26] enum { k_NUM_TYPES = ... };
//
// CREATORS
// [ 3] Datum() = default;
// [ 3] ~Datum() = default;
// [ 5] Datum(const Datum& original) = default;
//
// CLASS METHODS
// [15] Datum createArrayReference(const Datum *, SizeType, Allocator *);
// [15] Datum createArrayReference(const DatumArrayRef&, Allocator *);
// [ 3] Datum createBoolean(bool);
// [ 3] Datum createDate(const bdlt::Date&);
// [ 3] Datum createDatetime(const bdlt::Datetime&, bslma::Allocator *);
// [ 3] Datum createDatetimeInterval(const DatetimeInterval&, Allocator*);
// [ 3] Datum createDecimal64(Decimal);
// [ 3] Datum createDouble(double);
// [ 3] Datum createError(int);
// [ 3] Datum createError(int, const StringRef&, bslma::Allocator *);
// [ 3] Datum createInteger(int);
// [ 3] Datum createInteger64(bsls::Types::Int64, bslma::Allocator *);
// [ 3] Datum createNull();
// [ 3] Datum createStringRef(const char *, bslma::Allocator *);
// [ 3] Datum createStringRef(const char *, SizeType, bslma::Allocator *);
// [ 3] Datum createStringRef(const StringRef&, bslma::Allocator *);
// [ 3] Datum createTime(const bdlt::Time&);
// [ 3] Datum createUdt(void *data, int type);
// [ 3] Datum copyBinary(void *, int);
// [ 3] Datum copyString(const char *, bslma::Allocator *);
// [ 3] Datum copyString(const char *, SizeType, bslma::Allocator *);
// [ 3] Datum copyString(const bslstl::StringRef&, bslma::Allocator *);
// [15] Datum adoptArray(const DatumMutableArrayRef&);
// [16] Datum adoptIntMap(const DatumMutableIntMapRef& map);
// [17] Datum adoptMap(const DatumMutableMapRef& map);
// [17] Datum adoptMap(const DatumMutableMapOwningKeysRef& map);
// [15] Datum createArrayReference(const Datum *, SizeType, Allocator *);
// [15] void createUninitializedArray(DatumMutableArrayRef*,SizeType,...);
// [17] void createUninitializedMap(DatumMutableMapRef*, SizeType, ...);
// [17] void createUninitializedMap(DatumMutableMapOwningKeysRef *, ...);
// [18] char *createUninitializedString(Datum&, SizeType, Allocator *);
// [28] const char *dataTypeToAscii(Datum::DataType);
// [ 3] void destroy(const Datum&, bslma::Allocator *);
// [26] void disposeUninitializedArray(Datum *, basicAllocator *);
// [27] void disposeUninitializedMap(DatumMutableMapRef *, ...);
// [27] void disposeUninitializedMap(DatumMutableMapOwningKeysRef *, ...);
//
// MANIPULATORS
// [ 7] Datum& operator=(const Datum& rhs) = default;
//
// ACCESSORS
// [22] Datum clone(bslma::Allocator *basicAllocator) const;
// [15] bool isArray() const;
// [ 3] bool isBoolean() const;
// [ 3] bool isBinary() const;
// [ 3] bool isDate() const;
// [ 3] bool isDatetime() const;
// [ 3] bool isDatetimeInterval() const;
// [ 3] bool isDecimal64() const;
// [ 3] bool isDouble() const;
// [ 3] bool isError() const;
// [20] bool isExternalReference() const;
// [ 3] bool isInteger() const;
// [ 3] bool isInteger64() const;
// [16] bool isIntMap() const;
// [17] bool isMap() const;
// [ 3] bool isNull() const;
// [ 3] bool isString() const;
// [ 3] bool isTime() const;
// [ 3] bool isUdt() const;
// [15] DatumArrayRef theArray() const;
// [ 3] DatumBinaryRef theBinary() const;
// [ 3] bool theBoolean() const;
// [ 3] bdlt::Date theDate() const;
// [ 3] bdlt::Datetime theDatetime() const;
// [ 3] bdlt::DatetimeInterval theDatetimeInterval() const;
// [ 3] bdldfp::Decimal64 theDecimal64() const;
// [ 3] double theDouble() const;
// [ 3] DatumError theError() const;
// [ 3] int theInteger() const;
// [ 3] bsls::Types::Int64 theInteger64() const;
// [ 3] bslstl::StringRef theString() const;
// [ 3] bdlt::Time theTime() const;
// [ 3] DatumUdt theUdt() const;
// [16] DatumIntMapRef theIntMap() const;
// [17] DatumMapRef theMap() const;
// [21] DataType::Enum type() const;
// [22] template <class BDLD_VISITOR> void apply(BDLD_VISITOR&) const;
// [ 4] bsl::ostream& print(ostream&, int, int) const; // non-aggregate
// [19] bsl::ostream& print(ostream&, int, int) const; // aggregate
//
// FREE OPERATORS
// [ 6] bool operator==(const Datum&, const Datum&);  // non-aggregate
// [ 6] bool operator!=(const Datum&, const Datum&);  // non-aggregate
// [25] bool operator==(const Datum&, const Datum&);  // aggregate
// [25] bool operator!=(const Datum&, const Datum&);  // aggregate
// [ 4] bsl::ostream& operator<<(ostream&, const Datum&); // non-aggregate
// [19] bsl::ostream& operator<<(ostream&, const Datum&); // aggregate
// [29] bsl::ostream& operator<<(ostream&, const Datum::DataType);
// [34] void hashAppend(hashAlgorithm, datum);
//
//                            // -------------------
//                            // class DatumMapEntry
//                            // -------------------
//
// CREATORS
// [ 8] DatumMapEntry();
// [ 8] DatumMapEntry(const bslstl::StringRef& key, const Datum& value);
//
// MANIPULATORS
// [ 8] void setKey(const bslstl::StringRef&);
// [ 8] void setValue(const Datum&);
//
// ACCESSORS
// [ 8] const bslstl::StringRef& key() const;
// [ 8] const Datum& value() const;
// [ 8] bsl::ostream& print(bsl::ostream&, int, int) const;
//
// FREE OPERATORS
// [ 8] bool operator==(const DatumMapEntry&, const DatumMapEntry&);
// [ 8] bool operator!=(const DatumMapEntry&, const DatumMapEntry&);
// [ 8] bsl::ostream& operator<<(bsl::ostream&, const DatumMapEntry&);
//
//                          // --------------------------
//                          // class DatumMutableArrayRef
//                          // --------------------------
// CREATORS
// [ 9] DatumMutableArrayRef();
// [ 9] DatumMutableArrayRef(Datum *data, SizeType *length);
//
// ACCESSORS
// [ 9] Datum *data() const;
// [ 9] SizeType *length() const;
//
//                          // ------------------------
//                          // class DatumMutableMapRef
//                          // ------------------------
// CREATORS
// [10] DatumMutableMapRef();
// [10] DatumMutableMapRef(DatumMapEntry *, SizeType *, bool *);
//
// ACCESSORS
// [10] DatumMapEntry *data() const;
// [10] SizeType *size() const;
// [10] bool *sorted() const;
//
//                      // ----------------------------------
//                      // class DatumMutableMapOwningKeysRef
//                      // ----------------------------------
// CREATORS
// [11] DatumMutableMapOwningKeysRef();
// [11] DatumMutableMapOwningKeysRef(DatumMapEntry*,SizeType*,char*, ...);
//
// ACCESSORS
// [11] DatumMapEntry *data() const;
// [11] char *keys() const;
// [11] SizeType *size() const;
// [11] bool *sorted() const;
//
//                           // -------------------
//                           // class DatumArrayRef
//                           // -------------------
// CREATORS
// [12] DatumArrayRef();
// [12] DatumArrayRef(const Datum *data, SizeType length);
//
// ACCESSORS
// [12] const Datum& operator[](SizeType index) const;
// [12] const Datum *data() const;
// [12] SizeType length() const;
// [12] bsl::ostream& print(bsl::ostream& stream, int, int) const;
//
// FREE OPERATORS
// [12] bool operator==(const DatumArrayRef&, const DatumArrayRef&);
// [12] bool operator!=(const DatumArrayRef&, const DatumArrayRef&);
// [12] bsl::ostream& operator<<(bsl::ostream&, const DatumArrayRef&);
//
//                         // --------------------
//                         // class DatumIntMapRef
//                         // --------------------
// CREATORS
// [13] DatumIntMapRef(const DatumIntMapEntry *, SizeType, bool);
//
// ACCESSORS
// [13] const DatumIntMapEntry& operator[](SizeType index) const;
// [13] const DatumIntMapEntry *data() const;
// [13] bool isSorted() const;
// [13] SizeType size() const;
// [13] const Datum *find(int key) const;
// [13] bsl::ostream& print(bsl::ostream&, int,int) const;
//
// FREE OPERATORS
// [13] bool operator==(const DatumIntMapRef&, const DatumIntMapRef&);
// [13] bool operator!=(const DatumIntMapRef&, const DatumIntMapRef&);
// [13] bsl::ostream& operator<<(bsl::ostream&, const DatumIntMapRef&);
//
//                            // -----------------
//                            // class DatumMapRef
//                            // -----------------
// CREATORS
// [14] DatumMapRef(const DatumMapEntry *, SizeType, bool, bool);
//
// ACCESSORS
// [14] const DatumMapEntry& operator[](SizeType index) const;
// [14] const DatumMapEntry *data() const;
// [14] bool isSorted() const;
// [14] SizeType size() const;
// [14] const Datum *find(const bslstl::StringRef& key) const;
// [14] bsl::ostream& print(bsl::ostream&, int,int) const;
//
// FREE OPERATORS
// [14] bool operator==(const DatumMapRef& lhs, const DatumMapRef& rhs);
// [14] bool operator!=(const DatumMapRef& lhs, const DatumMapRef& rhs);
// [14] bsl::ostream& operator<<(bsl::ostream&, const DatumMapRef&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [36] USAGE EXAMPLE
// [35] VECTOR OF NULLS TEST
// [24] Datum_ArrayProctor
// [33] DATETIME ALLOCATION TESTS
// [32] MISALIGNED MEMORY ACCESS TEST (only on SUN machines)
// [31] COMPRESSIBILITY OF DECIMAL64
// [30] TYPE TRAITS
// [-2] EFFICIENCY TEST
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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                     GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bsls::Types::Int64 Int64;
typedef Datum::SizeType    SizeType;

const double k_DOUBLE_MIN          = numeric_limits<double>::min();
const double k_DOUBLE_MAX          = numeric_limits<double>::max();
const double k_DOUBLE_INFINITY     = numeric_limits<double>::infinity();
const double k_DOUBLE_NEG_INFINITY = -numeric_limits<double>::infinity();
const double k_DOUBLE_NEG_ZERO     = 1 / k_DOUBLE_NEG_INFINITY;
const double k_DOUBLE_QNAN         = numeric_limits<double>::quiet_NaN();
const double k_DOUBLE_SNAN         = numeric_limits<double>::signaling_NaN();
const double k_DOUBLE_LOADED_NAN   = -sqrt(-1.0);  // A NaN with random bits.
                                                  // These kinds of NaN values
                                                 // are used in the 32bit
                                                // 'Datum' implementation to
                                               // store values of other types.
                                              // We do not distinguish between
                                             // NaN values when compiling for
                                            // a 32 bit ABI.  When compiling
                                           // for a 64 bit ABI we store NaNs
                                          // without loss of information, but
                                         // that may change as we look to save
                                        // physical and virtual memory space.

#ifdef BSLS_PLATFORM_CPU_32_BIT
const bool k_DOUBLE_NAN_BITS_PRESERVED = false;  // "Some NaN" stored.
#else   // BSLS_PLATFORM_CPU_32_BIT
const bool k_DOUBLE_NAN_BITS_PRESERVED = true;  // All NaN bits stored.
#endif  // BSLS_PLATFORM_CPU_32_BIT

const Decimal64 k_DECIMAL64_MIN      = numeric_limits<Decimal64>::min();
const Decimal64 k_DECIMAL64_MAX      = numeric_limits<Decimal64>::max();
const Decimal64 k_DECIMAL64_INFINITY = numeric_limits<Decimal64>::infinity();
const Decimal64 k_DECIMAL64_QNAN     = numeric_limits<Decimal64>::quiet_NaN();
const Decimal64 k_DECIMAL64_SNAN  = numeric_limits<Decimal64>::signaling_NaN();
const Decimal64 k_DECIMAL64_NEG_INFINITY = -k_DECIMAL64_INFINITY;

const char *UNKNOWN_FORMAT = "(* UNKNOWN *)";

//=============================================================================
//                   GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
void populateWithNonAggregateValues(vector<Datum>    *elements,
                                    bslma::Allocator *allocator,
                                    bool              withNaNs = true)
    // Populates the specified vector 'elements' with distinct non-aggregate
    // Datum objects using the specified 'allocator' to allocate memory.
    // Optionally specify 'withNaNs' to include double and Decimal64 NaN
    // values.  The resulting vector is used to test equality operators for
    // Datum holding non-aggregate data types.  Note, that the caller is
    // responsible for destroying all 'Datum's in the populated vector.  Also
    // note, that every value generated by this function compare equal with
    // itself and not compare equal with any other values from the vector
    // (except optionally included NaN values).
{
    elements->push_back(Datum::createBoolean(true));
    elements->push_back(Datum::createBoolean(false));
    elements->push_back(Datum::createDate(Date()) );
    elements->push_back(Datum::createDate(Date(2015, 1, 1)));
    elements->push_back(Datum::createDate(Date(2015, 1, 2)));
    elements->push_back(Datum::createDate(Date(2015, 2, 1)));
    elements->push_back(Datum::createDate(Date(2016, 1, 1)));
    elements->push_back(Datum::createDatetime(Datetime(), allocator));
    elements->push_back(
        Datum::createDatetime(Datetime(2015, 1, 1, 1, 1, 1, 1, 1), allocator));
    elements->push_back(
        Datum::createDatetime(Datetime(2015, 1, 1, 1, 1, 1, 1, 2), allocator));
    elements->push_back(
        Datum::createDatetime(Datetime(2015, 1, 1, 1, 1, 1, 2, 1), allocator));
    elements->push_back(
        Datum::createDatetime(Datetime(2015, 1, 1, 1, 1, 2, 1, 1), allocator));
    elements->push_back(
        Datum::createDatetime(Datetime(2015, 1, 1, 1, 2, 1, 1, 1), allocator));
    elements->push_back(
        Datum::createDatetime(Datetime(2015, 1, 1, 2, 1, 1, 1, 1), allocator));
    elements->push_back(
        Datum::createDatetime(Datetime(2015, 1, 2, 1, 1, 1, 1, 1), allocator));
    elements->push_back(
        Datum::createDatetime(Datetime(2015, 2, 1, 1, 1, 1, 1, 1), allocator));
    elements->push_back(
        Datum::createDatetime(Datetime(2015, 1, 1, 1, 1, 1, 1), allocator));
    elements->push_back(
        Datum::createDatetime(Datetime(2015, 1, 1, 1, 1, 1, 2), allocator));
    elements->push_back(
        Datum::createDatetime(Datetime(2015, 1, 1, 1, 1, 2, 1), allocator));
    elements->push_back(
        Datum::createDatetime(Datetime(2015, 1, 1, 1, 2, 1, 1), allocator));
    elements->push_back(
        Datum::createDatetime(Datetime(2015, 1, 1, 2, 1, 1, 1), allocator));
    elements->push_back(
        Datum::createDatetime(Datetime(2015, 1, 2, 1, 1, 1, 1), allocator));
    elements->push_back(
        Datum::createDatetime(Datetime(2015, 2, 1, 1, 1, 1, 1), allocator));
    elements->push_back(Datum::createDatetimeInterval(
                               DatetimeInterval(0, 0, 0, 0, 0, 0), allocator));
    elements->push_back(Datum::createDatetimeInterval(
                               DatetimeInterval(0, 0, 0, 0, 0, 1), allocator));
    elements->push_back(Datum::createDatetimeInterval(
                               DatetimeInterval(0, 0, 0, 0, 1, 0), allocator));
    elements->push_back(Datum::createDatetimeInterval(
                               DatetimeInterval(0, 0, 0, 1, 0, 0), allocator));
    elements->push_back(Datum::createDatetimeInterval(
                               DatetimeInterval(0, 0, 1, 0, 0, 0), allocator));
    elements->push_back(Datum::createDatetimeInterval(
                               DatetimeInterval(0, 1, 0, 0, 0, 0), allocator));
    elements->push_back(Datum::createDatetimeInterval(
                               DatetimeInterval(1, 0, 0, 0, 0, 0), allocator));
    elements->push_back(Datum::createDatetimeInterval(
                               DatetimeInterval(1, 1, 1, 1, 1, 1), allocator));
    elements->push_back(
                    Datum::createDecimal64(BDLDFP_DECIMAL_DD(0.0), allocator));
    elements->push_back(
                    Datum::createDecimal64(BDLDFP_DECIMAL_DD(1.0), allocator));
    elements->push_back(
                      Datum::createDecimal64(k_DECIMAL64_INFINITY, allocator));
    elements->push_back(
                  Datum::createDecimal64(k_DECIMAL64_NEG_INFINITY, allocator));
    if (withNaNs) {
        elements->push_back(
                          Datum::createDecimal64(k_DECIMAL64_SNAN, allocator));
        elements->push_back(
                          Datum::createDecimal64(k_DECIMAL64_QNAN, allocator));
    }
    elements->push_back(Datum::createDouble(0.0));
    elements->push_back(Datum::createDouble(1.0));
    elements->push_back(Datum::createDouble(k_DOUBLE_MIN));
    elements->push_back(Datum::createDouble(k_DOUBLE_MAX));
    elements->push_back(Datum::createDouble(k_DOUBLE_INFINITY));
    elements->push_back(Datum::createDouble(k_DOUBLE_NEG_INFINITY));
    if (withNaNs) {
        elements->push_back(Datum::createDouble(k_DOUBLE_SNAN));
        elements->push_back(Datum::createDouble(k_DOUBLE_QNAN));
        elements->push_back(Datum::createDouble(k_DOUBLE_LOADED_NAN));
    }
    elements->push_back(Datum::createError(3));
    elements->push_back(Datum::createError(3, "error", allocator));
    elements->push_back(Datum::createError(4, "error", allocator));
    elements->push_back(Datum::createInteger(0));
    elements->push_back(Datum::createInteger(1));
    elements->push_back(Datum::createInteger64(0, allocator));
    elements->push_back(Datum::createInteger64(1, allocator));
    elements->push_back(Datum::createNull());
    elements->push_back(Datum::createStringRef("", 0, allocator));
    elements->push_back(Datum::createStringRef("a", 1, allocator));
    elements->push_back(Datum::createStringRef("b", 1, allocator));
    elements->push_back(Datum::createStringRef("aa", 2, allocator));
    elements->push_back(Datum::createStringRef("ab", 2, allocator));
    elements->push_back(Datum::createStringRef("ba", 2, allocator));
    elements->push_back(Datum::createStringRef("abcde", 5, allocator));
    elements->push_back(Datum::createStringRef("abcdef", 6, allocator));
    elements->push_back(
                     Datum::createStringRef("0123456789abcde", 15, allocator));
    elements->push_back(
                    Datum::createStringRef("0123456789abcdef", 16, allocator));
    elements->push_back(Datum::createTime(Time()));
    elements->push_back(Datum::createTime(Time(1, 1, 1, 1, 1)));
    elements->push_back(Datum::createTime(Time(1, 1, 1, 1, 2)));
    elements->push_back(Datum::createTime(Time(1, 1, 1, 2, 1)));
    elements->push_back(Datum::createTime(Time(1, 1, 2, 1, 1)));
    elements->push_back(Datum::createTime(Time(1, 2, 1, 1, 1)));
    elements->push_back(Datum::createTime(Time(2, 1, 1, 1, 1)));
    elements->push_back(Datum::createUdt(reinterpret_cast<void *>(0x1), 12));
    elements->push_back(Datum::createUdt(reinterpret_cast<void *>(0x2), 12));
    elements->push_back(Datum::createUdt(reinterpret_cast<void *>(0x2), 13));
    elements->push_back(Datum::copyBinary("abcde", 5, allocator));
    elements->push_back(Datum::copyBinary("01234567890abc", 13, allocator));
    elements->push_back(Datum::copyBinary("01234567890abcdef", 16, allocator));
    elements->push_back(Datum::copyString("1", allocator));
    elements->push_back(Datum::copyString("12", allocator));
    elements->push_back(Datum::copyString("123", allocator));
    elements->push_back(Datum::copyString("1234", allocator));
    elements->push_back(Datum::copyString("12345", allocator));
    elements->push_back(Datum::copyString("123456", allocator));
    elements->push_back(Datum::copyString("1234567", allocator));
    elements->push_back(Datum::copyString("abcdef012345678", allocator));
    elements->push_back(Datum::copyString("abcdef0123456789", allocator));
};

void loadRandomString(bsl::string *result, bsls::Types::size_type length)
    // Fill the specified 'result' with a random string of the specified
    // 'length'.  Note that this function employs weak random number
    // generation that is not suitable for cryptographic purposes.
{
    static const char charSet[] = "abcdefghijklmnopqrstuvwxyz"
                                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                  "1234567890";
    result->resize(length);

    for (bsls::Types::size_type i = 0; i < length; ++i) {
        (*result)[i] = charSet[rand() % (sizeof charSet - 1)];
    }
}

void loadRandomBinary(bsl::vector<unsigned char> *result,
                      bsls::Types::size_type      length)
    // Fill the specified 'result' with random bytes of the specified
    // 'length'.  Note that this function employs weak random number
    // generation that is not suitable for cryptographic purposes.
{
    result->resize(length);

    for (bsls::Types::size_type i = 0; i < length; ++i) {
        (*result)[i] = static_cast<unsigned char>(rand() % UCHAR_MAX);
    }
}

                           // ===========
                           // TestVisitor
                           // ===========
class TestVisitor {
    // This class provides a visitor to visit and store the type of 'Datum'
    // object with which it was called.

  private:
    // DATA
    Datum::DataType d_type;         // type of the invoking 'Datum' object
    bool            d_visitedFlag;  // whether 'Datum' object has been visited

  public:
    // CREATORS
    TestVisitor();
        // Create a 'TestVisitor' object.

    // MANIPULATORS
    void operator()(bslmf::Nil v);
        // Store the specified 'v' of 'Datum::e_NIL' type in 'd_type'.

    void operator()(const bdlt::Date& v);
        // Store the specified 'v' of 'Datum::e_DATE' type in 'd_type'.

    void operator()(const bdlt::Datetime& v);
        // Store the specified 'v' of 'Datum::e_DATETIME' type in 'd_type'.

    void operator()(const bdlt::DatetimeInterval& v);
        // Store the specified 'v' of 'Datum::e_DATETIME_INTERVAL' type in
        // 'd_type'.

    void operator()(const bdlt::Time& v);
        // Store the specified 'v' of 'Datum::e_TIME' type in 'd_type'.

    void operator()(bslstl::StringRef v);
        // Store the specified 'v' of 'Datum::e_STRING' type in 'd_type'.

    void operator()(bool v);
        // Store the specified 'v' of 'Datum::e_BOOLEAN' type in 'd_type'.

    void operator()(bsls::Types::Int64 v);
        // Store the specified 'v' of 'Datum::e_INTEGER64' type in 'd_type'.

    void operator()(double v);
        // Store the specified 'v' of 'Datum::e_DOUBLE' type in 'd_type'.

    void operator()(DatumError v);
        // Store the specified 'v' of 'Datum::e_ERROR' type in 'd_type'.

    void operator()(int v);
        // Store the specified 'v' of 'Datum::e_INTEGER' type in 'd_type'.

    void operator()(DatumUdt v);
        // Store the specified 'v' of 'Datum::e_USERDEFINED' type in
        // 'd_type'.

    void operator()(DatumArrayRef v);
        // Store the specified 'v' of 'Datum::e_ARRAY' type in 'd_type'.

    void operator()(DatumIntMapRef v);
        // Store the specified 'v' of 'Datum::e_INT_MAP' type in 'd_type'.

    void operator()(DatumMapRef v);
        // Store the specified 'v' of 'Datum::e_MAP' type in 'd_type'.

    void operator()(DatumBinaryRef v);
        // Store the specified 'v' of 'Datum::e_BINARY' type in 'd_type'.

    void operator()(Decimal64 v);
        // Store the specified 'v' of 'Datum::e_DECIMAL64' type in 'd_type'.

    // ACCESSORS
    Datum::DataType type() const;
        // Return the type of 'Datum' object with which this visitor was
        // called.

    bool objectVisited() const;
        // Return 'true' if this visitor has been called for some 'Datum'
        // object and 'false' otherwise.
};

                           // -----------
                           // TestVisitor
                           // -----------

TestVisitor::TestVisitor()
: d_visitedFlag(false)
{}

void TestVisitor::operator()(bslmf::Nil v)
{
    (void) v;
    d_type        = Datum::e_NIL;
    d_visitedFlag = true;
}

void TestVisitor::operator()(const bdlt::Date& v)
{
    (void) v;
    d_type = Datum::e_DATE;
    d_visitedFlag = true;
}

void TestVisitor::operator()(const bdlt::Datetime& v)
{
    (void) v;
    d_type = Datum::e_DATETIME;
    d_visitedFlag = true;
}

void TestVisitor::operator()(const bdlt::DatetimeInterval& v)
{
    (void) v;
    d_type = Datum::e_DATETIME_INTERVAL;
    d_visitedFlag = true;
}

void TestVisitor::operator()(const bdlt::Time& v)
{
    (void) v;
    d_type = Datum::e_TIME;
    d_visitedFlag = true;
}

void TestVisitor::operator()(bslstl::StringRef v)
{
    (void) v;
    d_type = Datum::e_STRING;
    d_visitedFlag = true;
}

void TestVisitor::operator()(bool v)
{
    (void) v;
    d_type = Datum::e_BOOLEAN;
    d_visitedFlag = true;
}

void TestVisitor::operator()(bsls::Types::Int64 v)
{
    (void) v;
    d_type = Datum::e_INTEGER64;
    d_visitedFlag = true;
}

void TestVisitor::operator()(double v)
{
    (void) v;
    d_type = Datum::e_DOUBLE;
    d_visitedFlag = true;
}

void TestVisitor::operator()(DatumError v)
{
    (void) v;
    d_type = Datum::e_ERROR;
    d_visitedFlag = true;
}

void TestVisitor::operator()(int v)
{
    (void) v;
    d_type = Datum::e_INTEGER;
    d_visitedFlag = true;
}

void TestVisitor::operator()(DatumUdt v)
{
    (void) v;
    d_type = Datum::e_USERDEFINED;
    d_visitedFlag = true;
}

void TestVisitor::operator()(DatumArrayRef v)
{
    (void) v;
    d_type = Datum::e_ARRAY;
    d_visitedFlag = true;
}

void TestVisitor::operator()(DatumMapRef v)
{
    (void) v;
    d_type = Datum::e_MAP;
    d_visitedFlag = true;
}

void TestVisitor::operator()(DatumIntMapRef v)
{
    (void)v;
    d_type = Datum::e_INT_MAP;
    d_visitedFlag = true;
}

void TestVisitor::operator()(DatumBinaryRef v)
{
    (void) v;
    d_type = Datum::e_BINARY;
    d_visitedFlag = true;
}

void TestVisitor::operator()(Decimal64 v)
{
    (void) v;
    d_type = Datum::e_DECIMAL64;;
    d_visitedFlag = true;
}

Datum::DataType TestVisitor::type() const
{
    return d_type;
}

bool TestVisitor::objectVisited() const
{
    return d_visitedFlag;
}

                              // ===============
                              // class Stopwatch
                              // ===============

class Stopwatch {
    // Temporary replacement for bsls::Stopwatch that delivers very unstable
    // user times.  Only a subset of the interface is implemented.

  private:
    // PRIVATE CLASS METHODS
    static double getCPUTime();
        // From http://nadeausoftware.com/articles/2012/03
        //                           /c_c_tip_how_measure_cpu_time_benchmarking
        // Returns the amount of CPU time used by the current process, in
        // seconds, or -1.0 if an error occurred.

    // DATA
    double d_userTime;
    double d_accumulatedUserTime;

  public:
    // CREATORS
    Stopwatch()
        // See bsls::Stopwatch.
    {
        reset();
    }

    // MANIPULATORS
    void reset()
        // See bsls::Stopwatch.
    {
        d_accumulatedUserTime = 0;
    }

    void start(bool)
        // See bsls::Stopwatch.
    {
        d_userTime = getCPUTime();
    }

    void stop()
        // See bsls::Stopwatch.
    {
        d_accumulatedUserTime += getCPUTime() - d_userTime;
    }

    // ACCESSORS
    double accumulatedUserTime() const
        // See bsls::Stopwatch.
    {
        return d_accumulatedUserTime;
    }
};

// PRIVATE CLASS METHODS
double Stopwatch::getCPUTime()
{
#if defined(_WIN32)
    /* Windows -------------------------------------------------- */
    FILETIME createTime;
    FILETIME exitTime;
    FILETIME kernelTime;
    FILETIME userTime;
    if ( GetProcessTimes(GetCurrentProcess( ),
                         &createTime,
                         &exitTime,
                         &kernelTime,
                         &userTime) != -1 )
    {
        SYSTEMTIME userSystemTime;
        if ( FileTimeToSystemTime( &userTime, &userSystemTime ) != -1 ) {
            return (double)userSystemTime.wHour * 3600.0 +
                (double)userSystemTime.wMinute * 60.0 +
                (double)userSystemTime.wSecond +
                (double)userSystemTime.wMilliseconds / 1000.0;
        }
    }

#elif defined(__unix__) || defined(__unix) || defined(unix) \
    || (defined(__APPLE__) && defined(__MACH__))
    /* AIX, BSD, Cygwin, Linux, OSX, and Solaris --------- */

#if _POSIX_TIMERS > 0
    /* Prefer high-res POSIX timers, when available. */
    {
        clockid_t       id;
        struct timespec ts;
#if _POSIX_CPUTIME > 0
        /* Clock ids vary by OS.  Query the id, if possible. */
        if ( clock_getcpuclockid( 0, &id ) == -1 )
#endif
#if defined(CLOCK_PROCESS_CPUTIME_ID)
            /* Use known clock id for AIX, Linux, or Solaris. */
            id = CLOCK_PROCESS_CPUTIME_ID;
#elif defined(CLOCK_VIRTUAL)
        /* Use known clock id for BSD. */
        id = CLOCK_VIRTUAL;
#else
        id = (clockid_t)-1;
#endif
        if ( id != static_cast<clockid_t>(-1) &&
             clock_gettime( id, &ts ) != -1 ) {
            double result = static_cast<double>(ts.tv_sec) +
                                static_cast<double>(ts.tv_nsec) / 1000000000.0;
            return result;                                            // RETURN
        }
    }
#endif

#if defined(RUSAGE_SELF)
    {
        struct rusage rusage;
        if ( getrusage( RUSAGE_SELF, &rusage ) != -1 ) {
            double result = static_cast<double>(rusage.ru_utime.tv_sec) +
                      static_cast<double>(rusage.ru_utime.tv_usec) / 1000000.0;
            return result;                                            // RETURN
        }
    }
#endif

#if defined(_SC_CLK_TCK)
    {
        const double ticks = static_cast<double>(sysconf( _SC_CLK_TCK ));
        struct tms   tms;
        if ( times( &tms ) != static_cast<clock_t>(-1) ) {
            return static_cast<double>(tms.tms_utime) / ticks;        // RETURN
        }
    }
#endif

#if defined(CLOCKS_PER_SEC)
    {
        clock_t cl = clock( );
        if ( cl != static_cast<clock_t>(-1)) {
            double result = static_cast<double>(cl) /
                                           static_cast<double>(CLOCKS_PER_SEC);
            return result;                                            // RETURN
        }
    }
#endif

#endif

    // Failed.
    return -1.0;
}

                             // =======================
                             // class Benchmark_Visitor
                             // =======================

class Benchmark_Visitor {
    // This component-local class provides a visitor to perform a simple, cheap
    // operation on a Datum for benchmarking purpose.

  private:
    // DATA
    double d_sum;

  public:
    // CREATORS
    Benchmark_Visitor();
        // Create a 'Benchmark_Visitor' object.

    // MANIPULATORS
    template<class VISITOR>
    void operator()(const VISITOR& v);
        // Overloaded function call operator accepting the specified 'v' to
        // store it's data.  Does nothing.

    void operator()(const bdlt::Date& v);
        // Overloaded function call operator accepting the specified 'v' to
        // store it's data.

    void operator()(const bdlt::Datetime& v);
        // Overloaded function call operator accepting the specified 'v' to
        // store it's data.

    void operator()(const bdlt::DatetimeInterval& v);
        // Overloaded function call operator accepting the specified 'v' to
        // store it's data.

    void operator()(const bdlt::Time& v);
        // Overloaded function call operator accepting the specified 'v' to
        // store it's data.

    void operator()(const bslstl::StringRef& v);
        // Overloaded function call operator accepting the specified 'v' to
        // store it's data.

    void operator()(bool v);
        // Overloaded function call operator accepting the specified 'v' to
        // store it's data.

    void operator()(bsls::Types::Int64 v);
        // Overloaded function call operator accepting the specified 'v' to
        // store it's data.

    void operator()(double v);
        // Overloaded function call operator accepting the specified 'v' to
        // store it's data.

    void operator()(const DatumError& v);
        // Overloaded function call operator accepting the specified 'v' to
        // store it's data.

    void operator()(int v);
        // Overloaded function call operator accepting the specified 'v' to
        // store it's data.
};

                             // -----------------------
                             // class Benchmark_Visitor
                             // -----------------------
// CREATORS
Benchmark_Visitor::Benchmark_Visitor()
: d_sum(0)
{ }

// MANIPULATORS
template<class VISITOR>
void Benchmark_Visitor::operator()(const VISITOR& v)
{
    (void) v;
}

void Benchmark_Visitor::operator()(const bdlt::Date& v)
{
    d_sum += v.day();
}

void Benchmark_Visitor::operator()(const bdlt::Datetime& v)
{
    d_sum += v.second();
}

void Benchmark_Visitor::operator()(const bdlt::DatetimeInterval& v)
{
    d_sum += v.seconds();
}

void Benchmark_Visitor::operator()(const bdlt::Time& v)
{
    d_sum += v.second();
}

void Benchmark_Visitor::operator()(const bslstl::StringRef& v)
{
    d_sum += static_cast<double>(v.length());
}

void Benchmark_Visitor::operator()(bool v)
{
    d_sum += v;
}

void Benchmark_Visitor::operator()(bsls::Types::Int64 v)
{
    d_sum += static_cast<double>(v);
}

void Benchmark_Visitor::operator()(double v)
{
    d_sum += v;
}

void Benchmark_Visitor::operator()(const DatumError& v)
{
    d_sum += v.code();
}

void Benchmark_Visitor::operator()(int v)
{
    d_sum += v;
}

                             // =============
                             // class Udt_Def
                             // =============

struct Udt_Def : DatumUdt {
    // A DatumUdt with a default constructor.

    // CREATORS
    Udt_Def();
        // Create a 'Udt_Def' object.

    // MANIPULATORS
    DatumUdt& operator=(const DatumUdt& rhs);
        // Assign to this object the value of the specified 'rhs' object.
};
                             // -------------
                             // class Udt_Def
                             // -------------

// CREATORS
Udt_Def::Udt_Def()
: DatumUdt(static_cast<void *>(0), 0)
{ }

// MANIPULATORS
DatumUdt& Udt_Def::operator=(const DatumUdt& rhs)
{
    return *static_cast<DatumUdt*>(this) = rhs;
}


                              // ==============
                              // BenchmarkSuite
                              // ==============

class BenchmarkSuite {
    // This class provides a visitor to visit and store the type of 'Datum'
    // object with which it was called.

  private:
    // INSTANCE DATA
    bsl::vector<bool>    d_active;     // Vector of active benchmarks.
    int                  d_iterations; // Number of iterations.
    int                  d_current;    // Current benchmark.
    double               d_scale;      // For conversion to nanoseconds.
    bslma::TestAllocator d_ta;
    bsl::vector<char>    d_buf;

    static const int k_DATUMS     = 1000;
    static const int k_ALLOC_SIZE = k_DATUMS * 100;

    bool next();
        // Move to the next benchmark. Return true if it should be run.

    void runVisit();
        // Run the visit benchmarks.

    void write(const char *label, double value) const;
        // Write the specified 'label and the specified 'value' to standard
        // output.

    void write(int index, const char *label, double value) const;
        // Write the specified 'index', the specified 'label and the specified
        // 'value' to standard output.

  public:
    // CREATORS
    BenchmarkSuite();
        // Create a 'BenchmarkSuite' object having the default value.  Note
        // that this method's definition is compiler generated.

    // MANIPULATORS
    void run(int iterations, int activeCount, char *activeArg[]);
        // Run suite with the specified 'iterations' number. The specified
        // 'activeCount' defines number of active benchmarks and the specified
        // 'activeArg' contains their numbers.
};

                              // --------------
                              // BenchmarkSuite
                              // --------------

BenchmarkSuite::BenchmarkSuite()
: d_buf(&d_ta)
{ }

// MANIPULATORS
void BenchmarkSuite::run(int   iterations,
                         int   activeCount,
                         char *activeArg[])
{

    d_iterations = iterations;
    d_scale = double(d_iterations) * k_DATUMS / (1000. * 1000 * 1000);
    d_current = -1;
    d_active.resize(0);
    d_buf.resize(k_ALLOC_SIZE);

    for (int i = 0; i < activeCount; ++i) {
        const int benchmark = atoi(activeArg[i]);
        if (benchmark >= static_cast<int>(d_active.size())) {
            d_active.resize(benchmark + 1);
        }
        d_active[benchmark] = true;
    }

    cout << "/// BEGIN BENCHMARK: ";
#if defined(BSLS_PLATFORM_OS_LINUX)
    cout << "Linux";
#elif defined(BSLS_PLATFORM_OS_SOLARIS)
    cout << "Solaris";
#elif defined(BSLS_PLATFORM_OS_AIX)
    cout << "AIX";
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    cout << "Windows";
#elif defined(BSLS_PLATFORM_OS_DARWIN)
    cout << "Darwin(OSX)";
#else
    BSLS_PLATFORM_COMPILER_ERROR;
#endif

#if defined(BSLS_PLATFORM_CPU_32_BIT)
    cout << " - 32-bit variant";
#else
    cout << " - 64-bit variant";
#endif

    cout << " - " << d_iterations << " iterations\n";

    Datum datums[k_DATUMS];
    bool  bools[k_DATUMS];

    // Avoidance of compiler warning.
    (void) bools;

    cout.setf(ios::fixed);
    cout.precision(1);

#define BENCHMARK(CREATOR, TESTER, GETTER, TYPE)                              \
    if (next()) {                                                             \
        Stopwatch csw, tsw, gsw, dsw;                                         \
        for (int j = 0; j < d_iterations; ++j) {                              \
            bdlma::BufferedSequentialAllocator alloc(d_buf.begin(),           \
                                                    k_ALLOC_SIZE);            \
            csw.start(true);                                                  \
            for (int i = 0; i < k_DATUMS; ++i) {                              \
                datums[i] = Datum::CREATOR;                                   \
            }                                                                 \
            csw.stop();                                                       \
            tsw.start(true);                                                  \
            for (int i = 0; i < k_DATUMS; ++i) {                              \
                bools[i] = datums[i].TESTER;                                  \
            }                                                                 \
            tsw.stop();                                                       \
            gsw.start(true);                                                  \
            static TYPE results[k_DATUMS];                                    \
            for (int i = 0; i < k_DATUMS; ++i) {                              \
                results[i] = datums[i].GETTER;                                \
            }                                                                 \
            (void) results;                                                   \
            gsw.stop();                                                       \
            dsw.start(true);                                                  \
            for (int i = 0; i < k_DATUMS; ++i) {                              \
                Datum::destroy(datums[i], &alloc);                            \
            }                                                                 \
            dsw.stop();                                                       \
        }                                                                     \
        write(d_current, #CREATOR,                                            \
              csw.accumulatedUserTime() / d_scale);                           \
        write(#TESTER, tsw.accumulatedUserTime() / d_scale);                  \
        write(#GETTER, gsw.accumulatedUserTime() / d_scale);                  \
        write("destroy()", dsw.accumulatedUserTime() / d_scale);              \
        cout << "\n";                                                         \
    }                                                                         \

    BENCHMARK(createNull(), isNull(), isNull(), bool);

    BENCHMARK(createBoolean(true), isBoolean(), theBoolean(), bool);

    BENCHMARK(createInteger(1), isInteger(), theInteger(), int);

    BENCHMARK(createInteger64(1LL, &alloc),
              isInteger64(),
              theInteger64(),
              Int64);

    BENCHMARK(createDouble(1.23), isDouble(), theDouble(), double);

    {
        unsigned char buffer[8]; // big enough fox max encoding size

#if defined(BSLS_PLATFORM_CPU_32_BIT)
        Decimal64      aSmallDecimal64(BDLDFP_DECIMAL_DD(1.));
        unsigned char *result =
            bdldfp::DecimalConvertUtil::decimal64ToVariableWidthEncoding(
                                                              buffer,
                                                              aSmallDecimal64);
        BSLS_ASSERT(result <= buffer + 6);
        BENCHMARK(createDecimal64(aSmallDecimal64,
                                  &alloc),
                  isDecimal64(),
                  theDecimal64(),
                  Decimal64);
#else
        ++d_current;
#endif

        Decimal64 aDecimal64(
            BDLDFP_DECIMAL_DD(1.23456789));
        BSLS_ASSERT(bdldfp::DecimalConvertUtil::
                    decimal64ToVariableWidthEncoding(buffer,
                                                     aDecimal64)
                    > buffer + 6);
        BENCHMARK(createDecimal64(aDecimal64, &alloc),
                  isDecimal64(),
                  theDecimal64(),
                  Decimal64);
    }

    BENCHMARK(copyString("1", &alloc), isString(), theString(), StringRef);

    BENCHMARK(
        copyString("1", &alloc), isString(), theString(), StringRef);

    BENCHMARK(
        copyString("12", &alloc), isString(), theString(), StringRef);

    BENCHMARK(
        copyString("123", &alloc), isString(), theString(), StringRef);

    BENCHMARK(copyString("1234", &alloc), isString(), theString(), StringRef);

    BENCHMARK(copyString("12345", &alloc), isString(), theString(), StringRef);

    BENCHMARK(copyString("123456", &alloc),
              isString(),
              theString(),
              StringRef);

#if defined(BSLS_PLATFORM_CPU_64_BIT)
    BENCHMARK(copyString("0123456789abcde", &alloc),
              isString(),
              theString(),
              StringRef);
#else
    ++d_current;
#endif
    BENCHMARK(copyString("abcdefghijklmnopqrstuvwxyz", &alloc),
              isString(),
              theString(),
              StringRef);

    BENCHMARK(createStringRef("12345678", 8, &alloc),
              isString(),
              theString(),
              StringRef);

    if (next()) {
        bdlma::BufferedSequentialAllocator alloc(d_buf.begin(), k_ALLOC_SIZE);
        Stopwatch                          csw;
        for (int j = 0; j < d_iterations; ++j) {
            bdlma::BufferedSequentialAllocator alloc(d_buf.begin(),
                                                     k_ALLOC_SIZE);
            csw.start(true);
            static char *p[k_DATUMS];
            for (int i = 0; i < k_DATUMS; ++i) {
                p[i] = Datum::createUninitializedString(datums + i,
                                                        80,
                                                        &alloc);
            }

            // Avoidance of compiler warning.
            (void) p;

            csw.stop();
        }

        write(d_current,
               "createUninitializedString",
               csw.accumulatedUserTime() / d_scale);
        cout << "\n";
    }

    bdlt::Date aDate(2010, 1, 5);
    BENCHMARK(createDate(aDate), isDate(), theDate(), bdlt::Date);

    bdlt::Time aTime(16, 45, 32, 12, 425);
    BENCHMARK(createTime(aTime), isTime(), theTime(), bdlt::Time);

    bdlt::Datetime aDatetime(9999, 1, 5, 16, 45, 32, 12);
    BENCHMARK(createDatetime(aDatetime, &alloc),
              isDatetime(),
              theDatetime(),
              bdlt::Datetime);

    bdlt::Datetime aNearDatetime(bdlt::CurrentTime::utc());
    BENCHMARK(createDatetime(aNearDatetime, &alloc),
              isDatetime(),
              theDatetime(),
              bdlt::Datetime);

    bdlt::DatetimeInterval aDatetimeInterval(34, 16, 45, 32, 12);
    BENCHMARK(
        createDatetimeInterval(aDatetimeInterval, &alloc),
        isDatetimeInterval(),
        theDatetimeInterval(),
        bdlt::DatetimeInterval
        );

    BENCHMARK(createError(12), isError(), theError(), DatumError);

    BENCHMARK(createError(12, "error", &alloc),
              isError(), theError(), DatumError);

    BENCHMARK(createUdt(static_cast<void *>(0), 0),
              isUdt(),
              theUdt(),
              Udt_Def);

    {
#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#ifndef BSLS_PLATFORM_CMP_CLANG
#pragma GCC diagnostic ignored "-Wlarger-than="
#endif
#endif
        static Datum array[100 * 1000];
#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

        BENCHMARK(createArrayReference(array,
                                       100,
                                       &alloc),
                  isArray(),
                  theArray(),
                  DatumArrayRef);

        BENCHMARK(createArrayReference(array,
                                       100 * 1000,
                                       &alloc),
                  isArray(),
                  theArray(),
                  DatumArrayRef);
    }

    runVisit();

    cout << "/// END BENCHMARK\n";
}

bool BenchmarkSuite::next()
{
    ++d_current;

    return (d_active.empty()) ||
           (d_current < static_cast<int>(d_active.size()) &&
            d_active[d_current]);
}

void BenchmarkSuite::runVisit()
{
    bdlma::BufferedSequentialAllocator oa(d_buf.begin(), k_ALLOC_SIZE);

    vector<Datum> combo;
    combo.push_back(Datum::copyString("1", &oa));
    combo.push_back(Datum::copyString("abcdefghijklmnopqrstuvwxyz", &oa));
    combo.push_back(Datum::createBoolean(true));
    combo.push_back(Datum::createDate(bdlt::Date(2010, 1, 5)));
    combo.push_back(Datum::createDouble(1));
    combo.push_back(Datum::createError(1));
    combo.push_back(Datum::createInteger(1));
    combo.push_back(Datum::createInteger64(1LL, &oa));

    Stopwatch comboTime;
    typedef vector<Datum>::const_iterator ConstDatumIter;
    double outputValue = 0.0;

    if (next()) {
        comboTime.start(true);

        for (int j = 0; j < d_iterations; ++j) {
            for (int i = 0; i < k_DATUMS; ++i) {
                double sum = 0;
                for (ConstDatumIter iter = combo.begin(), last = combo.end();
                     iter != last;
                     ++iter) {
                    switch (iter->type()) {
                      case Datum::e_INTEGER:
                        sum += iter->theInteger();
                        break;
                      case Datum::e_DOUBLE:
                        sum += iter->theDouble();
                        break;
                      case Datum::e_STRING:
                        sum += static_cast<double>(iter->theString().length());
                        break;
                      case Datum::e_BOOLEAN:
                        sum += iter->theBoolean();
                        break;
                      case Datum::e_ERROR:
                        sum += iter->theError().code();
                        break;
                      case Datum::e_DATE:
                        sum += iter->theDate().day();
                        break;
                      case Datum::e_TIME:
                        sum += iter->theTime().second();
                        break;
                      case Datum::e_DATETIME:
                        sum += iter->theDatetime().day();
                        break;
                      case Datum::e_DATETIME_INTERVAL:
                        sum += iter->theDatetimeInterval().seconds();
                        break;
                      case Datum::e_INTEGER64:
                        sum += static_cast<double>(iter->theInteger64());
                        break;
                      default:
                        break;
                    }
                }
            }
        }

        comboTime.stop();

        outputValue = comboTime.accumulatedUserTime() /
              static_cast<double>(combo.size()) / static_cast<double>(d_scale);
        write(
            d_current,
            "Process heterogeneous array (switch on type)", outputValue);
        cout << "\n";
    }

    if (next()) {

        Benchmark_Visitor visitor;

        comboTime.reset();
        comboTime.start(true);

        for (int j = 0; j < d_iterations; ++j) {
            for (int i = 0; i < k_DATUMS; ++i) {
                for (ConstDatumIter iter = combo.begin(), last = combo.end();
                     iter != last;
                     ++iter) {
                    iter->apply(visitor);
                }
            }
        }

        comboTime.stop();

        outputValue = comboTime.accumulatedUserTime() /
              static_cast<double>(combo.size()) / static_cast<double>(d_scale);
        write(d_current,"Process heterogeneous array (visitor)", outputValue);
    }
}

void BenchmarkSuite::write(const char *label, double value) const
{
    cout << setw(80) << label // justify label for console output readability
         << "\t"              // make it easy to copy-paste to Excel
         << setw(6) << value  // justify value for console output readability
         << "\n";
}

void BenchmarkSuite::write(int index, const char *label, double value) const
{
    cout << setw(2) << index << ')' // justify index and label
         << setw(77) << label       // for console output readability
         << "\t"                    // make it easy to copy-paste to Excel
         << setw(6) << value        // justify value for console output
         << "\n";
}

bool operator==(bslmf::Nil lhs, bslmf::Nil rhs)
    // Return 'true' regardless of the specified 'lhs' and the specified  'rhs'
    // equality.
{
    // Avoidance of compiler warnings.
    (void) lhs;
    (void) rhs;

    return true;
}

                         // ======================
                         // class RegionHexPrinter
                         // ======================

class RegionHexPrinter {
    // DATA
    const unsigned char *d_data_p;
    bsl::size_t          d_length;

  public:
    // CREATORS
    RegionHexPrinter()
        // Create a 'RegionHexPrinter' object that prints the text
        // "<**MISSING**>".
    : d_data_p(0), d_length(0) {}

    RegionHexPrinter(const unsigned char *data, bsl::size_t length)
        // Create a 'RegionHexPrinter' object that prints the specified 'data'
        // of the specified as 'length' as bytes in hexadecimal or prints the
        // text "<**MISSING**>" if 'data' is 0, in which case 'length' is
        // ignored.
    : d_data_p(data), d_length(length) {}

    template <class T>
    RegionHexPrinter(const T& object)
        // Create a 'RegionHexPrinter' object that prints the bytes in
        // hexadecimal notation that make up the specified 'object'.
    : d_data_p(static_cast<const unsigned char*>(
                                            static_cast<const void*>(&object)))
    , d_length(sizeof object)
    {}

    RegionHexPrinter(const char *data, bsl::size_t length)
        // Create a 'RegionHexPrinter' object that prints the specified 'data'
        // of the specified as 'length' as bytes in hexadecimal or prints the
        // text "<**MISSING**>" if 'data' is 0, in which case 'length' is
        // ignored.
    : d_data_p(static_cast<const unsigned char*>(
                                               static_cast<const void*>(data)))
    , d_length(length) {}

    friend bsl::ostream& operator<<(bsl::ostream& os, RegionHexPrinter obj)
    {
        if (0 == obj.d_data_p) {
            return os << "<**MISSING**>";                             // RETURN
        }

        static const char hexdigs[] = "0123456789ABCDEF";
        os << '[';
        for (bsl::size_t i = 0; i < obj.d_length; ++i) {
            if (i != 0) os << ' ';
            unsigned char e = obj.d_data_p[i];
            os << hexdigs[e >> 4] << hexdigs[e & 0x0F];
        }
        return os << ']';
    }
};

inline
RegionHexPrinter hexPrintMissing()
    // Return a 'RegionHexPrinter' object that prints the text "<**MISSING**>".
{
    return RegionHexPrinter();
}

inline
RegionHexPrinter hexPrintRegion(const unsigned char *data, bsl::size_t length)
    // Return a 'RegionHexPrinter' object that prints the specified 'data' of
    // the specified as 'length' as bytes in hexadecimal or prints the text
    // "<**MISSING**>" if 'data' is 0, in which case 'length' is ignored.
{
    return RegionHexPrinter(data, length);
}

template <class T>
inline
RegionHexPrinter hexPrintObject(const T& object)
    // Return a 'RegionHexPrinter' object that prints the bytes in hexadecimal
    // notation that make up the specified 'object'.
{
    return RegionHexPrinter(object);
}

inline
RegionHexPrinter hexPrintChars(const char *data, bsl::size_t length)
    // Return a 'RegionHexPrinter' object that prints the specified 'data' of
    // the specified as 'length' as bytes in hexadecimal or prints the text
    // "<**MISSING**>" if 'data' is 0, in which case 'length' is ignored.
{
    return RegionHexPrinter(data, length);
}

                    // ======================================
                    // class MockAccumulatingHashingAlgorithm
                    // ======================================

class MockAccumulatingHashingAlgorithm {
    // This class implements a mock hashing algorithm that provides a way to
    // accumulate and then examine data that is being passed into hashing
    // algorithms by 'hashAppend'.

  public:
    // PUBLIC TYPES
    typedef bsl::vector<unsigned char> Region;
    typedef bsl::vector<Region>        Regions;

    // DATA
    Regions d_data;  // saved hashed data

    // PRIVATE ACCESSORS
    template <class T>
    bool regionsAtIdxImp(bsl::size_t idx,
                         const T&    value) const;
        // Return 'true' if the next hashed regions starting at the specified
        // 'idx' contain the hashed segments of the specified 'value'.  Return
        // 'false' if there aren't enough hashed regions or if those regions
        // differ in value; that includes if 'idx >= numOfHashedRegions()'.

  public:
    // CREATORS
    MockAccumulatingHashingAlgorithm(bslma::Allocator *allocator);
        // Create an object of this type.

    ~MockAccumulatingHashingAlgorithm();
        // Destroy this object

    // MANIPULATORS
    void operator()(const void *voidPtr, bsl::size_t length);
        // Append the data of the specified 'length' at 'voidPtr' for later
        // inspection.

    void reset();
        // Remove all collected data.

    // ACCESSORS
    RegionHexPrinter regionAsHex(bsl::size_t index) const;
        // Return an object that when inserted into a 'bsl::ostream' will print
        // the region with the specified 'index' as bytes using hexadecimal
        // base or the text "<**MISSING**>" if 'index >= numOfHashedRegions()'.
        // The behavior is undefined if the returned object or a copy of it is
        // retained beyond a modifying operation on this algorithm object,
        // including destruction.

    bool regionAtIndexIs(bsl::size_t idx, const Region& region) const;
        // Return 'true' if the hashed region indicated by the specified 'idx'
        // equals to the specified 'value'.  Return 'false' if the hashed
        // region is different or if 'idx >= numOfHashedRegions()'.

    bool regionsAtIndexAre(bsl::size_t idx, bool                  value) const;
    bool regionsAtIndexAre(bsl::size_t idx, int                   value) const;
    bool regionsAtIndexAre(bsl::size_t idx, const bdlt::Date&     value) const;
    bool regionsAtIndexAre(bsl::size_t idx, const bdlt::Datetime& value) const;
    bool regionsAtIndexAre(bsl::size_t idx,
                           const bdlt::DatetimeInterval&          value) const;
    bool regionsAtIndexAre(bsl::size_t idx, bdldfp::Decimal64     value) const;
    bool regionsAtIndexAre(bsl::size_t idx, double                value) const;
    bool regionsAtIndexAre(bsl::size_t idx, bsl::size_t           value) const;
    bool regionsAtIndexAre(bsl::size_t idx,
                           const bslstl::StringRef&               value) const;
    bool regionsAtIndexAre(bsl::size_t idx, bsls::Types::Int64    value) const;
    bool regionsAtIndexAre(bsl::size_t idx, const bdlt::Time&     value) const;
    bool regionsAtIndexAre(bsl::size_t idx, const void *          value) const;
        // Return 'true' if the next hashed regions starting at the specified
        // 'idx' contain the hashed segments of the specified 'value'.  Return
        // 'false' if there aren't enough hashed regions or if those regions
        // differ in value; that includes if 'idx >= numOfHashedRegions()'.

    const Regions& getData() const;
        // Return a 'const' reference to the collected data.

    bsl::size_t numOfHashedRegions() const;
        // Return the number of memory regions hashed.
};

                    // --------------------------------------
                    // class MockAccumulatingHashingAlgorithm
                    // --------------------------------------

// PRIVATE ACCESSORS
template <class T>
bool MockAccumulatingHashingAlgorithm::regionsAtIdxImp(bsl::size_t idx,
                                                       const T&    value) const
{
    if (idx > d_data.size()) {
        return false;                                                 // RETURN
    }

    bslma::TestAllocator la("localhash");
    MockAccumulatingHashingAlgorithm ma(&la);
    using bslh::hashAppend;
    hashAppend(ma, value);

    for (std::size_t i = 0; i < ma.numOfHashedRegions(); ++i) {
        if (false == this->regionAtIndexIs(idx + i, ma.getData()[i])) {
            return false;                                             // RETURN
        }
    }

    return true;
}

// CREATORS
inline
MockAccumulatingHashingAlgorithm::MockAccumulatingHashingAlgorithm(
                                                   bslma::Allocator *allocator)
: d_data(allocator)
{
}

inline
MockAccumulatingHashingAlgorithm::~MockAccumulatingHashingAlgorithm()
{
}

// MANIPULATORS
inline
void MockAccumulatingHashingAlgorithm::operator()(const void  *voidPtr,
                                                  bsl::size_t  length)
{
    const unsigned char *p = static_cast<const unsigned char *>(voidPtr);
    d_data.emplace_back(p, p + length);
}

inline
void MockAccumulatingHashingAlgorithm::reset()
{
    d_data.clear();
}

// ACCESSORS
inline
RegionHexPrinter
MockAccumulatingHashingAlgorithm::regionAsHex(bsl::size_t index) const
{
    if (index >= d_data.size()) {
        return hexPrintMissing();                                     // RETURN
    }
    return hexPrintRegion(d_data[index].data(), d_data[index].size());
}

inline
bool MockAccumulatingHashingAlgorithm::regionAtIndexIs(
                                                    bsl::size_t   idx,
                                                    const Region& region) const
{
    if (idx > d_data.size() || d_data[idx].size() != region.size()) {
        return false;                                                 // RETURN
    }

    return memcmp(d_data[idx].data(), region.data(), region.size()) == 0;
}

inline
bool MockAccumulatingHashingAlgorithm::regionsAtIndexAre(
                                                       bsl::size_t idx,
                                                       bool        value) const
{
    return regionsAtIdxImp(idx, value);
}

inline
bool MockAccumulatingHashingAlgorithm::regionsAtIndexAre(
                                                       bsl::size_t idx,
                                                       int         value) const
{
    return regionsAtIdxImp(idx, value);
}

inline
bool MockAccumulatingHashingAlgorithm::regionsAtIndexAre(
                                                 bsl::size_t       idx,
                                                 const bdlt::Date& value) const
{
    return regionsAtIdxImp(idx, value);
}

inline
bool MockAccumulatingHashingAlgorithm::regionsAtIndexAre(
                                             bsl::size_t           idx,
                                             const bdlt::Datetime& value) const
{
    return regionsAtIdxImp(idx, value);
}

inline
bool MockAccumulatingHashingAlgorithm::regionsAtIndexAre(
                                     bsl::size_t                   idx,
                                     const bdlt::DatetimeInterval& value) const
{
    return regionsAtIdxImp(idx,value);
}

inline
bool MockAccumulatingHashingAlgorithm::regionsAtIndexAre(
                                                 bsl::size_t       idx,
                                                 bdldfp::Decimal64 value) const
{
    return regionsAtIdxImp(idx, value);
}

inline
bool MockAccumulatingHashingAlgorithm::regionsAtIndexAre(
                                                       bsl::size_t idx,
                                                       double      value) const
{
    return regionsAtIdxImp(idx, value);
}

inline
bool MockAccumulatingHashingAlgorithm::regionsAtIndexAre(
                                                       bsl::size_t idx,
                                                       bsl::size_t value) const
{
    return regionsAtIdxImp(idx, value);
}

inline
bool MockAccumulatingHashingAlgorithm::regionsAtIndexAre(
                                          bsl::size_t              idx,
                                          const bslstl::StringRef& value) const
{
    return regionsAtIdxImp(idx, value);
}

inline
bool MockAccumulatingHashingAlgorithm::regionsAtIndexAre(
                                                bsl::size_t        idx,
                                                bsls::Types::Int64 value) const
{
    return regionsAtIdxImp(idx, value);
}

inline
bool MockAccumulatingHashingAlgorithm::regionsAtIndexAre(
                                                 bsl::size_t       idx,
                                                 const bdlt::Time& value) const
{
    return regionsAtIdxImp(idx, value);
}

inline
bool MockAccumulatingHashingAlgorithm::regionsAtIndexAre(
                                                      bsl::size_t  idx,
                                                      const void  *value) const
{
    return regionsAtIdxImp(idx, value);
}

inline
const MockAccumulatingHashingAlgorithm::Regions&
MockAccumulatingHashingAlgorithm::getData() const
{
    return d_data;
}

inline
bsl::size_t MockAccumulatingHashingAlgorithm::numOfHashedRegions() const
{
    return d_data.size();
}

                       // ====================
                       // class HashHexPrinter
                       // ====================

class HashHexPrinter {
    // DATA
    const MockAccumulatingHashingAlgorithm *d_hasher_p;
    bsl::size_t                             d_startIndex;
    bsl::size_t                             d_endIndex;

  public:
    // CREATORS
    explicit
    HashHexPrinter(const MockAccumulatingHashingAlgorithm *hasher)
        // Create a 'HashHexPrinter' object that prints all the hashed regions
        // stored in the specified 'hasher'.
    : d_hasher_p(hasher)
    , d_startIndex(0)
    , d_endIndex(hasher->numOfHashedRegions())
    {}

    explicit
    HashHexPrinter(const MockAccumulatingHashingAlgorithm *hasher,
                   bsl::size_t                             startIndex,
                   bsl::size_t                             length)
        // Create a 'HashHexPrinter' object that prints all the hashed regions
        // stored in the specified 'hasher'.
    : d_hasher_p(hasher)
    , d_startIndex(startIndex)
    , d_endIndex(startIndex + length)
    {}

    friend bsl::ostream& operator<<(bsl::ostream& os, HashHexPrinter obj)
    {
        const MockAccumulatingHashingAlgorithm &hasher(*obj.d_hasher_p);
        const bsl::size_t start = obj.d_startIndex;
        const bsl::size_t end   = obj.d_endIndex;
        os << "[ ";
        for (bsl::size_t i = start; i < end; ++i) {
            if (i != 0) os << ", ";
            const bsl::size_t regionBytes = hasher.getData()[i].size();
            os << regionBytes << "byte" << (regionBytes == 1 ? ":" : "s:");
            if (i < hasher.numOfHashedRegions()) {
                os << hasher.regionAsHex(i);
            }
            else {
                os << hexPrintMissing();
            }
        }
        return os << " ]";
    }
};

HashHexPrinter hexPrint(const MockAccumulatingHashingAlgorithm& hasher)
    // Return a 'HashHexPrinter' object that prints all the currently
    // accumulated regions of the specified 'hasher' in hexadecimal format.
{
    return HashHexPrinter(&hasher);
}

HashHexPrinter hexPrint(const MockAccumulatingHashingAlgorithm& hasher,
                        bsl::size_t                             startIndex,
                        bsl::size_t                             numRegions)
    // Return a 'HashHexPrinter' object that prints in hexadecimal format, from
    // the currently accumulated regions of the specified 'hasher' the
    // specified 'numRegions' regions starting from the specified
    // 'startIndex'.  If there aren't enough accumulated regions the missing
    // regions are printed using the standard "<**MISSING**>" text.
{
    return HashHexPrinter(&hasher, startIndex, numRegions);
}

//=============================================================================
//                               MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: Unexpected 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    srand(static_cast<unsigned int>(time(static_cast<time_t *>(0))));

    switch (test) { case 0:
      case 36: {
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
        //:   leading comment characters and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'bdld::Datum'
///- - - - - - - - - - - - - - - - - - -
// This example illustrates the construction, manipulation and lifecycle of
// datums.  Datums are created via a set of static methods called 'createTYPE',
// 'copyTYPE' or 'adoptTYPE' where TYPE is one of the supported types.  The
// creation methods take a value and sometimes an allocator.
//
// First, we create an allocator that will supply dynamic memory needed for the
// 'Datum' objects being created:
//..
    bslma::TestAllocator oa("object");
//..
// Then, we create a 'Datum', 'number', having an integer value of '3':
//..
    Datum number = Datum::createInteger(3);
//..
// Next, we verify that the created object actually represents an integer value
// and verify that the value was set correctly:
//..
    ASSERT(true == number.isInteger());
    ASSERT(3    == number.theInteger());
//..
// Note that this object does not allocate any dynamic memory on any supported
// platforms and thus we do not need to explicitly destroy this object to
// release any dynamic memory.
//
// Then, we create a 'Datum', 'cityName', having the string value "Boston":
//..
    Datum cityName = Datum::copyString("Boston", strlen("Boston"), &oa);
//..
// Note, that the 'copyString' makes a copy of the specified string and will
// allocate memory to hold the copy.  Whether the copy is stored in the object
// internal storage buffer or in memory obtained from the allocator depends on
// the length of the string and the platform.
//
// Next, we verify that the created object actually represents a string value
// and verify that the value was set correctly:
//..
    ASSERT(true     == cityName.isString());
    ASSERT("Boston" == cityName.theString());
//..
// Finally, we destroy the 'cityName' object to deallocate memory used to hold
// string value:
//..
    Datum::destroy(cityName, &oa);
//..
///
///Example 2: Creating 'Datum' Referring to the Array of 'Datum' objects
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates the construction of the 'Datum' object referring
// to an existing array of 'Datum' object.
//
// First, we create array of the 'Datum' object:
//..
    const char theDay[] = "Birthday";
    const Datum array[2] = { Datum::createDate(bdlt::Date(2015, 10, 15)),
                             Datum::createStringRef(StringRef(theDay), &oa) };
//..
// Note, that in this case, the second element of the array does not make a
// copy of the string, but represents a string reference.
//
// Then, we create a 'Datum' that refers to the array of Datums:
//..
    const Datum arrayRef = Datum::createArrayReference(array, 2, &oa);
//..
// Next, we verify that the created 'Datum' represents the array value and that
// elements of this array can be accessed.  We also verify that the object
// refers to external data:
//..
    ASSERT(true == arrayRef.isArray());
    ASSERT(true == arrayRef.isExternalReference());
    ASSERT(2    == arrayRef.theArray().length());
    ASSERT(array[0] == arrayRef.theArray().data()[0]);
    ASSERT(array[1] == arrayRef.theArray().data()[1]);
//..
// Then, we call 'destroy' on 'arrayRef', releasing any memory it may have
// allocated, and verify that the external array is intact:
//..
    Datum::destroy(arrayRef, &oa);

    ASSERT(bdlt::Date(2015, 10, 15) == array[0].theDate());
    ASSERT("Birthday"               == array[1].theString());
//..
// Finally, we need to deallocate memory that was potentially allocated for the
// (external) 'Datum' string in the external 'array':
//..
    Datum::destroy(array[1], &oa);
//..
//
///Example 3: Creating the 'Datum' having the array value.
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example illustrates the construction of an owned array of
// datums.
//
// *WARNING*: Using corresponding builder components is a preferred way of
// constructing 'Datum' array objects.  This example shows how a user-facing
// builder component might use the primitives provided in 'bdld_datum'.
//
// First we create an array of datums:
//..
    DatumMutableArrayRef bartArray;
    Datum::createUninitializedArray(&bartArray, 3, &oa);
    bartArray.data()[0] = Datum::createStringRef("Bart", &oa);
    bartArray.data()[1] = Datum::createStringRef("Simpson", &oa);
    bartArray.data()[2] = Datum::createInteger(10);
    *bartArray.length() = 3;
//..
// Then, we construct the Datum that holds the array itself:
//..
    Datum bart = Datum::adoptArray(bartArray);
//..
// Note that after the 'bartArray' has been adopted, the 'bartArray' object can
// be destroyed without invalidating the array contained in the datum.
//
// A DatumArray may be adopted by only one datum. If the DatumArray is not
// adopted, it must be destroyed via 'disposeUnitializedArray'.
//
// Now, we can access the contents of the array through the datum:
//..
    ASSERT(3      == bart.theArray().length());
    ASSERT("Bart" == bart.theArray()[0].theString());
//..
// Finally, we destroy the datum, which releases all memory associated with the
// array:
//..
    Datum::destroy(bart, &oa);
//..
// Note that the same allocator must be used to create the array, the
// elements, and to destroy the datum.
//
///Example 4: Creating the 'Datum' having the map value
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example illustrates the construction of a map of datums
// indexed by string keys.
//
// *WARNING*: Using corresponding builder components is a preferred way of
// constructing 'Datum' map objects.  This example shows how a user-facing
// builder component might use the primitives provided in 'bdld_datum'.
//
// First we create a map of datums:
//..
    DatumMutableMapRef lisaMap;
    Datum::createUninitializedMap(&lisaMap, 3, &oa);
    lisaMap.data()[0] = DatumMapEntry(StringRef("firstName"),
                                      Datum::createStringRef("Lisa", &oa));
    lisaMap.data()[1] = DatumMapEntry(StringRef("lastName"),
                                      Datum::createStringRef("Simpson", &oa));
    lisaMap.data()[2] = DatumMapEntry(StringRef("age"),
                                      Datum::createInteger(8));
    *lisaMap.size() = 3;
//..
// Then, we construct the Datum that holds the map itself:
//..
    Datum lisa = Datum::adoptMap(lisaMap);
//..
// Note that after the 'lisaMap' has been adopted, the 'lisaMap' object can be
// destroyed without invalidating the map contained in the datum.
//
// A 'DatumMutableMapRef' may be adopted by only one datum. If the
// 'DatumMutableMapRef' is not adopted, it must be destroyed via
// 'disposeUninitializedMap'.
//
// Now, we can access the contents of the map through the datum:
//..
    ASSERT(3      == lisa.theMap().size());
    ASSERT("Lisa" == lisa.theMap().find("firstName")->theString());
//..
// Finally, we destroy the datum, which releases all memory associated with the
// array:
//..
    Datum::destroy(lisa, &oa);
//..
// Note that the same allocator must be used to create the map, the elements,
// and to destroy the datum.
///
///Example 5: Mass Destruction
///- - - - - - - - - - - - - -
// The following example illustrates an important idiom: the en masse
// destruction of a series of datums allocated in an arena.
//..
    {
        // scope
        bsls::AlignedBuffer<200> bufferStorage;
        bdlma::BufferedSequentialAllocator arena(bufferStorage.buffer(), 200);

        Datum patty = Datum::copyString("Patty Bouvier",
                                        strlen("Patty Bouvier"),
                                        &arena);

        Datum selma = Datum::copyString("Selma Bouvier",
                                        strlen("Selma Bouvier"),
                                        &arena);
        DatumMutableArrayRef maggieArray;
        Datum::createUninitializedArray(&maggieArray, 2, &arena);
        maggieArray.data()[0] = Datum::createStringRef("Maggie", &arena);
        maggieArray.data()[1] = Datum::createStringRef("Simpson", &arena);
        *maggieArray.length() = 2;
        Datum maggie = Datum::adoptArray(maggieArray);

        (void) patty;   // suppress compiler warning
        (void) selma;   // suppress compiler warning
        (void) maggie;  // suppress compiler warning
    } // end of scope
//..
// Here all the allocated memory is lodged in the 'arena' allocator. At the end
// of the scope the memory is freed in a single step. Calling 'destroy' for
// each datum individually is neither necessary nor permitted.
//
///Example 6: User-defined, error and binary types
///- - - - - - - - - - - - - - - - - - - - - - - -
// Imagine we are using 'Datum' within an expression evaluation subsystem.
// Within that subsystem, along with the set of types defined by
// 'Datum::DataType' we also need to hold 'Sequence' and 'Choice' types within
// 'Datum' values (which are not natively represented by 'Datum').  First, we
// define the set of types used by our subsystem that are an extension to the
// types in 'DatumType':
//..
    struct Sequence {
        struct Sequence *d_next_p;
        int              d_value;
    };

    enum ExtraExpressionTypes {
        e_SEQUENCE = 5,
        e_CHOICE = 6
    };
//..
// Notice that the numeric values will be provided as the 'type' attribute when
// constructing 'Datum' object.
//
// Then we create a 'Sequence' object, and create a 'Datum' to hold it (note
// that we've created the object on the stack for clarity):
//..
    Sequence sequence;
    const Datum datumS0 = Datum::createUdt(&sequence, e_SEQUENCE);
    ASSERT(true == datumS0.isUdt());
//..
// Next, we verify that the 'datumS0' refers to the external 'Sequence' object:
//..
    bdld::DatumUdt udt = datumS0.theUdt();
    ASSERT(e_SEQUENCE == udt.type());
    ASSERT(&sequence  == udt.data());
//..
// Then, we create a 'Datum' to hold a 'DatumError', consisting of an error
// code and an error description message:
//..
    enum { e_FATAL_ERROR = 100 };
    Datum datumError = Datum::createError(e_FATAL_ERROR, "Fatal error.", &oa);
    ASSERT(true == datumError.isError());
    DatumError error = datumError.theError();
    ASSERT(e_FATAL_ERROR == error.code());
    ASSERT("Fatal error." == error.message());
    Datum::destroy(datumError, &oa);
//..
// Finally, we create a 'Datum' that holds an arbitrary binary data:
//..
    int buffer[] = { 1, 2, 3 };
    Datum datumBlob = Datum::copyBinary(buffer, sizeof(buffer), &oa);
    buffer[2] = 666;
    ASSERT(true == datumBlob.isBinary());
    DatumBinaryRef blob = datumBlob.theBinary();
    ASSERT(blob.size() == 3 * sizeof(int));
    ASSERT(reinterpret_cast<const int*>(blob.data())[2] == 3);
    Datum::destroy(datumBlob, &oa);
//..
// Note, that the bytes have been copied.
      } break;
      case 35: {
        // --------------------------------------------------------------------
        // VECTOR OF NULLS TEST
        //  This test became necessary as of December 2022, when someone
        //  discovered that the Clang 13 (and later) started taking advantage
        //  of the dormant UB in the
        //  'bslalg::ArrayPrimitive::uninitializedFillN' caused by
        //  uninitialized anonymous union member of the 'd_as' 'TypedAccess'
        //  field when in optimized build modes.  The utility function was
        //  accessing uninitialized portion of the  'Datum' for internal
        //  optimization, resulting in UB.  Compiler optimizations reliably
        //  caused 'bsl::vector<bdld::Datum>'s constructor taking a 'count'
        //  number of copies of a 'value' to produce vectors of datums having
        //  erroneous values.  See {DRQS 170994605} for details.
        //
        //Concerns:
        //: 1 Creating a vector of null datums using the count-and-value
        //:   constructor, and supplying a null datum value created with
        //:   'bdld::Datum::createNull', produces a vector with the appropriate
        //:   number of null-valued datums (i.e., datums that do not have an
        //:   erroneous or corrupt value.)
        //
        //Plan:
        //: 1 Create 3 vectors having 1, 2, and 3 null datum objects,
        //:   respectively, using the count-and-value constructor and an
        //:   initial datum value created with 'bdld::Datum::createNull'.
        //:   Verify that the resulting datum elements have the null value, and
        //:   not an erroneous or corrupt value.
        //
        // Testing:
        //   VECTOR OF NULLS TEST
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "VECTOR OF NULLS TEST" << endl
                 << "====================" << endl;

        bsl::vector<bdld::Datum> v1(1, bdld::Datum::createNull());
        ASSERT(v1[0].isNull());

        bsl::vector<bdld::Datum> v2(2, bdld::Datum::createNull());
        ASSERT(v2[0].isNull());
        ASSERT(v2[1].isNull());

        bsl::vector<bdld::Datum> v3(3, bdld::Datum::createNull());
        ASSERT(v3[0].isNull());
        ASSERT(v3[1].isNull());
        ASSERT(v3[2].isNull());

      } break;
      case 34: {
        // --------------------------------------------------------------------
        // BSLH HASHING TESTS
        //
        // Concerns:
        //: 1 All data stored in the 'Datum' is hashed, including the data
        //:   type, and miscellaneous data like size of a collection/string.
        //:
        //: 2 No extra information (not forming the value of a 'Datum') is
        //:   hashed.
        //:
        //: 3 No memory is allocated during hashing other than by the hash
        //:   functor itself.
        //:
        //: 4 The 'Datum' parameter of 'hashAppend' is 'const'.
        //
        // Plan:
        //:   Hash 'const' values of each possible 'Datum' type and verify
        //:   using a 'MockAccumulatingHashingAlgorithm' that the type
        //:   identifier and all the data, and nothing but the data is hashed.
        //
        // Testing:
        //   void hashAppend(hashAlgorithm, datum);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BSLH HASHING TESTS" << endl
                          << "===================" << endl;

        if (verbose) cout << "\nTesting 'hashAppend' for Datums having "
                          << "non-aggregate values.\n";
        {
            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard guard(&da);

            if (verbose) cout << "\nTesting 'hashAppend' for boolean.\n";
            {
                const static bool  DATA[] = { true, false };
                const size_t       DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i < DATA_LEN; ++i) {
                    const bool VALUE = DATA[i];
                    const bool TEXT = VALUE ? "true" : "false";

                    if (veryVerbose) { T_ P(VALUE) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bslma::TestAllocator ha("hash", veryVeryVeryVerbose);

                    const Datum D = Datum::createBoolean(VALUE);

                    MockAccumulatingHashingAlgorithm hasher(&ha);

                    hashAppend(hasher, D);

                    ASSERTV(TEXT, 0 == oa.numBlocksInUse()); // no allocation

                    ASSERTV(TEXT, hasher.numOfHashedRegions(),
                            2 == hasher.numOfHashedRegions());
                    ASSERTV(TEXT, hasher.getData()[0].size(),
                            hasher.regionAsHex(0),
                            hexPrintObject(bdld::Datum::e_BOOLEAN),
                            hasher.regionsAtIndexAre(0,
                                                     bdld::Datum::e_BOOLEAN));
                    ASSERTV(TEXT, hasher.getData()[1].size(),
                            hasher.regionAsHex(1),
                            hexPrintObject(VALUE),
                            hasher.regionsAtIndexAre(1, VALUE));

                    Datum::destroy(D, &oa);
                }
            }

            if (verbose) cout << "\nTesting 'hashAppend' for 'Date'.\n";
            {
                const static struct {
                    int        d_line;    // line number
                    bdlt::Date d_value;   // 'Date' value
                } DATA[] = {
                    //LINE VALUE
                    //---- -------------------
                    { L_,  Date()             },
                    { L_,  Date(1999, 12, 31) },
                    { L_,  Date(2015,  1,  1) },
                    { L_,  Date(2200,  8, 12) },
                    { L_,  Date(9999, 12, 31) }
                };

                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i < DATA_LEN; ++i) {
                    const int        LINE = DATA[i].d_line;
                    const bdlt::Date VALUE = DATA[i].d_value;

                    if (veryVerbose) { T_ P(VALUE) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bslma::TestAllocator ha("hash", veryVeryVeryVerbose);

                    const Datum  D = Datum::createDate(VALUE);

                    MockAccumulatingHashingAlgorithm hasher(&ha);

                    hashAppend(hasher, D);

                    ASSERTV(LINE, 0 == oa.numBlocksInUse()); // no allocation

                    ASSERTV(LINE, hasher.numOfHashedRegions(),
                            2 == hasher.numOfHashedRegions());
                    ASSERTV(LINE, hasher.getData()[0].size(),
                            hasher.regionAsHex(0),
                            hexPrintObject(bdld::Datum::e_DATE),
                            hasher.regionsAtIndexAre(0, bdld::Datum::e_DATE));
                    ASSERTV(LINE, hasher.getData()[1].size(),
                            hasher.regionAsHex(1),
                            hexPrintObject(VALUE),
                            hasher.regionsAtIndexAre(1, VALUE));

                    Datum::destroy(D, &oa);
                }
            }

            if (verbose) cout << "\nTesting 'hashAppend' for 'Datetime'.\n";
            {
                const static struct {
                    int            d_line;    // line number
                    bdlt::Datetime d_value;   // 'Datetime' value
                } DATA[] = {
                    //LINE VALUE
                    //---- ---------------------------------------------
                    { L_,  Datetime()                                   },
                    { L_,  Datetime(1999, 12, 31, 12, 45, 31,  18, 34)  },
                    { L_,  Datetime(2015,  2,  2,  1,  1,  1,   1, 1)   },
                    { L_,  Datetime(2200,  9, 11, 18, 10, 59, 458, 342) },
                    { L_,  Datetime(9999,  9,  9,  9,  9,  9, 999, 999) },
                    { L_,  Datetime(1999, 12, 31, 12, 45, 31,  18)      },
                    { L_,  Datetime(2015,  2,  2,  1,  1,  1,   1)      },
                    { L_,  Datetime(2200,  9, 11, 18, 10, 59, 458)      },
                    { L_,  Datetime(9999,  9,  9,  9,  9,  9, 999)      },
                };

                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i < DATA_LEN; ++i) {
                    const int            LINE = DATA[i].d_line;
                    const bdlt::Datetime VALUE = DATA[i].d_value;

                    if (veryVerbose) { T_ P(VALUE) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bslma::TestAllocator ha("hash", veryVeryVeryVerbose);

                    const Datum D = Datum::createDatetime(VALUE, &oa);

                    const Int64 bytesInUse = oa.numBytesInUse();

                    MockAccumulatingHashingAlgorithm hasher(&ha);

                    hashAppend(hasher, D);

                    ASSERTV(LINE, bytesInUse == oa.numBytesInUse());

                    ASSERTV(LINE, hasher.numOfHashedRegions(),
                            2 == hasher.numOfHashedRegions());
                    ASSERTV(LINE, hasher.getData()[0].size(),
                            hasher.regionAsHex(0),
                            hexPrintObject(bdld::Datum::e_DATETIME),
                            hasher.regionsAtIndexAre(0,
                                                     bdld::Datum::e_DATETIME));
                    ASSERTV(LINE, hasher.getData()[1].size(),
                            hasher.regionAsHex(1),
                            hexPrintObject(VALUE),
                            hasher.regionsAtIndexAre(1, VALUE));

                    Datum::destroy(D, &oa);
                }
            }

            if (verbose)
                cout << "\nTesting 'hashAppend' for 'DatetimeInterval'.\n";
            {
                const static struct {
                    int                    d_line;    // line number
                    bdlt::DatetimeInterval d_value;   // interval value
                } DATA[] = {
                    //LINE VALUE
                    //---- ---------------------------------------------
                    { L_,  DatetimeInterval()                          },
                    { L_,  DatetimeInterval(0, 0, 0, 0,  1)            },
                    { L_,  DatetimeInterval(0, 0, 0, 0, -1)            },
                    { L_,  DatetimeInterval(0, 0, 0,  1)               },
                    { L_,  DatetimeInterval(0, 0, 0, -1)               },
                    { L_,  DatetimeInterval(0, 0,  1)                  },
                    { L_,  DatetimeInterval(0, 0, -1)                  },
                    { L_,  DatetimeInterval(0,  1)                     },
                    { L_,  DatetimeInterval(0, -1)                     },
                    { L_,  DatetimeInterval(1)                        },
                    { L_,  DatetimeInterval(-1)                        },
                    { L_,  DatetimeInterval(1, 1, 1, 1, 1)             },
                    { L_,  DatetimeInterval(-1, -1, -1, -1, -1)        },
                    { L_,  DatetimeInterval(1000, 12, 24)              },
                    { L_,  DatetimeInterval(100000000, 1, 1, 32, 587)  },
                    { L_,  DatetimeInterval(-100000000, 3, 2, 14, 319) },
                };

                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i < DATA_LEN; ++i) {
                    const int                    LINE = DATA[i].d_line;
                    const bdlt::DatetimeInterval VALUE = DATA[i].d_value;

                    if (veryVerbose) { T_ P(VALUE) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bslma::TestAllocator ha("hash", veryVeryVeryVerbose);

                    const Datum D = Datum::createDatetimeInterval(VALUE, &oa);

                    const Int64 bytesInUse = oa.numBytesInUse();

                    MockAccumulatingHashingAlgorithm hasher(&ha);

                    hashAppend(hasher, D);

                    ASSERTV(LINE, bytesInUse == oa.numBytesInUse());

                    ASSERTV(LINE, hasher.numOfHashedRegions(),
                            3 == hasher.numOfHashedRegions());
                    ASSERTV(LINE, hasher.getData()[0].size(),
                            hasher.regionAsHex(0),
                            hexPrintObject(bdld::Datum::e_DATETIME_INTERVAL),
                            hasher.regionsAtIndexAre(
                                0,
                                bdld::Datum::e_DATETIME_INTERVAL));
                    ASSERTV(LINE, hasher.getData()[1].size(),
                            hasher.regionAsHex(1),
                            hexPrintObject(VALUE),
                            hasher.regionsAtIndexAre(1, VALUE));

                    Datum::destroy(D, &oa);
                }
            }

            if (verbose) cout << "\nTesting 'hashAppend' for 'Decimal64'.\n";
            {
                const static struct {
                    int               d_line;   // line number
                    bdldfp::Decimal64 d_value;  // 'Decimal64' value
                } DATA[] = {
                    //LINE VALUE
                    //---- ---------------------
                    { L_,  BDLDFP_DECIMAL_DD(0.0)        },
                    { L_,  BDLDFP_DECIMAL_DD(0.253)      },
                    { L_,  BDLDFP_DECIMAL_DD(-0.253)     },
                    { L_,  BDLDFP_DECIMAL_DD(1.0)        },
                    { L_,  BDLDFP_DECIMAL_DD(-1.0)       },
                    { L_,  BDLDFP_DECIMAL_DD(12.345)     },
                    { L_,  BDLDFP_DECIMAL_DD(12.3456789) },
                    { L_,  k_DECIMAL64_MIN               },
                    { L_,  k_DECIMAL64_MAX               },
                    { L_,  k_DECIMAL64_INFINITY          },
                    { L_,  k_DECIMAL64_NEG_INFINITY      },
                    { L_,  k_DECIMAL64_SNAN              },
                    { L_,  k_DECIMAL64_QNAN              },
                };
                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i < DATA_LEN; ++i) {
                    const int               LINE = DATA[i].d_line;
                    const bdldfp::Decimal64 VALUE = DATA[i].d_value;

                    if (veryVerbose) { T_ P_(LINE) P(VALUE) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bslma::TestAllocator ha("hash", veryVeryVeryVerbose);

                    const Datum D = Datum::createDecimal64(VALUE, &oa);

                    const Int64 bytesInUse = oa.numBytesInUse();

                    MockAccumulatingHashingAlgorithm hasher(&ha);

                    hashAppend(hasher, D);

                    ASSERTV(LINE, bytesInUse == oa.numBytesInUse());

                    ASSERTV(LINE, hasher.numOfHashedRegions(),
                            2 == hasher.numOfHashedRegions());
                    ASSERTV(LINE, hasher.getData()[0].size(),
                            hasher.regionAsHex(0),
                            hexPrintObject(bdld::Datum::e_DECIMAL64),
                            hasher.regionsAtIndexAre(
                                                    0,
                                                    bdld::Datum::e_DECIMAL64));
                    ASSERTV(LINE, hasher.getData()[1].size(),
                            hasher.regionAsHex(1),
                            hexPrintObject(VALUE),
                            hasher.regionsAtIndexAre(1, VALUE));

                    Datum::destroy(D, &oa);
                }
            }

            if (verbose) cout << "\nTesting 'hashAppend' for 'double'.\n";
            {
                const static struct {
                    int      d_line;          // line number
                    double   d_value;         // double value
                } DATA[] = {
                    //LINE VALUE
                    //---- -----------------------
                    { L_,  0.0                    },
                    { L_,  k_DOUBLE_NEG_ZERO      },
                    { L_,  .01                    },
                    { L_,  -.01                   },
                    { L_,  2.25e-117              },
                    { L_,  2.25e117               },
                    { L_,  1924.25                },
                    { L_,  -1924.25               },
                    { L_,  k_DOUBLE_MIN           },
                    { L_,  k_DOUBLE_MAX           },
                    { L_,  k_DOUBLE_INFINITY      },
                    { L_,  k_DOUBLE_NEG_INFINITY  },
                    { L_,  k_DOUBLE_SNAN          },
                    { L_,  k_DOUBLE_QNAN          },
                    { L_,  k_DOUBLE_LOADED_NAN    },
                };

                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i < DATA_LEN; ++i) {
                    const int     LINE  = DATA[i].d_line;
                    const double& VALUE = DATA[i].d_value;

                    if (veryVerbose) { T_ P_(LINE) P(VALUE) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bslma::TestAllocator ha("hash", veryVeryVeryVerbose);

                    const Datum D = Datum::createDouble(VALUE);

                    MockAccumulatingHashingAlgorithm hasher(&ha);

                    hashAppend(hasher, D);

                    ASSERTV(LINE, 0 == oa.numBytesInUse());

                    ASSERTV(LINE, hasher.numOfHashedRegions(),
                            2 == hasher.numOfHashedRegions());
                    ASSERTV(LINE, hasher.getData()[0].size(),
                            hasher.regionAsHex(0),
                            hexPrintObject(bdld::Datum::e_DOUBLE),
                            hasher.regionsAtIndexAre(0,
                                                     bdld::Datum::e_DOUBLE));

                    const double HASHED_VALUE =
                        // 'Datum' may not preserve the extra bits of a NaN
                                (VALUE != VALUE) && k_DOUBLE_NAN_BITS_PRESERVED
                                ? VALUE
                                : D.theDouble();
                    ASSERTV(LINE, hasher.getData()[1].size(),
                            hasher.regionAsHex(1),
                            hexPrintObject(HASHED_VALUE),
                            hasher.regionsAtIndexAre(1, HASHED_VALUE));

                    Datum::destroy(D, &oa);
                }
            }

            if (verbose) cout << "\nTesting 'hashAppend' for 'Error'.\n";
            {
                const static struct {
                    int d_line;   // line number
                    int d_code;   // error code
                } DATA[] = {
                    //LINE CODE
                    //---- ---------------------------
                    { L_,   0                          },
                    { L_,   1                          },
                    { L_,  -1                          },
                    { L_,  numeric_limits<int>::min()  },
                    { L_,  numeric_limits<int>::max()  },
                };

                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i < DATA_LEN; ++i) {
                    const int  LINE = DATA[i].d_line;
                    const int  CODE = DATA[i].d_code;

                    if (veryVerbose) { T_ T_ P(CODE) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bslma::TestAllocator ha("hash", veryVeryVeryVerbose);

                    const Datum D = Datum::createError(CODE);

                    MockAccumulatingHashingAlgorithm hasher(&ha);

                    hashAppend(hasher, D);

                    ASSERTV(LINE, 0 == oa.numBytesInUse());

                    ASSERTV(LINE, hasher.numOfHashedRegions(),
                            4 == hasher.numOfHashedRegions());
                    ASSERTV(LINE, hasher.getData()[0].size(),
                            hasher.regionAsHex(0),
                            hexPrintObject(bdld::Datum::e_ERROR),
                            hasher.regionsAtIndexAre(0,
                                                     bdld::Datum::e_ERROR));
                    ASSERTV(LINE, hasher.getData()[1].size(),
                            hasher.regionAsHex(1),
                            hexPrintObject(CODE),
                            hasher.regionsAtIndexAre(1, CODE));
                    ASSERTV(LINE, hasher.getData()[2].size(),
                            hexPrint(hasher, 2, 2),
                            hexPrintObject(bsl::size_t(0)),
                            hexPrintChars("", 0),
                            hasher.regionsAtIndexAre(2, StringRef("", 0)));

                    Datum::destroy(D, &oa);
                }

                static const char errorMessage[] = "This is an error#$%\".";
                for (size_t i = 0; i < DATA_LEN; ++i) {
                    for (size_t j = 0; j <= strlen(errorMessage); ++j) {
                        const int  LINE = DATA[i].d_line;
                        const int  CODE = DATA[i].d_code;
                        const StringRef MSG(errorMessage, j);

                        if (veryVerbose) { T_ T_ P_(CODE) P(MSG) }

                        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                        bslma::TestAllocator ha("hash", veryVeryVeryVerbose);

                        const Datum D = Datum::createError(CODE,
                                                           MSG,
                                                           &oa);

                        const Int64 bytesInUse = oa.numBytesInUse();

                        MockAccumulatingHashingAlgorithm hasher(&ha);

                        hashAppend(hasher, D);

                        ASSERTV(LINE, bytesInUse == oa.numBytesInUse());

                        ASSERTV(LINE, hasher.numOfHashedRegions(),
                                4 == hasher.numOfHashedRegions());
                        ASSERTV(LINE, hasher.getData()[0].size(),
                                hasher.regionAsHex(0),
                                hexPrintObject(bdld::Datum::e_ERROR),
                                hasher.regionsAtIndexAre(
                                                        0,
                                                        bdld::Datum::e_ERROR));
                        ASSERTV(LINE, hasher.getData()[1].size(),
                                hasher.regionAsHex(1),
                                hexPrintObject(CODE),
                                hasher.regionsAtIndexAre(1, CODE));
                        ASSERTV(LINE, hasher.getData()[2].size(),
                                hexPrint(hasher, 2, 2),
                                hexPrintObject(MSG.size()),
                                hexPrintChars(MSG.data(), MSG.size()),
                                hasher.regionsAtIndexAre(2, MSG));

                        Datum::destroy(D, &oa);
                    }
                }
            }

            if (verbose) cout << "\nTesting 'hashAppend' for 'int'.\n";
            {
                const static struct {
                    int d_line;   // line number
                    int d_value;  // integer value
                } DATA[] = {
                    //LINE VALUE
                    //---- -------------------------------------
                    { L_,   0                                    },
                    { L_,   1                                    },
                    { L_,  -1                                    },
                    { L_,  numeric_limits<char>::min()           },
                    { L_,  numeric_limits<char>::max()           },
                    { L_,  numeric_limits<unsigned char>::max()  },
                    { L_,  numeric_limits<short>::min()          },
                    { L_,  numeric_limits<short>::max()          },
                    { L_,  numeric_limits<unsigned short>::max() },
                    { L_,  numeric_limits<int>::min()            },
                    { L_,  numeric_limits<int>::max()            },
                };

                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i < DATA_LEN; ++i) {
                    const int  LINE = DATA[i].d_line;
                    const int VALUE = DATA[i].d_value;

                    if (veryVerbose) { T_ P(VALUE) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bslma::TestAllocator ha("hash", veryVeryVeryVerbose);

                    const Datum D = Datum::createInteger(VALUE);

                    MockAccumulatingHashingAlgorithm hasher(&ha);

                    hashAppend(hasher, D);

                    ASSERTV(LINE, 0 == oa.numBytesInUse());

                    ASSERTV(LINE, hasher.numOfHashedRegions(),
                            2 == hasher.numOfHashedRegions());
                    ASSERTV(LINE, hasher.getData()[0].size(),
                            hasher.regionAsHex(0),
                            hexPrintObject(bdld::Datum::e_INTEGER),
                            hasher.regionsAtIndexAre(0,
                                                     bdld::Datum::e_INTEGER));
                    ASSERTV(LINE, hasher.getData()[1].size(),
                            hasher.regionAsHex(1),
                            hexPrintObject(VALUE),
                            hasher.regionsAtIndexAre(1, VALUE));

                    Datum::destroy(D, &oa);
                }
            }

            if (verbose) cout << "\nTesting 'hashAppend' for 'int64'.\n";
            {
                const static struct {
                    int   d_line;   // line number
                    Int64 d_value;  // Int64 value
                } DATA[] = {
                    //LINE VALUE
                    //---- -------------------------------------
                    { L_,   0                                    },
                    { L_,   1                                    },
                    { L_,  -1                                    },
                    { L_,  numeric_limits<char>::min()           },
                    { L_,  numeric_limits<char>::max()           },
                    { L_,  numeric_limits<unsigned char>::max()  },
                    { L_,  numeric_limits<short>::min()          },
                    { L_,  numeric_limits<short>::max()          },
                    { L_,  numeric_limits<unsigned short>::max() },
                    { L_,  numeric_limits<int>::min()            },
                    { L_,  numeric_limits<int>::max()            },
                    { L_,  numeric_limits<Int64>::min()          },
                    { L_,  numeric_limits<Int64>::max()          },
                };

                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i < DATA_LEN; ++i) {
                    const int   LINE = DATA[i].d_line;
                    const Int64 VALUE = DATA[i].d_value;

                    if (veryVerbose) { T_ P(VALUE) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bslma::TestAllocator ha("hash", veryVeryVeryVerbose);

                    const Datum D = Datum::createInteger64(VALUE, &oa);

                    const Int64 bytesInUse = oa.numBytesInUse();

                    MockAccumulatingHashingAlgorithm hasher(&ha);

                    hashAppend(hasher, D);

                    ASSERTV(LINE, bytesInUse == oa.numBytesInUse());

                    ASSERTV(LINE, hasher.numOfHashedRegions(),
                            2 == hasher.numOfHashedRegions());
                    ASSERTV(LINE, hasher.getData()[0].size(),
                            hasher.regionAsHex(0),
                            hexPrintObject(bdld::Datum::e_INTEGER64),
                            hasher.regionsAtIndexAre(
                                0,
                                bdld::Datum::e_INTEGER64));
                    ASSERTV(LINE, hasher.getData()[1].size(),
                            hasher.regionAsHex(1),
                            hexPrintObject(VALUE),
                            hasher.regionsAtIndexAre(1, VALUE));

                    Datum::destroy(D, &oa);
                }
            }

            if (verbose) cout << "\nTesting 'hashAppend' for 'null'.\n";
            {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                bslma::TestAllocator ha("hash", veryVeryVeryVerbose);

                const Datum D = Datum::createNull();

                MockAccumulatingHashingAlgorithm hasher(&ha);

                hashAppend(hasher, D);

                ASSERT(0 == oa.numBytesInUse());

                ASSERTV(hasher.numOfHashedRegions(),
                        1 == hasher.numOfHashedRegions());
                ASSERTV(hasher.getData()[0].size(),
                        hasher.regionAsHex(0),
                        hexPrintObject(bdld::Datum::e_NIL),
                        hasher.regionsAtIndexAre(0,
                                                 bdld::Datum::e_NIL));

                Datum::destroy(D, &oa);
            }

            if (verbose) cout << "\nTesting 'hashAppend' for strings.\n";
            {
                const static struct {
                    int         d_line;    // line number
                    bsl::size_t d_length;  // length of string
                } DATA[] = {
                    //LINE LENGTH
                    //---- --------------
                    { L_,            0 },
                    { L_,            1 },
                    { L_,            2 },
                    { L_,            3 },
                    { L_,            4 },
                    { L_,            5 },
                    { L_,            6 },
                    { L_,            7 },
                    { L_,            8 },
                    { L_,           12 },
                    { L_,           13 },
                    { L_,           14 },
                    { L_,           32 },
                    { L_,          255 },
                    { L_,          256 },
                    { L_,        65534 },
                    { L_,        65535 },
                    { L_,        65536 },
                    { L_,  1024 * 1024 },
                };

                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i < DATA_LEN; ++i) {
                    bslma::TestAllocator ma("misc", veryVeryVeryVerbose);

                    const int             LINE = DATA[i].d_line;
                    const Datum::SizeType LENGTH = DATA[i].d_length;
                    bsl::string buffer;
                    loadRandomString(&buffer, LENGTH);
                    const StringRef STRING = buffer;

                    if (veryVerbose) { T_ T_ P(LENGTH) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bslma::TestAllocator ha("hash", veryVeryVeryVerbose);

                    // We could use here 'copyString', makes no difference for
                    // the hash, all hash the string value using the same code.
                    const Datum D = Datum::createStringRef(buffer, &oa);

                    const Int64 bytesInUse = oa.numBytesInUse();

                    MockAccumulatingHashingAlgorithm hasher(&ha);

                    hashAppend(hasher, D);

                    ASSERTV(LINE, bytesInUse == oa.numBytesInUse());

                    ASSERTV(LINE, hasher.numOfHashedRegions(),
                            3 == hasher.numOfHashedRegions());
                    ASSERTV(LINE, hasher.getData()[0].size(),
                            hasher.regionAsHex(0),
                            hexPrintObject(bdld::Datum::e_STRING),
                            hasher.regionsAtIndexAre(0,
                                                     bdld::Datum::e_STRING));
                    ASSERTV(LINE, hasher.getData()[1].size(),
                            hexPrint(hasher, 1, 2),
                            hexPrintObject(STRING.size()),
                            hexPrintChars(STRING.data(), STRING.size()),
                            hasher.regionsAtIndexAre(1, STRING));

                    Datum::destroy(D, &oa);
                }
            }

            if (verbose) cout << "\nTesting 'hashAppend' for 'Time'.\n";
            {
                const static struct {
                    int        d_line;   // line number
                    bdlt::Time d_value;  // 'Time' value
                } DATA[] = {
                    //LINE VALUE
                    //---- --------------
                    { L_,  Time()                     },
                    { L_,  Time(0, 1, 1, 1, 1)        },
                    { L_,  Time(8, 0, 0, 999, 888)    },
                    { L_,  Time(23, 59, 59, 999, 999) },
                };

                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i < DATA_LEN; ++i) {
                    const int        LINE  = DATA[i].d_line;
                    const bdlt::Time VALUE = DATA[i].d_value;

                    if (veryVerbose) { T_ P(VALUE) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bslma::TestAllocator ha("hash", veryVeryVeryVerbose);

                    const Datum D = Datum::createTime(VALUE);

                    MockAccumulatingHashingAlgorithm hasher(&ha);

                    hashAppend(hasher, D);

                    ASSERTV(LINE, 0 == oa.numBytesInUse());

                    ASSERTV(LINE, hasher.numOfHashedRegions(),
                            2 == hasher.numOfHashedRegions());
                    ASSERTV(LINE, hasher.getData()[0].size(),
                            hasher.regionAsHex(0),
                            hexPrintObject(bdld::Datum::e_TIME),
                            hasher.regionsAtIndexAre(0,
                                                     bdld::Datum::e_TIME));
                    ASSERTV(LINE, hasher.getData()[1].size(),
                            hasher.regionAsHex(1),
                            hexPrintObject(VALUE),
                            hasher.regionsAtIndexAre(1, VALUE));

                    Datum::destroy(D, &oa);
                }
            }

            if (verbose) cout << "\nTesting 'hashAppend' for 'Udt'.\n";
            {
                static char dummy[]="";
                const static struct {
                    int   d_line;   // line number
                    void *d_ptr;    // UDT pointer value
                    int   d_type;   // UDT type code
                } DATA[] = {
                    //LINE PTR            TYPE
                    //---- -------------- ------------
                    { L_,  0,             0           },
                    { L_,  0,             1           },
                    { L_,  0,             2           },
                    { L_,  0,             3           },
                    { L_,  0,             6553        }, // max type
                    { L_,  &dummy,        0           },
                    { L_,  &dummy,        127         },
                    { L_,  &dummy,        65535       },
                    { L_,  &reviewGuard,  0           },
                    { L_,  &reviewGuard,  876         },
                };

                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i < DATA_LEN; ++i) {
                    const int   LINE = DATA[i].d_line;
                    void *const PTR  = DATA[i].d_ptr;
                    const int   TYPE = DATA[i].d_type;

                    if (veryVerbose) { T_ P_(PTR) P(TYPE) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bslma::TestAllocator ha("hash", veryVeryVeryVerbose);

                    const Datum D = Datum::createUdt(PTR, TYPE);

                    MockAccumulatingHashingAlgorithm hasher(&ha);

                    hashAppend(hasher, D);

                    ASSERTV(LINE, 0 == oa.numBytesInUse());

                    ASSERTV(LINE, hasher.numOfHashedRegions(),
                            3 == hasher.numOfHashedRegions());
                    ASSERTV(LINE, hasher.getData()[0].size(),
                            hasher.regionAsHex(0),
                            hexPrintObject(bdld::Datum::e_USERDEFINED),
                            hasher.regionsAtIndexAre(
                                0,
                                bdld::Datum::e_USERDEFINED));
                    ASSERTV(LINE, hasher.getData()[1].size(),
                            hasher.regionAsHex(1),
                            hexPrintObject(TYPE),
                            hasher.regionsAtIndexAre(1, TYPE));
                    ASSERTV(LINE, hasher.getData()[2].size(),
                            hasher.regionAsHex(2),
                            hexPrintObject(PTR),
                            hasher.regionsAtIndexAre(2, PTR));

                    Datum::destroy(D, &oa);
                }
            }

            if (verbose) cout << "\nTesting 'hashAppend' for 'Binary'.\n";
            {
                const static struct {
                    int         d_line;  // line number
                    bsl::size_t d_size;  // length of binary in bytes
                } DATA[] = {
                    //LINE SIZE
                    //---- --------------
                    { L_,            0 },
                    { L_,            1 },
                    { L_,            2 },
                    { L_,            3 },
                    { L_,            4 },
                    { L_,            5 },
                    { L_,            6 },
                    { L_,            7 },
                    { L_,            8 },
                    { L_,           12 },
                    { L_,           13 },
                    { L_,           14 },
                    { L_,           32 },
                    { L_,          255 },
                    { L_,          256 },
                    { L_,        65534 },
                    { L_,        65535 },
                    { L_,        65536 },
                    { L_,  1024 * 1024 },
                };

                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i < DATA_LEN; ++i) {
                    bslma::TestAllocator ma("misc", veryVeryVeryVerbose);

                    const int             LINE = DATA[i].d_line;
                    const Datum::SizeType SIZE = DATA[i].d_size;
                    bsl::vector<unsigned char> buffer;
                    loadRandomBinary(&buffer, SIZE);
                    const void *PTR = buffer.data();

                    if (veryVerbose) { T_ T_ P(SIZE) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bslma::TestAllocator ha("hash", veryVeryVeryVerbose);

                    const Datum D = Datum::copyBinary(PTR, SIZE, &oa);

                    const Int64 bytesInUse = oa.numBytesInUse();

                    MockAccumulatingHashingAlgorithm hasher(&ha);

                    hashAppend(hasher, D);

                    ASSERTV(LINE, bytesInUse == oa.numBytesInUse());

                    const bsl::size_t EXPECTED_REGIONS = ((SIZE > 0) ? 3 : 2);

                    ASSERTV(LINE,
                            hasher.numOfHashedRegions(),
                            EXPECTED_REGIONS,
                            EXPECTED_REGIONS == hasher.numOfHashedRegions());
                    ASSERTV(LINE, hasher.getData()[0].size(),
                            hasher.regionAsHex(0),
                            hexPrintObject(bdld::Datum::e_BINARY),
                            hasher.regionsAtIndexAre(0,
                                                     bdld::Datum::e_BINARY));
                    ASSERTV(LINE, hasher.getData()[1].size(),
                            hasher.regionAsHex(1),
                            hexPrintObject(SIZE),
                            hasher.regionsAtIndexAre(1, SIZE));
                    if (SIZE > 0) {
                        ASSERTV(LINE, hasher.getData()[2].size(),
                                hasher.regionAsHex(2),
                                hexPrintRegion(buffer.data(), SIZE),
                                hasher.regionAtIndexIs(2, buffer));
                    }

                    Datum::destroy(D, &oa);
                }
            }
        }

        if (verbose) cout << "\nTesting 'hashAppend' for Datums having "
                          << "aggregate values.\n";

        if (verbose) cout << "\nTesting 'hashAppend' for array.\n";
        {
            // Testing 'hashAppend' of an empty array
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            bslma::TestAllocator ha("hash", veryVeryVeryVerbose);

            DatumMutableArrayRef array;
            const Datum          D = Datum::adoptArray(array);

            const Int64 bytesInUse = oa.numBytesInUse();

            MockAccumulatingHashingAlgorithm hasher(&ha);

            hashAppend(hasher, D);

            ASSERTV(bytesInUse == oa.numBytesInUse());

            ASSERTV(hasher.numOfHashedRegions(),
                    2 == hasher.numOfHashedRegions());
            ASSERTV(hasher.getData()[0].size(),
                    hasher.regionsAtIndexAre(0, bdld::Datum::e_ARRAY));
            ASSERTV(hasher.getData()[1].size(),
                    hasher.regionsAtIndexAre(1, bsl::size_t(0)));

            Datum::destroy(D, &oa);
        }

        {
            // Testing 'hashAppend' of a non-empty array
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            bslma::TestAllocator ha("hash", veryVeryVeryVerbose);

            const SizeType       SIZE  = 6;
            DatumMutableArrayRef array;

            Datum::createUninitializedArray(&array, SIZE, &oa);
            array.data()[0] = Datum::createInteger(0);
            array.data()[1] = Datum::createDouble(-3.1416);
            array.data()[2] = Datum::copyString("A long string", &oa);
            array.data()[3] = Datum::copyString("Abc", &oa);
            array.data()[4] = Datum::createDate(bdlt::Date(2010,1,5));
            array.data()[5] = Datum::createDatetime(
                    bdlt::Datetime(2010,1,5, 16,45,32,12),
                    &oa);
            *(array.length()) = SIZE;
            const Datum D = Datum::adoptArray(array);

            const Int64 bytesInUse = oa.numBytesInUse();

            MockAccumulatingHashingAlgorithm hasher(&ha);

            hashAppend(hasher, D);

            ASSERTV(bytesInUse == oa.numBytesInUse());

            const bsl::size_t EXPECTED_REGIONS =
                2    +   //  2 - datum type + array size value
                2    +   //  4 - the integer datum
                2    +   //  6 - the double datum
                6    +   // 12 - the string type = length + data for 2 strings
                4        // 16 - a date and a date-time datum
                ;

            ASSERTV(hasher.numOfHashedRegions(),
                    EXPECTED_REGIONS,
                    EXPECTED_REGIONS == hasher.numOfHashedRegions());

            // "Header" of the array
            ASSERTV(hasher.getData()[0].size(),
                    hasher.regionsAtIndexAre(0, bdld::Datum::e_ARRAY));
            ASSERTV(hasher.getData()[1].size(),
                    hasher.regionsAtIndexAre(1, SIZE));

                                  // ARRAY ELEMENTS
            // The integer 0
            ASSERTV(hasher.getData()[2].size(),
                    hasher.regionsAtIndexAre(2, bdld::Datum::e_INTEGER));
            ASSERTV(hasher.getData()[3].size(),
                    hasher.regionsAtIndexAre(3, D.theArray()[0].theInteger()));
            // The double -3.1416
            ASSERTV(hasher.getData()[4].size(),
                    hasher.regionsAtIndexAre(4, bdld::Datum::e_DOUBLE));
            ASSERTV(hasher.getData()[5].size(),
                    hasher.regionsAtIndexAre(5, D.theArray()[1].theDouble()));
            // "A long string"
            ASSERTV(hasher.getData()[6].size(),
                    hasher.regionsAtIndexAre(6, bdld::Datum::e_STRING));
            ASSERTV(hasher.getData()[7].size(),
                    hasher.regionsAtIndexAre(7, D.theArray()[2].theString()));
            // "Abc"
            ASSERTV(hasher.getData()[9].size(),
                    hasher.regionsAtIndexAre(9, bdld::Datum::e_STRING));
            ASSERTV(hasher.getData()[10].size(),
                    hasher.regionsAtIndexAre(10, D.theArray()[3].theString()));
            // The date 2010.1.5 (ANSI)
            ASSERTV(hasher.getData()[12].size(),
                    hasher.regionsAtIndexAre(12, bdld::Datum::e_DATE));
            ASSERTV(hasher.getData()[13].size(),
                    hasher.regionsAtIndexAre(13, D.theArray()[4].theDate()));
            // The date-time 2010.1.5 16:45:32.012
            ASSERTV(hasher.getData()[14].size(),
                    hasher.regionsAtIndexAre(14, bdld::Datum::e_DATETIME));
            ASSERTV(hasher.getData()[15].size(),
                    hasher.regionsAtIndexAre(15,
                                             D.theArray()[5].theDatetime()));
            Datum::destroy(D, &oa);
        }

        if (verbose) cout << "\nTesting 'hashAppend' for map.\n";
        {
            // Testing 'hashAppend' of an empty map
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            bslma::TestAllocator ha("hash", veryVeryVeryVerbose);

            DatumMutableMapRef map;
            const Datum        D = Datum::adoptMap(map);

            const Int64 bytesInUse = oa.numBytesInUse();

            MockAccumulatingHashingAlgorithm hasher(&ha);

            hashAppend(hasher, D);

            ASSERTV(bytesInUse == oa.numBytesInUse());

            ASSERTV(hasher.numOfHashedRegions(),
                    2 == hasher.numOfHashedRegions());
            ASSERTV(hasher.getData()[0].size(),
                    hasher.regionsAtIndexAre(0, bdld::Datum::e_MAP));
            ASSERTV(hasher.getData()[1].size(),
                    hasher.regionsAtIndexAre(1, bsl::size_t(0)));

            Datum::destroy(D, &oa);
        }
        {
            // Testing 'hashAppend' of a non-empty map
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            bslma::TestAllocator ha("hash", veryVeryVeryVerbose);

            const StringRef key1("one");
            const StringRef key2("two");
            const StringRef key3("three");
            const StringRef key4("four");
            const StringRef key5("five");
            const StringRef key6("six");

            const SizeType     SIZE = 6;
            DatumMutableMapRef map;
            Datum::createUninitializedMap(&map, SIZE, &oa);
            map.data()[0] = DatumMapEntry(key1, Datum::createInteger(0));
            map.data()[1] = DatumMapEntry(key2, Datum::createDouble(-3.141));
            map.data()[2] = DatumMapEntry(
                                      key3,
                                      Datum::copyString("A long string", &oa));
            map.data()[3] = DatumMapEntry(key4,
                                          Datum::copyString("Abc", &oa));
            map.data()[4] = DatumMapEntry(
                                      key5,
                                      Datum::createDate(bdlt::Date(2010,1,5)));
            map.data()[5] = DatumMapEntry(
                                     key6,
                                     Datum::createDatetime(
                                         bdlt::Datetime(2010,1,5, 16,45,32,12),
                                         &oa));
            *(map.size())   = SIZE;
            *(map.sorted()) = false;
            const Datum D = Datum::adoptMap(map);

            const Int64 bytesInUse = oa.numBytesInUse();

            MockAccumulatingHashingAlgorithm hasher(&ha);

            hashAppend(hasher, D);

            ASSERTV(bytesInUse == oa.numBytesInUse());

            const bsl::size_t EXPECTED_REGIONS =
                1 + 1 +  //  2 - datum type + map size value
                2 + 2 +  //  6 - the integer datum and its key
                2 + 2 +  // 10 - the double datum and its key
                6 + 4 +  // 20 - the 2 strings and their keys
                4 + 4    // 28 - a date and a date-time datum and their keys
                ;

            ASSERTV(hasher.numOfHashedRegions(),
                    EXPECTED_REGIONS,
                    EXPECTED_REGIONS == hasher.numOfHashedRegions());

            // "Header" of the array
            ASSERTV(hasher.getData()[0].size(),
                    hasher.regionsAtIndexAre(0, bdld::Datum::e_MAP));
            ASSERTV(hasher.getData()[1].size(),
                    hasher.regionsAtIndexAre(1, SIZE));

                                  // MAP ELEMENTS
            // Key at index 0
            ASSERTV(hasher.getData()[2].size(),
                    hasher.regionsAtIndexAre(2, key1));
            // The integer value 0
            ASSERTV(hasher.getData()[4].size(),
                    hasher.regionsAtIndexAre(4, bdld::Datum::e_INTEGER));
            ASSERTV(hasher.getData()[5].size(),
                    hasher.regionsAtIndexAre(
                                          5,
                                          D.theMap()[0].value().theInteger()));
            // Key at index 1
            ASSERTV(hasher.getData()[6].size(),
                    hasher.regionsAtIndexAre(6, key2));
            // The double -3.1416
            ASSERTV(hasher.getData()[8].size(),
                    hasher.regionsAtIndexAre(8, bdld::Datum::e_DOUBLE));
            ASSERTV(hasher.getData()[9].size(),
                    hasher.regionsAtIndexAre(
                                           9,
                                           D.theMap()[1].value().theDouble()));
            // Key at index 2
            ASSERTV(hasher.getData()[10].size(),
                    hasher.regionsAtIndexAre(10, key3));
            // "A long string"
            ASSERTV(hasher.getData()[12].size(),
                    hasher.regionsAtIndexAre(12, bdld::Datum::e_STRING));
            ASSERTV(hasher.getData()[13].size(),
                    hasher.regionsAtIndexAre(
                                           13,
                                           D.theMap()[2].value().theString()));
            // Key at index 3
            ASSERTV(hasher.getData()[15].size(),
                    hasher.regionsAtIndexAre(15, key4));
            // "Abc"
            ASSERTV(hasher.getData()[17].size(),
                    hasher.regionsAtIndexAre(17, bdld::Datum::e_STRING));
            ASSERTV(hasher.getData()[18].size(),
                    hasher.regionsAtIndexAre(
                                           18,
                                           D.theMap()[3].value().theString()));
            // Key at index 4
            ASSERTV(hasher.getData()[20].size(),
                    hasher.regionsAtIndexAre(20, key5));
            // The date 2010.1.5 (ANSI)
            ASSERTV(hasher.getData()[22].size(),
                    hasher.regionsAtIndexAre(22, bdld::Datum::e_DATE));
            ASSERTV(hasher.getData()[23].size(),
                    hasher.regionsAtIndexAre(23,
                                             D.theMap()[4].value().theDate()));
            // Key at index 5
            ASSERTV(hasher.getData()[24].size(),
                    hasher.regionsAtIndexAre(24, key6));
            // The date-time 2010.1.5 16:45:32.012
            ASSERTV(hasher.getData()[26].size(),
                    hasher.regionsAtIndexAre(26, bdld::Datum::e_DATETIME));
            ASSERTV(hasher.getData()[27].size(),
                    hasher.regionsAtIndexAre(
                                         27,
                                         D.theMap()[5].value().theDatetime()));
            Datum::destroy(D, &oa);
        }

        if (verbose) cout << "\nTesting 'hashAppend' for int-map.\n";
        {
            // Testing 'hashAppend' of an empty map
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            bslma::TestAllocator ha("hash", veryVeryVeryVerbose);

            DatumMutableIntMapRef map;
            const Datum           D = Datum::adoptIntMap(map);

            const Int64 bytesInUse = oa.numBytesInUse();

            MockAccumulatingHashingAlgorithm hasher(&ha);

            hashAppend(hasher, D);

            ASSERTV(bytesInUse == oa.numBytesInUse());

            ASSERTV(hasher.numOfHashedRegions(),
                    2 == hasher.numOfHashedRegions());
            ASSERTV(hasher.getData()[0].size(),
                    hasher.regionsAtIndexAre(0, bdld::Datum::e_INT_MAP));
            ASSERTV(hasher.getData()[1].size(),
                    hasher.regionsAtIndexAre(1, bsl::size_t(0)));

            Datum::destroy(D, &oa);
        }
        {
            // Testing 'hashAppend' of a non-empty map
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            bslma::TestAllocator ha("hash", veryVeryVeryVerbose);

            const SizeType     SIZE = 6;
            DatumMutableIntMapRef map;
            Datum::createUninitializedIntMap(&map, SIZE, &oa);
            map.data()[0] = DatumIntMapEntry(1, Datum::createInteger(0));
            map.data()[1] = DatumIntMapEntry(2, Datum::createDouble(-3.141));
            map.data()[2] = DatumIntMapEntry(
                                      3,
                                      Datum::copyString("A long string", &oa));
            map.data()[3] = DatumIntMapEntry(4,
                                             Datum::copyString("Abc", &oa));
            map.data()[4] = DatumIntMapEntry(
                                      5,
                                      Datum::createDate(bdlt::Date(2010,1,5)));
            map.data()[5] = DatumIntMapEntry(
                                     6,
                                     Datum::createDatetime(
                                         bdlt::Datetime(2010,1,5, 16,45,32,12),
                                         &oa));
            *(map.size())   = SIZE;
            *(map.sorted()) = false;
            const Datum D = Datum::adoptIntMap(map);

            const Int64 bytesInUse = oa.numBytesInUse();

            MockAccumulatingHashingAlgorithm hasher(&ha);

            hashAppend(hasher, D);

            ASSERTV(bytesInUse == oa.numBytesInUse());

            const bsl::size_t EXPECTED_REGIONS =
                1 + 1 +  //  2 - datum type + map size value
                2 + 1 +  //  5 - the integer datum and its key
                2 + 1 +  //  8 - the double datum and its key
                6 + 2 +  // 16 - the 2 strings and their keys
                4 + 2    // 22 - a date and a date-time datum and their keys
                ;

            ASSERTV(hasher.numOfHashedRegions(),
                    EXPECTED_REGIONS,
                    EXPECTED_REGIONS == hasher.numOfHashedRegions());

            // "Header" of the array
            ASSERTV(hasher.getData()[0].size(),
                    hasher.regionsAtIndexAre(0, bdld::Datum::e_INT_MAP));
            ASSERTV(hasher.getData()[1].size(),
                    hasher.regionsAtIndexAre(1, SIZE));

                                  // MAP ELEMENTS
            // Key at index 0
            ASSERTV(hasher.getData()[2].size(),
                    hasher.regionsAtIndexAre(2, 1));
            // The integer value 0
            ASSERTV(hasher.getData()[3].size(),
                    hasher.regionsAtIndexAre(3, bdld::Datum::e_INTEGER));
            ASSERTV(hasher.getData()[4].size(),
                    hasher.regionsAtIndexAre(
                                       4,
                                       D.theIntMap()[0].value().theInteger()));
            // Key at index 1
            ASSERTV(hasher.getData()[5].size(),
                    hasher.regionsAtIndexAre(5, 2));
            // The double -3.1416
            ASSERTV(hasher.getData()[6].size(),
                    hasher.regionsAtIndexAre(6, bdld::Datum::e_DOUBLE));
            ASSERTV(hasher.getData()[7].size(),
                    hasher.regionsAtIndexAre(
                                        7,
                                        D.theIntMap()[1].value().theDouble()));
            // Key at index 2
            ASSERTV(hasher.getData()[8].size(),
                    hasher.regionsAtIndexAre(8, 3));
            // "A long string"
            ASSERTV(hasher.getData()[9].size(),
                    hasher.regionsAtIndexAre(9, bdld::Datum::e_STRING));
            ASSERTV(hasher.getData()[10].size(),
                    hasher.regionsAtIndexAre(
                                        10,
                                        D.theIntMap()[2].value().theString()));
            // Key at index 3
            ASSERTV(hasher.getData()[12].size(),
                    hasher.regionsAtIndexAre(12, 4));
            // "Abc"
            ASSERTV(hasher.getData()[13].size(),
                    hasher.regionsAtIndexAre(13, bdld::Datum::e_STRING));
            ASSERTV(hasher.getData()[14].size(),
                    hasher.regionsAtIndexAre(
                                        14,
                                        D.theIntMap()[3].value().theString()));
            // Key at index 4
            ASSERTV(hasher.getData()[16].size(),
                    hasher.regionsAtIndexAre(16, 5));
            // The date 2010.1.5 (ANSI)
            ASSERTV(hasher.getData()[17].size(),
                    hasher.regionsAtIndexAre(17, bdld::Datum::e_DATE));
            ASSERTV(hasher.getData()[18].size(),
                    hasher.regionsAtIndexAre(18,
                                          D.theIntMap()[4].value().theDate()));
            // Key at index 5
            ASSERTV(hasher.getData()[19].size(),
                    hasher.regionsAtIndexAre(19, 6));
            // The date-time 2010.1.5 16:45:32.012
            ASSERTV(hasher.getData()[20].size(),
                    hasher.regionsAtIndexAre(20, bdld::Datum::e_DATETIME));
            ASSERTV(hasher.getData()[21].size(),
                    hasher.regionsAtIndexAre(
                                      21,
                                      D.theIntMap()[5].value().theDatetime()));
            Datum::destroy(D, &oa);
        }
      } break;
      case 33: {
        // --------------------------------------------------------------------
        // DATETIME ALLOCATION TESTS
        //
        // Concerns:
        //: 1 Creating 'Datum' objects that store 'bdlt::Datetime' objects
        //    allocate memory only when outside of the date-time range between
        //    1930 Apr 15 00:00:00.000 and 2109 Sept 18 24:00:00.000.
        //
        // Plan:
        //    A manual, brute-force test creating values on both side on the
        //    boundary and check if allocation has occurred or not.  Note that
        //    we expect allocation to occur if the (day) difference between the
        //    stored date-time does not fit into a 'signed short' integer.
        //
        // Testing:
        //   Datum createDatetime(const bdlt::Datetime&, bslma::Allocator *);
        //   bdlt::Datetime theDatetime();
        // --------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CPU_32_BIT)
        if (verbose) cout << endl
                          << "DATETIME ALLOCATION TESTS" << endl
                          << "=========================" << endl;

        bslma::TestAllocator qa("qa", veryVeryVeryVerbose);

        const bdlt::Date threshold(2020, 1, 1);

        // No allocations

        const bdlt::Datetime lowNoAllocThreshold =
                                 threshold + bsl::numeric_limits<short>::min();
        Datum d = Datum::createDatetime(lowNoAllocThreshold, &qa);
        LOOP_ASSERT(qa.numBlocksInUse(), qa.numBlocksInUse() == 0);
        LOOP_ASSERT(d,
                    d.isDatetime() && d.theDatetime() == lowNoAllocThreshold);


        const bdlt::Datetime highNoAllocThreshold =
                                 threshold + bsl::numeric_limits<short>::max();
        d = Datum::createDatetime(highNoAllocThreshold, &qa);
        LOOP_ASSERT(qa.numBlocksInUse(), qa.numBlocksInUse() == 0);
        LOOP_ASSERT(d,
                    d.isDatetime() && d.theDatetime() == highNoAllocThreshold);

        // Has allocations

        const bdlt::Datetime lowAllocThreshold =
                               lowNoAllocThreshold - bdlt::DatetimeInterval(1);
        d = Datum::createDatetime(lowAllocThreshold, &qa);
        LOOP_ASSERT(qa.numBlocksInUse(), qa.numBlocksInUse() == 1);
        LOOP_ASSERT(d, d.isDatetime() && d.theDatetime() == lowAllocThreshold);
        Datum::destroy(d, &qa);
        LOOP_ASSERT(qa.numBlocksInUse(), qa.numBlocksInUse() == 0);

        const bdlt::Datetime highAllocThreshold =
                              highNoAllocThreshold + bdlt::DatetimeInterval(1);
        d = Datum::createDatetime(highAllocThreshold, &qa);
        LOOP_ASSERT(qa.numBlocksInUse(), qa.numBlocksInUse() == 1);
        LOOP_ASSERT(d, d.isDatetime() &&
                    d.theDatetime() == highAllocThreshold);
        Datum::destroy(d, &qa);
        LOOP_ASSERT(qa.numBlocksInUse(), qa.numBlocksInUse() == 0);

        // Has allocations due to non-zero microseconds (high resolution)

        bdlt::Datetime mSec = lowNoAllocThreshold;
        mSec.setMicrosecond(156);

        d = Datum::createDatetime(mSec, &qa);
        LOOP_ASSERT(qa.numBlocksInUse(), qa.numBlocksInUse() == 1);
        LOOP_ASSERT(d,
                    d.isDatetime() && d.theDatetime() == mSec);
        Datum::destroy(d, &qa);
        LOOP_ASSERT(qa.numBlocksInUse(), qa.numBlocksInUse() == 0);

        mSec = highAllocThreshold;
        mSec.setMicrosecond(156);
        d = Datum::createDatetime(mSec, &qa);
        LOOP_ASSERT(qa.numBlocksInUse(), qa.numBlocksInUse() == 1);
        LOOP_ASSERT(d, d.isDatetime() && d.theDatetime() == mSec);
        Datum::destroy(d, &qa);
        LOOP_ASSERT(qa.numBlocksInUse(), qa.numBlocksInUse() == 0);
#else
        if (verbose) cout << endl
                          << "DATETIME ALLOCATION TESTS ARE 32 BIT ONLY\n "
                          << "========================================="
                          << endl;
#endif
      } break;
      case 32: {
        // --------------------------------------------------------------------
        // MISALIGNED MEMORY ACCESS TEST
        //
        // Concerns:
        //: 1 Creating 'Datum' objects that store long string values using an
        //:   allocator that does not allocate aligned memory, should not cause
        //:   misaligned memory access error.
        //
        // Plan:
        //: 1 Create 'Datum' objects storing long string values using a
        //:   'bdlma::BufferedSequentialAllocator'.  Compile with
        //:   '-xmemalign=8s' option on any SUN machine.  Confirm there is no
        //:   bus error.  (C-1)
        //
        // Testing:
        //   MISALIGNED MEMORY ACCESS TEST (only on SUN machines)
        // --------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CPU_32_BIT)
        if (verbose) cout << endl
                         << "MISALIGNED MEMORY ACCESS TEST" << endl
                         << "=============================" << endl;

        char                               buf[50];
        bslma::TestAllocator               ta("test", veryVeryVeryVerbose);
        bdlma::BufferedSequentialAllocator bufAlloc(buf, sizeof(buf), &ta);

        const char *str = "1234567";  // long string
        const Datum obj1 = Datum::copyString(str, &bufAlloc);
        const Datum obj2 = Datum::copyString(str, strlen(str), &bufAlloc);
        Datum::destroy(obj1, &bufAlloc);
        Datum::destroy(obj2, &bufAlloc);
        ASSERT(0 == ta.status());
#else
        if (verbose) cout << endl
                          << "DATETIME ALLOCATION TESTS ARE 32 BIT ONLY\n "
                          << "========================================="
                          << endl;
#endif
      } break;
      case 31: {
        // --------------------------------------------------------------------
        // TESTING COMPRESSIBILITY OF DECIMAL64
        //    Check that 'Decimal64' fit in 6 bytes or not (as expected).
        //
        // Concerns:
        //: 1 Different values of Decimal64 can occupy different space.
        //
        // Plan:
        //: 1 Store two different Decimal64 values in the 'variable-width
        //:   encoding' format.  Verify number of bytes written to the buffer.
        //:   (C-1)
        //
        // Testing:
        //   COMPRESSIBILITY OF DECIMAL64
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING COMPRESSIBILITY OF DECIMAL64" << endl
                          << "====================================" << endl;
        if (verbose)
            cout << "\nTesting compressibility of Decimal64 values." << endl;

        {
            unsigned char buffer[9];

            if (verbose)
                cout << "\tCheck that '1.2345' fits in 6 bytes." << endl;
            unsigned char *variable1 =
                bdldfp::DecimalConvertUtil::decimal64ToVariableWidthEncoding(
                                                    buffer,
                                                    BDLDFP_DECIMAL_DD(1.2345));
            ASSERT(variable1 < buffer + 6);

            if (verbose)
                cout << "\tCheck that '12.3456789' does not fit in 6 bytes."
                     << endl;
            unsigned char *variable2 =
                bdldfp::DecimalConvertUtil::decimal64ToVariableWidthEncoding(
                                                buffer,
                                                BDLDFP_DECIMAL_DD(12.3456789));
            ASSERT(variable2 > buffer + 6);
        }
      } break;
      case 30: {
        // --------------------------------------------------------------------
        // TESTING TYPE TRAITS
        //   The object is trivially copyable, default constructible and
        //   bitwise copyable and should have appropriate bsl type traits to
        //   reflect this.
        //
        // Concerns:
        //: 1 The class has the bsl::is_trivially_copyable trait.
        //:
        //: 2 The class has the bsl::is_trivially_default_constructible trait.
        //:
        //: 3 The class has the bslmf::IsBitwiseMoveable trait.
        //:
        //: 4 The class doesn't have the bslma::UsesBslmaAllocator trait.
        //:
        //: 5 The class doesn't have the bslmf::IsBitwiseEqualityComparable
        //:   trait.
        //
        // Plan:
        //: 1 ASSERT the presence of each trait required by the type.  (C-1..5)
        //
        // Testing:
        //   TYPE TRAITS
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING TYPE TRAITS" << endl
                          << "===================" << endl;

        ASSERT((bsl::is_trivially_copyable<Datum>::value));
        ASSERT((bsl::is_trivially_default_constructible<Datum>::value));
        ASSERT((bslmf::IsBitwiseMoveable<Datum>::value));
        ASSERT(!(bslma::UsesBslmaAllocator<Datum>::value));
        ASSERT(!(bslmf::IsBitwiseEqualityComparable<Datum>::value));

      } break;
      case 29: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT ('<<') OPERATOR
        //
        // Concerns:
        //: 1 The '<<' operator writes the output to the specified stream.
        //:
        //: 2 The '<<' operator writes the string representation of each
        //:   enumerator in the intended format.
        //:
        //: 3 The '<<' operator writes a distinguished string when passed an
        //:   out-of-band value.
        //:
        //: 4 There is no output when the stream is invalid.
        //:
        //: 5 The '<<' operator has the expected signature.
        //:
        //: 6 The '<<' operator returns the supplied 'ostream'.
        //
        // Plan:
        //: 1 Verify that the '<<' operator produces the expected results for
        //:   each enumerator.  (C-1..2)
        //:
        //: 2 Verify that the '<<' operator writes a distinguished string when
        //:   passed an out-of-band value.  (C-3)
        //:
        //: 3 Verify that the address of the returned 'stream' is the same as
        //:   the supplied 'stream'.  (C-7)
        //:
        //: 4 Verify that there is no output when the stream is invalid.  (C-5)
        //:
        //: 5 Take the address of the '<<' (free) operator and use the result
        //:   to initialize a variable of the appropriate type.  (C-6)
        //
        // Testing:
        //   operator<<(ostream& s, Datum::DataType val);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING OUTPUT ('<<') OPERATOR" << endl
                                  << "==============================" << endl;

        static const struct {
            int              d_lineNum;  // source line number
            Datum::DataType  d_value;    // enumerator value
            const char      *d_exp_p;    // expected result
        } DATA[] = {
            // LINE  ENUMERATOR VALUE             EXPECTED OUTPUT
            // ----  ---------------------------  -------------------
            {  L_,   Datum::e_NIL,                "NIL"               },
            {  L_,   Datum::e_INTEGER,            "INTEGER"           },
            {  L_,   Datum::e_DOUBLE,             "DOUBLE"            },
            {  L_,   Datum::e_STRING,             "STRING"            },
            {  L_,   Datum::e_BOOLEAN,            "BOOLEAN"           },
            {  L_,   Datum::e_ERROR,              "ERROR"             },
            {  L_,   Datum::e_DATE,               "DATE"              },
            {  L_,   Datum::e_TIME,               "TIME"              },
            {  L_,   Datum::e_DATETIME,           "DATETIME"          },
            {  L_,   Datum::e_DATETIME_INTERVAL,  "DATETIME_INTERVAL" },
            {  L_,   Datum::e_INTEGER64,          "INTEGER64"         },
            {  L_,   Datum::e_USERDEFINED,        "USERDEFINED"       },
            {  L_,   Datum::e_ARRAY,              "ARRAY"             },
            {  L_,   Datum::e_MAP,                "MAP"               },
            {  L_,   Datum::e_BINARY,             "BINARY"            },
            {  L_,   Datum::e_DECIMAL64,          "DECIMAL64"         },
        };

        const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting '<<' operator for 'Datum::DataType'."
                          << endl;
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int             LINE  = DATA[ti].d_lineNum;
            const Datum::DataType VALUE = DATA[ti].d_value;
            const string          EXP   = DATA[ti].d_exp_p;

            if (veryVerbose) { T_; P_(ti); P(VALUE); }
            if (veryVerbose) cout << "EXPECTED FORMAT: " << EXP << endl;

            bslma::TestAllocator         sa("stream",  veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard sag(&sa);

            ostringstream os(&sa);

            ASSERTV(LINE, &os == &(os << VALUE));

            if (veryVerbose) cout << "  ACTUAL FORMAT: " << os.str() << endl;

            ASSERTV(LINE, ti, EXP, os.str(), EXP == os.str());
        }

        if (verbose) cout << "\tNothing is written to a bad stream." << endl;
        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int             LINE  = DATA[ti].d_lineNum;
            const Datum::DataType VALUE = DATA[ti].d_value;
            const string          EXP   = DATA[ti].d_exp_p;

            if (veryVerbose) { T_; P_(ti); P(VALUE); }

            bslma::TestAllocator         sa("stream",  veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard sag(&sa);

            ostringstream os(&sa);
            os.setstate(ios::failbit);

            ASSERTV(LINE, &os == &(os << VALUE));

            ASSERTV(LINE, ti, os.str(), os.str().empty());
        }

        if (verbose) cout << "\nVerify '<<' operator signature." << endl;
        {
            using namespace bdld;

            typedef bsl::ostream& (*FuncPtr)(bsl::ostream&, Datum::DataType);

            const FuncPtr FP = &operator<<;

            (void)FP;   // silence potential compiler warning
        }
      } break;
      case 28: {
        // -------------------------------------------------------------------
        // TESTING ENUMERATIONS AND 'dataTypeToAscii'
        //
        // Concerns:
        //: 1 The enumerator values are sequential, starting from 0.
        //:
        //: 2 The 'dataTypeTooAscii' method returns the expected string
        //:   representation for each enumerator.
        //:
        //: 3 The 'dataTypeToAscii' method returns a distinguished string when
        //:   passed an out-of-band value.
        //:
        //: 4 The string returned by 'dataTypeToAscii' is non-modifiable.
        //:
        //: 5 The 'dataTypeToAscii' method has the expected signature.
        //:
        //: 6 The 'k_NUM_TYPES' enumerator's value equals the number of types
        //:   of values that can be stored inside 'Datum'.
        //
        // Plan:
        //: 1 Confirm that the 'k_NUM_TYPES' enumerator's value equals the
        //:   number of types of values that can be stored inside 'Datum'.
        //:   (C-6)
        //:
        //: 2 Verify that the enumerator values are sequential, starting from
        //:   1.  (C-1)
        //:
        //: 3 Verify that the 'dataTypeToAscii' method returns the expected
        //:   string representation for each enumerator.  (C-2)
        //:
        //: 4 Verify that the 'dataTypeToAascii' method returns a distinguished
        //:   string when passed an out-of-band value.  (C-3)
        //:
        //: 5 Take the address of the 'dataTypeToAscii' (class) method and use
        //:   the result to initialize a variable of the appropriate type.
        //:   (C-4..5)
        //:
        //
        // Testing:
        //   enum DataType { ... };
        //   enum { k_NUM_TYPES = ... };
        //   const char *dataTypeToAscii(Datum::DataType val);
        // -------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING ENUMERATIONS AND 'dataTypeToAscii'" << endl
                 << "==========================================" << endl;

        static const struct {
            int              d_lineNum;  // source line number
            Datum::DataType  d_value;    // enumerator value
            const char      *d_exp_p;    // expected result
        } DATA[] = {
            // LINE  ENUMERATOR VALUE             EXPECTED OUTPUT
            // ----  ---------------------------  -------------------
            {  L_,   Datum::e_NIL,                "NIL"               },
            {  L_,   Datum::e_INTEGER,            "INTEGER"           },
            {  L_,   Datum::e_DOUBLE,             "DOUBLE"            },
            {  L_,   Datum::e_STRING,             "STRING"            },
            {  L_,   Datum::e_BOOLEAN,            "BOOLEAN"           },
            {  L_,   Datum::e_ERROR,              "ERROR"             },
            {  L_,   Datum::e_DATE,               "DATE"              },
            {  L_,   Datum::e_TIME,               "TIME"              },
            {  L_,   Datum::e_DATETIME,           "DATETIME"          },
            {  L_,   Datum::e_DATETIME_INTERVAL,  "DATETIME_INTERVAL" },
            {  L_,   Datum::e_INTEGER64,          "INTEGER64"         },
            {  L_,   Datum::e_USERDEFINED,        "USERDEFINED"       },
            {  L_,   Datum::e_ARRAY,              "ARRAY"             },
            {  L_,   Datum::e_MAP,                "MAP"               },
            {  L_,   Datum::e_BINARY,             "BINARY"            },
            {  L_,   Datum::e_DECIMAL64,          "DECIMAL64"         },
            {  L_,   Datum::e_INT_MAP,            "INT_MAP"           },
        };

        const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nVerify 'k_NUM_TYPES' enumerator's value."
                          << endl;
        {
            const int NUM_TYPES = 17;  // expected number of types

            ASSERT(NUM_TYPES          == Datum::k_NUM_TYPES);
            ASSERT(Datum::k_NUM_TYPES == NUM_DATA);
        }

        if (verbose) cout << "\nVerify enumerator values are sequential."
                          << endl;

        for (int i = 0; i < Datum::k_NUM_TYPES; ++i) {
            const Datum::DataType VALUE = DATA[i].d_value;

            if (veryVerbose) { T_; P_(i); P(VALUE); }

            ASSERTV(i, VALUE, i == VALUE);
        }

        if (verbose) cout << "\nTesting 'dataTypeToAscii'." << endl;

        for (size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int              LINE  = DATA[ti].d_lineNum;
            const Datum::DataType  VALUE = DATA[ti].d_value;
            const char            *EXP   = DATA[ti].d_exp_p;

            const char *result = Datum::dataTypeToAscii(VALUE);

            if (veryVerbose) { T_; P_(ti); P_(VALUE); P_(EXP); P(result); }

            ASSERTV(LINE, ti, strlen(EXP) == strlen(result));
            ASSERTV(LINE, ti,           0 == strcmp(EXP, result));
        }

        if (verbose) cout << "\nVerify 'dataTypeToAscii' signature." << endl;

        {
            typedef const char *(*FuncPtr)(Datum::DataType);

            const FuncPtr FP = &Datum::dataTypeToAscii;

            (void) FP;   // silence potential compiler warning
        }
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // TESTING 'disposeUninitializedMap'
        //
        // Concerns:
        //: 1 The 'disposeUninitializedMap' method disposes off uninitialized
        //:   map (both owning keys and not) properly.
        //:
        //: 2 Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create uninitialized map.  Verify that memory was allocated.
        //
        //: 2 Dispose the map.  Verify that the allocated memory was released.
        //:   (C-1)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones.  (C-2)
        //
        // Testing:
        //   void disposeUninitializedMap(DatumMutableMapRef *, ...);
        //   void disposeUninitializedMap(DatumMutableMapOwningKeysRef *, ...);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'disposeUninitializedMap'" << endl
                          << "=================================" << endl;

        if (verbose) cout << "\nTesting 'dispose' of a 'DatumMutableMapRef'."
                          << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            DatumMutableMapRef   map;
            SizeType             mapSize = 5;

            Datum::createUninitializedMap(&map, mapSize, &oa);
            ASSERT(0 != oa.numBytesInUse());

            Datum::disposeUninitializedMap(map, &oa);
            ASSERT(0 == oa.numBytesInUse());
        }

        if (verbose)
            cout << "\nTesting 'dispose' of a 'DatumMutableMapOwningKeysRef'."
                 << endl;
        {
            bslma::TestAllocator         oa("object", veryVeryVeryVerbose);
            DatumMutableMapOwningKeysRef map;
            SizeType                     mapSize = 5;
            SizeType                     keysSize= 25;

            Datum::createUninitializedMap(&map, mapSize, keysSize, &oa);
            ASSERT(0 != oa.numBytesInUse());

            Datum::disposeUninitializedMap(map, &oa);
            ASSERT(0 == oa.numBytesInUse());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            bslma::TestAllocator  oa("object", veryVeryVeryVerbose);
            bslma::Allocator     *nullAllocPtr =
                                            static_cast<bslma::Allocator *>(0);

            (void) nullAllocPtr;  // suppress compiler warning

            if (verbose) cout << "\tTesting 'DatumMutableMapRef'."
                              << endl;
            {
                DatumMutableMapRef map;
                Datum::createUninitializedMap(&map, 0, &oa);

                ASSERT_SAFE_FAIL(Datum::disposeUninitializedMap(map,
                                                                nullAllocPtr));
                ASSERT_SAFE_PASS(Datum::disposeUninitializedMap(map, &oa));
            }

            if (verbose) cout << "\tTesting 'DatumMutableMapOwningKeysRef'."
                              << endl;
            {
                DatumMutableMapOwningKeysRef map;
                Datum::createUninitializedMap(&map, 0, 0, &oa);

                ASSERT_SAFE_FAIL(Datum::disposeUninitializedMap(map,
                                                                nullAllocPtr));
                ASSERT_SAFE_PASS(Datum::disposeUninitializedMap(map, &oa));
            }
            ASSERT(0 == oa.status());
         }
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING 'disposeUninitializedArray'
        //
        // Concerns:
        //: 1 The 'disposeUninitializedArray' method disposes off uninitialized
        //:   array properly.
        //:
        //: 2 Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create uninitialized array.  Verify that memory was allocated.
        //
        //: 2 Dispose the array.  Verify that the allocated memory was
        //:   released.  (C-1)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones.  (C-2)
        //
        // Testing:
        //   void disposeUninitializedArray(Datum *, basicAllocator *);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'disposeUninitializedArray'" << endl
                          << "===================================" << endl;

        if (verbose) cout << "\nTesting 'dispose' of a 'DatumMutableArrayRef'."
                          << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            DatumMutableArrayRef array;
            SizeType             arraySize = 5;

            Datum::createUninitializedArray(&array, arraySize, &oa);
            ASSERT(0 != oa.numBytesInUse());

            Datum::disposeUninitializedArray(array, &oa);
            ASSERT(0 == oa.numBytesInUse());
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            bslma::TestAllocator  oa("object", veryVeryVeryVerbose);
            bslma::Allocator     *nullAllocPtr =
                                            static_cast<bslma::Allocator *>(0);
            DatumMutableArrayRef  array;

            (void) nullAllocPtr;  // suppress compiler warning

            Datum::createUninitializedArray(&array, 0, &oa);

            ASSERT_SAFE_FAIL(Datum::disposeUninitializedArray(array,
                                                              nullAllocPtr));
            ASSERT_SAFE_PASS(Datum::disposeUninitializedArray(array, &oa));

            ASSERT(0 == oa.status());
         }
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //   The definition of the equality of two 'Datum' objects depends on
        //   the type and the value of the data held by the objects.  Note,
        //   that for 'double' and 'Decimal64' values of 'NaN', the identity
        //   property of the quality is violated ( i.e. two 'NaN' objects never
        //   compare equal). Also note, that this test covers 'Datum's holding
        //   all possible data type ( including aggregated data types)
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if their
        //:   corresponding type and values compare equal (except 'NaN')
        //:
        //: 2 'true  == (X == X)' (i.e., identity).
        //:
        //: 3 'false == (X != X)' (i.e., identity).
        //:
        //: 4 'X == Y' if and only if 'Y == X' (i.e., commutativity).
        //:
        //: 5 'X != Y' if and only if 'Y != X' (i.e., commutativity).
        //:
        //: 6 'X != Y' if and only if '!(X == Y)'.
        //:
        //: 7 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 8 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //: 9 The equality-comparison operators' signatures and return types
        //:   are standard.
        //:
        //:10 Two Datums holding 'NaN' values are not equal.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free
        //:   equality-comparison operators defined in this component.
        //:   (C-7..9)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their type and value
        //:   representation.
        //:
        //: 3 For each row 'R1' in the table of P-2:  (C-1..6)
        //:
        //:   1 Create a 'const' reference to an object, 'X', having the value
        //:     from 'R1'.
        //:
        //:   2 Using 'X', verify the reflexive (anti-reflexive) property of
        //:     equality (inequality) in the presence of aliasing.  (C-2..3)
        //:
        //:   3 For each row 'R2' in the table of P-2:  (C-1, 4..6)
        //:
        //:     1 Record, in 'EXP', whether or not distinct objects set to
        //:       values from 'R1' and 'R2', respectively, are expected to have
        //:       the same value.
        //:
        //:     2 Create a 'const' reference to an object, 'Y', having the
        //:       value from 'R2'.
        //:
        //:     3 Using 'X' and 'Y', verify the commutativity property and
        //:       expected return value for both '==' and '!='.  (C-1, 4..6)
        //
        // Testing:
        //   bool operator==(const Datum&, const Datum&);  // aggregate
        //   bool operator!=(const Datum&, const Datum&);  // aggregate
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "EQUALITY-COMPARISON OPERATORS" << endl
                          << "=============================" << endl;

        if (verbose)
            cout << "\nCreate a table of distinct type/value combinations."
                 << "\nIncluding aggregate data types."
                 << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            bsl::vector<Datum> vector1;

            // creating table with distinct data types
            populateWithNonAggregateValues(&vector1, &oa, false);

            DatumMutableArrayRef array;
            Datum::createUninitializedArray(&array, vector1.size(), &oa);
            for (size_t i = 0; i < vector1.size(); ++i) {
                array.data()[i] = vector1[i];
            }
            *(array.length()) = vector1.size();

            const SizeType     mapSize = 6;
            DatumMutableMapRef map1;
            Datum::createUninitializedMap(&map1, mapSize, &oa);
            map1.data()[0] = DatumMapEntry(StringRef("one"),
                                           Datum::createInteger(0));
            map1.data()[1] = DatumMapEntry(StringRef("two"),
                                           Datum::createDouble(-3.1416));
            map1.data()[2] = DatumMapEntry(
                                      StringRef("three"),
                                      Datum::copyString("A long string", &oa));
            map1.data()[3] = DatumMapEntry(StringRef("four"),
                                           Datum::copyString("Abc", &oa));
            map1.data()[4] = DatumMapEntry(
                                      StringRef("five"),
                                      Datum::createDate(bdlt::Date(2010,1,5)));
            map1.data()[5] = DatumMapEntry(StringRef("six"),
                                           Datum::adoptArray(array));
            *(map1.size()) = mapSize;

            const SizeType        intMapSize = 5;
            DatumMutableIntMapRef intMap1;
            Datum::createUninitializedIntMap(&intMap1, intMapSize, &oa);
            intMap1.data()[0] = DatumIntMapEntry(1, Datum::createInteger(0));
            intMap1.data()[1] = DatumIntMapEntry(2,
                                                 Datum::createDouble(-3.1416));
            intMap1.data()[2] = DatumIntMapEntry(3,
                                      Datum::copyString("A long string", &oa));
            intMap1.data()[3] = DatumIntMapEntry(4,
                                                Datum::copyString("Abc", &oa));
            intMap1.data()[4] = DatumIntMapEntry(5,
                                      Datum::createDate(bdlt::Date(2010,1,5)));
            *(intMap1.size()) = intMapSize;

            // Create an array out of all the previously created map.
            Datum::createUninitializedArray(&array, 2, &oa);
            array.data()[0] = Datum::adoptMap(map1);
            array.data()[1] = Datum::adoptIntMap(intMap1);
            *(array.length()) = 2;
            vector1.push_back(Datum::adoptArray(array));

            // Create second vector with map owning keys instead of map
            bsl::vector<Datum> vector2;
            populateWithNonAggregateValues(&vector2, &oa, false);

            Datum::createUninitializedArray(&array, vector2.size(), &oa);
            for (size_t i = 0; i < vector2.size(); ++i) {
                array.data()[i] = vector2[i];
            }
            *(array.length()) = vector2.size();

            const char *keys[mapSize] = { "one",
                                          "two",
                                          "three",
                                          "four",
                                          "five",
                                          "six" };
            SizeType    keysSize = 0;
            for (size_t i = 0; i < mapSize; ++i) {
                keysSize += strlen(keys[i]);
            }
            DatumMutableMapOwningKeysRef map2;
            Datum::createUninitializedMap(&map2, mapSize, keysSize, &oa);
            char *keyBuffer = map2.keys();
            memcpy(keyBuffer, keys[0], strlen(keys[0]));
            map2.data()[0] = DatumMapEntry(
                       StringRef(keyBuffer, static_cast<int>(strlen(keys[0]))),
                       Datum::createInteger(0));
            keyBuffer += strlen(keys[0]);
            memcpy(keyBuffer, keys[1], strlen(keys[1]));
            map2.data()[1] = DatumMapEntry(
                       StringRef(keyBuffer, static_cast<int>(strlen(keys[1]))),
                       Datum::createDouble(-3.1416));
            keyBuffer += strlen(keys[1]);
            memcpy(keyBuffer, keys[2], strlen(keys[2]));
            map2.data()[2] = DatumMapEntry(
                       StringRef(keyBuffer, static_cast<int>(strlen(keys[2]))),
                       Datum::copyString("A long string", &oa));
            keyBuffer += strlen(keys[2]);
            memcpy(keyBuffer, keys[3], strlen(keys[3]));
            map2.data()[3] = DatumMapEntry(
                       StringRef(keyBuffer, static_cast<int>(strlen(keys[3]))),
                       Datum::copyString("Abc", &oa));
            keyBuffer += strlen(keys[3]);
            memcpy(keyBuffer, keys[4], strlen(keys[4]));
            map2.data()[4] = DatumMapEntry(
                       StringRef(keyBuffer, static_cast<int>(strlen(keys[4]))),
                       Datum::createDate(bdlt::Date(2010,1,5)));
            keyBuffer += strlen(keys[4]);
            memcpy(keyBuffer, keys[5], strlen(keys[5]));
            map2.data()[5] = DatumMapEntry(
                       StringRef(keyBuffer, static_cast<int>(strlen(keys[5]))),
                       Datum::adoptArray(array));
            *(map2.size()) = mapSize;

            DatumMutableIntMapRef intMap2;
            Datum::createUninitializedIntMap(&intMap2, intMapSize, &oa);
            intMap2.data()[0] = DatumIntMapEntry(1, Datum::createInteger(0));
            intMap2.data()[1] = DatumIntMapEntry(2,
                                                 Datum::createDouble(-3.1416));
            intMap2.data()[2] = DatumIntMapEntry(3,
                                      Datum::copyString("A long string", &oa));
            intMap2.data()[3] = DatumIntMapEntry(4,
                                                Datum::copyString("Abc", &oa));
            intMap2.data()[4] = DatumIntMapEntry(5,
                                      Datum::createDate(bdlt::Date(2010,1,5)));
            *(intMap2.size()) = intMapSize;

            // Create an array out of all the previously created map.
            Datum::createUninitializedArray(&array, 2, &oa);
            array.data()[0] = Datum::adoptMap(map2);
            array.data()[1] = Datum::adoptIntMap(intMap2);
            *(array.length()) = 2;
            vector2.push_back(Datum::adoptArray(array));

            ASSERT(vector1.size() == vector2.size());

            if (verbose)
                cout << "\nCompare every value with every value." << endl;
            {
                for (size_t i = 0; i < vector1.size(); ++i) {
                    const Datum& X = vector1[i];

                    if (veryVerbose) { T_ P(X) }

                    // Verify compare with itself (alias test).
                    ASSERTV(X,   X == X);
                    ASSERTV(X, !(X != X));

                    for (size_t j = 0; j < vector2.size(); ++j) {
                        const Datum& Y   = vector2[j];
                        const bool   EXP = (i == j); // expected equality


                        if (veryVerbose) { T_ T_ P_(EXP) P_(X) P(Y) }

                        // Verify value and commutative property

                        ASSERTV(Y, X,  EXP == (Y == X));
                        ASSERTV(X, Y,  EXP == (X == Y));

                        ASSERTV(Y, X, !EXP == (Y != X));
                        ASSERTV(X, Y, !EXP == (X != Y));
                    }
                }
            }

            Datum::destroy(vector1[vector1.size()-1], &oa);
            Datum::destroy(vector2[vector2.size()-1], &oa);

            ASSERT(0 == oa.status());
        }
      } break;
      case 24: {
        // ------------------------------------------------------------------
        // TESTING PROCTOR
        //
        // Concerns:
        //: 1 Proctor destructor destroys managed objects and releases
        //:   allocated memory when exception is thrown.
        //:
        //: 2 Proctor destructor doesn't affect managed objects if proctor's
        //:   'release' method is called before destruction.
        //
        // Plan:
        //: 1 Create 'Datum' object, holding an array.
        //:
        //: 2 Throw an exception (using BSLMA_TESTALLOCATOR_EXCEPTION_TEST)
        //:   mechanism while copying and verify that there are no memory
        //:   leaks.  (C-1)
        //:
        //: 3 Create a copy and verify that proctor doesn't affect the cloned
        //:   object if no exception occurs.  (C-2)
        //:
        //: 4 Create 'Datum' object, holding a map owning keys.
        //:
        //: 5 Throw an exception (using BSLMA_TESTALLOCATOR_EXCEPTION_TEST)
        //:   mechanism during it's copying and verify that no memory leaks.
        //:   (C-1)
        //:
        //: 6 Create a copy and verify that proctor doesn't affect clone
        //:   object if no exception occurs.  (C-2)
        //:
        //: 7 Create 'Datum' object, holding a map with external keys.
        //:
        //: 8 Throw an exception (using BSLMA_TESTALLOCATOR_EXCEPTION_TEST)
        //:   mechanism during it's copying and verify that no memory leaks.
        //:   (C-1)
        //:
        //: 9 Create a copy and verify that proctor doesn't affect clone
        //:   object if no exception occurs.  (C-2)
        //
        // Testing:
        //   Datum_ArrayProctor
        // ------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING PROCTOR" << endl
                          << "===============" << endl;
        bslma::TestAllocator ta("test", veryVeryVerbose);
        const SizeType       SIZE = 5;

        if (verbose) cout << "\nTesting 'Datum_ArrayProctor for arrays'."
                          << endl;
        {
            // Original array creation.

            DatumMutableArrayRef origin;
            Datum::createUninitializedArray(&origin, SIZE, &ta);

            for (size_t i = 0; i < SIZE; ++i) {
                origin.data()[i] = Datum::copyString(
                                                    "A long string constant",
                                                    &ta);
            }
            *(origin.length()) = SIZE;

            Datum        mD = Datum::adoptArray(origin);
            const Datum& D  = mD;

            ASSERT(0 != ta.numBytesInUse());
            Int64 numBytesInUse = ta.numBytesInUse();

            // Throw and catch an exception during cloning.

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {
                Datum mDCopy = D.clone(&ta);

                // Object will be created only if no exception occurs during
                // cloning.  Then we need to destroy it manually.  Otherwise
                // memory should be released by proctor.

                const Datum& DCopy = mDCopy;

                ASSERT(true == DCopy.isArray());
                ASSERT(SIZE == DCopy.theArray().length());
                ASSERT(true == DCopy.theArray()[0].isString());

                Datum::destroy(mDCopy, &ta);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV(numBytesInUse == ta.numBytesInUse());

            // Destroy the 'Datum' object.
            Datum::destroy(D, &ta);
        }

        if (verbose) cout << "\nTesting 'Datum_ArrayProctor for maps'."
                          << endl;
        {
            if (verbose) cout <<
                  "\nTesting cloning of the map with external keys." << endl;

            // Original map creation.

            const char *keys[SIZE] = { "one",
                                       "two",
                                       "three",
                                       "four",
                                       "five" };

            DatumMutableMapRef origin;
            Datum::createUninitializedMap(&origin, SIZE, &ta);

            for (size_t i = 0; i < SIZE; ++i) {
                origin.data()[i] = DatumMapEntry(
                             StringRef(keys[i]),
                             Datum::copyString("A long string constant", &ta));
            }
            *(origin.size()) = SIZE;

            Datum        mD = Datum::adoptMap(origin);
            const Datum& D  = mD;

            ASSERT(0 != ta.numBytesInUse());
            const Int64 numBytesInUse = ta.numBytesInUse();

            // Throw and catch an exception during cloning.

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {
                const Datum mDCopy = D.clone(&ta);

                // Object will be created only if no exception occurs during
                // cloning.  Then we need to destroy it manually.  Otherwise
                // memory should be released by proctor.

                const Datum& DCopy = mDCopy;

                ASSERT(true == DCopy.isMap());
                ASSERT(SIZE == DCopy.theMap().size());
                ASSERT(true == DCopy.theMap()[0].value().isString());

                Datum::destroy(mDCopy, &ta);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV(numBytesInUse == ta.numBytesInUse());

            // Destroy the 'Datum' object.
            Datum::destroy(mD, &ta);
        }

        if (verbose) cout << "\nTesting cloning of the map owning keys."
                          << endl;
        {
            // Original map creation.

            const char *keys[SIZE] = { "one",
                                       "two",
                                       "three",
                                       "four",
                                       "five" };
            SizeType    keysSize = 0;
            for (size_t i = 0; i < SIZE; ++i) {
                keysSize += strlen(keys[i]);
            }
            DatumMutableMapOwningKeysRef origin;
            Datum::createUninitializedMap(&origin, SIZE, keysSize, &ta);
            char *mapKeys = origin.keys();

            for (size_t i = 0; i < SIZE; ++i) {
                const int keySize = static_cast<int>(strlen(keys[i]));
                memcpy(mapKeys, keys[i], keySize);
                origin.data()[i] =
                    DatumMapEntry(StringRef(mapKeys, keySize),
                                  Datum::copyString("A long string constant",
                                  &ta));
                mapKeys += strlen(keys[i]);
            }
            *(origin.size()) = SIZE;

            Datum        mD = Datum::adoptMap(origin);
            const Datum& D  = mD;

            ASSERT(0 != ta.numBytesInUse());
            const Int64 numBytesInUse = ta.numBytesInUse();

            // Throw and catch an exception during cloning.

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {
                const Datum mDCopy = D.clone(&ta);

                // Object will be created only if no exception occurs during
                // cloning.  Then we need to destroy it manually.  Otherwise
                // memory should be released by proctor.

                const Datum& DCopy = mDCopy;

                ASSERT(true == DCopy.isMap());
                ASSERT(SIZE == DCopy.theMap().size());
                ASSERT(true == DCopy.theMap()[0].value().isString());

                Datum::destroy(mDCopy, &ta);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV(numBytesInUse == ta.numBytesInUse());

            // Destroy the 'Datum' object.
            Datum::destroy(D, &ta);
        }
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING 'clone' METHOD
        //
        // Concerns:
        //: 1 'Datum::clone' makes a deep copy of the 'Datum' object.
        //:
        //: 2 Memory should not be allocated when values are stored inline.
        //:
        //: 3 When the Datum references external data, the 'clone' method makes
        //:   a full copy of this externally referenced data.
        //
        // Plan:
        //: 1 Create 'Datum' objects by cloning the original objects holding
        //:   different value types.
        //:
        //: 2 Verify that both objects have the same type and value.  (C-1)
        //:
        //: 3 Verify that no memory was allocated when the copied value uses
        //:   inline object storage.  (C-2)
        //:
        //: 4 Verify that for externally referenced data the clone creates a
        //:   deep copy of the referenced data.  (C-3)
        //
        // Testing:
        //   Datum clone(bslma::Allocator *basicAllocator) const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'clone' METHOD" << endl
                          << "======================" << endl;

        if (verbose) cout << "\nTesting 'clone' for Datums having "
                          << "non-aggregate values." << endl;
        {
            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard guard(&da);

            if (verbose) cout << "\nTesting 'clone' for boolean." << endl;
            {
                const static bool DATA[] = { true, false };
                const size_t      DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i< DATA_LEN; ++i) {
                    const bool VALUE = DATA[i];

                    if (veryVerbose) { T_ P(VALUE) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bslma::TestAllocator ca("clone",  veryVeryVeryVerbose);

                    const Datum D = Datum::createBoolean(VALUE);

                    ASSERT(0 == oa.numBlocksInUse()); // non allocating type

                    ASSERT(true  == D.isBoolean());
                    ASSERT(VALUE == D.theBoolean());

                    const Datum DC = D.clone(&ca);

                    ASSERT(0 == oa.numBlocksInUse()); // non allocating type
                    ASSERT(0 == ca.numBlocksInUse()); // non allocating type

                    ASSERT(false == DC.isExternalReference());
                    ASSERT(true  == DC.isBoolean());
                    ASSERT(VALUE == DC.theBoolean());

                    Datum::destroy(D, &oa);
                    Datum::destroy(DC, &ca);

                    ASSERT(0 == oa.status());
                    ASSERT(0 == ca.status());
                }
            }

            if (verbose) cout << "\nTesting 'clone' for 'Date'." << endl;
            {
                const static bdlt::Date DATA[] = {
                    Date(),
                    Date(1999, 12, 31),
                    Date(2015,  1,  1),
                    Date(2200,  8, 12),
                    Date(9999, 12, 31),
                };

                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i< DATA_LEN; ++i) {
                    const bdlt::Date VALUE = DATA[i];

                    if (veryVerbose) { T_ P(VALUE) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bslma::TestAllocator ca("clone",  veryVeryVeryVerbose);

                    const Datum  D = Datum::createDate(VALUE);

                    ASSERT(0 == oa.numBlocksInUse()); // non allocating type

                    ASSERT(true  == D.isDate());
                    ASSERT(VALUE == D.theDate());

                    const Datum DC = D.clone(&ca);

                    ASSERT(0 == oa.numBlocksInUse()); // non allocating type
                    ASSERT(0 == ca.numBlocksInUse()); // non allocating type

                    ASSERT(false == DC.isExternalReference());

                    Datum::destroy(D, &oa);
                    Datum::destroy(DC, &ca);

                    ASSERT(0 == oa.status());
                    ASSERT(0 == ca.status());
                }
            }

            if (verbose) cout << "\nTesting 'clone' for 'Datetime'." << endl;
            {
                const static bdlt::Datetime DATA[] = {
                    Datetime(),
                    Datetime(1999, 12, 31, 12, 45, 31,  18, 34),
                    Datetime(2015,  2,  2,  1,  1,  1,   1, 1),
                    Datetime(2200,  9, 11, 18, 10, 59, 458, 342),
                    Datetime(9999,  9,  9,  9,  9,  9, 999, 999),
                    Datetime(1999, 12, 31, 12, 45, 31,  18),
                    Datetime(2015,  2,  2,  1,  1,  1,   1),
                    Datetime(2200,  9, 11, 18, 10, 59, 458),
                    Datetime(9999,  9,  9,  9,  9,  9, 999),
                };

                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i< DATA_LEN; ++i) {
                    const bdlt::Datetime VALUE = DATA[i];

                    if (veryVerbose) { T_ P(VALUE) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bslma::TestAllocator ca("clone",  veryVeryVeryVerbose);

                    const Datum D = Datum::createDatetime(VALUE, &oa);

                    ASSERT(0 == da.numBlocksInUse());

                    ASSERT(true  == D.isDatetime());
                    ASSERT(VALUE == D.theDatetime());

                    const Datum DC = D.clone(&ca);

                    ASSERT(0 == da.numBlocksInUse());

                    ASSERT(false == DC.isExternalReference());
                    ASSERT(true  == DC.isDatetime());
                    ASSERT(VALUE == DC.theDatetime());

                    Datum::destroy(D, &oa);
                    Datum::destroy(DC, &ca);

                    ASSERT(0 == oa.status());
                    ASSERT(0 == ca.status());
                }
            }

            if (verbose)
                cout << "\nTesting 'clone' for 'DatetimeInterval'." << endl;
            {
                const static bdlt::DatetimeInterval DATA[] = {
                    DatetimeInterval(),
                    DatetimeInterval(0, 0, 0, 0,  1),
                    DatetimeInterval(0, 0, 0, 0, -1),
                    DatetimeInterval(0, 0, 0,  1),
                    DatetimeInterval(0, 0, 0, -1),
                    DatetimeInterval(0, 0,  1),
                    DatetimeInterval(0, 0, -1),
                    DatetimeInterval(0,  1),
                    DatetimeInterval(0, -1),
                    DatetimeInterval( 1),
                    DatetimeInterval(-1),
                    DatetimeInterval(1, 1, 1, 1, 1),
                    DatetimeInterval(-1, -1, -1, -1, -1),
                    DatetimeInterval(1000, 12, 24),
                    DatetimeInterval(100000000, 1, 1, 32, 587),
                    DatetimeInterval(-100000000, 3, 2, 14, 319),
                };

                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i< DATA_LEN; ++i) {
                    const bdlt::DatetimeInterval VALUE = DATA[i];

                    if (veryVerbose) { T_ P(VALUE) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bslma::TestAllocator ca("clone",  veryVeryVeryVerbose);

                    const Datum D = Datum::createDatetimeInterval(VALUE, &oa);

                    const Int64 bytesInUse = oa.numBytesInUse();

                    ASSERT(true  == D.isDatetimeInterval());
                    ASSERT(VALUE == D.theDatetimeInterval());

                    const Datum DC = D.clone(&ca);

                    ASSERT(bytesInUse == ca.numBytesInUse());

                    ASSERT(false == DC.isExternalReference());
                    ASSERT(true  == DC.isDatetimeInterval());
                    ASSERT(VALUE == DC.theDatetimeInterval());

                    Datum::destroy(D, &oa);
                    Datum::destroy(DC, &ca);

                    ASSERT(0 == oa.status());
                    ASSERT(0 == ca.status());
                }
            }

            if (verbose) cout << "\nTesting 'clone' for 'Decimal64'." << endl;
            {
                const static struct {
                    int               d_line;          // line number
                    bdldfp::Decimal64 d_value;         // 'Decimal64' value
                    bool              d_compareEqual;  // compare equal itself
                } DATA[] = {
                    //LINE VALUE                           EQUAL
                    //---- ---------------------           -----
                    { L_,  BDLDFP_DECIMAL_DD(0.0),         true  },
                    { L_,  BDLDFP_DECIMAL_DD(0.253),       true  },
                    { L_,  BDLDFP_DECIMAL_DD(-0.253),      true  },
                    { L_,  BDLDFP_DECIMAL_DD(1.0),         true  },
                    { L_,  BDLDFP_DECIMAL_DD(-1.0),        true  },
                    { L_,  BDLDFP_DECIMAL_DD(12.345),      true  },
                    { L_,  BDLDFP_DECIMAL_DD(12.3456789),  true  },
                    { L_,  k_DECIMAL64_MIN,                true  },
                    { L_,  k_DECIMAL64_MAX,                true  },
                    { L_,  k_DECIMAL64_INFINITY,           true  },
                    { L_,  k_DECIMAL64_NEG_INFINITY,       true  },
                    { L_,  k_DECIMAL64_SNAN,               false },
                    { L_,  k_DECIMAL64_QNAN,               false },
                };
                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i< DATA_LEN; ++i) {
                    const int               LINE  = DATA[i].d_line;
                    const bdldfp::Decimal64 VALUE = DATA[i].d_value;
                    const bool              EQUAL = DATA[i].d_compareEqual;

                    if (veryVerbose) { T_ P_(LINE) P(VALUE) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bslma::TestAllocator ca("clone",  veryVeryVeryVerbose);

                    const Datum D = Datum::createDecimal64(VALUE, &oa);

                    const Int64 bytesInUse = oa.numBytesInUse();

                    ASSERT(true == D.isDecimal64());

                    if (EQUAL) {
                        ASSERT(VALUE == D.theDecimal64());
                    } else {
                        ASSERT(VALUE != D.theDecimal64());
                    }

                    const Datum DC = D.clone(&ca);

                    ASSERT(bytesInUse == ca.numBytesInUse());

                    ASSERT(false == DC.isExternalReference());
                    ASSERT(true  == DC.isDecimal64());

                    if (EQUAL) {
                        ASSERT(VALUE == DC.theDecimal64());
                    } else {
                        ASSERT(VALUE != DC.theDecimal64());
                    }

                    Datum::destroy(D, &oa);
                    Datum::destroy(DC, &ca);

                    ASSERT(0 == oa.status());
                    ASSERT(0 == ca.status());
                }
            }

            if (verbose) cout << "\nTesting 'clone' for 'double'." << endl;
            {
                const static struct {
                    int      d_line;          // line number
                    double   d_value;         // double value
                    bool     d_compareEqual;  // compare equal itself
                } DATA[] = {
                    //LINE VALUE                  EQUAL
                    //---- ---------------------- -----
                    { L_,  0.0,                   true  },
                    { L_,  k_DOUBLE_NEG_ZERO,     true  },
                    { L_,  .01,                   true  },
                    { L_,  -.01,                  true  },
                    { L_,  2.25e-117,             true  },
                    { L_,  2.25e117,              true  },
                    { L_,  1924.25,               true  },
                    { L_,  -1924.25,              true  },
                    { L_,  k_DOUBLE_MIN,          true  },
                    { L_,  k_DOUBLE_MAX,          true  },
                    { L_,  k_DOUBLE_INFINITY,     true  },
                    { L_,  k_DOUBLE_NEG_INFINITY, true  },
                    { L_,  k_DOUBLE_SNAN,         false },
                    { L_,  k_DOUBLE_QNAN,         false },
                    { L_,  k_DOUBLE_LOADED_NAN,   false },
                };

                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i< DATA_LEN; ++i) {
                    const int    LINE  = DATA[i].d_line;
                    const double VALUE = DATA[i].d_value;
                    const bool   EQUAL = DATA[i].d_compareEqual;

                    if (veryVerbose) { T_ P_(LINE) P(VALUE) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bslma::TestAllocator ca("clone",  veryVeryVeryVerbose);

                    const Datum D = Datum::createDouble(VALUE);

                    ASSERT(0 == oa.numBlocksInUse()); // non allocating type

                    ASSERT(true == D.isDouble());

                    if (EQUAL) {
                        ASSERT(VALUE == D.theDouble());
                    } else {
                        ASSERT(VALUE != D.theDouble());

                        if (k_DOUBLE_NAN_BITS_PRESERVED) {
                            // In 64bit 'Datum' we currently store 'NaN' values
                            // unchanged.
                            const double THE_DOUBLE = D.theDouble();
                            ASSERT(bsl::memcmp(&VALUE,
                                &THE_DOUBLE,
                                sizeof(double)) == 0);
                        }
                    }

                    const Datum DC = D.clone(&ca);

                    ASSERT(0 == ca.numBlocksInUse()); // non allocating type

                    ASSERT(false == DC.isExternalReference());
                    ASSERT(true  == DC.isDouble());

                    if (EQUAL) {
                        ASSERT(VALUE == DC.theDouble());
                    } else {
                        ASSERT(VALUE != DC.theDouble());

                        // Cloning a NaN must result in the exact same value
                        // bit-by-bit, regardless that we do not guarantee
                        // preserving all the bits of a NaN in 'createDouble'.
                        const double ORIGINAL_DOUBLE = D.theDouble();
                        const double CLONED_DOUBLE   = DC.theDouble();
                        ASSERT(bsl::memcmp(&ORIGINAL_DOUBLE,
                                           &CLONED_DOUBLE,
                                           sizeof(double)) == 0);
                    }

                    Datum::destroy(D, &oa);
                    Datum::destroy(DC, &ca);

                    ASSERT(0 == oa.status());
                    ASSERT(0 == ca.status());
                }
            }

            if (verbose) cout << "\nTesting 'clone' for 'Error'." << endl;
            {
                const static int DATA [] = {
                    0,
                    1,
                    -1,
                    numeric_limits<char>::min(),
                    numeric_limits<char>::max(),
                    numeric_limits<unsigned char>::max(),
                    numeric_limits<short>::min(),
                    numeric_limits<short>::max(),
                    numeric_limits<unsigned short>::max(),
                    numeric_limits<int>::min(),
                    numeric_limits<int>::max()
                };

                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i< DATA_LEN; ++i) {
                    const int  ERROR = DATA[i];

                    if (veryVerbose) { T_ T_ P(ERROR) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bslma::TestAllocator ca("clone",  veryVeryVeryVerbose);

                    const Datum D = Datum::createError(ERROR);

                    ASSERT(0 == oa.numBlocksInUse()); // non allocating type

                    ASSERT(true == D.isError());
                    ASSERT(DatumError(ERROR) == D.theError());

                    const Datum DC = D.clone(&ca);

                    ASSERT(0 == ca.numBlocksInUse()); // non allocating type

                    ASSERT(false == DC.isExternalReference());
                    ASSERT(true  == DC.isError());
                    ASSERT(DatumError(ERROR) == DC.theError());

                    Datum::destroy(D, &oa);
                    Datum::destroy(DC, &ca);

                    ASSERT(0 == oa.status());
                    ASSERT(0 == ca.status());
                }

                const char *errorMessage = "This is an error#$%\".";
                for (size_t i = 0; i< DATA_LEN; ++i) {
                    for (size_t j = 0; j <= strlen(errorMessage); ++j) {
                        const int       ERROR = DATA[i];
                        const StringRef MESSAGE(errorMessage,
                                                static_cast<int>(j));

                        if (veryVerbose) { T_ T_ P_(ERROR) P(MESSAGE) }

                        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                        bslma::TestAllocator ca("clone",  veryVeryVeryVerbose);

                        const Datum D = Datum::createError(ERROR,
                                                           MESSAGE,
                                                           &oa);

                        const Int64 bytesInUse = oa.numBytesInUse();

                        ASSERT(true == D.isError());
                        ASSERT(DatumError(ERROR, MESSAGE) == D.theError());

                        const Datum DC = D.clone(&ca);

                        ASSERT(bytesInUse == ca.numBytesInUse());

                        ASSERT(false == DC.isExternalReference());
                        ASSERT(true  == DC.isError());
                        ASSERT(DatumError(ERROR, MESSAGE) == DC.theError());

                        Datum::destroy(D, &oa);
                        Datum::destroy(DC, &ca);

                        ASSERT(0 == oa.status());
                        ASSERT(0 == ca.status());
                    }
                }
            }

            if (verbose) cout << "\nTesting 'clone' for 'int'." << endl;
            {
                const static int DATA[] = {
                    0,
                    1,
                    -1,
                    numeric_limits<char>::min(),
                    numeric_limits<char>::max(),
                    numeric_limits<unsigned char>::max(),
                    numeric_limits<short>::min(),
                    numeric_limits<short>::max(),
                    numeric_limits<unsigned short>::max(),
                    numeric_limits<int>::min(),
                    numeric_limits<int>::max(),
                };

                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i< DATA_LEN; ++i) {
                    const int VALUE = DATA[i];

                    if (veryVerbose) { T_ P(VALUE) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bslma::TestAllocator ca("clone",  veryVeryVeryVerbose);

                    const Datum D = Datum::createInteger(VALUE);

                    ASSERT(0 == oa.numBlocksInUse()); // non allocating type

                    ASSERT(true  == D.isInteger());
                    ASSERT(VALUE == D.theInteger());

                    const Datum DC = D.clone(&ca);

                    ASSERT(0 == ca.numBlocksInUse()); // non allocating type

                    ASSERT(false == DC.isExternalReference());
                    ASSERT(true  == DC.isInteger());
                    ASSERT(VALUE == DC.theInteger());

                    Datum::destroy(D, &oa);
                    Datum::destroy(DC, &ca);

                    ASSERT(0 == da.status());
                    ASSERT(0 == oa.status());
                }
            }

            if (verbose) cout << "\nTesting 'clone' for 'int64'." << endl;
            {
                const static Int64 DATA[] = {
                    0,
                    1,
                    -1,
                    numeric_limits<char>::min(),
                    numeric_limits<char>::max(),
                    numeric_limits<unsigned char>::max(),
                    numeric_limits<short>::min(),
                    numeric_limits<short>::max(),
                    numeric_limits<unsigned short>::max(),
                    numeric_limits<int>::min(),
                    numeric_limits<int>::max(),
                    numeric_limits<Int64>::min(),
                    numeric_limits<Int64>::max(),
                };

                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i< DATA_LEN; ++i) {
                    const Int64 VALUE = DATA[i];

                    if (veryVerbose) { T_ P(VALUE) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bslma::TestAllocator ca("clone",  veryVeryVeryVerbose);

                    const Datum D = Datum::createInteger64(VALUE, &oa);

                    const Int64 bytesInUse = oa.numBytesInUse();
#ifdef BSLS_PLATFORM_CPU_64_BIT
                    ASSERT(0 == oa.numBlocksInUse()); // non allocating type
#endif  // BSLS_PLATFORM_CPU_64_BIT

                    ASSERT(true  == D.isInteger64());
                    ASSERT(VALUE == D.theInteger64());

                    const Datum DC = D.clone(&ca);

                    ASSERT(bytesInUse == ca.numBytesInUse());
#ifdef BSLS_PLATFORM_CPU_64_BIT
                    ASSERT(0 == ca.numBlocksInUse()); // non allocating type
#endif  // BSLS_PLATFORM_CPU_64_BIT

                    ASSERT(false == DC.isExternalReference());
                    ASSERT(true  == DC.isInteger64());
                    ASSERT(VALUE == DC.theInteger64());

                    Datum::destroy(D, &oa);
                    Datum::destroy(DC, &ca);

                    ASSERT(0 == oa.status());
                    ASSERT(0 == ca.status());
                }
            }

            if (verbose) cout << "\nTesting 'clone' for 'null'." << endl;
            {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                bslma::TestAllocator ca("clone",  veryVeryVeryVerbose);

                const Datum D = Datum::createNull();

                ASSERT(0 == oa.numBlocksInUse()); // non allocating type

                ASSERT(true == D.isNull());

                const Datum DC = D.clone(&ca);

                ASSERT(0 == ca.numBlocksInUse()); // non allocating type

                ASSERT(false == DC.isExternalReference());
                ASSERT(true  == DC.isNull());

                Datum::destroy(D, &oa);
                Datum::destroy(DC, &ca);

                ASSERT(0 == oa.status());
                ASSERT(0 == ca.status());
            }

            if (verbose) cout << "\nTesting 'clone' for 'StringRef'." << endl;
            {
                const static Datum::SizeType DATA[] = {
                    0,
                    1,
                    2,
                    3,
                    4,
                    5,
                    6,
                    7,
                    8,
                    12,
                    13,
                    14,
                    32,
                    255,
                    256,
                    65534,
                    65535,
                    65536,
                    1024*1024,
                };

                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i< DATA_LEN; ++i) {
                    bslma::TestAllocator  ba("buffer", veryVeryVeryVerbose);
                    const Datum::SizeType SIZE = DATA[i];
                    const bsl::string     BUFFER(SIZE, 'x', &ba);
                    const StringRef       STRING(BUFFER.data(),
                                                 static_cast<int>(SIZE));

                    if (veryVerbose) { T_ T_ P(SIZE) }
                    {
                        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                        bslma::TestAllocator ca("clone",  veryVeryVeryVerbose);

                        const Datum D = Datum::createStringRef(STRING, &oa);

                        ASSERT(true   == D.isExternalReference());
                        ASSERT(true   == D.isString());
                        ASSERT(STRING == D.theString());

                        const Datum DC = D.clone(&ca);

#ifdef BSLS_PLATFORM_CPU_32_BIT
                        // Up to length 6 the strings are stored inline
                        if (SIZE <= 6) {
                            ASSERT(0 == ca.numBytesInUse());
                        } else {
                            ASSERT(0 != ca.numBytesInUse());
                        }
#else  // BSLS_PLATFORM_CPU_32_BIT
                        // Up to length 13 the strings are stored inline
                        if (SIZE <= 13) {
                            ASSERT(0 == ca.numBytesInUse());
                        } else {
                            ASSERT(0 != ca.numBytesInUse());
                        }
#endif // BSLS_PLATFORM_CPU_32_BIT

                        ASSERT(false  == DC.isExternalReference());
                        ASSERT(true   == DC.isString());
                        ASSERT(STRING == DC.theString());

                        Datum::destroy(D, &oa);
                        Datum::destroy(DC, &ca);

                        ASSERT(0 == oa.status());
                        ASSERT(0 == ca.status());
                    }
                }
            }

            if (verbose) cout << "\nTesting 'clone' for 'Time'." << endl;
            {
                const static bdlt::Time DATA[] = {
                    Time(),
                    Time(0, 1, 1, 1, 1),
                    Time(8, 0, 0, 999, 888),
                    Time(23, 59, 59, 999, 999),
                };

                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i< DATA_LEN; ++i) {
                    const bdlt::Time VALUE = DATA[i];

                    if (veryVerbose) { T_ P(VALUE) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bslma::TestAllocator ca("clone",  veryVeryVeryVerbose);

                    const Datum D = Datum::createTime(VALUE);

                    ASSERT(0 == oa.numBlocksInUse()); // non allocating type

                    ASSERT(true  == D.isTime());
                    ASSERT(VALUE == D.theTime());

                    const Datum DC = D.clone(&ca);

                    ASSERT(0 == ca.numBlocksInUse()); // non allocating type

                    ASSERT(false == DC.isExternalReference());
                    ASSERT(true  == DC.isTime());
                    ASSERT(VALUE == DC.theTime());

                    Datum::destroy(D, &oa);
                    Datum::destroy(DC, &ca);

                    ASSERT(0 == oa.status());
                    ASSERT(0 == ca.status());
                }
            }

            if (verbose) cout << "\nTesting 'clone' for 'Udt'." << endl;
            {
                char           BUFFER[] = "This is UDT placeholder";
                const DatumUdt VALUE(BUFFER, 0);

                if (veryVerbose) { T_ P(VALUE) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                bslma::TestAllocator ca("clone",  veryVeryVeryVerbose);

                const Datum D = Datum::createUdt(BUFFER, 0);

                ASSERT(0 == oa.numBlocksInUse()); // non allocating type

                ASSERT(true  == D.isExternalReference());
                ASSERT(true  == D.isUdt());
                ASSERT(VALUE == D.theUdt());

                const Datum DC = D.clone(&ca);

                ASSERT(0 == ca.numBlocksInUse()); // non allocating type

                ASSERT(true  == DC.isExternalReference());
                ASSERT(true  == DC.isUdt());
                ASSERT(VALUE == DC.theUdt());

                Datum::destroy(D, &oa);
                Datum::destroy(DC, &ca);

                ASSERT(0 == oa.status());
                ASSERT(0 == ca.status());
            }

            if (verbose) cout << "\nTesting 'clone' for 'Binary'." << endl;
            {
                for (size_t i = 0; i < 258; ++i) {
                    bslma::TestAllocator     ba("buffer", veryVeryVeryVerbose);
                    bsl::vector<unsigned char> BUFFER(&ba);
                    const size_t               SIZE = i;
                    loadRandomBinary(&BUFFER, SIZE);

                    const DatumBinaryRef REF(BUFFER.data(), SIZE);

                    if (veryVerbose) { T_ P_(SIZE) P(REF) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bslma::TestAllocator ca("clone",  veryVeryVeryVerbose);

                    const Datum D(Datum::copyBinary(BUFFER.data(), SIZE, &oa));

                    const Int64 bytesInUse = oa.numBytesInUse();
#ifdef BSLS_PLATFORM_CPU_32_BIT
                    ASSERT(0 != bytesInUse);   // always allocate
#else  // BSLS_PLATFORM_CPU_32_BIT
                    if (SIZE <= 13) {
                        ASSERT(0 == bytesInUse);
                    } else {
                        ASSERT(0 != bytesInUse);
                    }
#endif // BSLS_PLATFORM_CPU_32_BIT

                    ASSERT(true == D.isBinary());
                    ASSERT(REF  == D.theBinary());

                    const Datum DC = D.clone(&ca);

                    ASSERT(bytesInUse == ca.numBytesInUse());

                    ASSERT(false == DC.isExternalReference());
                    ASSERT(true  == DC.isBinary());
                    ASSERT(REF   == DC.theBinary());

                    Datum::destroy(D, &oa);
                    Datum::destroy(DC, &ca);

                    ASSERT(0 == oa.status());
                    ASSERT(0 == ca.status());
                }
            }

            if (verbose)
                cout << "\nTesting 'clone' for internal 'String'." << endl;
            {
                for (size_t i = 0; i < 258; ++i) {
                    bslma::TestAllocator ba("buffer", veryVeryVeryVerbose);
                    bsl::string          BUFFER(&ba);
                    const size_t         SIZE = i;
                    loadRandomString(&BUFFER, SIZE);

                    const StringRef REF(BUFFER.data(), static_cast<int>(SIZE));

                    if (veryVerbose) { T_ P_(SIZE) P(REF) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                    bslma::TestAllocator ca("clone",  veryVeryVeryVerbose);

                    const Datum D(Datum::copyString(BUFFER.data(), SIZE, &oa));

                    const Int64 bytesInUse = oa.numBytesInUse();
#ifdef BSLS_PLATFORM_CPU_32_BIT
                    // Up to length 6 inclusive the strings are stored inline
                    if (SIZE <= 6) {
#else  // BSLS_PLATFORM_CPU_32_BIT
                    // Up to length 13 inclusive the strings are stored inline
                    if (SIZE <= 13) {
#endif // BSLS_PLATFORM_CPU_32_BIT
                        ASSERT(0 == bytesInUse);
                    } else {
                        ASSERT(0 != bytesInUse);
                    }

                    ASSERT(true == D.isString());
                    ASSERT(REF  == D.theString());

                    const Datum DC = D.clone(&ca);

                    ASSERT(bytesInUse == ca.numBytesInUse());

                    ASSERT(false == DC.isExternalReference());
                    ASSERT(true  == DC.isString());
                    ASSERT(REF   == DC.theString());

                    Datum::destroy(D, &oa);
                    Datum::destroy(DC, &ca);

                    ASSERT(0 == oa.status());
                    ASSERT(0 == ca.status());
                }
            }
        }
        if (verbose) cout << "\nTesting 'clone' for Datums having "
                          << "aggregate values." << endl;

        if (verbose) cout << "\nTesting 'clone' for an array." << endl;
        {
            // Testing cloning of an empty array
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            bslma::TestAllocator ca("clone",  veryVeryVeryVerbose);

            DatumMutableArrayRef array;
            const Datum          D = Datum::adoptArray(array);

            Datum               dummyDatum;
            const DatumArrayRef REF(&dummyDatum, 0);

            ASSERT(true == D.isArray());
            ASSERT(REF  == D.theArray());

            const Int64 bytesInUse = oa.numBytesInUse();

            const Datum DC = D.clone(&ca);

            ASSERT(bytesInUse == ca.numBytesInUse());

            ASSERT(false == DC.isExternalReference());
            ASSERT(true  == DC.isArray());
            ASSERT(REF   == DC.theArray());

            Datum::destroy(D, &oa);
            Datum::destroy(DC, &ca);
        }

        {
            // Testing cloning of a non-empty array
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            bslma::TestAllocator ca("clone",  veryVeryVeryVerbose);

            const SizeType       arraySize  = 6;
            DatumMutableArrayRef array;

            Datum::createUninitializedArray(&array, arraySize, &oa);
            array.data()[0] = Datum::createInteger(0);
            array.data()[1] = Datum::createDouble(-3.1416);
            array.data()[2] = Datum::copyString("A long string", &oa);
            array.data()[3] = Datum::copyString("Abc", &oa);
            array.data()[4] = Datum::createDate(bdlt::Date(2010,1,5));
            array.data()[5] = Datum::createDatetime(
                                         bdlt::Datetime(2010,1,5, 16,45,32,12),
                                         &oa);
            *(array.length()) = arraySize;
            const Datum D = Datum::adoptArray(array);

            const DatumArrayRef REF(array.data(), 6);

            ASSERT(true == D.isArray());
            ASSERT(REF  == D.theArray());

            const Int64 bytesInUse = oa.numBytesInUse();

            const Datum DC = D.clone(&ca);

            ASSERT(bytesInUse == ca.numBytesInUse());

            ASSERT(false == DC.isExternalReference());
            ASSERT(true  == DC.isArray());
            ASSERT(REF   == DC.theArray());

            Datum::destroy(D, &oa);
            Datum::destroy(DC, &ca);

            ASSERT(0 == oa.status());
            ASSERT(0 == ca.status());
        }

        if (verbose) cout << "\nTesting 'clone' for a map." << endl;
        {
            // Testing cloning of an empty map
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            bslma::TestAllocator ca("clone",  veryVeryVeryVerbose);

            DatumMutableMapRef map;
            const Datum        D = Datum::adoptMap(map);

            ASSERT(true  == D.isMap());
            // Empty maps never owns the keys.
            ASSERT(false == D.theMap().ownsKeys());

            const Int64 bytesInUse = oa.numBytesInUse();

            const Datum DC = D.clone(&ca);

            ASSERT(bytesInUse == ca.numBytesInUse());

            ASSERT(false == DC.isExternalReference());
            ASSERT(true  == DC.isMap());
            // Empty maps never owns the keys.
            ASSERT(false == DC.theMap().ownsKeys());
            ASSERT(D.theMap() == DC.theMap());

            Datum::destroy(D, &oa);
            Datum::destroy(DC, &ca);

            ASSERT(0 == oa.status());
            ASSERT(0 == ca.status());
        }
        {
            // Testing cloning of a non-empty map
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            bslma::TestAllocator ca("clone",  veryVeryVeryVerbose);

            StringRef key1("one");
            StringRef key2("two");
            StringRef key3("three");
            StringRef key4("four");

            const SizeType     mapSize = 4;
            DatumMutableMapRef map;
            Datum::createUninitializedMap(&map, mapSize, &oa);
            map.data()[0] = DatumMapEntry(key1, Datum::createInteger(0));
            map.data()[1] = DatumMapEntry(key2, Datum::createDouble(-3.141));
            map.data()[2] = DatumMapEntry(key3,
                                          Datum::copyString("A long string",
                                                            &oa));
            map.data()[3] = DatumMapEntry(key4,
                                          Datum::copyString("Abc", &oa));
            *(map.size()) = mapSize;
            *(map.sorted()) = false;
            const Datum D = Datum::adoptMap(map);

            ASSERT(true  == D.isMap());
            ASSERT(false == D.theMap().ownsKeys());
            ASSERT(0     != oa.numBytesInUse());

            const Datum DC = D.clone(&ca);

            ASSERT(0    != ca.numBytesInUse());

            ASSERT(false == DC.isExternalReference());
            ASSERT(true  == DC.isMap());
            ASSERT(true  == DC.theMap().ownsKeys());
            ASSERT(D.theMap() == DC.theMap());

            Datum::destroy(D, &oa);
            Datum::destroy(DC, &ca);

            ASSERT(0 == oa.status());
            ASSERT(0 == ca.status());
        }

        if (verbose)
            cout << "\nTesting 'clone' for a map owning keys." << endl;
        {
            // Testing cloning of an empty map
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            bslma::TestAllocator ca("clone",  veryVeryVeryVerbose);

            DatumMutableMapOwningKeysRef map;
            const Datum                  D = Datum::adoptMap(map);

            ASSERT(true  == D.isMap());
            // Empty maps never owns the keys.
            ASSERT(false == D.theMap().ownsKeys());

            const Int64 bytesInUse = oa.numBytesInUse();

            const Datum DC = D.clone(&ca);

            ASSERT(bytesInUse == ca.numBytesInUse());

            ASSERT(false == DC.isExternalReference());
            ASSERT(true  == DC.isMap());
            ASSERT(false == DC.theMap().ownsKeys());
            ASSERT(D.theMap() == DC.theMap());

            Datum::destroy(D, &oa);
            Datum::destroy(DC, &ca);

            ASSERT(0 == oa.status());
            ASSERT(0 == ca.status());
        }

        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            bslma::TestAllocator ca("clone",  veryVeryVeryVerbose);

            const SizeType  mapCapacity = 4;
            const char     *keys[] = { "one", "two", "three", "four" };
            SizeType        keyCapacity = 0;

            for (size_t i = 0; i < 4; ++i) {
                keyCapacity += strlen(keys[i]);
            }

            DatumMutableMapOwningKeysRef map;
            Datum::createUninitializedMap(&map,
                                          mapCapacity,
                                          keyCapacity,
                                          &oa);
            char *keyBuffer = map.keys();

            bsl::memcpy(keyBuffer, keys[0], strlen(keys[0]));
            StringRef key(keyBuffer, static_cast<int>(strlen(keys[0])));
            keyBuffer += strlen(keys[0]);
            map.data()[0] = DatumMapEntry(key, Datum::createInteger(0));

            bsl::memcpy(keyBuffer, keys[1], strlen(keys[1]));
            key = StringRef(keyBuffer, static_cast<int>(strlen(keys[1])));
            keyBuffer += strlen(keys[1]);
            map.data()[1] = DatumMapEntry(key, Datum::createDouble(-3.1));

            bsl::memcpy(keyBuffer, keys[2], strlen(keys[2]));
            key = StringRef(keyBuffer, static_cast<int>(strlen(keys[2])));
            keyBuffer += strlen(keys[2]);
            map.data()[2] = DatumMapEntry(
                                      key,
                                      Datum::createDate(bdlt::Date(2010,1,5)));

            bsl::memcpy(keyBuffer, keys[3], strlen(keys[3]));
            key = StringRef(keyBuffer, static_cast<int>(strlen(keys[3])));
            keyBuffer += strlen(keys[3]);
            map.data()[3] = DatumMapEntry(key, Datum::createNull());

            *(map.size()) = mapCapacity;
            *(map.sorted()) = false;

            const Datum D = Datum::adoptMap(map);

            ASSERT(true == D.isMap());
            ASSERT(true == D.theMap().ownsKeys());

            const Int64 bytesInUse = oa.numBytesInUse();

            const Datum DC = D.clone(&ca);

            ASSERT(bytesInUse == ca.numBytesInUse());

            ASSERT(false == DC.isExternalReference());
            ASSERT(true  == DC.isMap());
            ASSERT(true  == DC.theMap().ownsKeys());
            ASSERT(D.theMap() == DC.theMap());

            Datum::destroy(D, &oa);
            Datum::destroy(DC, &ca);

            ASSERT(0 == oa.status());
            ASSERT(0 == ca.status());
        }

        if (verbose) cout <<
                  "\nTesting 'clone' for an array having map element." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            bslma::TestAllocator ca("clone",  veryVeryVeryVerbose);

            StringRef key1("one");
            StringRef key2("two");
            StringRef key3("three");
            StringRef key4("four");

            const SizeType     mapSize = 4;
            DatumMutableMapRef map;
            Datum::createUninitializedMap(&map, mapSize, &oa);
            map.data()[0] = DatumMapEntry(key1, Datum::createInteger(0));
            map.data()[1] = DatumMapEntry(key2, Datum::createDouble(-3.141));
            map.data()[2] = DatumMapEntry(key3,
                                          Datum::copyString("A long string",
                                                            &oa));
            map.data()[3] = DatumMapEntry(key4,
                                          Datum::copyString("Abc", &oa));
            *(map.size()) = mapSize;
            *(map.sorted()) = false;

            const SizeType       arraySize  = 4;
            DatumMutableArrayRef array;
            Datum::createUninitializedArray(&array, arraySize, &oa);
            array.data()[0] = Datum::createInteger(0);
            array.data()[1] = Datum::copyString("A long string", &oa);
            array.data()[2] = Datum::createDate(bdlt::Date(2010, 1, 5));
            array.data()[3] = Datum::adoptMap(map);
            *(array.length()) = arraySize;

            const Datum D = Datum::adoptArray(array);

            ASSERT(true == D.isArray());
            ASSERT(0    != oa.numBytesInUse());

            const Datum DC = D.clone(&ca);

            ASSERT(0 != ca.numBytesInUse());

            ASSERT(false == DC.isExternalReference());
            ASSERT(true  == DC.isArray());
            ASSERT(D.theArray() == DC.theArray());

            Datum::destroy(D, &oa);
            Datum::destroy(DC, &ca);

            ASSERT(0 == oa.status());
            ASSERT(0 == ca.status());
        }

        if (verbose) cout <<
                   "\nTesting 'clone' for a map having array element." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            bslma::TestAllocator ca("clone",  veryVeryVeryVerbose);

            StringRef key1("one");
            StringRef key2("two");
            StringRef key3("three");
            StringRef key4("four");

            const SizeType       arraySize  = 4;
            DatumMutableArrayRef array;
            Datum::createUninitializedArray(&array, arraySize, &oa);
            array.data()[0] = Datum::createInteger(0);
            array.data()[1] = Datum::copyString("A long string", &oa);
            array.data()[2] = Datum::createDate(bdlt::Date(2010, 1, 5));
            array.data()[3] = Datum::createDouble(3.1415);
            *(array.length()) = arraySize;

            const SizeType     mapSize = 4;
            DatumMutableMapRef map;
            Datum::createUninitializedMap(&map, mapSize, &oa);
            map.data()[0] = DatumMapEntry(key1, Datum::createInteger(0));
            map.data()[1] = DatumMapEntry(key2, Datum::createDouble(-3.141));
            map.data()[2] = DatumMapEntry(key3,
                    Datum::copyString("A long string", &oa));
            map.data()[3] = DatumMapEntry(key4, Datum::adoptArray(array));
            *(map.size()) = mapSize;
            *(map.sorted()) = false;

            const Datum D = Datum::adoptMap(map);

            ASSERT(true == D.isMap());
            ASSERT(0    != oa.numBytesInUse());

            const Datum DC = D.clone(&ca);

            ASSERT(0 != ca.numBytesInUse());

            ASSERT(false == DC.isExternalReference());
            ASSERT(true  == DC.isMap());
            ASSERT(D.theMap() == DC.theMap());

            Datum::destroy(D, &oa);
            Datum::destroy(DC, &ca);

            ASSERT(0 == oa.status());
            ASSERT(0 == ca.status());
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            bslma::TestAllocator  oa("object", veryVeryVeryVerbose);
            bslma::Allocator     *nullAllocPtr =
                                            static_cast<bslma::Allocator *>(0);

            (void) nullAllocPtr;  // suppress compiler warning

            const Datum D = Datum::createNull();

            ASSERT_FAIL(D.clone(nullAllocPtr));
            ASSERT_PASS(D.clone(&oa));

            ASSERT(0 == oa.status());
         }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING 'apply' METHOD
        //
        // Concerns:
        //: 1 The 'apply' function should call the appropriate overload of
        //:   'operator()' on the specified 'visitor'.
        //
        // Plan:
        //: 1 Call 'apply' function with a visitor that just stores the type
        //:   of the object with which it is called into a member variable.
        //:   Call this on all the different types of 'Datum' objects.
        //:   Retrieve the integer value and verify that the appropriate
        //:   overload of 'operator()' was called on the visitor.
        //
        // Testing:
        //   template <class BDLD_VISITOR> void apply(BDLD_VISITOR&) const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'apply' METHOD" << endl
                          << "======================" << endl;

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        if (verbose) cout << "\nTesting 'apply' for Datums having "
                          << "non-aggregate values." << endl;
        {
            const static struct {
                int             d_line;   // line number
                Datum           d_value;  // Datum object
                Datum::DataType d_type;   // expected type reported by visitor
            } DATA[] = {
//LINE VALUE                                       TYPE
//---- ------------------------------------------  ----------------------------
{ L_,  Datum::createBoolean(true),                 Datum::e_BOOLEAN          },
{ L_,  Datum::createDate(Date()),                  Datum::e_DATE             },
{ L_,  Datum::createDatetime(Datetime(), &oa),     Datum::e_DATETIME         },
{ L_,  Datum::createDatetimeInterval(DatetimeInterval(1), &oa),
                                                   Datum::e_DATETIME_INTERVAL},
{ L_,  Datum::createDecimal64(BDLDFP_DECIMAL_DD(1.0), &oa),
                                                   Datum::e_DECIMAL64        },
{ L_,  Datum::createDecimal64(k_DECIMAL64_INFINITY, &oa),
                                                   Datum::e_DECIMAL64        },
{ L_,  Datum::createDecimal64(k_DECIMAL64_SNAN, &oa),
                                                   Datum::e_DECIMAL64        },
{ L_,  Datum::createDecimal64(k_DECIMAL64_QNAN, &oa),
                                                   Datum::e_DECIMAL64        },
{ L_,  Datum::createDouble(0.0),                   Datum::e_DOUBLE           },
{ L_,  Datum::createDouble(k_DOUBLE_INFINITY),     Datum::e_DOUBLE           },
{ L_,  Datum::createDouble(k_DOUBLE_SNAN),         Datum::e_DOUBLE           },
{ L_,  Datum::createError(5),                      Datum::e_ERROR            },
{ L_,  Datum::createError(5, "error", &oa),        Datum::e_ERROR            },
{ L_,  Datum::createInteger(18),                   Datum::e_INTEGER          },
{ L_,  Datum::createInteger64(98765432LL, &oa),    Datum::e_INTEGER64        },
{ L_,  Datum::createNull(),                        Datum::e_NIL              },
{ L_,  Datum::createStringRef("", 0, &oa),         Datum::e_STRING           },
{ L_,  Datum::createStringRef("a", 1, &oa),        Datum::e_STRING           },
{ L_,  Datum::createStringRef("abcde", 5, &oa),    Datum::e_STRING           },
{ L_,  Datum::createStringRef("abcdef", 6, &oa),   Datum::e_STRING           },
{ L_,  Datum::createTime(Time()),                  Datum::e_TIME             },
{ L_,  Datum::createUdt(reinterpret_cast<void *>(0x2), 15),
                                                   Datum::e_USERDEFINED      },
{ L_,  Datum::copyBinary("abcde", 5, &oa),         Datum::e_BINARY           },
{ L_,  Datum::copyString("abcde", &oa),            Datum::e_STRING           },
            };
            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i < DATA_LEN; ++i) {
                const int             LINE  = DATA[i].d_line;
                const Datum&          VALUE = DATA[i].d_value;
                const Datum::DataType TYPE  = DATA[i].d_type;

                if (veryVerbose) { T_ P_(LINE) P_(VALUE) P(TYPE) }

                TestVisitor visitor;
                VALUE.apply(visitor);

                ASSERT(visitor.objectVisited());
                ASSERT(TYPE == visitor.type());

                Datum::destroy(VALUE, &oa);
            }
            ASSERT(0 == oa.status());
        }

        if (verbose) cout << "\nTesting 'apply' for Datums having "
                          << "aggregate values." << endl;

        if (verbose) cout << "\nTesting 'apply' with Datum having array ref"
                          << " value."
                          << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            const Datum array[2] = { Datum::createInteger(1),
                                     Datum::createDouble(18) };
            const Datum D = Datum::createArrayReference(array, 2, &oa);

            TestVisitor visitor;
            D.apply(visitor);

            ASSERT(visitor.objectVisited());
            ASSERT(Datum::e_ARRAY == visitor.type());

            Datum::destroy(D, &oa);
        }
        if (verbose)
            cout << "\nTesting 'apply' with Datum having array value." << endl;

        if (verbose)
            cout << "\tTesting 'apply' with Datum having empty array value."
                 << endl;
        {
            DatumMutableArrayRef array;
            const Datum          D = Datum::adoptArray(array);

            TestVisitor visitor;
            D.apply(visitor);

            ASSERT(visitor.objectVisited());
            ASSERT(Datum::e_ARRAY == visitor.type());

            Datum::destroy(D, &oa);
        }

        if (verbose) cout <<
                   "\tTesting 'apply' with Datum having non-empty array value."
                          << endl;

        {
            DatumMutableArrayRef array;
            Datum::createUninitializedArray(&array, 1, &oa);
            array.data()[0] = Datum::createInteger(1);
            *(array.length()) = 1;
            const Datum D = Datum::adoptArray(array);

            TestVisitor visitor;
            D.apply(visitor);

            ASSERT(visitor.objectVisited());
            ASSERT(Datum::e_ARRAY == visitor.type());

            Datum::destroy(D, &oa);
        }

        if (verbose) cout <<
                      "\nTesting 'apply' with Datum having map value." << endl;

        if (verbose) cout <<
                "\tTesting 'apply' with Datum having empty map value." << endl;
        {
            DatumMutableMapRef map;
            Datum              D = Datum::adoptMap(map);

            TestVisitor visitor;
            D.apply(visitor);

            ASSERT(visitor.objectVisited());
            ASSERT(Datum::e_MAP == visitor.type());

            Datum::destroy(D, &oa);
            ASSERT( 0 == oa.status());
        }
        if (verbose) cout <<
                     "\tTesting 'apply' with Datum having non-empty map value."
                          << endl;
        {
            DatumMutableMapRef map;
            Datum::createUninitializedMap(&map, 1, &oa);
            map.data()[0] = DatumMapEntry(StringRef("key"),
                                          Datum::createInteger(1));
            *(map.size()) = 1;
            Datum D = Datum::adoptMap(map);

            TestVisitor visitor;
            D.apply(visitor);

            ASSERT(visitor.objectVisited());
            ASSERT(Datum::e_MAP == visitor.type());

            Datum::destroy(D, &oa);
            ASSERT( 0 == oa.status());
        }

        if (verbose) cout << "\nTesting 'apply' with Datum having map"
                          << " (owning keys) value."
                          << endl;

        if (verbose) cout << "\tTesting 'apply' with Datum having empty map"
                          << " (owning keys) value."
                          << endl;
        {
            DatumMutableMapOwningKeysRef map;
            Datum                        D = Datum::adoptMap(map);

            TestVisitor visitor;
            D.apply(visitor);

            ASSERT(visitor.objectVisited());
            ASSERT(Datum::e_MAP == visitor.type());

            Datum::destroy(D, &oa);
            ASSERT( 0 == oa.status());
        }

        if (verbose) cout << "\tTesting 'apply' with Datum having non-empty"
                          << " map (owning keys) value."
                          << endl;
        {
            DatumMutableMapOwningKeysRef map;
            Datum::createUninitializedMap(&map, 1, strlen("key"), &oa);
            char *keys = map.keys();
            memcpy(keys, "key", strlen("key"));
            map.data()[0] = DatumMapEntry(
                              StringRef(keys, static_cast<int>(strlen("key"))),
                              Datum::createInteger(1));
            *(map.size()) = 1;
            Datum D = Datum::adoptMap(map);

            TestVisitor visitor;
            D.apply(visitor);

            ASSERT(visitor.objectVisited());
            ASSERT(Datum::e_MAP == visitor.type());

            Datum::destroy(D, &oa);
            ASSERT( 0 == oa.status());
        }

        if (verbose) cout <<
            "\tTesting 'apply' with Datum having empty int-map value." << endl;
        {
            DatumMutableIntMapRef map;
            Datum                 D = Datum::adoptIntMap(map);

            TestVisitor visitor;
            D.apply(visitor);

            ASSERT(visitor.objectVisited());
            ASSERT(Datum::e_INT_MAP == visitor.type());

            Datum::destroy(D, &oa);
            ASSERT( 0 == oa.status());
        }
        if (verbose) cout <<
                 "\tTesting 'apply' with Datum having non-empty int-map value."
                 << endl;
        {
            DatumMutableIntMapRef map;
            Datum::createUninitializedIntMap(&map, 1, &oa);
            map.data()[0] = DatumIntMapEntry(1, Datum::createInteger(1));
            *(map.size()) = 1;
            Datum D = Datum::adoptIntMap(map);

            TestVisitor visitor;
            D.apply(visitor);

            ASSERT(visitor.objectVisited());
            ASSERT(Datum::e_INT_MAP == visitor.type());

            Datum::destroy(D, &oa);
            ASSERT( 0 == oa.status());
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING 'type' METHOD
        //
        // Concerns:
        //: 1 The 'type' method should return the correct data type held by the
        //    Datum object.
        //
        // Plan:
        //: 1 Call 'type' method on all the different types of 'Datum' objects
        //:   and verify that correct value is returned.  (C-1)
        //
        // Testing:
        //   DataType::Enum type() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'type' METHOD" << endl
                          << "=====================" << endl;

        if (verbose) cout << "\nTesting 'type' for Datums having "
                          << "non-aggregate values." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            const static struct {
                int             d_line;    // line number
                Datum           d_value;   // Datum object
                Datum::DataType d_type;    // expected type
            } DATA[] = {
//LINE VALUE                                      TYPE
//---- -----------------------------------------  --------------------------
{ L_,  Datum::createBoolean(true),                Datum::e_BOOLEAN           },
{ L_,  Datum::createDate(Date()),                 Datum::e_DATE              },
{ L_,  Datum::createDatetime(Datetime(), &oa),    Datum::e_DATETIME          },
{ L_,  Datum::createDatetime(Datetime(9999,  9,  9,  9,  9,  9, 999), &oa),
                                                  Datum::e_DATETIME          },
{ L_,  Datum::createDatetimeInterval(DatetimeInterval(1), &oa),
                                                  Datum::e_DATETIME_INTERVAL },
{ L_,  Datum::createDatetimeInterval(
           DatetimeInterval(100000000, 1, 1, 32, 587), &oa),
                                                  Datum::e_DATETIME_INTERVAL },
{ L_,  Datum::createDecimal64(BDLDFP_DECIMAL_DD(1.0), &oa),
                                                  Datum::e_DECIMAL64         },
{ L_,  Datum::createDecimal64(k_DECIMAL64_INFINITY, &oa),
                                                  Datum::e_DECIMAL64         },
{ L_,  Datum::createDecimal64(k_DECIMAL64_SNAN, &oa),
                                                  Datum::e_DECIMAL64         },
{ L_,  Datum::createDecimal64(k_DECIMAL64_QNAN, &oa),
                                                  Datum::e_DECIMAL64         },
{ L_,  Datum::createDouble(0.0),                  Datum::e_DOUBLE            },
{ L_,  Datum::createDouble(k_DOUBLE_INFINITY),    Datum::e_DOUBLE            },
{ L_,  Datum::createDouble(k_DOUBLE_SNAN),        Datum::e_DOUBLE            },
{ L_,  Datum::createError(5),                     Datum::e_ERROR             },
{ L_,  Datum::createError(5, "error", &oa),       Datum::e_ERROR             },
{ L_,  Datum::createInteger(18),                  Datum::e_INTEGER           },
{ L_,  Datum::createInteger64(98765432LL, &oa),   Datum::e_INTEGER64         },
{ L_,  Datum::createNull(),                       Datum::e_NIL               },
{ L_,  Datum::createStringRef("", 0, &oa),        Datum::e_STRING            },
{ L_,  Datum::createStringRef("a", 1, &oa),       Datum::e_STRING            },
{ L_,  Datum::createStringRef("abcde", 5, &oa),   Datum::e_STRING            },
{ L_,  Datum::createStringRef("abcdef", 6, &oa),  Datum::e_STRING            },
{ L_,  Datum::createStringRef("0abcdef", 7, &oa), Datum::e_STRING            },
{ L_,  Datum::createStringRef("0123456789abc", 13, &oa),
                                                  Datum::e_STRING            },
{ L_,  Datum::createStringRef("0123456789abcd", 14, &oa),
                                                  Datum::e_STRING            },
{ L_,  Datum::createTime(Time()),                 Datum::e_TIME              },
{ L_,  Datum::createUdt(reinterpret_cast<void *>(0x2), 15),
                                                  Datum::e_USERDEFINED       },
{ L_,  Datum::copyBinary("abcde", 5, &oa),        Datum::e_BINARY            },
{ L_,  Datum::copyBinary("0123456789abc", 13, &oa),
                                                  Datum::e_BINARY            },
{ L_,  Datum::copyBinary("0123456789abcd", 14, &oa),
                                                  Datum::e_BINARY            },
{ L_,  Datum::copyString("abcde", &oa),           Datum::e_STRING            },
            };

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i < DATA_LEN; ++i) {
                const int             LINE  = DATA[i].d_line;
                const Datum&          VALUE = DATA[i].d_value;
                const Datum::DataType TYPE  = DATA[i].d_type;

                if (veryVerbose) { T_ P_(LINE) P_(VALUE) P(TYPE) }

                ASSERT(TYPE == VALUE.type());

                Datum::destroy(VALUE, &oa);
            }
            ASSERT(0 == oa.status());
        }

        if (verbose) cout << "\nTesting 'type' for Datums having "
                          << "aggregate values." << endl;

        if (verbose) cout << "\nTesting 'type' with Datum having array "
                          << "reference value." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            const Datum array[2] = { Datum::createInteger(1),
                                     Datum::createDouble(18) };
            const Datum D = Datum::createArrayReference(array, 2, &oa);

            ASSERT(Datum::e_ARRAY == D.type());

            Datum::destroy(D, &oa);
        }
        if (verbose)
            cout << "\nTesting 'type' with Datum having array value." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            DatumMutableArrayRef array;
            Datum                D = Datum::adoptArray(array);

            ASSERT(Datum::e_ARRAY == D.type());

            Datum::destroy(D, &oa);
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            DatumMutableArrayRef array;
            Datum::createUninitializedArray(&array, 1, &oa);
            array.data()[0] = Datum::createInteger(1);
            *(array.length()) = 1;
            Datum D = Datum::adoptArray(array);

            ASSERT(Datum::e_ARRAY == D.type());

            Datum::destroy(D, &oa);
        }

        if (verbose)
            cout << "\nTesting 'type' with Datum having map value." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            DatumMutableMapRef map;
            Datum              D = Datum::adoptMap(map);

            ASSERT(Datum::e_MAP == D.type());

            Datum::destroy(D, &oa);
            ASSERT( 0 == oa.status());
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            DatumMutableMapRef map;
            Datum::createUninitializedMap(&map, 1, &oa);
            map.data()[0] = DatumMapEntry(StringRef("key"),
                                          Datum::createInteger(1));
            *(map.size()) = 1;
            Datum D = Datum::adoptMap(map);

            ASSERT(Datum::e_MAP == D.type());

            Datum::destroy(D, &oa);
            ASSERT( 0 == oa.status());
        }

        if (verbose) cout <<
                  "\nTesting 'type' with Datum having map (owning keys) value."
                         << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            DatumMutableMapOwningKeysRef map;
            Datum                        D = Datum::adoptMap(map);

            ASSERT(Datum::e_MAP == D.type());

            Datum::destroy(D, &oa);
            ASSERT( 0 == oa.status());
        }
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            DatumMutableMapOwningKeysRef map;
            Datum::createUninitializedMap(&map, 1, strlen("key"), &oa);
            char *keys = map.keys();
            memcpy(keys, "key", strlen("key"));
            map.data()[0] = DatumMapEntry(
                              StringRef(keys, static_cast<int>(strlen("key"))),
                              Datum::createInteger(1));
            *(map.size()) = 1;
            Datum D = Datum::adoptMap(map);

            ASSERT(Datum::e_MAP == D.type());

            Datum::destroy(D, &oa);
            ASSERT( 0 == oa.status());
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING 'isExternalReference' METHOD
        //
        // Concerns:
        //: 1 The 'isExternalReference' method return 'true' when the 'Datum'
        //    object holds a reference to an externally managed data.
        //
        // Plan:
        //: 1 Call 'isExternalReference' method on all the different types of
        //:   'Datum' objects and verify that correct value is returned.  (C-1)
        //
        // Testing:
        //   bool isExternalReference() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'isExternalReference' METHOD" << endl
                          << "====================================" << endl;

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        if (verbose) cout << "\nTesting 'isExternalReference' for Datums "
                          << "having non-aggregate values." << endl;
        {
            const static struct {
                int     d_line;          // line number
                Datum   d_value;         // Datum object
                bool    d_isExternalRef; // expected accessor result
            } DATA[] = {
      //LINE VALUE                                                     EXP
      //---- --------------------------------------------------------  -----
      { L_,  Datum::createBoolean(true),                               false },
      { L_,  Datum::createDate(Date()),                                false },
      { L_,  Datum::createDatetime(Datetime(), &oa),                   false },
      { L_,  Datum::createDatetimeInterval(DatetimeInterval(1), &oa),  false },
      { L_,  Datum::createDecimal64(BDLDFP_DECIMAL_DD(1.0), &oa),      false },
      { L_,  Datum::createDecimal64(k_DECIMAL64_INFINITY, &oa),        false },
      { L_,  Datum::createDecimal64(k_DECIMAL64_SNAN, &oa),            false },
      { L_,  Datum::createDecimal64(k_DECIMAL64_QNAN, &oa),            false },
      { L_,  Datum::createDouble(0.0),                                 false },
      { L_,  Datum::createDouble(k_DOUBLE_INFINITY),                   false },
      { L_,  Datum::createDouble(k_DOUBLE_SNAN),                       false },
      { L_,  Datum::createError(5),                                    false },
      { L_,  Datum::createError(5, "error", &oa),                      false },
      { L_,  Datum::createInteger(18),                                 false },
      { L_,  Datum::createInteger64(98765432LL, &oa),                  false },
      { L_,  Datum::createNull(),                                      false },
      { L_,  Datum::createStringRef("", 0, &oa),                       true  },
      { L_,  Datum::createStringRef("a", 1, &oa),                      true  },
      { L_,  Datum::createStringRef("abcde", 5, &oa),                  true  },
      { L_,  Datum::createStringRef("abcdef", 6, &oa),                 true  },
      { L_,  Datum::createStringRef("0abcdef", 7, &oa),                true  },
      { L_,  Datum::createStringRef("0123456789abc", 13, &oa),         true  },
      { L_,  Datum::createStringRef("0123456789abcd", 14, &oa),        true  },
      { L_,  Datum::createTime(Time()),                                false },
      { L_,  Datum::createUdt(reinterpret_cast<void *>(0x2), 15),      true  },
      { L_,  Datum::copyBinary("abcde", 5, &oa),                       false },
      { L_,  Datum::copyString("abcde", &oa),                          false },
            };

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i < DATA_LEN; ++i) {
                const int    LINE  = DATA[i].d_line;
                const Datum& VALUE = DATA[i].d_value;
                const bool   EXP   = DATA[i].d_isExternalRef;

                if (veryVerbose) { T_ P_(LINE) P_(VALUE) P(EXP) }

                ASSERT(EXP == VALUE.isExternalReference());

                Datum::destroy(VALUE, &oa);
            }
            ASSERT(0 == oa.status());
        }

        if (verbose) cout << "\nTesting 'isExternalReference' for Datums "
                          << "having aggregate values." << endl;

        if (verbose)
            cout << "\nTesting with Datum having array reference value."
                 << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            const Datum array[2] = { Datum::createInteger(1),
                                     Datum::createDouble(18) };
            const Datum D = Datum::createArrayReference(array, 2, &oa);

            ASSERT(true == D.isExternalReference());

            Datum::destroy(D, &oa);
        }

        if (verbose)
            cout << "\nTesting with Datum having array value." << endl;

        if (verbose)
            cout << "\tTesting with Datum having empty array value." << endl;
        {
            DatumMutableArrayRef array;
            const Datum          D = Datum::adoptArray(array);

            ASSERT(false == D.isExternalReference());

            Datum::destroy(D, &oa);
        }

        if (verbose)
            cout << "\tTesting with Datum having non-empty array value."
                 << endl;

        {
            DatumMutableArrayRef array;
            Datum::createUninitializedArray(&array, 1, &oa);
            array.data()[0] = Datum::createInteger(1);
            *(array.length()) = 1;
            const Datum D = Datum::adoptArray(array);

            ASSERT(false == D.isExternalReference());

            Datum::destroy(D, &oa);
        }

        if (verbose) cout << "\nTesting with Datum having map value." << endl;

        if (verbose)
            cout << "\tTesting with Datum having empty map value." << endl;
        {
            DatumMutableMapRef map;
            Datum              D = Datum::adoptMap(map);

            ASSERT(false == D.isExternalReference());

            Datum::destroy(D, &oa);
            ASSERT( 0 == oa.status());
        }
        if (verbose)
            cout << "\tTesting with Datum having non-empty map value." << endl;
        {
            DatumMutableMapRef map;
            Datum::createUninitializedMap(&map, 1, &oa);
            map.data()[0] = DatumMapEntry(StringRef("key"),
                                          Datum::createInteger(1));
            *(map.size()) = 1;
            Datum D = Datum::adoptMap(map);

            ASSERT(false == D.isExternalReference());

            Datum::destroy(D, &oa);
            ASSERT( 0 == oa.status());
        }

        if (verbose)
            cout << "\nTesting with Datum having map (owning keys) value."
                 << endl;

        if (verbose) cout << "\tTesting with Datum having empty map (owning "
                          << "keys) value." << endl;
        {
            DatumMutableMapOwningKeysRef map;
            Datum                        D = Datum::adoptMap(map);

            ASSERT(false == D.isExternalReference());

            Datum::destroy(D, &oa);
            ASSERT( 0 == oa.status());
        }

        if (verbose) cout << "\tTesting with Datum having non-empty map "
                          << "(owning keys) value." << endl;
        {
            DatumMutableMapOwningKeysRef map;
            Datum::createUninitializedMap(&map, 1, strlen("key"), &oa);
            char *keys = map.keys();
            memcpy(keys, "key", strlen("key"));
            map.data()[0] = DatumMapEntry(
                              StringRef(keys, static_cast<int>(strlen("key"))),
                              Datum::createInteger(1));
            *(map.size()) = 1;
            Datum D = Datum::adoptMap(map);

            ASSERT(false == D.isExternalReference());

            Datum::destroy(D, &oa);
            ASSERT( 0 == oa.status());
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING PRINT AND OPERATOR<<
        //   Ensure that the value of the 'Datum' holding an aggregate value
        //   can be formatted appropriately on an 'ostream' in some standard,
        //   human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.
        //:
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)'.
        //:
        //: 4 The 'print' method's signature and return type are standard.
        //:
        //: 5 The 'print' method returns the supplied 'ostream'.
        //:
        //: 6 The optional 'level' and 'spacesPerLevel' parameters have the
        //:   correct default values (0 and 4, respectively).
        //:
        //: 7 The output 'operator<<'s signature and return type are standard.
        //:
        //: 8 The output 'operator<<' returns the destination 'ostream'.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-4, 7)
        //:
        //: 2 Using the table-driven technique:  (C-1..3, 5..6, 8)
        //:
        //:   1 Define combinations of object values, having distinct values
        //      and various values for the two formatting parameters, along
        //      with the expected output.
        //:
        //:     ( 'value' x  'level'   x 'spacesPerLevel' ):
        //:     1 { A   } x {  0     } x {  0, 1, -1, -8 } --> 4 expected o/ps
        //:     2 { A   } x {  3, -3 } x {  0, 2, -2, -8 } --> 8 expected o/ps
        //:     3 { A   } x {  2     } x {  3            } --> 1 expected o/ps
        //:     4 { A, B} x { -8     } x { -8            } --> 2 expected o/ps
        //:     5 { A, B} x { -9     } x { -9            } --> 2 expected o/ps
        //:
        //:   2 For each row in the table defined in P-2.1:  (C-1..3, 5..6, 8)
        //:
        //:     1 Using a 'const' 'Datum', supply each object value and pair of
        //:       formatting parameters to 'print', omitting the 'level' or
        //:       'spacesPerLevel' parameter if the value of that argument is
        //:       '-8'.  If the parameters are, arbitrarily, '(-9, -9)', then
        //:       invoke the 'operator<<' instead.
        //:
        //:     2 Use a standard 'ostringstream' to capture the actual output.
        //:
        //:     3 Verify the address of what is returned is that of the
        //:       supplied stream.  (C-5, 8)
        //:
        //:     4 Compare the contents captured in P-2.2.2 with what is
        //:       expected.  (C-1..3, 6)
        //
        // Testing:
        //   bsl::ostream& print(ostream&, int, int) const; // aggregate
        //   bsl::ostream& operator<<(ostream&, const Datum&); // aggregate
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PRINT AND OPERATOR<<" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nCreating aggregate test values." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            DatumMutableArrayRef array;
            const Datum          DA1 = Datum::adoptArray(array);

            const SizeType arraySize = 3;
            Datum::createUninitializedArray(&array, arraySize, &oa);
            array.data()[0] = Datum::createInteger(0);
            array.data()[1] = Datum::copyString("A string", &oa);
            array.data()[2] = Datum::createDate(bdlt::Date(2010,1,5));
            *(array.length()) = arraySize;
            const Datum DA2 = Datum::adoptArray(array);

            DatumMutableMapRef map1;
            const Datum        DM1 = Datum::adoptMap(map1);

            const SizeType mapSize = 2;
            Datum::createUninitializedMap(&map1, mapSize, &oa);
            map1.data()[0] = DatumMapEntry(StringRef("key1"),
                                           Datum::createInteger(1));
            map1.data()[1] = DatumMapEntry(StringRef("key2"),
                                           Datum::createDouble(0.34));
            *(map1.size()) = mapSize;
            const Datum DM2 = Datum::adoptMap(map1);

            DatumMutableMapOwningKeysRef map2;
            const Datum                  DM3 = Datum::adoptMap(map2);

            const char *keys[mapSize] = { "key1", "key2" };
            SizeType    keysSize = 0;
            for (size_t i = 0; i < mapSize; ++i) {
                keysSize += strlen(keys[i]);
            }
            Datum::createUninitializedMap(&map2, mapSize, keysSize, &oa);
            char *keyBuffer = map2.keys();
            memcpy(keyBuffer, keys[0], strlen(keys[0]));
            map2.data()[0] = DatumMapEntry(
                       StringRef(keyBuffer, static_cast<int>(strlen(keys[0]))),
                       Datum::createInteger(2));
            keyBuffer += strlen(keys[0]);
            memcpy(keyBuffer, keys[1], strlen(keys[1]));
            map2.data()[1] = DatumMapEntry(
                       StringRef(keyBuffer, static_cast<int>(strlen(keys[1]))),
                       Datum::createDouble(8.2));
            *(map2.size()) = mapSize;
            const Datum DM4 = Datum::adoptMap(map2);


            if (verbose) cout <<
                      "\nCreate a table of distinct value/format combinations."
                              << endl;

            static const struct {
                int         d_line;            // source line number
                int         d_level;           // number of indentation levels
                int         d_spacesPerLevel;  // spaces per indentation level
                const Datum d_value;           // datum
                const char *d_expected_p;      // expected output or 0
            } DATA[] = {
//LINE LEVEL  SPACES VALUE  EXPECTED OUTPUT
//            PER
//            LEVEL
//---- -----  ------ -----  ---------------------------------------------------
{ L_,   0,     0,    DA1,   "[\n"
                            "]\n"                                            },
{ L_,   0,     1,    DA1,   "[\n"
                            "]\n"                                            },
{ L_,   0,    -1,    DA1,   "[ ]"                                            },
{ L_,   0,    -8,    DA1,   "[\n"
                            "]\n"                                            },
{ L_,   3,     0,    DA1,   "[\n"
                            "]\n"                                            },
{ L_,   3,     2,    DA1,   "      [\n"
                            "      ]\n"                                      },
{ L_,   3,    -2,    DA1,   "      [ ]"                                      },
{ L_,   3,    -8,    DA1,   "            [\n"
                            "            ]\n"                                },
{ L_,  -3,     0,    DA1,   "[\n"
                            "]\n"                                            },
{ L_,  -3,     2,    DA1,   "[\n"
                            "      ]\n"                                      },
{ L_,  -3,    -2,    DA1,   "[ ]"                                            },
{ L_,  -3,    -8,    DA1,   "[\n"
                            "            ]\n"                                },
{ L_,   2,     3,    DA1,   "      [\n"
                            "      ]\n"                                      },
{ L_,  -8,    -8,    DA1,   "[\n"
                            "]\n"                                            },
{ L_,  -9,    -9,    DA1,   "[ ]"                                            },
{ L_,   0,     0,    DA2,   "[\n"
                            "0\n"
                            "\"A string\"\n"
                            "05JAN2010\n"
                            "]\n"                                            },
{ L_,   0,     1,    DA2,   "[\n"
                            " 0\n"
                            " \"A string\"\n"
                            " 05JAN2010\n"
                            "]\n"                                            },
{ L_,   0,    -1,    DA2,   "[ 0 \"A string\" 05JAN2010 ]"                   },
{ L_,   0,    -8,    DA2,   "[\n"
                            "    0\n"
                            "    \"A string\"\n"
                            "    05JAN2010\n"
                            "]\n"                                            },
{ L_,   3,     0,    DA2,   "[\n"
                            "0\n"
                            "\"A string\"\n"
                            "05JAN2010\n"
                            "]\n"                                            },
{ L_,   3,     2,    DA2,   "      [\n"
                            "        0\n"
                            "        \"A string\"\n"
                            "        05JAN2010\n"
                            "      ]\n"                                      },
{ L_,   3,    -2,    DA2,   "      [ 0 \"A string\" 05JAN2010 ]"             },
{ L_,   3,    -8,    DA2,
                            "            [\n"
                            "                0\n"
                            "                \"A string\"\n"
                            "                05JAN2010\n"
                            "            ]\n"                                },
{ L_,  -3,     0,    DA2,   "[\n"
                            "0\n"
                            "\"A string\"\n"
                            "05JAN2010\n"
                            "]\n"                                            },
{ L_,  -3,     2,    DA2,   "[\n"
                            "        0\n"
                            "        \"A string\"\n"
                            "        05JAN2010\n"
                            "      ]\n"                                      },
{ L_,  -3,    -2,    DA2,   "[ 0 \"A string\" 05JAN2010 ]"                   },
{ L_,  -3,    -8,    DA2,   "[\n"
                            "                0\n"
                            "                \"A string\"\n"
                            "                05JAN2010\n"
                            "            ]\n"                                },
{ L_,  -8,    -8,    DA2,   "[\n"
                            "    0\n"
                            "    \"A string\"\n"
                            "    05JAN2010\n"
                            "]\n"                                            },
{ L_,  -9,    -9,    DA2,   "[ 0 \"A string\" 05JAN2010 ]"                   },
{ L_,   0,     0,    DM1,   "[\n"
                            "]\n"                                            },
{ L_,   0,     1,    DM1,   "[\n"
                            "]\n"                                            },
{ L_,   0,    -1,    DM1,   "[ ]"                                            },
{ L_,   0,    -8,    DM1,   "[\n"
                            "]\n"                                            },
{ L_,   3,     0,    DM1,   "[\n"
                            "]\n"                                            },
{ L_,   3,     2,    DM1,   "      [\n"
                            "      ]\n"                                      },
{ L_,   3,    -2,    DM1,   "      [ ]"                                      },
{ L_,   3,    -8,    DM1,   "            [\n"
                            "            ]\n"                                },
{ L_,  -3,     0,    DM1,   "[\n"
                            "]\n"                                            },
{ L_,  -3,     2,    DM1,   "[\n"
                            "      ]\n"                                      },
{ L_,  -3,    -2,    DM1,   "[ ]"                                            },
{ L_,  -3,    -8,    DM1,   "[\n"
                            "            ]\n"                                },
{ L_,   2,     3,    DM1,   "      [\n"
                            "      ]\n"                                      },
{ L_,  -8,    -8,    DM1,   "[\n"
                            "]\n"                                            },
{ L_,  -9,    -9,    DM1,   "[ ]"                                            },
{ L_,   0,     0,    DM2,   "[\n"
                            "[\n"
                            "key1 = 1\n"
                            "]\n"
                            "[\n"
                            "key2 = 0.34\n"
                            "]\n"
                            "]\n"                                           },
{ L_,   0,     1,    DM2,   "[\n"
                            " [\n"
                            "  key1 = 1\n"
                            " ]\n"
                            " [\n"
                            "  key2 = 0.34\n"
                            " ]\n"
                            "]\n"                                            },
{ L_,   0,    -1,    DM2,   "[ [ key1 = 1 ] [ key2 = 0.34 ] ]"               },
{ L_,   0,    -8,    DM2,   "[\n"
                            "    [\n"
                            "        key1 = 1\n"
                            "    ]\n"
                            "    [\n"
                            "        key2 = 0.34\n"
                            "    ]\n"
                            "]\n"                                           },
{ L_,   3,     0,    DM2,   "[\n"
                            "[\n"
                            "key1 = 1\n"
                            "]\n"
                            "[\n"
                            "key2 = 0.34\n"
                            "]\n"
                            "]\n"                                            },
{ L_,   3,     2,    DM2,   "      [\n"
                            "        [\n"
                            "          key1 = 1\n"
                            "        ]\n"
                            "        [\n"
                            "          key2 = 0.34\n"
                            "        ]\n"
                            "      ]\n"                                      },
{ L_,   3,    -2,    DM2,   "      [ [ key1 = 1 ] [ key2 = 0.34 ] ]"         },
{ L_,   3,    -8,    DM2,   "            [\n"
                            "                [\n"
                            "                    key1 = 1\n"
                            "                ]\n"
                            "                [\n"
                            "                    key2 = 0.34\n"
                            "                ]\n"
                            "            ]\n"                                },
{ L_,  -3,     0,    DM2,   "[\n"
                            "[\n"
                            "key1 = 1\n"
                            "]\n"
                            "[\n"
                            "key2 = 0.34\n"
                            "]\n"
                            "]\n"                                            },
{ L_,  -3,     2,    DM2,   "[\n"
                            "        [\n"
                            "          key1 = 1\n"
                            "        ]\n"
                            "        [\n"
                            "          key2 = 0.34\n"
                            "        ]\n"
                            "      ]\n"                                      },
{ L_,  -3,    -2,    DM2,   "[ [ key1 = 1 ] [ key2 = 0.34 ] ]"               },
{ L_,  -3,    -8,    DM2,   "[\n"
                            "                [\n"
                            "                    key1 = 1\n"
                            "                ]\n"
                            "                [\n"
                            "                    key2 = 0.34\n"
                            "                ]\n"
                            "            ]\n"                                },
{ L_,   2,     3,    DM2,   "      [\n"
                            "         [\n"
                            "            key1 = 1\n"
                            "         ]\n"
                            "         [\n"
                            "            key2 = 0.34\n"
                            "         ]\n"
                            "      ]\n"                                      },
{ L_,  -8,    -8,    DM2,   "[\n"
                            "    [\n"
                            "        key1 = 1\n"
                            "    ]\n"
                            "    [\n"
                            "        key2 = 0.34\n"
                            "    ]\n"
                            "]\n"                                            },
{ L_,  -9,    -9,    DM2,   "[ [ key1 = 1 ] [ key2 = 0.34 ] ]"               },
{ L_,   0,     0,    DM3,   "[\n"
                            "]\n"                                            },
{ L_,   0,     1,    DM3,   "[\n"
                            "]\n"                                            },
{ L_,   0,    -1,    DM3,   "[ ]"                                            },
{ L_,   0,    -8,    DM3,   "[\n"
                            "]\n"                                            },
{ L_,   3,     0,    DM3,   "[\n"
                            "]\n"                                            },
{ L_,   3,     2,    DM3,   "      [\n"
                            "      ]\n"                                      },
{ L_,   3,    -2,    DM3,   "      [ ]"                                      },
{ L_,   3,    -8,    DM3,   "            [\n"
                            "            ]\n"                                },
{ L_,  -3,     0,    DM3,   "[\n"
                            "]\n"                                            },
{ L_,  -3,     2,    DM3,   "[\n"
                            "      ]\n"                                      },
{ L_,  -3,    -2,    DM3,   "[ ]"                                            },
{ L_,  -3,    -8,    DM3,   "[\n"
                            "            ]\n"                                },
{ L_,   2,     3,    DM3,   "      [\n"
                            "      ]\n"                                      },
{ L_,  -8,    -8,    DM3,   "[\n"
                            "]\n"                                            },
{ L_,  -9,    -9,    DM3,   "[ ]"                                            },
{ L_,   0,     0,    DM4,   "[\n"
                            "[\n"
                            "key1 = 2\n"
                            "]\n"
                            "[\n"
                            "key2 = 8.2\n"
                            "]\n"
                            "]\n"                                           },
{ L_,   0,     1,    DM4,   "[\n"
                            " [\n"
                            "  key1 = 2\n"
                            " ]\n"
                            " [\n"
                            "  key2 = 8.2\n"
                            " ]\n"
                            "]\n"                                            },
{ L_,   0,    -1,    DM4,   "[ [ key1 = 2 ] [ key2 = 8.2 ] ]"               },
{ L_,   0,    -8,    DM4,   "[\n"
                            "    [\n"
                            "        key1 = 2\n"
                            "    ]\n"
                            "    [\n"
                            "        key2 = 8.2\n"
                            "    ]\n"
                            "]\n"                                           },
{ L_,   3,     0,    DM4,   "[\n"
                            "[\n"
                            "key1 = 2\n"
                            "]\n"
                            "[\n"
                            "key2 = 8.2\n"
                            "]\n"
                            "]\n"                                            },
{ L_,   3,     2,    DM4,   "      [\n"
                            "        [\n"
                            "          key1 = 2\n"
                            "        ]\n"
                            "        [\n"
                            "          key2 = 8.2\n"
                            "        ]\n"
                            "      ]\n"                                      },
{ L_,   3,    -2,    DM4,   "      [ [ key1 = 2 ] [ key2 = 8.2 ] ]"         },
{ L_,   3,    -8,    DM4,   "            [\n"
                            "                [\n"
                            "                    key1 = 2\n"
                            "                ]\n"
                            "                [\n"
                            "                    key2 = 8.2\n"
                            "                ]\n"
                            "            ]\n"                                },
{ L_,  -3,     0,    DM4,   "[\n"
                            "[\n"
                            "key1 = 2\n"
                            "]\n"
                            "[\n"
                            "key2 = 8.2\n"
                            "]\n"
                            "]\n"                                            },
{ L_,  -3,     2,    DM4,   "[\n"
                            "        [\n"
                            "          key1 = 2\n"
                            "        ]\n"
                            "        [\n"
                            "          key2 = 8.2\n"
                            "        ]\n"
                            "      ]\n"                                      },
{ L_,  -3,    -2,    DM4,   "[ [ key1 = 2 ] [ key2 = 8.2 ] ]"               },
{ L_,  -3,    -8,    DM4,   "[\n"
                            "                [\n"
                            "                    key1 = 2\n"
                            "                ]\n"
                            "                [\n"
                            "                    key2 = 8.2\n"
                            "                ]\n"
                            "            ]\n"                                },
{ L_,   2,     3,    DM4,   "      [\n"
                            "         [\n"
                            "            key1 = 2\n"
                            "         ]\n"
                            "         [\n"
                            "            key2 = 8.2\n"
                            "         ]\n"
                            "      ]\n"                                      },
{ L_,  -8,    -8,    DM4,   "[\n"
                            "    [\n"
                            "        key1 = 2\n"
                            "    ]\n"
                            "    [\n"
                            "        key2 = 8.2\n"
                            "    ]\n"
                            "]\n"                                            },
{ L_,  -9,    -9,    DM4,   "[ [ key1 = 2 ] [ key2 = 8.2 ] ]"                },
            };

            const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

            if (verbose)
                cout << "\nTesting with various print specifications." << endl;
            {
                for (size_t i = 0; i < NUM_DATA; ++i) {
                    const int         LINE  = DATA[i].d_line;
                    const int         L     = DATA[i].d_level;
                    const int         SPL   = DATA[i].d_spacesPerLevel;
                    const Datum&      VALUE = DATA[i].d_value;
                    const char *const EXP   = DATA[i].d_expected_p;

                    if (veryVerbose) {
                        T_ P_(LINE) P_(L) P(SPL)
                    }

                    if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                    bslma::TestAllocator sa("stream", veryVeryVeryVerbose);

                    ostringstream os(&sa);

                    // Verify supplied stream is returned by reference.

                    if (-9 == L && -9 == SPL) {
                        ASSERTV(LINE, &os == &(os << VALUE));

                        if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                    }
                    else {
                        ASSERTV(LINE, -8 == SPL || -8 != L);

                        if (-8 != SPL) {
                            ASSERTV(LINE, &os == &VALUE.print(os, L, SPL));
                        }
                        else if (-8 != L) {
                            ASSERTV(LINE, &os == &VALUE.print(os, L));
                        }
                        else {
                            ASSERTV(LINE, &os == &VALUE.print(os));
                        }
                        if (veryVeryVerbose) { T_ T_ Q(print) }
                    }

                    {
                        bslma::TestAllocator         da("default",
                                                        veryVeryVeryVerbose);
                        bslma::DefaultAllocatorGuard dag(&da);

                        // Verify output is formatted as expected.

                        if (veryVeryVerbose) { cout << os.str(); }

                        if (EXP) {
                            ASSERTV(LINE, EXP, os.str(), EXP == os.str());
                        } else {
                            // The value has a platform-dependent
                            // representation.  For those values we test
                            // operator<< only;
                            ASSERTV(LINE, (-9 == L) && (-9 == SPL));
                            ostringstream expected;

                            if (VALUE.isDouble()) {
                                expected << VALUE.theDouble();
                                ASSERTV(LINE,
                                        expected.str(),
                                        os.str(),
                                        expected.str() == os.str());
                            } else if (VALUE.isDecimal64()) {
                                expected << VALUE.theDecimal64();
                                ASSERTV(LINE,
                                        expected.str(),
                                        os.str(),
                                        expected.str() == os.str());
                            } else {
                                ASSERTV(LINE, 0);
                            }
                        }
                    }
                }
            }
            Datum::destroy(DA1, &oa);
            Datum::destroy(DA2, &oa);
            Datum::destroy(DM1, &oa);
            Datum::destroy(DM2, &oa);
            Datum::destroy(DM3, &oa);
            Datum::destroy(DM4, &oa);
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING STRING CONSTRUCTION
        //    Verify that user can construct 'Datum' holding uninitialized
        //    string and fill the string buffer pointed by the created 'Datum'
        //    object later.
        //
        // Concerns:
        //: 1 Can create 'Datum' object holding an uninitialized string buffer.
        //:
        //: 2 All object memory comes from the supplied allocator.
        //:
        //: 3 All allocated memory is released when the 'Datum' object is
        //:   destroyed.
        //
        // Plan:
        //: 1 Create 'Datum' object holding uninitialized string buffer.
        //:   Verify the type and the value of the created 'Datum' object.
        //:   (C-1..3)
        //:
        //: 2 Verify that all object memory is released when the object is
        //:   destroyed.  (C-4)
        //
        // Testing:
        //   char *createUninitializedString(Datum&, SizeType, Allocator *);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING STRING CONSTRUCTION" << endl
                          << "===========================" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);

        if (verbose)
            cout << "\nTesting 'createUninitializedString()'." << endl;
        {
            const char BUFFER[] = "0123456789abcdef";

            for (size_t i = 0; i < sizeof(BUFFER); ++i) {
                const StringRef REF(BUFFER, static_cast<int>(i));

                if (veryVerbose) { T_ P_(i) P(REF) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Datum         mD;
                const Datum&  D = mD;
                char         *buffer = Datum::createUninitializedString(&mD,
                                                                        i,
                                                                        &oa);
                bsl::memcpy(buffer, BUFFER, i);

                ASSERT(0 == da.numBlocksInUse());
#ifdef BSLS_PLATFORM_CPU_32_BIT
                // Up to length 6 inclusive the strings are stored inline
                if (i <= 6) {
#else  // BSLS_PLATFORM_CPU_32_BIT
                // Up to length 13 inclusive the strings are stored inline
                if (i <= 13) {
#endif // BSLS_PLATFORM_CPU_32_BIT
                    ASSERT(0 == oa.numBlocksInUse());
                } else {
                    ASSERT(0 != oa.numBlocksInUse());
                }

                ASSERT(!D.isArray());
                ASSERT(!D.isBoolean());
                ASSERT(!D.isBinary());
                ASSERT(!D.isDate());
                ASSERT(!D.isDatetime());
                ASSERT(!D.isDatetimeInterval());
                ASSERT(!D.isDecimal64());
                ASSERT(!D.isDouble());
                ASSERT(!D.isError());
                ASSERT(!D.isInteger());
                ASSERT(!D.isInteger64());
                ASSERT(!D.isIntMap());
                ASSERT(!D.isMap());
                ASSERT(!D.isNull());
                ASSERT(D.isString());               // *
                ASSERT(!D.isTime());
                ASSERT(!D.isUdt());
                ASSERT(!D.isExternalReference());
                ASSERT(REF == D.theString());

                Datum::destroy(D, &oa);

                ASSERT(0 == da.status());
                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bslma::TestAllocator         oa("object", veryVeryVeryVerbose);
            bsls::AssertTestHandlerGuard hG;

            bslma::Allocator *nullAllocPtr =
                                            static_cast<bslma::Allocator *>(0);

            (void) nullAllocPtr;  // suppress compiler warning

            Datum  mD;
            Datum *nullDatumPtr = static_cast<Datum *>(0);

            (void) nullDatumPtr;  // suppress compiler warning

            ASSERT_FAIL(Datum::createUninitializedString(nullDatumPtr,
                                                         0,
                                                         &oa));
            ASSERT_FAIL(Datum::createUninitializedString(&mD,
                                                         0,
                                                         nullAllocPtr));
            ASSERT_PASS(Datum::createUninitializedString(&mD, 0, &oa));

            Datum::destroy(mD, &oa);
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING MAP CONSTRUCTION
        //
        // Concerns:
        //: 1 'Datum' object holding a map of 'Datum's can be created.
        //:
        //: 2 Destroying adopted map releases all memory allocated by Datums in
        //:   that map.
        //
        //: 3 All object memory comes from the supplied allocator.
        //:
        //: 4 All allocated memory is released when the 'Datum' object is
        //:   destroyed.
        //:
        //: 5 'theMap' accessor is the constant method.
        //:
        //: 6 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create 'Datum' object holding a map.  Verify the type and the
        //:   value of the created 'Datum' object.  (C-1..3)
        //:
        //: 2 Verify that all object memory is released when the object is
        //:   destroyed.  (C-4)
        //:
        //: 3 Invoke 'theMap' via a reference providing non-modifiable
        //:   access to the object.  Compilation of this test driver confirms
        //:   that accessor is 'const'-qualified.  (C-5)
        //
        // Testing:
        //   void createUninitializedMap(DatumMutableMapRef*, SizeType, ...);
        //   Datum adoptMap(const DatumMutableMapRef& map);
        //   void createUninitializedMap(DatumMutableMapOwningKeysRef *, ...);
        //   Datum adoptMap(const DatumMutableMapOwningKeysRef& map);
        //   bool isMap() const;
        //   DatumMapRef theMap() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING MAP CONSTRUCTION" << endl
                          << "========================" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);

        if (verbose)
            cout << "\nTesting creation of a map with external keys." << endl;
        {
            if (veryVerbose) cout << "\tCreating an empty map." << endl;
            {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                DatumMutableMapRef map;
                Datum              mD = Datum::adoptMap(map);
                const Datum&       D = mD;

                ASSERT(!D.isArray());
                ASSERT(!D.isBoolean());
                ASSERT(!D.isBinary());
                ASSERT(!D.isDate());
                ASSERT(!D.isDatetime());
                ASSERT(!D.isDatetimeInterval());
                ASSERT(!D.isDecimal64());
                ASSERT(!D.isDouble());
                ASSERT(!D.isError());
                ASSERT(!D.isInteger());
                ASSERT(!D.isInteger64());
                ASSERT(!D.isIntMap());
                ASSERT(D.isMap());              // *
                ASSERT(!D.isNull());
                ASSERT(!D.isString());
                ASSERT(!D.isTime());
                ASSERT(!D.isUdt());
                ASSERT(!D.isExternalReference());

                ASSERT(false == D.theMap().isSorted());

                DatumMapEntry     *mp = 0;
                const DatumMapRef  mapRef(mp, 0, false, false);
                ASSERT(mapRef == D.theMap());

                Datum::destroy(D, &oa);
                ASSERT(0 == oa.status());
            }

            if (veryVerbose)
                cout << "\tCreating Datum holding an unsorted map." << endl;
            {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                const SizeType     capacity = 4;
                DatumMutableMapRef map;
                Datum::createUninitializedMap(&map, capacity, &oa);

                StringRef key1("one");
                StringRef key2("two");
                StringRef key3("three");
                StringRef key4("four");

                map.data()[0] = DatumMapEntry(key1, Datum::createInteger(0));
                map.data()[1] = DatumMapEntry(key2, Datum::createDouble(-3.1));
                map.data()[2] = DatumMapEntry(key3,
                                      Datum::createDate(bdlt::Date(2010,1,5)));
                map.data()[3] = DatumMapEntry(key4, Datum::createNull());

                *(map.size()) = capacity;
                *(map.sorted()) = false;
                Datum        mD = Datum::adoptMap(map);
                const Datum& D = mD;

                ASSERT(!D.isArray());
                ASSERT(!D.isBoolean());
                ASSERT(!D.isBinary());
                ASSERT(!D.isDate());
                ASSERT(!D.isDatetime());
                ASSERT(!D.isDatetimeInterval());
                ASSERT(!D.isDecimal64());
                ASSERT(!D.isDouble());
                ASSERT(!D.isError());
                ASSERT(!D.isInteger());
                ASSERT(!D.isInteger64());
                ASSERT(!D.isIntMap());
                ASSERT(D.isMap());              // *
                ASSERT(!D.isNull());
                ASSERT(!D.isString());
                ASSERT(!D.isTime());
                ASSERT(!D.isUdt());
                ASSERT(!D.isExternalReference());

                ASSERT(false == D.theMap().isSorted());

                const DatumMapRef mapRef(map.data(), capacity, false, false);
                ASSERT(mapRef == D.theMap());

                Datum::destroy(D, &oa);
                ASSERT(0 == oa.status());
            }

            if (veryVerbose)
                cout << "\tCreating Datum holding a sorted map." << endl;
            {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                const SizeType     capacity = 4;
                DatumMutableMapRef map;
                Datum::createUninitializedMap(&map, capacity, &oa);

                StringRef key1("four");
                StringRef key2("one");
                StringRef key3("three");
                StringRef key4("two");

                map.data()[0] = DatumMapEntry(key1, Datum::createInteger(0));
                map.data()[1] = DatumMapEntry(key2, Datum::createDouble(-3.1));
                map.data()[2] = DatumMapEntry(
                                      key3,
                                      Datum::createDate(bdlt::Date(2010,1,5)));
                map.data()[3] = DatumMapEntry(key4, Datum::createNull());

                *(map.size()) = capacity;
                *(map.sorted()) = true;

                Datum        mD = Datum::adoptMap(map);
                const Datum& D = mD;

                ASSERT(!D.isArray());
                ASSERT(!D.isBoolean());
                ASSERT(!D.isBinary());
                ASSERT(!D.isDate());
                ASSERT(!D.isDatetime());
                ASSERT(!D.isDatetimeInterval());
                ASSERT(!D.isDecimal64());
                ASSERT(!D.isDouble());
                ASSERT(!D.isError());
                ASSERT(!D.isInteger());
                ASSERT(!D.isInteger64());
                ASSERT(!D.isIntMap());
                ASSERT(D.isMap());              // *
                ASSERT(!D.isNull());
                ASSERT(!D.isString());
                ASSERT(!D.isTime());
                ASSERT(!D.isUdt());
                ASSERT(!D.isExternalReference());

                ASSERT(true == D.theMap().isSorted());

                const DatumMapRef mapRef(map.data(), capacity, true, false);
                ASSERT(mapRef == D.theMap());

                Datum::destroy(D, &oa);
                ASSERT(0 == oa.status());
            }

            if (veryVerbose)
                cout << "\tInitial capacity more than length." << endl;
            {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                const SizeType     capacity = 6;
                DatumMutableMapRef map;
                Datum::createUninitializedMap(&map, capacity+16, &oa);

                StringRef key1("one");
                StringRef key2("two");
                StringRef key3("three");
                StringRef key4("four");
                StringRef key5("five");
                StringRef key6("six");

                map.data()[0] = DatumMapEntry(key1, Datum::createInteger(0));
                map.data()[1] = DatumMapEntry(key2,
                                              Datum::createDouble(-3.1416));
                map.data()[2] = DatumMapEntry(
                                      key3,
                                      Datum::copyString("A long string", &oa));
                map.data()[3] = DatumMapEntry(key4,
                                              Datum::copyString("Abc", &oa));
                map.data()[4] = DatumMapEntry(
                                      key5,
                                      Datum::createDate(bdlt::Date(2010,1,5)));
                map.data()[5] = DatumMapEntry(
                     key6,
                     Datum::createDatetime(
                                         bdlt::Datetime(2010,1,5, 16,45,32,12),
                                         &oa));
                *(map.size()) = capacity;
                *(map.sorted()) = false;

                Datum        mD = Datum::adoptMap(map);
                const Datum& D = mD;

                ASSERT(!D.isArray());
                ASSERT(!D.isBoolean());
                ASSERT(!D.isBinary());
                ASSERT(!D.isDate());
                ASSERT(!D.isDatetime());
                ASSERT(!D.isDatetimeInterval());
                ASSERT(!D.isDecimal64());
                ASSERT(!D.isDouble());
                ASSERT(!D.isError());
                ASSERT(!D.isInteger());
                ASSERT(!D.isInteger64());
                ASSERT(!D.isIntMap());
                ASSERT(D.isMap());              // *
                ASSERT(!D.isNull());
                ASSERT(!D.isString());
                ASSERT(!D.isTime());
                ASSERT(!D.isUdt());
                ASSERT(!D.isExternalReference());

                ASSERT(false == D.theMap().isSorted());

                const DatumMapRef mapRef(map.data(), capacity, false, false);
                ASSERT(mapRef == D.theMap());

                Datum::destroy(D, &oa);
                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nTesting creation of a map owning keys'."
                          << endl;
        {
            if (veryVerbose) cout << "\tCreating an empty map." << endl;
            {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                DatumMutableMapOwningKeysRef map;
                Datum                        mD = Datum::adoptMap(map);
                const Datum&                 D = mD;

                ASSERT(!D.isArray());
                ASSERT(!D.isBoolean());
                ASSERT(!D.isBinary());
                ASSERT(!D.isDate());
                ASSERT(!D.isDatetime());
                ASSERT(!D.isDatetimeInterval());
                ASSERT(!D.isDecimal64());
                ASSERT(!D.isDouble());
                ASSERT(!D.isError());
                ASSERT(!D.isInteger());
                ASSERT(!D.isInteger64());
                ASSERT(!D.isIntMap());
                ASSERT(D.isMap());              // *
                ASSERT(!D.isNull());
                ASSERT(!D.isString());
                ASSERT(!D.isTime());
                ASSERT(!D.isUdt());
                ASSERT(!D.isExternalReference());

                ASSERT(false == D.theMap().isSorted());

                DatumMapEntry     *mp = 0;
                const DatumMapRef  mapRef(mp, 0, false, true);
                ASSERT(mapRef == D.theMap());

                Datum::destroy(D, &oa);
                ASSERT(0 == oa.status());
            }

            if (veryVerbose)
                cout << "\tCreating Datum holding an unsorted map." << endl;
            {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                const SizeType  mapCapacity = 4;
                const char     *keys[] = { "one", "two", "three", "four" };
                SizeType        keyCapacity = 0;

                for (size_t i = 0; i < 4; ++i) {
                    keyCapacity += strlen(keys[i]);
                }

                DatumMutableMapOwningKeysRef map;
                Datum::createUninitializedMap(&map,
                                              mapCapacity,
                                              keyCapacity,
                                              &oa);
                char *keyBuffer = map.keys();

                bsl::memcpy(keyBuffer, keys[0], strlen(keys[0]));
                StringRef key(keyBuffer, static_cast<int>(strlen(keys[0])));
                keyBuffer += strlen(keys[0]);
                map.data()[0] = DatumMapEntry(key, Datum::createInteger(0));

                bsl::memcpy(keyBuffer, keys[1], strlen(keys[1]));
                key = StringRef(keyBuffer, static_cast<int>(strlen(keys[1])));
                keyBuffer += strlen(keys[1]);
                map.data()[1] = DatumMapEntry(key, Datum::createDouble(-3.1));

                bsl::memcpy(keyBuffer, keys[2], strlen(keys[2]));
                key = StringRef(keyBuffer, static_cast<int>(strlen(keys[2])));
                keyBuffer += strlen(keys[2]);
                map.data()[2] = DatumMapEntry(key,
                                      Datum::createDate(bdlt::Date(2010,1,5)));

                bsl::memcpy(keyBuffer, keys[3], strlen(keys[3]));
                key = StringRef(keyBuffer, static_cast<int>(strlen(keys[3])));
                keyBuffer += strlen(keys[3]);
                map.data()[3] = DatumMapEntry(key, Datum::createNull());

                *(map.size()) = mapCapacity;
                *(map.sorted()) = false;

                Datum        mD = Datum::adoptMap(map);
                const Datum& D = mD;

                ASSERT(!D.isArray());
                ASSERT(!D.isBoolean());
                ASSERT(!D.isBinary());
                ASSERT(!D.isDate());
                ASSERT(!D.isDatetime());
                ASSERT(!D.isDatetimeInterval());
                ASSERT(!D.isDecimal64());
                ASSERT(!D.isDouble());
                ASSERT(!D.isError());
                ASSERT(!D.isInteger());
                ASSERT(!D.isInteger64());
                ASSERT(!D.isIntMap());
                ASSERT(D.isMap());              // *
                ASSERT(!D.isNull());
                ASSERT(!D.isString());
                ASSERT(!D.isTime());
                ASSERT(!D.isUdt());
                ASSERT(!D.isExternalReference());

                ASSERT(false == D.theMap().isSorted());

                const DatumMapRef mapRef(map.data(), mapCapacity, false, true);
                ASSERT(mapRef == D.theMap());

                Datum::destroy(D, &oa);
                ASSERT(0 == oa.status());
            }

            if (veryVerbose)
                cout << "\tCreating Datum holding sorted map." << endl;
            {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                const SizeType  mapCapacity = 4;
                const char     *keys[] = { "four", "six", "three", "wind" };
                SizeType        keyCapacity = 0;

                for (size_t i = 0; i < 4; ++i) {
                    keyCapacity += strlen(keys[i]);
                }

                DatumMutableMapOwningKeysRef map;
                Datum::createUninitializedMap(&map,
                                              mapCapacity,
                                              keyCapacity,
                                              &oa);
                char *keyBuffer = map.keys();

                bsl::memcpy(keyBuffer, keys[0], strlen(keys[0]));
                StringRef key(keyBuffer, static_cast<int>(strlen(keys[0])));
                keyBuffer += strlen(keys[0]);
                map.data()[0] = DatumMapEntry(key, Datum::createInteger(0));

                bsl::memcpy(keyBuffer, keys[1], strlen(keys[1]));
                key = StringRef(keyBuffer, static_cast<int>(strlen(keys[1])));
                keyBuffer += strlen(keys[1]);
                map.data()[1] = DatumMapEntry(key, Datum::createDouble(-3.1));

                bsl::memcpy(keyBuffer, keys[2], strlen(keys[2]));
                key = StringRef(keyBuffer, static_cast<int>(strlen(keys[2])));
                keyBuffer += strlen(keys[2]);
                map.data()[2] = DatumMapEntry(key,
                                      Datum::createDate(bdlt::Date(2010,1,5)));

                bsl::memcpy(keyBuffer, keys[3], strlen(keys[3]));
                key = StringRef(keyBuffer, static_cast<int>(strlen(keys[3])));
                keyBuffer += strlen(keys[3]);
                map.data()[3] = DatumMapEntry(key, Datum::createNull());

                *(map.size()) = mapCapacity;
                *(map.sorted()) = true;

                Datum        mD = Datum::adoptMap(map);
                const Datum& D = mD;

                ASSERT(!D.isArray());
                ASSERT(!D.isBoolean());
                ASSERT(!D.isBinary());
                ASSERT(!D.isDate());
                ASSERT(!D.isDatetime());
                ASSERT(!D.isDatetimeInterval());
                ASSERT(!D.isDecimal64());
                ASSERT(!D.isDouble());
                ASSERT(!D.isError());
                ASSERT(!D.isInteger());
                ASSERT(!D.isInteger64());
                ASSERT(!D.isIntMap());
                ASSERT(D.isMap());              // *
                ASSERT(!D.isNull());
                ASSERT(!D.isString());
                ASSERT(!D.isTime());
                ASSERT(!D.isUdt());
                ASSERT(!D.isExternalReference());

                ASSERT(true == D.theMap().isSorted());

                const DatumMapRef mapRef(map.data(), mapCapacity, true, true);
                ASSERT(mapRef == D.theMap());

                Datum::destroy(D, &oa);
                ASSERT(0 == oa.status());
            }

            if (veryVerbose)
                cout << "\tInitial capacity more than length." << endl;
            {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                const SizeType  mapCapacity = 4;
                const char     *keys[] = { "one", "two", "three", "four" };
                SizeType        keyCapacity = 0;

                for (size_t i = 0; i < 4; ++i) {
                    keyCapacity += strlen(keys[i]);
                }

                DatumMutableMapOwningKeysRef map;
                Datum::createUninitializedMap(&map,
                                              mapCapacity+16,
                                              keyCapacity+16,
                                              &oa);
                char *keyBuffer = map.keys();

                bsl::memcpy(keyBuffer, keys[0], strlen(keys[0]));
                StringRef key(keyBuffer, static_cast<int>(strlen(keys[0])));
                keyBuffer += strlen(keys[0]);
                map.data()[0] = DatumMapEntry(key, Datum::createInteger(0));

                bsl::memcpy(keyBuffer, keys[1], strlen(keys[1]));
                key = StringRef(keyBuffer, static_cast<int>(strlen(keys[1])));
                keyBuffer += strlen(keys[1]);
                map.data()[1] = DatumMapEntry(key, Datum::createDouble(-3.1));

                bsl::memcpy(keyBuffer, keys[2], strlen(keys[2]));
                key = StringRef(keyBuffer, static_cast<int>(strlen(keys[2])));
                keyBuffer += strlen(keys[2]);
                map.data()[2] = DatumMapEntry(key,
                                      Datum::createDate(bdlt::Date(2010,1,5)));

                bsl::memcpy(keyBuffer, keys[3], strlen(keys[3]));
                key = StringRef(keyBuffer, static_cast<int>(strlen(keys[3])));
                keyBuffer += strlen(keys[3]);
                map.data()[3] = DatumMapEntry(key, Datum::createNull());

                *(map.size()) = mapCapacity;
                *(map.sorted()) = false;

                Datum        mD = Datum::adoptMap(map);
                const Datum& D = mD;

                ASSERT(!D.isArray());
                ASSERT(!D.isBoolean());
                ASSERT(!D.isBinary());
                ASSERT(!D.isDate());
                ASSERT(!D.isDatetime());
                ASSERT(!D.isDatetimeInterval());
                ASSERT(!D.isDecimal64());
                ASSERT(!D.isDouble());
                ASSERT(!D.isError());
                ASSERT(!D.isInteger());
                ASSERT(!D.isInteger64());
                ASSERT(!D.isIntMap());
                ASSERT(D.isMap());              // *
                ASSERT(!D.isNull());
                ASSERT(!D.isString());
                ASSERT(!D.isTime());
                ASSERT(!D.isUdt());
                ASSERT(!D.isExternalReference());

                ASSERT(false == D.theMap().isSorted());

                const DatumMapRef mapRef(map.data(), mapCapacity, false, true);
                ASSERT(mapRef == D.theMap());

                Datum::destroy(D, &oa);
                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bslma::TestAllocator         oa("object", veryVeryVeryVerbose);
            bsls::AssertTestHandlerGuard hG;

            bslma::Allocator *nullAllocPtr =
                                            static_cast<bslma::Allocator *>(0);

            (void) nullAllocPtr;  // suppress compiler warning

            if (verbose) cout << "\tTesting 'createUninitializedMap'"
                              << " for map with external keys"
                              << endl;
            {
                DatumMutableMapRef  map;
                DatumMutableMapRef *nullRefPtr =
                                          static_cast<DatumMutableMapRef *>(0);

                (void) nullRefPtr;  // suppress compiler warning

                ASSERT_FAIL(Datum::createUninitializedMap(nullRefPtr,
                                                          0,
                                                          &oa));
                ASSERT_FAIL(Datum::createUninitializedMap(&map,
                                                          0,
                                                          nullAllocPtr));
                ASSERT_PASS(Datum::createUninitializedMap(&map,
                                                          0,
                                                          &oa));
                Datum mD = Datum::adoptMap(map);
                Datum::destroy(mD, &oa);
            }

            if (verbose) cout << "\tTesting 'createUninitializedMap'"
                              << " for map owning keys"
                              << endl;
            {
                DatumMutableMapOwningKeysRef  map;
                DatumMutableMapOwningKeysRef *nullRefPtr =
                                static_cast<DatumMutableMapOwningKeysRef *>(0);

                (void) nullRefPtr;  // suppress compiler warning

                ASSERT_FAIL(Datum::createUninitializedMap(nullRefPtr,
                                                          0,
                                                          0,
                                                          &oa));
                ASSERT_FAIL(Datum::createUninitializedMap(&map,
                                                          0,
                                                          0,
                                                          nullAllocPtr));
                ASSERT_PASS(Datum::createUninitializedMap(&map,
                                                          0,
                                                          0,
                                                          &oa));
                Datum mD = Datum::adoptMap(map);
                Datum::destroy(mD, &oa);
            }

            if (verbose) cout << "\tTesting 'theMap'" << endl;
            {
                DatumMutableMapRef map;

                const Datum fakeMap = Datum::createNull();
                const Datum realMap = Datum::adoptMap(map);

                ASSERT_SAFE_FAIL(fakeMap.theMap());
                ASSERT_SAFE_PASS(realMap.theMap());

                Datum::destroy(fakeMap, &oa);
                Datum::destroy(realMap, &oa);
            }
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING INTEGER MAP CONSTRUCTION
        //
        // Concerns:
        //: 1 'Datum' object holding a integer map of 'Datum's can be created.
        //:
        //: 2 Destroying adopted map releases all memory allocated by Datums in
        //:   that map.
        //
        //: 3 All object memory comes from the supplied allocator.
        //:
        //: 4 All allocated memory is released when the 'Datum' object is
        //:   destroyed.
        //:
        //: 5 'theIntMap' accessor is a constant method.
        //:
        //: 6 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create 'Datum' object holding an integer map.  Verify the type
        //:   and the value of the created 'Datum' object.  (C-1..3)
        //:
        //: 2 Verify that all object memory is released when the object is
        //:   destroyed.  (C-4)
        //:
        //: 3 Invoke 'theIntMap' via a reference providing non-modifiable
        //:   access to the object.  Compilation of this test driver confirms
        //:   that accessor is 'const'-qualified.  (C-5)
        //
        // Testing:
        //   void createUninitializedIntMap(DatumMutableIntMapRef*, size, ...);
        //   Datum adoptIntMap(const DatumMutableIntMapRef& map);
        //   bool isIntMap() const;
        //   DatumMapRef theIntMap() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING INTEGER MAP CONSTRUCTION" << endl
                          << "================================" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);

        if (verbose)
            cout << "\nTesting creation of an integer map with external keys."
                 << endl;
        {
            if (veryVerbose) cout << "\tCreating an empty map." << endl;
            {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                DatumMutableIntMapRef map;
                Datum                 mD = Datum::adoptIntMap(map);
                const Datum&          D = mD;

                ASSERT(!D.isArray());
                ASSERT(!D.isBoolean());
                ASSERT(!D.isBinary());
                ASSERT(!D.isDate());
                ASSERT(!D.isDatetime());
                ASSERT(!D.isDatetimeInterval());
                ASSERT(!D.isDecimal64());
                ASSERT(!D.isDouble());
                ASSERT(!D.isError());
                ASSERT(!D.isInteger());
                ASSERT(!D.isInteger64());
                ASSERT(D.isIntMap());              // *
                ASSERT(!D.isMap());
                ASSERT(!D.isNull());
                ASSERT(!D.isString());
                ASSERT(!D.isTime());
                ASSERT(!D.isUdt());
                ASSERT(!D.isExternalReference());

                ASSERT(false == D.theIntMap().isSorted());

                DatumIntMapEntry     *mp = 0;
                const DatumIntMapRef  mapRef(mp, 0, false);
                ASSERT(mapRef == D.theIntMap());

                Datum::destroy(D, &oa);
                ASSERT(0 == oa.status());
            }

            if (veryVerbose)
                cout << "\tCreating Datum holding an unsorted map." << endl;
            {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                const SizeType        capacity = 4;
                DatumMutableIntMapRef map;
                Datum::createUninitializedIntMap(&map, capacity, &oa);

                map.data()[0] = DatumIntMapEntry(1, Datum::createInteger(0));
                map.data()[1] = DatumIntMapEntry(2, Datum::createDouble(-3.1));
                map.data()[2] = DatumIntMapEntry(3,
                                      Datum::createDate(bdlt::Date(2010,1,5)));
                map.data()[3] = DatumIntMapEntry(4, Datum::createNull());

                *(map.size()) = capacity;
                *(map.sorted()) = false;
                Datum        mD = Datum::adoptIntMap(map);
                const Datum& D = mD;

                ASSERT(!D.isArray());
                ASSERT(!D.isBoolean());
                ASSERT(!D.isBinary());
                ASSERT(!D.isDate());
                ASSERT(!D.isDatetime());
                ASSERT(!D.isDatetimeInterval());
                ASSERT(!D.isDecimal64());
                ASSERT(!D.isDouble());
                ASSERT(!D.isError());
                ASSERT(!D.isInteger());
                ASSERT(!D.isInteger64());
                ASSERT(D.isIntMap());              // *
                ASSERT(!D.isMap());
                ASSERT(!D.isNull());
                ASSERT(!D.isString());
                ASSERT(!D.isTime());
                ASSERT(!D.isUdt());
                ASSERT(!D.isExternalReference());

                ASSERT(false == D.theIntMap().isSorted());

                const DatumIntMapRef mapRef(map.data(), capacity, false);
                ASSERT(mapRef == D.theIntMap());

                Datum::destroy(D, &oa);
                ASSERT(0 == oa.status());
            }

            if (veryVerbose)
                cout << "\tCreating Datum holding a sorted map." << endl;
            {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                const SizeType        capacity = 4;
                DatumMutableIntMapRef map;
                Datum::createUninitializedIntMap(&map, capacity, &oa);

                map.data()[0] = DatumIntMapEntry(1, Datum::createInteger(0));
                map.data()[1] = DatumIntMapEntry(2, Datum::createDouble(-3.1));
                map.data()[2] = DatumIntMapEntry(3,
                                      Datum::createDate(bdlt::Date(2010,1,5)));
                map.data()[3] = DatumIntMapEntry(4, Datum::createNull());

                *(map.size()) = capacity;
                *(map.sorted()) = true;

                Datum        mD = Datum::adoptIntMap(map);
                const Datum& D = mD;

                ASSERT(!D.isArray());
                ASSERT(!D.isBoolean());
                ASSERT(!D.isBinary());
                ASSERT(!D.isDate());
                ASSERT(!D.isDatetime());
                ASSERT(!D.isDatetimeInterval());
                ASSERT(!D.isDecimal64());
                ASSERT(!D.isDouble());
                ASSERT(!D.isError());
                ASSERT(!D.isInteger());
                ASSERT(!D.isInteger64());
                ASSERT(D.isIntMap());              // *
                ASSERT(!D.isMap());
                ASSERT(!D.isNull());
                ASSERT(!D.isString());
                ASSERT(!D.isTime());
                ASSERT(!D.isUdt());
                ASSERT(!D.isExternalReference());

                ASSERT(true == D.theIntMap().isSorted());

                const DatumIntMapRef mapRef(map.data(), capacity, true);
                ASSERT(mapRef == D.theIntMap());

                Datum::destroy(D, &oa);
                ASSERT(0 == oa.status());
            }

            if (veryVerbose)
                cout << "\tInitial capacity more than length." << endl;
            {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                const SizeType        capacity = 6;
                DatumMutableIntMapRef map;
                Datum::createUninitializedIntMap(&map, capacity+16, &oa);

                map.data()[0] = DatumIntMapEntry(1, Datum::createInteger(0));
                map.data()[1] = DatumIntMapEntry(2,
                                                 Datum::createDouble(-3.1416));
                map.data()[2] = DatumIntMapEntry(3,
                                      Datum::copyString("A long string", &oa));
                map.data()[3] = DatumIntMapEntry(4,
                                                Datum::copyString("Abc", &oa));
                map.data()[4] = DatumIntMapEntry(5,
                                      Datum::createDate(bdlt::Date(2010,1,5)));
                map.data()[5] = DatumIntMapEntry(6,
                                      Datum::createDatetime(
                                         bdlt::Datetime(2010,1,5, 16,45,32,12),
                                         &oa));
                *(map.size()) = capacity;
                *(map.sorted()) = false;

                Datum        mD = Datum::adoptIntMap(map);
                const Datum& D = mD;

                ASSERT(!D.isArray());
                ASSERT(!D.isBoolean());
                ASSERT(!D.isBinary());
                ASSERT(!D.isDate());
                ASSERT(!D.isDatetime());
                ASSERT(!D.isDatetimeInterval());
                ASSERT(!D.isDecimal64());
                ASSERT(!D.isDouble());
                ASSERT(!D.isError());
                ASSERT(!D.isInteger());
                ASSERT(!D.isInteger64());
                ASSERT(D.isIntMap());              // *
                ASSERT(!D.isMap());
                ASSERT(!D.isNull());
                ASSERT(!D.isString());
                ASSERT(!D.isTime());
                ASSERT(!D.isUdt());
                ASSERT(!D.isExternalReference());

                ASSERT(false == D.theIntMap().isSorted());

                const DatumIntMapRef mapRef(map.data(), capacity, false);
                ASSERT(mapRef == D.theIntMap());

                Datum::destroy(D, &oa);
                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bslma::TestAllocator         oa("object", veryVeryVeryVerbose);
            bsls::AssertTestHandlerGuard hG;

            bslma::Allocator *nullAllocPtr =
                                            static_cast<bslma::Allocator *>(0);

            (void) nullAllocPtr;  // suppress compiler warning

            if (verbose) cout << "\tTesting 'createUninitializedIntMap'"
                              << " for map with external keys"
                              << endl;
            {
                DatumMutableIntMapRef  map;
                DatumMutableIntMapRef *nullRefPtr =
                                       static_cast<DatumMutableIntMapRef *>(0);

                (void) nullRefPtr;  // suppress compiler warning

                ASSERT_FAIL(Datum::createUninitializedIntMap(nullRefPtr,
                                                             0,
                                                             &oa));
                ASSERT_FAIL(Datum::createUninitializedIntMap(&map,
                                                             0,
                                                             nullAllocPtr));
                ASSERT_PASS(Datum::createUninitializedIntMap(&map,
                                                             0,
                                                             &oa));
                Datum mD = Datum::adoptIntMap(map);
                Datum::destroy(mD, &oa);
            }

            if (verbose) cout << "\tTesting 'theIntMap'" << endl;
            {
                DatumMutableIntMapRef map;

                const Datum fakeMap = Datum::createNull();
                const Datum realMap = Datum::adoptIntMap(map);

                ASSERT_SAFE_FAIL(fakeMap.theIntMap());
                ASSERT_SAFE_PASS(realMap.theIntMap());

                Datum::destroy(fakeMap, &oa);
                Datum::destroy(realMap, &oa);
            }
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING ARRAY CONSTRUCTION
        //
        // Concerns:
        //: 1 Can create 'Datum' object holding an array of 'Datum's.
        //:
        //: 2 Destroying adopted array releases all memory allocated by Datums
        //:   in that array.
        //
        //: 3 All object memory comes from the supplied allocator.
        //:
        //: 4 All allocated memory is released when the 'Datum' object is
        //:   destroyed.
        //:
        //: 5 All ACCESSORS methods are declared 'const'.
        //:
        //: 6 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create 'Datum' object holding an array.  Verify the type and the
        //:   value of the created 'Datum' object.  (C-1..3)
        //:
        //: 2 Verify that all object memory is released when the object is
        //:   destroyed.  (C-4)
        //:
        //: 3 Invoke each ACCESSOR via a reference providing non-modifiable
        //:   access to the object.  Compilation of this test driver confirms
        //:   that ACCESSORS are 'const'-qualified.  (C-5)
        //
        // Testing:
        //   Datum createArrayReference(const Datum *, SizeType, Allocator *);
        //   Datum createArrayReference(const DatumArrayRef&, Allocator *);
        //   void createUninitializedArray(DatumMutableArrayRef*,SizeType,...);
        //   Datum adoptArray(const DatumMutableArrayRef&);
        //   bool isArray() const;
        //   DatumArrayRef theArray() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING ARRAY CONSTRUCTION" << endl
                          << "==========================" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);
        bslma::TestAllocator         oa("object", veryVeryVeryVerbose);

        if (verbose) cout << "\nTesting creation of array reference." << endl;
        {
            // 32-bit implementation uses externalized storage at 2^16.

            const size_t DATA[] = { 0, 1, 30, 65535, 65536, 100000 };
            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            if (verbose) cout << "\tTaking array and size as a parameters."
                                  << endl;
            {
               for (size_t i = 0; i < DATA_LEN; ++i) {
                    Datum       dummyDatum;
                    const Datum D = Datum::createArrayReference(&dummyDatum,
                                                                DATA[i],
                                                                &oa);
                    ASSERTV(i, D.isArray());
                    ASSERTV(i, !D.isBoolean());
                    ASSERTV(i, !D.isBinary());
                    ASSERTV(i, !D.isDate());
                    ASSERTV(i, !D.isDatetime());
                    ASSERTV(i, !D.isDatetimeInterval());
                    ASSERTV(i, !D.isDecimal64());
                    ASSERTV(i, !D.isDouble());
                    ASSERTV(i, !D.isError());
                    ASSERTV(i, !D.isInteger());
                    ASSERTV(i, !D.isInteger64());
                    ASSERTV(i, !D.isIntMap());
                    ASSERTV(i, !D.isMap());
                    ASSERTV(i, !D.isNull());
                    ASSERTV(i, !D.isString());
                    ASSERTV(i, !D.isTime());
                    ASSERTV(i, !D.isUdt());
                    ASSERT(D.isExternalReference());

                    const DatumArrayRef arrayRef = D.theArray();
                    ASSERTV(i, &dummyDatum == arrayRef.data());
                    ASSERTV(i, DATA[i] == arrayRef.length());
                    Datum::destroy(D, &oa);

                    ASSERT(0 == oa.status());
                }
            }

            if (verbose) cout << "\tTaking 'DatumArrayRef' as a parameter."
                                  << endl;
            {
                // Testing overload with const DatumArrayRef&
                for (size_t i = 0; i < DATA_LEN; ++i) {
                    Datum       dummyDatum;
                    const Datum D = Datum::createArrayReference(&dummyDatum,
                                                                DATA[i],
                                                                &oa);
                    ASSERTV(i, D.isArray());

                    const DatumArrayRef arrayRef = D.theArray();

                    Datum       mDC = Datum::createArrayReference(arrayRef,
                                                                  &oa);
                    const Datum DC = mDC;

                    ASSERTV(i, DC.isArray());
                    ASSERTV(i, !DC.isBoolean());
                    ASSERTV(i, !DC.isBinary());
                    ASSERTV(i, !DC.isDate());
                    ASSERTV(i, !DC.isDatetime());
                    ASSERTV(i, !DC.isDatetimeInterval());
                    ASSERTV(i, !DC.isDecimal64());
                    ASSERTV(i, !DC.isDouble());
                    ASSERTV(i, !DC.isError());
                    ASSERTV(i, !DC.isInteger());
                    ASSERTV(i, !DC.isInteger64());
                    ASSERTV(i, !DC.isIntMap());
                    ASSERTV(i, !DC.isMap());
                    ASSERTV(i, !DC.isNull());
                    ASSERTV(i, !DC.isString());
                    ASSERTV(i, !DC.isTime());
                    ASSERTV(i, !DC.isUdt());
                    ASSERT(D.isExternalReference());

                    const DatumArrayRef arrayRefC = DC.theArray();
                    ASSERTV(i, &dummyDatum == arrayRefC.data());
                    ASSERTV(i, DATA[i] == arrayRefC.length());

                    // Do not compare array refs here - they must point to the
                    // valid Datum array.

                    Datum::destroy(D, &oa);
                    Datum::destroy(DC, &oa);

                    ASSERT(0 == oa.status());
                }
            }
        }

        if (verbose) cout << "\nTesting 'createUninitializedArray()' and "
                          << "'adoptArray()'." << endl;

        {
            if (veryVerbose) cout << "\tCreating empty array." << endl;
            {
                DatumMutableArrayRef array;

                const Datum D = Datum::adoptArray(array);

                ASSERT(D.isArray());
                ASSERT(!D.isBoolean());
                ASSERT(!D.isBinary());
                ASSERT(!D.isDate());
                ASSERT(!D.isDatetime());
                ASSERT(!D.isDatetimeInterval());
                ASSERT(!D.isDecimal64());
                ASSERT(!D.isDouble());
                ASSERT(!D.isError());
                ASSERT(!D.isInteger());
                ASSERT(!D.isInteger64());
                ASSERT(!D.isIntMap());
                ASSERT(!D.isMap());
                ASSERT(!D.isNull());
                ASSERT(!D.isString());
                ASSERT(!D.isTime());
                ASSERT(!D.isUdt());
                ASSERT(!D.isExternalReference());

                Datum               dummyDatum;
                const DatumArrayRef arrayRef(&dummyDatum, 0);

                ASSERT(arrayRef == D.theArray());

                Datum::destroy(D, &oa);
                ASSERT(0 == oa.status());
            }

            if (veryVerbose)
                cout << "\tInitial capacity more than length." << endl;
            {
                const SizeType       capacity = 10;
                DatumMutableArrayRef array;
                Datum::createUninitializedArray(&array, capacity, &oa);

                array.data()[0] = Datum::createInteger(0);
                array.data()[1] = Datum::createDouble(-3.1416);
                array.data()[2] = Datum::copyString("A long string", &oa);
                array.data()[3] = Datum::copyString("Abc", &oa);
                array.data()[4] = Datum::createDate(Date(2010,1,5));
                array.data()[5] = Datum::createError(2, "error", &oa);

                *(array.length()) = 6;
                Datum        mD = Datum::adoptArray(array);
                const Datum& D = mD;

                ASSERT(D.isArray());
                ASSERT(!D.isExternalReference());
                const DatumArrayRef arrayRef(array.data(), 6);
                ASSERT(arrayRef == D.theArray());

                // Adopt takes ownership of the Datum's memory, so all Datums
                // are destroyed correctly here.
                Datum::destroy(D, &oa);
                ASSERT(0 == oa.status());
            }

            if (veryVerbose)
                cout << "\tCreating array holding another array" << endl;
            {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                const SizeType       capacity1 = 3;
                DatumMutableArrayRef array1;
                Datum::createUninitializedArray(&array1, capacity1, &oa);

                array1.data()[0] = Datum::copyString("Once upon a time", &oa);
                array1.data()[1] = Datum::copyBinary("binary data", 11, &oa);
                array1.data()[2] = Datum::createError(2, "error", &oa);

                *(array1.length()) = 3;

                Datum        mD1 = Datum::adoptArray(array1);
                const Datum& D1 = mD1;

                ASSERT(D1.isArray());
                ASSERT(!D1.isExternalReference());
                const DatumArrayRef arrayRef1(array1.data(), 3);
                ASSERT(arrayRef1 == D1.theArray());

                const SizeType       capacity2 = 16; // Yes, 16
                DatumMutableArrayRef array2;
                Datum::createUninitializedArray(&array2, capacity2, &oa);

                array2.data()[0] = Datum::copyString("There was a hero", &oa);
                array2.data()[1] = D1;

                *(array2.length()) = 2;

                Datum        mD2 = Datum::adoptArray(array2);
                const Datum& D2 = mD2;

                ASSERT(D2.isArray());
                ASSERT(!D2.isExternalReference());
                DatumArrayRef arrayRef2(array2.data(), 2);
                ASSERT(arrayRef2 == D2.theArray());

                // Adopt takes ownership of the Datum's memory, so all Datums
                // are destroyed correctly here.
                Datum::destroy(D2, &oa);
                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bslma::TestAllocator         oa("object", veryVeryVeryVerbose);
            bsls::AssertTestHandlerGuard hG;

            bslma::Allocator *nullAllocPtr =
                                            static_cast<bslma::Allocator *>(0);

            const Datum  D            = Datum::createNull();
            const Datum *nullDatumPtr = static_cast<Datum *>(0);

            (void) nullAllocPtr;  // suppress compiler warning
            (void) nullDatumPtr;  // suppress compiler warning

            if (verbose) cout << "\tTesting 'createArrayReference"
                              << "(const Datum *, SizeType, Allocator *)'."
                              << endl;
            {

                ASSERT_SAFE_PASS(Datum::createArrayReference(nullDatumPtr,
                                                             0,
                                                             &oa));
                ASSERT_SAFE_FAIL(Datum::createArrayReference(nullDatumPtr,
                                                             1,
                                                             &oa));
                ASSERT_SAFE_PASS(Datum::createArrayReference(&D, 0, &oa));
                ASSERT_SAFE_PASS(Datum::createArrayReference(&D, 1, &oa));
                ASSERT_SAFE_FAIL(Datum::createArrayReference(&D,
                                                             1,
                                                             nullAllocPtr));
            }

            if (verbose) cout << "\tTesting 'createArrayReference"
                              << "(const DatumArrayRef&, Allocator *)'."
                              << endl;
            {
                const DatumArrayRef arrayRef(&D, 0);

                ASSERT_SAFE_FAIL(Datum::createArrayReference(arrayRef,
                                                             nullAllocPtr));
                ASSERT_SAFE_PASS(Datum::createArrayReference(arrayRef, &oa));
            }

            if (verbose) cout << "\tTesting 'createUninitializedArray'"
                              << endl;
            {
                DatumMutableArrayRef  array;
                DatumMutableArrayRef *nullRefPtr =
                                        static_cast<DatumMutableArrayRef *>(0);

                (void) nullRefPtr;  // suppress compiler warning

                ASSERT_FAIL(Datum::createUninitializedArray(nullRefPtr,
                                                            0,
                                                            &oa));
                ASSERT_FAIL(Datum::createUninitializedArray(&array,
                                                            0,
                                                            nullAllocPtr));
                ASSERT_PASS(Datum::createUninitializedArray(&array,
                                                            0,
                                                            &oa));
                Datum mD = Datum::adoptArray(array);
                Datum::destroy(mD, &oa);
            }

            if (verbose) cout << "\tTesting 'theArray'" << endl;
            {
                DatumMutableArrayRef array;

                const Datum fakeArray = Datum::createNull();
                const Datum realArray = Datum::adoptArray(array);

                ASSERT_SAFE_FAIL(fakeArray.theArray());
                ASSERT_SAFE_PASS(realArray.theArray());

                Datum::destroy(fakeArray, &oa);
                Datum::destroy(realArray, &oa);
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'DatumMapRef'
        //
        // Concerns:
        //: 1 Exercise a broad cross-section of value-semantic functionality.
        //
        // Plan:
        //: 1 Create a 'DatumMapRef' object and verify that values
        //:   were correctly passed down to the 'd_data_p' and 'd_size' data
        //:   members. Also exercise the copy construction functionality and
        //:   verify using the equality operators that these objects have the
        //:   same value. Verify that streaming operator outputs the correctly
        //:   formatted value. Also verify that accessors return the correct
        //:   value.  Verify that 'find' returns the expected result for both
        //:   sorted and unsorted maps.
        //
        // Testing:
        //   DatumMapRef(const DatumMapEntry *, SizeType, bool, bool);
        //   const DatumMapEntry& operator[](SizeType index) const;
        //   const DatumMapEntry *data() const;
        //   bool isSorted() const;
        //   bool ownsKeys() const;
        //   SizeType size() const;
        //   const Datum *find(const bslstl::StringRef& key) const;
        //   bsl::ostream& print(bsl::ostream&, int,int) const;
        //   bool operator==(const DatumMapRef& lhs, const DatumMapRef& rhs);
        //   bool operator!=(const DatumMapRef& lhs, const DatumMapRef& rhs);
        //   bsl::ostream& operator<<(bsl::ostream&, const DatumMapRef&);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'DatumMapRef'" << endl
                          << "=====================" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);

        const StringRef KEY1("key1");
        const StringRef KEY2("key2");
        const StringRef KEY3("key3");
        const StringRef KEY4("key4");

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        const SizeType      size = 3;
        const DatumMapEntry map[size] = {
            DatumMapEntry(KEY1, Datum::createInteger(3)),
            DatumMapEntry(KEY2, Datum::createDouble(3.75)),
            DatumMapEntry(KEY3, Datum::copyString("A String", &oa))
        };

        // remember how many bytes are allocated by Datums
        const Int64 bytesInUse = oa.numBytesInUse();

        if (verbose) cout << "\nTesting value constructor." << endl;
        {
            const DatumMapRef obj(map, size, false, false);

            ASSERT(bytesInUse == oa.numBytesInUse());
            ASSERT(size       == obj.size());
            ASSERT(false      == obj.isSorted());
            ASSERT(false      == obj.ownsKeys());
            for (size_t i = 0; i < obj.size(); ++i) {
                if (veryVerbose) { T_ P_(i) P_(map[i]) P(obj[i]) }

                ASSERTV(i, map[i] == obj.data()[i]);
                ASSERTV(i, map[i] == obj[i]);
            }
        }
        {
            const DatumMapRef obj(map, size, true, false);
            ASSERT(bytesInUse == oa.numBytesInUse());
            ASSERT(size       == obj.size());
            ASSERT(true       == obj.isSorted());
            ASSERT(false      == obj.ownsKeys());
            for (size_t i = 0; i < obj.size(); ++i) {
                if (veryVerbose) { T_ P_(i) P_(map[i]) P(obj[i]) }

                ASSERTV(i, map[i] == obj.data()[i]);
                ASSERTV(i, map[i] == obj[i]);
            }
        }
        {
            // Note that ownKeys is set to 'true' only to test 'ownsKey'
            // accessor.
            const DatumMapRef obj(map, size, true, true);
            ASSERT(bytesInUse == oa.numBytesInUse());
            ASSERT(size       == obj.size());
            ASSERT(true       == obj.isSorted());
            ASSERT(true       == obj.ownsKeys());
            for (size_t i = 0; i < obj.size(); ++i) {
                if (veryVerbose) { T_ P_(i) P_(map[i]) P(obj[i]) }

                ASSERTV(i, map[i] == obj.data()[i]);
                ASSERTV(i, map[i] == obj[i]);
            }
        }

        if (verbose) cout << "\nTesting copy constructor for non-sorted map."
                          << endl;
        {
            const DatumMapRef obj(map, size, false, false);

            ASSERT(bytesInUse == oa.numBytesInUse());

            const DatumMapRef objCopy(obj);

            ASSERT(size  == objCopy.size());
            ASSERT(false == objCopy.isSorted());
            for (size_t i = 0; i < objCopy.size(); ++i) {
                if (veryVerbose) { T_ P_(i) P_(map[i]) P(objCopy[i]) }

                ASSERTV(i, map[i] == objCopy.data()[i]);
                ASSERTV(i, map[i] == objCopy[i]);
            }

            ASSERT(size  == obj.size());
            ASSERT(false == obj.isSorted());
            ASSERT(false == obj.ownsKeys());
            for (size_t i = 0; i < obj.size(); ++i) {
                if (veryVerbose) { T_ P_(i) P_(map[i]) P(obj[i]) }

                ASSERTV(i, map[i] == obj.data()[i]);
                ASSERTV(i, map[i] == obj[i]);
            }
        }

        if (verbose) cout << "\nTesting copy constructor for sorted map."
                          << endl;
        {
            const DatumMapRef obj(map, size, true, false);

            ASSERT(bytesInUse == oa.numBytesInUse());

            const DatumMapRef objCopy(obj);

            ASSERT(size  == objCopy.size());
            ASSERT(true  == objCopy.isSorted());
            ASSERT(false == obj.ownsKeys());

            for (size_t i = 0; i < objCopy.size(); ++i) {
                if (veryVerbose) { T_ P_(i) P_(map[i]) P(objCopy[i]) }

                ASSERTV(i, map[i] == objCopy.data()[i]);
                ASSERTV(i, map[i] == objCopy[i]);
            }

            ASSERT(size  == obj.size());
            ASSERT(true  == obj.isSorted());
            ASSERT(false == obj.ownsKeys());

            for (size_t i = 0; i < obj.size(); ++i) {
                if (veryVerbose) { T_ P_(i) P_(map[i]) P(obj[i]) }

                ASSERTV(i, map[i] == obj.data()[i]);
                ASSERTV(i, map[i] == obj[i]);
            }
        }

        if (verbose) cout << "\nTesting 'print'." << endl;
        {
            const DatumMapRef obj(map, size, false, false);

            if (verbose) cout << "\tTesting single-line format." << endl;
            {
                ostringstream objStr;
                ostringstream expObjStr;

                expObjStr << "[ [ key1 = 3 ] "
                          << "[ key2 = 3.75 ] "
                          << "[ key3 = \"A String\" ] ]";

                obj.print(objStr, 0, -1);

                ASSERT(expObjStr.str() == objStr.str());
            }

            if (verbose) cout << "\tTesting multi-line format." << endl;
            {
                ostringstream objStr;
                ostringstream expObjStr;

                expObjStr << "["                   << "\n"
                          << "["                   << "\n"
                          << "key1 = 3"            << "\n"
                          << "]"                   << "\n"
                          << "["                   << "\n"
                          << "key2 = 3.75"         << "\n"
                          << "]"                   << "\n"
                          << "["                   << "\n"
                          << "key3 = \"A String\"" << "\n"
                          << "]"                   << "\n"
                          << "]"                   << "\n";

                obj.print(objStr, 0, 0);

                ASSERT(expObjStr.str() == objStr.str());

                // Clearing streams.

                expObjStr.str("");
                objStr.str("");

                // 4 spaces per level.

                expObjStr << "["                           << "\n"
                          << "    ["                       << "\n"
                          << "        key1 = 3"            << "\n"
                          << "    ]"                       << "\n"
                          << "    ["                       << "\n"
                          << "        key2 = 3.75"         << "\n"
                          << "    ]"                       << "\n"
                          << "    ["                       << "\n"
                          << "        key3 = \"A String\"" << "\n"
                          << "    ]"                       << "\n"
                          << "]"                           << "\n";

                obj.print(objStr);

                ASSERT(expObjStr.str() == objStr.str());
            }

            if (verbose) cout << "\tTesting level adjustment." << endl;
            {
                ostringstream objStr;
                ostringstream expObjStr;

                // Level 0.

                expObjStr << "["                     << "\n"
                          << " ["                    << "\n"
                          << "  key1 = 3"            << "\n"
                          << " ]"                    << "\n"
                          << " ["                    << "\n"
                          << "  key2 = 3.75"         << "\n"
                          << " ]"                    << "\n"
                          << " ["                    << "\n"
                          << "  key3 = \"A String\"" << "\n"
                          << " ]"                    << "\n"
                          << "]"                     << "\n";

                obj.print(objStr, 0, 1);

                ASSERT(expObjStr.str() == objStr.str());

                // Clearing streams.

                expObjStr.str("");
                objStr.str("");

                // Level 4.

                expObjStr << "    ["                     << "\n"
                          << "     ["                    << "\n"
                          << "      key1 = 3"            << "\n"
                          << "     ]"                    << "\n"
                          << "     ["                    << "\n"
                          << "      key2 = 3.75"         << "\n"
                          << "     ]"                    << "\n"
                          << "     ["                    << "\n"
                          << "      key3 = \"A String\"" << "\n"
                          << "     ]"                    << "\n"
                          << "    ]"                     << "\n";

                obj.print(objStr, 4, 1);

                ASSERT(expObjStr.str() == objStr.str());
            }
        }

        if (verbose) cout << "\nTesting 'operator<<'." << endl;
        {
            const DatumMapRef obj(map, size, false, false);

            bsl::ostringstream os;
            os << obj;

            const char *EXP = "[ [ key1 = 3 ] "
                                "[ key2 = 3.75 ] "
                                "[ key3 = \"A String\" ] ]";
            ASSERTV(EXP, os.str(),  EXP == os.str());
        }

        for (SizeType i = 0; i < size; ++i) {
            Datum::destroy(map[i].value(), &oa);
        }
        ASSERT(0 == oa.status());

        // Creating test values

        const SizeType      SIZE1 = 3;
        const DatumMapEntry map1[SIZE1] = {
            DatumMapEntry(KEY2, Datum::createDate(Date(2003, 10, 15))),
            DatumMapEntry(KEY3, Datum::createError(3, "error", &oa)),
            DatumMapEntry(KEY4, Datum::copyString("A String", &oa))
        };

        const DatumMapEntry map1c[SIZE1] = {
            DatumMapEntry(KEY2, Datum::createDate(Date(2003, 10, 15))),
            DatumMapEntry(KEY3, Datum::createError(3, "error", &oa)),
            DatumMapEntry(KEY4, Datum::copyString("A String", &oa))
        };

        const SizeType      SIZE2 = 3;
        const DatumMapEntry map2[SIZE2] = {
            DatumMapEntry(KEY1, Datum::createDate(Date(2003, 10, 15))),
            DatumMapEntry(KEY3, Datum::createError(3, "error", &oa)),
            DatumMapEntry(KEY4, Datum::copyString("A String", &oa))
        };

        const DatumMapEntry map2c[SIZE2] = {
            DatumMapEntry(KEY1, Datum::createDate(Date(2003, 10, 15))),
            DatumMapEntry(KEY3, Datum::createError(3, "error", &oa)),
            DatumMapEntry(KEY4, Datum::copyString("A String", &oa))
        };

        const SizeType      SIZE3 = 2;
        const DatumMapEntry map3[SIZE3] = {
            DatumMapEntry(KEY2, Datum::createDate(Date(2003, 10, 15))),
            DatumMapEntry(KEY3, Datum::createError(3, "error", &oa)),
        };

        const DatumMapEntry map3c[SIZE3] = {
            DatumMapEntry(KEY2, Datum::createDate(Date(2003, 10, 15))),
            DatumMapEntry(KEY3, Datum::createError(3, "error", &oa)),
        };

        if (verbose) cout << "\nTesting equality operators." << endl;
        {
            if (verbose) cout << "\tTesting operators format." << endl;
            {
                typedef bool (*operatorPtr)(const DatumMapRef&,
                                            const DatumMapRef&);

                // Verify that the signatures and return types are standard.

                operatorPtr operatorEq = bdld::operator==;
                operatorPtr operatorNe = bdld::operator!=;

                (void)operatorEq;  // quash potential compiler warnings
                (void)operatorNe;
            }

            if (verbose) cout << "\nTesting operators correctness." << endl;
            {
                const static struct {
                    int          d_line;    // line number
                    DatumMapRef  d_value1;  // 'DatumMapRef' value1
                    DatumMapRef  d_value2;  // 'DatumMapRef' value2
                } DATA[] = {
     //LINE VALUE
     //---- -------------------
     { L_,  DatumMapRef(map1, SIZE1, true, false),
                                     DatumMapRef(map1c, SIZE1, false, false) },
     { L_,  DatumMapRef(map2, SIZE2, true, false),
                                     DatumMapRef(map2c, SIZE2, false, false) },
     { L_,  DatumMapRef(map3, SIZE3, true, false),
                                     DatumMapRef(map3c, SIZE3, false, false) },
     };

                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i< DATA_LEN; ++i) {
                    const int         LINE  = DATA[i].d_line;
                    const DatumMapRef X     = DATA[i].d_value1;
                    const DatumMapRef Z     = DATA[i].d_value2;

                    if (veryVerbose) { T_ P_(LINE) P_(X) P(Z) }

                    ASSERTV(X, X == X);
                    ASSERTV(Z, Z == Z);
                    ASSERTV(X, !(X != X));
                    ASSERTV(Z, !(Z != Z));

                    for (size_t j = 0; j< DATA_LEN; ++j) {
                        const DatumMapRef Y = DATA[j].d_value1;
                        const DatumMapRef T = DATA[j].d_value2;
                        const bool        EXP = (i == j);

                        if (veryVerbose) { T_ T_ P_(Y) P(T) }

                        ASSERTV(EXP, X, Y, EXP == (X == Y));
                        ASSERTV(EXP, Y, X, EXP == (Y == X));
                        ASSERTV(EXP, X, T, EXP == (X == T));
                        ASSERTV(EXP, T, X, EXP == (T == X));
                        ASSERTV(EXP, Z, Y, EXP == (Z == Y));
                        ASSERTV(EXP, Y, Z, EXP == (Y == Z));
                        ASSERTV(EXP, Z, T, EXP == (Z == T));
                        ASSERTV(EXP, T, Z, EXP == (T == Z));
                        ASSERTV(EXP, X, T, EXP == (X == T));
                        ASSERTV(EXP, T, X, EXP == (T == X));

                        ASSERTV(EXP, X, Y, EXP != (X != Y));
                        ASSERTV(EXP, Y, X, EXP != (Y != X));
                        ASSERTV(EXP, X, T, EXP != (X != T));
                        ASSERTV(EXP, T, X, EXP != (T != X));
                        ASSERTV(EXP, Z, Y, EXP != (Z != Y));
                        ASSERTV(EXP, Y, Z, EXP != (Y != Z));
                        ASSERTV(EXP, Z, T, EXP != (Z != T));
                        ASSERTV(EXP, T, Z, EXP != (T != Z));
                    }
                }

                for (SizeType i = 0; i < SIZE1; ++i) {
                    Datum:: destroy(map1[i].value(), &oa);
                    Datum:: destroy(map1c[i].value(), &oa);
                }
                for (SizeType i = 0; i < SIZE2; ++i) {
                    Datum:: destroy(map2[i].value(), &oa);
                    Datum:: destroy(map2c[i].value(), &oa);
                }
                for (SizeType i = 0; i < SIZE3; ++i) {
                    Datum:: destroy(map3[i].value(), &oa);
                    Datum:: destroy(map3c[i].value(), &oa);
                }
                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nTesting 'find'." << endl;
        {
            // 'loadRandomString' does not use special characters
            const bsl::string notInMapLess = "$%;*&^"; // less than alpha-num
            const bsl::string notInMapGreater = "{}~"; // more than alpha-num

            const int                NUM_ENTRIES = 100;
            bsl::vector<bsl::string> keys(NUM_ENTRIES);
            for (bsls::Types::size_type i = 0; i < NUM_ENTRIES; ++i) {
                loadRandomString(&keys[i], i + 1);
            }

            // Unsorted map.
            {
                bsl::vector<DatumMapEntry> entries(NUM_ENTRIES);
                for (size_t i = 0; i < NUM_ENTRIES; ++i) {
                    Datum value = Datum::createInteger(
                                                      static_cast<int>(i) + 1);
                    entries[i] = DatumMapEntry(keys[i], value);
                }
                const DatumMapRef obj(entries.data(),
                                      NUM_ENTRIES,
                                      false,
                                      false);

                for (size_t i = 0; i < NUM_ENTRIES; ++i) {
                    const Datum *value = obj.find(entries[i].key());

                    if (veryVerbose) { T_ P_(*value) P(entries[i].value()) }

                    ASSERTV(i, *value == entries[i].value());
                }
                ASSERT(0 == obj.find(notInMapLess));
                ASSERT(0 == obj.find(notInMapGreater));
            }

            // Sorted map.
            {
                bsl::sort(keys.begin(), keys.end());
                bsl::vector<DatumMapEntry> entries(NUM_ENTRIES);
                for (size_t i = 0; i < NUM_ENTRIES; ++i) {
                    Datum value = Datum::createInteger(
                                                      static_cast<int>(i) + 1);
                    entries[i] = DatumMapEntry(keys[i], value);
                }
                const DatumMapRef obj(entries.data(),
                                      NUM_ENTRIES,
                                      true,
                                      false);

                for (size_t i = 0; i < NUM_ENTRIES; ++i) {
                    const Datum *value = obj.find(entries[i].key());

                    if (veryVerbose) { T_ P_(*value) P(entries[i].value()) }

                    ASSERTV(i, *value == entries[i].value());
                }
                ASSERT(0 == obj.find(notInMapLess));
                ASSERT(0 == obj.find(notInMapGreater));
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            if (verbose) cout << "\tTesting constructor." << endl;
            {
                const DatumMapEntry  TEMP(KEY1, Datum::createNull());
                const DatumMapEntry *NULL_DME_PTR =
                                               static_cast<DatumMapEntry *>(0);

                ASSERT_SAFE_PASS(DatumMapRef(NULL_DME_PTR, 0, false, false));
                ASSERT_SAFE_FAIL(DatumMapRef(NULL_DME_PTR, 1, false, false));
                ASSERT_SAFE_PASS(DatumMapRef(&TEMP,        0, false, false));
                ASSERT_SAFE_PASS(DatumMapRef(&TEMP,        1, false, false));
            }

            if (verbose) cout << "\tTesting 'operator[]'." << endl;
            {
                const int           SIZE = 3;
                const DatumMapEntry MAP[SIZE] = {
                    DatumMapEntry(KEY1, Datum::createNull()),
                    DatumMapEntry(KEY2, Datum::createNull()),
                    DatumMapEntry(KEY3, Datum::createNull())
                };

                const DatumMapRef obj(MAP, SIZE, false, false);

                ASSERT_SAFE_FAIL(obj[SIZE    ]);
                ASSERT_SAFE_PASS(obj[SIZE - 1]);
                ASSERT_SAFE_PASS(obj[0       ]);
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'DatumIntMapRef'
        //
        // Concerns:
        //: 1 Exercise a broad cross-section of value-semantic functionality.
        //
        // Plan:
        //: 1 Create a 'DatumIntMapRef' object and verify that values
        //:   were correctly passed down to the 'd_data_p' and 'd_size' data
        //:   members. Also exercise the copy construction functionality and
        //:   verify using the equality operators that these objects have the
        //:   same value. Verify that streaming operator outputs the correctly
        //:   formatted value. Also verify that accessors return the correct
        //:   value.  Verify that 'find' returns the expected result for both
        //:   sorted and unsorted maps.
        //
        // Testing:
        //   DatumIntMapRef(const DatumIntMapEntry *, SizeType, bool);
        //   const DatumIntMapEntry& operator[](SizeType index) const;
        //   const DatumIntMapEntry *data() const;
        //   bool isSorted() const;
        //   SizeType size() const;
        //   const Datum *find(int key) const;
        //   bsl::ostream& print(bsl::ostream&, int,int) const;
        //   bool operator==(const DatumIntMapRef&, const DatumIntMapRef&);
        //   bool operator!=(const DatumIntMapRef&, const DatumIntMapRef&);
        //   bsl::ostream& operator<<(bsl::ostream&, const DatumIntMapRef&);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'DatumIntMapRef'" << endl
                          << "========================" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        const SizeType         SIZE = 3;
        const DatumIntMapEntry map[SIZE] = {
            DatumIntMapEntry(1, Datum::createInteger(3)),
            DatumIntMapEntry(2, Datum::createDouble(3.75)),
            DatumIntMapEntry(3, Datum::copyString("A String", &oa))
        };

        // remember how many bytes are allocated by Datums
        const Int64 bytesInUse = oa.numBytesInUse();

        if (verbose) cout << "\nTesting value constructor." << endl;
        {
            const DatumIntMapRef obj(map, SIZE, false);

            ASSERT(bytesInUse == oa.numBytesInUse());
            ASSERT(SIZE       == obj.size());
            ASSERT(false      == obj.isSorted());
            for (size_t i = 0; i < obj.size(); ++i) {
                if (veryVerbose) { T_ P_(i) P_(map[i]) P(obj[i]) }

                ASSERTV(i, map[i] == obj.data()[i]);
                ASSERTV(i, map[i] == obj[i]);
            }
        }
        {
            const DatumIntMapRef obj(map, SIZE, true);
            ASSERT(bytesInUse == oa.numBytesInUse());
            ASSERT(SIZE       == obj.size());
            ASSERT(true       == obj.isSorted());
            for (size_t i = 0; i < obj.size(); ++i) {
                if (veryVerbose) { T_ P_(i) P_(map[i]) P(obj[i]) }

                ASSERTV(i, map[i] == obj.data()[i]);
                ASSERTV(i, map[i] == obj[i]);
            }
        }

        if (verbose) cout << "\nTesting copy constructor for non-sorted map."
                          << endl;
        {
            const DatumIntMapRef obj(map, SIZE, false);

            ASSERT(bytesInUse == oa.numBytesInUse());

            const DatumIntMapRef objCopy(obj);

            ASSERT(SIZE  == objCopy.size());
            ASSERT(false == objCopy.isSorted());
            for (size_t i = 0; i < objCopy.size(); ++i) {
                if (veryVerbose) { T_ P_(i) P_(map[i]) P(objCopy[i]) }

                ASSERTV(i, map[i] == objCopy.data()[i]);
                ASSERTV(i, map[i] == objCopy[i]);
            }

            ASSERT(SIZE  == obj.size());
            ASSERT(false == obj.isSorted());
            for (size_t i = 0; i < obj.size(); ++i) {
                if (veryVerbose) { T_ P_(i) P_(map[i]) P(obj[i]) }

                ASSERTV(i, map[i] == obj.data()[i]);
                ASSERTV(i, map[i] == obj[i]);
            }
        }

        if (verbose)
            cout << "\nTesting copy constructor for sorted map." << endl;
        {
            const DatumIntMapRef obj(map, SIZE, true);

            ASSERT(bytesInUse == oa.numBytesInUse());

            const DatumIntMapRef objCopy(obj);

            ASSERT(SIZE  == objCopy.size());
            ASSERT(true  == objCopy.isSorted());

            for (size_t i = 0; i < objCopy.size(); ++i) {
                if (veryVerbose) { T_ P_(i) P_(map[i]) P(objCopy[i]) }

                ASSERTV(i, map[i] == objCopy.data()[i]);
                ASSERTV(i, map[i] == objCopy[i]);
            }

            ASSERT(SIZE  == obj.size());
            ASSERT(true  == obj.isSorted());

            for (size_t i = 0; i < obj.size(); ++i) {
                if (veryVerbose) { T_ P_(i) P_(map[i]) P(obj[i]) }

                ASSERTV(i, map[i] == obj.data()[i]);
                ASSERTV(i, map[i] == obj[i]);
            }
        }

        if (verbose) cout << "\nTesting 'print'." << endl;
        {
            const DatumIntMapRef obj(map, SIZE, false);

            if (verbose) cout << "\tTesting single-line format." << endl;
            {
                ostringstream objStr;
                ostringstream expObjStr;

                expObjStr << "[ [ 1 = 3 ] "
                          << "[ 2 = 3.75 ] "
                          << "[ 3 = \"A String\" ] ]";

                obj.print(objStr, 0, -1);

                ASSERT(expObjStr.str() == objStr.str());
            }

            if (verbose) cout << "\tTesting multi-line format." << endl;
            {
                ostringstream objStr;
                ostringstream expObjStr;

                expObjStr << "["                << "\n"
                          << "["                << "\n"
                          << "1 = 3"            << "\n"
                          << "]"                << "\n"
                          << "["                << "\n"
                          << "2 = 3.75"         << "\n"
                          << "]"                << "\n"
                          << "["                << "\n"
                          << "3 = \"A String\"" << "\n"
                          << "]"                << "\n"
                          << "]"                << "\n";

                obj.print(objStr, 0, 0);

                ASSERTV(expObjStr.str(), objStr.str(),
                        expObjStr.str() == objStr.str());

                // Clearing streams.

                expObjStr.str("");
                objStr.str("");

                // 4 spaces per level.

                expObjStr << "["                        << "\n"
                          << "    ["                    << "\n"
                          << "        1 = 3"            << "\n"
                          << "    ]"                    << "\n"
                          << "    ["                    << "\n"
                          << "        2 = 3.75"         << "\n"
                          << "    ]"                    << "\n"
                          << "    ["                    << "\n"
                          << "        3 = \"A String\"" << "\n"
                          << "    ]"                    << "\n"
                          << "]"                        << "\n";

                obj.print(objStr);

                ASSERT(expObjStr.str() == objStr.str());
            }

            if (verbose) cout << "\tTesting level adjustment." << endl;
            {
                ostringstream objStr;
                ostringstream expObjStr;

                // Level 0.

                expObjStr << "["                  << "\n"
                          << " ["                 << "\n"
                          << "  1 = 3"            << "\n"
                          << " ]"                 << "\n"
                          << " ["                 << "\n"
                          << "  2 = 3.75"         << "\n"
                          << " ]"                 << "\n"
                          << " ["                 << "\n"
                          << "  3 = \"A String\"" << "\n"
                          << " ]"                 << "\n"
                          << "]"                  << "\n";

                obj.print(objStr, 0, 1);

                ASSERT(expObjStr.str() == objStr.str());

                // Clearing streams.

                expObjStr.str("");
                objStr.str("");

                // Level 4.

                expObjStr << "    ["                  << "\n"
                          << "     ["                 << "\n"
                          << "      1 = 3"            << "\n"
                          << "     ]"                 << "\n"
                          << "     ["                 << "\n"
                          << "      2 = 3.75"         << "\n"
                          << "     ]"                 << "\n"
                          << "     ["                 << "\n"
                          << "      3 = \"A String\"" << "\n"
                          << "     ]"                 << "\n"
                          << "    ]"                  << "\n";

                obj.print(objStr, 4, 1);

                ASSERT(expObjStr.str() == objStr.str());
            }
        }

        if (verbose) cout << "\nTesting 'operator<<'." << endl;
        {
            const DatumIntMapRef obj(map, SIZE, false);

            bsl::ostringstream os;
            os << obj;

            const char *EXP = "[ [ 1 = 3 ] "
                                "[ 2 = 3.75 ] "
                                "[ 3 = \"A String\" ] ]";
            ASSERTV(EXP, os.str(),  EXP == os.str());
        }

        for (SizeType i = 0; i < SIZE; ++i) {
            Datum:: destroy(map[i].value(), &oa);
        }
        ASSERT(0 == oa.status());

        // Creating test values

        const SizeType         SIZE1 = 3;
        const DatumIntMapEntry map1[SIZE1] = {
            DatumIntMapEntry(2, Datum::createDate(Date(2003, 10, 15))),
            DatumIntMapEntry(3, Datum::createError(3, "error", &oa)),
            DatumIntMapEntry(4, Datum::copyString("A String", &oa))
        };

        const DatumIntMapEntry map1c[SIZE1] = {
            DatumIntMapEntry(2, Datum::createDate(Date(2003, 10, 15))),
            DatumIntMapEntry(3, Datum::createError(3, "error", &oa)),
            DatumIntMapEntry(4, Datum::copyString("A String", &oa))
        };

        const SizeType         SIZE2 = 3;
        const DatumIntMapEntry map2[SIZE2] = {
            DatumIntMapEntry(1, Datum::createDate(Date(2003, 10, 15))),
            DatumIntMapEntry(3, Datum::createError(3, "error", &oa)),
            DatumIntMapEntry(4, Datum::copyString("A String", &oa))
        };

        const DatumIntMapEntry map2c[SIZE2] = {
            DatumIntMapEntry(1, Datum::createDate(Date(2003, 10, 15))),
            DatumIntMapEntry(3, Datum::createError(3, "error", &oa)),
            DatumIntMapEntry(4, Datum::copyString("A String", &oa))
        };

        const SizeType         SIZE3 = 2;
        const DatumIntMapEntry map3[SIZE3] = {
            DatumIntMapEntry(2, Datum::createDate(Date(2003, 10, 15))),
            DatumIntMapEntry(3, Datum::createError(3, "error", &oa)),
        };

        const DatumIntMapEntry map3c[SIZE3] = {
            DatumIntMapEntry(2, Datum::createDate(Date(2003, 10, 15))),
            DatumIntMapEntry(3, Datum::createError(3, "error", &oa)),
        };

        if (verbose) cout << "\nTesting equality operators." << endl;
        {
            if (verbose) cout << "\tTesting operators format." << endl;
            {
                typedef bool (*operatorPtr)(const DatumMapRef&,
                                            const DatumMapRef&);

                // Verify that the signatures and return types are standard.

                operatorPtr operatorEq = bdld::operator==;
                operatorPtr operatorNe = bdld::operator!=;

                (void)operatorEq;  // quash potential compiler warnings
                (void)operatorNe;
            }

            if (verbose) cout << "\nTesting operators correctness." << endl;
            {
                const static struct {
                    int             d_line;    // line number
                    DatumIntMapRef  d_value1;  // 'DatumMapRef' value1
                    DatumIntMapRef  d_value2;  // 'DatumMapRef' value2
                } DATA[] = {
     //LINE VALUE
     //---- -------------------
     { L_,  DatumIntMapRef(map1, SIZE1, true),
                                         DatumIntMapRef(map1c, SIZE1, false) },
     { L_,  DatumIntMapRef(map2, SIZE2, true),
                                         DatumIntMapRef(map2c, SIZE2, false) },
     { L_,  DatumIntMapRef(map3, SIZE3, true),
                                         DatumIntMapRef(map3c, SIZE3, false) },
     };

                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i< DATA_LEN; ++i) {
                    const int            LINE  = DATA[i].d_line;
                    const DatumIntMapRef X     = DATA[i].d_value1;
                    const DatumIntMapRef Z     = DATA[i].d_value2;

                    if (veryVerbose) { T_ P_(LINE) P_(X) P(Z) }

                    ASSERTV(X, X == X);
                    ASSERTV(Z, Z == Z);
                    ASSERTV(X, !(X != X));
                    ASSERTV(Z, !(Z != Z));

                    for (size_t j = 0; j< DATA_LEN; ++j) {
                        const DatumIntMapRef Y = DATA[j].d_value1;
                        const DatumIntMapRef T = DATA[j].d_value2;
                        const bool           EXP = (i == j);

                        if (veryVerbose) { T_ T_ P_(Y) P(T) }

                        ASSERTV(EXP, X, Y, EXP == (X == Y));
                        ASSERTV(EXP, Y, X, EXP == (Y == X));
                        ASSERTV(EXP, X, T, EXP == (X == T));
                        ASSERTV(EXP, T, X, EXP == (T == X));
                        ASSERTV(EXP, Z, Y, EXP == (Z == Y));
                        ASSERTV(EXP, Y, Z, EXP == (Y == Z));
                        ASSERTV(EXP, Z, T, EXP == (Z == T));
                        ASSERTV(EXP, T, Z, EXP == (T == Z));
                        ASSERTV(EXP, X, T, EXP == (X == T));
                        ASSERTV(EXP, T, X, EXP == (T == X));

                        ASSERTV(EXP, X, Y, EXP != (X != Y));
                        ASSERTV(EXP, Y, X, EXP != (Y != X));
                        ASSERTV(EXP, X, T, EXP != (X != T));
                        ASSERTV(EXP, T, X, EXP != (T != X));
                        ASSERTV(EXP, Z, Y, EXP != (Z != Y));
                        ASSERTV(EXP, Y, Z, EXP != (Y != Z));
                        ASSERTV(EXP, Z, T, EXP != (Z != T));
                        ASSERTV(EXP, T, Z, EXP != (T != Z));
                    }
                }

                for (SizeType i = 0; i < SIZE1; ++i) {
                    Datum:: destroy(map1[i].value(), &oa);
                    Datum:: destroy(map1c[i].value(), &oa);
                }
                for (SizeType i = 0; i < SIZE2; ++i) {
                    Datum:: destroy(map2[i].value(), &oa);
                    Datum:: destroy(map2c[i].value(), &oa);
                }
                for (SizeType i = 0; i < SIZE3; ++i) {
                    Datum:: destroy(map3[i].value(), &oa);
                    Datum:: destroy(map3c[i].value(), &oa);
                }
                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nTesting 'find'." << endl;
        {
            // Create a unique set of key

            const int notInMapLess    = INT_MIN;
            const int notInMapGreater = INT_MAX;

            const int               NUM_ENTRIES = 100;
            bsl::vector<int>        keys(NUM_ENTRIES);
            bsl::unordered_set<int> uKeys;
            uKeys.insert(notInMapLess);
            uKeys.insert(notInMapGreater);
            for (bsls::Types::size_type i = 0; i < NUM_ENTRIES; ++i) {
                do {
                    keys[i] = rand();
                } while (uKeys.find(keys[i]) != uKeys.end());
                uKeys.insert(keys[i]);
            }

            // Unsorted map.
            {
                bsl::vector<DatumIntMapEntry> entries(NUM_ENTRIES);
                for (size_t i = 0; i < NUM_ENTRIES; ++i) {
                    Datum value = Datum::createInteger(
                                                      static_cast<int>(i) + 1);
                    entries[i] = DatumIntMapEntry(keys[i], value);
                }
                const DatumIntMapRef obj(entries.data(), NUM_ENTRIES, false);

                for (size_t i = 0; i < NUM_ENTRIES; ++i) {
                    const Datum *value = obj.find(entries[i].key());

                    if (veryVerbose) { T_ P_(*value) P(entries[i].value()) }

                    ASSERTV(i, *value, entries[i].value(),
                            *value == entries[i].value());
                }
                ASSERT(0 == obj.find(notInMapLess));
                ASSERT(0 == obj.find(notInMapGreater));
            }

            // Sorted map.
            {
                bsl::sort(keys.begin(), keys.end());
                bsl::vector<DatumIntMapEntry> entries(NUM_ENTRIES);
                for (size_t i = 0; i < NUM_ENTRIES; ++i) {
                    Datum value = Datum::createInteger(
                                                      static_cast<int>(i) + 1);
                    entries[i] = DatumIntMapEntry(keys[i], value);
                }
                const DatumIntMapRef obj(entries.data(), NUM_ENTRIES, true);

                for (size_t i = 0; i < NUM_ENTRIES; ++i) {
                    const Datum *value = obj.find(entries[i].key());

                    if (veryVerbose) { T_ P_(*value) P(entries[i].value()) }

                    ASSERTV(i, *value == entries[i].value());
                }
                ASSERT(0 == obj.find(notInMapLess));
                ASSERT(0 == obj.find(notInMapGreater));
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            if (verbose) cout << "\tTesting constructor." << endl;
            {
                const DatumIntMapEntry  TEMP(1, Datum::createNull());
                const DatumIntMapEntry *NULL_DME_PTR =
                                            static_cast<DatumIntMapEntry *>(0);

                ASSERT_SAFE_PASS(DatumIntMapRef(NULL_DME_PTR, 0, false));
                ASSERT_SAFE_FAIL(DatumIntMapRef(NULL_DME_PTR, 1, false));
                ASSERT_SAFE_PASS(DatumIntMapRef(&TEMP,        0, false));
                ASSERT_SAFE_PASS(DatumIntMapRef(&TEMP,        1, false));
            }

            if (verbose) cout << "\tTesting 'operator[]'." << endl;
            {
                const int              SIZE = 3;
                const DatumIntMapEntry MAP[SIZE] = {
                    DatumIntMapEntry(1, Datum::createNull()),
                    DatumIntMapEntry(2, Datum::createNull()),
                    DatumIntMapEntry(3, Datum::createNull())
                };

                const DatumIntMapRef obj(MAP, SIZE, false);

                ASSERT_SAFE_FAIL(obj[SIZE   ]);
                ASSERT_SAFE_PASS(obj[SIZE - 1]);
                ASSERT_SAFE_PASS(obj[0      ]);
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'DatumArrayRef'
        //
        // Concerns:
        //: 1 Exercise a broad cross-section of value-semantic functionality.
        //
        // Plan:
        //: 1 Create a 'DatumArrayRef' object and verify that values were
        //:   correctly passed down to the 'd_data_p' and 'd_size' data
        //:   members.  Also exercise the copy construction and assignment
        //:   operator functionality and verify using the equality operators
        //:   that these objects have the same value. Verify that streaming
        //:   operator outputs the correctly formatted value. Also verify that
        //:   accessors return the correct value.
        //
        // Testing:
        //   DatumArrayRef();
        //   DatumArrayRef(const Datum *data, SizeType length);
        //   const Datum& operator[](SizeType index) const;
        //   const Datum *data() const;
        //   SizeType length() const;
        //   bsl::ostream& print(bsl::ostream& stream, int, int) const;
        //   bool operator==(const DatumArrayRef&, const DatumArrayRef&);
        //   bool operator!=(const DatumArrayRef&, const DatumArrayRef&);
        //   bsl::ostream& operator<<(bsl::ostream&, const DatumArrayRef&);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'DatumArrayRef'" << endl
                          << "=======================" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);

        if (verbose) cout << "\nTesting default constructor." << endl;
        {
            const DatumArrayRef obj;
            ASSERT(0 == obj.length());
            ASSERT(0 == obj.data());
        }

        if (verbose) cout << "\nTesting value constructor." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            const size_t SIZE = 3;
            const Datum  array[SIZE] = { Datum::createInteger(3),
                                         Datum::createDouble(2.25),
                                         Datum::createError(3, "error", &oa) };

            const Int64 bytesInUse = oa.numBytesInUse();

            const DatumArrayRef obj(array, SIZE);

            ASSERT(bytesInUse == oa.numBytesInUse());
            ASSERT(SIZE       == obj.length());
            ASSERT(array      == obj.data());
            for (size_t i = 0; i < 3; ++i) {
                ASSERTV(i, array[i] == obj[i]);
            }

            Datum::destroy(array[2], &oa);
            ASSERT(0 == oa.status());
        }

        if (verbose) cout << "\nTesting equality operators." << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            typedef bool (*operatorPtr)(const DatumArrayRef&,
                                        const DatumArrayRef&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = bdld::operator==;
            operatorPtr operatorNe = bdld::operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        if (verbose) cout << "\nCompare every value with every value." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            const Datum A[3] = { Datum::createInteger(3),
                                 Datum::createDouble(2.25),
                                 Datum::createError(3, "error", &oa) };

            const Datum B[1] = { Datum::createInteger(3) };

            const Datum C[2] = { Datum::createInteger(3),
                                 Datum::createDouble(5.0) };

            const static struct {
                int            d_line;   // line number
                DatumArrayRef  d_value;  // 'DatumArrayRef' value
            } DATA[] = {
                //LINE VALUE
                //---- -------------------
                { L_,  DatumArrayRef()                   },
                { L_,  DatumArrayRef(A, 3)  },
                { L_,  DatumArrayRef(B, 1)  },
                { L_,  DatumArrayRef(C, 2)  },
            };

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i< DATA_LEN; ++i) {
                const int           LINE  = DATA[i].d_line;
                const DatumArrayRef X = DATA[i].d_value;

                ASSERTV(LINE, X, X == X);
                ASSERTV(LINE, X, !(X != X));

                for (size_t j = 0; j< DATA_LEN; ++j) {
                    const DatumArrayRef Y = DATA[j].d_value;
                    const bool          EXP = (i == j);

                    if (veryVerbose) { T_ P_(i) P(j) }

                    ASSERTV(LINE, EXP, X, Y, EXP == (X == Y));
                    ASSERTV(LINE, EXP, Y, X, EXP == (Y == X));
                    ASSERTV(LINE, EXP, X, Y, EXP != (X != Y));
                    ASSERTV(LINE, EXP, Y, X, EXP != (Y != X));
                }
            }
            Datum::destroy(A[2], &oa);
            ASSERT(0 == oa.status());
        }

        if (verbose) cout << "\nTesting 'print'." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            const int   SIZE = 3;
            const Datum ARRAY[SIZE] = { Datum::createInteger(3),
                                        Datum::createDouble(2.25),
                                        Datum::createError(3, "error", &oa) };

            const DatumArrayRef obj(ARRAY, SIZE);

            if (verbose) cout << "\tTesting single-line format." << endl;
            {
                ostringstream objStr;
                ostringstream expObjStr;

                expObjStr << "[ 3 2.25 error(3,'error') ]";

                obj.print(objStr, 0, -1);

                ASSERT(expObjStr.str() == objStr.str());
            }

            if (verbose) cout << "\tTesting multi-line format." << endl;
            {
                ostringstream objStr;
                ostringstream expObjStr;

                expObjStr << "["                << "\n"
                          << "3"                << "\n"
                          << "2.25"             << "\n"
                          << "error(3,'error')" << "\n"
                          << "]"                << "\n";

                obj.print(objStr, 0, 0);

                ASSERTV(expObjStr.str(), objStr.str(),
                        expObjStr.str() == objStr.str());

                // Clearing streams.

                expObjStr.str("");
                objStr.str("");

                // 4 spaces per level.

                expObjStr << "["                    << "\n"
                          << "    3"                << "\n"
                          << "    2.25"             << "\n"
                          << "    error(3,'error')" << "\n"
                          << "]"                    << "\n";

                obj.print(objStr);

                ASSERTV(expObjStr.str(), objStr.str(),
                        expObjStr.str() == objStr.str());
            }

            if (verbose) cout << "\tTesting level adjustment." << endl;
            {
                ostringstream objStr;
                ostringstream expObjStr;

                // Level 0.

                expObjStr << "["                 << "\n"
                          << " 3"                << "\n"
                          << " 2.25"             << "\n"
                          << " error(3,'error')" << "\n"
                          << "]"                 << "\n";

                obj.print(objStr, 0, 1);

                ASSERTV(expObjStr.str(), objStr.str(),
                        expObjStr.str() == objStr.str());

                // Clearing streams.

                expObjStr.str("");
                objStr.str("");

                // Level 4.

                expObjStr << "    ["                 << "\n"
                          << "     3"                << "\n"
                          << "     2.25"             << "\n"
                          << "     error(3,'error')" << "\n"
                          << "    ]"                 << "\n";

                obj.print(objStr, 4, 1);

                ASSERTV(expObjStr.str(), objStr.str(),
                        expObjStr.str() == objStr.str());
            }

            Datum::destroy(ARRAY[2], &oa);
            ASSERT(0 == oa.status());
        }

        if (verbose) cout << "\nTesting streaming operator." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            const size_t SIZE = 3;
            const Datum  array[SIZE] = { Datum::createInteger(3),
                                         Datum::createDouble(2.25),
                                         Datum::createError(5, "error", &oa) };

            const DatumArrayRef obj(array, SIZE);

            bsl::ostringstream os;
            os << obj;

            ASSERTV(os.str(), "[ 3 2.25 error(5,'error') ]" == os.str());

            Datum::destroy(array[2], &oa);
            ASSERT(0 == oa.status());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            if (verbose) cout << "\tTesting constructor." << endl;
            {
                const Datum  TEMP = Datum::createNull();
                const Datum *NULL_DATUM_PTR = static_cast<Datum *>(0);

                ASSERT_SAFE_PASS(DatumArrayRef(NULL_DATUM_PTR, 0));
                ASSERT_SAFE_FAIL(DatumArrayRef(NULL_DATUM_PTR, 1));
                ASSERT_SAFE_PASS(DatumArrayRef(&TEMP,          0));
                ASSERT_SAFE_PASS(DatumArrayRef(&TEMP,          1));
            }

            if (verbose) cout << "\tTesting 'operator[]'." << endl;
            {
                const size_t SIZE = 3;
                const Datum  ARRAY[SIZE] = { Datum::createNull(),
                                             Datum::createNull(),
                                             Datum::createNull() };

                const DatumArrayRef obj(ARRAY, SIZE);

                ASSERT_SAFE_FAIL(obj[SIZE    ]);
                ASSERT_SAFE_PASS(obj[SIZE - 1]);
                ASSERT_SAFE_PASS(obj[0       ]);
            }
        }
        ASSERT(0 == da.status())
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'DatumMutableMapOwningKeysRef'
        //
        // Concerns:
        //: 1 Value constructor can create 'DatumMutableMapOwningKeysRef'
        //:   object.
        //:
        //: 2 Values supplied at construction is correctly passed down to the
        //:   data member and correctly reported by accessors.
        //
        // Plan:
        //: 1 Create a 'DatumMutableMapOwningKeysRef' object and verify using
        //:   accessors that values were correctly passed down to the
        //:   'd_data_p', 'd_size_p', 'd_keys_p' and 'd_sorted_p' data members.
        //:   (C-1,2)
        //
        // Testing:
        //   DatumMutableMapOwningKeysRef();
        //   DatumMutableMapOwningKeysRef(DatumMapEntry*,SizeType*,char*, ...);
        //   DatumMapEntry *data() const;
        //   SizeType *size() const;
        //   char *keys() const;
        //   bool *sorted() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'DatumMutableMapOwningKeysRef'" << endl
                          << "======================================" << endl;

        if (verbose) cout << "\nTesting default constructor." << endl;
        {
            DatumMutableMapOwningKeysRef obj;
            ASSERT(0 == obj.data());
            ASSERT(0 == obj.size());
            ASSERT(0 == obj.keys());
            ASSERT(0 == obj.sorted());
        }

        if (verbose) cout << "\nTesting value constructor." << endl;
        {
            DatumMapEntry                          data;
            DatumMutableMapOwningKeysRef::SizeType size;
            char                                   keys;
            bool                                   sorted;
            const DatumMutableMapOwningKeysRef     obj(&data,
                                                       &size,
                                                       &keys,
                                                       &sorted);

            ASSERT(&data   == obj.data());
            ASSERT(&size   == obj.size());
            ASSERT(&keys   == obj.keys());
            ASSERT(&sorted == obj.sorted());
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'DatumMutableMapRef'
        //
        // Concerns:
        //: 1 Value constructor can create 'DatumMutableMapRef' object.
        //:
        //: 2 Values supplied at construction is correctly passed down to the
        //:   data member and correctly reported by accessors.
        //
        // Plan:
        //: 1 Create a 'DatumMutableMapRef' object and verify using accessors
        //:   that values were correctly passed down to the 'd_data_p',
        //:   'd_size_p', and 'd_sorted_p' data members.  (C-1,2)
        //
        // Testing:
        //   DatumMutableMapRef();
        //   DatumMutableMapRef(DatumMapEntry *, SizeType *, bool *);
        //   DatumMapEntry *data() const;
        //   SizeType *size() const;
        //   bool *sorted() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'DatumMutableMapRef'" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting default constructor." << endl;
        {
            DatumMutableMapRef obj;
            ASSERT(0 == obj.data());
            ASSERT(0 == obj.size());
            ASSERT(0 == obj.sorted());
        }

        if (verbose) cout << "\nTesting value constructor." << endl;
        {
            DatumMapEntry                data;
            DatumMutableMapRef::SizeType size;
            bool                         sorted;
            const DatumMutableMapRef     obj(&data, &size, &sorted);
            ASSERT(&data   == obj.data());
            ASSERT(&size   == obj.size());
            ASSERT(&sorted == obj.sorted());
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'DatumMutableArrayRef'
        //
        // Concerns:
        //: 1 Value constructor can create 'DatumMutableArrayRef' object.
        //:
        //: 2 Values supplied at construction is correctly passed down to the
        //:   data member and correctly reported by accessors.
        //
        // Plan:
        //: 1 Create a 'DatumMutableArrayRef' object and verify using accessors
        //:   that values were correctly passed down to the 'd_data_p' and
        //:   'd_length_p' data members.  (C-1,2)
        //
        // Testing:
        //   DatumMutableArrayRef();
        //   DatumMutableArrayRef(Datum *data, SizeType *length);
        //   Datum *data() const;
        //   SizeType *length() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'DatumMutableArrayRef'" << endl
                          << "==============================" << endl;

        if (verbose) cout << "\nTesting default constructor." << endl;
        {
            DatumMutableArrayRef obj;
            ASSERT(0 == obj.data());
            ASSERT(0 == obj.length());
        }

        if (verbose) cout << "\nTesting value constructor." << endl;
        {
            Datum                      data;
            Datum::SizeType            length;
            const DatumMutableArrayRef obj(&data, &length);

            ASSERT(&data   == obj.data());
            ASSERT(&length == obj.length());
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'DatumMapEntry'
        //
        // Concerns:
        //: 1 Can create a 'DatumMapEntry' object using value constructor.
        //:
        //: 2 Verify that the key and value is correctly passed to the
        //:   corresponding class member.
        //:
        //: 3 The key and value of the 'DatumMapEntry' can be changed by
        //:   manipulators.
        //:
        //: 4 Two 'DatumMapEntry's object can be compared for equality.
        //:
        //: 5 The 'DatumMapEntry' object can be streamed with 'operator<<'.
        //
        // Plan:
        //: 1 Create a 'DatumMapEntry' object and verify that the values were
        //:   correctly passed down to the data members using accessors.
        //:
        //: 2 Change the existing 'DatumMapEntry" object using manipulators and
        //:   verify that the values were correctly passed down to the data
        //:   members.
        //:
        //: 3 Test equality operators for 'DatumMapEntry' objects.
        //:
        //: 4 Test streaming operator.
        //
        // Testing:
        //   DatumMapEntry();
        //   DatumMapEntry(const bslstl::StringRef& key, const Datum& value);
        //   const bslstl::StringRef& key() const;
        //   const Datum& value() const;
        //   void setKey(const bslstl::StringRef&);
        //   void setValue(const Datum&);
        //   bool operator==(const DatumMapEntry&, const DatumMapEntry&);
        //   bool operator!=(const DatumMapEntry&, const DatumMapEntry&);
        //   bsl::ostream& print(bsl::ostream&, int, int) const;
        //   bsl::ostream& operator<<(bsl::ostream&, const DatumMapEntry&);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'DatumMapEntry'" << endl
                          << "=======================" << endl;

        if (verbose) cout << "\nTesting default constructor." << endl;
        {
            const DatumMapEntry obj;
            ASSERT(StringRef() == obj.key());
            // The assertion below is invalid - the Datum value is
            // uninitialized and cannot be compared with any other Datum
            //ASSERT(Datum()     == obj.value());
        }

        if (verbose) cout << "\nTesting value constructor." << endl;
        {
            DatumMapEntry obj("key", Datum::createInteger(3));
            ASSERT(StringRef("key")        == obj.key());
            ASSERT(Datum::createInteger(3) == obj.value());
        }

        if (verbose) cout << "\nTesting manipulators." << endl;
        {
            DatumMapEntry obj("key", Datum::createInteger(3));
            ASSERT(StringRef("key")        == obj.key());
            ASSERT(Datum::createInteger(3) == obj.value());
            obj.setKey("anotherKey");
            ASSERT(StringRef("anotherKey") == obj.key());
            ASSERT(Datum::createInteger(3) == obj.value());
        }
        {
            DatumMapEntry obj("key", Datum::createInteger(3));
            ASSERT(StringRef("key")          == obj.key());
            ASSERT(Datum::createInteger(3)   == obj.value());
            obj.setValue(Datum::createDouble(2.25));
            ASSERT(StringRef("key")          == obj.key());
            ASSERT(Datum::createDouble(2.25) == obj.value());
        }

        if (verbose) cout << "\nTesting 'print'." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            const DatumMapEntry obj("key", Datum::createInteger(3));

            if (verbose) cout << "\tTesting single-line format." << endl;
            {
                ostringstream objStr;
                ostringstream expObjStr;

                expObjStr << "[ key = 3 ]";

                obj.print(objStr, 0, -1);

                ASSERT(expObjStr.str() == objStr.str());
            }

            if (verbose) cout << "\tTesting multi-line format." << endl;
            {
                ostringstream objStr;
                ostringstream expObjStr;

                expObjStr << "["       << "\n"
                          << "key = 3" << "\n"
                          << "]"       << "\n";

                obj.print(objStr, 0, 0);

                ASSERT(expObjStr.str() == objStr.str());

                // Clearing streams.

                expObjStr.str("");
                objStr.str("");

                // 4 spaces per level.

                expObjStr << "["           << "\n"
                          << "    key = 3" << "\n"
                          << "]"           << "\n";

                obj.print(objStr);

                ASSERT(expObjStr.str() == objStr.str());
            }

            if (verbose) cout << "\tTesting level adjustment." << endl;
            {
                ostringstream objStr;
                ostringstream expObjStr;

                // Level 0.

                expObjStr << "["        << "\n"
                          << " key = 3" << "\n"
                          << "]"        << "\n";

                obj.print(objStr, 0, 1);

                ASSERT(expObjStr.str() == objStr.str());

                // Clearing streams.

                expObjStr.str("");
                objStr.str("");

                // Level 4.

                expObjStr << "    ["        << "\n"
                          << "     key = 3" << "\n"
                          << "    ]"        << "\n";

                obj.print(objStr, 4, 1);

                ASSERT(expObjStr.str() == objStr.str());
            }

            ASSERT(0 == oa.status());
        }

        if (verbose) cout << "\nTesting 'operator<<'" << endl;
        {
            if (verbose) cout << "\nTesting operator correctness." << endl;
            {
                const DatumMapEntry obj("key", Datum::createInteger(3));
                bsl::ostringstream  os;
                ASSERT(&os == &(os << obj));
                ASSERT(string("[ key = 3 ]") == os.str());
            }

            if (verbose) cout << "\nTesting operator format." << endl;
            {
                typedef ostream& (*operatorPtr)(ostream&,const DatumMapEntry&);

                operatorPtr operatorOut = bdld::operator<<;
                (void) operatorOut;
            }
        }

        if (verbose) cout << "\nTesting equality operators." << endl;
        {
            if (verbose) cout << "\nTesting operators correctness." << endl;
            {
                const static struct {
                    int           d_line;   // line number
                    DatumMapEntry d_value;  // 'DatumMapEntry' value
                } DATA[] = {
                    //LINE VALUE
                    //---- -----------------------------------------------
                    { L_,  DatumMapEntry("",     Datum::createNull())      },
                    { L_,  DatumMapEntry("",     Datum::createInteger(1))  },
                    { L_,  DatumMapEntry("key1", Datum::createInteger(1))  },
                    { L_,  DatumMapEntry("key1", Datum::createInteger(2))  },
                    { L_,  DatumMapEntry("key1", Datum::createDouble(2.0)) },
                    { L_,  DatumMapEntry("key",  Datum::createInteger(1))  },
                };

                const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

                for (size_t i = 0; i< DATA_LEN; ++i) {
                    const int           LINE = DATA[i].d_line;
                    const DatumMapEntry X    = DATA[i].d_value;

                    if (veryVerbose) { T_ P_(LINE) P(X) }

                    ASSERTV(X, X == X);
                    ASSERTV(X, !(X != X));

                    for (size_t j = 0; j< DATA_LEN; ++j) {
                        if (veryVerbose) {
                            T_ P_(i) P(j)
                        }

                        const DatumMapEntry Y   = DATA[j].d_value;
                        const bool          EXP = (i == j);

                        ASSERTV(EXP, X, Y, EXP == (X == Y));
                        ASSERTV(EXP, Y, X, EXP == (Y == X));
                        ASSERTV(EXP, X, Y, EXP != (X != Y));
                        ASSERTV(EXP, Y, X, EXP != (Y != X));
                    }
                }
            }

            if (verbose) cout << "\nTesting operators format." << endl;
            {
                typedef bool (*operatorPtr)(const DatumMapEntry&,
                                            const DatumMapEntry&);

                // Verify that the signatures and return types are standard.

                operatorPtr operatorEq   = bdld::operator==;
                operatorPtr operatorNe   = bdld::operator!=;

                // Quash potential compiler warnings.

                (void)operatorEq;
                (void)operatorNe;
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //   Verify the assignment operator works as expected.
        //
        // Concerns:
        //: 1 Any value is assignable to an object having any initial value.
        //:
        //: 2 The original object is passed as a reference providing
        //:   non-modifiable access to that object.
        //:
        //: 3 Any object must be assignable to itself.
        //:
        //: 4 No memory is allocated by an assignment operator.
        //
        // Plan:
        //: 1 Construct and initialize a set S of (unique) objects with
        //:   substantial and varied differences in value.  Using all
        //:   combinations (u, v) in the cross product S x S, assign v to u and
        //:   assert that u == v and v is unchanged.  (C-1,2)
        //:
        //: 2 Test aliasing by assigning (a temporary copy of) each u to itself
        //:   and verifying that its value remains unchanged.  (C-3)
        //:
        //: 3 Verify that no additional memory was allocated by an assignment
        //:   operator.  (C-4)
        //
        // Testing:
        //   Datum& operator=(const Datum& rhs) = default;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING ASSIGNMENT OPERATOR" << endl
                          << "===========================" << endl;

        if (verbose) cout <<
             "\nCreate a table of distinct type/value combinations." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            bsl::vector<Datum> vector1;
            bsl::vector<Datum> vector2;

            populateWithNonAggregateValues(&vector1, &oa);
            populateWithNonAggregateValues(&vector2, &oa);

            ASSERT(vector1.size() == vector2.size());

            if (verbose)
                cout << "\nAssign every value with every value." << endl;
            {
                for (size_t i = 0; i < vector1.size(); ++i) {
                    // Copy construct or will loose allocated memory after
                    // assignment.
                    Datum        mD(vector1[i]);
                    const Datum& D = mD;

                    for (size_t j = 0; j < vector2.size(); ++j) {
                        const Datum& X = vector2[j];

                        // Setting expected compare value after assignment
                        bool  expected = true; // expected to be equal

                        // Handling NaNs cases - NaNs do not compare equal
                        if ((X.isDouble() &&
                                    X.theDouble() != X.theDouble()) ||
                            (X.isDecimal64() &&
                                    X.theDecimal64() != X.theDecimal64()) ) {
                            expected = false;
                        }
                        const bool EXP = expected;

                        if (veryVerbose) { T_ P_(D) P(X) }

                        mD = X;

                        ASSERTV(EXP, D, X, EXP == (D == X));
                    }
                }
            }

            for (size_t i = 0; i < vector1.size(); ++i) {
                if (veryVerbose) {
                    T_ P(i)
                }

                Datum::destroy(vector1[i], &oa);
                Datum::destroy(vector2[i], &oa);
            }

            ASSERT(0 == oa.status());
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //   The definition of the equality of two 'Datum' objects depends on
        //   the type and the value of the data held by the objects.  Note,
        //   that for 'double' and 'Decimal64' values of 'NaN', the identity
        //   property of the quality is violated ( i.e. two 'NaN' objects never
        //   compare equal). Also note, that this test covers only 'Datum's
        //   holding non-aggregate data type.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if their
        //:   corresponding type and values compare equal (except 'NaN')
        //:
        //: 2 'true  == (X == X)' (i.e., identity).
        //:
        //: 3 'false == (X != X)' (i.e., identity).
        //:
        //: 4 'X == Y' if and only if 'Y == X' (i.e., commutativity).
        //:
        //: 5 'X != Y' if and only if 'Y != X' (i.e., commutativity).
        //:
        //: 6 'X != Y' if and only if '!(X == Y)'.
        //:
        //: 7 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 8 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //: 9 The equality-comparison operators' signatures and return types
        //:   are standard.
        //:
        //:10 Two Datums holding 'NaN' values are not equal.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free
        //:   equality-comparison operators defined in this component.
        //:   (C-7..9)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their type and value
        //:   representation.
        //:
        //: 3 For each row 'R1' in the table of P-2:  (C-1..6)
        //:
        //:   1 Create a 'const' reference to an object, 'X', having the value
        //:     from 'R1'.
        //:
        //:   2 Using 'X', verify the reflexive (anti-reflexive) property of
        //:     equality (inequality) in the presence of aliasing.  (C-2..3)
        //:
        //:   3 For each row 'R2' in the table of P-2:  (C-1, 4..6)
        //:
        //:     1 Record, in 'EXP', whether or not distinct objects set to
        //:       values from 'R1' and 'R2', respectively, are expected to have
        //:       the same value.
        //:
        //:     2 Create a 'const' reference to an object, 'Y', having the
        //:       value from 'R2'.
        //:
        //:     3 Using 'X' and 'Y', verify the commutativity property and
        //:       expected return value for both '==' and '!='.  (C-1, 4..6)
        //
        // Testing:
        //   bool operator==(const Datum&, const Datum&);  // non-aggregate
        //   bool operator!=(const Datum&, const Datum&);  // non-aggregate
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EQUALITY-COMPARISON OPERATORS" << endl
                          << "=============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            typedef bool (*operatorPtr)(const Datum&, const Datum&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = bdld::operator==;
            operatorPtr operatorNe = bdld::operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        if (verbose)
            cout << "\nCreate a table of distinct non-aggregate type/value "
                 << "combinations." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            bsl::vector<Datum> vector1;
            bsl::vector<Datum> vector2;

            populateWithNonAggregateValues(&vector1, &oa);
            populateWithNonAggregateValues(&vector2, &oa);

            ASSERT(vector1.size() == vector2.size());

            if (verbose)
                cout << "\nCompare every value with every value." << endl;
            {
                for (size_t i = 0; i < vector1.size(); ++i) {
                    const Datum& X = vector1[i];

                    if (veryVerbose) { T_ P(X) }

                    // Verify compare with itself (alias test).
                    if ((X.isDouble() &&
                                X.theDouble() != X.theDouble()) ||
                        (X.isDecimal64() &&
                                X.theDecimal64() != X.theDecimal64())) {
                        // Handling NaN cases - NaNs do not compare equal
                        ASSERTV(X,   X != X);
                        ASSERTV(X, !(X == X));
                    } else {
                        // All other types must compare equal
                        ASSERTV(X,   X == X);
                        ASSERTV(X, !(X != X));
                    }

                    for (size_t j = 0; j < vector2.size(); ++j) {
                        const Datum& Y        = vector2[j];
                        bool         expected = (i == j); // expected equality

                        // Handling NaNs cases - NaNs do not compare equal
                        if ((X.isDouble() &&
                                    X.theDouble() != X.theDouble()) ||
                            (Y.isDouble() &&
                                    Y.theDouble() != Y.theDouble()) ||
                            (X.isDecimal64() &&
                                    X.theDecimal64() != X.theDecimal64()) ||
                            (Y.isDecimal64() &&
                                    Y.theDecimal64() != Y.theDecimal64()) ) {
                            expected = false;
                        }

                        const bool EXP = expected;

                        if (veryVerbose) { T_ T_ P_(EXP) P_(X) P(Y) }

                        // Verify value and commutative property

                        ASSERTV(Y, X,  EXP == (Y == X));
                        ASSERTV(X, Y,  EXP == (X == Y));

                        ASSERTV(Y, X, !EXP == (Y != X));
                        ASSERTV(X, Y, !EXP == (X != Y));
                    }
                }
            }

            for (size_t i = 0; i < vector1.size(); ++i) {
                if (veryVerbose) {
                    T_ P(i)
                }

                Datum::destroy(vector1[i], &oa);
                Datum::destroy(vector2[i], &oa);
            }

            ASSERT(0 == oa.status());
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.  Note,
        //   that this test covers only 'Datum's holding non-aggregate data
        //   type.
        //
        // Concerns:
        //: 1 The copy constructor creates an object having the same type and
        //:   value as that of the supplied original object.
        //:
        //: 2 The original object is passed as a reference providing
        //:   non-modifiable access to that object.
        //:
        //: 3 The value of the original object is unchanged.
        //:
        //: 4 No memory is allocated by the copy constructor.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their type and value.
        //:
        //: 2 For each row 'R' in the table of P-1:  (C-1..3)
        //:
        //:   1 Use the corresponding 'Datum' creator to create a 'const'
        //:     object 'Z', having the value from 'R'.
        //:
        //:   2 Use the copy constructor to create an object 'X' from 'Z'.
        //:     (C-2)
        //:
        //:   3 Use the basic accessors to verify that:  (C-1, 3)
        //:
        //:     1 'X' has the same type and value as that of 'Z'.  (C-1)
        //:
        //:     2 'Z' still has the same type and value.  (C-3)
        //:
        //:   4 Verify that no additional memory was allocated by the copy
        //:     constructor.  (C-4)
        //
        // Testing:
        //   Datum(const Datum& original) = default;;
        //---------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING COPY CONSTRUCTOR" << endl
                          << "========================" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);

        if (verbose) cout << "\nTesting 'boolean' data type" << endl;
        {
            const static bool DATA[] = { true, false };
            const size_t      DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i< DATA_LEN; ++i) {
                const bool VALUE = DATA[i];

                if (veryVerbose) { T_ P(VALUE) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                const Datum Z  = Datum::createBoolean(VALUE);

                ASSERT(0 == oa.numBlocksInUse()); // non allocating type

                ASSERT(true  == Z.isBoolean());
                ASSERT(VALUE == Z.theBoolean());

                const Datum X(Z);

                ASSERT(0 == oa.numBlocksInUse()); // non allocating type

                // Verify the type and the value of the object.
                ASSERT(true == X.isBoolean());
                ASSERTV(VALUE, VALUE == X.theBoolean());

                // Verify that the value of 'Z' has not changed.
                ASSERT(true  == Z.isBoolean());
                ASSERT(VALUE == Z.theBoolean());

                Datum::destroy(Z, &oa);

                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nTesting 'Date' data type" << endl;
        {
            const static bdlt::Date DATA[] = {
                Date(),
                Date(1999, 12, 31),
                Date(2015,  1,  1),
                Date(2200,  8, 12),
                Date(9999, 12, 31),
            };

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i< DATA_LEN; ++i) {
                const bdlt::Date VALUE = DATA[i];

                if (veryVerbose) { T_ P(VALUE) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                const Datum  Z = Datum::createDate(VALUE);

                ASSERT(0 == oa.numBlocksInUse()); // non allocating type

                ASSERT(true  == Z.isDate());
                ASSERT(VALUE == Z.theDate());

                const Datum X(Z);

                ASSERT(0 == oa.numBlocksInUse()); // non allocating type

                // Verify the type and the value of the object.
                ASSERT(true == X.isDate());
                ASSERTV(VALUE, VALUE == X.theDate());

                // Verify that the value of 'Z' has not changed.
                ASSERT(true  == Z.isDate());
                ASSERT(VALUE == Z.theDate());

                Datum::destroy(Z, &oa);

                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nTesting 'Datetime' data type." << endl;
        {
            const static bdlt::Datetime DATA[] = {
                Datetime(),
                Datetime(1999, 12, 31, 12, 45, 31,  18, 317),
                Datetime(2015,  2,  2,  1,  1,  1,   1, 1),
                Datetime(2200,  9, 11, 18, 10, 59, 458, 239),
                Datetime(9999,  9,  9,  9,  9,  9, 999, 999),
                Datetime(1999, 12, 31, 12, 45, 31,  18),
                Datetime(2015,  2,  2,  1,  1,  1,   1),
                Datetime(2200,  9, 11, 18, 10, 59, 458),
                Datetime(9999,  9,  9,  9,  9,  9, 999),
            };

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i< DATA_LEN; ++i) {
                const bdlt::Datetime VALUE = DATA[i];

                if (veryVerbose) { T_ P(VALUE) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                const Datum Z = Datum::createDatetime(VALUE, &oa);

                const Int64 bytesInUse = oa.numBytesInUse();

                ASSERT(true  == Z.isDatetime());
                ASSERT(VALUE == Z.theDatetime());

                const Datum X(Z);

                ASSERT(bytesInUse == oa.numBytesInUse());

                // Verify the type and the value of the object.
                ASSERT(true == X.isDatetime());
                ASSERTV(VALUE, VALUE == X.theDatetime());

                // Verify that the value of 'Z' has not changed.
                ASSERT(true  == Z.isDatetime());
                ASSERT(VALUE == Z.theDatetime());

                Datum::destroy(Z, &oa);

                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nTesting 'DatetimeInterval' data type." << endl;
        {
            const static bdlt::DatetimeInterval DATA[] = {
                DatetimeInterval(),
                DatetimeInterval(0, 0, 0, 0,  1),
                DatetimeInterval(0, 0, 0, 0, -1),
                DatetimeInterval(0, 0, 0,  1),
                DatetimeInterval(0, 0, 0, -1),
                DatetimeInterval(0, 0,  1),
                DatetimeInterval(0, 0, -1),
                DatetimeInterval(0,  1),
                DatetimeInterval(0, -1),
                DatetimeInterval( 1),
                DatetimeInterval(-1),
                DatetimeInterval(1, 1, 1, 1, 1),
                DatetimeInterval(-1, -1, -1, -1, -1),
                DatetimeInterval(1000, 12, 24),
                DatetimeInterval(100000, 1, 1, 32, 587),
                DatetimeInterval(-100000, 3, 2, 14, 319),
            };

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i< DATA_LEN; ++i) {
                const bdlt::DatetimeInterval VALUE = DATA[i];

                if (veryVerbose) { T_ P(VALUE) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                const Datum Z = Datum::createDatetimeInterval(VALUE, &oa);

                const Int64 bytesInUse = oa.numBytesInUse();

                ASSERT(true  == Z.isDatetimeInterval());
                ASSERT(VALUE == Z.theDatetimeInterval());

                const Datum X(Z);

                ASSERT(bytesInUse == oa.numBytesInUse());

                // Verify the type and the value of the object.
                ASSERT(true == X.isDatetimeInterval());
                ASSERTV(VALUE, VALUE == X.theDatetimeInterval());

                // Verify that the value of 'Z' has not changed.
                ASSERT(true  == Z.isDatetimeInterval());
                ASSERT(VALUE == Z.theDatetimeInterval());

                Datum::destroy(Z, &oa);

                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nTesting 'Decimal64' data type." << endl;
        {
            const static struct {
                int                d_line;          // line number
                bdldfp::Decimal64  d_value;         // 'Decimal64' value
                bool               d_compareEqual;  // does compare equal with
                                                    // itself
            } DATA[] = {
                //LINE VALUE                           EQUAL
                //---- ---------------------           -----
                { L_,  BDLDFP_DECIMAL_DD(0.0),         true  },
                { L_,  BDLDFP_DECIMAL_DD(0.253),       true  },
                { L_,  BDLDFP_DECIMAL_DD(-0.253),      true  },
                { L_,  BDLDFP_DECIMAL_DD(1.0),         true  },
                { L_,  BDLDFP_DECIMAL_DD(-1.0),        true  },
                { L_,  BDLDFP_DECIMAL_DD(12.345),      true  },
                { L_,  BDLDFP_DECIMAL_DD(12.3456789),  true  },
                { L_,  k_DECIMAL64_MIN,                true  },
                { L_,  k_DECIMAL64_MAX,                true  },
                { L_,  k_DECIMAL64_INFINITY,           true  },
                { L_,  k_DECIMAL64_NEG_INFINITY,       true  },
                { L_,  k_DECIMAL64_SNAN,               false },
                { L_,  k_DECIMAL64_QNAN,               false },
            };

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i< DATA_LEN; ++i) {
                const int               LINE  = DATA[i].d_line;
                const bdldfp::Decimal64 VALUE = DATA[i].d_value;
                const bool              EQUAL = DATA[i].d_compareEqual;

                if (veryVerbose) { T_ P_(LINE) P(VALUE) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                const Datum Z = Datum::createDecimal64(VALUE, &oa);

                const Int64 bytesInUse = oa.numBytesInUse();

                ASSERT(true == Z.isDecimal64());
                if (EQUAL) {
                    ASSERT(VALUE == Z.theDecimal64());
                } else {
                    ASSERT(VALUE != Z.theDecimal64());
                }

                const Datum X(Z);

                ASSERT(bytesInUse == oa.numBytesInUse());

                // Verify the type and the value of the object.
                ASSERT(true == X.isDecimal64());
                if (EQUAL) {
                    ASSERT(VALUE == X.theDecimal64());
                } else {
                    ASSERT(VALUE != X.theDecimal64());
                }

                // Verify that the value of 'Z' has not changed.
                ASSERT(true == Z.isDecimal64());
                if (EQUAL) {
                    ASSERT(VALUE == Z.theDecimal64());
                } else {
                    ASSERT(VALUE != Z.theDecimal64());
                }

                Datum::destroy(Z, &oa);

                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nTesting 'double' data type." << endl;
        {
            const static struct {
                int    d_line;          // line number
                double d_value;         // double value
                bool   d_compareEqual;  // does compare equal with itself
            } DATA[] = {
                //LINE VALUE                  EQUAL
                //---- ---------------------- -----
                { L_,  0.0,                   true  },
                { L_,  k_DOUBLE_NEG_ZERO,     true  },
                { L_,  .01,                   true  },
                { L_,  -.01,                  true  },
                { L_,  2.25e-117,             true  },
                { L_,  2.25e117,              true  },
                { L_,  1924.25,               true  },
                { L_,  -1924.25,              true  },
                { L_,  k_DOUBLE_MIN,          true  },
                { L_,  k_DOUBLE_MAX,          true  },
                { L_,  k_DOUBLE_INFINITY,     true  },
                { L_,  k_DOUBLE_NEG_INFINITY, true  },
                { L_,  k_DOUBLE_SNAN,         false },
                { L_,  k_DOUBLE_QNAN,         false },
                { L_,  k_DOUBLE_LOADED_NAN,   false },
            };

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i< DATA_LEN; ++i) {
                const int     LINE  = DATA[i].d_line;
                const double  VALUE = DATA[i].d_value;
                const bool    EQUAL = DATA[i].d_compareEqual;

                if (veryVerbose) { T_ P_(LINE) P(VALUE) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                const Datum Z = Datum::createDouble(VALUE);

                ASSERT(0 == oa.numBytesInUse()); // non allocating type

                ASSERT(true == Z.isDouble());
                if (EQUAL) {
                    ASSERT(VALUE == Z.theDouble());
                } else {
                    ASSERT(VALUE != Z.theDouble());
                    if (k_DOUBLE_NAN_BITS_PRESERVED) {
                        // In 64bit 'Datum' we currently store 'NaN' values
                        // unchanged.
                        const double THE_DOUBLE = Z.theDouble();
                        ASSERT(bsl::memcmp(&VALUE,
                                           &THE_DOUBLE,
                                           sizeof(double)) == 0);
                    }
                }

                const Datum X(Z);

                ASSERT(0 == oa.numBytesInUse()); // non allocating type

                // Verify the type and the value of the object.
                ASSERT(true == X.isDouble());
                if (EQUAL) {
                    ASSERT(VALUE == X.theDouble());
                } else {
                    ASSERT(VALUE != X.theDouble());
                }

                // Verify that the value of 'Z' has not changed.
                ASSERT(true == Z.isDouble());
                if (EQUAL) {
                    ASSERT(VALUE == Z.theDouble());
                } else {
                    ASSERT(VALUE != Z.theDouble());
                }

                Datum::destroy(Z, &oa);

                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nTesting 'DatumError' data type." << endl;
        {
            const static int DATA [] = {
                0,
                1,
                -1,
                numeric_limits<char>::min(),
                numeric_limits<char>::max(),
                numeric_limits<unsigned char>::max(),
                numeric_limits<short>::min(),
                numeric_limits<short>::max(),
                numeric_limits<unsigned short>::max(),
                numeric_limits<int>::min(),
                numeric_limits<int>::max()
            };

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            if (veryVerbose) cout << "\tTesting 'createError(int)'." << endl;
            for (size_t i = 0; i< DATA_LEN; ++i) {
                const int  ERROR = DATA[i];

                if (veryVerbose) { T_ T_ P(ERROR) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                const Datum Z = Datum::createError(ERROR);

                ASSERT(0 == oa.numBytesInUse()); // non allocating type

                ASSERT(true == Z.isError());
                ASSERTV(ERROR, DatumError(ERROR) == Z.theError());

                const Datum X(Z);

                ASSERT(0 == oa.numBytesInUse()); // non allocating type

                // Verify the type and the value of the object.
                ASSERT(true == X.isError());
                ASSERTV(ERROR, DatumError(ERROR) == X.theError());

                // Verify that the value of 'Z' has not changed.
                ASSERT(true == Z.isError());
                ASSERTV(ERROR, DatumError(ERROR) == Z.theError());

                Datum::destroy(Z, &oa);

                ASSERT(0 == oa.status());
            }

            if (veryVerbose)
                cout << "\tTesting 'createError(int, StringRef&)'." << endl;

            const char *errorMessage = "This is an error#$%\".";
            for (size_t i = 0; i< DATA_LEN; ++i) {
                for (size_t j = 0; j <= strlen(errorMessage); ++j) {
                    const int       ERROR = DATA[i];
                    const StringRef MESSAGE(errorMessage, static_cast<int>(j));

                    if (veryVerbose) { T_ T_ P_(ERROR) P(MESSAGE) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                    const Datum Z = Datum::createError(ERROR, MESSAGE, &oa);

                    const Int64 bytesInUse = oa.numBytesInUse();

                    ASSERT(true == Z.isError());
                    ASSERTV(ERROR, DatumError(ERROR, MESSAGE) == Z.theError());

                    const Datum X(Z);

                    ASSERT(bytesInUse == oa.numBytesInUse());

                    // Verify the type and the value of the object.
                    ASSERT(true == X.isError());
                    ASSERTV(ERROR, DatumError(ERROR, MESSAGE) == X.theError());

                    // Verify that the value of 'Z' has not changed.
                    ASSERT(true == Z.isError());
                    ASSERTV(ERROR, DatumError(ERROR, MESSAGE) == Z.theError());

                    Datum::destroy(Z, &oa);

                    ASSERT(0 == oa.status());
                }
            }
        }

        if (verbose) cout << "\nTesting 'Integer' data type." << endl;
        {
            const static int DATA[] = {
                0,
                1,
                -1,
                numeric_limits<char>::min(),
                numeric_limits<char>::max(),
                numeric_limits<unsigned char>::max(),
                numeric_limits<short>::min(),
                numeric_limits<short>::max(),
                numeric_limits<unsigned short>::max(),
                numeric_limits<int>::min(),
                numeric_limits<int>::max(),
            };
            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i< DATA_LEN; ++i) {
                const int VALUE = DATA[i];

                if (veryVerbose) { T_ P(VALUE) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                const Datum Z = Datum::createInteger(VALUE);

                ASSERT(0 == oa.numBytesInUse()); // non allocating type

                ASSERT(true == Z.isInteger());
                ASSERTV(VALUE, VALUE == Z.theInteger());

                const Datum X(Z);

                ASSERT(0 == oa.numBytesInUse()); // non allocating type

                // Verify the type and the value of the object.
                ASSERT(true == X.isInteger());
                ASSERTV(VALUE, VALUE == X.theInteger());

                // Verify that the value of 'Z' has not changed.
                ASSERT(true == Z.isInteger());
                ASSERTV(VALUE, VALUE == Z.theInteger());

                Datum::destroy(Z, &oa);

                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nTesting 'Integer64' data type." << endl;
        {
            const static Int64 DATA[] = {
                0,
                1,
                -1,
                numeric_limits<char>::min(),
                numeric_limits<char>::max(),
                numeric_limits<unsigned char>::max(),
                numeric_limits<short>::min(),
                numeric_limits<short>::max(),
                numeric_limits<unsigned short>::max(),
                numeric_limits<int>::min(),
                numeric_limits<int>::max(),
                numeric_limits<Int64>::min(),
                numeric_limits<Int64>::max(),
            };

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i< DATA_LEN; ++i) {
                const Int64 VALUE = DATA[i];

                if (veryVerbose) { T_ P(VALUE) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                const Datum Z = Datum::createInteger64(VALUE, &oa);

#ifdef BSLS_PLATFORM_CPU_32_BIT
                const Int64 bytesInUse = oa.numBytesInUse();
#else   // BSLS_PLATFORM_CPU_32_BIT
                ASSERT(0 == oa.numBytesInUse()); // non allocating type
#endif  // BSLS_PLATFORM_CPU_32_BIT

                ASSERT(true  == Z.isInteger64());
                ASSERTV(VALUE, VALUE == Z.theInteger64());

                const Datum X(Z);

#ifdef BSLS_PLATFORM_CPU_32_BIT
                ASSERT(bytesInUse == oa.numBytesInUse());
#else   // BSLS_PLATFORM_CPU_32_BIT
                ASSERT(0 == oa.numBytesInUse()); // non allocating type
#endif  // BSLS_PLATFORM_CPU_32_BIT

                // Verify the type and the value of the object.
                ASSERT(true == X.isInteger64());
                ASSERTV(VALUE, VALUE == X.theInteger64());

                // Verify that the value of 'Z' has not changed.
                ASSERT(true == Z.isInteger64());
                ASSERTV(VALUE, VALUE == Z.theInteger64());

                Datum::destroy(Z, &oa);

                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nTesting 'Null' data type." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            const Datum Z = Datum::createNull();

            ASSERT(0 == oa.numBytesInUse()); // non allocating type

            ASSERT(true == Z.isNull());

            const Datum X(Z);

            ASSERT(0 == oa.numBytesInUse()); // non allocating type

            // Verify the type and the value of the object.
            ASSERT(true == X.isNull());

            // Verify that the value of 'Z' has not changed.
            ASSERT(true == Z.isNull());

            Datum::destroy(Z, &oa);

            ASSERT(0 == oa.status());
        }

        if (verbose) cout << "\nTesting 'String' data type." << endl;
        {
            const static Datum::SizeType DATA[] = {
                0,
                1,
                32,
                255,
                256,
                65534,
                65535,
                65536,
                1024*1024,
            };

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            if (veryVerbose)
                cout << "\tTesting 'createStringRef(const char*, SizeType)'."
                     << endl;

            for (size_t i = 0; i< DATA_LEN; ++i) {
                bslma::TestAllocator  ba("buffer", veryVeryVeryVerbose);
                const Datum::SizeType SIZE = DATA[i];
                const bsl::string     BUFFER(SIZE, 'x', &ba);
                const StringRef       STRING(BUFFER.data(),
                                             static_cast<int>(SIZE));

                if (veryVerbose) { T_ T_ P(SIZE) }
                {
                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                    const Datum Z = Datum::createStringRef(BUFFER.data(),
                                                           SIZE, &oa);

                    const Int64 bytesInUse = oa.numBytesInUse();

                    ASSERT(true   == Z.isString());
                    ASSERT(STRING == Z.theString());

                    const Datum X(Z);

                    ASSERT(bytesInUse == oa.numBytesInUse());

                    // Verify the type and the value of the object.
                    ASSERT(true    == X.isString());
                    ASSERTV(STRING == X.theString());

                    // Verify that the value of 'Z' has not changed.
                    ASSERT(true   == Z.isString());
                    ASSERT(STRING == Z.theString());

                    Datum::destroy(Z, &oa);

                    ASSERT(0 == oa.status());
                }
            }
        }

        if (verbose) cout << "\nTesting 'Time' data type." << endl;
        {
            const static bdlt::Time DATA[] = {
                Time(),
                Time(0, 1, 1, 1, 1),
                Time(8, 0, 0, 999, 888),
                Time(23, 59, 59, 999, 999),
            };

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i< DATA_LEN; ++i) {
                const bdlt::Time VALUE = DATA[i];

                if (veryVerbose) { T_ P(VALUE) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                const Datum Z = Datum::createTime(VALUE);

                ASSERT(0 == oa.numBytesInUse()); // non allocating type

                ASSERT(true  == Z.isTime());
                ASSERT(VALUE == Z.theTime());

                const Datum X(Z);

                ASSERT(0 == oa.numBytesInUse()); // non allocating type

                // Verify the type and the value of the object.
                ASSERT(true  == X.isTime());
                ASSERT(VALUE == X.theTime());

                // Verify that the value of 'Z' has not changed.
                ASSERT(true  == Z.isTime());
                ASSERT(VALUE == Z.theTime());

                Datum::destroy(Z, &oa);

                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nTesting 'DatumUdt' data type." << endl;
        {
            char           BUFFER[] = "This is UDT placeholder";
            const DatumUdt VALUE(BUFFER, 0);

            if (veryVerbose) { T_ P(VALUE) }

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            const Datum Z = Datum::createUdt(BUFFER, 0);

            ASSERT(0 == oa.numBytesInUse()); // non allocating type

            ASSERT(true  == Z.isUdt());
            ASSERT(VALUE == Z.theUdt());

            const Datum X(Z);

            ASSERT(0 == oa.numBytesInUse()); // non allocating type

            // Verify the type and the value of the object.
            ASSERT(true  == X.isUdt());
            ASSERT(VALUE == X.theUdt());

            // Verify that the value of 'Z' has not changed.
            ASSERT(true  == Z.isUdt());
            ASSERT(VALUE == Z.theUdt());

            Datum::destroy(Z, &oa);

            ASSERT(0 == oa.status());
        }

        if (verbose) cout << "\nTesting 'DatumBinaryRef' data type." << endl;
        {
            for (size_t i = 0; i < 258; ++i) {
                bslma::TestAllocator       ba("buffer", veryVeryVeryVerbose);
                bsl::vector<unsigned char> BUFFER(&ba);
                const size_t               SIZE = i;
                loadRandomBinary(&BUFFER, SIZE);

                const DatumBinaryRef REF(BUFFER.data(), SIZE);

                if (veryVerbose) { T_ P_(SIZE) P(REF) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                const Datum Z = Datum::copyBinary(BUFFER.data(), SIZE, &oa);

                const Int64 bytesInUse = oa.numBytesInUse();
#ifdef BSLS_PLATFORM_CPU_64_BIT
                if (SIZE <= 13) {
                    ASSERT(0 == bytesInUse);
                } else {
                    ASSERT(0 != bytesInUse);
                }
#endif // BSLS_PLATFORM_CPU_64_BIT

                ASSERT(true == Z.isBinary());
                ASSERT(REF  == Z.theBinary());

                const Datum X(Z);

                ASSERT(bytesInUse == oa.numBytesInUse());

                // Verify the type and the value of the object.
                ASSERT(true == X.isBinary());
                ASSERT(REF  == X.theBinary());

                // Verify that the value of 'Z' has not changed.
                ASSERT(true == Z.isBinary());
                ASSERT(REF  == Z.theBinary());

                Datum::destroy(Z, &oa);

                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nTesting 'String' data type." << endl;
        {
            for (size_t i = 0; i < 258; ++i) {
                bslma::TestAllocator ba("buffer", veryVeryVeryVerbose);
                bsl::string          BUFFER(&ba);
                const size_t         SIZE = i;
                loadRandomString(&BUFFER, SIZE);

                const StringRef REF(BUFFER.data(), static_cast<int>(SIZE));

                if (veryVerbose) { T_ P_(SIZE) P(REF) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                const Datum Z = Datum::copyString(BUFFER.data(), SIZE, &oa);

                const Int64 bytesInUse = oa.numBytesInUse();
#ifdef BSLS_PLATFORM_CPU_32_BIT
                // Up to length 6 inclusive the strings are stored inline
                if (SIZE <= 6) {
                    ASSERT(0 == bytesInUse);
                } else {
                    ASSERT(0 != bytesInUse);
                }
#else  // BSLS_PLATFORM_CPU_32_BIT
                // Up to length 13 inclusive the strings are stored inline
                if (SIZE <= 13) {
                    ASSERT(0 == bytesInUse);
                } else {
                    ASSERT(0 != bytesInUse);
                }
#endif // BSLS_PLATFORM_CPU_32_BIT

                ASSERT(true == Z.isString());
                ASSERT(REF  == Z.theString());

                const Datum X(Z);

                ASSERT(bytesInUse == oa.numBytesInUse());

                // Verify the type and the value of the object.
                ASSERT(true == X.isString());
                ASSERT(REF  == X.theString());

                // Verify that the value of 'Z' has not changed.
                ASSERT(true == Z.isString());
                ASSERT(REF  == Z.theString());

                Datum::destroy(Z, &oa);

                ASSERT(0 == oa.status());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING PRINT AND OPERATOR<<
        //   Ensure that the value of the 'Datum' holding non-aggregate value
        //   can be formatted appropriately on an 'ostream' in some standard,
        //   human-readable form. Note, that 'Datum' implements custom printing
        //   only for the following non-aggregate types:
        //   o boolean
        //   o null
        //   o string
        //   For all other types, we only test that the print is dispatched to
        //   the right overload at the internal print visitor.
        //
        // Concerns:
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.
        //:
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)'.
        //:
        //: 4 The 'print' method's signature and return type are standard.
        //:
        //: 5 The 'print' method returns the supplied 'ostream'.
        //:
        //: 6 The optional 'level' and 'spacesPerLevel' parameters have the
        //:   correct default values (0 and 4, respectively).
        //:
        //: 7 The output 'operator<<'s signature and return type are standard.
        //:
        //: 8 The output 'operator<<' returns the destination 'ostream'.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-4, 7)
        //:
        //: 2 Using the table-driven technique:  (C-1..3, 5..6, 8)
        //:
        //:   1 Define combinations of object values, having distinct values
        //      and various values for the two formatting parameters, along
        //      with the expected output.
        //:
        //:     ( 'value' x  'level'   x 'spacesPerLevel' ):
        //:     1 { A   } x {  0     } x {  0, 1, -1, -8 } --> 4 expected o/ps
        //:     2 { A   } x {  3, -3 } x {  0, 2, -2, -8 } --> 8 expected o/ps
        //:     3 { A   } x {  2     } x {  3            } --> 1 expected o/ps
        //:     4 { A, B} x { -8     } x { -8            } --> 2 expected o/ps
        //:     5 { A, B} x { -9     } x { -9            } --> 2 expected o/ps
        //:
        //:   2 For each row in the table defined in P-2.1:  (C-1..3, 5..6, 8)
        //:
        //:     1 Using a 'const' 'Datum', supply each object value and pair of
        //:       formatting parameters to 'print', omitting the 'level' or
        //:       'spacesPerLevel' parameter if the value of that argument is
        //:       '-8'.  If the parameters are, arbitrarily, '(-9, -9)', then
        //:       invoke the 'operator<<' instead.
        //:
        //:     2 Use a standard 'ostringstream' to capture the actual output.
        //:
        //:     3 Verify the address of what is returned is that of the
        //:       supplied stream.  (C-5, 8)
        //:
        //:     4 Compare the contents captured in P-2.2.2 with what is
        //:       expected.  (C-1..3, 6)
        //
        // Testing:
        //   bsl::ostream& print(ostream&, int, int) const; // non-aggregate
        //   bsl::ostream& operator<<(ostream&, const Datum&); // non-aggregate
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PRINT AND OPERATOR<<" << endl
                          << "============================" << endl;

        if (verbose)
            cout << "\nAssign the addresses of 'print' and "
                 << "the output 'operator<<' to variables." << endl;
        {
            typedef ostream& (Datum::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Datum&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Datum::print;
            operatorPtr operatorOut = bdld::operator<<;

            (void) printMember;  // quash potential compiler warnings
            (void) operatorOut;
        }

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations." << endl;

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        static const struct {
            int         d_line;            // source line number
            int         d_level;           // number of indentation levels
            int         d_spacesPerLevel;  // spaces per indentation level
            const Datum d_value;           // datum
            const char *d_expected_p;      // expected output or 0
        } DATA[] = {
#define NL  "\n"
//LINE L   SPL VALUE                                EXP
//---- --- --- ---------------------------------    ------------------------
{ L_,   0,  0, Datum::createBoolean(true),          "true"                NL },
{ L_,   0,  1, Datum::createBoolean(true),          "true"                NL },
{ L_,   0, -1, Datum::createBoolean(true),          "true"                   },
{ L_,   0, -8, Datum::createBoolean(true),          "true"                NL },
{ L_,   3,  0, Datum::createBoolean(true),          "true"                NL },
{ L_,   3,  2, Datum::createBoolean(true),          "      true"          NL },
{ L_,   3, -2, Datum::createBoolean(true),          "      true"             },
{ L_,   3, -8, Datum::createBoolean(true),          "            true"    NL },
{ L_,  -3,  0, Datum::createBoolean(true),          "true"                NL },
{ L_,  -3,  2, Datum::createBoolean(true),          "true"                NL },
{ L_,  -3, -2, Datum::createBoolean(true),          "true"                   },
{ L_,  -3, -8, Datum::createBoolean(true),          "true"                NL },
{ L_,   2,  3, Datum::createBoolean(false),         "      false"         NL },
{ L_,  -8, -8, Datum::createBoolean(true),          "true"                NL },
{ L_,  -8, -8, Datum::createBoolean(false),         "false"               NL },
{ L_,  -9, -9, Datum::createBoolean(true),          "true"                   },
{ L_,  -9, -9, Datum::createBoolean(false),         "false"                  },
{ L_,   0,  0, Datum::createNull(),                 "nil"                 NL },
{ L_,   0,  1, Datum::createNull(),                 "nil"                 NL },
{ L_,   0, -1, Datum::createNull(),                 "nil"                    },
{ L_,   0, -8, Datum::createNull(),                 "nil"                 NL },
{ L_,   3,  0, Datum::createNull(),                 "nil"                 NL },
{ L_,   3,  2, Datum::createNull(),                 "      nil"           NL },
{ L_,   3, -2, Datum::createNull(),                 "      nil"              },
{ L_,   3, -8, Datum::createNull(),                 "            nil"     NL },
{ L_,  -3,  0, Datum::createNull(),                 "nil"                 NL },
{ L_,  -3,  2, Datum::createNull(),                 "nil"                 NL },
{ L_,  -3, -2, Datum::createNull(),                 "nil"                    },
{ L_,  -3, -8, Datum::createNull(),                 "nil"                 NL },
{ L_,  -8, -8, Datum::createNull(),                 "nil"                 NL },
{ L_,  -9, -9, Datum::createNull(),                 "nil"                    },
// Mainly testing that the quotes are placed correctly.
{ L_,   0,  0, Datum::createStringRef("", 0, &oa),  "\"\""                NL },
{ L_,   0,  1, Datum::createStringRef("", 0, &oa),  "\"\""                NL },
{ L_,   0, -1, Datum::createStringRef("", 0, &oa),  "\"\""                   },
{ L_,   0, -8, Datum::createStringRef("", 0, &oa),  "\"\""                NL },
{ L_,   3,  0, Datum::createStringRef("", 0, &oa),  "\"\""                NL },
{ L_,   3,  2, Datum::createStringRef("", 0, &oa),  "      \"\""          NL },
{ L_,   3, -2, Datum::createStringRef("", 0, &oa),  "      \"\""             },
{ L_,   3, -8, Datum::createStringRef("", 0, &oa),  "            \"\""    NL },
{ L_,  -3,  0, Datum::createStringRef("", 0, &oa),  "\"\""                NL },
{ L_,  -3,  2, Datum::createStringRef("", 0, &oa),  "\"\""                NL },
{ L_,  -3, -2, Datum::createStringRef("", 0, &oa),  "\"\""                   },
{ L_,  -3, -8, Datum::createStringRef("", 0, &oa),  "\"\""                NL },
{ L_,  -8, -8, Datum::createStringRef("", 0, &oa),  "\"\""                NL },
{ L_,  -9, -9, Datum::createStringRef("", 0, &oa),  "\"\""                   },
{ L_,   0,  0, Datum::createStringRef("a", 1, &oa), "\"a\""               NL },
{ L_,   0,  1, Datum::createStringRef("a", 1, &oa), "\"a\""               NL },
{ L_,   0, -1, Datum::createStringRef("a", 1, &oa), "\"a\""                  },
{ L_,   0, -8, Datum::createStringRef("a", 1, &oa), "\"a\""               NL },
{ L_,   3,  0, Datum::createStringRef("a", 1, &oa), "\"a\""               NL },
{ L_,   3,  2, Datum::createStringRef("a", 1, &oa), "      \"a\""         NL },
{ L_,   3, -2, Datum::createStringRef("a", 1, &oa), "      \"a\""            },
{ L_,   3, -8, Datum::createStringRef("a", 1, &oa), "            \"a\""   NL },
{ L_,  -3,  0, Datum::createStringRef("a", 1, &oa), "\"a\""               NL },
{ L_,  -3,  2, Datum::createStringRef("a", 1, &oa), "\"a\""               NL },
{ L_,  -3, -2, Datum::createStringRef("a", 1, &oa), "\"a\""                  },
{ L_,  -3, -8, Datum::createStringRef("a", 1, &oa), "\"a\""               NL },
{ L_,  -8, -8, Datum::createStringRef("a", 1, &oa), "\"a\""               NL },
{ L_,  -9, -9, Datum::createStringRef("a", 1, &oa), "\"a\""                  },
// Verify that all non-aggregate data types are correctly dispatched by the
// stream visitor.
{ L_,   0,  0, Datum::createDate(bdlt::Date()),     "01JAN0001"           NL },
{ L_,  -8, -8, Datum::createDate(bdlt::Date()),     "01JAN0001"           NL },
{ L_,  -9, -9, Datum::createDate(bdlt::Date()),     "01JAN0001"              },
{ L_,   0,  0, Datum::createDatetime(bdlt::Datetime(), &oa),
                                              "01JAN0001_24:00:00.000000" NL },
{ L_,  -8, -8, Datum::createDatetime(bdlt::Datetime(), &oa),
                                              "01JAN0001_24:00:00.000000" NL },
{ L_,  -9, -9, Datum::createDatetime(bdlt::Datetime(), &oa),
                                                 "01JAN0001_24:00:00.000000" },
{ L_,   0,  0, Datum::createDatetimeInterval(bdlt::DatetimeInterval(), &oa),
                                                    "+0_00:00:00.000000"  NL },
{ L_,  -8, -8, Datum::createDatetimeInterval(bdlt::DatetimeInterval(), &oa),
                                                    "+0_00:00:00.000000"  NL },
{ L_,  -9, -9, Datum::createDatetimeInterval(bdlt::DatetimeInterval(), &oa),
                                                    "+0_00:00:00.000000"     },
{ L_,   0,  0, Datum::createDecimal64(BDLDFP_DECIMAL_DD(12.345), &oa),
                                                    "12.345"              NL },
{ L_,  -8, -8, Datum::createDecimal64(BDLDFP_DECIMAL_DD(12.345), &oa),
                                                    "12.345"              NL },
{ L_,  -9, -9, Datum::createDecimal64(BDLDFP_DECIMAL_DD(12.345), &oa),
                                                    "12.345"                 },
{ L_,   0,  0, Datum::createDouble(0.25),           "0.25"                NL },
{ L_,   0,  0, Datum::createError(3),               "error(3)"            NL },
{ L_,   0,  0, Datum::createError(-1, "msg", &oa),  "error(-1,'msg')"     NL },
{ L_,   0,  1, Datum::createError(-1, "msg", &oa),  "error(-1,'msg')"     NL },
{ L_,   0, -1, Datum::createError(-1, "msg", &oa),  "error(-1,'msg')"        },
{ L_,   0, -8, Datum::createError(-1, "msg", &oa),  "error(-1,'msg')"     NL },
{ L_,   3,  0, Datum::createError(-1, "msg", &oa),  "error(-1,'msg')"     NL },
{ L_,   3,  2, Datum::createError(-1, "msg", &oa),
                                                  "      error(-1,'msg')" NL },
{ L_,   3, -2, Datum::createError(-1, "msg", &oa),
                                                  "      error(-1,'msg')"    },
{ L_,   3, -8, Datum::createError(-1, "msg", &oa),
                                            "            error(-1,'msg')" NL },
{ L_,  -3,  0, Datum::createError(-1, "msg", &oa),  "error(-1,'msg')"     NL },
{ L_,  -3,  2, Datum::createError(-1, "msg", &oa),  "error(-1,'msg')"     NL },
{ L_,  -3, -2, Datum::createError(-1, "msg", &oa),  "error(-1,'msg')"        },
{ L_,  -3, -8, Datum::createError(-1, "msg", &oa),  "error(-1,'msg')"     NL },
{ L_,  -8, -8, Datum::createError(-1, "msg", &oa),  "error(-1,'msg')"     NL },
{ L_,  -9, -9, Datum::createError(-1, "msg", &oa),  "error(-1,'msg')"        },
{ L_,   0,  0, Datum::createInteger(-18),           "-18"                 NL },
{ L_,   0,  0, Datum::createInteger64(987654, &oa), "987654"              NL },
{ L_,   0,  0, Datum::createTime(bdlt::Time()),     "24:00:00.000000"     NL },
{ L_,  -9, -9, Datum::createUdt(reinterpret_cast<void *>(0x1), 12),  0       },
// These values have platform-dependent representation
{ L_,  -9, -9, Datum::createDouble(k_DOUBLE_INFINITY),                0      },
{ L_,  -9, -9, Datum::createDouble(k_DOUBLE_NEG_INFINITY),            0      },
{ L_,  -9, -9, Datum::createDouble(k_DOUBLE_SNAN),                    0      },
{ L_,  -9, -9, Datum::createDouble(k_DOUBLE_QNAN),                    0      },
{ L_,  -9, -9, Datum::createDouble(k_DOUBLE_LOADED_NAN),              0      },
{ L_,  -9, -9, Datum::createDecimal64(k_DECIMAL64_INFINITY, &oa),     0      },
{ L_,  -9, -9, Datum::createDecimal64(k_DECIMAL64_NEG_INFINITY, &oa), 0      },
{ L_,  -9, -9, Datum::createDecimal64(k_DECIMAL64_SNAN, &oa),         0      },
{ L_,  -9, -9, Datum::createDecimal64(k_DECIMAL64_QNAN, &oa),         0      },
#undef NL
        };

        const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose)
            cout << "\nTesting with various print specifications." << endl;
        {
            for (size_t i = 0; i < NUM_DATA; ++i) {
                const int         LINE  = DATA[i].d_line;
                const int         L     = DATA[i].d_level;
                const int         SPL   = DATA[i].d_spacesPerLevel;
                const Datum&      VALUE = DATA[i].d_value;
                const char *const EXP   = DATA[i].d_expected_p;

                if (veryVerbose) {
                    T_ P_(LINE) P_(L) P(SPL)
                }

                if (veryVeryVerbose) {
                    if (EXP) { T_ T_ Q(EXPECTED) cout << EXP; }
                }

                bslma::TestAllocator sa("stream", veryVeryVeryVerbose);

                ostringstream os(&sa);

                // Verify supplied stream is returned by reference.

                if (-9 == L && -9 == SPL) {
                    ASSERTV(LINE, &os == &(os << VALUE));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {
                    ASSERTV(LINE, -8 == SPL || -8 != L);

                    if (-8 != SPL) {
                        ASSERTV(LINE, &os == &VALUE.print(os, L, SPL));
                    }
                    else if (-8 != L) {
                        ASSERTV(LINE, &os == &VALUE.print(os, L));
                    }
                    else {
                        ASSERTV(LINE, &os == &VALUE.print(os));
                    }
                    if (veryVeryVerbose) { T_ T_ Q(print) }
                }

                {
                    bslma::TestAllocator         da("default",
                                                    veryVeryVeryVerbose);
                    bslma::DefaultAllocatorGuard dag(&da);

                    // Verify output is formatted as expected.
                    if (veryVeryVerbose) { cout << os.str() << endl; }

                    if (EXP) {
                        ASSERTV(LINE, EXP, os.str(), EXP == os.str());
                    } else {
                        // The value has a platform-dependent representation.
                        // For those values we test operator<< only;
                        ASSERTV(LINE, (-9 == L) && (-9 == SPL));
                        ostringstream expected;

                        if (VALUE.isDouble()) {
                            expected << VALUE.theDouble();
                            ASSERTV(LINE,
                                    expected.str(),
                                    os.str(),
                                    expected.str() == os.str());
                        } else if (VALUE.isDecimal64()) {
                            expected << VALUE.theDecimal64();
                            ASSERTV(LINE,
                                    expected.str(),
                                    os.str(),
                                    expected.str() == os.str());
                        } else if (VALUE.isUdt()) {
                            expected << "user-defined("
                                     << VALUE.theUdt().data()
                                     << ","
                                     << VALUE.theUdt().type();
                        } else {
                            ASSERTV(LINE, 0);
                        }
                    }
                }
            }

            for (size_t i = 0; i < NUM_DATA; ++i) {
                if (veryVerbose) {
                    T_  P_(DATA[i].d_value) P(i)
                }

                Datum::destroy(DATA[i].d_value, &oa);
            }
        }

        if (verbose) cout <<
                  "\nTesting 'binary' data type print specifications." << endl;
        {
            const unsigned char buffer[] = "12345";
            const size_t        binarySize = sizeof(buffer);

            const Datum D = Datum::copyBinary(buffer, binarySize, &oa);

            bslma::TestAllocator sa("stream", veryVeryVeryVerbose);

            ostringstream expected(&sa);

            expected << "[ binary = "
                     << bsl::hex << bsl::showbase
                     << reinterpret_cast<bsls::Types::UintPtr>(
                                                          D.theBinary().data())
                     << bsl::dec
                     << " size = " << binarySize
                     << " ]";

            if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << expected.str(); }

            ostringstream os(&sa);
            os << D;

            if (veryVeryVerbose) { T_ T_ Q(operator<<) cout << os.str(); }

            ASSERTV(expected.str(), os.str(), expected.str() == os.str());

            Datum::destroy(D, &oa);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATOR AND BASIC ACCESSORS
        //   This test will verify that the primary manipulators are working as
        //   expected for 'Datum' holding non-aggregate data types. As basic
        //   accessors should be tested exactly the same way, these two tests
        //   have been united.
        //
        // Concerns:
        //: 1 All creators create 'Datum' object of the correct type and value.
        //:
        //: 2 The 'Datum' object can hold any value valid for a given type.
        //:
        //: 3 All object memory comes from the supplied allocator.
        //:
        //: 4 All allocated memory is released when the 'Datum' object is
        //:   destroyed.
        //:
        //: 5 All ACCESSORS methods are declared 'const'.
        //:
        //: 6 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create 'Datum' objects for each supported non-aggregate data
        //:   type.  Using table-based technique, iterate through the set of
        //:   representative values for a given data type.  Verify the type and
        //:   value of the created 'Datum' object.  (C-1,2)
        //:
        //: 2 Verify that all object memory is released when the object is
        //:   destroyed.  (C-4)
        //:
        //: 3 Invoke each ACCESSOR via a reference providing non-modifiable
        //:   access to the object.  Compilation of this test driver confirms
        //:   that ACCESSORS are 'const'-qualified.  (C-5)
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones.  (C-6)
        //
        // Testing:
        //   Datum() = default;
        //   ~Datum() = default;
        //   Datum createBoolean(bool);
        //   Datum createDate(const bdlt::Date&);
        //   Datum createDatetime(const bdlt::Datetime&, bslma::Allocator *);
        //   Datum createDatetimeInterval(const DatetimeInterval&, Allocator*);
        //   Datum createDecimal64(Decimal);
        //   Datum createDouble(double);
        //   Datum createError(int);
        //   Datum createError(int, const StringRef&, bslma::Allocator *);
        //   Datum createInteger(int);
        //   Datum createInteger64(bsls::Types::Int64, bslma::Allocator *);
        //   Datum createNull();
        //   Datum createStringRef(const char *, bslma::Allocator *);
        //   Datum createStringRef(const char *, SizeType, bslma::Allocator *);
        //   Datum createStringRef(const StringRef&, bslma::Allocator *);
        //   Datum createTime(const bdlt::Time&);
        //   Datum createUdt(void *data, int type);
        //   Datum copyBinary(void *, int);
        //   Datum copyString(const char *, bslma::Allocator *);
        //   Datum copyString(const char *, SizeType, bslma::Allocator *);
        //   Datum copyString(const bslstl::StringRef&, bslma::Allocator *);
        //   void destroy(const Datum&, bslma::Allocator *);
        //   bool isBoolean() const;
        //   bool isBinary() const;
        //   bool isDate() const;
        //   bool isDatetime() const;
        //   bool isDatetimeInterval() const;
        //   bool isDecimal64() const;
        //   bool isDouble() const;
        //   bool isError() const;
        //   bool isInteger() const;
        //   bool isInteger64() const;
        //   bool isNull() const;
        //   bool isString() const;
        //   bool isTime() const;
        //   bool isUdt() const;
        //   bool theBoolean() const;
        //   DatumBinaryRef theBinary() const;
        //   bdlt::Date theDate() const;
        //   bdlt::Datetime theDatetime() const;
        //   bdlt::DatetimeInterval theDatetimeInterval() const;
        //   bdldfp::Decimal64 theDecimal64() const;
        //   double theDouble() const;
        //   DatumError theError() const;
        //   int theInteger() const;
        //   bsls::Types::Int64 theInteger64() const;
        //   bslstl::StringRef theString() const;
        //   bdlt::Time theTime() const;
        //   DatumUdt theUdt() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "PRIMARY MANIPULATOR AND BASIC ACCESSORS" << endl
                          << "=======================================" << endl;

        if (verbose) cout << "\nTesting non-aggregate data types.\n" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);

        if (verbose) cout << "\nTesting 'createBoolean'." << endl;
        {
            const static bool DATA[]   = { true, false };
            const size_t      DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i< DATA_LEN; ++i) {
                const bool VALUE = DATA[i];

                if (veryVerbose) { T_ P(VALUE) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Datum        mD = Datum::createBoolean(VALUE);
                const Datum& D = mD;

                ASSERT(0 == oa.numBlocksInUse()); // non allocating type

                ASSERT(!D.isArray());
                ASSERT(D.isBoolean());              // *
                ASSERT(!D.isBinary());
                ASSERT(!D.isDate());
                ASSERT(!D.isDatetime());
                ASSERT(!D.isDatetimeInterval());
                ASSERT(!D.isDecimal64());
                ASSERT(!D.isDouble());
                ASSERT(!D.isError());
                ASSERT(!D.isInteger());
                ASSERT(!D.isInteger64());
                ASSERT(!D.isIntMap());
                ASSERT(!D.isMap());
                ASSERT(!D.isNull());
                ASSERT(!D.isString());
                ASSERT(!D.isTime());
                ASSERT(!D.isUdt());
                ASSERT(VALUE == D.theBoolean());

                Datum::destroy(D, &oa);

                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nTesting 'createDate'." << endl;
        {
            const static bdlt::Date DATA[] = {
                Date(),
                Date(1999, 12, 31),
                Date(2015,  1,  1),
                Date(2200,  8, 12),
                Date(9999, 12, 31),
            };

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i< DATA_LEN; ++i) {
                const bdlt::Date VALUE = DATA[i];

                if (veryVerbose) { T_ P(VALUE) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Datum        mD = Datum::createDate(VALUE);
                const Datum& D = mD;

                ASSERT(0 == oa.numBlocksInUse()); // non allocating type

                ASSERT(!D.isArray());
                ASSERT(!D.isBoolean());
                ASSERT(!D.isBinary());
                ASSERT(D.isDate());                 // *
                ASSERT(!D.isDatetime());
                ASSERT(!D.isDatetimeInterval());
                ASSERT(!D.isDecimal64());
                ASSERT(!D.isDouble());
                ASSERT(!D.isError());
                ASSERT(!D.isInteger());
                ASSERT(!D.isInteger64());
                ASSERT(!D.isIntMap());
                ASSERT(!D.isMap());
                ASSERT(!D.isNull());
                ASSERT(!D.isString());
                ASSERT(!D.isTime());
                ASSERT(!D.isUdt());
                ASSERT(VALUE == D.theDate());

                Datum::destroy(D, &oa);

                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nTesting 'createDatetime'." << endl;
        {
            const static bdlt::Datetime DATA[] = {
                Datetime(),
                Datetime(1999, 12, 31, 12, 45, 31,  18, 123),
                Datetime(2015,  2,  2,  1,  1,  1,   1, 1),
                Datetime(2200,  9, 11, 18, 10, 59, 458, 452),
                Datetime(9999,  9,  9,  9,  9,  9, 999, 999),
                Datetime(1999, 12, 31, 12, 45, 31,  18),
                Datetime(2015,  2,  2,  1,  1,  1,   1),
                Datetime(2200,  9, 11, 18, 10, 59, 458),
                Datetime(9999,  9,  9,  9,  9,  9, 999),
            };

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i< DATA_LEN; ++i) {
                const bdlt::Datetime VALUE = DATA[i];

                if (veryVerbose) { T_ P(VALUE) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Datum        mD = Datum::createDatetime(VALUE, &oa);
                const Datum& D = mD;

                ASSERT(!D.isArray());
                ASSERT(!D.isBoolean());
                ASSERT(!D.isBinary());
                ASSERT(!D.isDate());
                ASSERT(D.isDatetime());             // *
                ASSERT(!D.isDatetimeInterval());
                ASSERT(!D.isDecimal64());
                ASSERT(!D.isDouble());
                ASSERT(!D.isError());
                ASSERT(!D.isInteger());
                ASSERT(!D.isInteger64());
                ASSERT(!D.isIntMap());
                ASSERT(!D.isMap());
                ASSERT(!D.isNull());
                ASSERT(!D.isString());
                ASSERT(!D.isTime());
                ASSERT(!D.isUdt());
                ASSERT(VALUE == D.theDatetime());

                Datum::destroy(D, &oa);

                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nTesting 'createDatetimeInterval'." << endl;
        {
            const static bdlt::DatetimeInterval DATA[] = {
                DatetimeInterval(),
                DatetimeInterval(0, 0, 0, 0,  1),
                DatetimeInterval(0, 0, 0, 0, -1),
                DatetimeInterval(0, 0, 0,  1),
                DatetimeInterval(0, 0, 0, -1),
                DatetimeInterval(0, 0,  1),
                DatetimeInterval(0, 0, -1),
                DatetimeInterval(0,  1),
                DatetimeInterval(0, -1),
                DatetimeInterval( 1),
                DatetimeInterval(-1),
                DatetimeInterval(1, 1, 1, 1, 1),
                DatetimeInterval(-1, -1, -1, -1, -1),
                DatetimeInterval(1000, 12, 24),
                DatetimeInterval(100000000, 1, 1, 32, 587),
                DatetimeInterval(-100000000, 3, 2, 14, 319),
            };

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i< DATA_LEN; ++i) {
                const bdlt::DatetimeInterval VALUE = DATA[i];

                if (veryVerbose) { T_ P(VALUE) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Datum        mD = Datum::createDatetimeInterval(VALUE, &oa);
                const Datum& D = mD;

                ASSERT(!D.isArray());
                ASSERT(!D.isBoolean());
                ASSERT(!D.isBinary());
                ASSERT(!D.isDate());
                ASSERT(!D.isDatetime());
                ASSERT(D.isDatetimeInterval());     // *
                ASSERT(!D.isDecimal64());
                ASSERT(!D.isDouble());
                ASSERT(!D.isError());
                ASSERT(!D.isInteger());
                ASSERT(!D.isInteger64());
                ASSERT(!D.isIntMap());
                ASSERT(!D.isMap());
                ASSERT(!D.isNull());
                ASSERT(!D.isString());
                ASSERT(!D.isTime());
                ASSERT(!D.isUdt());
                ASSERT(VALUE == D.theDatetimeInterval());

                Datum::destroy(D, &oa);

                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nTesting 'createDecimal64'." << endl;
        {
            const static struct {
                int                d_line;          // line number
                bdldfp::Decimal64  d_value;         // 'Decimal64' value
                bool               d_compareEqual;  // does compare equal with
                                                    // itself
            } DATA[] = {
                //LINE VALUE                           EQUAL
                //---- ---------------------           -----
                { L_,  BDLDFP_DECIMAL_DD(0.0),         true  },
                { L_,  BDLDFP_DECIMAL_DD(0.253),       true  },
                { L_,  BDLDFP_DECIMAL_DD(-0.253),      true  },
                { L_,  BDLDFP_DECIMAL_DD(1.0),         true  },
                { L_,  BDLDFP_DECIMAL_DD(-1.0),        true  },
                { L_,  BDLDFP_DECIMAL_DD(12.345),      true  },
                { L_,  BDLDFP_DECIMAL_DD(12.3456789),  true  },
                { L_,  k_DECIMAL64_MIN,                true  },
                { L_,  k_DECIMAL64_MAX,                true  },
                { L_,  k_DECIMAL64_INFINITY,           true  },
                { L_,  k_DECIMAL64_NEG_INFINITY,       true  },
                { L_,  k_DECIMAL64_SNAN,               false },
                { L_,  k_DECIMAL64_QNAN,               false },
            };
            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i< DATA_LEN; ++i) {
                const int               LINE  = DATA[i].d_line;
                const bdldfp::Decimal64 VALUE = DATA[i].d_value;
                const bool              EQUAL = DATA[i].d_compareEqual;

                if (veryVerbose) { T_ P_(LINE) P(VALUE) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Datum        mD = Datum::createDecimal64(VALUE, &oa);
                const Datum& D = mD;

                ASSERT(!D.isArray());
                ASSERT(!D.isBoolean());
                ASSERT(!D.isBinary());
                ASSERT(!D.isDate());
                ASSERT(!D.isDatetime());
                ASSERT(!D.isDatetimeInterval());
                ASSERT(D.isDecimal64());            // *
                ASSERT(!D.isDouble());
                ASSERT(!D.isError());
                ASSERT(!D.isInteger());
                ASSERT(!D.isInteger64());
                ASSERT(!D.isIntMap());
                ASSERT(!D.isMap());
                ASSERT(!D.isNull());
                ASSERT(!D.isString());
                ASSERT(!D.isTime());
                ASSERT(!D.isUdt());
                if (EQUAL) {
                    ASSERT(VALUE == D.theDecimal64());
                } else {
                    ASSERT(VALUE != D.theDecimal64());
                }

                Datum::destroy(D, &oa);

                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nTesting 'createDouble'." << endl;
        {
            const static struct {
                int      d_line;          // line number
                double   d_value;         // double value
                bool     d_compareEqual;  // does compare equal with itself
            } DATA[] = {
                //LINE VALUE                  EQUAL
                //---- ---------------------- -----
                { L_,  0.0,                   true  },
                { L_,  k_DOUBLE_NEG_ZERO,     true  },
                { L_,  .01,                   true  },
                { L_,  -.01,                  true  },
                { L_,  2.25e-117,             true  },
                { L_,  2.25e117,              true  },
                { L_,  1924.25,               true  },
                { L_,  -1924.25,              true  },
                { L_,  k_DOUBLE_MIN,          true  },
                { L_,  k_DOUBLE_MAX,          true  },
                { L_,  k_DOUBLE_INFINITY,     true  },
                { L_,  k_DOUBLE_NEG_INFINITY, true  },
                { L_,  k_DOUBLE_SNAN,         false },
                { L_,  k_DOUBLE_QNAN,         false },
                { L_,  k_DOUBLE_LOADED_NAN,   false },
            };
            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i< DATA_LEN; ++i) {
                const int     LINE  = DATA[i].d_line;
                const double  VALUE = DATA[i].d_value;
                const bool    EQUAL = DATA[i].d_compareEqual;

                if (veryVerbose) { T_ P_(LINE) P(VALUE) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Datum        mD = Datum::createDouble(VALUE);
                const Datum& D = mD;

                ASSERT(0 == oa.numBlocksInUse()); // non allocating type
                ASSERT(!D.isArray());
                ASSERT(!D.isBoolean());
                ASSERT(!D.isBinary());
                ASSERT(!D.isDate());
                ASSERT(!D.isDatetime());
                ASSERT(!D.isDatetimeInterval());
                ASSERT(!D.isDecimal64());
                ASSERT(D.isDouble());               // *
                ASSERT(!D.isError());
                ASSERT(!D.isInteger());
                ASSERT(!D.isInteger64());
                ASSERT(!D.isIntMap());
                ASSERT(!D.isMap());
                ASSERT(!D.isNull());
                ASSERT(!D.isString());
                ASSERT(!D.isTime());
                ASSERT(!D.isUdt());
                if (EQUAL) {
                    ASSERT(VALUE == D.theDouble());
                } else {
                    ASSERT(VALUE != D.theDouble());
                    // In 32 bit mode we guarantee just "some NaN value".
                    if (k_DOUBLE_NAN_BITS_PRESERVED) {
                        // In 64 bit mode 'Datum' we currently store 'NaN'
                        // values unchanged.
                        const double THE_DOUBLE = D.theDouble();
                        ASSERT(bsl::memcmp(&VALUE,
                                           &THE_DOUBLE,
                                           sizeof(double)) == 0);
                    }
                }

                Datum::destroy(D, &oa);

                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nTesting 'createError'." << endl;
        {
            const static int DATA [] = {
                0,
                1,
                -1,
                numeric_limits<char>::min(),
                numeric_limits<char>::max(),
                numeric_limits<unsigned char>::max(),
                numeric_limits<short>::min(),
                numeric_limits<short>::max(),
                numeric_limits<unsigned short>::max(),
                numeric_limits<int>::min(),
                numeric_limits<int>::max()
            };

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            if (veryVerbose) cout << "\tTesting 'createError(int)'." << endl;
            for (size_t i = 0; i< DATA_LEN; ++i) {
                const int  ERROR = DATA[i];

                if (veryVerbose) { T_ T_ P(ERROR) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Datum        mD = Datum::createError(ERROR);
                const Datum& D = mD;

                ASSERT(0 == oa.numBlocksInUse()); // non allocating type

                ASSERT(!D.isArray());
                ASSERT(!D.isBoolean());
                ASSERT(!D.isBinary());
                ASSERT(!D.isDate());
                ASSERT(!D.isDatetime());
                ASSERT(!D.isDatetimeInterval());
                ASSERT(!D.isDecimal64());
                ASSERT(!D.isDouble());
                ASSERT(D.isError());                // *
                ASSERT(!D.isInteger());
                ASSERT(!D.isInteger64());
                ASSERT(!D.isIntMap());
                ASSERT(!D.isMap());
                ASSERT(!D.isNull());
                ASSERT(!D.isString());
                ASSERT(!D.isTime());
                ASSERT(!D.isUdt());
                ASSERT(DatumError(ERROR) == D.theError());

                Datum::destroy(D, &oa);

                ASSERT(0 == oa.status());
            }

            if (veryVerbose)
                cout << "\tTesting 'createError(int, StringRef&)'." << endl;

            const char *errorMessage = "This is an error#$%\".";
            for (size_t i = 0; i< DATA_LEN; ++i) {
                for (size_t j = 0; j <= strlen(errorMessage); ++j) {
                    const int       ERROR = DATA[i];
                    const StringRef MESSAGE(errorMessage, static_cast<int>(j));

                    if (veryVerbose) { T_ T_ P_(ERROR) P(MESSAGE) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                    Datum        mD = Datum::createError(ERROR, MESSAGE, &oa);
                    const Datum& D = mD;

                    ASSERT(!D.isArray());
                    ASSERT(!D.isBoolean());
                    ASSERT(!D.isBinary());
                    ASSERT(!D.isDate());
                    ASSERT(!D.isDatetime());
                    ASSERT(!D.isDatetimeInterval());
                    ASSERT(!D.isDecimal64());
                    ASSERT(!D.isDouble());
                    ASSERT(D.isError());                // *
                    ASSERT(!D.isInteger());
                    ASSERT(!D.isInteger64());
                    ASSERT(!D.isIntMap());
                    ASSERT(!D.isMap());
                    ASSERT(!D.isNull());
                    ASSERT(!D.isString());
                    ASSERT(!D.isTime());
                    ASSERT(!D.isUdt());
                    ASSERT(DatumError(ERROR, MESSAGE) == D.theError());

                    Datum::destroy(D, &oa);

                    ASSERT(0 == oa.status());
                }
            }
        }

        if (verbose) cout << "\nTesting 'createInteger'." << endl;
        {
            const static int DATA[] = {
                0,
                1,
                -1,
                numeric_limits<char>::min(),
                numeric_limits<char>::max(),
                numeric_limits<unsigned char>::max(),
                numeric_limits<short>::min(),
                numeric_limits<short>::max(),
                numeric_limits<unsigned short>::max(),
                numeric_limits<int>::min(),
                numeric_limits<int>::max(),
            };
            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i< DATA_LEN; ++i) {
                const int VALUE = DATA[i];

                if (veryVerbose) { T_ P(VALUE) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Datum        mD = Datum::createInteger(VALUE);
                const Datum& D = mD;

                ASSERT(0 == oa.numBlocksInUse()); // non allocating type

                ASSERT(!D.isArray());
                ASSERT(!D.isBoolean());
                ASSERT(!D.isBinary());
                ASSERT(!D.isDate());
                ASSERT(!D.isDatetime());
                ASSERT(!D.isDatetimeInterval());
                ASSERT(!D.isDecimal64());
                ASSERT(!D.isDouble());
                ASSERT(!D.isError());
                ASSERT(D.isInteger());              // *
                ASSERT(!D.isInteger64());
                ASSERT(!D.isIntMap());
                ASSERT(!D.isMap());
                ASSERT(!D.isNull());
                ASSERT(!D.isString());
                ASSERT(!D.isTime());
                ASSERT(!D.isUdt());
                ASSERT(VALUE == D.theInteger());

                Datum::destroy(D, &oa);

                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nTesting 'createInteger64'." << endl;
        {
            const static Int64 DATA[] = {
                0,
                1,
                -1,
                numeric_limits<char>::min(),
                numeric_limits<char>::max(),
                numeric_limits<unsigned char>::max(),
                numeric_limits<short>::min(),
                numeric_limits<short>::max(),
                numeric_limits<unsigned short>::max(),
                numeric_limits<int>::min(),
                numeric_limits<int>::max(),
                numeric_limits<Int64>::min(),
                numeric_limits<Int64>::max(),
            };

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i< DATA_LEN; ++i) {
                const Int64 VALUE = DATA[i];

                if (veryVerbose) { T_ P(VALUE) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Datum        mD = Datum::createInteger64(VALUE, &oa);
                const Datum& D = mD;

#ifdef BSLS_PLATFORM_CPU_64_BIT
                ASSERT(0 == oa.numBlocksInUse()); // non allocating type
#endif  // BSLS_PLATFORM_CPU_64_BIT

                ASSERT(!D.isArray());
                ASSERT(!D.isBoolean());
                ASSERT(!D.isBinary());
                ASSERT(!D.isDate());
                ASSERT(!D.isDatetime());
                ASSERT(!D.isDatetimeInterval());
                ASSERT(!D.isDecimal64());
                ASSERT(!D.isDouble());
                ASSERT(!D.isError());
                ASSERT(!D.isInteger());
                ASSERT(D.isInteger64());            // *
                ASSERT(!D.isIntMap());
                ASSERT(!D.isMap());
                ASSERT(!D.isNull());
                ASSERT(!D.isString());
                ASSERT(!D.isTime());
                ASSERT(!D.isUdt());
                ASSERT(VALUE == D.theInteger64());

                Datum::destroy(D, &oa);

                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nTesting 'createNull'." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Datum        mD = Datum::createNull();
            const Datum& D = mD;

            ASSERT(0 == oa.numBlocksInUse()); // non allocating type

            ASSERT(!D.isArray());
            ASSERT(!D.isBoolean());
            ASSERT(!D.isBinary());
            ASSERT(!D.isDate());
            ASSERT(!D.isDatetime());
            ASSERT(!D.isDatetimeInterval());
            ASSERT(!D.isDecimal64());
            ASSERT(!D.isDouble());
            ASSERT(!D.isError());
            ASSERT(!D.isInteger());
            ASSERT(!D.isInteger64());
            ASSERT(!D.isIntMap());
            ASSERT(!D.isMap());
            ASSERT(D.isNull());                 // *
            ASSERT(!D.isString());
            ASSERT(!D.isTime());
            ASSERT(!D.isUdt());

            Datum::destroy(D, &oa);

            ASSERT(0 == oa.status());
        }

        if (verbose) cout << "\nTesting 'createStringRef'." << endl;
        {
            const static Datum::SizeType DATA[] = {
                0,
                1,
                32,
                255,
                256,
                65534,
                65535,
                65536,
                1024*1024,
            };

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            if (veryVerbose)
                cout << "\tTesting 'createStringRef(const char*, SizeType)'."
                     << endl;

            for (size_t i = 0; i< DATA_LEN; ++i) {
                bslma::TestAllocator  ba("buffer", veryVeryVeryVerbose);
                const Datum::SizeType SIZE = DATA[i];
                const bsl::string     BUFFER(SIZE, 'x', &ba);
                const StringRef       STRING(BUFFER.data(),
                                             static_cast<int>(SIZE));

                if (veryVerbose) { T_ T_ P(SIZE) }
                {
                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                    Datum        mD = Datum::createStringRef(BUFFER.data(),
                                                             SIZE,
                                                             &oa);
                    const Datum& D = mD;

                    ASSERT(!D.isArray());
                    ASSERT(!D.isBoolean());
                    ASSERT(!D.isBinary());
                    ASSERT(!D.isDate());
                    ASSERT(!D.isDatetime());
                    ASSERT(!D.isDatetimeInterval());
                    ASSERT(!D.isDecimal64());
                    ASSERT(!D.isDouble());
                    ASSERT(!D.isError());
                    ASSERT(!D.isInteger());
                    ASSERT(!D.isInteger64());
                    ASSERT(!D.isIntMap());
                    ASSERT(!D.isMap());
                    ASSERT(!D.isNull());
                    ASSERT(D.isString());               // *
                    ASSERT(!D.isTime());
                    ASSERT(!D.isUdt());
                    ASSERT(STRING == D.theString());

                    Datum::destroy(D, &oa);

                    ASSERT(0 == oa.status());
                }
            }

            // This overload simply forwards to the above method
            if (veryVerbose)
                cout << "\tTesting 'createStringRef(const char*)'."
                     << endl;

            {
                const char BUFFER[] = "This is test string.";

                if (veryVerbose) { T_ T_ P(BUFFER) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Datum        mD = Datum::createStringRef(BUFFER, &oa);
                const Datum& D = mD;

                ASSERT(!D.isArray());
                ASSERT(!D.isBoolean());
                ASSERT(!D.isBinary());
                ASSERT(!D.isDate());
                ASSERT(!D.isDatetime());
                ASSERT(!D.isDatetimeInterval());
                ASSERT(!D.isDecimal64());
                ASSERT(!D.isDouble());
                ASSERT(!D.isError());
                ASSERT(!D.isInteger());
                ASSERT(!D.isInteger64());
                ASSERT(!D.isIntMap());
                ASSERT(!D.isMap());
                ASSERT(!D.isNull());
                ASSERT(D.isString());               // *
                ASSERT(!D.isTime());
                ASSERT(!D.isUdt());
                ASSERT(StringRef(BUFFER) == D.theString());

                Datum::destroy(D, &oa);

                ASSERT(0 == oa.status());
            }

            // This overload simply forwards to the above method
            if (veryVerbose)
                cout << "\tTesting 'createStringRef(const StringRef&)'."
                     << endl;
            {
                const char BUFFER[] = "This is test string.";

                if (veryVerbose) { T_ T_ P(BUFFER) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Datum        mD = Datum::createStringRef(StringRef(BUFFER),
                                                         &oa);
                const Datum& D = mD;

                ASSERT(!D.isArray());
                ASSERT(!D.isBoolean());
                ASSERT(!D.isBinary());
                ASSERT(!D.isDate());
                ASSERT(!D.isDatetime());
                ASSERT(!D.isDatetimeInterval());
                ASSERT(!D.isDecimal64());
                ASSERT(!D.isDouble());
                ASSERT(!D.isError());
                ASSERT(!D.isInteger());
                ASSERT(!D.isInteger64());
                ASSERT(!D.isIntMap());
                ASSERT(!D.isMap());
                ASSERT(!D.isNull());
                ASSERT(D.isString());               // *
                ASSERT(!D.isTime());
                ASSERT(!D.isUdt());
                ASSERT(StringRef(BUFFER) == D.theString());

                Datum::destroy(D, &oa);

                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nTesting 'createTime'." << endl;
        {
#if defined(BSLS_PLATFORM_CPU_32_BIT)
            {
                // Testing assumption that 'Time' fits into 48 bits
                bdlt::Time         time(24);
                short              s;
                int                i;
                bsls::Types::Int64 ll;

                // 24:00:00.000000

                *reinterpret_cast<bdlt::Time *>(&ll) = time;
                ASSERT(bdld::Datum_Helpers32::storeInt48(ll, &s, &i));

                bsls::Types::Int64 ll2 =
                                        bdld::Datum_Helpers32::loadInt48(s, i);
                LOOP2_ASSERT(ll, ll2, ll == ll2);

                // 00:00:00.000000

                time = Time();

                *reinterpret_cast<bdlt::Time *>(&ll) = time;
                ASSERT(bdld::Datum_Helpers32::storeInt48(ll, &s, &i));

                ll2 = bdld::Datum_Helpers32::loadInt48(s, i);
                LOOP2_ASSERT(ll, ll2, ll == ll2);
            }
#endif
            const static bdlt::Time DATA[] = {
                Time(),
                Time(0, 1, 1, 1, 1),
                Time(8, 0, 0, 999, 888),
                Time(23, 59, 59, 999, 999),
            };

            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i< DATA_LEN; ++i) {
                const bdlt::Time VALUE = DATA[i];

                if (veryVerbose) { T_ P(VALUE) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Datum        mD = Datum::createTime(VALUE);
                const Datum& D = mD;

                ASSERT(0 == oa.numBlocksInUse()); // non allocating type

                ASSERT(!D.isArray());
                ASSERT(!D.isBoolean());
                ASSERT(!D.isBinary());
                ASSERT(!D.isDate());
                ASSERT(!D.isDatetime());
                ASSERT(!D.isDatetimeInterval());
                ASSERT(!D.isDecimal64());
                ASSERT(!D.isDouble());
                ASSERT(!D.isError());
                ASSERT(!D.isInteger());
                ASSERT(!D.isInteger64());
                ASSERT(!D.isIntMap());
                ASSERT(!D.isMap());
                ASSERT(!D.isNull());
                ASSERT(!D.isString());
                ASSERT(D.isTime());                 // *
                ASSERT(!D.isUdt());
                ASSERT(VALUE == D.theTime());

                Datum::destroy(D, &oa);

                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nTesting 'createUdt'." << endl;
        {
            char           BUFFER[] = "This is UDT placeholder";
            const DatumUdt VALUE(BUFFER, 0);

            if (veryVerbose) { T_ P(VALUE) }

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Datum        mD = Datum::createUdt(BUFFER, 0);
            const Datum& D = mD;

            ASSERT(0 == oa.numBlocksInUse()); // non allocating type

            ASSERT(!D.isArray());
            ASSERT(!D.isBoolean());
            ASSERT(!D.isBinary());
            ASSERT(!D.isDate());
            ASSERT(!D.isDatetime());
            ASSERT(!D.isDatetimeInterval());
            ASSERT(!D.isDecimal64());
            ASSERT(!D.isDouble());
            ASSERT(!D.isError());
            ASSERT(!D.isInteger());
            ASSERT(!D.isInteger64());
            ASSERT(!D.isIntMap());
            ASSERT(!D.isMap());
            ASSERT(!D.isNull());
            ASSERT(!D.isString());
            ASSERT(!D.isTime());
            ASSERT(D.isUdt());                  // *
            ASSERT(VALUE == D.theUdt());

            Datum::destroy(D, &oa);

            ASSERT(0 == oa.status());
        }

        if (verbose) cout << "\nTesting 'copyBinary'." << endl;
        {
            for (size_t i = 0; i < 258; ++i) {
                bslma::TestAllocator ba("buffer", veryVeryVeryVerbose);
                bsl::vector<unsigned char> BUFFER(&ba);
                const size_t SIZE = i;
                loadRandomBinary(&BUFFER, SIZE);

                const DatumBinaryRef REF(BUFFER.data(), SIZE);

                if (veryVerbose) { T_ P_(SIZE) P(REF) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Datum        mD = Datum::copyBinary(BUFFER.data(), SIZE, &oa);
                const Datum& D = mD;

#ifdef BSLS_PLATFORM_CPU_32_BIT
                ASSERT(0 != oa.numBlocksInUse());   // always allocate
#else  // BSLS_PLATFORM_CPU_32_BIT
                if (i<=13) {
                    ASSERT(0 == oa.numBlocksInUse());
                } else {
                    ASSERT(0 != oa.numBlocksInUse());
                }
#endif // BSLS_PLATFORM_CPU_32_BIT

                ASSERT(!D.isArray());
                ASSERT(!D.isBoolean());
                ASSERT(D.isBinary());               // *
                ASSERT(!D.isDate());
                ASSERT(!D.isDatetime());
                ASSERT(!D.isDatetimeInterval());
                ASSERT(!D.isDecimal64());
                ASSERT(!D.isDouble());
                ASSERT(!D.isError());
                ASSERT(!D.isInteger());
                ASSERT(!D.isInteger64());
                ASSERT(!D.isIntMap());
                ASSERT(!D.isMap());
                ASSERT(!D.isNull());
                ASSERT(!D.isString());
                ASSERT(!D.isTime());
                ASSERT(!D.isUdt());
                ASSERT(REF == D.theBinary());

                Datum::destroy(D, &oa);

                ASSERT(0 == oa.status());
            }
        }

        if (verbose) cout << "\nTesting 'copyString'." << endl;
        {
            for (size_t i = 0; i < 258; ++i) {
                bslma::TestAllocator ba("buffer", veryVeryVeryVerbose);
                bsl::string          BUFFER(&ba);
                const size_t         SIZE = i;
                loadRandomString(&BUFFER, SIZE);

                const StringRef REF(BUFFER.data(), static_cast<int>(SIZE));
                for (int j = 0; j < 2; ++j) {

                    if (veryVerbose) { T_ P_(j) P_(SIZE) P(REF) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                    Datum        mD = j ? Datum::copyString(BUFFER.data(),
                                                            SIZE, &oa)
                                        : Datum::copyString(REF, &oa);
                    const Datum& D = mD;

                    const Int64 bytesInUse = oa.numBytesInUse();
#ifdef BSLS_PLATFORM_CPU_32_BIT
                    // Up to length 6 inclusive the strings are stored inline
                    if (SIZE <= 6) {
                        ASSERT(0 == bytesInUse);
                    } else {
                        ASSERT(0 != bytesInUse);
                    }
#else  // BSLS_PLATFORM_CPU_32_BIT
                    // Up to length 13 inclusive the strings are stored inline
                    if (SIZE <= 13) {
                        ASSERT(0 == bytesInUse);
                    } else {
                        ASSERT(0 != bytesInUse);
                    }
#endif // BSLS_PLATFORM_CPU_32_BIT

                    ASSERT(!D.isArray());
                    ASSERT(!D.isBoolean());
                    ASSERT(!D.isBinary());
                    ASSERT(!D.isDate());
                    ASSERT(!D.isDatetime());
                    ASSERT(!D.isDatetimeInterval());
                    ASSERT(!D.isDecimal64());
                    ASSERT(!D.isDouble());
                    ASSERT(!D.isError());
                    ASSERT(!D.isInteger());
                    ASSERT(!D.isInteger64());
                    ASSERT(!D.isIntMap());
                    ASSERT(!D.isMap());
                    ASSERT(!D.isNull());
                    ASSERT(D.isString());               // *
                    ASSERT(!D.isTime());
                    ASSERT(!D.isUdt());
                    ASSERT(REF == D.theString());

                    Datum::destroy(D, &oa);

                    ASSERT(0 == oa.status());
                }
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bslma::TestAllocator         ta("test", veryVeryVerbose);
            bsls::AssertTestHandlerGuard hG;

            bslma::Allocator *nullAllocPtr =
                                            static_cast<bslma::Allocator *>(0);

            (void) nullAllocPtr;  // suppress compiler warning

            if (verbose) cout << "\tTesting 'createDatetime'." << endl;
            {
                bdlt::Datetime temp;

                ASSERT_FAIL(Datum::createDatetime(temp, nullAllocPtr));
                ASSERT_PASS(Datum::destroy(
                                       Datum::createDatetime(temp, &ta), &ta));
            }

            if (verbose) cout << "\tTesting 'createDatetimeInterval'." << endl;
            {
                bdlt::DatetimeInterval temp;

                ASSERT_FAIL(Datum::createDatetimeInterval(temp, nullAllocPtr));

                ASSERT_PASS(Datum::destroy(
                               Datum::createDatetimeInterval(temp, &ta), &ta));
            }

            if (verbose) cout << "\tTesting 'createError'." << endl;
            {
                const int       CODE = 1;
                const StringRef MESSAGE("temp");

                ASSERT_FAIL(Datum::createError(CODE,
                                               MESSAGE,
                                               nullAllocPtr));
                ASSERT_PASS(Datum::destroy(
                                 Datum::createError(CODE, MESSAGE, &ta), &ta));
            }

            if (verbose) cout << "\tTesting 'createInteger64'." << endl;
            {
                bsls::Types::Int64 temp = 1;

                ASSERT_FAIL(Datum::createInteger64(temp, nullAllocPtr));
                ASSERT_PASS(Datum::destroy(
                                      Datum::createInteger64(temp, &ta), &ta));
            }

            if (verbose) cout <<
            "\tTesting 'createStringRef(const char *, SizeType, Allocator *)'."
                              << endl;
            {
                const char     *temp        = "";
                const SizeType  LENGTH      = 1;
                const char     *nullCharPtr = static_cast<const char *>(0);

                // Data check.
                ASSERT_SAFE_PASS(Datum::createStringRef(nullCharPtr, 0, &ta));
                ASSERT_SAFE_FAIL(Datum::createStringRef(nullCharPtr,
                                                        LENGTH,
                                                        &ta));
                ASSERT_SAFE_PASS(Datum::createStringRef(temp, 0,      &ta));
                ASSERT_SAFE_PASS(Datum::createStringRef(temp, LENGTH, &ta));

                // Allocator check.
                ASSERT_SAFE_FAIL(Datum::createStringRef(temp,
                                                        LENGTH,
                                                        nullAllocPtr));
            }

            if (verbose) cout <<
                      "\tTesting 'createStringRef(const char *, Allocator *)'."
                              << endl;
            {
                const char *temp = "temp";
                const char *nullCharPtr = static_cast<const char *>(0);

                (void) nullCharPtr;  // suppress compiler warning

                ASSERT_SAFE_FAIL(Datum::createStringRef(nullCharPtr, &ta));
                ASSERT_SAFE_PASS(Datum::createStringRef(temp,        &ta));
                ASSERT_SAFE_FAIL(Datum::createStringRef(temp, nullAllocPtr));
            }

            if (verbose) cout <<
                  "\tTesting 'createStringRef(const StringRef&, Allocator *)'."
                              << endl;
            {
                const bslstl::StringRef temp("temp");

                ASSERT_SAFE_FAIL(Datum::createStringRef(temp, nullAllocPtr));
                ASSERT_SAFE_PASS(Datum::createStringRef(temp, &ta));
            }

            if (verbose) cout << "\tTesting 'createUdt'." << endl;
            {
                void *temp = static_cast<void *>(0);

                ASSERT_SAFE_FAIL(Datum::createUdt(temp, -1));
                ASSERT_SAFE_FAIL(Datum::createUdt(temp, 65536));
                ASSERT_SAFE_PASS(Datum::createUdt(temp, 0));
                ASSERT_SAFE_PASS(Datum::createUdt(temp, 1));
                ASSERT_SAFE_PASS(Datum::createUdt(temp, 65534));
                ASSERT_SAFE_PASS(Datum::createUdt(temp, 65535));
            }

            if (verbose) cout << "\tTesting 'copyBinary'." << endl;
            {
                char            temp;
                const void     *VALUE   = static_cast<void *>(&temp);
                const SizeType  SIZE    = 1;
                const void     *nullPtr = static_cast<const void *>(0);

                // Data check.

                ASSERT_PASS(Datum::destroy(Datum::copyBinary(nullPtr, 0, &ta),
                                           &ta));
                ASSERT_FAIL(Datum::copyBinary(nullPtr, SIZE, &ta));
                ASSERT_PASS(Datum::destroy(Datum::copyBinary(VALUE, 0, &ta),
                                           &ta));
                ASSERT_PASS(Datum::destroy(Datum::copyBinary(VALUE, SIZE, &ta),
                                           &ta));

                // Allocator check.

                ASSERT_FAIL(Datum::copyBinary(VALUE, SIZE, nullAllocPtr));
            }

            if (verbose) cout << "\tTesting 'copyString'." << endl;
            {
                const char     *temp        = "temp";
                const SizeType  LEN         = 1;
                const char     *nullCharPtr = static_cast<const char *>(0);

                // Data check.

                ASSERT_PASS(Datum::copyString(nullCharPtr, 0,   &ta));
                ASSERT_FAIL(Datum::copyString(nullCharPtr, LEN, &ta));
                ASSERT_PASS(Datum::copyString(temp,        0,   &ta));
                ASSERT_PASS(Datum::copyString(temp,        LEN, &ta));

                // Allocator check.

                ASSERT_FAIL(Datum::copyString(temp, LEN, nullAllocPtr));
            }

            if (verbose) cout << "\tTesting 'destroy'." << endl;
            {
                const Datum temp = Datum::createNull();

                ASSERT_FAIL(Datum::destroy(temp, nullAllocPtr));
                ASSERT_PASS(Datum::destroy(temp, &ta));
            }

            if (verbose) cout << "\tTesting 'theBinary'." << endl;
            {
                void        *temp = static_cast<void *>(0);
                const Datum  fakeBinary = Datum::createNull();
                const Datum  realBinary = Datum::copyBinary(temp, 0, &ta);

                ASSERT_SAFE_FAIL(fakeBinary.theBinary());
                ASSERT_SAFE_PASS(realBinary.theBinary());

                Datum::destroy(fakeBinary, &ta);
                Datum::destroy(realBinary, &ta);
            }

            if (verbose) cout << "\tTesting 'theBoolean'." << endl;
            {
                const Datum fakeBoolean = Datum::createNull();
                const Datum realBoolean = Datum::createBoolean(true);

                ASSERT_SAFE_FAIL(fakeBoolean.theBoolean());
                ASSERT_SAFE_PASS(realBoolean.theBoolean());

                Datum::destroy(fakeBoolean, &ta);
                Datum::destroy(realBoolean, &ta);
            }

            if (verbose) cout << "\tTesting 'theDate'." << endl;
            {
                const Datum fakeDate = Datum::createNull();
                const Datum realDate = Datum::createDate(Date());

                ASSERT_SAFE_FAIL(fakeDate.theDate());
                ASSERT_SAFE_PASS(realDate.theDate());

                Datum::destroy(fakeDate, &ta);
                Datum::destroy(realDate, &ta);
            }

            if (verbose) cout << "\tTesting 'theDatetime'." << endl;
            {
                const Datum fakeDatetime = Datum::createNull();
                const Datum realDatetime = Datum::createDatetime(Datetime(),
                                                                 &ta);

                ASSERT_SAFE_FAIL(fakeDatetime.theDatetime());
                ASSERT_SAFE_PASS(realDatetime.theDatetime());

                Datum::destroy(fakeDatetime, &ta);
                Datum::destroy(realDatetime, &ta);
            }

            if (verbose) cout << "\tTesting 'theDatetimInterval'." << endl;
            {
                const Datum fakeDatetimeInterval = Datum::createNull();
                const Datum realDatetimeInterval =
                        Datum::createDatetimeInterval(DatetimeInterval(), &ta);

                ASSERT_SAFE_FAIL(fakeDatetimeInterval.theDatetimeInterval());
                ASSERT_SAFE_PASS(realDatetimeInterval.theDatetimeInterval());

                Datum::destroy(fakeDatetimeInterval, &ta);
                Datum::destroy(realDatetimeInterval, &ta);
            }

            if (verbose) cout << "\tTesting 'theDecimal64'." << endl;
            {
                const Datum fakeDecimal64 = Datum::createNull();
                const Datum realDecimal64 = Datum::createDecimal64(
                                                        BDLDFP_DECIMAL_DD(0.0),
                                                        &ta);

                ASSERT_SAFE_FAIL(fakeDecimal64.theDecimal64());
                ASSERT_SAFE_PASS(realDecimal64.theDecimal64());

                Datum::destroy(fakeDecimal64, &ta);
                Datum::destroy(realDecimal64, &ta);
            }

            if (verbose) cout << "\tTesting 'theDouble'." << endl;
            {
                const Datum fakeDouble = Datum::createNull();
                const Datum realDouble = Datum::createDouble(0.0);

                ASSERT_SAFE_FAIL(fakeDouble.theDouble());
                ASSERT_SAFE_PASS(realDouble.theDouble());

                Datum::destroy(fakeDouble, &ta);
                Datum::destroy(realDouble, &ta);
            }

            if (verbose) cout << "\tTesting 'theError'." << endl;
            {
                const Datum fakeError = Datum::createNull();
                const Datum realError = Datum::createError(0);

                ASSERT_SAFE_FAIL(fakeError.theError());
                ASSERT_SAFE_PASS(realError.theError());

                Datum::destroy(fakeError, &ta);
                Datum::destroy(realError, &ta);
            }

            if (verbose) cout << "\tTesting 'theInteger'." << endl;
            {
                const Datum fakeInteger = Datum::createNull();
                const Datum realInteger = Datum::createInteger(0);

                ASSERT_SAFE_FAIL(fakeInteger.theInteger());
                ASSERT_SAFE_PASS(realInteger.theInteger());

                Datum::destroy(fakeInteger, &ta);
                Datum::destroy(realInteger, &ta);
            }

            if (verbose) cout << "\tTesting 'theInteger64'." << endl;
            {
                const Datum fakeInteger64 = Datum::createNull();
                const Datum realInteger64 = Datum::createInteger64(0, &ta);

                ASSERT_SAFE_FAIL(fakeInteger64.theInteger64());
                ASSERT_SAFE_PASS(realInteger64.theInteger64());

                Datum::destroy(fakeInteger64, &ta);
                Datum::destroy(realInteger64, &ta);
            }

            if (verbose) cout << "\tTesting 'theString'." << endl;
            {
                const Datum fakeStringRef = Datum::createNull();
                const Datum realStringRef = Datum::createStringRef("temp",
                                                                   4,
                                                                   &ta);

                ASSERT_SAFE_FAIL(fakeStringRef.theString());
                ASSERT_SAFE_PASS(realStringRef.theString());

                Datum::destroy(fakeStringRef, &ta);
                Datum::destroy(realStringRef, &ta);
            }

            if (verbose) cout << "\tTesting 'theTime'." << endl;
            {
                const Datum fakeTime = Datum::createNull();
                const Datum realTime = Datum::createTime(Time());

                ASSERT_SAFE_FAIL(fakeTime.theTime());
                ASSERT_SAFE_PASS(realTime.theTime());

                Datum::destroy(fakeTime, &ta);
                Datum::destroy(realTime, &ta);
            }

            if (verbose) cout << "\tTesting 'theUdt'." << endl;
            {
                void        *temp = static_cast<void *>(0);
                const Datum  fakeUdt = Datum::createNull();
                const Datum  realUdt = Datum::createUdt(temp, 0);

                ASSERT_SAFE_FAIL(fakeUdt.theUdt());
                ASSERT_SAFE_PASS(realUdt.theUdt());

                Datum::destroy(fakeUdt, &ta);
                Datum::destroy(realUdt, &ta);
            }
            if (verbose) cout << "\tTesting zero-filled 'Datum'." << endl;
            {
                Datum temp;

                memset(static_cast<void *>(&temp), 0, sizeof(Datum));

#ifdef BSLS_PLATFORM_CPU_32_BIT
                ASSERT_SAFE_PASS(temp.type());
                ASSERT_SAFE_PASS(temp.isDouble());
#else   // BSLS_PLATFORM_CPU_32_BIT
                ASSERT_SAFE_FAIL(temp.type());
                ASSERT_SAFE_FAIL(temp.isDouble());
#endif  // BSLS_PLATFORM_CPU_32_BIT

            }
        }

        ASSERT(0 == da.numAllocations());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'Datum_Helpers32'
        //   Ensure that helper functions defined in this structure correctly
        //   store and load low 48 bits of 64-bit number to/from a pair of
        //   32-bit 'int' and 16-bit 'short' on 32-bit platforms.  Note that
        //   the upper (48th) bit of the 64-bit number is treated as a
        //   sign-bit.
        //
        // Concerns:
        //: 1 Any 64-bit number in the range -2^47..2^47-1 can be stored and
        //:   loaded back by the helper functions.
        //:
        //: 2 No 64-bit number more than 48 least significant bits cannot be
        //:   stored and l
        //
        // Plan:
        //: 1 Using a table-driven technique, iterate through the set of
        //:   representative 64-bit values.  For each value in the set, store
        //:   and load back the value using 'storeSmallInt64' and
        //:   'loadSmallInt64' functions.  Verify that all values from valid
        //:   range can be stored and loaded back.  Verify that for all values
        //:   not in valid range the 'storeSmallInt64' return 'false'.  (C-1,2)
        //
        // Testing:
        //   Int64 loadSmallInt64(short hight16, int low32);
        //   bool storeSmallInt64(Int64 value, short *phigh16, int *plow32);
        // --------------------------------------------------------------------
#ifdef BSLS_PLATFORM_CPU_32_BIT
        if (verbose) cout << endl
                          << "TESTING 'Datum_Helpers32'" << endl
                          << "=========================" << endl;

        {
            static const struct {
                int        d_line;     // line number
                long long  d_value;    // 64-bit value
                bool       d_isValid;  // falls into valid value range
            } DATA[] = {
                //LINE  VALUE                    VALID
                //----  ---------------------    -----
                { L_,    0LL,                    true  },
                { L_,    1LL,                    true  },
                { L_,   -1LL,                    true  },
                { L_,    0x1234LL,               true  },
                { L_,   -0x1234LL,               true  },
                { L_,    0x0000000012345678LL,   true  },
                { L_,    0x00001234567890abLL,   true  },
                { L_,    0x00000000ffffffffLL,   true  },  // 2^32-1
                { L_,    0x0000000100000000LL,   true  },  // 2^32
                { L_,    0x0000000100000001LL,   true  },  // 2^32+1
                { L_,    0x00007fffffffffffLL,   true  },  // 2^47-1
                { L_,    0x0000800000000000LL,   false },  // 2^47
                { L_,    0x0000800000000001LL,   false },  // 2^47+1
                { L_,    0x0000ba0987654321LL,   false },
                { L_,    0x00008fffffffffffLL,   false },
                { L_,    0x0000ffffffffffffLL,   false },
                { L_,    0x0001000000000000LL,   false },
                { L_,    0x0010000000000000LL,   false },
                { L_,    0x0100000000000000LL,   false },
                { L_,    0x1000000000000000LL,   false },
                { L_,   -0x7000000000000000LL,   false },
                { L_,   -0x0001000000000000LL,   false },
                { L_,   -0x0010000000000000LL,   false },
                { L_,   -0x0100000000000000LL,   false },
                { L_,   -0x1000000000000000LL,   false },
                { L_,   -0x0000800000000001LL,   false },  // -2^47-1
                { L_,   -0x0000800000000000LL,   true  },  // -2^47
                { L_,   -0x00007fffffffffffLL,   true  },  // -2^47+1
                { L_,   -0x7fffffffffffffffLL,   false },
            };
            const size_t DATA_LEN = sizeof DATA / sizeof *DATA;

            for (size_t i = 0; i< DATA_LEN; ++i) {
                const int   LINE  = DATA[i].d_line;
                const Int64 VALUE = Int64(DATA[i].d_value);
                const bool  VALID = DATA[i].d_isValid;

                short high16 = 0;
                int   low32  = 0;

                if (veryVerbose) {
                    T_ P_(LINE) P_(VALUE) P(VALID)
                }

                const bool storeResult =
                      Datum_Helpers32::storeSmallInt64(VALUE, &high16, &low32);

                ASSERTV(LINE, VALUE, VALID, VALID == storeResult);

                if (VALID) {
                    const Int64 valueResult =
                                Datum_Helpers32::loadSmallInt64(high16, low32);

                    ASSERTV(LINE, VALUE, valueResult, VALUE == valueResult);
                }
            }
        }
#else
        if (verbose) cout << endl
                          << "TESTING 'Datum_Helpers32'" << endl
                          << "=========================" << endl;
        if (verbose) cout << "\nSkipped on 64-bit platforms" << endl;
#endif
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
        //: 1 Developer test sandbox. (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bslma::TestAllocator ta("test", veryVeryVeryVerbose);

        if (verbose) cout << "\n'Datum' with integer value." << endl;
        {
            // Data type without memory allocation.
            Datum        mD = Datum::createInteger(0);
            const Datum& D = mD;

            ASSERT(D.isInteger());
            ASSERT(0 == D.theInteger());

            const Datum CD = D.clone(&ta);
            ASSERT(CD == D);

            ASSERT(0 == ta.status());
        }

        if (verbose) cout << "\n'Datum' with string value." << endl;
        {
            // Data type with memory allocation.
            const char   *str = "A long string abcdef";
            Datum         mD = Datum::copyString(str, &ta);
            const Datum&  D = mD;

            ASSERT(D.isString());
            ASSERT(bslstl::StringRef(str) == D.theString());

            const Datum CD = D.clone(&ta);
            ASSERT(CD == D);

            Datum::destroy(D, &ta);
            Datum::destroy(CD, &ta);
            ASSERT(0 == ta.status());
        }
      } break;
      default: {
        // --------------------------------------------------------------------
        // TESTING EFFICIENCY
        //
        // Concerns:
        //: 1 Efficiency of components satisfies performance requirements.
        //
        // Plan:
        //: 1 Run benchmark suite and verify results.
        //
        // Testing:
        //   EFFICIENCY TEST
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING EFFICIENCY" << endl
                          << "==================" << endl;

        int loops = test;

        if (test == -2) {
#if defined(BSLS_PLATFORM_OS_AIX)
            loops = -100 * 1000;
#elif defined(BSLS_PLATFORM_OS_LINUX)
            loops = -100 * 1000;
#elif defined(BSLS_PLATFORM_OS_SOLARIS)
            loops = -10 * 1000;
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
            loops = -1000 * 1000;
#elif defined(BSLS_PLATFORM_OS_DARWIN)
            loops = -100 * 1000;
#else
            loops = -100 * 1000;
#endif
        }
        if (test <= -2) {
            BenchmarkSuite benchmark;
            benchmark.run(-loops, argc - 2, argv + 2);
            break;
        }

        cerr << "WARNING: CASE '" << test << "' NOT FOUND." << endl;
        testStatus = -1;
        break;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
