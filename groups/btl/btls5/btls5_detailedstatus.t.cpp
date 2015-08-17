// btls5_detailedstatus.t.cpp                                         -*-C++-*-
#include <btls5_detailedstatus.h>

#include <btlso_endpoint.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The value-semantic struct 'DetailedStatus' provides information about
// the status of a SOCKS5 negotiation.  There are two attributes in the object:
// the human-readable 'description', and possibly empty 'address'.  The
// attribute values can be set at construction or subsequently using the two
// setter methods.
//
// The setters and getters are used to verify the functionality of this type,
// and 'bslma::TestAllocator' is used to check proper allocator use.
//
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] explicit DetailedStatus(bslma::Allocator *basicAllocator = 0);
// [ 3] DetailedStatus(const StringRef& d, *a = 0);
// [ 3] DetailedStatus(const StringRef& d, Endpoint& a, *a = 0);
// [ 7] DetailedStatus(const DetailedStatus& original, *a = 0);
// [ 2] ~DetailedStatus() = default;
//
// MANIPULATORS
// [ 9] DetailedStatus& operator=(DetailedStatus& rhs);
// [ 2] setDescription(const bslstl::StringRef& d);
// [ 2] setAddress(const Endpoint& a);
// [ 8] void swap(DetailedStatus& other);
//
// ACCESSORS
// [ 4] const string& description() const;
// [ 4] const btlso::Endpoint& address() const;
// [ 4] bslma::Allocator *allocator() const;
// [ 5] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [ 6] operator==(DetailedStatus& lhs, DetailedStatus& rhs);
// [ 6] operator!=(DetailedStatus& lhs, DetailedStatus& rhs);
// [ 5] operator<<(ostream& output, const DetailedStatus& object);
//
// FREE FUNCTIONS
// [ 8] void swap(DetailedStatus& a, DetailedStatus& b);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [11] USAGE EXAMPLE
// [ *] CONCERN: All memory allocation is from the object's allocator.
// [ *] CONCERN: Precondition violations are detected when enabled.

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
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
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

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
//                  EXCEPTION TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define EXCEPTION_COUNT bslmaExceptionCounter

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef btls5::DetailedStatus Obj;
typedef btlso::Endpoint       EndPoint;

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

// Define 'bsl::string' value long enough to ensure dynamic memory allocation.

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

struct DefaultDataRow {
    int         d_line;           // source line number
    char        d_mem;            // expected allocation: 'Y', 'N', '?'
    const char *d_description;
    const char *d_hostname;
    int         d_portNum;
};

static
const DefaultDataRow DEFAULT_DATA[] =
{
    //LINE  MEM             DESC       HOSTNAME            PORT
    //----  ---             ----       --------            ----

    // default (must be first)
    { L_,   'N',             "",               "",            0    },

    // 'description'
    { L_,   '?',            "a",          "host1",            1    },
    { L_,   '?',           "AB",          "host2",           16    },
    { L_,   '?',   "1234567890",          "host3",          255    },
    { L_,   'Y',    LONG_STRING,          "host4",         9999    },
    { L_,   'Y',  LONGER_STRING,          "host5",        32767    },
    { L_,   'Y', LONGEST_STRING,          "host6",        65535    },

    // 'hostname'
    { L_,   '?',        "host1",              "a",            1    },
    { L_,   '?',        "host2",             "AB",           16    },
    { L_,   '?',        "host3",     "1234567890",          255    },
    { L_,   'Y',        "host4",      LONG_STRING,         9999    },
    { L_,   'Y',        "host5",    LONGER_STRING,        32767    },
    { L_,   'Y',        "host6",   LONGEST_STRING,        65535    },

};
const int DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

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
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

///Example 1: Assign Error Information
///- - - - - - - - - - - - - - - - - -
// Suppose we encounter an authentication failure during a SOCKS5 negotiation.
// We would like to encode the information about this error before signaling
// the client code of the error.
//
// First, we construct an empty 'btls5::DetailedStatus' object with the failure
// description:
//..
    btls5::DetailedStatus error("authentication failure");
//..
//  Now, we set the address of the proxy host that reported the error:
//..
    btlso::Endpoint proxy("proxy1.corp.com", 1080);
    error.setAddress(proxy);
//..
// Finally, we have an encoded 'error' which provides detailed information
// about the failure.
//..
    ASSERT("authentication failure" == error.description());
    ASSERT(proxy                    == error.address());
//..

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
        //:
        //:15 QoI: Asserted precondition violations are detected when enabled.
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
        //   explicit DetailedStatus(bslma::Allocator *basicAllocator = 0);
        //   ~DetailedStatus() = default;
        //   void setDescription(const bslstl::StringRef& d);
        //   void setAddress(const btlso::Endpoint& a);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                       << "DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR" << endl
                       << "==========================================" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // 'D' values: These are the default-constructed values.

        const char     D1[] = "";       // 'description'
        const EndPoint D2("", 0);       // 'endpoint'

        // 'A' values: Should cause memory allocation if possible.

        const char     A1[] = "a_" SUFFICIENTLY_LONG_STRING;
        const EndPoint A2("b_" SUFFICIENTLY_LONG_STRING, 1);

        // 'B' values: Should NOT cause allocation (use alternate string type).

        const string   B1 = "description";
        const EndPoint B2("hostname", 65535);

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

            LOOP_ASSERT(CONFIG, &oa == X.description().allocator());

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

            LOOP3_ASSERT(CONFIG, D1, X.description(), D1 == X.description());
            LOOP3_ASSERT(CONFIG, D2, X.address(),     D2 == X.address());

            // -----------------------------------------------------
            // Verify that each attribute is independently settable.
            // -----------------------------------------------------

            // 'description'
            {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    bslma::TestAllocatorMonitor tam(&oa);
                    mX.setDescription(A1);
                    LOOP_ASSERT(CONFIG, tam.isInUseUp());

#ifdef BDE_BUILD_TARGET_EXC
                    ASSERT(0 < EXCEPTION_COUNT);
#endif

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                LOOP_ASSERT(CONFIG, A1 == X.description());
                LOOP_ASSERT(CONFIG, D2 == X.address());

                bslma::TestAllocatorMonitor tam(&oa);

                mX.setDescription(B1);
                LOOP_ASSERT(CONFIG, B1 == X.description());
                LOOP_ASSERT(CONFIG, D2 == X.address());

                mX.setDescription(D1);
                LOOP_ASSERT(CONFIG, D1 == X.description());
                LOOP_ASSERT(CONFIG, D2 == X.address());

                LOOP_ASSERT(CONFIG, tam.isTotalSame());
            }

            // 'endpoint'
            {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    bslma::TestAllocatorMonitor tam(&oa);
                    mX.setAddress(A2);
                    LOOP_ASSERT(CONFIG, tam.isInUseUp());

#ifdef BDE_BUILD_TARGET_EXC
                    ASSERT(0 < EXCEPTION_COUNT);
#endif

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                LOOP_ASSERT(CONFIG, D1 == X.description());
                LOOP_ASSERT(CONFIG, A2 == X.address());

                bslma::TestAllocatorMonitor tam(&oa);

                mX.setAddress(B2);
                LOOP_ASSERT(CONFIG, D1 == X.description());
                LOOP_ASSERT(CONFIG, B2 == X.address());

                mX.setAddress(D2);
                LOOP_ASSERT(CONFIG, D1 == X.description());
                LOOP_ASSERT(CONFIG, D2 == X.address());

                LOOP_ASSERT(CONFIG, tam.isTotalSame());
            }

            // Corroborate attribute independence.
            {
                // Set all attributes to their 'A' values.

                mX.setDescription(A1);
                mX.setAddress(A2);

                LOOP_ASSERT(CONFIG, A1 == X.description());
                LOOP_ASSERT(CONFIG, A2 == X.address());

                // Set all attributes to their 'B' values.

                mX.setDescription(B1);
                LOOP_ASSERT(CONFIG, B1 == X.description());
                LOOP_ASSERT(CONFIG, A2 == X.address());

                mX.setAddress(B2);
                LOOP_ASSERT(CONFIG, B1 == X.description());
                LOOP_ASSERT(CONFIG, B2 == X.address());
            }

            // Verify no temporary memory is allocated from the object
            // allocator.

            // TBD: Uncomment
//             LOOP2_ASSERT(CONFIG, oa.numBlocksMax(), 3 == oa.numBlocksMax());

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            LOOP_ASSERT(fa.numBlocksInUse(),  0 ==  fa.numBlocksInUse());
            LOOP_ASSERT(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());

            // TBD: Uncomment
//             LOOP_ASSERT(noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

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
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        Obj error1("Error 1");
        ASSERT(error1.description() == "Error 1");
        ASSERT(!error1.address().port());
        verbose && cout << "error1=" << error1 << endl;

        btlso::Endpoint address2("example.com", 80);
        Obj error2("Error 2", address2);
        ASSERT(error2.description() == "Error 2");
        ASSERT(error2.address() == address2);
        ASSERT(error1 != error2);
        verbose && cout << "error2=" << error2 << endl;

        Obj error3(error2);
        ASSERT(error3.description() == error2.description());
        ASSERT(error3.address().port());
        ASSERT(error3 == error2);
        verbose && cout << "error3=" << error3 << endl;

        error1 = error2;
        ASSERT(error1.description() == "Error 2");
        ASSERT(error1.address().port());
        verbose && cout << "error1=" << error3 << endl;

        error2.setDescription("Error 1");
        ASSERT(error2.description() == "Error 1");
        verbose && cout << "error2=" << error2 << endl;

        btlso::Endpoint address3("localhost", 8194);
        error1.setAddress(address3);
        ASSERT(error1.address() == address3);
        verbose && cout << "error1=" << error1 << endl;

        if (verbose) cout << "  c-tors, setters, getters" << endl;

        if (verbose) cout << "  default constructor";
        {
            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard guard(&da);
            bslma::TestAllocator ta("test", veryVeryVeryVerbose);

            // Concern 5: deafult-constructed attributes are empty

            Obj eNone;
            ASSERT(eNone.description().empty());
            ASSERT(eNone.address() == btlso::Endpoint());

            // Concern 3: no allocator supplied

            ASSERT(eNone.allocator() == &da);
            ASSERT(eNone.description().get_allocator().mechanism() == &da);
            ASSERT(eNone.address().allocator() == &da);

            // Concern 4: no allocator supplied

            Obj eZero(0);
            ASSERT(eZero.description().empty());         // C-5
            ASSERT(eZero.address() == btlso::Endpoint());  // C-5
            ASSERT(eZero.allocator() == &da);
            ASSERT(eZero.description().get_allocator().mechanism() == &da);
            ASSERT(eZero.address().allocator() == &da);

            // Concern 2: explicit allocator is injected properly

            Obj eExplicit(&ta);
            ASSERT(eExplicit.description().empty());         // C-5
            ASSERT(eExplicit.address() == btlso::Endpoint());  // C-5
            ASSERT(eExplicit.allocator() == &ta);
            ASSERT(eExplicit.description().get_allocator().mechanism() == &ta);
            ASSERT(eExplicit.address().allocator() == &ta);
        }

        Obj error4("Error 1");
        ASSERT(error4.description() == "Error 1");
        ASSERT(!error4.address().port());
        verbose && cout << "error4=" << error4 << endl;

        btlso::Endpoint address5("example.com", 80);
        Obj error5("Error 2", address5);
        ASSERT(error5.description() == "Error 2");
        ASSERT(error5.address() == address5);
        ASSERT(error4 != error5);
        verbose && cout << "error5=" << error5 << endl;

        Obj error6(error5);
        ASSERT(error6.description() == error5.description());
        ASSERT(error6.address().port());
        ASSERT(error6 == error5);
        verbose && cout << "error6=" << error6 << endl;

        error4 = error5;
        ASSERT(error4.description() == "Error 2");
        ASSERT(error4.address().port());
        verbose && cout << "error4=" << error6  << endl;

        error5.setDescription("Error 1");
        ASSERT(error5.description() == "Error 1");
        verbose && cout << "error5=" << error5 << endl;

        btlso::Endpoint address6("localhost", 8194);
        error4.setAddress(address6);
        ASSERT(error4.address() == address6);
        verbose && cout << "error4=" << error4 << endl;

        if (verbose) cout << "  allocator test" << endl;

        bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
        bslma::Default::setDefaultAllocatorRaw(&defaultAllocator);

        bslma::TestAllocator ta1("description", veryVeryVerbose);
        Obj e1("Error", &ta1);
        ASSERT(e1.description().allocator() == &ta1);
        ASSERT(e1.address().hostname().allocator() == &ta1);

        bslma::TestAllocator ta2("description + address", veryVeryVerbose);
        Obj e2("Error w/address", address5, &ta2);
        ASSERT(e2.description().allocator() == &ta2);

        bslma::TestAllocator ta3("copy constructor", veryVeryVerbose);
        Obj e3(e1, &ta3);
        ASSERT(e3.description().allocator() == &ta3);

        // CONCERN: All memory allocation is from the object's allocator.

        LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                    0 == defaultAllocator.numBlocksTotal());

        // Test allocators 'ta1', 'ta2' and 'ta3' will verify deallocation

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
