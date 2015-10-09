// btls5_detailedstatus.t.cpp                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

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
// [11] DetailedStatus(const StringRef& d, *a = 0);
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
// [12] USAGE EXAMPLE
// [ *] CONCERN: All memory allocation is from the object's allocator.
// [ *] CONCERN: Precondition violations are detected when enabled.

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
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

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                    EXCEPTION TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define EXCEPTION_COUNT bslmaExceptionCounter

// ============================================================================
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef btls5::DetailedStatus Obj;
typedef btlso::Endpoint       Endpoint;

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
    { L_,   '?',        "desc1",              "a",            1    },
    { L_,   '?',        "desc2",             "AB",           16    },
    { L_,   '?',        "desc3",     "1234567890",          255    },
    { L_,   'Y',        "desc4",      LONG_STRING,         9999    },
    { L_,   'Y',        "desc5",    LONGER_STRING,        32767    },
    { L_,   'Y',        "desc6",   LONGEST_STRING,        65535    },

};
const int DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

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
      case 11: {
        // --------------------------------------------------------------------
        // ONE-ARG CTOR
        //   Ensure that we can put an object into any initial state relevant
        //   for thorough testing.
        //
        // Concerns:
        //: 1 The one-arg constructor (with or without a supplied allocator)
        //:   can create an object having any value that does not violate the
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
        //:
        //:13 QoI: Asserted precondition violations are detected when enabled.
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
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-13)
        //
        // Testing:
        //   DetailedStatus(const StringRef& u, *a = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ONE-ARG CTOR" << endl
                          << "============" << endl;

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;

        struct DefaultDataRow {
            int         d_line;           // source line number
            char        d_mem;            // expected allocation: 'Y', 'N', '?'
            const char *d_description;
        };

        static const DefaultDataRow DATA[] =
            {
                //LINE  MEM             DESC
                //----  ---             ----

                // default (must be first)
                { L_,   'N',             ""   },

                // 'description'
                { L_,   '?',            "a"   },
                { L_,   '?',           "AB"   },
                { L_,   '?',   "1234567890"   },
                { L_,   'Y',    LONG_STRING   },
                { L_,   'Y',  LONGER_STRING   },
                { L_,   'Y', LONGEST_STRING   },
            };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const Endpoint ADDR;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory.
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const char        MEM    = DATA[ti].d_mem;
                const char *const DESC   = DATA[ti].d_description;

                if (veryVerbose) { T_ P_(MEM) P_(DESC) }

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
                        objPtr = new (fa) Obj(DESC);
                        objAllocatorPtr = &da;
                      } break;
                      case 'b': {
                        objPtr = new (fa) Obj(DESC, 0);
                        objAllocatorPtr = &da;
                      } break;
                      case 'c': {
                        objPtr = new (fa) Obj(DESC, &sa);
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

                    LOOP4_ASSERT(LINE, CONFIG, DESC, X.description(),
                                 DESC == X.description());

                    LOOP4_ASSERT(LINE, CONFIG, ADDR, X.address(),
                                 ADDR == X.address());

                    // -------------------------------------------------------
                    // Verify any attribute allocators are installed properly.
                    // -------------------------------------------------------

                    LOOP2_ASSERT(LINE, CONFIG,
                                 &oa == X.description().allocator());

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
                const int      LINE   = DATA[ti].d_line;
                const char     MEM    = DATA[ti].d_mem;
                const string   DESC(DATA[ti].d_description, &scratch);

                if (veryVerbose) { T_ P_(MEM) P_(DESC) P(ADDR) }

                bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj obj(DESC, &sa);
                    LOOP3_ASSERT(LINE, DESC, obj.description(),
                                 DESC == obj.description());
                    LOOP3_ASSERT(LINE, ADDR, obj.address(),
                                 ADDR == obj.address());

#ifdef BDE_BUILD_TARGET_EXC
                    if ('Y' == MEM) {
                        LOOP_ASSERT(LINE, 0 < EXCEPTION_COUNT);
                    }
#endif

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP2_ASSERT(LINE, da.numBlocksInUse(),
                             0 == da.numBlocksInUse());
                LOOP2_ASSERT(LINE, sa.numBlocksInUse(),
                             0 == sa.numBlocksInUse());
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            const Endpoint ADDR;

            if (veryVerbose) cout << "\tdescription" << endl;
            {
                ASSERT_SAFE_PASS(Obj(""));
#if 0
                // no null bslstl::StringRef
                ASSERT_SAFE_FAIL(Obj( 0));
#endif
            }
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
        //:   described in P-3:  (C-9..10)
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
        //:     target object, 'Z', still has the same value as that of 'ZZ'.
        //:     (C-10)
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
        //   operator=(const DetailedStatus& rhs);
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
        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const char        MEMSRC1 = DATA[ti].d_mem;
            const char *const DESC1   = DATA[ti].d_description;
            const char *const NAME1   = DATA[ti].d_hostname;
            const int         PORT1   = DATA[ti].d_portNum;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const Endpoint    ADDR1(NAME1, PORT1, &scratch);

            const Obj  Z(DESC1, ADDR1, &scratch);
            const Obj ZZ(DESC1, ADDR1, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE1, Obj(), Z, Obj() == Z);
                firstFlag = false;
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const char        MEMDST2 = DATA[tj].d_mem;
                const char *const DESC2   = DATA[tj].d_description;
                const char *const NAME2   = DATA[tj].d_hostname;
                const int         PORT2   = DATA[tj].d_portNum;
                const Endpoint    ADDR2(NAME2, PORT2, &scratch);

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(DESC2, ADDR2, &oa);  const Obj& X = mX;

                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    LOOP4_ASSERT(LINE1, LINE2, Z, X,
                                 (Z == X) == (LINE1 == LINE2));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        Obj *mR = &(mX = Z);
                        LOOP4_ASSERT(LINE1, LINE2,  Z,   X,  Z == X);
                        LOOP4_ASSERT(LINE1, LINE2, mR, &mX, mR == &mX);


#ifdef BDE_BUILD_TARGET_EXC
                        if ('N' == MEMDST2 && 'Y' == MEMSRC1) {
                            LOOP2_ASSERT(LINE1, LINE2, 0 < EXCEPTION_COUNT);
                        }
#endif

                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    LOOP4_ASSERT(LINE1, LINE2, ZZ, Z, ZZ == Z);

                    LOOP4_ASSERT(LINE1, LINE2, &oa, X.allocator(),
                                 &oa == X.allocator());
                    LOOP4_ASSERT(LINE1, LINE2, &scratch, Z.allocator(),
                                 &scratch == Z.allocator());

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

                Obj mX(DESC1, ADDR1, &oa);
                const Obj ZZ(DESC1, ADDR1, &scratch);

                const Obj& Z = mX;

                LOOP3_ASSERT(LINE1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj *mR = &(mX = Z);
                    LOOP3_ASSERT(LINE1, ZZ,   Z, ZZ == Z);
                    LOOP3_ASSERT(LINE1, mR, &mX, mR == &mX);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP3_ASSERT(LINE1, &oa, Z.allocator(), &oa == Z.allocator());

                LOOP_ASSERT(LINE1, !oam.isInUseUp());

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
        //:   5 Use the 'bslalg_SwapUtil' helper function template to swap the
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
        //   void swap(DetailedStatus& other);
        //   void swap(DetailedStatus& a, b);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SWAP MEMBER AND FREE FUNCTIONS" << endl
                          << "==============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each function to a variable." << endl;
        {
            using namespace btls5;

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
        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const char        MEM1    = DATA[ti].d_mem;
            const char *const DESC1   = DATA[ti].d_description;
            const char *const NAME1   = DATA[ti].d_hostname;
            const int         PORT1   = DATA[ti].d_portNum;

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const Endpoint    ADDR1(NAME1, PORT1, &scratch);

            Obj mW(DESC1, ADDR1, &oa);  const Obj& W = mW;
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
                bslma::TestAllocatorMonitor oam(&oa);

                mW.swap(mW);

                LOOP3_ASSERT(LINE1, XX, W, XX == W);
                LOOP_ASSERT(LINE1, &oa == W.allocator());
                LOOP_ASSERT(LINE1, oam.isTotalSame());
            }

            // free function 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

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
                const int         LINE2   = DATA[tj].d_line;
                const char *const DESC2   = DATA[ti].d_description;
                const char *const NAME2   = DATA[ti].d_hostname;
                const int         PORT2   = DATA[ti].d_portNum;
                const Endpoint    ADDR2(NAME2, PORT2, &scratch);

                      Obj mX(XX, &oa);  const Obj& X = mX;

                      Obj mY(DESC2, ADDR2, &oa);  const Obj& Y = mY;
                const Obj YY(Y, &scratch);

                if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

                // member 'swap'
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    mX.swap(mY);

                    LOOP4_ASSERT(LINE1, LINE2, YY, X, YY == X);
                    LOOP4_ASSERT(LINE1, LINE2, XX, Y, XX == Y);
                    LOOP2_ASSERT(LINE1, LINE2, &oa == X.allocator());
                    LOOP2_ASSERT(LINE1, LINE2, &oa == Y.allocator());
                    LOOP2_ASSERT(LINE1, LINE2, oam.isTotalSame());
                }

                // free function 'swap'
                {
                    bslma::TestAllocatorMonitor oam(&oa);

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

            const char     DESC[] = "a_" SUFFICIENTLY_LONG_STRING;
            const char     NAME[] = "b_" SUFFICIENTLY_LONG_STRING;
            const int      PORT   = 255;

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const Endpoint ADDR(NAME, PORT, &scratch);

                  Obj mX(&oa);  const Obj& X = mX;
            const Obj XX(X, &scratch);

                  Obj mY(DESC, ADDR, &oa);  const Obj& Y = mY;
            const Obj YY(Y, &scratch);

            if (veryVerbose) { T_ P_(X) P(Y) }

            bslma::TestAllocatorMonitor oam(&oa);

            bslalg::SwapUtil::swap(&mX, &mY);

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
        //   DetailedStatus(const DetailedStatus& o, *bA = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory.
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const char        MEM    = DATA[ti].d_mem;
                const char *const DESC   = DATA[ti].d_description;
                const char *const NAME   = DATA[ti].d_hostname;
                const int         PORT   = DATA[ti].d_portNum;
                const Endpoint    ADDR(NAME, PORT);

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                const Obj  Z(DESC, ADDR, &scratch);
                const Obj ZZ(DESC, ADDR, &scratch);

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
                                 &oa == X.description().allocator());

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
                const int         LINE   = DATA[ti].d_line;
                const char        MEM    = DATA[ti].d_mem;
                const char *const DESC   = DATA[ti].d_description;
                const char *const NAME   = DATA[ti].d_hostname;
                const int         PORT   = DATA[ti].d_portNum;
                const Endpoint    ADDR(NAME, PORT);

                if (veryVerbose) { T_ P_(MEM) P_(DESC) P(ADDR) }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                const Obj  Z(DESC, ADDR, &scratch);
                const Obj ZZ(DESC, ADDR, &scratch);

                bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj obj(Z, &sa);
                    LOOP3_ASSERT(LINE, Z, obj, Z == obj);

#ifdef BDE_BUILD_TARGET_EXC
                    if ('Y' == MEM) {
                        LOOP_ASSERT(LINE, 0 < EXCEPTION_COUNT);
                    }
#endif

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
        //   bool operator==(const DetailedStatus& lhs, rhs);
        //   bool operator!=(const DetailedStatus& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EQUALITY-COMPARISON OPERATORS" << endl
                          << "=============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            using namespace btls5;
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = operator==;
            operatorPtr operatorNe = operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        if (verbose) cout <<
            "\nDefine appropriate individual attribute values, 'Ai' and 'Bi'."
                                                                       << endl;

        // Attribute Types

        typedef const char     *T1;                // 'description'
        typedef const Endpoint  T2;                // 'address'

        // Attribute 1 Values: 'description'

        const T1 A1 = SUFFICIENTLY_LONG_STRING;     // baseline
        const T1 B1 = LONG_STRING;

        // Attribute 2 Values: 'address'

        const T2 A2(LONGER_STRING, 1);              // baseline
        const T2 B2(LONGEST_STRING, 65535);

        if (verbose) cout <<
            "\nCreate a table of distinct, but similar object values." << endl;

        static const struct {
            int             d_line;           // source line number
            const char     *d_description;
            const Endpoint  d_address;
        } DATA[] = {

        // The first row of the table below represents an object value
        // consisting of "baseline" attribute values (A1..An).  Each subsequent
        // row differs (slightly) from the first in exactly one attribute
        // value (Bi).

        //LINE  DESC  ADDR
        //----  ----  ----

        { L_,    A1,   A2 },          // baseline

        { L_,    B1,   A2 },
        { L_,    B1,   B2 },
        { L_,    A1,   B2 },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "\nCompare every value with every value." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1 = DATA[ti].d_line;
            const char *const DESC1 = DATA[ti].d_description;
            const Endpoint&   ADDR1 = DATA[ti].d_address;

            if (veryVerbose) { T_ P_(LINE1) P_(DESC1) P(ADDR1) }

            // Ensure an object compares correctly with itself (alias test).
            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                const Obj X(DESC1, ADDR1, &scratch);

                LOOP2_ASSERT(LINE1, X,   X == X);
                LOOP2_ASSERT(LINE1, X, !(X != X));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const char *const DESC2   = DATA[tj].d_description;
                const Endpoint&   ADDR2   = DATA[tj].d_address;

                if (veryVerbose) { T_ T_ P_(LINE2) P_(DESC2) P(ADDR2) }

                const bool EXP = ti == tj;  // expected for equality comparison

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                    const char CONFIG = cfg;  // Determines 'Y's allocator.

                    // Create two distinct test allocators, 'oax' and 'oay'.

                    bslma::TestAllocator oax("objectx", veryVeryVeryVerbose);
                    bslma::TestAllocator oay("objecty", veryVeryVeryVerbose);

                    // Map allocators above to objects 'X' and 'Y' below.

                    bslma::TestAllocator& xa = oax;
                    bslma::TestAllocator& ya = 'a' == CONFIG ? oax : oay;

                    const Obj X(DESC1, ADDR1, &xa);
                    const Obj Y(DESC2, ADDR2, &ya);

                    if (veryVerbose) {
                        T_ T_ T_ P_(EXP) P_(CONFIG) P_(X) P(Y) }

                    // Verify value, commutativity, and no memory allocation.

                    bslma::TestAllocatorMonitor oaxm(&oax), oaym(&oay);

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
        //:     two formatting parameters, along with the expected output.
        //:
        //:     ( 'value' x  'level'   x 'spacesPerLevel' ):
        //:     1 { A   } x {  0     } x {  0, 1, -1, -8 } --> 3 expected o/ps
        //:     2 { A   } x {  3, -3 } x {  0, 2, -2, -8 } --> 6 expected o/ps
        //:     3 { B   } x {  2     } x {  3            } --> 1 expected o/p
        //:     4 { A B } x { -8     } x { -8            } --> 2 expected o/ps
        //:     5 { A B } x { -9     } x { -9            } --> 2 expected o/ps
        //:
        //:   2 For each row in the table defined in P-2.1:  (C-1..3, 5..6, 8)
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
        //   operator<<(ostream& output, const DetailedStatus& object);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR" << endl
                          << "=========================" << endl;

        if (verbose) cout << "\nAssign the addresses of 'print' and "
                             "the output 'operator<<' to variables." << endl;
        {
            using namespace btls5;
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

            const char *d_description;
            const char *d_hostname;
            const int   d_portNum;

            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"
#define SP " "

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1, -8 } -->  4 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL  DESC  NAME PORT  EXP
        //---- - ---  ----  ---- ----  ---

        { L_,  0,  0, "DA", "HA", 255, "["                                   NL
                                       "description = \"DA\""                NL
                                       "address = ["                         NL
                                       "hostname = \"HA\""                   NL
                                       "port = 255"                          NL
                                       "]"                                   NL
                                       "]"                                   NL
                                                                             },

        { L_,  0,  0, "DA", "",   0,   "["                                   NL
                                       "description = \"DA\""                NL
                                       "]"                                   NL
                                                                             },

        { L_,  0,  1, "DA", "HA", 255, "["                                   NL
                                       " description = \"DA\""               NL
                                       " address = ["                        NL
                                       "  hostname = \"HA\""                 NL
                                       "  port = 255"                        NL
                                       " ]"                                  NL
                                       "]"                                   NL
                                                                             },

        { L_,  0,  1, "DA", "",   0,  "["                                    NL
                                       " description = \"DA\""               NL
                                       "]"                                   NL
                                                                             },

        { L_,  0, -1, "DA", "HA", 255, "["                                   SP
                                       "description = \"DA\""                SP
                                       "address = ["                         SP
                                       "hostname = \"HA\""                   SP
                                       "port = 255"                          SP
                                       "]"                                   SP
                                       "]"
                                                                             },

        { L_,  0, -1, "DA", "",   0,   "["                                   SP
                                       "description = \"DA\""                SP
                                       "]"
                                                                             },

        { L_,  0, -8, "DA", "HA", 255, "["                                   NL
                                       "    description = \"DA\""            NL
                                       "    address = ["                     NL
                                       "        hostname = \"HA\""           NL
                                       "        port = 255"                  NL
                                       "    ]"                               NL
                                       "]"                                   NL
                                                                             },

        { L_,  0, -8, "DA", "",   0,   "["                                   NL
                                       "    description = \"DA\""            NL
                                       "]"                                   NL
                                                                             },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2, -8 } -->  6 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL  DESC  NAME PORT  EXP
        //---- - ---  ----  ---- ----  ---

        { L_,  3,  0, "DA", "HA", 255, "["                                   NL
                                       "description = \"DA\""                NL
                                       "address = ["                         NL
                                       "hostname = \"HA\""                   NL
                                       "port = 255"                          NL
                                       "]"                                   NL
                                       "]"                                   NL
                                                                             },

        { L_,  3,  0, "DA", "",   0,   "["                                   NL
                                       "description = \"DA\""                NL
                                       "]"                                   NL
                                                                             },

        { L_,  3,  2, "DA", "HA", 255, "      ["                             NL
                                       "        description = \"DA\""        NL
                                       "        address = ["                 NL
                                       "          hostname = \"HA\""         NL
                                       "          port = 255"                NL
                                       "        ]"                           NL
                                       "      ]"                             NL
                                                                             },

        { L_,  3,  2, "DA", "",   0,   "      ["                             NL
                                       "        description = \"DA\""        NL
                                       "      ]"                             NL
                                                                             },

        { L_,  3, -2, "DA", "HA", 255, "      ["                             SP
                                       "description = \"DA\""                SP
                                       "address = ["                         SP
                                       "hostname = \"HA\""                   SP
                                       "port = 255"                          SP
                                       "]"                                   SP
                                       "]"
                                                                             },

        { L_,  3, -2, "DA", "",   0,   "      ["                             SP
                                       "description = \"DA\""                SP
                                       "]"
                                                                             },

        { L_,  3, -8, "DA", "HA", 255,
                           "            ["                                   NL
                           "                description = \"DA\""            NL
                           "                address = ["                     NL
                           "                    hostname = \"HA\""           NL
                           "                    port = 255"                  NL
                           "                ]"                               NL
                           "            ]"                                   NL
                                                                             },

        { L_,  3, -8, "DA", "",   0,
                           "            ["                                   NL
                           "                description = \"DA\""            NL
                           "            ]"                                   NL
                                                                             },

        { L_, -3,  0, "DA", "HA", 255, "["                                   NL
                                       "description = \"DA\""                NL
                                       "address = ["                         NL
                                       "hostname = \"HA\""                   NL
                                       "port = 255"                          NL
                                       "]"                                   NL
                                       "]"                                   NL
                                                                             },

        { L_, -3,  0, "DA", "",   0,   "["                                   NL
                                       "description = \"DA\""                NL
                                       "]"                                   NL
                                                                             },

        { L_, -3,  2, "DA", "HA", 255, "["                                   NL
                                       "        description = \"DA\""        NL
                                       "        address = ["                 NL
                                       "          hostname = \"HA\""         NL
                                       "          port = 255"                NL
                                       "        ]"                           NL
                                       "      ]"                             NL
                                                                             },

        { L_, -3,  2, "DA", "",   0,   "["                                   NL
                                       "        description = \"DA\""        NL
                                       "      ]"                             NL
                                                                             },

        { L_, -3, -2, "DA", "HA", 255, "["                                   SP
                                       "description = \"DA\""                SP
                                       "address = ["                         SP
                                       "hostname = \"HA\""                   SP
                                       "port = 255"                          SP
                                       "]"                                   SP
                                       "]"
                                                                             },

        { L_, -3, -2, "DA", "",   0,   "["                                   SP
                                       "description = \"DA\""                SP
                                       "]"
                                                                             },

        { L_, -3, -8, "DA", "HA", 255,
                            "["                                              NL
                            "                description = \"DA\""           NL
                            "                address = ["                    NL
                            "                    hostname = \"HA\""          NL
                            "                    port = 255"                 NL
                            "                ]"                              NL
                            "            ]"                                  NL
                                                                             },

        { L_, -3, -8, "DA", "",   0,
                            "["                                              NL
                            "                description = \"DA\""           NL
                            "            ]"                                  NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 }     x { 3 }            -->  1 expected o/p
        // -----------------------------------------------------------------

        //LINE L SPL  DESC  NAME PORT  EXP
        //---- - ---  ----  ---- ----  ---

        { L_,  2,  3, "DB", "HB", 255, "      ["                             NL
                                       "         description = \"DB\""       NL
                                       "         address = ["                NL
                                       "            hostname = \"HB\""       NL
                                       "            port = 255"              NL
                                       "         ]"                          NL
                                       "      ]"                             NL
                                                                             },

        { L_,  2,  3, "DB", "",   0,   "      ["                             NL
                                       "         description = \"DB\""       NL
                                       "      ]"                             NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -8 }   x { -8 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL  DESC  NAME PORT  EXP
        //---- - ---  ----  ---- ----  ---

        { L_, -8, -8, "DA", "HA", 255, "["                                   NL
                                       "    description = \"DA\""            NL
                                       "    address = ["                     NL
                                       "        hostname = \"HA\""           NL
                                       "        port = 255"                  NL
                                       "    ]"                               NL
                                       "]"                                   NL
                                                                             },

        { L_, -8, -8, "DA", "",   0,   "["                                   NL
                                       "    description = \"DA\""            NL
                                       "]"                                   NL
                                                                             },

        { L_, -8, -8, "DB", "HB", 65535, "["                                 NL
                                         "    description = \"DB\""          NL
                                         "    address = ["                   NL
                                         "        hostname = \"HB\""         NL
                                         "        port = 65535"              NL
                                         "    ]"                             NL
                                         "]"                                 NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.5: { A B } x { -9 }   x { -9 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL  DESC  NAME PORT  EXP
        //---- - ---  ----  ---- ----  ---

        { L_, -9, -9, "DA", "HA", 255,   "[ \"DA\" [ \"HA\" 255 ] ]"         },

        { L_, -9, -9, "DA", "",   0,     "[ \"DA\" ]"                        },

        { L_, -9, -9, "DB", "HB", 65535, "[ \"DB\" [ \"HB\" 65535 ] ]"       },

#undef NL
#undef SP
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const int         L      = DATA[ti].d_level;
                const int         SPL    = DATA[ti].d_spacesPerLevel;
                const char *const DESC   = DATA[ti].d_description;
                const char *const NAME   = DATA[ti].d_hostname;
                const int         PORT   = DATA[ti].d_portNum;
                const Endpoint    ADDR(NAME, PORT);
                const char *const EXP    = DATA[ti].d_expected_p;

                if (veryVerbose) { T_ P_(DESC) P(ADDR) }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                const Obj X(DESC, ADDR);

                ostringstream os;

                // Verify supplied stream is returned by reference.

                if (-9 == L && -9 == SPL) {
                    LOOP_ASSERT(LINE, &os == &(os << X));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {
                    LOOP_ASSERT(LINE, -8 == SPL || -8 != L);

                    if (-8 != SPL) {
                        LOOP_ASSERT(LINE, &os == &X.print(os, L, SPL));
                    }
                    else if (-8 != L) {
                        LOOP_ASSERT(LINE, &os == &X.print(os, L));
                    }
                    else {
                        LOOP_ASSERT(LINE, &os == &X.print(os));
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
        //   const string& description() const;
        //   const Endpoint& address() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        // Attribute Types

        typedef bsl::string T1;     // 'description'
        typedef Endpoint    T2;     // 'address'

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // -----------------------------------------------------
        // 'D' values: These are the default-constructed values.
        // -----------------------------------------------------

        const char     D1[] = "";   // 'description'
        const Endpoint D2;          // 'address'

        // -------------------------------------------------------
        // 'A' values: Should cause memory allocation if possible.
        // -------------------------------------------------------

        const char     A1[] = "a_" SUFFICIENTLY_LONG_STRING;
        const Endpoint A2("b_" SUFFICIENTLY_LONG_STRING, 65535);

        if (verbose) cout <<
           "\nCreate two test allocators; install one as the default." << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("object",  1);

        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
                 "\nCreate an object, passing in the other allocator." << endl;

        Obj mX(&oa);  const Obj& X = mX;

        if (verbose) cout <<
                "\nVerify all basic accessors report expected values." << endl;
        {
            const T1& description = X.description();
            LOOP2_ASSERT(D1, description, D1 == description);

            const T2& address = X.address();
            LOOP2_ASSERT(D2, address, D2 == address);

            ASSERT(&oa == X.allocator());
        }

        if (verbose) cout <<
            "\nApply primary manipulators and verify expected values." << endl;

        if (veryVerbose) { T_ Q(description) }
        {
            mX.setDescription(A1);

            bslma::TestAllocatorMonitor oam(&oa), dam(&da);

            const T1& description = X.description();
            LOOP2_ASSERT(A1, description, A1 == description);

            ASSERT(oam.isTotalSame());  ASSERT(dam.isTotalSame());
        }

        if (veryVerbose) { T_ Q(address) }
        {
            mX.setAddress(A2);

            bslma::TestAllocatorMonitor oam(&oa), dam(&da);

            const T2& address = X.address();
            LOOP2_ASSERT(A2, address, A2 == address);

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
        //:
        //:13 QoI: Asserted precondition violations are detected when enabled.
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
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-13)
        //
        // Testing:
        //   DetailedStatus(const StringRef& u, const Endpoint& p, *a = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VALUE CTOR" << endl
                          << "==========" << endl;

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory.
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const char        MEM    = DATA[ti].d_mem;
                const char *const DESC   = DATA[ti].d_description;
                const char *const NAME   = DATA[ti].d_hostname;
                const int         PORT   = DATA[ti].d_portNum;
                const Endpoint    ADDR(NAME, PORT);

                if (veryVerbose) { T_ P_(MEM) P_(DESC) P(ADDR) }

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
                        objPtr = new (fa) Obj(DESC, ADDR);
                        objAllocatorPtr = &da;
                      } break;
                      case 'b': {
                        objPtr = new (fa) Obj(DESC, ADDR, 0);
                        objAllocatorPtr = &da;
                      } break;
                      case 'c': {
                        objPtr = new (fa) Obj(DESC, ADDR, &sa);
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

                    LOOP4_ASSERT(LINE, CONFIG, DESC, X.description(),
                                 DESC == X.description());

                    LOOP4_ASSERT(LINE, CONFIG, ADDR, X.address(),
                                 ADDR == X.address());

                    // -------------------------------------------------------
                    // Verify any attribute allocators are installed properly.
                    // -------------------------------------------------------

                    LOOP2_ASSERT(LINE, CONFIG,
                                 &oa == X.description().allocator());

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
                const int      LINE   = DATA[ti].d_line;
                const char     MEM    = DATA[ti].d_mem;
                const string   DESC(DATA[ti].d_description, &scratch);
                const Endpoint ADDR(DATA[ti].d_hostname,
                                    DATA[ti].d_portNum,
                                    &scratch);

                if (veryVerbose) { T_ P_(MEM) P_(DESC) P(ADDR) }

                bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj obj(DESC, ADDR, &sa);
                    LOOP3_ASSERT(LINE, DESC, obj.description(),
                                 DESC == obj.description());
                    LOOP3_ASSERT(LINE, ADDR, obj.address(),
                                 ADDR == obj.address());

#ifdef BDE_BUILD_TARGET_EXC
                    if ('Y' == MEM) {
                        LOOP_ASSERT(LINE, 0 < EXCEPTION_COUNT);
                    }
#endif

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP2_ASSERT(LINE, da.numBlocksInUse(),
                             0 == da.numBlocksInUse());
                LOOP2_ASSERT(LINE, sa.numBlocksInUse(),
                             0 == sa.numBlocksInUse());
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            const Endpoint ADDR;

            if (veryVerbose) cout << "\tdescription" << endl;
            {
                ASSERT_SAFE_PASS(Obj("", ADDR));
#if 0
                // no null bslstl::StringRef
                ASSERT_SAFE_FAIL(Obj( 0, ADDR));
#endif
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
        const Endpoint D2("", 0);       // 'endpoint'

        // 'A' values: Should cause memory allocation if possible.

        const char     A1[] = "a_" SUFFICIENTLY_LONG_STRING;
        const Endpoint A2("b_" SUFFICIENTLY_LONG_STRING, 1);

        // 'B' values: Should NOT cause allocation (use alternate string type).

        const string   B1 = "description";
        const Endpoint B2("hostname", 65535);

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

            LOOP2_ASSERT(CONFIG, oa.numBlocksMax(), 2 == oa.numBlocksMax());

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

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj obj;

            if (veryVerbose) cout << "\tdescription" << endl;
            {
                ASSERT_SAFE_PASS(obj.setDescription(""));
#if 0
                // no null bslstl::StringRef
                ASSERT_SAFE_FAIL(obj.setDescription( 0));
#endif
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
