// bdlc_compactedarray.t.cpp                                          -*-C++-*-
#include <bdlc_compactedarray.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_asserttest.h>

#include <bsl_cctype.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iosfwd.h>
#include <bsl_iostream.h>
#include <bsl_set.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_unordered_map.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a (value-semantic) container class, and
// a 'const_iterator' for that container.  The primary manipulators are
// therefore the basic methods for adding ('append') elements and emptying the
// array ('pop_back' and 'removeAll').  The basic accessors are those providing
// access to the data placed within the array (operator[]) and those allowing
// access to primary attributes of the array ('allocator', 'capacity',
// 'isEmpty', 'length', 'uniqueElement', and 'uniqueLength').  After the
// standard 10 cases, the unique functionality of the container is tested.
// Effort is made to use only the primary manipulators and accessors whenever
// possible, thus making every test case independent.
//
// Primary Manipulators
//: o 'append'
//: o 'pop_back'
//: o 'removeAll'
//
// Basic Accessors
//: o 'operator[]'
//: o 'allocator'
//: o 'capacity'
//: o 'length'
//: o 'uniqueElement'
//: o 'uniqueLength'
//
// Certain standard value-semantic-type test cases are omitted:
//: o [ 8] -- GENERATOR FUNCTION
//: o [10] BDEX
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o Any allocated memory is always from the object allocator.
//: o An object's value is independent of the allocator used to supply memory.
//: o Injected exceptions are safely propagated during memory allocation.
//: o Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//: o All explicit memory allocations are presumed to use the global, default,
//:   or object allocator.
//: o ACCESSOR methods are 'const' thread-safe.
// ----------------------------------------------------------------------------
// [20] CompactedArray::const_iterator();
// [20] CompactedArray::const_iterator(const CACI& original);
// [20] ~CompactedArray::const_iterator();
// [20] CACI& operator=(const CACI& rhs);
// [20] CACI& CACI::operator++();
// [20] CACI& CACI::operator--();
// [20] const TYPE& CACI::operator*() const;
// [20] const TYPE *CACI::operator->() const;
// [20] const TYPE& CACI::operator[](bsl::ptrdiff_t offset) const;
// [20] bool operator==(const CACI& lhs, const PIAI& rhs);
// [20] bool operator!=(const CACI& lhs, const PIAI& rhs);
// [22] CACI& CACI::operator+=(bsl::ptrdiff_t offset);
// [22] CACI& CACI::operator-=(bsl::ptrdiff_t offset);
// [22] CACI CACI::operator++(CACI& iter, int);
// [22] CACI CACI::operator--(CACI& iter, int);
// [22] CACI CACI::operator+(const CACI& iterator, bsl::ptrdiff_t offset);
// [22] CACI CACI::operator+(bsl::ptrdiff_t offset, const CACI& iterator);
// [22] CACI CACI::operator-(const CACI& iterator, bsl::ptrdiff_t offset);
// [22] bsl::ptrdiff_t operator-(const CACI& lhs, const CACI& rhs);
// [21] bool operator<(const CACI& lhs, const CACI& rhs);
// [21] bool operator<=(const CACI& lhs, const CACI& rhs);
// [21] bool operator>(const CACI& lhs, const CACI& rhs);
// [21] bool operator>=(const CACI& lhs, const CACI& rhs);
// [ 2] CompactedArray(bslma::Allocator *basicAllocator = 0);
// [18] CompactedArray(numElements, value, basicAllocator);
// [ 7] CompactedArray(const CompactedArray& o, *bA = 0);
// [ 2] ~CompactedArray();
// [ 9] CompactedArray& operator=(const CompactedArray& rhs);
// [ 2] void append(const TYPE& value);
// [12] void append(const CompactedArray& srcArray);
// [12] void append(const CompactedArray& srcArray, si, ne);
// [13] void insert(di, value);
// [24] CACI insert(CACI dst, value);
// [13] void insert(di, const CompactedArray& srcArray);
// [13] void insert(di, const CompactedArray& srcArray, si, ne);
// [ 2] void pop_back();
// [12] void push_back(const TYPE& value);
// [14] void remove(di);
// [14] void remove(di, ne);
// [25] CACI remove(CACI dstFirst, CACI dstLast);
// [ 2] void removeAll();
// [15] void replace(di, value);
// [15] void replace(di, const CompactedArray& srcArray, si, ne);
// [16] void reserveCapacity(numElements);
// [16] void reserveCapacity(numElements, numUniqueElements);
// [17] void resize(bsl::size_t numElements);
// [11] void swap(CompactedArray& other);
// [ 4] const TYPE& operator[](bsl::size_t index) const;
// [ 4] bslma::Allocator *allocator() const;
// [19] const TYPE& back() const;
// [20] CompactedArray::const_iterator begin() const;
// [ 4] bsl::size_t capacity() const;
// [20] CompactedArray::const_iterator end() const;
// [19] const TYPE& front() const;
// [ 4] bool isEmpty() const;
// [ 6] bool isEqual(const CompactedArray& other) const;
// [ 4] bsl::size_t length() const;
// [ 5] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
// [ 4] const TYPE& uniqueElement(bsl::size_t index) const;
// [ 4] bsl::size_t uniqueLength() const;
// [ 5] ostream& operator<<(ostream& stream, const CompactedArray& array);
// [ 6] bool operator==(lhs, rhs);
// [ 6] bool operator!=(lhs, rhs);
// [11] void swap(CompactedArray& a, CompactedArray& b);
// [26] void hashAppend(HASHALG&, const CompactedArray&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [27] USAGE EXAMPLE
// [10] TESTING BDEX STREAMING
// [ 3] Obj& gg(Obj *object, const char *spec);
// [ 3] int ggg(Obj *object, const char *spec);
// [ 8] CompactedArray g(const char *spec);
// [18] CONCERN: Allocator is propagated to the stored objects.
// ----------------------------------------------------------------------------

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

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlc::CompactedArray<bsl::string> Obj;
typedef bdlc::CompactedArray<int>         ObjInt;

typedef bdlc::CompactedArray<bsl::string>::const_iterator Iterator;

// Define 'bsl::string' value long enough to ensure dynamic memory allocation.
#define SUFFICIENTLY_LONG_STRING "1234567890123456789012345678901234567890" \
                                 "1234567890123456789012345678901234567890"

const char *const LONG_STRING_1  = "a_"  SUFFICIENTLY_LONG_STRING;
const char *const LONG_STRING_2  = "ab_" SUFFICIENTLY_LONG_STRING;

// ============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: 'Storing Daily Schedules'
/// - - - - - - - - - - - - - - - - - -
// Suppose we are creating a sequence of daily schedules for an employee.  Most
// Mondays (and Tuesdays, Wednesdays, etc.) will have the same schedule,
// although some may differ.  Instead of storing this data in a
// 'bsl::vector<my_DailySchedule>', we can use
// 'bdlc::CompactedArray<my_DailySchedule>' to efficiently store this data.
//
// First, we declare and define a 'my_DailySchedule' class.  This class is not
// overly relevant to the example and is elided for the sake of brevity:
//..
                            // ================
                            // my_DailySchedule
                            // ================

    class my_DailySchedule {
        // A value-semantic class that provides a daily schedule and consumes a
        // significant amount of memory.

        int d_initialLocationId;

        // ...

        // FRIENDS
        friend bool operator<(const my_DailySchedule&,
                              const my_DailySchedule&);

      public:
        // CREATORS
        my_DailySchedule(int               initialLocationId,
                         bslma::Allocator *basicAllocator = 0);
            // Create a 'my_DailySchedule' object having the specified
            // 'initialLocationId'.  Optionally specify a 'basicAllocator' used
            // to supply memory.  If 'basicAllocator' is 0, the currently
            // installed default allocator is used.

        // ...

    };

    bool operator<(const my_DailySchedule& lhs, const my_DailySchedule& rhs);
        // Return 'true' if the specified 'lhs' is lexicographically less than
        // the specified 'rhs' object, and 'false' otherwise.

                             // ----------------
                             // my_DailySchedule
                             // ----------------

    // CREATORS
    inline
    my_DailySchedule::my_DailySchedule(int               initialLocationId,
                                       bslma::Allocator *basicAllocator)
    : d_initialLocationId(initialLocationId)
    {
        (void)basicAllocator;  // suppress unused variable compiler warning

        // ...
    }

    bool operator<(const my_DailySchedule& lhs, const my_DailySchedule& rhs)
    {
        if (lhs.d_initialLocationId < rhs.d_initialLocationId) {
            return true;                                              // RETURN
        }

        // ...

        return false;
    }
//..

// ============================================================================
//               GENERATOR FUNCTIONS 'gg' AND 'ggg' FOR TESTING
// ----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.
//
// LANGUAGE SPECIFICATION:
// -----------------------
//
// <SPEC> ::= <EMPTY>   | <LIST>
//
// <EMPTY> ::=
//
// <LIST> ::= <ITEM>    | <ITEM><LIST>
//
// <ITEM> ::= <ELEMENT> | 'D' | 'R' | 'P'
//
// <ELEMENT> ::= ['a' .. 'z']

int getValue(char specChar, bsl::uint64_t *value, int verboseFlag);
    // Place into the specified 'value' the value corresponding to the
    // specified 'specChar' and display errors to 'cerr' if the specified
    // 'verboseFlag' is set.  Return 0 if operation successful, return non-zero
    // otherwise.

int getValue(char specChar, char *value, int verboseFlag)
{
    enum {
        e_SUCCESS_APPEND         = -1,
        e_SUCCESS_APPEND_DEFAULT = -2,
        e_SUCCESS_REMOVEALL      = -3,
        e_SUCCESS_POPBACK        = -4
    };

    if ('a' <= specChar && 'z' >= specChar) {
        *value = specChar;
        return e_SUCCESS_APPEND;                                      // RETURN
    }
    else if ('D' == specChar) {
        return e_SUCCESS_APPEND_DEFAULT;                              // RETURN
    }
    else if ('R' == specChar) {
        return e_SUCCESS_REMOVEALL;                                   // RETURN
    }
    else if ('P' == specChar) {
        return e_SUCCESS_POPBACK;                                     // RETURN
    }

    if (verboseFlag) {
        cerr << "\t\tERROR!" << endl;
        cerr << specChar << " not recognized." << endl;
    }
    return 1;                                                         // RETURN
}

int ggg(Obj *object, const char *spec, int verboseFlag = 1)
    // Configure the specified 'object' according to the specified 'spec',
    // using only the primary manipulator function 'append' and white-box
    // manipulators 'pop_back' and 'removeAll'.  Optionally specify a zero
    // 'verboseFlag' to suppress 'spec' syntax error messages.  Return the
    // index of the first invalid character, and a negative value otherwise.
    // Note that this function is used to implement 'gg' as well as allow for
    // verification of syntax error detection.
{
    enum { e_SUCCESS = -1 };

    char v;

    for (int i = 0; spec[i]; ++i) {
        int rv = getValue(spec[i], &v, verboseFlag);
        if (rv > -1) {
            return i;                                                 // RETURN
        }
        else if (rv == -1) {
            bsl::string value;
            value += v;
            object->append(value);
        }
        else if (rv == -2) {
            object->append(bsl::string());
        }
        else if (rv == -3) {
            object->removeAll();
        }
        else {
            object->pop_back();
        }
    }

    return e_SUCCESS;
}

Obj& gg(Obj *object, const char *spec)
    // Return, by reference, the specified 'object' with its value adjusted
    // according to the specified 'spec'.
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int             verbose = argc > 2;
    int         veryVerbose = argc > 3;
    int     veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 27: {
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
                          << "=============" << endl;

// Then, we create our schedule, which is an array of 'my_DailySchedule' where
// the index of each element is the date offset (from an arbitrary epoch
// measured in days).
//..
    bdlc::CompactedArray<my_DailySchedule> schedule;
//..
// Now, we create some daily schedules and append them to the 'schedule':
//..
    my_DailySchedule evenDays(0);
    my_DailySchedule oddDays(1);

    // Population of the 'my_DailySchedule' objects is elided.

    schedule.push_back(evenDays);
    schedule.push_back(oddDays);
    schedule.push_back(evenDays);
    schedule.push_back(oddDays);
    schedule.push_back(evenDays);
//..
// Finally, we verify that the storage is compacted:
//..
    ASSERT(5 == schedule.length());
    ASSERT(2 == schedule.uniqueLength());
//..
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING 'hashAppend'
        //   Ensure that equal arrays hash to the same value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', hash to the same value if they compare
        //:   equal.
        //:
        //: 2 The capacity and storage method of the objects does not affect
        //:   the result of hashing.
        //:
        //: 3 Non-modifiable objects can be hashed (i.e., objects providing
        //:   only non-modifiable access).
        //
        // Plan:
        //: 1 Specify a set of specifications for the 'gg' function that result
        //:   in an empty object but with different capacity and bytes per
        //:   element characteristics.
        //:
        //: 2 Specify a set of specifications for distinct object values.
        //:
        //: 3 For every item in the cross-product of these two sets, verify
        //:   that the hash value is the same when the two items are.  Presume
        //:   that they are different when the two items are.  (C-1..3)
        //
        // Testing:
        //   void hashAppend(HASHALG&, const CompactedArray&);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING 'hashAppend'"
                          << "\n====================\n";

        typedef ::BloombergLP::bslh::Hash<> Hasher;
        typedef Hasher::result_type         HashType;
        Hasher                              hasher;

        if (verbose) {
            cout << "\nVerify hashing for the array.\n";
        }
        {
            static const struct {
                int         d_lineNum;
                const char *d_spec_p;
            } INIT[] = {
                //line  spec
                //----  ----
                { L_,   ""             },
                { L_,   "aP"           },
                { L_,   "aabbccabcR"   },
                { L_,   "aabbccabcRaP" },
            };
            const int NUM_INIT = static_cast<int>(sizeof INIT / sizeof *INIT);

            static const struct {
                int         d_lineNum;
                const char *d_spec_p;
            } DATA[] = {
                //line  spec
                //----  ----
                { L_,   ""          },
                { L_,   "a"         },
                { L_,   "b"         },
                { L_,   "c"         },
                { L_,   "ab"        },
                { L_,   "ac"        },
                { L_,   "bc"        },
                { L_,   "aaabbbccc" },
                { L_,   "abcabcabc" },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                for (int si = 0; si < NUM_INIT ; ++si) {
                    const int   LINE1 = DATA[ti].d_lineNum;
                    bsl::string SPEC1 = (bsl::string()
                                         + INIT[si].d_spec_p
                                         + DATA[ti].d_spec_p);

                    Obj mX;  const Obj& X = gg(&mX, SPEC1.c_str());

                    for (int tj = 0; tj < NUM_DATA; ++tj) {
                        for (int sj = 0; sj < NUM_INIT ; ++sj) {
                            const int   LINE2 = DATA[tj].d_lineNum;
                            bsl::string SPEC2 = (bsl::string()
                                                 + INIT[sj].d_spec_p
                                                 + DATA[tj].d_spec_p);

                            if (veryVerbose) { P_(SPEC1) P(SPEC2) }

                            Obj mY;  const Obj& Y = gg(&mY, SPEC2.c_str());

                            bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                            HashType hX = hasher(X);
                            HashType hY = hasher(Y);

                            LOOP2_ASSERT(LINE1,
                                         LINE2,
                                         (hX == hY) == (ti == tj));
                            LOOP2_ASSERT(LINE1,
                                         LINE2,
                                         defaultAllocator.numAllocations()
                                                               == allocations);
                        }
                    }
                }
            }
        }

      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING 'remove(CACI dstFirst, CACI dstLast)'
        //   The 'remove' method operates as expected.
        //
        // Concerns:
        //: 1 The methods produce the expected value.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Specify a set of specifications for the 'gg' function that result
        //:   in an empty object but with different capacity and bytes per
        //:   element characteristics.
        //:
        //: 2 Use the table-based approach to specify a set of test vectors.
        //:
        //: 3 For every item in the cross-product of these two sets, verify
        //:   the result for the method (the set from 2 is applied to only
        //:   the initial object).  (C-1)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   CACI remove(CACI dstFirst, CACI dstLast);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'remove(CACI dstFirst, CACI dstLast)'" << endl
                 << "===============================================" << endl;
        }

        static const struct {
            int         d_lineNum;
            const char *d_spec_p;
        } INIT[] = {
            //line  spec
            //----  ----
            { L_,   ""             },
            { L_,   "aP"           },
            { L_,   "aabbccabcR"   },
            { L_,   "aabbccabcRaP" },
        };
        const int NUM_INIT = static_cast<int>(sizeof INIT / sizeof *INIT);

        if (verbose) cout << "\nSub-array remove." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec_p;       // specification
                int         d_di;           // destination index
                int         d_ne;           // number of elements
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec      di  ne  exp
                //----  --------  --  --  ----------------------
                { L_,   "a",       0,  0, "a"                    },
                { L_,   "ab",      0,  0, "ab"                   },

                { L_,   "abcd",    0,  1, "bcd"                  },
                { L_,   "abcd",    1,  1, "acd"                  },
                { L_,   "abcd",    2,  1, "abd"                  },
                { L_,   "abcd",    3,  1, "abc"                  },

                { L_,   "abcd",    0,  2, "cd"                   },
                { L_,   "abcd",    1,  2, "ad"                   },
                { L_,   "abcd",    2,  2, "ab"                   },

                { L_,   "abcd",    0,  3, "d"                    },
                { L_,   "abcd",    1,  3, "a"                    },

                { L_,   "abcd",    0,  4, ""                     },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const int         DI      = DATA[i].d_di;
                const int         NE      = DATA[i].d_ne;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj mEXP;  gg(&mEXP, EXPSPEC);  const Obj& EXP = mEXP;

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC = (bsl::string()
                                        + INIT[j].d_spec_p
                                        + DATA[i].d_spec_p);

                    if (veryVerbose) { P(SPEC) }

                    Obj        mX;  gg(&mX, SPEC.c_str());
                    const Obj& X = mX;
                    Iterator   F = X.begin() + DI;
                    Iterator   L = F + NE;
                    LOOP_ASSERT(LINE, F == mX.remove(F, L));
                    LOOP_ASSERT(LINE, X == EXP);
                }
            }
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Obj mX;  const Obj& X = mX;  gg(&mX,  "abc");

            Obj mT;  const Obj& T = mT;

            mT = X;  ASSERT_SAFE_PASS(mT.remove(T.begin(), T.end()));
            mT = X;  ASSERT_SAFE_FAIL(mT.remove(T.end(),   T.begin()));
            mT = X;  ASSERT_SAFE_FAIL(mT.remove(X.begin(), T.end()));
            mT = X;  ASSERT_SAFE_FAIL(mT.remove(T.begin(), X.end()));
        }
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING 'insert(CACI dst, value)'
        //   The 'insert' method operates as expected.
        //
        // Concerns:
        //: 1 The method is exception neutral with respect to memory
        //:   allocation.
        //:
        //: 2 The method produces the expected value.
        //:
        //: 3 When there is initially sufficient storage in the result variable
        //:   to store the result, the methods produce the expected value.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Wrap all tests with the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
        //:   macros to ensure exception neutrality with respect to memory
        //:   allocation.  (C-1)
        //:
        //: 2 Specify a set of specifications for the 'gg' function that result
        //:   in an empty object but with different capacity and bytes per
        //:   element characteristics.
        //:
        //: 3 Use the table-based approach to specify a set of test vectors.
        //:
        //: 4 For every item in the cross-product of these two sets, verify
        //:   the result for the method (the set from 2 is applied to only
        //:   the initial object).  (C-2..3)
        //:
        //: 5 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   CACI insert(CACI dst, value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'insert(CACI dst, value)'" << endl
                          << "==================================" << endl;

        static const struct {
            int         d_lineNum;
            const char *d_spec_p;
        } INIT[] = {
            //line  spec
            //----  ----
            { L_,   ""             },
            { L_,   "aP"           },
            { L_,   "aabbccabcR"   },
            { L_,   "aabbccabcRaP" },
        };
        const int NUM_INIT = static_cast<int>(sizeof INIT / sizeof *INIT);

        if (verbose) cout << "\nValue insert with iterator." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec1_p;      // specification 1
                int         d_di;           // destination index
                const char  d_spec2;        // specification 2
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec1     di  spec2     exp
                //----  --------  --  --------  ----------------------------
                { L_,   "",        0, 'a',      "a"                          },

                { L_,   "a",       0, 'b',      "ba"                         },
                { L_,   "a",       1, 'b',      "ab"                         },

                { L_,   "ab",      0, 'c',      "cab"                        },
                { L_,   "ab",      1, 'c',      "acb"                        },
                { L_,   "ab",      2, 'c',      "abc"                        },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const int         DI      = DATA[i].d_di;
                const char        SPEC2   = DATA[i].d_spec2;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                bsl::string y(&SPEC2, 1);

                Obj        mEXP;  gg(&mEXP, EXPSPEC);
                const Obj& EXP =  mEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC1 = (bsl::string()
                                         + INIT[j].d_spec_p
                                         + DATA[i].d_spec1_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC1.c_str());
                        const Obj& X = mX;
                        Iterator   DST = X.begin() + DI;
                        LOOP_ASSERT(LINE, DST == mX.insert(DST, y));
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Obj mX;  const Obj& X = mX;  gg(&mX,  "abc");

            Obj mT;  const Obj& T = mT;

            const bsl::string Z("a", 1);

            mT = X;  ASSERT_SAFE_PASS(mT.insert(T.begin(), Z));
            mT = X;  ASSERT_SAFE_PASS(mT.insert(T.end(),   Z));
            mT = X;  ASSERT_SAFE_FAIL(mT.insert(X.begin(), Z));
        }
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // CONST ITERATOR REMAINS VALID OVER MODIFICATIONS
        //   Ensure the methods work correctly.
        //
        // Concerns:
        //: 1 The iterator remains valid as the data in the underlying array
        //:   is modified; including modifications that modify the size of the
        //:   array.
        //
        // Plan:
        //: 1 Use the table-based approach to specify a set of test vectors
        //:   'S'.
        //:
        //: 2 For every pair of items 'si' and 'sj' chosen from 'S', use the
        //:   generator function to produce an array from 'si', exhaustively
        //:   create an iterator at each position in the array, append an
        //:   array generated from 'sj', and verify the iterators remain
        //:   valid.  Then 'pop_back' as many elements as possible and ensure
        //:   the iterators remain valid as long as possible.
        //:
        //: 3 For every pair of items 'si' and 'sj' chosen from 'S', use the
        //:   generator function to produce an array from 'si', exhaustively
        //:   create an iterator at each position in the array, assign the
        //:   value of this array to an array generated from 'sj', and verify
        //:   the iterators remain valid if the new length of the array is
        //:   sufficient for the iterator to remain valid.  (C-1)
        //
        // Testing:
        //   CONCERN: iterators remain valid over modifications
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONST ITERATOR REMAINS VALID OVER MODIFICATIONS"
                          << endl
                          << "==============================================="
                          << endl;
        {
            static const struct {
                int          d_lineNum;          // source line number
                const char  *d_spec_p;           // specification string
                bsl::size_t  d_length;           // expected length
            } DATA[] = {
                //line  spec      len
                //----  --------  ---
                { L_,   "",         0 },
                { L_,   "D",        1 },
                { L_,   "a",        1 },
                { L_,   "b",        1 },
                { L_,   "c",        1 },
                { L_,   "aP",       0 },
                { L_,   "aPb",      1 },
                { L_,   "aPbP",     0 },
                { L_,   "abc",      3 },
                { L_,   "abcR",     0 },
                { L_,   "Dab",      3 },
                { L_,   "Dabcd",    5 },
                { L_,   "Dabcde",   6 },
                { L_,   "aDbcdef",  7 }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         ILINE = DATA[ti].d_lineNum;
                const char *const ISPEC = DATA[ti].d_spec_p;
                const bsl::size_t ILEN  = DATA[ti].d_length;

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int         JLINE = DATA[tj].d_lineNum;
                    const char *const JSPEC = DATA[tj].d_spec_p;
                    const bsl::size_t JLEN  = DATA[tj].d_length;

                    for (unsigned int index = 0; index < ILEN; ++index) {
                        Obj mX;  const Obj& X = gg(&mX, ISPEC);

                        // Create an iterator at a valid position.

                        Iterator iter(X.begin());
                        iter += index;
                        ASSERTV(ILINE, JLINE, *iter == X[index]);

                        Obj mY;
                        gg(&mY, JSPEC);

                        // Verify 'append' does not invalidate the iterator.

                        mX.append(mY);
                        ASSERTV(ILINE, JLINE, *iter == X[index]);

                        // Verify reducing the size does not invalidate the
                        // iterator.

                        mX.pop_back();
                        while (mX.length() > index) {
                            ASSERTV(ILINE, JLINE, *iter == X[index]);
                            mX.pop_back();
                        }
                    }

                    for (unsigned int index = 0;
                         index < ILEN && index < JLEN;
                         ++index) {
                        Obj mX;  const Obj& X = gg(&mX, ISPEC);

                        // Create an iterator at a valid position.

                        Iterator iter(X.begin());
                        iter += index;
                        ASSERTV(ILINE, JLINE, *iter == X[index]);

                        Obj mY;  const Obj& Y = gg(&mY, JSPEC);

                        // Verify assignment does not invalidate the iterator.

                        mX = Y;
                        ASSERTV(ILINE, JLINE, *iter == X[index]);
                    }
                }
            }
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // CONST ITERATOR OTHER METHODS
        //   Ensure the methods work correctly.
        //
        // Concerns:
        //: 1 The methods work as expected; including with negative deltas.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the generator function to produce objects of arbitrary state.
        //:   Exhaustively verify the methods with all valid starting
        //:   positions for an iterator and all valid deltas, positive and
        //:   negative, with the array's 'operator[]' as an oracle.  (C-1)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   CACI& CACI::operator+=(bsl::ptrdiff_t offset);
        //   CACI& CACI::operator-=(bsl::ptrdiff_t offset);
        //   CACI CACI::operator++(CACI& iter, int);
        //   CACI CACI::operator--(CACI& iter, int);
        //   CACI CACI::operator+(const CACI& iterator, bsl::ptrdiff_t offset);
        //   CACI CACI::operator+(bsl::ptrdiff_t offset, const CACI& iterator);
        //   CACI CACI::operator-(const CACI& iterator, bsl::ptrdiff_t offset);
        //   bsl::ptrdiff_t operator-(const CACI& lhs, const CACI& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONST ITERATOR OTHER METHODS" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting other methods."
                          << endl;
        {
            static const struct {
                int          d_lineNum;          // source line number
                const char  *d_spec_p;           // specification string
                bsl::size_t  d_length;           // expected length
            } DATA[] = {
                //line  spec      len
                //----  --------  ---
                { L_,   "",         0 },
                { L_,   "D",        1 },
                { L_,   "a",        1 },
                { L_,   "b",        1 },
                { L_,   "c",        1 },
                { L_,   "aP",       0 },
                { L_,   "aPb",      1 },
                { L_,   "aPbP",     0 },
                { L_,   "abc",      3 },
                { L_,   "abcR",     0 },
                { L_,   "Dab",      3 },
                { L_,   "Dabcd",    5 },
                { L_,   "Dabcde",   6 },
                { L_,   "aDbcdef",  7 }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const bsl::size_t LEN  = DATA[ti].d_length;

                Obj mX;  const Obj& X = gg(&mX, SPEC);   // original spec

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                LOOP_ASSERT(LINE, LEN == X.length());

                Iterator iter(X.begin());

                for (int i = 0; i < static_cast<int>(LEN); ++i) {
                    for (int j = 0; j < static_cast<int>(LEN); ++j) {
                        if (i + j < static_cast<int>(LEN)) {
                            Iterator iter2 = iter + j;
                            LOOP2_ASSERT(i, j, *iter2 == X[i + j]);

                            Iterator iter3 = j + iter;
                            LOOP2_ASSERT(i, j, *iter3 == X[i + j]);

                            Iterator iter4 = iter;
                            iter4 += j;
                            LOOP2_ASSERT(i, j, *iter4 == X[i + j]);

                            LOOP2_ASSERT(i, j, iter4 - iter ==  j);
                            LOOP2_ASSERT(i, j, iter - iter4 == -j);

                            Iterator iter5 = iter - (-j);
                            LOOP2_ASSERT(i, j, *iter5 == X[i + j]);

                            Iterator iter6 = iter;
                            iter6 -= (-j);
                            LOOP2_ASSERT(i, j, *iter6 == X[i + j]);
                        }
                        if (i - j >= 0) {
                            Iterator iter2 = iter - j;
                            LOOP2_ASSERT(i, j, *iter2 == X[i - j]);

                            Iterator iter3 = iter;
                            iter3 -= j;
                            LOOP2_ASSERT(i, j, *iter3 == X[i - j]);

                            LOOP2_ASSERT(i, j, iter3 - iter == -j);
                            LOOP2_ASSERT(i, j, iter - iter3 ==  j);

                            Iterator iter4 = iter + (-j);
                            LOOP2_ASSERT(i, j, *iter4 == X[i - j]);

                            Iterator iter5 = (-j) + iter;
                            LOOP2_ASSERT(i, j, *iter5 == X[i - j]);

                            Iterator iter6 = iter;
                            iter6 += (-j);
                            LOOP2_ASSERT(i, j, *iter6 == X[i - j]);
                        }
                    }

                    // Verify 'operator++(int)' and 'operator--(int)'.

                    Iterator iterOrig(iter);

                    Iterator iter2(iter++);
                    ASSERT(iter2 == iterOrig);

                    iter2++;
                    Iterator iter3(iter2--);
                    ASSERT(iter2 == iterOrig);
                    ASSERT(iter3 == iter);
                }

                ASSERT(iter == X.end());

                LOOP_ASSERT(LINE,
                            defaultAllocator.numAllocations() == allocations);
            }
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Obj mX;  const Obj& X = mX;
            mX.append("a");

            Obj mY;  const Obj& Y= mY;
            mY.append("a");

            Iterator iter;

            ASSERT_SAFE_FAIL(iter++);
            ASSERT_SAFE_FAIL(iter--);
            ASSERT_SAFE_FAIL(iter += 1);
            ASSERT_SAFE_FAIL(iter -= 1);
            ASSERT_SAFE_FAIL(iter + 1);
            ASSERT_SAFE_FAIL(1 + iter);
            ASSERT_SAFE_FAIL(iter - 1);

            iter = X.begin();
            ASSERT_SAFE_PASS(iter += 1);
            ASSERT_SAFE_FAIL(iter += 1);

            iter = X.begin();
            ASSERT_SAFE_FAIL(iter -= 1);

            iter = X.end();
            ASSERT_SAFE_PASS(iter -= 1);
            ASSERT_SAFE_FAIL(iter -= 1);

            iter = X.end();
            ASSERT_SAFE_FAIL(iter += 1);

            iter = X.begin();
            ASSERT_SAFE_PASS(iter++);
            ASSERT_SAFE_PASS(iter--);
            ASSERT_SAFE_FAIL(iter--);

            iter = X.end();
            ASSERT_SAFE_PASS(iter--);
            ASSERT_SAFE_PASS(iter++);
            ASSERT_SAFE_FAIL(iter++);

            iter = X.begin();
            ASSERT_SAFE_FAIL(iter - 1);
            ASSERT_SAFE_PASS(iter + 1);
            ASSERT_SAFE_FAIL(iter + 2);
            ASSERT_SAFE_PASS(1 + iter);
            ASSERT_SAFE_FAIL(2 + iter);

            iter = X.end();
            ASSERT_SAFE_FAIL(iter + 1);
            ASSERT_SAFE_FAIL(1 + iter);
            ASSERT_SAFE_PASS(iter - 1);
            ASSERT_SAFE_FAIL(iter - 2);

            ASSERT_SAFE_PASS(X.end()    - X.begin());
            ASSERT_SAFE_PASS(X.begin()  - X.end());
            ASSERT_SAFE_FAIL(X.begin()  - Y.end());
            ASSERT_SAFE_FAIL(X.begin()  - Iterator());
            ASSERT_SAFE_FAIL(Iterator() - X.begin());

            mX.append("a");

            ASSERT_SAFE_PASS(X.end()   - X.begin());
            ASSERT_SAFE_PASS(X.begin() - X.end());
            ASSERT_SAFE_FAIL(X.begin() - Y.end());
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // CONST ITERATOR RELATIONAL OPERATORS
        //   Ensure the relational operators work as expected.
        //
        // Concerns:
        //: 1 The methods work as expected.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Construct a sorted array and verify the results of an operator by
        //:   comparison with the result of the operator on the values at the
        //:   iterators.  (C-1)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   bool operator<(const CACI& lhs, const CACI& rhs);
        //   bool operator<=(const CACI& lhs, const CACI& rhs);
        //   bool operator>(const CACI& lhs, const CACI& rhs);
        //   bool operator>=(const CACI& lhs, const CACI& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONST ITERATOR RELATIONAL OPERATORS" << endl
                          << "===================================" << endl;

        if (verbose) cout << "\nTesting relational operators."
                          << endl;
        {
            Obj mX;  const Obj& X = mX;
            mX.append("a");  mX.append("b");  mX.append("c");
            mX.append("d");  mX.append("e");  mX.append("f");
            mX.append("g");  mX.append("h");  mX.append("i");

            for (Iterator iter1 = X.begin(); iter1 != X.end(); ++iter1) {
                for (Iterator iter2 = X.begin(); iter2 != X.end(); ++iter2) {
                    if (veryVerbose) { P_(*iter1) P(*iter2) }

                    ASSERT((iter1 <  iter2) == (*iter1 <  *iter2));
                    ASSERT((iter1 <= iter2) == (*iter1 <= *iter2));
                    ASSERT((iter1 >  iter2) == (*iter1 >  *iter2));
                    ASSERT((iter1 >= iter2) == (*iter1 >= *iter2));
                }
            }
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Obj        mX;
            const Obj& X = mX;
            Obj        mY;
            const Obj& Y= mY;

            ASSERT_SAFE_PASS(X.begin()  <  X.end());
            ASSERT_SAFE_PASS(X.begin()  <  X.end());
            ASSERT_SAFE_PASS(X.begin()  <= X.end());
            ASSERT_SAFE_PASS(X.begin()  >  X.end());
            ASSERT_SAFE_PASS(X.begin()  >= X.end());

            ASSERT_SAFE_FAIL(X.begin()  <  Y.end());
            ASSERT_SAFE_FAIL(X.begin()  <= Y.end());
            ASSERT_SAFE_FAIL(X.begin()  >  Y.end());
            ASSERT_SAFE_FAIL(X.begin()  >= Y.end());

            ASSERT_SAFE_FAIL(X.begin()  <  Iterator());
            ASSERT_SAFE_FAIL(Iterator() <  X.begin());
            ASSERT_SAFE_FAIL(X.begin()  <= Iterator());
            ASSERT_SAFE_FAIL(Iterator() <= X.begin());
            ASSERT_SAFE_FAIL(X.begin()  >  Iterator());
            ASSERT_SAFE_FAIL(Iterator() >  X.begin());
            ASSERT_SAFE_FAIL(X.begin()  >= Iterator());
            ASSERT_SAFE_FAIL(Iterator() >= X.begin());
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // CONST ITERATOR BASIC METHODS
        //   Ensure we can create a 'const' iterator, inspect the value at
        //   the iterator, and destroy the iterator.
        //
        // Concerns:
        //: 1 The equality, assignment, and copy construct methods work as
        //:   expected.
        //:
        //: 2 The other basic methods work correctly, including the use of
        //:   negative offsets in 'operator[]'.
        //:
        //: 3 The destructor functions properly.
        //:
        //: 4 The iterator returned by 'begin()' is valid and constant
        //:   regardless of the length of the array, and is equal to the
        //:   iterator returned by 'end()' if the array is empty.
        //:
        //: 5 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Construct arrays and verify the results of the equality,
        //:   assignment, and copy construct methods.  (C-1)
        //:
        //: 2 Use the generator function to produce objects of arbitrary state.
        //:   Verify the basic methods - excluding 'operator[]' - against the
        //:   expected values.  Exhaustively verify 'operator[]' with all valid
        //:   starting positions for an iterator and all valid offsets,
        //:   positive and negative, with the array's 'operator[]' as an
        //:   oracle.  (C-2)
        //:
        //: 3 Allow the iterators to go out-of-scope; there is nothing to
        //:   explicitly verify.  (C-3)
        //:
        //: 4 Create an empty array and verify 'begin() == end()'.  Use the
        //:   generator function to produce objects of arbitrary state and
        //:   assign the empty array to this array.  Verify the iterator
        //:   returned by 'begin()' on this array has the same value as the
        //:   one obtained while the array was empty.  (C-4)
        //:
        //: 5 Verify defensive checks are triggered for invalid values.  (C-5)
        //
        // Testing:
        //   CompactedArray::const_iterator begin() const;
        //   CompactedArray::const_iterator end() const;
        //   CompactedArray::const_iterator();
        //   CompactedArray::const_iterator(const CACI& original);
        //   ~CompactedArray::const_iterator();
        //   CACI& operator=(const CACI& rhs);
        //   CACI& CACI::operator++();
        //   CACI& CACI::operator--();
        //   TYPE CACI::operator*() const;
        //   TYPE CACI::operator->() const;
        //   TYPE CACI::operator[](bsl::ptrdiff_t offset) const;
        //   bool operator==(const CACI& lhs, const PIAI& rhs);
        //   bool operator!=(const CACI& lhs, const PIAI& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONST ITERATOR BASIC METHODS" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting equality operators."
                          << endl;
        {
            Obj mX;  const Obj& X = mX;
            mX.append("a");

            Obj mY;  const Obj& Y = mY;
            mY.append("a");

            Iterator iter1 = X.begin();
            Iterator iter2 = X.end();
            Iterator iter3 = Y.begin();
            Iterator iter4 = Y.end();

            ASSERT(iter1 == iter1);
            ASSERT(iter1 != iter2);
            ASSERT(iter1 != iter3);
            ASSERT(iter1 != iter4);

            ASSERT(iter2 != iter1);
            ASSERT(iter2 == iter2);
            ASSERT(iter2 != iter3);
            ASSERT(iter2 != iter4);

            ASSERT(iter3 != iter1);
            ASSERT(iter3 != iter2);
            ASSERT(iter3 == iter3);
            ASSERT(iter3 != iter4);

            ASSERT(iter4 != iter1);
            ASSERT(iter4 != iter2);
            ASSERT(iter4 != iter3);
            ASSERT(iter4 == iter4);

            mX.append("a");
            iter2 = mX.end();
            ASSERT(iter1 != iter2);
            ++iter1;
            ASSERT(iter1 != iter2);
            --iter2;
            ASSERT(iter1 == iter2);
        }

        if (verbose) cout << "\nTesting assignment and copy construct."
                          << endl;
        {
            Obj mX;  const Obj& X = mX;
            mX.append("a");

            Iterator iter1 = X.begin();
            Iterator iter2 = X.end();
            ASSERT(iter1 != iter2);

            {
                Iterator iter;

                ASSERT(iter != iter1);
                ASSERT(iter != iter2);

                iter = iter1;

                ASSERT(iter == iter1);
                ASSERT(iter != iter2);

                iter = iter2;

                ASSERT(iter != iter1);
                ASSERT(iter == iter2);
            }
            {
                Iterator iter(iter1);

                ASSERT(iter == iter1);
                ASSERT(iter != iter2);
            }
            {
                Iterator iter(iter2);

                ASSERT(iter != iter1);
                ASSERT(iter == iter2);
            }
        }

        if (verbose) cout << "\nTesting basic methods."
                          << endl;
        {
            static const struct {
                int          d_lineNum;          // source line number
                const char  *d_spec_p;           // specification string
                bsl::size_t  d_length;           // expected length
            } DATA[] = {
                //line  spec      len
                //----  --------  ---
                { L_,   "",         0 },
                { L_,   "D",        1 },
                { L_,   "a",        1 },
                { L_,   "b",        1 },
                { L_,   "c",        1 },
                { L_,   "aP",       0 },
                { L_,   "aPb",      1 },
                { L_,   "aPbP",     0 },
                { L_,   "abc",      3 },
                { L_,   "abcR",     0 },
                { L_,   "Dab",      3 },
                { L_,   "Dabcd",    5 },
                { L_,   "Dabcde",   6 },
                { L_,   "aDbcdef",  7 }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const bsl::size_t LEN  = DATA[ti].d_length;

                Obj mX;  const Obj& X = gg(&mX, SPEC);   // original spec

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                LOOP_ASSERT(LINE, LEN == X.length());

                // Verify assignment return value.

                Iterator iter;
                ASSERT(&iter == &(iter = X.begin()));

                for (int i = 0; i < static_cast<int>(LEN); ++i) {
                    // Verify 'operator*' and 'operator->'.

                    LOOP_ASSERT(i, *iter == X[i]);
                    LOOP_ASSERT(i, iter.operator->() == X[i]);

                    // Verify 'operator[]' including with negative index.

                    for (int j = 0; j < static_cast<int>(LEN); ++j) {
                        if (i + j < static_cast<int>(LEN)) {
                            LOOP2_ASSERT(i, j, iter[ j] == X[i + j]);
                        }
                        if (i - j >= 0) {
                            LOOP2_ASSERT(i, j, iter[-j] == X[i - j]);
                        }
                    }

                    // Verify 'operator++' and 'operator--'.

                    Iterator iterOrig(iter);

                    Iterator iter2(++iter);
                    ASSERT(iter2 == iter);

                    Iterator iter3(--iter2);
                    ASSERT(iter2 == iterOrig);
                    ASSERT(iter3 == iterOrig);
                }

                ASSERT(iter == X.end());

                LOOP_ASSERT(LINE,
                            defaultAllocator.numAllocations() == allocations);

                // Verify 'begin()'.

                Obj mY;  const Obj Y = mY;

                Iterator initialBegin = Y.begin();

                iter = Y.begin();
                ASSERT(iter == Y.end());
                ASSERT(iter == initialBegin);

                mY = X;
                iter = Y.begin();
                ASSERT(iter == initialBegin);
                ASSERT(Y.isEmpty() || *iter == Y[0]);
            }
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Obj mX;  const Obj& X = mX;
            mX.append("a");

            Iterator iter;
            ASSERT_SAFE_FAIL(*iter);
            ASSERT_SAFE_FAIL(iter.operator->());
            ASSERT_SAFE_FAIL(iter[0]);

            iter = X.begin();
            ASSERT_SAFE_FAIL(iter[-1]);
            ASSERT_SAFE_PASS(iter[ 0]);
            ASSERT_SAFE_FAIL(iter[ 1]);

            iter = X.begin();
            ASSERT_SAFE_PASS(++iter);
            ASSERT_SAFE_PASS(--iter);
            ASSERT_SAFE_FAIL(--iter);

            iter = X.end();
            ASSERT_SAFE_PASS(--iter);
            ASSERT_SAFE_PASS(++iter);
            ASSERT_SAFE_FAIL(++iter);

            iter = X.begin();
            ASSERT_SAFE_PASS(*iter);
            ASSERT_SAFE_PASS(iter.operator->());
            mX.removeAll();
            ASSERT_SAFE_FAIL(*iter);
            ASSERT_SAFE_FAIL(iter.operator->());
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING 'front' AND 'back'
        //   Ensure these accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding attribute of
        //:   the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //:
        //: 3 No accessor allocates any memory.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the generator function to produce objects of arbitrary state
        //:   and verify the accessors' return values against expected values.
        //:   (C-1)
        //:
        //: 2 The accessors will only be accessed from a 'const' reference to
        //:   the created object.  (C-2)
        //:
        //: 3 The default allocator will be used for all created objects
        //:   (excluding those used to test 'allocator') and the number of
        //:   allocation will be verified to ensure that no memory was
        //:   allocated during use of the accessors.  (C-3)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   TYPE back() const;
        //   TYPE front() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'front' AND 'back'" << endl
                          << "==========================" << endl;

        {
            static const struct {
                int          d_lineNum;  // source line number
                const char  *d_spec_p;   // specification string
                char         d_front;    // expected 'front' value
                char         d_back;     // expected 'back' value
            } DATA[] = {
                //LN    spec    front  back
                //--  --------  -----  ----
                { L_,      "a",   'a',  'a' },
                { L_,      "b",   'b',  'b' },
                { L_,     "ab",   'a',  'b' },
                { L_,     "ba",   'b',  'a' },
                { L_,    "abc",   'a',  'c' },
                { L_,    "cba",   'c',  'a' },
                { L_, "aaabbb",   'a',  'b' },
                { L_,    "aPb",   'b',  'b' },
                { L_,  "abcRa",   'a',  'a' },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec_p;
                const bsl::string FRONT = bsl::string(&DATA[ti].d_front, 1);
                const bsl::string BACK  = bsl::string(&DATA[ti].d_back,  1);

                Obj mX;  const Obj& X = gg(&mX, SPEC);   // original spec

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                LOOP_ASSERT(LINE, FRONT == X.front());
                LOOP_ASSERT(LINE, BACK  == X.back());

                LOOP_ASSERT(LINE,
                            defaultAllocator.numAllocations() == allocations);
            }
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // VALUE CONSTRUCTOR
        //   Ensure that we can create an array with a specified length and
        //   initial value.  Ensure that all attributes of the array are set
        //   appropriately for these parameters.
        //
        // Concerns:
        //: 1 The value constructor creates the correct initial value and has
        //:   the internal memory management system hooked up properly so that
        //:   *all* internally allocated memory draws from the same
        //:   user-supplied allocator whenever one is specified.
        //:
        //: 2 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 3 There is no temporary memory allocation from any allocator.
        //:
        //: 4 Any memory allocation is exception neutral.
        //:
        //: 5 The value constructor (with or without a supplied allocator)
        //:   creates an object having the specified numElements of the
        //:   specified initial value.
        //:
        //: 6 If an initial value is NOT supplied to the value constructor, the
        //:   default value of 0 is used.
        //:
        //: 7 All allocated memory is returned to the allocator.
        //
        // Plan:
        //: 1 Create an object using the value constructor with and without
        //:   passing in an allocator, verify the allocator is stored using the
        //:   'allocator' accessor, and verifying all allocations are done from
        //:   the allocator in future tests.  (C-1,2)
        //:
        //: 2 Monitor memory usage from the default allocator to ensure all
        //:   memory is obtained from the supplied allocator.  (C-3)
        //:
        //: 3 Use the loop-based approach to create objects with various
        //:   values for 'numElements' and 'value' with a
        //:   'bslma::TestAllocator' and compare all aspects of this object
        //:   with the oracle value obtained from a default constructed object
        //:   with the 'value' appended the appropriate number of times.  Also
        //:   vary the test allocator's allocation limit to verify behavior in
        //:   the presence of exceptions.  (C-4..6)
        //:
        //: 4 Use a supplied 'bslma::TestAllocator' that goes out-of-scope
        //:   at the conclusion of each test to ensure all memory is returned
        //:   to the allocator.  (C-7)
        //
        // Testing:
        //   CompactedArray(numElements, value, basicAllocator);
        //   CONCERN: Allocator is propagated to the stored objects.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VALUE CONSTRUCTOR" << endl
                          << "=================" << endl;

        if (verbose) cout << "\nTesting with various allocator configurations."
                          << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            Obj mX(3);  const Obj& X = mX;
            ASSERT(&defaultAllocator == X.allocator());
            ASSERT(3 == X.length());
            ASSERT(3 == X.capacity());
            ASSERT(allocations + 2 == defaultAllocator.numAllocations());
        }
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            Obj        mX(3,
                          bsl::string() + "a",
                          reinterpret_cast<bslma::TestAllocator *>(0));
            const Obj& X = mX;

            ASSERT(&defaultAllocator == X.allocator());
            ASSERT(3 == X.length());
            ASSERT(3 == X.capacity());
            ASSERT(allocations + 2 == defaultAllocator.numAllocations());
        }
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(3, bsl::string() + "a", &sa);  const Obj& X = mX;

            ASSERT(&sa == X.allocator());
            ASSERT(3 == X.length());
            ASSERT(3 == X.capacity());
            ASSERT(allocations == defaultAllocator.numAllocations());
            ASSERT(2 == sa.numAllocations());
        }

        if (verbose) cout << "\nTesting with 'numElements'."
                          << endl;
        {
            bslma::TestAllocator         sa(veryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&sa);

            for (bsl::size_t ne = 0; ne < 50; ++ne) {
                Obj                mEXP;
                const Obj&         EXP  = mEXP;

                for (bsl::size_t i = 0; i < ne; ++i) {
                    if (veryVerbose) { P_(ne) P(i) }

                    mEXP.append(bsl::string());
                }

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    Obj mX(ne);  const Obj& X = mX;

                    LOOP_ASSERT(ne, X == EXP);
                    LOOP_ASSERT(ne, X.isEmpty() == EXP.isEmpty());
                    if (false == X.isEmpty() && false == EXP.isEmpty()) {
                        LOOP_ASSERT(ne, X.capacity() == EXP.capacity());
                    }
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP_ASSERT(ne,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nTesting with 'numElements' and 'value'."
                          << endl;
        {
            const char        *VALUES = "abcdefghij";
            const bsl::size_t  NUM_VALUES = strlen(VALUES);

            for (bsl::size_t ne = 0; ne < 10; ++ne) {
                for (bsl::size_t vi = 0; vi <= NUM_VALUES; ++vi) {
                    bsl::string VALUE;

                    if (vi < NUM_VALUES) {
                        VALUE = bsl::string(VALUES + vi, 1);
                    }
                    else {
                        VALUE = LONG_STRING_1;
                    }

                    Obj                mEXP;
                    const Obj&         EXP  = mEXP;

                    for (bsl::size_t i = 0; i < ne; ++i) {
                        if (veryVerbose) { P_(ne) P_(vi) P(i) }

                        mEXP.append(VALUE);
                    }

                    bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                    bslma::TestAllocator sa(veryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj mX(ne, VALUE, &sa);  const Obj& X = mX;

                        LOOP_ASSERT(ne, X == EXP);
                        LOOP_ASSERT(ne, X.isEmpty() == EXP.isEmpty());
                        if (false == X.isEmpty() && false == EXP.isEmpty()) {
                            LOOP_ASSERT(ne, X.capacity() == EXP.capacity());
                        }
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    LOOP_ASSERT(ne,
                                allocations
                                         == defaultAllocator.numAllocations());
                }
            }
        }

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // RESIZE
        //   The 'resize' method operates as expected.
        //
        // Concerns:
        //: 1 The method is exception neutral with respect to memory
        //:   allocation.
        //:
        //: 2 The method produces the expected value.
        //
        // Plan:
        //: 1 Wrap all tests with the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
        //:   macros to ensure exception neutrality with respect to memory
        //:   allocation.  (C-1)
        //:
        //: 2 Specify a set of specifications for the 'gg' function that result
        //:   in an empty object but with different capacity and bytes per
        //:   element characteristics.
        //:
        //: 3 Use the table-based approach to specify a set of test vectors.
        //:
        //: 4 For every item in the cross-product of these two sets, verify
        //:   the result for the method (the set from 2 is applied to only
        //:   the initial object).  (C-2)
        //
        // Testing:
        //   void resize(bsl::size_t numElements);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "RESIZE" << endl
                          << "======" << endl;

        static const struct {
            int         d_lineNum;
            const char *d_spec_p;
        } INIT[] = {
            //line  spec
            //----  ----
            { L_,   ""             },
            { L_,   "aP"           },
            { L_,   "aabbccabcR"   },
            { L_,   "aabbccabcRaP" },
        };
        const int NUM_INIT = static_cast<int>(sizeof INIT / sizeof *INIT);

        if (verbose) cout << "\nTesting resize." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec_p;       // specification
                bsl::size_t d_ne;           // number of elements
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec      ne  exp
                //----  --------  --  ----------------------------
                { L_,   "",        0, ""                           },
                { L_,   "",        1, "D"                          },
                { L_,   "",        2, "DD"                         },
                { L_,   "",        3, "DDD"                        },
                { L_,   "",        4, "DDDD"                       },
                { L_,   "",        5, "DDDDD"                      },

                { L_,   "a",       0, ""                           },
                { L_,   "a",       1, "a"                          },
                { L_,   "a",       2, "aD"                         },
                { L_,   "a",       3, "aDD"                        },
                { L_,   "a",       4, "aDDD"                       },
                { L_,   "a",       5, "aDDDD"                      },

                { L_,   "ab",      0, ""                           },
                { L_,   "ab",      1, "a"                          },
                { L_,   "ab",      2, "ab"                         },
                { L_,   "ab",      3, "abD"                        },
                { L_,   "ab",      4, "abDD"                       },
                { L_,   "ab",      5, "abDDD"                      },

                { L_,   "abc",     0, ""                           },
                { L_,   "abc",     1, "a"                          },
                { L_,   "abc",     2, "ab"                         },
                { L_,   "abc",     3, "abc"                        },
                { L_,   "abc",     4, "abcD"                       },
                { L_,   "abc",     5, "abcDD"                      },

                { L_,   "abcd",    0, ""                           },
                { L_,   "abcd",    1, "a"                          },
                { L_,   "abcd",    2, "ab"                         },
                { L_,   "abcd",    3, "abc"                        },
                { L_,   "abcd",    4, "abcd"                       },
                { L_,   "abcd",    5, "abcdD"                      },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const bsl::size_t NE      = DATA[i].d_ne;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj mEXP;  gg(&mEXP, EXPSPEC);  const Obj& EXP = mEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC = (bsl::string()
                                        + INIT[j].d_spec_p
                                        + DATA[i].d_spec_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC.c_str());
                        const Obj& X = mX;
                        mX.resize(NE);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // RESERVE CAPACITY
        //   The 'reserveCapacity' methods operate as expected.
        //
        // Concerns:
        //: 1 The methods are exception neutral with respect to memory
        //:   allocation.
        //:
        //: 2 The methods produce the expected state without affecting the
        //:   value.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Wrap all tests with the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
        //:   macros to ensure exception neutrality with respect to memory
        //:   allocation.  (C-1)
        //:
        //: 2 Use the table-based approach to specify a set of initial states
        //:   and directly validate the expected results.  (C-2)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   void reserveCapacity(numElements);
        //   void reserveCapacity(numElements, numUniqueElements);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "RESERVE CAPACITY" << endl
                          << "================" << endl;

        static const struct {
            int                d_lineNum;            // line number
            const char        *d_spec_p;             // specification
            const bsl::size_t  d_numUniqueElements;  // # of unique elem.
        } DATA[] = {
            //line  spec      NUE
            //----  --------  ---
            { L_,         "",   0 },
            { L_,         "",   1 },
            { L_,         "",   5 },
            { L_,        "a",   0 },
            { L_,        "a",   1 },
            { L_,        "a",   5 },
            { L_,       "ab",   0 },
            { L_,       "ab",   1 },
            { L_,       "ab",   5 },
            { L_,      "abc",   0 },
            { L_,      "abc",   1 },
            { L_,      "abc",   5 },
            { L_,    "ababa",   0 },
            { L_,    "ababa",   1 },
            { L_,    "ababa",   5 },
            { L_,    "abcba",   0 },
            { L_,    "abcba",   1 },
            { L_,    "abcba",   5 },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) cout << "\nTesting 'reserveCapacity(numElements)'."
                          << endl;
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE = DATA[i].d_lineNum;
                const char *const SPEC = DATA[i].d_spec_p;

                Obj mEXP;  gg(&mEXP, SPEC);  const Obj& EXP = mEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                const bsl::size_t MAX_NE = 0 < EXP.length() ? 300 : 1;

                for (bsl::size_t NE = 0; NE < MAX_NE; ++NE) {
                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj mX(&sa);  gg(&mX, SPEC);  const Obj& X = mX;

                        mX.reserveCapacity(NE);

                        LOOP_ASSERT(LINE, X == EXP);

                        bsls::Types::Int64 alloc = sa.numAllocations();

                        for (bsl::size_t j = X.length(); j < NE; ++j) {
                            mX.push_back(bsl::string("a", 1));
                        }

                        ASSERTV(LINE, NE, alloc == sa.numAllocations());
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(LINE,
                            NE,
                            allocations == defaultAllocator.numAllocations());
                }
            }
        }

        if (verbose) cout << "\nTesting 'reserveCapacity(nE, nUE)'."
                          << endl;
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE = DATA[i].d_lineNum;
                const char *const SPEC = DATA[i].d_spec_p;
                const bsl::size_t NUE  = DATA[i].d_numUniqueElements;

                Obj mEXP;  gg(&mEXP, SPEC);  const Obj& EXP = mEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                const bsl::size_t MAX_NE = 0 < NUE ? 300 : 1;

                for (bsl::size_t NE = NUE; NE < MAX_NE; ++NE) {
                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj mX(&sa);  gg(&mX, SPEC);  const Obj& X = mX;

                        mX.reserveCapacity(NE, NUE);

                        LOOP_ASSERT(LINE, X == EXP);

                        bsls::Types::Int64 alloc = sa.numAllocations();

                        {
                            bsl::size_t j = X.length();

                            while (j < NE && X.uniqueLength() < NUE) {
                                if (j) {
                                    char s[4];
                                    s[0] =
                                      static_cast<char>('0' + (j / 1000) % 10);
                                    s[1] =
                                       static_cast<char>('0' + (j / 100) % 10);
                                    s[2] =
                                       static_cast<char>('0' + (j / 10) % 10);
                                    s[3] = static_cast<char>('0' + j % 10);

                                    mX.push_back(bsl::string(s, 4));
                                }
                                else {
                                    mX.push_back(bsl::string("a", 1));
                                }

                                ++j;
                            }

                            while (j < NE) {
                                mX.push_back(bsl::string("a", 1));
                                ++j;
                            }
                        }

                        ASSERTV(LINE, NE, NUE, alloc == sa.numAllocations());
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(LINE,
                            NE,
                            NUE,
                            allocations == defaultAllocator.numAllocations());
                }
            }
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Obj mX;

            ASSERT_FAIL(mX.reserveCapacity(1));

            ASSERT_PASS(mX.reserveCapacity(0));

            ASSERT_PASS(mX.reserveCapacity(0, 0));

            ASSERT_FAIL(mX.reserveCapacity(1, 0));

            ASSERT_FAIL(mX.reserveCapacity(0, 1));

            ASSERT_PASS(mX.reserveCapacity(1, 1));

            mX.push_back(bsl::string("a", 1));

            ASSERT_PASS(mX.reserveCapacity(1));
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // REPLACE
        //   The 'replace' methods operate as expected.
        //
        // Concerns:
        //: 1 The methods are exception neutral with respect to memory
        //:   allocation.
        //:
        //: 2 The methods produce the expected value.
        //:
        //: 3 The methods are alias safe.
        //:
        //: 4 When there is initially sufficient storage in the result variable
        //:   to store the result, the methods produce the expected value.
        //:
        //: 5 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Wrap all tests with the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
        //:   macros to ensure exception neutrality with respect to memory
        //:   allocation.  (C-1)
        //:
        //: 2 Specify a set of specifications for the 'gg' function that result
        //:   in an empty object but with different capacity and bytes per
        //:   element characteristics.
        //:
        //: 3 Use the table-based approach to specify a set of test vectors.
        //:
        //: 4 For every item in the cross-product of these two sets, verify
        //:   the result for the method (the set from 2 is applied to only
        //:   the initial object).  (C-2..4)
        //:
        //: 5 Verify defensive checks are triggered for invalid values.  (C-5)
        //
        // Testing:
        //   void replace(di, value);
        //   void replace(di, const CompactedArray& srcArray, si, ne);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "REPLACE" << endl
                          << "=======" << endl;

        static const struct {
            int         d_lineNum;
            const char *d_spec_p;
        } INIT[] = {
            //line  spec
            //----  ----
            { L_,   ""             },
            { L_,   "aP"           },
            { L_,   "aabbccabcR"   },
            { L_,   "aabbccabcRaP" },
        };
        const int NUM_INIT = static_cast<int>(sizeof INIT / sizeof *INIT);

        if (verbose) cout << "\nValue replace." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec1_p;      // specification 1
                int         d_di;           // destination index
                const char  d_spec2;        // specification 2
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec1     di  spec2     exp
                //----  --------  --  --------  ----------------------------
                { L_,   "a",       0, 'b',      "b"                          },

                { L_,   "ab",      0, 'c',      "cb"                         },
                { L_,   "ab",      1, 'c',      "ac"                         },

                { L_,   "abcd",    0, 'c',      "cbcd"                       },
                { L_,   "abcd",    1, 'c',      "accd"                       },
                { L_,   "abcd",    2, 'c',      "abcd"                       },
                { L_,   "abcd",    3, 'c',      "abcc"                       },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const int         DI      = DATA[i].d_di;
                const char        SPEC2   = DATA[i].d_spec2;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                bsl::string y(&SPEC2, 1);

                Obj        mEXP;  gg(&mEXP, EXPSPEC);
                const Obj& EXP = mEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC1 = (bsl::string()
                                         + INIT[j].d_spec_p
                                         + DATA[i].d_spec1_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC1.c_str());
                        const Obj& X = mX;
                        mX.replace(DI, y);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nSub-array replace." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec1_p;      // specification 1
                int         d_di;           // destination index
                const char *d_spec2_p;      // specification 2
                int         d_si;           // source index
                int         d_ne;           // number of elements
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec1     di  spec2     si  ne  exp
                //----  --------  --  --------  --  --  --------------------
                { L_,   "a",       0, "",        0,  0, "a"                  },

                { L_,   "ab",      0, "abcd",    0,  1, "ab"                 },
                { L_,   "ab",      0, "abcd",    1,  1, "bb"                 },
                { L_,   "ab",      0, "abcd",    2,  1, "cb"                 },
                { L_,   "ab",      0, "abcd",    3,  1, "db"                 },

                { L_,   "ab",      0, "abcd",    0,  2, "ab"                 },
                { L_,   "ab",      0, "abcd",    1,  2, "bc"                 },
                { L_,   "ab",      0, "abcd",    2,  2, "cd"                 },

                { L_,   "ab",      1, "abcd",    0,  1, "aa"                 },
                { L_,   "ab",      1, "abcd",    1,  1, "ab"                 },
                { L_,   "ab",      1, "abcd",    2,  1, "ac"                 },
                { L_,   "ab",      1, "abcd",    3,  1, "ad"                 },

                { L_,   "abcd",    0, "abcd",    0,  1, "abcd"               },
                { L_,   "abcd",    0, "abcd",    1,  1, "bbcd"               },
                { L_,   "abcd",    0, "abcd",    2,  1, "cbcd"               },
                { L_,   "abcd",    0, "abcd",    3,  1, "dbcd"               },

                { L_,   "abcd",    0, "abcd",    0,  3, "abcd"               },
                { L_,   "abcd",    0, "abcd",    1,  3, "bcdd"               },

                { L_,   "abcd",    1, "abcd",    0,  3, "aabc"               },
                { L_,   "abcd",    1, "abcd",    1,  3, "abcd"               },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const char *const SPEC2   = DATA[i].d_spec2_p;
                const int         DI      = DATA[i].d_di;
                const int         SI      = DATA[i].d_si;
                const int         NE      = DATA[i].d_ne;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj        mY;    gg(&mY,   SPEC2);
                const Obj& Y = mY;
                Obj        mEXP;  gg(&mEXP, EXPSPEC);
                const Obj& EXP = mEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC1 = (bsl::string()
                                         + INIT[j].d_spec_p
                                         + DATA[i].d_spec1_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC1.c_str());
                        const Obj& X = mX;
                        mX.replace(DI, Y, SI, NE);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nAliased sub-array replace." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec_p;       // specification
                int         d_di;           // destination index
                int         d_si;           // source index
                int         d_ne;           // number of elements
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec      di  si  ne  exp
                //----  --------  --  --  --  ----------------------
                { L_,   "a",       0,  0,  0, "a"                    },
                { L_,   "ab",      0,  0,  0, "ab"                   },

                { L_,   "abcd",    0,  0,  2, "abcd"                 },
                { L_,   "abcd",    0,  1,  2, "bccd"                 },
                { L_,   "abcd",    0,  2,  2, "cdcd"                 },

                { L_,   "abcd",    2,  0,  2, "abab"                 },
                { L_,   "abcd",    2,  1,  2, "abbc"                 },
                { L_,   "abcd",    2,  2,  2, "abcd"                 },

                { L_,   "abcd",    3,  0,  1, "abca"                 },
                { L_,   "abcd",    3,  1,  1, "abcb"                 },
                { L_,   "abcd",    3,  2,  1, "abcc"                 },
                { L_,   "abcd",    3,  3,  1, "abcd"                 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const int         DI      = DATA[i].d_di;
                const int         SI      = DATA[i].d_si;
                const int         NE      = DATA[i].d_ne;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj mEXP;  gg(&mEXP, EXPSPEC);  const Obj& EXP = mEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC = (bsl::string()
                                        + INIT[j].d_spec_p
                                        + DATA[i].d_spec_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC.c_str());
                        const Obj& X = mX;
                        mX.replace(DI, X, SI, NE);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Obj        mX;  gg(&mX, "abcd");
            Obj        mY;  gg(&mY, "aa");
            const Obj& Y = mY;

            ASSERT_FAIL(mX.replace(  4,  "a"));

            ASSERT_FAIL(mX.replace(  4,  Y, 0, 1));

            ASSERT_PASS(mX.replace(  0,  "a"));

            ASSERT_PASS(mX.replace(  0,  Y, 0, 0));
            ASSERT_PASS(mX.replace(  0,  Y, 0, 1));
            ASSERT_PASS(mX.replace(  0,  Y, 0, 2));
            ASSERT_FAIL(mX.replace(  0,  Y, 0, 3));
            ASSERT_PASS(mX.replace(  0,  Y, 1, 0));
            ASSERT_PASS(mX.replace(  0,  Y, 1, 1));
            ASSERT_FAIL(mX.replace(  0,  Y, 1, 2));
            ASSERT_PASS(mX.replace(  0,  Y, 2, 0));
            ASSERT_FAIL(mX.replace(  0,  Y, 2, 1));
            ASSERT_FAIL(mX.replace(  0,  Y, 3, 0));
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // REMOVE
        //   The 'remove' methods operate as expected.
        //
        // Concerns:
        //: 1 The methods produce the expected value.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Specify a set of specifications for the 'gg' function that result
        //:   in an empty object but with different capacity and bytes per
        //:   element characteristics.
        //:
        //: 2 Use the table-based approach to specify a set of test vectors.
        //:
        //: 3 For every item in the cross-product of these two sets, verify
        //:   the result for the method (the set from 2 is applied to only
        //:   the initial object).  (C-1)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   void remove(di);
        //   void remove(di, ne);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "REMOVE" << endl
                          << "======" << endl;

        static const struct {
            int         d_lineNum;
            const char *d_spec_p;
        } INIT[] = {
            //line  spec
            //----  ----
            { L_,   ""             },
            { L_,   "aP"           },
            { L_,   "aabbccabcR"   },
            { L_,   "aabbccabcRaP" },
        };
        const int NUM_INIT = static_cast<int>(sizeof INIT / sizeof *INIT);

        if (verbose) cout << "\nValue remove." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec_p;       // specification
                int         d_di;           // destination index
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec      di  exp
                //----  --------  --  ----------------------------
                { L_,   "a",       0, ""                            },

                { L_,   "ab",      0, "b"                           },
                { L_,   "ab",      1, "a"                           },

                { L_,   "abcd",    0, "bcd"                         },
                { L_,   "abcd",    1, "acd"                         },
                { L_,   "abcd",    2, "abd"                         },
                { L_,   "abcd",    3, "abc"                         },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const int         DI      = DATA[i].d_di;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj mEXP;  gg(&mEXP, EXPSPEC);  const Obj& EXP = mEXP;

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC = (bsl::string()
                                        + INIT[j].d_spec_p
                                        + DATA[i].d_spec_p);

                    if (veryVerbose) { P(SPEC) }

                    Obj        mX;           gg(&mX, SPEC.c_str());
                    const Obj& X = mX;
                    mX.remove(DI);
                    LOOP_ASSERT(LINE, X == EXP);
                }
            }
        }

        if (verbose) cout << "\nSub-array remove." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec_p;       // specification
                int         d_di;           // destination index
                int         d_ne;           // number of elements
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec      di  ne  exp
                //----  --------  --  --  ----------------------
                { L_,   "a",       0,  0, "a"                    },
                { L_,   "ab",      0,  0, "ab"                   },

                { L_,   "abcd",    0,  1, "bcd"                  },
                { L_,   "abcd",    1,  1, "acd"                  },
                { L_,   "abcd",    2,  1, "abd"                  },
                { L_,   "abcd",    3,  1, "abc"                  },

                { L_,   "abcd",    0,  2, "cd"                   },
                { L_,   "abcd",    1,  2, "ad"                   },
                { L_,   "abcd",    2,  2, "ab"                   },

                { L_,   "abcd",    0,  3, "d"                    },
                { L_,   "abcd",    1,  3, "a"                    },

                { L_,   "abcd",    0,  4, ""                     },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const int         DI      = DATA[i].d_di;
                const int         NE      = DATA[i].d_ne;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj mEXP;  gg(&mEXP, EXPSPEC);  const Obj& EXP = mEXP;

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC = (bsl::string()
                                        + INIT[j].d_spec_p
                                        + DATA[i].d_spec_p);

                    if (veryVerbose) { P(SPEC) }

                    Obj        mX;           gg(&mX, SPEC.c_str());
                    const Obj& X = mX;
                    mX.remove(DI, NE);
                    LOOP_ASSERT(LINE, X == EXP);
                }
            }
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Obj mX;  const Obj& X = mX;  gg(&mX, "abc");
            Obj mT;

            mT = X;  ASSERT_FAIL(mT.remove(4));

            mT = X;  ASSERT_PASS(mT.remove(0, 0));
            mT = X;  ASSERT_PASS(mT.remove(0, 1));
            mT = X;  ASSERT_PASS(mT.remove(0, 2));
            mT = X;  ASSERT_PASS(mT.remove(0, 3));
            mT = X;  ASSERT_FAIL(mT.remove(0, 4));
            mT = X;  ASSERT_PASS(mT.remove(1, 0));
            mT = X;  ASSERT_PASS(mT.remove(1, 1));
            mT = X;  ASSERT_PASS(mT.remove(1, 2));
            mT = X;  ASSERT_FAIL(mT.remove(1, 3));
            mT = X;  ASSERT_PASS(mT.remove(2, 0));
            mT = X;  ASSERT_PASS(mT.remove(2, 1));
            mT = X;  ASSERT_FAIL(mT.remove(2, 2));
            mT = X;  ASSERT_PASS(mT.remove(3, 0));
            mT = X;  ASSERT_FAIL(mT.remove(3, 1));
            mT = X;  ASSERT_FAIL(mT.remove(4, 0));
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // INSERT
        //   The 'insert' methods operate as expected.
        //
        // Concerns:
        //: 1 The methods are exception neutral with respect to memory
        //:   allocation.
        //:
        //: 2 The methods produce the expected value.
        //:
        //: 3 The methods are alias safe.
        //:
        //: 4 When there is initially sufficient storage in the result variable
        //:   to store the result, the methods produce the expected value.
        //:
        //: 5 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Wrap all tests with the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
        //:   macros to ensure exception neutrality with respect to memory
        //:   allocation.  (C-1)
        //:
        //: 2 Specify a set of specifications for the 'gg' function that result
        //:   in an empty object but with different capacity and bytes per
        //:   element characteristics.
        //:
        //: 3 Use the table-based approach to specify a set of test vectors.
        //:
        //: 4 For every item in the cross-product of these two sets, verify
        //:   the result for the method (the set from 2 is applied to only
        //:   the initial object).  (C-2..4)
        //:
        //: 5 Verify defensive checks are triggered for invalid values.  (C-5)
        //
        // Testing:
        //   void insert(di, value);
        //   void insert(di, const CompactedArray& srcArray);
        //   void insert(di, const CompactedArray& srcArray, si, ne);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "INSERT" << endl
                          << "======" << endl;

        static const struct {
            int         d_lineNum;
            const char *d_spec_p;
        } INIT[] = {
            //line  spec
            //----  ----
            { L_,   ""             },
            { L_,   "aP"           },
            { L_,   "aabbccabcR"   },
            { L_,   "aabbccabcRaP" },
        };
        const int NUM_INIT = static_cast<int>(sizeof INIT / sizeof *INIT);

        if (verbose) cout << "\nValue insert." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec1_p;      // specification 1
                int         d_di;           // destination index
                const char  d_spec2;        // specification 2
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec1     di  spec2     exp
                //----  --------  --  --------  ----------------------------
                { L_,   "",        0, 'a',      "a"                          },

                { L_,   "a",       0, 'b',      "ba"                         },
                { L_,   "a",       1, 'b',      "ab"                         },

                { L_,   "ab",      0, 'c',      "cab"                        },
                { L_,   "ab",      1, 'c',      "acb"                        },
                { L_,   "ab",      2, 'c',      "abc"                        },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const int         DI      = DATA[i].d_di;
                const char        SPEC2   = DATA[i].d_spec2;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                bsl::string y(&SPEC2, 1);

                Obj        mEXP;  gg(&mEXP, EXPSPEC);
                const Obj& EXP =  mEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC1 = (bsl::string()
                                         + INIT[j].d_spec_p
                                         + DATA[i].d_spec1_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC1.c_str());
                        const Obj& X = mX;
                        mX.insert(DI, y);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nFull array insert." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec1_p;      // specification 1
                int         d_di;           // destination index
                const char *d_spec2_p;      // specification 2
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec1     di  spec2     exp
                //----  --------  --  --------  ----------------------------
                { L_,   "",        0, "",       ""                           },

                { L_,   "",        0, "a",      "a"                          },
                { L_,   "a",       1, "",       "a"                          },

                { L_,   "",        0, "ab",     "ab"                         },
                { L_,   "a",       1, "b",      "ab"                         },
                { L_,   "ab",      2, "",       "ab"                         },

                { L_,   "",        0, "abc",    "abc"                        },
                { L_,   "a",       1, "bc",     "abc"                        },
                { L_,   "ab",      2, "c",      "abc"                        },
                { L_,   "abc",     3, "",       "abc"                        },

                { L_,   "",        0, "abcd",   "abcd"                       },
                { L_,   "a",       1, "bcd",    "abcd"                       },
                { L_,   "ab",      2, "cd",     "abcd"                       },
                { L_,   "abc",     3, "d",      "abcd"                       },
                { L_,   "abcd",    4, "",       "abcd"                       },

                { L_,   "ab",      0, "abcd",   "abcdab"                     },
                { L_,   "ab",      1, "abcd",   "aabcdb"                     },
                { L_,   "ab",      2, "abcd",   "ababcd"                     },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const int         DI      = DATA[i].d_di;
                const char *const SPEC2   = DATA[i].d_spec2_p;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj        mY;    gg(&mY,   SPEC2);
                const Obj& Y = mY;
                Obj        mEXP;  gg(&mEXP, EXPSPEC);
                const Obj& EXP = mEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC1 = (bsl::string()
                                         + INIT[j].d_spec_p
                                         + DATA[i].d_spec1_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC1.c_str());
                        const Obj& X = mX;
                        mX.insert(DI, Y);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nSub-array insert." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec1_p;      // specification 1
                int         d_di;           // destination index
                const char *d_spec2_p;      // specification 2
                int         d_si;           // source index
                int         d_ne;           // number of elements
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec1     di  spec2     si  ne  exp
                //----  --------  --  --------  --  --  --------------------
                { L_,   "",        0, "",        0,  0, ""                   },
                { L_,   "a",       1, "",        0,  0, "a"                  },
                { L_,   "ab",      2, "",        0,  0, "ab"                 },

                { L_,   "",        0, "abcd",    0,  0, ""                   },
                { L_,   "",        0, "abcd",    0,  1, "a"                  },
                { L_,   "",        0, "abcd",    0,  2, "ab"                 },
                { L_,   "",        0, "abcd",    0,  3, "abc"                },
                { L_,   "",        0, "abcd",    0,  4, "abcd"               },

                { L_,   "ab",      0, "abcd",    1,  2, "bcab"               },
                { L_,   "ab",      0, "abcd",    2,  2, "cdab"               },

                { L_,   "ab",      1, "abcd",    1,  2, "abcb"               },
                { L_,   "ab",      1, "abcd",    2,  2, "acdb"               },

                { L_,   "ab",      2, "abcd",    1,  2, "abbc"               },
                { L_,   "ab",      2, "abcd",    2,  2, "abcd"               },

                { L_,   "ab",      2, "abcd",    3,  0, "ab"                 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const char *const SPEC2   = DATA[i].d_spec2_p;
                const int         DI      = DATA[i].d_di;
                const int         SI      = DATA[i].d_si;
                const int         NE      = DATA[i].d_ne;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj        mY;    gg(&mY,   SPEC2);
                const Obj& Y = mY;
                Obj        mEXP;  gg(&mEXP, EXPSPEC);
                const Obj& EXP = mEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC1 = (bsl::string()
                                         + INIT[j].d_spec_p
                                         + DATA[i].d_spec1_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC1.c_str());
                        const Obj& X = mX;
                        mX.insert(DI, Y, SI, NE);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nAliased full array insert." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec_p;       // specification
                int         d_di;           // destination index
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec      di  exp
                //----  --------  --  ------------------------------
                { L_,   "",        0, ""                              },

                { L_,   "abcd",    0, "abcdabcd"                      },
                { L_,   "abcd",    1, "aabcdbcd"                      },
                { L_,   "abcd",    2, "ababcdcd"                      },
                { L_,   "abcd",    3, "abcabcdd"                      },
                { L_,   "abcd",    4, "abcdabcd"                      },

                { L_,   "a",       1, "aa"                            },
                { L_,   "ab",      2, "abab"                          },
                { L_,   "abc",     3, "abcabc"                        },
                { L_,   "abcd",    4, "abcdabcd"                      },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const int         DI      = DATA[i].d_di;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj mEXP;  gg(&mEXP, EXPSPEC);  const Obj& EXP = mEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC = (bsl::string()
                                        + INIT[j].d_spec_p
                                        + DATA[i].d_spec_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC.c_str());
                        const Obj& X = mX;
                        mX.insert(DI, X);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nAliased sub-array insert." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec_p;       // specification
                int         d_di;           // destination index
                int         d_si;           // source index
                int         d_ne;           // number of elements
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec      di  si  ne  exp
                //----  --------  --  --  --  ----------------------
                { L_,   "",        0,  0,  0, ""                     },
                { L_,   "a",       0,  0,  0, "a"                    },
                { L_,   "ab",      0,  0,  0, "ab"                   },

                { L_,   "abcd",    0,  0,  2, "ababcd"               },
                { L_,   "abcd",    0,  1,  2, "bcabcd"               },
                { L_,   "abcd",    0,  2,  2, "cdabcd"               },

                { L_,   "abcd",    2,  0,  2, "ababcd"               },
                { L_,   "abcd",    2,  1,  2, "abbccd"               },
                { L_,   "abcd",    2,  2,  2, "abcdcd"               },

                { L_,   "abcd",    4,  0,  1, "abcda"                },
                { L_,   "abcd",    4,  1,  1, "abcdb"                },
                { L_,   "abcd",    4,  2,  1, "abcdc"                },
                { L_,   "abcd",    4,  3,  1, "abcdd"                },

                { L_,   "abcd",    4,  0,  2, "abcdab"               },
                { L_,   "abcd",    4,  1,  2, "abcdbc"               },
                { L_,   "abcd",    4,  2,  2, "abcdcd"               },

                { L_,   "abcd",    4,  0,  3, "abcdabc"              },
                { L_,   "abcd",    4,  1,  3, "abcdbcd"              },

                { L_,   "abcd",    4,  0,  4, "abcdabcd"             },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const int         DI      = DATA[i].d_di;
                const int         SI      = DATA[i].d_si;
                const int         NE      = DATA[i].d_ne;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj mEXP;  gg(&mEXP, EXPSPEC);  const Obj& EXP = mEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC = (bsl::string()
                                        + INIT[j].d_spec_p
                                        + DATA[i].d_spec_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC.c_str());
                        const Obj& X = mX;
                        mX.insert(DI, X, SI, NE);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Obj        mX;
            Obj        mY;  gg(&mY, "aa");
            const Obj& Y =  mY;

            const bsl::string Z("a", 1);

            ASSERT_FAIL(mX.insert(  1,  Z));

            ASSERT_FAIL(mX.insert(  1,  Y));

            ASSERT_FAIL(mX.insert(  1,  Y, 0, 0));

            ASSERT_PASS(mX.insert(  0,  Z));

            ASSERT_PASS(mX.insert(  0,  Y));

            ASSERT_PASS(mX.insert(  0,  Y, 0, 0));
            ASSERT_PASS(mX.insert(  0,  Y, 0, 1));
            ASSERT_PASS(mX.insert(  0,  Y, 0, 2));
            ASSERT_FAIL(mX.insert(  0,  Y, 0, 3));
            ASSERT_PASS(mX.insert(  0,  Y, 1, 0));
            ASSERT_PASS(mX.insert(  0,  Y, 1, 1));
            ASSERT_FAIL(mX.insert(  0,  Y, 1, 2));
            ASSERT_PASS(mX.insert(  0,  Y, 2, 0));
            ASSERT_FAIL(mX.insert(  0,  Y, 2, 1));
            ASSERT_FAIL(mX.insert(  0,  Y, 3, 0));
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // APPEND
        //   The 'append' methods operate as expected.
        //
        // Concerns:
        //: 1 The methods are exception neutral with respect to memory
        //:   allocation.
        //:
        //: 2 The methods produce the expected value.
        //:
        //: 3 The methods are alias safe.
        //:
        //: 4 When there is initially sufficient storage in the result variable
        //:   to store the result, the methods produce the expected value.
        //:
        //: 5 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Wrap all tests with the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
        //:   macros to ensure exception neutrality with respect to memory
        //:   allocation.  (C-1)
        //:
        //: 2 Specify a set of specifications for the 'gg' function that result
        //:   in an empty object but with different capacity and bytes per
        //:   element characteristics.
        //:
        //: 3 Use the table-based approach to specify a set of test vectors.
        //:
        //: 4 For every item in the cross-product of these two sets, verify
        //:   the result for the method (the set from 2 is applied to only
        //:   the initial object).  (C-2..4)
        //:
        //: 5 Verify defensive checks are triggered for invalid values.  (C-5)
        //
        // Testing:
        //   void push_back(TYPE value);
        //   void append(const CompactedArray& srcArray);
        //   void append(const CompactedArray& srcArray, si, ne);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "APPEND" << endl
                          << "======" << endl;

        static const struct {
            int         d_lineNum;
            const char *d_spec_p;
        } INIT[] = {
            //line  spec
            //----  ----
            { L_,   ""             },
            { L_,   "aP"           },
            { L_,   "aabbccabcR"   },
            { L_,   "aabbccabcRaP" },
        };
        const int NUM_INIT = static_cast<int>(sizeof INIT / sizeof *INIT);

        if (verbose) cout << "\nValue append ('push_back')." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec1_p;      // specification 1
                const char  d_spec2;        // specification 2
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec1     spec2     exp
                //----  --------  --------  ----------------------------
                { L_,   "",       'a',      "a"                           },
                { L_,   "",       'b',      "b"                           },
                { L_,   "",       'c',      "c"                           },

                { L_,   "a",      'a',      "aa"                          },
                { L_,   "a",      'b',      "ab"                          },
                { L_,   "a",      'c',      "ac"                          },

                { L_,   "b",      'a',      "ba"                          },
                { L_,   "b",      'b',      "bb"                          },
                { L_,   "b",      'c',      "bc"                          },

                { L_,   "ac",     'a',      "aca"                         },
                { L_,   "ac",     'b',      "acb"                         },
                { L_,   "ac",     'c',      "acc"                         },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const char        SPEC2   = DATA[i].d_spec2;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                bsl::string y(&SPEC2, 1);

                Obj mEXP;  gg(&mEXP, EXPSPEC);  const Obj& EXP = mEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC1 = (bsl::string()
                                         + INIT[j].d_spec_p
                                         + DATA[i].d_spec1_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC1.c_str());
                        const Obj& X = mX;
                        mX.push_back(y);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nFull array append." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec1_p;      // specification 1
                const char *d_spec2_p;      // specification 2
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec1     spec2     exp
                //----  --------  --------  ------------------------------
                { L_,   "",       "",       ""                              },

                { L_,   "",       "a",      "a"                             },
                { L_,   "a",      "",       "a"                             },

                { L_,   "",       "ab",     "ab"                            },
                { L_,   "a",      "b",      "ab"                            },
                { L_,   "ab",     "",       "ab"                            },

                { L_,   "",       "abc",    "abc"                           },
                { L_,   "a",      "bc",     "abc"                           },
                { L_,   "ab",     "c",      "abc"                           },
                { L_,   "abc",    "",       "abc"                           },

                { L_,   "",       "aaaa",   "aaaa"                          },
                { L_,   "a",      "aaa",    "aaaa"                          },
                { L_,   "aa",     "aa",     "aaaa"                          },
                { L_,   "aaa",    "a",      "aaaa"                          },
                { L_,   "aaaa",   "",       "aaaa"                          },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const char *const SPEC2   = DATA[i].d_spec2_p;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj        mY;    gg(&mY,   SPEC2);
                const Obj& Y = mY;
                Obj        mEXP;  gg(&mEXP, EXPSPEC);
                const Obj& EXP = mEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC1 = (bsl::string()
                                         + INIT[j].d_spec_p
                                         + DATA[i].d_spec1_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC1.c_str());
                        const Obj& X = mX;

                        mX.append(Y);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nSub-array append." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec1_p;      // specification 1
                const char *d_spec2_p;      // specification 2
                int         d_si;           // source index
                int         d_ne;           // number of elements
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec1     spec2     si  ne  exp
                //----  --------  --------  --  --  ----------------------
                { L_,   "",       "",        0,  0, ""                     },
                { L_,   "a",      "",        0,  0, "a"                    },
                { L_,   "ab",     "",        0,  0, "ab"                   },

                { L_,   "",       "abcd",    0,  0, ""                     },
                { L_,   "",       "abcd",    0,  1, "a"                    },
                { L_,   "",       "abcd",    0,  2, "ab"                   },
                { L_,   "",       "abcd",    0,  3, "abc"                  },
                { L_,   "",       "abcd",    0,  4, "abcd"                 },

                { L_,   "ab",     "abcd",    1,  2, "abbc"                 },
                { L_,   "ab",     "abcd",    2,  2, "abcd"                 },

                { L_,   "ab",     "abcd",    3,  0, "ab"                   },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const char *const SPEC2   = DATA[i].d_spec2_p;
                const int         SI      = DATA[i].d_si;
                const int         NE      = DATA[i].d_ne;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj        mY;    gg(&mY,   SPEC2);
                const Obj& Y = mY;
                Obj        mEXP;  gg(&mEXP, EXPSPEC);
                const Obj& EXP = mEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC1 = (bsl::string()
                                         + INIT[j].d_spec_p
                                         + DATA[i].d_spec1_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC1.c_str());
                        const Obj& X = mX;
                        mX.append(Y, SI, NE);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nAliased full array append." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec_p;       // specification
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec      exp
                //----  --------  ------------------------------
                { L_,   "",       ""                              },

                { L_,   "a",      "aa"                            },
                { L_,   "ab",     "abab"                          },
                { L_,   "abc",    "abcabc"                        },
                { L_,   "abcd",   "abcdabcd"                      },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj mEXP;  gg(&mEXP, EXPSPEC);  const Obj& EXP = mEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC = (bsl::string()
                                        + INIT[j].d_spec_p
                                        + DATA[i].d_spec_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC.c_str());
                        const Obj& X = mX;
                        mX.append(X);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nAliased sub-array append." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                const char *d_spec_p;       // specification
                int         d_si;           // source index
                int         d_ne;           // number of elements
                const char *d_exp_p;        // expected output specification
            } DATA[] = {
                //line  spec      si  ne  exp
                //----  --------  --  --  ----------------------
                { L_,   "",       0,  0, ""                     },
                { L_,   "a",      0,  0, "a"                    },
                { L_,   "ab",     0,  0, "ab"                   },

                { L_,   "abcd",   0,  1, "abcda"                },
                { L_,   "abcd",   1,  1, "abcdb"                },
                { L_,   "abcd",   2,  1, "abcdc"                },
                { L_,   "abcd",   3,  1, "abcdd"                },

                { L_,   "abcd",   0,  2, "abcdab"               },
                { L_,   "abcd",   1,  2, "abcdbc"               },
                { L_,   "abcd",   2,  2, "abcdcd"               },

                { L_,   "abcd",   0,  3, "abcdabc"              },
                { L_,   "abcd",   1,  3, "abcdbcd"              },

                { L_,   "abcd",   0,  4, "abcdabcd"             },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const int         SI      = DATA[i].d_si;
                const int         NE      = DATA[i].d_ne;
                const char *const EXPSPEC = DATA[i].d_exp_p;

                Obj mEXP;  gg(&mEXP, EXPSPEC);  const Obj& EXP = mEXP;

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                for (int j = 0; j < NUM_INIT; ++j) {
                    bsl::string SPEC = (bsl::string()
                                        + INIT[j].d_spec_p
                                        + DATA[i].d_spec_p);

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                        Obj        mX(&sa);           gg(&mX, SPEC.c_str());
                        const Obj& X = mX;
                        mX.append(X, SI, NE);
                        LOOP_ASSERT(LINE, X == EXP);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                }

                LOOP_ASSERT(LINE,
                            allocations == defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

            Obj mX;
            Obj mY;

            gg(&mY, "aa");

            const Obj& Y = mY;

            ASSERT_PASS(mX.append(  Y, 0, 0));
            ASSERT_PASS(mX.append(  Y, 0, 1));
            ASSERT_PASS(mX.append(  Y, 0, 2));
            ASSERT_FAIL(mX.append(  Y, 0, 3));
            ASSERT_PASS(mX.append(  Y, 1, 0));
            ASSERT_PASS(mX.append(  Y, 1, 1));
            ASSERT_FAIL(mX.append(  Y, 1, 2));
            ASSERT_PASS(mX.append(  Y, 2, 0));
            ASSERT_FAIL(mX.append(  Y, 2, 1));
            ASSERT_FAIL(mX.append(  Y, 3, 0));
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTION
        //   Ensure that, when member and free 'swap' are implemented, we can
        //   exchange the values of any two objects that use the same
        //   allocator.
        //
        // Concerns:
        //: 1 Both functions exchange the values of the (two) supplied objects.
        //:
        //: 2 The common object allocator address in both objects is unchanged.
        //:
        //: 3 Neither function allocates memory from any allocator.
        //:
        //: 4 Both functions have standard signatures.
        //:
        //: 5 Using either function to swap an object with itself does not
        //:   affect the value of the object (alias-safety).
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the addresses of the 'swap' member and free functions defined
        //:   in this component to initialize, respectively, pointers to member
        //:   and free functions having the appropriate structures.  (C-4)
        //:
        //: 2 Construct a 'bslma::TestAllocator' object and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of widely varying object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //: 4 For each row 'R1' in the table of P-3: (C-1..3, 5)
        //:
        //:   1 Construct a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor to create a modifiable 'Obj', 'mW',
        //:     using 'oa' and having the value described by 'R1'; also use the
        //:     copy constructor to create a 'const' 'Obj' 'XX' (using a
        //:     "scratch" allocator) from 'mW'.
        //:
        //:   3 Use the member and free 'swap' functions to swap the value of
        //:     'mW' with itself, and then verify: (C-2, 3, 5)
        //:
        //:     1 The value is unchanged.
        //:
        //:     2 The object allocator address is unchanged.
        //:
        //:     3 There was no additional object memory allocation.
        //:
        //:   4 For each row 'R2' in the table of P-3:
        //:
        //:     1 Use the copy constructor to create a modifiable 'Obj', 'mX',
        //:       (using 'oa') from 'XX' (P-4.2).
        //:
        //:     2 Use the value constructor to create a modifiable 'Obj', 'mY',
        //:       using 'oa' and having the value described by 'R2'; also use
        //:       the copy constructor to create a 'const' 'Obj' 'YY' (using a
        //:       "scratch" allocator) from 'Y'.
        //:
        //:     3 Use the member 'swap' function to swap the values of 'mX' and
        //:       'mY', and then verify: (C-1..3)
        //:
        //:       1 The values have been exchanged.
        //:
        //:       2 The object allocator addresses are unchanged.
        //:
        //:       3 There was no additional object memory allocation.
        //:
        //:     4 Use the free 'swap' function to again swap the values of 'mX'
        //:       and 'mY', and then verify: (C-1..3)
        //:
        //:       1 The values have been exchanged.
        //:
        //:       2 The object allocator addresses are unchanged.
        //:
        //:       3 There was no additional object memory allocation.
        //:
        //: 5 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //:
        //: 6 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when an attempt is made to swap objects that do not
        //:   refer to the same allocator, but not when the allocators are the
        //:   same (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
        //
        // Testing:
        //   void swap(CompactedArray& other);
        //   void swap(CompactedArray& a, CompactedArray& b);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SWAP MEMBER AND FREE FUNCTION" << endl
                          << "=============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each function to a variable." << endl;
        {
            using namespace bdlc;

            typedef void (Obj::*funcPtr)(Obj&);

            typedef void (*freeFuncPtr)(Obj&, Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     memberSwap = &Obj::swap;
            freeFuncPtr freeSwap   = swap;

            // Quash unused variable warnings.

            Obj a;
            Obj b;
            (a.*memberSwap)(b);
            freeSwap(a, b);
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::Default::setDefaultAllocatorRaw(&da);

        if (verbose) cout << "\nCreate a table of distinct object values."
                                                                       << endl;

        static const struct {
            int         d_line;           // source line number
            const char *d_spec_p;
        } DATA[] = {
            //line  spec
            //----  -------------------
            { L_,                    "" },
            { L_,                   "a" },
            { L_,                  "aa" },
            { L_,                 "aaa" },
            { L_,                   "b" },
            { L_,                  "bb" },
            { L_,                 "bbb" },
            { L_,                "aaab" },
            { L_,                "aaba" },
            { L_,                "abaa" },
            { L_,                "baaa" },
            { L_,           "abcabcabc" },
            { L_,           "cbaabcbac" },
            { L_, "abcdefghijcbaabcbac" }
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const char *const SPEC1   = DATA[ti].d_spec_p;

            bslma::TestAllocator oa("object",       veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj        mW(&oa);
            const Obj& W = gg(&mW, SPEC1);
            const Obj  XX(W, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P_(W) P_(XX) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE1, W, Obj(&scratch), W == Obj(&scratch));
                firstFlag = false;
            }

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                mW.swap(mW);

                LOOP3_ASSERT(LINE1, W, XX, W == XX);
                LOOP_ASSERT(LINE1, &oa == W.allocator());
                LOOP_ASSERT(LINE1, oam.isTotalSame());
            }

            // free function 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                swap(mW, mW);

                LOOP3_ASSERT(LINE1, W, XX, W == XX);
                LOOP_ASSERT(LINE1, &oa == W.allocator());
                LOOP_ASSERT(LINE1, oam.isTotalSame());
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const char *const SPEC2   = DATA[tj].d_spec_p;

                Obj        mX(XX, &oa);
                const Obj& X = mX;

                Obj        mY(&oa);
                const Obj& Y = gg(&mY, SPEC2);
                const Obj  YY(Y, &scratch);

                if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P_(YY) }

                // member 'swap'
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    mX.swap(mY);

                    LOOP4_ASSERT(LINE1, LINE2, X, YY, X == YY);
                    LOOP4_ASSERT(LINE1, LINE2, Y, XX, Y == XX);
                    LOOP2_ASSERT(LINE1, LINE2, &oa == X.allocator());
                    LOOP2_ASSERT(LINE1, LINE2, &oa == Y.allocator());
                    LOOP2_ASSERT(LINE1, LINE2, oam.isTotalSame());
                }

                // free function 'swap'
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    swap(mX, mY);

                    LOOP4_ASSERT(LINE1, LINE2, X, XX, X == XX);
                    LOOP4_ASSERT(LINE1, LINE2, Y, YY, Y == YY);
                    LOOP2_ASSERT(LINE1, LINE2, &oa == X.allocator());
                    LOOP2_ASSERT(LINE1, LINE2, &oa == Y.allocator());
                    LOOP2_ASSERT(LINE1, LINE2, oam.isTotalSame());
                }
            }
        }

        // Verify no memory is allocated from the default allocator.

        LOOP_ASSERT(da.numBlocksTotal(), !da.numBlocksTotal());

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'swap' member function" << endl;
            {
                bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
                bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                Obj mA(&oa1);
                Obj mB(&oa1);
                Obj mC(&oa1);
                Obj mZ(&oa2);

                ASSERT_PASS(mA.swap(mB));
                ASSERT_FAIL(mC.swap(mZ));
            }

            if (veryVerbose) cout << "\t'swap' free function" << endl;
            {
                bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
                bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                Obj mA(&oa1);
                Obj mB(&oa1);
                Obj mC(&oa1);
                Obj mZ(&oa2);

                ASSERT_SAFE_PASS(swap(mA, mB));
                ASSERT_SAFE_FAIL(swap(mC, mZ));
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING BDEX STREAMING
        //
        // The functionality was omitted.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING BDEX STREAMING" << endl
                          << "======================" << endl;
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // COPY ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any target object
        //:   to that of any source object.
        //:
        //: 2 The address of the target object's allocator is unchanged.
        //:
        //: 3 Any memory allocation is from the target object's allocator.
        //:
        //: 4 The signature and return type are standard.
        //:
        //: 5 The reference returned is to the target object (i.e., '*this').
        //:
        //: 6 The value of the source object is not modified.
        //:
        //: 7 The allocator of the source object is not modified.
        //:
        //: 8 Any memory allocation is exception neutral.
        //:
        //: 9 Assigning an object to itself behaves as expected (alias-safety).
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a function pointer
        //:   having the appropriate structure for the copy assignment operator
        //:   defined in this component.  (C-4)
        //:
        //: 2 Construct a 'bslma::TestAllocator' object and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of valid object values (one per row) in terms of
        //:     their individual attributes, including (a) first, the default
        //:     value, (b) boundary values corresponding to every range of
        //:     values that each individual attribute can independently attain,
        //:     and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //: 4 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3: (C-1..3, 5..9)
        //:
        //:   1 Use the value constructor to create two 'const' 'Obj', 'Z' and
        //:     'ZZ', each having the value 'V' and using a "scratch"
        //:     allocator.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2):
        //:
        //:     1 Construct a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:     2 Use the value constructor to create a modifiable 'Obj', 'mX',
        //:       using 'oa' and having the value 'W'.
        //:
        //:     3 Use the equality comparison operators to verify that the two
        //:       objects, 'mX' and 'Z', are initially equal if and only if the
        //:       table indices from P-4 and P-4.2 are the same.
        //:
        //:     4 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:       (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:
        //:     5 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     6 Use the equality comparison operators to verify that: (C-1,
        //:       6)
        //:
        //:       1 The target object, 'mX', now has the same value as 'Z'.
        //:
        //:       2 'Z' still has the same value as 'ZZ'.
        //:
        //:     7 Use the 'allocator' accessor of both 'mX' and 'Z' to verify
        //:       that both object allocators are unchanged.  (C-2, 7)
        //:
        //:     8 Use appropriate test allocators to verify that: (C-3, 8)
        //:
        //:       1 No additional memory is allocated by the source object.
        //:
        //:       2 All object memory is released when the object is destroyed.
        //:
        //: 5 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-4.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-3: (C-1..3, 5..10)
        //:
        //:   1 Construct a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor to create a modifiable 'Obj' 'mX'
        //:     using 'oa' and a 'const' 'Obj' 'ZZ' (using a distinct "scratch"
        //:     allocator).
        //:
        //:   3 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   4 Use the equality comparison operators to verify that the target
        //:     object, 'mX', has the same value as 'ZZ'.
        //:
        //:   5 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:     (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:     (C-9)
        //:
        //:   6 Verify that the address of the return value is the same as that
        //:     of 'mX'.  (C-5)
        //:
        //:   7 Use the equality comparison operators to verify that the target
        //:     object, 'mX', still has the same value as 'ZZ'.  (C-10)
        //:
        //:   8 Use the 'allocator' accessor of 'mX' to verify that it is still
        //:     the object allocator.  (C-2)
        //:
        //:   9 Use appropriate test allocators to verify that: (C-3, 8)
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
        //   CompactedArray& operator=(const CompactedArray& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            // Quash unused variable warning.

            Obj a;
            Obj b;
            (a.*operatorAssignment)(b);
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::Default::setDefaultAllocatorRaw(&da);

        if (verbose) cout <<
        "\nCreate a table of distinct object values and expected memory usage."
                                                                       << endl;

        static const struct {
            int         d_line;           // source line number
            const char *d_spec_p;
        } DATA[] = {
            //line  spec
            //----  -------------------
            { L_,                    "" },
            { L_,                   "a" },
            { L_,                  "aa" },
            { L_,                 "aaa" },
            { L_,                   "b" },
            { L_,                  "bb" },
            { L_,                 "bbb" },
            { L_,                "aaab" },
            { L_,                "aaba" },
            { L_,                "abaa" },
            { L_,                "baaa" },
            { L_,           "abcabcabc" },
            { L_,           "cbaabcbac" },
            { L_, "abcdefghijcbaabcbac" }
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const char *const SPEC1   = DATA[ti].d_spec_p;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mZ(&scratch);   const Obj& Z = gg(&mZ,  SPEC1);

            Obj mZZ(&scratch);  const Obj& ZZ = gg(&mZZ, SPEC1);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P_(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(LINE1, Z, Obj(&scratch), Z == Obj(&scratch));
                firstFlag = false;
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const char *const SPEC2   = DATA[tj].d_spec_p;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(&oa);  const Obj& X = gg(&mX, SPEC2);

                    if (veryVerbose) { T_ P_(LINE2) P_(X) }

                    LOOP4_ASSERT(LINE1, LINE2, X, Z,
                                 (X == Z) == (LINE1 == LINE2));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    Obj *mR = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        mR = &(mX = Z);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    // Verify the address of the return value.

                    LOOP4_ASSERT(LINE1, LINE2, mR, &mX, mR == &mX);

                    LOOP4_ASSERT(LINE1, LINE2,  X, Z,  X == Z);
                    LOOP4_ASSERT(LINE1, LINE2, ZZ, Z, ZZ == Z);

                    LOOP4_ASSERT(LINE1, LINE2, &oa, X.allocator(),
                                 &oa == X.allocator());
                    LOOP4_ASSERT(LINE1, LINE2, &scratch, Z.allocator(),
                                 &scratch == Z.allocator());

                    LOOP2_ASSERT(LINE1, LINE2, sam.isInUseSame());

                    LOOP2_ASSERT(LINE1, LINE2, !da.numBlocksTotal());
                }

                // Verify all memory is released on object destruction.

                LOOP3_ASSERT(LINE1, LINE2, da.numBlocksInUse(),
                             0 == da.numBlocksInUse());
                LOOP3_ASSERT(LINE1, LINE2, oa.numBlocksInUse(),
                             0 == oa.numBlocksInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(&oa);   const Obj& X = gg(&mX, SPEC1);

                Obj mZZ(&oa);  const Obj& ZZ = gg(&mZZ, SPEC1);

                const Obj& Z = mX;

                LOOP3_ASSERT(LINE1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                Obj *mR = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    mR = &(mX = Z);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                // Verify the address of the return value.

                LOOP3_ASSERT(LINE1, mR, &mX, mR == &mX);

                LOOP3_ASSERT(LINE1, mR, &X, mR == &X);

                LOOP3_ASSERT(LINE1, Z, ZZ, ZZ == Z);

                LOOP3_ASSERT(LINE1, &oa, Z.allocator(), &oa == Z.allocator());

                if (0 == ti) {  // Empty, no allocation.
                    LOOP_ASSERT(LINE1, oam.isInUseSame());
                }

                LOOP_ASSERT(LINE1, sam.isInUseSame());

                LOOP_ASSERT(LINE1, !da.numBlocksTotal());
            }

            // Verify all memory is released on object destruction.

            LOOP2_ASSERT(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
            LOOP2_ASSERT(LINE1, da.numBlocksInUse(), 0 == da.numBlocksInUse());
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTION
        //   Verify the 'g' functions work properly.
        //
        // The function was omitted.
        //
        // Testing:
        //   CompactedArray g(const char *spec);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GENERATOR FUNCTION" << endl
                          << "==================" << endl;
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor (with or without a supplied allocator)
        //:   creates an object having the same value as the supplied original
        //:   object.
        //:
        //: 2 If an allocator is *not* supplied to the copy constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object (i.e., the
        //:   allocator of the original object is never copied).
        //:
        //: 3 If an allocator is supplied to the copy constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the copy constructor has no effect on
        //:   subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator, which is
        //:   returned by the 'allocator' accessor method.
        //:
        //: 7 There is no temporary memory allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 The original object is passed as a reference providing
        //:   non-modifiable access to the object.
        //:
        //:10 The value of the original object is unchanged.
        //:
        //:11 The allocator address of the original object is unchanged.
        //:
        //:12 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of widely varying object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1: (C-1..12)
        //:
        //:   1 Use the value constructor to create two 'const' 'Obj', 'Z' and
        //:     'ZZ', each having the value 'V' and using a "scratch"
        //:     allocator.
        //:
        //:   2 Execute an inner loop creating three distinct objects in turn,
        //:     each using the copy constructor on 'Z' from P-2.1, but
        //:     configured differently: (a) without passing an allocator, (b)
        //:     passing a null allocator address explicitly, and (c) passing
        //:     the address of a test allocator distinct from the default.
        //:
        //:   3 For each of these three iterations (P-2.2): (C-1..12)
        //:
        //:     1 Construct three 'bslma::TestAllocator' objects and install
        //:       one as the current default allocator (note that a ubiquitous
        //:       test allocator is already installed as the global allocator).
        //:
        //:     2 Use the copy constructor to dynamically create an object 'X',
        //:       supplying it the 'const' object 'Z' (see P-2.1), configured
        //:       appropriately (see P-2.2) using a distinct test allocator for
        //:       the object's footprint.  (C-9)
        //:
        //:     3 Use the equality comparison operators to verify that:
        //:
        //:       1 The newly constructed object, 'X', has the same value as
        //:         'Z'.  (C-1, 5)
        //:
        //:       2 'Z' still has the same value as 'ZZ'.  (C-10)
        //:
        //:     4 Use the 'allocator' accessor of each underlying attribute
        //:       capable of allocating memory to ensure that its object
        //:       allocator is properly installed; also apply the object's
        //:       'allocator' accessor, as well as that of 'Z'.  (C-6, 11)
        //:
        //:     5 Use appropriate test allocators to verify that: (C-2..4, 7,
        //:       8, 12)
        //:
        //:       1 Memory is always allocated, and comes from the object
        //:         allocator only (irrespective of the specific number of
        //:         allocations or the total amount of memory allocated).
        //:         (C-2, 4)
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
        //:
        //: 3 Create an object as an automatic variable in the presence of
        //:   injected exceptions (using the
        //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros) and verify that no
        //:   memory is leaked.  (C-12)
        //
        // Testing:
        //   CompactedArray(const CompactedArray& o, *bA = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        if (verbose) cout << "\nTesting arrays." << endl;
        {
            static const struct {
                int         d_line;           // source line number
                const char *d_spec_p;
            } DATA[] = {
                //line  spec
                //----  -------------------
                { L_,                    "" },
                { L_,                   "a" },
                { L_,                  "aa" },
                { L_,                 "aaa" },
                { L_,                   "b" },
                { L_,                  "bb" },
                { L_,                 "bbb" },
                { L_,                "aaab" },
                { L_,                "aaba" },
                { L_,                "abaa" },
                { L_,                "baaa" },
                { L_,           "abcabcabc" },
                { L_,           "cbaabcbac" },
                { L_, "abcdefghijcbaabcbac" }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory.

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const char *const SPEC   = DATA[ti].d_spec_p;

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mZ(&scratch);   const Obj& Z = gg(&mZ, SPEC);

                Obj mZZ(&scratch);  const Obj& ZZ = gg(&mZZ, SPEC);

                if (veryVerbose) { T_ P_(Z) P(ZZ) }

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::Default::setDefaultAllocatorRaw(&da);

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

                    LOOP4_ASSERT(LINE, CONFIG,  X, Z,  X == Z);

                    // Verify that the value of 'Z' has not changed.

                    LOOP4_ASSERT(LINE, CONFIG, ZZ, Z, ZZ == Z);

                    // Also apply the object's 'allocator' accessor, as well as
                    // that of 'Z'.

                    LOOP4_ASSERT(LINE, CONFIG, &oa, X.allocator(),
                                 &oa == X.allocator());

                    LOOP4_ASSERT(LINE, CONFIG, &scratch, Z.allocator(),
                                 &scratch == Z.allocator());

                    // Verify no allocation from the non-object allocator.

                    LOOP3_ASSERT(LINE, CONFIG, noa.numBlocksTotal(),
                                 0 == noa.numBlocksTotal());

                    // Verify memory is always allocated (because of bsl::set)
                    // except when default constructed (SPEC == "").

                    if (0 == ti) {
                        LOOP3_ASSERT(LINE, CONFIG, oa.numBlocksInUse(),
                                     0 == oa.numBlocksInUse());
                    }
                    else {
                        LOOP3_ASSERT(LINE, CONFIG, oa.numBlocksInUse(),
                                     0 != oa.numBlocksInUse());
                    }

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    LOOP3_ASSERT(LINE, CONFIG, fa.numBlocksInUse(),
                                 0 == fa.numBlocksInUse());
                    LOOP3_ASSERT(LINE, CONFIG, da.numBlocksInUse(),
                                 0 == da.numBlocksInUse());
                    LOOP3_ASSERT(LINE, CONFIG, sa.numBlocksInUse(),
                                 0 == sa.numBlocksInUse());
                }  // end foreach configuration

            }  // end foreach row

            // Double check that at least some object memory got allocated.

            ASSERT(anyObjectMemoryAllocatedFlag);

            // Note that memory should be independently allocated for each
            // attribute capable of allocating memory.

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const char *const SPEC   = DATA[ti].d_spec_p;

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mZ(&scratch);  const Obj& Z = gg(&mZ, SPEC);

                if (veryVerbose) { T_ P_(LINE) P(Z) }

                bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                bslma::Default::setDefaultAllocatorRaw(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj obj(Z, &oa);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP2_ASSERT(LINE, da.numBlocksInUse(), !da.numBlocksInUse());
                LOOP2_ASSERT(LINE, oa.numBlocksInUse(), !oa.numBlocksInUse());
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:   their corresponding salient attributes respectively compare
        //:   equal.
        //:
        //: 2 The capacity and storage method of the objects does not affect
        //:   the result of the equality comparison.
        //:
        //: 3 Non-modifiable objects can be compared (i.e., objects providing
        //:   only non-modifiable access).
        //:
        //: 4 No memory allocation occurs as a result of comparison (e.g., the
        //:   arguments are not passed by value).
        //:
        //: 5 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 6 The return type of the equality comparison operators is 'bool'.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate structure for
        //:   the two homogeneous, free equality comparison operators defined
        //:   in this component.  (C-5,6)
        //:
        //: 2 Use the default allocator for all object creation and verify
        //:   the comparison operators do not allocate memory.  (C-4)
        //:
        //: 3 Specify a set of specifications for the 'gg' function that result
        //:   in an empty object but with different capacity and bytes per
        //:   element characteristics.
        //:
        //: 4 Specify a set of specifications for distinct object values.
        //:
        //: 5 For every item in the cross-product of these two sets, verify
        //:   the result of the operators on the item to itself and the item to
        //:   every other item.  (C-1..3)
        //
        // Testing:
        //   bool isEqual(const CompactedArray& other) const;
        //   bool operator==(lhs, rhs);
        //   bool operator!=(lhs, rhs);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                         << "EQUALITY OPERATORS" << endl
                         << "==================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            using namespace bdlc;
            typedef bool (*operatorPtr)(const Obj&, const Obj&);
            // Verify that the signature and return type is standard.

            operatorPtr operatorEq = operator==;
            operatorPtr operatorNe = operator!=;

            // Quash unused variable warning.

            Obj a;
            Obj b;
            ASSERT(true == operatorEq(a, b));
            ASSERT(false == operatorNe(a, b));
        }

        if (verbose) cout << "\nVerify the operators for the array."
                          << endl;

        {
            static const struct {
                int         d_lineNum;
                const char *d_spec_p;
            } INIT[] = {
                //line  spec
                //----  -------------------
                { L_,           "" },
                { L_,         "aR" },
                { L_,        "aaR" },
                { L_,       "aaaR" },
                { L_,       "abcR" },
                { L_,   "abcdefgR" },
            };
            const int NUM_INIT = static_cast<int>(sizeof INIT / sizeof *INIT);

            static const struct {
                int         d_lineNum;
                const char *d_spec_p;
            } DATA[] = {
                //line  spec
                //----  -------------------
                { L_,                    "" },
                { L_,                   "a" },
                { L_,                  "aa" },
                { L_,                 "aaa" },
                { L_,                   "b" },
                { L_,                  "bb" },
                { L_,                 "bbb" },
                { L_,                "aaab" },
                { L_,                "aaba" },
                { L_,                "abaa" },
                { L_,                "baaa" },
                { L_,           "abcabcabc" },
                { L_,           "cbaabcbac" },
                { L_, "abcdefghijcbaabcbac" }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                for (int si = 0; si < NUM_INIT ; ++si) {
                    const int   LINE1 = DATA[ti].d_lineNum;
                    bsl::string SPEC1 = (bsl::string()
                                         + INIT[si].d_spec_p
                                         + DATA[ti].d_spec_p);

                    Obj mX;  const Obj& X = gg(&mX, SPEC1.c_str());

                    for (int tj = 0; tj < NUM_DATA; ++tj) {
                        for (int sj = 0; sj < NUM_INIT ; ++sj) {
                            const int   LINE2 = DATA[tj].d_lineNum;
                            bsl::string SPEC2 = (bsl::string()
                                                 + INIT[sj].d_spec_p
                                                 + DATA[tj].d_spec_p);

                            if (veryVerbose) { P_(SPEC1) P(SPEC2) }

                            Obj mY;  const Obj& Y = gg(&mY, SPEC2.c_str());

                            bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                            LOOP2_ASSERT(LINE1,
                                         LINE2,
                                         X.isEqual(Y) == (ti == tj));
                            LOOP2_ASSERT(LINE1, LINE2, (X == Y) == (ti == tj));
                            LOOP2_ASSERT(LINE1, LINE2, (X != Y) == (ti != tj));

                            LOOP2_ASSERT(LINE1,
                                         LINE2,
                                         defaultAllocator.numAllocations()
                                                               == allocations);
                        }
                    }
                }
            }
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
        //:   the appropriate signatures and return types.  (C-4,7)
        //:
        //: 2 Using the table-driven technique: (C-1..3,5..6,8)
        //:
        //:   1 Define fourteen carefully selected combinations of (two) object
        //:     values ('A' and 'B'), having distinct values for each
        //:     corresponding salient attribute, and various values for the two
        //:     formatting parameters, along with the expected output ( 'value'
        //:     x 'level' x 'spacesPerLevel' ):
        //:     1 { A } x { 0 } x { 0, 1, -1, -8 } --> 3 expected o/ps
        //:     2 { A } x { 3, -3 } x { 0, 2, -2, -8 } --> 8 expected o/ps
        //:     3 { B } x { 2 } x { 3 } --> 1 expected op
        //:     4 { A B } x { -8 } x { -8 } --> 2 expected o/ps
        //:     4 { A B } x { -9 } x { -9 } --> 2 expected o/ps
        //:
        //:   2 For each row in the table defined in P-2.1: (C-1..3,5,7)
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
        //:       supplied stream.  (C-5,8)
        //:
        //:     4 Compare the contents captured in P-2.2.2 with what is
        //:       expected.  (C-1..3,6)
        //
        // Testing:
        //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
        //   ostream& operator<<(ostream& stream, const CompactedArray& array);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR" << endl
                          << "=========================" << endl;

        if (verbose) cout << "\nAssign the addresses of 'print' and "
                             "the output 'operator<<' to variables." << endl;
        {
            using namespace bdlc;

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
            bsl::size_t d_numElems;
            const int   d_array [5];

            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"
#define SP " "

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 } x { 0, 1, -1, -8 } --> 4 expected o/ps
        // ------------------------------------------------------------------

        //LINE  L  SPL   NE   ARRAY       EXP
        //----  -  ---   --   -----       ---

        { L_,   0,   0,   0,  {},         "["                                NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   1,   0,  {},         "["                                NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   -1,  0,  {},         "["                                SP
                                          "]"                                },
        { L_,   0,   -8,  0,  {},         "["                                NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   0,   1,  {0},        "["                                NL
                                          "0"                                NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   1,   1,  {0},        "["                                NL
                                          " 0"                               NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   -1,  1,  {0},        "["                                SP
                                          "0"                                SP
                                          "]"                                },
        { L_,   0,   -8,  1,  {0},        "["                                NL
                                          "    0"                            NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   0,   3,  {0,1,-1},   "["                                NL
                                          "0"                                NL
                                          "1"                                NL
                                          "-1"                               NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   1,   3,  {0,1,-1},   "["                                NL
                                          " 0"                               NL
                                          " 1"                               NL
                                          " -1"                              NL
                                          "]"                                NL
                                                                             },
        { L_,   0,   -1,  3,  {0,1,-1},   "["                                SP
                                          "0"                                SP
                                          "1"                                SP
                                          "-1"                               SP
                                          "]"                                },
        { L_,   0,   -8,  3,  {0,1,-1},   "["                                NL
                                          "    0"                            NL
                                          "    1"                            NL
                                          "    -1"                           NL
                                          "]"                                NL
                                                                             },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2, -8 } --> 6 expected o/ps
        // ------------------------------------------------------------------

        //LINE  L  SPL   NE   ARRAY       EXP
        //----  -  ---   --   -----       ---

        { L_,   3,   0,   0,  {},         "["                                NL
                                          "]"                                NL
                                                                             },
        { L_,   3,   2,   0,  {},         "      ["                          NL
                                          "      ]"                          NL
                                                                             },
        { L_,   3,   -2,  0,  {},         "      ["                          SP
                                          "]"                                },
        { L_,   3,   -8,  0,  {},         "            ["                    NL
                                          "            ]"                    NL
                                                                             },
        { L_,   3,   0,   1,  {0},        "["                                NL
                                          "0"                                NL
                                          "]"                                NL
                                                                             },
        { L_,   3,   2,   1,  {0},        "      ["                          NL
                                          "        0"                        NL
                                          "      ]"                          NL
                                                                             },
        { L_,   3,   -2,  1,  {0},        "      ["                          SP
                                          "0"                                SP
                                          "]"                                },
        { L_,   3,   -8,  1,  {0},        "            ["                    NL
                                          "                0"                NL
                                          "            ]"                    NL
                                                                             },
        { L_,   3,   0,   3,  {0,1,-1},   "["                                NL
                                          "0"                                NL
                                          "1"                                NL
                                          "-1"                               NL
                                          "]"                                NL
                                                                             },
        { L_,   3,   2,   3,  {0,1,-1},   "      ["                          NL
                                          "        0"                        NL
                                          "        1"                        NL
                                          "        -1"                       NL
                                          "      ]"                          NL
                                                                             },
        { L_,   3,   -2,  3,  {0,1,-1},   "      ["                          SP
                                          "0"                                SP
                                          "1"                                SP
                                          "-1"                               SP
                                          "]"                                },
        { L_,   3,   -8,  3,  {0,1,-1},   "            ["                    NL
                                          "                0"                NL
                                          "                1"                NL
                                          "                -1"               NL
                                          "            ]"                    NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 } x { 3 } --> 1 expected o/p
        // -----------------------------------------------------------------

        //LINE  L  SPL   NE   ARRAY       EXP
        //----  -  ---   --   -----       ---

        { L_,  2,  3,   3,  {0,1,-1},     "      ["                          NL
                                          "         0"                       NL
                                          "         1"                       NL
                                          "         -1"                      NL
                                          "      ]"                          NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -8 } x { -8 } --> 2 expected o/ps
        // -----------------------------------------------------------------

        //LINE  L  SPL   NE   ARRAY       EXP
        //----  -  ---   --   -----       ---

        { L_,  -8,  -8,  1,   {0},        "["                                NL
                                          "    0"                            NL
                                          "]"                                NL
                                                                             },
        { L_,  -8,  -8,  3,   {0,1,-1},   "["                                NL
                                          "    0"                            NL
                                          "    1"                            NL
                                          "    -1"                           NL
                                          "]"                                NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.5: { A B } x { -9 } x { -9 } --> 2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL  OFF   FLAG  DESC  EXP
        //---- - ---  ---   ----  ----  ---

        // { L_,  -9,  -9,  1,   {0},        "[ 0 ]"                         },
        // { L_,  -9,  -9,  3,   {0,1,-1},   "[ 0 1 -1 ]"                    },

#undef NL
#undef SP

    };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE      = DATA[ti].d_line;
                const int         L         = DATA[ti].d_level;
                const int         SPL       = DATA[ti].d_spacesPerLevel;
                const bsl::size_t NUM_ELEMS = DATA[ti].d_numElems;
                const int  *const ARRAY    =  DATA[ti].d_array;
                const char *const EXP       = DATA[ti].d_expected_p;

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                ObjInt mX;  const ObjInt& X = mX;
                for (bsl::size_t i = 0; i < NUM_ELEMS; ++i) {
                    if (veryVerbose) { P_(LINE) P(i) }

                    mX.append(ARRAY[i]);
                }

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

                if (veryVerbose) {
                    cout << "\t\t" << LINE << endl;
                    cout << "PRINT STRING" << endl;
                    for ( bsl::size_t i = 0; i < os.str().size(); ++i) {
                        cout << static_cast<int>(os.str()[i]) << " ";
                    }
                    cout << "\nPRINT EXP" << endl;
                    for ( bsl::size_t i = 0; i < strlen(EXP); ++i) {
                        cout << static_cast<int>(EXP[i]) << " ";
                    }
                    cout << "\n----------------------------------------\n\n";
                }
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
        //: 1 Each accessor returns the value of the corresponding attribute of
        //:   the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //:
        //: 3 No accessor allocates any memory.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 To test 'allocator', create object with various allocators and
        //:   ensure the returned value matches the supplied allocator.
        //:
        //: 2 Use the generator function to produce objects of arbitrary state
        //:   and verify the accessors' return values against expected values.
        //:   (C-1)
        //:
        //: 3 The accessors will only be accessed from a 'const' reference to
        //:   the created object.  (C-2)
        //:
        //: 4 The default allocator will be used for all created objects
        //:   (excluding those used to test 'allocator') and the number of
        //:   allocation will be verified to ensure that no memory was
        //:   allocated during use of the accessors.  (C-3)
        //:
        //: 5 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when an attempt is made to swap objects that do not
        //:   refer to the same allocator, but not when the allocators are the
        //:   same (using the 'BSLS_ASSERTTEST_*' macros).  (C-4)
        //
        // Testing:
        //   const TYPE& operator[](bsl::size_t index) const;
        //   bslma::Allocator *allocator() const;
        //   bsl::size_t capacity() const;
        //   bool isEmpty() const;
        //   bsl::size_t length() const;
        //   const TYPE& uniqueElement(bsl::size_t index) const;
        //   bsl::size_t uniqueLength() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout << "\nTesting 'allocator'." << endl;
        {
            Obj mX;  const Obj& X = mX;
            ASSERT(&defaultAllocator == X.allocator());
        }
        {
            Obj        mX(reinterpret_cast<bslma::TestAllocator *>(0));
            const Obj& X = mX;
            ASSERT(&defaultAllocator == X.allocator());
        }
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;
            ASSERT(&sa == X.allocator());
        }

        if (verbose) cout << "\nTesting residual basic accessors." << endl;
        {
            static const struct {
                int          d_lineNum;          // source line number
                const char  *d_spec_p;           // specification string
                const char  *d_exp_p;            // expected value string
                const char  *d_unique_p;         // unique value string
                bsl::size_t  d_capacity;         // expected capacity
            } DATA[] = {
                //LN  spec               expected        unique     capacity
                //--  -----------------  --------------  ---------  --------
                { L_,                "",             "",        "",        0 },
                { L_,               "a",            "a",       "a",        1 },
                { L_,              "aa",           "aa",       "a",        3 },
                { L_,             "aaa",          "aaa",       "a",        3 },
                { L_,               "b",            "b",       "b",        1 },
                { L_,              "bb",           "bb",       "b",        3 },
                { L_,             "bbb",          "bbb",       "b",        3 },
                { L_,            "aaab",         "aaab",      "ab",        6 },
                { L_,            "aaba",         "aaba",      "ab",        6 },
                { L_,            "abaa",         "abaa",      "ab",        6 },
                { L_,            "baaa",         "baaa",      "ab",        6 },
                { L_,       "abcabcabc",    "abcabcabc",     "abc",       10 },
                { L_,       "cbaabcbac",    "cbaabcbac",     "abc",       10 },

                { L_,              "aP",             "",        "",        0 },
                { L_,            "aaPP",             "",        "",        0 },
                { L_,            "aabP",           "aa",       "a",        3 },
                { L_,            "abaP",           "ab",      "ab",        3 },
                { L_,           "abaPP",            "a",       "a",        3 },
                { L_,          "abaPPP",             "",        "",        0 },
                { L_,      "abcabcabcP",     "abcabcab",     "abc",       10 },
                { L_,     "abcabcabcPa",    "abcabcaba",     "abc",       10 },
                { L_,    "abcabcabcPPa",     "abcabcaa",     "abc",       10 },
                { L_,   "abcabcabcPPPa",      "abcabca",     "abc",       10 },

                { L_,        "abcabcPR",             "",        "",        0 },
                { L_,       "abcabcPaR",             "",        "",        0 },
                { L_,      "abcabcPPaR",             "",        "",        0 },
                { L_,     "abcabcPPPaR",             "",        "",        0 },
                { L_,    "abcabcPPPaRR",             "",        "",        0 },
                { L_,   "abcabcPPPaRRa",            "a",       "a",        6 },
                { L_,  "abcabcPPPaRRab",           "ab",      "ab",        6 },
                { L_, "abcabcPPPaRRabR",             "",        "",        0 }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const char *const EXP    = DATA[ti].d_exp_p;
                const char *const UNIQUE = DATA[ti].d_unique_p;
                const bsl::size_t CAP    = DATA[ti].d_capacity;

                Obj mX;  const Obj& X = gg(&mX, SPEC);   // original spec

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                LOOP_ASSERT(LINE, bsl::strlen(EXP)    == X.length());
                LOOP_ASSERT(LINE, bsl::strlen(UNIQUE) == X.uniqueLength());
                LOOP_ASSERT(LINE, (0 == strlen(EXP))  == X.isEmpty());
                LOOP_ASSERT(LINE,                 CAP == X.capacity());

                for (bsl::size_t i = 0; i < X.length(); i++) {
                    if (veryVerbose) { P_(EXP[i]) P(X[i]) }

                    LOOP_ASSERT(LINE, bsl::string(EXP + i, 1) == X[i]);
                }

                for (bsl::size_t i = 0; i < X.uniqueLength(); i++) {
                    if (veryVerbose) { P_(UNIQUE[i]) P(X.uniqueElement(i)); }

                    LOOP_ASSERT(LINE, bsl::string(UNIQUE + i, 1)
                                                        == X.uniqueElement(i));
                }

                LOOP_ASSERT(LINE,
                            defaultAllocator.numAllocations() == allocations);
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mX;  const Obj& X = mX;

            ASSERT_SAFE_FAIL(X[0]);
            ASSERT_SAFE_FAIL(X.uniqueElement(0));

            mX.append("a");

            ASSERT_SAFE_PASS(X[0]);
            ASSERT_SAFE_PASS(X.uniqueElement(0));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PRIMITIVE GENERATOR FUNCTIONS
        //   Ensure that the generator functions are able to create an object
        //   in any state.
        //
        // Concerns:
        //: 1 Valid syntax produces the expected results.
        //:
        //: 2 Invalid syntax is detected and reported.
        //
        // Plan:
        //: 1 Evaluate a series of test strings of increasing complexity to
        //:   set the state of a newly created object and verify the returned
        //:   object using basic accessors.  (C-1)
        //:
        //: 2 Evaluate the 'ggg' function with a series of test strings of
        //:   increasing complexity and verify its return value.  (C-2)
        //
        // Testing:
        //    Obj& gg(Obj *object, const char *spec);
        //    int ggg(Obj *object, const char *spec);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMITIVE GENERATOR FUNCTIONS" << endl
                          << "=============================" << endl;

        if (verbose) cout << "\nTesting generator on valid specs." << endl;
        {
            static const struct {
                int          d_lineNum;          // source line number
                const char  *d_spec_p;           // specification string
                const char  *d_exp_p;            // expected value string
                const char  *d_unique_p;         // unique value string
            } DATA[] = {
                //line  spec                 expected        unique
                //----  -------------------  --------------  ---------
                { L_,                    "",             "",        "" },
                { L_,                   "a",            "a",       "a" },
                { L_,                  "aa",           "aa",       "a" },
                { L_,                 "aaa",          "aaa",       "a" },
                { L_,                   "b",            "b",       "b" },
                { L_,                  "bb",           "bb",       "b" },
                { L_,                 "bbb",          "bbb",       "b" },
                { L_,                "aaab",         "aaab",      "ab" },
                { L_,                "aaba",         "aaba",      "ab" },
                { L_,                "abaa",         "abaa",      "ab" },
                { L_,                "baaa",         "baaa",      "ab" },
                { L_,           "abcabcabc",    "abcabcabc",     "abc" },
                { L_,           "cbaabcbac",    "cbaabcbac",     "abc" },

                { L_,                  "aP",             "",        "" },
                { L_,                "aaPP",             "",        "" },
                { L_,                "aabP",           "aa",       "a" },
                { L_,                "abaP",           "ab",      "ab" },
                { L_,               "abaPP",            "a",       "a" },
                { L_,              "abaPPP",             "",        "" },
                { L_,          "abcabcabcP",     "abcabcab",     "abc" },
                { L_,         "abcabcabcPa",    "abcabcaba",     "abc" },
                { L_,        "abcabcabcPPa",     "abcabcaa",     "abc" },
                { L_,       "abcabcabcPPPa",      "abcabca",     "abc" },

                { L_,         "abcabcabcPR",             "",        "" },
                { L_,        "abcabcabcPaR",             "",        "" },
                { L_,       "abcabcabcPPaR",             "",        "" },
                { L_,      "abcabcabcPPPaR",             "",        "" },
                { L_,     "abcabcabcPPPaRR",             "",        "" },
                { L_,    "abcabcabcPPPaRRa",            "a",       "a" },
                { L_,   "abcabcabcPPPaRRab",           "ab",      "ab" },
                { L_,  "abcabcabcPPPaRRabR",             "",        "" }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const char *const EXP    = DATA[ti].d_exp_p;
                const char *const UNIQUE = DATA[ti].d_unique_p;

                Obj mX;  const Obj& X = gg(&mX, SPEC);   // original spec

                LOOP_ASSERT(LINE, bsl::strlen(EXP)    == X.length());
                LOOP_ASSERT(LINE, bsl::strlen(UNIQUE) == X.uniqueLength());

                for (bsl::size_t i = 0; i < X.length(); i++) {
                    if (veryVerbose) { P_(EXP[i]) P(X[i]) }

                    LOOP_ASSERT(LINE, bsl::string(EXP + i, 1) == X[i]);
                }

                for (bsl::size_t i = 0; i < X.uniqueLength(); i++) {
                    if (veryVerbose) { P_(UNIQUE[i]) P(X.uniqueElement(i)); }

                    LOOP_ASSERT(LINE, bsl::string(UNIQUE + i, 1)
                                                        == X.uniqueElement(i));
                }
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
                { L_,   "",             -1,     }, // control

                { L_,   "a",            -1,     }, // control
                { L_,   " ",             0,     },
                { L_,   ".",             0,     },
                { L_,   "2",             0,     },

                { L_,   "ab",           -1,     }, // control
                { L_,   " a",            0,     },
                { L_,   ".a",            0,     },
                { L_,   "2a",            0,     },
                { L_,   "a ",            1,     },
                { L_,   "a.",            1,     },
                { L_,   "a2",            1,     },

                { L_,   "abc",          -1,     }, // control
                { L_,   " bc",           0,     },
                { L_,   ".bc",           0,     },
                { L_,   "2bc",           0,     },
                { L_,   "b c",           1,     },
                { L_,   "b.c",           1,     },
                { L_,   "b2c",           1,     },
                { L_,   "bc ",           2,     },
                { L_,   "bc.",           2,     },
                { L_,   "bc2",           2,     },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          LINE  = DATA[ti].d_lineNum;
                const char *const  SPEC  = DATA[ti].d_spec_p;
                const int          INDEX = DATA[ti].d_index;

                if (veryVerbose) { P_(SPEC) P(INDEX) }

                {
                    Obj mX;

                    int result = ggg(&mX, SPEC, veryVerbose);
                    LOOP_ASSERT(LINE, INDEX == result);
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS TEST
        //   The basic concern is that the default constructor, the destructor,
        //   and, under normal conditions (i.e., no aliasing), the primary
        //   manipulators:
        //      - append
        //      - pop_back
        //      - removeAll
        //   operate as expected.
        //
        // Concerns:
        //: 1 The default constructor creates the correct initial value and has
        //:   the internal memory management system hooked up properly so that
        //:   *all* internally allocated memory draws from the same
        //:   user-supplied allocator whenever one is specified.
        //:
        //: 2 The method 'append' produces the expected value, increases
        //:   capacity as needed, and is exception neutral with respect to
        //:   memory allocation.
        //:
        //: 3 The method 'removeAll' produces the expected value (empty) and
        //:   does not affect allocated memory.
        //:
        //: 4 The method 'pop_back' produces the expected value and does not
        //:   affect allocated memory or bytes per element.
        //:
        //: 5 Memory is not leaked by any method and the destructor properly
        //:   deallocates the residual allocated memory.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create an object using the default constructor with and without
        //:   passing in an allocator, verify the allocator is stored using the
        //:   (untested) 'allocator' accessor, and verifying all allocations
        //:   are done from the allocator in future tests.
        //:
        //: 2 Create objects using the 'bslma::TestAllocator', use the 'append'
        //:   method with various values, and the (untested) accessors to
        //:   verify the value of the object and that allocation occurred when
        //:   expected.  Also vary the test allocator's allocation limit to
        //:   verify behavior in the presence of exceptions.  (C-1,2)
        //:
        //: 3 Create objects using the 'bslma::TestAllocator', use the 'append'
        //:   to obtain various states, use 'removeAll', verify the objects are
        //:   empty, then repopulate the objects and ensure no allocation
        //:   occurs.  (C-3)
        //:
        //: 4 Create objects using the 'bslma::TestAllocator', use the 'append'
        //:   to obtain various states, use 'pop_all', verify the objects have
        //:   a reduced length but unchanged capacity.  (C-4)
        //:
        //: 5 Use a supplied 'bslma::TestAllocator' that goes out-of-scope
        //:   at the conclusion of each test to ensure all memory is returned
        //:   to the allocator.  (C-5)
        //:
        //: 6 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when an attempt is made to swap objects that do not
        //:   refer to the same allocator, but not when the allocators are the
        //:   same (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
        //
        // Testing:
        //   CompactedArray(bslma::Allocator *basicAllocator = 0);
        //   ~CompactedArray();
        //   void append(const TYPE& value);
        //   void pop_back();
        //   void removeAll();
        //   CONCERN: Allocator is propagated to the stored objects.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS TEST" << endl
                          << "=========================" << endl;

        const char *DATA = "abcabcaaadeefgahsjrldishslrpoppoopssskjfhjaaabbcc";
        bsl::size_t NUM_DATA = bsl::strlen(DATA);

        if (verbose) cout << "\nTesting with various allocator configurations."
                          << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            Obj mX;  const Obj& X = mX;

            ASSERT(&defaultAllocator == X.allocator());
            ASSERT(0 == X.length());
            ASSERT(0 == X.uniqueLength());
            ASSERT(allocations == defaultAllocator.numAllocations());

            mX.append("a");
            ASSERT(1 == X.length());
            ASSERT(1 == X.uniqueLength());
            ASSERT(allocations + 2 == defaultAllocator.numAllocations());
        }
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            Obj        mX(reinterpret_cast<bslma::TestAllocator *>(0));
            const Obj& X = mX;

            ASSERT(&defaultAllocator == X.allocator());
            ASSERT(0 == X.length());
            ASSERT(0 == X.uniqueLength());
            ASSERT(allocations == defaultAllocator.numAllocations());

            mX.append("a");

            ASSERT(1 == X.length());
            ASSERT(1 == X.uniqueLength());
            ASSERT(allocations + 2 == defaultAllocator.numAllocations());
        }
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            bslma::TestAllocator      sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch",  veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            mX.reserveCapacity(3, 3);

            bsls::Types::Int64 saAllocations = sa.numAllocations();

            ASSERT(&sa == X.allocator());

            ASSERT(                0 == X.length());
            ASSERT(                0 == X.uniqueLength());
            ASSERT(allocations       == defaultAllocator.numAllocations());

            bsl::string Z1(&scratch);  Z1 = LONG_STRING_1;
            ASSERT(1 == scratch.numAllocations());

            mX.append(Z1);

            ASSERT(                1 == X.length());
            ASSERT(                1 == X.uniqueLength());
            ASSERT(allocations       == defaultAllocator.numAllocations());
            ASSERT(saAllocations + 1 == sa.numAllocations());

            // Note that this 'append' will cause the previous string to be
            // moved in the unique array and result in an allocation for that
            // string.
            mX.append("a");

            ASSERT(                2 == X.length());
            ASSERT(                2 == X.uniqueLength());
            ASSERT(allocations       == defaultAllocator.numAllocations());
            ASSERT(saAllocations + 2 == sa.numAllocations());

            bsl::string Z2(&scratch);  Z2 = LONG_STRING_2;
            ASSERT(2 == scratch.numAllocations());

            mX.append(Z2);

            ASSERT(                3 == X.length());
            ASSERT(                3 == X.uniqueLength());
            ASSERT(allocations       == defaultAllocator.numAllocations());
            ASSERT(saAllocations + 3 == sa.numAllocations());
        }

        if (verbose) cout << "\nTesting 'append'." << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            bslma::TestAllocator oa("other",    veryVeryVeryVerbose);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                bsl::set<char> unique(&oa);

                Obj mX(&sa);  const Obj& X = mX;
                for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
                    bsl::string data(DATA + i, 1);

                    unique.insert(DATA[i]);
                    mX.append(data);

                    if (veryVerbose) {  P(X);  }

                    LOOP_ASSERT(i,         i + 1 == X.length());
                    LOOP_ASSERT(i, unique.size() == X.uniqueLength());

                    for (bsl::size_t j = 0; j <= i; ++j) {
                        LOOP2_ASSERT(i, j, DATA[j] == X[j][0]);
                    }
                }
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(allocations == defaultAllocator.numAllocations());
        }

        if (verbose) cout << "\nTesting 'removeAll'." << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
                bsl::string data(DATA + i, 1);

                mX.append(data);
            }

            if (veryVerbose) {  P(X);  }

            bsls::Types::Int64 na = sa.numAllocations();
            bsls::Types::Int64 nd = sa.numDeallocations();

            mX.removeAll();

            if (veryVerbose) {  P(X);  }

            ASSERT( 0 == X.length());
            ASSERT( 0 == X.uniqueLength());
            ASSERT(na == sa.numAllocations());
            ASSERT(nd == sa.numDeallocations());

            for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
                bsl::string data(DATA + i, 1);

                mX.append(data);
            }

            if (veryVerbose) {  P(X);  }

            ASSERT(NUM_DATA == X.length());
            for (bsl::size_t j = 0; j < NUM_DATA; ++j) {
                LOOP_ASSERT(j, DATA[j] == X[j][0]);
            }

            ASSERT(na == sa.numAllocations());
            ASSERT(nd == sa.numDeallocations());

            ASSERT(allocations == defaultAllocator.numAllocations());
        }

        if (verbose) cout << "\nTesting 'pop_back'." << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;

            for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
                bsl::string data(DATA + i, 1);

                mX.append(data);
            }

            if (veryVerbose) {  P(X);  }

            bsls::Types::Int64 na = sa.numAllocations();
            bsls::Types::Int64 nd = sa.numDeallocations();

            mX.pop_back();

            if (veryVerbose) {  P(X);  }

            ASSERT(NUM_DATA - 1 == X.length());
            for (bsl::size_t j = 0; j < NUM_DATA - 1; ++j) {
                LOOP_ASSERT(j, DATA[j] == X[j][0]);
            }
            ASSERT(na == sa.numAllocations());
            ASSERT(nd == sa.numDeallocations());

            mX.pop_back();

            if (veryVerbose) {  P(X);  }

            ASSERT(NUM_DATA - 2 == X.length());
            for (bsl::size_t j = 0; j < NUM_DATA - 2; ++j) {
                LOOP_ASSERT(j, DATA[j] == X[j][0]);
            }
            ASSERT(na == sa.numAllocations());
            ASSERT(nd == sa.numDeallocations());

            ASSERT(allocations == defaultAllocator.numAllocations());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mX;

            ASSERT_FAIL(mX.pop_back());

            mX.append("a");

            ASSERT_PASS(mX.pop_back());
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
        //: 1 Create objects using various constructors.
        //:
        //: 2 Exercise these objects using primary manipulators, basic
        //:   accessors, equality operators, and the assignment operator.
        //:
        //: 3 Verify expected values throughout.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << endl << "default constructor" << endl;
        {
            Obj mX;  const Obj& X = mX;

            if (verbose) {  cout << '\t'; P(X);  }

            ASSERT(0 == X.length());

            ASSERT(true  == (X == X));
            ASSERT(false == (X != X));
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << endl << "'append(const T&)'" << endl;
        {
            Obj mX;  const Obj& X = mX;

            mX.append("a");

            if (verbose) {  cout << '\t'; P(X);  }

            ASSERT(1 == X.length());
            ASSERT(1 == X.uniqueLength());

            ASSERT(X[0] == "a");

            ASSERT(true  == (X == X));
            ASSERT(false == (X != X));

            {
                Obj mY;  const Obj& Y = mY;

                ASSERT(false == (X == Y));
                ASSERT(true  == (X != Y));
                ASSERT(false == (Y == X));
                ASSERT(true  == (Y != X));
            }

            mX.append("b");

            if (verbose) {  cout << '\t'; P(X);  }

            ASSERT(2 == X.length());
            ASSERT(2 == X.uniqueLength());

            ASSERT(X[0] == "a");
            ASSERT(X[1] == "b");

            mX.append("c");

            if (verbose) {  cout << '\t'; P(X);  }

            ASSERT(3 == X.length());
            ASSERT(3 == X.uniqueLength());

            ASSERT(X[0] == "a");
            ASSERT(X[1] == "b");
            ASSERT(X[2] == "c");

            mX.append("a");

            if (verbose) {  cout << '\t'; P(X);  }

            ASSERT(4 == X.length());
            ASSERT(3 == X.uniqueLength());

            ASSERT(X[0] == "a");
            ASSERT(X[1] == "b");
            ASSERT(X[2] == "c");
            ASSERT(X[3] == "a");
        }

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << endl << "copy constructor" << endl;
        {
            Obj mX;  const Obj& X = mX;

            mX.append("a");

            Obj mY(X);  const Obj& Y = mY;

            if (verbose) {  cout << '\t'; P(Y);  }

            ASSERT(1 == Y.length());
            ASSERT(1 == Y.uniqueLength());

            ASSERT(Y[0] == "a");

            ASSERT(true  == (X == Y));
            ASSERT(false == (X != Y));

            ASSERT(true  == (Y == X));
            ASSERT(false == (Y != X));

            {
                Obj mZ;  const Obj& Z = mZ;

                ASSERT(false == (Y == Z));
                ASSERT(true  == (Y != Z));
                ASSERT(false == (Z == Y));
                ASSERT(true  == (Z != Y));
            }
        }
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
