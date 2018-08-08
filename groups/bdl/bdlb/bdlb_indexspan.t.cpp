// bdlb_indexspan.t.cpp                                               -*-C++-*-
#include <bdlb_indexspan.h>

#include <bslma_allocator.h>

#include <bslim_testutil.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_set.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// 'bdlb::IndexSpan' provides a value-semantic, unconstrained attribute type
// for representing a position and a length.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] IndexSpan();
// [ 2] IndexSpan(size_type position, size_type length);
// [ 5] IndexSpan(const IndexSpan& original);
//
// ACCESSORS
// [ 2] IndexSpan::size_type length() const;
// [ 2] IndexSpan::size_type position() const;
// [ 7] ostream& print(ostream& stream, int level, int sp) const;
//
// MANIPULATORS
// [ 6] IndexSpan& operator=(const IndexSpan& rhs);
//
// FREE OPERATORS
// [ 4] bool operator==(const IndexSpan& lhs, const IndexSpan& rhs);
// [ 4] bool operator!=(const IndexSpan& lhs, const IndexSpan& rhs);
// [ 3] ostream& operator<<(ostream& stream, const IndexSpan& object);
// [ 9] bsl::hash<IndexSpan>
// [ 9] bslh::Hash<>
//
// TRAITS
// [ 8] bslmf::IsBitwiseMoveable
// [ 8] bsl::is_trivially_copyable
// [ 8] bdlb::HasPrintMethod
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [10] USAGE EXAMPLE

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

typedef bdlb::IndexSpan Obj;

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: A Parsed Path
/// - - - - - - - - - - - -
// Suppose that we work with file system paths and we frequently need the path,
// the base name, the extension, and the filename, as well as the full path
// itself.  Therefore we want to create a class that stores the full path and a
// way to quickly access the individual parts.  (To keep things simple we will
// make the example code work on POSIX-style paths so we do not need to deal
// with the difference in path separators.)
//
// If we try the naive solution -- storing a 'bsl::string' of the full path and
// 'bslstl::StringRef' for path, basename and extension -- the class
// 'bslstl::StringRef' members will reference back into memory owned by the
// object, and the result is the compiler-generated (and naively written) move
// and copy operations will be broken.  In addition, explicitly implementing
// move and copy operations is quite difficult and error prone.
//
// The simplest (most readable) solution to the problem is to store a position
// and a length for the path, basename, and extension (rather than a
// 'bslstl::StringRef') because that representation is independent of the
// location of the memory for the string.  'IndexSpan' provides such a
// representation, and allow us to implement our class using a simple copy and
// move operations (or in other contexts, compiler supplied operations).
//
// First, we define the members and interface of the class.
//..
    class ParsedPath {
      private:
        // DATA
        bsl::string     d_full;  // The full path
        bdlb::IndexSpan d_path;  // The path part if present, otherwise empty
        bdlb::IndexSpan d_base;  // The base name if present, otherwise empty
        bdlb::IndexSpan d_ext;   // The extension if present, otherwise empty

      public:
        // CREATE
        explicit ParsedPath(const bslstl::StringRef&  full,
                            bslma::Allocator         *basicAllocator = 0);
            // Create a new 'ParsedPath' object by storing and parsing the
            // specified 'full' path.  Use the optionally specified
            // 'basicAllocator' to allocate memory.  If 'basicAllocator' is 0,
            // use the currently installed default allocator.

        ParsedPath(const ParsedPath& original,
                   bslma::Allocator *basicAllocator = 0);
            // Create a new 'ParsedPath' object that stores the same parsed
            // path as the specified 'original'.  Use the optionally specified
            // 'basicAllocator' to allocate memory.  Use the currently
            // installed default allocator if 'basicAllocator' is zero.

        // ACCESSORS
        bslstl::StringRef base() const;
            // Return a string reference to the base name part.  Note that it
            // may be an empty string reference.

        bslstl::StringRef ext()  const;
            // Return a string reference to the extension part.  Note that it
            // may be an empty string reference.

        const bsl::string& full() const;
            // Return a const reference to the full path parsed by this object.

        bslstl::StringRef path() const;
            // Return a string reference to the path part.  Note that it may be
            // an empty string reference.
    };
//..
//  Next, to make the parsing code short and readable, we implement a helper
//  function to create 'IndexSpan' objects from two positions.  (In practice we
//  would use the higher level utility function
//  'IndexSpanStringUtil::createFromPosition')
//..
    bdlb::IndexSpan createFromPositions(bdlb::IndexSpan::size_type startPos,
                                        bdlb::IndexSpan::size_type endPos)
        // Return an 'IndexSpan' describing the specified '[startPos, endPos)'
        // positions.
    {
        return bdlb::IndexSpan(startPos, endPos - startPos);
    }
//..
//  Then, we implement the parsing constructor using the 'create' function.
//..
    // CREATORS
    ParsedPath::ParsedPath(const bslstl::StringRef&  full,
                           bslma::Allocator         *basicAllocator)
    : d_full(full, basicAllocator)
    {
        typedef bsl::string::size_type Size;

        static const Size npos = bsl::string::npos;

        const Size slashPos = d_full.rfind('/');
        const Size dotPos   = d_full.rfind('.');

        const bool dotIsPresent   = (dotPos   != npos);
        const bool slashIsPresent = (slashPos != npos);

        const bool dotIsInPath = slashIsPresent && (dotPos < slashPos);

        const bool isDotFile = dotIsPresent &&
                                 dotPos == (slashIsPresent ? slashPos + 1 : 0);

        const bool hasExtension = dotIsPresent && !dotIsInPath && !isDotFile;
        const bool hasPath      = slashIsPresent;

        if (hasPath) {
            d_path = createFromPositions(0, slashPos + 1);
        }

        d_base = createFromPositions(slashPos + 1,
                                     hasExtension ? dotPos : full.length());

        if (hasExtension) {
            d_ext = createFromPositions(dotPos + 1, full.length());
        }
    }
//..
// Next, we implement the (now) simple copy constructor:
//..
    ParsedPath::ParsedPath(const ParsedPath& original,
                           bslma::Allocator *basicAllocator)
    : d_full(original.d_full, basicAllocator)
    , d_path(original.d_path)
    , d_base(original.d_base)
    , d_ext(original.d_ext)
    {
    }
//..
//  Next, to make the accessors simple (and readable), we implement a helper
//  function that creates a 'StringRef' from a 'StringRef' and an 'IndexSpan'.
//  (Don't do this in real code, use 'IndexSpanStringUtil::bind' that is
//  levelized above this component - so we cannot use it here.)
//..
    bslstl::StringRef bindSpan(const bslstl::StringRef& full,
                               const bdlb::IndexSpan&   part)
        // Return a string reference to the substring of the specified 'full'
        // thing defined by the specified 'part'.
    {
        BSLS_ASSERT(part.position() <= full.length());
        BSLS_ASSERT(part.position() + part.length() <= full.length());

        return bslstl::StringRef(full.data() + part.position(), part.length());
    }
//..
//  Then we implement the accessors:
//..
    // ACCESSORS
    bslstl::StringRef ParsedPath::base() const
    {
        return bindSpan(d_full, d_base);
    }

    bslstl::StringRef ParsedPath::ext() const
    {
        return bindSpan(d_full, d_ext);
    }

    const bsl::string& ParsedPath::full() const
    {
        return d_full;
    }

    bslstl::StringRef ParsedPath::path() const
    {
        return bindSpan(d_full, d_path);
    }
//..
//  See top test case for the verification steps.

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
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, and replace 'assert' with
        //:   'ASSERT'.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) cout << "\nUSAGE EXAMPLE"
                             "\n=============\n";

// See the rest of the code just before the 'main' function.
//
// Finally, we verify that the resulting class is copied properly.  We do that
// by determining that the original and the copy object has equal but distinct
// strings and that the (other) accessors return references into those strings.
//..
    ParsedPath aPath("path/path/basename.extension");
    ParsedPath theCopy(aPath);

    ASSERT(aPath.full()        == theCopy.full());
    ASSERT(aPath.full().data() != theCopy.full().data());

    ASSERT(aPath.path()        == theCopy.path());
    ASSERT(aPath.path().data() != theCopy.path().data());
    ASSERT(aPath.path().data() >= aPath.full().data());
    ASSERT(aPath.path().data() + aPath.path().length() <=
                                  aPath.full().data() + aPath.full().length());

    ASSERT(aPath.base()        == theCopy.base());
    ASSERT(aPath.base().data() != theCopy.base().data());
    ASSERT(aPath.base().data() >= aPath.full().data());
    ASSERT(aPath.base().data() + aPath.base().length() <=
                                  aPath.full().data() + aPath.full().length());

    ASSERT(aPath.ext()        == theCopy.ext());
    ASSERT(aPath.ext().data() != theCopy.ext().data());
    ASSERT(aPath.ext().data() >= aPath.full().data());
    ASSERT(aPath.ext().data() + aPath.ext().length() <=
                                  aPath.full().data() + aPath.full().length());
//..
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING HASH FUNCTION
        //
        // Concerns:
        //: 1 An index span object can be hashed by instances of
        //:   'bsl::hash<bdlb::IndexSpan>'.
        //:
        //: 2 A small sample of different index span objects produce different
        //:   hashes.
        //:
        //: 3 Invoking 'bsl::hash<bdlb::IndexSpan>' is identical to invoking
        //:   'bslh::DefaultHashAlgorithm' on the underlying data of the index
        //:   span object.
        //
        // Plan:
        //: 1 Hash some different index span objects and verify that the result
        //:   of using 'bsl::hash<bdlb::IndexSpan>' is identical to invoking
        //:   'bslh::DefaultHashAlgorithm' on the underlying attributes of the
        //:   index span object.
        //:
        //: 2 Hash a number of different index span objects and verify that
        //:   they produce distinct hashes.
        //
        // Testing:
        //   bsl::hash<IndexSpan>
        //   bslh::Hash<>
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING HASH FUNCTION"
                             "\n=====================\n";

        static struct TestData {
            long long   d_line;
            bsl::size_t d_pos;
            bsl::size_t d_len;
        } k_DATA[] =
        {
            { L_,  0,  0 },
            { L_,  0,  1 },
            { L_,  1,  0 },
            { L_,  1,  1 },
            { L_, 32, 32 },
            { L_, 32, 42 },
            { L_, 42, 32 },
            { L_, 42, 42 },
        };

        const bsl::size_t k_NUM_TESTS = sizeof(k_DATA) / sizeof(k_DATA[0]);

        bsl::hash<Obj>        bslHashFunction;
        bsl::set<bsl::size_t> hashResults;

        for (bsl::size_t i = 0; i < k_NUM_TESTS; ++i) {
            Obj          span(k_DATA[i].d_pos, k_DATA[i].d_len);
            bslh::Hash<> defaultHashAlgorithm;
            ASSERT(bslHashFunction(span) == defaultHashAlgorithm(span));
            ASSERT(hashResults.insert(bslHashFunction(span)).second);
        }
        ASSERT(hashResults.size() == k_NUM_TESTS);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING TRAITS
        //
        // Concerns:
        //: 1 bslmf::IsBitwiseEqualityComparable is 'true' for 'IndexSpan'.
        //: 2 bsl::is_trivially_copyable is 'true' for 'IndexSpan'.
        //: 3 bdlb::HasPrintMethod is 'true' for 'IndexSpan'.
        //
        // Plan:
        //: 1 Assert each trait.
        //
        // Testing:
        //   bslmf::IsBitwiseMoveable
        //   bsl::is_trivially_copyable
        //   bdlb::HasPrintMethod
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING TRAITS"
                             "\n==============\n";

        ASSERT(bslmf::IsBitwiseMoveable<Obj>::value);
        ASSERT(bsl::is_trivially_copyable<Obj>::value);
        ASSERT(bdlb::HasPrintMethod<Obj>::value);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING THE PRINT METHOD
        //
        // Concerns:
        //: 1 The print method formats the attributes of the object directly
        //:   from the underlying state information according to supplied
        //:   arguments.
        //
        // Plan:
        //: 1 For each of an enumerated set of object, 'level', and
        //:   'spacesPerLevel' values, use 'ostringstream' to 'print' that
        //:   object's value, using the tabulated parameters.  Verify that the
        //:   contents of the 'ostringstream buffer matches the literal
        //:   expected output.
        //
        // Testing:
        //   ostream& print(ostream& stream, int level, int sp) const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING THE PRINT METHOD"
                             "\n========================\n";

        {
            static const struct {
                int         d_lineNum;  // source line number
                bsl::size_t d_pos;      // position attribute
                bsl::size_t d_len;      // length attribute
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                const char *d_exp_p;    // expected output
            } k_DATA[] = {
//                line  pos   len     indent spaces
//                ----  ----  ------  ------ ------
//                expected output
//                -------------------------------------------------------------
                { L_,   0,    0,        0,    0,
                  "[\nposition = 0\nlength = 0\n]\n"                         },
                { L_,   0,    1,        0,    0,
                  "[\nposition = 0\nlength = 1\n]\n"                         },
                { L_,   2,    3,        0,    2,
                  "[\n  position = 2\n  length = 3\n]\n"                     },
                { L_,   4,    5,        1,    1,
                  " [\n  position = 4\n  length = 5\n ]\n"                   },
                { L_,   6,    7,        1,    2,
                  "  [\n    position = 6\n    length = 7\n  ]\n"             },
                { L_,   8,    9,       -1,    2,
                  "[\n    position = 8\n    length = 9\n  ]\n"               },
                { L_,  10,   11,       -2,    2,
                  "[\n      position = 10\n      length = 11\n    ]\n"       },
                { L_,  12,   13,        1,   -2,
                  "  [ position = 12 length = 13 ]"                          },
                { L_,  14,   15,        2,   -2,
                  "    [ position = 14 length = 15 ]"                        },
                { L_,  16,   17,       -1,   -3,
                  "[ position = 16 length = 17 ]"                            },
                { L_,  18,   19,       -2,   -3,
                  "[ position = 18 length = 19 ]"                            }
            };

            static const bsl::size_t k_NUM_TESTS =
                                                sizeof k_DATA / sizeof *k_DATA;

            for (bsl::size_t i = 0; i < k_NUM_TESTS;  ++i) {
                const int         k_LINE   = k_DATA[i].d_lineNum;
                const bsl::size_t k_POS    = k_DATA[i].d_pos;
                const bsl::size_t k_LEN    = k_DATA[i].d_len;
                const int         k_IND    = k_DATA[i].d_indent;
                const int         k_SPC    = k_DATA[i].d_spaces;
                const char *const k_EXP    = k_DATA[i].d_exp_p;

                Obj        mX(k_POS, k_LEN);
                const Obj& X = mX;
                const Obj  Y = X;

                if (veryVerbose) {
                    P_(k_LINE) P_(k_POS) P_(k_LEN) P_(k_IND) P_(k_SPC) P(k_EXP)
                }

                bsl::ostringstream out;
                X.print(out, k_IND, k_SPC);
                if (veryVerbose) { P(out.str()) }

                const bsl::string& printed = out.str();

                ASSERTV(k_LINE, k_EXP, printed, k_EXP == printed);
                ASSERTV(k_LINE, X, Y, X == Y);
            }
        }
      } break;
      case 6: {
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
        //:   varied differences.  To address concerns 1 - 3, construct tests
        //:   u = v for all (u, v) in S X S.  Using canonical controls UU and
        //:   VV, assert before the assignment that UU == u, VV == v, and
        //:   v == u if and only if and only if VV == UU.  After the
        //:   assignment, assert that VV == u, VV == v, and, for grins, that
        //:   v == u.  Let v go out of scope and confirm that VV == u.
        //:
        //:  2 As a separate exercise, we address 4 by constructing tests
        //:    y = y for all y in S.  Using a canonical control X, we will
        //:    verify that X == y before and after the assignment.
        //
        // Testing:
        //   IndexSpan& operator=(const IndexSpan& rhs);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING ASSIGNMENT OPERATOR"
                             "\n===========================\n";

        if (verbose) cout <<
            "\nAssign cross product of values with varied representations.\n";
        {
            static struct TestData {
                long long   d_line;
                bsl::size_t d_pos;
                bsl::size_t d_len;
            } k_DATA[] =
            {
                { L_,  0,  0 },
                { L_,  0,  1 },
                { L_,  1,  0 },
                { L_,  1,  1 },
                { L_, 32, 32 },
                { L_, 32, 42 },
                { L_, 42, 32 },
                { L_, 42, 42 },
            };

            const bsl::size_t k_NUM_TESTS = sizeof(k_DATA) / sizeof(k_DATA[0]);

            for (bsl::size_t i = 0; i < k_NUM_TESTS; ++i) {
                const TestData    k_TEST1 = k_DATA[i];
                const long long   k_LINE1 = k_TEST1.d_line;
                const bsl::size_t k_POS1  = k_TEST1.d_pos;
                const bsl::size_t k_LEN1  = k_TEST1.d_len;

                if (veryVerbose) { P_(k_LINE1) P_(k_POS1) P(k_LEN1) }

                const Obj UU(k_POS1, k_LEN1);               // control
                for (bsl::size_t j = 0; j < k_NUM_TESTS; ++j) {
                    const TestData    k_TEST2 = k_DATA[j];
                    const long long   k_LINE2 = k_TEST2.d_line;
                    const bsl::size_t k_POS2  = k_TEST2.d_pos;
                    const bsl::size_t k_LEN2  = k_TEST2.d_len;

                    if (veryVerbose) { P_(k_LINE2) P_(k_POS2) P(k_LEN2) }

                    const Obj VV(k_POS2, k_LEN2);           // control

                    const bool k_SAME = (i == j);

                    Obj        mU(k_POS1, k_LEN1);
                    const Obj& U = mU;

                    {
                        Obj        mV(k_POS2, k_LEN2);
                        const Obj& V = mV;

                        ASSERTV(k_LINE1, k_LINE2, UU == U);
                        ASSERTV(k_LINE1, k_LINE2, VV == V);
                        ASSERTV(k_LINE1, k_LINE2, k_SAME == (V == U));

                        if (veryVeryVerbose) { P_(U) P(V) }
                        mU = V; // test assignment here

                        ASSERTV(k_LINE1, k_LINE2, VV == U);
                        ASSERTV(k_LINE1, k_LINE2, VV == V);
                        ASSERTV(k_LINE1, k_LINE2,  V == U);
                    }

                    // 'mV' (and therefore 'V') now out of scope
                    ASSERTV(k_LINE1, k_LINE2, VV == U);
                }
            }
        }

        if (verbose) cout << "\nTesting self assignment (Aliasing).\n";
        {
            static struct TestData {
                long long   d_line;
                bsl::size_t d_pos;
                bsl::size_t d_len;
            } k_DATA[] =
            {
                { L_,  0,  0 },
                { L_,  0,  1 },
                { L_,  1,  0 },
                { L_,  1,  1 },
                { L_, 32, 32 },
                { L_, 32, 42 },
                { L_, 42, 32 },
                { L_, 42, 42 },
            };

            const bsl::size_t k_NUM_TESTS = sizeof(k_DATA) / sizeof(k_DATA[0]);

            for (bsl::size_t i = 0; i < k_NUM_TESTS; ++i) {
                const TestData    k_TEST = k_DATA[i];
                const long long   k_LINE = k_TEST.d_line;
                const bsl::size_t k_POS  = k_TEST.d_pos;
                const bsl::size_t k_LEN  = k_TEST.d_len;

                if (veryVerbose) { P_(k_LINE) P_(k_POS) P(k_LEN) }

                const Obj  X(k_POS, k_LEN);               // control

                Obj        mY(k_POS, k_LEN);
                const Obj& Y = mY;

                ASSERTV(k_LINE, Y == Y);
                ASSERTV(k_LINE, X == Y);

                mY = Y; // test assignment here

                ASSERTV(k_LINE, Y == Y);
                ASSERTV(k_LINE, X == Y);
            }
        }
      } break;
      case 5: {
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
        //:   no effect on the copy-constructed object.
        //
        // Plan:
        //: 1 To address concerns 1 and 2, specify a set S of object values
        //:   with substantial and varied differences.  For each value in S,
        //:   initialize objects w and x, copy construct y from x and use
        //:   'operator==' to verify that both x and y subsequently have the
        //:   same value as w.  Let x go out of scope and again verify that
        //:   w == y.
        //
        // Testing:
        //   IndexSpan(const IndexSpan& original);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING COPY CONSTRUCTOR"
                             "\n========================\n";

        if (verbose) cout <<
                      "\nCopy construct values with varied representations.\n";
        {
            static struct TestData {
                long long   d_line;
                bsl::size_t d_pos;
                bsl::size_t d_len;
            } k_DATA[] =
            {
                { L_,  0,  0 },
                { L_,  0,  1 },
                { L_,  1,  0 },
                { L_,  1,  1 },
                { L_, 32, 32 },
                { L_, 32, 42 },
                { L_, 42, 32 },
                { L_, 42, 42 },
            };

            const bsl::size_t k_NUM_TESTS = sizeof(k_DATA) / sizeof(k_DATA[0]);

            for (bsl::size_t i = 0; i < k_NUM_TESTS; ++i) {
                const TestData    k_TEST = k_DATA[i];
                const long long   k_LINE = k_TEST.d_line;
                const bsl::size_t k_POS = k_TEST.d_pos;
                const bsl::size_t k_LEN = k_TEST.d_len;

                if (veryVerbose) { P_(k_LINE) P_(k_POS) P(k_LEN) }

                // Create control object w.
                Obj         mW(k_POS, k_LEN);
                const Obj&  W = mW;

                Obj        *mX = new Obj(k_POS, k_LEN);
                const Obj&  X = *mX;
                ASSERTV(k_LINE, X == W);

                Obj        mY(X);
                const Obj &Y = mY;
                ASSERTV(k_LINE, X == W);
                ASSERTV(k_LINE, Y == W);

                if (veryVerbose) { T_ P_(W) P_(X) P(Y) }

                delete mX;
                ASSERTV(k_LINE, Y == W);
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        //
        // Concerns:
        //: 1 Since 'operators==' is implemented in terms of basic accessors,
        //:   it is sufficient to verify only that a difference in value of any
        //:   one basic accessor for any two given objects implies inequality.
        //
        // Plan:
        //: 1 First specify a set S of unique object values having various
        //:   differences.  Verify the correctness of 'operator==' and
        //:   'operator!=' using all elements (u, v) of the cross product
        //:   S X S.
        //
        // Testing:
        //   bool operator==(const IndexSpan& lhs, const IndexSpan& rhs);
        //   bool operator!=(const IndexSpan& lhs, const IndexSpan& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING EQUALITY OPERATORS"
                             "\n==========================\n";

        if (verbose) cout <<
                    "\nCompare each pair of similar values (u, v) in S X S.\n";
        {
            static struct TestData {
                long long   d_line;
                bsl::size_t d_pos;
                bsl::size_t d_len;
            } k_DATA[] =
            {
                { L_,  0,  0 },
                { L_,  0,  1 },
                { L_,  1,  0 },
                { L_,  1,  1 },
                { L_, 32, 32 },
                { L_, 32, 42 },
                { L_, 42, 32 },
                { L_, 42, 42 },
            };

            const bsl::size_t k_NUM_TESTS = sizeof(k_DATA) / sizeof(k_DATA[0]);

            for (bsl::size_t i = 0; i < k_NUM_TESTS; ++i) {
                const TestData    k_TEST1 = k_DATA[i];
                const long long   k_LINE1 = k_TEST1.d_line;
                const bsl::size_t k_POS1  = k_TEST1.d_pos;
                const bsl::size_t k_LEN1  = k_TEST1.d_len;

                if (veryVerbose) { P_(k_LINE1) P_(k_POS1) P(k_LEN1) }

                Obj        mX(k_POS1, k_LEN1);
                const Obj& X = mX;

                for (bsl::size_t j = 0; j < k_NUM_TESTS; ++j) {
                    const TestData    k_TEST2 = k_DATA[j];
                    const long long   k_LINE2 = k_TEST2.d_line;
                    const bsl::size_t k_POS2  = k_TEST2.d_pos;
                    const bsl::size_t k_LEN2  = k_TEST2.d_len;

                    if (veryVerbose) { P_(k_LINE2) P_(k_POS2) P(k_LEN2) }

                    Obj        mY(k_POS2, k_LEN2);
                    const Obj& Y = mY;

                    const bool k_EXP = (k_POS1 == k_POS2 && k_LEN1 == k_LEN2);

                    ASSERTV(i, j,  k_EXP == (X == Y));
                    ASSERTV(i, j,  1     == (X == X));
                    ASSERTV(i, j,  1     == (Y == Y));
                    ASSERTV(i, j, !k_EXP == (X != Y));
                    ASSERTV(i, j,  0     == (X != X));
                    ASSERTV(i, j,  0     == (Y != Y));
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        //
        // Concerns:
        //: 1 Since the output operator is layered on basic accessors, it is
        //:   sufficient to test only the output text.
        //
        // Plan:
        //: 1 For each of a small representative set of object values, use
        //:   'ostringstream' to write that object's value.  Verify that the
        //:   contents of the 'ostringstream' buffer is the same as the literal
        //:   expected output text .
        //
        // Testing:
        //   ostream& operator<<(ostream& stream, const IndexSpan& object);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING OUTPUT (<<) OPERATOR"
                             "\n============================\n";

        if (verbose) cout << "\nTesting 'operator<<' (ostream).\n";

        {
            static const struct {
                int         d_lineNum;  // source line number
                bsl::size_t d_pos;      // position attribute
                bsl::size_t d_len;      // length attribute
                const char *d_exp_p;    // expected output
            } k_DATA[] = {
                // line  pos  len   expected output
                // ----  ---  ----  -------------------------------
                {  L_,    0,   0,   "[ position = 0 length = 0 ]"   },
                {  L_,    0,   1,   "[ position = 0 length = 1 ]"   },
                {  L_,    1,   0,   "[ position = 1 length = 0 ]"   },
                {  L_,    1,   1,   "[ position = 1 length = 1 ]"   },
                {  L_,   32,  32,   "[ position = 32 length = 32 ]" },
                {  L_,   32,  42,   "[ position = 32 length = 42 ]" },
                {  L_,   42,  32,   "[ position = 42 length = 32 ]" },
                {  L_,   42,  42,   "[ position = 42 length = 42 ]" }
            };
            const bsl::size_t k_NUM_TESTS = sizeof k_DATA / sizeof *k_DATA;

            for (bsl::size_t i = 0; i < k_NUM_TESTS;  ++i) {
                const int         k_LINE = k_DATA[i].d_lineNum;
                const bsl::size_t k_POS  = k_DATA[i].d_pos;
                const bsl::size_t k_LEN  = k_DATA[i].d_len;
                const char *const k_EXP  = k_DATA[i].d_exp_p;

                if (veryVerbose) { P_(k_LINE) P_(k_POS) P_(k_LEN) P(k_EXP) }

                Obj        mX(k_POS, k_LEN);
                const Obj& X = mX;

                bsl::ostringstream out;
                out << X;

                ASSERTV(k_LINE, k_EXP, out.str(), k_EXP == out.str());
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CONSTRUCTORS / ACCESSORS TEST
        //
        // Concerns:
        //: 1 Default-constructed 'IndexSpan' attributes are both initialized
        //: to 0.
        //:
        //: 2 A 'IndexSpan' attributes are properly initialized by the
        //:   value-constructor.
        //:
        //: 3 'IndexSpan' accessors properly return the attribute values.
        //:
        //: 4 Accessors operate on a 'const' object.
        //
        // Plan:
        //: 1 Default construct an object, verify its attributes.
        //:
        //: 2 Value construct objects from a table, verify their attributes.
        //
        // Testing:
        //   IndexSpan();
        //   IndexSpan(size_type position, size_type length);
        //   IndexSpan::size_type length() const;
        //   IndexSpan::size_type position() const;
        // --------------------------------------------------------------------
        if (verbose) cout << "\nCONSTRUCTORS / ACCESSORS TEST"
                             "\n=============================\n";

        const Obj D;
        ASSERTV(D.position(), 0 == D.position());
        ASSERTV(D.length(),   0 == D.length());

        static struct TestData {
            long long   d_line;
            bsl::size_t d_pos;
            bsl::size_t d_len;
        } k_DATA[] =
        {
            { L_,  0,  0 },
            { L_,  0,  1 },
            { L_,  1,  0 },
            { L_,  1,  1 },
            { L_, 32, 32 },
            { L_, 32, 42 },
            { L_, 42, 32 },
            { L_, 42, 42 },
        };

        const bsl::size_t k_NUM_TESTS = sizeof(k_DATA) / sizeof(k_DATA[0]);

        for (bsl::size_t i = 0; i < k_NUM_TESTS; ++i) {
            const TestData    k_TEST = k_DATA[i];
            const long long   k_LINE = k_TEST.d_line;
            const bsl::size_t k_POS = k_TEST.d_pos;
            const bsl::size_t k_LEN = k_TEST.d_len;

            if (veryVerbose) { P_(k_LINE) P_(k_POS) P(k_LEN) }

            const Obj X(k_POS, k_LEN);

            ASSERTV(k_LINE, k_POS, X.position(), k_POS == X.position());
            ASSERTV(k_LINE, k_LEN, X.length(),   k_LEN == X.length());
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
        //: 1 Create a few objects, verify their attribute values.  Do some
        //:   copy construction and assignment, verify the resulting values.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------
        if (verbose) cout << "\nBREATHING TEST"
                             "\n==============\n";

        Obj X;
        ASSERT(0 == X.position());
        ASSERT(0 == X.length());

        Obj Y(42, 12);
        ASSERT(42 == Y.position());
        ASSERT(12 == Y.length());

        Obj Z(Y);
        ASSERT(42 == Z.position());
        ASSERT(12 == Z.length());

        X = Z;
        ASSERT(42 == X.position());
        ASSERT(12 == X.length());
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND.\n";
        testStatus = -1;
      }
    }
    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << ".\n";
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
