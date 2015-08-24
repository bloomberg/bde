// baltzo_localdatetime.t.cpp                                         -*-C++-*-

#include <baltzo_localdatetime.h>

#include <bdlt_datetime.h>

#include <bslx_testinstream.h>           // for testing only
#include <bslx_testinstreamexception.h>  // for testing only
#include <bslx_testoutstream.h>          // for testing only

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

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
    BSLS_ASSERT_OPT(a.allocator() == b.allocator());

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
// The component under test implements a single, unconstrained (value-semantic)
// attribute class that characterizes a local time value as a aggregation of a
// 'bdlt::DatetimeTz' value (i.e., 'bdlt::Datetime' value aggregated with an
// offset value (in minutes) from Coordinated Universal Time [UTC]), and an a
// time-zone identifier.  In practice, the time-zone identifier will correspond
// to an identifier from the Zoneinfo (a.k.a., Olson) database; however, the
// class does not enforce the practice.  The Primary Manipulators and Basic
// Accessors are therefore respectively the attribute setters and getters, each
// of which follows our standard unconstrained attribute-type naming
// conventions: 'setAttributeName' and 'attributeName'.
//
// Primary Manipulators:
//: o 'setDatetimeTz'
//: o 'setTimeZoneId'
//
// Basic Accessors:
//: o 'allocator' (orthogonal to value)
//: o 'datetimeTz'
//: o 'timeZoneId'
//
// This particular attribute class also provides a value constructor capable of
// creating an object in any state relevant for thorough testing, obviating the
// primitive generator function, 'gg', normally used for this purpose.  We will
// therefore follow our standard 10-case approach to testing value-semantic
// types except that we will test the value constructor in case 3 (in lieu of
// the generator function), with the default constructor and primary
// manipulators tested fully in case 2.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o Any allocated memory is always from the object allocator.
//: o An object's value is independent of the allocator used to supply memory.
//: o Injected exceptions are safely propagated during memory allocation.
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
// [11] static int maxSupportedBdexVersion();
//
// CREATORS
// [ 2] baltzo::LocalDatetime(bslma::Allocator *bA = 0);
// [ 3] baltzo::LocalDatetime(DatetimeTz& d, const char *t, *bA = 0);
// [ 7] baltzo::LocalDatetime(const baltzo::LocalDatetime& o, *bA = 0);
//
// MANIPULATORS
// [ 9] baltzo::LocalDatetime& operator=(const baltzo::LocalDatetime& rhs);
// [ 2] setDatetimeTz(const bdlt::DatetimeTz& value);
// [ 2] setTimeZoneId(const char *value);
//
// [ 8] swap(baltzo::LocalDatetime& other);
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
//
// ACCESSORS
// [ 4] bslma::Allocator *allocator() const;
// [ 4] const bdlt::DatetimeTz& datetimeTz() const;
// [ 4] const bsl::string& timeZoneId() const;
//
// [ 5] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const baltzo::LocalDatetime& lhs, rhs);
// [ 6] bool operator!=(const baltzo::LocalDatetime& lhs, rhs);
// [ 5] operator<<(ostream& s, const baltzo::LocalDatetime& d);
//
// FREE FUNCTIONS
// [ 8] swap(baltzo::LocalDatetime& a, b);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [11] USAGE EXAMPLE
// [ *] CONCERN: This test driver is reusable w/other, similar components.
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ 3] CONCERN: All creator/manipulator ptr./ref. parameters are 'const'.
// [ 5] CONCERN: All accessor methods are declared 'const'.
// [ 3] CONCERN: String arguments can be either 'char *' or 'string'.
// [ 9] CONCERN: All memory allocation is from the object's allocator.
// [ 9] CONCERN: All memory allocation is exception neutral.
// [ 9] CONCERN: Object value is independent of the object allocator.
// [ 9] CONCERN: There is no temporary allocation from any allocator.
// [ 8] CONCERN: Precondition violations are detected when enabled.

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

// ============================================================================
//                     SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

// ============================================================================
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef baltzo::LocalDatetime   Obj;

typedef bslma::TestAllocator TestAllocator;
typedef bslx::TestInStream   In;
typedef bslx::TestOutStream  Out;

#define VERSION_SELECTOR 20140601

// ============================================================================
//                       HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

static bool someDiff(const Obj& a, const Obj& b)
{
    return a.datetimeTz() != b.datetimeTz()
        || a.timeZoneId() != b.timeZoneId();
}

// ============================================================================
//                                TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT((bslalg::HasTrait<Obj,
                               bslalg::TypeTraitBitwiseMoveable>::VALUE));
BSLMF_ASSERT((bslalg::HasTrait<Obj,
                               bslalg::TypeTraitUsesBslmaAllocator>::VALUE));

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

const char *const LONG_STRING    = "a_"   SUFFICIENTLY_LONG_STRING;
const char *const LONGER_STRING  = "ab_"  SUFFICIENTLY_LONG_STRING;
const char *const LONGEST_STRING = "abc_" SUFFICIENTLY_LONG_STRING;

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

    TestAllocator globalAllocator(veryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 11: {
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
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Creation and Use of a 'baltzo::LocalDatetime' Object
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// First, we default-construct a 'baltzo::LocalDatetime' object:
//..
    baltzo::LocalDatetime localDatetime;
//..
// Next, we update the time referred to by 'localDatetime' to the New York
// time "December 25, 2009, 11:00" with the time-zone identifier set to
// "America/New_York":
//..
    bdlt::Datetime   datetime(2009, 12, 25, 11, 00, 00);
    bdlt::DatetimeTz datetimeTz(datetime, -5 * 60);  // offset is specified
                                                    // in minutes from UTC
    bsl::string     timeZoneId("America/New_York");
    localDatetime.setDatetimeTz(datetimeTz);
    localDatetime.setTimeZoneId(timeZoneId);

    ASSERT(datetimeTz == localDatetime.datetimeTz());
    ASSERT(timeZoneId == localDatetime.timeZoneId());
//..
// Now, we change the time-zone identifier to another string, for example
// "Europe/Berlin":
//..
    bsl::string anotherTimeZoneId("Europe/Berlin");
    localDatetime.setTimeZoneId(anotherTimeZoneId);

    ASSERT(datetimeTz        == localDatetime.datetimeTz());
    ASSERT(anotherTimeZoneId == localDatetime.timeZoneId());
//..
// Finally, we stream 'localDatetime' to 'bsl::cout':
//..
    bsl::cout << localDatetime << bsl::endl;
//..
// This statement produces the following output on 'stdout':
//..
//  [ 25DEC2009_11:00:00.000-0500 "Europe/Berlin" ]
//..

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // BSLX STREAMING
        //   Ensure that we can serialize the value of any object of the class
        //   via its 'bdexStreamOut' method, and then deserialize that value
        //   back into any object of the class, via its 'bdexStreamIn' method.
        //
        // Concerns:
        //: 1 The signature and return type of 'bdexStreamOut', 'bdexStreamIn',
        //:   and 'maxSupportedBdexVersion' are standard.
        //:
        //: 2 The 'maxSupportedBdexVersion' method returns the expected value.
        //:
        //: 3 For all supported versions, any sequence of valid values can be
        //:   externalized using the 'bdexStreamOut' method.
        //:
        //: 4 For all supported versions, any valid value on the wire can
        //:   unexternalized by 'bdexStreamIn' into an object having that
        //:   value, irrespective of the initial value of the object.
        //:
        //: 5 Both stream methods always return a reference with modifiable
        //:   access to the specified 'stream'.
        //:
        //: 6 For both stream methods, the specified 'stream' is left in an
        //:   invalid state, with no other effect, if the specified 'version'
        //:   is outside the range '[1 .. maxSupportedBdexVersion]'.
        //:
        //: 7 Both stream methods must return with no effect, if the specified
        //:   'stream' is invalid on entry.
        //:
        //: 8 If 'stream' becomes invalid during an invocation of
        //:   'bdexStreamIn', the object is left in a valid state, but possibly
        //:   modified, state.
        //:
        //: 9 If an exception is thrown during a call to the 'bdexStreamIn'
        //:   method, the object is left in an unspecified but valid state.
        //:   In no event is memory leaked.
        //:
        //:10 The wire format of this object is be the concatenation of the the
        //:   wire formats of its constituent attributes, in the order of their
        //:   declaration.  (TBD)
        //:
        //:11 The specified 'version' is not part of the wire format generated
        //:   by the 'bdexStreamOut' method, and is not expected by the
        //:   'bdexStreamIn' method.
        //:
        //:12 The 'bdexStreamIn' method leaves the object unchanged, when the
        //:   specified 'stream' is empty.
        //:
        //:13 The 'bdexStreamIn' method leaves the object in a valid but
        //:   unspecified state, when the specified 'stream' contains
        //:   well-formed data that violates object constraints.
        //:
        //:14 The 'bdexStreamIn' method leaves the object in a valid (but
        //:   unspecified) state when the specified 'stream' contains valid but
        //:   incomplete data.
        //:
        //:15 QoI: No memory is allocated by the object by the 'bdexStreamOut'
        //:   method.
        //:
        //:16 QoI: All memory allocated by the 'bdexStreamIn' method is from
        //:   the object allocator.
        //
        // Plan:
        //: 1 Use the addresses of the (templated) 'bdexStreamIn' and
        //:   'bdexStreamOut', instantiated using the 'bslx::TestInStream' and
        //:   'bslx::TestOutStream types, respectively, to initialize
        //:   member-function pointers each having the standard signature and
        //:   return type for the for these member-functions.  Use the
        //:   'maxSupportedBdexVersion' static function to initialize a
        //:   function pointer having the standard signature and return type
        //:   for that function.  (C-1)
        //:
        //: 2 Compare the return value of the 'maxSupportedBdexVersion' to the
        //:   expected value for this implementation.  (C-2)
        //:
        //: 3 In test cases with both valid and invalid input, compare the
        //:   return values of 'bdexStreamIn' and 'bdexStreamOut' methods with
        //:   their specified 'stream'. (C-5)
        //:
        //: 3 Specify a set 'S' of unique object values with substantial and
        //:   varied differences.  For each value in 'S', construct an object
        //:   'x' along with a sequence of similarly constructed duplicates
        //:   'x1,   x2, ..., xN'.  Attempt to affect every aspect of white-box
        //:   state by altering each 'xi' in a unique way.  Let the union of
        //:   all such objects be the set 'T', programmatically represented by
        //:   the 'VALUES' array of objects.
        //:
        //: 4 Using all combinations of '(u, v)' in 'T X T', stream-out the
        //:   value of u into a buffer and stream it back into (an independent
        //:   object of) 'v', and assert that 'u == v'.  (C-3) Compare the
        //:   return value of the each 'bdexStreamIn' and 'bdexStreamOut'
        //:   method call with the supplied 'stream'.  (C-4)
        //:
        //: 5 Throughout this test case, wrap 'bdexStreamIn' calls with the
        //:   standard 'BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN' and
        //:   'BSLX_TESTINSTREAM_EXCEPTION_TEST_END' macros to confirm
        //:   exception neutrality.  (C-7)
        //:
        //: 6 For each 'x' in 'T', attempt to stream into (a temporary copy of)
        //:   'x' from an empty (but valid) and then invalid stream.  Verify
        //:   after each try that the object is unchanged and that the stream
        //:   is invalid.  (C-7, C-12) For each 'x' in 'T' attempt to stream
        //:   'x' into an initially invalid stream.  Check that the stream is
        //:   unchanged.  For each stream operation (both in and out), compare
        //:   the return value of the each 'bdexStreamIn' and 'bdexStreamOut'
        //:   method call with the supplied 'stream'.  (C-5)
        //:
        //: 7 Write three distinct objects to an output stream buffer of total
        //:   length 'N'.  For each partial stream length from 0 to 'N - 1',
        //:   construct a truncated input stream and attempt to read into
        //:   objects initialized with distinct values.  Verify values of
        //:   objects that are either successfully modified or left entirely
        //:   unmodified, and that the stream became invalid immediately after
        //:   the first incomplete read.  Finally ensure that each object
        //:   streamed into is in some valid state by creating a copy and then
        //:   assigning a known value to that copy; allow the original object
        //:   to leave scope without further modification, so that the
        //:   destructor asserts internal object invariants appropriately.
        //:   (C-14)
        //:
        //: 8 TBD: Iteratively write three distinct objects to an output
        //:   stream buffer, instructing the output stream to make the next
        //:   output operation invalid before one of the calls to  the
        //:   object's 'bdexStreamOut' method.  On each iteration, change the
        //:   point at which the invalid operation is introduced.  In each
        //:   case, the reconstruction of the object from the stream must
        //:   return normally up to the point at which the error was introduced
        //:   (known) and, after that point, an invalid stream, with valid
        //:   though unspecified objects.  (C-8)
        //:
        //: 9 For every supported version, check that an object can be streamed
        //:   and reconstructed (streamed) successfully.  For version numbers
        //:   below and above the supported range (i.e., 0 and
        //:   'maxBdexVersion() + 1), confirm that these operations have no
        //:   effect and the initially valid stream is left in an invalid
        //:   state.  (C-6)
        //:
        //:10 For every object 'x' in 'T' create two outstream: one via the
        //:   object's 'bdexStreamOut' method, and the other by explicitly
        //:   outstreaming each constituent attribute (in order of
        //:   declaration).  (C-10) Confirm that the outstream created via
        //:   individual attributes can be used to reconstruct the object, with
        //:   no data left unread. This accounts for all of the information
        //:   conveyed from the original to the duplicate object; thus, no
        //:   version information was sent.  (C-11).
        //:
        //:11 This class imposes no restrictions on the values of its
        //:   constituent attributes.  If an instream contains data that is
        //:   well-formed but in violation of object constraints, the
        //:   'bdexStreamIn' method of that constituent attribute will fail and
        //:   leave this object in some valid, though unspecified, state.
        //:   (C-13)
        //:
        //:12 In each test using the 'bdexStreamIn' or 'bdexStreamOut' method
        //:   install 'bslma::TestAllocator' object as the default allocator.
        //:   After each 'bdexStreamIn' and 'bdexStreamOut' invocation check
        //:   that the default allocator was not used.  Additionally, after
        //:   'bdexStreamOut' invocation, check that the object allocator was
        //:   not used.  (C15, C-16)
        //
        // Testing:
        //   static int maxSupportedBdexVersion();
        //   STREAM& bdexStreamIn(STREAM& stream, int version);
        //   STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BSLX STREAMING" << endl
                          << "==============" << endl;

        if (verbose) cout <<
                   "\nAssign the addresses of the 'bdex' methods to variables."
                                                                       << endl;
        {
            // Verify that the signatures and return types are standard.

            typedef In&  (Obj::*funcInPtr) (In&,  int);
            typedef Out& (Obj::*funcOutPtr)(Out&, int) const;
            typedef int  (*funcVerPtr)(int);

            funcInPtr  fIn  = &Obj::bdexStreamIn<In>;
            funcOutPtr fOut = &Obj::bdexStreamOut<Out>;
            funcVerPtr fVer =  Obj::maxSupportedBdexVersion;

            // quash potential compiler warnings
            (void)fIn;
            (void)fOut;
            (void)fVer;
        }

        if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'." << endl;
        {
            if (veryVerbose) cout << "\tusing object syntax:" << endl;
            const Obj X;
            ASSERT(1 == X.maxSupportedBdexVersion(VERSION_SELECTOR));
            if (veryVerbose) cout << "\tusing class method syntax:" << endl;
            ASSERT(1 == Obj::maxSupportedBdexVersion(VERSION_SELECTOR));
        }

        // ------------------------------------
        // Values used in several stream tests.
        // ------------------------------------

        const int MAX_VERSION = Obj::maxSupportedBdexVersion(VERSION_SELECTOR);

        bdlt::DatetimeTz defaultDtz;

        bdlt::Datetime   smallDt; smallDt.addMilliseconds(1);
        bdlt::Datetime    someDt(2011,  5,  3, 15, 32);
        bdlt::Datetime   largeDt(9999, 12, 31, 23, 59, 59, 999);

        bdlt::DatetimeTz smallDtz(smallDt, -(24 * 60 - 1));
        bdlt::DatetimeTz  someDtz( someDt, -( 4 * 60 - 0));
        bdlt::DatetimeTz largeDtz(largeDt,  (24 * 60 - 1));

        const char *defaultTzId = "";
        const char   *smallTzId = "a";
        const char   *largeTzId = LONGEST_STRING;

        const Obj V0(defaultDtz, defaultTzId);
        const Obj V1(  smallDtz, defaultTzId);
        const Obj V2(   someDtz, defaultTzId);
        const Obj V3(  largeDtz, defaultTzId);
        const Obj V4(defaultDtz,   smallTzId);
        const Obj V5(  smallDtz,   smallTzId);
        const Obj V6(   someDtz,   smallTzId);
        const Obj V7(  largeDtz,   smallTzId);
        const Obj V8(defaultDtz,   largeTzId);
        const Obj V9(defaultDtz,   largeTzId);
        const Obj VA(   someDtz,   largeTzId);
        const Obj VB(   someDtz,   largeTzId);

        const Obj VALUES[]   =
                            { V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, VA, VB };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        if (verbose) cout << "\nDirect initial trial of 'streamOut' and"
                             " (valid) 'streamIn' functionality." << endl;
        //TBD: What does this *test*?
        {
            for (int version = 1; version < MAX_VERSION; ++version) {
                if (veryVerbose) { T_ T_ P(version) }

                bslma::TestAllocator testAllocator("tA", veryVeryVeryVerbose);

                const Obj X(bdlt::DatetimeTz(bdlt::Datetime(2011, 5, 3, 15),
                                            -4 * 60),
                            "a_" SUFFICIENTLY_LONG_STRING,
                            &testAllocator);
                if (veryVerbose) { cout << "\t   Value being streamed: ";
                                   P(X); }
                Out out(1);
                ASSERT(&out == &(X.bdexStreamOut(out, version)));

                const char *const OD  = out.data();
                const int         LOD = out.length();

                In in(OD, LOD);  ASSERT(in);  ASSERT(!in.isEmpty());

                Obj t(&testAllocator);  ASSERT(X != t);

                if (veryVerbose) { cout << "\tValue being overwritten: ";
                                   P(t); }
                ASSERT(X != t);

                t.bdexStreamIn(in, version);
                ASSERT(in);
                ASSERT(in.isEmpty());

                if (veryVerbose) { cout << "\t  Value after overwrite: ";
                                   P(t); }
                ASSERT(X == t);
            }
        }

        if (verbose) cout << "\tOn valid, non-empty stream data." << endl;
        {
            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard guard(&da);

            for (int version = 1; version < MAX_VERSION; ++version) {
                if (veryVerbose) { T_ T_ P(version) }

                for (int ui = 0; ui < NUM_VALUES; ++ui) {
                    if (veryVerbose) { T_ T_ P(ui) }

                    bslma::TestAllocator oau("oau", veryVeryVeryVerbose);
                    Obj       mU(VALUES[ui], &oau); const Obj& U = mU;
                    const Obj  Z(VALUES[ui], &oau);

                    Out                        out(1);
                    TestAllocatorMonitor oaum(oau), dam(da);
                    LOOP_ASSERT(ui, &out == &(U.bdexStreamOut(out, version)));
                    LOOP_ASSERT(ui, oaum.isTotalSame());
                    LOOP_ASSERT(ui,  dam.isTotalSame());

                    const char *const OD  = out.data();
                    const int         LOD = out.length();

                    In in(OD, LOD);

                    LOOP_ASSERT(U, in);
                    LOOP_ASSERT(U, !in.isEmpty());

                    // Verify that each new value overwrites every old value
                    // and that the input stream is emptied, but remains valid.

                    for (int vi = 0; vi < NUM_VALUES; ++vi) {
                        if (veryVerbose) { T_ T_ P(vi) }

                        bslma::TestAllocator oav("oav", veryVeryVeryVerbose);

                        Obj mV(VALUES[vi], &oav); const Obj& V = mV;


                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oav) {
                        BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {

                            in.reset();
                            TestAllocatorMonitor dam(da);
                            LOOP_ASSERT(vi,
                                        &in == &(mV.bdexStreamIn(in,
                                                                 version)));
                            LOOP3_ASSERT(version, ui, vi, dam.isTotalSame());

                        } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                        LOOP3_ASSERT(version, ui, vi, U == Z);
                        LOOP3_ASSERT(version, ui, vi, V == Z);
                        LOOP3_ASSERT(version, ui, vi, in);
                        LOOP3_ASSERT(version, ui, vi, in.isEmpty());
                    }
                }
            }
        }

        if (verbose) cout << "\tOn empty and invalid input streams." << endl;
        {
            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard guard(&da);

            Out               out(1);
            const char *const OD  = out.data();
            const int         LOD = out.length();
            ASSERT(0 == LOD);

            for (int version = 1; version < MAX_VERSION; ++version) {
                if (veryVerbose) { T_ T_ P(version) }

                for (int i = 0; i < NUM_VALUES; ++i) {
                    In in(OD, LOD);
                    LOOP_ASSERT(i, in);
                    LOOP_ASSERT(i, in.isEmpty());

                    // Ensure that reading from an empty or invalid input
                    // stream leaves the stream invalid and the target object
                    // unchanged.

                    const Obj X(VALUES[i]);  Obj t1(X), t2(X);
                                             LOOP_ASSERT(i, X == t1);
                                             LOOP_ASSERT(i, X == t2);

                    BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                      in.reset();
                      LOOP_ASSERT(i, in);

                      // read from empty
                      TestAllocatorMonitor dam1(da);
                      LOOP_ASSERT(i, &in == &(t1.bdexStreamIn(in, version)));
                      LOOP_ASSERT(i, dam1.isTotalSame());
                      LOOP_ASSERT(i, !in);
                      LOOP_ASSERT(i, X == t1);

                      // read from (the now) invalid stream
                      TestAllocatorMonitor dam2(da);
                      LOOP_ASSERT(i, &in == &(t2.bdexStreamIn(in, version)));
                      LOOP_ASSERT(i, dam2.isTotalSame());
                      LOOP_ASSERT(i, !in);
                      LOOP_ASSERT(i, X == t2);

                    } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
                }
            }
        }

        if (verbose) cout << "\tOn invalid out streams." << endl;
        {
            for (int version = 1; version < MAX_VERSION; ++version) {
                if (veryVerbose) { T_ T_ P(version) }

                for (int i = 0; i < NUM_VALUES; ++i) {
                    if (veryVerbose) { T_ T_ P(i) }

                    bslma::TestAllocator         oa("oa", veryVeryVeryVerbose);
                    bslma::TestAllocator         da("default",
                                                    veryVeryVeryVerbose);
                    bslma::DefaultAllocatorGuard guard(&da);

                    Obj mU(VALUES[i], &oa); const Obj& U = mU;

                    Out out(1);
                    out.invalidate();
                    LOOP_ASSERT(i, !out);
                    const void *data   = out.data();
                    int         length = out.length();

                    TestAllocatorMonitor oam(oa), dam(da);
                    LOOP2_ASSERT(version, i, &out ==
                                             &(U.bdexStreamOut(out, version)));
                    LOOP2_ASSERT(version, i, dam.isTotalSame());
                    LOOP2_ASSERT(version, i, oam.isTotalSame());
                    LOOP2_ASSERT(version, i, !out);
                    LOOP2_ASSERT(version, i, data   == out.data());
                    LOOP2_ASSERT(version, i, length == out.length());
                }
            }
        }
        if (verbose) cout << "\tOn incomplete (but otherwise valid) data."
                          << endl;
        {
            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard guard(&da);

            // Each object has unique attributes w.r.t. objects to be compared
            // (e.g., W1, X1, Y1).  Note that 'smallDtz', 'someDtz', and
            // 'largeDtz' differ in each constituent attribute and that each
            // 'timeZoneId' is unique.  Thus, partially constructed 'tn'
            // objects will match their target value only when 'tn' is
            // completed assembled.

            const int VERSION = 1;

            const Obj W1(smallDtz, "");
            const Obj X1( someDtz, "a");
            const Obj Y1(largeDtz, "bb");

            const Obj W2(largeDtz, "ccc");
            const Obj X2(smallDtz, "dddd");
            const Obj Y2( someDtz, "eeeee");

            const Obj W3( someDtz, "ffffff");
            const Obj X3(largeDtz, "hhhhhhh");
            const Obj Y3(smallDtz, "iiiiiiii");

            Out out(1);

            X1.bdexStreamOut(out, VERSION); const int LOD1 = out.length();
            X2.bdexStreamOut(out, VERSION); const int LOD2 = out.length();
            X3.bdexStreamOut(out, VERSION); const int LOD  = out.length();

            const char *const OD = out.data();

            for (int i = 0; i < LOD; ++i) {
                In in(OD, i);

                BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                  in.reset();
                  LOOP_ASSERT(i, in);
                  LOOP_ASSERT(i, !i == in.isEmpty());

                  Obj t1(W1), t2(W2), t3(W3);

                  if (i < LOD1) {
                      TestAllocatorMonitor dam1(da);
                      LOOP_ASSERT(i, &in == &(t1.bdexStreamIn(in, VERSION)));
                      LOOP_ASSERT(i, dam1.isTotalSame());
                      LOOP_ASSERT(i, !in);
                      LOOP_ASSERT(i, someDiff(X1, t1));

                      LOOP_ASSERT(i, bdlt::DatetimeTz::isValid(
                                          t1.datetimeTz().dateTz().localDate(),
                                          t1.datetimeTz().offset()));

                      TestAllocatorMonitor dam2(da);
                      LOOP_ASSERT(i, &in == &(t2.bdexStreamIn(in, VERSION)));
                      LOOP_ASSERT(i, dam2.isTotalSame());
                      LOOP_ASSERT(i, !in);
                      LOOP_ASSERT(i, W2 == t2);

                      TestAllocatorMonitor dam3(da);
                      LOOP_ASSERT(i, &in == &(t3.bdexStreamIn(in, VERSION)));
                      LOOP_ASSERT(i, dam3.isTotalSame());
                      LOOP_ASSERT(i, !in);    LOOP_ASSERT(i, W3 == t3);
                  }
                  else if (i < LOD2) {
                      TestAllocatorMonitor dam1(da);
                      LOOP_ASSERT(i, &in == &(t1.bdexStreamIn(in, VERSION)));
                      LOOP_ASSERT(i, dam1.isTotalSame());
                      LOOP_ASSERT(i, in);
                      LOOP_ASSERT(i, X1 == t1);

                      TestAllocatorMonitor dam2(da);
                      LOOP_ASSERT(i, &in == &(t2.bdexStreamIn(in, VERSION)));
                      LOOP_ASSERT(i, dam2.isTotalSame());
                      LOOP_ASSERT(i, !in);
                      LOOP_ASSERT(i, someDiff(X2, t2));

                      LOOP_ASSERT(i, bdlt::DatetimeTz::isValid(
                                          t2.datetimeTz().dateTz().localDate(),
                                          t2.datetimeTz().offset()));

                      TestAllocatorMonitor dam3(da);
                      LOOP_ASSERT(i, &in == &(t3.bdexStreamIn(in, VERSION)));
                      LOOP_ASSERT(i, dam3.isTotalSame());
                      LOOP_ASSERT(i, !in);
                      LOOP_ASSERT(i, W3 == t3);
                  }
                  else {
                      TestAllocatorMonitor dam1(da);
                      LOOP_ASSERT(i, &in == &(t1.bdexStreamIn(in, VERSION)));
                      LOOP_ASSERT(i, dam1.isTotalSame());
                      LOOP_ASSERT(i, in);
                      LOOP_ASSERT(i, X1 == t1);

                      TestAllocatorMonitor dam2(da);
                      LOOP_ASSERT(i, &in == &(t2.bdexStreamIn(in, VERSION)));
                      LOOP_ASSERT(i, dam2.isTotalSame());
                      LOOP_ASSERT(i, in);
                      LOOP_ASSERT(i, X2 == t2);

                      TestAllocatorMonitor dam3(da);
                      LOOP_ASSERT(i, &in == &(t3.bdexStreamIn(in, VERSION)));
                      LOOP_ASSERT(i, dam3.isTotalSame());
                      LOOP_ASSERT(i, !in);
                      LOOP_ASSERT(i, someDiff(X3, t3));

                      LOOP_ASSERT(i, bdlt::DatetimeTz::isValid(
                                          t3.datetimeTz().dateTz().localDate(),
                                          t3.datetimeTz().offset()));
                  }

                  // Check the validity of the target objects, 'tn', with some
                  // (light) usage (assignment).

                                LOOP_ASSERT(i, Y1 != t1);
                    t1 = Y1;    LOOP_ASSERT(i, Y1 == t1);

                                LOOP_ASSERT(i, Y2 != t2);
                    t2 = Y2;    LOOP_ASSERT(i, Y2 == t2);

                                LOOP_ASSERT(i, Y3 != t3);
                    t3 = Y3;    LOOP_ASSERT(i, Y3 == t3);

                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }

#if TBD  // Check 'makeNextInvalid' method.
        if (verbose) cout << "\tOn complete data in an invalidated stream."
                          << endl;
        {
            // Each object unique has unique attributes w.r.t. objects to be
            // compared (e.g., W1, X1, Y1).  Note that 'smallDtz', 'someDtz',
            // and 'largeDtz' differ in each constituent attribute and that
            // each 'timeZoneId' is unique.  Thus, partially constructed 'tn'
            // objects will match their target value only 'tn' is completed
            // assembled.

            const int VERSION = 1;

            const Obj W1(smallDtz, "");
            const Obj X1( someDtz, "a");
            const Obj Y1(largeDtz, "bb");

            const Obj W2(largeDtz, "ccc");
            const Obj X2(smallDtz, "dddd");
            const Obj Y2( someDtz, "eeeee");

            const Obj W3( someDtz, "ffffff");
            const Obj X3(largeDtz, "hhhhhhh");
            const Obj Y3(smallDtz, "iiiiiiii");

            Out out(1);

            X1.bdexStreamOut(out, VERSION);
            ASSERT(&out == &(X1.bdexStreamOut(out, VERSION)));
            ASSERT(&out == &(X2.bdexStreamOut(out, VERSION)));
            out.makeNextInvalid();
            ASSERT(&out == &(X3.bdexStreamOut(out, VERSION)));

            In in(out.data(), out.length());

            BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {

              Obj t1(W1), t2(W2), t3(W3);

              ASSERT(&in == &(t1.bdexStreamIn(in, VERSION)));
              ASSERT( out);
              ASSERT(&in == &(t2.bdexStreamIn(in, VERSION)));
              ASSERT( out);
              ASSERT(&in == &(t3.bdexStreamIn(in, VERSION)));
              ASSERT(!out);

              // Check the validity of the target objects, 'tn', with some
              // (light) usage (assignment).

                            ASSERT(Y1 != t1);
                t1 = Y1;    ASSERT(Y1 == t1);

                            ASSERT(Y2 != t2);
                t2 = Y2;    ASSERT(Y2 == t2);

                            ASSERT(Y3 != t3);
                t3 = Y3;    ASSERT(Y3 == t3);

            } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
        }
#endif

        if (verbose) cout << "\tOn invalid versions" << endl;
        {
            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::TestAllocator         oa("object",  veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);

            const Obj W(&oa);               // default value
            const Obj X(someDtz, "A", &oa); // original (control) value
            const Obj Y(someDtz, "B", &oa); // new (streamed-out) value

            if (veryVerbose) cout << "\t\tGood Version on instream" << endl;
            {
                const char version = 1;

                Out out(1);
                out.putString(Y.timeZoneId());        // 1. Stream out "new"
                                                      //    value
                Y.datetimeTz().bdexStreamOut(out, 1); // 2. Stream out "new"
                                                      //    value
                const char *const OD  = out.data();
                const int         LOD = out.length();

                Obj t(X);  ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
                In in(OD, LOD);  ASSERT(in);
                TestAllocatorMonitor dam(da);
                ASSERT(&in == &(t.bdexStreamIn(in, version)));
                ASSERT(dam.isTotalSame());
                ASSERT(in);
                ASSERT(W != t); ASSERT(X != t);  ASSERT(Y == t);
            }

            if (veryVerbose) cout << "\t\tBad versions on instream." << endl;
            {
                const char version = 0; // too small ('version' must be >= 1)

                Out out(1);
                Y.datetimeTz().bdexStreamOut(out, 1); // 1. Stream out "new"
                                                      //    value
                out.putString(Y.timeZoneId());        // 2. Stream out "new"
                                                      //    value

                const char *const OD  = out.data();
                const int         LOD = out.length();

                Obj t(X);  ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
                In in(OD, LOD);  ASSERT(in);
                in.setQuiet(!veryVerbose);
                TestAllocatorMonitor dam(da);
                ASSERT(&in == &(t.bdexStreamIn(in, version)));
                ASSERT(dam.isTotalSame());
                ASSERT(!in);
                ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            }

            {
                const char version = 2 ; // too large (current version is 1)

                Out out(1);
                Y.datetimeTz().bdexStreamOut(out, 1); // 1. Stream out "new"
                                                      //    value
                out.putString(Y.timeZoneId());        // 2. Stream out "new"
                                                      //    value

                const char *const OD  = out.data();
                const int         LOD = out.length();

                Obj t(X);  ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);

                In in(OD, LOD);  ASSERT(in);
                in.setQuiet(!veryVerbose);
                TestAllocatorMonitor dam(da);
                ASSERT(&in == &(t.bdexStreamIn(in, version)));
                ASSERT(dam.isTotalSame());
                ASSERT(!in);
                ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            }

            if (veryVerbose) cout << "\t\tGood Version on outstream" << endl;
            {
                const char version = 1;

                bslma::TestAllocator sa("scratch", veryVeryVeryVerbose);

                Out out(1, &sa);
                ASSERT(out);
                ASSERT(0 == out.length());

                TestAllocatorMonitor oam(oa), dam(da);
                ASSERT(&out == &(Y.bdexStreamOut(out, version)));
                ASSERT(dam.isTotalSame());
                ASSERT(oam.isTotalSame());
                ASSERT(out);
                ASSERT(0 != out.length());
            }

            if (veryVerbose) cout << "\t\tBad versions on instream." << endl;
            {
                const char version = 0; // too small ('version' must be >= 1)

                Out out(1);
                ASSERT(out);
                ASSERT(0 == out.length());

                TestAllocatorMonitor oam(oa), dam(da);
                ASSERT(&out == &(Y.bdexStreamOut(out, version)));
                ASSERT(dam.isTotalSame());
                ASSERT(oam.isTotalSame());
                ASSERT(!out);
                ASSERT(0 == out.length());
            }

            {
                const char version = 2 ; // too large, current max version is 1

                Out out(1);
                ASSERT(out);
                ASSERT(0 == out.length());

                TestAllocatorMonitor oam(oa), dam(da);
                ASSERT(&out == &(Y.bdexStreamOut(out, version)));
                ASSERT(dam.isTotalSame());
                ASSERT(oam.isTotalSame());
                ASSERT(!out);
                ASSERT(0 == out.length());
            }
        }

        if (verbose) cout << "\nWire format direct tests." << endl;
        {
            for (int i = 0; i < NUM_VALUES; ++i) {

                Obj       mX(VALUES[i]);  const Obj& X = mX;
                const int VERSION = 1;

                if (veryVerbose) { T_ P(X) }

                Out outO(1);  X.bdexStreamOut(outO, VERSION);

                Out outA(1);  outA.putString(X.timeZoneId());
                           X.datetimeTz().bdexStreamOut(outA, VERSION);

                LOOP_ASSERT(i, outA.length() == outO.length());
                LOOP_ASSERT(i, 0             == memcmp(outO.data(),
                                                       outA.data(),
                                                       outA.length()));

                Obj mY;  const Obj& Y = mY;
                In in(outA.data(), outA.length());
                ASSERT(&in == &(mY.bdexStreamIn(in, VERSION)));

                LOOP_ASSERT(i, in.isEmpty());
                LOOP_ASSERT(i, X == Y);
            }
        }

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
        //: 2 The allocator address held by the target object is unchanged.
        //:
        //: 3 Any memory allocation is from the target object's allocator.
        //:
        //: 4 The signature and return type are standard.
        //:
        //: 5 The reference returned is to the target object (i.e., '*this').
        //:
        //: 6 The value of the source object is not modified.
        //:
        //: 7 The allocator address held by the source object is unchanged.
        //:
        //: 8 QoI: Assigning a source object having the default-constructed
        //:   value allocates no memory.
        //:
        //: 9 Any memory allocation is exception neutral.
        //:
        //:10 Assigning an object to itself behaves as expected (alias-safety).
        //:
        //:11 Every object releases any allocated memory at destruction.
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-4)
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
        //: 4 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3:  (C-1..2, 5..8, 11)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2):  (C-1..2, 5..8, 11)
        //:
        //:     1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:     2 Use the value constructor and 'oa' to create a modifiable
        //:       'Obj', 'mX', having the value 'W'.
        //:
        //:     3 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:       (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:
        //:     4 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     5 Use the equality-comparison operator to verify that: (C-1, 6)
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-6)
        //:
        //:     6 Use the 'allocator' accessor of both 'mX' and 'Z' to verify
        //:       that the respective allocator addresses held by the target
        //:       and source objects are unchanged.  (C-2, 7)
        //:
        //:     7 Use the appropriate test allocators to verify that:
        //:       (C-8, 11)
        //:
        //:       1 For an object that (a) is initialized with a value that did
        //:         NOT require memory allocation, and (b) is then assigned a
        //:         value that DID require memory allocation, the target object
        //:         DOES allocate memory from its object allocator only
        //:         (irrespective of the specific number of allocations or the
        //:         total amount of memory allocated); also cross check with
        //:         what is expected for 'mX' and 'Z'.
        //:
        //:       2 An object that is assigned a value that did NOT require
        //:         memory allocation, does NOT allocate memory from its object
        //:         allocator; also cross check with what is expected for 'Z'.
        //:
        //:       3 No additional memory is allocated by the source object.
        //:         (C-8)
        //:
        //:       4 All object memory is released when the object is destroyed.
        //:         (C-11)
        //:
        //: 5 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-4.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-3:  (C-9)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor and 'oa' to create a modifiable 'Obj'
        //:     'mX'; also use the value constructor and a distinct "scratch"
        //:     allocator to create a 'const' 'Obj' 'ZZ'.
        //:
        //:   3 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   4 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:     (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:     (C-9)
        //:
        //:   5 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   6 Use the equality-comparison operator to verify that the
        //:     target object, 'mX', still has the same value as that of 'ZZ'.
        //:
        //:   7 Use the 'allocator' accessor of 'mX' to verify that it is still
        //:     the object allocator.
        //:
        //:   8 Use the appropriate test allocators to verify that:
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
        //   operator=(const baltzo::LocalTimeDescriptor& rhs);
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

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        bdlt::DatetimeTz defaultDtz;

        bdlt::Datetime   smallDt; smallDt.addMilliseconds(1);
        bdlt::Datetime    someDt(2011,  5,  3, 15, 32);
        bdlt::Datetime   largeDt(9999, 12, 31, 23, 59, 59, 999);

        bdlt::DatetimeTz smallDtz(smallDt, -(24 * 60 - 1));
        bdlt::DatetimeTz  someDtz( someDt, -( 4 * 60 - 0));
        bdlt::DatetimeTz largeDtz(largeDt,  (24 * 60 - 1));

        const char *defaultTzId = "";
        const char   *smallTzId = "a";
        const char   *largeTzId = LONGEST_STRING;

        const struct {
            int              d_line;  // source line number
            char             d_mem;   // expected allocation: 'Y', 'N', '?'
            bdlt::DatetimeTz *d_datetimeTz;
            const char      *d_timeZoneId;
        } DATA[] = {

            //LINE  MEM  DTTZ          TZID
            //----  ---  -----------   -----------

            // default (must be first)
            { L_,   'N', &defaultDtz,  defaultTzId },

            // 'datetimeTz'
            { L_,   'N',   &smallDtz,  defaultTzId },
            { L_,   'N',    &someDtz,  defaultTzId },
            { L_,   'N',   &largeDtz,  defaultTzId },

            // 'timeZoneId'
            { L_,   '?', &defaultDtz,    smallTzId },
            { L_,   'Y', &defaultDtz,    largeTzId },

            // other
            { L_,   '?',    &someDtz,    smallTzId },
            { L_,   'Y',    &someDtz,    largeTzId },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int              LINE1   =  DATA[ti].d_line;
            const char             MEMSRC1 =  DATA[ti].d_mem;
            const bdlt::DatetimeTz& DTTZ1   = *DATA[ti].d_datetimeTz;
            const char *const      TZID1   =  DATA[ti].d_timeZoneId;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const Obj  Z(DTTZ1, TZID1, &scratch);
            const Obj ZZ(DTTZ1, TZID1, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE1, Obj(), Z, Obj() == Z);
                firstFlag = false;
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int              LINE2   =  DATA[tj].d_line;
                const char             MEMDST2 =  DATA[tj].d_mem;
                const bdlt::DatetimeTz& DTTZ2   = *DATA[tj].d_datetimeTz;
                const char *const      TZID2   =  DATA[tj].d_timeZoneId;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(DTTZ2, TZID2, &oa);  const Obj& X = mX;

                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    LOOP4_ASSERT(LINE1, LINE2, Z, X,
                                 (Z == X) == (LINE1 == LINE2));

                    TestAllocatorMonitor oam(oa), sam(scratch);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        Obj *mR = &(mX = Z);
                        LOOP4_ASSERT(LINE1, LINE2,  Z,   X,  Z == X);
                        LOOP4_ASSERT(LINE1, LINE2, mR, &mX, mR == &mX);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    LOOP4_ASSERT(LINE1, LINE2, ZZ, Z, ZZ == Z);

                    LOOP4_ASSERT(LINE1, LINE2, &oa, X.allocator(),
                                 &oa == X.allocator());
                    LOOP4_ASSERT(LINE1, LINE2, &scratch, Z.allocator(),
                                 &scratch == Z.allocator());

                    if ('N' == MEMDST2 && 'Y' == MEMSRC1) {
                        LOOP2_ASSERT(LINE1, LINE2, oam.isInUseUp());
                    }
                    else if ('Y' == MEMDST2) {
                        LOOP2_ASSERT(LINE1, LINE2, oam.isInUseSame());
                    }

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    LOOP2_ASSERT(LINE1, LINE2, sam.isInUseSame());

                    LOOP2_ASSERT(LINE1, LINE2, 0 == da.numBlocksTotal());
                }

                // Verify all memory is released on object destruction.

                LOOP3_ASSERT(LINE1, LINE2, oa.numBlocksInUse(),
                             0 == oa.numBlocksInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                      Obj mX(DTTZ1, TZID1, &oa);
                const Obj ZZ(DTTZ1, TZID1, &scratch);

                const Obj& Z = mX;

                LOOP3_ASSERT(LINE1, ZZ, Z, ZZ == Z);

                TestAllocatorMonitor oam(oa), sam(scratch);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj *mR = &(mX = Z);
                    LOOP3_ASSERT(LINE1, ZZ,   Z, ZZ == Z);
                    LOOP3_ASSERT(LINE1, mR, &mX, mR == &mX);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP3_ASSERT(LINE1, &oa, Z.allocator(), &oa == Z.allocator());

                LOOP_ASSERT(LINE1, oam.isInUseSame());

                LOOP_ASSERT(LINE1, sam.isInUseSame());

                LOOP_ASSERT(LINE1, 0 == da.numBlocksTotal());
            }

            // Verify all object memory is released on destruction.

            LOOP2_ASSERT(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
        }

        // Double check that some object memory was allocated.

        ASSERT(anyObjectMemoryAllocatedFlag);

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
        //:
        //: 7 QoI: Asserted precondition violations are detected when enabled.
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
        //: 4 For each row 'R1' in the table of P-3:  (C-1..2, 5)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor and 'oa' to create a modifiable
        //:     'Obj', 'mW', having the value described by 'R1'; also use the
        //:     copy constructor and a "scratch" allocator to create a 'const'
        //:     'Obj' 'XX' from 'mW'.
        //:
        //:   3 Use the member and free 'swap' functions to swap the value of
        //:     'mW' with itself; verify, after each swap, that:  (C-5)
        //:
        //:     1 The value is unchanged.  (C-5)
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
        //:   (C-6)
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
        //:   5 Use the 'invokeAdlSwap' helper function template to swap the
        //:     values of 'mX' and 'mY', using the free 'swap' function defined
        //:     in this component, then verify that:  (C-6)
        //:
        //:     1 The values have been exchanged.
        //:
        //:     2 There was no additional object memory allocation.  (C-6)
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //:
        //: 7 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when an attempt is made to swap objects that do not
        //:   refer to the same allocator, but not when the allocators are the
        //:   same (using the 'BSLS_ASSERTTEST_*' macros).  (C-7)
        //
        // Testing:
        //   void swap(baltzo::LocalTimeDescriptor& other);
        //   void swap(baltzo::LocalTimeDescriptor& a, b);
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

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        bdlt::DatetimeTz defaultDtz;

        bdlt::Datetime   smallDt; smallDt.addMilliseconds(1);
        bdlt::Datetime    someDt(2011,  5,  3, 15, 32);
        bdlt::Datetime   largeDt(9999, 12, 31, 23, 59, 59, 999);

        bdlt::DatetimeTz smallDtz(smallDt, -(24 * 60 - 1));
        bdlt::DatetimeTz  someDtz( someDt, -( 4 * 60 - 0));
        bdlt::DatetimeTz largeDtz(largeDt,  (24 * 60 - 1));

        const char *defaultTzId = "";
        const char   *smallTzId = "a";
        const char   *largeTzId = LONGEST_STRING;

        const struct {
            int              d_line;  // source line number
            char             d_mem;   // expected allocation: 'Y', 'N', '?'
            bdlt::DatetimeTz *d_datetimeTz;
            const char      *d_timeZoneId;
        } DATA[] = {

            //LINE  MEM  DTTZ          TZID
            //----  ---  -----------   -----------

            // default (must be first)
            { L_,   'N', &defaultDtz,  defaultTzId },

            // 'datetimeTz'
            { L_,   'N',   &smallDtz,  defaultTzId },
            { L_,   'N',    &someDtz,  defaultTzId },
            { L_,   'N',   &largeDtz,  defaultTzId },

            // 'timeZoneId'
            { L_,   '?', &defaultDtz,    smallTzId },
            { L_,   'Y', &defaultDtz,    largeTzId },

            // other
            { L_,   '?',    &someDtz,    smallTzId },
            { L_,   'Y',    &someDtz,    largeTzId },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int              LINE1 =  DATA[ti].d_line;
            const char             MEM1  =  DATA[ti].d_mem;
            const bdlt::DatetimeTz& DTTZ1 = *DATA[ti].d_datetimeTz;
            const char *const      TZID1 =  DATA[ti].d_timeZoneId;

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                  Obj mW(DTTZ1, TZID1, &oa);  const Obj& W = mW;
            const Obj XX(W, &scratch);

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
                TestAllocatorMonitor oam(oa);

                mW.swap(mW);

                LOOP3_ASSERT(LINE1, XX, W, XX == W);
                LOOP_ASSERT(LINE1, &oa == W.allocator());
                LOOP_ASSERT(LINE1, oam.isTotalSame());
            }

            // free function 'swap'
            {
                TestAllocatorMonitor oam(oa);

                swap(mW, mW);

                LOOP3_ASSERT(LINE1, XX, W, XX == W);
                LOOP_ASSERT(LINE1, &oa == W.allocator());
                LOOP_ASSERT(LINE1, oam.isTotalSame());
            }

            // Verify expected ('Y'/'N') object-memory allocations.

            if ('?' != MEM1) {
                LOOP3_ASSERT(LINE1, MEM1, oa.numBlocksInUse(),
                           ('N' == MEM1) == (0 == oa.numBlocksInUse()));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int              LINE2 =  DATA[tj].d_line;
                const char             MEM2  =  DATA[tj].d_mem;
                const bdlt::DatetimeTz& DTTZ2 = *DATA[tj].d_datetimeTz;
                const char *const      TZID2 =  DATA[tj].d_timeZoneId;

                      Obj mX(XX, &oa);  const Obj& X = mX;

                      Obj mY(DTTZ2, TZID2, &oa);  const Obj& Y = mY;
                const Obj YY(Y, &scratch);

                if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

                // member 'swap'
                {
                    TestAllocatorMonitor oam(oa);

                    mX.swap(mY);

                    LOOP4_ASSERT(LINE1, LINE2, YY, X, YY == X);
                    LOOP4_ASSERT(LINE1, LINE2, XX, Y, XX == Y);
                    LOOP2_ASSERT(LINE1, LINE2, &oa == X.allocator());
                    LOOP2_ASSERT(LINE1, LINE2, &oa == Y.allocator());
                    LOOP2_ASSERT(LINE1, LINE2, oam.isTotalSame());
                }

                // free function 'swap'
                {
                    TestAllocatorMonitor oam(oa);

                    swap(mX, mY);

                    LOOP4_ASSERT(LINE1, LINE2, XX, X, XX == X);
                    LOOP4_ASSERT(LINE1, LINE2, YY, Y, YY == Y);
                    LOOP2_ASSERT(LINE1, LINE2, &oa == X.allocator());
                    LOOP2_ASSERT(LINE1, LINE2, &oa == Y.allocator());
                    LOOP2_ASSERT(LINE1, LINE2, oam.isTotalSame());
                }
            }

            // Record if some object memory was allocated.

            anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();
        }

        // Double check that some object memory was allocated.

        ASSERT(anyObjectMemoryAllocatedFlag);

        if (verbose) cout <<
                "\nInvoke free 'swap' function in a context where ADL is used."
                                                                       << endl;
        {
            // 'A' values: Should cause memory allocation if possible.

            const bdlt::DatetimeTz A1(bdlt::Datetime(2011, 5, 3, 15), -4 * 60);
            const char            A2[] = "a_" SUFFICIENTLY_LONG_STRING;

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                  Obj mX(&oa);  const Obj& X = mX;
            const Obj XX(X, &scratch);

                  Obj mY(A1, A2, &oa);  const Obj& Y = mY;
            const Obj YY(Y, &scratch);

            if (veryVerbose) { T_ P_(X) P(Y) }

            TestAllocatorMonitor oam(oa);

            invokeAdlSwap(mX, mY);

            LOOP2_ASSERT(YY, X, YY == X);
            LOOP2_ASSERT(XX, Y, XX == Y);
            ASSERT(oam.isTotalSame());

            if (veryVerbose) { T_ P_(X) P(Y) }
        }

        // Verify no memory is allocated from the default allocator.

        LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'swap' member function" << endl;
            {
                bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
                bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                Obj mA(&oa1);  Obj mB(&oa1);
                Obj mZ(&oa2);

                ASSERT_SAFE_PASS(mA.swap(mB));
                ASSERT_SAFE_FAIL(mA.swap(mZ));
            }

            if (veryVerbose) cout << "\t'swap' free function" << endl;
            {
                bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
                bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                Obj mA(&oa1);  Obj mB(&oa1);
                Obj mZ(&oa2);

                ASSERT_SAFE_PASS(swap(mA, mB));
                ASSERT_SAFE_FAIL(swap(mA, mZ));
            }
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor (with or without a supplied allocator)
        //:   creates an object having the same value as that of the supplied
        //:   original object.
        //:
        //: 2 If an allocator is NOT supplied to the copy constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object (i.e., the
        //:   allocator of the original object is never copied).
        //:
        //: 3 If an allocator IS supplied to the copy constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the copy constructor has no effect
        //:   on subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary memory allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 The original object is passed as a reference providing
        //:   non-modifiable access to that object.
        //:
        //:10 The value of the original object is unchanged.
        //:
        //:11 The allocator address held by the original object is unchanged.
        //:
        //:12 QoI: Copying an object having the default-constructed value
        //:   allocates no memory.
        //:
        //:13 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Using the table-driven technique:
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
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1..12)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop creating three distinct objects in turn,
        //:     each using the copy constructor on 'Z' from P-2.1, but
        //:     configured differently: (a) without passing an allocator,
        //:     (b) passing a null allocator address explicitly, and (c)
        //:     passing the address of a test allocator distinct from the
        //:     default.
        //:
        //:   3 For each of these three iterations (P-2.2):  (C-1..12)
        //:
        //:     1 Create three 'bslma::TestAllocator' objects, and install one
        //:       as the current default allocator (note that a ubiquitous test
        //:       allocator is already installed as the global allocator).
        //:
        //:     2 Use the copy constructor to dynamically create an object 'X',
        //:       with its object allocator configured appropriately (see
        //:       P-2.2), supplying it the 'const' object 'Z' (see P-2.1); use
        //:       a distinct test allocator for the object's footprint.  (C-9)
        //:
        //:     3 Use the equality-comparison operator to verify that:
        //:       (C-1, 5, 10)
        //:
        //:       1 The newly constructed object, 'X', has the same value as
        //:         that of 'Z'.  (C-1, 5)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-10)
        //:
        //:     4 Use the 'allocator' accessor of each underlying attribute
        //:       capable of allocating memory to ensure that its object
        //:       allocator is properly installed; also use the 'allocator'
        //:       accessor of 'X' to verify that its object allocator is
        //:       properly installed, and use the 'allocator' accessor of 'Z'
        //:       to verify that the allocator address that it holds is
        //:       unchanged.  (C-6, 11)
        //:
        //:     5 Use the appropriate test allocators to verify that:  (C-2..4,
        //:       7..8, 12)
        //:
        //:       1 An object that IS expected to allocate memory does so
        //:         from the object allocator only (irrespective of the
        //:         specific number of allocations or the total amount of
        //:         memory allocated).  (C-2, 4)
        //:
        //:       2 An object that is expected NOT to allocate memory doesn't.
        //:         (C-12)
        //:
        //:       3 If an allocator was supplied at construction (P-2.1c), the
        //:         current default allocator doesn't allocate any memory.
        //:         (C-3)
        //:
        //:       4 No temporary memory is allocated from the object allocator.
        //:         (C-7)
        //:
        //:       5 All object memory is released when the object is destroyed.
        //:         (C-8)
        //:
        //: 3 Test again, using the data of P-1, but this time just for the
        //:   supplied allocator configuration (P-2.2c), and create the object
        //:   as an automatic variable in the presence of injected exceptions
        //:   (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:   (C-13)
        //
        // Testing:
        //   baltzo::LocalTimeDescriptor(const LTDescriptor& o, *bA = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;

        bdlt::DatetimeTz defaultDtz;

        bdlt::Datetime   smallDt; smallDt.addMilliseconds(1);
        bdlt::Datetime    someDt(2011,  5,  3, 15, 32);
        bdlt::Datetime   largeDt(9999, 12, 31, 23, 59, 59, 999);

        bdlt::DatetimeTz smallDtz(smallDt, -(24 * 60 - 1));
        bdlt::DatetimeTz  someDtz( someDt, -( 4 * 60 - 0));
        bdlt::DatetimeTz largeDtz(largeDt,  (24 * 60 - 1));

        const char *defaultTzId = "";
        const char   *smallTzId = "a";
        const char   *largeTzId = LONGEST_STRING;

        const struct {
            int              d_line;  // source line number
            char             d_mem;   // expected allocation: 'Y', 'N', '?'
            bdlt::DatetimeTz *d_datetimeTz;
            const char      *d_timeZoneId;
        } DATA[] = {

            //LINE  MEM  DTTZ          TZID
            //----  ---  -----------   -----------

            // default (must be first)
            { L_,   'N', &defaultDtz,  defaultTzId },

            // 'datetimeTz'
            { L_,   'N',   &smallDtz,  defaultTzId },
            { L_,   'N',    &someDtz,  defaultTzId },
            { L_,   'N',   &largeDtz,  defaultTzId },

            // 'timeZoneId'
            { L_,   '?', &defaultDtz,    smallTzId },
            { L_,   'Y', &defaultDtz,    largeTzId },

            // other
            { L_,   '?',    &someDtz,    smallTzId },
            { L_,   'Y',    &someDtz,    largeTzId },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory.
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int              LINE =  DATA[ti].d_line;
                const char             MEM  =  DATA[ti].d_mem;
                const bdlt::DatetimeTz& DTTZ = *DATA[ti].d_datetimeTz;
                const char *const      TZID =  DATA[ti].d_timeZoneId;

                LOOP2_ASSERT(LINE, MEM, MEM && strchr("YN?", MEM));

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                const Obj  Z(DTTZ, TZID, &scratch);
                const Obj ZZ(DTTZ, TZID, &scratch);

                if (veryVerbose) { T_ P_(Z) P(ZZ) }

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
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

                    LOOP4_ASSERT(LINE, CONFIG,  Z, X,  Z == X);

                    // Verify that the value of 'Z' has not changed.

                    LOOP4_ASSERT(LINE, CONFIG, ZZ, Z, ZZ == Z);

                    // -------------------------------------------------------
                    // Verify any attribute allocators are installed properly.
                    // -------------------------------------------------------

                    LOOP2_ASSERT(LINE, CONFIG,
                                 &oa == X.timeZoneId().allocator());

                    // Also invoke the object's 'allocator' accessor, as well
                    // as that of 'Z'.

                    LOOP4_ASSERT(LINE, CONFIG, &oa, X.allocator(),
                                 &oa == X.allocator());

                    LOOP4_ASSERT(LINE, CONFIG, &scratch, Z.allocator(),
                                 &scratch == Z.allocator());

                    // Verify no allocation from the non-object allocator.

                    LOOP3_ASSERT(LINE, CONFIG, noa.numBlocksTotal(),
                                 0 == noa.numBlocksTotal());

                    // Verify no temporary memory is allocated from the object
                    // allocator.

                    LOOP4_ASSERT(LINE, CONFIG, oa.numBlocksTotal(),
                                                           oa.numBlocksInUse(),
                                 oa.numBlocksTotal() == oa.numBlocksInUse());

                    // Verify expected ('Y'/'N') object-memory allocations.

                    if ('?' != MEM) {
                        LOOP4_ASSERT(LINE, CONFIG, MEM, oa.numBlocksInUse(),
                                   ('N' == MEM) == (0 == oa.numBlocksInUse()));
                    }

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    LOOP3_ASSERT(LINE, CONFIG, da.numBlocksInUse(),
                                 0 == da.numBlocksInUse());
                    LOOP3_ASSERT(LINE, CONFIG, fa.numBlocksInUse(),
                                 0 == fa.numBlocksInUse());
                    LOOP3_ASSERT(LINE, CONFIG, sa.numBlocksInUse(),
                                 0 == sa.numBlocksInUse());
                }  // end foreach configuration

            }  // end foreach row

            // Double check that some object memory was allocated.

            ASSERT(anyObjectMemoryAllocatedFlag);

            // Note that memory should be independently allocated for each
            // attribute capable of allocating memory.
        }

        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int              LINE =  DATA[ti].d_line;
                const char             MEM  =  DATA[ti].d_mem;
                const bdlt::DatetimeTz& DTTZ = *DATA[ti].d_datetimeTz;
                const char *const      TZID =  DATA[ti].d_timeZoneId;

                if (veryVerbose) { T_ P_(MEM) P_(DTTZ) P(TZID) }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                const Obj  Z(DTTZ, TZID, &scratch);
                const Obj ZZ(DTTZ, TZID, &scratch);

                bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);


                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj obj(Z, &sa);
                    LOOP3_ASSERT(LINE, Z, obj, Z == obj);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP3_ASSERT(LINE, ZZ, Z, ZZ == Z);

                LOOP3_ASSERT(LINE, &scratch, Z.allocator(),
                             &scratch == Z.allocator());
                LOOP2_ASSERT(LINE, da.numBlocksInUse(),
                             0 == da.numBlocksInUse());
                LOOP2_ASSERT(LINE, sa.numBlocksInUse(),
                             0 == sa.numBlocksInUse());
            }
        }

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
        //: 3 No non-salient attributes (i.e., 'allocator') participate.
        //:
        //: 4 'true  == (X == X)'  (i.e., identity)
        //:
        //: 5 'false == (X != X)'  (i.e., identity)
        //:
        //: 6 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:
        //: 7 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:
        //: 8 'X != Y' if and only if '!(X == Y)'
        //:
        //: 9 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //:10 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //:11 No memory allocation occurs as a result of comparison (e.g., the
        //:   arguments are not passed by value).
        //:
        //:12 The equality operator's signature and return type are standard.
        //:
        //:13 The inequality operator's signature and return type are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.
        //:   (C-9..10, 12..13)
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
        //:
        //: 4 For each row 'R1' in the table of P-3:  (C-1..8)
        //:
        //:   1 Create a single object, using a "scratch" allocator, and
        //:     use it to verify the reflexive (anti-reflexive) property of
        //:     equality (inequality) in the presence of aliasing.  (C-4..5)
        //:
        //:   2 For each row 'R2' in the table of P-3:  (C-1..3, 6..8)
        //:
        //:     1 Record, in 'EXP', whether or not distinct objects created
        //:       from 'R1' and 'R2', respectively, are expected to have the
        //:       same value.
        //:
        //:     2 For each of two configurations, 'a' and 'b':  (C-1..3, 6..8)
        //:
        //:       1 Create two (object) allocators, 'oax' and 'oay'.
        //:
        //:       2 Create an object 'X', using 'oax', having the value 'R1'.
        //:
        //:       3 Create an object 'Y', using 'oax' in configuration 'a' and
        //:         'oay' in configuration 'b', having the value 'R2'.
        //:
        //:       4 Verify the commutativity property and expected return value
        //:         for both '==' and '!=', while monitoring both 'oax' and
        //:         'oay' to ensure that no object memory is ever allocated by
        //:         either operator.  (C-1..3, 6..8)
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

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
            "\nDefine appropriate individual attribute values, 'Ai' and 'Bi'."
                                                                       << endl;

        // Attribute Types

        typedef bdlt::DatetimeTz  T1;  // 'datetimeTz'
        typedef const char       *T2;  // 'timeZoneId'

        // Attribute 1 Values: 'datetimeTz'

        const T1 A1(bdlt::Datetime(2011, 5, 3, 15), -4 * 60);
        const T1 B1(bdlt::Datetime(2011, 5, 3, 15), -5 * 60);

        // Attribute 2 Values: 'timeZoneId'

        const T2 A2 =   LONG_STRING;
        const T2 B2 = LONGER_STRING;

        if (verbose) cout <<
            "\nCreate a table of distinct, but similar object values." << endl;

        struct {
            int       d_line;           // source line number
            const T1 *d_datetimeTz;
            const T2  d_timeZoneId;
        } DATA[] = {

            // The first row of the table below represents an object value
            // consisting of "baseline" attribute values (A1..An).  Each
            // subsequent row differs (slightly) from the first in exactly one
            // attribute value (Bi).

            //LINE  DTTZ          TZID
            //----  -----------   -----------

            { L_,    &A1,          A2},          // baseline

            { L_,    &B1,          A2},
            { L_,    &A1,          B2},
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nCompare every value with every value." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE1 =  DATA[ti].d_line;
            const T1& DTTZ1 = *DATA[ti].d_datetimeTz;
            const T2  TZID1 =  DATA[ti].d_timeZoneId;

            if (veryVerbose) { T_ P_(LINE1) P_(DTTZ1) P(TZID1) }

            // Ensure an object compares correctly with itself (alias test).
            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                const Obj X(DTTZ1, TZID1, &scratch);

                LOOP2_ASSERT(LINE1, X,   X == X);
                LOOP2_ASSERT(LINE1, X, !(X != X));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int LINE2 =  DATA[tj].d_line;
                const T1& DTTZ2 = *DATA[tj].d_datetimeTz;
                const T2  TZID2 =  DATA[tj].d_timeZoneId;

                if (veryVerbose) {
                               T_ T_ P_(LINE2) P_(DTTZ2) P(TZID2) }

                const bool EXP = ti == tj;  // expected for equality comparison

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                    const char CONFIG = cfg;  // Determines 'Y's allocator.

                    // Create two distinct test allocators, 'oax' and 'oay'.

                    bslma::TestAllocator oax("objectx", veryVeryVeryVerbose);
                    bslma::TestAllocator oay("objecty", veryVeryVeryVerbose);

                    // Map allocators above to objects 'X' and 'Y' below.

                    bslma::TestAllocator& xa = oax;
                    bslma::TestAllocator& ya = 'a' == CONFIG ? oax : oay;

                    const Obj X(DTTZ1, TZID1, &xa);
                    const Obj Y(DTTZ2, TZID2, &ya);

                    if (veryVerbose) {
                        T_ T_ T_ P_(EXP) P_(CONFIG) P_(X) P(Y) }

                    // Verify value, commutativity, and no memory allocation.

                    TestAllocatorMonitor oaxm(oax), oaym(oay);

                    LOOP5_ASSERT(LINE1, LINE2, CONFIG, X, Y,  EXP == (X == Y));
                    LOOP5_ASSERT(LINE1, LINE2, CONFIG, Y, X,  EXP == (Y == X));

                    LOOP5_ASSERT(LINE1, LINE2, CONFIG, X, Y, !EXP == (X != Y));
                    LOOP5_ASSERT(LINE1, LINE2, CONFIG, Y, X, !EXP == (Y != X));

                    LOOP3_ASSERT(LINE1, LINE2, CONFIG, oaxm.isTotalSame());
                    LOOP3_ASSERT(LINE1, LINE2, CONFIG, oaym.isTotalSame());

                    // Double check that some object memory was allocated.

                    LOOP3_ASSERT(LINE1, LINE2, CONFIG,
                                 1 <= xa.numBlocksInUse());
                    LOOP3_ASSERT(LINE1, LINE2, CONFIG,
                                 1 <= ya.numBlocksInUse());

                    // Note that memory should be independently allocated for
                    // each attribute capable of allocating memory.
                }
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
        //:   the appropriate signatures and return types.  (C-4, 7)
        //:
        //: 2 Using the table-driven technique:  (C-1..3, 5..6, 8)
        //:
        //:   1 Define fourteen carefully selected combinations of (two) object
        //:     values ('A' and 'B'), having distinct values for each
        //:     corresponding salient attribute, and various values for the
        //:     two formatting parameters, along with the expected output
        //:     ( 'value' x  'level'   x 'spacesPerLevel' ):
        //:     1 { A   } x {  0     } x {  0, 1, -1, -8 } --> 3 expected o/ps
        //:     2 { A   } x {  3, -3 } x {  0, 2, -2, -8 } --> 8 expected o/ps
        //:     3 { B   } x {  2     } x {  3            } --> 1 expected op
        //:     4 { A B } x { -8     } x { -8            } --> 2 expected o/ps
        //:     4 { A B } x { -9     } x { -9            } --> 2 expected o/ps
        //:
        //:   2 For each row in the table defined in P-2.1:  (C-1..3, 5, 7)
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
        //:       supplied stream.  (C-5, 8)
        //:
        //:     4 Compare the contents captured in P-2.2.2 with what is
        //:       expected.  (C-1..3, 6)
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

        static const Obj A(bdlt::DatetimeTz(bdlt::Datetime(2011, 5, 3, 15),
                                           -4 * 60),
                           "EDT");

        static const Obj B(bdlt::DatetimeTz(bdlt::Datetime(2011, 1, 9, 10),
                                           2 * 60),
                           "IST");

        static const struct {
            int         d_line;           // source line number
            int         d_level;
            int         d_spacesPerLevel;
            const Obj  *d_object_p;
            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"
#define SP " "

        // -------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1, -8 }  -->  4 expected o/ps
        // -------------------------------------------------------------------

        //LINE L SPL  OBJ   EXPECTED
        //---- - ---  ---   ---------------------------------------------------

        { L_,  0,  0,  &A,  "["                                              NL
                            "datetimeTz = 03MAY2011_15:00:00.000-0400"       NL
                            "timeZoneId = \"EDT\""                           NL
                            "]"                                              NL
                                                                             },

        { L_,  0,  1,  &A,  "["                                              NL
                            " datetimeTz = 03MAY2011_15:00:00.000-0400"      NL
                            " timeZoneId = \"EDT\""                          NL
                            "]"                                              NL
                                                                             },

        { L_,  0, -1,  &A,  "["                                              SP
                            "datetimeTz = 03MAY2011_15:00:00.000-0400"       SP
                            "timeZoneId = \"EDT\""                           SP
                            "]"
                                                                             },

        { L_,  0, -8,  &A,  "["                                              NL
                            "    datetimeTz = 03MAY2011_15:00:00.000-0400"   NL
                            "    timeZoneId = \"EDT\""                       NL
                            "]"                                              NL
                                                                             },
        // -------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2 }      -->  8 expected o/ps
        // -------------------------------------------------------------------

        //LINE L SPL  OBJ   EXPECTED
        //---- - ---  ---   ---------------------------------------------------

        { L_,  3,  0,  &A,  "["                                              NL
                            "datetimeTz = 03MAY2011_15:00:00.000-0400"       NL
                            "timeZoneId = \"EDT\""                           NL
                            "]"                                              NL
                                                                             },

        { L_,  3,  2,  &A,
                          "      ["                                          NL
                          "        datetimeTz = 03MAY2011_15:00:00.000-0400" NL
                          "        timeZoneId = \"EDT\""                     NL
                          "      ]"                                          NL
                                                                             },

        { L_,  3, -2,  &A,  "      ["                                        SP
                            "datetimeTz = 03MAY2011_15:00:00.000-0400"       SP
                            "timeZoneId = \"EDT\""                           SP
                            "]"
                                                                             },

        { L_,  3, -8,  &A,
                  "            ["                                            NL
                  "                datetimeTz = 03MAY2011_15:00:00.000-0400" NL
                  "                timeZoneId = \"EDT\""                     NL
                  "            ]"                                            NL
                                                                             },

        { L_, -3,  0,  &A,  "["                                              NL
                            "datetimeTz = 03MAY2011_15:00:00.000-0400"       NL
                            "timeZoneId = \"EDT\""                           NL
                            "]"                                              NL
                                                                             },

        { L_, -3,  2,  &A,
                          "["                                                NL
                          "        datetimeTz = 03MAY2011_15:00:00.000-0400" NL
                          "        timeZoneId = \"EDT\""                     NL
                          "      ]"                                          NL
                                                                             },

        { L_, -3, -2,  &A,  "["                                              SP
                            "datetimeTz = 03MAY2011_15:00:00.000-0400"       SP
                            "timeZoneId = \"EDT\""                           SP
                            "]"
                                                                             },

        { L_, -3, -8,  &A,
                  "["                                                        NL
                  "                datetimeTz = 03MAY2011_15:00:00.000-0400" NL
                  "                timeZoneId = \"EDT\""                     NL
                  "            ]"                                            NL
                                                                             },

        // ------------------------------------------------------------------
        // P-2.1.3: { B } x { 2 }     x { 3 }             -->  1 expected o/p
        // ------------------------------------------------------------------

        //LINE L SPL  OBJ   EXPECTED
        //---- - ---  ---   ---------------------------------------------------

        { L_,  2,  3,  &B,
                         "      ["                                           NL
                         "         datetimeTz = 09JAN2011_10:00:00.000+0200" NL
                         "         timeZoneId = \"IST\""                     NL
                         "      ]"                                           NL
                                                                             },
        // -------------------------------------------------------------------
        // P-2.1.4: { A B } x { -8 }  x { -8 }            -->  2 expected o/ps
        // -------------------------------------------------------------------

        //LINE L SPL  OBJ   EXPECTED
        //---- - ---  ---   ---------------------------------------------------

        { L_, -8, -8,  &A,
                           "["                                               NL
                           "    datetimeTz = 03MAY2011_15:00:00.000-0400"    NL
                           "    timeZoneId = \"EDT\""                        NL
                           "]"                                               NL
                                                                             },

        { L_, -8, -8,  &B,
                           "["                                               NL
                           "    datetimeTz = 09JAN2011_10:00:00.000+0200"    NL
                           "    timeZoneId = \"IST\""                        NL
                           "]"                                               NL
                                                                             },
        // -------------------------------------------------------------------
        // P-2.1.5: { A B } x { -9 }  x { -9 }            -->  2 expected o/ps
        // -------------------------------------------------------------------

        //LINE L SPL  OBJ   EXPECTED
        //---- - ---  ---   ---------------------------------------------------

        { L_, -9, -9,  &A,  "["                                              SP
                            "03MAY2011_15:00:00.000-0400"                    SP
                            "\"EDT\""                                        SP
                            "]"
                                                                             },

        { L_, -9, -9,  &B,  "["                                              SP
                            "09JAN2011_10:00:00.000+0200"                    SP
                            "\"IST\""                                        SP
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
                const int   LINE =  DATA[ti].d_line;
                const int   L    =  DATA[ti].d_level;
                const int   SPL  =  DATA[ti].d_spacesPerLevel;
                const Obj&  OBJ  = *DATA[ti].d_object_p;
                const char *EXP  =  DATA[ti].d_expected_p;

                if (veryVerbose) { T_ P_(L) P_(SPL) P(OBJ) }

                if (veryVeryVerbose) { T_ T_ Q(EXP) cout << EXP; }

                ostringstream os;

                // Verify supplied stream is returned by reference.

                if (-9 == L && -9 == SPL) {
                    LOOP_ASSERT(LINE, &os == &(os << OBJ));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {
                    LOOP_ASSERT(LINE, -8 == SPL || -8 != L);

                    if (-8 != SPL) {
                        LOOP_ASSERT(LINE, &os == &OBJ.print(os, L, SPL));
                    }
                    else if (-8 != L) {
                        LOOP_ASSERT(LINE, &os == &OBJ.print(os, L));
                    }
                    else {
                        LOOP_ASSERT(LINE, &os == &OBJ.print(os));
                    }

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
        //   bslma::Allocator *allocator() const;
        //   const bdlt::DatetimeTz& datetimeTz() const;
        //   const bsl::string& timeZoneId() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // Attribute Types

        typedef bdlt::DatetimeTz T1;  // 'datetimeTz'
        typedef bsl::string      T2;  // 'timeZoneId'

        // -----------------------------------------------------
        // 'D' values: These are the default-constructed values.
        // -----------------------------------------------------

        const T1 D1;       // default value
        const T2 D2 = "";  // default value

        // -------------------------------------------------------
        // 'A' values: Should cause memory allocation if possible.
        // -------------------------------------------------------

        const T1 A1(bdlt::Datetime(2011, 5, 3, 15), -4 * 60);
        const T2 A2 = "a_" SUFFICIENTLY_LONG_STRING;

        if (verbose) cout <<
           "\nCreate two test allocators; install one as the default." << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
                 "\nCreate an object, passing in the other allocator." << endl;

        Obj mX(&oa);  const Obj& X = mX;

        if (verbose) cout <<
                "\nVerify all basic accessors report expected values." << endl;
        {
            const T1& datetimeTz = X.datetimeTz();
            LOOP2_ASSERT(D1, datetimeTz, D1 == datetimeTz);

            const T2 timeZoneId = X.timeZoneId();
            LOOP2_ASSERT(D2, timeZoneId, D2 == timeZoneId);

            ASSERT(&oa == X.allocator());
        }

        if (verbose) cout <<
            "\nApply primary manipulators and verify expected values." << endl;

        if (veryVerbose) { T_ Q(datetimeTz) }
        {
            mX.setDatetimeTz(A1);

            TestAllocatorMonitor oam(oa), dam(da);

            const T1& datetimeTz = X.datetimeTz();
            LOOP2_ASSERT(A1, datetimeTz, A1 == datetimeTz);

            ASSERT(oam.isInUseSame());  ASSERT(dam.isInUseSame());
        }

        if (veryVerbose) { T_ Q(timeZoneId) }
        {
            mX.setTimeZoneId(A2);

            TestAllocatorMonitor oam(oa), dam(da);

            const T2& timeZoneId = X.timeZoneId();
            LOOP2_ASSERT(A2, timeZoneId, A2 == timeZoneId);

            ASSERT(oam.isTotalSame());  ASSERT(dam.isTotalSame());
        }

        // Double check that some object memory was allocated.

        ASSERT(1 <= oa.numBlocksTotal());

        // Note that memory should be independently allocated for each
        // attribute capable of allocating memory.

        LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // VALUE CTOR
        //   Ensure that we can put an object into any initial state relevant
        //   for thorough testing.
        //
        // Concerns:
        //: 1 The value constructor (with or without a supplied allocator) can
        //:   create an object having any value that does not violate the
        //:   constructor's documented preconditions.
        //:
        //: 2 Any string arguments can be of type 'char *' or 'string'.
        //:
        //: 3 Any argument can be 'const'.
        //:
        //: 4 If an allocator is NOT supplied to the value constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 5 If an allocator IS supplied to the value constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 6 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 7 Supplying an allocator to the value constructor has no effect
        //:   on subsequent object values.
        //:
        //: 8 Any memory allocation is from the object allocator.
        //:
        //: 9 There is no temporary memory allocation from any allocator.
        //:
        //:10 Every object releases any allocated memory at destruction.
        //:
        //:11 QoI: Creating an object having the default-constructed value
        //:   allocates no memory.
        //:
        //:12 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Using the table-driven technique:
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
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1, 3..11)
        //:
        //:   1 Execute an inner loop creating three distinct objects, in turn,
        //:     each object having the same value, 'V', but configured
        //:     differently: (a) without passing an allocator, (b) passing a
        //:     null allocator address explicitly, and (c) passing the address
        //:     of a test allocator distinct from the default allocator.
        //:
        //:   2 For each of the three iterations in P-2.1:  (C-1, 4..11)
        //:
        //:     1 Create three 'bslma::TestAllocator' objects, and install one
        //:       as the current default allocator (note that a ubiquitous test
        //:       allocator is already installed as the global allocator).
        //:
        //:     2 Use the value constructor to dynamically create an object
        //:       having the value 'V', with its object allocator configured
        //:       appropriately (see P-2.1), supplying all the arguments as
        //:       'const' and representing any string arguments as 'char *';
        //:       use a distinct test allocator for the object's footprint.
        //:
        //:     3 Use the (as yet unproven) salient attribute accessors to
        //:       verify that all of the attributes of each object have their
        //:       expected values.  (C-1, 7)
        //:
        //:     4 Use the 'allocator' accessor of each underlying attribute
        //:       capable of allocating memory to ensure that its object
        //:       allocator is properly installed; also invoke the (as yet
        //:       unproven) 'allocator' accessor of the object under test.
        //:       (C-8)
        //:
        //:     5 Use the appropriate test allocators to verify that:  (C-4..6,
        //:       9..11)
        //:
        //:       1 An object that IS expected to allocate memory does so
        //:         from the object allocator only (irrespective of the
        //:         specific number of allocations or the total amount of
        //:         memory allocated).  (C-4, 6)
        //:
        //:       2 An object that is expected NOT to allocate memory doesn't.
        //:         (C-11)
        //:
        //:       3 If an allocator was supplied at construction (P-2.1c), the
        //:         default allocator doesn't allocate any memory.  (C-5)
        //:
        //:       4 No temporary memory is allocated from the object allocator.
        //:         (C-9)
        //:
        //:       5 All object memory is released when the object is destroyed.
        //:         (C-10)
        //:
        //: 3 Repeat the steps in P-2 for the supplied allocator configuration
        //:   (P-2.1c) on the data of P-1, but this time create the object as
        //:   an automatic variable in the presence of injected exceptions
        //:   (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros);
        //:   represent any string arguments in terms of 'string' using a
        //:   "scratch" allocator.  (C-2, 12)
        //
        // Testing:
        //   baltzo::LocalDatetime(DatetimeTz& d, const char *t, *bA = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VALUE CTOR" << endl
                          << "==========" << endl;

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;

        bdlt::DatetimeTz defaultDtz;

        bdlt::Datetime   smallDt; smallDt.addMilliseconds(1);
        bdlt::Datetime    someDt(2011,  5,  3, 15, 32);
        bdlt::Datetime   largeDt(9999, 12, 31, 23, 59, 59, 999);

        bdlt::DatetimeTz smallDtz(smallDt, -(24 * 60 - 1));
        bdlt::DatetimeTz  someDtz( someDt, -( 4 * 60 - 0));
        bdlt::DatetimeTz largeDtz(largeDt,  (24 * 60 - 1));

        const char *defaultTzId = "";
        const char   *smallTzId = "a";
        const char   *largeTzId = LONGEST_STRING;

        const struct {
            int                d_line;  // source line number
            char               d_mem;   // expected allocation: 'Y', 'N', '?'
            bdlt::DatetimeTz   *d_datetimeTz;
            const char *const  d_timeZoneId;
        } DATA[] = {

            //LINE  MEM  DTTZ          TZID
            //----  ---  -----------   -----------

            // default (must be first)
            { L_,   'N', &defaultDtz,  defaultTzId },

            // 'datetimeTz'
            { L_,   'N',   &smallDtz,  defaultTzId },
            { L_,   'N',    &someDtz,  defaultTzId },
            { L_,   'N',   &largeDtz,  defaultTzId },

            // 'timeZoneId'
            { L_,   '?', &defaultDtz,    smallTzId },
            { L_,   'Y', &defaultDtz,    largeTzId },

            // other
            { L_,   '?',    &someDtz,    smallTzId },
            { L_,   'Y',    &someDtz,    largeTzId },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory.
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int              LINE =  DATA[ti].d_line;
                const char             MEM  =  DATA[ti].d_mem;
                const bdlt::DatetimeTz& DTTZ = *DATA[ti].d_datetimeTz;
                const char *const      TZID =  DATA[ti].d_timeZoneId;

                if (veryVerbose) { T_ P_(MEM) P_(DTTZ) P(TZID) }

                LOOP2_ASSERT(LINE, MEM, MEM && strchr("YN?", MEM));

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    if (veryVerbose) { T_ T_ P(CONFIG) }

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj                  *objPtr;
                    bslma::TestAllocator *objAllocatorPtr;

                    switch (CONFIG) {
                      case 'a': {
                        objPtr = new (fa) Obj(DTTZ, TZID);
                        objAllocatorPtr = &da;
                      } break;
                      case 'b': {
                        objPtr = new (fa) Obj(DTTZ, TZID, 0);
                        objAllocatorPtr = &da;
                      } break;
                      case 'c': {
                        objPtr = new (fa) Obj(DTTZ, TZID, &sa);
                        objAllocatorPtr = &sa;
                      } break;
                      default: {
                        LOOP2_ASSERT(LINE, CONFIG, !"Bad allocator config.");
                      } break;
                    }
                    LOOP2_ASSERT(LINE, CONFIG,
                                 sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                    // Use untested functionality to help ensure the first row
                    // of the table contains the default-constructed value.

                    static bool firstFlag = true;
                    if (firstFlag) {
                        LOOP4_ASSERT(LINE, CONFIG, Obj(), *objPtr,
                                     Obj() == *objPtr)
                        firstFlag = false;
                    }

                    // -------------------------------------
                    // Verify the object's attribute values.
                    // -------------------------------------

                    LOOP4_ASSERT(LINE, CONFIG, DTTZ, X.datetimeTz(),
                                 DTTZ == X.datetimeTz());

                    LOOP4_ASSERT(LINE, CONFIG, TZID, X.timeZoneId(),
                                 TZID == X.timeZoneId());

                    // -------------------------------------------------------
                    // Verify any attribute allocators are installed properly.
                    // -------------------------------------------------------

                    LOOP2_ASSERT(LINE, CONFIG,
                                 &oa == X.timeZoneId().allocator());

                    // Also invoke the object's 'allocator' accessor.

                    LOOP4_ASSERT(LINE, CONFIG, &oa, X.allocator(),
                                 &oa == X.allocator());

                    // Verify no allocation from the non-object allocator.

                    LOOP3_ASSERT(LINE, CONFIG, noa.numBlocksTotal(),
                                 0 == noa.numBlocksTotal());

                    // Verify no temporary memory is allocated from the object
                    // allocator.

                    LOOP4_ASSERT(LINE, CONFIG, oa.numBlocksTotal(),
                                                           oa.numBlocksInUse(),
                                 oa.numBlocksTotal() == oa.numBlocksInUse());

                    // Verify expected ('Y'/'N') object-memory allocations.

                    if ('?' != MEM) {
                        LOOP4_ASSERT(LINE, CONFIG, MEM, oa.numBlocksInUse(),
                                   ('N' == MEM) == (0 == oa.numBlocksInUse()));
                    }

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    LOOP3_ASSERT(LINE, CONFIG, da.numBlocksInUse(),
                                 0 == da.numBlocksInUse());
                    LOOP3_ASSERT(LINE, CONFIG, fa.numBlocksInUse(),
                                 0 == fa.numBlocksInUse());
                    LOOP3_ASSERT(LINE, CONFIG, sa.numBlocksInUse(),
                                 0 == sa.numBlocksInUse());

                }  // end foreach configuration

            }  // end foreach row

            // Double check that some object memory was allocated.

            ASSERT(anyObjectMemoryAllocatedFlag);

            // Note that memory should be independently allocated for each
            // attribute capable of allocating memory.
        }

        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            // Note that any string arguments are now of type 'string', which
            // require their own "scratch" allocator.

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int              LINE =  DATA[ti].d_line;
                const char             MEM  =  DATA[ti].d_mem;
                const bdlt::DatetimeTz& DTTZ = *DATA[ti].d_datetimeTz;
                const char *const      TZID =  DATA[ti].d_timeZoneId;

                if (veryVerbose) { T_ P_(MEM) P_(DTTZ) P(TZID) }

                bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj obj(DTTZ, TZID, &sa);
                    LOOP3_ASSERT(LINE, DTTZ, obj.datetimeTz(),
                                 DTTZ == obj.datetimeTz());
                    LOOP3_ASSERT(LINE, TZID, obj.timeZoneId(),
                                 TZID == obj.timeZoneId());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP2_ASSERT(LINE, da.numBlocksInUse(),
                             0 == da.numBlocksInUse());
                LOOP2_ASSERT(LINE, sa.numBlocksInUse(),
                             0 == sa.numBlocksInUse());
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR
        //   Ensure that we can use the default constructor to create an
        //   object (having the default-constructed value), use the primary
        //   manipulators to put that object into any state relevant for
        //   thorough testing, and use the destructor to destroy it safely.
        //
        // Concerns:
        //: 1 An object created with the default constructor (with or without
        //:   a supplied allocator) has the contractually specified default
        //:   value.
        //:
        //: 2 If an allocator is NOT supplied to the default constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 3 If an allocator IS supplied to the default constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the default constructor has no effect
        //:   on subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 QoI: The default constructor allocates no memory.
        //:
        //:10 Each attribute is modifiable independently.
        //:
        //:11 Each attribute can be set to represent any value that does not
        //:   violate that attribute's documented constraints.
        //:
        //:12 Any string arguments can be of type 'char *' or 'string'.
        //:
        //:13 Any argument can be 'const'.
        //:
        //:14 Any memory allocation is exception neutral.
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
        //: 2 Using a loop-based approach, default-construct three distinct
        //:   objects, in turn, but configured differently: (a) without passing
        //:   an allocator, (b) passing a null allocator address explicitly,
        //:   and (c) passing the address of a test allocator distinct from the
        //:   default.  For each of these three iterations:  (C-1..14)
        //:
        //:   1 Create three 'bslma::TestAllocator' objects, and install one as
        //:     as the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Use the default constructor to dynamically create an object
        //:     'X', with its object allocator configured appropriately (see
        //:     P-2); use a distinct test allocator for the object's footprint.
        //:
        //:   3 Use the 'allocator' accessor of each underlying attribute
        //:     capable of allocating memory to ensure that its object
        //:     allocator is properly installed; also invoke the (as yet
        //:     unproven) 'allocator' accessor of the object under test.
        //:     (C-2..4)
        //:
        //:   4 Use the appropriate test allocators to verify that no memory
        //:     is allocated by the default constructor.  (C-9)
        //:
        //:   5 Use the individual (as yet unproven) salient attribute
        //:     accessors to verify the default-constructed value.  (C-1)
        //:
        //:   6 For each attribute 'i', in turn, create a local block.  Then
        //:     inside the block, using brute force, set that attribute's
        //:     value, passing a 'const' argument representing each of the
        //:     three test values, in turn (see P-1), first to 'Ai', then to
        //:     'Bi', and finally back to 'Di'.  If attribute 'i' can allocate
        //:     memory, verify that it does so on the first value transition
        //:     ('Di' -> 'Ai'), and that the corresponding primary manipulator
        //:     is exception neutral (using the
        //:     'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).  In all other
        //:     cases, verify that no memory allocation occurs.  After each
        //:     transition, use the (as yet unproven) basic accessors to verify
        //:     that only the intended attribute value changed.  (C-5..6,
        //:     11..14)
        //:
        //:   7 Corroborate that attributes are modifiable independently by
        //:     first setting all of the attributes to their 'A' values.  Then
        //:     incrementally set each attribute to it's corresponding  'B'
        //:     value and verify after each manipulation that only that
        //:     attribute's value changed.  (C-10)
        //:
        //:   8 Verify that no temporary memory is allocated from the object
        //:     allocator.  (C-7)
        //:
        //:   9 Verify that all object memory is released when the object is
        //:     destroyed.  (C-8)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-15)
        //
        // Testing:
        //   baltzo::LocalDatetime(bslma::Allocator *bA = 0);
        //   baltzo::LocalDatetime(DatetimeTz& d, const char *t, *bA = 0);
        //   setDatetimeTz(const bdlt::DatetimeTz& value);
        //   setTimeZoneId(const char *value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                       << "DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR" << endl
                       << "==========================================" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // 'D' values: These are the default-constructed values.

        const bdlt::DatetimeTz D1   = bdlt::DatetimeTz();  // default value
        const char            D2[] = "";                 // default value

        // 'A' values: Should cause memory allocation if possible.

        const bdlt::DatetimeTz A1(bdlt::Datetime(2011, 5, 3, 15), -4 * 60);
        const char            A2[] = "a_" SUFFICIENTLY_LONG_STRING;

        // 'B' values: Should NOT cause allocation (use alternate string type).

        const bdlt::Datetime   maxDatetime(9999, 12, 31, 23, 59, 59, 999);
        const bdlt::DatetimeTz B1   = bdlt::DatetimeTz(maxDatetime, 1440 - 1);
        const char            B2[] = "xyz";

        if (verbose) cout << "\nTesting with various allocator configurations."
                          << endl;

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr;
            bslma::TestAllocator *objAllocatorPtr;

            switch (CONFIG) {
              case 'a': {
                objPtr = new (fa) Obj();
                objAllocatorPtr = &da;
              } break;
              case 'b': {
                objPtr = new (fa) Obj(0);
                objAllocatorPtr = &da;
              } break;
              case 'c': {
                objPtr = new (fa) Obj(&sa);
                objAllocatorPtr = &sa;
              } break;
              default: {
                LOOP_ASSERT(CONFIG, !"Bad allocator config.");
              } break;
            }

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            // -------------------------------------------------------
            // Verify any attribute allocators are installed properly.
            // -------------------------------------------------------

            LOOP_ASSERT(CONFIG, &oa == X.timeZoneId().allocator());

            // Also invoke the object's 'allocator' accessor.

            LOOP3_ASSERT(CONFIG, &oa, X.allocator(), &oa == X.allocator());

            // Verify no allocation from the object/non-object allocators.

            LOOP2_ASSERT(CONFIG,  oa.numBlocksTotal(),
                         0 ==  oa.numBlocksTotal());
            LOOP2_ASSERT(CONFIG, noa.numBlocksTotal(),
                         0 == noa.numBlocksTotal());

            // -------------------------------------
            // Verify the object's attribute values.
            // -------------------------------------

            LOOP3_ASSERT(CONFIG, D1, X.datetimeTz(),
                         D1 == X.datetimeTz());
            LOOP3_ASSERT(CONFIG, D2, X.timeZoneId(),
                         D2 == X.timeZoneId());

            // -----------------------------------------------------
            // Verify that each attribute is independently settable.
            // -----------------------------------------------------

            // 'datetimeTz'
            {
                TestAllocatorMonitor tam(oa);

                mX.setDatetimeTz(A1);
                LOOP_ASSERT(CONFIG, A1 == X.datetimeTz());
                LOOP_ASSERT(CONFIG, D2 == X.timeZoneId());

                mX.setDatetimeTz(B1);
                LOOP_ASSERT(CONFIG, B1 == X.datetimeTz());
                LOOP_ASSERT(CONFIG, D2 == X.timeZoneId());

                mX.setDatetimeTz(D1);
                LOOP_ASSERT(CONFIG, D1 == X.datetimeTz());
                LOOP_ASSERT(CONFIG, D2 == X.timeZoneId());

                LOOP_ASSERT(CONFIG, tam.isTotalSame());
            }

            // 'timeZoneId'
            {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    TestAllocatorMonitor tam(oa);
                    mX.setTimeZoneId(A2);
                    LOOP_ASSERT(CONFIG, tam.isInUseUp());
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                LOOP_ASSERT(CONFIG, D1 == X.datetimeTz());
                LOOP_ASSERT(CONFIG, A2 == X.timeZoneId());

                TestAllocatorMonitor tam(oa);

                mX.setTimeZoneId(B2);
                LOOP_ASSERT(CONFIG, D1 == X.datetimeTz());
                LOOP_ASSERT(CONFIG, B2 == X.timeZoneId());

                mX.setTimeZoneId(D2);
                LOOP_ASSERT(CONFIG, D1 == X.datetimeTz());
                LOOP_ASSERT(CONFIG, D2 == X.timeZoneId());

                LOOP_ASSERT(CONFIG, tam.isTotalSame());
            }

            // Corroborate attribute independence.
            {
                // Set all attributes to their 'A' values.

                mX.setDatetimeTz(A1);
                mX.setTimeZoneId(A2);

                LOOP_ASSERT(CONFIG, A1 == X.datetimeTz());
                LOOP_ASSERT(CONFIG, A2 == X.timeZoneId());

                // Set all attributes to their 'B' values.

                mX.setDatetimeTz(B1);
                LOOP_ASSERT(CONFIG, B1 == X.datetimeTz());
                LOOP_ASSERT(CONFIG, A2 == X.timeZoneId());

                mX.setTimeZoneId(B2);
                LOOP_ASSERT(CONFIG, B1 == X.datetimeTz());
                LOOP_ASSERT(CONFIG, B2 == X.timeZoneId());
            }

            // Verify no temporary memory is allocated from the object
            // allocator.

            LOOP2_ASSERT(CONFIG, oa.numBlocksMax(), 1 == oa.numBlocksMax());

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            LOOP_ASSERT(fa.numBlocksInUse(),  0 ==  fa.numBlocksInUse());
            LOOP_ASSERT(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());
            LOOP_ASSERT(noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            // Double check that some object memory was allocated.

            LOOP_ASSERT(CONFIG, 1 <= oa.numBlocksTotal());

            // Note that memory should be independently allocated for each
            // attribute capable of allocating memory.
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

        // Attribute Types

        typedef bdlt::DatetimeTz  T1; // 'datetimeTz'
        typedef const char      *T2; // 'timeZoneId'

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Values for testing

        // Attribute 1 Values: 'datetimeTz'

        const T1 D1;  // default value
        const T1 A1(bdlt::Datetime(2011, 5, 3, 15), -4 * 60);

        // Attribute 2 Values: 'timeZoneId'

        const T2 D2 = "";                 // default value
        const T2 A2 = "America/New_York";

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 1. Create an object 'w' (default ctor)."
                             "\t\t{ w:D             }" << endl;

        Obj mW;  const Obj& W = mW;

        if (veryVerbose) cout << "\ta. Check initial value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(D1 == W.datetimeTz());
        ASSERT(D2 == W.timeZoneId());

        if (veryVerbose) cout <<
                  "\tb. Try equality operators: 'w' <op> 'w'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 2. Create an object 'x' (copy from 'w')."
                             "\t\t{ w:D x:D         }" << endl;

        Obj mX(W);  const Obj& X = mX;

        if (veryVerbose) cout << "\ta. Check initial value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(D1 == X.datetimeTz());
        ASSERT(D2 == X.timeZoneId());

        if (veryVerbose) cout <<
                   "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(1 == (X == W));        ASSERT(0 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 3. Set 'x' to 'A' (value distinct from 'D')."
                             "\t\t{ w:D x:A         }" << endl;

        mX.setDatetimeTz(A1);
        mX.setTimeZoneId(A2);

        if (veryVerbose) cout << "\ta. Check new value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == X.datetimeTz());
        ASSERT(A2 == X.timeZoneId());

        if (veryVerbose) cout <<
             "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 4. Create an object 'y' (init. to 'A')."
                             "\t\t{ w:D x:A y:A     }" << endl;

        Obj mY(A1, A2);  const Obj& Y = mY;

        if (veryVerbose) cout << "\ta. Check initial value of 'y'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Y) }

        ASSERT(A1 == Y.datetimeTz());
        ASSERT(A2 == Y.timeZoneId());

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

        ASSERT(A1 == Z.datetimeTz());
        ASSERT(A2 == Z.timeZoneId());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (Z == W));        ASSERT(1 == (Z != W));
        ASSERT(1 == (Z == X));        ASSERT(0 == (Z != X));
        ASSERT(1 == (Z == Y));        ASSERT(0 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 6. Set 'z' to 'D' (the default value)."
                             "\t\t\t{ w:D x:A y:A z:D }" << endl;

        mZ.setDatetimeTz(D1);
        mZ.setTimeZoneId(D2);

        if (veryVerbose) cout << "\ta. Check new value of 'z'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Z) }

        ASSERT(D1 == Z.datetimeTz());
        ASSERT(D2 == Z.timeZoneId());

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

        ASSERT(A1 == W.datetimeTz());
        ASSERT(A2 == W.timeZoneId());

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

        ASSERT(D1 == W.datetimeTz());
        ASSERT(D2 == W.timeZoneId());

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

        ASSERT(A1 == X.datetimeTz());
        ASSERT(A2 == X.timeZoneId());

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
