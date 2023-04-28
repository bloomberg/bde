// ball_recordjsonformatter.t.cpp                                     -*-C++-*-
#include <ball_recordjsonformatter.h>

#include <ball_attribute.h>
#include <ball_record.h>
#include <ball_recordattributes.h>
#include <ball_severity.h>
#include <ball_userfields.h>

#include <bdlf_bind.h>

#include <bdlt_currenttime.h>
#include <bdlt_datetime.h>
#include <bdlt_iso8601util.h>
#include <bdlt_localtimeoffset.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmt_threadutil.h>

#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>                  // for 'strcmp'
#include <bsl_iostream.h>
#include <bsl_iomanip.h>
#include <bsl_ostream.h>
#include <bsl_string.h>
#include <bsl_sstream.h>


#ifdef BSLS_PLATFORM_OS_UNIX
   #include <unistd.h>                      // for 'getpid'
#endif

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// The component under test is implemented using two 'bsl::string' objects and
// one 'bdlt::DatetimeInterval' object.  The value-semantic correctness of this
// component therefore largely depends on that of those two contained classes.
// We simply follow the standard 10-case test suite.  In addition, since the
// implemented class is a function object, the 'operator()' method that
// provides string-based formatting support is extensively tested.
//
// CREATORS
// [ 2] RecordJsonFormatter();
// [ 2] RecordJsonFormatter(bslma::Allocator *bA);
// [ 5] RecordJsonFormatter(const RecordJsonFormatter& o, *bA = 0);
// [ 7] RecordJsonFormatter(MovableRef<RecordJsonFormatter> o);
// [ 7] RecordJsonFormatter(MovableRef<RecordJsonFormatter> o, allocator);
//
// MANIPULATORS
// [ 6] RecordJsonFormatter& operator=(const RecordJsonFormatter& rhs);
// [ 8] RecordJsonFormatter& operator=(MovableRef<RecordJsonFormatter> rhs);
// [ 2] int setFormat(const bsl::string_view& format);
// [ 2] void setRecordSeparator(const bsl::string_view& recordSeparator);
//
// ACCESSORS
// [ 4] int operator(bsl::ostream& stream, const Record& record) const;
// [ 3] const bsl::string& format() const;
// [ 3] const bsl::string& recordSeparator() const;
// [ 3] const allocator_type& allocator() const;
//
// FREE OPERATORS
// ----------------------------------------------------------------------------
// [ 1] BREATING TEST
// [ 9] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        bsl::cout << "Error " __FILE__ "(" << line << "): " << message
                  << "    (failed)" << bsl::endl;

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

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef ball::RecordJsonFormatter Obj;
typedef ball::Record              Rec;
typedef ball::RecordAttributes    RA;
typedef ball::UserFields          UF;

typedef bslmf::MovableRefUtil     MoveUtil;

const char *k_DEFAULT_FORMAT = "[\"timestamp\","
                                "\"pid\","
                                "\"tid\","
                                "\"severity\","
                                "\"file\","
                                "\"line\","
                                "\"category\","
                                "\"message\","
                                "\"attributes\"]";
const char *k_DEFAULT_RECORD_SEPARATOR = "\n";

const bsl::string_view k_NULL_SV("\0", 1);

struct DefaultDataRow{
    int              d_line;
    const char      *d_spec_p;
    bsl::string_view d_recordSeparator;
};


static const  DefaultDataRow DEFAULT_DATA[] = {
    //-------------------------------------------------------------------------
    // LINE                          SPEC                          RSEP
    //-------------------------------------------------------------------------
    {  L_,   "",                                                   "\n"      },
    {  L_,   "",                                                   ""        },
    {  L_,   "[\"timestamp\"]",                                    "\n"      },
    {  L_,   "[\"timestamp\"]",                                    " "       },
    {  L_,   "[{\"timestamp\":{\"name\":\"My Time\"}}]",           "\r\n"    },
    {  L_,   "[{\"timestamp\":{\"format\":\"iso8601\"}}]",         k_NULL_SV },
    {  L_,   "[{\"timestamp\":{\"fractionalSecPrecision\":\"none\"}}]",
                                                                   "\n"      },
    {  L_,   "[{\"timestamp\":{\"timeZone\":\"utc\"}}]",           ""        },
    {  L_,   "[\"pid\"]",                                          " "       },
    {  L_,   "[{\"pid\":{\"name\":\"My pid\"}}]",                  "\r\n"    },
    {  L_,   "[\"tid\"]",                                          k_NULL_SV },
    {  L_,   "[{\"tid\":{\"name\":\"My tid\"}}]",                  "\n"      },
    {  L_,   "[{\"tid\":{\"format\":\"decimal\"}}]",               ""        },
    {  L_,   "[\"file\"]",                                         " "       },
    {  L_,   "[{\"file\":{\"name\":\"My file\"}}]",                "\r\n"    },
    {  L_,   "[{\"file\":{\"path\":\"full\"}}]",                   k_NULL_SV },
    {  L_,   "[\"line\"]",                                         "\n"      },
    {  L_,   "[{\"line\":{\"name\":\"My line\"}}]",                ""        },
    {  L_,   "[\"category\"]",                                     " "       },
    {  L_,   "[{\"category\":{\"name\":\"My Category\"}}]",        "\r\n"    },
    {  L_,   "[\"severity\"]",                                     k_NULL_SV },
    {  L_,   "[{\"severity\":{\"name\":\"My Severity\"}}]",        "\n"      },
    {  L_,   "[\"message\"]",                                      ""        },
    {  L_,   "[{\"message\":{\"name\":\"My msg\"}}]",              " "       },
    {  L_,   "[\"attributes\"]",                                   "\r\n"    },
    {  L_,   "[\"a3\",\"a2\",\"a1\"]",                             k_NULL_SV },
};
enum { DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA };

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

struct LocalTimeOffsetUtil {

    static bsls::Types::Int64 d_offset;

    // CLASS METHODS
    static
    bsls::TimeInterval localTimeOffset(const bdlt::Datetime& utcDatetime);
};

bsls::Types::Int64 LocalTimeOffsetUtil::d_offset = 0;

bsls::TimeInterval LocalTimeOffsetUtil::localTimeOffset(const bdlt::Datetime&)
{
    return bsls::TimeInterval(d_offset, 0);
}

}  // close unnamed namespace

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int                 test = (argc > 1) ? bsl::atoi(argv[1]) : 1;
    bool             verbose = (argc > 2);
    bool         veryVerbose = (argc > 3);
    bool     veryVeryVerbose = (argc > 4);
    bool veryVeryVeryVerbose = (argc > 5);

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 9: {
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl
;
// First we instantiate a JSON record formatter:
//..
   ball::RecordJsonFormatter formatter;
//..
// Next we set a format specification to the newly created 'formatter':
//..
   int rc = formatter.setFormat("[\"tid\",\"message\"]");
   BSLS_ASSERT(0 == rc);
//..
// The chosen format specification indicates that, when a record is formatted
// using 'formatter', the thread Id attribute of the record will be output
// followed by the message attribute of the record.
//
// Then we create a default 'ball::Record' and set the thread Id and message
// attributes of the record to dummy values:
//..
   ball::Record record;

   record.fixedFields().setThreadID(6);
   record.fixedFields().setMessage("Hello, World!");
//..
// Finally, invocation of the 'formatter' function object to format 'record' to
// 'bsl::cout':
//..
   formatter(bsl::cout, record);
//..
// yields this output:
//..
//  {"tid":6,"message":"Hello, World!"}
//..
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // MOVE-ASSIGNMENT OPERATOR
        //
        // Concerns:
        //: 1 The signature and return type are standard.
        //:
        //: 2 The reference returned is to the target object (i.e., '*this').
        //:
        //: 3 The move-assignment operator can change the value of a modifiable
        //:   target object to that of any source object.
        //:
        //: 4 The object has its internal memory management system hooked up
        //:   properly so that *all* internally allocated memory draws from a
        //:   user-supplied allocator whenever one is specified.
        //:
        //: 5 The allocator address held by the target object is unchanged.
        //:
        //: 6 The source object is left in a valid but unspecified state and
        //:   the allocator address held by the original object is unchanged.
        //:
        //: 7 Subsequent changes to or destruction of the original object have
        //:   no effect on the move-assigned object and vice-versa.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 Assigning an object to itself behaves as expected (alias-safety).
        //:
        //: 10 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   move-assignment operator defined in this component.
        //:
        //: 2 Create a control object representing the source of the
        //:   assignment, with its own scratch allocator.
        //:
        //: 3 Create an object representing the target of the assignment, with
        //:   its own unique object allocator.
        //:
        //: 4 In a loop consisting of two iterations, create a source object (a
        //:   copy of the control object in P-1) with 1) a different allocator
        //:   than that of target, and 2) the same allocator as that of the
        //:   target.
        //:
        //: 5 Call the move-assignment operator in the presence of exceptions
        //:   during memory allocations (using a 'bslma::TestAllocator' and
        //:   varying its allocation limit) and verify the following:
        //:
        //:   1 The address of the return value is the same as that of the
        //:     target object.
        //:
        //:   2 The object being assigned to has the same value as that of the
        //:     source object before assignment (i.e., the control object).
        //:
        //:   3 Ensure that the source, target, and control objects continue to
        //:     have the correct allocator and that all memory allocations come
        //:     from the appropriate allocators.
        //:
        //:   4 Destroy the source object (after assignment) as a minimal test
        //:     that it is in a valid state, then verify that the target object
        //:     is not affected.
        //:
        //:   5 Verify all memory is released when the source and target
        //:     objects are destroyed.
        //
        // Testing:
        //   RecordJsonFormatter& operator=(MovableRef<RecordJsonFormatter>);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "MOVE-ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(bslmf::MovableRef<Obj>);

            // Verify that the signature and return type are standard.

            operatorPtr operatorMoveAssignment = &Obj::operator=;

            (void)operatorMoveAssignment;  // quash potential compiler warning
        }

        {
            bslma::TestAllocator da("default", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            // Create control object 'W'.
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mW(&scratch); const Obj& W = mW;

            for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                if (veryVerbose) { P(cfg) }

                const char CONFIG = cfg;  // how we specify allocator

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator da(  "default", veryVeryVeryVerbose);
                bslma::TestAllocator oa(   "object", veryVeryVeryVerbose);
                bslma::TestAllocator za("different", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                // Create source object 'Y'.
                Obj                  *srcPtr = 0;
                bslma::TestAllocator *srcAllocatorPtr = 0;

                switch (CONFIG) {
                  case 'a': {
                      srcPtr = new (fa) Obj(&za);
                      srcAllocatorPtr = &za;
                  } break;
                  case 'b': {
                      srcPtr = new (fa) Obj(&oa);
                      srcAllocatorPtr = &oa;
                  } break;
                  default: {
                      ASSERTV(CONFIG, !"Bad allocator config.");
                  } break;
                }
                bslma::TestAllocator& sa = *srcAllocatorPtr;

                Obj& mY = *srcPtr;  const Obj& Y = mY;

                // Verify the value of the object.
                ASSERTV(CONFIG, Y == W);

                // Create target object 'X'.
                Obj *objPtr = new (fa) Obj(&oa);
                Obj& mX = *objPtr;  const Obj& X = mX;
                mX.setFormat("[\"timestamp\"]");

                // Verify the value of the object.
                ASSERTV(CONFIG, X != W);

                bslma::TestAllocatorMonitor oam(&oa), dam(&da);

                Obj *mR = &(mX = bslmf::MovableRefUtil::move(mY));
                (void)mR;

                // Verify the value of the object.
                ASSERTV(CONFIG, X == W);

                // Verify that 'X' and 'Y' have the correct allocator.
                ASSERTV(CONFIG, &oa,   X.allocator().mechanism(),
                                &oa == X.allocator().mechanism());
                ASSERTV(CONFIG, srcAllocatorPtr,   Y.allocator().mechanism(),
                                srcAllocatorPtr == Y.allocator().mechanism());

                // Verify expected object allocator use.
                const bool isTotalSame = 'b' == CONFIG;
                ASSERTV(CONFIG, isTotalSame == oam.isTotalSame());
                ASSERTV(CONFIG, true        == dam.isInUseSame());

                // Destroy the source object and verify that the target
                // object is not affected.
                fa.deleteObject(srcPtr);

                // Verify the value of the object.
                ASSERTV(CONFIG, X == W);

                fa.deleteObject(objPtr);

                ASSERTV(0 == oa.numBlocksInUse());
                ASSERTV(0 == sa.numBlocksInUse());
            }
            {
                // self-assignment

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                {
                    bslma::TestAllocator scratch("scratch",
                                                 veryVeryVeryVerbose);

                    Obj mZ(&scratch);  const Obj& Z = mZ;

                    // Create target object.
                    Obj mX(&oa);  const Obj& X = mX;

                    ASSERTV(X == Z);

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        Obj *mR = &(mX = bslmf::MovableRefUtil::move(mX));
                        ASSERTV(X == Z);
                        ASSERTV(mR == &X);

                        // Verify that 'X' has the correct allocator.
                        ASSERTV(&oa,   X.allocator().mechanism(),
                                &oa == X.allocator().mechanism());
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(sam.isTotalSame());
                    ASSERTV(oam.isTotalSame());
                }
            }
        }

        if (verbose) printf("\nTesting move assignment with exceptions.\n");
        {
            const int NUM_DATA                     = DEFAULT_NUM_DATA;
            const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int              LINE1 = DATA[ti].d_line;
                const char *const      SPEC1 = DATA[ti].d_spec_p;
                const bsl::string_view RSEP1 = DATA[ti].d_recordSeparator;

                // Create control object 'W'.
                Obj mW;  const Obj& W = mW;
                mW.setFormat(SPEC1);
                mW.setRecordSeparator(RSEP1);

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int              LINE2 = DATA[tj].d_line;
                    const char *const      SPEC2 = DATA[tj].d_spec_p;
                    const bsl::string_view RSEP2 = DATA[tj].d_recordSeparator;

                    for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                        const char CONFIG = cfg;  // how we specify allocator

                        bslma::TestAllocator oa(   "object",
                                                veryVeryVeryVerbose);
                        bslma::TestAllocator za("different",
                                                veryVeryVeryVerbose);

                        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                            if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                            // Create source object 'Y'.
                            bslma::TestAllocator *srcAllocatorPtr = 0;

                            switch (CONFIG) {
                              case 'a': {
                                srcAllocatorPtr = &za;
                              } break;
                              case 'b': {
                                srcAllocatorPtr = &oa;
                              } break;
                              default: {
                                ASSERTV(CONFIG, !"Bad allocator config.");
                              } break;
                            }
                            bslma::TestAllocator& sa = *srcAllocatorPtr;

                            Obj mY(&sa);  const Obj& Y = mY;
                            mY.setFormat(SPEC1);
                            mY.setRecordSeparator(RSEP1);

                            ASSERTV(LINE1, LINE2, CONFIG, Y == W);

                            // Create target object 'X'.
                            Obj mX(&oa);  const Obj& X = mX;
                            mX.setFormat(SPEC2);
                            mX.setRecordSeparator(RSEP2);

                            ASSERTV(LINE1, LINE2, CONFIG,
                                    (X == Y) == (ti == tj));

                            Obj *mR = &(mX = bslmf::MovableRefUtil::move(mY));
                            ASSERTV(LINE1, LINE2, CONFIG, mR == &mX);

                            // Verify the value of the object.
                            ASSERTV(LINE1, LINE2, CONFIG, X == W);
                        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    }
                }
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // MOVE CTOR
        //
        // Concerns:
        //: 1 The newly created object produces the same output (using the
        //:   functional operator) as that of the original object before the
        //:   call.
        //:
        //: 2 All internal representations of a given object can be used to
        //:   create a new object that produces the same output.
        //:
        //: 3 The allocator is propagated to the newly created object if (and
        //:   only if) no allocator is specified in the call to the move
        //:   constructor.
        //:
        //: 4 The original object is always left in a valid state and the
        //:   allocator address held by the original object is unchanged.
        //:
        //: 5 Subsequent changes to or destruction of the original object have
        //:   no effect on the move-constructed object and vice-versa.
        //:
        //: 6 The object has its internal memory management system hooked up
        //:   properly so that *all* internally allocated memory draws from a
        //:   user-supplied allocator whenever one is specified.
        //:
        //: 7 Every object releases any allocated memory at destruction.
        //
        // Plan:
        //: 1 Specify a control 'W' object.  (C-2)
        //:
        //: 2 Call the move constructor to create the object in all relevant
        //:   use cases involving the allocator: 1) no allocator passed in, 2)
        //:   a 0 is explicitly passed in as the allocator argument, 3) the
        //:   same allocator as that of the original object is explicitly
        //:   passed in, and 4) a different allocator than that of the original
        //:   object is passed in.
        //:
        //: 3 For the object value (P-1) and for each configuration (P-2),
        //:   verify the following:
        //:
        //:   1 Verify the newly created object has the same value as that of
        //:     the original object before the call to the move constructor
        //:     (control value).  (C-1)
        //:
        //:   2 Ensure that the new, original, and control object have the
        //:     correct allocator and that all memory allocations come from the
        //:     appropriate allocator.  (C-3,6)
        //:
        //:   3 Destroy the original object (after the move construction) as a
        //:     minimal test that it is in a valid state, then verify that the
        //:     new object is not affected.  (C-4..5)
        //:
        //:   4 Verify all memory is released when the object is destroyed.
        //:     (C-7)
        //
        // Testing:
        //   RecordJsonFormatter(MovableRef<RecordJsonFormatter> o);
        //   RecordJsonFormatter(MovableRef<RecordJsonFormatter> o, alloc);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "MOVE CTOR" << endl
                          << "=========" << endl;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            // Create control object 'W' with a scratch allocator.

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mW(&scratch);  const Obj& W = mW;

            P(W.format());

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {

                const char CONFIG = cfg;  // how we specify the allocator

                if (veryVerbose) { P(CONFIG) }

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator za("different", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                // Create source object 'Y'.
                Obj *pY = new (sa) Obj(&sa);
                Obj& mY = *pY;  const Obj& Y = mY;

                bslma::TestAllocatorMonitor sam(&za), oam(&za);

                Obj                  *objPtr          = 0;
                bslma::TestAllocator *objAllocatorPtr = 0;

                switch (CONFIG) {
                  case 'a': {
                    sam.reset(&sa);  // supplied allocator
                    oam.reset(&za);  // other allocator
                    objPtr = new (fa) Obj(MoveUtil::move(mY));
                    objAllocatorPtr = &sa;
                  } break;
                  case 'b': {
                    sam.reset(&da);
                    oam.reset(&za);
                    objPtr = new (fa) Obj(MoveUtil::move(mY),
                                          (bslma::Allocator *)0);
                    objAllocatorPtr = &da;
                  } break;
                  case 'c': {
                    sam.reset(&sa);
                    oam.reset(&da);
                    objPtr = new (fa) Obj(MoveUtil::move(mY), &sa);
                    objAllocatorPtr = &sa;
                  } break;
                  case 'd': {
                    sam.reset(&za);
                    oam.reset(&da);
                    objPtr = new (fa) Obj(MoveUtil::move(mY), &za);
                    objAllocatorPtr = &za;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } break;
                }

                bslma::TestAllocator& oa = *objAllocatorPtr;

                Obj& mX = *objPtr;  const Obj& X = mX;

                // Verify that 'X' and 'Y' have the correct allocator.
                ASSERTV(CONFIG, &oa,   X.allocator().mechanism(),
                                &oa == X.allocator().mechanism());
                ASSERTV(CONFIG, &sa,   Y.allocator().mechanism(),
                                &sa == Y.allocator().mechanism());

                // Verify no allocation from the non-object allocator and that
                // the object allocator is hooked up.
                switch (CONFIG) {
                  case 'a':
                  case 'c': {
                    ASSERTV(CONFIG, sam.isTotalSame());
                    ASSERTV(CONFIG, oam.isTotalSame());
                  } break;
                  case 'b': {
                    ASSERTV(CONFIG, sam.isTotalUp());
                    ASSERTV(CONFIG, oam.isTotalSame());
                  } break;
                  case 'd': {
                    ASSERTV(CONFIG, sam.isTotalUp());
                    ASSERTV(CONFIG, oam.isInUseSame());
                  } break;
                }

                // Verify the value of the object.
                ASSERTV(CONFIG, X == W);

                // Destroy the source object and verify that the new object is
                // not affected.
                sa.deleteObject(pY);
                ASSERTV(CONFIG, X == W);

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.
                ASSERTV(0 == fa.numBlocksInUse());
                ASSERTV(0 == da.numBlocksInUse());
                ASSERTV(0 == sa.numBlocksInUse());
                ASSERTV(0 == za.numBlocksInUse());
            }
        }

      } break;
      case 6: {
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
        //: 8 Any memory allocation is exception neutral.
        //:
        //: 9 Assigning an object to itself behaves as expected (alias-safety).
        //:
        //:10 Every object releases any allocated memory at destruction.
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
        //:   1 Specify a set of (unique) valid object values (one per row),
        //:     including the default value.
        //:
        //: 4 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3:  (C-1..2, 5..7, 10)
        //:
        //:   1 Use the default constructor, and a "scratch" allocator to
        //:     create two 'const' 'Obj', 'Z' and 'ZZ', each having the value
        //:     'V'.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2):  (C-1..2, 5..7, 10)
        //:
        //:     1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:     2 Use the default constructor, and 'oa' to create a modifiable
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
        //:     7 Use the appropriate test allocators to verify that all object
        //:       memory is released when the object is destroyed.  (C-10)
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
        //:   2 Use the default constructor, and 'oa' to create a modifiable
        //:     'Obj' 'mX'; also use 'setFormat' primary manipulator and a
        //:     distinct "scratch" allocator to create a 'const' 'Obj' 'ZZ'.
        //:
        //:   3 Let 'Z' be a 'const' reference to 'mX'.
        //:
        //:   4 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:     (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:     (C-8)
        //:
        //:   5 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   6 Use the equality-comparison operator to verify that the
        //:     target object, 'Z', still has the same value as that of 'ZZ'.
        //:     (C-9)
        //:
        //:   7 Use the 'allocator' accessor of 'mX' to verify that it is still
        //:     the object allocator.
        //:
        //:   8 Use the appropriate test allocators to verify that:
        //:
        //:     1 Any memory that is allocated is from the object allocator.
        //:
        //:     2 All object memory is released when the object is destroyed.
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   HttpHeader& operator=(const HttpHeader& rhs);
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

        if (verbose) cout << "\nUse a table of distinct object values."
                                                                       << endl;
        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int              LINE1 = DATA[ti].d_line;
            const char *const      SPEC1 = DATA[ti].d_spec_p;
            const bsl::string_view RSEP1 = DATA[ti].d_recordSeparator;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj  mZ(&scratch);  const Obj& Z  = mZ;
            Obj mZZ(&scratch);  const Obj& ZZ = mZZ;

            mZ.setFormat(SPEC1);
            mZ.setRecordSeparator(RSEP1);
            mZZ.setFormat(SPEC1);
            mZZ.setRecordSeparator(RSEP1);

            if (veryVerbose) { T_ P_(LINE1) P_(Z.format()) P(ZZ.format()) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(LINE1, Obj() == Z);
                firstFlag = false;
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int              LINE2 = DATA[tj].d_line;
                const char *const      SPEC2 = DATA[tj].d_spec_p;
                const bsl::string_view RSEP2 = DATA[tj].d_recordSeparator;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(&oa);  const Obj& X = mX;

                    mX.setFormat(SPEC2);
                    mX.setRecordSeparator(RSEP2);

                    if (veryVerbose) { T_ P_(LINE2) P(X.format()) }

                    ASSERTV(LINE1, LINE2, (Z == X) == (LINE1 == LINE2));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        Obj *mR = &(mX = Z);
                        ASSERTV(LINE1, LINE2,  Z == X);
                        ASSERTV(LINE1, LINE2, mR == &mX);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(LINE1, LINE2, ZZ == Z);

                    ASSERTV(LINE1, LINE2, &oa,   X.allocator().mechanism(),
                                          &oa == X.allocator().mechanism());
                    ASSERTV(LINE1, LINE2, &scratch,   Z.allocator().mechanism(),
                                          &scratch == Z.allocator().mechanism());

                    ASSERTV(LINE1, LINE2, sam.isInUseSame());

                    ASSERTV(LINE1, LINE2, 0 == da.numBlocksInUse());
                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINE1, LINE2, oa.numBlocksInUse(),
                                                     0 == oa.numBlocksInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj  mX(&oa);
                Obj mZZ(&scratch);  const Obj& ZZ = mZZ;

                mX.setFormat(SPEC1);
                mX.setRecordSeparator(RSEP1);
                mZZ.setFormat(SPEC1);
                mZZ.setRecordSeparator(RSEP1);

                const Obj& Z = mX;

                ASSERTV(LINE1, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj *mR = &(mX = Z);
                    ASSERTV(LINE1, ZZ == Z);
                    ASSERTV(LINE1, mR == &mX);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                      ASSERTV(LINE1, &oa,   Z.allocator().mechanism(),
                                     &oa == Z.allocator().mechanism());

                ASSERTV(LINE1, !oam.isInUseUp());

                ASSERTV(LINE1, 0 == da.numBlocksInUse());
            }

            // Verify all object memory is released on destruction.

            ASSERTV(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
        }
      } break;
      case 5: {
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
        //: 7 Every object releases any allocated memory at destruction.
        //:
        //: 8 The original object is passed as a 'const' reference.
        //:
        //: 9 The value of the original object is unchanged.
        //:
        //:10 The allocator address held by the original object is unchanged.
        //:
        //:12 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row),
        //:     including the default value.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1..11)
        //:
        //:   1 Use the default constructor, and a "scratch" allocator to
        //:     create two 'const' 'Obj', 'Z' and 'ZZ', each having the value
        //:     'V'.
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
        //:       a distinct test allocator for the object's footprint.  (C-8)
        //:
        //:     3 Use the equality-comparison operator to verify that:
        //:       (C-1, 5, 9)
        //:
        //:       1 The newly constructed object, 'X', has the same value as
        //:         that of 'Z'.  (C-1, 5)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-9)
        //:
        //:     4 Use the 'allocator' accessor of 'X' to verify that its object
        //:       allocator is properly installed, and use the 'allocator'
        //:       accessor of 'Z' to verify that the allocator address that it
        //:       holds is unchanged.  (C-6, 10)
        //:
        //:     5 Use the appropriate test allocators to verify that:  (C-2..4,
        //:       8)
        //:
        //:       1 An object allocates a memory from the object allocator only
        //:         (irrespective of the specific number of allocations or the
        //:         total amount of memory allocated).  (C-2, 4)
        //:
        //:       2 If an allocator was supplied at construction (P-2.1c), the
        //:         current default allocator doesn't allocate any memory.
        //:         (C-3)
        //:
        //:       3 All object memory is released when the object is destroyed.
        //:         (C-7)
        //:
        //: 3 Test again, using the data of P-1, but this time just for the
        //:   supplied allocator configuration (P-2.2c), and create the object
        //:   as an automatic variable in the presence of injected exceptions
        //:   (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:   (C-12)
        //
        // Testing:
        //   RecordJsonFormatter(const RecordJsonFormatter& o, a = alloc());
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        if (verbose) cout << "\nUse a table of distinct object values."
                                                                       << endl;
        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int              LINE = DATA[ti].d_line;
                const char *const      SPEC = DATA[ti].d_spec_p;
                const bsl::string_view RSEP = DATA[ti].d_recordSeparator;

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                Obj  mZ(&scratch);  const Obj& Z  = mZ;
                Obj mZZ(&scratch);  const Obj& ZZ = mZZ;

                mZ.setFormat(SPEC);
                mZ.setRecordSeparator(RSEP);
                mZZ.setFormat(SPEC);
                mZZ.setRecordSeparator(RSEP);

                if (veryVerbose) {
                    bsl::ostringstream oss(&scratch);
                    T_ P(Z.format())
                    T_ P(ZZ.format())
                }

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    bslma::TestAllocatorMonitor dam(&da);

                    Obj                  *objPtr          = 0;
                    bslma::TestAllocator *objAllocatorPtr = 0;

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
                        ASSERTV(CONFIG, !"Bad allocator config.");
                      } break;
                    }
                    ASSERTV(LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) {
                        T_ T_ P_(CONFIG) P(X.format())
                    }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;

                    // Ensure the first row of the table contains the
                    // default-constructed value.

                    static bool firstFlag = true;
                    if (!firstFlag) {
                        ASSERTV(LINE, CONFIG, Obj() == *objPtr);
                        firstFlag = false;
                    }

                    // Verify the value of the object.

                    ASSERTV(LINE, CONFIG,  Z == X);

                    // Verify that the value of 'Z' has not changed.

                    ASSERTV(LINE, CONFIG, ZZ == Z);

                    // Invoke the object's 'allocator' accessor, as well as
                    // that of 'Z'.

                    ASSERTV(LINE, CONFIG, &oa,   X.allocator().mechanism(),
                                          &oa == X.allocator().mechanism());

                    ASSERTV(LINE, CONFIG, &scratch, Z.allocator().mechanism(),
                                        &scratch == Z.allocator().mechanism());

                    // Verify no allocation from the non-object allocator.

                    ASSERTV(LINE, CONFIG, 'c' != CONFIG || dam.isInUseSame());

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                                                     0 == da.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                                                     0 == fa.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, sa.numBlocksInUse(),
                                                     0 == sa.numBlocksInUse());
                }  // end foreach configuration

            }  // end foreach row
        }

        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int              LINE = DATA[ti].d_line;
                const char *const      SPEC = DATA[ti].d_spec_p;
                const bsl::string_view RSEP = DATA[ti].d_recordSeparator;

                if (veryVerbose) { T_ P(SPEC) }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                Obj  mZ(&scratch);  const Obj& Z  = mZ;
                Obj mZZ(&scratch);  const Obj& ZZ = mZZ;

                mZ.setFormat(SPEC);
                mZ.setRecordSeparator(RSEP);
                mZZ.setFormat(SPEC);
                mZZ.setRecordSeparator(RSEP);

                bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj obj(Z, &sa);
                    ASSERTV(LINE, Z == obj);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE, ZZ == Z);

                ASSERTV(LINE, &scratch,   Z.allocator().mechanism(),
                              &scratch == Z.allocator().mechanism());
                ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
                ASSERTV(LINE, sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING OPERATOR()
        //
        // Concerns:
        //: 1  'operator ()' should print out the given 'record' in the format
        //:    defined by 'd_format', followed by 'd_recordSeparator'.
        //:
        //: 2 No memory is allocated from the default or global allocators.
        //
        // Test plan:
        //   Test every pattern to see if the output is the same as expected.
        //
        // Testing:
        //   void operator()(bsl::ostream&, const ball::Record&) const;
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTIMESTAMP TEST"
                               << "\n==============" << bsl::endl;

        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",     veryVeryVeryVerbose);

            static const struct {
                int                 d_line;
                const char         *d_spec;
                bsls::Types::Int64  d_offset;
                const char         *d_expected;
            } DATA[] = {
//-----------------------------------------------------------------------------
{ L_,                                                                 // LINE
  "[\"timestamp\"]",                                                  // SPEC
  0,                                                                  // OFFSET
  "{\"timestamp\":\"2021-01-02T03:04:05.006Z\"}"                      // EXPECT
},
{ L_,                                                                 // LINE
  "[{\"timestamp\":{}}]",                                             // SPEC
  0,                                                                  // OFFSET
  "{\"timestamp\":\"2021-01-02T03:04:05.006Z\"}"                      // EXPECT
},
//-------------------------------- name ---------------------------------------
{ L_,                                                                 // LINE
  "[{\"timestamp\":{\"name\":\"My Time\"}}]",                         // SPEC
  0,                                                                  // OFFSET
  "{\"My Time\":\"2021-01-02T03:04:05.006Z\"}"                        // EXPECT
},

//------------------------------- format --------------------------------------
{ L_,                                                                 // LINE
  "[{\"timestamp\":{\"format\":\"iso8601\"}}]",                       // SPEC
  0,                                                                  // OFFSET
  "{\"timestamp\":\"2021-01-02T03:04:05.006Z\"}"                      // EXPECT
},
{ L_,                                                                 // LINE
  "[{\"timestamp\":{\"format\":\"bdePrint\"}}]",                      // SPEC
  0,                                                                  // OFFSET
  "{\"timestamp\":\"02JAN2021_03:04:05.006\"}"                        // EXPECT
},

//----------------------- fractionalSecPrecision ------------------------------
{ L_,                                                                 // LINE
  "[{\"timestamp\":{\"fractionalSecPrecision\":\"none\"}}]",          // SPEC
  0,                                                                  // OFFSET
  "{\"timestamp\":\"2021-01-02T03:04:05Z\"}"                          // EXPECT
},
{ L_,                                                                 // LINE
  "[{\"timestamp\":{\"fractionalSecPrecision\":\"milliseconds\"}}]",  // SPEC
  0,                                                                  // OFFSET
  "{\"timestamp\":\"2021-01-02T03:04:05.006Z\"}"                      // EXPECT
},
{ L_,                                                                 // LINE
  "[{\"timestamp\":{\"fractionalSecPrecision\":\"microseconds\"}}]",  // SPEC
  0,                                                                  // OFFSET
  "{\"timestamp\":\"2021-01-02T03:04:05.006007Z\"}"                   // EXPECT
},
//------------------------------- timeZone ------------------------------------
{ L_,                                                                 // LINE
  "[{\"timestamp\":{\"timeZone\":\"utc\"}}]",                         // SPEC
  0,                                                                  // OFFSET
  "{\"timestamp\":\"2021-01-02T03:04:05.006Z\"}"                      // EXPECT
},
{ L_,                                                                 // LINE
  "[{\"timestamp\":{\"timeZone\":\"local\"}}]",                       // SPEC
  0,                                                                  // OFFSET
  "{\"timestamp\":\"2021-01-02T03:04:05.006Z\"}"                      // EXPECT
},
{ L_,                                                                 // LINE
  "[{\"timestamp\":{\"timeZone\":\"local\"}}]",                       // SPEC
  3600,                                                               // OFFSET
  "{\"timestamp\":\"2021-01-02T04:04:05.006+01:00\"}"                 // EXPECT
},
{ L_,                                                                 // LINE
  "[{\"timestamp\":{\"timeZone\":\"local\"}}]",                       // SPEC
  -3600,                                                              // OFFSET
  "{\"timestamp\":\"2021-01-02T02:04:05.006-01:00\"}"                 // EXPECT
},
            };

            bdlt::LocalTimeOffset::LocalTimeOffsetCallback defaultCallback =
                             bdlt::LocalTimeOffset::setLocalTimeOffsetCallback(
                                        &LocalTimeOffsetUtil::localTimeOffset);

            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < NUM_DATA; ++i) {

                const int           LINE     = DATA[i].d_line;
                bsl::string_view    SPEC     = DATA[i].d_spec;
                bsls::Types::Int64  OFFSET   = DATA[i].d_offset;
                const char         *EXPECTED = DATA[i].d_expected;

                bslma::TestAllocatorMonitor dam(&defaultAllocator);

                Obj mX(&sa); const Obj& X = mX;

                int rc = mX.setFormat(SPEC);
                ASSERTV(rc, 0 == rc);
                mX.setRecordSeparator("");

                bsl::ostringstream oss(&scratch);
                RA                 fields(&scratch);

                fields.setTimestamp(bdlt::Datetime(2021, 1, 2, 3, 4, 5, 6, 7));
                LocalTimeOffsetUtil::d_offset = OFFSET;

                X(oss, Rec(fields, UF(&scratch)));

                if (veryVeryVerbose) P(oss.str().c_str());

                ASSERTV(LINE, oss.str().c_str(), EXPECTED,
                              oss.str() ==       EXPECTED);
                ASSERTV(dam.isInUseSame());
            }
            // Verify all memory is released.
            ASSERTV(0 == sa.numBlocksInUse());
            ASSERTV(0 == scratch.numBlocksInUse());

            bdlt::LocalTimeOffset::setLocalTimeOffsetCallback(defaultCallback);
        }

        if (verbose) bsl::cout << "\nPROCESS ID TEST"
                               << "\n==============" << bsl::endl;

        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",     veryVeryVeryVerbose);

            static const struct {
                int         d_line;
                const char *d_spec;
                const int   d_pid;
                const char *d_expected;
            } DATA[] = {
                //-------------------------------------------------------------
                // LINE SPEC                       PID  EXPECTED
                {  L_,  "[\"pid\"]",               18,  "{\"pid\":18}"    },
                {  L_,  "[{\"pid\":{}}]",          64,  "{\"pid\":64}"    },
                {  L_,  "[{\"pid\":"
                        "{\"name\":\"My pid\"}}]", 1,   "{\"My pid\":1}"  }
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE     = DATA[i].d_line;
                const char *SPEC     = DATA[i].d_spec;
                const int   PID      = DATA[i].d_pid;
                const char *EXPECTED = DATA[i].d_expected;

                Obj mX(&sa); const Obj& X = mX;

                bslma::TestAllocatorMonitor dam(&defaultAllocator);

                int rc = mX.setFormat(SPEC);
                ASSERTV(rc, 0 == rc);
                mX.setRecordSeparator("");

                bsl::ostringstream oss(&scratch);
                RA                 fields(&scratch);

                fields.setProcessID(PID);

                X(oss, Rec(fields, UF(&scratch)));

                if (veryVeryVerbose) P(oss.str().c_str());

                ASSERTV(LINE, oss.str().c_str(), EXPECTED,
                              oss.str() ==       EXPECTED);

                ASSERTV(dam.isInUseSame());
            }
            // Verify all memory is released.
            ASSERTV(0 == sa.numBlocksInUse());
            ASSERTV(0 == scratch.numBlocksInUse());
        }

        if (verbose) bsl::cout << "\nTHREAD ID TEST"
                               << "\n==============" << bsl::endl;

        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",     veryVeryVeryVerbose);

            static const struct {
                int         d_line;
                const char *d_spec;
                const int   d_tid;
                const char *d_expected;
            } DATA[] = {
              //---------------------------------------------------------------
              // LINE SPEC                          PID  EXPECTED
              {  L_,  "[\"tid\"]",                  42,  "{\"tid\":42}"     },
              {  L_,  "[{\"tid\":{}}]",             34,  "{\"tid\":34}"     },
              {  L_,  "[{\"tid\":"
                      "{\"name\":\"My tid\"}}]",    88,  "{\"My tid\":88}"  },
              {  L_,  "[{\"tid\":"
                      "{\"format\":\"decimal\"}}]", 42,  "{\"tid\":42}"     },
              {  L_,  "[{\"tid\":"
                      "{\"format\":\"hex\"}}]",     42,  "{\"tid\":\"2A\"}" }
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < NUM_DATA; ++i) {

                const int         LINE     = DATA[i].d_line;
                bsl::string_view  SPEC     = DATA[i].d_spec;
                const int         TID      = DATA[i].d_tid;
                const char       *EXPECTED = DATA[i].d_expected;

                bslma::TestAllocatorMonitor dam(&defaultAllocator);

                Obj mX(&sa); const Obj& X = mX;

                int rc = mX.setFormat(SPEC);
                ASSERTV(rc, 0 == rc);
                mX.setRecordSeparator("");

                bsl::ostringstream oss(&scratch);
                RA                 fields(&scratch);

                fields.setThreadID(TID);

                X(oss, Rec(fields, UF(&scratch)));

                if (veryVeryVerbose) P(oss.str().c_str());

                ASSERTV(LINE, oss.str().c_str(), EXPECTED,
                              oss.str() ==       EXPECTED);

                ASSERTV(dam.isInUseSame());
            }
            // Verify all memory is released.
            ASSERTV(0 == sa.numBlocksInUse());
            ASSERTV(0 == scratch.numBlocksInUse());
        }

        if (verbose) bsl::cout << "\nFILE TEST"
                               << "\n=========" << bsl::endl;

        {
#ifdef BSLS_PLATFORM_OS_WINDOWS
#define PS "\\"
#else
#define PS "/"
#endif
            const char *FILE_NAME = PS "bar" PS "foo.c";

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",     veryVeryVeryVerbose);

            static const struct {
                int         d_line;
                const char *d_spec;
                const char *d_file;
                const char *d_expected;
            } DATA[] = {
    //-------------------------------------------------------------------------
    // LINE  SPEC                        FILE       EXPECTED
    {  L_,  "[\"file\"]",                FILE_NAME, "{\"file\":\"\\"
                                                    PS "bar\\" PS "foo.c\"}" },
    {  L_,  "[{\"file\":{}}]",           FILE_NAME, "{\"file\":\"\\"
                                                    PS "bar\\" PS "foo.c\"}" },
    {  L_,  "[{\"file\":"
             "{\"name\":\"My file\"}}]", FILE_NAME, "{\"My file\":\"\\"
                                                    PS "bar\\" PS "foo.c\"}" },
    {  L_,  "[{\"file\":"
             "{\"path\":\"full\"}}]",    FILE_NAME, "{\"file\":\"\\"
                                                    PS "bar\\" PS "foo.c\"}" },
    {  L_,  "[{\"file\":"
             "{\"path\":\"file\"}}]",    FILE_NAME, "{\"file\":\"foo.c\"}"   },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE     = DATA[i].d_line;
                const char *SPEC     = DATA[i].d_spec;
                const char *FILE     = DATA[i].d_file;
                const char *EXPECTED = DATA[i].d_expected;

                bslma::TestAllocatorMonitor dam(&defaultAllocator);

                Obj mX(&sa); const Obj& X = mX;

                int rc = mX.setFormat(SPEC);
                ASSERTV(rc, 0 == rc);
                mX.setRecordSeparator("");

                bsl::ostringstream oss(&scratch);
                RA                 fields(&scratch);

                fields.setFileName(FILE);

                X(oss, Rec(fields, UF(&scratch)));

                if (veryVeryVerbose) P(oss.str().c_str());

                ASSERTV(LINE, oss.str().c_str(), EXPECTED,
                              oss.str() ==       EXPECTED);

                ASSERTV(dam.isInUseSame());
            }

            // Verify all memory is released.
            ASSERTV(0 == sa.numBlocksInUse());
            ASSERTV(0 == scratch.numBlocksInUse());
#undef PS
        }

        if (verbose) bsl::cout << "\nLINE TEST"
                               << "\n=========" << bsl::endl;

        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",     veryVeryVeryVerbose);

            static const struct {
                int         d_line;
                const char *d_spec;
                const int   d_lineNumber;
                const char *d_expected;
            } DATA[] = {
              //---------------------------------------------------------------
              // LINE SPEC                        LINE  EXPECTED
              {  L_,  "[\"line\"]",               42,   "{\"line\":42}"      },
              {  L_,  "[{\"line\":{}}]",          33,   "{\"line\":33}"      },
              {  L_,  "[{\"line\":"
                      "{\"name\":\"My line\"}}]", 1234, "{\"My line\":1234}" },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE        = DATA[i].d_line;
                const char *SPEC        = DATA[i].d_spec;
                const int   LINE_NUMBER = DATA[i].d_lineNumber;
                const char *EXPECTED    = DATA[i].d_expected;

                bslma::TestAllocatorMonitor dam(&defaultAllocator);

                Obj mX(&sa); const Obj& X = mX;

                int rc = mX.setFormat(SPEC);
                ASSERTV(rc, 0 == rc);
                mX.setRecordSeparator("");

                bsl::ostringstream oss(&scratch);
                RA                 fields(&scratch);

                fields.setLineNumber(LINE_NUMBER);

                X(oss, Rec(fields, UF(&scratch)));

                if (veryVeryVerbose) P(oss.str().c_str());

                ASSERTV(LINE, oss.str().c_str(), EXPECTED,
                              oss.str() ==       EXPECTED);

                ASSERTV(dam.isInUseSame());
            }

            // Verify all memory is released.
            ASSERTV(0 == sa.numBlocksInUse());
            ASSERTV(0 == scratch.numBlocksInUse());
        }

        if (verbose) bsl::cout << "\nCATEGORY TEST"
                               << "\n=============" << bsl::endl;

        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",     veryVeryVeryVerbose);

            static const struct {
                int         d_line;
                const char *d_spec;
                const char *d_category;
                const char *d_expected;
            } DATA[] = {
  //---------------------------------------------------------------------------
  // LINE SPEC                    CATEGORY     EXPECTED
  {  L_,  "[\"category\"]",       "Category",  "{\"category\":\"Category\"}" },
  {  L_,  "[{\"category\":{}}]",  "Category",  "{\"category\":\"Category\"}" },
  {  L_,  "[{\"category\":"
           "{\"name\":"
            "\"My Category\"}}]", "Category",  "{\"My Category\":"
                                                "\"Category\"}"  },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE     = DATA[i].d_line;
                const char *SPEC     = DATA[i].d_spec;
                const char *CATEGORY = DATA[i].d_category;
                const char *EXPECTED = DATA[i].d_expected;

                bslma::TestAllocatorMonitor dam(&defaultAllocator);

                Obj mX(&sa); const Obj& X = mX;

                int rc = mX.setFormat(SPEC);
                ASSERTV(rc, 0 == rc);
                mX.setRecordSeparator("");

                bsl::ostringstream oss(&scratch);
                RA                 fields(&scratch);

                fields.setCategory(CATEGORY);

                X(oss, Rec(fields, UF(&scratch)));

                if (veryVeryVerbose) P(oss.str().c_str());

                ASSERTV(LINE, oss.str().c_str(), EXPECTED,
                              oss.str() ==       EXPECTED);

                ASSERTV(dam.isInUseSame());
            }

            // Verify all memory is released.
            ASSERTV(0 == sa.numBlocksInUse());
            ASSERTV(0 == scratch.numBlocksInUse());
        }

        if (verbose) bsl::cout << "\nSEVERITY TEST"
                               << "\n=============" << bsl::endl;

        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",     veryVeryVeryVerbose);

            typedef ball::Severity S;

            static const struct {
                int         d_line;
                const char *d_spec;
                int         d_severity;
                const char *d_expected;
            } DATA[] = {
  //---------------------------------------------------------------------------
  // LINE SPEC                      SEVERITY    EXPECTED
  {  L_,  "[\"severity\"]",         S::e_OFF,   "{\"severity\":\"OFF\"}"     },
  {  L_,  "[\"severity\"]",         S::e_FATAL, "{\"severity\":\"FATAL\"}"   },
  {  L_,  "[\"severity\"]",         S::e_ERROR, "{\"severity\":\"ERROR\"}"   },
  {  L_,  "[\"severity\"]",         S::e_WARN,  "{\"severity\":\"WARN\"}"    },
  {  L_,  "[\"severity\"]",         S::e_INFO,  "{\"severity\":\"INFO\"}"    },
  {  L_,  "[\"severity\"]",         S::e_DEBUG, "{\"severity\":\"DEBUG\"}"   },
  {  L_,  "[\"severity\"]",         S::e_TRACE, "{\"severity\":\"TRACE\"}"   },
  {  L_,  "[\"severity\"]",         42,         "{\"severity\":"
                                                 "\"(* UNKNOWN *)\"}"        },
  {  L_,  "[{\"severity\":{}}]",    S::e_INFO,  "{\"severity\":\"INFO\"}"    },
  {  L_,  "[{\"severity\":"
           "{\"name\":"
            "\"My Severity\"}}]",   S::e_INFO,  "{\"My Severity\":\"INFO\"}" },
            };

            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE     = DATA[i].d_line;
                const char *SPEC     = DATA[i].d_spec;
                const int   SEVERITY = DATA[i].d_severity;
                const char *EXPECTED = DATA[i].d_expected;

                bslma::TestAllocatorMonitor dam(&defaultAllocator);

                Obj mX(&sa); const Obj& X = mX;

                int rc = mX.setFormat(SPEC);
                ASSERTV(rc, 0 == rc);
                mX.setRecordSeparator("");

                bsl::ostringstream oss(&scratch);
                RA                 fields(&scratch);

                fields.setSeverity(SEVERITY);

                X(oss, Rec(fields, UF(&scratch)));

                if (veryVeryVerbose) P(oss.str().c_str());

                ASSERTV(LINE, oss.str().c_str(), EXPECTED,
                              oss.str() ==       EXPECTED);

                ASSERTV(dam.isInUseSame());
            }

            // Verify all memory is released.
            ASSERTV(0 == sa.numBlocksInUse());
            ASSERTV(0 == scratch.numBlocksInUse());
        }

        if (verbose) bsl::cout << "\nMESSAGE TEST"
                               << "\n=============" << bsl::endl;

        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",     veryVeryVeryVerbose);

            static const struct {
                int         d_line;
                const char *d_spec;
                const char *d_message;
                const char *d_expected;
            } DATA[] = {
    //-------------------------------------------------------------------------
    // LINE SPEC                        MESSAGE         EXPECTED
    //-------------------------------------------------------------------------
    {  L_,  "[\"message\"]",            "",             "{\"message\":"
                                                        "\"\"}" },
    {  L_,  "[\"message\"]",            "Hello, world", "{\"message\":"
                                                        "\"Hello, world\"}" },
    {  L_,  "[{\"message\":{}}]",       "Hello, world", "{\"message\":"
                                                        "\"Hello, world\"}" },
    {  L_,  "[{\"message\":"
             "{\"name\":\"My msg\"}}]", "Hello, world", "{\"My msg\":"
                                                        "\"Hello, world\"}" },
            };

            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE     = DATA[i].d_line;
                const char *SPEC     = DATA[i].d_spec;
                const char *MESSAGE  = DATA[i].d_message;
                const char *EXPECTED = DATA[i].d_expected;

                bslma::TestAllocatorMonitor dam(&defaultAllocator);

                Obj mX(&sa); const Obj& X = mX;

                int rc = mX.setFormat(SPEC);
                ASSERTV(rc, 0 == rc);
                mX.setRecordSeparator("");

                bsl::ostringstream oss(&scratch);
                RA                 fields(&scratch);

                fields.setMessage(MESSAGE);

                X(oss, Rec(fields, UF(&scratch)));

                if (veryVeryVerbose) P(oss.str().c_str());

                ASSERTV(LINE, oss.str().c_str(), EXPECTED,
                              oss.str() ==       EXPECTED);

                ASSERTV(dam.isInUseSame());
            }

            // Verify all memory is released.
            ASSERTV(0 == sa.numBlocksInUse());
            ASSERTV(0 == scratch.numBlocksInUse());
        }

        if (verbose) bsl::cout << "\nUSER ATTRIBUTE TEST"
                               << "\n===================" << bsl::endl;

        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",     veryVeryVeryVerbose);

            static const struct {
                int         d_line;
                const char *d_spec;
                const char *d_expected;
            } DATA[] = {
                //-------------------------------------------------------------
                // LINE SPEC                       EXPECTED
                {  L_,  "[\"attributes\"]",        "{\"a1\":\"v1\","
                                                    "\"a2\":\"v2\","
                                                    "\"a3\":\"v3\"}" },
                {  L_,  "[\"attributes\","
                         "\"attributes\"]",        "{\"a1\":\"v1\","
                                                    "\"a2\":\"v2\","
                                                    "\"a3\":\"v3\","
                                                    "\"a1\":\"v1\","
                                                    "\"a2\":\"v2\","
                                                    "\"a3\":\"v3\"}" },
                {  L_,  "[\"a1\"]",                "{\"a1\":\"v1\"}" },
                {  L_,  "[\"a1\",\"a1\"]",         "{\"a1\":\"v1\","
                                                    "\"a1\":\"v1\"}" },
                {  L_,  "[\"a1\",\"a2\"]",         "{\"a1\":\"v1\","
                                                    "\"a2\":\"v2\"}" },
                {  L_,  "[\"a3\",\"a2\",\"a1\"]",  "{\"a3\":\"v3\","
                                                    "\"a2\":\"v2\","
                                                    "\"a1\":\"v1\"}" },
                {  L_,  "[\"a4\"]",                "{\"a4\":\"N\\/A\"}" },
                {  L_,  "[\"attributes\",\"a1\"]", "{\"a2\":\"v2\","
                                                    "\"a3\":\"v3\","
                                                    "\"a1\":\"v1\"}" },
                {  L_,  "[\"a1\",\"attributes\"]", "{\"a1\":\"v1\","
                                                    "\"a2\":\"v2\","
                                                    "\"a3\":\"v3\"}" },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE     = DATA[i].d_line;
                const char *SPEC     = DATA[i].d_spec;
                const char *EXPECTED = DATA[i].d_expected;

                bslma::TestAllocatorMonitor dam(&defaultAllocator);

                Obj mX(&sa); const Obj& X = mX;

                int rc = mX.setFormat(SPEC);
                ASSERTV(rc, 0 == rc);
                mX.setRecordSeparator("");

                bsl::ostringstream oss(&scratch);
                RA                 ra(&scratch);
                UF                 uf(&scratch);
                Rec                rec(ra, uf, &scratch);

                rec.addAttribute(ball::Attribute("a1", "v1", &scratch));
                rec.addAttribute(ball::Attribute("a2", "v2", &scratch));
                rec.addAttribute(ball::Attribute("a3", "v3", &scratch));

                X(oss, rec);

                if (veryVeryVerbose) P(oss.str().c_str());

                ASSERTV(LINE, oss.str().c_str(), EXPECTED,
                              oss.str() ==       EXPECTED);

                ASSERTV(dam.isInUseSame());
            }

            // Verify all memory is released.
            ASSERTV(0 == sa.numBlocksInUse());
            ASSERTV(0 == scratch.numBlocksInUse());
        }
        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",     veryVeryVeryVerbose);

            static const struct {
                int         d_line;
                const char *d_spec;
                const char *d_expected;
            } DATA[] = {
                //-------------------------------------------------------------
                // LINE SPEC                   EXPECTED
                {  L_,  "[\"string\"]",        "{\"string\":\"string\"}" },
                {  L_,  "[\"int\"]",           "{\"int\":-42}"           },
                {  L_,  "[\"llong\"]",         "{\"llong\":-4242}"       },
                {  L_,  "[\"ullong\"]",        "{\"ullong\":42}"         },
                {  L_,  "[\"void_ptr\"]",      "{\"void_ptr\":\"0x2a\"}" },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < NUM_DATA; ++i) {

                const int   LINE     = DATA[i].d_line;
                const char *SPEC     = DATA[i].d_spec;
                const char *EXPECTED = DATA[i].d_expected;

                bslma::TestAllocatorMonitor dam(&defaultAllocator);

                Obj mX(&sa); const Obj& X = mX;

                int rc = mX.setFormat(SPEC);
                ASSERTV(rc, 0 == rc);
                mX.setRecordSeparator("");

                bsl::ostringstream  oss(&scratch);
                RA                  ra(&scratch);
                UF                  uf(&scratch);
                Rec                 rec(ra, uf, &scratch);
                int                *value_p = (int*)42;

                rec.addAttribute(
                          ball::Attribute("string",   "string",     &scratch));
                rec.addAttribute(
                          ball::Attribute("int",      -42,          &scratch));
                rec.addAttribute(
                          ball::Attribute("llong",    -4242LL, &scratch));
                rec.addAttribute(
                          ball::Attribute("ullong",   42ULL,   &scratch));
                rec.addAttribute(
                          ball::Attribute("void_ptr", value_p,      &scratch));

                X(oss, rec);

                if (veryVeryVerbose) P(oss.str().c_str());

                ASSERTV(LINE, oss.str().c_str(), EXPECTED,
                              oss.str() ==       EXPECTED);

                ASSERTV(dam.isInUseSame());
            }

            // Verify all memory is released.
            ASSERTV(0 == sa.numBlocksInUse());
            ASSERTV(0 == scratch.numBlocksInUse());
        }

        if (verbose) bsl::cout << "\nRECORD SEPARATOR TEST"
                               << "\n=====================" << bsl::endl;

        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",     veryVeryVeryVerbose);

            const char *const SPEC = "[\"message\"]";
            const bsl::string k_NULL_S(1, '\0');
            const struct {
                int         d_line;
                bsl::string d_recordSeparator;
                bsl::string d_expected;
            } DATA[] = {
                //-------------------------------------------------------------
                // LINE  SEP     EXPECTED
                //-------------------------------------------------------------
                {  L_,   "\n",   "{\"message\":\"Hello, world\"}\n"          },
                {  L_,   "",     "{\"message\":\"Hello, world\"}"            },
                {  L_,   " ",    "{\"message\":\"Hello, world\"} "           },
                {  L_,   "\r\n", "{\"message\":\"Hello, world\"}\r\n"        },
                {  L_,   k_NULL_S,
                                 "{\"message\":\"Hello, world\"}" + k_NULL_S },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE     = DATA[i].d_line;
                const bsl::string RSEP     = DATA[i].d_recordSeparator;
                const bsl::string EXPECTED = DATA[i].d_expected;

                bslma::TestAllocatorMonitor dam(&defaultAllocator);

                Obj mX(&sa); const Obj& X = mX;

                int rc = mX.setFormat(SPEC);
                ASSERTV(rc, 0 == rc);
                mX.setRecordSeparator(RSEP);

                bsl::ostringstream oss(&scratch);
                RA                 fields(&scratch);

                fields.setMessage("Hello, world");

                X(oss, Rec(fields, UF(&scratch)));

                if (veryVeryVerbose) P(oss.str().c_str());

                ASSERTV(LINE, oss.str(), EXPECTED, oss.str() == EXPECTED);

                ASSERTV(dam.isInUseSame());
            }

            // Verify all memory is released.
            ASSERTV(0 == sa.numBlocksInUse());
            ASSERTV(0 == scratch.numBlocksInUse());
        }
      } break;
      case 3: {
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
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row).
        //:
        //: 2 For each row 'R' in the table of P-1:  (C-1..3)
        //:
        //:   1 Create an object 'X' using the default constructor and a test
        //:     allocator.  Then configure the object to have the value from
        //:     'R'.
        //:
        //:   2 Verify that the 'format', 'recordSeparator', and 'allocator'
        //:     accessors, invoked on a 'const' reference to the object created
        //:     in P-2, return the expected values.
        //:
        //:   3 Monitor the memory allocated from both the default and object
        //:     allocators before and after calling the accessors; verify that
        //:     there is no change in total memory allocation.  (C-3)
        //
        // Testing:
        //   const bsl::string& format() const;
        //   const bsl::string& recordSeparator() const;
        //   const allocator_type& allocator() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "\nUse a table of distinct object values."
                                                                       << endl;
        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 2; ti < NUM_DATA; ++ti) {
            const int              LINE = DATA[ti].d_line;
            const char *const      SPEC = DATA[ti].d_spec_p;
            const bsl::string_view RSEP = DATA[ti].d_recordSeparator;

            if (veryVerbose) { P_(LINE) P_(SPEC) P(RSEP) }

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mX(&oa);  const Obj& X = mX;
            mX.setFormat(SPEC);
            mX.setRecordSeparator(RSEP);

            bslma::TestAllocatorMonitor oam(&oa), dam(&da);

            ASSERTV(LINE, SPEC, SPEC == X.format());
            ASSERTV(LINE, RSEP, RSEP == X.recordSeparator());
            ASSERTV(LINE, SPEC, &oa == X.allocator().mechanism());

            ASSERTV(LINE, SPEC, oam.isTotalSame());
            ASSERTV(LINE, SPEC, dam.isTotalSame());
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
        //: 7 Every object releases any allocated memory at destruction.
        //:
        //: 8 Any memory allocation is exception neutral.
        //:
        //: 9 Each attribute is modifiable independently.
        //
        // Plan:
        //: 1 Create a set of distinct specification values for the format
        //    string and record separator (for use in P-5).
        //:
        //: 2 Using a loop-based approach, default-construct three distinct
        //:   objects, in turn, but configured differently: (a) without passing
        //:   an allocator, (b) passing a null allocator address explicitly,
        //:   and (c) passing the address of a test allocator distinct from the
        //:   default.  For each of these three iterations:  (C-1..8)
        //:
        //:   1 Create three 'bslma::TestAllocator' objects, and install one as
        //:     as the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Use the default constructor to dynamically create an object
        //:     'X', with its object allocator configured appropriately (see
        //:     P-2); use a distinct test allocator for the object's footprint.
        //:
        //:   3 Use the (as yet unproven) 'allocator' accessor of 'X' to verify
        //:     that its object allocator is properly installed.  (C-2..4)
        //:
        //:   4 Use the (as yet unproven) basic accessors to verify the
        //:     default-constructed value.  (C-1)
        //:
        //:   5 In turn, for each value, 'V', in the table of format strings
        //:     described in P-1, supply 'V' to the primary manipulator,
        //:     'setFormat' (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
        //:     macros).  After each invocation, verify that 'X' has the
        //:     expected value and that any memory allocation is as expected.
        //:     Then, repeat this process with the table of record separators
        //:     and the 'setRecordSeparator' primary manipulator.  (C-5..6)
        //:
        //:   6 Corroborate that attributes are modifiable independently by
        //:     first setting all attributes to one set of values. Then
        //:     incrementally set each attribute to a different value and
        //:     verify after each manipulation that only that attribute's value
        //:     changed.  (C-9)
        //:
        //:   7 Verify that all object memory is released when the object is
        //:     destroyed.  (C-7)
        //
        // Testing:
        //   RecordJsonFormatter();
        //   RecordJsonFormatter(const allocator_type& a);
        //   ~RecordJsonFormatter();
        //   int setFormat(const bsl::string_view& format);
        //   void setRecordSeparator(const bsl::string_view& recordSeparator);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                       << "DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR" << endl
                       << "==========================================" << endl;

        static const struct {
            int         d_line;     // source line number
            const char *d_spec_p;
        } FORMAT_DATA[] = {
    //LINE                            SPEC
    //----  -------------------------------------------------------------------
    { L_,   ""                                                               },
    { L_,   "[]"                                                             },
    { L_,   "[\"timestamp\"]"                                                },
    { L_,   "[\"timestamp\",\"pid\"]"                                        },
    { L_,   "[\"timestamp\",\"pid\",\"tid\"]"                                },
    { L_,   "[\"timestamp\",\"pid\",\"tid\",\"severity\"]"                   },
    { L_,   "[\"timestamp\",\"pid\",\"tid\",\"severity\",\"file\"]"          },
    { L_,   "[\"timestamp\",\"pid\",\"tid\",\"severity\",\"file\",\"line\"]" },
    { L_,   "[\"timestamp\",\"pid\",\"tid\",\"severity\",\"file\",\"line\","
             "\"category\"]"                                                 },
    { L_,   "[\"timestamp\",\"pid\",\"tid\",\"severity\",\"file\",\"line\","
              "\"category\",\"message\"]"                                    },
    { L_,   "[\"timestamp\",\"pid\",\"tid\",\"severity\",\"file\",\"line\","
             "\"category\",\"message\",\"attributes\"]"                      },
    { L_,   "[\"timestamp\",\"pid\",\"tid\",\"severity\",\"file\",\"line\","
             "\"category\",\"message\",\"attributes\",\"a1\"]"               },
        };
        enum { NUM_FORMAT_DATA = sizeof FORMAT_DATA / sizeof *FORMAT_DATA };

        static const struct {
            int         d_line;  // source line number
            bsl::string d_recordSeparator;
        } RSEP_DATA[] = {
            //LINE     RECORD SEPARATOR
            //----  ----------------------
            {L_,    "\n"                },
            {L_,    ""                  },
            {L_,    " "                 },
            {L_,    "\r\n"              },
            {L_,    bsl::string(1, '\0')},
        };
        enum { NUM_RSEP_DATA = sizeof RSEP_DATA / sizeof *RSEP_DATA };

        if (verbose) cout << "\nTesting with various allocator configurations."
                          << endl;

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr          = 0;
            bslma::TestAllocator *objAllocatorPtr = 0;
            bslma::TestAllocator *othAllocatorPtr = 0;

            switch (CONFIG) {
              case 'a': {
                objPtr = new (fa) Obj();
                objAllocatorPtr = &da;
                othAllocatorPtr = &sa;
              } break;
              case 'b': {
                objPtr = new (fa) Obj(0);
                objAllocatorPtr = &da;
                othAllocatorPtr = &sa;
              } break;
              case 'c': {
                objPtr = new (fa) Obj(&sa);
                objAllocatorPtr = &sa;
                othAllocatorPtr = &da;
              } break;
              default: {
                ASSERTV(CONFIG, !"Bad allocator config.");
              } break;
            }

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = *othAllocatorPtr;

            // ---------------------------------------
            // Verify allocator is installed properly.
            // ---------------------------------------

            ASSERTV(CONFIG, &oa,   X.allocator().mechanism(),
                            &oa == X.allocator().mechanism());

            // Verify no allocation from the object/non-object allocators.

            ASSERTV(CONFIG,  oa.numBlocksTotal(), 0 < oa.numBlocksTotal());
            ASSERTV(CONFIG, noa.numBlocksTotal(),
                    CONFIG == 'c' || 0 == noa.numBlocksTotal());

            // --------------------------------------------
            // Verify object has default-constructed value.
            // --------------------------------------------

            ASSERTV(CONFIG, X.format(), k_DEFAULT_FORMAT == X.format());
            ASSERTV(CONFIG, X.recordSeparator(),
                    k_DEFAULT_RECORD_SEPARATOR == X.recordSeparator());

            // ----------------------------
            // Verify primary manipulators.
            // ----------------------------

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            for (int ti = 0; ti < NUM_FORMAT_DATA; ++ti) {
                const int         LINE  = FORMAT_DATA[ti].d_line;
                const char *const SPEC  = FORMAT_DATA[ti].d_spec_p;

                bslma::TestAllocatorMonitor oam(&oa);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    mX.setFormat(SPEC);

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                // Verify object value and expected memory use.

                ASSERTV(CONFIG, LINE, SPEC, SPEC[0] == 0 ||
                                            SPEC == X.format());
                ASSERTV(CONFIG, LINE, SPEC,
                        k_DEFAULT_RECORD_SEPARATOR == X.recordSeparator());
                ASSERTV(CONFIG, LINE, CONFIG == 'c' ||
                                      0 == noa.numBlocksTotal());
            }

            const bsl::string lastFormat(X.format(), &scratch);

            for (int ti = 0; ti < NUM_RSEP_DATA; ++ti) {
                const int   LINE  = RSEP_DATA[ti].d_line;
                bsl::string RSEP  = RSEP_DATA[ti].d_recordSeparator;

                bslma::TestAllocatorMonitor oam(&oa);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    mX.setRecordSeparator(RSEP);

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                // Verify object value and expected memory use.

                ASSERTV(CONFIG, LINE, RSEP, RSEP == X.recordSeparator());
                ASSERTV(CONFIG, LINE, lastFormat, lastFormat == X.format());
                ASSERTV(CONFIG, LINE, CONFIG == 'c' ||
                                      0 == noa.numBlocksTotal());
            }

            // Corroborate attribute independence.
            {
                const char *const F1  = "[]";
                const char *const F2  = "[\"timestamp\"]";
                const char *const RS1 = "\n";
                const char *const RS2 = "\r\n";

                mX.setFormat(F1);
                mX.setRecordSeparator(RS1);

                ASSERTV(F1,  F1 == X.format());
                ASSERTV(RS1, RS1 == X.recordSeparator());

                mX.setRecordSeparator(RS2);

                ASSERTV(F1,  F1 == X.format());
                ASSERTV(RS2, RS2 == X.recordSeparator());

                mX.setFormat(F2);

                ASSERTV(F2,  F2 == X.format());
                ASSERTV(RS2, RS2 == X.recordSeparator());
            }

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            ASSERTV(CONFIG, fa.numBlocksInUse(),  0 ==  fa.numBlocksInUse());
            ASSERTV(CONFIG, oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());
            ASSERTV(CONFIG, noa.numBlocksInUse(), 0 == noa.numBlocksInUse());

            // Double check that some object memory was allocated.

            ASSERTV(CONFIG, 1 <= oa.numBlocksTotal());

            // Note that memory should be independently allocated for each
            // attribute capable of allocating memory.
        }
      } break;
      case 1: {
          if (verbose) bsl::cout << "\nBREATHING TEST"
                                 << "\n==============" << bsl::endl;

          Obj mX;
          int rc = 0;
          rc = mX.setFormat("["
                       "\"timestamp\", {\"timestamp\":{\"name\":\"Time\"}},"
                       ",\"pid\", {\"pid\":{\"name\":\"PID\"}}"
                       ",\"line\", {\"line\":{\"name\":\"ln\"}}"
                       ",\"category\", {\"category\":{\"name\":\"cat\"}}"
                       ",\"message\", {\"message\":{\"name\":\"msg\"}}"
                       ",\"attributes\""
                       "]");

          ASSERTV(rc, 0 == rc);

        bsl::ostringstream oss;

        const int   lineNum   = 542;
        const char *filename  = "subdir/process.cpp";
        const bsls::Types::Uint64
                               threadID  = bslmt::ThreadUtil::selfIdAsUint64();
#ifdef BSLS_PLATFORM_OS_UNIX
        const pid_t processID = getpid();
#else
        const int   processID = 0;
#endif
        const char *MSG       = "Hello\3 world!\t";
        ball::RecordAttributes fixedFields(bdlt::Datetime(),
                                          processID,
                                          threadID,
                                          filename,
                                          lineNum,
                                          "FOO.BAR.BAZ",
                                          ball::Severity::e_WARN,
                                          MSG);

        fixedFields.setTimestamp(bdlt::CurrentTime::utc());

        ball::UserFields userFields;
        userFields.appendNull();
        userFields.appendString("string");
        userFields.appendDouble(3.14159265);
        userFields.appendInt64(1000000);
        userFields.appendDatetimeTz(bdlt::DatetimeTz());

        bsl::vector<char> mCA;
        mCA.push_back('h');
        mCA.push_back('e');
        mCA.push_back('l');
        mCA.push_back('l');
        mCA.push_back('o');
        userFields.appendCharArray(mCA);

        Rec mRecord(fixedFields, userFields);
        mRecord.addAttribute(ball::Attribute("name", "Name"));
        mRecord.addAttribute(ball::Attribute("name1", "Name1"));

        P(mX.format());

        mX(oss, mRecord);

        P(oss.str().c_str());

      } break;
      default: {
          bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND."
                    << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
