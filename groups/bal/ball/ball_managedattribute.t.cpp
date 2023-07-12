// ball_managedattribute.t.cpp                                        -*-C++-*-
#include <ball_managedattribute.h>

#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bslim_testutil.h>

#include <bsls_asserttest.h>
#include <bsls_review.h>
#include <bsls_types.h>

#include <bsltf_simpletesttype.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The class under test is a standard value-semantic type.  For the standard
// 10-step test procedure, we choose the constructor that takes a name and a
// value of type 'ball::Attribute::Value' as the primary manipulator, and the
// 'name' and 'value' methods as the basic accessors.  Our concerns regarding
// the implementation of this component are that (1) all supported
// value-semantic methods are implemented correctly; (2) the hash values must
// be calculated correctly and must be re-calculated after the objects have
// been modified.
//-----------------------------------------------------------------------------
// [13] static int hash(const ManagedAttribute&, int size);
// [10] ManagedAttribute(const str_view& n, int v, alloc);
// [10] ManagedAttribute(const str_view& n, long v, alloc);
// [10] ManagedAttribute(const str_view& n, long long v, alloc);
// [10] ManagedAttribute(const str_view& n, unsigned int v, alloc);
// [10] ManagedAttribute(const str_view& n, unsigned long v, alloc);
// [10] ManagedAttribute(const str_view& n, unsigned long long v, alloc);
// [10] ManagedAttribute(const str_view& n, const string_view& v, alloc);
// [10] ManagedAttribute(const str_view& n, const char *v, alloc);
// [10] ManagedAttribute(const str_view& n, const void *v, alloc);
// [10] ManagedAttribute(const Attribute&, alloc);
// [ 2] ManagedAttribute(const str_view& n, const Value& v, alloc);
// [ 7] ManagedAttribute(const ManagedAttribute&, alloc);
// [ 2] ~ManagedAttribute();
// [ 9] ManagedAttribute& operator=(const ManagedAttribute& rhs);
// [12] void setName(const bsl::string_view& name);
// [12] void setValue(int value);
// [12] void setValue(long value);
// [12] void setValue(long long value);
// [12] void setValue(unsigned int value);
// [12] void setValue(unsigned long value);
// [12] void setValue(unsigned long long value);
// [12] void setValue(const bsl::string_view& value);
// [12] void setValue(const char *value);
// [12] void setValue(const void *value);
// [11] void setValue(const Attribute::Value& value);
// [ 4] const Attribute& attribute() const;
// [ 4] const bsl::string& key() const;
// [ 4] const char *name() const;
// [ 4] const Value& value() const;
// [  ] allocator_type get_allocator() const;
// [ 5] bsl::ostream& print(bsl::ostream& stream, int lvl, int spl) const;
// [ 6] operator==(const ManagedAttribute&, const ManagedAttribute&);
// [ 6] operator!=(const ManagedAttribute&, const ManagedAttribute&);
// [ 5] bsl::ostream& operator<<(bsl::ostream&, const ManagedAttribute&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] Value createValue(VALUE *value, int size, int i);
// [ 8] UNUSED
// [14] USAGE EXAMPLE

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::ManagedAttribute Obj;
typedef ball::Attribute::Value Value;

bsltf::SimpleTestType SIMPLE_VALUE;

#define VA_NAME   ""
#define VA_VALUE  0
#define VB_NAME   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define VB_VALUE  1111111111
#define VC_NAME   "abcdefghijklmnopqrstuvwxyz"
#define VC_VALUE  1111111111LL
#define VD_NAME   "1234567890"
#define VD_VALUE  "1234567890"
#define VE_NAME   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define VE_VALUE  4242424242LL
#define VF_NAME   "SIMPLE_VALUE"
#define VF_VALUE  &SIMPLE_VALUE

#define VA VA_NAME, VA_VALUE
#define VB VB_NAME, VB_VALUE
#define VC VC_NAME, VC_VALUE
#define VD VD_NAME, VD_VALUE
#define VE VE_NAME, VE_VALUE
#define VF VF_NAME, VF_VALUE

const void *VV = (void*)1;

const char *LONG_STRING = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                          "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                          "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                          "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                          "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                          "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                          "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                          "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                          "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                          "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

#define SV SIMPLE_VALUE

const struct {
    int         d_line;       // line number
    const char *d_name_p;     // attribute name
} NAMES[] = {
    // line     name
    // ----     ----
    {  L_,      ""                            },
    {  L_,      "A"                           },
    {  L_,      "B"                           },
    {  L_,      "a"                           },
    {  L_,      "AA"                          },
    {  L_,      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"  },
    {  L_,      LONG_STRING                   },
};

enum { NUM_NAMES = sizeof NAMES / sizeof *NAMES };

static const struct Values {
    int                 d_line;    // line number

    int                 d_type;    // type of attribute value:
                                   // 0 - int
                                   // 1 - long
                                   // 2 - long long
                                   // 3 - unsigned int
                                   // 4 - unsigned long
                                   // 5 - unsigned long long
                                   // 6 - string
                                   // 7 - const void *

    long long           d_ivalue;  // integer value - used when d_type == 0-2

    unsigned long long  d_uivalue; // integer value - used when d_type == 3-5

    const char         *d_svalue;  // string value  - used when d_type == 6

    const void         *d_pvalue;  // const void * value - when d_type == 7
} VALUES[] = {
    ///line  type  ivalue     uivalue     svalue       pvalue
    ///----  ----  ------     -------     ------       ------
    {  L_,   0,    0,         0,          0,           0             },
    {  L_,   0,    1,         0,          0,           0             },
    {  L_,   0,    -1,        0,          0,           0             },
    {  L_,   0,    INT_MAX,   0,          0,           0             },
    {  L_,   0,    INT_MIN,   0,          0,           0             },

    {  L_,   1,    0,         0,          0,           0             },
    {  L_,   1,    1,         0,          0,           0             },
    {  L_,   1,    -1,        0,          0,           0             },
    {  L_,   1,    LONG_MAX,  0,          0,           0             },
    {  L_,   1,    LONG_MIN,  0,          0,           0             },

    {  L_,   2,    0,         0,          0,           0             },
    {  L_,   2,    1,         0,          0,           0             },
    {  L_,   2,    -1,        0,          0,           0             },
    {  L_,   2,    LLONG_MAX, 0,          0,           0             },
    {  L_,   2,    LLONG_MIN, 0,          0,           0             },

    {  L_,   3,    0,         0,          0,           0             },
    {  L_,   3,    0,         1,          0,           0             },
    {  L_,   3,    0,         INT_MAX,    0,           0             },

    {  L_,   4,    0,         0,          0,           0             },
    {  L_,   4,    0,         1,          0,           0             },
    {  L_,   4,    0,         LONG_MAX,   0,           0             },

    {  L_,   5,    0,         0,          0,           0             },
    {  L_,   5,    0,         1,          0,           0             },
    {  L_,   5,    0,         LLONG_MAX,  0,           0             },

    {  L_,   6,    0,         0,          "",          0             },
    {  L_,   6,    0,         0,          "0",         0             },
    {  L_,   6,    0,         0,          "A",         0             },
    {  L_,   6,    0,         0,          "B",         0             },
    {  L_,   6,    0,         0,          "a",         0             },
    {  L_,   6,    0,         0,          "AA",        0             },
    {  L_,   6,    0,         0,          LONG_STRING, 0             },

    {  L_,   7,    0,         0,          0,           0             },
    {  L_,   7,    0,         0,          0,           &SIMPLE_VALUE },
};


enum { NUM_VALUES = sizeof VALUES / sizeof *VALUES };

static const struct HashData {
    int                 d_line;       // line number
    const char         *d_name;       // attribute name
    int                 d_type;       // type of attribute value
    long long           d_ivalue;     // integer attribute value
    long long           d_uivalue;    // unsigned integer value
    const char         *d_svalue;     // string attribute value
    const void         *d_pvalue;     // const void* attribute value
    int                 d_hashSize;   // hashtable size
    int                 d_hashValue;  // expected hash value
} HASH_DATA[] = {
// line  name  type  ivalue     uivalue    svalue  pvalue  hsize   hash value
// ----  ----  ----  ------     -------    ------  ------  -----   ----------
{  L_,   "",   0,    0,         0,         0,      0,        256,        246 },
{  L_,   "A",  0,    0,         0,         0,      0,        256,        54  },
{  L_,   "A",  0,    1,         0,         0,      0,        256,        35  },
{  L_,   "A",  0,    INT_MAX,   0,         0,      0,        256,        194 },
{  L_,   "A",  0,    INT_MIN,   0,         0,      0,        256,        82  },
#ifdef BSLS_PLATFORM_OS_WINDOWS
{  L_,   "",   1,    0,         0,         0,      0,        256,        246 },
{  L_,   "A",  1,    0,         0,         0,      0,        256,        54  },
{  L_,   "A",  1,    1,         0,         0,      0,        256,        35  },
{  L_,   "A",  1,    LONG_MAX,  0,         0,      0,        256,        194 },
{  L_,   "A",  1,    LONG_MIN,  0,         0,      0,        256,        82  },
#else
{  L_,   "",   1,    0,         0,         0,      0,        256,        72  },
{  L_,   "A",  1,    0,         0,         0,      0,        256,        136 },
{  L_,   "A",  1,    1,         0,         0,      0,        256,        34  },
{  L_,   "A",  1,    LONG_MAX,  0,         0,      0,        256,        15  },
{  L_,   "A",  1,    LONG_MIN,  0,         0,      0,        256,        10  },
#endif
{  L_,   "",   2,    0,         0,         0,      0,        256,        72  },
{  L_,   "A",  2,    0,         0,         0,      0,        256,        136 },
{  L_,   "A",  2,    1,         0,         0,      0,        256,        34  },
{  L_,   "A",  2,    INT_MAX,   0,         0,      0,        256,        122 },
{  L_,   "A",  2,    INT_MIN,   0,         0,      0,        256,        50  },
{  L_,   "A",  2,    LLONG_MAX, 0,         0,      0,        256,        15  },
{  L_,   "A",  2,    LLONG_MIN, 0,         0,      0,        256,        10  },
{  L_,   "",   3,    0,         0,         0,      0,        256,        246 },
{  L_,   "A",  3,    0,         0,         0,      0,        256,        54  },
{  L_,   "A",  3,    0,         1,         0,      0,        256,        35  },
{  L_,   "A",  3,    0,         INT_MAX,   0,      0,        256,        194 },
#ifdef BSLS_PLATFORM_OS_WINDOWS
{  L_,   "",   4,    0,         0,         0,      0,        256,        246 },
{  L_,   "A",  4,    0,         0,         0,      0,        256,        54  },
{  L_,   "A",  4,    0,         1,         0,      0,        256,        35  },
{  L_,   "A",  4,    0,         LONG_MAX,  0,      0,        256,        194 },
#else
{  L_,   "",   4,    0,         0,         0,      0,        256,        72  },
{  L_,   "A",  4,    0,         0,         0,      0,        256,        136 },
{  L_,   "A",  4,    0,         1,         0,      0,        256,        34  },
{  L_,   "A",  4,    0,         LONG_MAX,  0,      0,        256,        15  },
#endif
{  L_,   "",   5,    0,         0,         0,      0,        256,        72  },
{  L_,   "A",  5,    0,         0,         0,      0,        256,        136 },
{  L_,   "A",  5,    0,         1,         0,      0,        256,        34  },
{  L_,   "A",  5,    0,         INT_MAX,   0,      0,        256,        122 },
{  L_,   "A",  5,    0,         INT_MIN,   0,      0,        256,        50  },
{  L_,   "A",  5,    0,         LLONG_MAX, 0,      0,        256,        15  },
{  L_,   "A",  5,    0,         LLONG_MIN, 0,      0,        256,        10  },
{  L_,   "",   6,    0,         0,         "",     0,        256,        26  },
{  L_,   "A",  6,    0,         0,         "",     0,        256,        90  },
{  L_,   "A",  6,    0,         0,         "A",    0,        256,        154 },
{  L_,   "",   6,    0,         0,         "ABCD", 0,        256,        162 },
{  L_,   "A",  6,    0,         0,         "ABCD", 0,        256,        226 },
{  L_,   "",   7,    0,         0,         0,      0,        256,        72  },
{  L_,   "A",  7,    0,         0,         0,      0,        256,        136 },
{  L_,   "A",  7,    0,         0,         0,     VV,        256,        34 },
{  L_,   "",   0,    0,         0,         0,      0,      65536,      36086 },
{  L_,   "A",  0,    0,         0,         0,      0,      65536,      1846  },
{  L_,   "A",  0,    1,         0,         0,      0,      65536,      55843 },
{  L_,   "A",  0,    INT_MAX,   0,         0,      0,      65536,      4290  },
{  L_,   "A",  0,    INT_MIN,   0,         0,      0,      65536,      26706 },
#ifdef BSLS_PLATFORM_OS_WINDOWS
{  L_,   "",   1,    0,         0,         0,      0,      65536,      36086 },
{  L_,   "A",  1,    0,         0,         0,      0,      65536,      1846  },
{  L_,   "A",  1,    1,         0,         0,      0,      65536,      55843 },
{  L_,   "A",  1,    LONG_MAX,  0,         0,      0,      65536,      4290  },
{  L_,   "A",  1,    LONG_MIN,  0,         0,      0,      65536,      26706 },
#else
{  L_,   "",   1,    0,         0,         0,      0,      65536,      45128 },
{  L_,   "A",  1,    0,         0,         0,      0,      65536,      10888 },
{  L_,   "A",  1,    1,         0,         0,      0,      65536,      40738 },
{  L_,   "A",  1,    LONG_MAX,  0,         0,      0,      65536,      61711 },
{  L_,   "A",  1,    LONG_MIN,  0,         0,      0,      65536,      10506 },
#endif
{  L_,   "",   2,    0,         0,         0,      0,      65536,      45128 },
{  L_,   "A",  2,    0,         0,         0,      0,      65536,      10888 },
{  L_,   "A",  2,    1,         0,         0,      0,      65536,      40738 },
{  L_,   "A",  2,    INT_MAX,   0,         0,      0,      65536,      20346 },
{  L_,   "A",  2,    INT_MIN,   0,         0,      0,      65536,      17970 },
{  L_,   "A",  2,    LLONG_MAX, 0,         0,      0,      65536,      61711 },
{  L_,   "A",  2,    LLONG_MIN, 0,         0,      0,      65536,      10506 },
{  L_,   "",   3,    0,         0,         0,      0,      65536,      36086 },
{  L_,   "A",  3,    0,         0,         0,      0,      65536,      1846  },
{  L_,   "A",  3,    0,         1,         0,      0,      65536,      55843 },
{  L_,   "A",  3,    0,         INT_MAX,   0,      0,      65536,      4290  },
#ifdef BSLS_PLATFORM_OS_WINDOWS
{  L_,   "",   4,    0,         0,         0,      0,      65536,      36086 },
{  L_,   "A",  4,    0,         0,         0,      0,      65536,      1846  },
{  L_,   "A",  4,    0,         1,         0,      0,      65536,      55843 },
{  L_,   "A",  4,    0,         LONG_MAX,  0,      0,      65536,      4290  },
#else
{  L_,   "",   4,    0,         0,         0,      0,      65536,      45128 },
{  L_,   "A",  4,    0,         0,         0,      0,      65536,      10888 },
{  L_,   "A",  4,    0,         1,         0,      0,      65536,      40738 },
{  L_,   "A",  4,    0,         LONG_MAX,  0,      0,      65536,      61711 },
#endif
{  L_,   "",   5,    0,         0,         0,      0,      65536,      45128 },
{  L_,   "A",  5,    0,         0,         0,      0,      65536,      10888 },
{  L_,   "A",  5,    0,         1,         0,      0,      65536,      40738 },
{  L_,   "A",  5,    0,         INT_MAX,   0,      0,      65536,      20346 },
{  L_,   "A",  5,    0,         INT_MIN,   0,      0,      65536,      17970 },
{  L_,   "A",  5,    0,         LLONG_MAX, 0,      0,      65536,      61711 },
{  L_,   "A",  5,    0,         LLONG_MIN, 0,      0,      65536,      10506 },
{  L_,   "",   6,    0,         0,         "",     0,      65536,      41498 },
{  L_,   "A",  6,    0,         0,         "",     0,      65536,      7258  },
{  L_,   "A",  6,    0,         0,         "A",    0,      65536,      38554 },
{  L_,   "",   6,    0,         0,         "ABCD", 0,      65536,      52898 },
{  L_,   "A",  6,    0,         0,         "ABCD", 0,      65536,      18658 },
{  L_,   "",   7,    0,         0,         0,      0,      65535,      50129 },
{  L_,   "A",  7,    0,         0,         0,      0,      65535,      8746  },
{  L_,   "A",  7,    0,         0,         0,     VV,      65535,      44110 },
{  L_,   "",   0,    0,         0,         0,      0,          7,          1 },
{  L_,   "A",  0,    0,         0,         0,      0,          7,          4 },
{  L_,   "A",  0,    1,         0,         0,      0,          7,          0 },
{  L_,   "A",  0,    INT_MAX,   0,         0,      0,          7,          0 },
{  L_,   "A",  0,    INT_MIN,   0,         0,      0,          7,          5 },
#ifdef BSLS_PLATFORM_OS_WINDOWS
{  L_,   "",   1,    0,         0,         0,      0,          7,          1 },
{  L_,   "A",  1,    0,         0,         0,      0,          7,          4 },
{  L_,   "A",  1,    1,         0,         0,      0,          7,          0 },
{  L_,   "A",  1,    LONG_MAX,  0,         0,      0,          7,          0 },
{  L_,   "A",  1,    LONG_MIN,  0,         0,      0,          7,          5 },
#else
{  L_,   "",   1,    0,         0,         0,      0,          7,          5 },
{  L_,   "A",  1,    0,         0,         0,      0,          7,          5 },
{  L_,   "A",  1,    1,         0,         0,      0,          7,          1 },
{  L_,   "A",  1,    LONG_MAX,  0,         0,      0,          7,          6 },
{  L_,   "A",  1,    LONG_MIN,  0,         0,      0,          7,          0 },
#endif
{  L_,   "",   2,    0,         0,         0,      0,          7,          5 },
{  L_,   "A",  2,    0,         0,         0,      0,          7,          5 },
{  L_,   "A",  2,    1,         0,         0,      0,          7,          1 },
{  L_,   "A",  2,    INT_MAX,   0,         0,      0,          7,          6 },
{  L_,   "A",  2,    INT_MIN,   0,         0,      0,          7,          0 },
{  L_,   "A",  2,    LLONG_MAX, 0,         0,      0,          7,          6 },
{  L_,   "A",  2,    LLONG_MIN, 0,         0,      0,          7,          0 },
{  L_,   "",   3,    0,         0,         0,      0,          7,          1 },
{  L_,   "A",  3,    0,         0,         0,      0,          7,          4 },
{  L_,   "A",  3,    0,         1,         0,      0,          7,          0 },
{  L_,   "A",  3,    0,         INT_MAX,   0,      0,          7,          0 },
#ifdef BSLS_PLATFORM_OS_WINDOWS
{  L_,   "",   4,    0,         0,         0,      0,          7,          1 },
{  L_,   "A",  4,    0,         0,         0,      0,          7,          4 },
{  L_,   "A",  4,    0,         1,         0,      0,          7,          0 },
{  L_,   "A",  4,    0,         LONG_MAX,  0,      0,          7,          0 },
#else
{  L_,   "",   4,    0,         0,         0,      0,          7,          5 },
{  L_,   "A",  4,    0,         0,         0,      0,          7,          5 },
{  L_,   "A",  4,    0,         1,         0,      0,          7,          1 },
{  L_,   "A",  4,    0,         LONG_MAX,  0,      0,          7,          6 },
#endif
{  L_,   "",   5,    0,         0,         0,      0,          7,          5 },
{  L_,   "A",  5,    0,         0,         0,      0,          7,          5 },
{  L_,   "A",  5,    0,         1,         0,      0,          7,          1 },
{  L_,   "A",  5,    0,         INT_MAX,   0,      0,          7,          6 },
{  L_,   "A",  5,    0,         INT_MIN,   0,      0,          7,          0 },
{  L_,   "A",  5,    0,         LLONG_MAX, 0,      0,          7,          6 },
{  L_,   "A",  5,    0,         LLONG_MIN, 0,      0,          7,          0 },
{  L_,   "",   6,    0,         0,         "",     0,          7,          5 },
{  L_,   "A",  6,    0,         0,         "",     0,          7,          1 },
{  L_,   "A",  6,    0,         0,         "A",    0,          7,          4 },
{  L_,   "",   6,    0,         0,         "ABCD", 0,          7,          0 },
{  L_,   "A",  6,    0,         0,         "ABCD", 0,          7,          3 },
{  L_,   "",   7,    0,         0,         0,      0,          7,          5 },
{  L_,   "A",  7,    0,         0,         0,      0,          7,          5 },
{  L_,   "A",  7,    0,         0,         0,     VV,          7,          1 },
{  L_,   "",   0,    0,         0,         0,      0, 1610612741, 1185910006 },
{  L_,   "A",  0,    0,         0,         0,      0, 1610612741, 717686582  },
{  L_,   "A",  0,    1,         0,         0,      0, 1610612741, 1358289443 },
{  L_,   "A",  0,    INT_MAX,   0,         0,      0, 1610612741, 981602493  },
{  L_,   "A",  0,    INT_MIN,   0,         0,      0, 1610612741, 388327501  },
#ifdef BSLS_PLATFORM_OS_WINDOWS
{  L_,   "",   1,    0,         0,         0,      0, 1610612741, 1185910006 },
{  L_,   "A",  1,    0,         0,         0,      0, 1610612741, 717686582  },
{  L_,   "A",  1,    1,         0,         0,      0, 1610612741, 1358289443 },
{  L_,   "A",  1,    LONG_MAX,  0,         0,      0, 1610612741, 981602493  },
{  L_,   "A",  1,    LONG_MIN,  0,         0,      0, 1610612741, 388327501  },
#else
{  L_,   "",   1,    0,         0,         0,      0, 1610612741, 327790664  },
{  L_,   "A",  1,    0,         0,         0,      0, 1610612741, 933309054  },
{  L_,   "A",  1,    1,         0,         0,      0, 1610612741, 221028130  },
{  L_,   "A",  1,    INT_MAX,   0,         0,      0, 1610612741, 371216250  },
{  L_,   "A",  1,    INT_MIN,   0,         0,      0, 1610612741, 929711661  },
#endif
{  L_,   "",   2,    0,         0,         0,      0, 1610612741, 327790664  },
{  L_,   "A",  2,    0,         0,         0,      0, 1610612741, 933309054  },
{  L_,   "A",  2,    1,         0,         0,      0, 1610612741, 221028130  },
{  L_,   "A",  2,    INT_MAX,   0,         0,      0, 1610612741, 371216250  },
{  L_,   "A",  2,    INT_MIN,   0,         0,      0, 1610612741, 929711661  },
{  L_,   "A",  2,    LLONG_MAX, 0,         0,      0, 1610612741, 1138749706 },
{  L_,   "A",  2,    LLONG_MIN, 0,         0,      0, 1610612741, 60893445   },
{  L_,   "",   3,    0,         0,         0,      0, 1610612741, 1185910006 },
{  L_,   "A",  3,    0,         0,         0,      0, 1610612741, 717686582  },
{  L_,   "A",  3,    0,         1,         0,      0, 1610612741, 1358289443 },
{  L_,   "A",  3,    0,         INT_MAX,   0,      0, 1610612741, 981602493  },
#ifdef BSLS_PLATFORM_OS_WINDOWS
{  L_,   "",   4,    0,         0,         0,      0, 1610612741, 1185910006 },
{  L_,   "A",  4,    0,         0,         0,      0, 1610612741, 717686582  },
{  L_,   "A",  4,    0,         1,         0,      0, 1610612741, 1358289443 },
{  L_,   "A",  4,    0,         LONG_MAX,  0,      0, 1610612741, 981602493  },
#else
{  L_,   "",   4,    0,         0,         0,      0, 1610612741, 327790664  },
{  L_,   "A",  4,    0,         0,         0,      0, 1610612741, 933309054  },
{  L_,   "A",  4,    0,         1,         0,      0, 1610612741, 221028130  },
{  L_,   "A",  4,    0,         LONG_MAX,  0,      0, 1610612741, 1138749706 },
#endif
{  L_,   "",   5,    0,         0,         0,      0, 1610612741, 327790664  },
{  L_,   "A",  5,    0,         0,         0,      0, 1610612741, 933309054  },
{  L_,   "A",  5,    0,         1,         0,      0, 1610612741, 221028130  },
{  L_,   "A",  5,    0,         INT_MAX,   0,      0, 1610612741, 371216250  },
{  L_,   "A",  5,    0,         INT_MIN,   0,      0, 1610612741, 929711661  },
{  L_,   "A",  5,    0,         LLONG_MAX, 0,      0, 1610612741, 1138749706 },
{  L_,   "A",  5,    0,         LLONG_MIN, 0,      0, 1610612741, 60893445   },
{  L_,   "",   6,    0,         0,         "",     0, 1610612741, 445882901  },
{  L_,   "A",  6,    0,         0,         "",     0, 1610612741, 1588272218 },
{  L_,   "A",  6,    0,         0,         "A",    0, 1610612741, 1120048794 },
{  L_,   "",   6,    0,         0,         "ABCD", 0, 1610612741, 427544216  },
{  L_,   "A",  6,    0,         0,         "ABCD", 0, 1610612741, 1569933533 },
{  L_,   "",   7,    0,         0,         0,      0, 1610612741, 327790664  },
{  L_,   "A",  7,    0,         0,         0,      0, 1610612741, 933309054  },
{  L_,   "A",  7,    0,         0,         0,     VV, 1610612741, 221028130  }
};

enum { NUM_HASH_DATA = sizeof HASH_DATA / sizeof *HASH_DATA };

static const struct PrintData {
    int                 d_line;            // line number
    const char         *d_name;            // attribute name
    int                 d_type;            // type of attribute value
    int                 d_ivalue;          // integer attribute value
    unsigned long long  d_uivalue;         // unsigned integer attribute value
    const char         *d_svalue;          // string attribute value
    const void         *d_pvalue;          // const void * attribute value
    const char         *d_output;          // expected output format
} PRINT_DATA[] = {
    // line name type ivalue uivalue svalue pvalue     expected
    // ---- ---- ---- ------ ------- ------ ------     --------
    {  L_,  "",  0,   0,     0,      0,     0,         " [ \"\" = 0 ]"    },
    {  L_,  "",  1,   0,     0,      0,     0,         " [ \"\" = 0 ]"    },
    {  L_,  "",  2,   0,     0,      0,     0,         " [ \"\" = 0 ]"    },
    {  L_,  "",  3,   0,     0,      0,     0,         " [ \"\" = 0 ]"    },
    {  L_,  "",  4,   0,     0,      0,     0,         " [ \"\" = 0 ]"    },
    {  L_,  "",  5,   0,     0,      0,     0,         " [ \"\" = 0 ]"    },
    {  L_,  "",  6 ,  0,     0,      "0",   0,         " [ \"\" = 0 ]"    },
    {  L_,  "",  7 ,  0,     0,      0,     (void*)42, " [ \"\" = 0x2a ]" },
    {  L_,  "A", 0,   1,     0,      0,     0,         " [ \"A\" = 1 ]"   },
    {  L_,  "A", 6,   0,     0,      "1",   0,         " [ \"A\" = 1 ]"   },
};

const int NUM_PRINT_DATA = sizeof PRINT_DATA / sizeof *PRINT_DATA;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

template <class VALUE>
Value createValue(const VALUE *values, int size, int i)
    // Return an attribute created from value at the specified location 'i' in
    // the specified 'values' array of the specified 'size'.
{
    BSLS_ASSERT(0 <= i);
    BSLS_ASSERT(i <  size);  (void)size;

    Value variant;
    switch (values[i].d_type) {
       case 0: {
        variant.assign<int>(static_cast<int>(values[i].d_ivalue));
      } break;
      case 1: {
        variant.assign<long>(static_cast<long>(values[i].d_ivalue));
      } break;
      case 2: {
        variant.assign<long long>(values[i].d_ivalue);
      } break;
      case 3: {
        variant.assign<unsigned int>(
                               static_cast<unsigned int>(values[i].d_uivalue));
      } break;
      case 4: {
        variant.assign<unsigned long>(static_cast<unsigned long>(values[i].d_uivalue));
      } break;
      case 5: {
        variant.assign<unsigned long long>(values[i].d_uivalue);
      } break;
      case 6: {
        variant.assign<string>(values[i].d_svalue);
      } break;
      case 7: {
        variant.assign<const void *>(values[i].d_pvalue);
      } break;
      default: {
        BSLS_ASSERT_INVOKE_NORETURN("unreachable");
      }
    }
    return variant;
}

bool compareText(const bsl::string_view& lhs,
                 const bsl::string_view& rhs,
                 bsl::ostream&           errorStream = bsl::cout)
    // Return 'true' if the specified 'lhs' has the same value as the specified
    // 'rhs' and 'false' otherwise.  Optionally specify an 'errorStream', on
    // which, if 'lhs' and 'rhs' are not the same', a description of how the
    // two strings differ will be written.  If 'errorStream' is not supplied,
    // 'cout' will be used to report an error description.
{
    for (unsigned int i = 0; i < lhs.length() && i < rhs.length(); ++i) {
        if (lhs[i] != rhs[i]) {
            errorStream << "lhs: \"" << lhs << "\"\n"
                        << "rhs: \"" << rhs << "\"\n"
                        << "Strings differ at index (" << i << ") "
                        << "lhs[i] = " << lhs[i] << "(" << (int)lhs[i] << ") "
                        << "rhs[i] = " << rhs[i] << "(" << (int)rhs[i] << ")"
                        << endl;
            return false;                                             // RETURN
        }
    }

    if (lhs.length() < rhs.length()) {
        bsl::size_t i = lhs.length();
        errorStream << "lhs: \"" << lhs << "\"\n"
                    << "rhs: \"" << rhs << "\"\n"
                    << "Strings differ at index (" << i << ") "
                    << "lhs[i] = END-OF-STRING "
                    << "rhs[i] = " << rhs[i] << "(" << (int)rhs[i] << ")"
                    << endl;
        return false;                                                 // RETURN

    }
    if (lhs.length() > rhs.length()) {
        bsl::size_t i = rhs.length();
        errorStream << "lhs: \"" << lhs << "\"\n"
                    << "rhs: \"" << rhs << "\"\n"
                    << "Strings differ at index (" << i << ") "
                    << "lhs[i] = " << lhs[i] << "(" << (int)lhs[i] << ") "
                    << "rhs[i] = END-OF-STRING"
                    << endl;
        return false;                                                 // RETURN
    }
    return true;

}


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    bslma::TestAllocator testAllocator(veryVeryVeryVerbose);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    switch (test) { case 0:  // Zero is always the leading case.
      case 14: {
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

        if (verbose) cout << "\nUSAGE EXAMPLE"
                          << "\n=============" << endl;

///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Basic properties of ManagedAttribute
///- - - - - - - - - - - - - - - - - - - - - - - -
// This example create few 'ball::ManagedAttribute' object and show basic
// properties of those objects:
//..
    ball::ManagedAttribute p1("uuid", 4044457);
    ball::ManagedAttribute p2("name", "Bloomberg");

    ASSERT("uuid" == p1.key());
    ASSERT("name" == p2.key());

    ASSERT(true        == p1.value().is<int>());
    ASSERT(4044457     == p1.value().the<int>());
    ASSERT(true        == p2.value().is<bsl::string>());
    ASSERT("Bloomberg" == p2.value().the<bsl::string>());
//..
// Finaly, we show that 'ball::ManagedAttribute' manages the storage for the
// attribute name after construction:
//..
    char buffer[] = "Hello";
    ball::ManagedAttribute p3(buffer, 1);
    bsl::strcpy(buffer, "World");
    ASSERT("Hello" == p3.key());
//..

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING HASH FUNCTION
        //   Verify the hash of this object is the same as the hash of the
        //   contained attribute object.  Note that absolute value of the hash
        //   function is tested in the 'ball::Attribute' test driver.
        //
        // Concerns:
        //: 1 The hash value is correctly calculated for the contained
        //:   'ball::Attribute' object with the same value.
        //
        // Plan:
        //: 1  Specifying a set of test vectors and verify the return value.
        //
        // Testing:
        //   static int hash(const ManagedAttribute&, int size);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING HASH FUNCTION"
                          << "\n=====================" << endl;

        for (int i = 0; i < NUM_HASH_DATA; ++i) {
            int LINE = HASH_DATA[i].d_line;

            const Obj X(HASH_DATA[i].d_name,
                        createValue<HashData>(HASH_DATA, NUM_HASH_DATA, i));

            const ball::Attribute Y(HASH_DATA[i].d_name,
                                    createValue<HashData>(HASH_DATA,
                                                          NUM_HASH_DATA,
                                                          i));

            int hash = Obj::hash(X, HASH_DATA[i].d_hashSize);
            if (veryVerbose) {
                cout <<  X  << " ---> " << hash << endl;
            }
            ASSERTV(LINE,
                    hash == ball::Attribute::hash(Y, HASH_DATA[i].d_hashSize));
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING NAME/VALUE MANIPULATORS
        //   The 'setName' and 'setValue' method should set the corresponding
        //   fields correctly.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each pair (u, v) in the cross product
        //   S X S, construct u using the primary constructor, and then change
        //   its name and value using v's name and value.  Verify that two
        //   objects have the same value.
        //
        // Testing:
        //   void setName(const bsl::string_view& name);
        //   void setValue(int value);
        //   void setValue(long value);
        //   void setValue(long long value);
        //   void setValue(unsigned int value);
        //   void setValue(unsigned long value);
        //   void setValue(unsigned long long value);
        //   void setValue(const bsl::string_view& value);
        //   void setValue(const char* value);
        //   void setValue(const void* value);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING NAME/VALUE MANIPULATORS"
                          << "\n===============================" << endl;

        for (int i = 0; i < NUM_NAMES; ++i) {
        for (int j = 0; j < NUM_VALUES; ++j) {
            int LINE1 = NAMES[i].d_line;
            int LINE2 = VALUES[j].d_line;

            Value value1 = createValue<Values>(VALUES, NUM_VALUES, j);

            const Obj V(NAMES[i].d_name_p, value1);

            for (int k = 0; k < NUM_NAMES; ++k) {
            for (int l = 0; l < NUM_VALUES; ++l) {
                int LINE3 = NAMES[k].d_line;
                int LINE4 = VALUES[l].d_line;

                Value value2 = createValue<Values>(VALUES, NUM_VALUES, l);

                const Obj U(NAMES[k].d_name_p, value2);
                if (veryVerbose) {
                    cout << "\t";
                    P_(U);
                    P(V);
                }

                bool isSame = i == k && j == l;

                Obj mW1(V); const Obj& W1 = mW1;

                ASSERTV(LINE1, LINE2, LINE3, LINE4, W1 == V);
                ASSERTV(LINE1, LINE2, LINE3, LINE4, (W1 == U) == isSame);
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        Obj::hash(W1, 65536) == Obj::hash(V, 65536));

                ASSERTV(LINE1, LINE2, LINE3, LINE4, W1.value() == V.value());
                ASSERTV(LINE1, LINE2, LINE3, LINE4, W1.key() == V.key());

                mW1.setName(U.name());
                ASSERTV(LINE1, LINE2, LINE3, LINE4, W1.value() == V.value());
                ASSERTV(LINE1, LINE2, LINE3, LINE4, W1.key() == U.key());

                if (U.value().is<int>()) {
                    mW1.setValue(U.value().the<int>());
                }
                else if (U.value().is<long>()) {
                    mW1.setValue(U.value().the<long>());
                }
                else if (U.value().is<long long>()) {
                    mW1.setValue(U.value().the<long long>());
                }
                else if (U.value().is<unsigned int>()) {
                    mW1.setValue(U.value().the<unsigned int>());
                }
                else if (U.value().is<unsigned long>()) {
                    mW1.setValue(U.value().the<unsigned long>());
                }
                else if (U.value().is<unsigned long long>()) {
                    mW1.setValue(U.value().the<unsigned long long>());
                }
                else if (U.value().is<bsl::string>()) {
                    mW1.setValue(U.value().the<bsl::string>().c_str());
                    ASSERTV(LINE1, LINE2, LINE3, LINE4, W1 == U);
                    mW1.setValue(U.value().the<bsl::string>().c_str());
                }
                else if (U.value().is<const void *>()) {
                    mW1.setValue(U.value().the<const void *>());
                }

                ASSERTV(LINE1, LINE2, LINE3, LINE4, W1.value() == U.value());
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        0 == strcmp(W1.name(), U.name()));

                ASSERTV(LINE1, LINE2, LINE3, LINE4, W1 == U);
                ASSERTV(LINE1, LINE2, LINE3, LINE4, (W1 == V) == isSame);
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        Obj::hash(W1, 65536) == Obj::hash(U, 65536));

                Obj mW2(V); const Obj& W2 = mW2;

                ASSERTV(LINE1, LINE2, LINE3, LINE4, W2 == V);
                ASSERTV(LINE1, LINE2, LINE3, LINE4, (W2 == U) == isSame);
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        Obj::hash(W2, 65536) == Obj::hash(V, 65536));

                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        W2.value() == V.value());
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        0 == strcmp(W2.name(), V.name()));

                if (U.value().is<int>()) {
                    mW2.setValue(U.value().the<int>());
                }
                else if (U.value().is<long>()) {
                    mW2.setValue(U.value().the<long>());
                }
                else if (U.value().is<long long>()) {
                    mW2.setValue(U.value().the<long long>());
                }
                else if (U.value().is<unsigned int>()) {
                    mW2.setValue(U.value().the<unsigned int>());
                }
                else if (U.value().is<unsigned long>()) {
                    mW2.setValue(U.value().the<unsigned long>());
                }
                else if (U.value().is<unsigned long long>()) {
                    mW2.setValue(U.value().the<unsigned long long>());
                }
                else if (U.value().is<bsl::string>()) {
                    mW2.setValue(U.value().the<bsl::string>().c_str());
                    ASSERTV(LINE1, LINE2, LINE3, LINE4,
                            W2.value() == U.value());
                    mW2.setValue(U.value().the<bsl::string>());
                }
                else if (U.value().is<const void *>()) {
                    mW2.setValue(U.value().the<const void *>());
                }

                ASSERTV(LINE1, LINE2, LINE3, LINE4, W2.value() == U.value());
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        0 == strcmp(W2.name(), V.name()));

                mW2.setName(U.name());
                ASSERTV(LINE1, LINE2, LINE3, LINE4, W2.value() == U.value());
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        0 == strcmp(W2.name(), U.name()));

                ASSERTV(LINE1, LINE2, LINE3, LINE4, W2 == U);
                ASSERTV(LINE1, LINE2, LINE3, LINE4, (W2 == V) == isSame);
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        Obj::hash(W2, 65536) == Obj::hash(U, 65536));
            }
            }
        }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING NAME/VALUE MANIPULATORS
        //   The 'setName' and 'setValue' method should set the corresponding
        //   fields correctly.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each pair (u, v) in the cross product
        //   S X S, construct u using the primary constructor, and then change
        //   its name and value using v's name and value.  Verify that two
        //   objects have the same value.
        //
        //   We also in this test case verify that hash values are correct
        //   after objects have been modified.
        //
        // Testing:
        //   void setName(const bsl::string_view& name);
        //   void setValue(const Attribute::Value& value);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING NAME/VALUE MANIPULATORS"
                          << "\n===============================" << endl;

        for (int i = 0; i < NUM_NAMES; ++i) {
        for (int j = 0; j < NUM_VALUES; ++j) {
            int LINE1 = NAMES[i].d_line;
            int LINE2 = VALUES[j].d_line;

            Value value1 = createValue<Values>(VALUES, NUM_VALUES, j);

            const Obj V(NAMES[i].d_name_p, value1);

            for (int k = 0; k < NUM_NAMES; ++k) {
            for (int l = 0; l < NUM_VALUES; ++l) {
                int LINE3 = NAMES[k].d_line;
                int LINE4 = VALUES[l].d_line;

                Value value2 = createValue<Values>(VALUES, NUM_VALUES, l);

                const Obj U(NAMES[k].d_name_p, value2);
                if (veryVerbose) {
                    cout << "\t";
                    P_(U);
                    P(V);
                }

                bool isSame = i == k && j == l;

                Obj mW1(V); const Obj& W1 = mW1;

                ASSERTV(LINE1, LINE2, LINE3, LINE4, W1 == V);
                ASSERTV(LINE1, LINE2, LINE3, LINE4, (W1 == U) == isSame);
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        Obj::hash(W1, 65536) == Obj::hash(V, 65536));

                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        W1.value() == V.value());
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        0 == strcmp(W1.name(), V.name()));

                mW1.setName(U.name());
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        W1.value() == V.value());
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        0 == strcmp(W1.name(), U.name()));

                mW1.setValue(U.value());
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        W1.value() == U.value());
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        0 == strcmp(W1.name(), U.name()));

                ASSERTV(LINE1, LINE2, LINE3, LINE4, W1 == U);
                ASSERTV(LINE1, LINE2, LINE3, LINE4, (W1 == V) == isSame);
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        Obj::hash(W1, 65536) == Obj::hash(U, 65536));

                Obj mW2(V); const Obj& W2 = mW2;

                ASSERTV(LINE1, LINE2, LINE3, LINE4, W2 == V);
                ASSERTV(LINE1, LINE2, LINE3, LINE4, (W2 == U) == isSame);
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        Obj::hash(W2, 65536) == Obj::hash(V, 65536));

                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        W2.value() == V.value());
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        0 == strcmp(W2.name(), V.name()));

                mW2.setValue(U.value());
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        W2.value() == U.value());
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        0 == strcmp(W2.name(), V.name()));

                mW2.setName(U.name());
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        W2.value() == U.value());
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        0 == strcmp(W2.name(), U.name()));

                ASSERTV(LINE1, LINE2, LINE3, LINE4, W2 == U);
                ASSERTV(LINE1, LINE2, LINE3, LINE4, (W2 == V) == isSame);
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        Obj::hash(W2, 65536) == Obj::hash(U, 65536));
            }
            }
        }
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING NAME/VALUE CONSTRUCTORS
        //   The name/value constructors must initialize members correctly.
        //
        // Plan:
        //: 1 Specify a set S whose elements have substantial and varied
        //:   differences in value.
        //: 2 For each element in S, construct an object X using the primary
        //:   constructor, and another object Y using the corresponding
        //:   name/value constructor.
        //: 3 Use the equality operator to assert that both X and Y have the
        //:   same value as W.
        //
        // Testing:
        //   ManagedAttribute(const str_view& n, int v, alloc);
        //   ManagedAttribute(const str_view& n, long v, alloc);
        //   ManagedAttribute(const str_view& n, long long v, alloc);
        //   ManagedAttribute(const str_view& n, unsigned int v, alloc);
        //   ManagedAttribute(const str_view& n, unsigned long v, alloc);
        //   ManagedAttribute(const str_view& n, unsigned long long v, alloc);
        //   ManagedAttribute(const str_view& n, const string_view& v, alloc);
        //   ManagedAttribute(const str_view& n, const char *v, alloc);
        //   ManagedAttribute(const str_view& n, const void *v, alloc);
        //   ManagedAttribute(const Attribute&, alloc);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Name/Value Constructors"
                          << "\n===============================" << endl;

        for (int i = 0; i < NUM_NAMES; ++i) {
        for (int j = 0; j < NUM_VALUES; ++j) {
            int LINE1 = NAMES[i].d_line;
            int LINE2 = VALUES[j].d_line;

            const char *name = NAMES[i].d_name_p;
            Value value = createValue<Values>(VALUES, NUM_VALUES, j);

            const Obj X(name, value);

            switch (VALUES[j].d_type) {
              case 0: {
                  const Obj Y1(name, static_cast<int>(VALUES[j].d_ivalue));
                ASSERTV(LINE1, LINE2, Y1 == X);
                const Obj Y2(name,
                             static_cast<int>(VALUES[j].d_ivalue),
                             &testAllocator);
                ASSERTV(LINE1, LINE2, Y2 == X);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Obj Y3(name,
                                 static_cast<int>(VALUES[j].d_ivalue),
                                 &testAllocator);
                    ASSERTV(LINE1, LINE2, Y3 == X);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
              } break;
              case 1: {
                  const Obj Y1(name, static_cast<long>(VALUES[j].d_ivalue));
                ASSERTV(LINE1, LINE2, Y1 == X);
                const Obj Y2(name,
                             static_cast<long>(VALUES[j].d_ivalue),
                             &testAllocator);
                ASSERTV(LINE1, LINE2, Y2 == X);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Obj Y3(name,
                                 static_cast<long>(VALUES[j].d_ivalue),
                                 &testAllocator);
                    ASSERTV(LINE1, LINE2, Y3 == X);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
              } break;
              case 2: {
                  const Obj Y1(name, VALUES[j].d_ivalue);
                ASSERTV(LINE1, LINE2, Y1 == X);
                const Obj Y2(name,
                             VALUES[j].d_ivalue,
                             &testAllocator);
                ASSERTV(LINE1, LINE2, Y2 == X);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Obj Y3(name,
                                 VALUES[j].d_ivalue,
                                 &testAllocator);
                    ASSERTV(LINE1, LINE2, Y3 == X);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
              } break;
              case 3: {
                const Obj Y1(name,
                             static_cast<unsigned int>(VALUES[j].d_uivalue));
                ASSERTV(LINE1, LINE2, Y1 == X);
                const Obj Y2(name,
                             static_cast<unsigned int>(VALUES[j].d_uivalue),
                             &testAllocator);
                ASSERTV(LINE1, LINE2, Y2 == X);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Obj Y3(
                                name,
                                static_cast<unsigned int>(VALUES[j].d_uivalue),
                                &testAllocator);
                    ASSERTV(LINE1, LINE2, Y3 == X);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
              } break;
              case 4: {
                const Obj Y1(name,
                             static_cast<unsigned long>(VALUES[j].d_uivalue));
                ASSERTV(LINE1, LINE2, Y1 == X);
                const Obj Y2(name,
                             static_cast<unsigned long>(VALUES[j].d_uivalue),
                             &testAllocator);
                ASSERTV(LINE1, LINE2, Y2 == X);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Obj Y3(
                               name,
                               static_cast<unsigned long>(VALUES[j].d_uivalue),
                               &testAllocator);
                    ASSERTV(LINE1, LINE2, Y3 == X);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
              } break;
              case 5: {
                const Obj Y1(name, VALUES[j].d_uivalue);
                ASSERTV(LINE1, LINE2, Y1 == X);
                const Obj Y2(name,
                             VALUES[j].d_uivalue,
                             &testAllocator);
                ASSERTV(LINE1, LINE2, Y2 == X);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Obj Y3(name,
                                 VALUES[j].d_uivalue,
                                 &testAllocator);
                    ASSERTV(LINE1, LINE2, Y3 == X);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
              } break;
              case 6: {
                const Obj Y1(name, VALUES[j].d_svalue);
                ASSERTV(LINE1, LINE2, Y1 == X);
                const Obj Y2(name, VALUES[j].d_svalue, &testAllocator);
                ASSERTV(LINE1, LINE2, Y2 == X);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Obj Y3(name,
                                 VALUES[j].d_svalue,
                                 &testAllocator);
                    ASSERTV(LINE1, LINE2, Y3 == X);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
              } break;
              case 7: {
                const Obj Y1(name, VALUES[j].d_pvalue);
                ASSERTV(LINE1, LINE2, Y1 == X);
                const Obj Y2(name, VALUES[j].d_pvalue, &testAllocator);
                ASSERTV(LINE1, LINE2, Y2 == X);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Obj Y3(name,
                                 VALUES[j].d_pvalue,
                                 &testAllocator);
                    ASSERTV(LINE1, LINE2, Y3 == X);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
              } break;
            }

            // Testing const Attribute& constructor
            ball::Attribute mO(name, value); const ball::Attribute& O = mO;

            const Obj Y1(O);
            ASSERTV(LINE1, LINE2, Y1 == X);

            const Obj Y2(O, &testAllocator);
            ASSERTV(LINE1, LINE2, Y2 == X);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                const Obj Y3(O, &testAllocator);
                ASSERTV(LINE1, LINE2, Y3 == X);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //   Any value must be assignable to an object having any initial value
        //   without affecting the rhs operand value.  Also, any object must be
        //   assignable to itself.
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
        //   ManagedAttribute& operator=(const ManagedAttribute& rhs);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING ASSIGNMENT OPERATOR"
                          << "\n===========================" << endl;

        if (verbose) cout << "\nTesting assignment u = v.";

        for (int i = 0; i < NUM_NAMES; ++i) {
        for (int j = 0; j < NUM_VALUES; ++j) {
            int LINE1 = NAMES[i].d_line;
            int LINE2 = VALUES[j].d_line;

            Value value1 = createValue<Values>(VALUES, NUM_VALUES, j);

            const Obj V(NAMES[i].d_name_p, value1);

            for (int k = 0; k < NUM_NAMES; ++k) {
            for (int l = 0; l < NUM_VALUES; ++l) {
                int LINE3 = NAMES[k].d_line;
                int LINE4 = VALUES[l].d_line;

                  Value value2 = createValue<Values>(VALUES, NUM_VALUES, l);

                  Obj mU(NAMES[k].d_name_p, value2);  const Obj& U = mU;
                  if (veryVerbose) {
                      cout << "\t";
                      P_(U);
                      P(V);
                  }

                  const Obj W(V);  // control
                  mU = V;
                  ASSERTV(LINE1, LINE2, LINE3, LINE4, W == U);
                  ASSERTV(LINE1, LINE2, LINE3, LINE4, W == V);
            }
            }
        }
        }

        if (verbose) cout << "\nTesting assignment u = u (Aliasing).";

        for (int i = 0; i < NUM_NAMES; ++i) {
        for (int j = 0; j < NUM_VALUES; ++j) {
            int LINE1 = NAMES[i].d_line;
            int LINE2 = VALUES[j].d_line;
            Value value1 = createValue<Values>(VALUES, NUM_VALUES, j);

            Obj mU(NAMES[i].d_name_p, value1);  const Obj& U = mU;
            const Obj W(U);  // control
            mU = mU;

            if (veryVerbose) { T_; P_(U); P_(W); }
            ASSERTV(LINE1, LINE2, U == W);
        }
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING SECONDARY TEST APPARATUS:
        //   Void for 'ball::ManagedAttribute'.
        // --------------------------------------------------------------------

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //   Any value must be able to be copy constructed without affecting
        //   its argument.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identically valued objects w and x using tested
        //   methods.  Then copy construct and object y from x, and use the
        //   equality operator to assert that both x and y have the same value
        //   as w.
        //
        // Testing:
        //   ManagedAttribute(const ManagedAttribute&, alloc);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING COPY CONSTRUCTOR"
                          << "\n========================" << endl;

        for (int i = 0; i < NUM_NAMES; ++i) {
        for (int j = 0; j < NUM_VALUES; ++j) {
            int LINE1 = NAMES[i].d_line;
            int LINE2 = VALUES[j].d_line;

            const char *name = NAMES[i].d_name_p;
            Value value = createValue<Values>(VALUES, NUM_VALUES, j);

            const Obj W(name, value);  // control
            const Obj X(name, value);
            const Obj Y(X);

            if (veryVerbose) { T_; P_(W); P_(X); P(Y); }

            ASSERTV(LINE1, LINE2, X == W);
            ASSERTV(LINE1, LINE2, Y == W);
            ASSERTV(LINE1, LINE2, Y == X);
        }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        //   Any subtle variation in value must be detected by the equality
        //   operators.
        //
        // Plan:
        //   Specify a set S of unique names and a set T of unique value.
        //   Construct a set W of unique 'ball::ManagedAttribute' objects using
        //   every element of the cross product S X T.  Verify the correctness
        //   of 'operator==' and 'operator!=' for all elements (u, v) of the
        //   cross product W X W.  Next for each element in W, make a copy of
        //   of the attribute, and then use that copy along with the same
        //   attribute value to create another 'ball::ManagedAttribute' object
        //   to verify that is the same as that created directly from the
        //   name/value pair.
        //
        // Testing:
        //   operator==(const ManagedAttribute&, const ManagedAttribute&);
        //   operator!=(const ManagedAttribute&, const ManagedAttribute&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING EQUALITY OPERATORS"
                          << "\n==========================" << endl;

        if (verbose) cout <<
            "\nCompare each pair of values (u, v) in W X W." << endl;
        {
            for (int i = 0; i < NUM_NAMES; ++i) {
            for (int j = 0; j < NUM_VALUES; ++j) {
                int LINE1 = NAMES[i].d_line;
                int LINE2 = VALUES[j].d_line;

                Value value1 = createValue<Values>(VALUES, NUM_VALUES, j);

                const Obj X(NAMES[i].d_name_p, value1);

                for (int k = 0; k < NUM_NAMES; ++k) {
                for (int l = 0; l < NUM_VALUES; ++l) {
                    int LINE3 = NAMES[k].d_line;
                    int LINE4 = VALUES[l].d_line;

                    Value value2 = createValue<Values>(VALUES, NUM_VALUES, l);

                    const Obj Y(NAMES[k].d_name_p, value2);
                    if (veryVerbose) {
                        cout << "\t";
                        P_(X);
                        P(Y);
                    }

                    bool isSame = i == k && j == l;
                    ASSERTV(LINE1, LINE2, LINE3, LINE4,
                                 isSame == (X == Y));
                    ASSERTV(LINE1, LINE2, LINE3, LINE4,
                                 !isSame == (X != Y));

                }
                }
            }
            }
        }

        if (verbose) cout <<
            "\nVerify that attribute names are compared by content." << endl;
        {
            for (int i = 0; i < NUM_NAMES; ++i) {
            for (int j = 0; j < NUM_VALUES; ++j) {
                int LINE1 = NAMES[i].d_line;
                int LINE2 = VALUES[j].d_line;

                Value value1 = createValue<Values>(VALUES, NUM_VALUES, j);

                const Obj X(NAMES[i].d_name_p, value1);

                string name(NAMES[i].d_name_p);
                const Obj Y(name.c_str(), value1);
                ASSERTV(LINE1, LINE2, X == Y);
                ASSERTV(LINE1, LINE2, !(X != Y));
            }
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        //   The output operator is trivially implemented using the
        //   'bsl::ostream' output operators; a very few test vectors can
        //   sufficiently test wc this functionality.
        //
        // Plan:
        //   For each of a small representative set of object values, use
        //   'ostrstream' to write that object's value to a character buffer
        //   and then compare the contents of that buffer with the expected
        //   output format.
        //
        // Testing:
        //   bsl::ostream& operator<<(bsl::ostream&, const ManagedAttribute&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING OUTPUT (<<) OPERATOR"
                          << "\n============================" << endl;

        if (verbose) cout << "\nTesting 'operator<<' (ostream)." << endl;

        for (int i = 0; i < NUM_PRINT_DATA; ++i) {
            int LINE = PRINT_DATA[i].d_line;
            Value value = createValue<PrintData>(PRINT_DATA, NUM_PRINT_DATA, i);

            const Obj X(PRINT_DATA[i].d_name, value);

            ostringstream os;
            os << X;

            if (veryVerbose) {
                cout << "\t";
                P_(X);
                P_(PRINT_DATA[i].d_output);
                P(os.str());
            }
            ASSERTV(LINE, compareText(os.str(), PRINT_DATA[i].d_output));
        }

        if (verbose) cout << "\nTesting 'const void *'." << endl;

        {
            bsltf::SimpleTestType value;
            Obj                   mX("name", static_cast<const void *>(&value));

            ostringstream os;
            os << mX;

            ostringstream ptr;
            ptr << hex << showbase
                << reinterpret_cast<bsls::Types::UintPtr>(&value);

            char buf[256];
            snprintf(buf, 256, " [ \"name\" = %s ]", ptr.str().c_str());

            const bsl::string  EXPECTED(buf);
            const bsl::string& ACTUAL = os.str();

            if (veryVerbose) {
                cout << "\t";
                P_(mX);
                P_(ACTUAL);
                P(EXPECTED);
            }

            ASSERTV(EXPECTED, ACTUAL, EXPECTED == ACTUAL);
        }

        static const struct {
            int         d_line;            // line number
            const char *d_name_p;          // attribute name
            const char *d_svalue_p;        // string attribute value
            int         d_level;           // tab level
            int         d_spacesPerLevel;  // spaces per level
            const char *d_output;          // expected output format
        } PDATA[] = {
            // line name svalue level space expected
            // ---- ---- ------ ----- ----- -----------------------
            {  L_,  "A", "1",   0,    -1,   " [ \"A\" = 1 ]"       },
            {  L_,  "A", "1",   4,    1,    "     [ \"A\" = 1 ]\n" },
            {  L_,  "A", "1",   -1,   -2,   " [ \"A\" = 1 ]"       },
        };

        const int NUM_PDATA = sizeof PDATA / sizeof *PDATA;

        if (verbose) cout << "\nTesting 'print'." << endl;

        for (int i = 0; i < NUM_PDATA; ++i) {
            int   LINE  = PDATA[i].d_line;
            Value VALUE = Value(bsl::string(PDATA[i].d_svalue_p));

            const Obj X(PDATA[i].d_name_p, VALUE);

            ostringstream os;
            X.print(os, PDATA[i].d_level, PDATA[i].d_spacesPerLevel);

            if (veryVerbose) {
                cout << "\t";
                P_(X);
                P_(PDATA[i].d_output);
                P(os.str());
            }
            ASSERTV(LINE, compareText(os.str(), PDATA[i].d_output));
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //   We want to verify that each individual field is returned
        //   correctly.
        //
        // Plan:
        //   Specify a set S of unique names and a set T of unique value.
        //   Construct one ball::ManagedAttribute object for every element of
        //   the cross product S X T using the primary constructor.  Verify
        //   that each of the basic accessors returns the correct value.
        //
        // Testing:
        //   const char *name() const;
        //   const bsl::string& key() const;
        //   const Value& value() const;
        //   const Attribute& attribute() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING BASIC ACCESSORS"
                          << "\n=======================" << endl;

        for (int i = 0; i < NUM_NAMES; ++i) {
        for (int j = 0; j < NUM_VALUES; ++j) {
            int LINE1 = NAMES[i].d_line;
            int LINE2 = VALUES[j].d_line;

            Value value = createValue<Values>(VALUES, NUM_VALUES, j);

            const Obj X(NAMES[i].d_name_p, value);

            if (veryVerbose) {
                cout << "\t";
                P_(NAMES[i].d_name_p);
                P_(value);
                P(X);
            }
            ASSERTV(LINE1, LINE2, 0 == strcmp(X.name(), NAMES[i].d_name_p));
            ASSERTV(LINE1, LINE2, X.key() == NAMES[i].d_name_p);
            ASSERTV(LINE1, LINE2, X.value() == value);
            ASSERTV(LINE1, LINE2,
                    0 == strcmp(X.attribute().name(), NAMES[i].d_name_p));
            ASSERTV(LINE1, LINE2, X.attribute().value() == value);
        }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING TEST APPARATUS
        //   The 'createValue' method must correctly create a 'bdlb::Variant'
        //   object having the specified type and value.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in type and value.  For each element in S, construct
        //   a 'bdlb::Variant' object using the 'createValue' method, and
        //   verify that the resultant has the specified type and value.
        //
        // Testing:
        //   Value createValue(VALUE *values, int size, int i);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING TEST APPARATUS"
                          << "\n======================" << endl;

        if (verbose) cout << "\nTesting 'createValue'." << endl;

        for (int i = 0; i < NUM_VALUES; ++i) {
            int LINE = VALUES[i].d_line;

            Value value = createValue<Values>(VALUES, NUM_VALUES, i);

            if (veryVerbose) { cout << "\t"; P(value); }

            switch (VALUES[i].d_type) {
              case 0: {
                ASSERTV(LINE, value.is<int>());
                ASSERTV(LINE, VALUES[i].d_ivalue == value.the<int>());
              } break;
              case 1: {
                ASSERTV(LINE, value.is<long>());
                ASSERTV(LINE, VALUES[i].d_ivalue == value.the<long>());
              } break;
              case 2: {
                ASSERTV(LINE, value.is<long long>());
                ASSERTV(LINE, VALUES[i].d_ivalue == value.the<long long>());
              } break;
              case 3: {
                ASSERTV(LINE, value.is<unsigned int>());
                ASSERTV(LINE, VALUES[i].d_uivalue == value.the<unsigned int>());
              } break;
              case 4: {
                ASSERTV(LINE, value.is<unsigned long>());
                ASSERTV(LINE, VALUES[i].d_uivalue ==
                              value.the<unsigned long>());
              } break;
              case 5: {
                ASSERTV(LINE, value.is<unsigned long long>());
                ASSERTV(LINE, VALUES[i].d_uivalue ==
                              value.the<unsigned long long>());
              } break;
              case 6: {
                ASSERTV(LINE, value.is<string>());
                ASSERTV(LINE, VALUES[i].d_svalue == value.the<string>());
              } break;
              case 7: {
                ASSERTV(LINE, value.is<const void *>());
                ASSERTV(LINE, VALUES[i].d_pvalue == value.the<const void *>());
              } break;
              default:
                BSLS_ASSERT_INVOKE_NORETURN("unreachable");
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
        //   We want to verify the primary manipulators set the member fields
        //   correctly.
        //
        // Plan:
        //   Construct ball::ManagedAttribute objects with distinct name and
        //   value pairs, verify the values with the basic accessors, verify
        //   the equality and inequality of these objects.  The destructor is
        //   exercised as the objects being tested leave scope.
        //
        // Testing:
        //   ManagedAttribute(const str_view& n, const Value& v, alloc);
        //   ~ManagedAttribute();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING PRIMARY MANIPULATORS"
                          << "\n============================" << endl;

        if (verbose) cout << "\nTesting primary manipulators (thoroughly)."
                          << endl;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            Value mV;  const Value& V = mV;

            mV.assign<int>(VA_VALUE);
            const Obj X(VA_NAME, V);
            ASSERT(0        == strcmp(X.name(), VA_NAME));
            ASSERT(true     == X.value().is<int>());
            ASSERT(VA_VALUE == X.value().the<int>());

            mV.assign<int>(VB_VALUE);
            const Obj Y(VB_NAME, V);
            ASSERT(0        == strcmp(Y.name(), VB_NAME));
            ASSERT(true     == Y.value().is<int>());
            ASSERT(VB_VALUE == Y.value().the<int>());

            mV.assign<long long>(VC_VALUE);
            const Obj Z(VC_NAME, V);
            ASSERT(0        == strcmp(Z.name(), VC_NAME));
            ASSERT(true     == Z.value().is<long long>());
            ASSERT(VC_VALUE == Z.value().the<long long>());

            mV.assign<string>(VD_VALUE);
            const Obj W(VD_NAME, V);
            ASSERT(0        == strcmp(W.name(), VD_NAME));
            ASSERT(true     == W.value().is<string>());
            ASSERT(VD_VALUE == W.value().the<string>());

            mV.assign<unsigned long long>(VE_VALUE);
            const Obj U(VE_NAME, V);
            ASSERT(0        == strcmp(U.name(), VE_NAME));
            ASSERT(true     == U.value().is<unsigned long long>());
            ASSERT(VE_VALUE == U.value().the<unsigned long long>());

            mV.assign<const void *>(VF_VALUE);
            const Obj S(VF_NAME, V);
            ASSERT(0        == strcmp(S.name(), VF_NAME));
            ASSERT(true     == S.value().is<const void *>());
            ASSERT(VF_VALUE == S.value().the<const void *>());

            if (veryVerbose) {
                cout << "\t\t"; P_(X); P_(Y); P_(Z); P_(W); P_(U); P(S);
            }
        }

        if (verbose) cout << "\tPassing in an allocator." << endl;

        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            Value mV(&testAllocator);  const Value& V = mV;

            mV.assign<int>(VA_VALUE);
            const Obj X(VA_NAME, V, &testAllocator);
            ASSERT(0        == strcmp(X.name(), VA_NAME));
            ASSERT(true     == X.value().is<int>());
            ASSERT(VA_VALUE == X.value().the<int>());

            mV.assign<int>(VB_VALUE);
            const Obj Y(VB_NAME, V, &testAllocator);
            ASSERT(0        == strcmp(Y.name(), VB_NAME));
            ASSERT(true     == Y.value().is<int>());
            ASSERT(VB_VALUE == Y.value().the<int>());

            mV.assign<long long>(VC_VALUE);
            const Obj Z(VC_NAME, V, &testAllocator);
            ASSERT(0        == strcmp(Z.name(), VC_NAME));
            ASSERT(true     == Z.value().is<long long>());
            ASSERT(VC_VALUE == Z.value().the<long long>());

            mV.assign<string>(VD_VALUE);
            const Obj W(VD_NAME, V, &testAllocator);
            ASSERT(0        == strcmp(W.name(), VD_NAME));
            ASSERT(true     == W.value().is<string>());
            ASSERT(VD_VALUE == W.value().the<string>());

            mV.assign<unsigned long long>(VE_VALUE);
            const Obj U(VE_NAME, V, &testAllocator);
            ASSERT(0        == strcmp(U.name(), VE_NAME));
            ASSERT(true     == U.value().is<unsigned long long>());
            ASSERT(VE_VALUE == U.value().the<unsigned long long>());

            mV.assign<const void *>(VF_VALUE);
            const Obj S(VF_NAME, V);
            ASSERT(0        == strcmp(S.name(), VF_NAME));
            ASSERT(true     == S.value().is<const void *>());
            ASSERT(VF_VALUE == S.value().the<const void *>());

            if (veryVerbose) {
                cout << "\t\t"; P_(X); P_(Y); P_(Z); P_(W); P_(U); P(S);
            }
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            Value mV(&testAllocator);  const Value& V = mV;

            mV.assign<int>(VA_VALUE);
            const Obj X(VA_NAME, V, &testAllocator);
            ASSERT(0        == strcmp(X.name(), VA_NAME));
            ASSERT(true     == X.value().is<int>());
            ASSERT(VA_VALUE == X.value().the<int>());

            mV.assign<int>(VB_VALUE);
            const Obj Y(VB_NAME, V, &testAllocator);
            ASSERT(0        == strcmp(Y.name(), VB_NAME));
            ASSERT(true     == Y.value().is<int>());
            ASSERT(VB_VALUE == Y.value().the<int>());

            mV.assign<long long>(VC_VALUE);
            const Obj Z(VC_NAME, V, &testAllocator);
            ASSERT(0        == strcmp(Z.name(), VC_NAME));
            ASSERT(true     == Z.value().is<long long>());
            ASSERT(VC_VALUE == Z.value().the<long long>());

            mV.assign<string>(VD_VALUE);
            const Obj W(VD_NAME, V, &testAllocator);
            ASSERT(0        == strcmp(W.name(), VD_NAME));
            ASSERT(true     == W.value().is<string>());
            ASSERT(VD_VALUE == W.value().the<string>());

            mV.assign<unsigned long long>(VE_VALUE);
            const Obj U(VE_NAME, V, &testAllocator);
            ASSERT(0        == strcmp(U.name(), VE_NAME));
            ASSERT(true     == U.value().is<unsigned long long>());
            ASSERT(VE_VALUE == U.value().the<unsigned long long>());

            mV.assign<const void *>(VF_VALUE);
            const Obj S(VF_NAME, V, &testAllocator);
            ASSERT(0        == strcmp(S.name(), VF_NAME));
            ASSERT(true     == S.value().is<const void *>());
            ASSERT(VF_VALUE == S.value().the<const void *>());

            if (veryVerbose) {
                cout << "\t\t"; P_(X); P_(Y); P_(Z); P_(W); P_(U); P(S);
            }
          } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }

      } break;
      case 1: {
       // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Exercise a broad cross-section of value-semantic functionality
        //   before beginning testing in earnest.  Probe that functionality
        //   systematically and incrementally to discover basic errors in
        //   isolation.
        //
        // Plan:
        //   Create four test objects by using the initializing and copy
        //   constructors.  Exercise the basic value-semantic methods and the
        //   equality operators using these test objects.  Invoke the primary
        //   manipulator [3, 6], copy constructor [2, 8], and assignment
        //   operator without [9, 10] and with [11] aliasing.  Use the direct
        //   accessors to verify the expected results.  Display object values
        //   frequently in verbose mode.  Note that 'VA', 'VB', 'VC', and 'VD'
        //   denote unique, but otherwise arbitrary, object values.
        //
        // 1.  Create an object x1 using VA.        { x1:VA }
        // 2.  Create an object x2 (copy from x1).  { x1:VA x2:VA }
        // 3.  Set x1 to VB.                        { x1:VB x2:VA }
        // 4.  Set x2 to VB.                        { x1:VB x2:VB }
        // 5.  Set x2 to VC.                        { x1:VB x2:VC }
        // 6.  Set x1 to VA.                        { x1:VA x2:VC }
        // 7.  Create an object x3 (with value VD). { x1:VA x2:VC x3:VD }
        // 8.  Create an object x4 (copy from x1).  { x1:VA x2:VC x3:VD x4:VA }
        // 9.  Assign x2 = x1.                      { x1:VA x2:VA x3:VD x4:VA }
        // 10. Assign x2 = x3.                      { x1:VA x2:VD x3:VD x4:VA }
        // 11. Assign x1 = x1 (aliasing).           { x1:VA x2:VD x3:VD x4:VA }
        //
        // Testing:
        //   This Test Case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        if (verbose) cout << "\n 1. Create an object x1 using VA." << endl;
        Obj mX1(VA);  const Obj& X1 = mX1;
        ASSERT(0 == bsl::strcmp(VA_NAME, X1.name()));
        ASSERT(VA_VALUE == X1.value().the<int>());

        if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                          << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        ASSERT(0 == bsl::strcmp(VA_NAME, X2.name()));
        ASSERT(VA_VALUE == X2.value().the<int>());

        if (verbose) cout << "\n 3. Set x1 to VB." << endl;
        mX1.setName(VB_NAME);
        mX1.setValue(Value(VB_VALUE));
        ASSERT(0 == bsl::strcmp(VB_NAME, X1.name()));
        ASSERT(VB_VALUE == X1.value().the<int>());
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        if (verbose) cout << "\n 4. Set x2 to VB." << endl;
        mX2.setName(VB_NAME);
        mX2.setValue(Value(VB_VALUE));
        ASSERT(0 == bsl::strcmp(VB_NAME, X2.name()));
        ASSERT(VB_VALUE == X2.value().the<int>());
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X1 == X2));        ASSERT(0 == (X1 != X2));

        if (verbose) cout << "\n 5. Set x2 to VC." << endl;
        mX2.setName(VC_NAME);
        mX2.setValue(Value(VC_VALUE));
        ASSERT(0 == bsl::strcmp(VC_NAME, X2.name()));
        ASSERT(VC_VALUE == X2.value().the<long long>());
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        if (verbose) cout << "\n 6. Set x1 to VA." << endl;
        mX1.setName(VA_NAME);
        mX1.setValue(Value(VA_VALUE));
        ASSERT(0 == bsl::strcmp(VA_NAME, X1.name()));
        ASSERT(VA_VALUE == X1.value().the<int>());
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        if (verbose) cout << "\n 7. Create an object x3 (with value VD)."
                           << endl;
        Obj mX3(VD);  const Obj& X3 = mX3;
        ASSERT(0 == bsl::strcmp(VD_NAME, X3.name()));
        ASSERT(VD_VALUE == X3.value().the<string>());
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));

        if (verbose) cout << "\n 8. Create an object x4 (copy from x1)."
                           << endl;
        Obj mX4(X1);  const Obj& X4 = mX4;
        ASSERT(0 == bsl::strcmp(VA_NAME, X4.name()));
        ASSERT(VA_VALUE == X4.value().the<int>());
        ASSERT(1 == (X4 == X1));        ASSERT(0 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(0 == (X4 == X3));        ASSERT(1 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        if (verbose) cout << "\n 9. Assign x2 = x1." << endl;
        mX2 = X1;
        ASSERT(0 == bsl::strcmp(VA_NAME, X2.name()));
        ASSERT(VA_VALUE == X2.value().the<int>());
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(1 == (X2 == X4));        ASSERT(0 == (X2 != X4));

        if (verbose) cout << "\n 10. Assign x2 = x3." << endl;
        mX2 = X3;
        ASSERT(0 == bsl::strcmp(VD_NAME, X2.name()));
        ASSERT(VD_VALUE == X2.value().the<string>());
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        if (verbose) cout << "\n 11. Assign x1 = x1 (aliasing)." << endl;
        mX1 = X1;
        ASSERT(0 == bsl::strcmp(VA_NAME, X1.name()));
        ASSERT(VA_VALUE == X1.value().the<int>());
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(1 == (X1 == X4));        ASSERT(0 == (X1 != X4));
      } break;

      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      } break;
    }
    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
