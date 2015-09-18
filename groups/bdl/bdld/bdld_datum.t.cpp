// bdld_datum.t.cpp                                                   -*-C++-*-

#include <bdld_datum.h>
#include <bdlma_bufferedsequentialallocator.h>  // for testing only
#include <bdlt_datetime.h>
#include <bdlt_datetimeinterval.h>
#include <bdlt_currenttime.h>
#include <bdldfp_decimalutil.h>
#include <bdldfp_decimalutil.h>

#include <bdlma_bufferedsequentialallocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bsl_algorithm.h>
#include <bsl_iostream.h>
#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_memory.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>
#include <bsls_stopwatch.h>
#include <bsls_timeutil.h>
#include <bdldfp_decimalconvertutil.h>

#include <time.h>

using namespace BloombergLP;
using namespace bsl;
using namespace bslstl;
using namespace BloombergLP::bdld;

// ============================================================================
//                                TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test is an *in-core* *value-semantic* class that
// represents all general purpose values.  Our testing concerns are limited to
// creating 'Datum' objects with different types of values, retrieving the
// type and actual value within these objects, copying these objects and
// finally destroying them.  At the end of these operations, the same amount of
// memory should be deallocated as was allocated while performing these
// operations.
//
// Our goal is to test both 'Datum' and 'Datum' as the creators for 'Datum' are
// static methods within 'Datum'.  Note that 'Datum' holds values that are
// immutable.  However, 'Datum' is copy-assignable.  So a 'Datum' object can be
// copied by another 'Datum' object.  We will also test 'ConstDatumArrayRef'
// which provides a read-only view to an array of 'Datum' objects.  It acts as
// return value for accessors inside the 'Datum' class which return an array of
// 'Datum' objects.  We will also test 'ConstDatumMapRef' which is provides a
// read-only view to a map 'Datum' objects.  It acts as return value for
// accessors inside the 'Datum' class which return a map of 'Datum' objects
// keyed by string values.  We will also test 'Datum_ArrayProctor' which is a
// *proctor* class to manage an array of (otherwise-unmanaged) 'Datum' objects.
// Unless explicitly released, the contiguous managed objects are destroyed
// automatically when the range proctor goes out of scope by calling ':destroy'
// on each (managed) 'Datum' object.  The memory allocated for the whole array
// is also deallocated.  We will also test 'Datum_MapProctor' which is a
// *proctor* class to manage a map of (otherwise-unmanaged) 'Datum' objects.
// Unless explicitly released, the contiguous managed objects are destroyed
// automatically when the range proctor goes out of scope by calling 'destroy'
// on each (managed) 'Datum' object.  The memory allocated for the whole map is
// also deallocated.
//-----------------------------------------------------------------------------
// CREATORS
// [ 3] Datum createAggregate(void *, Datum::InternalType);
// [ 7] Datum copyAggregateOrUdt(const Udt&, bslma::Allocator *);
// [ 7] Datum copyArray(const Datum *, bslma::Allocator *);
// [ 7] Datum copyMap(const DatumMapEntry *, bslma::Allocator *);
// [ 7] Datum copyMapOwningKeys(const DatumMapEntry *,
//                                     bslma::Allocator *);
// [ 3] Datum adoptArray(const DatumArrayRef&);
// [ 3] Datum adoptMap(const DatumMapRef&);
// [ 3] Datum adoptMapOwningKeys(const DatumMapOwningKeysRef&);
// [ 3] Datum createBoolean(bool);
// [ 3] Datum createDate(const bdlt::Date&);
// [ 3] Datum createDatetime(const bdlt::Datetime&, bslma::Allocator *);
// [ 3] Datum createDatetimeInterval(const bdlt::DatetimeInterval&,
//                                          bslma::Allocator *);
// [ 3] Datum createDecimal64(Decimal);
// [ 3] Datum createDouble(double);
// [ 3] Datum createError(int);
// [ 3] Datum createError(int, const bslstl::StringRef&, bslma::Allocator *);
// [ 3] Datum createInteger(int);
// [ 3] Datum createInteger64(bsls::Types::Int64, bslma::Allocator *);
// [ 3] Datum createNull();
// [ 3] Datum copyString(const char *, bslma::Allocator *);
// [ 3] Datum copyString(const char *, size_type, bslma::Allocator *);
// [ 3] Datum copyString(const bslstl::StringRef&, bslma::Allocator *);
// [ 3] Datum createTime(const bdlt::Time&);
// [ 3] Datum createUdt(void *, int);
// [ 3] Datum copyBinary(void *, int);
// [ 7] Datum clone(const Datum&, bslma::Allocator *);
// [ 3] void createUninitializedArray(DatumArrayRef *,
//                                    size_type,
//                                    bslma::Allocator *);
// [ 3] void createUninitializedMap(DatumMapRef *,
//                                  size_type,
//                                  bslma::Allocator *);
// [ 3] Datum createStringRef(const char *, bslma::Allocator *);
// [ 3] Datum createStringRef(const char *,
//                                  SizeType,
//                                  bslma::Allocator *);
// [ 3] Datum createStringRef(const bslstl::StringRef&,
//                                  bslma::Allocator *);
// [ 3] void createUninitializedMapOwningKeys(DatumMapOwningKeysRef *,
//                                            size_type,
//                                            size_type,
//                                            bslma::Allocator *);
// [ 3] char *createUninitializedString(Datum *,
//                                      size_type,
//                                      bslma::Allocator *);
// [ 3] void destroy(const Datum&, bslma::Allocator *);
// [18] void disposeUninitializedArray(const DatumArrayRef&*,
//                                     bslma::Allocator *);
// [19] void disposeUninitializedMap(const DatumMapRef&,
//                                   bslma::Allocator *);
// [20] void disposeUninitializedMapOwningKeys(
//                                        const DatumMapOwningKeysRef&,
//                                        bslma::Allocator *);
//
// MANIPULATORS
// [ 3] Datum& operator=(const Datum&);
//
// ACCESSORS
// [ 3] ConstDatumArrayRef theArray() const;
// [ 3] bool theBoolean() const;
// [ 3] bdlt::Date theDate() const;
// [ 3] const bdlt::Datetime& theDatetime() const;
// [ 3] const bdlt::DatetimeInterval& theDatetimeInterval() const;
// [ 3] double theDecimal64() const;
// [ 3] double theDouble() const;
// [ 3] Error the Error() const;
// [ 3] int theInteger() const;
// [ 3] bsls::Types::Int64 theInteger64() const;
// [ 3] ConstDatumMapRef theMap() const;
// [ 3] bslstl::StringRef theString() const;
// [ 3] bdlt::Time theTime() const;
// [ 3] Udt theUdt() const;
// [ 5] bool isArray() const;
// [ 5] bool isBoolean() const;
// [ 5] bool isDate() const;
// [ 5] bool isDatetime() const;
// [ 5] bool isDatetimeInterval() const;
// [ 5] bool isDecimal64() const;
// [ 5] bool isDouble() const;
// [ 5] bool isError() const;
// [ 5] bool isInteger() const;
// [ 5] bool isInteger64() const;
// [ 5] bool isMap() const;
// [ 5] bool isNull() const;
// [ 5] bool isString() const;
// [ 5] bool isTime() const;
// [ 5] bool isUdt() const;
// [ 5] DataType typeFromUdt() const;
// [ 5] bool isBinary();
// [ 5] DataType type() const;
// [ 8] template <typename BDLD_VISITOR> void apply(BDLD_VISITOR&) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const Datum&, const Datum&);
// [ 6] bool operator!=(const Datum&, const Datum&);
// [ 9] bsl::ostream& operator<<(bsl::ostream&, const Datum&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] DATUM_HELPERS32
// [22] USAGE EXAMPLE
// [ 10] ConstDatumArrayRef TEST
// [11] Datum_ArrayProctor TEST
// [12] DatumMapEntry TEST
// [13] ConstDatumMapRef TEST
// [14] Datum_MapProctor TEST
// [15] copyArray EXCEPTION TEST
// [16] copyMap EXCEPTION TEST
// [17] copyMapOwningKeys EXCEPTION TEST
// [21] MISALIGNED MEMORY ACCESS TEST (only on SUN - see test for more details)
// ----------------------------------------------------------------------------
// [-2] BENCHMARK
// [-X] BENCHMARK - X > 2 = ITERATION COUNT

//=============================================================================
//                     STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                   STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                              \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                 \
                     << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"    \
                     << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"    \
                     << #K << ": " << K << "\t" << #L << ": " << L << "\n";   \
                aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"    \
                     << #K << ": " << K << "\t" << #L << ": " << L << "\t";   \
                     << #M << ": " << M << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"    \
                     << #K << ": " << K << "\t" << #L << ": " << L << "\t";   \
                     << #M << ": " << M << "\t" << #N << ": " << N << "\n";   \
                aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                     SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'.
#define T_ cout << "\t" << flush;             // Print tab without newline.
#define L_ __LINE__                           // Current line number.

                            //-----------------
                            // Global variables

bool verbose;
int test_, subtest_;

typedef bsls::Types::Int64     Int64;
typedef bsls::Types::size_type size_type;

const double DBL_MIN2    = numeric_limits<double>::min();
const double DBL_MAX2    = numeric_limits<double>::max();
const double DBL_INF     = numeric_limits<double>::infinity();
const double DBL_NEGINF  = -1 * DBL_INF;
const double DBL_ZERO    = 0.0;
const double DBL_NEGZERO = 1 / DBL_NEGINF;
const double DBL_QNAN2   = numeric_limits<double>::quiet_NaN();
#ifdef WIN32
const double DBL_IND     = sqrt(-1.0); // indeterminate NaN for MSVS
#endif

bdlt::Date udtValue;
bdlt::Date udtValue2;

const int UDT_TYPE = 20;

const Datum NULL_VALUE = Datum::createNull();
#if defined(_WIN32)
#include <Windows.h>

#elif defined(__unix__) || defined(__unix) || defined(unix) \
    || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <time.h>

#else
#error "Unable to define getCPUTime( ) for an unknown OS."
#endif

const int decimal64Values = 5;
const int comparableDecimal64Values = decimal64Values - 1;
using bdldfp::Decimal64;

struct Decimal64Value {
    const char    *label;
    Decimal64      value;
    const char    *str;
} decimal64Value[decimal64Values] = {
    { "small",     BDLDFP_DECIMAL_DD(0.10),                "0.10"       },
    { "large",     BDLDFP_DECIMAL_DD(12.3456789),          "12.3456789" },
    { "infinity",  numeric_limits<Decimal64>::infinity(),  "infinity"   },
    { "-infinity", -numeric_limits<Decimal64>::infinity(), "-infinity"  },
    { "NaN",       numeric_limits<Decimal64>::quiet_NaN(), "nan"        },
};

                           // ==========
                           // LineStream
                           // ==========

class LineStream {
    // This class provides a simple way of conditionally writing one line to a
    // stream

  private:
    ostream *m_pos;

  public:
    // CREATORS
    explicit LineStream(ostream *pos) : m_pos(pos) { }
        // Create a LineStream object.

    ~LineStream() { if (m_pos) *m_pos << "\n"; }
        // Destroy a LineStream object. If associated to a stream, write a
        // newline to it.

    // MANIPULATORS
    template<class T>
    LineStream& operator <<(const T& value)
        // If associated to a stream, write 'value' to it.
    {
        if (m_pos) {
            *m_pos << value;
        }
        return *this;
    }
};

//=============================================================================
//                   GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

void assertHandler(const char *text, const char *file, int line)
{
    bsl::cout << "Error " << file << "(" << line << "): " << text
              << "    (assert)" << bsl::endl;

    int *i = 0;
    // cppcheck-suppress nullPointer
    *i = 0;
}

void TESTCASE(const char *label)
    // Start a new test case. Print header if verbose is on. Return a
    // LineStream associated to 'cout' or null, depending on verbosity.
{
    test_ = 1;
    if (verbose) {
        cout << "\n" << label << "\n";
        while (*label++) cout << '=';
        cout << "\n";
    }
}

LineStream TEST(const char *label)
    // Start a new test. Print header if verbose is on. Return a LineStream
    // associated to 'cout' or null, depending on verbosity.
{
    subtest_ = 0;
    if (verbose) {
        cout << test_ << ". " << label;
        ++test_;
        return LineStream(&cout);
    }
    else {
        ++test_;
        return LineStream(0);
    }
}

LineStream SUBTEST(const char *label)
    // Start a new subtest. Print header if verbose is on. Return a LineStream
    // associated to 'cout' or null, depending on verbosity.
{
    if (verbose) {
        bsl::string index;
        int n = subtest_++;
        while (true) {
            index = char('a' + n % 26) + index;
            if (n / 26 == 0)
                break;
            n = n / 26 - 1;
        }
        cout << "\t" << index << ". " << label;
        return LineStream(&cout);
    }
    else {
        ++subtest_;
        return LineStream(0);
    }
}

                          // ------------------------
                          // createArrayOfScalarTypes

void createArrayOfScalarTypes(vector<Datum>    *elements,
                              bslma::Allocator *basicAllocator)
    // Load some sample scalar type 'Datum' objects in the specified
    // 'elements'. Use the specified 'basicAllocator' to allocate memory.
{
    elements->push_back(Datum::createNull());                             //  0
    elements->push_back(Datum::createInteger(0));                         //  1
    elements->push_back(Datum::createInteger(1));
    elements->push_back(Datum::createBoolean(true));
    elements->push_back(Datum::createBoolean(false));
    elements->push_back(Datum::createError(0));                    //  5
    elements->push_back(Datum::createError(1, "some error", basicAllocator));
    elements->push_back(Datum::createDate(bdlt::Date(2010,1,5)));
    elements->push_back(Datum::createDate(bdlt::Date(1,1,1)));
    elements->push_back(Datum::createTime(bdlt::Time(16,45,32,12)));
    elements->push_back(Datum::createTime(bdlt::Time(1,1,1,1)));        // 10
    elements->push_back(Datum::createDatetime(bdlt::Datetime(2010,1,5,
                                                            16,45,32,12),
                                              basicAllocator));
    elements->push_back(Datum::createDatetime(bdlt::Datetime(1,1,1,
                                                            1,1,1,1),
                                              basicAllocator));
    elements->push_back(Datum::createDatetimeInterval(
                                        bdlt::DatetimeInterval(34, 16,45,32,12),
                                        basicAllocator));
    elements->push_back(Datum::createDatetimeInterval(
                                            bdlt::DatetimeInterval(1, 1,1,1,1),
                                            basicAllocator));
    elements->push_back(Datum::createInteger64(Int64(9223372036854775807LL),
                                               basicAllocator));          // 15
    elements->push_back(Datum::createInteger64(Int64(1229782938247303441LL),
                                               basicAllocator));
    elements->push_back(Datum::copyString("A long string", basicAllocator));
    elements->push_back(Datum::copyString("A very long string",
                                          basicAllocator));
    elements->push_back(Datum::copyString("abc", basicAllocator));
    elements->push_back(Datum::copyString("Abc", basicAllocator));        // 20
    elements->push_back(Datum::createDouble(1.0));
    elements->push_back(Datum::createDouble(1.2345));
    elements->push_back(Datum::createDouble(-1.2346));
    elements->push_back(Datum::createDouble(DBL_MIN2));
    elements->push_back(Datum::createDouble(DBL_MAX2));                   // 25
    elements->push_back(Datum::createDouble(DBL_INF));
    elements->push_back(Datum::createDouble(DBL_NEGINF));

    // This test will take care of 0.0 as -0.0 == 0.0.
    elements->push_back(Datum::createDouble(DBL_NEGZERO));

    elements->push_back(Datum::createUdt(&udtValue,  UDT_TYPE));
    elements->push_back(Datum::createUdt(&udtValue,  UDT_TYPE + 1));      // 30
    elements->push_back(Datum::createUdt(&udtValue2, UDT_TYPE));

    // Create empty arrays.
    DatumArrayRef emptyArray;
    Datum::createUninitializedArray(&emptyArray, 0, basicAllocator);
    *(emptyArray.length()) = 0;
    elements->push_back(Datum::adoptArray(emptyArray));
    Datum::createUninitializedArray(&emptyArray, 0, basicAllocator);
    *(emptyArray.length()) = 0;
    elements->push_back(Datum::adoptArray(emptyArray));

    // Create empty maps.
    DatumMapRef mapping;
    Datum::createUninitializedMap(&mapping, 0, basicAllocator);
    *(mapping.size()) = 0;
    elements->push_back(Datum::adoptMap(mapping));
    Datum::createUninitializedMap(&mapping, 0, basicAllocator);
    *(mapping.size()) = 0;
    elements->push_back(Datum::adoptMap(mapping));                        // 35

    // Create empty maps (owning keys).
    DatumMapOwningKeysRef mappingOwningKeys;
    Datum::createUninitializedMapOwningKeys(&mappingOwningKeys,
                                            0, 0,
                                            basicAllocator);
    *(mappingOwningKeys.size())     = 0;
    elements->push_back(Datum::adoptMapOwningKeys(mappingOwningKeys));
    Datum::createUninitializedMapOwningKeys(&mappingOwningKeys,
                                            0, 0,
                                            basicAllocator);
    *(mappingOwningKeys.size())     = 0;
    elements->push_back(Datum::adoptMapOwningKeys(mappingOwningKeys));    // 37

    elements->push_back(Datum::copyBinary("abcd", 4, basicAllocator));

    for (int i = 0; i < comparableDecimal64Values; ++i) {
        elements->push_back(Datum::createDecimal64(decimal64Value[i].value,
                                                   basicAllocator));
    }
}

bsl::size_t nextValue(bsl::string *value, const bsl::string& input)
    // Extract the next value from a list of comma separated values in the
    // specified 'input' string and load it in the specified 'value'.  Return
    // the index of the next value within 'input'.
{
    if (!input.empty()) {
        int start = 0;
        bsl::size_t nextIndex = input.find(',', start);
        if (bsl::string::npos != nextIndex) {
            *value = input.substr(start, nextIndex - start);
        }
        else {
            *value = input.substr(start);
        }
        return nextIndex;
    }
    return bsl::string::npos;
}

Datum convertToDatum(const bsl::string&  value,
                     bslma::Allocator   *basicAllocator)
    // Convert the specified 'value' into the appropriate type of scalar value
    // and then create and return a 'Datum' object using that value.  Use the
    // specified 'basicAllocator' to allocate memory.
{
    bool isInteger = true;
    bool isDouble = false;
    bool isBoolean = false;
    for (int i = 0; i < value.size(); ++i) {
        if (!isdigit(value[i])) {
            if ('.' == value[i] && !isDouble) {
                isDouble = true;
                isInteger = false;
                continue;
            }
            isInteger = false;
            isDouble = false;
            break;
        }
    }

    if (!isInteger && !isDouble) {
        if ("true" == value || "false" == value) {
            isBoolean = true;
        }
    }

    if (isInteger) { // integer value
        return Datum::createInteger(atoi(value.c_str()));
    }
    else if (isDouble) { // double value
        return Datum::createDouble(atof(value.c_str()));
    }
    else if (isBoolean) { // boolean value
        return Datum::createBoolean("true" == value ? true : false);
    }
    else { // string value
        return Datum::copyString(value, basicAllocator);
    }
}

bsls::Types::size_type getNewCapacity(bsls::Types::size_type capacity,
                                      bsls::Types::size_type length)
     // Calculate the new capacity needed to accommodate data having the
     // specified 'length' for the datum array having the specified 'capacity'.
{
    // Maximum length of array that is supported.
    static const bsl::size_t MAX_ARRAY_LENGTH =
                                        (~bsl::size_t(0) / 2) / sizeof(Datum);
    if (length > MAX_ARRAY_LENGTH / 2) {
        capacity = MAX_ARRAY_LENGTH / 2;
    }
    else {
        capacity += !capacity;
        while (capacity < length) {
            capacity *= 2;
        }
    }
    return capacity;
}

static void createArrayStorage(DatumArrayRef          *array,
                               bsls::Types::size_type  capacity,
                               bslma::Allocator       *basicAllocator)
    // Load the specified 'array' with a reference to newly created datum array
    // having the specified 'capacity', using the specified specified
    // 'basicAllocator'.
{
    Datum::createUninitializedArray(array,
                                    capacity,
                                    basicAllocator);
    // Initialize the memory.
    bsl::uninitialized_fill_n(array->data(), capacity, Datum());
}

bsl::string randomStringGenerator(bsls::Types::size_type length)
    // Return a random string of the specified 'length'.
{
    static bsl::string charSet = "abcdefghijklmnopqrstuvwxyz"
                                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "1234567890";
    bsl::string result;
    result.resize(length);

    for (bsls::Types::size_type i = 0; i < length; ++i) {
        result[i] = charSet[rand() % charSet.length()];
    }

    return result;
}

                            // =======================
                            // class DatumArrayBuilder
                            // =======================

class DatumArrayBuilder {
    // This 'class' provides a utility to build a 'Datum' object holding an
    // array of 'Datum' objects.

public:
    // TYPES
    typedef bsls::Types::size_type SizeType;
        // 'SizeType' is an alias for a signed value, representing the capacity
        // or length of a datum array.

private:
    // DATA
    DatumArrayRef     d_array;        // mutable access to the datum array
    SizeType          d_capacity;     // capacity of the datum array
    bslma::Allocator *d_allocator_p;  // allocator for memory

private:
    // NOT IMPLEMENTED
    DatumArrayBuilder(const DatumArrayBuilder&);
    DatumArrayBuilder& operator=(const DatumArrayBuilder&);

public:
    // CREATORS
    explicit DatumArrayBuilder(bslma::Allocator *basicAllocator);
        // Create a 'DatumArrayBuilder' object.  Note that this holds a copy of
        // the specified 'basicAllocator' pointer, but does not allocate any
        // memory.  A datum array will be created and memory will be allocated
        // when pushBack/append is called.  The behavior is undefined unless
        // '0 != basicAllocator'.

    DatumArrayBuilder(SizeType          initialCapacity,
                      bslma::Allocator *basicAllocator);
        // Create a 'DatumArrayBuilder' object. This constructor creates a
        // datum array having the specified 'initialCapacity' using the
        // specified 'basicAllocator'.  The behavior is undefined unless
        // '0 < initialCapacity' and '0 != basicAllocator'.

    ~DatumArrayBuilder();
        // Destroy this object.  If this object is holding a datum array that
        // has not been adopted, then the datum array is disposed after
        // destroying each of its elements.

    // MANIPULATORS
    void pushBack(const Datum& value);
        // Append the specified 'value' to the end of the held datum array.  If
        // the datum array is full, a new datum array with larger capacity is
        // allocated and any previous datum array is disposed after copying its
        // elements.  The behavior is undefined if 'value' needs dynamic memory
        // and it was allocated using a different allocator than the one used
        // to construct this object.  The behavior is also undefined if
        // 'commit' has already been called on this object.

    void append(const Datum *values, SizeType length);
        // Append the specified array 'values' having the specified 'length' to
        // the end of the held datum array.  Note that if the datum array is
        // full, a new datum array with larger capacity is allocated and the
        // previous datum array is disposed after copying its elements.  The
        // behavior is undefined unless '0 != length' and '0 != values' and
        // each element in 'values' that needs dynamic memory, is allocated
        // with the same allocator that was used to construct this object.  The
        // behavior is undefined if 'commit' has already been called on this
        // object.

    Datum commit();
        // Return a 'Datum' object holding an array of 'Datum' objects built
        // using 'pushBack' or 'append'.  This method indicates that the caller
        // is finished building the datum array and no further values shall be
        // appended.  It is undefined behavior to call any method on this
        // object after this method has been called.

    // ACCESSORS
    SizeType capacity() const;
        // Return the capacity.  The behavior is undefined if 'commit' has
        // already been called on this object.
};

                            // -----------------------
                            // class DatumArrayBuilder
                            // -----------------------

// CREATORS
DatumArrayBuilder::DatumArrayBuilder(bslma::Allocator *basicAllocator)
: d_capacity(0)
, d_allocator_p(basicAllocator)
{
    BSLS_ASSERT(basicAllocator);
    // Do not create a datum array.  Defer this to the first call to
    // 'pushBack' or 'append'.
}

DatumArrayBuilder::DatumArrayBuilder(SizeType          initialCapacity,
                                     bslma::Allocator *basicAllocator)
: d_capacity(initialCapacity)
, d_allocator_p(basicAllocator)
{
    BSLS_ASSERT(basicAllocator);

    // Do not create a datum array, if 'initialCapacity' is 0.  Defer it to the
    // first call to 'pushBack' or 'append'.
    if (initialCapacity) {
        createArrayStorage(&d_array, d_capacity, d_allocator_p);
    }
}

DatumArrayBuilder::~DatumArrayBuilder()
{
    if (d_array.data()) {
        for (SizeType i = 0; i < *d_array.length(); ++i) {
            Datum::destroy(d_array.data()[i], d_allocator_p);
        }
        Datum::disposeUninitializedArray(d_array, d_allocator_p);
    }
}

// MANIPULATORS
void DatumArrayBuilder::pushBack(const Datum& value)
{
    append(&value, 1);
}

void DatumArrayBuilder::append(const Datum *values,
                               SizeType     length)
{
    BSLS_ASSERT(0 == d_capacity || 0 != d_array.data());

    // Get the new array capacity.
    SizeType newCapacity = d_capacity ?
                      getNewCapacity(d_capacity, *d_array.length() + length) :
                      getNewCapacity(d_capacity, length);

    // If the initial capacity was zero, create an array with the new capacity.
    if (!d_capacity) {
        d_capacity = newCapacity;
        createArrayStorage(&d_array, d_capacity, d_allocator_p);
    }
    else if (d_capacity < newCapacity) {
        // Capacity has to be increased.
        d_capacity = newCapacity;

        // Create a new array with the higher capacity.
        DatumArrayRef array;
        createArrayStorage(&array, d_capacity, d_allocator_p);

        // Copy the existing data and dispose the old array.
        *array.length() = *d_array.length();
        bsl::memcpy(array.data(),
                    d_array.data(),
                    sizeof(Datum) * (*d_array.length()));
        Datum::disposeUninitializedArray(d_array, d_allocator_p);
        d_array = array;
    }

    // Copy the new elements.
    bsl::memcpy(d_array.data() + *d_array.length(),
                values,
                sizeof(Datum) * length);
    *d_array.length() += length;
}

Datum DatumArrayBuilder::commit()
{
    Datum result = Datum::adoptArray(d_array);
    d_array = DatumArrayRef();
    d_capacity = 0;
    return result;
}

// ACCESSORS
DatumArrayBuilder::SizeType DatumArrayBuilder::capacity() const
{
    return d_capacity;
}


                           // ===========
                           // TestVisitor
                           // ===========
class TestVisitor {
    // This class provides a visitor to visit and store the type of
    // 'Datum' object with which it was called.

private:
    // INSTANCE DATA
    Datum::DataType d_type; // type of the invoking 'Datum'
                            // object

public:
    // CREATORS
    TestVisitor();
        // Create a 'TestVisitor' object.

    // MANIPULATORS
    void operator()(bslmf::Nil v);
        // Store 'Datum::NIL' in 'd_type'.

    void operator()(const bdlt::Date& v);
        // Store 'Datum::DATE' in 'd_type'.

    void operator()(const bdlt::Datetime& v);
        // Store 'Datum::DATETIME' in 'd_type'.

    void operator()(const bdlt::DatetimeInterval& v);
        // Store 'Datum::DATETIME_INTERVAL' in 'd_type'.

    void operator()(const bdlt::Time& v);
        // Store 'Datum::TIME' in 'd_type'.

    void operator()(bslstl::StringRef v);
        // Store 'Datum::STRING' in 'd_type'.

    void operator()(bool v);
        // Store 'Datum::BOOLEAN' in 'd_type'.

    void operator()(bsls::Types::Int64 v);
        // Store 'Datum::INTEGER64' in 'd_type'.

    void operator()(double v);
        // Store 'Datum::REAL' in 'd_type'.

    void operator()(Error v);
        // Store 'Datum::ERROR_CODE' in 'd_type'.

    void operator()(int v);
        // Store 'Datum::INTEGER' in 'd_type'.

    void operator()(Udt v);
        // Store 'Datum::USERDEFINED' in 'd_type'.

    void operator()(ConstDatumArrayRef v);
        // Store 'Datum::ARRAY' in 'd_type'.

    void operator()(ConstDatumMapRef v);
        // Store 'Datum::MAP' in 'd_type'.

    void operator()(ConstDatumBinaryRef v);
        // Store 'Datum::Binary' in 'd_type'.

    void operator()(Decimal64 v);
        // Store 'Datum::MAP' in 'd_type'.

    // ACCESSORS
    Datum::DataType type() const;
        // Return the type of 'Datum' object with which this visitor
        // was called.
};

                           // -----------
                           // TestVisitor
                           // -----------

TestVisitor::TestVisitor()
{
    d_type = Datum::k_NUM_TYPES;
}

void TestVisitor::operator()(bslmf::Nil v)
{
    d_type = Datum::e_NIL;
}

void TestVisitor::operator()(const bdlt::Date& v)
{
    d_type = Datum::e_DATE;
}

void TestVisitor::operator()(const bdlt::Datetime& v)
{
    d_type = Datum::e_DATETIME;
}

void TestVisitor::operator()(const bdlt::DatetimeInterval& v)
{
    d_type = Datum::e_DATETIME_INTERVAL;
}

void TestVisitor::operator()(const bdlt::Time& v)
{
    d_type = Datum::e_TIME;
}

void TestVisitor::operator()(bslstl::StringRef v)
{
    d_type = Datum::e_STRING;
}

void TestVisitor::operator()(bool v)
{
    d_type = Datum::e_BOOLEAN;
}

void TestVisitor::operator()(bsls::Types::Int64 v)
{
    d_type = Datum::e_INTEGER64;
}

void TestVisitor::operator()(double v)
{
    d_type = Datum::e_REAL;
}

void TestVisitor::operator()(Error v)
{
    d_type = Datum::e_ERROR_VALUE;
}

void TestVisitor::operator()(int v)
{
    d_type = Datum::e_INTEGER;
}

void TestVisitor::operator()(Udt v)
{
    d_type = Datum::e_USERDEFINED;
}

void TestVisitor::operator()(ConstDatumArrayRef v)
{
    d_type = Datum::e_ARRAY;
}

void TestVisitor::operator()(ConstDatumMapRef v)
{
    d_type = Datum::e_MAP;
}

void TestVisitor::operator()(ConstDatumBinaryRef v)
{
    d_type = Datum::e_BINARY;
}

void TestVisitor::operator()(Decimal64 v)
{
    d_type = Datum::e_DECIMAL64;;
}

Datum::DataType TestVisitor::type() const
{
    return d_type;
}

                              // ===============
                              // class Stopwatch
                              // ===============

class Stopwatch {
    // Temporary replacement for bsls::Stopwatch which delivers very unstable
    // user times. Only a subset of the interface is implemented.
    // See DRQS 60392965.

  private:
    // PRIVATE CLASS METHODS
    static double getCPUTime();
        // From http://nadeausoftware.com/articles/2012/03/c_c_tip_how_measure_cpu_time_benchmarking
        // Returns the amount of CPU time used by the current process,
        // in seconds, or -1.0 if an error occurred.

    double d_userTime, d_accumulatedUserTime;

  public:
    // CREATORS
    Stopwatch() {
        // See bsls::Stopwtach.
        reset();
    }

    // MANIPULATORS
    void reset() {
        // See bsls::Stopwtach.
        d_accumulatedUserTime = 0;
    }

    void start(bool) {
        // See bsls::Stopwtach.
        d_userTime = getCPUTime();
    }

    void stop() {
        // See bsls::Stopwtach.
        d_accumulatedUserTime += getCPUTime() - d_userTime;
    }

    // ACCESSORS
    double accumulatedUserTime() const {
        // See bsls::Stopwtach.
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
        if ( FileTimeToSystemTime( &userTime, &userSystemTime ) != -1 )
            return (double)userSystemTime.wHour * 3600.0 +
                (double)userSystemTime.wMinute * 60.0 +
                (double)userSystemTime.wSecond +
                (double)userSystemTime.wMilliseconds / 1000.0;
    }

#elif defined(__unix__) || defined(__unix) || defined(unix) \
    || (defined(__APPLE__) && defined(__MACH__))
    /* AIX, BSD, Cygwin, HP-UX, Linux, OSX, and Solaris --------- */

#if _POSIX_TIMERS > 0
    /* Prefer high-res POSIX timers, when available. */
    {
        clockid_t id;
        struct timespec ts;
#if _POSIX_CPUTIME > 0
        /* Clock ids vary by OS.  Query the id, if possible. */
        if ( clock_getcpuclockid( 0, &id ) == -1 )
#endif
#if defined(CLOCK_PROCESS_CPUTIME_ID)
            /* Use known clock id for AIX, Linux, or Solaris. */
            id = CLOCK_PROCESS_CPUTIME_ID;
#elif defined(CLOCK_VIRTUAL)
        /* Use known clock id for BSD or HP-UX. */
        id = CLOCK_VIRTUAL;
#else
        id = (clockid_t)-1;
#endif
        if ( id != (clockid_t)-1 && clock_gettime( id, &ts ) != -1 )
            return (double)ts.tv_sec +
                (double)ts.tv_nsec / 1000000000.0;
    }
#endif

#if defined(RUSAGE_SELF)
    {
        struct rusage rusage;
        if ( getrusage( RUSAGE_SELF, &rusage ) != -1 )
            return (double)rusage.ru_utime.tv_sec +
                (double)rusage.ru_utime.tv_usec / 1000000.0;
    }
#endif

#if defined(_SC_CLK_TCK)
    {
        const double ticks = (double)sysconf( _SC_CLK_TCK );
        struct tms tms;
        if ( times( &tms ) != (clock_t)-1 )
            return (double)tms.tms_utime / ticks;
    }
#endif

#if defined(CLOCKS_PER_SEC)
    {
        clock_t cl = clock( );
        if ( cl != (clock_t)-1 )
            return (double)cl / (double)CLOCKS_PER_SEC;
    }
#endif

#endif

    return -1.0;		/* Failed. */
}

                             // -----------------
                             // Benchmark_Visitor
                             // -----------------

class Benchmark_Visitor {
    // This component-local class provides a visitor to perform a simple, cheap
    // operation on a Datum for benchmarking purpose.

  private:
    // DATA
    double d_sum;

  public:
    // CREATORS
    Benchmark_Visitor() : d_sum(0) { }

    template<class T>
    void operator()(const T& v)
    {
    }

    void operator()(const bdlt::Date& v)
    {
        d_sum += v.day();
    }

    void operator()(const bdlt::Datetime& v)
    {
        d_sum += v.second();
    }

    void operator()(const bdlt::DatetimeInterval& v)
    {
        d_sum += v.seconds();
    }

    void operator()(const bdlt::Time& v)
    {
        d_sum += v.second();
    }

    void operator()(const bslstl::StringRef& v)
    {
        d_sum += v.length();
    }

    void operator()(bool v)
    {
        d_sum += v;
    }

    void operator()(bsls::Types::Int64 v)
    {
        d_sum += v;
    }

    void operator()(double v)
    {
        d_sum += v;
    }

    void operator()(const Error& v)
    {
        d_sum += v.code();
    }

    void operator()(int v)
    {
        d_sum += v;
    }
};

                              // --------------
                              // BenchmarkSuite
                              // --------------

class BenchmarkSuite {
    // This class provides a visitor to visit and store the type of
    // 'Datum' object with which it was called.

  private:
    // INSTANCE DATA
    bsl::vector<bool>          d_active;     // Vector of active benchmarks.
    int                        d_iterations; // Number of iterations.
    int                        d_current;    // Current benchmark.
    double                     d_scale;      // For conversion to nanoseconds.
    bslma::TestAllocator       d_ta;
    bsl::vector<char>          d_buf;

    static const int           k_DATUMS     = 1000;
    static const int           k_ALLOC_SIZE = k_DATUMS * 100;

    bool next();
        // Move to the next benchmark. Return true if it should be run.

    void runVisit();
        // Run the visit benchmarks.

    void write(const char *label, double value) const;
        // Write 'label and 'value' to standard output.

    void write(int index, const char *label, double value) const;
        // Write 'index', 'label and 'value' to standard output.

  public:
    // CREATORS
    BenchmarkSuite();
        // Create a 'BenchmarkSuite' object having the default value.  Note
        // that this method's definition is compiler generated.

    // MANIPULATORS
    void run(int iterations, int activeCount, char *activeArg[]);
        // Run all the benchmarks.
};

struct Udt_Def : Udt {
    // A Udt with a default constructor.

    Udt_Def() : Udt(NULL, 0) { }

    Udt& operator =(const Udt& udt) {
        return *static_cast<Udt*>(this) = udt;
    }
};

BenchmarkSuite::BenchmarkSuite() : d_buf(&d_ta)
{
}

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
        if (benchmark >= d_active.size()) {
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
    bool bools[k_DATUMS];

    cout.setf(ios::fixed);
    cout.precision(1);

#define BENCHMARK(CREATOR, TESTER, GETTER, TYPE)                              \
    if (next()) {                                                             \
        Stopwatch csw, tsw, gsw, dsw;                                         \
        for (int j = 0; j < d_iterations; ++j) {                              \
            bdlma::BufferedSequentialAllocator alloc(d_buf.begin(),            \
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
              isInteger64(), theInteger64(), Int64);

    BENCHMARK(createDouble(1.23), isDouble(), theDouble(), double);

    {
        unsigned char buffer[8]; // big enough fox max encoding size

#if defined(BSLS_PLATFORM_CPU_32_BIT)
        Decimal64 aSmallDecimal64(BDLDFP_DECIMAL_DD(1.));
        BSLS_ASSERT(bdldfp::DecimalConvertUtil::
                    decimal64ToVariableWidthEncoding(buffer,
                                                     aSmallDecimal64)
                    <= buffer + 6);
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
        BENCHMARK(createDecimal64(aDecimal64,
                                  &alloc),
                  isDecimal64(),
                  theDecimal64(),
                  Decimal64);
    }

    BENCHMARK(copyString("1", &alloc), isString(), theString(),
              StringRef);

    BENCHMARK(
        copyString("1", &alloc), isString(), theString(), StringRef);

    BENCHMARK(
        copyString("12", &alloc), isString(), theString(), StringRef);

    BENCHMARK(
        copyString("123", &alloc), isString(), theString(), StringRef);

    BENCHMARK(copyString("1234",
                         &alloc),
              isString(),
              theString(),
              StringRef);

    BENCHMARK(
        copyString("12345", &alloc),
        isString(), theString(), StringRef);

    BENCHMARK(
        copyString("123456", &alloc),
        isString(), theString(), StringRef);

#if defined(BSLS_PLATFORM_CPU_64_BIT)
    BENCHMARK(
        copyString("0123456789abcde", &alloc),
        isString(), theString(), StringRef);
#else
    ++d_current;
#endif
    BENCHMARK(copyString("abcdefghijklmnopqrstuvwxyz", &alloc),
              isString(), theString(), StringRef);


    BENCHMARK(createStringRef("12345678", 8, &alloc),
              isString(), theString(), StringRef);

    if (next()) {
        bdlma::BufferedSequentialAllocator alloc(d_buf.begin(),
                                                k_ALLOC_SIZE);
        Stopwatch csw;
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
            csw.stop();
        }

        write(d_current,
               "createUninitializedString",
               csw.accumulatedUserTime() / d_scale);
        cout << "\n";
    }

    bdlt::Date aDate(2010, 1, 5);
    BENCHMARK(createDate(aDate), isDate(), theDate(), bdlt::Date);

    bdlt::Time aTime(16, 45, 32, 12);
    BENCHMARK(createTime(aTime), isTime(), theTime(), bdlt::Time);

    bdlt::Datetime aDatetime(3000000000UL, 1, 5, 16, 45, 32, 12);
    BENCHMARK(createDatetime(aDatetime, &alloc),
              isDatetime(), theDatetime(), bdlt::Datetime);

    bdlt::Datetime aNearDatetime(bdlt::CurrentTime::utc());
    BENCHMARK(createDatetime(aNearDatetime, &alloc),
              isDatetime(), theDatetime(), bdlt::Datetime);

    bdlt::DatetimeInterval aDatetimeInterval(34, 16, 45, 32, 12);
    BENCHMARK(
        createDatetimeInterval(aDatetimeInterval, &alloc),
        isDatetimeInterval(),
        theDatetimeInterval(),
        bdlt::DatetimeInterval
        );

    BENCHMARK(createError(12), isError(), theError(), Error);

    BENCHMARK(createError(12, "error", &alloc),
              isError(), theError(), Error);

    BENCHMARK(createUdt(NULL, 0), isUdt(), theUdt(), Udt_Def);

    {
        static Datum array[100 * 1000];

        BENCHMARK(createArrayReference(array,
                                       100,
                                       &alloc),
                  isArray(),
                  theArray(),
                  ConstDatumArrayRef);

        BENCHMARK(createArrayReference(array,
                                       100 * 1000,
                                       &alloc),
                  isArray(),
                  theArray(),
                  ConstDatumArrayRef);
    }

    runVisit();

    cout << "/// END BENCHMARK\n";
}

void BenchmarkSuite::runVisit()
{
    bdlma::BufferedSequentialAllocator alloc(d_buf.begin(),
                                            k_ALLOC_SIZE);

    vector<Datum> combo;
    combo.push_back(Datum::copyString("1", &alloc));
    combo.push_back(Datum::copyString("abcdefghijklmnopqrstuvwxyz",
                                      &alloc));
    combo.push_back(Datum::createBoolean(true));
    combo.push_back(Datum::createDate(bdlt::Date(2010, 1, 5)));
    combo.push_back(Datum::createDouble(1));
    combo.push_back(Datum::createError(1));
    combo.push_back(Datum::createInteger(1));
    combo.push_back(Datum::createInteger64(1LL, &alloc));

    Stopwatch comboTime;
    typedef vector<Datum>::const_iterator ConstDatumIter;

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
                      case Datum::e_REAL:
                        sum += iter->theDouble();
                        break;
                      case Datum::e_STRING:
                        sum += iter->theString().length();
                        break;
                      case Datum::e_BOOLEAN:
                        sum += iter->theBoolean();
                        break;
                      case Datum::e_ERROR_VALUE:
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
                        sum += iter->theDatetimeInterval()
                            .seconds();
                        break;
                      case Datum::e_INTEGER64:
                        sum += iter->theInteger64();
                        break;
                    }
                }
            }
        }

        comboTime.stop();

        write(
            d_current,
            "Process heterogeneous array (switch on type)",
            comboTime.accumulatedUserTime() / combo.size() / d_scale);
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

        write(d_current,
              "Process heterogeneous array (visitor)",
              comboTime.accumulatedUserTime()
              / combo.size() / d_scale);
    }
}

bool BenchmarkSuite::next()
{
    ++d_current;

    return d_active.empty()
        || d_current < d_active.size() && d_active[d_current];
}

void BenchmarkSuite::write(const char *label, double value) const
{
    cout << setw(80) << label // justify label for console output readibility
         << "\t"              // make it easy to copy-paste to Excel
         << setw(6) << value  // justify value for console output readibility
         << "\n";
}

void BenchmarkSuite::write(int index, const char *label, double value) const
{
    cout << setw(2) << index << ')' // justify index and label
         << setw(77) << label       // for console output readibility
         << "\t"                    // make it easy to copy-paste to Excel
         << setw(6) << value        // justify value for console output
         << "\n";
}


                                // ============
                                // ArrayProctor
                                // ============
class ArrayProctor {
    // This 'class' provides a specialized proctor object that, upon
    // destruction and unless the 'release' method has been called,
    // destroys the elements in an array of 'Datum' objects. The
    // elements destroyed are delimited by the "guarded" range
    // '[ begin(), end() )'.

private:
    // INSTANCE DATA
    Datum *d_begin_p;  // start of the range in the array to be managed
    Datum *d_end_p;    // one past the end address in the range to be managed
    bslma::Allocator *d_allocator_p; // allocator used for the array

    // NOT IMPLEMENTED
    ArrayProctor(const ArrayProctor&);
    ArrayProctor& operator=(const ArrayProctor&);

private:
    // PRIVATE MANIPULATORS
    void destroy();
        // Destroy the contiguous sequence of 'Datum' objects managed by
        // this range proctor (if any) by invoking
        // 'Datum::destroy' on each (managed) object.

public:
    // CREATORS
    ArrayProctor(Datum *begin, Datum *end, bslma::Allocator *basicAllocator);
        // Create an array exception guard object for the array of
        // 'Datum' objects delimited by the  range specified by
        // '[ begin, end )'. The memory inside the array is managed using the
        // specified 'basicAllocator'. The behavior is undefined unless
        // '0 != begin', '0 != basicAllocator', 'begin <= end' (if '0 != end'),
        // and each element in the range '[ begin, end )' has been initialized.

    ~ArrayProctor();
        // Destroy this range proctor along with the contiguous sequence of
        // 'Datum' objects it manages (if any) by invoking
        // 'Datum::destroy' on each (managed) object.

    // MANIPULATORS
    Datum *moveEnd(bsl::ptrdiff_t offset = 1);
        // Move the end pointer by the specified 'offset', and return the new
        // end pointer.

    void release();
        // Set the range of elements guarded by this object to be empty. Note
        // that 'd_begin_p == d_end_p' following this operation, but the
        // specific value is unspecified.
};

                                // ------------
                                // ArrayProctor
                                // ------------

ArrayProctor::ArrayProctor(Datum            *begin,
                           Datum            *end,
                           bslma::Allocator *basicAllocator)
: d_begin_p(begin)
, d_end_p(end)
, d_allocator_p(basicAllocator)
{
    BSLS_ASSERT_SAFE(begin);
    BSLS_ASSERT_SAFE(!end || begin <= end);
    BSLS_ASSERT_SAFE(basicAllocator);
}

ArrayProctor::~ArrayProctor()
{
    destroy();
}

Datum *ArrayProctor::moveEnd(bsl::ptrdiff_t offset)
{
    d_end_p += offset;
    return d_end_p;
}

void ArrayProctor::release()
{
    d_begin_p = d_end_p;
}

void ArrayProctor::destroy()
{
    BSLS_ASSERT(d_begin_p <= d_end_p);

    for (Datum *ptr = d_begin_p; ptr < d_end_p; ++ptr) {
        Datum::destroy(*ptr, d_allocator_p);
    }
}

                                  // ==========
                                  // MapProctor
                                  // ==========
class MapProctor {
    // This 'class' provides a specialized proctor object that, upon
    // destruction and unless the 'release' method has been called, destroys
    // the elements in a map of 'Datum' objects keyed by string keys where the
    // keys may or may not be owned.  The elements destroyed are delimited by
    // the "guarded" range '[ begin(), end() )'.

private:
    // INSTANCE DATA

    DatumMapEntry    *d_begin_p;     // starting address of the range in
                                     // the map to be managed
    DatumMapEntry    *d_end_p;       // address of the first element beyond
                                     // the last element of the range in
                                     // the map to be managed
    bslma::Allocator *d_allocator_p; // allocator used to manage memory
                                     // for the map of 'Datum' objects

    // NOT IMPLEMENTED
    MapProctor(const MapProctor&);
    MapProctor& operator=(const MapProctor&);

private:
    // PRIVATE MANIPULATORS
    void destroy();
        // Destroy the contiguous sequence of 'DatumMapEntry' objects
        // managed by this range proctor (if any) by invoking
        // 'Datum::destroy' on each (managed) object.

public:
    // CREATORS
    MapProctor(DatumMapEntry    *begin,
               DatumMapEntry    *end,
               bslma::Allocator *basicAllocator);
        // Create an exception guard object for the map of 'Datum' objects
        // keyed by string values delimited by the range specified by
        // '[ begin, end )'.  The memory inside the map is managed using the
        // specified 'basicAllocator'.  The behavior is undefined unless
        // '0 != begin', '0 != basicAllocator', 'begin <= end' (if '0 != end'),
        // and each element in the range '[ begin, end )' has been initialized.

    ~MapProctor();
        // Destroy this range proctor along with the 'Datum' objects it
        // manages (if any) by invoking 'Datum::destroy'
        // on each (managed) object.

    // MANIPULATORS
    DatumMapEntry *moveEnd(bsl::ptrdiff_t offset = 1);
        // Move the end pointer by the specified 'offset', and return the new
        // end pointer.

    void release();
        // Set the range of elements guarded by this object to be empty. Note
        // that 'd_begin_p == d_end_p' following this operation, but the
        // specific value is unspecified.
};

                                  // ----------
                                  // MapProctor
                                  // ----------

MapProctor::MapProctor(DatumMapEntry    *begin,
                       DatumMapEntry    *end,
                       bslma::Allocator *basicAllocator)
: d_begin_p(begin)
, d_end_p(end)
, d_allocator_p(basicAllocator)
{
    BSLS_ASSERT_SAFE(begin);
    BSLS_ASSERT_SAFE(!end || begin <= end);
    BSLS_ASSERT_SAFE(basicAllocator);
}

MapProctor::~MapProctor()
{
    destroy();
}

DatumMapEntry *MapProctor::moveEnd(bsl::ptrdiff_t offset)
{
    d_end_p += offset;
    return d_end_p;
}

void MapProctor::release()
{
    d_begin_p = d_end_p;
}

void MapProctor::destroy()
{
    BSLS_ASSERT(d_begin_p <= d_end_p);

    for (DatumMapEntry *ptr = d_begin_p; ptr < d_end_p; ++ptr) {
        Datum::destroy(ptr->value(), d_allocator_p);
    }
}

                            //------------------
                            // createStringArray

Datum createStringArray(const char       **values,
                        size_type          length,
                        bslma::Allocator   *basicAllocator)
    // Create and return a 'Datum' object holding an array of 'Datum' objects
    // having values from the specified array 'values' and having the specified
    // 'length' and using the specified 'basicAllocator' to allocate memory (if
    // needed). The behavior is undefined unless '0 != values',
    // '0 != basicAllocator' and '0 < length'.
{
    ASSERT(values);
    ASSERT(basicAllocator);
    ASSERT(0 < length);

    DatumArrayRef array;
    Datum::createUninitializedArray(&array, length, basicAllocator);
    for (size_type i = 0; i < length; ++i) {
        array.data()[i] = Datum::copyString(values[i], basicAllocator);
    }
    *(array.length()) = length;
    return Datum::adoptArray(array);
}

                             //----------------
                             // createStringMap

Datum createStringMap(const char       **keys,
                      const char       **values,
                      size_type          size,
                      bslma::Allocator  *basicAllocator)
    // Create and load in the specified 'result', a map of 'Datum'
    // objects holding values from the specified array 'values' having the
    // specified 'size', keyed by the specified array 'keys' having the
    // specified 'size' and using the specified 'basicAllocator' to allocate
    // memory (if needed). Store the size of the map as an integer value in the
    // first element of the map. The behavior is undefined unless
    // '0 != result', '0 != keys', '0 != values', '0 != basicAllocator' and
    // '0 < size'. Note that the 'keys' are not deep-copied.
{
    ASSERT(keys);
    ASSERT(values);
    ASSERT(basicAllocator);
    ASSERT(0 < size);

    DatumMapRef mapping;
    Datum::createUninitializedMap(&mapping, size, basicAllocator);
    for (size_type i = 0; i < size; ++i) {
        mapping.data()[i] =
            DatumMapEntry(StringRef(keys[i]),
                          Datum::copyString(values[i], basicAllocator));
    }
    *(mapping.size()) = size;
    return Datum::adoptMap(mapping);
}

bool operator==(bslmf::Nil lhs, bslmf::Nil rhs)
{
    return true;
}

                              // ----------------
                              // createNumberList

Datum createNumberList(int              *numbers,
                       size_type         length,
                       bslma::Allocator *basicAllocator)
    // Return a 'Datum' object having an array of 'Datum' objects
    // populated with integer values from the specified array 'numbers' having
    // the specified 'length' and using the specified 'basicAllocator' to
    // allocate memory (if needed). The behavior is undefined unless
    // '0 != numbers', '0 < length', and '0 != basicAllocator'.
{
    ASSERT(numbers);
    ASSERT(0 < length);
    ASSERT(basicAllocator);

    // Create an uninitialized array of 'Datum' objects of the specified
    // 'length'.
    DatumArrayRef array;
    Datum::createUninitializedArray(&array, length, basicAllocator);
    // Create and assign 'int' 'Datum' objects to the uninitialized
    // array.
    for (size_type i = 0; i < length; ++i) {
        array.data()[i] = Datum::createInteger(numbers[i]);
    }
    *(array.length()) = length;
    return Datum::adoptArray(array);
}

                               // ---------------
                               // createDirectory

Datum createDirectory(const char       **names,
                      Datum             *numbers,
                      size_type          size,
                      bslma::Allocator  *basicAllocator)
    // Return a 'Datum' object having a map of 'Datum' objects
    // populated with the keys in the specified array 'names' and values in the
    // specified array 'numbers' having the specified 'size' using the
    // specified 'basicAllocator' to allocate memory (if needed). The behavior
    // is undefined unless '0 != numbers', '0 != names', '0 < size', and
    // '0 != basicAllocator'.
{
    ASSERT(names);
    ASSERT(numbers);
    ASSERT(0 < size);
    ASSERT(basicAllocator);

    size_type keysSize = 0;
    for (size_type i = 0; i < size; ++i) {
        keysSize += strlen(names[i]);
    }
    // Create an uninitialized map of 'Datum' objects of the specified
    // 'size'.
    DatumMapOwningKeysRef mapping;
    Datum::createUninitializedMapOwningKeys(&mapping,
                                            size,
                                            keysSize,
                                            basicAllocator);
    // Assign 'Datum' objects from 'numbers' to the uninitialized map
    // and copy keys from 'names'.
    char *keys = mapping.keys();
    for (size_type i = 0; i < size; ++i) {
        bsl::memcpy(keys, names[i], strlen(names[i]));
        mapping.data()[i] =
                     DatumMapEntry(StringRef(keys, strlen(keys)), numbers[i]);
        keys += strlen(names[i]);
    }
    *(mapping.size()) = size;
    return Datum::adoptMapOwningKeys(mapping);
}

// Following macros are used to fool emacs indenter and make code and comments
// start on first column, so they can be copy-pasted to component doc.

#define BEGIN_USAGE_EXAMPLE void usageExample()
#define BEGIN_SCOPE {
#define END_SCOPE }

namespace {
BEGIN_USAGE_EXAMPLE
BEGIN_SCOPE;

// --------------------------------------------------------------------
// USAGE EXAMPLE TEST:
//
// Concerns:
//    The usage example provided in the component header file must
//    compile, link, and run on all platforms as shown.
//
// Plan:
//    Incorporate usage example from header into driver, remove
//    leading comment characters, and replace 'assert' with 'ASSERT'.
//
// Testing:
//   USAGE EXAMPLE
// --------------------------------------------------------------------

TESTCASE("USAGE EXAMPLE TEST");
ostringstream cout; // Prevent usage from actually printing to console.
bslma::TestAllocator ta("test", false);
const bslma::TestAllocator& defaultAlloc(ta);
bslma::Allocator *alloc = &ta;

///Usage
///-----
///Example 1: Datum Basics
///-----------------------
// The following examples illustrate the construction, manipulation and
// lifecycle of datums.
//
// Datums are created via a set of static methods called 'createTYPE',
// 'copyTYPE' or 'adoptTYPE' where TYPE is any of the supported
// types. The creation methods take a value and sometimes an allocator.
//..
Datum cityName = Datum::copyString("Springfield",
                                   strlen("Springfield"),
                                   alloc);
//..
// Here a copy of the string is made. Whether the copy is stored right
// inside the datum, or in memory obtained from the allocator
// depends on the length of the string and the platform.
// The type of the value can be queried using the 'isTYPE' methods The
// value is retrieved via the 'theTYPE' methods:
//..
ASSERT(cityName.isString());
ASSERT(cityName.theString() == "Springfield");
//..
// As a convenience, datums can be written to standard streams:
//..
cout << cityName << "\n";
//..
// The form of the output is not specified and may change at any time. Do not
// use in production code.
//..
// The static method 'destroy' releases the memory obtained from the
// allocator, if any. It is not mandatory to call 'destroy', if the
// memory possibly allocated during 'Datum' creation is reclaimed by
// other means (e.g. by destroying the allocator - see below):
//..
Datum::destroy(cityName, alloc);
//..
///
///Example 2: Arrays
///-----------------
//
// The following example illustrates the construction of an array of
// datums. First we create an array of datums:
//..
DatumArrayRef bartArray;
Datum::createUninitializedArray(&bartArray, 3, alloc);
bartArray.data()[0] = Datum::createStringRef("Bart", alloc);
bartArray.data()[1] = Datum::createStringRef("Simpson", alloc);
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
cout << bart.theArray().length() << "\n"; // 3
cout << bart.theArray()[0] << "\n";       // "Bart"
cout << bart << "\n";                     // [ "Bart" "Simpson" 10 ]
//..
// Finally, we destroy the datum, which releases all memory associated
// with the array.
//..
Datum::destroy(bart, alloc);
//..
// Note that the same allocator must be used to create the array, the
// elements, and to destroy the datum.
//
///Example 3: Maps
///---------------
// The following example illustrates the construction of a map of
// datums indexed by string keys. First we create a map of datums:
//..
DatumMapRef lisaMap;
Datum::createUninitializedMap(&lisaMap, 3, alloc);
lisaMap.data()[0] = DatumMapEntry(StringRef("firstName"),
                                  Datum::createStringRef("Lisa",
                                                         alloc));
lisaMap.data()[1] = DatumMapEntry(StringRef("lastName"),
                                  Datum::createStringRef("Simpson",
                                                         alloc));
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
// A DatumMapRef may be adopted by only one datum. If the DatumMapRef is not
// adopted, it must be destroyed via 'disposeUninitializedMap'.
//
// Now, we can access the contents of the map through the datum:
//
//..
cout << lisa.theMap().size() << "\n";             // 3
cout << *lisa.theMap().find("firstName") << "\n"; // "Lisa"
//..
// Finally, we destroy the datum, which releases all memory associated
// with the array
//..
Datum::destroy(lisa, alloc);
//..
// Note that the same allocator must be used to create the map, the
// elements, and to destroy the datum.
///
///Example 4: Mass Destruction
///---------------------------
//
// The following example illustrates an important idiom: the en masse
// destruction of a series of datums allocated in an arena:
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
    DatumArrayRef maggieArray;
    Datum::createUninitializedArray(&maggieArray, 2, &arena);
    maggieArray.data()[0] = Datum::createStringRef("Maggie", &arena);
    maggieArray.data()[1] = Datum::createStringRef("Simpson", &arena);
    *maggieArray.length() = 2;
    Datum maggie = Datum::adoptArray(maggieArray);
} // end of scope
//..
// Here all the allocated memory is lodged in the 'arena' allocator. At the end
// of the scope the memory is freed in a single step. Calling 'destroy' for
// each datum individually is neither necessary nor permitted.
//
///Example 5: User-defined, error and binary types
///-----------------------------------------------
// A datum can contain a user-defined type (Udt), consisting in a pointer and
// an integer. The pointer is held, not owned:
//..
struct Beer { };
struct Donut { };
Beer duff;
enum Type { e_BEER, e_DONUT };
Datum treat = Datum::createUdt(&duff, e_BEER);
ASSERT(treat.isUdt());
Udt content = treat.theUdt();
ASSERT(content.type() == e_BEER);
ASSERT(content.data() == &duff);
//..
// A datum can contain an error, consisting in a code and an optional
// StringRef:
//..
enum { e_NO_MORE_BEER };
Datum error = Datum::createError(e_NO_MORE_BEER, "doh!", alloc);
ASSERT(error.isError());
Error what = error.theError();
ASSERT(what.code() == e_NO_MORE_BEER);
ASSERT(what.message() == "doh!");
Datum::destroy(error, alloc);
//..
// A datum can contain an arbitrary sequence of bytes:
//..
int array[] = { 1, 2, 3 };
Datum bob = Datum::copyBinary(array, sizeof array, alloc);
array[2] = 666;
ASSERT(bob.isBinary());
ConstDatumBinaryRef stuff = bob.theBinary();
ASSERT(stuff.size() == 3 * sizeof(int));
ASSERT(reinterpret_cast<const int*>(stuff.data())[2] == 3);
Datum::destroy(bob, alloc);
//..
// Note that the bytes have been copied.
///

END_SCOPE;
} // anonymous namespace

//=============================================================================
//                               MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    verbose                  = test >= 0 && argc > 2;
    bool veryVerbose         = test >= 0 && argc > 3;
    bool veryVeryVerbose     = test >= 0 && argc > 4;
    bool veryVeryVeryVerbose = test >= 0 && argc > 5;

    bslma::TestAllocator ta("test", veryVeryVeryVerbose);
    const bslma::TestAllocator& defaultAlloc(ta);
    bslma::Allocator *alloc = &ta;

    bsls::Assert::setFailureHandler(assertHandler);

    Int64 numAllocs = 0;

    srand(time(NULL));

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
    case 22: {
      usageExample();
    } break;

    case 21: {
        // --------------------------------------------------------------------
        // MISALIGNED MEMORY ACCESS TEST (only for SUN machines on 32-bit)
        //
        // Concerns:
        //    Creating 'Datum' objects that store long string values using an
        //    allocator that does not allocate aligned memory, should not cause
        //    misaligned memory access error.
        //
        // Plan:
        //    Create 'Datum' objects storing long string values using a
        //    'bdlma::BufferedSequentialAllocator'.  Compile with
        //    '-xmemalign=8s' option on any SUN machine.  Confirm there is no
        //    bus error.
        //
        // Testing:
        //   Datum copyString(const char *, bslma::Allocator *);
        //   Datum copyString(const char *, SizeType, bslma::Allocator *);
        // --------------------------------------------------------------------

        TESTCASE("MISALIGNED MEMORY ACCESS TEST");

        char buf[50];
        bdlma::BufferedSequentialAllocator bufAlloc(buf, sizeof(buf), &ta);
        const char *str = "1234567";  // long string
        const Datum obj1 = Datum::copyString(str, &bufAlloc);
        const Datum obj2 = Datum::copyString(str, strlen(str), &bufAlloc);
        Datum::destroy(obj1, &bufAlloc);
        Datum::destroy(obj2, &bufAlloc);
        ASSERT(0 == ta.status());

    } break;

    case 20: {
        // --------------------------------------------------------------------
        // disposeUninitializedMapOwningKeys() TEST:
        //
        // Concerns:
        //    Confirm that an uninitialized map owning keys is disposed off
        //    properly when this method is called.
        //
        // Plan:
        //    Try to create a 'Datum' object holding a map (owning
        //    keys). Use an allocator with a preset allocation limit that
        //    raises and exception while allocating. Destroy any elements in
        //    the map that were already allocated and call
        //    'disposeUninitializedMapOwningKeys' with the map. Verify that the
        //    memory for the map was released.
        //
        // Testing:
        //   void disposeUninitializedMapOwningKeys(DatumMapEntry *,
        //                                          basicAllocator *);
        // --------------------------------------------------------------------

        TESTCASE("disposeUninitializedMapOwningKeys() TEST");

        TEST("Create an uninitialized map (owning keys)"
             " of Datum objects with a custom allocator"
             " that has preset allocation limit.");

        // Create a test allocator with the number of allocation requests set
        // to a number less than what is required to create the map of
        // 'Datum' objects.
        bslma::TestAllocator customDefaultAlloc(false);
        customDefaultAlloc.setAllocationLimit(4);
        DatumMapOwningKeysRef mapping;
        size_type capacity = 5;
        const char *kys[5] = {"first", "second", "third", "fourth", "fifth"};
        size_type keysCapacity = 0;
        for (size_type i = 0; i < 5; ++i) {
            keysCapacity += strlen(kys[i]);
        }

        try {
            Datum::createUninitializedMapOwningKeys(&mapping,
                                                    capacity,
                                                    keysCapacity,
                                                    &customDefaultAlloc);

            TEST("Initialize the map with elements of different types.");

            // Track the allocated elements and destroy them if any of the
            // allocations below throws.
            MapProctor proctor(mapping.data(),
                               mapping.data(),
                               &customDefaultAlloc);
            char *keys = mapping.keys();
            memcpy(keys, kys[0], strlen(kys[0]));
            mapping.data()[0] =
                DatumMapEntry(StringRef(keys, strlen(kys[0])),
                              Datum::copyString("A long string constant",
                                                &customDefaultAlloc));
            keys += strlen(kys[0]);
            proctor.moveEnd();
            memcpy(keys, kys[1], strlen(kys[1]));
            mapping.data()[1] =
                DatumMapEntry(StringRef(keys, strlen(kys[1])),
                              Datum::copyString("A long string constant",
                                                &customDefaultAlloc));
            keys += strlen(kys[1]);
            proctor.moveEnd();
            memcpy(keys, kys[2], strlen(kys[2]));
            mapping.data()[2] = DatumMapEntry(StringRef(keys, strlen(kys[2])),
                                    Datum::copyString("A long string constant",
                                                      &customDefaultAlloc));
            keys += strlen(kys[2]);
            proctor.moveEnd();
            memcpy(keys, kys[3], strlen(kys[3]));
            mapping.data()[3] =
                DatumMapEntry(StringRef(keys, strlen(kys[3])),
                              Datum::copyString("This will cause exception",
                                                &customDefaultAlloc));
            keys += strlen(kys[3]);
            proctor.moveEnd();
            memcpy(keys, kys[4], strlen(kys[4]));
            mapping.data()[4] =
                DatumMapEntry(StringRef(keys, strlen(kys[4])),
                              Datum::copyString("A long string constant",
                                                &customDefaultAlloc));
            keys += strlen(kys[4]);
            proctor.moveEnd();

            proctor.release();
            *(mapping.size()) = capacity;
        } catch (bslma::TestAllocatorException&) {
            Datum::disposeUninitializedMapOwningKeys(mapping,
                                                     &customDefaultAlloc);
            // Verify that no memory was leaked.
            ASSERT(0 == customDefaultAlloc.status());
        }
    } break;

    case 19: {
        // --------------------------------------------------------------------
        // disposeUninitializedMap() TEST:
        //
        // Concerns:
        //    Confirm that an uninitialized map is disposed off properly when
        //    this method is called.
        //
        // Plan:
        //    Try to create a 'Datum' object holding a map. Use an
        //    allocator with a preset allocation limit that raises and
        //    exception while allocating. Destroy any elements in the map that
        //    were already allocated and call 'disposeUninitializedMap' with
        //    the map. Verify that the memory for the map was released.
        //
        // Testing:
        //   void disposeUninitializedMap(DatumMapEntry *,
        //                                basicAllocator *);
        // --------------------------------------------------------------------

        TESTCASE("disposeUninitializedMap() TEST");

        TEST("Create an uninitialized map of "
             "Datum objects with a custom allocator "
             "that has preset allocation limit.");

        // Create a test allocator with the number of allocation requests set
        // to a number less than what is required to create the map of
        // 'Datum' objects.
        bslma::TestAllocator customDefaultAlloc(false);
        customDefaultAlloc.setAllocationLimit(4);
        DatumMapRef mapping;
        size_type capacity = 5;

        try {
            Datum::createUninitializedMap(&mapping,
                                          capacity,
                                          &customDefaultAlloc);

            TEST("Initialize the map with elements of different types.");

            // Track the allocated elements and destroy them if any of the
            // allocations below throws.
            MapProctor proctor(mapping.data(),
                               mapping.data(),
                               &customDefaultAlloc);
            mapping.data()[0] =
                DatumMapEntry(StringRef("first", strlen("first")),
                              Datum::copyString("A long string constant",
                                                &customDefaultAlloc));

            proctor.moveEnd();
            mapping.data()[1] =
                DatumMapEntry(StringRef("second", strlen("second")),
                              Datum::copyString("A long string constant",
                                                &customDefaultAlloc));

            proctor.moveEnd();
            mapping.data()[2] = DatumMapEntry("third",
                              Datum::copyString("A long string constant",
                                                &customDefaultAlloc));
            proctor.moveEnd();
            mapping.data()[3] =
                DatumMapEntry(StringRef("fourth", strlen("fourth")),
                              Datum::copyString("This will cause exception",
                                                &customDefaultAlloc));
            proctor.moveEnd();
            mapping.data()[4] =
                DatumMapEntry(StringRef("fifth", strlen("fifth")),
                              Datum::copyString("A long string constant",
                                                &customDefaultAlloc));
            proctor.moveEnd();

            proctor.release();
            *(mapping.size()) = capacity;
        } catch (bslma::TestAllocatorException&) {
            Datum::disposeUninitializedMap(mapping, &customDefaultAlloc);
            // Verify that no memory was leaked.
            ASSERT(0 == customDefaultAlloc.status());
        }
    } break;

    case 18: {
        // --------------------------------------------------------------------
        // disposeUninitializedArray() TEST:
        //
        // Concerns:
        //    Confirm that an uninitialized array is disposed off properly
        //    when this method is called.
        //
        // Plan:
        //    Try to create a 'Datum' object holding an array. Use an
        //    allocator with a preset allocation limit that raises and
        //    exception while allocating. Destroy any elements in the array
        //    that were already allocated and call 'disposeUninitializedArray'
        //    with the array. Verify that the memory for the array was
        //    released.
        //
        // Testing:
        //   void disposeUninitializedArray(Datum *, basicAllocator *);
        // --------------------------------------------------------------------

        TESTCASE("disposeUninitializedArray() TEST");

        TEST("Create an uninitialized array of "
             "Datum objects with a custom allocator "
             "that has preset allocation limit.");

        // Create a test allocator with the number of allocation requests set
        // to a number less than what is required to create the array of
        // 'Datum' objects.
        bslma::TestAllocator customDefaultAlloc(false);
        customDefaultAlloc.setAllocationLimit(4);
        DatumArrayRef array;
        size_type capacity = 5;

        try {
            Datum::createUninitializedArray(&array,
                                            capacity,
                                            &customDefaultAlloc);

            TEST("Initialize the array with elements of different types.");

            // Track the allocated elements and destroy them if any of the
            // allocations below throws.
            ArrayProctor proctor(array.data(),
                                 array.data(),
                                 &customDefaultAlloc);
            array.data()[0] = Datum::copyString("A long string constant",
                &customDefaultAlloc);
            proctor.moveEnd();
            array.data()[1] = Datum::copyString("A long string constant",
                &customDefaultAlloc);
            proctor.moveEnd();
            array.data()[2] = Datum::copyString("A long string constant",
                &customDefaultAlloc);
            proctor.moveEnd();
            array.data()[3] = Datum::copyString("This will cause exception",
                                                &customDefaultAlloc);
            proctor.moveEnd();
            array.data()[4] = Datum::copyString("A long string constant",
                                                &customDefaultAlloc);
            proctor.moveEnd();

            proctor.release();
            *(array.length()) = capacity;
        } catch (bslma::TestAllocatorException&) {
            Datum::disposeUninitializedArray(array, &customDefaultAlloc);
            // Verify that no memory was leaked.
            ASSERT(0 == customDefaultAlloc.status());
        }
    } break;

    case 17: {
        // --------------------------------------------------------------------
        // copyMapOwningKeys() EXCEPTION TEST:
        //
        // Concerns:
        //    Confirm that when an out-of-memory exception is thrown while
        //    allocating any element in the map, all the initialized elements
        //    and the whole map are destroyed automatically by the proctor.
        //
        // Plan:
        //    Try to copy a map of 'Datum' objects. Use an allocator
        //    with a preset allocation limit that raises and exception while
        //    allocating. Verify that the map and its initialized elements
        //    are destroyed automatically.
        //
        // Testing:
        //   Datum copyMapOwningKeys(const DatumMapEntry *,
        //                                  bslma::Allocator *);
        // --------------------------------------------------------------------

        TESTCASE("copyMapOwningKeys() EXCEPTION TEST");

        TEST("Create a Datum object holding a "
             "map of Datum object (owning keys).");

        const size_type mapSize  = 5;
        const char *kys[mapSize] = {"first", "second", "third",
                                    "fourth", "fifth"};
        size_type keysSize = 0;
        for (size_type i = 0; i < mapSize; ++i) {
            keysSize += strlen(kys[i]);
        }
        DatumMapOwningKeysRef mapping;
        Datum::createUninitializedMapOwningKeys(&mapping,
                                                mapSize,
                                                keysSize,
                                                alloc);
        char *keys = mapping.keys();
        strncpy(keys, kys[0], strlen(kys[0]));
        mapping.data()[0] =
            DatumMapEntry(StringRef(keys, strlen(kys[0])),
                          Datum::copyString("A long string constant", alloc));
        keys += strlen(kys[0]);
        strncpy(keys, kys[1], strlen(kys[1]));
        mapping.data()[1] =
            DatumMapEntry(StringRef(keys, strlen(kys[1])),
                          Datum::copyString("A long string constant", alloc));
        keys += strlen(kys[1]);
        strncpy(keys, kys[2], strlen(kys[2]));
        mapping.data()[2] =
             DatumMapEntry(StringRef(keys, strlen(kys[2])),
                           Datum::copyString("A long string constant", alloc));
        keys += strlen(kys[2]);
        strncpy(keys, kys[3], strlen(kys[3]));
        mapping.data()[3] =
                      DatumMapEntry(StringRef(keys, strlen(kys[3])),
                           Datum::copyString("A long string constant", alloc));
        keys += strlen(kys[3]);
        strncpy(keys, kys[4], strlen(kys[4]));
        mapping.data()[4] =
          DatumMapEntry(StringRef(keys, strlen(kys[4])),
                        Datum::copyString("This will cause exception", alloc));
        *(mapping.size()) = mapSize;
        Datum obj = Datum::adoptMapOwningKeys(mapping);

        TEST("Copy the previously created Datum"
             " object (holding a map owning keys). Use a custom"
             " allocator with a preset allocation limit.");

        // Create a test allocator with the number of allocation requests set
        // to a number less than what is required to create the map of
        // 'Datum' objects.
        bslma::TestAllocator customDefaultAlloc(false);
        customDefaultAlloc.setAllocationLimit(5);

        try {
            obj.clone(&customDefaultAlloc);
        } catch (bslma::TestAllocatorException&) {
            // Verify that no memory was leaked.
            ASSERT(0 == customDefaultAlloc.status());
        }

        // Destroy the 'Datum' object.
        Datum::destroy(obj, alloc);
    } break;

    case 16: {
        // --------------------------------------------------------------------
        // copyMap() EXCEPTION TEST:
        //
        // Concerns:
        //    Confirm that when an out-of-memory exception is thrown while
        //    allocating any element in the map, all the initialized elements
        //    and the whole map are destroyed automatically by the proctor.
        //
        // Plan:
        //    Try to copy a map of 'Datum' objects. Use an allocator
        //    with a preset allocation limit that raises and exception while
        //    allocating. Verify that the map and its initialized elements
        //    are destroyed automatically.
        //
        // Testing:
        //   Datum copyMap(const DatumMapEntry *,
        //                        bslma::Allocator *);
        // --------------------------------------------------------------------

        TESTCASE("copyMap() EXCEPTION TEST");

        TEST("Create a Datum object holding a map of Datum objects.");

        const size_type mapSize  = 5;
        DatumMapRef mapping;
        Datum::createUninitializedMap(&mapping, mapSize, alloc);
        mapping.data()[0] = DatumMapEntry(StringRef("first", strlen("first")),
                           Datum::copyString("A long string constant", alloc));
        mapping.data()[1] =
            DatumMapEntry(StringRef("second", strlen("second")),
                          Datum::copyString("A long string constant", alloc));
        mapping.data()[2] = DatumMapEntry("third",
                           Datum::copyString("A long string constant", alloc));
        mapping.data()[3] =
            DatumMapEntry("fourth",
                          Datum::copyString("A long string constant", alloc));
        mapping.data()[4] = DatumMapEntry("fifth",
                                          Datum::copyString(
                                                   "This will cause exception",
                                                   alloc));
        *(mapping.size()) = mapSize;
        Datum obj = Datum::adoptMap(mapping);

        TEST("Copy the previously created Datum"
             " object (holding a map). Use a custom allocator "
             " with a preset allocation limit.");

        // Create a test allocator with the number of allocation requests set
        // to a number less than what is required to create the map of
        // 'Datum' objects.
        bslma::TestAllocator customDefaultAlloc(false);
        customDefaultAlloc.setAllocationLimit(5);

        try {
            obj.clone(&customDefaultAlloc);
        } catch (bslma::TestAllocatorException&) {
            // Verify that no memory was leaked.
            ASSERT(0 == customDefaultAlloc.status());
        }

        // Destroy the 'Datum' object.
        Datum::destroy(obj, alloc);
    } break;

    case 15: {
        // --------------------------------------------------------------------
        // copyArray() EXCEPTION TEST:
        //
        // Concerns:
        //    Confirm that when an out-of-memory exception is thrown while
        //    allocating any element in the array, all the initialized
        //    elements and the whole array are destroyed automatically by
        //    the proctor.
        //
        // Plan:
        //    Try to copy an array of 'Datum' objects. Use an allocator
        //    with a preset allocation limit that raises and exception while
        //    allocating. Verify that the array and its initialized elements
        //    elements are destroyed automatically.
        //
        // Testing:
        //   Datum copyArray(const Datum *,
        //                          bslma::Allocator *);
        // --------------------------------------------------------------------

        TESTCASE("copyArray() EXCEPTION TEST");

        TEST("Create a Datum object holding an array of Datum objects.");

        const size_type arraySize  = 5;
        DatumArrayRef array;
        Datum::createUninitializedArray(&array, arraySize, alloc);
        array.data()[0] = Datum::copyString("A long string constant", alloc);
        array.data()[1] = Datum::copyString("A long string constant", alloc);
        array.data()[2] = Datum::copyString("A long string constant", alloc);
        array.data()[3] = Datum::copyString("A long string constant", alloc);
        array.data()[4] = Datum::copyString("This will cause exception",
                                            alloc);
        *(array.length()) = arraySize;
        Datum obj = Datum::adoptArray(array);

        TEST("Copy the previously created Datum"
             " object (holding an array). Use a custom"
             " allocator with a preset allocation limit.");

        // Create a test allocator with the number of allocation requests set
        // to a number less than what is required to copy 'Datum' object
        // holding an array.
        bslma::TestAllocator customDefaultAlloc(false);
        customDefaultAlloc.setAllocationLimit(5);

        try {
            obj.clone(&customDefaultAlloc);
        } catch (bslma::TestAllocatorException&) {
            // Verify that no memory was leaked.
            ASSERT(0 == customDefaultAlloc.status());
        }

        // Destroy the 'Datum' object.
        Datum::destroy(obj, alloc);
    } break;

    case 14: {
        // --------------------------------------------------------------------
        // Datum_MapProctor TEST:
        //
        // Concerns:
        //    Confirm that the managed map is not destroyed automatically by
        //    the proctor when no exception is raised. Also confirm that the
        //    managed array is destroyed automatically by the proctor when an
        //    out-of-memory exception is raised.
        //
        // Plan:
        //    Create a map of 'Datum' objects holding string values.  Make sure
        //    no exception is raised.  Verify that the map and its elements are
        //    not destroyed automatically by checking whether the map has the
        //    expected number of elements and each element has the expected
        //    value.  Create another map of 'Datum' objects holding string
        //    values using an allocator with a preset allocation limit that
        //    raises an exception during allocation. Verify that the map and
        //    its initialized elements are destroyed automatically.
        //
        // Testing:
        //    Datum_MapProctor(void *,
        //                            DatumMapEntry *,
        //                            DatumMapEntry *,
        //                            bslma::Allocator *);
        //    DatumMapEntry *moveEnd(bsl::ptrdiff_t);
        //    void release();
        //    ~Datum_MapProctor();
        // --------------------------------------------------------------------

        TESTCASE("Datum_MapProctor TEST");

        const size_type SIZE = 4;
        const char *keys[SIZE] = { "first", "second", "third", "fourth" };
        const char *values[SIZE] = {
            "This is a",
            "test",
            "for the proctor",
            "class usage."
        };

        {
            TEST("Create a map of 4 Datum"
                 " objects each of which holds character"
                 " strings using Datum_MapProctor to manage the object.");

            Datum result = createStringMap(keys, values, SIZE, alloc);
            bslma::TestAllocator cloneAlloc(veryVeryVeryVerbose);
            Datum copy;
            bool except = false;

            // 'clone' may throw.
            try {
                copy = result.clone(&cloneAlloc);
                TEST("Verify the values in the map.");
                ConstDatumMapRef ref = result.theMap();
                for (size_type i = 0; i < SIZE; ++i) {
                    ASSERT(bslstl::StringRef(ref[i].key()) ==
                                                   bslstl::StringRef(keys[i]));
                    ASSERT(ref[i].value().isString());
                    ASSERT(ref[i].value().theString() ==
                              bslstl::StringRef(values[i], strlen(values[i])));
                }
            } catch (bslma::TestAllocatorException&) {
                TEST("Verify no memory is leaked.");
                except = true;
                // Verify that no memory is leaked.
                ASSERT(0 == cloneAlloc.status());
            }

            TEST("Destroy the Datum object and that no memory was leaked.");
            // Destroy the cloned object, if there was no exception.
            Datum::destroy(result, alloc);
            // Verify that memory was leaked.
            ASSERT(0 == defaultAlloc.status());
            // Destroy the cloned object, if there was no exception.
            if (!except) {
                Datum::destroy(copy, &cloneAlloc);
                // Verify that memory was leaked.
                ASSERT(0 == cloneAlloc.status());
            }
        }
        {
            TEST("Create a map of 4 Datum "
                 "objects each of which holds character "
                 "strings using Datum_MapProctor to "
                 "manage the object.");

            Datum result = createStringMap(keys, values, SIZE, alloc);
            bslma::TestAllocator cloneAlloc(veryVeryVeryVerbose);
            Datum copy;
            bool except = false;

            // 'clone' may throw.
            try {
                copy = result.clone(&cloneAlloc);
                TEST("Verify the values in the map.");
                ConstDatumMapRef ref = result.theMap();
                for (size_type i = 0; i < SIZE; ++i) {
                    ASSERT(bslstl::StringRef(ref[i].key()) ==
                                                   bslstl::StringRef(keys[i]));
                    ASSERT(ref[i].value().isString());
                    ASSERT(ref[i].value().theString() ==
                              bslstl::StringRef(values[i], strlen(values[i])));
                }
            } catch (bslma::TestAllocatorException&) {
                TEST("Verify no memory is leaked.");
                except = true;
                // Verify that no memory is leaked.
                ASSERT(0 == cloneAlloc.status());
            }

            TEST("Destroy the Datum object and that no memory was leaked.");
            // Destroy the cloned object, if there was no exception.
            Datum::destroy(result, alloc);
            // Verify that memory was leaked.
            ASSERT(0 == defaultAlloc.status());
            // Destroy the cloned object, if there was no exception.
            if (!except) {
                Datum::destroy(copy, &cloneAlloc);
                // Verify that memory was leaked.
                ASSERT(0 == cloneAlloc.status());
            }
        }
    } break;

    case 13: {
        // --------------------------------------------------------------------
        // ConstDatumMapRef TEST:
        //
        // Concerns:
        //    Exercise a broad cross-section of value-semantic functionality.
        //
        // Plan:
        //    Create a 'ConstDatumMapRef' object and verify that values
        //    were correctly passed down to the 'd_data_p' and 'd_size' data
        //    members. Also exercise the copy construction functionality and
        //    verify using the equality operators that these objects have the
        //    same value. Verify that streaming operator outputs the correctly
        //    formatted value. Also verify that accessors return the correct
        //    value.  Verify that 'find' returns the expected result for both
        //    sorted and unsorted maps.
        //
        // Testing:
        //    ConstDatumMapRef(const DatumMapEntry *,
        //                     size_type,
        //                     bool);
        //    ConstDatumMapRef(const ConstDatumMapRef&);
        //    const DatumMapEntry *data() const;
        //    size_type size() const;
        //    bool isSorted() const;
        //    const DatumMapEntry& operator[](bsls::Types::size_type) const;
        //    const Datum *find(const bslstl::StringRef&) const;
        //    bool operator==(const ConstDatumMapRef&,
        //                    const ConstDatumMapRef&);
        //    bool operator!=(const ConstDatumMapRef&,
        //                    const ConstDatumMapRef&);
        //    bsl::ostream& operator<<(bsl::ostream&,
        //                             const ConstDatumMapRef&);
        // --------------------------------------------------------------------

        TESTCASE("ConstDatumMapRef TEST");

        const size_type mp1Size = 3;
        const DatumMapEntry mp1[mp1Size] = {
            DatumMapEntry(StringRef("first", strlen("first")),
                          Datum::createInteger(3)),
            DatumMapEntry(StringRef("second", strlen("second")),
                          Datum::createDouble(3.13)),
            DatumMapEntry(StringRef("third", strlen("third")),
                          Datum::copyString("A String", alloc))
        };

        const size_type mp2Size = 1;
        const DatumMapEntry mp2[mp2Size] = {
            DatumMapEntry(StringRef("other", strlen("other")),
                          Datum::createInteger(5))
        };

        const size_type mp3Size = 4;
        const DatumMapEntry mp3[mp3Size] = {
            DatumMapEntry(StringRef("first", strlen("first")),
                          Datum::createInteger(3)),
            DatumMapEntry(StringRef("fourth", strlen("fourth")),
                          Datum::createDouble(3.13)),
            DatumMapEntry(StringRef("second", strlen("second")),
                          Datum::copyString("A String", alloc)),
            DatumMapEntry(StringRef("third", strlen("third")),
                          Datum::createBoolean(true))
        };

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Create ConstDatumMapRef with"
                 " the explicit constructor and verify that it"
                 " has the same array that was initially assigned.");
            const ConstDatumMapRef mref1(mp1, mp1Size, false);
            ASSERT(mref1.size() == mp1Size);
            ASSERT(mref1.isSorted() == false);
            for (size_type i = 0; i < mref1.size(); ++i) {
                LOOP_ASSERT(i, mref1[i] == mp1[i]);
            }

            const ConstDatumMapRef mref2(mp3, mp3Size, true);
            ASSERT(mref2.size() == mp3Size);
            ASSERT(mref2.isSorted() == true);
            for (size_type i = 0; i < mref2.size(); ++i) {
                LOOP_ASSERT(i, mref2.data()[i] == mp3[i]);
            }
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Copy-construct a ConstDatumMapRef object from"
                 " another and verify that they have the same value.");
            const ConstDatumMapRef mref1(mp1, mp1Size, false);
            const ConstDatumMapRef mref2(mref1);
            ASSERT(mref1.size() == mp1Size);
            ASSERT(mref2.size() == mp1Size);
            ASSERT(mref1.isSorted() == false);
            ASSERT(mref2.isSorted() == false);
            for (size_type i = 0; i < mref1.size(); ++i) {
                LOOP_ASSERT(i, mref1[i] == mp1[i]);
                LOOP_ASSERT(i, mref2.data()[i] == mp1[i]);
            }

            const ConstDatumMapRef mref3(mp3, mp3Size, true);
            const ConstDatumMapRef mref4(mref3);
            ASSERT(mref3.size() == mp3Size);
            ASSERT(mref4.size() == mp3Size);
            ASSERT(mref3.isSorted() == true);
            ASSERT(mref4.isSorted() == true);
            for (size_type i = 0; i < mref3.size(); ++i) {
                LOOP_ASSERT(i, mref3[i] == mp3[i]);
                LOOP_ASSERT(i, mref4.data()[i] == mp3[i]);
            }
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Create two ConstDatumMapRef"
                 " objects with the same value and verify that"
                 " they are equal. Create another"
                 " ConstDatumMapRef object with a"
                 " different value and verify that it is not "
                 " equal to the previously created"
                 " ConstDatumRef object.  Verify that the"
                 " sorted flag does not affect equality.");
            const ConstDatumMapRef mref1(mp1, mp1Size, false);
            const ConstDatumMapRef mref2(mp1, mp1Size, true);
            ASSERT(mref1 == mref2);
            const ConstDatumMapRef mref3(mp1, mp2Size, false);
            ASSERT(mref1 != mref3);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Verify that streaming operator outputs"
                 " the correctly formatted value.");
            const ConstDatumMapRef mref(mp1, mp1Size, false);
            bsl::ostringstream out;
            out << mref;
            ASSERT(
              "[ [ first = 3 ] [ second = 3.13 ] [ third = \"A String\" ] ]" ==
              out.str());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Verify that 'find' returns the expected"
                 " result for both sorted and unsorted maps.");
            // randomStringGenerator does not use special characters
            const bsl::string notInMapLess = "$%;*&^"; // less than alpha-num
            const bsl::string notInMapGreater = "{}~"; // more than alpha-num

            const int NUM_ENTRIES = 100;
            bsl::vector<bsl::string> keys(NUM_ENTRIES);
            for (bsls::Types::size_type i = 0; i < NUM_ENTRIES; ++i) {
                keys[i] = randomStringGenerator(i + 1);
            }

            // Unsorted map.
            {
                bsl::vector<DatumMapEntry> entries(NUM_ENTRIES);
                for (bsls::Types::size_type i = 0; i < NUM_ENTRIES; ++i) {
                    Datum value = Datum::createInteger(i + 1);
                    entries[i] = DatumMapEntry(keys[i], value);
                }
                const ConstDatumMapRef mref(entries.data(),
                                            NUM_ENTRIES,
                                            false);
                for (bsls::Types::size_type i = 0; i < NUM_ENTRIES; ++i) {
                    const Datum *value = mref.find(entries[i].key());
                    LOOP_ASSERT(i, *value == entries[i].value());
                }
                ASSERT(0 == mref.find(notInMapLess));
                ASSERT(0 == mref.find(notInMapGreater));
            }

            // Sorted map.
            {
                bsl::sort(keys.begin(), keys.end());
                bsl::vector<DatumMapEntry> entries(NUM_ENTRIES);
                for (bsls::Types::size_type i = 0; i < NUM_ENTRIES; ++i) {
                    Datum value = Datum::createInteger(i + 1);
                    entries[i] = DatumMapEntry(keys[i], value);
                }
                const ConstDatumMapRef mref(entries.data(),
                                            NUM_ENTRIES,
                                            true);
                for (bsls::Types::size_type i = 0; i < NUM_ENTRIES; ++i) {
                    const Datum *value = mref.find(entries[i].key());
                    LOOP_ASSERT(i, *value == entries[i].value());
                }
                ASSERT(0 == mref.find(notInMapLess));
                ASSERT(0 == mref.find(notInMapGreater));
            }
        }

        // Cleanup.
        Datum::destroy(mp1[2].value(), alloc);
        Datum::destroy(mp3[2].value(), alloc);
    } break;

    case 12: {
        // --------------------------------------------------------------------
        // DatumMapEntry TEST:
        //
        // Concerns:
        //    Exercise a broad cross-section of value-semantic functionality.
        //
        // Plan:
        //    Create a 'DatumMapEntry' object and verify that values
        //    were correctly passed down to the 'd_keys' and 'd_value' data
        //    members. Verify that the equality and streaming operators work as
        //    expected. Also verify that manipulators set the correct value and
        //    accessors return the correct value.
        //
        // Testing:
        //    DatumMapEntry(const bslstl::StringRef&, const Datum&);
        //    const bslstl::StringRef& key() const;
        //    const Datum& value() const;
        //    void setKey(const bslstl::StringRef&);
        //    void setValue(const Datum&);
        //    bool operator==(const DatumMapEntry&,
        //                    const DatumMapEntry&);
        //    bool operator!=(const DatumMapEntry&,
        //                    const DatumMapEntry&);
        //    bsl::ostream& operator<<(bsl::ostream&,
        //                             const DatumMapEntry&);
        // --------------------------------------------------------------------

        TESTCASE("DatumMapEntry TEST");

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Create DatumMapEntry with the"
                 " explicit constructor and verify that it"
                 " has the correct value.");
            const DatumMapEntry obj("first", Datum::createInteger(3));
            ASSERT(string(obj.key()) == "first");
            ASSERT(obj.value() == Datum::createInteger(3));
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Test manipulators.");
            DatumMapEntry obj("first", Datum::createInteger(3));
            obj.setKey("second");
            obj.setValue(Datum::createDouble(3.55));
            ASSERT(string(obj.key()) == "second");
            ASSERT(obj.value() == Datum::createDouble(3.55));
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Create two DatumMapEntry "
                 " objects with the same value and verify that"
                 " they are equal. Create another"
                 " DatumMapEntry object with a"
                 " different value and verify that it is not"
                 " equal to the previously created"
                 " DatumMapEntry object.");
            const DatumMapEntry obj1("first", Datum::createInteger(3));
            const DatumMapEntry obj2("first", Datum::createInteger(3));
            ASSERT(obj1 == obj2);
            const DatumMapEntry obj3("third", Datum::createDouble(3.55));
            ASSERT(obj1 != obj3);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Verify that streaming operator outputs"
                 " the correctly formatted value.");
            const DatumMapEntry obj("first", Datum::createInteger(3));
            bsl::ostringstream out;
            out << obj;
            ASSERT(string("[ first = 3 ]") == out.str());
        }
    } break;

    case 11: {
        // --------------------------------------------------------------------
        // Datum_ArrayProctor TEST:
        //
        // Concerns:
        //    Confirm that the managed array is not destroyed automatically
        //    by the proctor when no exception is raised. Also confirm that
        //    the managed array is destroyed automatically by the proctor when
        //    an out-of-memory exception is raised.
        //
        // Plan:
        //    Create an array of 'Datum' objects holding character
        //    string values. Make sure no exception is raised. Verify that the
        //    array and its elements are not destroyed automatically by
        //    checking whether the array has the expected number of elements
        //    and each element has the expected value. Create another array of
        //    'Datum' objects holding string values using an allocator with a
        //    preset allocation limit that raises an exception during
        //    allocation. Verify that the array and its initialized elements
        //    are destroyed automatically.
        //
        // Testing:
        //    Datum_ArrayProctor(void *,
        //                              Datum *,
        //                              Datum *,
        //                              bslma::Allocator *);
        //    Datum *moveEnd(bsl::ptrdiff_t);
        //    void release();
        //    ~Datum_ArrayProctor();
        // --------------------------------------------------------------------

        TESTCASE("Datum_ArrayProctor TEST");

        const size_type SIZE = 4;
        const char *values[SIZE] = {
            "This is a",
            "test",
            "for the proctor",
            "class usage."
        };

        {
            TEST("Create an array of 4 Datum"
                 " objects each of which holds character"
                 " strings using Datum_ArrayProctor to"
                 " manage the object.");

            Datum result = createStringArray(values, SIZE, alloc);
            bslma::TestAllocator cloneAlloc(veryVeryVeryVerbose);
            Datum copy;
            bool except = false;

            // 'clone' may throw.
            try {
                copy = result.clone(&cloneAlloc);
                TEST("Verify the values in the array.");
                ConstDatumArrayRef ref = result.theArray();
                for (size_type i = 0; i < SIZE; ++i) {
                    ASSERT(ref[i].isString());
                    ASSERT(ref[i].theString() ==
                              bslstl::StringRef(values[i], strlen(values[i])));
                }
            } catch (bslma::TestAllocatorException&) {
                TEST("Verify no memory is leaked.");
                except = true;
                // Verify that no memory is leaked.
                ASSERT(0 == cloneAlloc.status());
            }

            TEST("Destroy the Datum object and that no memory was leaked.");
            // Destroy the cloned object, if there was no exception.
            Datum::destroy(result, alloc);
            // Verify that memory was leaked.
            ASSERT(0 == defaultAlloc.status());
            // Destroy the cloned object, if there was no exception.
            if (!except) {
                Datum::destroy(copy, &cloneAlloc);
                // Verify that memory was leaked.
                ASSERT(0 == cloneAlloc.status());
            }
        }

        {
            TEST("Create an array of 4 Datum"
                 " objects each of which holds character"
                 " strings using Datum_ArrayProctor to"
                 " manage the object. Use an allocator with a"
                 " preset allocation limit that is less than"
                 " the number of required allocations.");

            Datum result = createStringArray(values, SIZE, alloc);
            bslma::TestAllocator cloneAlloc(veryVeryVeryVerbose);
            // Change the allocation limit to cause an out of memory exception.
            cloneAlloc.setAllocationLimit(3);
            Datum copy;

            bool except = false;

            // 'clone' may throw.
            try {
                copy = result.clone(&cloneAlloc);
                TEST("Verify the values in the array.");
                ConstDatumArrayRef ref = result.theArray();
                for (size_type i = 0; i < SIZE; ++i) {
                    ASSERT(ref[i].isString());
                    ASSERT(ref[i].theString() ==
                              bslstl::StringRef(values[i], strlen(values[i])));
                }
            } catch (bslma::TestAllocatorException&) {
                TEST("Verify no memory is leaked.");
                except = true;
                // Verify that no memory is leaked.
                ASSERT(0 == cloneAlloc.status());
            }

            TEST("Destroy the Datum object and that no memory was leaked.");
            Datum::destroy(result, alloc);
            // Verify that memory was leaked.
            ASSERT(0 == defaultAlloc.status());
            // Destroy the cloned object, if there was no exception.
            if (!except) {
                Datum::destroy(copy, &cloneAlloc);
                // Verify that memory was leaked.
                ASSERT(0 == cloneAlloc.status());
            }
        }
    } break;

    case 10: {
        // --------------------------------------------------------------------
        // ConstDatumArrayRef TEST:
        //
        // Concerns:
        //    Exercise a broad cross-section of value-semantic functionality.
        //
        // Plan:
        //    Create a 'ConstDatumArrayRef' object and verify that
        //    values were correctly passed down to the 'd_data_p' and 'd_size'
        //    data members. Also exercise the copy construction and assignment
        //    operator functionality and verify using the equality operators
        //    that these objects have the same value. Verify that streaming
        //    operator outputs the correctly formatted value. Also verify that
        //    accessors return the correct value.
        //
        // Testing:
        //    ConstDatumArrayRef(const Datum *, size_type);
        //    ConstDatumArrayRef(const ConstDatumArrayRef&);
        //    ConstDatumArrayRef&
        //        operator=(const ConstDatumArrayRef&);
        //    const Datum *data() const;
        //    size_type length() const;
        //    bool operator==(const ConstDatumArrayRef&,
        //                    const ConstDatumArrayRef&);
        //    bool operator!=(const ConstDatumArrayRef&,
        //                    const ConstDatumArrayRef&);
        //    bsl::ostream& operator<<(bsl::ostream&,
        //                             const ConstDatumArrayRef&);
        // --------------------------------------------------------------------

        TESTCASE("ConstDatumArrayRef TEST");

        const size_type arr1Length = 3;
        const Datum arr1[arr1Length] = {
            Datum::createInteger(3),
            Datum::createDouble(3.13),
            Datum::copyString("A String", alloc)
        };

        const size_type arr2Length = 1;
        const Datum arr2[arr2Length] = {
            Datum::createInteger(5)
        };

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Create ConstDatumArrayRef with"
                 " the explicit constructor and verify that it"
                 " has the same array that was initially assigned.");
            const ConstDatumArrayRef aref(arr1, arr1Length);
            ASSERT(aref.length() == arr1Length);
            for (size_type i = 0; i < aref.length(); ++i) {
                LOOP_ASSERT(i, aref[i] == arr1[i]);
            }
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Copy-construct a ConstDatumArrayRef object from"
                 " another and verify that they have the same value.");
            const ConstDatumArrayRef aref1(arr1, arr1Length);
            const ConstDatumArrayRef aref2(aref1);
            for (size_type i = 0; i < aref1.length(); ++i) {
                LOOP_ASSERT(i, aref1[i] == arr1[i]);
            }
            ASSERT(aref1.length() == arr1Length &&
                   aref2.length() == arr1Length);
            for (size_type i = 0; i < aref2.length(); ++i) {
                LOOP_ASSERT(i, aref2[i] == arr1[i]);
            }
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Assign a ConstDatumArrayRef"
                 " object to another and verify that they have"
                 " the same value.");
            const ConstDatumArrayRef aref1(arr1, arr1Length);
            ConstDatumArrayRef aref2(arr2, arr2Length);
            aref2 = aref1;
            ASSERT(aref1.length() == arr1Length &&
                   aref2.length() == arr1Length);
            for (size_type i = 0; i < aref1.length(); ++i) {
                LOOP_ASSERT(i, aref1[i] == arr1[i]);
                LOOP_ASSERT(i, aref2[i] == arr1[i]);
            }
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Create two ConstDatumArrayRef"
                 " objects with the same value and verify that"
                 " they are equal. Create another"
                 " ConstDatumArrayRef object with a"
                 " different value and verify that it is not"
                 " equal to the previously created"
                 " ConstDatumArrayRef object.");
            const ConstDatumArrayRef aref1(arr1, arr1Length);
            const ConstDatumArrayRef aref2(arr1, arr1Length);
            ASSERT(aref1 == aref2);
            const ConstDatumArrayRef aref3(arr2, arr2Length);
            ASSERT(aref1 != aref3);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Verify that streaming operator outputs"
                 " the correctly formatted value.");
            const ConstDatumArrayRef aref(arr1, arr1Length);
            bsl::ostringstream out;
            out << aref;
            ASSERT(string("[ 3 3.13 \"A String\" ]") == out.str());
        }

        // Cleanup.
        Datum::destroy(arr1[2], alloc);
    } break;

    case 9: {
        // --------------------------------------------------------------------
        // TESTING STREAMING OPERATOR:
        //
        // Concerns:
        //    The output streaming operator should print to the specified
        //    'stream' in the appropriate format.
        //
        // Plan:
        //    Verify that the output streaming operator produces the desired
        //    result when invoked with 'Datum' objects holding different
        //    types of values.
        //
        // Testing:
        //   bsl::ostream& operator<<(bsl::ostream&, const Datum&);
        // --------------------------------------------------------------------

        TESTCASE("TESTING STREAMING OPERATOR");

        {
            TEST("Streaming Datum having null value.");
            ostringstream out;
            out << Datum::createNull();
            ASSERT("[nil]" == out.str());
        }

        {
            TEST("Streaming Datum having integer value.");
            ostringstream out;
            out << Datum::createInteger(3423);
            ASSERT("3423" == out.str());
        }

        {
            TEST("Streaming Datum having boolean value.");
            ostringstream out;
            out << Datum::createBoolean(true);
            ASSERT("true" == out.str());
        }

        {
            TEST("Streaming Datum having error code value.");
            ostringstream out;
            out << Datum::createError(5);
            ASSERT("error(5)" == out.str());
        }

        {
            TEST("Streaming Datum having error code and message values.");
            const Datum obj = Datum::createError(5, "some error", alloc);
            ostringstream out;
            out << obj;
            ASSERT("error(5,'some error')" == out.str());
            Datum::destroy(obj, alloc);
        }

        {
            TEST("Streaming Datum having string value.");
            const Datum obj = Datum::copyString("This is a string", alloc);
            ostringstream out;
            out << obj;
            ASSERT("\"This is a string\"" == out.str());
            Datum::destroy(obj, alloc);
        }

        {
            TEST("Streaming Datum having Date value.");
            ostringstream out;
            out << Datum::createDate(bdlt::Date(2010, 1, 5));
            ASSERT("05JAN2010" == out.str());
        }

        {
            TEST("Streaming Datum having Time value.");
            ostringstream out;
            out << Datum::createTime(bdlt::Time(16, 45, 32, 12));
            ASSERT("16:45:32.012" == out.str());
        }

        {
            TEST("Streaming Datum having DateTime value.");
            const Datum obj = Datum::createDatetime(
                                          bdlt::Datetime(2010,1,5, 16,45,32,12),
                                          alloc);
            ostringstream out;
            out << obj;
            ASSERT("05JAN2010_16:45:32.012" == out.str());
            Datum::destroy(obj, alloc);
        }

        {
            TEST("Streaming Datum having DatetimeInterval value.");
            const Datum obj = Datum::createDatetimeInterval(
                                        bdlt::DatetimeInterval(34, 16,45,32,12),
                                        alloc);
            ostringstream out;
            out << obj;
            ASSERT("+34_16:45:32.012" == out.str());
            Datum::destroy(obj, alloc);
        }

        {
            TEST("Streaming Datum having Int64 value.");
            const Datum obj =
                   Datum::createInteger64(Int64(9223372036854775807LL), alloc);
            ostringstream out;
            out << obj;
            ASSERT(out.str() == "9223372036854775807");
            Datum::destroy(obj, alloc);
        }

        {
            TEST("Streaming Datum having double value.");
            ostringstream out;
            out << Datum::createDouble(-3.1416);
            ASSERT("-3.1416" == out.str());
        }

        {
            TEST("Streaming Datum having infinity double value.");
            ostringstream out;
            out << Datum::createDouble(DBL_INF);
            ostringstream infString;
            infString << DBL_INF;
            ASSERT(infString.str() == out.str());
        }

        {
            TEST("Streaming Datum having NaN double value.");
            ostringstream out;
            out << Datum::createDouble(DBL_QNAN2);
            ostringstream nanString;
            nanString << DBL_QNAN2;
            ASSERT(nanString.str() == out.str());
        }

        {
            TEST("Streaming Datum having Udt value.");
            ostringstream out;
            out << Datum::createUdt(&udtValue, UDT_TYPE);
            ostringstream udtStr;
            udtStr << "user-defined(" << &udtValue << ',' << UDT_TYPE << ')';
            ASSERT(udtStr.str() == out.str());
        }
        {
            TEST("Streaming Datum having array value.");
            const size_type arraySize = 6;
            DatumArrayRef array;
            Datum::createUninitializedArray(&array, arraySize, alloc);
            array.data()[0] = Datum::createInteger(0);
            array.data()[1] = Datum::createDouble(-3.1416);
            array.data()[2] = Datum::copyString("A long string", alloc);
            array.data()[3] = Datum::copyString("Abc", alloc);
            array.data()[4] = Datum::createDate(bdlt::Date(2010,1,5));
            array.data()[5] = Datum::createDatetime(
                                          bdlt::Datetime(2010,1,5, 16,45,32,12),
                                          alloc);
            *(array.length()) = arraySize;
            Datum obj = Datum::adoptArray(array);
            ostringstream out;
            out << obj;
            ASSERT(
     "[ 0 -3.1416 \"A long string\" \"Abc\" 05JAN2010 05JAN2010_16:45:32.012 ]"
     == out.str());
            Datum::destroy(obj, alloc);
        }
        {
            TEST("Streaming Datum having empty array value.");
            DatumArrayRef array;
            Datum obj = Datum::adoptArray(array);
            ostringstream out;
            out << obj;
            ASSERT("[ ]" == out.str());
            Datum::destroy(obj, alloc);
        }
        {
            TEST("Streaming Datum having map value.");
            const size_type mapSize = 2;
            DatumMapRef mapping;
            Datum::createUninitializedMap(&mapping, mapSize, alloc);
            mapping.data()[0] =
                DatumMapEntry(StringRef("key1", strlen("key1")),
                              Datum::createInteger(1));
            mapping.data()[1] =
                DatumMapEntry(StringRef("key2", strlen("key2")),
                                        Datum::createDouble(0.34));
            *(mapping.size()) = mapSize;
            Datum obj = Datum::adoptMap(mapping);
            ostringstream out;
            out << obj;
            ASSERT("[ [ key1 = 1 ] [ key2 = 0.34 ] ]" == out.str());
            Datum::destroy(obj, alloc);
        }
        {
            TEST("Streaming Datum having empty map value.");
            DatumMapRef mapping;
            Datum obj = Datum::adoptMap(mapping);
            ostringstream out;
            out << obj;
            ASSERT("[ ]" == out.str());
            Datum::destroy(obj, alloc);
        }
        {
            TEST("Streaming Datum having map (owning keys) value.");
            const size_type mapSize = 2;
            const char *kys[mapSize] = {"key1", "key2"};
            size_type keysSize = 0;
            for (size_type i = 0; i < mapSize; ++i) {
                keysSize += strlen(kys[i]);
            }
            DatumMapOwningKeysRef mapping;
            Datum::createUninitializedMapOwningKeys(&mapping,
                                                    mapSize,
                                                    keysSize,
                                                    alloc);
            char *keys = mapping.keys();
            strncpy(keys, kys[0], strlen(kys[0]));
            mapping.data()[0] = DatumMapEntry(StringRef(keys, strlen(kys[0])),
                                              Datum::createInteger(1));
            keys += strlen(kys[0]);
            strncpy(keys, kys[1], strlen(kys[1]));
            mapping.data()[1] = DatumMapEntry(StringRef(keys, strlen(kys[1])),
                                              Datum::createDouble(0.34));
            *(mapping.size()) = mapSize;
            Datum obj = Datum::adoptMapOwningKeys(mapping);
            ostringstream out;
            out << obj;
            ASSERT("[ [ key1 = 1 ] [ key2 = 0.34 ] ]" == out.str());
            Datum::destroy(obj, alloc);
        }
        {
            TEST("Streaming Datum having empty map (owning keys) value.");
            DatumMapOwningKeysRef mapping;
            Datum obj = Datum::adoptMapOwningKeys(mapping);
            ostringstream out;
            out << obj;
            ASSERT("[ ]" == out.str());
            Datum::destroy(obj, alloc);
        }

        TEST("Streaming Datum having binary value.");

        SUBTEST("ConstDatumBinaryRef streaming - multiline, indent.");

        {
            bsl::ostringstream out;
            ConstDatumBinaryRef((void*)0xDEADBEEF, 12).print(out, 1, 2);
            ASSERT(out.str() ==
                   "  [\n"
                   "    binary = 0xdeadbeef\n"
                   "    size = 12\n"
                   "  ]\n"
                );
        }

        SUBTEST("ConstDatumBinaryRef streaming - multiline, indent"
                " all but first line.");

        {
            bsl::ostringstream out;
            ConstDatumBinaryRef((void*)0xDEADBEEF, 12).print(out, -1, 2);
            ASSERT(out.str() ==
                   "[\n"
                   "    binary = 0xdeadbeef\n"
                   "    size = 12\n"
                   "  ]\n"
                );
        }

        SUBTEST("ConstDatumBinaryRef streaming - single line.");

        {
            bsl::ostringstream out;
            ConstDatumBinaryRef((void*)0xDEADBEEF, 12).print(out, 0, -1);
            ASSERT(out.str() == "[ binary = 0xdeadbeef size = 12 ]" );
        }


        SUBTEST("ConstDatumBinaryRef streaming - single line, indent.");

        {
            bsl::ostringstream out;
            ConstDatumBinaryRef((void*)0xDEADBEEF, 12).print(out, 1, -2);
            ASSERT(out.str() == "  [ binary = 0xdeadbeef size = 12 ]" );
        }

        SUBTEST("Binary Datum streaming.");

        {
            const unsigned char binary[] =
                "1234567890abcdefghijklmnopqrstuvwxyz";
            const int binarySize = sizeof(binary);
            const Datum obj = Datum::copyBinary(binary, binarySize, &ta);

            bsl::ostringstream expected;

            expected << "[ binary = "
                     << bsl::hex << bsl::showbase // what bslim::Printer does
                     << reinterpret_cast<bsls::Types::UintPtr>(
                                                        obj.theBinary().data())
                     << bsl::dec << " size = "  << obj.theBinary().size()
                     << " ]";

            bsl::ostringstream out;
            out << obj;
            ASSERT(out.str() == expected.str());
            Datum::destroy(obj, &ta);
        }

        for (int i = 0; i < decimal64Values; ++i) {
            TEST("Streaming Datum having ") << decimal64Value[i].label
                << " Decimal64 value.";
            ostringstream out;
            Datum obj(Datum::createDecimal64(decimal64Value[i].value,
                                             &ta));
            out << obj;
            LOOP_ASSERT(i, decimal64Value[i].str == out.str());
            Datum::destroy(obj, &ta);
        }
    } break;

    case 8: {
        // --------------------------------------------------------------------
        // TESTING APPLY FUNCTION:
        //
        // Concerns:
        //    The 'apply' function should call the appropriate overload of
        //    'operator()' on the specified 'visitor'.
        //
        // Plan:
        //    Call 'apply' function with a visitor that just stores the type
        //    of the object with which it is called into a member variable.
        //    Call this on all the different types of 'Datum' objects.
        //    Retrieve the integer value and verify that the appropriate
        //    overload of 'operator()' was called on the visitor.
        //
        // Testing:
        //   template <typename BDLD_VISITOR> void apply(BDLD_VISITOR&) const;
        // --------------------------------------------------------------------
        TESTCASE("TESTING APPLY FUNCTION");

        {
            TEST("Testing apply with Datum having null value.");
            TestVisitor visitor;
            Datum::createNull().apply(visitor);
            ASSERT(Datum::e_NIL == visitor.type());
        }

        {
            TEST("Testing apply with Datum having integer value.");
            TestVisitor visitor;
            Datum::createInteger(3423).apply(visitor);
            ASSERT(Datum::e_INTEGER == visitor.type());
        }

        {
            TEST("Testing apply with Datum having boolean value.");
            TestVisitor visitor;
            Datum::createBoolean(true).apply(visitor);
            ASSERT(Datum::e_BOOLEAN == visitor.type());
        }

        {
            TEST("Testing apply with Datum having error code value.");
            TestVisitor visitor;
            Datum::createError(5).apply(visitor);
            ASSERT(Datum::e_ERROR_VALUE == visitor.type());
        }

        {
            TEST("Testing apply with Datum having error code"
                 " and message values.");
            TestVisitor visitor;
            const Datum obj = Datum::createError(5, "some error", alloc);
            obj.apply(visitor);
            ASSERT(Datum::e_ERROR_VALUE == visitor.type());
            Datum::destroy(obj, alloc);
        }

        {
            TEST("Testing apply with Datum having short string value.");
            TestVisitor visitor;
            Datum::copyString("abc", alloc).apply(visitor);
            ASSERT(Datum::e_STRING == visitor.type());
        }

        {
            TEST("Testing apply with Datum having long string value.");
            TestVisitor visitor;
            const Datum obj = Datum::copyString("This is a long string",
                                                alloc);
            obj.apply(visitor);
            ASSERT(Datum::e_STRING == visitor.type());
            Datum::destroy(obj, alloc);
        }

        {
            TEST("Testing apply with Datum having Date value.");
            TestVisitor visitor;
            Datum::createDate(bdlt::Date(2010, 1, 5)).apply(visitor);
            ASSERT(Datum::e_DATE == visitor.type());
        }

        {
            TEST("Testing apply with Datum having Time value.");
            TestVisitor visitor;
            Datum::createTime(bdlt::Time(16,45,32,12)).apply(visitor);
            ASSERT(Datum::e_TIME == visitor.type());
        }

        {
            TEST("Testing apply with Datum having DateTime value.");
            TestVisitor visitor;
            const Datum obj = Datum::createDatetime(
                                          bdlt::Datetime(2010,1,5, 16,45,32,12),
                                          alloc);
            obj.apply(visitor);
            ASSERT(Datum::e_DATETIME == visitor.type());
            Datum::destroy(obj, alloc);
        }

        {
            TEST("Testing apply with Datum having DatetimeInterval value.");
            TestVisitor visitor;
            const Datum obj = Datum::createDatetimeInterval(
                                        bdlt::DatetimeInterval(34, 16,45,32,12),
                                        alloc);
            obj.apply(visitor);
            ASSERT(Datum::e_DATETIME_INTERVAL == visitor.type());
            Datum::destroy(obj, alloc);
        }

        {
            TEST("Testing apply with Datum having Int64 value.");
            TestVisitor visitor;
            const Datum obj =
                   Datum::createInteger64(Int64(9223372036854775807LL), alloc);
            obj.apply(visitor);
            ASSERT(Datum::e_INTEGER64 == visitor.type());
            Datum::destroy(obj, alloc);
        }

        {
            TEST("Testing apply with Datum having double value.");
            TestVisitor visitor;
            Datum::createDouble(-3.1416).apply(visitor);
            ASSERT(Datum::e_REAL == visitor.type());
        }

        {
            TEST("Testing apply with Datum having infinity double value.");
            TestVisitor visitor;
            Datum::createDouble(DBL_INF).apply(visitor);
            ASSERT(Datum::e_REAL == visitor.type());
        }

        {
            TEST("Testing apply with Datum having NaN double value.");
            TestVisitor visitor;
            Datum::createDouble(DBL_QNAN2).apply(visitor);
            ASSERT(Datum::e_REAL == visitor.type());
        }

        {
            TEST("Testing apply with Datum having Udt value.");
            TestVisitor visitor;
            Datum::createUdt(&udtValue, UDT_TYPE).apply(visitor);
            ASSERT(Datum::e_USERDEFINED == visitor.type());
        }
        {
            TEST("Testing apply with Datum having array value.");
            TestVisitor visitor;
            DatumArrayRef array;
            Datum::createUninitializedArray(&array, 1, alloc);
            array.data()[0] = Datum::createInteger(1);
            *(array.length()) = 1;
            Datum obj = Datum::adoptArray(array);
            obj.apply(visitor);
            ASSERT(Datum::e_ARRAY == visitor.type());
            Datum::destroy(obj, alloc);
        }
        {
            TEST("Testing apply with Datum having empty array value.");
            TestVisitor visitor;
            DatumArrayRef array;
            Datum obj = Datum::adoptArray(array);
            obj.apply(visitor);
            ASSERT(Datum::e_ARRAY == visitor.type());
            Datum::destroy(obj, alloc);
        }
        {
            TEST("Testing apply with Datum having map value.");
            TestVisitor visitor;
            DatumMapRef mapping;
            Datum::createUninitializedMap(&mapping, 1, alloc);
            mapping.data()[0] = DatumMapEntry(StringRef("key", strlen("key")),
                                              Datum::createInteger(1));
            *(mapping.size()) = 1;
            Datum obj = Datum::adoptMap(mapping);
            obj.apply(visitor);
            ASSERT(Datum::e_MAP == visitor.type());
            Datum::destroy(obj, alloc);
        }
        {
            TEST("Testing apply with Datum having empty map value.");
            TestVisitor visitor;
            DatumMapRef mapping;
            Datum obj = Datum::adoptMap(mapping);
            obj.apply(visitor);
            ASSERT(Datum::e_MAP == visitor.type());
            Datum::destroy(obj, alloc);
        }
        {
            TEST("Testing apply with Datum having map (owning keys) value.");
            TestVisitor visitor;
            DatumMapOwningKeysRef mapping;
            Datum::createUninitializedMapOwningKeys(&mapping,
                                                    1,
                                                    strlen("key"),
                                                    alloc);
            char *keys = mapping.keys();
            strncpy(keys, "key", strlen("key"));
            mapping.data()[0] = DatumMapEntry(StringRef(keys, strlen("key")),
                                              Datum::createInteger(1));
            *(mapping.size()) = 1;
            Datum obj = Datum::adoptMapOwningKeys(mapping);
            obj.apply(visitor);
            ASSERT(Datum::e_MAP == visitor.type());
            Datum::destroy(obj, alloc);
        }
        {
            TEST("Testing apply with Datum having empty map"
                 " (owning keys) value.");
            TestVisitor visitor;
            DatumMapOwningKeysRef mapping;
            Datum obj = Datum::adoptMapOwningKeys(mapping);
            obj.apply(visitor);
            ASSERT(Datum::e_MAP == visitor.type());
            Datum::destroy(obj, alloc);
        }

        {
            TEST("Testing apply with Datum having binary value.");
            TestVisitor visitor;
            Datum obj = Datum::copyBinary("abcd", 4, alloc);
            obj.apply(visitor);
            ASSERT(Datum::e_BINARY == visitor.type());
            Datum::destroy(obj, alloc);
        }

        {
            TEST("Testing apply with Datum having Decimal64 value.");

            for (int i = 0; i < decimal64Values; ++i) {
                SUBTEST("Create a ") << decimal64Value[i].label
                                     << " Decimal64 value.";
                TestVisitor visitor;
                Datum obj = Datum::createDecimal64(decimal64Value[i].value,
                                                   alloc);
                obj.apply(visitor);
                LOOP_ASSERT(i, Datum::e_DECIMAL64 == visitor.type());
                Datum::destroy(obj, alloc);
            }
        }
    } break;

    case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //
        // Concerns:
        //    'Datum::clone' should make a deep copy of
        //    the given 'Datum' object. Memory should not be allocated
        //    when values are copied inline.
        //
        // Plan:
        //    Create 'Datum' objects with different types of values.
        //    Create other 'Datum' objects by copying the original ones.
        //    Verify that both objects have the same values. Verify that no
        //    memory was allocated when the copied value was stored inline.
        //
        // Testing:
        //   Datum clone(const Datum&, bslma::Allocator *);
        // --------------------------------------------------------------------

        TESTCASE("TESTING COPY CONSTRUCTOR");

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Copying Datum with a nil value.");

            SUBTEST("Create Datum with a nil value.");
            const Datum obj = Datum::createNull();

            SUBTEST("Create a copy of the Datum.");
            const Datum cObj = obj.clone(alloc);

            SUBTEST("Verify that both objects have the same"
                    " value and no memory was allocated while copying." );
            ASSERT(cObj == obj);
            ASSERT(0 == defaultAlloc.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Copying Datum with an integer value.");

            SUBTEST("Create Datum with an integer value of '1'.");
            const Datum obj = Datum::createInteger(1);

                    SUBTEST("Create a copy of the Datum.");
            const Datum cObj = obj.clone(alloc);

            SUBTEST("Verify that both objects have the same"
                    " value and no memory was allocated while copying.");
            ASSERT(cObj == obj);
            ASSERT(0 == defaultAlloc.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Copying Datum with a boolean value.");

            SUBTEST("Create Datum with a boolean value of 'true'.");
            const Datum obj = Datum::createBoolean(true);

            SUBTEST("Create a copy of the Datum.");
            const Datum cObj = obj.clone(alloc);

            SUBTEST("Verify that both objects have the same"
                    " value and no memory was allocated while copying.");
            ASSERT(cObj == obj);
            ASSERT(0 == defaultAlloc.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        TEST("Copying Datum with an Error value.");

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with an error code value.");
            const Datum obj = Datum::createError(5);

            SUBTEST("Create a copy of the Datum.");
            const Datum cObj = obj.clone(alloc);

            SUBTEST("Verify that both objects have the same"
                    " value and no memory was allocated while copying.");
            ASSERT(cObj == obj);
            ASSERT(0 == defaultAlloc.status());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with an error code value "
                    "and an error message value.");
            const Datum obj = Datum::createError(5, "some error", alloc);

            SUBTEST("Create a copy of the Datum.");
            const Datum cObj = obj.clone(alloc);

            SUBTEST("Verify that both objects have the same value.");
            ASSERT(cObj == obj);

            SUBTEST("Destroy both the objects and verify "
                    "that there are no memory leaks.");
            Datum::destroy(obj, alloc);
            Datum::destroy(cObj, alloc);
            ASSERT(0 == defaultAlloc.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        TEST("Copying Datum with different double values.");

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with a double value -3.1416'.");
            const Datum obj = Datum::createDouble(-3.1416);

            SUBTEST("Create a copy of the Datum.");
            const Datum cObj = obj.clone(alloc);

            SUBTEST("Verify that both objects have the same"
                    " value and no memory was allocated while copying.");
            ASSERT(cObj == obj);
            ASSERT(0 == defaultAlloc.status());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the infinity double value.");
            const Datum obj = Datum::createDouble(DBL_INF);

            SUBTEST("Create a copy of the Datum.");
            const Datum cObj = obj.clone(alloc);

            SUBTEST("Verify that both objects have the same"
                    " value and no memory was allocated while copying.");
            ASSERT(cObj == obj);
            ASSERT(0 == defaultAlloc.status());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the NaN double value.");
            const Datum obj = Datum::createDouble(DBL_QNAN2);

            SUBTEST("Create a copy of the Datum.");
            const Datum cObj = obj.clone(alloc);

            SUBTEST("Verify that both objects do not have "
                    "the same value and no memory was allocated "
                    "while copying.");
            ASSERT(cObj != obj);
            ASSERT(0 == defaultAlloc.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Copying Datum with a Date value.");

            SUBTEST("Create Datum with a date value.");
            const Datum obj = Datum::createDate(bdlt::Date(2010,1,5));

            SUBTEST("Create a copy of the Datum.");
            const Datum cObj = obj.clone(alloc);

            SUBTEST("Verify that both objects have the same "
                    "value and no memory was allocated while copying.");
            ASSERT(cObj == obj);
            ASSERT(0 == defaultAlloc.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Copying Datum with a Time value.");

            SUBTEST("Create Datum with a time value.");
            const Datum obj = Datum::createTime(bdlt::Time(16,45,32,12));

            SUBTEST("Create a copy of the Datum.");
            const Datum cObj = obj.clone(alloc);

            SUBTEST("Verify that both objects have the same"
                    " value and no memory was allocated while copying.");
            ASSERT(cObj == obj);
            ASSERT(0 == defaultAlloc.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        TEST("Copying Datum with a string value.");

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with a short string value.");
            const Datum obj = Datum::copyString("Abc", alloc);

            SUBTEST("Create a copy of the Datum.");
            const Datum cObj = obj.clone(alloc);

            SUBTEST("Verify that both objects have the same"
                    " value and no memory was allocated while copying.");
            ASSERT(cObj == obj);
            ASSERT(0 == defaultAlloc.status());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with a long string value.");
            const Datum obj = Datum::copyString("This is a long string",
                                                alloc);

            SUBTEST("Create a copy of the Datum.");
            const Datum cObj = obj.clone(alloc);

            SUBTEST("Verify that both objects have the same value.");
            ASSERT(cObj == obj);

            SUBTEST("Destroy both the objects and verify "
                    "that there are no memory leaks.");
            Datum::destroy(obj, alloc);
            Datum::destroy(cObj, alloc);
            ASSERT(0 == defaultAlloc.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Copying Datum with a Datetime value.");

            SUBTEST("Create Datum with a date+time value.");
            const Datum obj = Datum::createDatetime(
                                          bdlt::Datetime(2010,1,5, 16,45,32,12),
                                          alloc);

            SUBTEST("Create a copy of the Datum.");
            const Datum cObj = obj.clone(alloc);

            SUBTEST("Verify that both objects have the same value.");
            ASSERT(cObj == obj);

            SUBTEST("Destroy both the objects and verify "
                    "that there are no memory leaks.");
            Datum::destroy(obj, alloc);
            Datum::destroy(cObj, alloc);
            ASSERT(0 == defaultAlloc.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Copying Datum with a DatetimeInterval value.");

            SUBTEST("Create Datum with a date+time interval value.");
            const Datum obj = Datum::createDatetimeInterval(
                                        bdlt::DatetimeInterval(34, 16,45,32,12),
                                        alloc);

            SUBTEST("Create a copy of the Datum.");
            const Datum cObj = obj.clone(alloc);

            SUBTEST("Verify that both objects have the same value.");
            ASSERT(cObj == obj);

            SUBTEST("Destroy both the objects and verify "
                    "that there are no memory leaks.");
            Datum::destroy(obj, alloc);
            Datum::destroy(cObj, alloc);
            ASSERT(0 == defaultAlloc.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Copying Datum with an Int64 value.");

            SUBTEST("Create Datum with an Int64 value.");
            const Datum obj =
                   Datum::createInteger64(Int64(9223372036854775807LL), alloc);

            SUBTEST("Create a copy of the Datum.");
            const Datum cObj = obj.clone(alloc);

            SUBTEST("Verify that both objects have the same value.");
            ASSERT(cObj == obj);

            SUBTEST("Destroy both the objects and verify "
                    "that there are no memory leaks.");
            Datum::destroy(obj, alloc);
            Datum::destroy(cObj, alloc);
            ASSERT(0 == defaultAlloc.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        TEST("Copying Datum with an array of values.");

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with an array value.");
            const size_type arraySize  = 6;
            DatumArrayRef array;
            Datum::createUninitializedArray(&array, arraySize, alloc);
            array.data()[0] = Datum::createInteger(0);
            array.data()[1] = Datum::createDouble(-3.1416);
            array.data()[2] = Datum::copyString("A long string", alloc);
            array.data()[3] = Datum::copyString("Abc", alloc);
            array.data()[4] = Datum::createDate(bdlt::Date(2010,1,5));
            array.data()[5] = Datum::createDatetime(
                                          bdlt::Datetime(2010,1,5, 16,45,32,12),
                                          alloc);
            *(array.length()) = arraySize;
            const Datum obj = Datum::adoptArray(array);

            SUBTEST("Create a copy of the Datum.");
            const Datum cObj = obj.clone(alloc);

            SUBTEST("Verify that both objects have the same value.");
            ASSERT(cObj == obj);

            SUBTEST("Destroy both the objects and verify "
                    "that there are no memory leaks.");
            Datum::destroy(obj, alloc);
            Datum::destroy(cObj, alloc);
            ASSERT(0 == defaultAlloc.status());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with an empty "
                    "array value.");
            DatumArrayRef array;
            const Datum obj = Datum::adoptArray(array);

            SUBTEST("Create a copy of the Datum.");
            const Datum cObj = obj.clone(alloc);

            SUBTEST("Verify that both objects have the same value.");
            ASSERT(cObj == obj);

            SUBTEST("Destroy both the objects and verify "
                    "that there are no memory leaks.");
            Datum::destroy(obj, alloc);
            Datum::destroy(cObj, alloc);
            ASSERT(0 == defaultAlloc.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        TEST("Copying Datum with a map of values.");

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with a map value.");
            const size_type mapSize = 6;
            DatumMapRef mapping;
            Datum::createUninitializedMap(&mapping, mapSize, alloc);
            mapping.data()[0] =
                DatumMapEntry(StringRef("first", strlen("first")),
                              Datum::createInteger(0));
            mapping.data()[1] =
                DatumMapEntry(StringRef("second", strlen("second")),
                              Datum::createDouble(-3.1416));
            mapping.data()[2] =
                DatumMapEntry(StringRef("third", strlen("third")),
                              Datum::copyString("A long string", alloc));
            mapping.data()[3] =
                DatumMapEntry(StringRef("fourth", strlen("fourth")),
                              Datum::copyString("Abc", alloc));
            mapping.data()[4] =
                DatumMapEntry(StringRef("fifth", strlen("fifth")),
                              Datum::createDate(bdlt::Date(2010,1,5)));
            mapping.data()[5] =
                DatumMapEntry(StringRef("sixth", strlen("sixth")),
                              Datum::createDatetime(bdlt::Datetime(2010,1,5,
                                                                  16,45,32,12),
                                                    alloc));
            *(mapping.size()) = mapSize;
            const Datum obj = Datum::adoptMap(mapping);

            SUBTEST("Create a copy of the Datum.");
            const Datum cObj = obj.clone(alloc);

            SUBTEST("Verify that both objects have the same value.");
            ASSERT(cObj == obj);

            SUBTEST("Destroy both the objects and verify "
                    "that there are no memory leaks.");
            Datum::destroy(obj, alloc);
            Datum::destroy(cObj, alloc);
            ASSERT(0 == defaultAlloc.status());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with an empty map value.");
            DatumMapRef mapping;
            const Datum obj = Datum::adoptMap(mapping);

            SUBTEST("Create a copy of the Datum.");
            const Datum cObj = obj.clone(alloc);

            SUBTEST("Verify that both objects have the same value.");
            ASSERT(cObj == obj);

            SUBTEST("Destroy both the objects and verify "
                    "that there are no memory leaks.");
            Datum::destroy(obj, alloc);
            Datum::destroy(cObj, alloc);
            ASSERT(0 == defaultAlloc.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        TEST("Copying Datum with a map (owning keys) value.");

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with a map (owning keys).");
            const size_type mapSize = 6;
            const char *kys[mapSize] = {"fifth", "first", "fourth",
                                        "second", "sixth", "third"};
            size_type keysSize = 0;
            for (size_type i = 0; i < mapSize; ++i) {
                keysSize += strlen(kys[i]);
            }

            DatumMapOwningKeysRef mapping;
            Datum::createUninitializedMapOwningKeys(&mapping,
                                                    mapSize,
                                                    keysSize,
                                                    alloc);
            char *keys = mapping.keys();
            strncpy(keys, kys[0], strlen(kys[0]));
            mapping.data()[0] =
                DatumMapEntry(StringRef(keys, strlen(kys[0])),
                              Datum::createInteger(0));
            keys += strlen(kys[0]);
            strncpy(keys, kys[1], strlen(kys[1]));
            mapping.data()[1] =
                DatumMapEntry(StringRef(keys, strlen(kys[1])),
                              Datum::createDouble(-3.1416));
            keys += strlen(kys[1]);
            strncpy(keys, kys[2], strlen(kys[2]));
            mapping.data()[2] =
                DatumMapEntry(StringRef(keys, strlen(kys[2])),
                              Datum::copyString("A long string", alloc));
            keys += strlen(kys[2]);
            strncpy(keys, kys[3], strlen(kys[3]));
            mapping.data()[3] =
                DatumMapEntry(StringRef(keys, strlen(kys[3])),
                              Datum::copyString("Abc", alloc));
            keys += strlen(kys[3]);
            strncpy(keys, kys[4], strlen(kys[4]));
            mapping.data()[4] =
                DatumMapEntry(StringRef(keys, strlen(kys[4])),
                              Datum::createDate(bdlt::Date(2010,1,5)));
            keys += strlen(kys[4]);
            strncpy(keys, kys[5], strlen(kys[5]));
            mapping.data()[5] =
                DatumMapEntry(StringRef(keys, strlen(kys[5])),
                              Datum::createDatetime(
                                                    bdlt::Datetime(2010,1,5,
                                                                  16,45,32,12),
                                                    alloc));
            *(mapping.size()) = mapSize;
            *(mapping.sorted()) = true;
            const Datum obj = Datum::adoptMapOwningKeys(mapping);

            SUBTEST("Create a copy of the Datum.");
            const Datum cObj = obj.clone(alloc);

            SUBTEST("Verify that both objects have the same value.");
            ASSERT(cObj == obj);

            SUBTEST("Destroy both the objects and verify "
                    "that there are no memory leaks.");
            Datum::destroy(obj, alloc);
            Datum::destroy(cObj, alloc);
            ASSERT(0 == defaultAlloc.status());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with an empty map (owning keys) value.");
            DatumMapOwningKeysRef mapping;
            const Datum obj = Datum::adoptMapOwningKeys(mapping);

            SUBTEST("Create a copy of the Datum.");
            const Datum cObj = obj.clone(alloc);

            SUBTEST("Verify that both objects have the same value.");
            ASSERT(cObj == obj);

            SUBTEST("Destroy both the objects and verify "
                    "that there are no memory leaks.");
            Datum::destroy(obj, alloc);
            Datum::destroy(cObj, alloc);
            ASSERT(0 == defaultAlloc.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Copying Datum with a user defined object.");

            SUBTEST("Create Datum with a user defined object.");
            const Datum obj = Datum::createUdt(&udtValue, UDT_TYPE);

            SUBTEST("Create a copy of the Datum.");
            const Datum cObj = obj.clone(alloc);

            SUBTEST("Verify that both objects have the same value.");
            ASSERT(cObj == obj);

            SUBTEST("Destroy both the objects and verify "
                    "that there are no memory leaks.");
            Datum::destroy(obj, alloc);
            Datum::destroy(cObj, alloc);
            ASSERT(0 == defaultAlloc.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Copying Datum with a array having a map as an element.");

            SUBTEST("Create Datum with an array value.");
            const size_type mapSize = 6;
            DatumMapRef mapping;
            Datum::createUninitializedMap(&mapping, mapSize, alloc);
            mapping.data()[0] =
                DatumMapEntry(StringRef("first", strlen("first")),
                              Datum::createInteger(0));
            mapping.data()[1] =
                DatumMapEntry(StringRef("second", strlen("second")),
                              Datum::createDouble(-3.1416));
            mapping.data()[2] =
                DatumMapEntry(StringRef("third", strlen("third")),
                              Datum::copyString("A long string", alloc));
            mapping.data()[3] =
                DatumMapEntry(StringRef("fourth", strlen("fourth")),
                              Datum::copyString("Abc", alloc));
            mapping.data()[4] =
                DatumMapEntry(StringRef("fifth", strlen("fifth")),
                              Datum::createDate(bdlt::Date(2010,1,5)));
            mapping.data()[5] =
                DatumMapEntry(StringRef("sixth", strlen("sixth")),
                              Datum::createDatetime(
                                          bdlt::Datetime(2010,1,5, 16,45,32,12),
                                          alloc));
            *(mapping.size()) = mapSize;

            const size_type arraySize  = 6;
            DatumArrayRef array;
            Datum::createUninitializedArray(&array, arraySize, alloc);
            array.data()[0] = Datum::createInteger(0);
            array.data()[1] = Datum::createDouble(-3.1416);
            array.data()[2] = Datum::copyString("A long string", alloc);
            array.data()[3] = Datum::copyString("Abc", alloc);
            array.data()[4] = Datum::createDate(bdlt::Date(2010, 1, 5));
            array.data()[5] = Datum::adoptMap(mapping);
            *(array.length()) = arraySize;
            const Datum obj = Datum::adoptArray(array);

            SUBTEST("Create a copy of the Datum.");
            const Datum cObj = obj.clone(alloc);

            SUBTEST("Verify that both objects have the same value.");
            ASSERT(cObj == obj);

            SUBTEST("Destroy both the objects and verify "
                    "that there are no memory leaks.");
            Datum::destroy(obj, alloc);
            Datum::destroy(cObj, alloc);
            ASSERT(0 == defaultAlloc.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Copying Datum with a map having an array as an element.");

            SUBTEST("Create Datum with a map value.");
            const size_type arraySize  = 6;
            DatumArrayRef array;
            Datum::createUninitializedArray(&array, arraySize, alloc);
            array.data()[0] = Datum::createInteger(0);
            array.data()[1] = Datum::createDouble(-3.1416);
            array.data()[2] = Datum::copyString("A long string", alloc);
            array.data()[3] = Datum::copyString("Abc", alloc);
            array.data()[4] = Datum::createDate(bdlt::Date(2010,1,5));
            array.data()[5] = Datum::createDatetime(
                                          bdlt::Datetime(2010,1,5, 16,45,32,12),
                                          alloc);
            *(array.length()) = arraySize;

            const size_type mapSize = 6;
            DatumMapRef mapping;
            Datum::createUninitializedMap(&mapping, mapSize, alloc);
            mapping.data()[0] =
                DatumMapEntry(StringRef("first", strlen("first")),
                              Datum::createInteger(0));
            mapping.data()[1] =
                DatumMapEntry(StringRef("second", strlen("second")),
                              Datum::createDouble(-3.1416));
            mapping.data()[2] =
                DatumMapEntry(StringRef("third", strlen("third")),
                              Datum::copyString("A long string", alloc));
            mapping.data()[3] =
                DatumMapEntry(StringRef("fourth", strlen("fourth")),
                              Datum::copyString("Abc", alloc));
            mapping.data()[4] =
                DatumMapEntry(StringRef("fifth", strlen("fifth")),
                              Datum::createDate(bdlt::Date(2010,1,5)));
            mapping.data()[5] =
                DatumMapEntry(StringRef("sixth", strlen("sixth")),
                              Datum::adoptArray(array));
            *(mapping.size()) = mapSize;
            Datum obj = Datum::adoptMap(mapping);

            SUBTEST("Create a copy of the Datum.");
            const Datum cObj = obj.clone(alloc);

            SUBTEST("Verify that both objects have the same value.");
            ASSERT(cObj == obj);

            SUBTEST("Destroy both the objects and verify "
                    "that there are no memory leaks.");
            Datum::destroy(obj, alloc);
            Datum::destroy(cObj, alloc);
            ASSERT(0 == defaultAlloc.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Copying Datum with a binary value.");

            SUBTEST("Create Datum with a binary value.");
            const unsigned char binary[] =
                "1234567890abcdefghijklmnopqrstuvwxyz";
            const int binarySize = sizeof(binary) - 1;
            const Datum obj = Datum::copyBinary(binary, binarySize, alloc);

            SUBTEST("Create a copy of the Datum.");
            const Datum cObj = obj.clone(alloc);

            SUBTEST("Verify that both objects have the same value.");
            ASSERT(cObj == obj);

            SUBTEST("Destroy both the objects and verify "
                    "that there are no memory leaks.");
            Datum::destroy(obj, alloc);
            Datum::destroy(cObj, alloc);
            ASSERT(0 == defaultAlloc.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        TEST("Copying Datum with different Decimal64 values.");

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        for (int i = 0; i < decimal64Values; ++i) {
            SUBTEST("Create Datum with a ") << decimal64Value[i].label
                                            << " Decimal64 value.";

            const Datum obj = Datum::createDecimal64(decimal64Value[i].value,
                                                     alloc);

            const Datum cObj = obj.clone(alloc);

            if (i < comparableDecimal64Values) {
                LOOP_ASSERT(i, cObj == obj);
            }

            Datum::destroy(obj, alloc);
            Datum::destroy(cObj, alloc);
            LOOP_ASSERT(i, 0 == defaultAlloc.status());
        }
    } break;

    case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS:
        //
        // Concerns:
        //    Any variation in value or type of 'Datum' object must be
        //    detected by the equality operators.
        //
        // Plan:
        //    Create 'Datum' objects of different types. Also create
        //    'Datum' objects of the same type but with subtle
        //    variations in values. Verify the correctness of 'operator=='
        //    and 'operator!='.
        //
        // Testing:
        //   bool operator==(const Datum&, const Datum&);
        //   bool operator!=(const Datum&, const Datum&);
        // --------------------------------------------------------------------

        TESTCASE("Testing Equality Operators");

        TEST("Create some Datum objects of different types.");

        bsl::vector<Datum> elements;
        createArrayOfScalarTypes(&elements, alloc);
        DatumArrayRef array;
        Datum::createUninitializedArray(&array, elements.size(), alloc);
        for (size_type i = 0; i < elements.size(); ++i) {
            array.data()[i] = elements[i];
        }
        *(array.length()) = elements.size();

        const size_type mapSize = 6;
        DatumMapRef mapping;
        Datum::createUninitializedMap(&mapping, mapSize, alloc);
        mapping.data()[0] =
            DatumMapEntry(StringRef("first", strlen("first")),
                          Datum::createInteger(0));
        mapping.data()[1] =
            DatumMapEntry(StringRef("second", strlen("second")),
                          Datum::createDouble(-3.1416));
        mapping.data()[2] =
            DatumMapEntry(StringRef("third", strlen("third")),
                          Datum::copyString("A long string", alloc));
        mapping.data()[3] =
            DatumMapEntry(StringRef("fourth", strlen("fourth")),
                          Datum::copyString("Abc", alloc));
        mapping.data()[4] =
            DatumMapEntry(StringRef("fifth", strlen("fifth")),
                          Datum::createDate(bdlt::Date(2010,1,5)));
        mapping.data()[5] =
            DatumMapEntry(StringRef("sixth", strlen("sixth")),
                          Datum::adoptArray(array));
       *(mapping.size()) = mapSize;

       // Create an array out of all the previously created map.
       Datum::createUninitializedArray(&array, 1, alloc);
       array.data()[0] = Datum::adoptMap(mapping);
       *(array.length()) = 1;
       elements.push_back(Datum::adoptArray(array));


       // Treat NaNs specially as value == value is false for NaN.
       elements.push_back(Datum::createDouble(DBL_QNAN2));
       elements.push_back(Datum::createDouble(
                                     numeric_limits<double>::signaling_NaN()));

       // Repeat the above sequence but with a map owning keys instead of a map
       // of 'Datum' objects. Both should compare equal.
       bsl::vector<Datum> elementsCopy;
       createArrayOfScalarTypes(&elementsCopy, alloc);
       Datum::createUninitializedArray(&array, elementsCopy.size(), alloc);
       for (size_type i = 0; i < elementsCopy.size(); ++i) {
           array.data()[i] = elementsCopy[i];
       }
       *(array.length()) = elementsCopy.size();

       const char *kys[mapSize] = {"first", "second", "third",
                                   "fourth", "fifth", "sixth" };
       size_type keysSize = 0;
       for (size_type i = 0; i < mapSize; ++i) {
           keysSize += strlen(kys[i]);
       }
       DatumMapOwningKeysRef ownedMapping;
       Datum::createUninitializedMapOwningKeys(&ownedMapping,
                                               mapSize,
                                               keysSize,
                                               alloc);
       char *keys = ownedMapping.keys();
       strncpy(keys, kys[0], strlen(kys[0]));
       ownedMapping.data()[0] =
           DatumMapEntry(StringRef(keys, strlen(kys[0])),
                         Datum::createInteger(0));
       keys += strlen(kys[0]);
       strncpy(keys, kys[1], strlen(kys[1]));
       ownedMapping.data()[1] =
           DatumMapEntry(StringRef(keys, strlen(kys[1])),
                         Datum::createDouble(-3.1416));
       keys += strlen(kys[1]);
       strncpy(keys, kys[2], strlen(kys[2]));
       ownedMapping.data()[2] =
           DatumMapEntry(StringRef(keys, strlen(kys[2])),
                         Datum::copyString("A long string", alloc));
       keys += strlen(kys[2]);
       strncpy(keys, kys[3], strlen(kys[3]));
       ownedMapping.data()[3] =
           DatumMapEntry(StringRef(keys, strlen(kys[3])),
                         Datum::copyString("Abc", alloc));
       keys += strlen(kys[3]);
       strncpy(keys, kys[4], strlen(kys[4]));
       ownedMapping.data()[4] =
           DatumMapEntry(StringRef(keys, strlen(kys[4])),
                         Datum::createDate(bdlt::Date(2010,1,5)));
       keys += strlen(kys[4]);
       strncpy(keys, kys[5], strlen(kys[5]));
       ownedMapping.data()[5] =
           DatumMapEntry(StringRef(keys, strlen(kys[5])),
                         Datum::adoptArray(array));
       *(ownedMapping.size()) = mapSize;

       // Create an array out of all the previously created map.
       Datum::createUninitializedArray(&array, 1, alloc);
       array.data()[0] = Datum::adoptMapOwningKeys(ownedMapping);
       *(array.length()) = 1;
       elementsCopy.push_back(Datum::adoptArray(array));

       // Treat NaNs specially as value == value is false for NaN.
       elementsCopy.push_back(Datum::createDouble(DBL_QNAN2));
       elementsCopy.push_back(Datum::createDouble(
                                     numeric_limits<double>::signaling_NaN()));

       TEST("Compare Datum objects.");
        for (size_type i = 0; i < elements.size(); ++i) {
            const Datum &elemI = elements[i];
            const Datum &copyI = elementsCopy[i];
            for (size_type j = 0; j < elements.size(); ++j) {
                const Datum &elemJ = elements[j];
                const Datum &copyJ = elementsCopy[j];
                if (i == j) {
                    if (elemI.isDouble() &&
                        elemI.theDouble() != elemI.theDouble()) {
                            LOOP2_ASSERT(i, j, elemI != elemJ);
                    }
                    else {
                        LOOP2_ASSERT(i, j, elemI == elemJ);
                    }
                    if (copyI.isDouble() &&
                        copyI.theDouble() != copyI.theDouble()) {
                            LOOP2_ASSERT(i, j, copyI != copyJ);
                    }
                    else {
                        LOOP2_ASSERT(i, j, copyI == copyJ);
                    }
                }
                else {
                    if (elemI.isArray() && elemJ.isArray() &&
                        elemI.theArray().length() == 0 &&
                        elemJ.theArray().length() == 0) {
                            LOOP2_ASSERT(i, j, elemI == elemJ);
                    }
                    else if (elemI.isMap() && elemJ.isMap() &&
                             elemI.theMap().size() == 0 &&
                             elemJ.theMap().size() == 0) {
                                 LOOP2_ASSERT(i, j, elemI == elemJ);
                    }
                    else {
                        LOOP2_ASSERT(i, j, elemI != elemJ);
                    }
                    if (copyI.isArray() && copyJ.isArray() &&
                        copyI.theArray().length() == 0 &&
                        copyJ.theArray().length() == 0) {
                            LOOP2_ASSERT(i, j, copyI == copyJ);
                    }
                    else if (copyI.isMap() && copyJ.isMap() &&
                             copyI.theMap().size() == 0 &&
                             copyJ.theMap().size() == 0) {
                              LOOP2_ASSERT(i, j, copyI == copyJ);
                    }
                    else {
                        LOOP2_ASSERT(i, j, copyI != copyJ);
                    }
                }
            }

            if (elemI.isDouble() && copyI.isDouble() &&
                elemI.theDouble() != elemI.theDouble() &&
                copyI.theDouble() != copyI.theDouble()) {
                    LOOP_ASSERT(i, elemI != copyI);
            }
            else {
                LOOP_ASSERT(i, elemI == copyI);
            }
        }

        TEST("Destroy all the Datum objects.");
        // Only destroy the last array instead of all the individual
        // elements, as the array was initialized out of those elements.
        Datum::destroy(elements    [elements.size()     - 3], alloc);
        Datum::destroy(elementsCopy[elementsCopy.size() - 3], alloc);
    } break;

    case 5: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS:
        //
        // Concerns:
        //    Confirm that isXXX functions (where XXX is a particular type)
        //    return true for the correct type of 'Datum' object and
        //    false otherwise.
        //
        // Plan:
        //    Create 'Datum' objects with each of the different
        //    creators. Verify that the created 'Datum' objects are of
        //    the correct type and are not of any other type also.
        //
        // Testing:
        //   bool isArray() const;
        //   bool isBinary() const;
        //   bool isBoolean() const;
        //   bool isDate() const;
        //   bool isDatetime() const;
        //   bool isDatetimeInterval() const;
        //   bool isDecimal64() const;
        //   bool isDouble() const;
        //   bool isError() const;
        //   bool isInteger() const;
        //   bool isInteger64() const;
        //   bool isMap() const;
        //   bool isNull() const;
        //   bool isString() const;
        //   bool isTime() const;
        //   bool isUdt() const;
        //   DataType type() const;
        // --------------------------------------------------------------------

        TESTCASE("TESTING BASIC ACCESSORS");

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Testing isNull()");

            SUBTEST("Create Datum with a nil value.");
            const Datum obj = Datum::createNull();

            SUBTEST("Check if object has only a nil value.");
            ASSERT(obj.isNull());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isError());
            ASSERT(!obj.isDecimal64());
            ASSERT(!obj.isDouble());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isString());
            ASSERT(!obj.isArray());
            ASSERT(!obj.isMap());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_NIL == obj.type());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Testing isInteger()");

            SUBTEST("Create Datum with an integer value.");
            const Datum obj = Datum::createInteger(1);

            SUBTEST("Check if object has only an integer value.");
            ASSERT(obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isError());
            ASSERT(!obj.isDecimal64());
            ASSERT(!obj.isDouble());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isString());
            ASSERT(!obj.isArray());
            ASSERT(!obj.isMap());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_INTEGER == obj.type());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Testing isBoolean()");

            SUBTEST("Create Datum with a boolean value.");
            const Datum obj = Datum::createBoolean(true);

            SUBTEST("Check if object has only a boolean value.");
            ASSERT(obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(!obj.isError());
            ASSERT(!obj.isDecimal64());
            ASSERT(!obj.isDouble());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isString());
            ASSERT(!obj.isArray());
            ASSERT(!obj.isMap());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_BOOLEAN == obj.type());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        TEST("Testing isError()");

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with an error code value.");
            const Datum obj = Datum::createError(5);

            SUBTEST("Check if object has only an Error value.");
            ASSERT(obj.isError());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(!obj.isDecimal64());
            ASSERT(!obj.isDouble());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isString());
            ASSERT(!obj.isArray());
            ASSERT(!obj.isMap());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_ERROR_VALUE == obj.type());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with an error code value "
                    "and an error message value."
                             );
            const Datum obj = Datum::createError(5, "some error", alloc);

            SUBTEST("Check if object has only an Error value.");
            ASSERT(obj.isError());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(!obj.isDecimal64());
            ASSERT(!obj.isDouble());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isString());
            ASSERT(!obj.isArray());
            ASSERT(!obj.isMap());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_ERROR_VALUE == obj.type());

            SUBTEST("Destroy the Datum object.");
            Datum::destroy(obj, alloc);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Testing isDate()");

            SUBTEST("Create Datum with a Date value.");
            const Datum obj = Datum::createDate(bdlt::Date(2010,1,5));

            SUBTEST("Check if object has only a date value.");
            ASSERT(obj.isDate());
            ASSERT(!obj.isError());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(!obj.isDecimal64());
            ASSERT(!obj.isDouble());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isString());
            ASSERT(!obj.isArray());
            ASSERT(!obj.isMap());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_DATE == obj.type());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Testing isTime()");

            SUBTEST("Create Datum with a Time value.");
            const Datum obj = Datum::createTime(bdlt::Time(16,45,32,12));

            SUBTEST("Check if object has only a time value.");
            ASSERT(obj.isTime());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isError());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(!obj.isDecimal64());
            ASSERT(!obj.isDouble());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isString());
            ASSERT(!obj.isArray());
            ASSERT(!obj.isMap());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_TIME == obj.type());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Testing isDatetime()");

            SUBTEST("Create Datum with a date+time value.");
            const Datum obj = Datum::createDatetime(
                                          bdlt::Datetime(2010,1,5, 16,45,32,12),
                                          alloc);

            SUBTEST("Check if object has only a date+time value.");
            ASSERT(obj.isDatetime());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isError());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(!obj.isDecimal64());
            ASSERT(!obj.isDouble());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isString());
            ASSERT(!obj.isArray());
            ASSERT(!obj.isMap());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_DATETIME == obj.type());

            SUBTEST("Destroy the Datum object.");
            Datum::destroy(obj, alloc);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Testing isDatetimeInterval()");

            SUBTEST("Create Datum with a date+time interval value.");
            const Datum obj = Datum::createDatetimeInterval(
                                        bdlt::DatetimeInterval(34, 16,45,32,12),
                                        alloc);

            SUBTEST("Check if object has only a date+time interval value.");
            ASSERT(obj.isDatetimeInterval());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isError());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(!obj.isDecimal64());
            ASSERT(!obj.isDouble());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isString());
            ASSERT(!obj.isArray());
            ASSERT(!obj.isMap());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_DATETIME_INTERVAL == obj.type());

            SUBTEST("Destroy the Datum object.");
            Datum::destroy(obj, alloc);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Testing isInteger64()");

            SUBTEST("Create Datum with an Int64 value.");
            const Datum obj =
                   Datum::createInteger64(Int64(9223372036854775807LL), alloc);

            SUBTEST("Check if object has only an Int64 value.");
            ASSERT(obj.isInteger64());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isError());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(!obj.isDecimal64());
            ASSERT(!obj.isDouble());
            ASSERT(!obj.isString());
            ASSERT(!obj.isArray());
            ASSERT(!obj.isMap());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_INTEGER64 == obj.type());

            SUBTEST("Destroy the Datum object.");
            Datum::destroy(obj, alloc);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        TEST("Testing isString()");

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with a long string value.");
            const Datum obj = Datum::copyString("A long string", alloc);

            SUBTEST("Check if object has only a string value.");
            ASSERT(obj.isString());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isError());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(!obj.isDecimal64());
            ASSERT(!obj.isDouble());
            ASSERT(!obj.isArray());
            ASSERT(!obj.isMap());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_STRING == obj.type());

            SUBTEST("Destroy the Datum object.");
            Datum::destroy(obj, alloc);
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with a short string value.");
            const Datum obj = Datum::copyString("abc", alloc);

            SUBTEST("Check if object has only a string value.");
            ASSERT(obj.isString());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isError());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(!obj.isDecimal64());
            ASSERT(!obj.isDouble());
            ASSERT(!obj.isArray());
            ASSERT(!obj.isMap());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_STRING == obj.type());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        TEST("Testing isDecimal64()");

        for (int i = 0; i < decimal64Values; ++i) {
            SUBTEST("Create a ") << decimal64Value[i].label
                                 << " Decimal64 value.";
            Datum obj = Datum::createDecimal64(decimal64Value[i].value,
                                               alloc);

            SUBTEST("Check if object has only a Decimal64 value.");
            ASSERT(obj.isDecimal64());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isError());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(!obj.isDouble());
            ASSERT(!obj.isString());
            ASSERT(!obj.isArray());
            ASSERT(!obj.isMap());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_DECIMAL64 == obj.type());

            SUBTEST("Destroy the Datum object.");
            Datum::destroy(obj, alloc);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        TEST("Testing isDouble()");

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the minimum double value.");
            const Datum obj = Datum::createDouble(DBL_MIN2);

            SUBTEST("Check if object has only a double value.");
            ASSERT(obj.isDouble());
            ASSERT(!obj.isString());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isError());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(!obj.isArray());
            ASSERT(!obj.isMap());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_REAL == obj.type());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the maximum double value.");
            const Datum obj = Datum::createDouble(DBL_MAX2);

            SUBTEST("Check if object has only a double value.");
            ASSERT(obj.isDouble());
            ASSERT(!obj.isString());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isError());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(!obj.isArray());
            ASSERT(!obj.isMap());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_REAL == obj.type());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the double value of infinity.");
            const Datum obj = Datum::createDouble(DBL_INF);

            SUBTEST("Check if object has only a double value.");
            ASSERT(obj.isDouble());
            ASSERT(!obj.isString());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isError());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(!obj.isArray());
            ASSERT(!obj.isMap());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_REAL == obj.type());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the double value"
                    " of negative infinity.");
            const Datum obj = Datum::createDouble(DBL_NEGINF);

            SUBTEST("Check if object has only a double value.");
            ASSERT(obj.isDouble());
            ASSERT(!obj.isString());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isError());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(!obj.isArray());
            ASSERT(!obj.isMap());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_REAL == obj.type());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the double value of negative zero.");
            const Datum obj = Datum::createDouble(DBL_NEGZERO);

            SUBTEST("Check if object has only a double value.");
            ASSERT(obj.isDouble());
            ASSERT(!obj.isString());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isError());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(!obj.isArray());
            ASSERT(!obj.isMap());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_REAL == obj.type());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the double value of quiet NaN.");
            const Datum obj = Datum::createDouble(DBL_QNAN2);

            SUBTEST("Check if object has a double value only.");
            ASSERT(obj.isDouble());
            ASSERT(!obj.isString());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isError());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(!obj.isArray());
            ASSERT(!obj.isMap());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_REAL == obj.type());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the double value of signaling NaN.");
            const Datum obj = Datum::createDouble(
                                      numeric_limits<double>::signaling_NaN());

            SUBTEST("Check if object has only a double value.");
            ASSERT(obj.isDouble());
            ASSERT(!obj.isString());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isError());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(!obj.isArray());
            ASSERT(!obj.isMap());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_REAL == obj.type());
        }

        // Windows test only.
    #ifdef WIN32
        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the indeterminate double value.");
            const Datum obj = Datum::createDouble(DBL_IND);

            SUBTEST("Check if object has only a double value.");
            ASSERT(obj.isDouble());
            ASSERT(!obj.isString());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isError());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(!obj.isArray());
            ASSERT(!obj.isMap());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_REAL == obj.type());
        }
    #endif

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        TEST("Testing isArray()");

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with an array value.");
            const size_type arraySize  = 6;
            DatumArrayRef array;
            Datum::createUninitializedArray(&array, arraySize, alloc);
            array.data()[0] = Datum::createInteger(0);
            array.data()[1] = Datum::createDouble(-3.1416);
            array.data()[2] = Datum::copyString("A long string", alloc);
            array.data()[3] = Datum::copyString("Abc", alloc);
            array.data()[4] = Datum::createDate(bdlt::Date(2010,1,5));
            array.data()[5] = Datum::createDatetime(
                                          bdlt::Datetime(2010,1,5, 16,45,32,12),
                                          alloc);
            *(array.length()) = arraySize;
            Datum obj = Datum::adoptArray(array);

            SUBTEST("Check if object has only an array value.");
            ASSERT(obj.isArray());
            ASSERT(!obj.isMap());
            ASSERT(!obj.isDecimal64());
            ASSERT(!obj.isDouble());
            ASSERT(!obj.isString());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isError());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_ARRAY == obj.type());

            SUBTEST("Destroy the Datum object.");
            Datum::destroy(obj, alloc);
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with an empty array value.");
            DatumArrayRef array;
            Datum obj = Datum::adoptArray(array);

            SUBTEST("Check if object has only an array value.");
            ASSERT(obj.isArray());
            ASSERT(!obj.isMap());
            ASSERT(!obj.isDecimal64());
            ASSERT(!obj.isDouble());
            ASSERT(!obj.isString());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isError());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_ARRAY == obj.type());

            SUBTEST("Destroy the Datum object.");
            Datum::destroy(obj, alloc);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        TEST("Testing isMap()");

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with a map value.");
            const size_type mapSize = 6;
            DatumMapRef mapping;
            Datum::createUninitializedMap(&mapping, mapSize, alloc);
            mapping.data()[0] =
                DatumMapEntry(StringRef("first", strlen("first")),
                              Datum::createInteger(0));
            mapping.data()[1] =
                DatumMapEntry(StringRef("second", strlen("second")),
                              Datum::createDouble(-3.1416));
            mapping.data()[2] =
                DatumMapEntry(StringRef("third", strlen("third")),
                              Datum::copyString("A long string", alloc));
            mapping.data()[3] =
                DatumMapEntry(StringRef("fourth", strlen("fourth")),
                              Datum::copyString("Abc", alloc));
            mapping.data()[4] =
                DatumMapEntry(StringRef("fifth", strlen("fifth")),
                              Datum::createDate(bdlt::Date(2010,1,5)));
            mapping.data()[5] =
                DatumMapEntry(StringRef("sixth", strlen("sixth")),
                              Datum::createDatetime(
                                          bdlt::Datetime(2010,1,5, 16,45,32,12),
                                          alloc));
            *(mapping.size()) = 6;
            Datum obj = Datum::adoptMap(mapping);

            SUBTEST("Check if object has only a map value.");
            ASSERT(obj.isMap());
            ASSERT(!obj.isArray());
            ASSERT(!obj.isDecimal64());
            ASSERT(!obj.isDouble());
            ASSERT(!obj.isString());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isError());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_MAP == obj.type());

            SUBTEST("Destroy the Datum object.");
            Datum::destroy(obj, alloc);
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with an empty map value.");
            DatumMapRef mapping;
            Datum obj = Datum::adoptMap(mapping);

            SUBTEST("Check if object has only a map value.");
            ASSERT(obj.isMap());
            ASSERT(!obj.isArray());
            ASSERT(!obj.isDecimal64());
            ASSERT(!obj.isDouble());
            ASSERT(!obj.isString());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isError());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_MAP == obj.type());

            SUBTEST("Destroy the Datum object.");
            Datum::destroy(obj, alloc);
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with a map (owning keys) value.");
            const size_type mapSize = 6;
            const char *kys[6] = {"fifth", "first", "fourth",
                                  "second", "sixth", "third" };
            size_type keysCapacity = 0;
            for (size_type i = 0; i < 6; ++i) {
                keysCapacity += strlen(kys[i]);
            }
            DatumMapOwningKeysRef mapping;
            Datum::createUninitializedMapOwningKeys(&mapping,
                                                    mapSize,
                                                    keysCapacity,
                                                    alloc);
            char *keys = mapping.keys();
            bsl::memcpy(keys, kys[0], strlen(kys[0]));
            mapping.data()[0] =
                DatumMapEntry(StringRef(keys, strlen(kys[0])),
                              Datum::createInteger(0));
            keys += strlen(kys[0]);
            bsl::memcpy(keys, kys[1], strlen(kys[1]));
            mapping.data()[1] =
                DatumMapEntry(StringRef(keys, strlen(kys[1])),
                              Datum::createDouble(-3.1416));
            keys += strlen(kys[1]);
            bsl::memcpy(keys, kys[2], strlen(kys[2]));
            mapping.data()[2] =
                DatumMapEntry(StringRef(keys, strlen(kys[2])),
                              Datum::copyString("A long string", alloc));
            keys += strlen(kys[2]);
            bsl::memcpy(keys, kys[3], strlen(kys[3]));
            mapping.data()[3] =
                DatumMapEntry(StringRef(keys, strlen(kys[3])),
                              Datum::copyString("Abc", alloc));
            keys += strlen(kys[3]);
            bsl::memcpy(keys, kys[4], strlen(kys[4]));
            mapping.data()[4] =
                DatumMapEntry(StringRef(keys, strlen(kys[4])),
                              Datum::createDate(bdlt::Date(2010,1,5)));
            keys += strlen(kys[4]);
            bsl::memcpy(keys, kys[5], strlen(kys[5]));
            mapping.data()[5] =
                DatumMapEntry(StringRef(keys, strlen(kys[5])),
                              Datum::createDatetime(
                                          bdlt::Datetime(2010,1,5, 16,45,32,12),
                                          alloc));
            *(mapping.size()) = 6;
            *(mapping.sorted()) = true;
            Datum obj = Datum::adoptMapOwningKeys(mapping);

            SUBTEST("Check if object has only a map value.");
            ASSERT(obj.isMap());
            ASSERT(!obj.isArray());
            ASSERT(!obj.isDecimal64());
            ASSERT(!obj.isDouble());
            ASSERT(!obj.isString());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isError());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_MAP == obj.type());

            SUBTEST("Destroy the Datum object.");
            Datum::destroy(obj, alloc);
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with an empty map (owning keys) value.");
            DatumMapOwningKeysRef mapping;
            const Datum obj = Datum::adoptMapOwningKeys(mapping);

            SUBTEST("Check if object has only a map value.");
            ASSERT(obj.isMap());
            ASSERT(!obj.isArray());
            ASSERT(!obj.isDecimal64());
            ASSERT(!obj.isDouble());
            ASSERT(!obj.isString());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isError());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isBinary());
            ASSERT(Datum::e_MAP == obj.type());

            SUBTEST("Destroy the Datum object.");
            Datum::destroy(obj, alloc);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Testing isUdt()");

            SUBTEST("Create Datum with a user defined object.");
            const Datum obj = Datum::createUdt(&udtValue, UDT_TYPE);

            SUBTEST("Check if object has only a user defined object.");
            ASSERT(obj.isUdt());;
            ASSERT(!obj.isArray());
            ASSERT(!obj.isMap());
            ASSERT(!obj.isDecimal64());
            ASSERT(!obj.isDouble());
            ASSERT(!obj.isString());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isError());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(Datum::e_USERDEFINED == obj.type());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Testing isBinary()");

            SUBTEST("Create Datum with a binary object.");

            const Datum obj = Datum::copyBinary("x", 1, &ta);

            SUBTEST("Check if object is only a binary object.");
            ASSERT(obj.isBinary());
            ASSERT(!obj.isUdt());
            ASSERT(!obj.isArray());
            ASSERT(!obj.isMap());
            ASSERT(!obj.isDecimal64());
            ASSERT(!obj.isDouble());
            ASSERT(!obj.isString());
            ASSERT(!obj.isInteger64());
            ASSERT(!obj.isDatetimeInterval());
            ASSERT(!obj.isDatetime());
            ASSERT(!obj.isTime());
            ASSERT(!obj.isDate());
            ASSERT(!obj.isError());
            ASSERT(!obj.isBoolean());
            ASSERT(!obj.isInteger());
            ASSERT(!obj.isNull());
            ASSERT(Datum::e_BINARY == obj.type());

            Datum::destroy(obj, alloc);
        }

         ASSERT(0 == defaultAlloc.status());

    } break;
    case 4: {
        // --------------------------------------------------------------------
        // TESTING TEST APPARATUS:
        //    Check that 'Decimal64' fit in 6 bytes or not (as expected).
        // --------------------------------------------------------------------

        TESTCASE("TESTING TEST APPARATUS");

        {
            TEST("Testing compressibility of Decimal64 values.");
            unsigned char buffer[9];

            SUBTEST("Check that decimal64Value[0] fits in 6 bytes.");
            ASSERT(bdldfp::DecimalConvertUtil::
                   decimal64ToVariableWidthEncoding(buffer,
                                                    decimal64Value[0].value)
                   < buffer + 6);

            SUBTEST("Check that decimal64Value[1] does not fit in 6 bytes.");
            ASSERT(bdldfp::DecimalConvertUtil::
                   decimal64ToVariableWidthEncoding(buffer,
                                                    decimal64Value[1].value)
                   > buffer + 6);
        }

    } break;
    case 3: {
        // --------------------------------------------------------------------
        // TESTING CREATORS AND PRIMARY MANIPULATOR:
        //
        // Concerns:
        //    The creators should create 'Datum' objects of the correct type
        //    and value.  No memory should be allocated when 'Datum' with a
        //    short length string (less than 6 characters in length) is
        //    created.  A 'Datum' object should take the value of the object
        //    from which it is copy-assigned and should lose its old value.  If
        //    the source object has any dynamically allocated memory, then it
        //    should be now shared between the two objects and no new
        //    allocations should happen.  'destroy' should deallocate any
        //    previously allocated memory.
        //
        // Plan:
        //    Create 'Datum' objects with each of the different creators using
        //    different values of the same type.  Use the basic accessors to
        //    verify that the type and value inside the 'Datum' objects is as
        //    expected.  Verify that no memory was allocated when object stores
        //    a short string.  Copy-assign a 'Datum' object storing a different
        //    type of value and verify that the 'Datum' now holds the new type
        //    of value.  Also verify that copy-assigning a 'Datum' object
        //    holding dynamically allocated memory, should not cause any new
        //    memory allocations.  Finally destroy these objects and verify
        //    that no memory was leaked.  Display steps of operation frequently
        //    in verbose mode.
        //
        // Testing:
        //   Datum adoptArray(const DatumArrayRef&);
        //   Datum adoptMap(const DatumMapRef&);
        //   Datum adoptMapOwningKeys(const DatumMapOwningKeysRef&);
        //   Datum clone(const Datum&, bslma::Allocator *);
        //   Datum copyBinary(const void*, SizeType, bslma::Allocator *);
        //   Datum copyString(const bslstl::StringRef&, bslma::Allocator *);
        //   Datum copyString(const char *, SizeType, bslma::Allocator *);
        //   Datum createBoolean(bool);
        //   Datum createDate(const bdlt::Date&);
        //   Datum createDatetime(const bdlt::Datetime&, bslma::Allocator *);
        //   Datum createDatetimeInterval(const bdlt::DatetimeInterval&,
        //                                bslma::Allocator *);
        //   Datum createDecimal64(Decimal64);
        //   Datum createDouble(double);
        //   Datum createError(int);
        //   Datum createError(int,
        //                     const bslstl::StringRef&,
        //                     bslma::Allocator *);
        //   Datum createInteger(int);
        //   Datum createInteger64(Int64, bslma::Allocator *);
        //   Datum createNull();
        //   Datum createTime(const bdlt::Time&);
        //   Datum createUdt(void *, int);
        //   void createUninitializedArray(DatumArrayRef *,
        //                                 SizeType,
        //                                 bslma::Allocator *);
        //   void createUninitializedMap(DatumMapRef *,
        //                               SizeType,
        //                               bslma::Allocator *);
        //   createUninitializedMapOwningKeys(DatumMapOwningKeysRef *,
        //                                    SizeType,
        //                                    SizeType,
        //                                    bslma::Allocator *);
        //   Datum& operator=(const Datum&);
        //   void destroy(const Datum&, bslma::Allocator *);
        // --------------------------------------------------------------------

        TESTCASE("TESTING CREATORS AND PRIMARY MANIPULATORS");

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Testing createNull().");

            SUBTEST("Create Datum with a nil value.");
            const Datum obj = Datum::createNull();

            SUBTEST("Check if object has a nil value.");
            ASSERT(obj.isNull());

            SUBTEST("Copy-assign this object and verify that"
                    " both objects have the same type and value.");
            Datum cObj = Datum::createInteger(2);
            cObj = obj;
            ASSERT(cObj.isNull());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        TEST("Testing Datum::createInteger().");

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the integer value of '1'.");
            const Datum obj = Datum::createInteger(1);

            SUBTEST("Check if object has the same integer value.");
            ASSERT(obj.isInteger());
            ASSERT(1 == obj.theInteger());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the integer value of '-1'.");
            const Datum obj = Datum::createInteger(-1);

            SUBTEST("Check if object has the same integer value.");
            ASSERT(obj.isInteger());
            ASSERT(-1 == obj.theInteger());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the integer value of '0'.");
            const Datum obj = Datum::createInteger(0);

            SUBTEST("Check if object has the same integer value.");
            ASSERT(obj.isInteger());
            ASSERT(0 == obj.theInteger());

            SUBTEST("Copy-assign this object and verify that"
                    " both objects have the same type and value.");
            Datum cObj = Datum::createNull();
            cObj = obj;
            ASSERT(cObj.isInteger());
            ASSERT(0 == cObj.theInteger());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        TEST("Testing Datum::createBoolean().");

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the boolean value of 'true'.");
            const Datum obj = Datum::createBoolean(true);

            SUBTEST("Check if object has the same boolean value.");
            ASSERT(obj.isBoolean());
            ASSERT(true == obj.theBoolean());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the boolean value of false'.");
            const Datum obj = Datum::createBoolean(false);

            SUBTEST("Check if object has the same boolean value.");
            ASSERT(obj.isBoolean());
            ASSERT(false == obj.theBoolean());

            SUBTEST("Copy-assign this object and verify "
                    "that both objects have the same type and value.");
            Datum cObj = Datum::createNull();
            cObj = obj;
            ASSERT(cObj.isBoolean());
            ASSERT(false == cObj.theBoolean());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        TEST("Testing Datum::createError().");

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with an error code value.");
            const Datum obj = Datum::createError(5);

            SUBTEST("Check if object has the same Error value.");
            ASSERT(obj.isError());
            ASSERT(Error(5) == obj.theError());

            SUBTEST("Copy-assign this object and verify "
                    "that both objects have the same type and value.");
            Datum cObj = Datum::createNull();
            cObj = obj;
            ASSERT(cObj.isError());
            ASSERT(Error(5) == cObj.theError());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with an error code and an "
                    "error message value.");
            const Datum obj = Datum::createError(5, "some error", alloc);

            SUBTEST("Check if object has the same Error value.");
            ASSERT(obj.isError());
            ASSERT(Error(5, "some error") == obj.theError());

            SUBTEST("Copy-assign this object and verify "
                    "that both objects have the same type and value.");
            Datum cObj = Datum::createNull();
            cObj = obj;
            ASSERT(cObj.isError());
            ASSERT(Error(5, "some error") == cObj.theError());

            SUBTEST("Destroy the object and verify there "
                    "are no memory leaks.");
            Datum::destroy(obj, &ta);
            ASSERT(0 == ta.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Testing Datum::createDate().");

            SUBTEST("Create Datum with a Date value.");
            const Datum obj = Datum::createDate(bdlt::Date(2010,1,5));

            SUBTEST("Check if object has the same date value.");
            ASSERT(obj.isDate());
            ASSERT(obj.theDate() == bdlt::Date(2010,1,5));

            SUBTEST("Copy-assign this object and verify "
                    "that both objects have the same type and value.");
            Datum cObj = Datum::createNull();
            cObj = obj;
            ASSERT(cObj.isDate());
            ASSERT(cObj.theDate() == bdlt::Date(2010,1,5));
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Testing Datum::createTime().");

            SUBTEST("Create Datum with a Time value.");
            const Datum obj = Datum::createTime(bdlt::Time(16,45,32,12));

            SUBTEST("Check if object has the same time value.");
            ASSERT(obj.isTime());
            ASSERT(bdlt::Time(16,45,32,12) == obj.theTime());

            SUBTEST("Copy-assign this object and verify "
                    "that both objects have the same type and value.");
            Datum cObj = Datum::createNull();
            cObj = obj;
            ASSERT(cObj.isTime());
            ASSERT(bdlt::Time(16,45,32,12) == cObj.theTime());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        TEST("Testing Datum::copyString().");

        bslma::TestAllocator tmpa("temp", veryVeryVeryVerbose);

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datums by copying a long string value.");
            const char *str = "A long string constant";
            const Datum obj1 = Datum::copyString(str, &ta);
            const Datum obj2 = Datum::copyString(str, strlen(str), &tmpa);

            SUBTEST("Check if objects have the same string value.");
            ASSERT(obj1.isString());
            ASSERT(obj2.isString());
            ASSERT(obj1.theString() == bslstl::StringRef(str, strlen(str)));
            ASSERT(obj2.theString() == bslstl::StringRef(str, strlen(str)));

            SUBTEST("Verify that the string was copied.");
            ASSERT(0 != ta.status());
            ASSERT(0 != tmpa.status());

            SUBTEST("Destroy the objects and verify there "
                    "are no memory leaks.");
            Datum::destroy(obj1, &ta);
            Datum::destroy(obj2, &tmpa);
            ASSERT(0 == ta.status());
            ASSERT(0 == tmpa.status());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datums by copying a zero-length string value.");
            const char *str = "";
            const Datum obj1 = Datum::copyString(str, &ta);
            const Datum obj2 = Datum::copyString(str, strlen(str), &tmpa);

            SUBTEST("Check if objects have the same string value.");
            ASSERT(obj1.isString());
            ASSERT(obj2.isString());
            ASSERT(obj1.theString() == bslstl::StringRef(str, strlen(str)));
            ASSERT(obj2.theString() == bslstl::StringRef(str, strlen(str)));

            SUBTEST("Verify that no memory was allocated during creation.");
            ASSERT(0 == ta.status());
            ASSERT(0 == tmpa.status());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datums by copying a string value of length 1.");
            const char *str = "1";
            const Datum obj1 = Datum::copyString(str, &ta);
            const Datum obj2 = Datum::copyString(str, strlen(str), &tmpa);

            SUBTEST("Check if objects have the same string value.");
            ASSERT(obj1.isString());
            ASSERT(obj2.isString());
            ASSERT(obj1.theString() == bslstl::StringRef(str, strlen(str)));
            ASSERT(obj2.theString() == bslstl::StringRef(str, strlen(str)));

            SUBTEST("Verify that no memory was allocated during creation.");
            ASSERT(0 == ta.status());
            ASSERT(0 == tmpa.status());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datums by copying a string value of length 2.");
            const char *str = "12";
            const Datum obj1 = Datum::copyString(str, &ta);
            const Datum obj2 = Datum::copyString(str, strlen(str), &tmpa);

            SUBTEST("Check if objects have the same string value.");
            ASSERT(obj1.isString());
            ASSERT(obj2.isString());
            ASSERT(obj1.theString() == bslstl::StringRef(str, strlen(str)));
            ASSERT(obj2.theString() == bslstl::StringRef(str, strlen(str)));

            SUBTEST("Verify that no memory was allocated during creation.");
            ASSERT(0 == ta.status());
            ASSERT(0 == tmpa.status());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datums by copying a string value of length 3.");
            const char *str = "123";
            const Datum obj1 = Datum::copyString(str, &ta);
            const Datum obj2 = Datum::copyString(str, strlen(str), &tmpa);

            SUBTEST("Check if objects have the same string value.");
            ASSERT(obj1.isString());
            ASSERT(obj2.isString());
            ASSERT(obj1.theString() == bslstl::StringRef(str, strlen(str)));
            ASSERT(obj2.theString() == bslstl::StringRef(str, strlen(str)));

            SUBTEST("Verify that no memory was allocated during creation.");
            ASSERT(0 == ta.status());
            ASSERT(0 == tmpa.status());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datums by copying a string value of length 4.");
            const char *str = "1234";
            const Datum obj1 = Datum::copyString(str, &ta);
            const Datum obj2 = Datum::copyString(str, strlen(str), &tmpa);

            SUBTEST("Check if objects have the same string value.");
            ASSERT(obj1.isString());
            ASSERT(obj2.isString());
            ASSERT(obj1.theString() == bslstl::StringRef(str, strlen(str)));
            ASSERT(obj2.theString() == bslstl::StringRef(str, strlen(str)));

            SUBTEST("Verify that no memory was allocated during creation.");
            ASSERT(0 == ta.status());
            ASSERT(0 == tmpa.status());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datums by copying a string value of length 5.");
            const char *str = "12345";
            const Datum obj1 = Datum::copyString(str, &ta);
            const Datum obj2 = Datum::copyString(str, strlen(str), &ta);
            const Datum obj3 = Datum::copyString(bslstl::StringRef(str), &ta);

            SUBTEST("Check if objects have the same string value.");
            ASSERT(obj1.isString());
            ASSERT(obj2.isString());
            ASSERT(obj3.isString());
            ASSERT(obj1.theString() == bslstl::StringRef(str, strlen(str)));
            ASSERT(obj2.theString() == bslstl::StringRef(str, strlen(str)));
            ASSERT(obj3.theString() == bslstl::StringRef(str, strlen(str)));

            SUBTEST("Verify that no memory was allocated during creation.");
            ASSERT(0 == ta.status());
            ASSERT(0 == tmpa.status());

            SUBTEST("Copy-assign these objects and verify "
                    "that both objects have the same type and value.");
            Datum cObj1 = Datum::createNull();
            cObj1 = obj1;
            ASSERT(cObj1.isString());
            ASSERT(cObj1.theString() == bslstl::StringRef(str, strlen(str)));

            Datum cObj2 = Datum::createNull();
            cObj2 = obj2;
            ASSERT(cObj2.isString());
            ASSERT(cObj2.theString() == bslstl::StringRef(str, strlen(str)));
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datums by copying a string value of length 6.");
            const char *str = "123456";
            const Datum obj1 = Datum::copyString(str, &ta);
            const Datum obj2 = Datum::copyString(str, strlen(str), &ta);

            SUBTEST("Check if objects have the same string value.");
            ASSERT(obj1.isString());
            ASSERT(obj2.isString());
            ASSERT(obj1.theString() == bslstl::StringRef(str, strlen(str)));
            ASSERT(obj2.theString() == bslstl::StringRef(str, strlen(str)));

            SUBTEST("Verify that no memory was allocated during creation.");
            ASSERT(0 == ta.status());
            ASSERT(0 == tmpa.status());

            SUBTEST("Copy-assign these objects and verify "
                    "that both objects have the same type and value.");
            Datum cObj1 = Datum::createNull();
            cObj1 = obj1;
            ASSERT(cObj1.isString());
            ASSERT(cObj1.theString() == bslstl::StringRef(str, strlen(str)));

            Datum cObj2 = Datum::createNull();
            cObj2 = obj2;
            ASSERT(cObj2.isString());
            ASSERT(cObj2.theString() == bslstl::StringRef(str, strlen(str)));
        }

#if defined(BSLS_PLATFORM_CPU_64_BIT)
        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datums (64-bit) by copying"
                    " a string value of length 13.");
            const char *str = "0123456789abc"; // 16 - discriminator - length
            const Datum obj1 = Datum::copyString(str, &ta);
            const Datum obj2 = Datum::copyString(str, strlen(str), &ta);

            SUBTEST("Check if objects have the same string value.");
            ASSERT(obj1.isString());
            ASSERT(obj2.isString());
            ASSERT(obj1.theString() == bslstl::StringRef(str, strlen(str)));
            ASSERT(obj2.theString() == bslstl::StringRef(str, strlen(str)));

            SUBTEST("Verify that no memory was allocated during creation.");
            ASSERT(0 == ta.status());
            ASSERT(0 == tmpa.status());

            SUBTEST("Copy-assign these objects and verify "
                    "that both objects have the same type and value.");
            Datum cObj1 = Datum::createNull();
            cObj1 = obj1;
            ASSERT(cObj1.isString());
            ASSERT(cObj1.theString() == bslstl::StringRef(str, strlen(str)));

            Datum cObj2 = Datum::createNull();
            cObj2 = obj2;
            ASSERT(cObj2.isString());
            ASSERT(cObj2.theString() == bslstl::StringRef(str, strlen(str)));
        }
#endif

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datums by copying a string"
                    " value of length greater than the length "
                    "of a short-length string by 1 character.");
            const char *str =
#if defined(BSLS_PLATFORM_CPU_32_BIT)
                              "1234567";
#else
                              "123456789abcdefx";
#endif
            const Datum obj1 = Datum::copyString(str, &ta);
            const Datum obj2 = Datum::copyString(str, strlen(str), &tmpa);

            SUBTEST("Check if objects have the same string value.");
            ASSERT(obj1.isString());
            ASSERT(obj2.isString());
            ASSERT(obj1.theString() == bslstl::StringRef(str, strlen(str)));
            ASSERT(obj2.theString() == bslstl::StringRef(str, strlen(str)));

            SUBTEST("Verify that the string was copied.");
            ASSERT(0 != ta.status());
            ASSERT(0 != tmpa.status());

            SUBTEST("Copy-assign these objects and verify "
                    "that both objects have the same type and value.");
            Datum cObj1 = Datum::createNull();
            cObj1 = obj1;
            ASSERT(cObj1.isString());
            ASSERT(cObj1.theString() == bslstl::StringRef(str, strlen(str)));

            Datum cObj2 = Datum::createNull();
            cObj2 = obj2;
            ASSERT(cObj2.isString());
            ASSERT(cObj2.theString() == bslstl::StringRef(str, strlen(str)));

            SUBTEST("Destroy the objects and verify there "
                    "are no memory leaks.");
            Datum::destroy(obj1, &ta);
            Datum::destroy(obj2, &tmpa);
            ASSERT(0 == ta.status());
            ASSERT(0 == tmpa.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        TEST("Testing Datum::createUninitializedString().");

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum that holds an "
                    "uninitialized string buffer capable of "
                    "storing a long string value.");
            const char *str = "A long string constant";
            Datum obj;
            char *buffer = Datum::createUninitializedString(&obj,
                                                            strlen(str),
                                                            &ta);
            bsl::memcpy(buffer, str, strlen(str));

            SUBTEST("Check if object has the same string value.");
            ASSERT(obj.isString());
            ASSERT(obj.theString() == bslstl::StringRef(str, strlen(str)));

            SUBTEST("Verify that memory was allocated.");
            ASSERT(0 != ta.status());

            SUBTEST("Destroy the object and verify there"
                    " are no memory leaks.");
            Datum::destroy(obj, &ta);
            ASSERT(0 == ta.status());
        }

#if defined(BSLS_PLATFORM_CPU_64_BIT)
        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum (64-bit) that holds an "
                    " uninitialized string buffer capable of "
                    "storing a string value of length 13.");
            const char *str = "0123456789abc"; // 16 - discriminator - length
            Datum obj;
            char *buffer = Datum::createUninitializedString(&obj,
                                                            strlen(str),
                                                            &ta);
            bsl::memcpy(buffer, str, strlen(str));

            SUBTEST("Check if object has the same string value.");
            ASSERT(obj.isString());
            ASSERT(obj.theString() == bslstl::StringRef(str, strlen(str)));

            SUBTEST("Verify that no memory was allocated during creation.");
            ASSERT(0 == ta.status());
        }
#endif
        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum that holds an "
                    " uninitialized string buffer capable of "
                    "storing a zero-length string value.");
            const char *str = "";
            Datum obj;
            char *buffer = Datum::createUninitializedString(&obj,
                                                            strlen(str),
                                                            &ta);
            bsl::memcpy(buffer, str, strlen(str));

            SUBTEST("Check if object has the same string value.");
            ASSERT(obj.isString());
            ASSERT(obj.theString() == bslstl::StringRef(str, strlen(str)));

            SUBTEST("Verify that no memory was allocated during creation.");
            ASSERT(0 == ta.status());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum that holds an "
                    " uninitialized string buffer capable of "
                    "storing a string value of length 1.");
            const char *str = "1";
            Datum obj;
            char *buffer = Datum::createUninitializedString(&obj,
                                                            strlen(str),
                                                            &ta);
            bsl::memcpy(buffer, str, strlen(str));

            SUBTEST("Check if object has the same string value.");
            ASSERT(obj.isString());
            ASSERT(obj.theString() == bslstl::StringRef(str, strlen(str)));

            SUBTEST("Verify that no memory was allocated during creation.");
            ASSERT(0 == ta.status());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum that holds an "
                    " uninitialized string buffer capable of "
                    "storing a string value of length 2.");
            const char *str = "12";
            Datum obj;
            char *buffer = Datum::createUninitializedString(&obj,
                                                            strlen(str),
                                                            &ta);
            bsl::memcpy(buffer, str, strlen(str));

            SUBTEST("Check if object has the same string value.");
            ASSERT(obj.isString());
            ASSERT(obj.theString() == bslstl::StringRef(str, strlen(str)));

            SUBTEST("Verify that no memory was allocated during creation.");
            ASSERT(0 == ta.status());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum that holds an "
                    " uninitialized string buffer capable of "
                    "storing a string value of length 3.");
            const char *str = "123";
            Datum obj;
            char *buffer = Datum::createUninitializedString(&obj,
                                                            strlen(str),
                                                            &ta);
            bsl::memcpy(buffer, str, strlen(str));

            SUBTEST("Check if object has the same string value.");
            ASSERT(obj.isString());
            ASSERT(obj.theString() == bslstl::StringRef(str, strlen(str)));

            SUBTEST("Verify that no memory was allocated during creation.");
            ASSERT(0 == ta.status());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum that holds an "
                    " uninitialized string buffer capable of "
                    "storing a string value of length 4.");
            const char *str = "1234";
            Datum obj;
            char *buffer = Datum::createUninitializedString(&obj,
                                                            strlen(str),
                                                            &ta);
            bsl::memcpy(buffer, str, strlen(str));

            SUBTEST("Check if object has the same string value.");
            ASSERT(obj.isString());
            ASSERT(obj.theString() == bslstl::StringRef(str, strlen(str)));

            SUBTEST("Verify that no memory was allocated during creation.");
            ASSERT(0 == ta.status());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum that holds an "
                    " uninitialized string buffer capable of "
                    "storing a string value of length 5.");
            const char *str = "12345";
            Datum obj;
            char *buffer = Datum::createUninitializedString(&obj,
                                                            strlen(str),
                                                            &ta);
            bsl::memcpy(buffer, str, strlen(str));

            SUBTEST("Check if object has the same string value.");
            ASSERT(obj.isString());
            ASSERT(obj.theString() == bslstl::StringRef(str, strlen(str)));

            SUBTEST("Verify that no memory was allocated during creation.");
            ASSERT(0 == ta.status());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum that holds an "
                    " uninitialized string buffer capable of "
                    "storing a string value of length 6.");
            const char *str = "123456";
            Datum obj;
            char *buffer = Datum::createUninitializedString(&obj,
                                                            strlen(str),
                                                            &ta);
            bsl::memcpy(buffer, str, strlen(str));

            SUBTEST("Check if object has the same string value.");
            ASSERT(obj.isString());
            ASSERT(obj.theString() == bslstl::StringRef(str, strlen(str)));

            SUBTEST("Verify that no memory was allocated during creation.");
            ASSERT(0 == ta.status());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum that holds an "
                    "uninitialized string buffer capable of "
                    "storing a string value of length greater "
                    "than the length of a short-length string by "
                    "1 character.");
            const char *str =
#if defined(BSLS_PLATFORM_CPU_32_BIT)
                "1234567";
#else
                "123456789abcdefx";
#endif
            Datum obj;
            char *buffer = Datum::createUninitializedString(&obj,
                                                            strlen(str),
                                                            &ta);
            bsl::memcpy(buffer, str, strlen(str));

            SUBTEST("Check if object has the same string value.");
            ASSERT(obj.isString());
            ASSERT(obj.theString() == bslstl::StringRef(str, strlen(str)));

            SUBTEST("Verify that memory was allocated.");
            ASSERT(0 != ta.status());

            SUBTEST("Copy-assign the object and verify "
                    "that it has the same type and value.");
            Datum cObj = Datum::createNull();
            cObj = obj;
            ASSERT(cObj.isString());
            ASSERT(cObj.theString() == bslstl::StringRef(str, strlen(str)));

            SUBTEST("Destroy the object and verify there "
                    "are no memory leaks.");
            Datum::destroy(obj, &ta);
            ASSERT(0 == ta.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Testing Datum::createDatetime().");

            SUBTEST("Create Datum with a date+time value.");

            numAllocs = ta.numAllocations();
            const Datum obj = Datum::createDatetime(
                                          bdlt::Datetime(2010,1,5, 16,45,32,12),
                                          &ta);
            SUBTEST("Check that the date+time was stored"
                    " without using allocated storage.");

            ASSERT(ta.numAllocations() == numAllocs);

            SUBTEST("Check if object has the same date+time value.");
            ASSERT(obj.isDatetime());
            ASSERT(bdlt::Datetime(2010,1,5, 16,45,32,12) == obj.theDatetime());

            SUBTEST("Copy-assign this object and verify "
                    "that both objects have the same type and value.");
            Datum cObj = Datum::createNull();
            numAllocs = ta.numAllocations();
            cObj = obj;
            ASSERT(cObj.isDatetime());
            ASSERT(bdlt::Datetime(2010,1,5, 16,45,32,12) == cObj.theDatetime());
            ASSERT(ta.numAllocations() == numAllocs);

            SUBTEST("Destroy the object and verify there "
                    "are no memory leaks.");
            Datum::destroy(obj, &ta);
            ASSERT(0 == ta.status());
        }

#if defined(BSLS_PLATFORM_CPU_32_BIT)
        {
            SUBTEST("Create Datum with large date+time value.");
            numAllocs = ta.numAllocations();
            const Datum obj = Datum::createDatetime(
                bdlt::Datetime(9999, 1, 5, 16, 45, 32, 12),
                &ta);
            SUBTEST("Check that the date+time was stored"
                    " using allocated storage.");

            ASSERT(ta.numAllocations() > numAllocs);

            SUBTEST("Check if object has the same date+time value.");
            ASSERT(obj.isDatetime());
            ASSERT(bdlt::Datetime(9999,1,5, 16,45,32,12) == obj.theDatetime());

            SUBTEST("Copy-assign this object and verify "
                    "that both objects have the same type and value.");
            Datum cObj = Datum::createNull();
            numAllocs = ta.numAllocations();
            cObj = obj;
            ASSERT(cObj.isDatetime());
            ASSERT(bdlt::Datetime(9999,1,5,16,45,32,12) == cObj.theDatetime());
            ASSERT(ta.numAllocations() == numAllocs);

            SUBTEST("Destroy the object and verify there "
                    "are no memory leaks.");
            Datum::destroy(obj, &ta);
            ASSERT(0 == ta.status());
        }
#endif

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Testing Datum::createDatetimeInterval().");

            SUBTEST("Create Datum with a date+time interval value.");
            const Datum obj = Datum::createDatetimeInterval(
                                        bdlt::DatetimeInterval(34, 16,45,32,12),
                                        &ta);

            SUBTEST("Check that the interval was stored"
                    " without using allocated storage.");
            ASSERT(ta.numAllocations() == numAllocs);

            SUBTEST("Check if object has the same date+time interval value.");
            ASSERT(obj.isDatetimeInterval());
            ASSERT(bdlt::DatetimeInterval(34, 16,45,32,12) ==
                                                    obj.theDatetimeInterval());

            SUBTEST("Copy-assign this object and verify that"
                    " both objects have the same type and value "
                    "and no new allocations happened.");
            Datum cObj = Datum::createNull();
            numAllocs = ta.numAllocations();
            cObj = obj;
            ASSERT(cObj.isDatetimeInterval());
            ASSERT(bdlt::DatetimeInterval(34, 16,45,32,12) ==
                                                   cObj.theDatetimeInterval());
            ASSERT(ta.numAllocations() == numAllocs);

            SUBTEST("Destroy the object and verify there "
                    "are no memory leaks.");
            Datum::destroy(obj, &ta);
            ASSERT(0 == ta.status());
        }

#if defined(BSLS_PLATFORM_CPU_32_BIT)
        // Force use of allocated storage
        // ------------------------------

        {
            numAllocs = ta.numAllocations();
            const Datum obj = Datum::createDatetimeInterval(
                bdlt::DatetimeInterval(1000 * 1000 * 1000, 12, 34),
                &ta);
            SUBTEST("Create a big interval and check it was"
                    " stored using allocated storage.");
            ASSERT(ta.numAllocations() > numAllocs);
            SUBTEST("Check if object has the same date+time interval value.");
            ASSERT(obj.isDatetimeInterval());
            ASSERT(bdlt::DatetimeInterval(1000 * 1000 * 1000, 12, 34) ==
                   obj.theDatetimeInterval());

            SUBTEST("Copy-assign this object and verify that"
                    " both objects have the same type and value "
                    "and no new allocations happened.");
            Datum cObj = Datum::createNull();
            numAllocs = ta.numAllocations();
            cObj = obj;
            ASSERT(cObj.isDatetimeInterval());
            ASSERT(bdlt::DatetimeInterval(1000 * 1000 * 1000, 12, 34) ==
                   cObj.theDatetimeInterval());
            ASSERT(ta.numAllocations() == numAllocs);

            SUBTEST("Destroy the object and verify there "
                    "are no memory leaks.");
            Datum::destroy(obj, &ta);
            ASSERT(0 == ta.status());
        }
#endif
        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Testing Datum::createInteger64().");

            SUBTEST("Create Datum with an Int64 value.");
            const Datum obj = Datum::createInteger64(
                                            Int64(9223372036854775807LL), &ta);

            SUBTEST("Check if object has the same Int64 value.");
            ASSERT(obj.isInteger64());
            ASSERT(Int64(9223372036854775807LL) == obj.theInteger64());

            SUBTEST("Verify that streaming operator outputs the correctly"
                    "formatted value.");

            bsl::ostringstream out;
            out << obj;
            ASSERT(out.str() ==  "9223372036854775807");

            SUBTEST("Copy-assign this object and verify that"
                    " both objects have the same type and value"
                    " and no new allocations happened.");
            Datum cObj = Datum::createNull();
            numAllocs = ta.numAllocations();
            cObj = obj;
            ASSERT(cObj.isInteger64());
            ASSERT(Int64(9223372036854775807LL) == cObj.theInteger64());
            ASSERT(ta.numAllocations() == numAllocs);

            SUBTEST("Destroy the object and verify there "
                    "are no memory leaks.");
            Datum::destroy(obj, &ta);
            ASSERT(0 == ta.status());
        }

        {
            SUBTEST("Create Datum objects with a small Int64 value.");

            numAllocs = ta.numAllocations();
            typedef bsls::Types::Int64 Int64;
            Int64 expectedValue1(140737488355327LL); // (1 << 47) - 1;
            Int64 expectedValue2(~expectedValue1);
            const Datum obj1 = Datum::createInteger64(expectedValue1, &ta);
            const Datum obj2 = Datum::createInteger64(expectedValue2, &ta);

            SUBTEST("Check that the integers were stored"
                    " without using allocated storage.");
            ASSERT(ta.numAllocations() == numAllocs);

            SUBTEST("Check if objects have the same Int64 value.");
            ASSERT(obj1.isInteger64());
            ASSERT(expectedValue1 == obj1.theInteger64());
            ASSERT(obj2.isInteger64());
            ASSERT(expectedValue2 == obj2.theInteger64());

            SUBTEST("Copy-assign an object and verify that"
                    " both objects have the same type and value"
                    " and no new allocations happened.");
            Datum cObj = Datum::createNull();
            numAllocs = ta.numAllocations();
            cObj = obj1;
            ASSERT(cObj.isInteger64());
            ASSERT(expectedValue1 == cObj.theInteger64());
            ASSERT(ta.numAllocations() == numAllocs);

            SUBTEST("Destroy the objects and verify there are"
                    " no memory leaks.");
            Datum::destroy(obj1, &ta);
            Datum::destroy(obj2, &ta);
            ASSERT(0 == ta.status());

            SUBTEST("Verify that streaming operator outputs"
                    " the correctly formatted value.");
            bsl::ostringstream out;
            out << obj1;
            ASSERT(out.str() == "140737488355327");
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        TEST("Testing Datum::createDouble().");

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with a double value of '3.1416'.");
            const Datum obj = Datum::createDouble(3.1416);

            SUBTEST("Check if object has the same double value.");
            ASSERT(obj.isDouble());
            ASSERT(3.1416 == obj.theDouble());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with a double value of '-3.1416'.");
            const Datum obj = Datum::createDouble(-3.1416);

            SUBTEST("Check if object has the same double value.");
            ASSERT(obj.isDouble());
            ASSERT(-3.1416 == obj.theDouble());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with a double value of '0'.");
            const Datum obj = Datum::createDouble(DBL_ZERO);

            SUBTEST("Check if object has the same double value.");
            ASSERT(obj.isDouble());
            ASSERT(DBL_ZERO == obj.theDouble());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the minimum double value.");
            const Datum obj = Datum::createDouble(DBL_MIN2);

            SUBTEST("Check if object has the same double value.");
            ASSERT(obj.isDouble());
            ASSERT(DBL_MIN2 == obj.theDouble());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the maximum double value.");
            const Datum obj = Datum::createDouble(DBL_MAX2);

            SUBTEST("Check if object has the same double value.");
            ASSERT(obj.isDouble());
            ASSERT(DBL_MAX2 == obj.theDouble());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the double value of infinity.");
            const Datum obj = Datum::createDouble(DBL_INF);

            SUBTEST("Check if object has the same double value.");
            ASSERT(obj.isDouble());
            ASSERT(DBL_INF == obj.theDouble());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the double value "
                    "of negative infinity.");
            const Datum obj = Datum::createDouble(DBL_NEGINF);

            SUBTEST("Check if object has the same double value.");
            ASSERT(obj.isDouble());
            ASSERT(DBL_NEGINF == obj.theDouble());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the double value of negative zero.");
            const Datum obj = Datum::createDouble(DBL_NEGZERO);

            SUBTEST("Check if object has the same double value.");
            ASSERT(obj.isDouble());
            ASSERT(DBL_NEGZERO == obj.theDouble());

            SUBTEST("Copy-assign this object and verify that"
                    " both objects have the same type and value.");
            Datum cObj = Datum::createNull();
            cObj = obj;
            ASSERT(cObj.isDouble());
            ASSERT(DBL_NEGZERO == cObj.theDouble());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the double "
                    "value of quiet NaN.");
            const Datum obj = Datum::createDouble(DBL_QNAN2);

            SUBTEST("Check if object has the same double value.");
            ASSERT(obj.isDouble());
            ASSERT(!(obj.theDouble() == obj.theDouble()));
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the double value of signaling NaN.");
            const Datum obj = Datum::createDouble(
                                      numeric_limits<double>::signaling_NaN());

            SUBTEST("Check if object has the same double value.");
            ASSERT(obj.isDouble());
            ASSERT(!(obj.theDouble() == obj.theDouble()));
        }

        // Windows test only.
#ifdef WIN32
        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the indeterminate double value.");
            const Datum obj = Datum::createDouble(DBL_IND);

            SUBTEST("Check if object has the same double value.");
            ASSERT(obj.isDouble());
            ASSERT(!(obj.theDouble() == obj.theDouble()));
        }
#endif

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        TEST("Testing Datum::createDecimal64().");
        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with a Decimal64 value of '0.20'.");
            const Datum obj = Datum::createDecimal64(BDLDFP_DECIMAL_DD(0.20),
                                                     alloc);

            SUBTEST("Check if object has the same Decimal64 value.");
            ASSERT(obj.isDecimal64());
            ASSERT(BDLDFP_DECIMAL_DD(0.20) == obj.theDecimal64());

            SUBTEST("Verify that streaming operator outputs"
                    " the correctly formatted value.");
            bsl::ostringstream out;
            out << obj;
            ASSERT(out.str() == "0.20");
            Datum::destroy(obj, alloc);
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with a Decimal64 value of '-0.2'.");
            const Datum obj = Datum::createDecimal64(BDLDFP_DECIMAL_DD(-0.2),
                                                     alloc);

            SUBTEST("Check if object has the same Decimal64 value.");
            ASSERT(obj.isDecimal64());
            ASSERT(BDLDFP_DECIMAL_DD(-0.2) == obj.theDecimal64());
            Datum::destroy(obj, alloc);
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with a Decimal64 value of '0'.");
            const Datum obj = Datum::createDecimal64(BDLDFP_DECIMAL_DD(0.),
                                                     alloc);

            SUBTEST("Check if object has the same Decimal64 value.");
            ASSERT(obj.isDecimal64());
            ASSERT(BDLDFP_DECIMAL_DD(0.) == obj.theDecimal64());
            Datum::destroy(obj, alloc);
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the minimum Decimal64 value.");
            const Datum obj = Datum::createDecimal64(
                                              numeric_limits<Decimal64>::min(),
                                              alloc);

            SUBTEST("Check if object has the same Decimal64 value.");
            ASSERT(obj.isDecimal64());
            ASSERT(numeric_limits<Decimal64>::min() == obj.theDecimal64());
            Datum::destroy(obj, alloc);
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the maximum Decimal64 value.");
            const Datum obj = Datum::createDecimal64(
                                              numeric_limits<Decimal64>::max(),
                                              alloc);

            SUBTEST("Check if object has the same Decimal64 value.");
            ASSERT(obj.isDecimal64());
            ASSERT(numeric_limits<Decimal64>::max() == obj.theDecimal64());
            Datum::destroy(obj, alloc);
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the Decimal64 value of infinity.");
            const Datum obj = Datum::createDecimal64(
                                         numeric_limits<Decimal64>::infinity(),
                                         alloc);

            SUBTEST("Check if object has the same Decimal64 value.");
            ASSERT(obj.isDecimal64());
            ASSERT(
                  numeric_limits<Decimal64>::infinity() == obj.theDecimal64());
            Datum::destroy(obj, alloc);
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the Decimal64 value of negative"
                    "infinity.");
            Decimal64 negativeInfinity;
            bdldfp::DecimalUtil::parseDecimal64(&negativeInfinity, "-inf");
            const Datum obj = Datum::createDecimal64(negativeInfinity,
                                                     alloc);

            SUBTEST("Check if object has the same Decimal64 value.");
            ASSERT(obj.isDecimal64());
            ASSERT(negativeInfinity == obj.theDecimal64());
            Datum::destroy(obj, alloc);
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the Decimal64 value of negative zero.");
            const Datum obj = Datum::createDecimal64(BDLDFP_DECIMAL_DD(-0.0),
                                                     alloc);

            SUBTEST("Check if object has the same Decimal64 value.");
            ASSERT(obj.isDecimal64());
            ASSERT(BDLDFP_DECIMAL_DD(-0.0) == obj.theDecimal64());

            SUBTEST("Copy-assign this object and verify that"
                    " both objects have the same type and value.");
            Datum cObj = Datum::createNull();
            cObj = obj;
            ASSERT(cObj.isDecimal64());
            ASSERT(BDLDFP_DECIMAL_DD(-0.0) == cObj.theDecimal64());
            Datum::destroy(obj, alloc);
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the Decimal64 "
                    "value of quiet NaN.");
            const Datum obj = Datum::createDecimal64(
                                        numeric_limits<Decimal64>::quiet_NaN(),
                                        alloc);

            SUBTEST("Check if object has the same Decimal64 value.");
            ASSERT(obj.isDecimal64());
            ASSERT(!(obj.theDecimal64() == obj.theDecimal64()));
            Datum::destroy(obj, alloc);
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum with the Decimal64 value of signaling NaN.");
            const Datum obj = Datum::createDecimal64(
                                    numeric_limits<Decimal64>::signaling_NaN(),
                                    alloc);

            SUBTEST("Check if object has the same Decimal64 value.");
            ASSERT(obj.isDecimal64());
            ASSERT(!(obj.theDecimal64() == obj.theDecimal64()));
            Datum::destroy(obj, alloc);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
        TEST("Testing Datum::createUdt().");

            SUBTEST("Create Datum with a user defined object.");
            const Datum obj = Datum::createUdt(&udtValue, UDT_TYPE);

            SUBTEST("Check if object has the same user defined object.");
            ASSERT(obj.isUdt());
            const Udt udt(&udtValue, UDT_TYPE);
            ASSERT(obj.theUdt() == udt);

            SUBTEST("Copy-assign this object and verify that"
                    " both objects have the same type and value"
                    " and no new allocations happened.");
            Datum cObj = Datum::createNull();
            numAllocs = ta.numAllocations();
            cObj = obj;
            ASSERT(cObj.isUdt());
            ASSERT(cObj.theUdt() == udt);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Testing Datum::createUdt().");

            SUBTEST("Create Datum with a zero pointer");
            const Datum obj = Datum::createUdt(0, UDT_TYPE);

            SUBTEST("Check if object has the same user defined object.");
            ASSERT(obj.isUdt());
            const Udt udt(0, UDT_TYPE);
            ASSERT(obj.theUdt() == udt);
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        TEST("Testing Datum::createUninitializedArray()"
             " and Datum::adoptArray().");

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create an uninitialized array of "
                    "Datum objects having larger capacity than length.");
            const size_type capacity = 10;
            DatumArrayRef array;
            Datum::createUninitializedArray(&array, capacity, &ta);

            SUBTEST("Fill in elements into the array not"
                " up to capacity and adopt the array into a Datum object.");
            array.data()[0] = Datum::createInteger(0);
            array.data()[1] = Datum::createDouble(-3.1416);
            array.data()[2] = Datum::copyString("A long string", &ta);
            array.data()[3] = Datum::copyString("Abc", &ta);
            array.data()[4] = Datum::createDate(bdlt::Date(2010,1,5));
            array.data()[5] = Datum::createDatetime(
                                          bdlt::Datetime(2010,1,5, 16,45,32,12),
                                          &ta);
            *(array.length()) = 6;
            Datum obj = Datum::adoptArray(array);

            SUBTEST("Check if object has the same array value.");
            ASSERT(obj.isArray());
            ConstDatumArrayRef aref(array.data(), *array.length());
            ASSERT(obj.theArray() == aref);

            SUBTEST("Copy-assign this object and verify "
                    " that both objects have the same type and"
                    " value and no new allocations happened.");
            Datum cObj = Datum::createNull();
            const Int64 numAllocs = ta.numAllocations();
            cObj = obj;
            ASSERT(cObj.isArray());
            ASSERT(cObj.theArray() == aref);
            ASSERT(ta.numAllocations() == numAllocs);

            SUBTEST("Destroy the object and verify there "
                    "are no memory leaks.");
            Datum::destroy(obj, &ta);
            ASSERT(0 == ta.status());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum holding an empty array.");
            DatumArrayRef array;
            Datum obj = Datum::adoptArray(array);

            SUBTEST("Check if object has the same array value.");
            ASSERT(obj.isArray());
            Datum arr[1];
            ConstDatumArrayRef aref(arr, 0);
            ASSERT(obj.theArray() == aref);

            SUBTEST("Copy-assign this object and verify "
                    "that both objects have the same type and "
                    "value and no new allocations happened.");
            Datum cObj = Datum::createNull();
            numAllocs = ta.numAllocations();
            cObj = obj;
            ASSERT(cObj.isArray());
            ASSERT(cObj.theArray() == aref);
            ASSERT(ta.numAllocations() == numAllocs);

            SUBTEST("Destroy the object and verify there are"
                    " no memory leaks.");
            Datum::destroy(obj, &ta);
            ASSERT(0 == ta.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        TEST("Testing Datum::createUninitializedMap() and Datum::adoptMap().");

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create an uninitialized map of "
                    "Datum objects having larger capacity than length.");
            const size_type capacity = 10;
            DatumMapRef mapping;
            Datum::createUninitializedMap(&mapping,
                                                     capacity,
                                                     &ta);

            SUBTEST("Fill in elements into the map not "
                    "up to capacity and adopt the map into a "
                    "Datum object.");
            mapping.data()[0] =
                DatumMapEntry(StringRef("first", strlen("first")),
                              Datum::createInteger(0));
            mapping.data()[1] =
                DatumMapEntry(StringRef("second", strlen("second")),
                              Datum::createDouble(-3.1416));
            mapping.data()[2] =
                DatumMapEntry(StringRef("third", strlen("third")),
                              Datum::copyString("A long string", &ta));
            mapping.data()[3] =
                DatumMapEntry(StringRef("fourth", strlen("fourth")),
                              Datum::copyString("Abc", &ta));
            mapping.data()[4] =
                DatumMapEntry(StringRef("fifth", strlen("fifth")),
                              Datum::createDate(bdlt::Date(2010,1,5)));
            mapping.data()[5] =
                DatumMapEntry(StringRef("sixth", strlen("sixth")),
                              Datum::createDatetime(
                                          bdlt::Datetime(2010,1,5, 16,45,32,12),
                                          &ta));
            *(mapping.size()) = 6;
            Datum obj = Datum::adoptMap(mapping);

            SUBTEST("Check if object has the same map value.");
            ASSERT(obj.isMap());
            ConstDatumMapRef mref(mapping.data(), *mapping.size(), false);
            ASSERT(obj.theMap() == mref);

            SUBTEST("Copy-assign this object and verify"
                    " that both objects have the same type and"
                    " value and no new allocations happened.");
            Datum cObj = Datum::createNull();
            const Int64 numAllocs = ta.numAllocations();
            cObj = obj;
            ASSERT(cObj.isMap());
            ASSERT(cObj.theMap() == mref);
            ASSERT(ta.numAllocations() == numAllocs);

            SUBTEST("Destroy the object and verify there "
                    "are no memory leaks.");
            Datum::destroy(obj, &ta);
            ASSERT(0 == ta.status());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum holding an empty map.");
            DatumMapRef mapping;
            Datum obj = Datum::adoptMap(mapping);

            SUBTEST("Check if object has the same map value.");
            ASSERT(obj.isMap());
            DatumMapEntry *mp = 0;
            ConstDatumMapRef mref(mp, 0, false);
            ASSERT(obj.theMap() == mref);

            SUBTEST("Copy-assign this object and verify "
                    " that both objects have the same type and"
                    " value and no new allocations happened.");
            Datum cObj = Datum::createNull();
            numAllocs = ta.numAllocations();
            cObj = obj;
            ASSERT(cObj.isMap());
            ASSERT(cObj.theMap() == mref);
            ASSERT(ta.numAllocations() == numAllocs);

            SUBTEST("Destroy the object and verify there "
                    "are no memory leaks.");
            Datum::destroy(obj, &ta);
            ASSERT(0 == ta.status());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create an uninitialized map of "
                    " Datum objects having larger capacity"
                    " than length, which is sorted.");
            const size_type capacity = 10;
            DatumMapRef mapping;
            Datum::createUninitializedMap(&mapping,
                                          capacity,
                                          &ta);

            SUBTEST("Fill in elements into the map not"
                    " up to capacity in a sorted manner and adopt"
                    " the map into a Datum object.");
            mapping.data()[0] =
                DatumMapEntry(StringRef("fifth", strlen("fifth")),
                              Datum::createInteger(0));
            mapping.data()[1] =
                DatumMapEntry(StringRef("first", strlen("first")),
                              Datum::createDouble(-3.1416));
            mapping.data()[2] =
                DatumMapEntry(StringRef("fourth", strlen("fourth")),
                              Datum::copyString("A long string", &ta));
            mapping.data()[3] =
                DatumMapEntry(StringRef("second", strlen("second")),
                              Datum::copyString("Abc", &ta));
            mapping.data()[4] =
                DatumMapEntry(StringRef("sixth", strlen("sixth")),
                              Datum::createDate(bdlt::Date(2010,1,5)));
            mapping.data()[5] =
                DatumMapEntry(StringRef("third", strlen("third")),
                              Datum::createDatetime(
                                          bdlt::Datetime(2010,1,5, 16,45,32,12),
                                          &ta));
            *(mapping.size()) = 6;
            *(mapping.sorted()) = true;
            Datum obj = Datum::adoptMap(mapping);

            SUBTEST("Check if object has the same map value.");
            ASSERT(obj.isMap());
            ConstDatumMapRef mref(mapping.data(), *mapping.size(), true);
            ASSERT(obj.theMap() == mref);

            SUBTEST("Copy-assign this object and verify"
                    " that both objects have the same type and"
                    " value and no new allocations happened.");
            Datum cObj = Datum::createNull();
            const Int64 numAllocs = ta.numAllocations();
            cObj = obj;
            ASSERT(cObj.isMap());
            ASSERT(cObj.theMap() == mref);
            ASSERT(ta.numAllocations() == numAllocs);

            SUBTEST("Destroy the object and verify there "
                    "are no memory leaks.");
            Datum::destroy(obj, &ta);
            ASSERT(0 == ta.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        TEST("Testing Datum::createUninitializedMapOwningKeys()"
             " and Datum::adoptMapOwningKeys().");

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create an uninitialized map (owning "
                    "keys) of Datum objects having larger "
                    "capacity than length.");
            const size_type capacity = 10;
            DatumMapOwningKeysRef mapping;
            const char *kys[6] = {"first", "second", "third",
                                  "fourth", "fifth", "sixth" };
            size_type keysCapacity = 0;
            for (size_type i = 0; i < 6; ++i) {
                keysCapacity += strlen(kys[i]);
            }
            Datum::createUninitializedMapOwningKeys(&mapping,
                                                    capacity,
                                                    keysCapacity,
                                                    &ta);

            SUBTEST("Fill in elements into the map not"
                    " up to capacity and adopt the map into a Datum object.");
            char *keys = mapping.keys();
            bsl::memcpy(keys, kys[0], strlen(kys[0]));
            mapping.data()[0] =
                DatumMapEntry(StringRef(keys, strlen(kys[0])),
                              Datum::createInteger(0));
            keys += strlen(kys[0]);
            bsl::memcpy(keys, kys[1], strlen(kys[1]));
            mapping.data()[1] =
                DatumMapEntry(StringRef(keys, strlen(kys[1])),
                              Datum::createDouble(-3.1416));
            keys += strlen(kys[1]);
            bsl::memcpy(keys, kys[2], strlen(kys[2]));
            mapping.data()[2] =
                DatumMapEntry(StringRef(keys, strlen(kys[2])),
                              Datum::copyString("A long string", &ta));
            keys += strlen(kys[2]);
            bsl::memcpy(keys, kys[3], strlen(kys[3]));
            mapping.data()[3] =
                DatumMapEntry(StringRef(keys, strlen(kys[3])),
                              Datum::copyString("Abc", &ta));
            keys += strlen(kys[3]);
            bsl::memcpy(keys, kys[4], strlen(kys[4]));
            mapping.data()[4] =
                DatumMapEntry(StringRef(keys, strlen(kys[4])),
                              Datum::createDate(bdlt::Date(2010,1,5)));
            keys += strlen(kys[4]);
            bsl::memcpy(keys, kys[5], strlen(kys[5]));
            mapping.data()[5] =
                DatumMapEntry(StringRef(keys, strlen(kys[5])),
                              Datum::createDatetime(
                                          bdlt::Datetime(2010,1,5, 16,45,32,12),
                                          &ta));
            *(mapping.size()) = 6;
            Datum obj = Datum::adoptMapOwningKeys(mapping);

            SUBTEST("Check if object has the same map value.");
            ASSERT(obj.isMap());
            ConstDatumMapRef mref(mapping.data(), *mapping.size(), false);
            ASSERT(obj.theMap() == mref);

            SUBTEST("Copy-assign this object and verify"
                    " that both objects have the same type and"
                    " value and no new allocations happened.");
            Datum cObj = Datum::createNull();
            const Int64 numAllocs = ta.numAllocations();
            cObj = obj;
            ASSERT(cObj.isMap());
            ASSERT(cObj.theMap() == mref);
            ASSERT(ta.numAllocations() == numAllocs);

            SUBTEST("Destroy the object and verify there "
                    "are no memory leaks.");
            Datum::destroy(obj, &ta);
            ASSERT(0 == ta.status());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datum holding an empty map (owning keys).");
            DatumMapOwningKeysRef mapping;
            Datum obj = Datum::adoptMapOwningKeys(mapping);

            SUBTEST("Check if object has the same map value.");
            ASSERT(obj.isMap());
            DatumMapEntry *mp = 0;
            ConstDatumMapRef mref(mp, 0, false);
            ASSERT(obj.theMap() == mref);

            SUBTEST("Copy-assign this object and verify"
                    " that both objects have the same type and"
                    " value and no new allocations happened.");
            Datum cObj = Datum::createNull();
            numAllocs = ta.numAllocations();
            cObj = obj;
            ASSERT(cObj.isMap());
            ASSERT(cObj.theMap() == mref);
            ASSERT(ta.numAllocations() == numAllocs);

            SUBTEST("Destroy the object and verify there "
                    "are no memory leaks.");
            Datum::destroy(obj, &ta);
            ASSERT(0 == ta.status());
        }

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create an uninitialized map (owning"
                    " keys) of Datum objects having larger"
                    " capacity than length, which is sorted.");
            const size_type capacity = 10;
            DatumMapOwningKeysRef mapping;
            const char *kys[6] = {"fifth", "first", "fourth",
                                  "second", "sixth", "third" };
            size_type keysCapacity = 0;
            for (size_type i = 0; i < 6; ++i) {
                keysCapacity += strlen(kys[i]);
            }
            Datum::createUninitializedMapOwningKeys(&mapping,
                                                    capacity,
                                                    keysCapacity,
                                                    &ta);

            SUBTEST("Fill in elements into the map not "
                    "up to capacity and adopt the map into a "
                    "Datum object.");
            char *keys = mapping.keys();
            bsl::memcpy(keys, kys[0], strlen(kys[0]));
            mapping.data()[0] =
                DatumMapEntry(StringRef(keys, strlen(kys[0])),
                              Datum::createInteger(0));
            keys += strlen(kys[0]);
            bsl::memcpy(keys, kys[1], strlen(kys[1]));
            mapping.data()[1] =
                DatumMapEntry(StringRef(keys, strlen(kys[1])),
                              Datum::createDouble(-3.1416));
            keys += strlen(kys[1]);
            bsl::memcpy(keys, kys[2], strlen(kys[2]));
            mapping.data()[2] =
                DatumMapEntry(StringRef(keys, strlen(kys[2])),
                              Datum::copyString("A long string", &ta));
            keys += strlen(kys[2]);
            bsl::memcpy(keys, kys[3], strlen(kys[3]));
            mapping.data()[3] =
                DatumMapEntry(StringRef(keys, strlen(kys[3])),
                              Datum::copyString("Abc", &ta));
            keys += strlen(kys[3]);
            bsl::memcpy(keys, kys[4], strlen(kys[4]));
            mapping.data()[4] =
                DatumMapEntry(StringRef(keys, strlen(kys[4])),
                              Datum::createDate(bdlt::Date(2010,1,5)));
            keys += strlen(kys[4]);
            bsl::memcpy(keys, kys[5], strlen(kys[5]));
            mapping.data()[5] =
                DatumMapEntry(StringRef(keys, strlen(kys[5])),
                              Datum::createDatetime(
                                          bdlt::Datetime(2010,1,5, 16,45,32,12),
                                          &ta));
            *(mapping.size()) = 6;
            *(mapping.sorted()) = true;
            Datum obj = Datum::adoptMapOwningKeys(mapping);

            SUBTEST("Check if object has the same map value.");
            ASSERT(obj.isMap());
            ConstDatumMapRef mref(mapping.data(), *mapping.size(), true);
            ASSERT(obj.theMap() == mref);

            SUBTEST("Copy-assign this object and verify "
                    "that both objects have the same type and "
                    "value and no new allocations happened.");
            Datum cObj = Datum::createNull();
            const Int64 numAllocs = ta.numAllocations();
            cObj = obj;
            ASSERT(cObj.isMap());
            ASSERT(cObj.theMap() == mref);
            ASSERT(ta.numAllocations() == numAllocs);

            SUBTEST("Destroy the object and verify there "
                    "are no memory leaks.");
            Datum::destroy(obj, &ta);
            ASSERT(0 == ta.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Testing creation of array holding a map as an element.");

            SUBTEST("Create Datum with an array value.");

            const size_type srcMapSize = 6;
            DatumMapEntry srcMap[srcMapSize] = {
                DatumMapEntry(StringRef("first", strlen("first")),
                              Datum::createInteger(0)),
                DatumMapEntry(StringRef("second", strlen("second")),
                              Datum::createDouble(-3.1416)),
                DatumMapEntry(StringRef("third", strlen("third")),
                              Datum::copyString("A long string", &ta)),
                DatumMapEntry(StringRef("fourth", strlen("fourth")),
                              Datum::copyString("Abc", &ta)),
                DatumMapEntry(StringRef("fifth", strlen("fifth")),
                              Datum::createDate(bdlt::Date(2010,1,5))),
                DatumMapEntry(StringRef("sixth", strlen("sixth")),
                              Datum::createDatetime(
                                          bdlt::Datetime(2010,1,5, 16,45,32,12),
                                          &ta))
            };

            DatumMapRef mapping;
            Datum::createUninitializedMap(&mapping, srcMapSize, &ta);
            for (size_type i = 0; i < srcMapSize; ++i) {
                mapping.data()[i] = srcMap[i];
            }
            *(mapping.size()) = srcMapSize;

            const size_type srcArraySize  = 6;
            Datum srcArray[srcArraySize];
            srcArray[0] = Datum::createInteger(0);
            srcArray[1] = Datum::createDouble(-3.1416);
            srcArray[2] = Datum::copyString("A long string", &ta);
            srcArray[3] = Datum::copyString("Abc", &ta);
            srcArray[4] = Datum::createDate(bdlt::Date(2010,1,5));
            srcArray[5] = Datum::adoptMap(mapping);

            DatumArrayRef array;
            Datum::createUninitializedArray(&array, srcArraySize, &ta);
            for (size_type i = 0; i < srcArraySize; ++i) {
                array.data()[i] = srcArray[i];
            }
            *(array.length()) = srcArraySize;
            Datum obj = Datum::adoptArray(array);

            SUBTEST("Check if object has the same array value.");
            ASSERT(obj.isArray());
            ConstDatumArrayRef aref(srcArray, srcArraySize);
            ASSERT(obj.theArray() == aref);

            SUBTEST("Copy-assign this object and verify "
                    "that both objects have the same type and "
                    "value and no new allocations happened.");
            Datum cObj = Datum::createNull();
            numAllocs = ta.numAllocations();
            cObj = obj;
            ASSERT(cObj.isArray());
            ASSERT(cObj.theArray() == aref);
            ASSERT(ta.numAllocations() == numAllocs);

            SUBTEST("Destroy the object and verify there "
                    "are no memory leaks.");
            Datum::destroy(obj, &ta);
            ASSERT(0 == ta.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
        TEST("Testing creation of map holding an array as an element.");

            SUBTEST("Create Datum with a map value.");

            const size_type srcArraySize  = 6;
            Datum srcArray[srcArraySize];
            srcArray[0] = Datum::createInteger(0);
            srcArray[1] = Datum::createDouble(-3.1416);
            srcArray[2] = Datum::copyString("A long string", &ta);
            srcArray[3] = Datum::copyString("Abc", &ta);
            srcArray[4] = Datum::createDate(bdlt::Date(2010,1,5));
            srcArray[5] = Datum::createDatetime(
                                          bdlt::Datetime(2010,1,5, 16,45,32,12),
                                          &ta);

            DatumArrayRef array;
            Datum::createUninitializedArray(&array, srcArraySize, &ta);
            for (size_type i = 0; i < srcArraySize; ++i) {
                array.data()[i] = srcArray[i];
            }
            *(array.length()) = srcArraySize;

            const size_type srcMapSize = 6;
            DatumMapEntry srcMap[srcMapSize] = {
                DatumMapEntry(StringRef("first", strlen("first")),
                              Datum::createInteger(0)),
                DatumMapEntry(StringRef("second", strlen("second")),
                              Datum::createDouble(-3.1416)),
                DatumMapEntry(StringRef("third", strlen("third")),
                              Datum::copyString("A long string", &ta)),
                DatumMapEntry(StringRef("fourth", strlen("fourth")),
                              Datum::copyString("Abc", &ta)),
                DatumMapEntry(StringRef("fifth", strlen("fifth")),
                              Datum::createDate(bdlt::Date(2010,1,5))),
                DatumMapEntry(StringRef("sixth", strlen("sixth")),
                              Datum::adoptArray(array))
            };

            DatumMapRef mapping;
            Datum::createUninitializedMap(&mapping, srcMapSize, &ta);
            for (size_type i = 0; i < srcMapSize; ++i) {
                mapping.data()[i] = srcMap[i];
            }
            *(mapping.size()) = srcMapSize;
            Datum obj = Datum::adoptMap(mapping);

            SUBTEST("Check if object has the same map value.");
            ASSERT(obj.isMap());
            ConstDatumMapRef mref(srcMap, srcMapSize, false);
            ASSERT(obj.theMap() == mref);

            SUBTEST("Copy-assign this object and verify "
                    "that both objects have the same type and "
                    "value and no new allocations happened.");
            Datum cObj = Datum::createNull();
            numAllocs = ta.numAllocations();
            cObj = obj;
            ASSERT(cObj.isMap());
            ASSERT(cObj.theMap() == mref);
            ASSERT(ta.numAllocations() == numAllocs);

            SUBTEST("Destroy the object and verify there "
                    "are no memory leaks.");
            Datum::destroy(obj, &ta);
            ASSERT(0 == ta.status());
        }
        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Testing creation of string reference");

            // We know all 'createStringRef' variants funnel through the same
            // implementation, the one taking an array and a value.  We'll
            // exercise it thoroughly, then touch the rest.

            // We know the 32-bit implementation has to move to externalized
            // storage at 2^16.

            const int sizes[] = {0, 1, 30, 65535, 65536, 100000};

            for (int i = 0; i < sizeof(sizes) / sizeof(sizes[0]); ++i) {
                const char *c = "";
                Datum d = Datum::createStringRef(c, sizes[i], &ta);
                LOOP_ASSERT(i, d.isString());
                bslstl::StringRef r = d.theString();
                LOOP3_ASSERT(i,
                             static_cast<const void *>(c),
                             static_cast<const void *>(r.data()),
                             c == r.data());
                LOOP2_ASSERT(i, r.length(), sizes[i] == r.length());
                Datum::destroy(d, &ta);
            }

            const char *c = "";
            const Datum d = Datum::createStringRef(c, 0, &ta);
            ASSERT(d == Datum::createStringRef(c, &ta));
            ASSERT(d ==
                      Datum::createStringRef(bslstl::StringRef(c), &ta));
        }
        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Testing creation of array reference");

            // We know the 32-bit implementation has to move to externalized
            // storage at 2^16.

            const int sizes[] = {0, 1, 30, 65535, 65536, 100000};

            for (int i = 0; i < sizeof(sizes) / sizeof(sizes[0]); ++i) {
                Datum start;
                Datum d = Datum::createArrayReference(&start, sizes[i], &ta);
                LOOP_ASSERT(i, d.isArray());
                ConstDatumArrayRef  r = d.theArray();
                LOOP3_ASSERT(i,
                             static_cast<const void *>(&start),
                             static_cast<const void *>(r.data()),
                             &start == r.data());
                LOOP2_ASSERT(i, r.length(), sizes[i] == r.length());
                Datum::destroy(d, &ta);
            }

            // Test that the version taking 'ConstDatumArrayRef' works.

            Datum start;
            const Datum d = Datum::createArrayReference(&d, 0, &ta);
            ASSERT(d ==
              Datum::createArrayReference(ConstDatumArrayRef(&start, 0), &ta));
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        TEST("Testing Datum::copyBinary().");


        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datums by copying a large binary value.");
            const unsigned char binary[] =
                "1234567890abcdefghijklmnopqrstuvwxyz";
            const int binarySize = sizeof(binary) - 1;

            const Datum obj1 = Datum::copyBinary(binary, binarySize, &ta);
            const Datum obj2 = Datum::copyBinary(binary, binarySize, &tmpa);

            SUBTEST("Check if objects have the same binary value.");
            ASSERT(obj1.isBinary());
            ASSERT(obj2.isBinary());
            ASSERT(obj1.theBinary() == ConstDatumBinaryRef(binary,
                                                           binarySize));
            ASSERT(obj2.theBinary() == ConstDatumBinaryRef(binary,
                                                           binarySize));

            SUBTEST("Verify that the string was copied.");
            ASSERT(0 != ta.status());
            ASSERT(0 != tmpa.status());

            SUBTEST("Destroy the objects"
                    " and verify there are no memory leaks.");
            Datum::destroy(obj1, &ta);
            Datum::destroy(obj2, &tmpa);
            ASSERT(0 == ta.status());
            ASSERT(0 == tmpa.status());
        }

#if defined(BSLS_PLATFORM_CPU_64_BIT)
        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create Datums by copying a small binary value.");
            const unsigned char binary[] =
                "1234567890abcdefghijklmnopqrstuvwxyz";
            const int binarySize = 13;

            const Datum obj1 = Datum::copyBinary(binary, binarySize, &ta);
            const Datum obj2 = Datum::copyBinary(binary, binarySize, &tmpa);

            SUBTEST("Check if objects have the same binary value.");
            ASSERT(obj1.isBinary());
            ASSERT(obj2.isBinary());
            ASSERT(obj1.theBinary() == ConstDatumBinaryRef(binary,
                                                           binarySize));
            ASSERT(obj2.theBinary() == ConstDatumBinaryRef(binary,
                                                           binarySize));

            SUBTEST("Verify that no memory was allocated.");
            ASSERT(0 == ta.status());
            ASSERT(0 == tmpa.status());

            SUBTEST("Destroy the objects and verify there"
                    " are no deallocation overruns.");
            Datum::destroy(obj1, &ta);
            Datum::destroy(obj2, &tmpa);
            ASSERT(0 == ta.status());
            ASSERT(0 == tmpa.status());
        }
#endif

        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create binary Datum objects via cloning.");

            const unsigned char binary[] =
                "1234567890abcdefghijklmnopqrstuvwxyz";
            const int binarySize = sizeof(binary) - 1;

            const Datum obj = Datum::copyBinary(binary, binarySize, &ta);
            const Datum cObj = obj.clone(&tmpa);

            SUBTEST("Check if objects have the same binary value.");
            ASSERT(cObj.theBinary() == ConstDatumBinaryRef(binary,
                                                           binarySize));

            SUBTEST("Verify that the binary was copied.");
            ASSERT(0 != tmpa.status());

            SUBTEST("Destroy the objects and verify there"
                    " are no deallocation overruns.");
            Datum::destroy(obj, &ta);
            Datum::destroy(cObj, &tmpa);
            ASSERT(0 == ta.status());
            ASSERT(0 == tmpa.status());
        }

#if defined(BSLS_PLATFORM_CPU_64_BIT)
        //  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            SUBTEST("Create small binary Datum objects via cloning.");

            const unsigned char binary[] =
                "1234567890abcdefghijklmnopqrstuvwxyz";

            const int binarySize = 12;
            const Datum obj = Datum::copyBinary(binary, binarySize, &ta);
            const Datum cObj = obj.clone(&tmpa);

            SUBTEST("Check if objects have the same binary value.");
            ASSERT(cObj.isBinary());
            ASSERT(cObj.theBinary() == ConstDatumBinaryRef(binary,
                                                           binarySize));

            SUBTEST("Verify that the binary was copied.");
            ASSERT(0 == ta.status());
            ASSERT(0 == tmpa.status());

            SUBTEST("Destroy the objects and verify there"
                     " are no deallocation overruns.");
            Datum::destroy(obj, &ta);
            Datum::destroy(cObj, &tmpa);
            ASSERT(0 == ta.status());
            ASSERT(0 == tmpa.status());
        }
#endif
        {
            SUBTEST("Compare ConstDatumBinaryRef objects.");
            typedef ConstDatumBinaryRef Ref;

            enum {
                eq = 1,      ne = 1 << 1, lt = 1 << 2,
                le = 1 << 3, gt = 1 << 4, ge = 1 << 5
            };


            ConstDatumBinaryRef a3("\0ab",  3);
            ConstDatumBinaryRef a4("\0abc", 4);
            ConstDatumBinaryRef b4("\0bbb", 4);

            struct Test {
                ConstDatumBinaryRef l, r;
                const int expected;
            } tests[] = {
                a4, a4, eq | le | ge,
                a3, a4, ne | lt | le,
                a4, b4, ne | lt | le,
                b4, a4, ne | gt | ge,
            };

            const int k_numTests = sizeof(tests) / sizeof(*tests);

            for (int t = 0; t < k_numTests; t++) {
                const ConstDatumBinaryRef& l = tests[t].l;
                const ConstDatumBinaryRef& r = tests[t].r;

                for (int i = 0; i < 6; ++i) {
                    bool result;
                    switch (1 << i) {
                      case eq: result = l == r; break; // 0
                      case ne: result = l != r; break; // 1
                      case lt: result = l <  r; break; // 2
                      case le: result = l <= r; break; // 3
                      case gt: result = l >  r; break; // 4
                      case ge: result = l >= r; break; // 5
                      default: ASSERT(false);
                    }

                    LOOP2_ASSERT(t, i, result ==
                                 ((tests[t].expected >> i) & 1));
                }
            }
        }
    } break;

    case 2: {
#ifdef BSLS_PLATFORM_CPU_32_BIT
        TESTCASE("TESTING DATUM_HELPERS32");

        short up16;
        int lo32;
        typedef bsls::Types::Int64 Int64;
        char subcase = 'a';

        TEST("Store a \"small\" value.");

        ASSERT(Datum_Helpers32::storeSmallInt64(Int64(0x123456789abcLL),
                                                &up16, &lo32));
        ASSERT(Datum_Helpers32::loadSmallInt64(up16,
                                               lo32) == 0x123456789abcLL);

        TEST("Store zero.");

        ASSERT(Datum_Helpers32::storeSmallInt64(Int64(0), &up16, &lo32));
        ASSERT(Datum_Helpers32::loadSmallInt64(up16, lo32) == 0LL);

        TEST("Store one.");

        ASSERT(Datum_Helpers32::storeSmallInt64(Int64(1), &up16, &lo32));
        ASSERT(Datum_Helpers32::loadSmallInt64(up16, lo32) == 1LL);

        TEST("Store minus one.");

        ASSERT(Datum_Helpers32::storeSmallInt64(-1LL, &up16, &lo32));
        ASSERT(Datum_Helpers32::loadSmallInt64(up16, lo32) == -1LL);

        TEST("Store a positive value twice too large.");

        const Int64 max48 = (1LL << 48) - 1;
        ASSERT(!Datum_Helpers32::storeSmallInt64(max48, &up16, &lo32));
        ASSERT(!Datum_Helpers32::storeSmallInt64(-max48, &up16, &lo32));

        TEST("Store the largest positive \"small\" value.");

        const Int64 max47 = (1LL << 47) - 1;
        ASSERT(Datum_Helpers32::storeSmallInt64(max47, &up16, &lo32));
        ASSERT(Datum_Helpers32::loadSmallInt64(up16, lo32) ==  max47);

        TEST("Store the largest negative \"small\" value.");

        const Int64 min47 = ~max47;
        ASSERT(Datum_Helpers32::storeSmallInt64(min47, &up16, &lo32));
        ASSERT(Datum_Helpers32::loadSmallInt64(up16, lo32) == min47);

        TEST("Store largest positive \"small\" value + 1.");

        const Int64 justTooBig = max47 + 1;
        ASSERT(!Datum_Helpers32::storeSmallInt64(justTooBig, &up16, &lo32));

        TEST("Store largest negative \"small\" value - 1.");

        const Int64 justTooSmall = min47 - 1;
        ASSERT(!Datum_Helpers32::storeSmallInt64(justTooSmall, &up16, &lo32));
#else
        TESTCASE("DATUM_HELPERS32 SKIPPED ON THIS PLATFORM");
#endif
    } break;

    case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Concerns:
        //    Exercise a broad cross-section of in-core value-semantic
        //    functionality before beginning testing in earnest.  Probe the
        //    functionality systematically and incrementally to discover
        //    basic errors in isolation.
        //
        // Plan:
        //    Create two test objects having an integer and a string value.
        //    We select integer and string because the former is stored inline
        //    the latter is stored as an externally allocated pointer.  Make
        //    a copy of each of these objects. Use the basic accessors to
        //    verify the expected results.  Finally destroy these objects and
        //    verify that no memory was leaked and no memory was allocated
        //    while copying the first type of object.  Display object values
        //    frequently in verbose mode.
        //
        // Testing:
        //     This test case exercises basic in-core value-semantic
        //     functionality.
        // --------------------------------------------------------------------

        TESTCASE("BREATHING TEST");

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Create Datum with an integer value.");
            const Datum obj = Datum::createInteger(0);

            SUBTEST("Check if object has the same integer value.");
            ASSERT(obj.isInteger());
            ASSERT(0 == obj.theInteger());

            SUBTEST("Create a copy of the object and verify"
                    " it is the same as the original.");
            const Datum cObj = obj.clone(&ta);
            ASSERT(cObj == obj);

            SUBTEST("Verify that no memory was allocated while copying.");
            ASSERT(0 == ta.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            TEST("Create Datum with a string value.");
            const char *str = "A long string abcdef";
            const Datum obj = Datum::copyString(str, &ta);

            SUBTEST("Check if object has the same string value.");
            ASSERT(obj.isString());
            ASSERT(obj.theString() == bslstl::StringRef(str, strlen(str)));

            SUBTEST("Create a copy of the object and verify"
                    " it is the same as the original.");
            const Datum cObj = obj.clone(&ta);
            ASSERT(cObj == obj);

            SUBTEST("Destroy both the objects and verify"
                    " there are no memory leaks.");
            Datum::destroy(obj, &ta);
            Datum::destroy(cObj, &ta);
            ASSERT(0 == ta.status());
        }

        // --------------------------------------------------------------------
        // Plan:
        //    Create a 'DatumArrayRef' object and verify using accessors that
        //    values were correctly passed down to the 'd_data_p' and
        //    'd_length_p' data members.  Also exercise the copy construction
        //    and assignment operator functionality and verify that these are
        //    the same values.
        //
        // Testing:
        //    DatumArrayRef();
        //    DatumArrayRef(Datum *, int *);
        //    DatumArrayRef(const DatumArrayRef&);
        //    DatumArrayRef& operator=(const DatumArrayRef&);
        //
        // ACCESSORS
        //    Datum *data() const;
        //    int *length() const;
        // --------------------------------------------------------------------

        TEST("Testing DatumArrayRef.");

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Create DatumArrayRef using default "
                    "constructor and verify that pointer values "
                    " are 0.");
            DatumArrayRef obj;
            ASSERT(0 == obj.data() && 0 == obj.length());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Create DatumArrayRef and verify that it"
                    " has the same value that was initially "
                    "assigned.");
            Datum                  data;
            bsls::Types::size_type length;
            const DatumArrayRef obj(&data, &length);

            ASSERT(&data   == obj.data());
            ASSERT(&length == obj.length());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Copy-construct a DatumArrayRef object "
                    "from another and verify that they have the "
                    "same value.");
            Datum                  data;
            bsls::Types::size_type length;
            const DatumArrayRef    obj1(&data, &length);
            const DatumArrayRef    obj2 = obj1;

            ASSERT(&data   == obj2.data());
            ASSERT(&length == obj2.length());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Assign a DatumArrayRef object to "
                    "another and verify that they have the same "
                    "value.");
            Datum                  data;
            bsls::Types::size_type length;
            const DatumArrayRef    obj1(&data, &length);
            DatumArrayRef          obj2;
            obj2 = obj1;

            ASSERT(&data   == obj2.data());
            ASSERT(&length == obj2.length());
        }

        // --------------------------------------------------------------------
        // Plan:
        //    Create a 'DatumMapOwningKeysRef' object and verify using
        //    accessors that values were correctly passed down to the
        //    'd_data_p', 'd_size_p', 'd_keys_p' and 'd_sorted_p' data members.
        //    Also exercise the copy construction and assignment operator
        //    functionality and verify that these are the same values.
        //
        // Testing:
        //    DatumMapOwningKeysRef();
        //    DatumMapOwningKeysRef(DatumMapEntry *,
        //                          bsls::Types::size_type *,
        //                          char *,
        //                          bool *);
        //    DatumMapOwningKeysRef(const DatumMapOwningKeysRef&);
        //    DatumMapOwningKeysRef& operator=(
        //                               const DatumMapOwningKeysRef&);
        //
        // ACCESSORS
        //    DatumMapEntry *data() const;
        //    bsls::Types::size_type *size() const;
        //    char *keys() const;
        //    bool *sorted() const;
        // --------------------------------------------------------------------

        TEST("Testing DatumMapOwningKeysRef.");

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Create DatumMapOwningKeysRef using "
                    "default constructor and verify that pointer"
                    " values are 0.");
            DatumMapOwningKeysRef obj;
            ASSERT(0 == obj.data());
            ASSERT(0 == obj.size());
            ASSERT(0 == obj.keys());
            ASSERT(0 == obj.sorted());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Create DatumMapOwningKeysRef and verify"
                    "that it has the same value that was "
                    "initially assigned.");
            double data;
            bsls::Types::size_type size;
            char keys;
            bool sorted;
            const DatumMapOwningKeysRef obj(
                reinterpret_cast<DatumMapEntry *>(&data),
                &size,
                &keys,
                &sorted);
            ASSERT(&data   == reinterpret_cast<double *>(obj.data()));
            ASSERT(&size   == obj.size());
            ASSERT(&keys   == obj.keys());
            ASSERT(&sorted == obj.sorted());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Copy-construct a DatumMapOwningKeysRef"
                    " object from  another and verify that they "
                    "have the same value.");
            double data;
            bsls::Types::size_type size;
            char keys;
            bool sorted;
            const DatumMapOwningKeysRef obj1(
                reinterpret_cast<DatumMapEntry *>(&data),
                &size,
                &keys,
                &sorted);
            const DatumMapOwningKeysRef obj2 = obj1;

            ASSERT(&data   == reinterpret_cast<double *>(obj1.data()));
            ASSERT(&size   == obj1.size());
            ASSERT(&keys   == obj1.keys());
            ASSERT(&sorted == obj1.sorted());

            ASSERT(&data   == reinterpret_cast<double *>(obj2.data()));
            ASSERT(&size   == obj2.size());
            ASSERT(&keys   == obj2.keys());
            ASSERT(&sorted == obj2.sorted());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Assign a DatumMapOwningKeysRef object "
                    "to another and verify that they have the "
                    "same value.");
            double data;
            bsls::Types::size_type size;
            char keys;
            bool sorted;
            const DatumMapOwningKeysRef obj1(
                reinterpret_cast<DatumMapEntry *>(&data),
                &size,
                &keys,
                &sorted);
            DatumMapOwningKeysRef obj2;
            obj2 = obj1;

            ASSERT(&data   == reinterpret_cast<double *>(obj1.data()));
            ASSERT(&size   == obj1.size());
            ASSERT(&keys   == obj1.keys());
            ASSERT(&sorted == obj1.sorted());

            ASSERT(&data   == reinterpret_cast<double *>(obj2.data()));
            ASSERT(&size   == obj2.size());
            ASSERT(&keys   == obj2.keys());
            ASSERT(&sorted == obj2.sorted());
        }
        // --------------------------------------------------------------------
        // Plan:
        //    Create a 'DatumMapRef' object and verify using accessors that
        //    values were correctly passed down to the 'd_data_p', 'd_size_p',
        //    and 'd_sorted_p' data members.  Also exercise the copy
        //    construction and assignment operator functionality and verify
        //    that these are the same values.
        //
        // Testing:
        //    DatumMapRef();
        //    DatumMapRef(DatumMapEntry *, bsls::Types::size_type *, bool *);
        //    DatumMapRef(const DatumMapRef&);
        //    DatumMapRef& operator=(const DatumMapRef&);
        //
        // ACCESSORS
        //    DatumMapEntry *data() const;
        //    bsls::Types::size_type *size() const;
        //    bool *sorted() const;
        // --------------------------------------------------------------------

        TEST("Testing DatumMapRef");

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Create DatumMapRef using default "
                    "constructor and verify that pointer values "
                    "are 0.");
            DatumMapRef obj;
            ASSERT(0 == obj.data());
            ASSERT(0 == obj.size());
            ASSERT(0 == obj.sorted());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Create DatumMapRef and verify that it "
                    "has the same value that was initially "
                    "assigned.");
            double data;
            bsls::Types::size_type size;
            bool sorted;
            const DatumMapRef obj(
                     reinterpret_cast<DatumMapEntry *>(&data), &size, &sorted);
            ASSERT(&data   == reinterpret_cast<double *>(obj.data()));
            ASSERT(&size   == obj.size());
            ASSERT(&sorted == obj.sorted());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Copy-construct a DatumMapRef object "
                    "from another and verify that they have the "
                    "same value.");
            double data;
            bsls::Types::size_type size;
            bool sorted;
            const DatumMapRef obj1(
                     reinterpret_cast<DatumMapEntry *>(&data), &size, &sorted);
            const DatumMapRef obj2 = obj1;

            ASSERT(&data   == reinterpret_cast<double *>(obj1.data()));
            ASSERT(&size   == obj1.size());
            ASSERT(&sorted == obj1.sorted());

            ASSERT(&data   == reinterpret_cast<double *>(obj2.data()));
            ASSERT(&size   == obj2.size());
            ASSERT(&sorted == obj2.sorted());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Assign a DatumMapRef object to another"
                    " and verify that they have the same value.");

            double data;
            bsls::Types::size_type size;
            bool sorted;
            const DatumMapRef obj1(
                     reinterpret_cast<DatumMapEntry *>(&data), &size, &sorted);
            DatumMapRef obj2;
            obj2 = obj1;

            ASSERT(&data   == reinterpret_cast<double *>(obj1.data()));
            ASSERT(&size   == obj1.size());
            ASSERT(&sorted == obj1.sorted());

            ASSERT(&data   == reinterpret_cast<double *>(obj2.data()));
            ASSERT(&size   == obj2.size());
            ASSERT(&sorted == obj2.sorted());
        }

        // --------------------------------------------------------------------
        // Plan:
        //    Create an 'Error' value using the default constructor and verify
        //    that it has a value of '0'.  Create another 'Error' value with an
        //    error code value and verify that the value was correctly passed
        //    down to the 'd_code' data member.  Create one more 'Error' value
        //    with an error code value and an error message value and verify
        //    that the values were correctly passed down to the 'd_code' and
        //    'd_message' data members.  Also exercise the copy construction
        //    and assignment operator functionality and verify using the
        //    equality operators that these are the same values.  Verify all
        //    comparison operators work as expected.  Verify that streaming
        //    operator outputs the correctly formatted value.
        //
        // Testing:
        //    Error();
        //    Error(int);
        //    Error(int, const bslstl::StringRef&);
        //    Error(const Error&);
        //    Error& operator=(const Error&);
        //    int code() const;
        //    bslstl::StringRef message() const;
        //    bool operator==(const Error&, const Error&);
        //    bool operator!=(const Error&, const Error&);
        //    bool operator<(const Error&, const Error&);
        //    bool operator<=(const Error&, const Error&);
        //    bool operator>(const Error&, const Error&);
        //    bool operator>=(const Error&, const Error&);
        //    bsl::ostream& operator<<(bsl::ostream&, const Error&);
        // --------------------------------------------------------------------

        TEST("Testing Error.");

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Create Error with the default "
                    "constructor and verify that it has an error "
                    "code value of 0 and an error message value "
                    "of empty string.");

            const Error obj;
            ASSERT(0 == obj.code());
            ASSERT("" == obj.message());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Create Error with the explicit "
                    "constructor and verify that it has the same "
                    "error code value that it was initialized "
                    "with.");
            const Error obj(5);
            ASSERT(5 == obj.code());
            ASSERT("" == obj.message());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Create Error with a code and message "
                    "values and verify that it has the same "
                    "values that it was initialized with.");
            const Error obj(5, "custom error");
            ASSERT(5 == obj.code());
            ASSERT("custom error" == obj.message());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Copy-construct an Error value from "
                    "another and verify that they have the same "
                    "value.");
            const Error obj1(2, "custom error");
            const Error obj2 = obj1;
            ASSERT(2 == obj1.code());
            ASSERT(2 == obj2.code());
            ASSERT("custom error" == obj1.message());
            ASSERT("custom error" == obj2.message());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Assign an Error value to another and "
                    "verify that they have the same value.");
            const Error obj1(3, "custom error");
            Error obj2;
            obj2 = obj1;
            ASSERT(3 == obj1.code());
            ASSERT(3 == obj2.code());
            ASSERT("custom error" == obj1.message());
            ASSERT("custom error" == obj2.message());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Create two Error objects with the same "
                    "value and verify that they are equal.  "
                    "Create another Error object with a different"
                    " value and verify that it is not equal to "
                    "the previously created Error object.  Also "
                    "test the case when the codes are equal but "
                    " messages are same and vice versa.");
            const Error obj1(4);
            const Error obj2(4);
            const Error obj3(6);
            ASSERT(obj1 == obj2);
            ASSERT(obj3 != obj2);

            const Error obj4(4, "e");
            const Error obj5(4, "e");
            const Error obj6(6, "e");
            const Error obj7(4, "f");
            ASSERT(obj4 == obj5);
            ASSERT(obj4 != obj6);
            ASSERT(obj4 != obj7);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Verify that streaming operator outputs "
                    "the correctly formatted error code value.");
            {
                const Error obj;
                ostringstream out;
                out << obj;
                if ("error(0)" != out.str()) {
                    cout << "<<<" << out.str() << ">>> <<<error(0)>>>\n";
                }
                ASSERT("error(0)" == out.str());
            }
            {
                const Error obj(5);
                ostringstream out;
                out << obj;
                if ("error(5)" != out.str()) {
                    cout << "<<<" << out.str() << ">>> <<<error(5)>>>\n";
                }
                ASSERT("error(5)" == out.str());
            }
            {
                const Error obj(5, "generic error");
                ostringstream out;
                out << obj;
                if ("error(5)" != out.str()) {
                    cout << "<<<" << out.str() << ">>> <<<error(5,'generic error')>>>\n";
                }
                ASSERT("error(5,'generic error')" == out.str());
            }
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Create two Error objects with same "
                    "error code value and create another Error "
                    "object with less error code value and verify"
                    " that the first object is greater than or "
                    "equal to both the objects. Also verify that "
                    "the third object is less than the first "
                    "object and that the reverse is not true.  "
                    "Repeat the same tests with one attribute "
                    "same and the other different.");
            const Error obj1(7);
            const Error obj2(7);
            const Error obj3(5);
            ASSERT(obj1 >= obj2);
            ASSERT(obj1 >= obj3);
            ASSERT(obj3 < obj1);
            ASSERT(!(obj1 < obj3));

            const Error obj4(6, "f");
            const Error obj5(6, "f");
            const Error obj6(4, "f");
            const Error obj7(6, "e");
            ASSERT(obj4 >= obj5);
            ASSERT(obj4 >= obj6);
            ASSERT(obj4 >= obj7);
            ASSERT(obj6 < obj4);
            ASSERT(obj7 < obj4);
            ASSERT(!(obj4 < obj6));
            ASSERT(!(obj4 < obj7));
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Create two Error objects with same "
                    "error code value and create another Error "
                    "object with greater error code value and "
                    "verify that the first object is less than or"
                    " equal to both the objects. Also verify that"
                    " the third object is greater than the first "
                    "object and that the reverse is not true.  "
                    "Repeat the same tests with one attribute "
                    "same and the other different.");
            const Error obj1(5);
            const Error obj2(5);
            const Error obj3(7);
            ASSERT(obj1 <= obj2);
            ASSERT(obj1 <= obj3);
            ASSERT(obj3 > obj1);
            ASSERT(!(obj1 > obj3));

            const Error obj4(4, "e");
            const Error obj5(4, "e");
            const Error obj6(6, "e");
            const Error obj7(4, "f");
            ASSERT(obj4 <= obj5);
            ASSERT(obj4 <= obj6);
            ASSERT(obj4 <= obj7);
            ASSERT(obj6 > obj4);
            ASSERT(obj7 > obj4);
            ASSERT(!(obj4 > obj6));
            ASSERT(!(obj4 > obj7));
        }
        // -------------------------------------------------------------------
        // Plan:
        //    Create a 'Udt' object and verify that values were correctly
        //    passed down to the 'd_data_p' and 'd_type' data members.  Also
        //    exercise the copy construction and assignment operator
        //    functionality and verify using the equality operator that these
        //    objects have the same value.  Verify that all comparison
        //    operators work as expected.  Verify that streaming operator
        //    outputs the correctly formatted value.
        //
        // Testing:
        //    Udt(const void *, int);
        //    Udt(cons Udt&);
        //    Udt& operator=(const Udt&);
        //    const void *data() const;
        //    int type() const;
        //    bool operator==(const Udt&, const Udt&);
        //    bool operator!=(const Udt&, const Udt&);
        //    bool operator <(const Udt&, const Udt&);
        //    bool operator <=(const Udt&, const Udt&);
        //    bool operator >(const Udt&, const Udt&);
        //    bool operator >=(const Udt&, const Udt&);
        //    bsl::ostream& operator<<(bsl::ostream&, const Udt&);
        // -------------------------------------------------------------------

        TEST("Testing Udt.");

        class TestUdt {
            // Test user data class.
        };

        TestUdt udt;
        const int TYPE = 2;

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Create a Udt object with the explicit "
                    "constructor and verify that it has the same"
                    " object that was initially assigned.");
            const Udt obj(&udt, TYPE);
            ASSERT(obj.data() == &udt);
            ASSERT(obj.type() == TYPE);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Copy-construct a Udt object from "
                                 "another and verify that they have the same "
                                 "value.");
            const Udt obj1(&udt, TYPE);
            const Udt obj2(obj1);
            ASSERT(obj1.data() == &udt && obj2.data() == &udt);
            ASSERT(obj1.type() == TYPE && obj2.type() == TYPE);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Assign a Udt object to another and "
                    "verify that they have the same value.");
            const Udt obj1(&udt, TYPE);
            Udt obj2(0, 0);
            obj2 = obj1;
            ASSERT(obj1.data() == &udt && obj2.data() == &udt);
            ASSERT(obj1.type() == TYPE && obj2.type() == TYPE);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Create two Udt objects with the same "
                    "value and verify that they are equal. "
                    "Create another Udt object with a different "
                    "value and verify that it is not equal to "
                    "the previously created Udt object.");
            const Udt obj1(&udt, TYPE);
            const Udt obj2(&udt, TYPE);
            ASSERT(obj1 == obj2);
            const Udt obj3(0, 0);
            ASSERT(obj1 != obj3);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Create few Udt objects and verify all "
                    "comparison operators work as expected.");
            const Udt obj1(reinterpret_cast<void *>(0xDEADBEEE), 1);
            const Udt obj2(reinterpret_cast<void *>(0xDEADBEEF), 1);
            const Udt obj3(reinterpret_cast<void *>(0xDEADBEEE), 2);

            ASSERT(obj1 <  obj2);
            ASSERT(obj1 <= obj2);
            ASSERT(obj1 <= obj1);
            ASSERT(obj1 <  obj3);
            ASSERT(obj1 <= obj3);
            ASSERT(obj2 >  obj1);
            ASSERT(obj2 >= obj1);
            ASSERT(obj2 >= obj2);
            ASSERT(obj3 >  obj1);
            ASSERT(obj3 >= obj1);
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            SUBTEST("Verify that streaming operator "
                    "outputs the correctly formatted value.");
            const Udt obj(&udt, TYPE);
            bsl::ostringstream out1;
            out1 << obj;
            bsl::ostringstream out2;
            out2 << "user-defined(" << &udt << "," << TYPE << ")";
            ASSERT(out1.str() == out2.str());
        }
    } break;

    case -2:
#if defined(BSLS_PLATFORM_OS_AIX)
        test = -100 * 1000;
#elif defined(BSLS_PLATFORM_OS_LINUX)
        test = -100 * 1000;
#elif defined(BSLS_PLATFORM_OS_SOLARIS)
        test = -10 * 1000;
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
        test = -1000 * 1000;
#else
        test = -100 * 1000;
#endif
    default:

        if (test < -2) {
            BenchmarkSuite benchmark;
            benchmark.run(-test, argc - 2, argv + 2);
            break;
        }

        cerr << "WARNING: CASE '" << test << "' NOT FOUND." << endl;
        testStatus = -1;
        break;
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
