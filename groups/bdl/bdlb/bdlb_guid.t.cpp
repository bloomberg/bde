// bdlb_guid.t.cpp                                                    -*-C++-*-
#include <bdlb_guid.h>

#include <bslim_testutil.h>

#include <bslmf_assert.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_set.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// 'bdlb::Guid' provides a value-semantic type for representing Globally Unique
// Identifiers (GUID), without providing the functionality to create a GUID.
// The tests for this component are table based, i.e., testing actual results
// against a table of expected results.  Most tests construct objects and
// compare them to each other and expected results.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] bdlb::Guid(const unsigned char values[16]);
// [ 7] bdlb::Guid(const bdlb::Guid& original);
// [14] bdlb::Guid(ul, us, us, uc, uc, u64);
//
// ACCESSORS
// [ 2] const unsigned char& operator[](unsigned int offset) const;
// [11] ostream& print(ostream& stream, int level, int sp) const;
// [ 2] const unsigned char *begin() const;
// [ 2] const unsigned char *data() const;
// [ 2] const unsigned char *end() const;
// [14] unsigned char clockSeqHi() const;
// [14] unsigned char clockSeqHiRes() const;
// [14] unsigned char clockSeqLow() const;
// [14] bsls::Types::Uint64 node() const;
// [14] unsigned short timeHi() const;
// [14] unsigned short timeHiAndVersion() const;
// [14] unsigned long timeLow() const;
// [14] unsigned short timeMid() const;
// [14] unsigned char variant() const;
// [14] unsigned char version() const;
//
// MANIPULATORS
// [ 3] Guid& operator=(unsigned char buffer[k_GUID_NUM_BYTES]);
// [ 9] bdlb::Guid& operator=(const bdlb::Guid& rhs);
//
// FREE OPERATORS
// [ 6] bool operator==(const bdlb::Guid& lhs, const bdlb::Guid& rhs);
// [ 6] bool operator!=(const bdlb::Guid& lhs, const bdlb::Guid& rhs);
// [12] bool operator< (const bdlb::Guid& lhs, const bdlb::Guid& rhs);
// [12] bool operator<=(const bdlb::Guid& lhs, const bdlb::Guid& rhs);
// [12] bool operator> (const bdlb::Guid& lhs, const bdlb::Guid& rhs);
// [12] bool operator>=(const bdlb::Guid& lhs, const bdlb::Guid& rhs);
// [ 5] bsl::ostream& operator<<(bsl::ostream& stream, const Guid& guid);
// [15] bslh::Hash<>
//
// TRAITS
// [13] bslmf::IsBitwiseEqualityComparable
// [13] bsl::is_trivially_copyable
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] bdlb::Guid& gg(bdlb::Guid *object, const char *spec);
// [ 8] bdlb::Guid g(const char *spec);
// [ 4] int ggg(bdlb::Guid *object, const char *spec, int vF = 1);
// [16] USAGE EXAMPLE
// [10] int maxSupportedBdexVersion() const;
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;

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

//=============================================================================
//                    GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef bdlb::Guid          Obj;
typedef const unsigned char Element[16];
const Element               VALUES[] =
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
              &V1 = VALUES[1], &VA = V1,  // conjunction with the VALUES array.
              &V2 = VALUES[2], &VB = V2,
              &V3 = VALUES[3], &VC = V3,  // VA, VB, ... are used in
              &V4 = VALUES[4], &VD = V4,  // conjunction with 'g' and 'gg'.
              &V5 = VALUES[5], &VE = V5,
              &V6 = VALUES[6], &VF = V6,
              &V7 = VALUES[7], &VG = V7,
              &V8 = VALUES[8], &VH = V8;

namespace BloombergLP {
namespace bdlb {

void debugprint(const Obj& v)
    // Print the specified Guid 'v' to bsl::cout.
{
    v.print(bsl::cout);
}

}  // close package namespace
}  // close enterprise namespace

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------
///Usage
///-----
// Suppose we are building a utility to create globally unique names which may
// be based on a common base name, such as a code-generator.
//
// First, let us define the core types needed, the first of which is a utility
// to allocate GUIDs.
//..
struct MyGuidGeneratorUtil {
    // This struct provides a namespace for methods to generate GUIDs.

    // CLASS METHODS
    static int generate(bdlb::Guid *guid);
        // Generate a version 1 GUID placing the value into the specified
        // 'guid' pointer.  Return 0 on success, non-zero otherwise.
};

// CLASS METHODS
inline
int MyGuidGeneratorUtil::generate(bdlb::Guid *guid)
{
    // For brevity, we use a static sequence of pre-generated GUIDs.

    static unsigned char GUIDS[][bdlb::Guid::k_GUID_NUM_BYTES] = {
        { 0xde, 0xad, 0xbe, 0xef, 0xca, 0xfe, 0xba, 0xbe,
          0x91, 0x91, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 },

        { 0x5c, 0x9d, 0x4e, 0x51, 0x0d, 0xf1, 0x11, 0xe4,
          0x91, 0x91, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 },

        { 0x5c, 0x9d, 0x4e, 0x52, 0x0d, 0xf1, 0x11, 0xe4,
          0x91, 0x91, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 },

        { 0x5c, 0x9d, 0x4e, 0x53, 0x0d, 0xf1, 0x11, 0xe4,
          0x91, 0x91, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 },

        { 0x5c, 0x9d, 0x4e, 0x54, 0x0d, 0xf1, 0x11, 0xe4,
          0x91, 0x91, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 },

        { 0x5c, 0x9d, 0x4e, 0x55, 0x0d, 0xf1, 0x11, 0xe4,
          0x91, 0x91, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 },

        { 0x5c, 0x9d, 0x4e, 0x56, 0x0d, 0xf1, 0x11, 0xe4,
          0x91, 0x91, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 },
    };

    const bsl::size_t NUM_GUIDS = sizeof GUIDS / sizeof *GUIDS;

    static bsl::size_t nextGuidIdx = 0;

    int rval = -1;
    if (nextGuidIdx++ < NUM_GUIDS) {
        *guid = bdlb::Guid(GUIDS[nextGuidIdx]);
        rval = 0;
    }
    return rval;
}
//..
// Next, we create a utility to create unique strings.
//..
struct UniqueStringGenerator {
    // This struct provides methods to create globally unique strings.

    static int uniqueStringFromBase(bsl::string        *unique,
                                    const bsl::string&  base);
        // Create a globally unique string from the specified non-unique
        // 'base' string placing the result into the specified 'unique' string
        // pointer.
};

int
UniqueStringGenerator::uniqueStringFromBase(bsl::string        *unique,
                                            const bsl::string&  base)
{
    bdlb::Guid guid;

    int rval = MyGuidGeneratorUtil::generate(&guid);
    if (rval == 0) {
        ostringstream convert;
        convert << base << "-" << guid;
        *unique = convert.str();
    }
    return rval;
}
//..

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.  Uppercase
// letters [A .. H] correspond to arbitrary (but unique) unsigned char [16]
// values with which to construct the object.  The character '0' refers to the
// value of a default-constructed object.
//
// LANGUAGE SPECIFICATION:
// -----------------------
//
// <SPEC>       ::= <EMPTY>   |  <DEFAULT> | <VALUE>
//
// <EMPTY>      ::=
//
// <DEFAULT>    ::= '0'
//
// <VALUE>      ::= 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H'
//                                      // unique but otherwise arbitrary
//-----------------------------------------------------------------------------

int ggg(Obj *object, const char *spec, int verboseFlag = 1)
    // Configure the specified 'object' according to the specified 'spec',
    // using only the default and value assignment.  Optionally specify a zero
    // 'verboseFlag' to suppress 'spec' syntax error messages.  Return the
    // index of the first invalid character, and a negative value otherwise.
    // Note that this function is used to implement 'gg' as well as allow for
    // verification of syntax error detection.
{
    int rcode = 2;
    enum { SUCCESS = -1 };
    bsl::size_t spec_len = bsl::strlen(spec);

    switch (spec_len) {
      case 0: {
        rcode = SUCCESS;
      } break;
      case 1: {
        if ('A' <= spec[0] && spec[0] <= 'H') {
            *object = VALUES[spec[0] - 'A' + 1];
            rcode = SUCCESS;
        }
        else if ('0' == spec[0]) {
            *object = VALUES[0];
            rcode = SUCCESS;
        }
        else {
            if (verboseFlag) {
                cout << "Error, bad character ('" << spec[0]
                     << "') in spec \"" << spec << "\" at position "
                     << 0 << '.' << endl;
            }
            rcode = 0;
        }
      } break;
      default: {
        if (verboseFlag) {
            cout << "Spec " << spec << " is too long." << endl;
        }
        rcode = 2;
      } break;
    }
    return rcode;
}

Obj& gg(Obj *object, const char *spec)
    // Return, by reference, the specified 'object' with its value adjusted
    // according to the specified 'spec'.
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

Obj g(const char *spec)
    // Return, by value, a new object corresponding to the specified 'spec'.
{
    Obj object;
    return gg(&object, spec);
}


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test            = argc > 1 ? bsl::atoi(argv[1]) : 0;
    bool verbose         = argc > 2;
    bool veryVerbose     = argc > 3;
    bool veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test)  { case 0:
      case 16: {
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
        //:   leading comment characters with spaces, and replace 'assert' with
        //:   'ASSERT'.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;
//  Finally, we implement a program to generate unique names for a code
//  auto-generator.
//..
        bsl::string baseFileName = "foo.cpp";
        bsl::string uniqueFileName;
        bsl::string previousFileName;

        const bsl::size_t NUM_FILES = 5;
        for (bsl::size_t i = 0; i < NUM_FILES; ++i) {
            UniqueStringGenerator::uniqueStringFromBase(&uniqueFileName,
                                                        baseFileName);
            ASSERT(previousFileName != uniqueFileName);
            previousFileName = uniqueFileName;
        }
//..
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING HASH FUNCTION
        //
        // Concerns:
        //: 1 A guid object can be hashed by instances of
        //:   'bsl::hash<bdlb::Guid>'.
        //:
        //: 2 A small sample of different guid objects produce different
        //:   hashes.
        //:
        //: 3 Invoking 'bsl::hash<bdlb::Guid>' is identical to invoking
        //:   'bslh::DefaultHashAlgorithm' on the underlying data of the guid
        //:   object.
        //
        // Plan:
        //: 1 Hash some different guid objects and verify that the result of
        //:   using 'bsl::hash<bdlb::Guid>' is identical to invoking
        //:   'bslh::DefaultHashAlgorithm' on the underlying data of the guid
        //:   object.
        //:
        //: 2 Hash a number of different guid objects and verify that they
        //:   produce distinct hashes.
        //
        // Testing:
        //   bsl::hash<bdlb::Guid>
        //   bslh::Hash<>
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING HASH FUNCTION" << endl
                          << "=====================" << endl;

        const bsl::size_t NUM_VALUES = sizeof(VALUES) / sizeof(VALUES[0]);

        bsl::hash<Obj>        bslHashFunction;
        bsl::set<bsl::size_t> hashResults;

        for (bsl::size_t i = 0; i < NUM_VALUES; ++i) {
            Obj guid(VALUES[i]);
            bslh::DefaultHashAlgorithm defaultHashAlgorithm;
            defaultHashAlgorithm(guid.data(), 16);
            ASSERT(bslHashFunction(guid) ==
                                           defaultHashAlgorithm.computeHash());
            hashResults.insert(bslHashFunction(guid));
        }
        ASSERT(hashResults.size() == NUM_VALUES);
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING RFC 4122 ACCESS
        //
        // Concerns:
        //: 1 A guid object can be constructed using field values as specified
        //:   by RFC 4122.
        //:
        //: 2 RFC 4122-based field accessors return the values with which the
        //:   guid was constructed.
        //:
        //: 3 Verify that only the least significant 48 bits of the 'node'
        //:   argument are used to construct the guid.
        //
        // Plan:
        //: 1 Construct a guid using arbitrary values for the field-based
        //:   arguments and verify that they can be retrieved correctly.
        //
        // Testing:
        //   bdlb::Guid(ul, us, us, uc, uc, u64);
        //   unsigned char clockSeqHi() const;
        //   unsigned char clockSeqHiRes() const;
        //   unsigned char clockSeqLow() const;
        //   bsls::Types::Uint64 node() const;
        //   unsigned short timeHi() const;
        //   unsigned short timeHiAndVersion() const;
        //   unsigned long timeLow() const;
        //   unsigned short timeMid() const;
        //   unsigned char variant() const;
        //   unsigned char version() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING RFC 4122 ACCESS" << endl
                          << "=======================" << endl;

        const unsigned long       timeLow          = 0xFEDCBA98;
        const unsigned short      timeMid          = 0xEBFC;
        const unsigned short      timeHiAndVersion = 0x4F20;
        const unsigned short      timeHi           = timeHiAndVersion & 0xFFF;
        const unsigned short      version          = timeHiAndVersion >> 12;
        const unsigned char       clockSeqHiRes    = 0xB1;
        const unsigned char       clockSeqHi       = clockSeqHiRes & 0x1F;
        const unsigned char       variant          = clockSeqHiRes >> 5;
        const unsigned char       clockSeqLow      = 0xF7;
        const bsls::Types::Uint64 node             = 0x0000F0E1D2C3B4A5ULL;
        const bsls::Types::Uint64 ignored          = 0xDCBA000000000000ULL;

        Obj X(timeLow,
              timeMid,
              timeHiAndVersion,
              clockSeqHiRes,
              clockSeqLow,
              node | ignored);

        ASSERT(clockSeqHi       == X.clockSeqHi());
        ASSERT(clockSeqHiRes    == X.clockSeqHiRes());
        ASSERT(clockSeqLow      == X.clockSeqLow());
        ASSERT(node             == X.node());
        ASSERT(timeHi           == X.timeHi());
        ASSERT(timeHiAndVersion == X.timeHiAndVersion());
        ASSERT(timeLow          == X.timeLow());
        ASSERT(timeMid          == X.timeMid());
        ASSERT(variant          == X.variant());
        ASSERT(version          == X.version());
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING TRAITS
        //
        // Concerns:
        //: 1 bslmf::IsBitwiseEqualityComparable is true for Guid.
        //: 2 bsl::is_trivially_copyable is true for Guid.
        //
        // Plan:
        //: 1 Assert each trait.
        //
        // Testing:
        //   bslmf::IsBitwiseEqualityComparable
        //   bsl::is_trivially_copyable
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING TRAITS" << endl
                          << "==============" << endl;

        ASSERT(bslmf::IsBitwiseEqualityComparable<Obj>::value);
        ASSERT(bsl::is_trivially_copyable<Obj>::value);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING COMPARISON OPERATORS
        //
        // Concerns:
        //: 1 'operator<' returns the lexicographic comparison of two 'guid's.
        //: 2 Each operator implements a strict total order.
        //
        // Plan:
        //: 1 Construct objects in increasing order, and compare them to each
        //:   other in all combinations verifying the expected return value.
        //
        // Testing:
        //   bool operator< (const bdlb::Guid& lhs, const bdlb::Guid& rhs);
        //   bool operator<=(const bdlb::Guid& lhs, const bdlb::Guid& rhs);
        //   bool operator> (const bdlb::Guid& lhs, const bdlb::Guid& rhs);
        //   bool operator>=(const bdlb::Guid& lhs, const bdlb::Guid& rhs);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING COMPARISON OPERATORS" << endl
                          << "============================" << endl;
        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec;     // string
        } DATA[] = {
            //line      spec
            //----      ----
            { L_,       "0" },
            { L_,       "A" },
            { L_,       "H" },
            { L_,       "B" },
            { L_,       "C" },
            { L_,       "D" },
            { L_,       "E" },
            { L_,       "F" },
            { L_,       "G" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         X_LINE   = DATA[ti].d_lineNum;
            const char *const X_SPEC   = DATA[ti].d_spec;
            const Obj         X        = g(X_SPEC);
            for (int tj = ti + 1; tj < NUM_DATA; ++tj) {
                const int         Y_LINE = DATA[tj].d_lineNum;
                const char *const Y_SPEC = DATA[tj].d_spec;
                const Obj         Y      = g(Y_SPEC);
                if (veryVerbose) { P_(X) P(Y) }
                LOOP4_ASSERT(X_LINE, Y_LINE, X_SPEC, Y_SPEC,   X <= Y);
                LOOP4_ASSERT(X_LINE, Y_LINE, X_SPEC, Y_SPEC,   X <  Y);
                LOOP4_ASSERT(X_LINE, Y_LINE, X_SPEC, Y_SPEC,   Y >= X);
                LOOP4_ASSERT(X_LINE, Y_LINE, X_SPEC, Y_SPEC,   Y >  X);
                LOOP4_ASSERT(X_LINE, Y_LINE, X_SPEC, Y_SPEC, !(X >= Y));
                LOOP4_ASSERT(X_LINE, Y_LINE, X_SPEC, Y_SPEC, !(X >  Y));
                LOOP4_ASSERT(X_LINE, Y_LINE, X_SPEC, Y_SPEC, !(Y <= X));
                LOOP4_ASSERT(X_LINE, Y_LINE, X_SPEC, Y_SPEC, !(Y <  X));

                LOOP_ASSERT(Y_SPEC,   Y <= Y);
                LOOP_ASSERT(Y_SPEC,   Y >= Y);
                LOOP_ASSERT(Y_SPEC,   X <= X);
                LOOP_ASSERT(Y_SPEC,   X >= X);
                LOOP_ASSERT(Y_SPEC, !(Y < Y));
                LOOP_ASSERT(Y_SPEC, !(Y > Y));
                LOOP_ASSERT(Y_SPEC, !(X < X));
                LOOP_ASSERT(Y_SPEC, !(X > X));
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'print' METHOD
        //
        // Concerns:
        //: 1 The print method formats the value of the object directly from
        //:   the underlying state information according to supplied arguments.
        //:
        //: 2 Ensure that the method formats properly for:
        //:   - empty and non-empty values
        //:   - negative, 0, and positive levels.
        //:   - 0 and non-zero spaces per level.
        //
        // Plan:
        //: 1 For each of an enumerated set of object, 'level', and
        //:   'spacesPerLevel' values, ordered by increasing object length, use
        //:   'ostringstream' to 'print' that object's value, using the
        //:   tabulated parameters, to two separate character buffers each with
        //:   different initial values.  Compare the contents of these buffers
        //:   with the literal expected output format and verify that the
        //:   characters beyond the null characters are unaffected in both
        //:   buffers.
        //
        // Testing:
        //   ostream& print(ostream& stream, int level, int sp) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'print' METHOD" << endl
                          << "======================" << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
//                line  spec  indent  s/tab format
//                ----  ----  ------  ----- --------------------------------
                { L_,   "",   0,      0,
                                    "00000000-0000-0000-0000-000000000000\n" },
                { L_,   "",   0,      2,
                                    "00000000-0000-0000-0000-000000000000\n" },
                { L_,   "",   1,      1,
                                   " 00000000-0000-0000-0000-000000000000\n" },
                { L_,   "",   1,      2,
                                  "  00000000-0000-0000-0000-000000000000\n" },
                { L_,   "",  -1,      2,
                                    "00000000-0000-0000-0000-000000000000\n" },
                { L_,   "",  -2,      2,
                                    "00000000-0000-0000-0000-000000000000\n" },
                { L_,   "",   1,     -2,
                                  "  00000000-0000-0000-0000-000000000000"   },
                { L_,   "",   2,     -2,
                                "    00000000-0000-0000-0000-000000000000"   },
                { L_,   "",  -1,     -3,
                                    "00000000-0000-0000-0000-000000000000"   },
                { L_,   "",  -2,     -3,
                                    "00000000-0000-0000-0000-000000000000"   },
                { L_,   "A",  0,      0,
                                    "00010203-0405-0607-0809-0a0b0c0d0e0f\n" },
                { L_,   "H",  0,      0,
                                    "00102030-4050-6070-8090-a0b0c0d0e0f0\n" },
                { L_,   "B",  0,      2,
                                    "5c9d4e50-0df1-11e4-9191-0800200c9a66\n" },
                { L_,   "D",  1,      2,
                                  "  5c9d4e52-0df1-11e4-9191-0800200c9a66\n" },
            };

            const int  NUM_DATA = sizeof DATA / sizeof *DATA;
            const int  SIZE     = 1000; // Big enough to hold output string.
            const char Z1 = static_cast<char>(0xFF); // Value 1 used for an
                                                     // unset char.
            const char Z2 = 0x00;  // Value 2 used to represent an unset char.

            char        mCtrlBuf1[SIZE];
            const char *CTRL_BUF1 = mCtrlBuf1;
            memset(mCtrlBuf1, Z1, SIZE);

            char        mCtrlBuf2[SIZE];
            const char *CTRL_BUF2 = mCtrlBuf2;
            memset(mCtrlBuf2, Z2, SIZE);

            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const int         IND    = DATA[ti].d_indent;
                const int         SPL    = DATA[ti].d_spaces;
                const char *const FMT    = DATA[ti].d_fmt_p;

                Obj        mX;
                const Obj& X = gg(&mX, SPEC);

                if (veryVerbose) { P_(SPEC) P_(IND) P_(SPL) P(FMT) }
                ostringstream out1(bsl::string(CTRL_BUF1, SIZE));
                X.print(out1, IND, SPL) << ends;
                ostringstream out2(bsl::string(CTRL_BUF2, SIZE));
                X.print(out2, IND, SPL) << ends;
                if (veryVerbose) { P(out1.str()) }
                const int SZ = strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LINE,
                            Z1 == out1.str()[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE,
                            Z2 == out2.str()[SIZE - 1]);  // Check for overrun.
                LOOP2_ASSERT(LINE,
                             SZ,
                             0 == strncmp(out1.str().c_str(), FMT, SZ - 1));
                LOOP2_ASSERT(LINE,
                             SZ,
                             0 == strncmp(out2.str().c_str(), FMT, SZ - 1));

                LOOP_ASSERT(LINE, 0 == memcmp(out1.str().c_str() + SZ,
                                              CTRL_BUF1 + SZ,
                                              REST));
                LOOP_ASSERT(LINE, 0 == memcmp(out2.str().c_str() + SZ,
                                              CTRL_BUF2 + SZ,
                                              REST));
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING STREAMING FUNCTIONALITY
        //
        // Concerns:
        //:  1 'maxSupportedBdexVersion' returns the correct version.
        //:
        //:  2 Ensure that streaming works under the following conditions:
        //:     VALID      - may contain any sequence of valid values.
        //:     EMPTY      - valid, but contains no data.
        //:     INVALID    - may or may not be empty.
        //:     INCOMPLETE - the stream is truncated, but otherwise valid.
        //:     CORRUPTED  - the data contains explicitly inconsistent fields.
        //
        // Plan:
        //: 1 First test 'maxSupportedBdexVersion' explicitly, and then
        //:   perform a trivial direct (breathing) test of the 'bdexStreamOut'
        //:   and 'bdexStreamIn' methods.
        //:
        //: 2 Next, specify a set S of unique object values with substantial
        //:   and varied differences, ordered by increasing length.
        //:
        //: 3 VALID STREAMS (and exceptions)
        //:    Using all combinations of (u, v) in S X S, stream-out the value
        //:    of u into a buffer and stream it back into (an independent
        //:    instance of) v, and assert that u == v.
        //:
        //: 4 EMPTY AND INVALID STREAMS
        //:    For each x in S, attempt to stream into (a temporary copy of) x
        //:    from an empty and then invalid stream.  Verify after each try
        //:    that the object is unchanged and that the stream is invalid.
        //:
        //: 5 INCOMPLETE (BUT OTHERWISE VALID) DATA
        //:    Write 3 distinct objects to an output stream buffer of total
        //:    length N.  For each partial stream length from 0 to N - 1,
        //:    construct a truncated input stream and attempt to read into
        //:    objects initialized with distinct values.  Verify values of
        //:    objects that are either successfully modified or left entirely
        //:    unmodified,  and that the stream became invalid immediately
        //:    after the first incomplete read.  Finally ensure that each
        //:    object streamed into is in some valid state by creating a copy
        //:    and then assigning a known value to that copy; allow the
        //:    original instance to leave scope without further modification,
        //:    so that the destructor asserts internal object invariants
        //:    appropriately.
        //:
        //: 6 CORRUPTED DATA
        //:    We will assume that the incomplete test fail every field,
        //:    including a char (multi-byte representation) hence we need
        //:    only to produce values that are inconsistent with a valid
        //:    value and verify that they are detected.  Use the underlying
        //:    stream package to simulate an instance of a typical valid
        //:    (control) stream and verify that it can be streamed in
        //:    successfully.  Then for each data field in the stream (beginning
        //:    with the version number), provide one or more similar tests with
        //:    that data field corrupted.  After each test, verify that the
        //:    object is in some valid state after streaming, and that the
        //:    input stream has gone invalid.
        //
        // Testing:
        //   int maxSupportedBdexVersion() const;
        //   STREAM& bdexStreamIn(STREAM& stream, int version);
        //   STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING STREAMING FUNCTIONALITY" << endl
                          << "===============================" << endl;
        cout << "NOT IMPLEMENTED" << endl;
      } break;
      case 9 : {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //
        // Concerns:
        //:  1 The value represented by any instance can be assigned to any
        //:    other instance.
        //:
        //:  2 The 'rhs' value must not be affected by the operation.
        //:
        //:  3 'rhs' going out of scope has no effect on the value of 'lhs'
        //:    after the assignment.
        //:
        //:  4 Aliasing (x = x): The assignment operator must always work --
        //:    even when the lhs and rhs are the same object.
        //
        // Plan:
        //:  1 Specify a set S of unique object values with substantial and
        //:   varied differences, ordered by increasing length.  To address
        //:   concerns 1 - 3, construct tests u = v for all (u, v) in S X S.
        //:   Using canonical controls UU and VV, assert before the assignment
        //:   that UU == u, VV == v, and v == u if and only if and only if
        //:   VV == UU.  After the assignment, assert that VV == u, VV == v,
        //:   and, for grins, that v == u.  Let v go out of scope and confirm
        //:   that VV == u.
        //:
        //:  2 As a separate exercise, we address 4 by constructing tests
        //:    y = y for all y in S.  Using a canonical control X, we will
        //:    verify that X == y before and after the assignment.
        //
        // Testing:
        //   bdlb::Guid& operator=(const bdlb::Guid& rhs);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING ASSIGNMENT OPERATOR" << endl
                          << "===========================" << endl;

        if (verbose) cout <<
            "\nAssign cross product of values with varied representations."
                                                                       << endl;
        {
            static const char *SPECS[] = {  // len: 0 - 5, 7, 8, 9
                "0", "A", "B", "C", "D", "E", "F", "G", "H", 0
            };

            for (int ui = 0; SPECS[ui]; ++ui) {
                const char *const U_SPEC = SPECS[ui];

                const Obj UU = g(U_SPEC);               // control

                for (int vi = 0; SPECS[vi]; ++vi) {
                    const char *const V_SPEC = SPECS[vi];

                    const Obj VV = g(V_SPEC);           // control

                    const int Z = ui == vi;  // flag indicating same values

                    Obj        mU;
                    const Obj& U = mU;
                    gg(&mU, U_SPEC);

                    {

                        Obj        mV;
                        const Obj& V = mV;
                        gg(&mV, V_SPEC);

                        LOOP2_ASSERT(U_SPEC, V_SPEC, UU == U);
                        LOOP2_ASSERT(U_SPEC, V_SPEC, VV == V);
                        LOOP2_ASSERT(U_SPEC, V_SPEC, Z == (V == U));

                        if (veryVeryVerbose) { P_(U) P(V) }
                        mU = V; // test assignment here

                        LOOP2_ASSERT(U_SPEC, V_SPEC, VV == U);
                        LOOP2_ASSERT(U_SPEC, V_SPEC, VV == V);
                        LOOP2_ASSERT(U_SPEC, V_SPEC,  V == U);
                    }

                    // 'mV' (and therefore 'V') now out of scope
                    LOOP2_ASSERT(U_SPEC, V_SPEC, VV == U);
                }
            }
        }

        if (verbose) cout << "\nTesting self assignment (Aliasing)." << endl;
        {
            static const char *SPECS[] = {  // len: 0 - 5, 7, 8, 9
                "", "0", "A", "B", "C", "D", "E", "F", "G", "H", 0
            };

            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];
                const Obj         X = g(SPEC);  // control

                Obj        mY;
                const Obj& Y = mY;
                gg(&mY, SPEC);

                if (veryVerbose) { T_ T_ P(Y) }

                LOOP_ASSERT(SPEC, Y == Y);
                LOOP_ASSERT(SPEC, X == Y);

                mY = Y; // test assignment here

                LOOP_ASSERT(SPEC, Y == Y);
                LOOP_ASSERT(SPEC, X == Y);
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'g'
        //
        // Concerns:
        //: 1 Since 'g' is implemented almost entirely using 'gg', we need to
        //:   verify only that the arguments are properly forwarded, and that
        //:   'g' returns an object by value.
        //
        // Plan:
        //: 2 For each SPEC in a short list of specifications, compare the
        //:   object returned (by value) from the generator function, 'g(SPEC)'
        //:   with the value of a newly constructed OBJECT configured using
        //:   'gg(&OBJECT, SPEC)'.  Finally, use 'sizeof' to confirm that the
        //:   (temporary) returned by 'g' differs in size from that returned by
        //:   'gg'.
        //
        // Testing:
        //   bdlb::Guid g(const char *spec);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING GENERATOR FUNCTION 'g'" << endl
                          << "==============================" << endl;

        static const char *SPECS[] = {
            "", "0", "A", "B", "C", "D", "E", "F", "G", "H", 0
        };

        if (verbose) cout <<
            "\nCompare values produced by 'g' and 'gg' for various inputs."
                                                                       << endl;
        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *spec = SPECS[ti];
            if (veryVerbose) { P_(ti);  P(spec); }
            Obj        mX;
            const Obj& X = mX;
            gg(&mX, spec);
            LOOP_ASSERT(ti, X == g(spec));
        }

        if (verbose) cout << "\nConfirm return-by-value." << endl;

        {
            const char *spec = "A";

            ASSERT(sizeof(Obj) == sizeof g(spec));      // compile-time fact

            Obj        x;                      // runtime tests
            Obj&       r1 = gg(&x, spec);
            Obj&       r2 = gg(&x, spec);
            const Obj& r3 = g(spec);
            const Obj& r4 = g(spec);
            ASSERT(&r2 == &r1);
            ASSERT(&x  == &r1);
            ASSERT(&r4 != &r3);
            ASSERT(&x  != &r3);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //
        // Concerns:
        //: 1 The new object's value is the same as that of the original
        //:   object (relying on the previously tested equality operators).
        //:
        //: 2 The value of the original object is left unaffected.
        //:
        //: 3 Subsequent changes in or destruction of the source object have
        //    no effect on the copy-constructed object.
        //
        // Plan:
        //: 1 To address concerns 1 and 2, specify a set S of object values
        //:   with substantial and varied differences, ordered by increasing
        //:   length.  For each value in S, initialize objects w and x, copy
        //:   construct y from x and use 'operator==' to verify that both x and
        //:   y subsequently have the same value as w.  Let x go out of scope
        //:   and again verify that w == y.
        //
        // Testing:
        //   bdlb::Guid(const bdlb::Guid& original);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING COPY CONSTRUCTOR" << endl
                          << "========================" << endl;

        if (verbose) cout <<
            "\nCopy construct values with varied representations." << endl;
        {
            static const char *SPECS[] = {
                "", "0", "A", "B", "C", "D", "E", "F", "G", "H", 0
            };

            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];

                // Create control object w.
                Obj        mW;
                const Obj& W = mW;
                gg(&mW, SPEC);

                if (veryVerbose) { T_ P(W) }

                Obj        *mX = new Obj;
                const Obj&  X = *mX;
                gg(mX, SPEC);
                LOOP_ASSERT(ti, X == W);

                Obj        mY(X);
                const Obj &Y = mY;
                LOOP_ASSERT(ti, X == W);
                LOOP_ASSERT(ti, Y == W);

                if (veryVerbose) { T_ P_(W) P_(X) P(Y) }

                delete mX;
                LOOP_ASSERT(ti, Y == W);
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        //
        // Concerns:
        //: 1 Since 'operators==' is implemented in terms of basic accessors,
        //:   it is sufficient to verify only that a difference in value of any
        //:   one basic accessor for any two given objects implies inequality.
        //:
        // Plan:
        //: 1 First specify a set S of unique object values having various
        //:   minor or subtle differences, ordered by non-decreasing length.
        //:   Verify the correctness of 'operator==' and 'operator!=' using all
        //:   elements (u, v) of the cross product S X S.
        //
        // Testing:
        //   bool operator==(const bdlb::Guid& lhs, const bdlb::Guid& rhs);
        //   bool operator!=(const bdlb::Guid& lhs, const bdlb::Guid& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING EQUALITY OPERATORS" << endl
                          << "==========================" << endl;

        if (verbose) cout <<
            "\nCompare each pair of similar values (u, v) in S X S." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec;     // string
            } DATA[] = {
                //line spec
                //---- ---------------------
                { L_,  ""                 },
                { L_,  "0"                },
                { L_,  "A"                },
                { L_,  "B"                },
                { L_,  "C"                },
                { L_,  "D"                },
                { L_,  "E"                },
                { L_,  "F"                },
                { L_,  "G"                },
                { L_,  "H"                },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         ILINE = DATA[i].d_lineNum;
                const char *const ISPEC = DATA[i].d_spec;

                Obj        mX;
                const Obj& X = mX;
                gg(&mX, ISPEC);

                if (veryVerbose) { P_(ILINE) P_(ISPEC) P(X) }

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int         JLINE = DATA[j].d_lineNum;
                    const char *const JSPEC = DATA[j].d_spec;

                    Obj        mY;
                    const Obj& Y = mY;
                    gg(&mY, JSPEC);

                    if (veryVerbose) { T_ P_(JLINE) P_(JSPEC) P(Y) }

                    int r = 1;

                    for (int k = 0; k < 16; ++k) {
                        if (X[k] != Y[k]) {
                            r = 0;
                            break;
                        }
                    }
                    LOOP2_ASSERT(i, j,  r == (X == Y));
                    LOOP2_ASSERT(i, j,  1 == (X == X));
                    LOOP2_ASSERT(i, j,  1 == (Y == Y));
                    LOOP2_ASSERT(i, j, !r == (X != Y));
                    LOOP2_ASSERT(i, j,  0 == (X != X));
                    LOOP2_ASSERT(i, j,  0 == (Y != Y));
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        //
        // Concerns:
        //: 1 Since the output operator is layered on basic accessors, it is
        //:   sufficient to test only the output format (and to ensure that no
        //:   additional characters are written past the terminating null).
        //
        // Plan:
        //: 1 For each of a small representative set of object values, ordered
        //:   by increasing length, use 'ostringstream' to write that object's
        //:   value to two separate character buffers each with different
        //:   initial values.  Compare the contents of these buffers with the
        //:   literal expected output format and verify that the characters
        //:   beyond the null characters are unaffected in both buffers.  Note
        //:   that the output ordering is not guaranteed and the function
        //:   'arePrintedValuesEquivalent' is used to validate equality of the
        //:   output to the expected output.
        //
        // Testing:
        //   bsl::ostream& operator<<(bsl::ostream& stream, const Guid& guid);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING OUTPUT (<<) OPERATOR" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting 'operator<<' (ostream)." << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //line  spec      output format
                //----  --------  ---------------------------------
                { L_,   "",       "[ 00000000-0000-0000-0000-000000000000 ]"},
                { L_,   "0",      "[ 00000000-0000-0000-0000-000000000000 ]"},
                { L_,   "A",      "[ 00010203-0405-0607-0809-0a0b0c0d0e0f ]"},
                { L_,   "B",      "[ 5c9d4e50-0df1-11e4-9191-0800200c9a66 ]"},
                { L_,   "C",      "[ 5c9d4e51-0df1-11e4-9191-0800200c9a66 ]"},
                { L_,   "D",      "[ 5c9d4e52-0df1-11e4-9191-0800200c9a66 ]"},
                { L_,   "E",      "[ 5c9d4e53-0df1-11e4-9191-0800200c9a66 ]"},
                { L_,   "F",      "[ 5c9d4e54-0df1-11e4-9191-0800200c9a66 ]"},
                { L_,   "G",      "[ 5c9d4e56-0df1-11e4-9191-0800200c9a66 ]"},
                { L_,   "H",      "[ 00102030-4050-6070-8090-a0b0c0d0e0f0 ]"}
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            const int SIZE     = 1000; // Must be big enough to hold output
                                       // string.
            const char Z1 = 'a';  // Value 1 used to represent an unset char.
            const char Z2 = 'b';  // Value 2 used to represent an unset char.

            char        mCtrlBuf1[SIZE];
            const char *CTRL_BUF1 = mCtrlBuf1;
            memset(mCtrlBuf1, Z1, SIZE);

            char        mCtrlBuf2[SIZE];
            const char *CTRL_BUF2 = mCtrlBuf2;
            memset(mCtrlBuf2, Z2, SIZE);

            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const char *const FMT    = DATA[ti].d_fmt_p;

                Obj        mX;
                const Obj& X = gg(&mX, SPEC);

                if (veryVerbose) { P_(SPEC) P(FMT) }
                ostringstream out1(bsl::string(CTRL_BUF1, SIZE));
                out1 << X << ends;
                ostringstream out2(bsl::string(CTRL_BUF2, SIZE));
                out2 << X << ends;
                if (veryVerbose) { P(out1.str()) }
                const int SZ   = strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP2_ASSERT(LINE, ti, SZ < SIZE);  // Check buffer is large
                                                    // enough.
                LOOP2_ASSERT(LINE,
                             ti,
                             Z1 == out1.str()[SIZE - 1]); // Check for overrun.
                LOOP2_ASSERT(LINE,
                             ti,
                             Z2 == out2.str()[SIZE - 1]); // Check for overrun.
                LOOP2_ASSERT(LINE, ti, 0 == memcmp(out1.str().c_str() + SZ,
                                                   CTRL_BUF1 + SZ,
                                                   REST));
                LOOP2_ASSERT(LINE, ti, 0 == memcmp(out2.str().c_str() + SZ,
                                                   CTRL_BUF2 + SZ,
                                                   REST));
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING PRIMITIVE GENERATOR FUNCTION 'gg'
        //
        // Concerns:
        //: 1 Having demonstrated that our primary manipulators work as
        //:   expected under normal conditions, we want to verify that valid
        //:   generator syntax produces expected results and that invalid
        //:   syntax is detected and reported.
        //
        // Plan:
        //: 1 For each of an enumerated sequence of 'spec' values, ordered by
        //:   increasing 'spec' length, use the primitive generator function
        //:   'gg' to set the state of a newly created object.  Verify that
        //:   'gg' returns a valid reference to the modified argument object
        //:   and, using basic accessors, that the value of the object is as
        //:   expected.  Note that we are testing the parser only; the primary
        //:   manipulators are already assumed to work.
        //
        // Testing:
        //   bdlb::Guid& gg(bdlb::Guid *object, const char *spec);
        //   int ggg(bdlb::Guid *object, const char *spec, int vF = 1);
        // --------------------------------------------------------------------
        if (verbose)
            cout << endl
                 << "TESTING PRIMITIVE GENERATOR FUNCTION 'gg'" << endl
                 << "=========================================" << endl;

        if (verbose) cout << "\nTesting generator on valid specs." << endl;
        {
            static const struct {
                int            d_lineNum;          // source line number
                const char    *d_spec_p;           // specification string
                const Element *d_element;
            } DATA[] = {
                //line      spec    element
                //----      ----    -------
                { L_,       "" ,    &V0 },
                {L_,        "0",    &V0 },
                { L_,       "A",    &V1 },
                { L_,       "B",    &V2 },
                { L_,       "C",    &V3 },
                { L_,       "D",    &V4 },
                { L_,       "E",    &V5 },
                { L_,       "F",    &V6 },
                { L_,       "G",    &V7 },
                { L_,       "H",    &V8 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int            LINE =  DATA[ti].d_lineNum;
                const char *const    SPEC =  DATA[ti].d_spec_p;
                const Element *const ELEM =  DATA[ti].d_element;

                Obj        mX;
                const Obj& X = gg(&mX, SPEC);   // original spec
                if (veryVerbose) { T_ P_(SPEC) P(X) }
                LOOP_ASSERT(LINE, 0 == memcmp(&X[0], ELEM, 16));
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
                { L_,   "",             -1,  }, // control

                { L_,   "A",            -1,  }, // control
                { L_,   " ",             0,  },
                { L_,   ".",             0,  },
                { L_,   "J",             0,  },

                {L_,    "AE",            2,  },
                {L_,    "AA",            2,  },
                {L_,    "Af",            2,  },
                {L_,    " B",            2,  },
                {L_,    "  ",            2,  },

                {L_,    "AEA",           2,  },
                {L_,    "AAA",           2,  },
                {L_,    "BAA",           2,  },
                {L_,    " AA",           2,  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const int         INDEX  = DATA[ti].d_index;

                Obj mX;
                if (veryVerbose) { T_ P(SPEC) }

                int result = ggg(&mX, SPEC, veryVerbose);

                LOOP_ASSERT(LINE, INDEX == result);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ARRAY ASSIGNMENT OPERATOR
        //
        // Concerns:
        //: 1 The resulting guid after assignment relates equal to the original
        //:   array when compared byte-wise.
        //
        // Plan:
        //: 1 Assign one Guid object to another and compare them byte-for-byte.
        //
        // Testing:
        //     Guid& operator=(unsigned char buffer[k_GUID_NUM_BYTES]);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING ARRAY ASSIGNMENT OPERATOR" << endl
                          << "=================================" << endl;
        static const struct {
            int            d_line;
            const Element *d_array;
        } DATA[] = {
          // line  array
          // ----  -----
      // default value
          {L_,     &V0},
      // arbitrary test values
          {L_,     &V1},
          {L_,     &V8},

      // GUIDs generated by 'uuid_generate()'
          {L_,     &V2},
          {L_,     &V3},
          {L_,     &V4},
          {L_,     &V5},
          {L_,     &V6},
          {L_,     &V7},
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;
        for (int i = 0; i < NUM_DATA; ++i) {
            int            LINE    = DATA[i].d_line;
            const Element *ARRAY_P = DATA[i].d_array;

            Obj mX;
            mX = *ARRAY_P;
            if (veryVerbose) { P_(ARRAY_P) P(mX) }
            for (int i = 0; i < Obj::k_GUID_NUM_BYTES; ++i) {
                if (veryVeryVerbose) { P_(i) P_(ARRAY_P[i]) P(mX[i]) }
                LOOP_ASSERT(LINE, (*ARRAY_P)[i] == mX[i]);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CONSTRUCTORS / ACCESSORS TEST
        //
        // Concerns:
        //: 1  A default-constructed 'Guid' is initialized to all 0s.
        //:
        //: 2 A value constructed guid compares equal to specified array.
        //:
        //: 3 A the reference returned by operator[] is unmodifiable.
        //:
        //: 4 The reference returned by operator[] is suitable for iteration.
        //:
        //: 5 The address of the object is equal to the address of the first
        //:   element.
        //:
        //: 6 The 'begin' and 'data' accessors point to the first element.
        //:
        //: 7 The 'end' accessor points one past the last element.
        //
        // Plan:
        //: 1 Construct a default-constructed Guid and use 'operator[]' to
        //:   access the individual bytes and verify they expected values.
        //:   Then, using a tabular approach value, construct a Guid and check
        //:   each of the above concerns in order.
        //
        // Testing:
        //   const unsigned char& operator[](unsigned int offset) const;
        //   bdlb::Guid(const unsigned char values[16]);
        //   const unsigned char *begin() const;
        //   const unsigned char *data() const;
        //   const unsigned char *end() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "CONSTRUCTORS / ACCESSORS TEST" << endl
                          << "=============================" << endl;

        if (veryVerbose) cout << endl
                              << "Testing default constructor" << endl;

        Obj g;
        for (bsl::size_t i = 0; i < Obj::k_GUID_NUM_BYTES; ++i) {
            // 1.  A default-constructed 'Guid' is initialized to all 0s.
            if (veryVeryVerbose) {
                cout << hex << static_cast<unsigned int>(g[i]);
            }
            LOOP_ASSERT(i, 0 == g[i]);
        }

        static const struct {
            int      d_line;
            Element *d_array;
        } DATA[] = {
          // line  array
          // ----  -----
      // default value
          {L_,     &V0},
      // arbitrary test values
          {L_,     &V1},
          {L_,     &V8},

      // GUIDs generated by 'uuid_generate()'
          {L_,     &V2},
          {L_,     &V3},
          {L_,     &V4},
          {L_,     &V5},
          {L_,     &V6},
          {L_,     &V7},
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;
        if (veryVerbose) {
            cout << endl << "Testing value constructor" << endl;
        }
        for (int i = 0; i < NUM_DATA; ++i) {
            int      LINE    = DATA[i].d_line;
            Element& ARRAY_P = *DATA[i].d_array;

            Obj x(ARRAY_P);
            if (veryVerbose) {
                cout << "Expected Value: [";
                for (int i = 0; i < Obj::k_GUID_NUM_BYTES; ++i) {
                    cout << hex << static_cast<unsigned int>(ARRAY_P[i]);
                }
                cout << "]" << endl;
                cout << "Actual value: "; P(x);
            }
            //   2. A value constructed guid compares equal to specified array.
            LOOP_ASSERT(LINE, 0 == memcmp(&x[0], ARRAY_P,
                                          Obj::k_GUID_NUM_BYTES));

            // 3. A the reference returned by operator[] is unmodifiable.
            const unsigned char&
                (Obj::*f)(bsl::size_t) const = &Obj::operator[];
            (void)f;

            //   5. The address of the object is equal to the address of the
            //      first element.
            ASSERT(reinterpret_cast<const void *>(&x) ==
                   reinterpret_cast<const void *>(&x[0]));

            //   4. The reference returned by operator[] is suitable for
            //      iteration.
            const unsigned char *beg = &x[0];
            const unsigned char *end = &x[Obj::k_GUID_NUM_BYTES - 1];

            for (int j = 0; j < Obj::k_GUID_NUM_BYTES - 1; ++j) {
                if (veryVerbose) { P(beg); P(end); }
                LOOP2_ASSERT(LINE, j, &x[j] + 1 == &x[j + 1]);
                LOOP2_ASSERT(LINE, Obj::k_GUID_NUM_BYTES - 1 - j,
                                     end == &x[Obj::k_GUID_NUM_BYTES - 1 - j]);
                LOOP_ASSERT(j, beg == &x[j]);
                ++beg;
                --end;
            }

            //   6. The 'begin' and 'data' accessors point to the first
            //      element.
            ASSERT(&x[0] == x.begin());
            ASSERT(&x[0] == x.data());

            //   7. The 'end' accessor points one past the last element.
            ASSERT(x.end() == x.begin() + Obj::k_GUID_NUM_BYTES);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 We want to exercise basic value-semantic functionality.  In
        //:   particular we want to demonstrate a base-line level of correct
        //:   operation of the following methods and operators:
        //:      - default and copy constructors (and also the destructor)
        //:      - the assignment operator (including aliasing)
        //:      - equality operators: 'operator==' and 'operator!='
        //:      - the output operator: 'operator<<'
        //:      - primary manipulators: 'operator='
        //:      - basic accessors: 'length' and 'operator[]'
        //:   In addition we would like to exercise objects with potentially
        //:   different internal organizations representing the same value.
        //
        // Plan:
        //: 1 Create four objects using both the default and copy constructors.
        //:   Exercise these objects using primary manipulators, basic
        //:   accessors, equality operators, and the assignment operator.
        //:   Invoke the primary manipulator [1&5], copy constructor [2&8], and
        //:   assignment operator [9&10] in situations where the internal data
        //:   (i) must *not* and (ii) *must* be resized.  Try aliasing with
        //:   assignment for a non-empty instance [11] and allow the result to
        //:   leave scope, enabling the destructor to assert internal object
        //:   invariants.  Display object values frequently in verbose mode:
        //:
        //:   Create an object x1 (default ctor)       { x1: }
        //:   Create a second object x2 (copy from x1) { x1: x2: }
        //:   Add an element value A to x1             { x1:A x2: }
        //:   Add the same element value A to x2       { x1:A x2:A }
        //:   Add another element value B to x2        { x1:A x2:AB }
        //:   Remove all elements from x1              { x1: x2:AB }
        //:   Create a third object x3 (default ctor)  { x1: x2:AB x3: }
        //:   Create a fourth object x4 (copy of x2)   { x1: x2:AB x3: x4:AB }
        //:   Assign x2 = x1 (non-empty becomes empty) { x1: x2: x3: x4:AB }
        //:   Assign x3 = x4 (empty becomes non-empty) { x1: x2: x3:AB x4:AB }
        //:   Assign x4 = x4 (aliasing)                { x1: x2: x3:AB x4:AB }
        //
        // Testing:
        //   BREATHING TEST
        // ------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        BSLMF_ASSERT(16 == sizeof(Obj) && 16 == Obj::k_GUID_NUM_BYTES);
         // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1) Create an object x1 (default ctor)."
                             "\t\t\t{ x1: }" << endl;
        Obj mX1;  const Obj& X1 = mX1;
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout <<
            "\ta) Check initial state of x1." << endl;

        ASSERT(0 == memcmp(&X1[0], V0, 16));

        if (verbose) cout <<
            "\tb) Try equality operators: x1 <op> x1." << endl;
        ASSERT(X1 == X1);          ASSERT(!(X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2) Create a second object x2 (copy from x1)."
                             "\t\t{ x1: x2: }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta) Check the initial state of x2." << endl;
        ASSERT(0 == memcmp(&X1[0], V0, 16));
        if (verbose) cout <<
            "\tb) Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(X2 == X1);          ASSERT(!(X2 != X1));
        ASSERT(X2 == X2);          ASSERT(!(X2 != X2));

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3) Add an element value A to x1."
                             "\t\t\t{ x1:A x2: }" << endl;
        mX1 = VA;
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout <<
            "\ta) Check new state of x1." << endl;
        ASSERT(0 == memcmp(&mX1[0], VA, 16));
        if (verbose) cout <<
            "\tb) Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(  X1 == X1);           ASSERT(!(X1 != X1));
        ASSERT(!(X1 == X2));          ASSERT(  X1 != X2);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4) Add the same element value A to x2."
                             "\t\t\t{ x1:A x2:A }" << endl;
        mX2= VA;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta) Check new state of x2." << endl;
        ASSERT(0 == memcmp(&mX2[0], VA, 16));

        if (verbose) cout <<
            "\tb) Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(X2 == X1);          ASSERT(!(X2 != X1));
        ASSERT(X2 == X2);          ASSERT(!(X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5) Add another element value B to x2."
                             "\t\t\t{ x1:A x2:AB }" << endl;

        mX2 = VB;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout <<
            "\ta) Check new state of x2." << endl;
        ASSERT(0 == memcmp(&mX2[0], VB, 16));
        if (verbose) cout <<
            "\tb) Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(!(X2 == X1));       ASSERT( (X2 != X1));
        ASSERT(  X2 == X2);        ASSERT(!(X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7) Create a third object x3 (default ctor)."
                             "\t\t{ x1: x2:AB x3: }" << endl;

        Obj mX3;  const Obj& X3 = mX3;
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout <<
            "\ta) Check new state of x3." << endl;
        ASSERT(0 == memcmp(V0, &X3, 16));
        if (verbose) cout <<
            "\tb) Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(!(X3 == X1));          ASSERT(X3 != X1);
        ASSERT(!(X3 == X2));          ASSERT(X3 != X2);
        ASSERT(X3 == X3);             ASSERT(!(X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8) Create a forth object x4 (copy of x2)."
                             "\t\t{ x1: x2:AB x3: x4:AB }" << endl;

        Obj mX4(X2);  const Obj& X4 = mX4;
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout <<
            "\ta) Check new state of x4." << endl;
        ASSERT(0 == memcmp(VB, &X4, 16));

        if (verbose) cout <<
            "\tb) Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(!(X4 == X1));       ASSERT(X4 != X1);
        ASSERT(X4 == X2);          ASSERT(!(X4 != X2));
        ASSERT(!(X4 == X3));       ASSERT(X4 != X3);
        ASSERT(X4 == X4);          ASSERT(!(X4 != X4));


        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n11) Assign x4 = x4 (aliasing)."
                             "\t\t\t\t{ x1: x2: x3:AB x4:AB }" << endl;

        mX4 = X4;
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout <<
            "\ta) Check new state of x4." << endl;
        ASSERT(0 == memcmp(VB, &mX4, 16));

        if (verbose) cout <<
            "\tb) Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(!(X4 == X1));          ASSERT(  X4 != X1);
        ASSERT(X4 == X2);             ASSERT(  X4 == X2);
        ASSERT(!(X4 == X3));          ASSERT(  X4 != X3);
        ASSERT(X4 == X4);             ASSERT(!(X4 != X4));
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\nTrying the 'print' method..." << endl;
        if (verbose) {
            cout << endl;
            X4.print(cout);
            X4.print(cout,  0, 2);
            X4.print(cout,  1, 2);
            X4.print(cout,  2, 2);
            X4.print(cout,  0, -1);  cout << endl;
            X4.print(cout,  0, -2);  cout << endl;
            X4.print(cout,  0, -4);  cout << endl;
            X4.print(cout,  2, -4);  cout << endl;
            X4.print(cout,  4, -4);  cout << endl;
            X4.print(cout, -2, -4);  cout << endl;
            X4.print(cout, -4, -4);  cout << endl;
            X4.print(cout,  0, 2);
            cout << X4 << endl;
        }
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\nTrying 'operator<<'..." << endl;
        if (verbose) {
            cout << endl;
            cout << X4 << endl;
            cout << X4 << X4 << X4 << endl;
        }
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
