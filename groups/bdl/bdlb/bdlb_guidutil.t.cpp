// bdlb_guidutil.t.cpp                                                -*-C++-*-
#include <bdlb_guidutil.h>

#include <bdlb_guid.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_byteorder.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component is a utility wrapping platform dependent GUID generation
// functionality.  Since it is mostly an adapter the concerns enforced on the
// properties of the GUID itself are not exhaustive, since those are the
// responsibility of the generators themselves.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [1] void generate(Guid *out, size_t cnt)
// [1] void generate(unsigned char *out, size_t cnt)
// [1] Guid generate()
// [2] int getVersion(const Guid& guid)
// [3] int guidFromString(Guid *result, StrRef guidString)
// [3] Guid guidFromString(StrRef guidString)
// [4] void guidToString(bsl::string *result, const Guid& guid)
// [4] bsl::string guidToString(const Guid& guid)
// [5] Uint64 getMostSignificantBits(const Guid& guid)
// [6] Uint64 getLeastSignificantBits(const Guid& guid)
// ----------------------------------------------------------------------------
// [7] USAGE EXAMPLE

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

// ============================================================================
//                          DEBUG PRINT SUPPORT
// ----------------------------------------------------------------------------

namespace BloombergLP {
namespace bdlb {

void debugprint(const Guid& guid)
    // Print the specified 'guid' to the standard output stream.
{
    guid.print(bsl::cout);
}

void debugprint(const bsl::string& string)
    // Print the specified 'string' to the standard output stream.
{
    bsl::cout << string;
}

}  // close package namespace
}  // close enterprise namespace

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlb::GuidUtil      Util;
typedef bdlb::Guid          Obj;
typedef const unsigned char Element[16];
struct UuidComponents {
    struct { unsigned char low[4], mid[2], high_vers[2]; } time;
    unsigned char                                          clock_seq[2];
    unsigned char                                          node_id[6];
};

const Element VALUES[] =
{
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },

    { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
      0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f },

    { 0x5c, 0x9d, 0x4e, 0x50, 0x0d, 0xf1, 0x11, 0xe4,
      0x91, 0x91, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 },

    { 0x5c, 0x9d, 0x4e, 0x51, 0x0d, 0xf1, 0x11, 0xe4,
      0x91, 0x91, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 },

    { 0x5c, 0x9d, 0x4e, 0x52, 0x0d, 0xf1, 0x11, 0xe4,
      0x91, 0x91, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 },

    { 0x5c, 0x9d, 0x4e, 0x53, 0x0d, 0xf1, 0x11, 0xe4,
      0x91, 0x91, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 },

    { 0x5c, 0x9d, 0x4e, 0x54, 0x0d, 0xf1, 0x11, 0xe4,
      0x91, 0x91, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 },

    { 0x5c, 0x9d, 0x4e, 0x56, 0x0d, 0xf1, 0x11, 0xe4,
      0x91, 0x91, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 },

    { 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
      0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0 }
};

const Element &V0 = VALUES[0],            // V0, V1, ... are used in
              &V1 = VALUES[1],
              &V2 = VALUES[2],
              &V3 = VALUES[3],
              &V4 = VALUES[4],
              &V5 = VALUES[5],
              &V6 = VALUES[6],
              &V7 = VALUES[7],
              &V8 = VALUES[8];

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------
// Suppose we are building a system for managing records for employees in a
// large international firm.  These records have no natural field which can be
// used as a unique ID, so a GUID must be created for each employee.
//
// First let us define a value-type for employees.
//..
    class MyEmployee {
        // This class provides a value-semantic type to represent an employee
        // record.  These records are for internal use only.
//..
// For the sake of brevity, we provide a limited amount of data in each record.
// We additionally show a very limited scope of functionality.
//..
        // DATA
        bsl::string d_name;    // name of the employee
        double      d_salary;  // salary in some common currency
        bdlb::Guid  d_guid;    // a GUID for the employee

      public:
        // CREATORS
        MyEmployee(const bsl::string& name, double salary);
            // Create an object with the specified 'name' and specified
            //'salary', generating a new GUID to represent the employee.

        // ...

        // ACCESSORS
        const bdlb::Guid& Guid() const;
            // Return the 'guid' of this object.

        const bsl::string& name() const;
            // Return the 'name' of this object.

        double salary() const;
            // Return the 'salary' of this object.
        // ...
 };
//..
// Next, we create free functions 'operator<' and 'operator==' to allow
// comparison of 'MyEmployee' objects.  We take advantage of the monotonically
// increasing nature of sequential GUIDs to implement these methods.
//..
bool operator== (const MyEmployee& lhs, const MyEmployee& rhs);
    // Return 'true' if the specified 'lhs' object has the same value as the
    // specified 'rhs' object, and 'false' otherwise.  Note that two
    // 'MyEmployee' objects have the same value if they have the same guid.

bool operator< (const MyEmployee& lhs, const MyEmployee& rhs);
    // Return 'true' if the value of the specified 'lhs' MyEmployee object is
    // less than the value of the specified 'rhs' MyEmployee object, and
    // 'false' otherwise.  A MyEmployee object is less than another if the
    // guid is less than the other.  Note that this is equivalent to saying
    // that one employee object was created before another.

// ...

// CREATORS
MyEmployee::MyEmployee(const string& name, double salary)
: d_name(name)
, d_salary(salary)
{
     bdlb::GuidUtil::generate(&d_guid);
}

// ACCESSORS
const bdlb::Guid& MyEmployee::Guid() const
{
    return d_guid;
}

const bsl::string& MyEmployee::name() const
{
    return d_name;
}

double MyEmployee::salary() const
{
    return d_salary;
}

// FREE FUNCTIONS
bool operator==(const MyEmployee& lhs, const MyEmployee& rhs)
{
    return lhs.Guid() == rhs.Guid();
}

bool operator<(const MyEmployee& lhs, const MyEmployee& rhs)
{
     return lhs.Guid() < rhs.Guid();
}
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test            = argc > 1 ? bsl::atoi(argv[1]) : 0;
    bool verbose         = argc > 2;
    bool veryVerbose     = argc > 3;
    bool veryVeryVerbose = argc > 4;

    bslma::TestAllocator defaultAllocator("default", veryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    bslma::TestAllocator globalAllocator("global", veryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;
    switch (test)  { case 0:
      case 7: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, replace 'assert' with
        //:   'ASSERT', and insert 'if (veryVerbose)' before all output
        //:   operations.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;
//..
//  Next, we create some employees:
//..
        MyEmployee e1("Foo Bar"     , 1011970);
        MyEmployee e2("John Doe"    , 12345);
        MyEmployee e3("Joe Six-pack", 1);
//..
//  Finally, we verify that the generated GUIDs are unique.
///..
        ASSERT(e1 < e2 || e2 < e1);
        ASSERT(e2 < e3 || e3 < e2);
        ASSERT(e1 < e3 || e3 < e1);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'getLeastSignificantBits'
        //
        // Concerns:
        //: 1 The least significant bytes are returned.
        //: 2 The method does not modify the passed in 'Guid'.
        //
        // Plan:
        //: 1 Generate 'NUM_ITERS' Guids of each type, verifying the version
        //:   for each.
        //
        // Testing:
        //   Uint64 getLeastSignificantBits(const Guid& guid)
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'getLeastSignificantBits'" << endl
                          << "=================================" << endl;
            static const struct {
                  int            d_lineNum;  // source line number
                  const Element *d_array;    // byte array
            } DATA[] = {
                // Line             // Array
                { L_,                &V0},
                { L_,                &V1},
                { L_,                &V2},
                { L_,                &V3},
                { L_,                &V4},
                { L_,                &V5},
                { L_,                &V6},
                { L_,                &V7},
                { L_,                &V8},
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int      LINE  = DATA[ti].d_lineNum;
                const Element& ARRAY = *DATA[ti].d_array;

                Obj       mX(ARRAY);
                const Obj  X = mX;

                bsls::Types::Uint64 lsb = Util::getLeastSignificantBits(X);
                ASSERT(X == mX);
                if (veryVerbose) {
                    P(LINE);
                    P(X);
                    cout << "lsb = " << hex
                         << BSLS_BYTEORDER_BE_U64_TO_HOST(lsb)
                         << dec      << endl;
                }
                ASSERT(0 == ::memcmp(&X[8], &lsb, 8));
            }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'getMostSignificantBits'
        //
        // Concerns:
        //: 1 The most significant bytes are returned.
        //: 2 The method does not modify the passed in 'Guid'.
        //
        // Plan:
        //: 1 Generate various 'Guids' with different values, request the most
        //:   significant bytes and verify that the match the expected value.
        //
        // Testing:
        //   Uint64 getMostSignificantBits(const Guid& guid)
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'getMostSignificantBits'" << endl
                          << "================================" << endl;
            static const struct {
                  int            d_lineNum;  // source line number
                  const Element *d_array;    // byte array
            } DATA[] = {
                // Line             // Array
                { L_,                &V0},
                { L_,                &V1},
                { L_,                &V2},
                { L_,                &V3},
                { L_,                &V4},
                { L_,                &V5},
                { L_,                &V6},
                { L_,                &V7},
                { L_,                &V8},
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int      LINE  = DATA[ti].d_lineNum;
                const Element& ARRAY = *DATA[ti].d_array;

                Obj       mX(ARRAY);
                const Obj  X = mX;

                bsls::Types::Uint64 msb = Util::getMostSignificantBits(X);
                ASSERT(X == mX);
                if (veryVerbose) {
                    P(LINE);
                    P(X);
                    cout << "msb = " << hex
                         << BSLS_BYTEORDER_BE_U64_TO_HOST(msb)
                         << dec      << endl;
                }
                ASSERT(0 == ::memcmp(&X, &msb, 8));
            }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'guidToString'
        //
        // Concerns:
        //: 1 The guid loaded GUID makes the expected value.
        //
        // Plan:
        //: 1 Compose various guid strings of various types, and check the
        //:   return values.
        //
        // Testing:
        //  void guidToString(bsl::string *result, const Guid& guid)
        //  bsl::string guidToString(const Guid& guid)
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'guidToString'" << endl
                          << "======================" << endl;

        static const struct {
            int           d_lineNum;       // source line number
            const Element d_guidArray;     // an array with the same value as
                                           // the expected guid
            const char *  d_expGuidStr_p;  // specification string
        } DATA[] = {
        //    Line Array          Spec
        //    ---- -------------- ------------------------------------------
            { L_,  { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                     0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f },
                                  "00010203-0405-0607-0809-0a0b0c0d0e0f" },
            { L_,  { 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
                     0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0 },
                                  "00102030-4050-6070-8090-a0b0c0d0e0f0" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;
        for (int ti = 0; ti < NUM_DATA;  ++ti) {
            const int         LINE       = DATA[ti].d_lineNum;
            const char *const GUID_STR   = DATA[ti].d_expGuidStr_p;
            const Element&    GUID_ARRAY = DATA[ti].d_guidArray;

            const Obj          guid(GUID_ARRAY);
            bsl::string        result;
            const bsl::string  EXP(GUID_STR);
            Util::guidToString(&result, guid);
            if (veryVeryVerbose) { P_(LINE) P(result.c_str()) }
            LOOP_ASSERT(LINE, EXP == result);
            result = Util::guidToString(guid);
            if (veryVeryVerbose) { P_(LINE) P(result.c_str()) }
            LOOP_ASSERT(LINE, EXP == result);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'guidFromString'
        //
        // Concerns:
        //:  1 Each of the various GUID formats are accepted, with the correct
        //:    Guid and return code.
        //:
        //:  2 If a string does not match the grammar it is rejected.
        //:
        //:  3 If a string is rejected, the result is unchanged.
        //:
        //:  4 If a string is rejected, the return code is non-zero.
        //
        // Plan:
        //:  1 Compose various guid strings of various types, and check the
        //:    return values.
        //
        // Testing:
        //   int guidFromString(Guid *result, StrRef guidString)
        //   Guid guidFromString(StrRef guidString)
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'guidFromString''" << endl
                          << "=========================" << endl;

        static const struct {
            int           d_lineNum;       // source line number
            const char *  d_guidStr_p;     // specification string
            int           d_returnCode;    // return status
            const Element d_expGuidArray;  // an array with the same value as
                                           // the expected guid
        } DATA[] = {
        //  line   spec                                         code  expected
        //  ----   -------------------------------------------- ----  --------
        // valid guids
            { L_,  "000102030405060708090a0b0c0d0e0f",          0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f } },
            { L_,  "[000102030405060708090a0b0c0d0e0f]",        0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f } },
            { L_,  "[ 000102030405060708090a0b0c0d0e0f ]",      0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f } },
            { L_,  "[000102030405060708090a0b0c0d0e0f]",        0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f } },
            { L_,  "[ 000102030405060708090a0b0c0d0e0f ]",      0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f} },
            { L_,  "{000102030405060708090a0b0c0d0e0f}",        0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f} },
            { L_,  "{ 000102030405060708090a0b0c0d0e0f }",      0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f} },
        // dashed
            { L_,  "00010203-0405-0607-0809-0a0b0c0d0e0f",      0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f} },
            { L_,  "[00010203-0405-0607-0809-0a0b0c0d0e0f]",    0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f} },
            { L_,  "[ 00010203-0405-0607-0809-0a0b0c0d0e0f ]",  0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f} },
            { L_,  "[00010203-0405-0607-0809-0a0b0c0d0e0f]",    0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f} },
            { L_,  "[ 00010203-0405-0607-0809-0a0b0c0d0e0f ]",  0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f} },
            { L_,  "{00010203-0405-0607-0809-0a0b0c0d0e0f}",    0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f} },
            { L_,  "{ 00010203-0405-0607-0809-0a0b0c0d0e0f }",  0,    {
                            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f} },
        // different values
            { L_,  "00102030405060708090a0b0c0d0e0f0",          0,    {
                            0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
                            0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0} },
            { L_,  "00102030405060708090A0B0C0D0E0F0",          0,    {
                            0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
                            0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0} },
            { L_,  "AA1A2A3A4A5A6A7A8A9AAABACADAEAFA",          0,    {
                            0xaa, 0x1a, 0x2a, 0x3a, 0x4a, 0x5a, 0x6a, 0x7a,
                            0x8a, 0x9a, 0xaa, 0xba, 0xca, 0xda, 0xea, 0xfa} },
        // invalid guids
            { L_,  "",                                          1,    {
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} },
            { L_,  "{ 00010203-0405-0607-0809-0a0b0c0d0e0f } ", 1,    {
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} },
            { L_,  "{ 00010203-0405-0607-0809-0a0b0c0d0e0f ]",  1,    {
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} },
            { L_,  "{ 00010203-0405-060708090a0b0c0d0e0f }",    1,    {
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;
        for (int ti = 0; ti < NUM_DATA;  ++ti) {
            const int         LINE       = DATA[ti].d_lineNum;
            const char *const GUID_STR   = DATA[ti].d_guidStr_p;
            const int         RET        = DATA[ti].d_returnCode;
            const Element&    GUID_ARRAY = DATA[ti].d_expGuidArray;

            const Obj  EXP_GUID = Obj(GUID_ARRAY);
            Obj        mX;
            const Obj& X = mX;

            // double bang to send 0 -> 0  and !0 -> 1
            LOOP_ASSERT(LINE, !!RET == !!Util::guidFromString(&mX, GUID_STR));
            if (veryVerbose) { P_(X) P(EXP_GUID) }
            LOOP3_ASSERT(LINE, X, EXP_GUID, mX == EXP_GUID);
            if (RET == 0) {
                LOOP_ASSERT(LINE, EXP_GUID == Util::guidFromString(GUID_STR));
            }
            else {
                LOOP_ASSERT(LINE, Obj() == Util::guidFromString(GUID_STR));
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'getVersion'
        //
        // Concerns:
        //: 1 The correct type is returned for a generated GUID.
        //
        // Plan:
        //: 1 Generate 'NUM_ITERS' Guids, verifying the version for each.
        //
        // Testing:
        //   int getVersion(const Guid& guid)
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'getVersion'" << endl
                          << "====================" << endl;

        enum  { NUM_ITERS = 25 };
        for (bsl::size_t i = 0; i < NUM_ITERS; ++i) {
            Obj g;
            Util::generate(&g);
            if (veryVeryVerbose) { P_(i) P(g) }
            ASSERT(4 == Util::getVersion(g));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'generate'
        //
        // Concerns:
        //: 1 A single GUID can be passed and loaded.
        //: 2 If 'count' is passed, 'count' GUIDs are returned.
        //: 3 The correct type of GUID is returned.
        //: 4 Memory outside the designated range is left unchanged.
        //
        // Plan:
        //: 1 Call the 'generate' method with a count of 1, and call the
        //:   single-value 'generate' method.  (C-1)
        //:
        //: 2 Call the 'generate' method with different count values.  (C-2)
        //:
        //: 3 Check the internal structure of returned GUIDs to verify that
        //:   they are the right type.  (C-3)
        //:
        //: 4 Inspect memory areas just before and after the region that
        //:   receives GUIDs to verify that it is unchanged.
        //
        // Testing:
        //   void generate(Guid *out, size_t cnt)
        //   void generate(unsigned char *out, size_t cnt)
        //   Guid generate()
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'generate'" << endl
                          << "==================" << endl;
        enum  { NUM_ITERS = 15 };

        Obj guids[NUM_ITERS + 1]; // one larger to be allow checking bounds.
        Obj prev_guids[NUM_ITERS + 1];
        cout << dec;
        bsl::memset(guids, 0, sizeof(guids));
        if (veryVerbose) {
            cout << endl
                 << "A single GUID can be passed and loaded." << endl
                 << "---------------------------------------" << endl;
        }
        for (bsl::size_t i = 0; i < NUM_ITERS; ++i) {
            if (i % 3 == 0) {
                Util::generate(&guids[i], 1);
            }
            else if (i % 3 == 1) {
                Util::generate(reinterpret_cast<unsigned char*>(&guids[i]), 1);
            }
            else if (i % 3 == 2) {
                guids[i] = Util::generate();
            }
            prev_guids[i] = guids[i];
            if (veryVerbose) { P_(i) P(guids[i]); }
            bsl::size_t j;
            for (j = 0; j <= i; ++j) {
                if (veryVeryVerbose) { P_(j) P(guids[j]); }
                LOOP2_ASSERT(i, j, guids[j]      != Obj());
                LOOP2_ASSERT(i, j, prev_guids[j] == guids[j]);
            }
            for (; j < NUM_ITERS + 1; ++j) {
                if (veryVeryVerbose) { P_(j) P(guids[j]); }
                LOOP2_ASSERT(i, j, guids[j] == Obj());
            }
        }
        if (veryVerbose) {
            cout << endl
                 << "Get multiple GUIDs." << endl
                 << "-------------------" << endl;
        }
        for (bsl::size_t i = 0; i < NUM_ITERS; ++i) {
            bsl::memset(guids, 0, sizeof(guids));
            if (i & 1) {
                Util::generate(guids, i);
            }
            else {
                Util::generate(reinterpret_cast<unsigned char *>(guids), i);
            }
            if (veryVerbose)  {
                int idx = i ? i - 1 : 0;
                P_(idx) P(guids[idx]);
            }
            bsl::size_t j;
            for (j = 0; j < i; ++j) {
                if (veryVeryVerbose)  { P_(j) P(guids[j]); }
                LOOP2_ASSERT(i, j, guids[j] != Obj());
            }
            for (; j < NUM_ITERS + 1; ++j) {
                if (veryVeryVerbose)  { P_(j) P(guids[j]); }
                LOOP2_ASSERT(i, j, guids[j] == Obj());
            }
        }
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
