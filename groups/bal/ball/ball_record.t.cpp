// ball_record.t.cpp                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_record.h>

#include <ball_severity.h>
#include <ball_userfields.h>

#include <bdls_processutil.h>

#include <bdlt_currenttime.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_datetimeutil.h>
#include <bdlt_epochutil.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bslma_testallocatormonitor.h>

#include <bslmt_threadutil.h>

#include <bslim_testutil.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_ctime.h>
#include <bsl_iostream.h>
#include <bsl_new.h>          // placement 'new' syntax
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bsl_cstdlib.h>      // atoi()
#include <bsl_cstring.h>      // strlen(), memset(), memcpy(), memcmp()
#ifdef BSLS_PLATFORM_OS_UNIX
#include <unistd.h>           // getpid()
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test has value-semantic and contains value-semantic
// members.  In addition to standard value-semantic methods, this component
// also implements a method to provide a (tight) upper bound to the dynamic
// memory required to hold the current instance, which is implemented with a
// tested component (ball_countingallocator).  Our concerns regarding the
// implementation of this component are that (1) all the standard
// value-semantic methods are implemented correctly (2) the bookkeeping of the
// dynamically allocated memory are done correctly and (3) the bookkeeping is
// done correctly when exception are thrown.
//
// Note that places where test drivers in this family are likely to require
// adjustment are indicated by the tag: "ADJ".
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] ball::Record(bslma::Allocator *alloc = 0);
// [ 2] ball::Record(const RecordAttributes&, const UserFields&, *ba = 0);
// [ 7] ball::Record(const ball::Record& original, *ba = 0);
// [ 2] ~ball::Record();
//
// MANIPULATORS
// [ 8] ball::Record& operator=(const ball::Record& rhs);
// [ 1] ball::RecordAttributes& fixedFields();
// [ 2] void setFixedFields(const ball::RecordAttributes& fixedFields);
// [ 2] void setCustomFields(const ball::UserFields& customFields);
// [  ] void addAttribute(const ball::Attribute& attribute);
// [ 1] ball::UserFields& customFields();
//
// ACCESSORS
// [ 4] const ball::RecordAttributes& fixedFields() const;
// [ 4] const ball::UserFields& customFields() const;
// [ 2] const bsl::vector<ball::ManagedAttribute>& attributes() const
// [ 9] int numAllocatedBytes() const;
//
// [  ] bsl::ostream& print(bsl::ostream& stream, int level, int spl) const;
//
//  FREE OPERATORS
// [ 6] bool operator==(const ball::Record& lhs, const ball::Record& rhs);
// [ 6] bool operator!=(const ball::Record& lhs, const ball::Record& rhs);
// [  ] bsl::ostream& operator<<(bsl::ostream& stream, const ball::Record& r);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] TESTING GENERATOR FUNCTIONS 'GG' AND 'GGG' ('ball::UserFields')
// [10] USAGE EXAMPLE 1

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
//                    EXCEPTION TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define EXCEPTION_COUNT bslmaExceptionCounter

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::Record           Obj;
typedef ball::RecordAttributes Record_Attr;
typedef ball::UserFields       Values;
typedef bsls::Types::Int64     Int64;

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

const char *const LONG_STRING    = "a_" SUFFICIENTLY_LONG_STRING;

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

    // ------------------------------------------------------------------------
    // The following "global constants for testing" are in 'main' because the
    // test allocator (defined in 'main') is used in the constructors of the
    // test lists and tables.
    //
    // Make three sets of useful constants for testing: Ui, Xi, Yi
    // ------------------------------------------------------------------------

    bslma::TestAllocator  testAllocator(veryVeryVeryVerbose);
    // TBD:
    testAllocator.setQuiet(1);
    bslma::Allocator     *Z = &testAllocator;

    bsl::vector<char> mCA;  const bsl::vector<char>& CA = mCA;
    mCA.push_back('a');  mCA.push_back('b');  mCA.push_back('c');

    Values VALUES_A(Z);
    Values VALUES_B(Z);
    VALUES_B.appendInt64(1);
    Values VALUES_C(Z);
    VALUES_C.appendInt64(1);
    VALUES_C.appendDouble(2.0);
    VALUES_C.appendString("A");
    VALUES_C.appendDatetimeTz(bdlt::DatetimeTz(bdlt::Datetime(1970,1,1), -10));
    VALUES_C.appendCharArray(CA);

    Values *VALUES_DATA[] = { &VALUES_A, &VALUES_B, &VALUES_C };
    const int NUM_VALUES_DATA = sizeof(VALUES_DATA) / sizeof(*VALUES_DATA);

    struct {
        int d_pid;
        int d_tid;
        int d_lineNum;
        int d_severity;
        int d_year;
        int d_month;
        int d_day;
        const char *d_fileName;
        const char *d_category;
        const char *d_message;
    } ATTR_VALUES[] = {
///pid     tid     line sev year  month day  file name   category  message
///------- ------- ---- --- ----  ----  ---  ----------  --------  ----------
 { 1,      0,      0,   0,  2004, 1,    1,   "",         "",       ""        },
 { 0,      1,      0,   0,  2004, 1,    1,   "",         "",       ""        },
 { 0,      0,      1,   0,  2004, 1,    1,   "",         "",       ""        },
 { 0,      0,      0,   1,  2004, 1,    1,   "",         "",       ""        },
 { 0,      0,      0,   0,  2000, 1,    1,   "",         "",       ""        },
 { 0,      0,      0,   0,  2004, 2,    1,   "",         "",       ""        },
 { 0,      0,      0,   0,  2004, 1,    2,   "",         "",       ""        },
 { 0,      0,      0,   0,  2004, 1,    1,   "FILE1",    "",       ""        },
 { 0,      0,      0,   0,  2004, 1,    1,   "",         "CATE1",  ""        },
 { 0,      0,      0,   0,  2004, 1,    1,   "",         "",       "MSG1"    }
        };

    const int NUM_ATTRS = sizeof ATTR_VALUES / sizeof ATTR_VALUES[0];
    Record_Attr REC_ATTRS[10];
    for (int jj = 0; jj < NUM_ATTRS; ++jj) {
        const int PID        = ATTR_VALUES[jj].d_pid;
        const int TID        = ATTR_VALUES[jj].d_tid;
        const int YEAR       = ATTR_VALUES[jj].d_year;
        const int MONTH      = ATTR_VALUES[jj].d_month;
        const int DAY        = ATTR_VALUES[jj].d_day;
        const int LINE       = ATTR_VALUES[jj].d_lineNum;
        const int SEVERITY   = ATTR_VALUES[jj].d_severity;
        const char *FILENAME = ATTR_VALUES[jj].d_fileName;
        const char *CATEGORY = ATTR_VALUES[jj].d_category;
        const char *MESSAGE  = ATTR_VALUES[jj].d_message;

        Record_Attr mY(bdlt::Datetime(YEAR, MONTH, DAY),
                       PID,
                       TID,
                       FILENAME,
                       LINE,
                       CATEGORY,
                       SEVERITY,
                       MESSAGE);

        REC_ATTRS[jj] = mY;
    }

    const Record_Attr AA = REC_ATTRS[0];
    const Record_Attr AB = REC_ATTRS[1];
    const Record_Attr AC = REC_ATTRS[2];
    const Record_Attr AD = REC_ATTRS[3];
    const Record_Attr AE = REC_ATTRS[4];
    const Record_Attr AF = REC_ATTRS[5];
    const Record_Attr AG = REC_ATTRS[6];
    const Record_Attr AH = REC_ATTRS[7];
    const Record_Attr AI = REC_ATTRS[8];
    const Record_Attr AJ = REC_ATTRS[9];
    const Record_Attr AU;   // default value

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 10: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example-2 provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example-2 from header into driver, remove
        //   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Usage Example-2" << endl
                                  << "=======================" << endl;


///Usage
///------
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'ball::Record'
/// - - - - - - - - - - - - - - - - - - -
// The following example demonstrates how to create and set the properties of
// a 'ball::Record'.  Note that users of the 'ball' logging subsystem are not
// expected to create records directly.
//
// First we default create a 'ball::Record', 'record', and verify it has a
// default set of attributes:
//..
    ball::Record record;

    ASSERT(ball::RecordAttributes() == record.fixedFields());
    ASSERT(0                        == record.customFields().length());
//..
// Then, we set the fixed fields of the record to contain a simple message:
//..
    int                 processId = bdls::ProcessUtil::getProcessId();
    bsls::Types::Uint64 threadId  = bslmt::ThreadUtil::selfIdAsUint64();

    ball::RecordAttributes attributes(bdlt::CurrentTime::utc(), // time stamp
                                      processId,                // process id
                                      threadId,                 // thread id
                                      __FILE__,                 // filename
                                      __LINE__,                 // line number
                                      "ExampleCategory",        // category
                                      ball::Severity::e_WARN,   // severity
                                      "Simple Test Message");   // message
    record.setFixedFields(attributes);

    ASSERT(attributes == record.fixedFields());
//..
// Next, we add an additional attribute to the log record:
//..
    record.addAttribute(ball::Attribute("myLib.name", "John Smith"));
//..
// Finally, we write the record to a stream:
//..
    bsl::ostringstream output;
    output << record << bsl::endl;
//..
    if (verbose) {
        bsl::cout << output.str() << bsl::endl;
    }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING FUNCTION numAllocatedBytes()
        //   Verify the memory usage report is correct.
        //
        // Plan:
        //   Create a sequence of objects with a test allocator.  Call the
        //   method under testing and compare the results with those from
        //   the test allocator.
        //
        // Testing:
        //   int numAllocatedBytes() const;
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting 'numAllocatedBytes'." << endl
                 << "==============================" << endl;
        }

        const Obj VU(AU, VALUES_A);
        const Obj VA(AA, VALUES_B);
        const Obj VB(AB, VALUES_C);
        const Obj VC(AC, VALUES_A);
        const Obj VD(AD, VALUES_B);
        const Obj VE(AE, VALUES_C);

        const int NUM_VALUES = 6;
        const Obj VALUES[NUM_VALUES] = { VU, VA, VB, VC, VD, VE };
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        bslma::TestAllocator localTestAllocator;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj X(VALUES[i], &localTestAllocator);
            const Int64 memUsage = X.numAllocatedBytes();
            const Int64 memAlloc = localTestAllocator.numBytesInUse();

            if (veryVerbose) { P_(memUsage); P(memAlloc); }
            ASSERTV(i, memUsage == memAlloc);
        }

      } break;
      case 8: {
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
        //   ball::Record& operator=(const ball::Record& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Assignment Operator"
                          << "\n===========================" << endl;
        {
            const int NUM_RECATTRS = sizeof REC_ATTRS / sizeof REC_ATTRS[0];

            for (int i = 0; i < NUM_RECATTRS; ++i) {
                Obj v;  const Obj& V = v;
                int ii = i % NUM_VALUES_DATA;

                v.setFixedFields(REC_ATTRS[i]);
                v.setCustomFields(*VALUES_DATA[ii]);
                for (int j = 0; j < NUM_RECATTRS; ++j) {
                    Obj u;  const Obj& U = u;
                    int jj = j % NUM_VALUES_DATA;
                    u.setFixedFields(REC_ATTRS[j]);
                    u.setCustomFields(*VALUES_DATA[jj]);
                    if (veryVerbose) { T_;  P_(V);  P_(U); }
                    Obj w(V);  const Obj &W = w;          // control
                    u = V;
                    if (veryVerbose) P(U);
                    ASSERTV(i, j, W == U);
                    ASSERTV(i, j, W == V);
                }
            }

            if (verbose) cout << "\nTesting assignment u = u (Aliasing)."
                              << endl;

            for (int i = 0; i < NUM_RECATTRS; ++i) {
                Obj u;  const Obj& U = u;
                int ii = i % NUM_VALUES_DATA;
                u.setFixedFields(REC_ATTRS[i]);
                u.setCustomFields(*VALUES_DATA[ii]);
                Obj w(U);  const Obj &W = w;              // control
                u = u;
                if (veryVerbose) { T_;  P_(U);  P(W); }
                ASSERTV(i, W == U);
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
        //: 4 Supplying a default-constructed allocator has the same effect as
        //:   not supplying an allocator.
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
        //: 9 The original object is passed as a 'const' reference.
        //:
        //:10 The value of the original object is unchanged.
        //:
        //:11 The allocator used by the original object is unchanged.
        //
        // Plan:
        //: 1  Specify a set S whose elements have substantial and varied
        //:    differences in value.
        //
        //: 2  For each distinct object value: (C-1..11)
        //:
        //:   1 Use the default constructor, basic manipulators, and a
        //:     "scratch" allocator to create two 'const' 'Obj', 'Z' and 'ZZ',
        //:     each having the value 'V'.
        //:
        //:   2 Execute an inner loop that creates an object by
        //:     copy-constructing from value 'Z' from P-2.1, but invokes the
        //:     copy constructor differently in each iteration: (a) without
        //:     passing an allocator, (b) passing a default allocator
        //:     explicitly, (c) passing a test allocator distinct from the
        //:     default.
        //:
        //:   3 For each of these iterations (P-2.2):  (C-1..12)
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
        //:     4 Use the appropriate test allocators to verify that:  (C-2..4,
        //:       7..8, 12)
        //:
        //:       1 An object that IS expected to allocate memory does so
        //:         from the object allocator only (irrespective of the
        //:         specific number of allocations or the total amount of
        //:         memory allocated).  (C-2, 4)
        //:
        //:       2 If an allocator was supplied at construction (P-2.1c), the
        //:         current default allocator doesn't allocate any memory.
        //:         (C-3)
        //:
        //:       3 No temporary memory is allocated from the object allocator.
        //:         (C-7)
        //:
        //:       4 All object memory is released when the object is destroyed.
        //:         (C-8)
        //
        // Testing:
        //   ball::Record(const ball::Record& original, *ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nCOPY CONSTRUCTOR"
                          << "\n================" << endl;
        {
            const int NUM_RECATTRS = sizeof REC_ATTRS / sizeof REC_ATTRS[0];

            for (int i = 0; i < NUM_RECATTRS; ++i) {
                for (int j = 0; j < NUM_VALUES_DATA; ++j) {

                   bslma::TestAllocator scratch("scratch",
                                                veryVeryVeryVerbose);

                   Obj zZ(&scratch);  const Obj& ZZ = zZ;     // control

                    zZ.setFixedFields(REC_ATTRS[i]);
                    zZ.setCustomFields(*VALUES_DATA[j]);
                    zZ.addAttribute(ball::Attribute("t.name", LONG_STRING));
                    zZ.addAttribute(ball::Attribute("t.value", 15));


                    Obj z(&scratch);  const Obj& Z = z;      // source object
                    z.setFixedFields(REC_ATTRS[i]);
                    z.setCustomFields(*VALUES_DATA[j]);
                    z.addAttribute(ball::Attribute("t.name", LONG_STRING));
                    z.addAttribute(ball::Attribute("t.value", 15));


                    for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                        const char CONFIG = cfg;

                        bslma::TestAllocator da("default",
                                                veryVeryVeryVerbose);
                        bslma::TestAllocator fa("footprint",
                                                veryVeryVeryVerbose);
                        bslma::TestAllocator sa("supplied",
                                                veryVeryVeryVerbose);

                        bslma::DefaultAllocatorGuard dag(&da);

                        Obj                  *objPtr = 0;
                        bslma::TestAllocator *objAllocatorPtr = 0;

                        switch (CONFIG) {
                          case 'a': {
                            objAllocatorPtr = &da;
                            objPtr = new (fa) Obj(Z);
                          } break;
                          case 'b': {
                            objAllocatorPtr = &da;
                            objPtr = new (fa) Obj(Z, objAllocatorPtr);
                          } break;
                          case 'c': {
                            objAllocatorPtr = &sa;
                            objPtr = new (fa) Obj(Z, objAllocatorPtr);
                          } break;
                          default: {
                            BSLS_ASSERT_OPT(!"Bad allocator config.");
                          } break;
                        }
                        ASSERTV(CONFIG, sizeof(Obj) == fa.numBytesInUse());

                        Obj& mX = *objPtr;  const Obj& X = mX;

                        if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                        bslma::TestAllocator&  oa = *objAllocatorPtr;
                        bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

                        // Verify the value of the object.

                        ASSERTV(CONFIG,  Z, X,  Z == X);

                        // Verify that the value of 'Z' has not changed.

                        ASSERTV(CONFIG, ZZ, Z, ZZ == Z);

                        // Verify no allocation from the non-object allocator.

                        ASSERTV(CONFIG, noa.numBlocksTotal(),
                                0 == noa.numBlocksTotal());

                        // Verify no temporary memory is allocated from the
                        // object allocator.

                        ASSERTV(CONFIG,
                                oa.numBlocksTotal(),
                                oa.numBlocksInUse(),
                                oa.numBlocksTotal() == oa.numBlocksInUse());

                        // Reclaim dynamically allocated object under test.

                        fa.deleteObject(objPtr);

                        // Verify all memory is released on object destruction.

                        ASSERTV(CONFIG, da.numBlocksInUse(),
                                0 == da.numBlocksInUse());
                        ASSERTV(CONFIG, fa.numBlocksInUse(),
                                0 == fa.numBlocksInUse());
                        ASSERTV(CONFIG, sa.numBlocksInUse(),
                                0 == sa.numBlocksInUse());
                    }
                }
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
        //: 1 Specify a set S of unique object values having various minor or
        //:   subtle differences.  Verify the correctness of 'operator==' and
        //:   'operator!=' using all elements (u, v) of the cross product
        //:   S X S.
        //
        // Testing:
        //   bool operator==(const ball::Record& lhs, const ball::Record& rhs);
        //   bool operator!=(const ball::Record& lhs, const ball::Record& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING EQUALITY OPERATORS"
                          << "\n==========================" << endl;

        if (verbose) cout <<
            "\nCompare each pair of values (u, v) in S X S." << endl;
        {

            ball::Attribute AA("t.name", LONG_STRING);
            ball::Attribute AB("t.value", 15);
            ball::Attribute AC("t.another", 14LL);

            bsl::vector<ball::Attribute> ATTRS;
            ATTRS.push_back(AA);
            ATTRS.push_back(AB);
            ATTRS.push_back(AC);

            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    for (int k = 0; k < 3; ++k) {
                        Obj u;  const Obj& U = u;
                        u.setFixedFields(REC_ATTRS[i]);
                        u.setCustomFields(*VALUES_DATA[j]);
                        u.addAttribute(ATTRS[k]);

                        for (int ii = 0; ii < 3; ++ii) {
                            for (int ij = 0; ij < 3; ++ij) {
                                for (int ik = 0; ik < 3; ++ik) {
                                    Obj v;  const Obj& V = v;

                                    v.setFixedFields(REC_ATTRS[ii]);
                                    v.setCustomFields(*VALUES_DATA[ij]);
                                    v.addAttribute(ATTRS[ik]);

                                    bool isSame = (i == ii)
                                               && (j == ij)
                                               && (k == ik);

                                    if (veryVerbose) { T_;  P_(U);  P(V); }
                                    ASSERTV(i, j, k,  isSame == (U == V));
                                    ASSERTV(i, j, k, !isSame == (U != V));
                                    ASSERTV(i, j, k,  isSame == (V == U));
                                    ASSERTV(i, j, k, !isSame == (V != U));
                                }
                            }
                        }
                    }
                }
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        //
        // Plan:  BUG
        //
        // Testing:
        //   operator<<(ostream&, const ball::Record&);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Testing Output (<<) Operator" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting 'operator<<' (ostream)." << endl;

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //
        //   We want to verify that each individual field is returned
        //   correctly.
        //
        // Plan:
        //   For a sequence of unique object values, verify that each of the
        //   basic accessors returns the correct value.
        //
        // Testing:
        //   const ball::RecordAttributes& fixedFields();
        //   const ball::UserFields& customFields();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing Basic Accessors" << endl
            << "=======================" << endl;


        for (int ii = 0; ii < NUM_VALUES_DATA; ++ii) {
            const Values& X = *VALUES_DATA[ii];

            if (verbose) { cout << "\t"; P(X); }

            for (int jj = 0; jj < NUM_ATTRS; ++jj) {

                const Record_Attr& Y = REC_ATTRS[jj];

                Obj mT(Y, X, &testAllocator);

                if (veryVeryVerbose) { P(mT); }
                ASSERTV(ii, jj, mT.fixedFields()  == Y);
                ASSERTV(ii, jj, mT.customFields() == X);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING '
        // Plan:
        //   Test the behavior of 'gg' and 'ggg'.
        //   * Verify that all test values are distinct.
        //   * Show 'gg' returns a correct reference to its first argument.
        //   * Enumerate representative input, ordered by length.
        //   * Show the correct accessing of all 66 unique values (of 22
        //     unique types) that are a part of the 'gg' language.
        //   * Test large legal input strings.
        //   * Test 'ggg' syntax checking with syntactically illegal inputs.
        //
        // Testing:
        //   ball::UserFields& gg(ball::UserFields *address, const char *spec);
        //   int ggg(UserFields *address, const char *spec, int showErrorFlag);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting 'gg' and 'ggg' generator functions"
                          << "\n=========================================="
                          << endl;


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
        //: 4 Supplying a default-constructed allocator has the same effect as
        //:   not supplying an allocator.
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
        //
        // Plan:
        //: 1 Create three sets of attribute values for the object: ('FD')
        //:   values corresponding to the default-constructed object, ('FA')
        //:   values that allocate memory if possible, and ('FB') other values
        //:   that do not cause additional memory allocation beyond that which
        //:   may be incurred by 'FA'.  Both the 'FA' and 'FB' attribute values
        //:   should be chosen to be boundary values where possible.
        //:
        //: 2 Execute an inner loop that creates an object by
        //:   default-construction, but invokes the default constructor
        //:   differently in each iteration: (a) without passing an allocator,
        //:   (b) passing a default-constructed allocator explicitly, and (c)
        //:   passing the address of a test allocator distinct from the
        //:   default.  For each of these iterations: (C-1..14)
        //:
        //:   1 Create three 'bslma::TestAllocator' objects, and install one as
        //:     as the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Use the default constructor to dynamically create an object
        //:     'X', with its object allocator configured appropriately (see
        //:     P-2); use a distinct test allocator for the object's footprint.
        //:
        //:   3 Use the 'get_allocator' accessor of each underlying attribute
        //:     capable of allocating memory to ensure that its object
        //:     allocator is properly installed; also invoke the (as yet
        //:     unproven) 'get_allocator' accessor of the object under test.
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
        //   ball::Record(bslma::Allocator *alloc = 0);
        //   ball::Record(const RecordAttributes&, const UserFields&, *ba = 0);
        //   void setFixedFields(const ball::RecordAttributes &);
        //   void setCustomFields(const ball::UserFields &);
        //   void addAttribute(const ball::Attribute& attribute);
        //   ~ball::Record();
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nDEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR"
                 << "\n==========================================" << endl;
        }

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        Record_Attr FD;
        // This RecordAttributes will allocate.
        Record_Attr FA(bdlt::Datetime(2004, 1, 21, 12, 30, 25, 150),
                       bdls::ProcessUtil::getProcessId(),
                       0,
                       __FILE__,
                       __LINE__,
                       "MyCategory1",
                       ball::Severity::e_INFO,
                       "Long test message "
                       "0123456789001234567890123456789001234567890123456789"
                       "0123456789001234567890123456789001234567890123456789"
                       "0123456789001234567890123456789001234567890123456789"
                       "0123456789001234567890123456789001234567890123456789");
        // This RecordAttributes will not allocate (all strings are short).
        Record_Attr FB(bdlt::Datetime(2000, 2, 29, 10, 13, 55, 111),
                       bdls::ProcessUtil::getProcessId(),
                       0,
                       "myFile",
                       __LINE__,
                       "MyCategory2",
                       ball::Severity::e_ERROR,
                       "Test message");


        if (verbose) cout << "\nTesting with various allocator configurations."
                          << endl;

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr = 0;
            bslma::TestAllocator *objAllocatorPtr = 0;

            switch (CONFIG) {
              case 'a': {
                objAllocatorPtr = &da;
                objPtr = new (fa) Obj();
              } break;
              case 'b': {
                objAllocatorPtr = &da;
                objPtr = new (fa) Obj(objAllocatorPtr);
              } break;
              case 'c': {
                objAllocatorPtr = &sa;
                objPtr = new (fa) Obj(objAllocatorPtr);
              } break;
              default: {
                BSLS_ASSERT_OPT(!"Bad allocator config.");
              } break;
            }

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

            // -------------------------------------------------------
            // Verify any attribute allocators are installed properly.
            // -------------------------------------------------------

            // Cannot check it - internally ball::Record uses counting
            // allocator and thus all attributes expose different allocator.
            //ASSERTV(CONFIG, &oa == X.customFields().allocator());
            //ASSERTV(CONFIG, &oa ==
            //                     X.attributes().get_allocator().mechanism());

            // Verify no allocation from the object/non-object allocators.

            ASSERTV(CONFIG,  oa.numBlocksTotal(), 0 ==  oa.numBlocksTotal());
            ASSERTV(CONFIG, noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            // Reclaim dynamically allocated object under test.

            // Testing 'setFixedFields()'
            {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    bslma::TestAllocatorMonitor tam(&oa);
                    mX.setFixedFields(FA);
                    ASSERTV(CONFIG, tam.isInUseUp());

#ifdef BDE_BUILD_TARGET_EXC
                    ASSERT(0 < EXCEPTION_COUNT);
#endif
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(CONFIG, FA == X.fixedFields());
                ASSERTV(CONFIG, VALUES_A == X.customFields());

                bslma::TestAllocatorMonitor tam(&oa);

                mX.setFixedFields(FB);
                ASSERTV(CONFIG, FB == X.fixedFields());
                ASSERTV(CONFIG, VALUES_A == X.customFields());

                mX.setFixedFields(FD);
                ASSERTV(CONFIG, FD == X.fixedFields());
                ASSERTV(CONFIG, VALUES_A == X.customFields());

                ASSERTV(CONFIG, tam.isTotalSame());
            }

            // Testing 'setCustomFields()'
            {
                // Allocating UserFields.
                Values uFA;
                const Values& UFA = uFA;
                uFA.appendString(LONG_STRING);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    bslma::TestAllocatorMonitor tam(&oa);
                    mX.setCustomFields(UFA);
                    ASSERTV(CONFIG, tam.isInUseUp());

#ifdef BDE_BUILD_TARGET_EXC
                    ASSERT(0 < EXCEPTION_COUNT);
#endif
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(CONFIG, UFA == X.customFields());
                ASSERTV(CONFIG, FD == X.fixedFields());

                bslma::TestAllocatorMonitor tam(&oa);

                mX.setCustomFields(VALUES_B);
                ASSERTV(CONFIG, VALUES_B == X.customFields());
                ASSERTV(CONFIG, FD == X.fixedFields());

                mX.setCustomFields(VALUES_A);
                ASSERTV(CONFIG, VALUES_A == X.customFields());
                ASSERTV(CONFIG, FD == X.fixedFields());

                ASSERTV(CONFIG, tam.isTotalSame());
            }

            // Testing 'addAttribute()'
            {
                // Attributes for testing
                const ball::Attribute AA("test.name", LONG_STRING);
                const ball::Attribute AB("test.value", 15);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    bslma::TestAllocatorMonitor tam(&oa);
                    mX.addAttribute(AA);
                    ASSERTV(CONFIG, tam.isInUseUp());

#ifdef BDE_BUILD_TARGET_EXC
                    ASSERT(0 < EXCEPTION_COUNT);
#endif
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(CONFIG, 1 == X.attributes().size());
                ASSERTV(CONFIG, AA == X.attributes()[0]);

                ASSERTV(CONFIG, FD == X.fixedFields());
                ASSERTV(CONFIG, VALUES_A == X.customFields());

                bslma::TestAllocatorMonitor tam(&oa);

                mX.addAttribute(AB);
                ASSERTV(CONFIG, 2 == X.attributes().size());
                ASSERTV(CONFIG, AA == X.attributes()[0]);
                ASSERTV(CONFIG, AB == X.attributes()[1]);

                ASSERTV(CONFIG, FD == X.fixedFields());
                ASSERTV(CONFIG, VALUES_A == X.customFields());

                ASSERTV(CONFIG, tam.isTotalUp());
            }

            fa.deleteObject(objPtr);
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {

            bslma::TestAllocator ta("test", veryVeryVeryVerbose);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {
                if (veryVerbose) {
                    cout << "\tTesting Exceptions In Default Ctor" << endl;
                }
                const Obj X(&ta);

#ifdef BDE_BUILD_TARGET_EXC
                ASSERTV(0 == EXCEPTION_COUNT);
#endif
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }

        // Legacy test. Not sure I understand what they are testing.
        if (verbose) cout << "\nTesting ctor (thoroughly)." << endl;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            const Obj X(FA, *VALUES_DATA[0], (bslma::Allocator *)0);
            if (veryVerbose) { cout << "\t\t"; P(X); }
        }

        if (verbose) cout << "\tPassing in an allocator." << endl;

        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            const Obj X(FA, *VALUES_DATA[1], &testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            if (veryVerbose) cout <<
                "\tTesting Exceptions In Ctor" << endl;
            const Obj X(FA, *VALUES_DATA[1], &testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
          } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }
        if (verbose) cout << "\nTesting primary manipulator." << endl;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            Obj mX;
            Obj mY(FA, *VALUES_DATA[1]);
            Obj mZ(FB, *VALUES_DATA[2]);

            if (veryVeryVerbose) { P(mX); P(mY); P(mZ); }

            ASSERT(FD != FA);
            ASSERT(FD != FB);
            ASSERT(FA != FB);

            ASSERT(0 == (mX == mY));
            ASSERT(0 == (mX == mZ));
            ASSERT(0 == (mY == mZ));

            ASSERT(mX.fixedFields() == FD);
            ASSERT(mY.fixedFields() == FA);
            ASSERT(mZ.fixedFields() == FB);

            ASSERT(mX.customFields() == Values());
            ASSERT(mY.customFields() == *VALUES_DATA[1]);
            ASSERT(mZ.customFields() == *VALUES_DATA[2]);

            if (veryVerbose) cout << "\tSetting mX with mY's initializer."
                                  << endl;
            mX.setFixedFields(FA);
            ASSERT(mX.fixedFields() == FA);

            mX.setCustomFields(*VALUES_DATA[1]);
            ASSERT(mX.customFields() == *VALUES_DATA[1]);

            ASSERT(mX == mY);

            if (veryVerbose) cout << "\tSetting mX with mZ's initializer."
                                  << endl;
            mX.setFixedFields(FB);
            ASSERT(mX.fixedFields() == FB);

            mX.setCustomFields(*VALUES_DATA[2]);
            ASSERT(mX.customFields() == *VALUES_DATA[2]);

            ASSERT(mX == mZ);
        }

        if (verbose) cout << "\tWith an allocator." << endl;
        if (verbose) cout << "\t\tWithout exceptions." << endl;
        {
            bslma::TestAllocator testAllocatorX(veryVeryVeryVerbose);
            bslma::TestAllocator testAllocatorY(veryVeryVeryVerbose);
            bslma::TestAllocator testAllocatorZ(veryVeryVeryVerbose);
            Obj mX(&testAllocatorX);
            Obj mY(FA, *VALUES_DATA[1], &testAllocatorY);
            Obj mZ(FB, *VALUES_DATA[2], &testAllocatorZ);

            if (veryVeryVerbose) { P(mX); P(mY); P(mZ); }

            ASSERT(FD != FA);
            ASSERT(FD != FB);
            ASSERT(FA != FB);

            ASSERT(0 == (mX == mY));
            ASSERT(0 == (mX == mZ));
            ASSERT(0 == (mY == mZ));

            ASSERT(mX.fixedFields() == FD);
            ASSERT(mY.fixedFields() == FA);
            ASSERT(mZ.fixedFields() == FB);

            ASSERT(mX.customFields() == Values());
            ASSERT(mY.customFields() == *VALUES_DATA[1]);
            ASSERT(mZ.customFields() == *VALUES_DATA[2]);

            if (veryVerbose) cout << "\tSetting mX with mY's initializer."
                                  << endl;
            mX.setFixedFields(FA);
            ASSERT(mX.fixedFields() == FA);

            mX.setCustomFields(*VALUES_DATA[1]);
            ASSERT(mX.customFields() == *VALUES_DATA[1]);

            ASSERT(mX == mY);

            if (veryVerbose) cout << "\tSetting mX with mZ's initializer."
                                  << endl;
            mX.setFixedFields(FB);
            ASSERT(mX.fixedFields() == FB);

            mX.setCustomFields(*VALUES_DATA[2]);
            ASSERT(mX.customFields() == *VALUES_DATA[2]);

            ASSERT(mX == mZ);
        }
        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
            bslma::TestAllocator testAllocatorX(veryVeryVeryVerbose);
            bslma::TestAllocator testAllocatorY(veryVeryVeryVerbose);
            bslma::TestAllocator testAllocatorZ(veryVeryVeryVerbose);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                if (veryVerbose) cout <<
                    "\tTesting Exceptions In Primary Manipulator" << endl;
                Obj mX(&testAllocatorX);
                Obj mY(FA, *VALUES_DATA[1], &testAllocatorY);
                Obj mZ(FB, *VALUES_DATA[2], &testAllocatorZ);

                if (veryVeryVerbose) { P(mX); P(mY); P(mZ); }

                ASSERT(FD != FA);
                ASSERT(FD != FB);
                ASSERT(FA != FB);
                ASSERT(0 == (mX == mY));
                ASSERT(0 == (mX == mZ));
                ASSERT(0 == (mY == mZ));

                ASSERT(mX.fixedFields() == FD);
                ASSERT(mY.fixedFields() == FA);
                ASSERT(mZ.fixedFields() == FB);

                ASSERT(mX.customFields() == Values());
                ASSERT(mY.customFields() == *VALUES_DATA[1]);
                ASSERT(mZ.customFields() == *VALUES_DATA[2]);

                if (veryVerbose) cout << "\tSetting mX with mY's initializer."
                                      << endl;
                mX.setFixedFields(FA);
                ASSERT(mX.fixedFields() == FA);

                mX.setCustomFields(*VALUES_DATA[1]);
                ASSERT(mX.customFields() == *VALUES_DATA[1]);

                ASSERT(mX == mY);

                if (veryVerbose) cout << "\tSetting mX with mZ's initializer."
                                << endl;
                mX.setFixedFields(FB);
                ASSERT(mX.fixedFields() == FB);

                mX.setCustomFields(*VALUES_DATA[2]);
                ASSERT(mX.customFields() == *VALUES_DATA[2]);

                ASSERT(mX == mZ);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
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

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        Record_Attr FA(bdlt::CurrentTime::utc(),
                       bdls::ProcessUtil::getProcessId(),
                       0,
                       __FILE__,
                       __LINE__,
                       "EQUITY.NASD",
                       ball::Severity::e_INFO,
                       "Ticker Summary");

        if (verbose) { P(FA) ; }

        Obj RA(FA, *VALUES_DATA[1]);

        ASSERTV(FA == RA.fixedFields());
        ASSERTV(*VALUES_DATA[1] == RA.customFields());

        Record_Attr FB(bdlt::CurrentTime::utc(),
                       bdls::ProcessUtil::getProcessId(),
                       0,
                       __FILE__,
                       __LINE__,
                       "USER_SESSION",
                       ball::Severity::e_TRACE,
                       "User Session Info");

        if (verbose) { P(FB) ; }

        Obj RB(FB, *VALUES_DATA[2]);
        ASSERT(FB == RB.fixedFields());
        ASSERT(*VALUES_DATA[2] == RB.customFields());

        Obj RC(RA);
        ASSERT(RC == RA);               ASSERT(RC != RB);

        RA.setFixedFields(FB);
        RA.setCustomFields(*VALUES_DATA[2]);
        ASSERT(FB == RA.fixedFields());
        ASSERT(*VALUES_DATA[2] == RA.customFields());
        ASSERT(RA == RB);               ASSERT(RA != RC);

        RA.setFixedFields(FA);
        RA.setCustomFields(*VALUES_DATA[1]);
        ASSERT(FA == RA.fixedFields());
        ASSERT(*VALUES_DATA[1] == RA.customFields());
        ASSERT(RA != RB);               ASSERT(RA == RC);

        RC = RB;
        ASSERT(FB == RC.fixedFields());
        ASSERT(*VALUES_DATA[2] == RC.customFields());
        ASSERT(RA != RC);               ASSERT(RB == RC);

        const int sizea = RA.numAllocatedBytes();
        const int sizeb = RB.numAllocatedBytes();
        if (verbose) { P_(sizea); P(sizeb); }
        ASSERT(0 < sizea);
        ASSERT(0 < sizeb);

        {
            if (verbose) {
                cout << "testing const versions of 'fixedFields'"
                     << "and 'customFields'"
                     << endl;
            }

            Obj RA(FA, *VALUES_DATA[1]);
            Record_Attr &ff = RA.fixedFields();
            ff = FB;
            Values &uf = RA.customFields();
            uf = *VALUES_DATA[2];
            const Obj &CRA = RA;
            ASSERTV(CRA.fixedFields(), CRA.fixedFields() == FB);
            ASSERTV(CRA.customFields(), CRA.customFields() == *VALUES_DATA[2]);
            if (verbose) {
                cout << "tested const versions of 'fixedFields'"
                     << "and 'customFields'"
                     << endl;
            }
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
