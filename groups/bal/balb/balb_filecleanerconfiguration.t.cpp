// balb_filecleanerconfiguration.t.cpp                                -*-C++-*-
#include <balb_filecleanerconfiguration.h>

#include <bslma_default.h>
#include <bslma_allocator.h>
#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_destructorguard.h>
#include <bslma_testallocatormonitor.h>

#include <bslim_testutil.h>
#include <bsls_assert.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen()
#include <bsl_functional.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// This test plan follows the standard approach for components implementing
// value-semantic types.
//
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] FileCleanerConfiguration(bslma::Allocator *alloc = 0);
// [ 8] FileCleanerConfiguration(filePattern, maxAge, minNumber, *alloc);
// [ 7] FileCleanerConfiguration(const FileCleanerConfiguration&, *alloc);
// [ 2] ~FileCleanerConfiguration();
//
// MANIPULATORS
// [ 9] FileCleanerConfiguration operator=(const FCC&);
// [ 2] void setFilePattern(const bslstl::StringRef& filePattern);
// [ 2] void setMaxFileAge(const bsls::TimeInterval& maxAge);
// [ 2] void setMinNumFiles(int minNumber);
//
// ACCESSORS
// [ 4] const bsl::string& filePattern() const;
// [ 4] bsls::TimeInterval maxFileAge() const;
// [ 4] int minNumFiles() const;
// [ 5] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//
// FREE FUNCTIONS
// [ 6] bool operator==(const FCC& lhs, const FCC& rhs);
// [ 6] bool operator!=(const FCC& lhs, const FCC& rhs);
// [ 5] ostream& operator<<(ostream&, const balb::FCC&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [10] USAGE EXAMPLE
// [ 3] FCC& gg(FCC *object, const char *spec);
// [ 3] int ggg(FCC *object, const char *spec, int verbose = 1);
//-----------------------------------------------------------------------------

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef balb::FileCleanerConfiguration Obj;

struct DefaultDataRow {
    int         d_line;     // source line number
    const char *d_spec_p;   // specification string
};

// The table contains valid configuration specs used by multiple tests.
static const DefaultDataRow DEFAULT_DATA[] = {
    //line  spec
    //----  -----
    { L_,   "AAA"  },    // equivalent to default
    { L_,   "AAB"  },
    { L_,   "AAC"  },
    { L_,   "AAD"  },
    { L_,   "ABA"  },
    { L_,   "ABB"  },
    { L_,   "ABC"  },
    { L_,   "ABD"  },
    { L_,   "ACA"  },
    { L_,   "ACB"  },
    { L_,   "ACC"  },
    { L_,   "ACD"  },
    { L_,   "ADA"  },
    { L_,   "ADB"  },
    { L_,   "ADC"  },
    { L_,   "ADD"  },
    { L_,   "BAA"  },
    { L_,   "BAB"  },
    { L_,   "BAC"  },
    { L_,   "BAD"  },
    { L_,   "BBA"  },
    { L_,   "BBB"  },
    { L_,   "BBC"  },
    { L_,   "BBD"  },
    { L_,   "BCA"  },
    { L_,   "BCB"  },
    { L_,   "BCC"  },
    { L_,   "BCD"  },
    { L_,   "BDA"  },
    { L_,   "BDB"  },
    { L_,   "BDC"  },
    { L_,   "BDD"  },
    { L_,   "CAA"  },
    { L_,   "CAB"  },
    { L_,   "CAC"  },
    { L_,   "CAD"  },
    { L_,   "CBA"  },
    { L_,   "CBB"  },
    { L_,   "CBC"  },
    { L_,   "CBD"  },
    { L_,   "CCA"  },
    { L_,   "CCB"  },
    { L_,   "CCC"  },
    { L_,   "CCD"  },
    { L_,   "CDA"  },
    { L_,   "CDB"  },
    { L_,   "CDC"  },
    { L_,   "CDD"  },
    { L_,   "DAA"  },
    { L_,   "DAB"  },
    { L_,   "DAC"  },
    { L_,   "DAD"  },
    { L_,   "DBA"  },
    { L_,   "DBB"  },
    { L_,   "DBC"  },
    { L_,   "DBD"  },
    { L_,   "DCA"  },
    { L_,   "DCB"  },
    { L_,   "DCC"  },
    { L_,   "DCD"  },
    { L_,   "DDA"  },
    { L_,   "DDB"  },
    { L_,   "DDC"  },
    { L_,   "DDD"  },
};

enum { DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA };

//=============================================================================
//                         GLOBAL HELPER FUNCTIONS
//-----------------------------------------------------------------------------
namespace {

Obj& gg(Obj *object, const char *spec);
int ggg(Obj *object, const char *spec, int verbose = 1);

Obj& gg(Obj *object, const char *spec)
{
    ASSERTV(ggg(object, spec) < 0);
    return *object;
}

int ggg(Obj *object, const char *spec, int verbose)
{
    enum { SUCCESS = -1 };

    if (strlen(spec) != 3) {
        if (verbose) {
            cout << "Error, bad spec length: " << strlen(spec) << endl;
        }
        return 0;                                                     // RETURN
    }

    int idx = 0;

    switch (spec[idx]) {
      case 'A': {
        object->setFilePattern("");
      } break;
      case 'B': {
        object->setFilePattern("FirstFilePattern");
      } break;
      case 'C': {
        object->setFilePattern("SecondFilePattern");
      } break;
      case 'D': {
        object->setFilePattern("LongLongLongLongLongLongLongLongFilePattern");
      } break;
      case 'Z': {
        object->setFilePattern("UniquePattern");
      } break;
      default: {
        if (verbose) {
            cout << "Error, bad character (" << spec[idx] << ") "
                 << "in spec \"" << spec << "%s\""
                 << "at position " << idx << endl;
        }
        return idx;                                                   // RETURN
      }
    }

    ++idx;

    switch (spec[idx]) {
      case 'A': {
        object->setMaxFileAge(bsls::TimeInterval());
      } break;
      case 'B': {
        object->setMaxFileAge(bsls::TimeInterval(20,0));
      } break;
      case 'C': {
        object->setMaxFileAge(bsls::TimeInterval(-20,0));
      } break;
      case 'D': {
        object->setMaxFileAge(bsls::TimeInterval(10000,0));
      } break;
      case 'Z': {
        object->setMaxFileAge(bsls::TimeInterval(99999,0));
      } break;
      default: {
        if (verbose) {
            cout << "Error, bad character (" << spec[idx] << ") "
                 << "in spec \"" << spec << "%s\""
                 << "at position " << idx << endl;
        }
        return idx;                                                   // RETURN
      }
    }

    ++idx;

    switch (spec[idx]) {
      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'Z': {
        object->setMinNumFiles(spec[idx] - 'A');
      } break;
      default: {
        if (verbose) {
            cout << "Error, bad character (" << spec[idx] << ") "
                 << "in spec \"" << spec << "%s\""
                 << "at position " << idx << endl;
        }
        return idx;                                                   // RETURN
      }
    }

    return SUCCESS;
}

}  // close unnamed namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test                = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose             = argc > 2;
    bool veryVerbose         = argc > 3;
    bool veryVeryVerbose     = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 10: {
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
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// The following code illustrates how to create a configuration that can be
// later supplied to a file cleanup utility (see {'balb_filecleanerutil'} for
// an example of how to use the created configuration to perform file
// cleaning).
//
// First, we create a 'balb::FileCleanerConfiguration' object having the
// default value:
//..
    balb::FileCleanerConfiguration config("/var/log/myApp/log*",
                                          bsls::TimeInterval(60*60*24),
                                          4);
//..
// Now, we verify the options are configured correctly:
//..
    ASSERT("/var/log/myApp/log*" == config.filePattern());
    ASSERT(bsls::TimeInterval(60*60*24) == config.maxFileAge());
    ASSERT(4 == config.minNumFiles());
//..
// Finally, we print the configuration value to 'stdout' and return:
//..
    if (verbose) {
        bsl::cout << config << bsl::endl;
    }
//..
// This produces the following (multi-line) output:
//..
//  [
//      FilePattern = /var/log/myApp/log*
//      MaxFileAge = (86400, 0)
//      MinNumFiles = 4
//  ]
//..
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING COPY-ASSIGNMENT OPERATOR
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
        //:   1 Specify a set of (unique) valid object values.
        //:
        //: 4 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3: (C-1..2, 5..8, 10)
        //:
        //:   1 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3. For each of the iterations (P-4.2):
        //:     (C-1..2, 5..8, 10)
        //:
        //:     1 Use the value constructor and a "scratch" allocator to create
        //:       two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:     2 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:     3 Use the value constructor and 'oa' to create a modifiable
        //:       'Obj','mX', having the value 'W'.
        //:
        //:     4 Assign 'mX' from 'Z'.
        //:
        //:     5 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     6 Use the equality-comparison operator to verify that: (C-1, 6)
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-6)
        //:
        //:     7 Verify that the respective allocator addresses held by 'mX'
        //:       and 'Z' are unchanged.  (C-2, 7)
        //:
        //:     8 Use the appropriate test allocators to verify that: (C-8, 10)
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
        //:         (C-10)
        //:
        //: 5 Repeat steps similar to those described in P-4 except that, this
        //:   time, there is no inner loop (as in P-4.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-3: (C-9)
        //:
        //:   1 Create a 'bslma_TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor and 'oa' to create a modifiable 'Obj'
        //:     'mX'; also use the value constructor and a distinct "scratch"
        //:     allocator to create a 'const' 'Obj' 'ZZ'.
        //:
        //:   3 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   4 Assign 'mX' from 'Z'.
        //:
        //:   5 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   6 Use the equality-comparison operator to verify that the target
        //:     object, 'mX', still has the same value as that of 'ZZ'.
        //:
        //:   7 Verify 'mX' is still using the object allocator.
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
        //   FileCleanerConfiguration operator=(const FCC&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING COPY-ASSIGNMENT OPERATORS"
                          << "\n=================================" << endl;

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        {
            // Create first object
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE1   = DATA[ti].d_line;
                const char *const SPEC1   = DATA[ti].d_spec_p;

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                // Create second object
                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int         LINE2   = DATA[tj].d_line;
                    const char *const SPEC2   = DATA[tj].d_spec_p;

                    Obj mZ(&scratch);  const Obj& Z  = gg(&mZ,  SPEC1);
                    Obj mZZ(&scratch); const Obj& ZZ = gg(&mZZ, SPEC1);

                    if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                    {
                        Obj mX(&oa);  const Obj& X  = gg(&mX,  SPEC2);

                        if (veryVerbose) { T_ P_(LINE2) P(X) }

                        bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                        Obj *mR = &(mX = Z);
                        ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                        ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);

                        ASSERTV(LINE1, LINE2, ZZ, Z, ZZ == Z);

                        ASSERTV(LINE1, LINE2, sam.isInUseSame());

                        ASSERTV(LINE1, LINE2,
                                &oa == X.filePattern().get_allocator());
                        ASSERTV(LINE1, LINE2,
                                &scratch == Z.filePattern().get_allocator());

                        ASSERTV(LINE1, LINE2, 0 == da.numBlocksTotal());
                    }

                    // Verify all memory is released on object destruction.

                    ASSERTV(LINE1, LINE2, oa.numBlocksInUse(),
                                 0 == oa.numBlocksInUse());
                }

                // self-assignment

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    bslma::TestAllocator scratch("scratch",
                                                 veryVeryVeryVerbose);

                    Obj  mX(&oa);        const Obj& X  = gg(&mX,  SPEC1);
                    Obj  mZZ(&scratch);  const Obj& ZZ = gg(&mZZ,  SPEC1);
                    Obj& mZ = mX;        const Obj& Z  = mZ;

                    ASSERTV(LINE1, ZZ, Z, ZZ == Z);

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    Obj *mR = &(mX = Z);
                    ASSERTV(LINE1, ZZ,   Z, ZZ == Z);
                    ASSERTV(LINE1, mR,  &X, mR == &X);

                    ASSERTV(LINE1, sam.isTotalSame());
                    ASSERTV(LINE1, oam.isTotalSame());

                    ASSERTV(LINE1, &oa == Z.filePattern().get_allocator());

                    ASSERTV(LINE1, 0 == da.numBlocksTotal());
                }

                // Verify all object memory is released on destruction.

                ASSERTV(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING VALUE CONSTRUCTOR
        //
        // Concern:
        //: 1 The value constructor creates an object having the specified
        //:   attribute values.
        //:
        //: 2 If an allocator is NOT supplied to the value constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 3 If an allocator IS supplied to the value constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the value constructor has no effect
        //:   on subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary memory allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 QoI: Creating an object having the default-constructed value
        //:   allocates no memory.
        //:
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object attribute values.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1..8)
        //:
        //:   1 Execute an inner loop creating three distinct objects, in turn,
        //:     each object having the same value, 'V', but configured
        //:     differently: (a) without passing an allocator, (b) passing a
        //:     null allocator address explicitly, and (c) passing the address
        //:     of a test allocator distinct from the default allocator.
        //:
        //:   2 For each of the three iterations in P-2.1:  (C-1..8)
        //:
        //:     1 Create three 'bslma::TestAllocator' objects, and install one
        //:       as the current default allocator (note that a ubiquitous test
        //:       allocator is already installed as the global allocator).
        //:
        //:     2 Use the value constructor to dynamically create an object
        //:       using the attribute values, with object allocator configured
        //:       appropriately (see P-2.2.1), supplying all the arguments as
        //:       'const'; use a distinct test allocator for the object's
        //:       footprint.
        //:
        //:     3 Verify that all of the attributes of each object have their
        //:       expected values.  (C-1, 5)
        //:
        //:     4 Use the appropriate test allocators to verify that:
        //:       (C-2..4, 6..8)
        //:
        //:       1 An object that IS expected to allocate memory does so from
        //:         the object allocator only (irrespective of the specific
        //:         number of allocations or the total amount of memory
        //:         allocated).  (C-6)
        //:
        //:       2 An object that is expected NOT to allocate memory does not
        //:         allocate memory.
        //:
        //:       3 If an allocator was supplied at construction (P-2.1c), the
        //:         default allocator doesn't allocate any memory.  (C-6)
        //:
        //:       4 No temporary memory is allocated from the object allocator.
        //:         (C-7)
        //:
        //:       5 All object memory is released when the object is destroyed.
        //:         (C-8)
        //
        // Testing:
        //   FileCleanerConfiguration(filePattern, maxAge, minNumber, *alloc);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING VALUE CONSTRUCTOR"
                          << "\n=========================" << endl;

        static const struct {
            int                 d_line;
            const char         *d_filePattern_p;
            bsls::TimeInterval  d_interval;
            int                 d_minNumFiles;
        } DATA [] = {
        //LINE   FILEPATTERN  INTERVAL                  MIN_NUM_FILES
        //----   -----------  ---------------------     -------------
        { L_,    "",          bsls::TimeInterval(),     0             },
        { L_,    "filepat*",  bsls::TimeInterval(),     0             },
        { L_,    "fIlepat*",  bsls::TimeInterval(),     0             },
        { L_,    "filepatt*", bsls::TimeInterval(1),    0             },
        { L_,    "filepatt*", bsls::TimeInterval(2),    0             },
        { L_,    "Filepatt*", bsls::TimeInterval(2),    1             },
        { L_,    "Filepatt*", bsls::TimeInterval(2),    2             },
        };

        enum { DATA_LEN = sizeof(DATA) / sizeof(*DATA) };

        {
            for (int ti = 0; ti < DATA_LEN; ++ti) {
                const int         LINE    = DATA[ti].d_line;
                const char *FILE_PATTERN  = DATA[ti].d_filePattern_p;
                const int   MIN_NUM_FILES = DATA[ti].d_minNumFiles;

                const bsls::TimeInterval INTERVAL = DATA[ti].d_interval;


                if (veryVerbose) {
                    P_(LINE) P_(FILE_PATTERN) P_(INTERVAL) P(MIN_NUM_FILES);
                }

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
                    const char CONFIG = cfg;  // how we specify the allocator

                    if (veryVerbose) { T_ T_ P(CONFIG) }

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    // Install default allocator.

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj                 *objPtr;
                    bslma::TestAllocator *objAllocatorPtr;

                    switch (CONFIG) {
                      case 'a': {
                          objPtr = new (fa) Obj(FILE_PATTERN,
                                                INTERVAL,
                                                MIN_NUM_FILES);
                          objAllocatorPtr = &da;
                      } break;
                      case 'b': {
                          objPtr = new (fa) Obj(FILE_PATTERN,
                                                INTERVAL,
                                                MIN_NUM_FILES,
                                                (bslma::Allocator*)0);
                          objAllocatorPtr = &da;
                      } break;
                      case 'c': {
                          objPtr = new (fa) Obj(FILE_PATTERN,
                                                INTERVAL,
                                                MIN_NUM_FILES,
                                                &sa);
                          objAllocatorPtr = &sa;
                      } break;
                      default: {
                          ASSERTV(LINE, CONFIG, !"Bad allocator config.");
                      } break;
                    }
                    ASSERTV(LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                    // Verify no temporary memory is allocated from the object
                    // allocator.

                    ASSERTV(LINE, CONFIG,
                            oa.numBlocksTotal(), oa.numBlocksInUse(),
                            oa.numBlocksTotal() == oa.numBlocksInUse());

                    // Ensure the first row of the table contains the
                    // default-constructed value.

                    static bool firstFlag = true;
                    if (firstFlag) {
                        ASSERTV(LINE, CONFIG, Obj(), *objPtr,
                                Obj() == *objPtr);
                        firstFlag = false;
                    }

                    // Verify the expected attributes values.

                    ASSERTV(LINE, FILE_PATTERN  == X.filePattern());
                    ASSERTV(LINE, INTERVAL      == X.maxFileAge());
                    ASSERTV(LINE, MIN_NUM_FILES == X.minNumFiles());

                    // Verify any attribute allocators are installed properly.

                    ASSERTV(LINE, CONFIG,
                            &oa == X.filePattern().get_allocator());

                    // Verify no allocation from the non-object allocator.

                    ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                            0 == noa.numBlocksTotal());

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
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //
        // Concerns:
        //: 1 The new object's value is the same as that of the original object
        //:   (relying on the equality operator).
        //:
        //: 2 All internal representations of a given value can be used to
        //:   create a new object of equivalent value.
        //:
        //: 3 The value of the original object is left unaffected.
        //:
        //: 4 Subsequent changes in or destruction of the source object have no
        //:   effect on the copy-constructed object.
        //:
        //: 5 Subsequent changes on the created object have no effect on the
        //:   original object.
        //:
        //: 6 The object has its internal memory management system hooked up
        //:   properly so that *all* internally allocated memory draws from a
        //:   user-supplied allocator whenever one is specified.
        //:
        //: 7 The function is exception neutral w.r.t. memory allocation.
        //
        // Plan:
        //: 1 Specify a set S of object values with substantial and varied
        //:   differences to be used in the following tests.
        //:
        //: 2 For each value in S, initialize objects 'W' and 'X', copy
        //:   construct 'Y' from 'X' and use 'operator==' to verify that both
        //:   'X' and 'Y' subsequently have the same value as 'W'.  Let 'X' go
        //:   out of scope and again verify that 'W == Y'.  (C-1..4)
        //:
        //: 3 For each value in S initialize objects 'W' and 'X', and copy
        //:   construct 'Y' from 'X'.  Change the state of 'Y'.  Using the
        //:   'operator!=' verify that 'Y' differs from 'X' and 'W'.  (C-5)
        //:
        //: 4 Perform tests performed as P-2:  (C-6)
        //:   1 While passing an allocator as a parameter to the new object
        //:     and ascertaining that the new object gets its memory from the
        //:     provided allocator.
        //:
        //:   2 Verify neither of global and default allocator is used to
        //:     supply memory.  (C-6)
        //:
        //: 5 Perform tests as P-2 in the presence of exceptions during memory
        //:   allocations using a 'bslma::TestAllocator' and varying its
        //:   *allocation* *limit*.  (C-7)
        //
        // Testing:
        //   FileCleanerConfiguration(const FileCleanerConfiguration&, *alloc);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING COPY CONSTRUCTOR"
                          << "\n========================" << endl;

        {
            const int NUM_DATA                     = DEFAULT_NUM_DATA;
            const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            for (size_t ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE = DATA[ti].d_line;
                const char *const SPEC = DATA[ti].d_spec_p;

                if (veryVerbose) {
                    T_ P_(LINE) P(SPEC);
                }

                // Create control object w.
                Obj mW;  const Obj& W = gg(&mW, SPEC);

                ASSERTV(SPEC, bslma::Default::defaultAllocator()
                                           == W.filePattern().get_allocator());

                if (veryVerbose) { cout << "\t\tControl Obj: "; P(W); }

                Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

                ASSERTV(SPEC, &oa == X.filePattern().get_allocator());

                if (veryVerbose) { cout << "\t\tDynamic Obj: "; P(X); }

                {   // Testing concern 1..4.

                    Obj *pX = new Obj(&oa);
                    gg(pX, SPEC);

                    const Obj Y0(*pX);

                    ASSERTV(SPEC, W == Y0);
                    ASSERTV(SPEC, W == X);

                    delete pX;
                    ASSERTV(SPEC, W == Y0);
                }

                {   // Testing concern 5.
                    if (veryVerbose)
                        cout << "\t\t\tModify, no allocator." << endl;

                    Obj Y1(X);

                    if (veryVeryVerbose) {
                        cout << "\t\t\t\tBefore modification: "; P(Y1);
                    }

                    ASSERTV(SPEC, W == Y1);
                    ASSERTV(SPEC, X == Y1);

                    gg(&Y1, "ZZZ");

                    if (veryVeryVerbose) {
                        cout << "\t\t\t\tAfter modification : "; P(Y1);
                    }

                    ASSERTV(SPEC, W != Y1);
                    ASSERTV(SPEC, X != Y1);
                    ASSERTV(SPEC, W == X);

                    ASSERTV(SPEC, bslma::Default::defaultAllocator()
                                          == Y1.filePattern().get_allocator());
                }

                {   // Testing concern 5 with test allocator.

                    if (veryVerbose)
                        cout << "\t\t\tModify, with test allocator." << endl;

                    Obj Y2(X, &oa);

                    ASSERTV(SPEC, W == Y2);
                    ASSERTV(SPEC, X == Y2);

                    ASSERTV(SPEC, &oa == Y2.filePattern().get_allocator());

                    if (veryVeryVerbose) {
                        cout << "\t\t\t\tBefore modification: "; P(Y2);
                    }

                    gg(&Y2, "ZZZ");

                    if (veryVeryVerbose) {
                        cout << "\t\t\t\tAfter modification : "; P(Y2);
                    }

                    ASSERTV(SPEC, W != Y2);
                    ASSERTV(SPEC, X != Y2);
                    ASSERTV(SPEC, W == X);

                    ASSERTV(SPEC, &oa == Y2.filePattern().get_allocator());
                }
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if they
        //:   contain the same values.
        //:
        //: 2 No non-salient attributes (i.e., 'allocator') participate.
        //:
        //: 3 'true  == (X == X)' (i.e., identity)
        //:
        //: 4 'false == (X != X)' (i.e., identity)
        //:
        //: 5 'X == Y' if and only if 'Y == X' (i.e., commutativity)
        //:
        //: 6 'X != Y' if and only if 'Y != X' (i.e., commutativity)
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
        //:
        //:11 The equality operator's signature and return type are standard.
        //:
        //:12 The inequality operator's signature and return type are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.  (C-8..9, 11..12)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique, specify a set of distinct
        //:   specifications for the 'gg' function.
        //:
        //: 4 For each row 'R1' in the table of P-3: (C-1..7)
        //:
        //:   1 Create a single object, usining a "scratch" allocator, and use
        //:     it to verify the reflexive (anti-reflexive) property of
        //:     equality (inequality) in the presence of aliasing.  (C-3..4)
        //:
        //:   2 For each row 'R2' in the table of P-3: (C-1..2, 5..7)
        //:
        //:     1 Record, in 'EXP', whether or not distinct objects created
        //:       from 'R1' and 'R2', respectively, are expected to have the
        //:       same value.
        //:
        //:     2 For each of two configurations, 'a' and 'b': (C-1..2, 5..7)
        //:
        //:       1 Create two (object) allocators, 'oax' and 'oay'.
        //:
        //:       2 Create an object 'X', using 'oax', having the value 'R1'.
        //:
        //:       3 Create an object 'Y', using 'oax' in configuration 'a' and
        //:         'oay' in configuration 'b', having the value 'R2'.
        //:
        //:       5 Verify the commutativity property and expected return value
        //:         for both '==' and '!=', while monitoring both 'oax' and
        //:         'oay' to ensure that no object memory is ever allocated by
        //:         either operator.  (C-1..2, 5..7, 10)
        //:
        //: 5 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-11)
        //
        // Testing:
        //   bool operator==(const FCC& lhs, const FCC& rhs);
        //   bool operator!=(const FCC& lhs, const FCC& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING EQUALITY OPERATORS"
                          << "\n==========================" << endl;

        if (verbose) cout << "\tVerify operator signatures." << endl;
        {
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            using namespace balb;

            operatorPtr operatorEq = operator==;
            operatorPtr operatorNe = operator!=;

            (void) operatorEq;  // quash potential compiler warnings
            (void) operatorNe;
        }

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose)
            cout << "\tCompare every value with every value.\n" << endl;
        {
            // Create first object
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE1   = DATA[ti].d_line;
                const char *const SPEC1   = DATA[ti].d_spec_p;

                if (veryVerbose) { T_ T_ P_(LINE1) P(SPEC1) }

                // Ensure an object compares correctly with itself (alias
                // test).
                {
                    bslma::TestAllocator scratch("scratch",
                                                 veryVeryVeryVerbose);

                    Obj mX(&scratch); const Obj& X = gg(&mX, SPEC1);

                    ASSERTV(LINE1, X,   X == X);
                    ASSERTV(LINE1, X, !(X != X));
                }

                for (size_t tj = 0; tj < NUM_DATA; ++tj) {
                    const int         LINE2   = DATA[tj].d_line;
                    const char *const SPEC2   = DATA[tj].d_spec_p;

                    if (veryVerbose) { T_ T_ P_(LINE2) P(SPEC2) }

                    const bool EXP = (LINE1 == LINE2);  // expected result

                    for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                        const char CONFIG = cfg;  // Determines 'Y's allocator.

                        // Create two distinct test allocators, 'oax' and
                        // 'oay'.

                        bslma::TestAllocator oax("oax", veryVeryVeryVerbose);
                        bslma::TestAllocator oay("oay", veryVeryVeryVerbose);

                        // Map allocators above to objects 'X' and 'Y' below.

                        bslma::TestAllocator& xa = oax;
                        bslma::TestAllocator& ya = 'a' == CONFIG ? oax : oay;

                        Obj mX(&xa); const Obj& X = gg(&mX, SPEC1);
                        Obj mY(&ya); const Obj& Y = gg(&mY, SPEC2);

                        if (veryVerbose) { T_ T_ P_(X) P(Y); }

                        // Verify value, commutativity, and no memory
                        // allocation.

                        bslma::TestAllocatorMonitor oaxm(&xa);
                        bslma::TestAllocatorMonitor oaym(&ya);

                        ASSERTV(LINE1, LINE2, CONFIG,  EXP == (X == Y));
                        ASSERTV(LINE1, LINE2, CONFIG,  EXP == (Y == X));

                        ASSERTV(LINE1, LINE2, CONFIG, !EXP == (X != Y));
                        ASSERTV(LINE1, LINE2, CONFIG, !EXP == (Y != X));

                        ASSERTV(LINE1, LINE2, CONFIG, oaxm.isTotalSame());
                        ASSERTV(LINE1, LINE2, CONFIG, oaym.isTotalSame());
                    }
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR (<<)
        //
        // Concerns:
        //: 1 First concern.
        //
        // Plan:
        //: 1 Plan.
        //
        // Testing:
        //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
        //   ostream& operator<<(ostream&, const balb::FCC&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR (<<)" << endl
                          << "==============================" << endl;

        // TBD

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each of the three basic accessors returns the value of the
        //:   corresponding attribute of the object.
        //:
        //: 2 Each basic accessor method is declared 'const'.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their attributes.
        //:
        //: 2 For each row 'R' in the table of P-1:  (C-1..2)
        //:
        //:   1 Use the 3-argument value constructor to create a 'const'
        //:     object, 'X', having the value from 'R'.
        //:
        //:   2 Verify that each basic accessor, invoked on 'X', returns the
        //:     expected value.  (C-1..2)
        //
        // Testing:
        //   const bsl::string& filePattern() const;
        //   bsls::TimeInterval maxFileAge() const;
        //   int minNumFiles() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING BASIC ACCESSORS"
                          << "\n=======================" << endl;

        if (verbose) cout << "\nUse a table of distinct object values."
                          << endl;

        static const struct {
            int                 d_line;
            const char         *d_filePattern_p;
            bsls::TimeInterval  d_interval;
            int                 d_minNumFiles;
        } DATA [] = {
        //LINE   FILE_PATTERN  INTERVAL                  MIN_NUM_FILES
        //----   ------------  ---------------------     -------------
        { L_,    "",           bsls::TimeInterval(),     0             },
        { L_,    "filepat*",   bsls::TimeInterval(),     0             },
        { L_,    "filepatt*",  bsls::TimeInterval(1),    0             },
        { L_,    "Filepatt*",  bsls::TimeInterval(2),    1             },
        };

        enum { DATA_LEN = sizeof(DATA) / sizeof(*DATA) };

        for (int ti = 0; ti < DATA_LEN; ++ti) {
            const int                 LINE          = DATA[ti].d_line;
            const char               *FILE_PATTERN  = DATA[ti].d_filePattern_p;
            const bsls::TimeInterval  INTERVAL      = DATA[ti].d_interval;
            const int                 MIN_NUM_FILES = DATA[ti].d_minNumFiles;

            const Obj X(FILE_PATTERN, INTERVAL, MIN_NUM_FILES);

            if (veryVerbose) {
                T_ P_(FILE_PATTERN) P_(INTERVAL) P(MIN_NUM_FILES)
            }

            // Verify the expected attributes values.

            ASSERTV(LINE, FILE_PATTERN  == X.filePattern());
            ASSERTV(LINE, INTERVAL      == X.maxFileAge());
            ASSERTV(LINE, MIN_NUM_FILES == X.minNumFiles());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMITIVE GENERATOR FUNCTIONS 'gg' AND 'ggg'
        //   Having demonstrated that our primary manipulators work as expected
        //   under normal conditions
        //
        // Concerns:
        //: 1 Valid generator syntax produces expected results.
        //:
        //: 2 Invalid syntax is detected and reported.
        //
        // Plan:
        //: 1 For each of an enumerated sequence of 'spec' values, ordered by
        //:   increasing 'spec' length:
        //:
        //:   1 Use the primitive generator function 'gg' to set the state of a
        //:     newly created object.
        //:
        //:   2 Verify that 'gg' returns a valid reference to the modified
        //:     argument object.
        //:
        //:   3 Use the basic accessors to verify that the value of the object
        //:     is as expected.  (C-1)
        //:
        //: 2 For each of an enumerated sequence of 'spec' values, ordered by
        //:   increasing 'spec' length, use the primitive generator function
        //:   'ggg' to set the state of a newly created object.
        //:
        //:   1 Verify that 'ggg' returns the expected value corresponding to
        //:     the location of the first invalid value of the 'spec'.  (C-2)
        //
        // Testing:
        //   FCC& gg(FCC *object, const char *spec);
        //   int ggg(FCC *object, const char *spec, int verbose = 1);
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING PRIMITIVE GENERATOR FUNCTIONS 'gg' AND 'ggg'"
                 << "\n===================================================="
                 << endl;

        bslma::TestAllocator oa("object", veryVeryVerbose);

        if (verbose) cout << "\tTesting generator on valid specs." << endl;
        {
            const int NUM_DATA                     = DEFAULT_NUM_DATA;
            const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const char *const SPEC   = DATA[ti].d_spec_p;

                Obj        mX(&oa);
                const Obj& X = gg(&mX, SPEC);  // original spec

                Obj        mY(&oa);
                const Obj& Y = gg(&mY, SPEC);  // extended spec

                if (veryVerbose) {
                    cout << "\t\tSpec = \"" << SPEC << "\"" << endl;
                    T_ T_ T_ P(X);
                    T_ T_ T_ P(Y);
                }
            }
        }

        if (verbose) cout << "\tTesting generator on other specs." << endl;
        {
            static const struct {
                int         d_line;    // source line number
                const char *d_spec_p;  // specification string
                int         d_index;   // offending character index
            } DATA[] = {
                //line  spec      index
                //----  --------  -----
                { L_,   "",        0,   },
                { L_,   "A",       0,   },
                { L_,   "B",       0,   },
                { L_,   "C",       0,   },
                { L_,   "D",       0,   },
                { L_,   "E",       0,   },
                { L_,   " ",       0,   },
                { L_,   ".",       0,   },

                { L_,   "AB",      0,   },
                { L_,   "AB",      0,   },
                { L_,   "Ae",      0,   },
                { L_,   ".~",      0,   },
                { L_,   "~!",      0,   },
                { L_,   "  ",      0,   },

                { L_,   "ABC",    -1,   },  // valid
                { L_,   "ZZZ",    -1,   },  // valid
                { L_,   " BC",     0,   },
                { L_,   "A C",     1,   },
                { L_,   "AB ",     2,   },
                { L_,   "?#:",     0,   },
                { L_,   "   ",     0,   },

                { L_,   "ABCDE",   0,   },
                { L_,   "aBCDE",   0,   },
                { L_,   "ABcDE",   0,   },
                { L_,   "ABCDe",   0,   },
                { L_,   "AbCdE",   0,   },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const int         INDEX  = DATA[ti].d_index;

                Obj mX(&oa);

                if (veryVerbose) {
                    cout << "\t\tSpec = \"" << SPEC << "\"" << endl;
                }

                int RESULT = ggg(&mX, SPEC, veryVerbose);

                ASSERTV(LINE, INDEX == RESULT);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS
        //
        // Concerns:
        //: 1 An object created with the default constructor (with or without a
        //:   supplied allocator) has the contractually specified default
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
        //
        // Plan:
        //: 1 Using a loop-based approach, default-construct three distinct
        //:   objects, in turn, but configured differently: (a) without passing
        //:   an allocator, (b) passing a null allocator address explicitly,
        //:   and (c) passing the address of a test allocator distinct from the
        //:   default.  For each of these three iterations:  (C-1..9)
        //:
        //:   1 Create three 'bslma::TestAllocator' objects, and install one as
        //:     the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Use the default constructor to dynamically create an object
        //:     'X', with its object allocator configured appropriately (see
        //:     P-2); use a distinct test allocator for the object's footprint.
        //:
        //:   3 Use the appropriate test allocators to verify that no memory is
        //:     allocated by the default constructor.  (C-9)
        //:
        //:   4 Use the individual (as yet unproven) salient attribute
        //:     accessors to verify the default-constructed value.  (C-1)
        //:
        //:   5 Manupulate the object to force memory allocation and verify
        //:     that memory comes from the correct allocator.  (C-6)
        //:
        //:   6 Verify that no temporary memory is allocated from the object
        //:     allocator.  (C-7)
        //:
        //:   7 Verify that all object memory is released when the object is
        //:     destroyed.  (C-8)
        //
        // Testing:
        //   FileCleanerConfiguration(bslma::Allocator *alloc = 0);
        //   ~FileCleanerConfiguration();
        //   void setFilePattern(const bslstl::StringRef& filePattern);
        //   void setMaxFileAge(const bsls::TimeInterval& maxAge);
        //   void setMinNumFiles(int minNumber);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING PRIMARY MANIPULATORS"
                          << "\n============================" << endl;

        if (verbose) {
            cout << "\tTesting with various allocator configurations.\n"
                 << endl;
        }

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
                  if (veryVerbose) {
                      cout << "\t\tTesting default constructor." << endl;
                  }
                  objPtr = new (fa) Obj();
                  objAllocatorPtr = &da;
              } break;
              case 'b': {
                  if (veryVerbose) {
                      cout << "\t\tTesting constructor with null allocator."
                           << endl;
                  }
                  objPtr = new (fa) Obj(0);
                  objAllocatorPtr = &da;
              } break;
              case 'c': {
                  if (veryVerbose) {
                      cout << "\t\tTesting constructor with an allocator."
                           << endl;
                  }
                  objPtr = new (fa) Obj(&sa);
                  objAllocatorPtr = &sa;
              } break;
              default: {
                  ASSERTV(CONFIG, !"Bad allocator config.");
              } break;
            }

            Obj&                  mX  = *objPtr;
            const Obj&            X   = mX;
            bslma::TestAllocator& oa  = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            // Verify object value.

            ASSERTV(CONFIG, "" == X.filePattern());
            ASSERTV(CONFIG, bsls::TimeInterval() == X.maxFileAge());
            ASSERTV(CONFIG, 0 == X.minNumFiles());

            // Verify no allocation from the object/non-object allocators.

            ASSERTV(CONFIG, oa.numBlocksTotal(),  0 ==  oa.numBlocksTotal());
            ASSERTV(CONFIG, noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            ASSERTV(CONFIG, &oa == X.filePattern().get_allocator());

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            ASSERTV(CONFIG, da.numBlocksInUse(), 0 == da.numBlocksInUse());
            ASSERTV(CONFIG, fa.numBlocksInUse(), 0 == fa.numBlocksInUse());
            ASSERTV(CONFIG, sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
        }

        if (verbose) cout << "\tTesting primary manipulators." << endl;
        {
            static const struct {
                int                       d_line;          // source line
                const char               *d_filePattern_p; // file pattern
                const bsls::TimeInterval  d_maxAge;        // file age
                int                       d_minNumber;     // min files
            } DATA[] = {
                //line  filePattern  maxAge                      minNumber
                //----  -----------  --------------------------  ---------
                { L_,   "",          bsls::TimeInterval(),       0           },
                { L_,   "file1",     bsls::TimeInterval(),       0           },
                { L_,   "file1",     bsls::TimeInterval(10, 0),  0           },
                { L_,   "file1",     bsls::TimeInterval(10, 0),  4           },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int                 LINE      = DATA[ti].d_line;
                const char *const         PATTERN   = DATA[ti].d_filePattern_p;
                const bsls::TimeInterval& MAXAGE    = DATA[ti].d_maxAge;
                int                       MINNUMBER = DATA[ti].d_minNumber;

                Obj mX;  const Obj& X = mX;

                if (veryVerbose) {
                    T_ P_(LINE) P_(PATTERN) P_(MAXAGE) P(MINNUMBER);
                }

                // Verify object value.

                ASSERTV(LINE, "" == X.filePattern());
                ASSERTV(LINE, bsls::TimeInterval() == X.maxFileAge());
                ASSERTV(LINE, 0 == X.minNumFiles());

                mX.setFilePattern(PATTERN);
                ASSERTV(LINE, X.filePattern(), PATTERN == X.filePattern());

                mX.setMaxFileAge(MAXAGE);
                ASSERTV(LINE, X.maxFileAge(), MAXAGE == X.maxFileAge());

                mX.setMinNumFiles(MINNUMBER);
                ASSERTV(LINE, X.minNumFiles(),
                        MINNUMBER == X.minNumFiles());
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
        //: 1 Run each method with arbitrary inputs and verify the behavior is
        //:   as expected.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        Obj config("/var/log/myApp/log*", bsls::TimeInterval(60), 4);

        ASSERT("/var/log/myApp/log*" == config.filePattern());
        ASSERT(bsls::TimeInterval(60) == config.maxFileAge());
        ASSERT(4 == config.minNumFiles());

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.
    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
