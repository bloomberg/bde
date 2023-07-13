// ball_attribute.t.cpp                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_attribute.h>

#include <bdlb_hashutil.h>

#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bslma_usesbslmaallocator.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslim_testutil.h>
#include <bsls_platform.h>

#include <bslma_default.h>

#include <bslim_printer.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_types.h>

#include <bsltf_simpletesttype.h>

#include <bsl_climits.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <bsl_c_time.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is an in-core value-semantic component without the
// support for 'bdex' stream-in operation.  We choose the constructor that
// takes a literal name and a 'bdlb_variant' value as the primary manipulator,
// and use the 'createValue' method as the primitive test apparatus.  The
// 10-step standard test procedure is then performed.  We will also verify that
// the hash values must be calculated correctly and must be re-calculated after
// the objects have been modified.
//-----------------------------------------------------------------------------
// [14] static int hash(const Attribute&, int size);
// [11] Attribute(const char *n, int v, const alct& a);
// [11] Attribute(const char *n, long v, const alct& a);
// [11] Attribute(const char *n, long long, const alct& a);
// [11] Attribute(const char *n, unsigned int v, const alct& a);
// [11] Attribute(const char *n, unsigned long v, const alct& a);
// [11] Attribute(const char *n, unsigned long long, const alct& a);
// [11] Attribute(const char *n, const str_view& v, const alct& a);
// [11] Attribute(const char *n, const char *v, const alct& a);
// [11] Attribute(const char *n, const void *v, const alct& a);
// [ 2] Attribute(const char *n, const VALUE& v, const alct& a);
// [ 7] Attribute(const Attribute&, const alct& a);
// [ 2] ~Attribute();
// [ 9] Attribute& operator=(const Attribute& rhs);
// [12] void setName(const char *n);
// [12] void setValue(const Value& value);
// [13] void setValue(int n);
// [13] void setValue(long n);
// [13] void setValue(long long n);
// [13] void setValue(unsigned int n);
// [13] void setValue(unsigned long n);
// [13] void setValue(unsigned long long n);
// [13] void setValue(const bsl::string_view& s);
// [13] void setValue(const char *v);
// [13] void setValue(const void *v);
// [ 4] const char *name() const;
// [ 4] const VALUE& value() const;
// [ 5] bsl::ostream& print(bsl::ostream& stream, int lvl, int spl) const;
// [ 6] operator==(const ball::Attribute&, const ball::Attribute&);
// [ 6] operator!=(const ball::Attribute&, const ball::Attribute&);
// [ 5] bsl::ostream& operator<<(bsl::ostream&, const ball::Attribute&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] PRIMITIVE TEST APPARATUS
// [ 8] UNUSED
// [10] UNUSED
// [15] PERFORMANCE TEST
// [16] USAGE EXAMPLE

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

typedef ball::Attribute     Obj;
typedef bsls::Types::Int64  Int64;

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
#define VE_VALUE  4242424242ULL
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


const struct {
    int         d_line;     // line number
    const char *d_name;     // attribute name
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

// Select hash value for bitness dependent types.
#define LONG_HASH(a,b) ((sizeof(int)==sizeof(long))?(a):(b))
#define ULONG_HASH(a,b) ((sizeof(unsigned int)==sizeof(unsigned long))?(a):(b))
#define PTR_HASH(a,b) ((sizeof(unsigned int)==sizeof(const void*))?(a):(b))

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

{  L_,   "",   1,    0,         0,         0,      0,        256,
                                                           LONG_HASH(246,72) },
{  L_,   "A",  1,    0,         0,         0,      0,        256,
                                                           LONG_HASH(54,136) },
{  L_,   "A",  1,    1,         0,         0,      0,        256,
                                                           LONG_HASH(35,34)  },
{  L_,   "A",  1,    LONG_MAX,  0,         0,      0,        256,
                                                           LONG_HASH(194,15) },
{  L_,   "A",  1,    LONG_MIN,  0,         0,      0,        256,
                                                           LONG_HASH(82,10)  },
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
{  L_,   "",   4,    0,         0,         0,      0,        256,
                                                          ULONG_HASH(246,72) },
{  L_,   "A",  4,    0,         0,         0,      0,        256,
                                                          ULONG_HASH(54,136) },
{  L_,   "A",  4,    0,         1,         0,      0,        256,
                                                          ULONG_HASH(35,34)  },
{  L_,   "A",  4,    0,         LONG_MAX,  0,      0,        256,
                                                          ULONG_HASH(194,15) },
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
{  L_,   "",   7,    0,         0,         0,      0,        256,
                                                            PTR_HASH(246,72) },
{  L_,   "A",  7,    0,         0,         0,      0,        256,
                                                            PTR_HASH(54,136) },
{  L_,   "A",  7,    0,         0,         0,     VV,        256,
                                                            PTR_HASH(35, 34) },
{  L_,   "",   0,    0,         0,         0,      0,      65536,      36086 },
{  L_,   "A",  0,    0,         0,         0,      0,      65536,      1846  },
{  L_,   "A",  0,    1,         0,         0,      0,      65536,      55843 },
{  L_,   "A",  0,    INT_MAX,   0,         0,      0,      65536,      4290  },
{  L_,   "A",  0,    INT_MIN,   0,         0,      0,      65536,      26706 },
{  L_,   "",   1,    0,         0,         0,      0,      65536,
                                                     LONG_HASH(36086, 45128) },
{  L_,   "A",  1,    0,         0,         0,      0,      65536,
                                                     LONG_HASH(1846,  10888) },
{  L_,   "A",  1,    1,         0,         0,      0,      65536,
                                                     LONG_HASH(55843, 40738) },
{  L_,   "A",  1,    LONG_MAX,  0,         0,      0,      65536,
                                                     LONG_HASH(4290,  61711) },
{  L_,   "A",  1,    LONG_MIN,  0,         0,      0,      65536,
                                                     LONG_HASH(26706, 10506) },
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
{  L_,   "",   4,    0,         0,         0,      0,      65536,
                                                    ULONG_HASH(36086, 45128) },
{  L_,   "A",  4,    0,         0,         0,      0,      65536,
                                                    ULONG_HASH(1846,  10888) },
{  L_,   "A",  4,    0,         1,         0,      0,      65536,
                                                    ULONG_HASH(55843, 40738) },
{  L_,   "A",  4,    0,         LONG_MAX,  0,      0,      65536,
                                                    ULONG_HASH(4290,  61711) },
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
{  L_,   "",   7,    0,         0,         0,      0,      65535,
                                                      PTR_HASH(54181, 50129) },
{  L_,   "A",  7,    0,         0,         0,      0,      65535,
                                                      PTR_HASH(12797,  8746) },
{  L_,   "A",  7,    0,         0,         0,     VV,      65535,
                                                      PTR_HASH(11033, 44110) },
{  L_,   "",   0,    0,         0,         0,      0,          7,          1 },
{  L_,   "A",  0,    0,         0,         0,      0,          7,          4 },
{  L_,   "A",  0,    1,         0,         0,      0,          7,          0 },
{  L_,   "A",  0,    INT_MAX,   0,         0,      0,          7,          0 },
{  L_,   "A",  0,    INT_MIN,   0,         0,      0,          7,          5 },
{  L_,   "",   1,    0,         0,         0,      0,          7,
                                                             LONG_HASH(1, 5) },
{  L_,   "A",  1,    0,         0,         0,      0,          7,
                                                             LONG_HASH(4, 5) },
{  L_,   "A",  1,    1,         0,         0,      0,          7,
                                                             LONG_HASH(0, 1) },
{  L_,   "A",  1,    LONG_MAX,  0,         0,      0,          7,
                                                             LONG_HASH(0, 6) },
{  L_,   "A",  1,    LONG_MIN,  0,         0,      0,          7,
                                                             LONG_HASH(5, 0) },
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
{  L_,   "",   4,    0,         0,         0,      0,          7,
                                                            ULONG_HASH(1, 5) },
{  L_,   "A",  4,    0,         0,         0,      0,          7,
                                                            ULONG_HASH(4, 5) },
{  L_,   "A",  4,    0,         1,         0,      0,          7,
                                                            ULONG_HASH(0, 1) },
{  L_,   "A",  4,    0,         LONG_MAX,  0,      0,          7,
                                                            ULONG_HASH(0, 6) },
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
{  L_,   "",   7,    0,         0,         0,      0,          7,
                                                              PTR_HASH(1, 5) },
{  L_,   "A",  7,    0,         0,         0,      0,          7,
                                                              PTR_HASH(4, 5) },
{  L_,   "A",  7,    0,         0,         0,     VV,          7,
                                                              PTR_HASH(0, 1) },
{  L_,   "",   0,    0,         0,         0,      0, 1610612741, 1185910006 },
{  L_,   "A",  0,    0,         0,         0,      0, 1610612741, 717686582  },
{  L_,   "A",  0,    1,         0,         0,      0, 1610612741, 1358289443 },
{  L_,   "A",  0,    INT_MAX,   0,         0,      0, 1610612741, 981602493  },
{  L_,   "A",  0,    INT_MIN,   0,         0,      0, 1610612741, 388327501  },
{  L_,   "",   1,    0,         0,         0,      0, 1610612741,
                                            LONG_HASH(1185910006, 327790664) },
{  L_,   "A",  1,    0,         0,         0,      0, 1610612741,
                                            LONG_HASH(717686582,  933309054) },
{  L_,   "A",  1,    1,         0,         0,      0, 1610612741,
                                            LONG_HASH(1358289443, 221028130) },
{  L_,   "A",  1,    LONG_MAX,  0,         0,      0, 1610612741,
                                            LONG_HASH(981602493,  1138749706) },
{  L_,   "A",  1,    LONG_MIN,  0,         0,      0, 1610612741,
                                            LONG_HASH(388327501,  60893445) },
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
{  L_,   "",   4,    0,         0,         0,      0, 1610612741,
                                           ULONG_HASH(1185910006, 327790664) },
{  L_,   "A",  4,    0,         0,         0,      0, 1610612741,
                                           ULONG_HASH(717686582,  933309054) },
{  L_,   "A",  4,    0,         1,         0,      0, 1610612741,
                                           ULONG_HASH(1358289443, 221028130) },
{  L_,   "A",  4,    0,         LONG_MAX,  0,      0, 1610612741,
                                           ULONG_HASH(981602493, 1138749706) },
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
{  L_,   "",   7,    0,         0,         0,      0, 1610612741,
                                             PTR_HASH(1185910006, 327790664) },
{  L_,   "A",  7,    0,         0,         0,      0, 1610612741,
                                             PTR_HASH( 717686582, 933309054) },
{  L_,   "A",  7,    0,         0,         0,     VV, 1610612741,
                                             PTR_HASH(1358289443, 221028130) }
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

enum { NUM_PRINT_DATA = sizeof PRINT_DATA / sizeof *PRINT_DATA };


//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

template <class VALUE>
Obj::Value createValue(const VALUE *values, int size, int i)
    // Return an attribute created from value at the specified location 'i' in
    // the specified 'values' array of the specified 'size'.
{
    BSLS_ASSERT(0 <= i);
    BSLS_ASSERT(i <  size);  (void)size;

    Obj::Value variant;
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
        variant.assign<unsigned long>(
                              static_cast<unsigned long>(values[i].d_uivalue));
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
    // 'rhs' and 'false' otherwise.  Optionally specify a 'errorStream', on
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
//                  CLASS DEFINITIONS FOR TEST CASE 15
//-----------------------------------------------------------------------------

class MyAttributeValue {
  public:
    // TYPES
    enum ValueType {
        k_INT32 = 0,
        k_LONG_LONG,
        k_STRING,
        k_UNSIGNED_LONG_LONG
    };

  private:
    // DATA
    ValueType                               d_type;
    bslma::Allocator                       *d_allocator_p;
    union {
        bsls::AlignmentUtil::MaxAlignedType d_align;
        int                                 d_int32Value;
        long long                           d_llValue;
        char                                d_stringValue[sizeof(bsl::string)];
        unsigned long long                  d_ullValue;
    };

  public:
    BSLMF_NESTED_TRAIT_DECLARATION(MyAttributeValue,
                                   bslma::UsesBslmaAllocator);

    MyAttributeValue(int value, bslma::Allocator *basicAllocator = 0)
    : d_type(k_INT32)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        d_int32Value = value;
    }

    MyAttributeValue(long long         value,
                     bslma::Allocator *basicAllocator = 0)
    : d_type(k_LONG_LONG)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        d_llValue = value;
    }

    MyAttributeValue(unsigned long long  value,
                     bslma::Allocator   *basicAllocator = 0)
    : d_type(k_UNSIGNED_LONG_LONG)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        d_ullValue = value;
    }

    MyAttributeValue(const char *value, bslma::Allocator *basicAllocator = 0)
    : d_type(k_STRING)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        new (d_stringValue) bsl::string(value, basicAllocator);
    }

    MyAttributeValue(const MyAttributeValue&  rhs,
                     bslma::Allocator        *basicAllocator = 0)
    : d_type(rhs.d_type)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        switch (d_type) {
          case k_INT32: {
            d_int32Value = rhs.d_int32Value;
          } break;
          case k_LONG_LONG: {
            d_llValue = rhs.d_llValue;
          } break;
          case k_STRING: {
            const bsl::string *string_p = static_cast<const bsl::string *>(
                            reinterpret_cast<const void *>(rhs.d_stringValue));
            new (d_stringValue) bsl::string(*string_p, d_allocator_p);
          } break;
          case k_UNSIGNED_LONG_LONG: {
            d_ullValue = rhs.d_ullValue;
          } break;
          default: {
              BSLS_ASSERT_INVOKE_NORETURN("unreachable");
          }
        }
    }

    ~MyAttributeValue()
    {
        using bsl::string;
        if (k_STRING == d_type) {
            static_cast<bsl::string *>(
                           reinterpret_cast<void *>(d_stringValue))->~string();
        }
    }

    MyAttributeValue& operator=(const MyAttributeValue& rhs)
    {
        if (this != &rhs && d_type == rhs.d_type) {
            switch (d_type) {
              case k_INT32: {
                d_int32Value = rhs.d_int32Value;
              } break;
              case k_LONG_LONG: {
                d_llValue = rhs.d_llValue;
              } break;
              case k_STRING: {
                bsl::string *string_p = static_cast<bsl::string *>(
                                      reinterpret_cast<void *>(d_stringValue));
                *string_p = *static_cast<const bsl::string *>(
                            reinterpret_cast<const void *>(rhs.d_stringValue));
              } break;
              case k_UNSIGNED_LONG_LONG: {
                d_ullValue = rhs.d_ullValue;
              } break;
            default: {
              BSLS_ASSERT_INVOKE_NORETURN("unreachable");
            }
          }
        }
        return *this;
    }

    ValueType type() const
    {
        return d_type;
    }

    int int32Value() const
    {
        return d_int32Value;
    }

    long long llValue() const
    {
        return d_llValue;
    }

    unsigned long long ullValue() const
    {
        return d_ullValue;
    }

    const char *stringValue() const
    {
        return static_cast<const bsl::string *>(
                       reinterpret_cast<const void *>(d_stringValue))->c_str();
    }

    bool operator==(const MyAttributeValue& rhs) const
    {
        if (d_type != rhs.d_type) {
            return false;                                             // RETURN
        }
        switch (d_type) {
          case k_INT32: {
            return d_int32Value == rhs.d_int32Value;                  // RETURN
          }
          case k_LONG_LONG: {
            return d_llValue == rhs.d_llValue;                        // RETURN
          }
          case k_STRING: {
            return *static_cast<const bsl::string *>(
                                 reinterpret_cast<const void *>(d_stringValue))
                == *static_cast<const bsl::string *>(
                            reinterpret_cast<const void *>(rhs.d_stringValue));
                                                                      // RETURN
          }
          case k_UNSIGNED_LONG_LONG: {
            return d_ullValue == rhs.d_ullValue;                      // RETURN
          }
        }
        return false;
    }

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const
    {
        (void)level;
        (void)spacesPerLevel;

        switch (d_type) {
          case k_INT32: {
            stream << d_int32Value;
          } break;
          case k_LONG_LONG: {
            stream << d_llValue;
          } break;
          case k_STRING: {
            stream << *static_cast<const bsl::string *>(
                                reinterpret_cast<const void *>(d_stringValue));
          } break;
          case k_UNSIGNED_LONG_LONG: {
            stream << d_ullValue;
          } break;
        }
        return stream;
    }
};

class MyAttribute {
    // A 'MyAttribute' object contains an attribute name which is not managed
    // and an attribute value which is managed.

  public:
    // TYPES
    typedef MyAttributeValue Value;

  private:
    const char  *d_name;       // attribute name

    Value        d_value;      // attribute value

    mutable int  d_hashValue;  // hash value (-1 means unset)

    mutable int  d_hashSize;   // hash size from which the hash value was
                               // calculated

    // FRIEND FREE OPERATORS
    friend bool operator==(const MyAttribute& lhs,
                           const MyAttribute& rhs);

    friend bool operator!=(const MyAttribute& lhs,
                           const MyAttribute& rhs);

    friend bsl::ostream& operator<<(bsl::ostream&      output,
                                    const MyAttribute& attribute);

  public:
    BSLMF_NESTED_TRAIT_DECLARATION(MyAttribute, bslma::UsesBslmaAllocator);

    // CLASS METHODS
    static int hash(const MyAttribute& attribute, int size);
        // Return a hash value calculated from the specified 'attribute' using
        // the specified 'size' as the number of slots.  The hash value is
        // guaranteed to be in the range [0 ..size - 1].  The behavior is
        // undefined unless '0 <= size'.

    // CREATORS
    MyAttribute(const char       *name,
                int               value,
                bslma::Allocator *basicAllocator = 0 );
        // Create a 'MyAttribute' object having the specified (literal) 'name'
        // and (32-bit integer) 'value'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator will be used.  Note that 'name' is not
        // managed by this object and therefore must remain valid while in use
        // by any 'MyAttribute' object.

    MyAttribute(const char       *name,
                long long         value,
                bslma::Allocator *basicAllocator = 0 );
        // Create a 'MyAttribute' object having the specified (literal) 'name'
        // and (64-bit integer) 'value'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator will be used.  Note that 'name' is not
        // managed by this object and therefore must remain valid while in use
        // by any 'MyAttribute' object.

    MyAttribute(const char         *name,
                unsigned long long  value,
                bslma::Allocator   *basicAllocator = 0 );
        // Create a 'MyAttribute' object having the specified (literal) 'name'
        // and (unsigned 64-bit integer) 'value'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator will be used.  Note that
        // 'name' is not managed by this object and therefore must remain valid
        // while in use by any 'MyAttribute' object.

    MyAttribute(const char       *name,
                const char       *value,
                bslma::Allocator *basicAllocator = 0 );
        // Create a 'MyAttribute' object having the specified (literal) 'name'
        // and (character string)'value'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator will be used.  Note that
        // 'name' is not managed by this object and therefore must remain valid
        // while in use by any 'MyAttribute' object.

    MyAttribute(const char       *name,
                const Value&      value,
                bslma::Allocator *basicAllocator = 0 );
        // Create a 'MyAttribute' object having the specified (literal) 'name'
        // and 'value'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator will be used.  Note that 'name' is not managed by this
        // object and therefore must remain valid while in use by any
        // 'MyAttribute' object.

    MyAttribute(const MyAttribute&  original,
                bslma::Allocator   *basicAllocator = 0);
        // Create a 'MyAttribute' object having the same (literal) name and
        // attribute value as the specified 'original' object.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator
        // will be used.

    ~MyAttribute();
        // Destroy this attribute object.

    // MANIPULATORS
    MyAttribute& operator=(const MyAttribute& rhs);
        // Assign the value of the specified 'rhs' object to this object.

    void setName(const char *name);
        // Set the attribute name of this object to the specified (literal)
        // 'name'.  Note that 'name' is not managed by this object and
        // therefore must remain valid while in use by any 'MyAttribute'
        // object.

    void setValue(const Value& value);
        // Set the attribute value of this object to the specified 'value'.

    // ACCESSORS
    const char *name() const;
        // Return the name of this object.

    const Value& value() const;
        // Return a reference to the non-modifiable attribute value of this
        // object.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // (absolute value of) the optionally specified indentation 'level'
        // and return a reference to 'stream'.  If 'level' is specified,
        // optionally specify 'spacesPerLevel', the number of spaces per
        // indentation level for this and all of its nested objects.  If
        // 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, format the entire output on one line,
        // suppressing all but the initial indentation (as governed by
        // 'level').  If 'stream' is not valid on entry, this operation has no
        // effect.
};

// FREE OPERATORS
bool operator==(const MyAttribute& lhs,
                const MyAttribute& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'MyAttribute' objects have the same
    // value if they have the same name (but not necessarily the identical
    // representation in memory), the same attribute value type, and the same
    // attribute value.

bool operator!=(const MyAttribute& lhs,
                const MyAttribute& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'MyAttribute' objects do not
    // have the same value if any of their respective names (value, not
    // address), attribute value types, or attribute values differ.

bsl::ostream& operator<<(bsl::ostream&      output,
                         const MyAttribute& attribute);
    // Write the value of the specified 'attribute' to the specified 'output'
    // stream.  Return the specified 'output' stream.

// CREATORS
inline
MyAttribute::MyAttribute(const char       *name,
                         int               value,
                         bslma::Allocator *basicAllocator)
: d_name(name)
, d_value(value, basicAllocator)
, d_hashValue(-1)
{
}

inline
MyAttribute::MyAttribute(const char       *name,
                         long long         value,
                         bslma::Allocator *basicAllocator)
: d_name(name)
, d_value(value, basicAllocator)
, d_hashValue(-1)
{
}

inline
MyAttribute::MyAttribute(const char         *name,
                         unsigned long long  value,
                         bslma::Allocator   *basicAllocator)
: d_name(name)
, d_value(value, basicAllocator)
, d_hashValue(-1)
{
}

inline
MyAttribute::MyAttribute(const char       *name,
                         const char       *value,
                         bslma::Allocator *basicAllocator)
: d_name(name)
, d_value(value, basicAllocator)
, d_hashValue(-1)
{
}

inline
MyAttribute::MyAttribute(const char       *name,
                         const Value&      value,
                         bslma::Allocator *basicAllocator)
: d_name(name)
, d_value(value, basicAllocator)
, d_hashValue(-1)
{
}

inline
MyAttribute::MyAttribute(const MyAttribute&  original,
                         bslma::Allocator   *basicAllocator)
: d_name(original.d_name)
, d_value(original.d_value, basicAllocator)
, d_hashValue(original.d_hashValue)
, d_hashSize(original.d_hashSize)
{
}

inline
MyAttribute::~MyAttribute()
{
}

// MANIPULATORS
inline
MyAttribute& MyAttribute::operator=(const MyAttribute& rhs)
{
    d_name      = rhs.d_name;
    d_value     = rhs.d_value;
    d_hashValue = rhs.d_hashValue;
    d_hashSize  = rhs.d_hashSize;
    return *this;
}

inline
void MyAttribute::setName(const char *name)
{
    d_name = name;
    d_hashValue = -1;
}

inline
void MyAttribute::setValue(const Value& value)
{
    d_value = value;
    d_hashValue = -1;
}

// ACCESSORS
inline
const char *MyAttribute::name() const
{
    return d_name;
}

inline
const MyAttribute::Value& MyAttribute::value() const
{
    return d_value;
}

// FREE OPERATORS
inline
bool operator==(const MyAttribute& lhs,
                const MyAttribute& rhs)
{
    return 0 == bsl::strcmp(lhs.d_name, rhs.d_name)
        && lhs.d_value == rhs.d_value;
}

inline
bool operator!=(const MyAttribute& lhs,
                const MyAttribute& rhs)
{
    return !(lhs == rhs);
}

// CLASS METHODS
int MyAttribute::hash(const MyAttribute& attribute, int size)
{
    if (attribute.d_hashValue < 0 || attribute.d_hashSize != size) {

        unsigned int hash = bdlb::HashUtil::hash1(
                              attribute.d_name,
                              static_cast<int>(bsl::strlen(attribute.d_name)));

        switch (attribute.d_value.type()) {
          case MyAttributeValue::k_INT32: {
            hash += bdlb::HashUtil::hash1(attribute.d_value.int32Value());
          } break;
          case MyAttributeValue::k_LONG_LONG: {
            hash += bdlb::HashUtil::hash1(attribute.d_value.llValue());
          } break;
          case MyAttributeValue::k_STRING: {
            hash += bdlb::HashUtil::hash1(attribute.d_value.stringValue());
          } break;
          case MyAttributeValue::k_UNSIGNED_LONG_LONG: {
            hash += bdlb::HashUtil::hash1(attribute.d_value.ullValue());
          } break;
          default: {
              BSLS_ASSERT_INVOKE_NORETURN("unreachable");
          }
        }

        attribute.d_hashValue = hash % size;
        attribute.d_hashSize  = size;
    }
    return attribute.d_hashValue;
}

// ACCESSORS
bsl::ostream& MyAttribute::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("name", d_name);
    printer.printAttribute("value", d_value);
    printer.end();

    return stream;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&      output,
                         const MyAttribute& attribute)
{
    attribute.print(output, 0, -1);
    return output;
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

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 16: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.  Suppress
        //   all 'cout' statements in non-verbose mode, and add streaming to
        //   a buffer to test programmatically the printing examples.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting usage example"
                          << "\n====================="
                          << endl;

// This section illustrates intended use of this component.
//
///Example 1: Basic 'Attribute' usage
/// - - - - - - - - - - - - - - - - -
// The following code creates four attributes having the same name, but
// different attribute value types.

        ball::Attribute a1("day", "Sunday");
        ball::Attribute a2("day", 7);
        ball::Attribute a3("day", 7LL);
        ball::Attribute a4("day", 7ULL);

// The names of the attributes can be found by calling the 'name' method:

        ASSERT(0 == bsl::strcmp("day", a1.name()));
        ASSERT(0 == bsl::strcmp("day", a2.name()));
        ASSERT(0 == bsl::strcmp("day", a3.name()));
        ASSERT(0 == bsl::strcmp("day", a4.name()));

        ASSERT(true     == a1.value().is<bsl::string>());
        ASSERT("Sunday" == a1.value().the<bsl::string>());

        ASSERT(true     == a2.value().is<int>());
        ASSERT(7        == a2.value().the<int>());

        ASSERT(true     == a3.value().is<long long>());
        ASSERT(7        == a3.value().the<long long>());

        ASSERT(true     == a4.value().is<unsigned long long>());
        ASSERT(7        == a4.value().the<unsigned long long>());

// Note that the name string that is passed to the constructor of
// 'ball::Attribute' *must* remain valid and unchanged after the
// 'ball::Attribute' object is created.  In the next example, we create a
// temporary buffer to store the name string, and then use the buffer to
// create an attribute.  Note that any subsequent changes to this temporary
// buffer will also modify the name of the attribute:
//..
        char buffer[] = "Hello";
        ball::Attribute a5(buffer, 1);                   // BAD IDEA!!!
        bsl::strcpy(buffer, "World");
        ASSERT(0 == bsl::strcmp("World", a5.name()));
//..
// The 'ball::Attribute' class also provides a constructor that takes a value
// of type 'ball::Attribute::Value':
//..
        ball::Attribute::Value value;
        value.assign<bsl::string>("Sunday");
        ball::Attribute a6("day", value);
        ASSERT(a6 == a1);
//..
///Example 2: Using 'Attribute' to log pointers to opaque structure
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Consider we have an event scheduler that operates on events referred to by
// event handle:
//..
        struct Event {
            int d_id;
        };

        typedef Event * EventHandle;
//..
// The event handler value can be logged using 'ball::Attribute' as follows:
//..
        Event           event;
        EventHandle     handle = &event;
        ball::Attribute a7("event", handle);

        ASSERT(true   == a7.value().is<const void *>());
        ASSERT(handle == a7.value().the<const void *>());
//..
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING PERFORMANCE
        //
        // Testing:
        //   PERFORMANCE
        // --------------------------------------------------------------------
#ifdef BSLS_PLATFORM_OS_SOLARIS
        struct timespec t1, t2;
        const Int64 oneBillion = 1000000000;
        const int numTrials = 1000000;

        Int64 totalCreationTime;
        Int64 totalCopyTime;

        {
            cout << "sizeof(ball::Attribute::Value): "
                 << sizeof(ball::Attribute::Value) << endl;

            totalCreationTime = totalCopyTime = 0;
            for (int i = 0; i < numTrials; ++i) {
                clock_gettime(CLOCK_HIGHRES, &t1);
                ball::Attribute attribute1("UUID", 1234);
                clock_gettime(CLOCK_HIGHRES, &t2);
                totalCreationTime += (t2.tv_sec - t1.tv_sec) * oneBillion
                                   + t2.tv_nsec - t1.tv_nsec;

                clock_gettime(CLOCK_HIGHRES, &t1);
                ball::Attribute attribute2(attribute1);
                clock_gettime(CLOCK_HIGHRES, &t2);
                totalCopyTime += (t2.tv_sec - t1.tv_sec) * oneBillion
                                + t2.tv_nsec - t1.tv_nsec;
            }

            cout << "int attribute - average creation time: "
                 << totalCreationTime / numTrials / 1000
                 << " us,  average copy time: "
                 << totalCopyTime / numTrials / 1000
                 << " us" << endl;

            totalCreationTime = totalCopyTime = 0;
            for (int i = 0; i < numTrials; ++i) {
                clock_gettime(CLOCK_HIGHRES, &t1);
                ball::Attribute attribute1("UUID", "12345678910");
                clock_gettime(CLOCK_HIGHRES, &t2);
                totalCreationTime += (t2.tv_sec - t1.tv_sec) * oneBillion
                                    + t2.tv_nsec - t1.tv_nsec;

                clock_gettime(CLOCK_HIGHRES, &t1);
                ball::Attribute attribute2(attribute1);
                clock_gettime(CLOCK_HIGHRES, &t2);
                totalCopyTime += (t2.tv_sec - t1.tv_sec) * oneBillion
                                + t2.tv_nsec - t1.tv_nsec;
            }

            cout << "string attribute - average creation time: "
                 << totalCreationTime / numTrials / 1000
                 << " us,  average copy time: "
                 << totalCopyTime / numTrials / 1000
                 << " us" << endl;
        }

        {
            cout << "sizeof(MyAttributeValue): "
                 << sizeof(MyAttributeValue) << endl;

            totalCreationTime = totalCopyTime = 0;
            for (int i = 0; i < numTrials; ++i) {
                clock_gettime(CLOCK_HIGHRES, &t1);
                MyAttribute attribute1("UUID", 1234);
                clock_gettime(CLOCK_HIGHRES, &t2);
                totalCreationTime += (t2.tv_sec - t1.tv_sec) * oneBillion
                                   + t2.tv_nsec - t1.tv_nsec;

                clock_gettime(CLOCK_HIGHRES, &t1);
                MyAttribute attribute2(attribute1);
                clock_gettime(CLOCK_HIGHRES, &t2);
                totalCopyTime += (t2.tv_sec - t1.tv_sec) * oneBillion
                                + t2.tv_nsec - t1.tv_nsec;
            }

            cout << "int attribute - average creation time: "
                 << totalCreationTime / numTrials / 1000
                 << " us,  average copy time: "
                 << totalCopyTime / numTrials / 1000
                 << " us" << endl;

            totalCreationTime = totalCopyTime = 0;
            for (int i = 0; i < numTrials; ++i) {
                clock_gettime(CLOCK_HIGHRES, &t1);
                MyAttribute attribute1("UUID", "12345678910");
                clock_gettime(CLOCK_HIGHRES, &t2);
                totalCreationTime += (t2.tv_sec - t1.tv_sec) * oneBillion
                                    + t2.tv_nsec - t1.tv_nsec;

                clock_gettime(CLOCK_HIGHRES, &t1);
                MyAttribute attribute2(attribute1);
                clock_gettime(CLOCK_HIGHRES, &t2);
                totalCopyTime += (t2.tv_sec - t1.tv_sec) * oneBillion
                                + t2.tv_nsec - t1.tv_nsec;
            }

            cout << "string attribute - average creation time: "
                 << totalCreationTime / numTrials / 1000
                 << " us,  average copy time: "
                 << totalCopyTime / numTrials / 1000
                 << " us" << endl;
        }

#endif
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING HASH FUNCTION (VALUE):
        //   Verify the hash return value is constant across all platforms for
        //   a given input.
        //
        // Plan:
        //   Specify a set of test vectors and verify the return value.
        //
        //   Note that 7 is the smallest hash table size and 1610612741 is
        //   largest size (that can fit into an int) used by stlport
        //   hashtable.
        //
        // Testing:
        //   static int hash(const Attribute&, int size);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting hash function"
                          << "\n====================="
                          << endl;

        for (int i = 0; i < NUM_HASH_DATA; ++i) {
            int LINE = HASH_DATA[i].d_line;

            Obj        mX(HASH_DATA[i].d_name,
                          createValue<HashData>(HASH_DATA, NUM_HASH_DATA, i));

            const Obj& X = mX;

            int hash = Obj::hash(X, HASH_DATA[i].d_hashSize);
            if (veryVerbose) {
                cout <<  X  << " ---> " << hash << endl;
            }
            ASSERTV(LINE, 0 <= hash);
            ASSERTV(LINE, hash < HASH_DATA[i].d_hashSize);
            ASSERTV(LINE, hash, HASH_DATA[i].d_hashValue == hash);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard guard;

            const Obj X("x", 0);

            ASSERT_PASS(Obj::hash(X,  1));
            ASSERT_FAIL(Obj::hash(X,  0));
            ASSERT_FAIL(Obj::hash(X, -1));
        }

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING NAME/VALUE MANIPULATORS
        //   The 'setName' and 'setValue' methods should set the corresponding
        //   fields correctly.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each pair (u, v) in the cross product
        //   S X S, construct u using the primary constructor, and then change
        //   its name and value using v's name and value.  Verify that the two
        //   objects have the same value.
        //
        // Testing:
        //   void setName(const char *n);
        //   void setValue(int n);
        //   void setValue(long n);
        //   void setValue(long long n);
        //   void setValue(unsigned int n);
        //   void setValue(unsigned long n);
        //   void setValue(unsigned long long n);
        //   void setValue(const bsl::string_view& s);
        //   void setValue(const char *s);
        //   void setValue(const void *s);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Name/Value Manipulators"
                          << "\n===============================" << endl;

        for (int i = 0; i < NUM_NAMES; ++i) {
        for (int j = 0; j < NUM_VALUES; ++j) {
            int LINE1 = NAMES[i].d_line;
            int LINE2 = VALUES[j].d_line;

            Obj::Value value1 = createValue<Values>(VALUES, NUM_VALUES, j);
            const Obj  V(NAMES[i].d_name, value1);

            for (int k = 0; k < NUM_NAMES; ++k) {
            for (int l = 0; l < NUM_VALUES; ++l) {
                int LINE3 = NAMES[k].d_line;
                int LINE4 = VALUES[l].d_line;

                Obj::Value value2 = createValue<Values>(VALUES,
                                                        NUM_VALUES,
                                                        l);
                const Obj  U(NAMES[k].d_name, value2);

                if (veryVerbose) {
                    cout << "\t";
                    P_(U);
                    P_(V);
                }

                bool isSame = i == k && j == l;

                Obj mW1(V); const Obj& W1 = mW1;

                ASSERTV(LINE1, LINE2, LINE3, LINE4, W1 == V);
                ASSERTV(LINE1, LINE2, LINE3, LINE4, (W1 == U) == isSame);
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        Obj::hash(W1, 65536) == Obj::hash(V, 65536));

                ASSERTV(LINE1, LINE2, LINE3, LINE4, W1.value() == V.value());
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        0 == strcmp(W1.name(), V.name()));

                mW1.setName(U.name());
                ASSERTV(LINE1, LINE2, LINE3, LINE4, W1.value() == V.value());
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        0 == strcmp(W1.name(), U.name()));

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
                    mW1.setValue(U.value().the<bsl::string>());
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

                ASSERTV(LINE1, LINE2, LINE3, LINE4, W2.value() == V.value());
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
        //   void setName(const char *n);
        //   void setValue(const Value& value);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Name/Value Manipulators"
                          << "\n===============================" << endl;

        for (int i = 0; i < NUM_NAMES; ++i) {
        for (int j = 0; j < NUM_VALUES; ++j) {
            int LINE1 = NAMES[i].d_line;
            int LINE2 = VALUES[j].d_line;

            Obj::Value value1 = createValue<Values>(VALUES, NUM_VALUES, j);
            Obj        v(NAMES[i].d_name, value1);  const Obj& V = v;

            for (int k = 0; k < NUM_NAMES; ++k) {
            for (int l = 0; l < NUM_VALUES; ++l) {
                int LINE3 = NAMES[k].d_line;
                int LINE4 = VALUES[l].d_line;

                Obj::Value value2 = createValue<Values>(VALUES, NUM_VALUES, l);
                Obj        u(NAMES[k].d_name, value2);  const Obj& U = u;

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
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        0 == strcmp(W1.name(), V.name()));

                mW1.setName(U.name());
                ASSERTV(LINE1, LINE2, LINE3, LINE4, W1.value() == V.value());
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                        0 == strcmp(W1.name(), U.name()));

                mW1.setValue(U.value());
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

                ASSERTV(LINE1, LINE2, LINE3, LINE4, W2.value() == V.value());
                ASSERTV(LINE1, LINE2, LINE3, LINE4,
                       0 == strcmp(W2.name(), V.name()));

                mW2.setValue(U.value());
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
        // TESTING NAME/VALUE CONSTRUCTORS
        //   The name/value constructors must initialize members correctly.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct an object
        //   x using the primary constructor, and another object y using the
        //   corresponding name/value constructor.  Use the equality operator
        //   to assert that both x and y have the same value as w.
        //
        // Testing:
        //   Attribute(const char *n, int v, const alct& a);
        //   Attribute(const char *n, long v, const alct& a);
        //   Attribute(const char *n, long long v, const alct& a);
        //   Attribute(const char *n, unsigned int v, const alct& a);
        //   Attribute(const char *n, unsigned long v, const alct& a);
        //   Attribute(const char *n, unsigned long long v, const alct& a);
        //   Attribute(const char *n, const str_view& v, const alct& a);
        //   Attribute(const char *n, const char *v, const alct& a);
        //   Attribute(const char *n, const void *v, const alct& a);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Name/Value Constructors"
                          << "\n=================================" << endl;

        for (int i = 0; i < NUM_NAMES; ++i) {
        for (int j = 0; j < NUM_VALUES; ++j) {
            int LINE1 = NAMES[i].d_line;
            int LINE2 = VALUES[j].d_line;

            const char *name = NAMES[i].d_name;
            Obj::Value value = createValue<Values>(VALUES, NUM_VALUES, j);
            Obj        y(name, value); const Obj& Y = y;

            switch (VALUES[j].d_type) {
              case 0: {
                const Obj X1(name, (int)VALUES[j].d_ivalue);
                ASSERTV(LINE1, LINE2, X1 == Y);
                const Obj X2(name, (int)VALUES[j].d_ivalue, &testAllocator);
                ASSERTV(LINE1, LINE2, X2 == Y);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Obj X3(name,
                                 (int)VALUES[j].d_ivalue,
                                 &testAllocator);
                    ASSERTV(LINE1, LINE2, X3 == Y);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
              } break;
              case 1: {
                const Obj X1(name, (long)VALUES[j].d_ivalue);
                ASSERTV(LINE1, LINE2, X1 == Y);
                const Obj X2(name, (long)VALUES[j].d_ivalue, &testAllocator);
                ASSERTV(LINE1, LINE2, X2 == Y);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Obj X3(name,
                                 (long)VALUES[j].d_ivalue,
                                 &testAllocator);
                    ASSERTV(LINE1, LINE2, X3 == Y);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
              } break;
              case 2: {
                const Obj X1(name, VALUES[j].d_ivalue);
                ASSERTV(LINE1, LINE2, X1 == Y);
                const Obj X2(name, VALUES[j].d_ivalue, &testAllocator);
                ASSERTV(LINE1, LINE2, X2 == Y);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Obj X3(name,
                                 VALUES[j].d_ivalue,
                                 &testAllocator);
                    ASSERTV(LINE1, LINE2, X3 == Y);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
              } break;
              case 3: {
                const Obj X1(name, (unsigned int)VALUES[j].d_uivalue);
                ASSERTV(LINE1, LINE2, X1 == Y);
                const Obj X2(name,
                             (unsigned int)VALUES[j].d_uivalue,
                             &testAllocator);
                ASSERTV(LINE1, LINE2, X2 == Y);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Obj X3(name,
                                 (unsigned int)VALUES[j].d_uivalue,
                                 &testAllocator);
                    ASSERTV(LINE1, LINE2, X3 == Y);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
              } break;
              case 4: {
                const Obj X1(name, (unsigned long)VALUES[j].d_uivalue);
                ASSERTV(LINE1, LINE2, X1 == Y);
                const Obj X2(name,
                             (unsigned long)VALUES[j].d_uivalue,
                             &testAllocator);
                ASSERTV(LINE1, LINE2, X2 == Y);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Obj X3(name,
                                 (unsigned long)VALUES[j].d_uivalue,
                                 &testAllocator);
                    ASSERTV(LINE1, LINE2, X3 == Y);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
              } break;
              case 5: {
                const Obj X1(name, VALUES[j].d_uivalue);
                ASSERTV(LINE1, LINE2, X1 == Y);
                const Obj X2(name,
                             VALUES[j].d_uivalue,
                             &testAllocator);
                ASSERTV(LINE1, LINE2, X2 == Y);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Obj X3(name,
                                 VALUES[j].d_uivalue,
                                 &testAllocator);
                    ASSERTV(LINE1, LINE2, X3 == Y);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
              } break;
              case 6: {
                const Obj X1(name, VALUES[j].d_svalue);
                ASSERTV(LINE1, LINE2, X1 == Y);
                const Obj X2(name, VALUES[j].d_svalue, &testAllocator);
                ASSERTV(LINE1, LINE2, X2 == Y);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Obj X3(name, VALUES[j].d_svalue, &testAllocator);
                    ASSERTV(LINE1, LINE2, X3 == Y);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
              } break;
              case 7: {
                const Obj X1(name, VALUES[j].d_pvalue);
                ASSERTV(LINE1, LINE2, X1 == Y);
                const Obj X2(name,
                             VALUES[j].d_pvalue,
                             &testAllocator);
                ASSERTV(LINE1, LINE2, X2 == Y);
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    const Obj X3(name,
                                 VALUES[j].d_pvalue,
                                 &testAllocator);
                    ASSERTV(LINE1, LINE2, X3 == Y);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
              } break;
              default:
                BSLS_ASSERT_INVOKE_NORETURN("unreachable");
            }
        }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'bdex' STREAMING FUNCTIONALITY:
        //   Void for 'ball::Attribute'.
        // --------------------------------------------------------------------

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
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
        //   Attribute& operator=(const Attribute& rhs);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Assignment Operator"
                          << "\n===========================" << endl;

        if (verbose) cout << "\nTesting assignment u = v.";

        for (int i = 0; i < NUM_NAMES; ++i) {
        for (int j = 0; j < NUM_VALUES; ++j) {
            int LINE1 = NAMES[i].d_line;
            int LINE2 = VALUES[j].d_line;

            Obj::Value value1 = createValue<Values>(VALUES, NUM_VALUES, j);
            Obj        v(NAMES[i].d_name, value1);  const Obj& V = v;

            for (int k = 0; k < NUM_NAMES; ++k) {
            for (int l = 0; l < NUM_VALUES; ++l) {
                int LINE3 = NAMES[k].d_line;
                int LINE4 = VALUES[l].d_line;

                  Obj::Value value2 = createValue<Values>(VALUES,
                                                          NUM_VALUES,
                                                          l);
                  Obj        u(NAMES[k].d_name, value2);  const Obj& U = u;

                  if (veryVerbose) {
                      cout << "\t";
                      P_(U);
                      P(V);
                  }

                  Obj w(V); const Obj& W = w;      // control
                  u = V;
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

            Obj::Value value1 = createValue<Values>(VALUES, NUM_VALUES, j);
            Obj        u(NAMES[i].d_name, value1);  const Obj& U = u;
            Obj        w(U); const Obj& W = w;                       // control

            u = u;

            if (veryVerbose) { T_; P_(U); P_(W); }
            ASSERTV(LINE1, LINE2, U == W);
        }
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING SECONDARY TEST APPARATUS:
        //   Void for 'ball::Attribute'.
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
        //   Attribute(const Attribute&);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting Copy Constructor"
                          << "\n========================" << endl;

        for (int i = 0; i < NUM_NAMES; ++i) {
        for (int j = 0; j < NUM_VALUES; ++j) {
            int LINE1 = NAMES[i].d_line;
            int LINE2 = VALUES[j].d_line;

            const char *name = NAMES[i].d_name;
            Obj::Value value = createValue<Values>(VALUES, NUM_VALUES, j);
            Obj        w(name, value); const Obj& W = w;  // control
            Obj        x(name, value); const Obj& X = x;
            Obj        y(X);           const Obj& Y = y;

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
        //   Construct a set W of unique ball::Attribute objects using every
        //   element of the cross product S X T.  Verify the correctness of
        //   'operator==' and 'operator!=' for all elements (u, v) of the
        //   cross product W X W.  Next for each element in W, make a copy of
        //   of the attribute, and then use that copy along with the same
        //   attribute value to create another ball::Attribute object to verify
        //   that is the same as that created directly from the name/value
        //   pair.
        //
        // Testing:
        //   operator==(const ball::Attribute&, const ball::Attribute&);
        //   operator!=(const ball::Attribute&, const ball::Attribute&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Equality Operators"
                          << "\n==========================" << endl;
        if (verbose) cout <<
            "\nCompare each pair of values (u, v) in W X W." << endl;
        {
            for (int i = 0; i < NUM_NAMES; ++i) {
            for (int j = 0; j < NUM_VALUES; ++j) {
                int LINE1 = NAMES[i].d_line;
                int LINE2 = VALUES[j].d_line;

                Obj::Value value1 = createValue<Values>(VALUES, NUM_VALUES, j);
                Obj        mX(NAMES[i].d_name, value1);  const Obj& X = mX;

                for (int k = 0; k < NUM_NAMES; ++k) {
                for (int l = 0; l < NUM_VALUES; ++l) {
                    int LINE3 = NAMES[k].d_line;
                    int LINE4 = VALUES[l].d_line;

                    Obj::Value value2 = createValue<Values>(VALUES,
                                                            NUM_VALUES,
                                                            l);
                    Obj        mY(NAMES[k].d_name, value2);  const Obj& Y = mY;

                    if (veryVerbose) {
                        cout << "\t";
                        P_(X);
                        P(Y);
                    }

                    bool isSame = i == k && j == l;
                    ASSERTV(LINE1, LINE2, LINE3, LINE4,  isSame == (X == Y));
                    ASSERTV(LINE1, LINE2, LINE3, LINE4, !isSame == (X != Y));
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

                Obj::Value value1 = createValue<Values>(VALUES, NUM_VALUES, j);
                Obj mX(NAMES[i].d_name, value1);  const Obj& X = mX;

                string name(NAMES[i].d_name);
                Obj mY(name.c_str(), value1);  const Obj& Y = mY;
                ASSERTV(LINE1, LINE2, X == Y);
                ASSERTV(LINE1, LINE2, !(X != Y));
              }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR AND 'print':
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
        //   bsl::ostream& operator<<(bsl::ostream&, const ball::Attribute&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'operator<<' and 'print'" << endl
                          << "================================" << endl;

        if (verbose) cout << "\nTesting 'operator<<' (ostream)." << endl;

        for (int i = 0; i < NUM_PRINT_DATA; ++i) {
            int LINE = PRINT_DATA[i].d_line;
            Obj::Value value = createValue<PrintData>(PRINT_DATA,
                                                      NUM_PRINT_DATA,
                                                      i);

            Obj mX(PRINT_DATA[i].d_name, value);  const Obj& X = mX;

            ostringstream os;
            os << X;

            if (veryVerbose) {
                cout << "\t";
                P_(X);
                P_(PRINT_DATA[i].d_output);
                P(os.str());
            }
            ASSERTV(LINE, os.str(), PRINT_DATA[i].d_output,
                    compareText(os.str(), PRINT_DATA[i].d_output));
        }

        if (verbose) cout << "\nTesting 'const void *'." << endl;

        {
            const void *value = (void*)42;
            Obj         mX("name", value);

            ostringstream os;
            os << mX;

            const bsl::string  EXPECTED = " [ \"name\" = 0x2a ]";
            const bsl::string& ACTUAL = os.str();

            if (veryVerbose) {
                cout << "\t";
                P_(mX);
                P_(ACTUAL);
                P(EXPECTED);
            }

            ASSERTV(EXPECTED, ACTUAL, EXPECTED == ACTUAL);
        }

        if (verbose) cout << "\nTesting 'print'." << endl;

        static const struct {
            int         d_line;            // line number
            const char *d_name;            // attribute name
            const char *d_svalue;          // string attribute value
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

        enum { NUM_PDATA = sizeof PDATA / sizeof *PDATA };

        for (int i = 0; i < NUM_PDATA; ++i) {
            int        LINE = PDATA[i].d_line;
            Obj::Value VALUE(bsl::string(PDATA[i].d_svalue));
            Obj        mX(PDATA[i].d_name, VALUE);  const Obj& X = mX;

            ostringstream os;
            X.print(os, PDATA[i].d_level, PDATA[i].d_spacesPerLevel);

            if (veryVerbose) {
                cout << "\t";
                P_(X);
                P_(PDATA[i].d_output);
                P(os.str());
            }
            ASSERTV(LINE, os.str(), PDATA[i].d_output,
                    compareText(os.str(), PDATA[i].d_output));
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
        //   Construct one ball::Attribute object for every element of the
        //   cross product S X T using the primary constructor.  Verify that
        //   each of the basic accessors returns the correct value.
        //
        // Testing:
        //   const char *name();
        //   const Value& value();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing Basic Accessors" << endl
            << "=======================" << endl;

        for (int i = 0; i < NUM_NAMES; ++i) {
        for (int j = 0; j < NUM_VALUES; ++j) {
            int LINE1 = NAMES[i].d_line;
            int LINE2 = VALUES[j].d_line;

            Obj::Value value = createValue<Values>(VALUES, NUM_VALUES, j);
            Obj        mX(NAMES[i].d_name, value);  const Obj& X = mX;

            if (veryVerbose) {
                    cout << "\t";
                    P_(NAMES[i].d_name);
                    P_(value);
                    P(X);
            }
            ASSERTV(LINE1, LINE2, 0 == bsl::strcmp(X.name(), NAMES[i].d_name));
            ASSERTV(LINE1, LINE2, X.value() == value);
        }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMITIVE TEST APPARATUS:
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
        //   Value createValue(const DATA*, int, int);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Primitive Test Apparatus"
                          << "\n================================" << endl;

        if (verbose) cout << "\nTesting 'createValue'." << endl;

        for (int i = 0; i < NUM_VALUES; ++i) {
            int LINE = VALUES[i].d_line;

            Obj::Value value = createValue<Values>(VALUES, NUM_VALUES, i);

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
        //   Construct ball::Attribute objects with distinct name and value
        //   pairs, verify the values with the basic accessors, verify the
        //   equality and inequality of these objects.  The destructor is
        //   exercised as the objects being tested leave scope.
        //
        // Testing:
        //   Attribute(const char *n, const VALUE& v, const alct& a);
        //   ~Attribute();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Primary Manipulators"
                          << "\n============================" << endl;

        if (verbose) cout << "\nTesting primary manipulators." << endl;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            Obj::Value mV;  const Obj::Value& V = mV;

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
            Obj::Value mV(&testAllocator);  const Obj::Value& V = mV;

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
            Obj::Value mV(&testAllocator);  const Obj::Value& V = mV;

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
        mX1.setValue(Obj::Value(VB_VALUE));
        ASSERT(0 == bsl::strcmp(VB_NAME, X1.name()));
        ASSERT(VB_VALUE == X1.value().the<int>());
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        if (verbose) cout << "\n 4. Set x2 to VB." << endl;
        mX2.setName(VB_NAME);
        mX2.setValue(Obj::Value(VB_VALUE));
        ASSERT(0 == bsl::strcmp(VB_NAME, X2.name()));
        ASSERT(VB_VALUE == X2.value().the<int>());
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X1 == X2));        ASSERT(0 == (X1 != X2));

        if (verbose) cout << "\n 5. Set x2 to VC." << endl;
        mX2.setName(VC_NAME);
        mX2.setValue(Obj::Value(VC_VALUE));
        ASSERT(0 == bsl::strcmp(VC_NAME, X2.name()));
        ASSERT(VC_VALUE == X2.value().the<long long>());
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        if (verbose) cout << "\n 6. Set x1 to VA." << endl;
        mX1.setName(VA_NAME);
        mX1.setValue(Obj::Value(VA_VALUE));
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
