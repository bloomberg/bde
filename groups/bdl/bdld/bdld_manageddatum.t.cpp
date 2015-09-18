// bdld_manageddatum.t.cpp                                            -*-C++-*-
#include <bdld_manageddatum.h>

#include <bdlt_datetime.h>

#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_sstream.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bsls_asserttest.h>

using namespace BloombergLP;
using namespace bsl;
using namespace bslstl;
using namespace BloombergLP::bdld;

// ============================================================================
//                                TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test is an full *value-semantic* class that represents
// general purpose values and their datums. Our testing concerns are
// limited to creating 'ManagedDatum' objects with different types of
// values, retrieving the type and actual value within these objects, copying
// these objects, assigning to these objects, streaming these objects and
// finally destroying them. At the end of these operations, the same amount of
// memory should be deallocated as was allocated while performing these
// operations.
//
// Primary Manipulators:
//: o void adopt(const Datum&)
//: o void copy(const Datum&)
//: o void makeNull();
//: o Datum release();
//
// Basic Accessors:
//: o Datum datum() const;
//
// Global Concerns:
//: o No memory is leaked.
//
// Global Assumptions:
//: o ACCESSOR methods are 'const' thread-safe.
//
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] ManagedDatum(bslma::Allocator *);
// [ 2] ManagedDatum(const Datum&, bslma::Allocator *);
// [ 7] ManagedDatum(const ManagedDatum&, bslma::Allocator *);
// [ 2] ~ManagedDatum();
//
// MANIPULATORS
// [ 9] ManagedDatum& operator=(const ManagedDatum&);
// [ 3] void adopt(const Datum&);
// [ 3] void copy(const Datum&);
// [ 3] Datum release();
// [ 3] void makeNull();
// [ 8] void swap(ManagedDatum&);
//
// ACCESSORS
// [ 2] Datum datum() const;
// [ 2] const Datum *operator->() const;
// [ 5] const Datum *operator->() const;
//
// FREE OPERATORS
// [ 6] bool operator==(const ManagedDatum&, const ManagedDatum&);
// [ 6] bool operator!=(const ManagedDatum&, const ManagedDatum&);
// [11] bsl::ostream& operator<<(bsl::ostream&, const ManagedDatum&);
// ---------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [12] USAGE EXAMPLE

// ============================================================================
//                        STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && 100 >= testStatus) {
            ++testStatus;
        }
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
// ----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                 \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       aSsErT(1, #X, __LINE__); } }

#define LOOP0_ASSERT ASSERT
#define LOOP1_ASSERT LOOP_ASSERT

// ============================================================================
//                   STANDARD BDE VARIADIC ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define NUM_ARGS_IMPL(X5, X4, X3, X2, X1, X0, N, ...)   N
#define NUM_ARGS(...) NUM_ARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1, 0, "")

#define LOOPN_ASSERT_IMPL(N, ...) LOOP ## N ## _ASSERT(__VA_ARGS__)
#define LOOPN_ASSERT(N, ...)      LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define ASSERTV(...) LOOPN_ASSERT(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

// ============================================================================
//                       SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------


#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// ============================================================================
//                      NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

// ============================================================================
//                    GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

void assertHandler(const char *text, const char *file, int line)
{
    bsl::cout << "Error " << file << "(" << line << "): " << text
              << "    (assert)" << bsl::endl;

    int *i = 0;
    // cppcheck-suppress nullPointer
    *i = 0;
}

typedef ManagedDatum Obj;
typedef bsls::Types::Int64 Int64;

const double k_DBL_MIN2    = numeric_limits<double>::min();
const double k_DBL_MAX2    = numeric_limits<double>::max();
const double k_DBL_INF     = numeric_limits<double>::infinity();
const double k_DBL_QNAN2   = numeric_limits<double>::quiet_NaN();
const double k_DBL_SNAN    = numeric_limits<double>::signaling_NaN();
const double k_DBL_NEGINF  = -1 * k_DBL_INF;
const double k_DBL_ZERO    = 0.0;
const double k_DBL_NEGZERO = 1 / k_DBL_NEGINF;

// ============================================================================
//                              TEST APPARATUS
// ----------------------------------------------------------------------------

#define   PV(X)         if (verbose) cout << endl << X << endl;
#define  PVV(X)     if (veryVerbose) cout << endl << X << endl;
#define PVVV(X) if (veryVeryVerbose) cout << endl << X << endl;

class TestVisitor {
    // This class provides a visitor to visit and store the type of
    // 'ManagedDatum' object with which it was called.

private:
    // INSTANCE DATA
    Datum::DataType d_type;  // type of the 'ManagedDatum' object

public:
    // CREATORS
    TestVisitor();
        // Create a 'TestVisitor' object.

    // MANIPULATORS
    void operator()(bslmf::Nil v);
        // Store 'Datum::e_NIL' in 'd_type'.

    void operator()(const bdlt::Date& v);
        // Store 'Datum::e_DATE' in 'd_type'.

    void operator()(const bdlt::Datetime& v);
        // Store 'Datum::e_DATETIME' in 'd_type'.

    void operator()(const bdlt::DatetimeInterval& v);
        // Store 'Datum::e_DATETIME_INTERVAL' in 'd_type'.

    void operator()(const bdlt::Time& v);
        // Store 'Datum::e_TIME' in 'd_type'.

    void operator()(bslstl::StringRef v);
        // Store 'Datum::e_STRING' in 'd_type'.

    void operator()(bool v);
        // Store 'Datum::e_BOOLEAN' in 'd_type'.

    void operator()(bsls::Types::Int64 v);
        // Store 'Datum::e_INTEGER64' in 'd_type'.

    void operator()(double v);
        // Store 'Datum::e_REAL' in 'd_type'.

    void operator()(Error v);
        // Store 'Datum::e_ERROR_VALUE' in 'd_type'.

    void operator()(int v);
        // Store 'Datum::e_INTEGER' in 'd_type'.

    void operator()(Udt v);
        // Store 'Datum::e_USERDEFINED' in 'd_type'.

    void operator()(ConstDatumArrayRef v);
        // Store 'Datum::e_ARRAY' in 'd_type'.

    void operator()(ConstDatumMapRef v);
        // Store 'Datum::e_MAP' in 'd_type'.

    // ACCESSORS
    Datum::DataType type() const;
        // Return the type of 'ManagedDatum' object with which this visitor was
        // called.
};

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

Datum::DataType TestVisitor::type() const
{
    return d_type;
}

bool operator==(bslmf::Nil lhs, bslmf::Nil rhs)
{
    return true;
}

Datum createArray(bslma::Allocator *alloc)
{
    DatumArrayRef array;
    const bsls::Types::size_type arraySize = 6;
    Datum::createUninitializedArray(&array,
                                    arraySize,
                                    alloc);
    array.data()[0] = Datum::createInteger(0);
    array.data()[1] = Datum::createDouble(-3.1416);
    array.data()[2] = Datum::copyString("A long string", alloc);
    array.data()[3] = Datum::copyString("Abc", alloc);
    array.data()[4] = Datum::createDate(bdlt::Date(2010, 1, 5));
    array.data()[5] = Datum::createDatetime(
                            bdlt::Datetime(2010, 1, 5, 16, 45, 32, 12),
                            alloc);
    *(array.length()) = arraySize;
    return Datum::adoptArray(array);
}

Datum createEmptyArray()
{
    DatumArrayRef array;
    return Datum::adoptArray(array);
}

Datum createEmptyMap()
{
    DatumMapRef map;
    return Datum::adoptMap(map);
}

Datum createMap(bslma::Allocator *alloc)
{
    DatumMapRef map;
    const bsls::Types::size_type mapSize = 6;
    Datum::createUninitializedMap(&map,
                                  mapSize,
                                  alloc);
    map.data()[0] = DatumMapEntry(StringRef("first", strlen("first")),
                                  Datum::createInteger(0));
    map.data()[1] = DatumMapEntry(StringRef("second", strlen("second")),
                                  Datum::createDouble(-3.1416));
    map.data()[2] = DatumMapEntry(StringRef("third", strlen("third")),
                                  Datum::copyString("A long string",
                                  alloc));
    map.data()[3] = DatumMapEntry(StringRef("fourth", strlen("fourth")),
                                  Datum::copyString("Abc",
                                                    alloc));
    map.data()[4] = DatumMapEntry(StringRef("fifth", strlen("fifth")),
                                  Datum::createDate(bdlt::Date(2010, 1, 5)));
    map.data()[5] = DatumMapEntry(StringRef("sixth", strlen("sixth")),
                                  Datum::createDatetime(
                                    bdlt::Datetime(2010, 1, 5, 16, 45, 32, 12),
                                    alloc));
    *(map.size()) = mapSize;
    return Datum::adoptMap(map);
}

// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator ta("test", veryVeryVeryVerbose);

    bsls::Assert::setFailureHandler(assertHandler);

    bdlt::Date udt;
    const int UDT_TYPE = 2;
    bdlt::Date udt1;

    typedef bsls::Types::size_type size_type;

    const size_type arraySize  = 6;
    bslma::TestAllocator arrayAlloc(false);
    Datum array[arraySize] = {
        Datum::createInteger(0),
        Datum::createDouble(-3.1416),
        Datum::copyString("A long string", &arrayAlloc),
        Datum::copyString("Abc", &arrayAlloc),
        Datum::createDate(bdlt::Date(2010, 1, 5)),
        Datum::createDatetime(bdlt::Datetime(2010, 1, 5, 16, 45, 32, 12),
                                  &arrayAlloc),
    };

    const size_type mapSize  = 6;
    bslma::TestAllocator mapAlloc(false);
    DatumMapEntry map[mapSize] = {
        DatumMapEntry(StringRef("first", strlen("first")),
                      Datum::createInteger(0)),
        DatumMapEntry(StringRef("second", strlen("second")),
                      Datum::createDouble(-3.1416)),
        DatumMapEntry(StringRef("third", strlen("third")),
                      Datum::copyString("A long string", &mapAlloc)),
        DatumMapEntry(StringRef("fourth", strlen("fourth")),
                      Datum::copyString("Abc", &mapAlloc)),
        DatumMapEntry(StringRef("fifth", strlen("fifth")),
                      Datum::createDate(bdlt::Date(2010, 1, 5))),
        DatumMapEntry(StringRef("sixth", strlen("sixth")),
                      Datum::createDatetime(
                                     bdlt::Datetime(2010, 1, 5, 16, 45, 32, 12),
                                     &mapAlloc))
    };

    switch (test) { case 0:  // Zero is always the leading case.
      case 11: {
        //---------------------------------------------------------------------
        // USAGE EXAMPLE:
        //
        // Concerns:
        //    The usage example provided in the component header file must
        //    compile, link, and run on all platforms.
        //
        // Plan:
        //    Incorporate usage example from header into the test driver,
        //    remove leading comment characters, and replace 'assert' with
        //    'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

        bslma::Allocator *allocator = bslma::Default::defaultAllocator();
        const ManagedDatum realObj(Datum::createDouble(-3.457),
                                       allocator);
        ASSERT(realObj->isDouble());
        ASSERT(-3.457 == realObj->theDouble());

        const char *str = "This is a string";
        const ManagedDatum strObj(Datum::copyString(str, &ta), &ta);
        ASSERT(strObj->isString());
        ASSERT(str == strObj->theString());
        const ManagedDatum strObj1 = strObj;
        ASSERT(strObj == strObj1);
        const ManagedDatum strObj2(strObj, &ta);
        ASSERT(strObj == strObj2);

        ManagedDatum udtObj(Datum::createUdt(&udt, UDT_TYPE),
                                allocator);
        ASSERT(udtObj->isUdt());
        ASSERT(&udt == udtObj->theUdt().data());
        ASSERT(UDT_TYPE == udtObj->theUdt().type());
        udtObj.adopt(Datum::createBoolean(true));
        ASSERT(udtObj->isBoolean());
        ASSERT(true == udtObj->theBoolean());

        const Datum datumArray[2] = {
            Datum::createInteger(12),
            Datum::copyString("A long string", &ta)
        };
        DatumArrayRef arr;
        Datum::createUninitializedArray(&arr, 2 , &ta);
        for (int i = 0; i < 2; ++i) {
            arr.data()[i] = datumArray[i];
        }
        *(arr.length()) = 2;
        const ManagedDatum arrayObj(Datum::adoptArray(arr),
                                    &ta);
        ASSERT(arrayObj->isArray());
        ASSERT(ConstDatumArrayRef(datumArray, 2) == arrayObj->theArray());

        const DatumMapEntry datumMap[2] = {
            DatumMapEntry(StringRef("first", strlen("first")),
                          Datum::createInteger(12)),
            DatumMapEntry(StringRef("second", strlen("second")),
                          Datum::copyString("A long string",
                                            &ta))
        };
        DatumMapRef mp;
        Datum::createUninitializedMap(&mp, 2 , &ta);
        for (int i = 0; i < 2; ++i) {
            mp.data()[i] = datumMap[i];
        }
        *(mp.size()) = 2;
        const ManagedDatum mapObj(Datum::adoptMap(mp), &ta);
        ASSERT(mapObj->isMap());
        ASSERT(ConstDatumMapRef(datumMap, 2, false) == mapObj->theMap());

        const Datum rcObj = Datum::copyString("This is a string", &ta);
        ManagedDatum obj(Datum::createInteger(1), &ta);
        obj.adopt(rcObj);
        ASSERT(obj.datum() == rcObj);
        const Datum internalObj = obj.release();
        ASSERT(obj->isNull());
        ASSERT(internalObj == rcObj);
        Datum::destroy(internalObj, obj.allocator());
      } break;
      case 10: {
        //---------------------------------------------------------------------
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
        //   bsl::ostream& operator<<(bsl::ostream&, const ManagedDatum&);
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING STREAMING OPERATOR" << endl
                                  << "==========================" << endl;

        ostringstream infString;
        infString << k_DBL_INF;

        ostringstream nanString;
        nanString << k_DBL_QNAN2;

        ostringstream udtString;
        udtString << "user-defined(" << &udt << "," << UDT_TYPE << ")";

        ostringstream arrayString;
        arrayString << "[ 0 -3.1416 \"A long string\" \"Abc\" 05JAN2010";
        arrayString << " 05JAN2010_16:45:32.012 ]";

        ostringstream mapString;
        mapString << "[";
        mapString << " [ first = 0 ] [ second = -3.1416 ]";
        mapString << " [ third = \"A long string\" ]";
        mapString << " [ fourth = \"Abc\" ] [ fifth = 05JAN2010 ]";
        mapString << " [ sixth = 05JAN2010_16:45:32.012 ]";
        mapString << " ]";

        struct StreamHelper {
            Obj    d_obj;
            string d_streamStr;
            string d_message;
        };

        const StreamHelper datumArray[] = {
            {
                Obj(),
                "[nil]",
                "null"
            },
            {
                Obj(Datum::createInteger(3423), &ta),
                "3423",
                "integer"
            },
            {
                Obj(Datum::createBoolean(true), &ta),
                "true",
                "boolean"
            },
            {
                Obj(Datum::createError(5), &ta),
                "error(5)",
                "Error"
            },
            {
                Obj(Datum::createError(5, "some error", &ta), &ta),
                    "error(5,'some error')",
                    "Error"
            },
            {
                Obj(Datum::copyString("This is a string", &ta), &ta),
                "\"This is a string\"",
                "string"
            },
            {
                Obj(Datum::createDate(bdlt::Date(2010, 1, 5)), &ta),
                "05JAN2010",
                "Date"
            },
            {
                Obj(Datum::createTime(bdlt::Time(16, 45, 32, 12)), &ta),
                "16:45:32.012",
                "Time"
            },
            {
                Obj(Datum::createDatetime(
                         bdlt::Datetime(2010, 1, 5, 16, 45, 32, 12), &ta), &ta),
                "05JAN2010_16:45:32.012",
                "DateTime"
            },
            {
                Obj(Datum::createDatetimeInterval(
                         bdlt::DatetimeInterval(34, 16, 45, 32, 12), &ta), &ta),
                "+34_16:45:32.012",
                "DatetimeInterval"
            },
            {
                Obj(Datum::createInteger64(9223372036854775807LL, &ta), &ta),
                "9223372036854775807",
                "Int64"
            },
            {
                Obj(Datum::createDouble(-3.1416), &ta),
                "-3.1416",
                "double"
            },
            {
                Obj(Datum::createDouble(k_DBL_INF), &ta),
                infString.str(),
                "infinity double"
            },
            {
                Obj(Datum::createDouble(k_DBL_QNAN2), &ta),
                nanString.str(),
                "NaN double"
            },
            {
                Obj(Datum::createUdt(&udt, UDT_TYPE), &ta),
                udtString.str(),
                "User defined"
            },
            {
                Obj(createArray(&ta), &ta),
                arrayString.str(),
                "array"
            },
            {
                Obj(createEmptyArray(), &ta),
                "[ ]",
                "array"
            },
            {
                Obj(createMap(&ta), &ta),
                mapString.str(),
                "map"
            },
            {
                Obj(createEmptyMap(), &ta),
                "[ ]",
                "map"
            }
        };
        const int SIZE = sizeof(datumArray) / sizeof(StreamHelper);

        for (int i = 0; i < SIZE; ++i) {
            PVV((i + 1) << ". Streaming 'ManagedDatum' with '"
                        << datumArray[i].d_message
                        << "' value.")
            ostringstream out;
            out << datumArray[i].d_obj;
            LOOP3_ASSERT(i, out.str(), datumArray[i].d_streamStr,
                         datumArray[i].d_streamStr == out.str());
        }

      } break;
      case 9: {
        //---------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        //
        // Concerns:
        //    The assignment operator should destroy the existing
        //    'Datum' object held onto by this 'Datum' and release
        //    any dynamically allocated memory held inside the 'Datum'
        //    object. A new 'Datum' object should be created out of the
        //    passed in object. Dynamic memory should be only allocated when
        //    needed. The currently stored allocator should be used to allocate
        //    any memory. Copy-assigning the same 'Datum' object should be
        //    a no-op. The object that is assigned, should not be modified.
        //
        // Plan:
        //    Create 'Datum' object with a sufficiently long string value,
        //    so as to trigger a dynamic memory allocation. Assign const
        //    'Datum' objects with each of the different types of values.
        //    Verify that previously held 'Datum' object is destroyed
        //    and its memory is released. Verify that both both objects have
        //    the same value. Verify that no memory was allocated when the
        //    copied object did not have any dynamically allocated memory.
        //    Verify that self-assignment is a no-op. Finally destroy these
        //    objects and verify that no memory was leaked. Display steps of
        //    operation frequently in verbose mode.
        //
        // Testing:
        //   ManagedDatum& operator=(const ManagedDatum&);
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "ASSIGNMENT OPERATOR" << endl
                                  << "===================" << endl;

        // Allocator for object that is to be copy-assigned.
        bslma::TestAllocator assignAlloc(false);
        bslma::Allocator *aa(&assignAlloc);

        enum DynamicMemory {
            DYNAMIC_MEMORY__NO    = 0
          , DYNAMIC_MEMORY__YES   = 1
          , DYNAMIC_MEMORY__MAYBE = 2
        };

        struct AssignHelper {
            Datum         d_obj;
            string        d_message;
            DynamicMemory d_dynamicMemory;
        };

        const AssignHelper datumArray[] = {
            {
                Datum::createNull(),
                "null",
                DYNAMIC_MEMORY__NO
            },
            {
                Datum::createInteger(1),
                "integer",
                DYNAMIC_MEMORY__NO
            },
            {
                Datum::createBoolean(true),
                "boolean",
                DYNAMIC_MEMORY__NO
            },
            {
                Datum::createError(5),
                "Error",
                DYNAMIC_MEMORY__NO
            },
            {
                Datum::createDate(bdlt::Date(2010, 1, 5)),
                "Date",
                DYNAMIC_MEMORY__NO
            },
            {
                Datum::createTime(bdlt::Time(16, 45, 32, 12)),
                "Time",
                DYNAMIC_MEMORY__NO
            },
            {
                Datum::copyString("Abc", aa),
                "short string",
                DYNAMIC_MEMORY__NO
            },
            {
                Datum::copyString("Another long string", aa),
                "long string",
                DYNAMIC_MEMORY__YES
            },
            {
                Datum::createDatetime(
                               bdlt::Datetime(2010, 1, 5, 16, 45, 32, 12), aa),
                "DateTime",
                DYNAMIC_MEMORY__MAYBE
            },
            {
                Datum::createDatetimeInterval(
                               bdlt::DatetimeInterval(34, 16, 45, 32, 12), aa),
                "DatetimeInterval",
                DYNAMIC_MEMORY__MAYBE
            },
            {
                Datum::createInteger64(9223372036854775807LL, aa),
                "Int64",
                DYNAMIC_MEMORY__MAYBE
            },
            {
                Datum::createUdt(&udt, UDT_TYPE),
                "user defined",
                DYNAMIC_MEMORY__NO
            },
            {
                Datum::createDouble(-3.14),
                "double",
                DYNAMIC_MEMORY__NO
            },
            {
                createArray(aa),
                "array",
                DYNAMIC_MEMORY__YES
            },
            {
                createEmptyArray(),
                "array",
                DYNAMIC_MEMORY__NO
            },
            {
                createMap(aa),
                "map",
                DYNAMIC_MEMORY__YES
            },
            {
                createEmptyMap(),
                "map",
                DYNAMIC_MEMORY__NO
            }
        };
        const int SIZE  = sizeof(datumArray) / sizeof(AssignHelper);

        for (int i = 0; i < SIZE; ++i) {

            bool hasDynamicMemory;
            switch (datumArray[i].d_dynamicMemory) {
              case DYNAMIC_MEMORY__NO: {
                hasDynamicMemory = false;

              } break;
              case DYNAMIC_MEMORY__YES: {
                hasDynamicMemory = true;

              } break;
              case DYNAMIC_MEMORY__MAYBE: {
                // We trust the Datum::clone() does the right thing, because
                // it must have been tested in the bdld_datum.t.  Therefore we
                // can use the Datum::clone to detect if dynamic memory was
                // allocated.
                int na = assignAlloc.numAllocations();
                bdld::Datum temp = datumArray[i].d_obj.clone(&assignAlloc);
                hasDynamicMemory = na != assignAlloc.numAllocations();
                bdld::Datum::destroy(temp, &assignAlloc);

              } break;
              default:
                ASSERT(!"unknown value");
            }

            char idx = 'a';
            PVV((i + 1) << ". Testing assignment with "
                        << datumArray[i].d_message
                        << " type ManagedDatum.");

            PVVV('\t' << idx++
                      << ". Create ManagedDatum with a string value.");
            ManagedDatum obj(Datum::copyString("A long string constant", &ta),
                             &ta);

            PVVV('\t' << idx++
                      << ". Assign a ManagedDatum with "
                      << datumArray[i].d_message
                      << " value and verify that the previous object "
                      << "is destroyed.");
            const ManagedDatum aObj(datumArray[i].d_obj, aa);

            Int64 numCurrAllocs = ta.numAllocations();
            const Int64 numCurrDeallocs = ta.numDeallocations();
            const Int64 numAssignAllocs = assignAlloc.numAllocations();

            obj = aObj;
            LOOP_ASSERT(i, ta.numDeallocations() > numCurrDeallocs);
            LOOP_ASSERT(i, assignAlloc.numAllocations() == numAssignAllocs);

            if (hasDynamicMemory) {
                LOOP_ASSERT(i, ta.numAllocations() > numCurrAllocs);
            }
            else {
                LOOP_ASSERT(i, ta.numAllocations() == numCurrAllocs);
            }

            PVVV('\t' << idx++
                      << ". Verify that both objects have the same value.");
            LOOP_ASSERT(i, obj == aObj);

            PVVV('\t' << idx++ << ". Test for self-assignment.");
            numCurrAllocs = ta.numAllocations();
            obj = obj;
            LOOP_ASSERT(i, obj == aObj);
            LOOP_ASSERT(i, ta.numAllocations() == numCurrAllocs);
        }

        ASSERT(0 == ta.status());
        ASSERT(0 == assignAlloc.status());

      } break;
      case 8: {
        //---------------------------------------------------------------------
        // TESTING SWAP FUNCTION:
        //
        // Concerns:
        //    The swap function should swap the corresponding (underlying)
        //    'Datum' objects.
        //
        // Plan:
        //    Create 'Datum' object with a sufficiently long string value,
        //    so as to trigger a dynamic memory allocation. Create 'Datum'
        //    objects with each of the different types of values and swap
        //    them with the first object and verify that the objects were
        //    indeed swapped and there were no memory allocations or
        //    deallocations (in the process). Swap these objects back and
        //    verify that they in their original state. Display steps of
        //    operation frequently in verbose mode.
        //
        // Testing:
        //   void swap(Datum&);
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING SWAP FUNCION" << endl
                                  << "====================" << endl;

        struct SwapHelper {
            Datum  d_obj;
            string d_message;
        };

        const SwapHelper datumArray[] = {
            {
                Datum::createNull(),
                "null"
            },
            {
                Datum::createInteger(1),
                "integer"
            },
            {
                Datum::createBoolean(true),
                "boolean"
            },
            {
                Datum::createError(5, "some error", &ta),
                "Error"
            },
            {
                Datum::createDate(bdlt::Date(2010, 1, 5)),
                "Date"
            },
            {
                Datum::createTime(bdlt::Time(16, 45, 32, 12)),
                "Time"
            },
            {
                Datum::copyString("Abc", &ta),
                "short string"
            },
            {
                Datum::copyString("Another long string", &ta),
                "long string"
            },
            {
                Datum::createDatetime(
                               bdlt::Datetime(2010, 1, 5, 16, 45, 32, 12), &ta),
                "DateTime"
            },
            {
                Datum::createDatetimeInterval(
                               bdlt::DatetimeInterval(34, 16, 45, 32, 12), &ta),
                "DatetimeInterval"
            },
            {
                Datum::createInteger64(9223372036854775807LL, &ta),
                "Int64"
            },
            {
                Datum::createUdt(&udt, UDT_TYPE),
                "user defined"
            },
            {
                Datum::createDouble(-3.14),
                "double"
            },
            {
                createArray(&ta),
                "array",
            },
            {
                createEmptyArray(),
                "array",
            },
            {
                createMap(&ta),
                "map",
            },
            {
                createEmptyMap(),
                "map",
            }
        };
        const int SIZE  = sizeof(datumArray) / sizeof(SwapHelper);

        const char *str = "A very long string";

        for (int i = 0; i < SIZE; ++i) {
            char idx = 'a';
            if (verbose) cout << "\n"
                              << (i + 1)
                              << ". Testing swap with "
                              << datumArray[i].d_message
                              << " type ManagedDatum."
                              << endl;

            if (verbose) cout << "\t"
                              << idx++
                              << ". Swap two ManagedDatum objects and verify that"
                              << " there were no memory allocations or "
                              << "deallocations."
                              << endl;
            ManagedDatum obj1(Datum::copyString(str, &ta), &ta);
            ManagedDatum obj2(datumArray[i].d_obj, &ta);

            const Int64 numCurrAllocs = ta.numAllocations();
            const Int64 numCurrDeallocs = ta.numDeallocations();
            obj1.swap(obj2);
            LOOP_ASSERT(i, ta.numAllocations() == numCurrAllocs);
            LOOP_ASSERT(i, ta.numDeallocations() == numCurrDeallocs);

            if (verbose) cout << "\t"
                              << idx++
                              << ". Verify that both objects have expected "
                              << "values."
                              << endl;
            LOOP_ASSERT(i, obj2->isString() && str == obj2->theString());
            LOOP_ASSERT(i, obj1.datum() == datumArray[i].d_obj);

            if (verbose) cout << "\t"
                              << idx++
                              << ". Swap them back and verify the values."
                              << endl;
            obj2.swap(obj1);
            LOOP_ASSERT(i, ta.numAllocations() == numCurrAllocs);
            LOOP_ASSERT(i, ta.numDeallocations() == numCurrDeallocs);
            LOOP_ASSERT(i, obj1->isString() && str == obj1->theString());
            LOOP_ASSERT(i, obj2.datum() == datumArray[i].d_obj);
        }

        ASSERT(0 == ta.status());

      } break;
      case 7: {
        //---------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //
        // Concerns:
        //    'Datum' objects should be copy-constructed without affecting
        //    passed in argument. Both the objects should compare equal. No
        //    dynamic memory should be allocated when the object that is
        //    copied does not have dynamically allocated memory. Memory should
        //    should only be allocated from the passed in allocator (or the
        //    default allocator, in case no allocator is passed) and the
        //    allocator of the source object should not be used.
        //
        // Plan:
        //    Create 'Datum' objects with different types of values. Create
        //    other 'Datum' objects by copying the original ones. Verify
        //    that both objects have the same value. Verify that no memory
        //    was allocated when the copied object did not have any
        //    dynamically allocated memory. Verify no memory was leaked on .
        //    destruction. Verify that allocator of the source object is not
        //    used and only the passed in allocator is used to allocate memory.
        //
        // Testing:
        //   ManagedDatum(const ManagedDatum&, bslma::Allocator *);
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING COPY CONSTRUCTOR" << endl
                                  << "========================" << endl;

        // Allocator for copy-construction of new objects.
        bslma::TestAllocator copyAlloc;

        struct CopyHelper {
            Datum  d_obj;
            string d_message;
            bool   d_hasDynamicMemory;
        };

        const CopyHelper datumArray[] = {
            {
                Datum::createNull(),
                "null",
                false
            },
            {
                Datum::createInteger(1),
                "integer",
                false
            },
            {
                Datum::createBoolean(true),
                "boolean",
                false
            },
            {
                Datum::createError(5),
                "Error",
                false
            },
            {
                Datum::createDouble(-3.1416),
                "regular double",
                false
            },
            {
                Datum::createDouble(k_DBL_INF),
                "infinity double",
                false
            },
            {
                Datum::createDouble(k_DBL_QNAN2),
                "NaN double",
                false
            },
            {
                Datum::createDate(bdlt::Date(2010, 1, 5)),
                "Date",
                false
            },
            {
                Datum::createTime(bdlt::Time(16, 45, 32, 12)),
                "Time",
                false
            },
            {
                Datum::copyString("Abc", &ta),
                "short string",
                false
            },
            {
                Datum::copyString("This is a long string", &ta),
                "long string",
                true
            },
            {
                Datum::createDatetime(
                               bdlt::Datetime(2010, 1, 5, 16, 45, 32, 12), &ta),
                "DateTime",
                true
            },
            {
                Datum::createDatetimeInterval(
                               bdlt::DatetimeInterval(34, 16, 45, 32, 12), &ta),
                "DatetimeInterval",
                true
            },
            {
                Datum::createInteger64(9223372036854775807LL, &ta),
                "Int64",
                true
            },
            {
                Datum::createUdt(&udt, UDT_TYPE),
                "user defined",
                false
            },
            {
                createArray(&ta),
                "array",
                true
            },
            {
                createEmptyArray(),
                "array",
                true
            },
            {
                createMap(&ta),
                "map",
                true
            },
            {
                createEmptyMap(),
                "map",
                true
            }
        };
        const int SIZE  = sizeof(datumArray) / sizeof(CopyHelper);

        for (int i = 0; i < SIZE; ++i) {
            char idx = 'a';
            if (verbose) cout << "\n"
                              << (i + 1)
                              << ". Copying ManagedDatum with a "
                              << datumArray[i].d_message
                              << " value."
                              << endl;

            if (verbose) cout << "\t"
                              << idx++
                              << ". Copy-construct a ManagedDatum."
                              << endl;
            const Int64 numCurrAllocs = ta.numAllocations();
            ManagedDatum obj(datumArray[i].d_obj, &ta);
            const ManagedDatum cObj(obj, &copyAlloc);

            if (verbose) cout << "\t"
                              << idx++
                              << ". Verify that both objects have the same "
                              << "value."
                              << endl;
            if (obj == obj) {
                LOOP_ASSERT(i, cObj == obj);
            }
            else { // NaN
                LOOP_ASSERT(i, cObj != obj);
            }

            if (datumArray[i].d_hasDynamicMemory) {
                if (verbose) cout << "\t"
                                  << idx++
                                  << ". Verify that memory was allocated from"
                                  << " the copy allocator while copying."
                                  << endl;
                ASSERT(ta.numAllocations() == numCurrAllocs);
                ASSERT(0 != copyAlloc.numAllocations());
            }
            else {
                if (verbose) cout << "\t"
                                  << idx++
                                  << ". Verify that no memory was allocated "
                                  << "while copying."
                                  << endl;
                ASSERT(ta.numAllocations() == numCurrAllocs);
                LOOP_ASSERT(i, 0 == copyAlloc.status());
            }
        }

        ASSERT(0 == ta.status());
        ASSERT(0 == copyAlloc.status());

      } break;
      case 6: {
        //---------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS:
        //
        // Concerns:
        //    Any variation in value or type of 'Datum' object must be
        //    detected by the equality operators.
        //
        // Plan:
        //    Create 'Datum' objects of different types. Also create
        //    'Datum' objects of the same type but with subtle variations
        //    in values. Verify the correctness of 'operator==' and
        //    'operator!='.
        //
        // Testing:
        //   bool operator==(const ManagedDatum&, const ManagedDatum&);
        //   bool operator!=(const ManagedDatum&, const ManagedDatum&);
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Equality Operators" << endl
                                  << "==========================" << endl;

        PVV("1. Create some ManagedDatum objects of different types.");

        Obj datumArray[] = {
            Obj(),
            Obj(Datum::createInteger(0), &ta),
            Obj(Datum::createInteger(1), &ta),
            Obj(Datum::createBoolean(true), &ta),
            Obj(Datum::createBoolean(false), &ta),
            Obj(Datum::createError(0), &ta),
            Obj(Datum::createError(1, "some error", &ta), &ta),
            Obj(Datum::createDate(bdlt::Date(2010, 1, 5)), &ta),
            Obj(Datum::createDate(bdlt::Date(1, 1, 1)), &ta),
            Obj(Datum::createTime(bdlt::Time(16, 45, 32, 12)), &ta),
            Obj(Datum::createTime(bdlt::Time(1, 1, 1, 1)), &ta),
            Obj(Datum::createDatetime(
                        bdlt::Datetime(2010, 1, 5, 16, 45, 32, 12), &ta), &ta),
            Obj(Datum::createDatetime(
                        bdlt::Datetime(1, 1, 1, 1, 1, 1, 1), &ta), &ta),
            Obj(Datum::createDatetimeInterval(
                        bdlt::DatetimeInterval(34, 16, 45, 32, 12), &ta), &ta),
            Obj(Datum::createDatetimeInterval(
                        bdlt::DatetimeInterval(1, 1, 1, 1, 1), &ta), &ta),
            Obj(Datum::createInteger64(9223372036854775807LL, &ta), &ta),
            Obj(Datum::createInteger64(1229782938247303441LL, &ta), &ta),
            Obj(Datum::copyString("A long string", &ta), &ta),
            Obj(Datum::copyString("A very long string", &ta), &ta),
            Obj(Datum::copyString("abc", &ta), &ta),
            Obj(Datum::copyString("Abc", &ta), &ta),
            Obj(Datum::createDouble(1.0), &ta),
            Obj(Datum::createDouble(1.2345), &ta),
            Obj(Datum::createDouble(-1.2346), &ta),
            Obj(Datum::createDouble(k_DBL_MIN2), &ta),
            Obj(Datum::createDouble(k_DBL_MAX2), &ta),
            Obj(Datum::createDouble(k_DBL_INF), &ta),
            Obj(Datum::createDouble(k_DBL_NEGINF), &ta),
            // This test will take care of 0.0 as -0.0 == 0.0.
            Obj(Datum::createDouble(k_DBL_NEGZERO), &ta),
            Obj(Datum::createUdt(&udt, UDT_TYPE), &ta),
            Obj(Datum::createUdt(&udt, 1), &ta),
            Obj(Datum::createUdt(&udt1, UDT_TYPE), &ta),
            // Treat NaNs specially as value == value is false for NaN.
            Obj(Datum::createDouble(k_DBL_QNAN2), &ta),
            Obj(Datum::createDouble(k_DBL_SNAN), &ta),
            Obj(createArray(&ta), &ta),
            Obj(createEmptyArray(), &ta),
            Obj(createMap(&ta), &ta),
            Obj(createEmptyMap(), &ta)
        };
        const int datumArraySize = sizeof(datumArray) / sizeof(*datumArray);

        if (verbose) cout << "\n2. Compare ManagedDatum objects" << endl;
        for (int i = 0; i < datumArraySize; ++i) {
            for (int j = 0; j < datumArraySize; ++j) {
                if (i == j) {
                    if (datumArray[i]->isDouble() &&
                            datumArray[i]->theDouble() !=
                                datumArray[i]->theDouble()) {
                        LOOP2_ASSERT(i,
                                     j,
                                     datumArray[i] != datumArray[j]);
                    }
                    else {
                        LOOP2_ASSERT(i,
                                     j,
                                     datumArray[i] == datumArray[j]);
                    }
                }
                else {
                    LOOP2_ASSERT(i, j, datumArray[i] != datumArray[j]);
                }
            }
        }

      } break;
      case 5: {
        //---------------------------------------------------------------------
        // BASIC ACCESSORS:
        //
        // Concerns:
        //    Confirm that we can invoke 'isXXX' functions (where XXX is a
        //    particular type) on 'Datum' objects using 'operator->()', and
        //    that they return 'true' for the correct type of 'Datum'
        //    object and 'false' otherwise.
        //
        // Plan:
        //    Create 'Datum' objects with each of the different creators.
        //    Verify that the created 'Datum' objects are of the correct
        //    type and are not of any other type also.
        //
        // Testing:
        //   const Datum *operator->() const;
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "BASIC ACCESSORS" << endl
                                  << "===============" << endl;

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            PVV("1. Testing isNull()");

            PVVV("\ta. Create ManagedDatum with a nil value.");
            const Obj obj;

            PVVV("\tb. Check if object has only a nil value.");
            ASSERT(obj->isNull());
            ASSERT(!obj->isInteger());
            ASSERT(!obj->isBoolean());
            ASSERT(!obj->isError());
            ASSERT(!obj->isDouble());
            ASSERT(!obj->isInteger64());
            ASSERT(!obj->isDate());
            ASSERT(!obj->isTime());
            ASSERT(!obj->isDatetime());
            ASSERT(!obj->isDatetimeInterval());
            ASSERT(!obj->isString());
            ASSERT(!obj->isUdt());
            ASSERT(!obj->isArray());
            ASSERT(!obj->isMap());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            PVV("2. Testing isInteger()");

            PVVV("\ta. Create ManagedDatum with an integer value.");
            const Obj obj(Datum::createInteger(1), &ta);

            PVVV("\tb. Check if object has only an integer value.");
            ASSERT(obj->isInteger());
            ASSERT(!obj->isNull());
            ASSERT(!obj->isBoolean());
            ASSERT(!obj->isError());
            ASSERT(!obj->isDouble());
            ASSERT(!obj->isInteger64());
            ASSERT(!obj->isDate());
            ASSERT(!obj->isTime());
            ASSERT(!obj->isDatetime());
            ASSERT(!obj->isDatetimeInterval());
            ASSERT(!obj->isString());
            ASSERT(!obj->isUdt());
            ASSERT(!obj->isArray());
            ASSERT(!obj->isMap());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            PVV("3. Testing isBoolean()");

            PVVV("\ta. Create ManagedDatum with a boolean value.");
            const Obj obj(Datum::createBoolean(true), &ta);

            PVVV("\tb. Check if object has only a boolean value.");
            ASSERT(obj->isBoolean());
            ASSERT(!obj->isInteger());
            ASSERT(!obj->isNull());
            ASSERT(!obj->isError());
            ASSERT(!obj->isDouble());
            ASSERT(!obj->isInteger64());
            ASSERT(!obj->isDate());
            ASSERT(!obj->isTime());
            ASSERT(!obj->isDatetime());
            ASSERT(!obj->isDatetimeInterval());
            ASSERT(!obj->isString());
            ASSERT(!obj->isUdt());
            ASSERT(!obj->isArray());
            ASSERT(!obj->isMap());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            PVV("4. Testing isError()");

            PVVV("\ta. Create ManagedDatum with an Error value.");
            const Obj obj(Datum::createError(5, "some error", &ta), &ta);

            PVVV("\tb. Check if object has only an Error value");
            ASSERT(obj->isError());
            ASSERT(!obj->isBoolean());
            ASSERT(!obj->isInteger());
            ASSERT(!obj->isNull());
            ASSERT(!obj->isDouble());
            ASSERT(!obj->isInteger64());
            ASSERT(!obj->isDate());
            ASSERT(!obj->isTime());
            ASSERT(!obj->isDatetime());
            ASSERT(!obj->isDatetimeInterval());
            ASSERT(!obj->isString());
            ASSERT(!obj->isUdt());
            ASSERT(!obj->isArray());
            ASSERT(!obj->isMap());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            PVV("5. Testing isDate()");

            PVVV("\ta. Create ManagedDatum with a Date value.");
            const Obj obj(Datum::createDate(bdlt::Date(2010, 1, 5)), &ta);

            PVVV("\tb. Check if object has only a date value.");
            ASSERT(obj->isDate());
            ASSERT(!obj->isError());
            ASSERT(!obj->isBoolean());
            ASSERT(!obj->isInteger());
            ASSERT(!obj->isNull());
            ASSERT(!obj->isDouble());
            ASSERT(!obj->isInteger64());
            ASSERT(!obj->isTime());
            ASSERT(!obj->isDatetime());
            ASSERT(!obj->isDatetimeInterval());
            ASSERT(!obj->isString());
            ASSERT(!obj->isUdt());
            ASSERT(!obj->isArray());
            ASSERT(!obj->isMap());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            PVV("6. Testing isTime()");

            PVVV("\ta. Create ManagedDatum with a Time value.");
            const Obj obj(Datum::createTime(bdlt::Time(16, 45, 32, 12)), &ta);

            PVVV("\tb. Check if object has only a time value.");
            ASSERT(obj->isTime());
            ASSERT(!obj->isDate());
            ASSERT(!obj->isError());
            ASSERT(!obj->isBoolean());
            ASSERT(!obj->isInteger());
            ASSERT(!obj->isNull());
            ASSERT(!obj->isDouble());
            ASSERT(!obj->isInteger64());
            ASSERT(!obj->isDatetime());
            ASSERT(!obj->isDatetimeInterval());
            ASSERT(!obj->isString());
            ASSERT(!obj->isUdt());
            ASSERT(!obj->isArray());
            ASSERT(!obj->isMap());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            PVV("7. Testing isDatetime()");

            PVVV("\ta. Create ManagedDatum with a date+time value.");
            const Obj obj(Datum::createDatetime(
                         bdlt::Datetime(2010, 1, 5, 16, 45, 32, 12), &ta), &ta);

            PVVV("\tb. Check if object has only a date+time value.");
            ASSERT(obj->isDatetime());
            ASSERT(!obj->isTime());
            ASSERT(!obj->isDate());
            ASSERT(!obj->isError());
            ASSERT(!obj->isBoolean());
            ASSERT(!obj->isInteger());
            ASSERT(!obj->isNull());
            ASSERT(!obj->isDouble());
            ASSERT(!obj->isInteger64());
            ASSERT(!obj->isDatetimeInterval());
            ASSERT(!obj->isString());
            ASSERT(!obj->isUdt());
            ASSERT(!obj->isArray());
            ASSERT(!obj->isMap());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            PVV("8. Testing isDatetimeInterval()");

            PVVV("\ta. Create Datum with a date+time interval value.");
            const Obj obj(Datum::createDatetimeInterval(
                         bdlt::DatetimeInterval(34, 16, 45, 32, 12), &ta), &ta);

            PVVV("\tb. Check if object has only a date+time interval value.");
            ASSERT(obj->isDatetimeInterval());
            ASSERT(!obj->isDatetime());
            ASSERT(!obj->isTime());
            ASSERT(!obj->isDate());
            ASSERT(!obj->isError());
            ASSERT(!obj->isBoolean());
            ASSERT(!obj->isInteger());
            ASSERT(!obj->isNull());
            ASSERT(!obj->isDouble());
            ASSERT(!obj->isInteger64());
            ASSERT(!obj->isString());
            ASSERT(!obj->isUdt());
            ASSERT(!obj->isArray());
            ASSERT(!obj->isMap());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            PVVV("9. Testing isInteger64()");

            PVVV("\a. Create ManagedDatum with an Int64 value.");
            const Obj obj(Datum::createInteger64(9223372036854775807LL, &ta),
                          &ta);

            PVVV("\tb. Check if object has only an Int64 value.");
            ASSERT(obj->isInteger64());
            ASSERT(!obj->isDatetimeInterval());
            ASSERT(!obj->isDatetime());
            ASSERT(!obj->isTime());
            ASSERT(!obj->isDate());
            ASSERT(!obj->isError());
            ASSERT(!obj->isBoolean());
            ASSERT(!obj->isInteger());
            ASSERT(!obj->isNull());
            ASSERT(!obj->isDouble());
            ASSERT(!obj->isString());
            ASSERT(!obj->isUdt());
            ASSERT(!obj->isArray());
            ASSERT(!obj->isMap());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        PVV("10. Testing isString()");

        //- -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            PVVV("\ta. Create ManagedDatum with a long string value.");
            const Obj obj(Datum::copyString("A long string", &ta), &ta);

            PVVV("\tb. Check if object has only a string value.");
            ASSERT(obj->isString());
            ASSERT(!obj->isInteger64());
            ASSERT(!obj->isDatetimeInterval());
            ASSERT(!obj->isDatetime());
            ASSERT(!obj->isTime());
            ASSERT(!obj->isDate());
            ASSERT(!obj->isError());
            ASSERT(!obj->isBoolean());
            ASSERT(!obj->isInteger());
            ASSERT(!obj->isNull());
            ASSERT(!obj->isDouble());
            ASSERT(!obj->isUdt());
            ASSERT(!obj->isArray());
            ASSERT(!obj->isMap());
        }

        //- -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            PVVV("\td. Create ManagedDatum with a short string value.");
            const Obj obj(Datum::copyString("abc", &ta), &ta);

            PVVV("\te. Check if object has only a string value.");
            ASSERT(obj->isString());
            ASSERT(!obj->isInteger64());
            ASSERT(!obj->isDatetimeInterval());
            ASSERT(!obj->isDatetime());
            ASSERT(!obj->isTime());
            ASSERT(!obj->isDate());
            ASSERT(!obj->isError());
            ASSERT(!obj->isBoolean());
            ASSERT(!obj->isInteger());
            ASSERT(!obj->isNull());
            ASSERT(!obj->isDouble());
            ASSERT(!obj->isUdt());
            ASSERT(!obj->isArray());
            ASSERT(!obj->isMap());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        PVV("11. Testing isDouble()");

        //- -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            PVVV("\ta. Create ManagedDatum with the minimum double value.");
            const Obj obj(Datum::createDouble(k_DBL_MIN2), &ta);

            PVVV("\tb. Check if object has only a double value.");
            ASSERT(obj->isDouble());
            ASSERT(!obj->isString());
            ASSERT(!obj->isInteger64());
            ASSERT(!obj->isDatetimeInterval());
            ASSERT(!obj->isDatetime());
            ASSERT(!obj->isTime());
            ASSERT(!obj->isDate());
            ASSERT(!obj->isError());
            ASSERT(!obj->isBoolean());
            ASSERT(!obj->isInteger());
            ASSERT(!obj->isNull());
            ASSERT(!obj->isUdt());
            ASSERT(!obj->isArray());
            ASSERT(!obj->isMap());
        }

        //- -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            PVVV("\tc. Create ManagedDatum with the maximum double value.");
            const Obj obj(Datum::createDouble(k_DBL_MAX2), &ta);

            PVVV("\td. Check if object has only a double value.");
            ASSERT(obj->isDouble());
            ASSERT(!obj->isString());
            ASSERT(!obj->isInteger64());
            ASSERT(!obj->isDatetimeInterval());
            ASSERT(!obj->isDatetime());
            ASSERT(!obj->isTime());
            ASSERT(!obj->isDate());
            ASSERT(!obj->isError());
            ASSERT(!obj->isBoolean());
            ASSERT(!obj->isInteger());
            ASSERT(!obj->isNull());
            ASSERT(!obj->isUdt());
            ASSERT(!obj->isArray());
            ASSERT(!obj->isMap());
        }

        //- -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            PVVV("\te. Create ManagedDatum with the double value of infinity.");
            const Obj obj(Datum::createDouble(k_DBL_INF), &ta);

            PVVV("\tf. Check if object has only a double value");
            ASSERT(obj->isDouble());
            ASSERT(!obj->isString());
            ASSERT(!obj->isInteger64());
            ASSERT(!obj->isDatetimeInterval());
            ASSERT(!obj->isDatetime());
            ASSERT(!obj->isTime());
            ASSERT(!obj->isDate());
            ASSERT(!obj->isError());
            ASSERT(!obj->isBoolean());
            ASSERT(!obj->isInteger());
            ASSERT(!obj->isNull());
            ASSERT(!obj->isUdt());
            ASSERT(!obj->isArray());
            ASSERT(!obj->isMap());
        }

        //- -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
        {
            PVVV("\tk. Create ManagedDatum with the double value of quiet NaN.");
            const Obj obj(Datum::createDouble(k_DBL_QNAN2), &ta);

            PVVV("\tl. Check if object has only a double value.");
            ASSERT(obj->isDouble());
            ASSERT(!obj->isString());
            ASSERT(!obj->isInteger64());
            ASSERT(!obj->isDatetimeInterval());
            ASSERT(!obj->isDatetime());
            ASSERT(!obj->isTime());
            ASSERT(!obj->isDate());
            ASSERT(!obj->isError());
            ASSERT(!obj->isBoolean());
            ASSERT(!obj->isInteger());
            ASSERT(!obj->isNull());
            ASSERT(!obj->isUdt());
            ASSERT(!obj->isArray());
            ASSERT(!obj->isMap());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            PVV("12. Testing isUdt()");

            PVVV("\ta. Create ManagedDatum with a user defined object");
            const Obj obj(Datum::createUdt(&udt, UDT_TYPE), &ta);

            PVVV("\tb. Check if object has only a user defined object");
            ASSERT(obj->isUdt());
            ASSERT(!obj->isDouble());
            ASSERT(!obj->isString());
            ASSERT(!obj->isInteger64());
            ASSERT(!obj->isDatetimeInterval());
            ASSERT(!obj->isDatetime());
            ASSERT(!obj->isTime());
            ASSERT(!obj->isDate());
            ASSERT(!obj->isError());
            ASSERT(!obj->isBoolean());
            ASSERT(!obj->isInteger());
            ASSERT(!obj->isNull());
            ASSERT(!obj->isArray());
            ASSERT(!obj->isMap());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            PVV("13. Testing isArray()");

            PVVV("\ta. Create ManagedDatum with an array value.");
            const Obj obj(createArray(&ta), &ta);

            PVVV("\tb. Check if object has only an array value.");
            ASSERT(obj->isArray());
            ASSERT(!obj->isUdt());
            ASSERT(!obj->isDouble());
            ASSERT(!obj->isString());
            ASSERT(!obj->isInteger64());
            ASSERT(!obj->isDatetimeInterval());
            ASSERT(!obj->isDatetime());
            ASSERT(!obj->isTime());
            ASSERT(!obj->isDate());
            ASSERT(!obj->isError());
            ASSERT(!obj->isBoolean());
            ASSERT(!obj->isInteger());
            ASSERT(!obj->isNull());
            ASSERT(!obj->isMap());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        {
            PVV("14. Testing isMap()");

            PVVV("\ta. Create ManagedDatum with a map value.");
            const Obj obj(createMap(&ta), &ta);

            PVVV("\tb. Check if object has only a map value.");
            ASSERT(obj->isMap());
            ASSERT(!obj->isArray());
            ASSERT(!obj->isUdt());
            ASSERT(!obj->isDouble());
            ASSERT(!obj->isString());
            ASSERT(!obj->isInteger64());
            ASSERT(!obj->isDatetimeInterval());
            ASSERT(!obj->isDatetime());
            ASSERT(!obj->isTime());
            ASSERT(!obj->isDate());
            ASSERT(!obj->isError());
            ASSERT(!obj->isBoolean());
            ASSERT(!obj->isInteger());
            ASSERT(!obj->isNull());
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING TEST APPARATUS:
        //    Nothing to test.
        // --------------------------------------------------------------------

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS:
        //
        // Concerns:
        //    The manipulators should destroy the existing 'Datum'
        //    object held onto by this 'Datum' object and release any
        //    dynamically allocated memory held inside the 'Datum'
        //    object. A new 'Datum' object should be created out of the
        //    value passed to the manipulator. Dynamic memory should be only
        //    allocated when needed. The currently stored allocator should be
        //    used to allocate any memory. Assigning the value of an object to
        //    that same object should be the same as assigning a different
        //    value to that object.
        //
        // Plan:
        //    Create 'Datum' object with a sufficiently long string value,
        //    so as to trigger a dynamic memory allocation. Assign each of the
        //    different types of values. Confirm that previously held
        //    'Datum' object is destroyed and its memory is released.
        //    Use the basic accessors to verify that the type and value inside
        //    the 'Datum' objects is as expected. Verify that memory
        //    allocations are as expected. Finally destroy these objects and
        //    verify that no memory was leaked. Verify that assigning the value
        //    of an object to that same object triggers destruction of
        //    previously held object. Display steps of operation frequently in
        //    verbose mode.
        //
        // Testing:
        //   void adopt(const Datum&);
        //   void copy(const Datum&);
        //   Datum release();
        //   void makeNull();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PRIMARY MANIPULATORS" << endl
                                  << "====================" << endl;

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("1. Testing 'makeNull'.");
        {
            PVVV("\ta. Create 'ManagedDatum' with a string value.");
            Obj obj(Datum::copyString("A long string", &ta), &ta);

            PVVV("\tb. Call 'makeNull' and verify that the object is destroyed.");
            obj.makeNull();
            ASSERT(0 == ta.status());
            ASSERT(obj->isNull());
        }
        if (verbose) cout << "\tc. Verify no memory was leaked." << endl;
        ASSERT(0 == ta.status());

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("2. Testing adopting a Datum object.");
        {
            PVVV("\ta. Create ManagedDatum with a string value.");
            Obj obj(Datum::copyString("A long string", &ta), &ta);

            PVVV("\tb. Adopt a Datum object and verify "
                 "that the previous object is destroyed and "
                 "the new object is adopted and not copied.");
            const Datum rcObj = Datum::copyString("This is a string", &ta);
            Int64 numDeallocs = ta.numDeallocations();
            const Int64 numAllocs = ta.numAllocations();
            obj.adopt(rcObj);
            ASSERT(numDeallocs <= ta.numDeallocations());
            ASSERT(numAllocs == ta.numAllocations());
            ASSERT(obj.datum() == rcObj);

            PVVV("\tc. Adopt object already stored inside and "
                 "verify the state of the object.");
            numDeallocs = ta.numDeallocations();
            obj.adopt(obj.datum());
            ASSERT(numDeallocs == ta.numDeallocations());
            ASSERT(obj.datum() == rcObj);
        }
        PVVV("\td. Verify no memory was leaked.");
        ASSERT(0 == ta.status());

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("3. Testing copying a Datum object.");
        {
            PVVV("\ta. Create ManagedDatum with a string value.");
            Obj obj(Datum::copyString("A long string", &ta), &ta);

            PVVV("\tb. Copy a Datum object and verify "
                 "that the previous object is destroyed and "
                 "the new object is copied.");
            const Datum rcObj = Datum::copyString("This is a string",
                                                          &ta);
            Int64 numDeallocs = ta.numDeallocations();
            Int64 numAllocs = ta.numAllocations();
            obj.clone(rcObj);
            ASSERT(numDeallocs <= ta.numDeallocations());
            ASSERT(numAllocs < ta.numAllocations());
            ASSERT(obj.datum() == rcObj);

            PVVV("\tc. Copy object already stored inside and "
                 "verify the state of the object.");
            numDeallocs = ta.numDeallocations();
            numAllocs = ta.numAllocations();
            obj.adopt(obj.datum());
            ASSERT(numDeallocs == ta.numDeallocations());
            ASSERT(numAllocs == ta.numAllocations());
            ASSERT(obj.datum() == rcObj);

            Datum::destroy(rcObj, &ta);
        }
        PVVV("\td. Verify no memory was leaked.");
        ASSERT(0 == ta.status());

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("4. Testing releasing a Datum object.");
        {
            PVVV("\ta. Create ManagedDatum with a string value.");
            Obj obj(Datum::copyString("A long string", &ta), &ta);
            const Datum internalObj = obj.datum();

            PVVV("\tb. Call release on the object and verify "
                 "that the previous value is returned and it "
                 "is not destroyed. Also verify that the "
                 "object does not have any value.");
            const Int64 numDeallocs = ta.numDeallocations();
            const Datum rcObj = obj.release();
            ASSERT(internalObj == rcObj);
            ASSERT(numDeallocs == ta.numDeallocations());
            ASSERT(obj.datum().isNull());

            PVVV("\tc. Destroy the released 'Datum' object.");
            Datum::destroy(rcObj, &ta);
        }
        PVVV("\td. Verify no memory was leaked.");
        ASSERT(0 == ta.status());

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CREATORS:
        //
        // Concerns:
        //    The constructors should correctly forward the passed in fields
        //    to the underlying 'Datum' and thus create 'datumArray'
        //    having the correct type and holding the correct value. Dynamic
        //    memory should be only allocated when needed. If no allocator is
        //    passed in, the currently installed default allocator should be
        //    used to allocate any dynamically allocated memory. The destructor
        //    should trigger a deallocation of any dynamically allocated
        //    memory.
        //
        // Plan:
        //    Create 'Datum' objects with each of the different
        //    constructors using different values of the same type. Use the
        //    basic accessors to verify that the type and value inside the
        //    'Datum' objects is as expected. Verify that memory
        //    allocations are as expected. Finally destroy these objects and
        //    verify that no memory was leaked. Display steps of operation
        //    frequently in verbose mode.
        //
        // Testing:
        //   ManagedDatum(bslma::Allocator *);
        //   ManagedDatum(const Datum&, bslma::Allocator *);
        //   ~ManagedDatum();
        //   const Datum& datum() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CREATORS" << endl
                                  << "========" << endl;

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("1. Testing constructor with no arguments.")
        {
            PVVV("\ta. Create ManagedDatum with a nil value.");
            const ManagedDatum obj(&ta);

            PVV("\tb. Check if object has a nil value.")
            ASSERT(obj.datum().isNull());

            PVV("\tc. Verify that no memory was allocated.")
            ASSERT(0 == ta.status());
        }

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        PVV("2. Testing constructor with Datum argument.")
        {
            PVVV("\ta. Create ManagedDatum with a Datum value.")
            const Datum rcObj =
                Datum::copyString("A long string", &ta);
            const Int64 numCurrBytesInUse = ta.numBytesInUse();
            const ManagedDatum obj(rcObj, &ta);

            PVVV("\tb. Check if object has the same Datum value.")
            ASSERT(rcObj == obj.datum());

            PVVV("\tc. Verify that no memory was allocated.")
            ASSERT(numCurrBytesInUse == ta.numBytesInUse());
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Concerns:
        //    Exercise a broad cross-section of full value-semantic
        //    functionality before beginning testing in earnest. Probe the
        //    functionality systematically and incrementally to discover
        //    basic errors in isolation.
        //
        // Plan:
        //    Create 'Datum' objects with integer, string and double
        //    values. Verify that they have the correct type of value and the
        //    correct value stored in them. Compare all the 'Datum'
        //    objects. Assign a string value to the 'Datum' object holding
        //    integer value. Now assign an integer value to the 'Datum'
        //    object holding a string value and verify that no memory was
        //    leaked. Create a 'Datum' object with a date+time value. Copy
        //    construct another object from it. Verify both objects are equal.
        //    Destroy all the objects and verify that no memory was leaked.
        //
        // Testing:
        //     This test case exercises basic full value-semantic
        //     functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;
        {
            PVV("1. Create few ManagedDatum objects.");
            Obj obj1(Datum::createInteger(5), &ta);
            Obj obj2(Datum::copyString("This is a string", &ta), &ta);
            const Obj obj3(Datum::createDouble(5.0), &ta);

            PVV("2. Verify that the objects are of correct "
                "type and have the correct value.");
            ASSERT(obj1->isInteger());
            ASSERT(5 == obj1->theInteger());
            ASSERT(obj2->isString());
            ASSERT("This is a string" == obj2->theString());
            ASSERT(obj3->isDouble());
            ASSERT(5.0 == obj3->theDouble());

            PVV("3. Compare these objects.");
            ASSERT(obj1 != obj3);
            ASSERT(obj1 != obj2);

            PVV("4. Assign different values to existing "
                "objects and verify no memory is leaked.");
            obj1.adopt(Datum::copyString("This is a string", &ta));
            ASSERT(obj1 == obj2);
            const Int64 numCurrBytesInUse = ta.numBytesInUse();
            obj2.adopt(Datum::createInteger(5));
            ASSERT(ta.numBytesInUse() < numCurrBytesInUse);

            PVV("5. Copy construct an object. Verify it is"
                " equal to the existing object.");
            const Obj obj4(Datum::createDatetime(
                         bdlt::Datetime(2010, 1, 5, 16, 45, 32, 12), &ta), &ta);
            const Obj obj5(obj4, &ta);
            ASSERT(obj4 == obj5);
        }
        PVV("6. Verify that no memory was leaked.");
        ASSERT(0 == ta.status());

      } break;
      default: {
          cerr << "WARNING: CASE '" << test << "' NOT FOUND." << endl;
          testStatus = -1;
      }
    }

    for (int i = 0; i < arraySize; ++i) {
        Datum::destroy(array[i], &arrayAlloc);
    }

    for (int i = 0; i < mapSize; ++i) {
        Datum::destroy(map[i].value(), &mapAlloc);
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
