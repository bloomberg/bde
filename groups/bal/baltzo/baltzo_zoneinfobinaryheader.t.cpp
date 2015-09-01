// baltzo_zoneinfobinaryheader.t.cpp                                  -*-C++-*-

#include <baltzo_zoneinfobinaryheader.h>

#include <bsl_string.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_platform.h>

#include <bsl_climits.h>
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_sstream.h>

// ============================================================================
//                           ADL SWAP TEST HELPER
// ----------------------------------------------------------------------------

// TBD move this into its own component?
template <class TYPE>
void invokeAdlSwap(TYPE& a, TYPE& b)
    // Exchange the values of the specified 'a' and 'b' objects using the
    // 'swap' method found by ADL (Argument Dependent Lookup).  The behavior
    // is undefined unless 'a' and 'b' were created with the same allocator.
{
    //BSLS_ASSERT_OPT(a.allocator() == b.allocator()); // XXX: no allocators!

    using namespace bsl;
    swap(a, b);
}

// The following 'using' directives must come *after* the definition of
// 'invokeAdlSwap' (above).

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a single, simply constrained
// (value-semantic) attribute class.  The Primary Manipulators and Basic
// Accessors are therefore, respectively, the attribute setters and getters,
// each of which follows our standard unconstrained attribute-type naming
// conventions: 'setAttributeName' and 'attributeName':
//
// Primary Manipulators:
//: o 'setVersion'
//: o 'setNumIsGmt'
//: o 'setNumIsStd'
//: o 'setNumLeaps'
//: o 'setNumTransitions'
//: o 'setNumLocalTimeTypes'
//: o 'setAbbrevDataSize'
//
// Basic Accessors:
//: o 'char version'
//: o 'numIsGmt'
//: o 'numIsStd'
//: o 'numLeaps'
//: o 'numTransitions'
//: o 'numLocalTimeTypes'
//: o 'abbrevDataSize'
//
// This particular attribute class also provides a value constructor capable of
// creating an object in any state relevant for thorough testing, obviating the
// primitive generator function, 'gg', normally used for this purpose.  We will
// therefore follow our standard 10-case approach to testing value-semantic
// types except that we will test the value constructor in case 3 (in lieu of
// the generator function), with the default constructor and primary
// manipulators tested fully in case 2.
//
// Certain standard test cases are omitted:
//: o [ 3] -- No custom test apparatus is needed.
//: o [10] -- BDEX streaming is not (yet) implemented for this class.
//
// This class *does* not support an allocator for memory so the standard tests
// with respect to to allocators and exceptions from allocators are simplified
// to checks that no memory is every allocated from neither the default nor the
// global allocator.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
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
// CLASS METHODS
// [11] static bool isValidVersion(char value);
// [11] static bool isValidNumIsGmt(int value);
// [11] static bool isValidNumIsStd(int value);
// [11] static bool isValidNumLeaps(int value);
// [11] static bool isValidNumTransitions(int value);
// [11] static bool isValidNumLocalTimeTypes(int value);
// [11] static bool isValidAbbrevDataSize(int value);
//
// CREATORS
// [ 2] baltzo::ZoneinfoBinaryHeader();
// [ 3] baltzo::ZoneinfoBinaryHeader(char, int, int, int, int, int, int);
// [ 2] ~baltzo::ZoneinfoBinaryHeader();
//
// MANIPULATORS
// [ 9] operator=(const baltzo::ZoneinfoBinaryHeader& rhs);
// [ 2] void setVersion(char value);
// [ 2] void setNumIsGmt(int value);
// [ 2] void setNumIsStd(int value);
// [ 2] void setNumLeaps(int value);
// [ 2] void setNumTransitions(int value);
// [ 2] void setNumLocalTimeTypes(int value);
// [ 2] void setAbbrevDataSize(int value);
//
// [ 8] void swap(baltzo::ZoneinfoBinaryHeader& other);
//
// ACCESSORS
// [ 4] char version() const;
// [ 4] int numIsGmt() const;
// [ 4] int numIsStd() const;
// [ 4] int numLeaps() const;
// [ 4] int numTransitions() const;
// [ 4] int numLocalTimeTypes() const;
// [ 4] int abbrevDataSize() const;
//
// [ 5] ostream& print(ostream& s, int level = 0, int SPL = 4) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const baltzo::ZoneinfoBinaryHeader& lhs, rhs);
// [ 6] bool operator!=(const baltzo::ZoneinfoBinaryHeader& lhs, rhs);
//
// FREE FUNCTIONS
// [ 5] void swap(baltzo::ZoneinfoBinaryHeader& a, b);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [12] USAGE EXAMPLE
// [ *] CONCERN: This test driver is reusable w/other, similar components.
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ 5] CONCERN: All accessor methods are declared 'const'.
// [ 9] CONCERN: There is no temporary allocation from any allocator.
// [ 3] CONCERN: Precondition violations are detected when enabled.

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                   STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

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

// ============================================================================
//                     SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline
#define L_ __LINE__                           // current Line number

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

// ============================================================================
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef baltzo::ZoneinfoBinaryHeader Obj;

typedef char T1;  // 'version'
typedef int  T2;  // 'numIsGmt'
typedef int  T3;  // 'numIsStd'
typedef int  T4;  // 'numLeaps'
typedef int  T5;  // 'numTransitions'
typedef int  T6;  // 'numLocalTimeTypes'
typedef int  T7;  // 'abbrevDataSize'

// Define DEFAULT DATA used by test cases 3, 7, 8, and 9.

struct DefaultDataRow {
    int  d_line;           // source line number
    char d_version;
    int  d_numIsGmt;
    int  d_numIsStd;
    int  d_numLeaps;
    int  d_numTransitions;
    int  d_numLocalTimeTypes;
    int  d_abbrevDataSize;
};

static
const DefaultDataRow DEFAULT_DATA[] =
{
    //LINE  VER   NISGMT   NISSTD   NLEAPS   NTRANS   NLTT     ABBRSZ
    //----  ---   -------- -------- -------- -------- -------- -------

    // default (must be first)
    { L_,   '\0',       0,       0,       0,       0,       1,       1 },

    // 'version'
    { L_,   '\0',       0,       0,       0,       0,       1,       1 },
    { L_,    '2',       0,       0,       0,       0,       1,       1 },

    // 'numIsGmt'
    { L_,   '\0',       0,       0,       0,       0,       1,       1 },
    { L_,   '\0', INT_MAX,       0,       0,       0,       1,       1 },

    // 'numIsStd'
    { L_,   '\0',       0,       0,       0,       0,       1,       1 },
    { L_,   '\0',       0, INT_MAX,       0,       0,       1,       1 },

    // 'numLeaps'
    { L_,   '\0',       0,       0,       0,       0,       1,       1 },

    // 'numTransitions'
    { L_,   '\0',       0,       0,       0,       0,       1,       1 },
    { L_,   '\0',       0,       0,       0, INT_MAX,       1,       1 },


    // 'numLocalTimeTypes'
    { L_,   '\0',       0,       0,       0,       0,       1,       1 },
    { L_,   '\0',       0,       0,       0,       0, INT_MAX,       1 },

    // 'abbrevDataSize'
    { L_,   '\0',       0,       0,       0,       0,       1,       1 },
    { L_,   '\0',       0,       0,       0,       0,       1, INT_MAX },

    // other
    { L_,   '\0', INT_MAX, INT_MAX,       0, INT_MAX, INT_MAX, INT_MAX },
    { L_,   '\0', INT_MAX, INT_MAX,       0, INT_MAX, INT_MAX,       1 },
    { L_,   '\0', INT_MAX, INT_MAX,       0, INT_MAX,       1, INT_MAX },
    { L_,   '\0', INT_MAX, INT_MAX,       0,       0, INT_MAX, INT_MAX },
    { L_,   '\0', INT_MAX, INT_MAX,       0, INT_MAX, INT_MAX, INT_MAX },
    { L_,   '\0', INT_MAX,       0,       0, INT_MAX, INT_MAX, INT_MAX },
    { L_,   '\0',       0, INT_MAX,       0, INT_MAX, INT_MAX, INT_MAX },

    { L_,   '\0', INT_MAX, INT_MAX,       0, INT_MAX, INT_MAX, INT_MAX },
    { L_,   '\0', INT_MAX, INT_MAX,       0, INT_MAX, INT_MAX,       2 },
    { L_,   '\0', INT_MAX, INT_MAX,       0, INT_MAX,       2, INT_MAX },
    { L_,   '\0', INT_MAX, INT_MAX,       0,       1, INT_MAX, INT_MAX },
    { L_,   '\0', INT_MAX, INT_MAX,       0, INT_MAX, INT_MAX, INT_MAX },
    { L_,   '\0', INT_MAX,       1,       0, INT_MAX, INT_MAX, INT_MAX },
    { L_,   '\0',       1, INT_MAX,       0, INT_MAX, INT_MAX, INT_MAX },

    { L_,    '2', INT_MAX, INT_MAX,       0, INT_MAX, INT_MAX, INT_MAX },
    { L_,    '2', INT_MAX, INT_MAX,       0, INT_MAX, INT_MAX,       1 },
    { L_,    '2', INT_MAX, INT_MAX,       0, INT_MAX,       1, INT_MAX },
    { L_,    '2', INT_MAX, INT_MAX,       0,       0, INT_MAX, INT_MAX },
    { L_,    '2', INT_MAX, INT_MAX,       0, INT_MAX, INT_MAX, INT_MAX },
    { L_,    '2', INT_MAX,       0,       0, INT_MAX, INT_MAX, INT_MAX },
    { L_,    '2',       0, INT_MAX,       0, INT_MAX, INT_MAX, INT_MAX },

    { L_,    '2', INT_MAX, INT_MAX,       0, INT_MAX, INT_MAX, INT_MAX },
    { L_,    '2', INT_MAX, INT_MAX,       0, INT_MAX, INT_MAX,       2 },
    { L_,    '2', INT_MAX, INT_MAX,       0, INT_MAX,       2, INT_MAX },
    { L_,    '2', INT_MAX, INT_MAX,       0,       1, INT_MAX, INT_MAX },
    { L_,    '2', INT_MAX, INT_MAX,       0, INT_MAX, INT_MAX, INT_MAX },
    { L_,    '2', INT_MAX,       1,       0, INT_MAX, INT_MAX, INT_MAX },
    { L_,    '2',       1, INT_MAX,       0, INT_MAX, INT_MAX, INT_MAX },

};
const int DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

// ============================================================================
//                     GLOBAL CONSTANTS USED FOR TESTING
// ----------------------------------------------------------------------------

// Define 'bsl::string' value long enough to ensure dynamic memory allocation.

// JSL: Do we want to move this string to the component of bsl::string itself?
// JSL: e.g.,  #define BSLSTL_LONG_STRING ...   TBD!

#ifdef BSLS_PLATFORM_CPU_32_BIT
#define SUFFICIENTLY_LONG_STRING "123456789012345678901234567890123"
#else  // 64_BIT
#define SUFFICIENTLY_LONG_STRING "12345678901234567890123456789012" \
                                 "123456789012345678901234567890123"
#endif
BSLMF_ASSERT(sizeof SUFFICIENTLY_LONG_STRING > sizeof(bsl::string));

#define IMX INT_MAX

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Creating a 'baltzo::ZoneinfoBinaryHeader' from User Input
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// We define the 'getNextZoneinfoBinaryHeader' helper function, reads data from
// a stream, validates the data, and constructs a
// 'baltzo::ZoneinfoBinaryHeader' object.
//..
    int getNextZoneinfoBinaryHeader(baltzo::ZoneinfoBinaryHeader *object,
                                    bsl::istream&                 stream)
        // Set to the specified 'object' the value extracted from the specified
        // 'stream'.  Return 0 on success, and a non-zero value otherwise, with
        // no change to 'object'.  The 'stream' contains white-space separated
        // decimal representations of the attributes of
        // 'baltzo::ZoneinfoBinaryHeader' in the following order: 'version',
        // 'numIsGmt', 'numIsStd', 'numLeaps', 'numTransitions',
        // 'numLocalTimeTypes', and 'abbrevDataSize'.
    {
        int version;  // not 'char'
        int numIsGmt;
        int numIsStd;
        int numLeaps;
        int numTransitions;
        int numLocalTimeTypes;
        int abbrevDataSize;

        if (!(stream >> version
           && stream >> numIsGmt
           && stream >> numIsStd
           && stream >> numLeaps
           && stream >> numTransitions
           && stream >> numLocalTimeTypes
           && stream >> abbrevDataSize)) {
            return 1;                                                 // RETURN
        }

        if (!(baltzo::ZoneinfoBinaryHeader::isValidVersion(version)
           && baltzo::ZoneinfoBinaryHeader::isValidNumIsGmt(numIsGmt)
           && baltzo::ZoneinfoBinaryHeader::isValidNumIsStd(numIsStd)
           && baltzo::ZoneinfoBinaryHeader::isValidNumLeaps(numLeaps)
           && baltzo::ZoneinfoBinaryHeader::isValidNumTransitions(
                                                                numTransitions)
           && baltzo::ZoneinfoBinaryHeader::isValidNumLocalTimeTypes(
                                                             numLocalTimeTypes)
           && baltzo::ZoneinfoBinaryHeader::isValidAbbrevDataSize(
                                                            abbrevDataSize))) {
            return 2;                                                 // RETURN
        }

        object->setVersion(version);
        object->setNumIsGmt(numIsGmt);
        object->setNumIsStd(numIsStd);
        object->setNumLeaps(numLeaps);
        object->setNumTransitions(numTransitions);
        object->setNumLocalTimeTypes(numLocalTimeTypes);
        object->setAbbrevDataSize(abbrevDataSize);

        return 0;
    }
//..

// ============================================================================
//                               TEST APPARATUS
// ----------------------------------------------------------------------------
// JSL: REMOVE THIS after it is moved to the test allocator.
// JSL: change the name to 'TestAllocatorMonitor'.

class TestAllocatorMonitor {
    // TBD

    // DATA
    int                               d_lastInUse;
    int                               d_lastMax;
    int                               d_lastTotal;
    const bslma::TestAllocator *const d_allocator_p;

  public:
    // CREATORS
    TestAllocatorMonitor(const bslma::TestAllocator& basicAllocator);
        // TBD

    ~TestAllocatorMonitor();
        // TBD

    // ACCESSORS
    bool isInUseSame() const;
        // TBD

    bool isInUseUp() const;
        // TBD

    bool isMaxSame() const;
        // TBD

    bool isMaxUp() const;
        // TBD

    bool isTotalSame() const;
        // TBD

    bool isTotalUp() const;
        // TBD
};

// CREATORS
inline
TestAllocatorMonitor::TestAllocatorMonitor(
                                    const bslma::TestAllocator& basicAllocator)
: d_lastInUse(basicAllocator.numBlocksInUse())
, d_lastMax(basicAllocator.numBlocksMax())
, d_lastTotal(basicAllocator.numBlocksTotal())
, d_allocator_p(&basicAllocator)
{
}

inline
TestAllocatorMonitor::~TestAllocatorMonitor()
{
}

// ACCESSORS
inline
bool TestAllocatorMonitor::isInUseSame() const
{
    BSLS_ASSERT(d_lastInUse <= d_allocator_p->numBlocksInUse());

    return d_allocator_p->numBlocksInUse() == d_lastInUse;
}

inline
bool TestAllocatorMonitor::isInUseUp() const
{
    BSLS_ASSERT(d_lastInUse <= d_allocator_p->numBlocksInUse());

    return d_allocator_p->numBlocksInUse() != d_lastInUse;
}

inline
bool TestAllocatorMonitor::isMaxSame() const
{
    return d_allocator_p->numBlocksMax() == d_lastMax;
}

inline
bool TestAllocatorMonitor::isMaxUp() const
{
    return d_allocator_p->numBlocksMax() != d_lastMax;
}

inline
bool TestAllocatorMonitor::isTotalSame() const
{
    return d_allocator_p->numBlocksTotal() == d_lastTotal;
}

inline
bool TestAllocatorMonitor::isTotalUp() const
{
    return d_allocator_p->numBlocksTotal() != d_lastTotal;
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

    // CONCERN: This test driver is reusable w/other, similar components.

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 12: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

// To use our helper function, we supply it with a stream of (decimal,
// whitespace-separated values).  The resulting object has the expected value.
//..
    bsl::stringstream           input("50 1 2 0 3 4 5");
    baltzo::ZoneinfoBinaryHeader header;
    int rc;

    rc = getNextZoneinfoBinaryHeader(&header, input);

    ASSERT( 0  == rc);
    ASSERT('2' == header.version());
    ASSERT( 1  == header.numIsGmt());
    ASSERT( 2  == header.numIsStd());
    ASSERT( 0  == header.numLeaps());
    ASSERT( 3  == header.numTransitions());
    ASSERT( 4  == header.numLocalTimeTypes());
    ASSERT( 5  == header.abbrevDataSize());
//..
// Since all of the data in the stream has now been consumed, another call to
// the function returns an error and leaves the object unchanged.
//..
    header.setVersion(0);
    header.setNumIsGmt(10);
    header.setNumIsStd(20);
    header.setNumLeaps(0);
    header.setNumTransitions(30);
    header.setNumLocalTimeTypes(40);
    header.setAbbrevDataSize(50);

    rc = getNextZoneinfoBinaryHeader(&header, input);

    ASSERT(  0  != rc);
    ASSERT('\0' == header.version());
    ASSERT( 10  == header.numIsGmt());
    ASSERT( 20  == header.numIsStd());
    ASSERT(  0  == header.numLeaps());
    ASSERT( 30  == header.numTransitions());
    ASSERT( 40  == header.numLocalTimeTypes());
    ASSERT( 50  == header.abbrevDataSize());
//..

      } break;

      case 11: {
        // --------------------------------------------------------------------
        // CLASS METHODS 'isValid*'
        //   Ensure that each method correctly identifies its valid range.
        //
        // Concerns:
        //: 1 Each 'isValid*' method correctly identifies the valid range of
        //:   values for the its corresponding attribute (e.g., the
        //:   'isValidVersion' identifies the valid range for the 'version'
        //:   attribute).
        //:
        //: 2 The (predicate) method under test has return type 'bool'.
        //:
        //: 3 There is no allocation from any allocator.
        //
        // Plan:
        //: 1 Use the address of the 'isValidUtcOffsetInSeconds' class method
        //:   to initialize a pointer to a function having the appropriate
        //:   signature and return type.  (C-2)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Using the category partitioning method, select candidate
        //:     attribute values (including the boundaries) from each
        //:     equivalent range of values, treating the default value (0)
        //:     as a distinguished range having a single value.
        //:
        //:   2 Additionally provide a column, 'EXPECTED', that specifies
        //:     whether the value in that row is valid for the attribute.
        //:
        //: 4 For each row (representing a distinct attribute value, 'V') in
        //:   the table of P-3, verify that the class method returns the
        //:   expected value.  (C-1)
        //:
        //: 5 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   static bool isValidVersion(char value);
        //   static bool isValidNumIsGmt(int value);
        //   static bool isValidNumIsStd(int value);
        //   static bool isValidNumLeaps(int value);
        //   static bool isValidNumTransitions(int value);
        //   static bool isValidNumLocalTimeTypes(int value);
        //   static bool isValidAbbrevDataSize(int value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CLASS METHODS 'isValid*'" << endl
                          << "========================" << endl;

        if (verbose) cout <<
                 "\nAssign the addressess of 'isValid*' to variables." << endl;

        if (veryVerbose) cout <<
             "\nAssign the address of 'isValidVersion' to a variable." << endl;
        {
            typedef bool (*freeFuncPtr)(char);

            // Verify that the signature and return type are as expected.

            freeFuncPtr funcPtr = &Obj::isValidVersion;

            (void)funcPtr;  // quash potential compiler warning
        }

        if (veryVerbose) cout <<
            "\nAssign the address of 'isValidNumIsGmt' to a variable." << endl;
        {
            typedef bool (*freeFuncPtr)(int);

            // Verify that the signature and return type are as expected.

            freeFuncPtr funcPtr = &Obj::isValidNumIsGmt;

            (void)funcPtr;  // quash potential compiler warning
        }

        if (veryVerbose) cout <<
            "\nAssign the address of 'isValidNumIsStd' to a variable." << endl;
        {
            typedef bool (*freeFuncPtr)(int);

            // Verify that the signature and return type are as expected.

            freeFuncPtr funcPtr = &Obj::isValidNumIsStd;

            (void)funcPtr;  // quash potential compiler warning
        }

        if (veryVerbose) cout <<
            "\nAssign the address of 'isValidNumLeaps' to a variable." << endl;
        {
            typedef bool (*freeFuncPtr)(int);

            // Verify that the signature and return type are as expected.

            freeFuncPtr funcPtr = &Obj::isValidNumLeaps;

            (void)funcPtr;  // quash potential compiler warning
        }

        if (veryVerbose) cout <<
               "\nAssign the address of 'isValidNumTransitions' to a variable."
                                                                       << endl;
        {
            typedef bool (*freeFuncPtr)(int);

            // Verify that the signature and return type are as expected.

            freeFuncPtr funcPtr = &Obj::isValidNumTransitions;

            (void)funcPtr;  // quash potential compiler warning
        }

        if (veryVerbose) cout <<
            "\nAssign the address of 'isValidNumLocalTimeTypes' to a variable."
                                                                       << endl;
        {
            typedef bool (*freeFuncPtr)(int);

            // Verify that the signature and return type are as expected.

            freeFuncPtr funcPtr = &Obj::isValidNumLocalTimeTypes;

            (void)funcPtr;  // quash potential compiler warning
        }

        if (veryVerbose) cout <<
               "\nAssign the address of 'isValidAbbrevDataSize' to a variable."
                                                                       << endl;
        {
            typedef bool (*freeFuncPtr)(int);

            // Verify that the signature and return type are as expected.

            freeFuncPtr funcPtr = &Obj::isValidAbbrevDataSize;

            (void)funcPtr;  // quash potential compiler warning
        }

        if (veryVerbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::Default::setDefaultAllocatorRaw(&da);

        if (verbose) cout <<
            "\nCreate tables of distinct candidate attribute values." << endl;

        if (veryVerbose) cout << "\nCreate table for 'version'." << endl;
        {
            static const struct {
                int   d_line;           // source line number
                char  d_version;
                bool  d_isValidVersion;
            } DATA[] = {

                //LINE  VER             EXPECTED
                //----  ------------    --------
#if CHAR_MIN < 0
                { L_,   CHAR_MIN + 0,   false   },
                { L_,   CHAR_MIN + 1,   false   },
#endif

#if CHAR_MIN < 0
                { L_,       '\0' - 1,   false   },
#endif
                { L_,       '\0' + 0,    true   },
                { L_,       '\0' + 1,   false   },

                { L_,        '2' - 1,   false   },
                { L_,        '2' + 0,    true   },
                { L_,        '2' + 1,   false   },

                { L_,   CHAR_MAX - 1,   false   },
                { L_,   CHAR_MAX + 0,   false   },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const char VER      = DATA[ti].d_version;
                const bool EXPECTED = DATA[ti].d_isValidVersion;

                if (veryVerbose) { T_ P_(LINE) P_((int)VER) P(EXPECTED) }

                LOOP3_ASSERT(LINE, (int)VER, EXPECTED,
                             EXPECTED == Obj::isValidVersion(VER));
            }

            LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }

        if (veryVerbose) cout << "\nCreate table for 'numIsGmt'." << endl;
        {
            static const struct {
                int  d_line;           // source line number
                int  d_numIsGmt;
                bool d_isValidNumIsGmt;
            } DATA[] = {

                //LINE   NISMGT         EXPECTED
                //----   -----------    --------
                { L_,    INT_MIN + 0,   false   },
                { L_,    INT_MIN + 1,   false   },

                { L_,          0 - 1,   false   },
                { L_,          0 + 0,    true   },
                { L_,          0 + 1,    true   },

                { L_,    INT_MAX - 1,   true    },
                { L_,    INT_MAX + 0,   true    },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const int  NISGMT   = DATA[ti].d_numIsGmt;
                const bool EXPECTED = DATA[ti].d_isValidNumIsGmt;

                if (veryVerbose) { T_ P_(LINE) P_(NISGMT) P(EXPECTED) }

                LOOP3_ASSERT(LINE, NISGMT, EXPECTED,
                             EXPECTED == Obj::isValidNumIsGmt(NISGMT));

            }

            LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }

        if (veryVerbose) cout << "\nCreate table for 'numIsStd'." << endl;
        {
            static const struct {
                int  d_line;           // source line number
                int  d_numIsStd;
                bool d_isValidNumIsStd;
            } DATA[] = {

                //LINE   NISMGT         EXPECTED
                //----   -----------    --------
                { L_,    INT_MIN + 0,   false   },
                { L_,    INT_MIN + 1,   false   },

                { L_,          0 - 1,   false   },
                { L_,          0 + 0,   true    },
                { L_,          0 + 1,   true    },

                { L_,    INT_MAX - 1,   true    },
                { L_,    INT_MAX + 0,   true    },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const int  NISSTD   = DATA[ti].d_numIsStd;
                const bool EXPECTED = DATA[ti].d_isValidNumIsStd;

                if (veryVerbose) { T_ P_(LINE) P_(NISSTD) P(EXPECTED) }

                LOOP3_ASSERT(LINE, NISSTD, EXPECTED,
                             EXPECTED == Obj::isValidNumIsStd(NISSTD));

            }

            LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }

        if (veryVerbose) cout << "\nCreate table for 'numLeaps'." << endl;
        {
            static const struct {
                int  d_line;           // source line number
                int  d_numLeaps;
                bool d_isValidNumLeaps;
            } DATA[] = {

                //LINE   NISMGT         EXPECTED
                //----   -----------    --------
                { L_,    INT_MIN + 0,   false   },
                { L_,    INT_MIN + 1,   false   },

                { L_,          0 - 1,   false   },
                { L_,          0 + 0,    true   },
                { L_,          0 + 1,   false   },

                { L_,    INT_MAX - 1,   false   },
                { L_,    INT_MAX + 0,   false   },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const int  NLEAPS   = DATA[ti].d_numLeaps;
                const bool EXPECTED = DATA[ti].d_isValidNumLeaps;

                if (veryVerbose) { T_ P_(LINE) P_(NLEAPS) P(EXPECTED) }

                LOOP3_ASSERT(LINE, NLEAPS, EXPECTED,
                             EXPECTED == Obj::isValidNumLeaps(NLEAPS));

            }

            LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }

        if (veryVerbose) cout << "\nCreate table for 'numTransitions'."
                                                                       << endl;
        {
            static const struct {
                int  d_line;           // source line number
                int  d_numTransitions;
                bool d_isValidNumTransitions;
            } DATA[] = {

                //LINE   NTRANS         EXPECTED
                //----   -----------    --------
                { L_,    INT_MIN + 0,   false   },
                { L_,    INT_MIN + 1,   false   },

                { L_,          0 - 1,   false   },
                { L_,          0 + 0,   true    },
                { L_,          0 + 1,   true    },

                { L_,    INT_MAX - 1,   true    },
                { L_,    INT_MAX + 0,   true    },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const int  NTRANS   = DATA[ti].d_numTransitions;
                const bool EXPECTED = DATA[ti].d_isValidNumTransitions;

                if (veryVerbose) { T_ P_(LINE) P_(NTRANS) P(EXPECTED) }

                LOOP3_ASSERT(LINE, NTRANS, EXPECTED,
                             EXPECTED == Obj::isValidNumTransitions(NTRANS));

            }

            LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }

        if (veryVerbose) cout << "\nCreate table for 'numLocalTimeTypes'."
                                                                       << endl;
        {
            static const struct {
                int  d_line;           // source line number
                int  d_numLocalTimeTypes;
                bool d_isValidNumLocalTimeTypes;
            } DATA[] = {

                //LINE   NLTTS          EXPECTED
                //----   -----------    --------
                { L_,    INT_MIN + 0,   false   },
                { L_,    INT_MIN + 1,   false   },

                { L_,          1 - 1,   false   },
                { L_,          1 + 0,   true    },
                { L_,          1 + 1,   true    },

                { L_,    INT_MAX - 1,   true    },
                { L_,    INT_MAX + 0,   true    },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const int  NLTTS    = DATA[ti].d_numLocalTimeTypes;
                const bool EXPECTED = DATA[ti].d_isValidNumLocalTimeTypes;

                if (veryVerbose) { T_ P_(LINE) P_(NLTTS ) P(EXPECTED) }

                LOOP3_ASSERT(LINE, NLTTS, EXPECTED,
                             EXPECTED == Obj::isValidNumLocalTimeTypes(NLTTS));

            }

            LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }

        if (veryVerbose) cout << "\nCreate table for 'abbrevDataSize'."
                                                                       << endl;
        {
            static const struct {
                int  d_line;           // source line number
                int  d_abbrevDataSize;
                bool d_isValidAbbrevDataSize;
            } DATA[] = {

                //LINE   ABBRSZ         EXPECTED
                //----   -----------    --------
                { L_,    INT_MIN + 0,   false   },
                { L_,    INT_MIN + 1,   false   },

                { L_,          1 - 1,   false   },
                { L_,          1 + 0,   true    },
                { L_,          1 + 1,   true    },

                { L_,    INT_MAX - 1,   true    },
                { L_,    INT_MAX + 0,   true    },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE     = DATA[ti].d_line;
                const int  ABBRSZ   = DATA[ti].d_abbrevDataSize;
                const bool EXPECTED = DATA[ti].d_isValidAbbrevDataSize;

                if (veryVerbose) { T_ P_(LINE) P_(ABBRSZ ) P(EXPECTED) }

                LOOP3_ASSERT(LINE, ABBRSZ, EXPECTED,
                             EXPECTED == Obj::isValidAbbrevDataSize(ABBRSZ));

            }

            LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // BSLX STREAMING
        //   Ensure that we can serialize the value of any object of the class,
        //   and then deserialize that value back into any object of the class.
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //   Reserved for 'bslx' streaming.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BSLX STREAMING" << endl
                          << "==============" << endl;

        if (verbose) cout << "Not yet implemented." << endl;

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any modifiable
        //:   target object to that of any source object.
        //:
        //: 2 No memory is allocated from any allocator.
        //:
        //: 3 The signature and return type are standard.
        //:
        //: 4 The reference returned is to the target object (i.e., '*this').
        //:
        //: 5 The value of the source object is not modified.
        //:
        //: 6 Assigning an object to itself behaves as expected (alias-safety).
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-3)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique: Specify a set of (unique) valid
        //:   object values (one per row) in terms of their individual
        //:   attributes, including (a) first, the default value, and (b)
        //:   boundary values corresponding to every range of values that each
        //:   individual attribute can independently attain.
        //:
        //: 4 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3: (C-1, 4..5)
        //:
        //:   1 Use the value constructor to create two 'const' 'Obj', 'Z' and
        //:     'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2): (C-1, 4..5)
        //:
        //:     1 Use the value constructor to create a modifiable 'Obj', 'mX',
        //:       having the value 'W'.
        //:
        //:     2 Assign 'mX' from 'Z'.
        //:
        //:     3 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-4)
        //:
        //:     4 Use the equality-comparison operator to verify that: (C-1, 5)
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-5)
        //:
        //: 5 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-4.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-3: (C-6)
        //:
        //:   1 Use the value constructor to create a modifiable 'Obj' 'mX';
        //:     also use the value constructor to create a 'const' 'Obj' 'ZZ'.
        //:
        //:   2 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   3 Assign 'mX' from 'Z'.
        //:
        //:   4 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   5 Use the equality-comparison operator to verify that the target
        //:     object, 'mX', still has the same value as that of 'ZZ'.
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-2)
        //
        // Testing:
        //   operator=(const baltzo::ZoneinfoBinaryHeader& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY-ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::Default::setDefaultAllocatorRaw(&da);

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int                    NUM_DATA  = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  LINE1   = DATA[ti].d_line;
            const char VER1    = DATA[ti].d_version;
            const int  NISGMT1 = DATA[ti].d_numIsGmt;
            const int  NLEAPS1 = DATA[ti].d_numLeaps;
            const int  NISSTD1 = DATA[ti].d_numIsStd;
            const int  NTRANS1 = DATA[ti].d_numTransitions;
            const int  NLTT1   = DATA[ti].d_numLocalTimeTypes;
            const int  ABBRSZ1 = DATA[ti].d_abbrevDataSize;

            const Obj  Z(VER1,
                         NISGMT1,
                         NISSTD1,
                         NLEAPS1,
                         NTRANS1,
                         NLTT1,
                         ABBRSZ1);
            const Obj ZZ(VER1,
                         NISGMT1,
                         NISSTD1,
                         NLEAPS1,
                         NTRANS1,
                         NLTT1,
                         ABBRSZ1);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE1, Obj(), Z, Obj() == Z);
                firstFlag = false;
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int  LINE2   = DATA[tj].d_line;
                const char VER2    = DATA[tj].d_version;
                const int  NISGMT2 = DATA[tj].d_numIsGmt;
                const int  NISSTD2 = DATA[tj].d_numIsStd;
                const int  NLEAPS2 = DATA[tj].d_numLeaps;
                const int  NTRANS2 = DATA[tj].d_numTransitions;
                const int  NLTT2   = DATA[tj].d_numLocalTimeTypes;
                const int  ABBRSZ2 = DATA[tj].d_abbrevDataSize;

                if (veryVerbose) { T_ P_(LINE2)
                                      P_((int)VER2)
                                      P_(NISGMT2)
                                      P_(NISSTD2)
                                      P_(NLEAPS2)
                                      P_(NTRANS2)
                                      P_(NLTT2)
                                      P(ABBRSZ2)
                                 }

                {
                    Obj mX(VER2,
                           NISGMT2,
                           NISSTD2,
                           NLEAPS2,
                           NTRANS2,
                           NLTT2,
                           ABBRSZ2); const Obj& X = mX;

                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    Obj *mR = &(mX = Z);

                    LOOP4_ASSERT(LINE1, LINE2,  Z,   X,  Z == X);
                    LOOP4_ASSERT(LINE1, LINE2, mR, &mX, mR == &mX);

                    LOOP4_ASSERT(LINE1, LINE2, ZZ, Z, ZZ == Z);

                    LOOP2_ASSERT(LINE1, LINE2, 0 == da.numBlocksTotal());
                }
            }

            // self-assignment
            {
                      Obj mX(VER1,
                             NISGMT1,
                             NISSTD1,
                             NLEAPS1,
                             NTRANS1,
                             NLTT1,
                             ABBRSZ1);

                const Obj ZZ(VER1,
                             NISGMT1,
                             NISSTD1,
                             NLEAPS1,
                             NTRANS1,
                             NLTT1,
                             ABBRSZ1);

                const Obj& Z = mX;

                LOOP3_ASSERT(LINE1, ZZ, Z, ZZ == Z);

                Obj *mR = &(mX = Z);

                LOOP3_ASSERT(LINE1, ZZ,   Z, ZZ == Z);
                LOOP3_ASSERT(LINE1, mR, &mX, mR == &mX);

                LOOP_ASSERT(LINE1, 0 == da.numBlocksTotal());
            }
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
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
        //: 3 Neither function allocates memory from any allocator.
        //:
        //: 4 Both functions have standard signatures and return types.
        //:
        //: 5 Using either function to swap an object with itself does not
        //:   affect the value of the object (alias-safety).
        //:
        //: 6 The free 'swap' function is discoverable through ADL (Argument
        //:   Dependent Lookup).
        //
        // Plan:
        //: 1 Use the addresses of the 'swap' member and free functions defined
        //:   in this component to initialize, respectively, member-function
        //:   and free-function pointers having the appropriate signatures and
        //:   return types.  (C-4)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique: Specify a set of (unique) valid
        //:   object values (one per row) in terms of their individual
        //:   attributes, including (a) first, the default value, and (b)
        //:   boundary values corresponding to every range of values that each
        //:   individual attribute can independently attain.
        //:
        //: 4 For each row 'R1' in the table of P-3: (C-1..2, 5)
        //:
        //:   1 Use the value constructor to create a modifiable 'Obj', 'mW',
        //:     having the value described by 'R1'; also use
        //:     the value constructor and the accessors to create a
        //:     'const' 'Obj' 'XX' from 'mW'.
        //:
        //:   2 Use the member and free 'swap' functions to swap the value of
        //:     'mW' with itself; verify, after each swap, that the value is
        //:     unchanged.  (C-5)
        //:
        //:   3 For each row 'R2' in the table of P-3: (C-1..2)
        //:
        //:     1 Use the value constructor and the accessors to create a
        //:       modifiable 'Obj', 'mX', from 'XX' (P-4.2).
        //:
        //:     2 Use the value constructor to create a modifiable 'Obj', 'mY',
        //:       and having the value described by 'R2'; also use the value
        //:       constructor and the accessors to create, a 'const' 'Obj',
        //:       'YY', from 'Y'.
        //:
        //:     3 Use, in turn, the member and free 'swap' functions to swap
        //:       the values of 'mX' and 'mY'; verify, after each swap, that:
        //:       (C-1..2) the values have been exchanged.  (C-1)
        //:
        //: 5 Verify that the free 'swap' function is discoverable through ADL:
        //:   (C-6)
        //:
        //:   1 Create a set of attribute values, 'A', distinct from the values
        //:     corresponding to the default-constructed object, choosing
        //:     values that allocate memory if possible.
        //:
        //:   3 Use the default constructor to create a modifiable 'Obj' 'mX'
        //:     (having default attribute values); also use the value
        //:     constructor and the accessors to create a 'const' 'Obj' 'XX'
        //:     from 'mX'.
        //:
        //:   4 Use the value constructor to create a modifiable 'Obj' 'mY'
        //:     having the value described by the 'Ai' attributes; also use the
        //:     value constructor and the accessors to create a 'const' 'Obj'
        //:     'YY' from 'mY'.
        //:
        //:   5 Use the 'invokeAdlSwap' helper function template to swap the
        //:     values of 'mX' and 'mY', using the free 'swap' function defined
        //:     in this component, then verify that the values have been
        //:     exchanged.  (C-6)
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   void swap(baltzo::ZoneinfoBinaryHeader& other);
        //   void swap(baltzo::ZoneinfoBinaryHeader& a, b);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "SWAP MEMBER AND FREE FUNCTIONS" << endl
                          << "==============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each function to a variable." << endl;
        {
            typedef void (Obj::*funcPtr)(Obj&);
            typedef void (*freeFuncPtr)(Obj&, Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     memberSwap = &Obj::swap;
            freeFuncPtr freeSwap   = swap;

            (void)memberSwap;  // quash potential compiler warnings
            (void)freeSwap;
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::Default::setDefaultAllocatorRaw(&da);

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int                    NUM_DATA  = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  LINE1   = DATA[ti].d_line;
            const char VER1    = DATA[ti].d_version;
            const int  NISGMT1 = DATA[ti].d_numIsGmt;
            const int  NLEAPS1 = DATA[ti].d_numLeaps;
            const int  NISSTD1 = DATA[ti].d_numIsStd;
            const int  NTRANS1 = DATA[ti].d_numTransitions;
            const int  NLTT1   = DATA[ti].d_numLocalTimeTypes;
            const int  ABBRSZ1 = DATA[ti].d_abbrevDataSize;

            if (veryVerbose) { T_ P_(LINE1)
                                  P_((int)VER1)
                                  P_(NISGMT1)
                                  P_(NISSTD1)
                                  P_(NTRANS1)
                                  P_(NLTT1)
                                  P(ABBRSZ1)
                             }

                  Obj mW(VER1,
                         NISGMT1,
                         NISSTD1,
                         NLEAPS1,
                         NTRANS1,
                         NLTT1,
                         ABBRSZ1); const Obj& W = mW;
            const Obj XX(VER1,
                         NISGMT1,
                         NISSTD1,
                         NLEAPS1,
                         NTRANS1,
                         NLTT1,
                         ABBRSZ1);

            if (veryVerbose) { T_ P_(LINE1) P_(W) P(XX) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE1, Obj(), W, Obj() == W);
                firstFlag = false;
            }

            // member 'swap'
            {
                mW.swap(mW);

                LOOP3_ASSERT(LINE1, XX, W, XX == W);
            }

            // free function 'swap'
            {
                swap(mW, mW);

                LOOP3_ASSERT(LINE1, XX, W, XX == W);
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int  LINE2   = DATA[tj].d_line;
                const char VER2    = DATA[tj].d_version;
                const int  NISGMT2 = DATA[tj].d_numIsGmt;
                const int  NISSTD2 = DATA[tj].d_numIsStd;
                const int  NLEAPS2 = DATA[tj].d_numLeaps;
                const int  NTRANS2 = DATA[tj].d_numTransitions;
                const int  NLTT2   = DATA[tj].d_numLocalTimeTypes;
                const int  ABBRSZ2 = DATA[tj].d_abbrevDataSize;

                if (veryVerbose) { T_ P_(LINE2)
                                      P_((int)VER2)
                                      P_(NISGMT2)
                                      P_(NISSTD2)
                                      P_(NLEAPS2)
                                      P_(NTRANS2)
                                      P_(NLTT2)
                                      P(ABBRSZ2)
                                 }

                      Obj mX(XX.version(),
                             XX.numIsGmt(),
                             XX.numIsStd(),
                             XX.numLeaps(),
                             XX.numTransitions(),
                             XX.numLocalTimeTypes(),
                             XX.abbrevDataSize()); const Obj& X = mX;

                      Obj mY(VER2,
                             NISGMT2,
                             NISSTD2,
                             NLEAPS2,
                             NTRANS2,
                             NLTT2,
                             ABBRSZ2);             const Obj& Y = mY;

                const Obj YY(Y.version(),
                             Y.numIsGmt(),
                             Y.numIsStd(),
                             Y.numLeaps(),
                             Y.numTransitions(),
                             Y.numLocalTimeTypes(),
                             Y.abbrevDataSize());

                if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

                // member 'swap'
                {
                    mX.swap(mY);

                    LOOP4_ASSERT(LINE1, LINE2, YY, X, YY == X);
                    LOOP4_ASSERT(LINE1, LINE2, XX, Y, XX == Y);
                }

                // free function 'swap'
                {
                    swap(mX, mY);

                    LOOP4_ASSERT(LINE1, LINE2, XX, X, XX == X);
                    LOOP4_ASSERT(LINE1, LINE2, YY, Y, YY == Y);
                }
            }
        }

        if (verbose) cout <<
                "\nInvoke free 'swap' function in a context where ADL is used."
                                                                       << endl;
        {
            // 'A' values

            const T1 A1 = '2';
            const T2 A2 =  1;
            const T3 A3 =  1;
            const T4 A4 =  0;
            const T5 A5 =  1;
            const T6 A6 =  1;
            const T7 A7 =  1;

                  Obj mX;  const Obj& X = mX;
            const Obj XX(X.version(),
                         X.numIsGmt(),
                         X.numIsStd(),
                         X.numLeaps(),
                         X.numTransitions(),
                         X.numLocalTimeTypes(),
                         X.abbrevDataSize());

                  Obj mY(A1, A2, A3, A4, A5, A6, A7);  const Obj& Y = mY;
            const Obj YY(Y.version(),
                         Y.numIsGmt(),
                         Y.numIsStd(),
                         Y.numLeaps(),
                         Y.numTransitions(),
                         Y.numLocalTimeTypes(),
                         Y.abbrevDataSize());

            if (veryVerbose) { T_ P_(X) P(Y) }

            invokeAdlSwap(mX, mY);

            LOOP2_ASSERT(YY, X, YY == X);
            LOOP2_ASSERT(XX, Y, XX == Y);

            if (veryVerbose) { T_ P_(X) P(Y) }
        }

        // Verify no memory is allocated from the default allocator.

        LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //   Reserved for copy construction.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        if (verbose) cout << "Not yet implemented." << endl;

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:   their corresponding salient attributes respectively compares
        //:   equal.
        //:
        //: 2 All salient attributes participate in the comparison.
        //:
        //: 3 'true  == (X == X)'  (i.e., identity)
        //:
        //: 4 'false == (X != X)'  (i.e., identity)
        //:
        //: 5 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:
        //: 6 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:
        //: 7 'X != Y' if and only if '!(X == Y)'
        //:
        //: 8 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 9 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //:10 No memory allocation occurs as a result of comparison (e.g., the
        //:   arguments are not passed by value).
        //:   o Note that this class does not support object allocators so
        //:     we need only check the default and global allocators.
        //:
        //:11 The equality operator's signature and return type are standard.
        //:
        //:12 The inequality operator's signature and return type are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.
        //:   (C-8..9, 11..12)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their individual salient
        //:   attributes such that (a) for each salient attribute, there exists
        //:   a pair of rows that differ (slightly) in only the column
        //:   corresponding to that attribute, and (b) all attribute values
        //:   that can allocate memory on construction do so.
        //:   o Note that 'numLeaps' (Attribute 4) is excluded here because it
        //:     it must always be 0.
        //:
        //: 4 For each row 'R1' in the table of P-3:  (C-1..7)
        //:
        //:   1 Create a single object and
        //:     use it to verify the reflexive (anti-reflexive) property of
        //:     equality (inequality) in the presence of aliasing.  (C-3..4)
        //:
        //:   2 For each row 'R2' in the table of P-3:  (C-1..2, 5..7)
        //:
        //:     1 Record, in 'EXP', whether or not distinct objects created
        //:       from 'R1' and 'R2', respectively, are expected to have the
        //:       same value.
        //:
        //:     2 For each of two sets of values:  (C-1..2, 5..7)
        //:
        //:       1 Create an object 'X' having the value 'R1'.
        //:
        //:       2 Create an object 'Y' having the value 'R2'.
        //:
        //:       3 Verify the commutativity property and expected return value
        //:         for both '==' and '!='.  (C-1..2, 5..7)
        //:
        //: 5 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-11)
        //
        // Testing:
        //   bool operator==(const baltzo::LocalTimeDescriptor& lhs, rhs);
        //   bool operator!=(const baltzo::LocalTimeDescriptor& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EQUALITY-COMPARISON OPERATORS" << endl
                          << "=============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            using namespace baltzo;
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = operator==;
            operatorPtr operatorNe = operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        const int           NLEAPS = 0;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::Default::setDefaultAllocatorRaw(&da);

        if (verbose) cout <<
            "\nDefine appropriate individual attribute values, 'Ai' and 'Bi'."
                                                                       << endl;
        // Attribute 1 Values: 'version'

        const T1 A1 = '\0';                 // baseline
        const T1 B1 =  '2';

        // Attribute 2 Values: 'numIsGmt'

        const T2 A2 =  1;                   // baseline
        const T2 B2 =  2;

        // Attribute 3 Values: 'numIsStd'

        const T3 A3 =  3;                   // baseline
        const T3 B3 =  4;

        // Attribute 4 Values: 'numLeaps'   Excluded per P-3.

        // Attribute 5 Values: 'numTransitions'

        const T5 A5 =  5;                   // baseline
        const T5 B5 =  6;

        // Attribute 6 Values: 'numLocalTimeTypes'

        const T6 A6 =  7;                   // baseline
        const T6 B6 =  8;

        // Attribute 7 Values: 'abbrevDataSize'

        const T7 A7 =  9;                   // baseline
        const T7 B7 = 10;

        if (verbose) cout <<
            "\nCreate a table of distinct, but similar object values." << endl;

        static const struct {
            int  d_line;           // source line number
            char d_version;
            int  d_numIsGmt;
            int  d_numIsStd;
            int  d_numTransitions;
            int  d_numLocalTimeTypes;
            int  d_abbrevDataSize;
        } DATA[] = {
            // The first row of the table below represents an object value
            // consisting of "baseline" attribute values (A1..A3,A5..An).  Each
            // subsequent row differs (slightly) from the first in exactly one
            // attribute value (Bi).  Note that 'numLeaps' (A4) is excluded be
            // it must always be 0.

            //LINE VER NISGMT NISSTD NTRANS NLTT ABBRSZ
            //---- --- ------ ------ ------ ---- ------

            { L_,  A1, A2,    A3,    A5,    A6,  A7    },  // baseline

            { L_,  B1, A2,    A3,    A5,    A6,  A7    },
            { L_,  A1, B2,    A3,    A5,    A6,  A7    },
            { L_,  A1, A2,    B3,    A5,    A6,  A7    },
            { L_,  A1, A2,    A3,    B5,    A6,  A7    },
            { L_,  A1, A2,    A3,    A5,    B6,  A7    },
            { L_,  A1, A2,    A3,    A5,    A6,  B7    },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nCompare every value with every value." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  LINE1   = DATA[ti].d_line;
            const char VER1    = DATA[ti].d_version;
            const int  NISGMT1 = DATA[ti].d_numIsGmt;
            const int  NISSTD1 = DATA[ti].d_numIsStd;
            const int  NTRANS1 = DATA[ti].d_numTransitions;
            const int  NLTT1   = DATA[ti].d_numLocalTimeTypes;
            const int  ABBRSZ1 = DATA[ti].d_abbrevDataSize;

            if (veryVerbose) { T_ P_(LINE1)
                                  P_((int)VER1)
                                  P_(NISGMT1)
                                  P_(NISSTD1)
                                  P_(NTRANS1)
                                  P_(NLTT1)
                                  P(ABBRSZ1)
                             }

            // Ensure an object compares correctly with itself (alias test).
            {
                const Obj X(VER1,
                            NISGMT1,
                            NISSTD1,
                            NLEAPS,
                            NTRANS1,
                            NLTT1,
                            ABBRSZ1);

                LOOP2_ASSERT(LINE1, X,   X == X);
                LOOP2_ASSERT(LINE1, X, !(X != X));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int  LINE2   = DATA[tj].d_line;
                const char VER2    = DATA[tj].d_version;
                const int  NISGMT2 = DATA[tj].d_numIsGmt;
                const int  NISSTD2 = DATA[tj].d_numIsStd;
                const int  NTRANS2 = DATA[tj].d_numTransitions;
                const int  NLTT2   = DATA[tj].d_numLocalTimeTypes;
                const int  ABBRSZ2 = DATA[tj].d_abbrevDataSize;

                if (veryVerbose) { T_ P_(LINE2)
                                      P_((int)VER2)
                                      P_(NISGMT2)
                                      P_(NISSTD2)
                                      P_(NTRANS2)
                                      P_(NLTT2)
                                      P(ABBRSZ2)
                                 }

                const bool EXP = ti == tj;  // expected for equality comparison

                const Obj X(VER1,
                            NISGMT1,
                            NISSTD1,
                            NLEAPS,
                            NTRANS1,
                            NLTT1,
                            ABBRSZ1);

                const Obj Y(VER2,
                            NISGMT2,
                            NISSTD2,
                            NLEAPS,
                            NTRANS2,
                            NLTT2,
                            ABBRSZ2);

                if (veryVerbose) {
                    T_ T_ T_ P_(ti) P_(tj) P_(EXP) P_(X) P(Y) }

                // Verify value, commutativity, and no memory allocation.

                TestAllocatorMonitor dam(da);

                LOOP5_ASSERT(LINE1, LINE2, EXP, X, Y,  EXP == (X == Y));
                LOOP5_ASSERT(LINE1, LINE2, EXP, Y, X,  EXP == (Y == X));

                LOOP5_ASSERT(LINE1, LINE2, EXP, X, Y, !EXP == (X != Y));
                LOOP5_ASSERT(LINE1, LINE2, EXP, Y, X, !EXP == (Y != X));

                LOOP2_ASSERT(LINE1, LINE2, dam.isTotalSame());
            }

            LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
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
        //: 6 The output 'operator<<' signature and return type are standard.
        //:
        //: 7 The output 'operator<<' returns the supplied 'ostream'.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-4)
        //:
        //: 2 Using the table-driven technique:  (C-1..3, 5, 7)
        //:
        //:   1 Define twelve carefully selected combinations of (two) object
        //:     values ('A' and 'B'), having distinct values for each
        //:     corresponding salient attribute, and various values for the
        //:     two formatting parameters, along with the expected output
        //:     ( 'value' x  'level'   x 'spacesPerLevel' ):
        //:     1 { A   } x {  0     } x {  0, 1, -1 }  -->  3 expected outputs
        //:     2 { A   } x {  3, -3 } x {  0, 2, -2 }  -->  6 expected outputs
        //:     3 { B   } x {  2     } x {  3        }  -->  1 expected output
        //:     4 { A B } x { -9     } x { -9        }  -->  2 expected output
        //:
        //:   2 For each row in the table defined in P-2.1:  (C-1..3, 5, 7)
        //:
        //:     1 Using a 'const' 'Obj', supply each object value and pair of
        //:       formatting parameters to 'print', unless the parameters are,
        //:       arbitrarily, (-9, -9), in which case 'operator<<' will be
        //:       invoked instead.
        //:
        //:     2 Use a standard 'ostringstream' to capture the actual output.
        //:
        //:     3 Verify the address of what is returned is that of the
        //:       supplied stream.  (C-5, 7)
        //:
        //:     4 Compare the contents captured in P-2.2.2 with what is
        //:       expected.  (C-1..3)
        //
        // Testing:
        //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
        //   operator<<(ostream& s, const baltzo::LocalTimeDescriptor& d);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR" << endl
                          << "=========================" << endl;

        if (verbose) cout << "\nAssign the addresses of 'print' and "
                             "the output 'operator<<' to variables." << endl;
        {
            using namespace baltzo;
            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Obj::print;
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

            char        d_version;
            int         d_numIsGmt;
            int         d_numIsStd;
            int         d_numLeaps;
            int         d_numTransitions;
            int         d_numLocalTimeTypes;
            int         d_abbrevDataSize;

            const char *d_expected_p;
        } DATA[] = {
#define NL "\n"
#define SP " "
        //LINE LVL SPL  VER NISGMT NISSTD NLEAPS NTRANS NLTT ABBRSZ \
        //---- --- ---  --- ------ ------ ------ ------ ---- ------ \
        //                                          EXPECTED
        //                                          --------

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1 }  -->  3 expected outputs
        // ------------------------------------------------------------------

        { L_,   0,  0, '\0',    11,    12,     0,    13,  14,    15,
                                                    "["                      NL
                                                    "version = 0"            NL
                                                    "numIsGmt = 11"          NL
                                                    "numIsStd = 12"          NL
                                                    "numLeaps = 0"           NL
                                                    "numTransitions = 13"    NL
                                                    "numLocalTimeTypes = 14" NL
                                                    "abbrevDataSize = 15"    NL
                                                    "]"                      NL
                                                                             },
        { L_,   0,  1, '\0',    11,    12,     0,    13,  14,    15,
                                                   "["                       NL
                                                   " version = 0"            NL
                                                   " numIsGmt = 11"          NL
                                                   " numIsStd = 12"          NL
                                                   " numLeaps = 0"           NL
                                                   " numTransitions = 13"    NL
                                                   " numLocalTimeTypes = 14" NL
                                                   " abbrevDataSize = 15"    NL
                                                   "]"                       NL
                                                                             },
        { L_,   0, -1, '\0',    11,    12,     0,    13,  14,    15,
                                                    "["                      SP
                                                    "version = 0"            SP
                                                    "numIsGmt = 11"          SP
                                                    "numIsStd = 12"          SP
                                                    "numLeaps = 0"           SP
                                                    "numTransitions = 13"    SP
                                                    "numLocalTimeTypes = 14" SP
                                                    "abbrevDataSize = 15"    SP
                                                    "]"
                                                                             },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2 }  -->  6 expected outputs
        // ------------------------------------------------------------------

        { L_,   3,  0, '\0',    11,    12,     0,    13,  14,    15,
                                                    "["                      NL
                                                    "version = 0"            NL
                                                    "numIsGmt = 11"          NL
                                                    "numIsStd = 12"          NL
                                                    "numLeaps = 0"           NL
                                                    "numTransitions = 13"    NL
                                                    "numLocalTimeTypes = 14" NL
                                                    "abbrevDataSize = 15"    NL
                                                    "]"                      NL
                                                                             },
        { L_,   3,  2, '\0',    11,    12,     0,    13,  14,    15,
                                            "      ["                        NL
                                            "        version = 0"            NL
                                            "        numIsGmt = 11"          NL
                                            "        numIsStd = 12"          NL
                                            "        numLeaps = 0"           NL
                                            "        numTransitions = 13"    NL
                                            "        numLocalTimeTypes = 14" NL
                                            "        abbrevDataSize = 15"    NL
                                            "      ]"                        NL
                                                                             },
        { L_,   3, -2, '\0',    11,    12,     0,    13,  14,    15,
                                                   "      ["                 SP
                                                   "version = 0"             SP
                                                   "numIsGmt = 11"           SP
                                                   "numIsStd = 12"           SP
                                                   "numLeaps = 0"            SP
                                                   "numTransitions = 13"     SP
                                                   "numLocalTimeTypes = 14"  SP
                                                   "abbrevDataSize = 15"     SP
                                                   "]"
                                                                             },
        { L_,  -3,  0, '\0',    11,    12,     0,    13,  14,    15,
                                                    "["                      NL
                                                    "version = 0"            NL
                                                    "numIsGmt = 11"          NL
                                                    "numIsStd = 12"          NL
                                                    "numLeaps = 0"           NL
                                                    "numTransitions = 13"    NL
                                                    "numLocalTimeTypes = 14" NL
                                                    "abbrevDataSize = 15"    NL
                                                    "]"                      NL
                                                                             },
        { L_,  -3,  2, '\0',    11,    12,     0,    13,  14,    15,
                                            "["                              NL
                                            "        version = 0"            NL
                                            "        numIsGmt = 11"          NL
                                            "        numIsStd = 12"          NL
                                            "        numLeaps = 0"           NL
                                            "        numTransitions = 13"    NL
                                            "        numLocalTimeTypes = 14" NL
                                            "        abbrevDataSize = 15"    NL
                                            "      ]"                        NL
                                                                             },
        { L_,  -3, -2, '\0',    11,    12,     0,    13,  14,    15,
                                                   "["                       SP
                                                   "version = 0"             SP
                                                   "numIsGmt = 11"           SP
                                                   "numIsStd = 12"           SP
                                                   "numLeaps = 0"            SP
                                                   "numTransitions = 13"     SP
                                                   "numLocalTimeTypes = 14"  SP
                                                   "abbrevDataSize = 15"     SP
                                                   "]"
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 }     x { 3 }         -->  1 expected output
        // -----------------------------------------------------------------

        { L_,   2,  3,  '2',    21,    22,     0,    23,  24,    25,
                                           "      ["                         NL
                                           "         version = 50"           NL
                                           "         numIsGmt = 21"          NL
                                           "         numIsStd = 22"          NL
                                           "         numLeaps = 0"           NL
                                           "         numTransitions = 23"    NL
                                           "         numLocalTimeTypes = 24" NL
                                           "         abbrevDataSize = 25"    NL
                                           "      ]"                         NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -9 }   x { -9 }      -->  2 expected outputs
        // -----------------------------------------------------------------

        { L_,  -9, -9, '\0',    11,    12,     0,    13,  14,    15,
                                                    "["                      SP
                                                    "version = 0"            SP
                                                    "numIsGmt = 11"          SP
                                                    "numIsStd = 12"          SP
                                                    "numLeaps = 0"           SP
                                                    "numTransitions = 13"    SP
                                                    "numLocalTimeTypes = 14" SP
                                                    "abbrevDataSize = 15"    SP
                                                    "]"
                                                                             },
        { L_,  -9, -9,  '2',    21,    22,     0,    23,  24,    25,
                                                    "["                      SP
                                                    "version = 50"           SP
                                                    "numIsGmt = 21"          SP
                                                    "numIsStd = 22"          SP
                                                    "numLeaps = 0"           SP
                                                    "numTransitions = 23"    SP
                                                    "numLocalTimeTypes = 24" SP
                                                    "abbrevDataSize = 25"    SP
                                                    "]"
                                                                             },
#undef NL
#undef SP
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const int         LVL    = DATA[ti].d_level;
                const int         SPL    = DATA[ti].d_spacesPerLevel;
                const char        VER    = DATA[ti].d_version;
                const int         NISGMT = DATA[ti].d_numIsGmt;
                const int         NISSTD = DATA[ti].d_numIsStd;
                const int         NLEAPS = DATA[ti].d_numLeaps;
                const int         NTRANS = DATA[ti].d_numTransitions;
                const int         NLTT   = DATA[ti].d_numLocalTimeTypes;
                const int         ABBRSZ = DATA[ti].d_abbrevDataSize;
                const char *const EXP    = DATA[ti].d_expected_p;

                if (veryVerbose) { T_ P_(LVL) P_(SPL)
                                      P_((int)VER)
                                      P_(NISGMT)
                                      P_(NISSTD)
                                      P_(NLEAPS)
                                      P_(NTRANS)
                                      P_(NLTT)
                                      P(ABBRSZ)
                                 }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                const Obj X(VER,
                            NISGMT,
                            NISSTD,
                            NLEAPS,
                            NTRANS,
                            NLTT,
                            ABBRSZ);

                ostringstream os;

                if (-9 == LVL && -9 == SPL) {

                    // Verify supplied stream is returned by reference.

                    LOOP_ASSERT(LINE, &os == &(os << X));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {

                    // Verify supplied stream is returned by reference.

                    LOOP_ASSERT(LINE, &os == &X.print(os, LVL, SPL));

                    if (veryVeryVerbose) { T_ T_ Q(print) }
                }

                // Verify output is formatted as expected.

                if (veryVeryVerbose) { P(os.str()) }

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
        //: 1 Each accessor returns the value of the corresponding attribute
        //:   of the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //:
        //: 3 No accessor allocates any memory.
        //:
        //: 4 Accessors for attributes that can allocate memory (i.e., those
        //:   that take an allocator in their constructor) return a reference
        //:   providing only non-modifiable access.
        //
        // Plan:
        //   In case 3 we demonstrated that all basic accessors work properly
        //   with respect to attributes initialized by the value constructor.
        //   Here we use the default constructor and primary manipulators,
        //   which were fully tested in case 2, to further corroborate that
        //   these accessors are properly interpreting object state.
        //
        //: 1 Create two 'bslma::TestAllocator' objects, and install one as
        //:   the current default allocator (note that a ubiquitous test
        //:   allocator is already installed as the global allocator).
        //:
        //: 2 Use the default constructor, using the other test allocator
        //:   from P-1, to create an object (having default attribute values).
        //:
        //: 3 Verify that each basic accessor, invoked on a reference providing
        //:   non-modifiable access to the object created in P2, returns the
        //:   expected value.  (C-2)
        //:
        //: 4 For each salient attribute (contributing to value):  (C-1, 3..4)
        //:   1 Use the corresponding primary manipulator to set the attribute
        //:     to a unique value, making sure to allocate memory if possible.
        //:
        //:   2 Use the corresponding basic accessor to verify the new
        //:     expected value.  (C-1)
        //:
        //:   3 Monitor the memory allocated from both the default and object
        //:     allocators before and after calling the accessor; verify that
        //:     there is no change in total memory allocation.  (C-3..4)
        //
        // Testing:
        //   char version() const;
        //   int numIsGmt() const;
        //   int numIsStd() const;
        //   int numLeaps() const;
        //   int numTransitions() const;
        //   int numLocalTimeTypes() const;
        //   int abbrevDataSize() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // -----------------------------------------------------
        // 'D' values: These are the default-constructed values.
        // -----------------------------------------------------

        const T1 D1 = '\0'; // 'version'
        const T2 D2 =  0;   // 'numIsGmt'
        const T3 D3 =  0;   // 'numIsStd'
        const T4 D4 =  0;   // 'numLeaps'
        const T5 D5 =  0;   // 'numTransitions'
        const T6 D6 =  1;   // 'numLocalTimeTypes'
        const T7 D7 =  1;   // 'abbrevDataSize'

        // ----------
        // 'A' values
        // ----------

        const T1 A1 = '2';
        const T2 A2 =  1;
        const T3 A3 =  1;
        const T4 A4 =  0;
        const T5 A5 =  1;
        const T6 A6 =  1;
        const T7 A7 =  1;

        if (verbose) cout <<
           "\nCreate a test allocators; install it as the default." << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);

        bslma::Default::setDefaultAllocatorRaw(&da);

        if (verbose) cout <<
                 "\nCreate an object." << endl;

        Obj mX;  const Obj& X = mX;

        if (verbose) cout <<
                "\nVerify all basic accessors report expected values." << endl;
        {
            const T1& version = X.version();
            LOOP2_ASSERT(D1, version, D1 == version);

            const T2& numIsGmt = X.numIsGmt();
            LOOP2_ASSERT(D2, numIsGmt, D2 == numIsGmt);

            const T3& numIsStd = X.numIsStd();
            LOOP2_ASSERT(D3, numIsStd, D3 == numIsStd);

            const T4& numLeaps = X.numLeaps();
            LOOP2_ASSERT(D4, numLeaps, D4 == numLeaps);

            const T5& numTransitions = X.numTransitions();
            LOOP2_ASSERT(D5, numTransitions,
                         D5 == numTransitions);

            const T6& numLocalTimeTypes = X.numLocalTimeTypes();
            LOOP2_ASSERT(D6, numLocalTimeTypes,
                         D6 == numLocalTimeTypes);

            const T7& abbrevDataSize = X.abbrevDataSize();
            LOOP2_ASSERT(D7, abbrevDataSize,
                         D7 == abbrevDataSize);
        }

        if (verbose) cout <<
            "\nApply primary manipulators and verify expected values." << endl;

        if (veryVerbose) { T_ Q(version) }
        {
            mX.setVersion(A1);

            TestAllocatorMonitor dam(da);

            const T1& version = X.version();
            LOOP2_ASSERT(A1, version, A1 == version);

            ASSERT(dam.isInUseSame());
        }

        if (veryVerbose) { T_ Q(numIsGmt) }
        {
            mX.setNumIsGmt(A2);

            TestAllocatorMonitor dam(da);

            const T2& numIsGmt = X.numIsGmt();
            LOOP2_ASSERT(A2, numIsGmt, A2 == numIsGmt);

            ASSERT(dam.isInUseSame());
        }

        if (veryVerbose) { T_ Q(numIsStd) }
        {
            mX.setNumIsStd(A3);

            TestAllocatorMonitor dam(da);

            const T3& numIsStd = X.numIsStd();
            LOOP2_ASSERT(A3, numIsStd, A3 == numIsStd);

            ASSERT(dam.isInUseSame());
        }

        if (veryVerbose) { T_ Q(numLeaps) }
        {
            mX.setNumLeaps(A4);

            TestAllocatorMonitor dam(da);

            const T4& numLeaps = X.numLeaps();
            LOOP2_ASSERT(A4, numLeaps, A4 == numLeaps);

            ASSERT(dam.isInUseSame());
        }

        if (veryVerbose) { T_ Q(numTransitions) }
        {
            mX.setNumTransitions(A5);

            TestAllocatorMonitor dam(da);

            const T5& numTransitions = X.numTransitions();
            LOOP2_ASSERT(A5, numTransitions, A5 == numTransitions);

            ASSERT(dam.isInUseSame());
        }

        if (veryVerbose) { T_ Q(numLocalTimeTypes) }
        {
            mX.setNumLocalTimeTypes(A6);

            TestAllocatorMonitor dam(da);

            const T6& numLocalTimeTypes = X.numLocalTimeTypes();
            LOOP2_ASSERT(A6, numLocalTimeTypes, A6 == numLocalTimeTypes);

            ASSERT(dam.isInUseSame());
        }

        if (veryVerbose) { T_ Q(abbrevDataSize) }
        {
            mX.setAbbrevDataSize(A7);

            TestAllocatorMonitor dam(da);

            const T7& abbrevDataSize = X.abbrevDataSize();
            LOOP2_ASSERT(A7, abbrevDataSize, A7 == abbrevDataSize);

            ASSERT(dam.isInUseSame());
        }

        // Double check that no default memory was allocated.

        LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // VALUE CTOR Ensure that we can put an object into any initial state
        //   relevant for thorough testing.
        //
        // Concerns:
        //: 1 The value constructor can create an object having any value that
        //:   does not violate the constructor's documented preconditions.
        //:
        //: 2 Any argument can be 'const'.
        //:
        //: 3 There is no temporary memory allocation from any allocator
        //:   allocator.
        //:
        //: 4 There is no memory allocation from the any allocator.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven approach: Specify a set of widely varying
        //:   object values (one per row) in terms of their individual
        //:   attributes, including (a) first, the default value, and (b)
        //:   boundary values corresponding to every range of values that each
        //:   individual attribute can independently attain.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1: (C-1..4)
        //:
        //:     1 Create an 'bslma::TestAllocator' objects and install it as
        //:       the current default allocator (note that a ubiquitous test
        //:       allocator is already installed as the global allocator) and
        //:       another 'bslma::TestAllocator' used for the object footprint.
        //:
        //:     2 Use the value constructor to dynamically create an object
        //:       having the value 'V', supplying all the arguments as 'const',
        //:       configured appropriately (see P-2.1) using the distinct test
        //:       allocator for the object's footprint.
        //:
        //:     3 Use the (as yet unproven) salient attribute accessors to
        //:       verify that all of the attributes of each object have their
        //:       expected values.  (C-1)
        //:
        //:     5 Use the appropriate test allocators to verify that no memory
        //:       is allocated, even temporarily, from the default allocator
        //:       (C-3..4).
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-5)
        //
        // Testing:
        //   baltzo::ZoneinfoBinaryHeader(char, int, int, int, int, int, int);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VALUE CTOR" << endl
                          << "==========" << endl;

        if (verbose) cout << "\nUse a table of distinct object values."
                          << endl;

        const int                    NUM_DATA  = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) cout << "\nCreate objects with various values." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const char LINE   = DATA[ti].d_line;
                const int  VER    = DATA[ti].d_version;
                const int  NISGMT = DATA[ti].d_numIsGmt;
                const int  NISSTD = DATA[ti].d_numIsStd;
                const int  NLEAPS = DATA[ti].d_numLeaps;
                const int  NTRANS = DATA[ti].d_numTransitions;
                const int  NLTT   = DATA[ti].d_numLocalTimeTypes;
                const int  ABBRSZ = DATA[ti].d_abbrevDataSize;

                if (veryVerbose) { T_
                                   P_((int)VER)
                                   P_(NISGMT)
                                   P_(NISSTD)
                                   P_(NLEAPS)
                                   P_(NTRANS)
                                   P_(NLTT)
                                   P(ABBRSZ) }

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);

                bslma::Default::setDefaultAllocatorRaw(&da);

                Obj *objPtr = new (fa) Obj(VER,
                                           NISGMT,
                                           NISSTD,
                                           NLEAPS,
                                           NTRANS,
                                           NLTT,
                                           ABBRSZ);

                LOOP_ASSERT(LINE, sizeof(Obj) == fa.numBytesInUse());

                Obj& mX = *objPtr;  const Obj& X = mX;

                if (veryVerbose) { T_ T_ P_(X) }

                // Use untested functionality to help ensure the first row of
                // the table contains the default-constructed value.

                static bool firstFlag = true;
                if (firstFlag) {
                    LOOP3_ASSERT(LINE, Obj(), *objPtr,
                                 Obj() == *objPtr)
                    firstFlag = false;
                }

                // -------------------------------------
                // Verify the object's attribute values.
                // -------------------------------------

                const T1& version = X.version();
                LOOP2_ASSERT(VER, version, VER == version);

                const T2& numIsGmt = X.numIsGmt();
                LOOP2_ASSERT(NISGMT, numIsGmt, NISGMT == numIsGmt);

                const T3& numIsStd = X.numIsStd();
                LOOP2_ASSERT(NISSTD, numIsStd, NISSTD == numIsStd);

                const T4& numLeaps = X.numLeaps();
                LOOP2_ASSERT(NLEAPS, numLeaps, NLEAPS == numLeaps);

                const T5& numTransitions = X.numTransitions();
                LOOP2_ASSERT(NTRANS, numTransitions,
                             NTRANS == numTransitions);

                const T6& numLocalTimeTypes = X.numLocalTimeTypes();
                LOOP2_ASSERT(NLTT, numLocalTimeTypes,
                             NLTT == numLocalTimeTypes);

                const T7& abbrevDataSize = X.abbrevDataSize();
                LOOP2_ASSERT(ABBRSZ, abbrevDataSize,
                             ABBRSZ == abbrevDataSize);

                // Verify no allocation from the default allocator.

                LOOP2_ASSERT(LINE, da.numBlocksTotal(),
                             0 == da.numBlocksTotal());

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.

                LOOP2_ASSERT(LINE, fa.numBlocksInUse(),
                             0 == fa.numBlocksInUse());
                LOOP2_ASSERT(LINE, da.numBlocksInUse(),
                             0 == da.numBlocksInUse());

            }  // end foreach row
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            const char VER    = '2';
            const int  NISGMT = 1 + 10;
            const int  NISSTD = 2 + 10;
            const int  NLEAPS = 0;
            const int  NTRANS = 3 + 10;
            const int  NLTT   = 4 + 10;
            const int  ABBRSZ = 5 + 10;

            if (veryVeryVerbose) { T_ T_
                                   P_((int)VER)
                                   P_(NISGMT)
                                   P_(NISSTD)
                                   P_(NLEAPS)
                                   P_(NTRANS)
                                   P_(NLTT)
                                   P(ABBRSZ)
                                 }

            ASSERT_SAFE_PASS(Obj(VER,
                                 NISGMT,
                                 NISSTD,
                                 NLEAPS,
                                 NTRANS,
                                 NLTT,
                                 ABBRSZ));

            if (veryVerbose) { T_ Q(version) }
            {
                ASSERT_SAFE_FAIL(Obj('\0' - 1,       // version
                                     NISGMT,
                                     NISSTD,
                                     NLEAPS,
                                     NTRANS,
                                     NLTT,
                                     ABBRSZ));
                ASSERT_SAFE_PASS(Obj('\0',           // version
                                     NISGMT,
                                     NISSTD,
                                     NLEAPS,
                                     NTRANS,
                                     NLTT,
                                     ABBRSZ));
                ASSERT_SAFE_FAIL(Obj('\0' + 1,       // version
                                     NISGMT,
                                     NISSTD,
                                     NLEAPS,
                                     NTRANS,
                                     NLTT,
                                     ABBRSZ));
                ASSERT_SAFE_FAIL(Obj('2' - 1,        // version
                                     NISGMT,
                                     NISSTD,
                                     NLEAPS,
                                     NTRANS,
                                     NLTT,
                                     ABBRSZ));
                ASSERT_SAFE_PASS(Obj('2',            // version
                                     NISGMT,
                                     NISSTD,
                                     NLEAPS,
                                     NTRANS,
                                     NLTT,
                                     ABBRSZ));
                ASSERT_SAFE_FAIL(Obj('2' + 1,        // version
                                     NISGMT,
                                     NISSTD,
                                     NLEAPS,
                                     NTRANS,
                                     NLTT,
                                     ABBRSZ));
            }

            if (veryVerbose) { T_ Q(numIsGmt) }
            {
                ASSERT_SAFE_FAIL(Obj(VER,
                                     0 - 1,          // numIsGmt
                                     NISSTD,
                                     NLEAPS,
                                     NTRANS,
                                     NLTT,
                                     ABBRSZ));
                ASSERT_SAFE_PASS(Obj(VER,
                                     0,              // numIsGmt
                                     NISSTD,
                                     NLEAPS,
                                     NTRANS,
                                     NLTT,
                                     ABBRSZ));
                ASSERT_SAFE_PASS(Obj(VER,
                                     0 + 1,          // numIsGmt
                                     NISSTD,
                                     NLEAPS,
                                     NTRANS,
                                     NLTT,
                                     ABBRSZ));
                ASSERT_SAFE_PASS(Obj(VER,
                                     INT_MAX - 1,    // numIsGmt
                                     NISSTD,
                                     NLEAPS,
                                     NTRANS,
                                     NLTT,
                                     ABBRSZ));
                ASSERT_SAFE_PASS(Obj(VER,
                                     INT_MAX,        // numIsGmt
                                     NISSTD,
                                     NLEAPS,
                                     NTRANS,
                                     NLTT,
                                     ABBRSZ));
            }

            if (veryVerbose) { T_ Q(numIsStd) }
            {
                ASSERT_SAFE_FAIL(Obj(VER,
                                     NISGMT,
                                     0 - 1,          // numIsStd
                                     NLEAPS,
                                     NTRANS,
                                     NLTT,
                                     ABBRSZ));
                ASSERT_SAFE_PASS(Obj(VER,
                                     NISGMT,
                                     0,              // numIsStd
                                     NLEAPS,
                                     NTRANS,
                                     NLTT,
                                     ABBRSZ));
                ASSERT_SAFE_PASS(Obj(VER,
                                     NISGMT,
                                     0 + 1,          // numIsStd
                                     NLEAPS,
                                     NTRANS,
                                     NLTT,
                                     ABBRSZ));
                ASSERT_SAFE_PASS(Obj(VER,
                                     NISGMT,
                                     INT_MAX - 1,    // numIsStd
                                     NLEAPS,
                                     NTRANS,
                                     NLTT,
                                     ABBRSZ));
                ASSERT_SAFE_PASS(Obj(VER,
                                     NISGMT,
                                     INT_MAX,        // numIsStd
                                     NLEAPS,
                                     NTRANS,
                                     NLTT,
                                     ABBRSZ));
            }

            if (veryVerbose) { T_ Q(numLeaps) }
            {
                ASSERT_SAFE_FAIL(Obj(VER,
                                     NISGMT,
                                     NISSTD,
                                     0 - 1,          // numLeaps
                                     NTRANS,
                                     NLTT,
                                     ABBRSZ));
                ASSERT_SAFE_PASS(Obj(VER,
                                     NISGMT,
                                     NISSTD,
                                     0,              // numLeaps
                                     NTRANS,
                                     NLTT,
                                     ABBRSZ));
                ASSERT_SAFE_FAIL(Obj(VER,
                                     NISGMT,
                                     NISSTD,
                                     0 + 1,          // numLeaps
                                     NTRANS,
                                     NLTT,
                                     ABBRSZ));
            }

            if (veryVerbose) { T_ Q(numTransitions) }
            {
                ASSERT_SAFE_FAIL(Obj(VER,
                                     NISGMT,
                                     NISSTD,
                                     NLEAPS,
                                     0 - 1,          // numTransitions
                                     NLTT,
                                     ABBRSZ));
                ASSERT_SAFE_PASS(Obj(VER,
                                     NISGMT,
                                     NISSTD,
                                     NLEAPS,
                                     0,              // numTransitions
                                     NLTT,
                                     ABBRSZ));
                ASSERT_SAFE_PASS(Obj(VER,
                                     NISGMT,
                                     NISSTD,
                                     NLEAPS,
                                     0 + 1,          // numTransitions
                                     NLTT,
                                     ABBRSZ));
                ASSERT_SAFE_PASS(Obj(VER,
                                     NISGMT,
                                     NISSTD,
                                     NLEAPS,
                                     INT_MAX - 1,    // numTransitions
                                     NLTT,
                                     ABBRSZ));
                ASSERT_SAFE_PASS(Obj(VER,
                                     NISGMT,
                                     NISSTD,
                                     NLEAPS,
                                     INT_MAX,        // numTransitions
                                     NLTT,
                                     ABBRSZ));
            }

            if (veryVerbose) { T_ Q(numLocalTimeTypes) }
            {
                ASSERT_SAFE_FAIL(Obj(VER,
                                     NISGMT,
                                     NISSTD,
                                     NLEAPS,
                                     NTRANS,
                                     1 - 1,          // numLocalTimeTypes
                                     ABBRSZ));
                ASSERT_SAFE_PASS(Obj(VER,
                                     NISGMT,
                                     NISSTD,
                                     NLEAPS,
                                     NTRANS,
                                     1,              // numLocalTimeTypes
                                     ABBRSZ));
                ASSERT_SAFE_PASS(Obj(VER,
                                     NISGMT,
                                     NISSTD,
                                     NLEAPS,
                                     NTRANS,
                                     1 + 1,          // numLocalTimeTypes
                                     ABBRSZ));
                ASSERT_SAFE_PASS(Obj(VER,
                                     NISGMT,
                                     NISSTD,
                                     NLEAPS,
                                     NTRANS,
                                     INT_MAX - 1,    // numLocalTimeTypes
                                     ABBRSZ));
                ASSERT_SAFE_PASS(Obj(VER,
                                     NISGMT,
                                     NISSTD,
                                     NLEAPS,
                                     NTRANS,
                                     INT_MAX,        // numLocalTimeTypes
                                     ABBRSZ));
            }

            if (veryVerbose) { T_ Q(abbrevDataSize) }
            {
                ASSERT_SAFE_FAIL(Obj(VER,
                                     NISGMT,
                                     NISSTD,
                                     NLEAPS,
                                     NTRANS,
                                     NLTT,
                                     1 - 1));        // abbrevDataSize
                ASSERT_SAFE_PASS(Obj(VER,
                                     NISGMT,
                                     NISSTD,
                                     NLEAPS,
                                     NTRANS,
                                     NLTT,
                                     1));            // abbrevDataSize
                ASSERT_SAFE_PASS(Obj(VER,
                                     NISGMT,
                                     NISSTD,
                                     NLEAPS,
                                     NTRANS,
                                     NLTT,
                                     1 + 1));        // abbrevDataSize
                ASSERT_SAFE_PASS(Obj(VER,
                                     NISGMT,
                                     NISSTD,
                                     NLEAPS,
                                     NTRANS,
                                     NLTT,
                                     INT_MAX - 1));  // abbrevDataSize
                ASSERT_SAFE_PASS(Obj(VER,
                                     NISGMT,
                                     NISSTD,
                                     NLEAPS,
                                     NTRANS,
                                     NLTT,
                                     INT_MAX));      // abbrevDataSize
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR Ensure that we can use
        //   the default constructor to create an object (having the
        //   default-constructed value), use the primary manipulators to put
        //   that object into any state relevant for thorough testing, and use
        //   the destructor to destroy it safely.
        //
        // Concerns:
        //: 1 An object created with the default constructor (with or without a
        //:   supplied allocator) has the contractually specified default
        //:   value.
        //:
        //: 2 There is no temporary allocation from any allocator.
        //:
        //: 3 Each attribute is modifiable independently.
        //:
        //: 4 Each attribute can be set to represent any value that does not
        //:   violate that attribute's documented constraints.
        //:
        //: 5 Any argument can be 'const'.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create three sets of attribute values for the object: ('D')
        //:   values corresponding to the default-constructed object, ('A')
        //:   values that allocate memory if possible, and ('B') other values
        //:   that do not cause additional memory allocation beyond that which
        //:   may be incurred by 'A'.  Both the 'A' and 'B' attribute values
        //:   should be chosen to be boundary values where possible.  If an
        //:   attribute can be supplied via alternate C++ types (e.g., 'string'
        //:   instead of 'char *'), use the alternate type for 'B'.
        //:
        //: 2 Since objects of this class do not take an allocator, use a
        //:   simplified form of the standard plan for simply constrained
        //:   attribute types.  (C-1..6)
        //:
        //:   1 Create two 'bslma::TestAllocator' objects, and install one as
        //:     as the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Use the default constructor to dynamically create an object,
        //:     configured appropriately (see P-2) using a distinct test
        //:     allocator for the object's footprint.
        //:
        //:   4 Check the default allocator to verify that no memory is
        //:     allocated by the default constructor.  (C-2)
        //:
        //:   5 Use the individual (as yet unproven) salient attribute
        //:     accessors to verify the default-constructed value.  (C-1)
        //:
        //:   6 For each attribute 'i', in turn, create a local block.  Then
        //:     inside the block, using brute force, set that attribute's
        //:     value, passing a 'const' argument representing each of the
        //:     three test values, in turn (see P-1), first to 'Ai', then to
        //:     ('Di' -> 'Ai'), and that the corresponding primary manipulator.
        //:     After each transition, use the (as yet unproven) basic
        //:     accessors to verify that only the intended attribute value
        //:     changed.  (C-4..5)
        //:
        //:   7 Corroborate that attributes are modifiable independently by
        //:     first setting all of the attributes to their 'A' values, then
        //:     setting all of the attributes to their 'B' values.  (C-3)
        //:
        //:   8 Verify that no (temporary) memory is allocated from the default
        //:     allocator.  (C-2)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-6)
        //
        // Testing:
        //   baltzo::ZoneinfoBinaryHeader();
        //   ~baltzo::ZoneinfoBinaryHeader();
        //   void setVersion(char value);
        //   void setNumIsGmt(int value);
        //   void setNumIsStd(int value);
        //   void setNumLeaps(int value);
        //   void setNumTransitions(int value);
        //   void setNumLocalTimeTypes(int value);
        //   void setAbbrevDataSize(int value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                       << "DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR" << endl
                       << "==========================================" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // 'D' values: These are the default-constructed values.

        const char D1   = '\0';     // 'version'
        const int  D2   =  0;       // 'numIsGmt'
        const int  D3   =  0;       // 'numIsStd'
        const int  D4   =  0;       // 'numLeaps'
        const int  D5   =  0;       // 'numTransitions'
        const int  D6   =  1;       // 'numLocalTimeTypes'
        const int  D7   =  1;       // 'abbrevDataSize'

        // 'A' values

        const char A1   = '2';
        const int  A2   =  1;
        const int  A3   =  1;
        const int  A4   =  0;
        const int  A5   =  1;
        const int  A6   =  1;
        const int  A7   =  1;

        // 'B' values

        const char B1   = '\0';
        const int  B2   =  INT_MAX;
        const int  B3   =  INT_MAX;
        const int  B4   =  0;
        const int  B5   =  INT_MAX;
        const int  B6   =  INT_MAX;
        const int  B7   =  INT_MAX;

        if (verbose) cout << "\nTesting with various allocator configurations."
                          << endl;

        bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
        bslma::TestAllocator da("default",   veryVeryVeryVerbose);

        bslma::Default::setDefaultAllocatorRaw(&da);

        Obj                   *objPtr = new (fa) Obj();
        Obj&                   mX     = *objPtr;  const Obj& X = mX;
        bslma::TestAllocator&  noa     =  da;

        // Verify no allocation from the default allocator.

        LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());

        // -------------------------------------
        // Verify the object's attribute values.
        // -------------------------------------

        LOOP2_ASSERT(D1, X.version(),           D1 == X.version());
        LOOP2_ASSERT(D2, X.numIsGmt(),          D2 == X.numIsGmt());
        LOOP2_ASSERT(D3, X.numIsStd(),          D3 == X.numIsStd());
        LOOP2_ASSERT(D1, X.numLeaps(),          D4 == X.numLeaps());
        LOOP2_ASSERT(D2, X.numTransitions(),    D5 == X.numTransitions());
        LOOP2_ASSERT(D3, X.numLocalTimeTypes(), D6 == X.numLocalTimeTypes());
        LOOP2_ASSERT(D3, X.abbrevDataSize(),    D7 == X.abbrevDataSize());

        // -----------------------------------------------------
        // Verify that each attribute is independently settable.
        // -----------------------------------------------------

        if (veryVerbose) { T_ Q(version) }
        {
            TestAllocatorMonitor tam(da);

            mX.setVersion(A1);
            ASSERT(A1 == X.version());           // version
            ASSERT(D2 == X.numIsGmt());
            ASSERT(D3 == X.numIsStd());
            ASSERT(D4 == X.numLeaps());
            ASSERT(D5 == X.numTransitions());
            ASSERT(D6 == X.numLocalTimeTypes());
            ASSERT(D7 == X.abbrevDataSize());

            mX.setVersion(B1);
            ASSERT(B1 == X.version());           // version
            ASSERT(D2 == X.numIsGmt());
            ASSERT(D3 == X.numIsStd());
            ASSERT(D4 == X.numLeaps());
            ASSERT(D5 == X.numTransitions());
            ASSERT(D6 == X.numLocalTimeTypes());
            ASSERT(D7 == X.abbrevDataSize());

            mX.setVersion(D1);
            ASSERT(D1 == X.version());           // version
            ASSERT(D2 == X.numIsGmt());
            ASSERT(D3 == X.numIsStd());
            ASSERT(D4 == X.numLeaps());
            ASSERT(D5 == X.numTransitions());
            ASSERT(D6 == X.numLocalTimeTypes());
            ASSERT(D7 == X.abbrevDataSize());

            ASSERT(tam.isTotalSame());
        }

        if (veryVerbose) { T_ Q(numIsGmt) }
        {
            TestAllocatorMonitor tam(da);

            mX.setNumIsGmt(A2);
            ASSERT(D1 == X.version());
            ASSERT(A2 == X.numIsGmt());           // numIsGmt
            ASSERT(D3 == X.numIsStd());
            ASSERT(D4 == X.numLeaps());
            ASSERT(D5 == X.numTransitions());
            ASSERT(D6 == X.numLocalTimeTypes());
            ASSERT(D7 == X.abbrevDataSize());

            mX.setNumIsGmt(B2);
            ASSERT(D1 == X.version());
            ASSERT(B2 == X.numIsGmt());           // numIsGmt
            ASSERT(D3 == X.numIsStd());
            ASSERT(D4 == X.numLeaps());
            ASSERT(D5 == X.numTransitions());
            ASSERT(D6 == X.numLocalTimeTypes());
            ASSERT(D7 == X.abbrevDataSize());

            mX.setNumIsGmt(D2);
            ASSERT(D1 == X.version());
            ASSERT(D2 == X.numIsGmt());           // numIsGmt
            ASSERT(D3 == X.numIsStd());
            ASSERT(D4 == X.numLeaps());
            ASSERT(D5 == X.numTransitions());
            ASSERT(D6 == X.numLocalTimeTypes());
            ASSERT(D7 == X.abbrevDataSize());

            ASSERT(tam.isTotalSame());
        }

        if (veryVerbose) { T_ Q(numIsStd) }
        {
            TestAllocatorMonitor tam(da);

            mX.setNumIsStd(A3);
            ASSERT(D1 == X.version());
            ASSERT(D2 == X.numIsGmt());
            ASSERT(A3 == X.numIsStd());           // numIsStd
            ASSERT(D4 == X.numLeaps());
            ASSERT(D5 == X.numTransitions());
            ASSERT(D6 == X.numLocalTimeTypes());
            ASSERT(D7 == X.abbrevDataSize());

            mX.setNumIsStd(B3);
            ASSERT(D1 == X.version());
            ASSERT(D2 == X.numIsGmt());
            ASSERT(B3 == X.numIsStd());           // numIsStd
            ASSERT(D4 == X.numLeaps());
            ASSERT(D5 == X.numTransitions());
            ASSERT(D6 == X.numLocalTimeTypes());
            ASSERT(D7 == X.abbrevDataSize());

            mX.setNumIsStd(D3);
            ASSERT(D1 == X.version());
            ASSERT(D2 == X.numIsGmt());
            ASSERT(D3 == X.numIsStd());           // numIsStd
            ASSERT(D4 == X.numLeaps());
            ASSERT(D5 == X.numTransitions());
            ASSERT(D6 == X.numLocalTimeTypes());
            ASSERT(D7 == X.abbrevDataSize());

            ASSERT(tam.isTotalSame());
        }

        if (veryVerbose) { T_ Q(numLeaps) }
        {
            TestAllocatorMonitor tam(da);

            mX.setNumLeaps(A4);
            ASSERT(D1 == X.version());
            ASSERT(D2 == X.numIsGmt());
            ASSERT(D3 == X.numIsStd());
            ASSERT(A4 == X.numLeaps());           // numLeaps
            ASSERT(D5 == X.numTransitions());
            ASSERT(D6 == X.numLocalTimeTypes());
            ASSERT(D7 == X.abbrevDataSize());

            mX.setNumLeaps(B4);
            ASSERT(D1 == X.version());
            ASSERT(D2 == X.numIsGmt());
            ASSERT(D3 == X.numIsStd());
            ASSERT(B4 == X.numLeaps());           // numLeaps
            ASSERT(D5 == X.numTransitions());
            ASSERT(D6 == X.numLocalTimeTypes());
            ASSERT(D7 == X.abbrevDataSize());

            mX.setNumLeaps(D4);
            ASSERT(D1 == X.version());
            ASSERT(D2 == X.numIsGmt());
            ASSERT(D3 == X.numIsStd());
            ASSERT(D4 == X.numLeaps());           // numLeaps
            ASSERT(D5 == X.numTransitions());
            ASSERT(D6 == X.numLocalTimeTypes());
            ASSERT(D7 == X.abbrevDataSize());

            ASSERT(tam.isTotalSame());
        }

        if (veryVerbose) { T_ Q(numTransitions) }
        {
            TestAllocatorMonitor tam(da);

            mX.setNumTransitions(A5);
            ASSERT(D1 == X.version());
            ASSERT(D2 == X.numIsGmt());
            ASSERT(D3 == X.numIsStd());
            ASSERT(D4 == X.numLeaps());
            ASSERT(A5 == X.numTransitions());     // numTransitions
            ASSERT(D6 == X.numLocalTimeTypes());
            ASSERT(D7 == X.abbrevDataSize());

            mX.setNumTransitions(B5);
            ASSERT(D1 == X.version());
            ASSERT(D2 == X.numIsGmt());
            ASSERT(D3 == X.numIsStd());
            ASSERT(D4 == X.numLeaps());
            ASSERT(B5 == X.numTransitions());     // numTransitions
            ASSERT(D6 == X.numLocalTimeTypes());
            ASSERT(D7 == X.abbrevDataSize());

            mX.setNumTransitions(D5);
            ASSERT(D1 == X.version());
            ASSERT(D2 == X.numIsGmt());
            ASSERT(D3 == X.numIsStd());
            ASSERT(D4 == X.numLeaps());
            ASSERT(D5 == X.numTransitions());     // numTransitions
            ASSERT(D6 == X.numLocalTimeTypes());
            ASSERT(D7 == X.abbrevDataSize());

            ASSERT(tam.isTotalSame());
        }

        if (veryVerbose) { T_ Q(numLocalTimeTypes) }
        {
            TestAllocatorMonitor tam(da);

            mX.setNumLocalTimeTypes(A6);
            ASSERT(D1 == X.version());
            ASSERT(D2 == X.numIsGmt());
            ASSERT(D3 == X.numIsStd());
            ASSERT(D4 == X.numLeaps());
            ASSERT(D5 == X.numTransitions());
            ASSERT(A6 == X.numLocalTimeTypes());  // numLocalTimeTypes
            ASSERT(D7 == X.abbrevDataSize());

            mX.setNumLocalTimeTypes(B6);
            ASSERT(D1 == X.version());
            ASSERT(D2 == X.numIsGmt());
            ASSERT(D3 == X.numIsStd());
            ASSERT(D4 == X.numLeaps());
            ASSERT(D5 == X.numTransitions());
            ASSERT(B6 == X.numLocalTimeTypes());  // numLocalTimeTypes
            ASSERT(D7 == X.abbrevDataSize());

            mX.setNumLocalTimeTypes(D6);
            ASSERT(D1 == X.version());
            ASSERT(D2 == X.numIsGmt());
            ASSERT(D3 == X.numIsStd());
            ASSERT(D4 == X.numLeaps());
            ASSERT(D5 == X.numTransitions());
            ASSERT(D6 == X.numLocalTimeTypes());  // numLocalTimeTypes
            ASSERT(D7 == X.abbrevDataSize());

            ASSERT(tam.isTotalSame());
        }

        if (veryVerbose) { T_ Q(abbrevDataSize) }
        {
            TestAllocatorMonitor tam(da);

            mX.setAbbrevDataSize(A7);
            ASSERT(D1 == X.version());
            ASSERT(D2 == X.numIsGmt());
            ASSERT(D3 == X.numIsStd());
            ASSERT(D4 == X.numLeaps());
            ASSERT(D5 == X.numTransitions());
            ASSERT(D6 == X.numLocalTimeTypes());
            ASSERT(A7 == X.abbrevDataSize());     // abbrevDataSize

            mX.setAbbrevDataSize(B7);
            ASSERT(D1 == X.version());
            ASSERT(D2 == X.numIsGmt());
            ASSERT(D3 == X.numIsStd());
            ASSERT(D4 == X.numLeaps());
            ASSERT(D5 == X.numTransitions());
            ASSERT(D6 == X.numLocalTimeTypes());
            ASSERT(B7 == X.abbrevDataSize());     // abbrevDataSize

            mX.setAbbrevDataSize(D7);
            ASSERT(D1 == X.version());
            ASSERT(D2 == X.numIsGmt());
            ASSERT(D3 == X.numIsStd());
            ASSERT(D4 == X.numLeaps());
            ASSERT(D5 == X.numTransitions());
            ASSERT(D6 == X.numLocalTimeTypes());
            ASSERT(D7 == X.abbrevDataSize());     // abbrevDataSize

            ASSERT(tam.isTotalSame());
        }

        // Corroborate attribute independence.
        {
            TestAllocatorMonitor tam(da);

            // Set all attributes to their 'A' values.

            mX.setVersion(A1);
            mX.setNumIsGmt(A2);
            mX.setNumIsStd(A3);
            mX.setNumLeaps(A4);
            mX.setNumTransitions(A5);
            mX.setNumLocalTimeTypes(A6);
            mX.setAbbrevDataSize(A7);

            ASSERT(A1 == X.version());
            ASSERT(A2 == X.numIsGmt());
            ASSERT(A3 == X.numIsStd());
            ASSERT(A4 == X.numLeaps());
            ASSERT(A5 == X.numTransitions());
            ASSERT(A6 == X.numLocalTimeTypes());
            ASSERT(A7 == X.abbrevDataSize());

            // Set all attributes to their 'B' values.

            mX.setVersion(B1);
            mX.setNumIsGmt(B2);
            mX.setNumIsStd(B3);
            mX.setNumLeaps(B4);
            mX.setNumTransitions(B5);
            mX.setNumLocalTimeTypes(B6);
            mX.setAbbrevDataSize(B7);

            ASSERT(B1 == X.version());
            ASSERT(B2 == X.numIsGmt());
            ASSERT(B3 == X.numIsStd());
            ASSERT(B4 == X.numLeaps());
            ASSERT(B5 == X.numTransitions());
            ASSERT(B6 == X.numLocalTimeTypes());
            ASSERT(B7 == X.abbrevDataSize());

            ASSERT(tam.isTotalSame());
        }

        // Verify no temporary memory is allocated from the default allocator.

        LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());

        // Reclaim dynamically allocated object under test.

        fa.deleteObject(objPtr);

        // Verify all memory is released on object destruction.

        LOOP_ASSERT(fa.numBlocksInUse(), 0 == fa.numBlocksInUse());
        LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj obj;

            if (veryVerbose) cout << "\tversion" << endl;
            {
                ASSERT_SAFE_FAIL(obj.setVersion('\0' - 1));
                ASSERT_SAFE_PASS(obj.setVersion('\0'    ));
                ASSERT_SAFE_FAIL(obj.setVersion('\0' + 1));

                ASSERT_SAFE_FAIL(obj.setVersion('2' - 1));
                ASSERT_SAFE_PASS(obj.setVersion('2'    ));
                ASSERT_SAFE_FAIL(obj.setVersion('2' + 1));
            }

            if (veryVerbose) cout << "\tnumIsGmt" << endl;
            {
                ASSERT_SAFE_FAIL(obj.setNumIsGmt(0 - 1));
                ASSERT_SAFE_PASS(obj.setNumIsGmt(0    ));
                ASSERT_SAFE_PASS(obj.setNumIsGmt(0 + 1));

                ASSERT_SAFE_PASS(obj.setNumIsGmt(INT_MAX - 1));
                ASSERT_SAFE_PASS(obj.setNumIsGmt(INT_MAX    ));
            }

            if (veryVerbose) cout << "\tnumIsStd" << endl;
            {
                ASSERT_SAFE_FAIL(obj.setNumIsStd(0 - 1));
                ASSERT_SAFE_PASS(obj.setNumIsStd(0    ));
                ASSERT_SAFE_PASS(obj.setNumIsStd(0 + 1));

                ASSERT_SAFE_PASS(obj.setNumIsStd(INT_MAX - 1));
                ASSERT_SAFE_PASS(obj.setNumIsStd(INT_MAX    ));
            }

            if (veryVerbose) cout << "\tnumLeaps" << endl;
            {
                ASSERT_SAFE_FAIL(obj.setNumLeaps(0 - 1));
                ASSERT_SAFE_PASS(obj.setNumLeaps(0    ));
                ASSERT_SAFE_FAIL(obj.setNumLeaps(0 + 1));
            }

            if (veryVerbose) cout << "\tnumTransitions" << endl;
            {
                ASSERT_SAFE_FAIL(obj.setNumTransitions(0 - 1));
                ASSERT_SAFE_PASS(obj.setNumTransitions(0    ));
                ASSERT_SAFE_PASS(obj.setNumTransitions(0 + 1));

                ASSERT_SAFE_PASS(obj.setNumTransitions(INT_MAX - 1));
                ASSERT_SAFE_PASS(obj.setNumTransitions(INT_MAX    ));
            }
            if (veryVerbose) cout << "\tnumLocalTimeTypes" << endl;
            {
                ASSERT_SAFE_FAIL(obj.setNumLocalTimeTypes(1 - 1));
                ASSERT_SAFE_PASS(obj.setNumLocalTimeTypes(1    ));
                ASSERT_SAFE_PASS(obj.setNumLocalTimeTypes(1 + 1));

                ASSERT_SAFE_PASS(obj.setNumLocalTimeTypes(INT_MAX - 1));
                ASSERT_SAFE_PASS(obj.setNumLocalTimeTypes(INT_MAX    ));
            }

            if (veryVerbose) cout << "\tabbrevDataSize" << endl;
            {
                ASSERT_SAFE_FAIL(obj.setAbbrevDataSize(1 - 1));
                ASSERT_SAFE_PASS(obj.setAbbrevDataSize(1    ));
                ASSERT_SAFE_PASS(obj.setAbbrevDataSize(1 + 1));

                ASSERT_SAFE_PASS(obj.setAbbrevDataSize(INT_MAX - 1));
                ASSERT_SAFE_PASS(obj.setAbbrevDataSize(INT_MAX    ));
            }
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
        //: 1 Create an object 'w' (default ctor).       { w:D }
        //: 2 Create an object 'x' (copy from 'w').      { w:D x:D }
        //: 3 Set 'x' to 'A' (value distinct from 'D').  { w:D x:A }
        //: 4 Create an object 'y' (init. to 'A').       { w:D x:A y:A }
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

        // Attribute Types

        typedef char T1;  // 'version'
        typedef int  T2;  // 'numIsGmt'
        typedef int  T3;  // 'numIsStd'
        typedef int  T4;  // 'numLeaps'
        typedef int  T5;  // 'numTransitions'
        typedef int  T6;  // 'numLocalTimeTypes'
        typedef int  T7;  // 'abbrevDataSize'

        // Attribute 1 Values: 'version'

        const T1 D1 = '\0';            // default value
        const T1 A1 = '2';

        // Attribute 2 Values: 'numIsGmt'

        const T2 D2 = 0;              // default value
        const T2 A2 = 1;

        // Attribute 3 Values: 'numIsStd'

        const T3 D3 = 0;              // default value
        const T3 A3 = 1;

        // Attribute 4 Values: 'numLeaps'

        const T4 D4 = 0;              // default value
        const T4 A4 = 0;              // no alternate value

        // Attribute 5 Values: 'numTransitions'

        const T5 D5 = 0;              // default value
        const T5 A5 = 1;

        // Attribute 6 Values: 'numLocalTimeTypes'

        const T6 D6 = 1;              // default value
        const T6 A6 = 2;

        // Attribute 7 Values: 'abbrevDataSize'

        const T7 D7 = 1;              // default value
        const T7 A7 = 2;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 1. Create an object 'w' (default ctor)."
                             "\t\t{ w:D }" << endl;

        Obj mW;  const Obj& W = mW;

        if (veryVerbose) cout << "\ta. Check initial value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(D1 == W.version());
        ASSERT(D2 == W.numIsGmt());
        ASSERT(D3 == W.numIsStd());
        ASSERT(D4 == W.numLeaps());
        ASSERT(D5 == W.numTransitions());
        ASSERT(D6 == W.numLocalTimeTypes());
        ASSERT(D7 == W.abbrevDataSize());

        if (veryVerbose) cout <<
                  "\tb. Try equality operators: 'w' <op> 'w'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 2. Create an object 'x' (copy from 'w')."
                             "\t\t{ w:D x:D }" << endl;

        Obj mX(W);  const Obj& X = mX;

        if (veryVerbose) cout << "\ta. Check initial value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(D1 == X.version());
        ASSERT(D2 == X.numIsGmt());
        ASSERT(D3 == X.numIsStd());
        ASSERT(D4 == X.numLeaps());
        ASSERT(D5 == X.numTransitions());
        ASSERT(D6 == X.numLocalTimeTypes());
        ASSERT(D7 == X.abbrevDataSize());

        if (veryVerbose) cout <<
                   "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(1 == (X == W));        ASSERT(0 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 3. Set 'x' to 'A' (value distinct from 'D')."
                             "\t\t{ w:D x:A }" << endl;

        mX.setVersion(A1);
        mX.setNumIsGmt(A2);
        mX.setNumIsStd(A3);
        mX.setNumLeaps(A4);
        mX.setNumTransitions(A5);
        mX.setNumLocalTimeTypes(A6);
        mX.setAbbrevDataSize(A7);

        if (veryVerbose) cout << "\ta. Check new value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == X.version());
        ASSERT(A2 == X.numIsGmt());
        ASSERT(A3 == X.numIsStd());
        ASSERT(A4 == X.numLeaps());
        ASSERT(A5 == X.numTransitions());
        ASSERT(A6 == X.numLocalTimeTypes());
        ASSERT(A7 == X.abbrevDataSize());

        if (veryVerbose) cout <<
             "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 4. Create an object 'y' (init. to 'A')."
                             "\t\t{ w:D x:A y:A }" << endl;

        Obj mY(A1, A2, A3, A4, A5, A6, A7);  const Obj& Y = mY;

        if (veryVerbose) cout << "\ta. Check initial value of 'y'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Y) }

        ASSERT(A1 == Y.version());
        ASSERT(A2 == Y.numIsGmt());
        ASSERT(A3 == Y.numIsStd());
        ASSERT(A4 == Y.numLeaps());
        ASSERT(A5 == Y.numTransitions());
        ASSERT(A6 == Y.numLocalTimeTypes());
        ASSERT(A7 == Y.abbrevDataSize());

        if (veryVerbose) cout <<
             "\tb. Try equality operators: 'y' <op> 'w', 'x', 'y'" << endl;

        ASSERT(0 == (Y == W));        ASSERT(1 == (Y != W));
        ASSERT(1 == (Y == X));        ASSERT(0 == (Y != X));
        ASSERT(1 == (Y == Y));        ASSERT(0 == (Y != Y));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 5. Create an object 'z' (copy from 'y')."
                             "\t\t{ w:D x:A y:A z:A }" << endl;

        Obj mZ(Y);  const Obj& Z = mZ;

        if (veryVerbose) cout << "\ta. Check initial value of 'z'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Z) }

        ASSERT(A1 == Z.version());
        ASSERT(A2 == Z.numIsGmt());
        ASSERT(A3 == Z.numIsStd());
        ASSERT(A4 == Z.numLeaps());
        ASSERT(A5 == Z.numTransitions());
        ASSERT(A6 == Z.numLocalTimeTypes());
        ASSERT(A7 == Z.abbrevDataSize());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (Z == W));        ASSERT(1 == (Z != W));
        ASSERT(1 == (Z == X));        ASSERT(0 == (Z != X));
        ASSERT(1 == (Z == Y));        ASSERT(0 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 6. Set 'z' to 'D' (the default value)."
                             "\t\t\t{ w:D x:A y:A z:D }" << endl;
        mZ.setVersion(D1);
        mZ.setNumIsGmt(D2);
        mZ.setNumIsStd(D3);
        mZ.setNumLeaps(D4);
        mZ.setNumTransitions(D5);
        mZ.setNumLocalTimeTypes(D6);
        mZ.setAbbrevDataSize(D7);

        if (veryVerbose) cout << "\ta. Check new value of 'z'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Z) }

        ASSERT(D1 == Z.version());
        ASSERT(D2 == Z.numIsGmt());
        ASSERT(D3 == Z.numIsStd());
        ASSERT(D4 == Z.numLeaps());
        ASSERT(D5 == Z.numTransitions());
        ASSERT(D6 == Z.numLocalTimeTypes());
        ASSERT(D7 == Z.abbrevDataSize());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (Z == W));        ASSERT(0 == (Z != W));
        ASSERT(0 == (Z == X));        ASSERT(1 == (Z != X));
        ASSERT(0 == (Z == Y));        ASSERT(1 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 7. Assign 'w' from 'x'."
                             "\t\t\t\t{ w:A x:A y:A z:D }" << endl;
        mW = X;

        if (veryVerbose) cout << "\ta. Check new value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(A1 == W.version());
        ASSERT(A2 == W.numIsGmt());
        ASSERT(A3 == W.numIsStd());
        ASSERT(A4 == W.numLeaps());
        ASSERT(A5 == W.numTransitions());
        ASSERT(A6 == W.numLocalTimeTypes());
        ASSERT(A7 == W.abbrevDataSize());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'w' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        ASSERT(1 == (W == X));        ASSERT(0 == (W != X));
        ASSERT(1 == (W == Y));        ASSERT(0 == (W != Y));
        ASSERT(0 == (W == Z));        ASSERT(1 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 8. Assign 'w' from 'z'."
                             "\t\t\t\t{ w:D x:A y:A z:D }" << endl;
        mW = Z;

        if (veryVerbose) cout << "\ta. Check new value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(D1 == W.version());
        ASSERT(D2 == W.numIsGmt());
        ASSERT(D3 == W.numIsStd());
        ASSERT(D4 == W.numLeaps());
        ASSERT(D5 == W.numTransitions());
        ASSERT(D6 == W.numLocalTimeTypes());
        ASSERT(D7 == W.abbrevDataSize());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'x' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        ASSERT(0 == (W == X));        ASSERT(1 == (W != X));
        ASSERT(0 == (W == Y));        ASSERT(1 == (W != Y));
        ASSERT(1 == (W == Z));        ASSERT(0 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 9. Assign 'x' from 'x' (aliasing)."
                             "\t\t\t{ w:D x:A y:A z:D }" << endl;
        mX = X;

        if (veryVerbose) cout << "\ta. Check (same) value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == X.version());
        ASSERT(A2 == X.numIsGmt());
        ASSERT(A3 == X.numIsStd());
        ASSERT(A4 == X.numLeaps());
        ASSERT(A5 == X.numTransitions());
        ASSERT(A6 == X.numLocalTimeTypes());
        ASSERT(A7 == X.abbrevDataSize());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'x' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));
        ASSERT(1 == (X == Y));        ASSERT(0 == (X != Y));
        ASSERT(0 == (X == Z));        ASSERT(1 == (X != Z));

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

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
