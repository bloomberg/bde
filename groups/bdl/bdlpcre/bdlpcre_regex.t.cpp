// bdlpcre_regex.t.cpp                                                -*-C++-*-
#include <bdlpcre_regex.h>

#include <bslim_testutil.h>

#include <bdlma_bufferedsequentialallocator.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslmt_threadutil.h>

#include <bsls_alignedbuffer.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsls_stopwatch.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;
using namespace bdlpcre;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test is a mechanism that wraps the Perl Compatible
// Regular Expression (PCRE2) C library.  In this test, we want to ensure that
// the PCRE2 library is "plugged in" correctly and that the component conforms
// to the documented interface.  We will not test the implementation details of
// the PCRE2 library.
//
// After breathing the component, we will test the basic mechanism of preparing
// and clearing a regular expression object.  We will then test that the
// matching functions are plugged in correctly.  Then we will test the various
// optional flags that can be passed when preparing a regular expression
// object.  We will then test that subpatterns are supported correctly and that
// the captured substrings are returned correctly.  Finally, we will test the
// usage example from the component's header file.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] RegEx(bslma::Allocator *basicAllocator = 0);
// [ 2] ~RegEx();
//
// MANIPULATORS
// [ 3] void clear();
// [ 3] int prepare(bsl::string*, size_t *, const char *, int, size_t);
// [15] int setDepthLimit(int);
// [15] int setDefaultDepthLimit(int);
//
// ACCESSORS
// [ 6] int flags() const;
// [ 3] bool isPrepared() const;
// [14] size_t jitStackSize() const;
// [ 4] int match(const bsl::string_view& subject, ...) const;
// [ 4] int match(const char *subject, ...) const;
// [ 4] int match(bsl::pair<size_t, size_t> *result, ...) const;
// [ 4] int match(bsl::vector<bsl::pair<size_t, size_t> > *result, ...) const;
// [ 4] int match(bsl::vector<bsl::string_view> *result, ...) const;
// [ 4] int match(std::vector<bsl::string_view> *result, ...) const;
// [ 4] int match(std::pmr::vector<bsl::string_view> *result, ...) const;
// [ 4] int matchRaw(const bsl::string_view& subject, ...) const;
// [ 4] int matchRaw(const char *subject, ...) const;
// [ 4] int matchRaw(bsl::pair<size_t, size_t> *result, ...) const;
// [ 4] int matchRaw(bsl::vector<bsl::pair<size_t, size_t> > *result,...)const;
// [ 4] int matchRaw(bsl::vector<bsl::string_view> *result, ...) const;
// [ 4] int matchRaw(std::vector<bsl::string_view> *result, ...) const;
// [ 4] int matchRaw(std::pmr::vector<bsl::string_view> *result, ...) const;
// [ 5] int match(bslstl::StringRef *result, ...) const;
// [ 5] int match(bsl::vector<bslstl::StringRef> *result, ...) const;
// [ 5] int matchRaw(bslstl::StringRef *result, ...) const;
// [ 5] int matchRaw(bsl::vector<bslstl::StringRef> *result, ...) const;
// [11] int numSubpatterns() const;
// [ 2] const bsl::string& pattern() const;
// [11] int subpatternIndex(const char *name) const;
// [15] int depthLimit(int);
// [15] int defaultDepthLimit(int);
// [13] bool isJitAvailable();
// [16] int replace(bsl::string *, int *, const string_view&, ...) const;
// [16] int replace(std::string *, int *, const string_view&, ...) const;
// [16] int replace(std::pmr::string *, int *, const string_view&, ...) const;
// [16] int replaceRaw(bsl::string *, int *, const string_view&, ...) const;
// [16] int replaceRaw(std::string *, int *, const string_view&, ...) const;
// [16] int replaceRaw(std::pmr::string *, int *, ...) const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] k_FLAG_CASELESS
// [ 8] k_FLAG_MULTILINE
// [ 9] k_FLAG_UTF8
// [10] k_FLAG_DOTMATCHESALL
// [12] ALLOCATOR PROPAGATION
// [14] JIT OPTIMIZATION SUPPORT
// [17] UNICODE CHARACTER PROPERTY SUPPORT
// [18] MEMORY ALIGNMENT
// [19] CONCERN: 'match' IS THREAD-SAFE
// [21] DUPLICATE NAMED GROUPS
// [22] USAGE EXAMPLE
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

//=============================================================================
//                       TEMPLATIZED OUTPUT FUNCTIONS
//=============================================================================

template <class T>
void printValue(ostream& out, const T& value)
{
    out << value;
}

static const char *printableCharacters[256]=
{
    "\\x00" , //   0   0
    "\\x01" , //   1   1
    "\\x02" , //   2   2
    "\\x03" , //   3   3
    "\\x04" , //   4   4
    "\\x05" , //   5   5
    "\\x06" , //   6   6
    "\\x07" , //   7   7
    "\\x08" , //   8   8 - BACKSPACE
    "\\t" ,   //   9   9 - TAB
    "\\n" ,   //  10   a - LF
    "\\x0b" , //  11   b
    "\\x0c" , //  12   c
    "\\r"   , //  13   d - CR
    "\\x0e" , //  14   e
    "\\x0f" , //  15   f
    "\\x10" , //  16  10
    "\\x11" , //  17  11
    "\\x12" , //  18  12
    "\\x13" , //  19  13
    "\\x14" , //  20  14
    "\\x15" , //  21  15
    "\\x16" , //  22  16
    "\\x17" , //  23  17
    "\\x18" , //  24  18
    "\\x19" , //  25  19
    "\\x1a" , //  26  1a
    "\\x1b" , //  27  1b
    "\\x1c" , //  28  1c
    "\\x1d" , //  29  1d
    "\\x1e" , //  30  1e
    "\\x1f" , //  31  1f
    " "     , //  32  20 - SPACE
    "!"     , //  33  21 - !
    "\\\""  , //  34  22 - "
    "#"     , //  35  23 - #
    "$"     , //  36  24 - $
    "%"     , //  37  25 - %
    "&"     , //  38  26 - &
    "'"     , //  39  27 - '
    "("     , //  40  28 - (
    ")"     , //  41  29 - )
    "*"     , //  42  2a - *
    "+"     , //  43  2b - +
    ","     , //  44  2c - ,
    "-"     , //  45  2d - -
    "."     , //  46  2e - .
    "/"     , //  47  2f - /
    "0"     , //  48  30 - 0
    "1"     , //  49  31 - 1
    "2"     , //  50  32 - 2
    "3"     , //  51  33 - 3
    "4"     , //  52  34 - 4
    "5"     , //  53  35 - 5
    "6"     , //  54  36 - 6
    "7"     , //  55  37 - 7
    "8"     , //  56  38 - 8
    "9"     , //  57  39 - 9
    ":"     , //  58  3a - :
    ";"     , //  59  3b - ;
    "<"     , //  60  3c - <
    "="     , //  61  3d - =
    ">"     , //  62  3e - >
    "?"     , //  63  3f - ?
    "@"     , //  64  40 - @
    "A"     , //  65  41 - A
    "B"     , //  66  42 - B
    "C"     , //  67  43 - C
    "D"     , //  68  44 - D
    "E"     , //  69  45 - E
    "F"     , //  70  46 - F
    "G"     , //  71  47 - G
    "H"     , //  72  48 - H
    "I"     , //  73  49 - I
    "J"     , //  74  4a - J
    "K"     , //  75  4b - K
    "L"     , //  76  4c - L
    "M"     , //  77  4d - M
    "N"     , //  78  4e - N
    "O"     , //  79  4f - O
    "P"     , //  80  50 - P
    "Q"     , //  81  51 - Q
    "R"     , //  82  52 - R
    "S"     , //  83  53 - S
    "T"     , //  84  54 - T
    "U"     , //  85  55 - U
    "V"     , //  86  56 - V
    "W"     , //  87  57 - W
    "X"     , //  88  58 - X
    "Y"     , //  89  59 - Y
    "Z"     , //  90  5a - Z
    "["     , //  91  5b - [
    "\\\\"  , //  92  5c - '\'
    "]"     , //  93  5d - ]
    "^"     , //  94  5e - ^
    "_"     , //  95  5f - _
    "`"     , //  96  60 - `
    "a"     , //  97  61 - a
    "b"     , //  98  62 - b
    "c"     , //  99  63 - c
    "d"     , // 100  64 - d
    "e"     , // 101  65 - e
    "f"     , // 102  66 - f
    "g"     , // 103  67 - g
    "h"     , // 104  68 - h
    "i"     , // 105  69 - i
    "j"     , // 106  6a - j
    "k"     , // 107  6b - k
    "l"     , // 108  6c - l
    "m"     , // 109  6d - m
    "n"     , // 110  6e - n
    "o"     , // 111  6f - o
    "p"     , // 112  70 - p
    "q"     , // 113  71 - q
    "r"     , // 114  72 - r
    "s"     , // 115  73 - s
    "t"     , // 116  74 - t
    "u"     , // 117  75 - u
    "v"     , // 118  76 - v
    "w"     , // 119  77 - w
    "x"     , // 120  78 - x
    "y"     , // 121  79 - y
    "z"     , // 122  7a - z
    "{"     , // 123  7b - {
    "|"     , // 124  7c - |
    "}"     , // 125  7d - }
    "~"     , // 126  7e - ~
    "\\x7f" , // 127  7f - DEL
    "\\x80" , // 128  80
    "\\x81" , // 129  81
    "\\x82" , // 130  82
    "\\x83" , // 131  83
    "\\x84" , // 132  84
    "\\x85" , // 133  85
    "\\x86" , // 134  86
    "\\x87" , // 135  87
    "\\x88" , // 136  88
    "\\x89" , // 137  89
    "\\x8a" , // 138  8a
    "\\x8b" , // 139  8b
    "\\x8c" , // 140  8c
    "\\x8d" , // 141  8d
    "\\x8e" , // 142  8e
    "\\x8f" , // 143  8f
    "\\x90" , // 144  90
    "\\x91" , // 145  91
    "\\x92" , // 146  92
    "\\x93" , // 147  93
    "\\x94" , // 148  94
    "\\x95" , // 149  95
    "\\x96" , // 150  96
    "\\x97" , // 151  97
    "\\x98" , // 152  98
    "\\x99" , // 153  99
    "\\x9a" , // 154  9a
    "\\x9b" , // 155  9b
    "\\x9c" , // 156  9c
    "\\x9d" , // 157  9d
    "\\x9e" , // 158  9e
    "\\x9f" , // 159  9f
    "\\xa0" , // 160  a0
    "\\xa1" , // 161  a1
    "\\xa2" , // 162  a2
    "\\xa3" , // 163  a3
    "\\xa4" , // 164  a4
    "\\xa5" , // 165  a5
    "\\xa6" , // 166  a6
    "\\xa7" , // 167  a7
    "\\xa8" , // 168  a8
    "\\xa9" , // 169  a9
    "\\xaa" , // 170  aa
    "\\xab" , // 171  ab
    "\\xac" , // 172  ac
    "\\xad" , // 173  ad
    "\\xae" , // 174  ae
    "\\xaf" , // 175  af
    "\\xb0" , // 176  b0
    "\\xb1" , // 177  b1
    "\\xb2" , // 178  b2
    "\\xb3" , // 179  b3
    "\\xb4" , // 180  b4
    "\\xb5" , // 181  b5
    "\\xb6" , // 182  b6
    "\\xb7" , // 183  b7
    "\\xb8" , // 184  b8
    "\\xb9" , // 185  b9
    "\\xba" , // 186  ba
    "\\xbb" , // 187  bb
    "\\xbc" , // 188  bc
    "\\xbd" , // 189  bd
    "\\xbe" , // 190  be
    "\\xbf" , // 191  bf
    "\\xc0" , // 192  c0
    "\\xc1" , // 193  c1
    "\\xc2" , // 194  c2
    "\\xc3" , // 195  c3
    "\\xc4" , // 196  c4
    "\\xc5" , // 197  c5
    "\\xc6" , // 198  c6
    "\\xc7" , // 199  c7
    "\\xc8" , // 200  c8
    "\\xc9" , // 201  c9
    "\\xca" , // 202  ca
    "\\xcb" , // 203  cb
    "\\xcc" , // 204  cc
    "\\xcd" , // 205  cd
    "\\xce" , // 206  ce
    "\\xcf" , // 207  cf
    "\\xd0" , // 208  d0
    "\\xd1" , // 209  d1
    "\\xd2" , // 210  d2
    "\\xd3" , // 211  d3
    "\\xd4" , // 212  d4
    "\\xd5" , // 213  d5
    "\\xd6" , // 214  d6
    "\\xd7" , // 215  d7
    "\\xd8" , // 216  d8
    "\\xd9" , // 217  d9
    "\\xda" , // 218  da
    "\\xdb" , // 219  db
    "\\xdc" , // 220  dc
    "\\xdd" , // 221  dd
    "\\xde" , // 222  de
    "\\xdf" , // 223  df
    "\\xe0" , // 224  e0
    "\\xe1" , // 225  e1
    "\\xe2" , // 226  e2
    "\\xe3" , // 227  e3
    "\\xe4" , // 228  e4
    "\\xe5" , // 229  e5
    "\\xe6" , // 230  e6
    "\\xe7" , // 231  e7
    "\\xe8" , // 232  e8
    "\\xe9" , // 233  e9
    "\\xea" , // 234  ea
    "\\xeb" , // 235  eb
    "\\xec" , // 236  ec
    "\\xed" , // 237  ed
    "\\xee" , // 238  ee
    "\\xef" , // 239  ef
    "\\xf0" , // 240  f0
    "\\xf1" , // 241  f1
    "\\xf2" , // 242  f2
    "\\xf3" , // 243  f3
    "\\xf4" , // 244  f4
    "\\xf5" , // 245  f5
    "\\xf6" , // 246  f6
    "\\xf7" , // 247  f7
    "\\xf8" , // 248  f8
    "\\xf9" , // 249  f9
    "\\xfa" , // 250  fa
    "\\xfb" , // 251  fb
    "\\xfc" , // 252  fc
    "\\xfd" , // 253  fd
    "\\xfe" , // 254  fe
    "\\xff"   // 255  ff
};

void printValue(ostream& out, const char* value)
    // Specialize for char*.  Need to expand \r, \n, \t and surround with
    // DQUOTE characters.
{
    out << '"';

    while (*value) {
        out << printableCharacters[(int)*value];
        ++value;
    }

    out << '"';
}

void printValue(ostream& out, const string& value)
    // Need to expand \r, \n, \t and surround with DQUOTE characters.
{
    printValue(out, value.c_str());
}

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
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

// ============================================================================
//                     GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------
typedef RegEx Obj;

//=============================================================================
//                      GLOBAL HELPER CLASSES
//-----------------------------------------------------------------------------

namespace {

                        // ========
                        // MatchJob
                        // ========

class MatchJob {
    // This class is used to test thread safety of the 'match' method.

  private:
    // DATA
    const RegEx *d_regEx_p;    // prepared RegEx
    const char  *d_subject_p;  // pointer to the subject to match
    const char  *d_match_p;    // pointer to the expected match result

  public:
    // CREATORS
    MatchJob(const RegEx* regEx, const char *subject, const char *match)
        // Create match job object with the specified 'regEx', 'subject',
        // and 'match'.
    : d_regEx_p(regEx)
    , d_subject_p(subject)
    , d_match_p(match)
    {
    };

    // ACCESSORS
    void doMatch() const;
        // Invoke 'match' method on the 'RegEx' object supplied at the
        // construction and verify that the result matches the expected.
};

// ACCESSORS
void MatchJob::doMatch() const
{
    int retCode;

    const size_t subjectLength = strlen(d_subject_p);

    // Return code only match result.
    retCode = d_regEx_p->match(d_subject_p, subjectLength, 0);

    ASSERTV(retCode, 0 == retCode);

    // Pair (offsets) match result.
    pair<size_t, size_t> result1;

    retCode = d_regEx_p->match(&result1, d_subject_p, subjectLength, 0);

    ASSERTV(retCode, 0 == retCode);

    ASSERTV(result1.first,
            result1.second,
            subjectLength >= result1.first + result1.second);

    ASSERTV(d_match_p == bslstl::StringRef(d_subject_p + result1.first,
                                           result1.second));


    // Vector (offsets) match result.
    vector<pair<size_t, size_t> > result2;

    retCode = d_regEx_p->match(&result2, d_subject_p, subjectLength, 0);

    ASSERTV(retCode, 0 == retCode);

    for(size_t i = 0; i < result2.size(); ++i) {
        ASSERTV(i, d_subject_p,
                result2[i].first,
                result2[i].second,
                subjectLength >= result2[i].first + result2[i].second);
    }

    ASSERTV(d_match_p == bslstl::StringRef(d_subject_p + result2[0].first,
                                           result2[0].second));

    // StringRef match result.
    bslstl::StringRef result3;

    retCode = d_regEx_p->match(&result3, d_subject_p, subjectLength, 0);

    ASSERTV(retCode, 0 == retCode);
    ASSERTV(result3, d_match_p == result3);
}

extern "C" void *testMatchFunction(void *threadArg)
    // This thread function performs match test for precompiled 'RegEx' object.
{
    const MatchJob *job = static_cast<const MatchJob *>(threadArg);

    if (veryVeryVerbose) {
        cout << "Match thread started with id: "
             << bslmt::ThreadUtil::selfIdAsUint64()
             << endl;
    }

    for (int i = 0; i < 1024; ++i) {
        job->doMatch();
    }
    return 0;
}

                        // ==========
                        // PrepareJob
                        // ==========

class PrepareJob {
    // This class is used to do performance tests.  This class prepares a
    // pattern in a separate thread, forcing subsequent matches to use non
    // optimal code path (with context allocation).

  private:
    // DATA
    RegEx      *d_regEx_p;       // unprepared RegEx
    const char *d_pattern_p;     // pointer to the pattern
    int         d_flags;         // flags to 'prepare'
    size_t      d_jitStackSize;  // JIT stack size

  public:
    // CREATORS
    PrepareJob(RegEx      *regEx,
               const char *pattern,
               int         flags,
               size_t      jitStackSize)
        // Create match prepare object with the specified 'regEx', 'pattern',
        // 'flags', and 'jitStackSize'
    : d_regEx_p(regEx)
    , d_pattern_p(pattern)
    , d_flags(flags)
    , d_jitStackSize(jitStackSize)
    {
    };

    // MANIPULATORS
    void doPrepare();
        // Invoke 'prepare' method on the 'RegEx' object supplied at the
        // construction.
};

// MANIPULATORS
void PrepareJob::doPrepare()
{
    int retCode = d_regEx_p->prepare(0,
                                     0,
                                     d_pattern_p,
                                     d_flags,
                                     d_jitStackSize);

    ASSERTV(retCode, 0 == retCode);
}

extern "C" void *testPrepareFunction(void *threadArg)
    // This thread function performs match test for precompiled 'RegEx' object.
{
    PrepareJob *job = static_cast<PrepareJob *>(threadArg);

    if (veryVeryVerbose) {
        cout << "Prepare thread started with id: "
             << bslmt::ThreadUtil::selfIdAsUint64()
             << endl;
    }

    job->doPrepare();
    return 0;
}

int countNames(
    const bsl::vector<bsl::pair<bsl::string_view,int> >&  names,
    const char                                           *name)
{
    int count = 0;
    for (unsigned i = 0; i < names.size(); ++i) {
        if (names[i].first == name) {
            ++count;
        }
    }
    return count;
}

}  // close unnamed namespace

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// The following snippets of code illustrate using this component to extract
// the text of the "Subject:" field from an Internet e-mail message (RFC822).
// The following 'parseSubject' function accepts an RFC822-compliant message of
// a specified length and returns the text of the message's subject in the
// 'result' "out" parameter:
//..
    int parseSubject(bsl::string *result,
                     const char  *message,
                     bsl::size_t  messageLength)
//      // Parse the specified 'message' of the specified 'messageLength' for
//      // the "Subject:" field of 'message'.  Return 0 on success and load the
//      // specified 'result' with the text of the subject of 'message'; return
//      // a non-zero value otherwise with no effect on 'result'.
    {
//..
// The following is the regular expression that will be used to find the
// subject text of 'message'.  The "?P<subjectText>" syntax, borrowed from
// Python, allows us later to refer to a particular matched sub-pattern (i.e.,
// the text between the ':' and the '\r' in the "Subject:" field of the header)
// by the name "subjectText":
//..
        const char PATTERN[] = "^subject:(?P<subjectText>[^\r]*)";
//..
// First we compile the 'PATTERN', using the 'prepare' method, in order to
// match subject strings against it.  In the event that 'prepare' fails, the
// first two arguments will be loaded with diagnostic information (an
// informational string and an index into the pattern at which the error
// occurred, respectively).  Two flags, 'RegEx::k_FLAG_CASELESS' and
// 'RegEx::k_FLAG_MULTILINE', are used in preparing the pattern since Internet
// message headers contain case-insensitive content as well as '\n' characters.
// The 'prepare' method returns 0 on success, and a non-zero value otherwise:
//..
        RegEx       regEx;
        bsl::string errorMessage;
        size_t      errorOffset;

        int returnValue = regEx.prepare(&errorMessage,
                                        &errorOffset,
                                        PATTERN,
                                        RegEx::k_FLAG_CASELESS |
                                        RegEx::k_FLAG_MULTILINE);
        ASSERT(0 == returnValue);
//..
// Next we call 'match' supplying 'message' and its length.  The 'matchVector'
// will be populated with (offset, length) pairs describing substrings in
// 'message' that match the prepared 'PATTERN'.  All variants of the overloaded
// 'match' method return 0 if a match is found, and return a non-zero value
// otherwise:
//..
        bsl::vector<bsl::pair<size_t, size_t> > matchVector;
        returnValue = regEx.match(&matchVector, message, messageLength);

        if (0 != returnValue) {
            return returnValue;  // no match                          // RETURN
        }
//..
// Then we pass "subjectText" to the 'subpatternIndex' method to obtain the
// index into 'matchVector' that describes how to locate the subject text
// within 'message'.  The text is then extracted from 'message' and assigned to
// the 'result' "out" parameter:
//..
        const bsl::pair<size_t, size_t> capturedSubject =
                             matchVector[regEx.subpatternIndex("subjectText")];

        *result = bsl::string(&message[capturedSubject.first],
                              capturedSubject.second);

        return 0;
    }

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test           = argc > 1 ? atoi(argv[1]) : 0;

    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    // CONCERN: In no case does memory come from the default allocator.

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 22: {
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
        //:   leading comment characters and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

//..
// The following array contains the sample Internet e-mail message from which
// we will extract the subject:
//..
    const char RFC822_MESSAGE[] =
        "Received: ; Fri, 23 Apr 2004 14:30:00 -0400\r\n"
        "Message-ID: <12345@mailgate.bloomberg.net>\r\n"
        "Date: Fri, 23 Apr 2004 14:30:00 -0400\r\n"
        "From: <someone@bloomberg.net>\r\n"
        "To: <someone_else@bloomberg.net>\r\n"
        "Subject: This is the subject text\r\n"
        "MIME-Version: 1.0\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "This is the message body.\r\n"
        ".\r\n";
//..
// Finally, we call 'parseSubject' to extract the subject from
// 'RFC822_MESSAGE'.  The assertions verify that the subject of the message is
// correctly extracted and assigned to the local 'subject' variable:
//..
//  int main()
//  {
        bsl::string subject;
        const int   returnValue = parseSubject(&subject,
                                               RFC822_MESSAGE,
                                               sizeof(RFC822_MESSAGE) - 1);
        ASSERT(0 == returnValue);
        ASSERT(" This is the subject text" == subject);
//  }
//..
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING DUPLICATE NAMED GROUPS
        //
        // Concerns:
        //: 1 Duplicate sub-pattern names (named groups) are allowed iff
        //:   the pattern is prepared with 'k_FLAG_DUPNAMES' flag or the '(?J)'
        //:   option is used inside the pattern.
        //:
        //: 2 The 'namedSubpatterns' function correctly returns a full set of
        //:   the used named sub-patterns.  Each duplicate name has own
        //:   corresponding index.
        //:
        //: 3 The 'RegEx::match' function works correctly if duplicate
        //:   sub-pattern names are used.
        //:
        //: 4 The 'namedSubpatterns' function work correctly for non-duplicate
        //:   names as well.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create a pattern with duplicated sub-pattern names.  Call the
        //:   'prepare' function:
        //:     1.1 Without 'k_FLAG_DUPNAMES' flag and '(?J)' option.  Verify
        //:         that the function fails.
        //:     1.2 With '(?J)' option.  Verify that the function succeeds.
        //:     1.3 With 'k_FLAG_DUPNAMES' flag.  Verify that the function
        //:         succeeds.
        //:
        //: 2 Verify that the 'subpatternIndex' returns error for the duplicate
        //:   sub-pattern name.
        //:
        //: 3 Call the 'namedSubpatterns' function and verify that the returned
        //:   object contains the duplicate name with more than one index.
        //:
        //: 4 Verify that the 'RegEx::match' function works well for the
        //:   pattern with duplicate sub-pattern names.
        //:
        //: 5 Prepare a pattern with 2 differently named sub-patterns.  Call
        //:   the 'namedSubpatterns' function.  Verify that the set contains
        //    exactly one element for each of the sub-pattern names.
        //:
        //: 6 Do negative testing to verify that asserts catch all the
        //:   undefined behavior in the contract.
        //
        // Testing:
        //   DUPLICATE NAMED GROUPS
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING DUPLICATE NAMED GROUPS"
                          << "\n==============================" << endl;

        typedef bsl::vector<bsl::pair<bsl::string_view,int> > NamedSubpatterns;

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        Obj mX(&ta);  const Obj& X = mX;

        const char PATTERN[] = "(?<group>\\d+)\\.(?<group>\\d+)";
        const char SUBJECT[] = "12.34";

        const char SUBPATTERN_NAME[] = "group";

        if (veryVerbose) {
            T_ P_(PATTERN) P(SUBPATTERN_NAME)
        }

        bsl::string errorMsg;
        size_t      errorOffset;

        if (verbose) cout << "\tVerify not supplying duplicate option"
                             " results in an error" << endl;
        {
            int retCode = mX.prepare(&errorMsg, &errorOffset, PATTERN);
            ASSERTV(errorMsg, errorOffset, retCode != 0);
        }

        if (verbose) cout << "\tSupply (?J) in the regular expression" << endl;
        {
            bsl::string pattern("(?J)");
            pattern += PATTERN;
            int retCode = mX.prepare(&errorMsg, &errorOffset, pattern.c_str());
            ASSERTV(errorMsg, errorOffset, retCode == 0);
        }

        if (verbose) cout << "\tSupply 'k_FLAG_DUPNAMES' flag to prepare" <<
                                                                          endl;
        {
            int retCode = mX.prepare(&errorMsg,
                                     &errorOffset,
                                     PATTERN,
                                     Obj::k_FLAG_DUPNAMES);
            ASSERTV(errorMsg, errorOffset, retCode == 0);
        }

        if (verbose) cout << "\tVerify 'subpatternIndex()' fails with"
                             " duplicate name" << endl;
        {
            ASSERT(X.subpatternIndex(SUBPATTERN_NAME) == -1);
        }

        if (verbose) cout << "\tVerify 'namedSubpatterns()'" << endl;
        {
            NamedSubpatterns subpatterns;
            X.namedSubpatterns(&subpatterns);

            int subpatternsFound = 0;
            for (NamedSubpatterns::const_iterator name  = subpatterns.begin();
                                                  name != subpatterns.end();
                                                  ++name) {
                if (veryVerbose) {
                    cout << name->first << ": " << name->second << '\n';
                }
                if (name->first == SUBPATTERN_NAME) {
                    subpatternsFound++;
                    ASSERT(name->second == 1 || name->second == 2);
                }
            }
            ASSERT(subpatternsFound > 1); // more than one index
        }

        if (verbose) cout << "\tVerify 'RegEx::match()'" << endl;
        {
            bsl::vector<bsl::string_view> matches;
            int retCode = X.match(&matches, SUBJECT);
            ASSERTV(retCode, retCode == 0);

            if (veryVerbose) {
                cout << "Matches (" << matches.size() << "): "<< endl;
                for (unsigned i = 0; i < matches.size(); i++) {
                    cout << i << ": " << matches[i] << endl;
                }
            }
            ASSERTV(matches.size() == 3);
            ASSERTV(matches[0] == SUBJECT);
            ASSERTV(matches[1] == "12");
            ASSERTV(matches[2] == "34");
        }

        if (verbose) cout << "\tVerify that the feature works for"
                             " non-duplicate names as well" << endl;
        {
            const char PATTERN[] = "(?<name1>\\d+)\\.(?<name2>\\d+)";

            int retCode = mX.prepare(&errorMsg, &errorOffset, PATTERN);
            ASSERTV(errorMsg, errorOffset, retCode == 0);

            NamedSubpatterns subpatterns;
            X.namedSubpatterns(&subpatterns);

            ASSERT(countNames(subpatterns, "name1") == 1);
            ASSERT(countNames(subpatterns, "name2") == 1);
            ASSERT(countNames(subpatterns, "nonexistent") == 0);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            NamedSubpatterns subpatterns;
            ASSERT_PASS(X.namedSubpatterns(&subpatterns));

            Obj unprepared(&ta);
            ASSERT_FAIL(unprepared.namedSubpatterns(&subpatterns));
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // JIT REGRESSION
        //   Testing no regression from pcre 3.37
        //
        // Concerns:
        //: 1 Verify that JIT flag does not change results of the match.
        //
        // Plan:
        //: 1 Perform target pattern match with and without jit flag and verify
        //:   that the result is the same.
        //
        // Testing:
        //   JIT REGRESSION
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "JIT REGRESSION" << endl
                          << "==============" << endl;

        const char* PATTERN = "#(A+)#0+";
        const char* SUBJECT = "#A#A#0";

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Obj mX(&oa);

        bsl::string  errorMsg;
        size_t       errorOffset;
        int          retCode = -1;

        retCode = mX.prepare(&errorMsg,
                             &errorOffset,
                             PATTERN,
                             Obj::k_FLAG_JIT,
                             0);
        ASSERTV(retCode, errorMsg, errorOffset, 0 == retCode);

        retCode = mX.match(SUBJECT);
        ASSERTV(retCode, errorMsg, errorOffset, 0 == retCode);

        Obj mY(&oa);

        retCode = mY.prepare(&errorMsg,
                             &errorOffset,
                             PATTERN,
                             0,
                             0);
        ASSERTV(retCode, errorMsg, errorOffset, 0 == retCode);

        retCode = mY.match(SUBJECT);
        ASSERTV(retCode, errorMsg, errorOffset, 0 == retCode);

      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING 'match' THREAD SAFETY
        //
        // Concerns:
        //: 1 'match' can be safely called from the multiple threads.
        //
        // Plan:
        //: 1 Create and prepare a pattern (with and without JIT support)
        //:
        //: 2 Spawn muliple thread and call 'match' from all those thread,
        //:   verify the matchs result in all threads.  C-1)
        //
        // Testing:
        //   CONCERN: 'match' IS THREAD-SAFE
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'match' THREAD SAFETY" << endl
                          << "=============================" << endl;

        const char *SIMPLE_PATTERN     = "X(abc)*Z";
        const char *EMAIL_PATTERN      = "[A-Za-z0-9._-]+@[[A-Za-z0-9.-]+";
        const char *IP_ADDRESS_PATTERN = "(?:[0-9]{1,3}\\.){3}[0-9]{1,3}";

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_pattern;  // pattern string
            const char *d_subject;  // subject string
            const char *d_match;    // match string
        } DATA[] = {
            //line pattern             subject            match
            //---- ------------------  -----------------  -----------------
            { L_,  SIMPLE_PATTERN,     "XXXabcZZZ",       "XabcZ"           },
            { L_,  SIMPLE_PATTERN,     "XXXabcabcZZZ",    "XabcabcZ"        },
            { L_,  EMAIL_PATTERN,      "john@bloom.net",  "john@bloom.net"  },
            { L_,  IP_ADDRESS_PATTERN, "255.255.255.255", "255.255.255.255" },
            { L_,  IP_ADDRESS_PATTERN, "a10.10.196.1:",   "10.10.196.1"     },
        };

        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        bsl::string  errorMsg;
        size_t       errorOffset;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
                const int   LINE    = DATA[ti].d_lineNum;
                const char *PATTERN = DATA[ti].d_pattern;
                const char *SUBJECT = DATA[ti].d_subject;
                const char *MATCH   = DATA[ti].d_match;

                Obj mX(&oa);  const Obj& X = mX;

                int retCode = -1;

                switch (cfg) {
                  case 'a': {
                    retCode = mX.prepare(&errorMsg,
                                         &errorOffset,
                                         PATTERN,
                                         0,
                                         0);
                  } break;
                  case 'b': {
                    retCode = mX.prepare(&errorMsg,
                                         &errorOffset,
                                         PATTERN,
                                         Obj::k_FLAG_JIT,
                                         0);
                  } break;
                  case 'c': {
                    retCode = mX.prepare(&errorMsg,
                                         &errorOffset,
                                         PATTERN,
                                         Obj::k_FLAG_JIT,
                                         8192);
                  } break;
                  default: {
                    ASSERT(!"Invalid configuration!");
                  }
                }
                ASSERTV(LINE, errorMsg, errorOffset, 0 == retCode);

                MatchJob job(&X, SUBJECT, MATCH);

                bslmt::ThreadUtil::Handle threads[8];

                for (int i = 0; i < 8; ++i) {
                    int rc = bslmt::ThreadUtil::create(&threads[i],
                            testMatchFunction,
                            &job);
                    ASSERTV(rc, 0 == rc);
                }

                for (int i = 0; i < 8; ++i) {
                    int rc = bslmt::ThreadUtil::join(threads[i]);
                    ASSERTV(rc, 0 == rc);
                }
            }
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING MEMORY ALIGNMENT
        //
        // Concerns:
        //: 1 That alignment requirements aren't violated.
        //
        // Plan:
        //: 1 Run this test built with the Sun CC compiler and linker with the
        //:   '-xmemalign=8s' option set.  If alignment violations occur, there
        //:   will be a bus error.  (C-1)
        //
        // Testing:
        //   MEMORY ALLIGNMENT
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING MEMORY ALIGNMENT" << endl
                          <<"=========================" << endl;

        static const char k_PATTERN[] =
                  "\\b[A-Za-z0-9._-]+@[A-Za-z0-9\\.\\-]+\\.[A-Za-zaz]{2,4}\\b";

        bsls::AlignedBuffer<256>           buffer;
        bdlma::BufferedSequentialAllocator arena(buffer.buffer(),
                                                 sizeof(buffer));

        Obj regex(&arena);

        regex.prepare(0, 0, k_PATTERN, 0);
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING UNICODE CHARACTER PROPERTY SUPPORT
        //   This will test that the underlying PCRE2 library correctly handles
        //   \p{..}, \P{..} and \X escape sequences
        //
        // Concerns:
        //: 1 We want to make sure that \p{..}, \P{..} and \X escape sequences
        //:   can be used in patterns.
        //
        // Plan:
        //: 1 Create a set of UTF8 patterns containing required escape
        //:   sequences.
        //:
        //: 2 Exercise the match function using a UTF8 subject that matches the
        //:   pattern being tested.
        //
        // Testing:
        //   UNICODE CHARACTER PROPERTY SUPPORT
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                       << "TESTING UNICODE CHARACTER PROPERTY SUPPORT" << endl
                       << "==========================================" << endl;

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        static const struct {
            int            d_lineNum;         // source line number
            const char    *d_pattern;         // pattern string
            unsigned char  d_utf8Subject[2];  // utf8 subject
            bool           d_match;           // match flag
        } DATA[] = {
            //line   pattern      utf8Subject     match
            //----   -------      -----------     -----
            // \p{L} - any Unicode letter
            { L_,    "\\p{L}",    { 0xC3, 0x80 }, true  }, // 'A' with grave
            { L_,    "\\p{L}",    { 0xC3, 0x91 }, true  }, // 'N' with tilde
            { L_,    "\\p{L}",    { 0xC2, 0xAE }, false }, // Registered sign
            { L_,    "\\p{L}",    { 0xC2, 0xB0 }, false }, // Degree sign

            // \P{L} - any Unicode non-letter
            { L_,    "\\P{L}",    { 0xC3, 0x80 }, false }, // 'A' with grave
            { L_,    "\\P{L}",    { 0xC3, 0x91 }, false }, // 'N' with tilde
            { L_,    "\\P{L}",    { 0xC2, 0xAE }, true  }, // Registered sign
            { L_,    "\\P{L}",    { 0xC2, 0xB0 }, true  }, // Degree sign

            // \P{L} - any valid Unicode
            { L_,    "\\X",       { 0xC3, 0x80 }, true  }, // 'A' with grave
            { L_,    "\\X",       { 0xC3, 0x91 }, true  }, // 'N' with tilde
            { L_,    "\\X",       { 0xC2, 0xAE }, true  }, // Registered sign
            { L_,    "\\X",       { 0xC2, 0xB0 }, true  }, // Degree sign

            // Assorted list of character properties
            { L_,    "\\p{Ll}",   { 0xC3, 0xA1 }, true  }, // 'a' with acute
            { L_,    "\\p{Lu}",   { 0xC3, 0xA1 }, false }, // 'a' with acute
            { L_,    "\\p{Ll}",   { 0xC5, 0x90 }, false }, // 'O' with dblacute
            { L_,    "\\p{Lu}",   { 0xC5, 0x90 }, true  }, // 'O' with dblacute
        };
        const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nRunning test data." << endl;

        for (size_t i = 0; i < NUM_DATA; ++i) {
            const int   LINE    = DATA[i].d_lineNum;
            const char *PATTERN = DATA[i].d_pattern;

            if (veryVerbose) {
                cout << "\tPreparing the regular expression objects.  ";
                P_(LINE) P(PATTERN)
            }

            Obj mX(&ta); const Obj& X = mX;

            bsl::string errorMsg;
            size_t      errorOffset;

            int retCode = mX.prepare(&errorMsg,
                                     &errorOffset,
                                     PATTERN,
                                     Obj::k_FLAG_UTF8);

            ASSERTV(errorMsg, errorOffset, 0 == retCode);

            const unsigned char *UTF8_SUBJECT = DATA[i].d_utf8Subject;
            const bool           MATCH        = DATA[i].d_match;

            retCode = X.match((const char*)UTF8_SUBJECT, 2);

            ASSERTV(LINE, retCode, MATCH == !retCode);
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING 'replace' and 'replaceRaw' METHODS
        //
        // Concerns:
        //: 1 Although we are not testing the implementation of the PCRE
        //:   library, we want to check that the interface is plugged in
        //:   correctly and works as documented.  In particular, we want to
        //:   check that the 'subject', 'replacement', and 'options'
        //:   arguments are passed to PCRE correctly and the resulting string
        //:   is copied correctly to the 'result'.
        //:
        //: 2 'replace' and 'replaceRaw' returns an error and a position of the
        //:   error is loaded to 'errorOffset' when a syntax error detected in
        //:   the replacement string.
        //:
        //: 3 If the size of the resulting string does not match the supplied
        //:   string 'result', then 'replace' and 'replaceRaw' computes the
        //:   required size and resizes 'result' to hold the result.
        //:
        //: 4 'options' are reflected correctly to the corresponding PCRE flags
        //:   and propagated to the underlying PCRE2 substitution function as
        //:   expected.
        //:
        //: 5 If 'options' is not supplied then 'options' is set to 0 by
        //:   default.
        //:
        //: 6 No memory is allocated from the default allocator.
        //
        // Plan:
        //: 1 Prepare a regular expression object using a given 'PATTERN'
        //:   string, create a set of subjects that contain a match for
        //:   'PATTERN'.
        //:
        //: 2 Exercise the 'replace' and 'replaceRaw'  methods using
        //:   'replacement' and 'options' values.
        //
        // Testing:
        //   int replace(bsl::string *, int *,  ...) const;
        //   int replace(std::string *, int *, const string_view&, ...) const;
        //   int replace(std::pmr::string *, int *, ...) const;
        //   int replaceRaw(bsl::string *, int *,  ...) const;
        //   int replaceRaw(std::string *, int *,  ...) const;
        //   int replaceRaw(std::pmr::string *, int *, ...) const;
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING 'replace' and 'replaceRaw' METHOD" << endl
                 << "=========================================" << endl;

        enum {
            k_L   = Obj::k_REPLACE_LITERAL,
            k_G   = Obj::k_REPLACE_GLOBAL,
            k_X   = Obj::k_REPLACE_EXTENDED,
            k_U   = Obj::k_REPLACE_UNKNOWN_UNSET,
            k_E   = Obj::k_REPLACE_UNSET_EMPTY,
            k_GUE = k_G | k_U | k_E
        };

        bslma::TestAllocatorMonitor dam(&defaultAllocator);
        bslma::TestAllocator        ta(veryVeryVeryVerbose);

        {
            const char PATTERN[] = "a(b)?(c)?";

            static const struct {
                int         d_lineNum;      // source line number
                const char *d_subject;      // subject string
                const char *d_replacement;
                size_t      d_flags;
                const char *d_expectedString;
                int         d_expectedResult;
                int         d_expectedOffset;
            } DATA[] = {
//                                  expected     expected  expected  error
//line   subject      replacement    flags        string      rc     offset
//----   -------      -----------   --------     --------  --------  ------
{ L_,    "=**=",      "A",              0,       "=**=",      0,        0  },
{ L_,    "=ac=",      "",               0,       "==",        1,        0  },
{ L_,    "=ac=",      "A",              0,       "=A=",       1,        0  },
{ L_,    "=abc=",     "A",              0,       "=A=",       1,        0  },
{ L_,    "=abc=abc=", "A",              0,       "=A=abc=",   1,        0  },
{ L_,    "=abc=",     "+$1$0$1+",       0,       "=+babcb+=", 1,        0  },
{ L_,    "=abc=",     "+${1$0$1+",      0,       "",         -1,        4  },
{ L_,    "=abc=",     "+$3+",           0,       "",         -1,        3  },
{ L_,    "=ab=",      "+$2+",           0,       "",         -1,        3  },
// k_REPLACE_LITERAL ------------------------------------------------------
{ L_,    "=abc=",     "+$1+",           0,       "=+b+=",     1,        0  },
{ L_,    "=abc=",     "+$1+",         k_L,       "=+$1+=",    1,        0  },
// k_REPLACE_GLOBAL -------------------------------------------------------
{ L_,    "=abc=abc=", "A",            k_G,       "=A=A=",     2,        0  },
// k_REPLACE_EXTENDED -----------------------------------------------------
{ L_,    "=abc=",     "+${1:-D}+",    k_X,       "=+b+=",     1,        0  },
{ L_,    "=ac=",      "+${1:-D}+",    k_X,       "=+D+=",     1,        0  },
{ L_,    "=abc=",     "+${1:+D:d}+",  k_X,       "=+D+=",     1,        0  },
{ L_,    "=ac=",      "+${1:+D:d}+",  k_X,       "=+d+=",     1,        0  },
{ L_,    "=ab=",      "+${1:+D:d}"
                       "${2:+E:e}+",  k_X,       "=+De+=",    1,        0  },
{ L_,    "=ab=ac=",   "+${1:+D:d}"
                       "${2:+E:e}+",  k_X | k_G, "=+De+="
                                                  "+dE+=",    2,        0  },
// k_REPLACE_UNKNOWN_UNSET and k_REPLACE_UNSET_EMPTY-----------------------
{ L_,    "=ac=",      "+$3+",         k_U,       "",         -1,        3  },
{ L_,    "=ac=",      "+$0$1$0+",     k_E,       "=+acac+=",  1,        0  },
{ L_,    "=ac=",      "+$0$3$0+",     k_E,       "",         -1,        5  },
{ L_,    "=ac=",      "+$0$3$0+",     k_E | k_U, "=+acac+=",  1,        0  },
// All flags except k_REPLACE_EXTENDED ------------------------------------
{ L_,    "=ab=ac=",   "+$0$1$2+",     k_GUE,     "=+abb+="
                                                  "+acc+=",   2,        0  }
        };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            Obj mX(&ta); const Obj& X = mX;

            bsl::string errorMsg(&ta);
            size_t      errorOffset;

            int retCode = mX.prepare(&errorMsg, &errorOffset, PATTERN, 0, 0);

            ASSERTV(errorMsg, errorOffset, 0 == retCode);

            for (size_t i = 0; i < NUM_DATA; ++i) {
                const int     LINE            = DATA[i].d_lineNum;
                const char   *SUBJECT         = DATA[i].d_subject;
                const char   *REPLACEMENT     = DATA[i].d_replacement;
                const size_t  FLAGS           = DATA[i].d_flags;
                const char   *EXPECTED        = DATA[i].d_expectedString;
                const size_t  EXPECTED_LENGTH = bsl::strlen(EXPECTED);
                const int     RC              = DATA[i].d_expectedResult;
                const int     OFFSET          = DATA[i].d_expectedOffset;

                if (veryVerbose) {
                    T_ P_(LINE)  P_(SUBJECT) P_(REPLACEMENT)
                       P_(FLAGS) P_(RC)      P(EXPECTED)
                }

                for (size_t j = 0; j < EXPECTED_LENGTH + 1; ++j) {

                    { // replace
                        int         offset = 0;
                        bsl::string result1(j, 0, &ta);

                        retCode = X.replace(&result1,
                                            &offset,
                                            SUBJECT,
                                            REPLACEMENT,
                                            FLAGS);

                        ASSERTV(LINE, RC, retCode,   RC       == retCode);
                        ASSERTV(LINE,     retCode,   OFFSET,     offset,
                                     0 <= retCode || OFFSET   == offset);
                        ASSERTV(LINE,     retCode,   EXPECTED,   result1,
                                     0 >  retCode || EXPECTED == result1);

                        std::string result2(10000, 0);

                        retCode = X.replace(&result2,
                                            &offset,
                                            SUBJECT,
                                            REPLACEMENT,
                                            FLAGS);

                        ASSERTV(LINE, RC, retCode,   RC       == retCode);
                        ASSERTV(LINE,     retCode,   OFFSET,     offset,
                                     0 <= retCode || OFFSET   == offset);
                        ASSERTV(LINE,     retCode,   EXPECTED,   result2,
                                     0 >  retCode || EXPECTED == result2);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                        std::pmr::string result3(j, 0);

                        retCode = X.replace(&result3,
                                            &offset,
                                            SUBJECT,
                                            REPLACEMENT,
                                            FLAGS);

                        ASSERTV(LINE, RC, retCode,   RC       == retCode);
                        ASSERTV(LINE,     retCode,   OFFSET,     offset,
                                     0 <= retCode || OFFSET   == offset);
                        ASSERTV(LINE,     retCode,   EXPECTED,   result3,
                                     0 >  retCode || EXPECTED == result3);
#endif
                    }
                    { // replaceRaw
                        int         offset = 0;
                        bsl::string result1(j, 0, &ta);

                        retCode = X.replaceRaw(&result1,
                                               &offset,
                                               SUBJECT,
                                               REPLACEMENT,
                                               FLAGS);

                        ASSERTV(LINE, RC, retCode,   RC       == retCode);
                        ASSERTV(LINE,     retCode,   OFFSET,     offset,
                                     0 <= retCode || OFFSET   == offset);
                        ASSERTV(LINE,     retCode,   EXPECTED,   result1,
                                     0 >  retCode || EXPECTED == result1);

                        std::string result2(10000, 0);

                        retCode = X.replaceRaw(&result2,
                                               &offset,
                                               SUBJECT,
                                               REPLACEMENT,
                                               FLAGS);

                        ASSERTV(LINE, RC, retCode,   RC       == retCode);
                        ASSERTV(LINE,     retCode,   OFFSET,     offset,
                                     0 <= retCode || OFFSET   == offset);
                        ASSERTV(LINE,     retCode,   EXPECTED,   result2,
                                     0 >  retCode || EXPECTED == result2);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                        std::pmr::string result3(j, 0);

                        retCode = X.replaceRaw(&result3,
                                               &offset,
                                               SUBJECT,
                                               REPLACEMENT,
                                               FLAGS);

                        ASSERTV(LINE, RC, retCode,   RC       == retCode);
                        ASSERTV(LINE,     retCode,   OFFSET,     offset,
                                     0 <= retCode || OFFSET   == offset);
                        ASSERTV(LINE,     retCode,   EXPECTED,   result3,
                                     0 >  retCode || EXPECTED == result3);
#endif
                    }
                }
            }
        }

        {  // Test NO UTF8 CHECK

            const char          *PATTERN      = "\\xC0";
            const unsigned char  SUBJECT_UTF8[2] = { 0xC3, 0x80 };
            const unsigned char  SUBJECT_REGULAR =   0xC0;

            const unsigned char  REPLACE_UTF8[2] = { 0xC3, 0x80 };
            const unsigned char  REPLACE_REGULAR =   0xC0;

            static const struct {
                int         d_lineNum;      // source line number
                int         d_prepareOptions;
                bool        d_UTF8Validation;
                int         d_resultUTF8;
                int         d_resultRegular;
            } DATA[] = {
            //Line Prepare Options  Validation UTF8 Regular
            //---- ---------------  ---------- ---- -------
            { L_,                0,          1,   0,    1   },
            { L_,                0,          0,   0,    1   },
            { L_, Obj::k_FLAG_UTF8,          1,   1,   -1   },
            { L_, Obj::k_FLAG_UTF8,          0,   1,    0   },
            };

            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < NUM_DATA; ++i) {

                const int    LINE            = DATA[i].d_lineNum;
                const int    PREPARE_OPTIONS = DATA[i].d_prepareOptions;
                const bool   UTF8_VALIDATION = DATA[i].d_UTF8Validation;
                const int    RESULT_UTF8     = DATA[i].d_resultUTF8;
                const int    RESULT_REGULAR  = DATA[i].d_resultRegular;

                Obj mX(&ta); const Obj& X = mX;

                bsl::string errorMsg(&ta);
                size_t      errorOffset;
                int         retCode = mX.prepare(&errorMsg,
                                                 &errorOffset,
                                                 PATTERN,
                                                 PREPARE_OPTIONS);

                ASSERTV(LINE, i, errorMsg, errorOffset, 0 == retCode);
                ASSERTV(LINE, i, X.flags(), PREPARE_OPTIONS == X.flags());

                bsl::string result(&ta);
                int         offset = 0;

                retCode = UTF8_VALIDATION
                          ? X.replace(&result,
                                    &offset,
                                    string_view((const char *)SUBJECT_UTF8, 2),
                                    string_view((const char *)REPLACE_UTF8, 2),
                                    0)
                          : X.replaceRaw(&result,
                                    &offset,
                                    string_view((const char *)SUBJECT_UTF8, 2),
                                    string_view((const char *)REPLACE_UTF8, 2),
                                    0);

                ASSERTV(LINE, i, retCode, RESULT_UTF8 == retCode);

                retCode = UTF8_VALIDATION
                          ? X.replace(
                                &result,
                                &offset,
                                string_view((const char *)&SUBJECT_REGULAR, 1),
                                string_view((const char *)&REPLACE_REGULAR, 1),
                                0)
                          : X.replaceRaw(
                                &result,
                                &offset,
                                string_view((const char *)&SUBJECT_REGULAR, 1),
                                string_view((const char *)&REPLACE_REGULAR, 1),
                                0);

                ASSERTV(LINE, i, retCode, RESULT_REGULAR == retCode);

                ASSERTV(LINE, i, offset,  0 <= retCode || 0 > offset);
            }
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING DEPTH LIMIT
        //  This will test both the default and per-regex depth limit
        //  attributes.
        //
        // Concerns:
        //: 1 The object depth limit attribute should take on the default value
        //:   by default, and be modifiable by the relevant accessor.
        //:
        //: 2 The object depth limit attribute should limit the recursion depth
        //:   for regular expression matches.
        //
        // Plan:
        //: 1 Default-construct a regular expression, and make sure that the
        //:   depth limit matches the process default.  (C-1)
        //:
        //: 2 Modify the process default, making sure that the already
        //:   constructed regex is not affected, that the process default
        //:   accessor returns the new value, and that a new regex object is
        //:   affected.  (C-1)
        //:
        //: 3 Modify the depth limit for both objects, and make sure they don't
        //:   affect each other or the default value.  (C-1)
        //:
        //: 4 Modify the depth limit for a regular expression and make sure it
        //:   affects the behaviour of the various 'match' overloads as
        //:   expected.  (C-2)
        //
        // Testing:
        //   int setDepthLimit(int);
        //   int setDefaultDepthLimit(int);
        //   int depthLimit(int);
        //   int defaultDepthLimit(int);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING DEPTH LIMIT" << endl
                          << "===================" << endl;

        Obj x;
        int originalDepthLimit = x.depthLimit();

        ASSERT(x.depthLimit() == Obj::defaultDepthLimit());

        int previousGlobalLimit = Obj::setDefaultDepthLimit(3);

        ASSERT(3                  == Obj::defaultDepthLimit());
        ASSERT(3                  != originalDepthLimit);
        ASSERT(originalDepthLimit == x.depthLimit());
        ASSERT(originalDepthLimit == previousGlobalLimit);

        Obj y;

        ASSERT(y.depthLimit() == Obj::defaultDepthLimit());

        int previousXLimit = x.setDepthLimit(5);

        ASSERT(5              == x.depthLimit());
        ASSERT(5              != originalDepthLimit);
        ASSERT(3              == Obj::defaultDepthLimit());
        ASSERT(previousXLimit == previousGlobalLimit);
        ASSERT(y.depthLimit() == Obj::defaultDepthLimit());

        const char *testString       = "a\n\n\n\n\nb";
        bsl::size_t testStringLength = bsl::strlen(testString);
        const char *testRegex        = "a(\n)+b";

        bsl::string errorMessage;
        size_t      errorOffset;

        bsl::pair<size_t, size_t>               resultPair;
        bsl::vector<bsl::pair<size_t, size_t> > resultVector;

        // We expect this to fail at depth 3, since it requires depth 14.
        ASSERT(0 == y.prepare(&errorMessage, &errorOffset, testRegex));
        ASSERT(3 == y.depthLimit());
        // Match failure due to depth limit returns 1.
        ASSERT(1 == y.match(testString, testStringLength));
        ASSERT(1 == y.match(&resultPair, testString, testStringLength));
        ASSERT(1 == y.match(&resultVector, testString, testStringLength));

        // We expect this to fail at depth 5.
        ASSERT(0 == x.prepare(&errorMessage, &errorOffset, testRegex));
        ASSERT(5 == x.depthLimit());
        // Match failure due to depth limit returns 1.
        ASSERT(1 == x.match(testString, testStringLength));
        ASSERT(1 == x.match(&resultPair, testString, testStringLength));
        ASSERT(1 == x.match(&resultVector, testString, testStringLength));

        // We know this succeeds at depth 14
        x.setDepthLimit(14);
        ASSERT(14 == x.depthLimit());
        // Successful match returns 0.
        ASSERT( 0 == x.match(testString, testStringLength));
        ASSERT( 0 == x.match(&resultPair, testString, testStringLength));
        ASSERT( 0 == x.match(&resultVector, testString, testStringLength));
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING JIT OPTIMIZATION SUPPORT
        //  As component doesn't provide a mechanism to find out if JIT
        //  optimization is used for matching, the only way to confirm it's
        //  using is comparison of memory allocations. JIT optimization needs
        //  some memory to store the result of pattern JIT compiling. Also
        //  JIT stack machinery occupies some memory (if it is allocated).
        //  These statements are matter only if JIT optimization is supported
        //  on hardware platform.
        //
        // Concerns:
        //: 1 JIT optimization is used only if it is supported on current
        //:   hardware platform.
        //:
        //: 2 All actions, necessary for JIT optimization, are performed if
        //:   respective flag is provided to 'prepare' method.
        //:
        //: 3 JIT stack size can be set via last parameter of 'prepare' method.
        //:
        //: 4 Proper failure code is returned when the memory used for the JIT
        //:   stack is insufficient.
        //:
        //: 5 'jitStackSize' returns the requested size of the dynamically
        //:   allocated JIT stack if it was set by user and '0' otherwise.
        //
        // Plan:
        //: 1 Create two objects using two different test allocators. Provide
        //:   default flag value to 'prepare' method for the first object and
        //:   k_JIT_FLAG for another. Verify that second method's call invokes
        //:   more memory allocations than the first one, if JIT optimization
        //:   is supported and the same number, if it isn't.  Verify that
        //:   'jitStackSize' method returns correct value.  (C-1,2,4)
        //:
        //: 2 Create two objects using two different test allocators. Provide
        //:   k_JIT_FLAG to 'prepare' method for both of them. Provide default
        //:   jitStackSize value to 'prepare' method for the first object and
        //:   non-default for another. Verify that second method's call invokes
        //:   more memory allocations than the first one, if JIT optimization
        //:   is supported and the same number, if it isn't.  Verify that
        //:   'jitStackSize' method returns correct value.  (C-1,2,4)
        //:
        //: 3 Create an object. Provide k_JIT_FLAG and zero jitStackSize value
        //:   to 'prepare' method.  Exercise the 'match' method using a subject
        //:   that matches the pattern passed to 'prepare' method.  Verify that
        //:   method call succeeds.  Provide the same pattern, k_JIT_FLAG and
        //:   tiniest non-zero jitStackSize value to 'prepare' method.
        //:   Exercise the 'match' method using the same subject.  Verify that
        //:   method call fails, if JIT optimization is supported and succeeds,
        //:   if it isn't.  Provide the same pattern k_JIT_FLAG and bigger
        //:   jitStackSize value to 'prepare' method.  Exercise the 'match'
        //:   method using the same subject.  Verify that method call succeeds.
        //:   After each 'prepare' method call verify that 'jitStackSize'
        //:   method returns correct value.  (C-1,2..4)

        //
        // Testing:
        //  JIT OPTIMIZATION SUPPORT
        //  size_t jitStackSize() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING JIT OPTIMIZATION SUPPORT" << endl
                          << "================================" << endl;

        const char PATTERN[] = "(A)*";
        const int  SUBJECT_LENGTH = 1024;
        char       SUBJECT[SUBJECT_LENGTH];

        for (int i = 0; i < SUBJECT_LENGTH ; ++i) {
            SUBJECT[i] = 'A';
        }

        bsl::string errorMsg;
        size_t      errorOffset;

        {
            if (verbose) {
                cout << "\nJIT compiling verification." << endl;
            }

            bslma::TestAllocator allocator1(veryVeryVeryVerbose);
            bslma::TestAllocator allocator2(veryVeryVeryVerbose);

            bslma::TestAllocator *Z1 = &allocator1;
            bslma::TestAllocator *Z2 = &allocator2;

            Obj mX1(Z1);  const Obj& X1 = mX1;
            Obj mX2(Z2);  const Obj& X2 = mX2;

            ASSERT(Z1->numAllocations() == Z2->numAllocations());

            ASSERT(0 == mX1.prepare(&errorMsg,
                                    &errorOffset,
                                    PATTERN,
                                    0));
            ASSERT(0 == mX2.prepare(&errorMsg,
                                    &errorOffset,
                                    PATTERN,
                                    Obj::k_FLAG_JIT));

            ASSERT(0               == X1.flags());
            ASSERT(Obj::k_FLAG_JIT == X2.flags());
            ASSERT(0               == X1.jitStackSize());
            ASSERT(0               == X2.jitStackSize());

            if (Obj::isJitAvailable()) {
                ASSERT(Z1->numAllocations() <  Z2->numAllocations());
            } else {
                ASSERT(Z1->numAllocations() == Z2->numAllocations());
            }
        }

        {
            if (verbose) {
                cout << "\nTesting JIT stack allocation." << endl;
            }

            bslma::TestAllocator allocator1(veryVeryVeryVerbose);
            bslma::TestAllocator allocator2(veryVeryVeryVerbose);

            bslma::TestAllocator *Z1 = &allocator1;
            bslma::TestAllocator *Z2 = &allocator2;

            Obj mX1(Z1);  const Obj& X1 = mX1;
            Obj mX2(Z2);  const Obj& X2 = mX2;

            ASSERT(Z1->numAllocations() == Z2->numAllocations());

            ASSERT(0 == mX1.prepare(&errorMsg,
                                    &errorOffset,
                                    PATTERN,
                                    Obj::k_FLAG_JIT,
                                    0));
            ASSERT(0 == mX2.prepare(&errorMsg,
                                    &errorOffset,
                                    PATTERN,
                                    Obj::k_FLAG_JIT,
                                    1));

            ASSERT(Obj::k_FLAG_JIT == X1.flags());
            ASSERT(Obj::k_FLAG_JIT == X2.flags());
            ASSERT(0               == X1.jitStackSize());
            if (Obj::isJitAvailable()) {
                ASSERT(1                    == X2.jitStackSize());
                ASSERT(Z1->numAllocations() <  Z2->numAllocations());
            } else {
                ASSERT(0                    == X2.jitStackSize());
                ASSERT(Z1->numAllocations() == Z2->numAllocations());
            }
        }

        {
            if (verbose) {
                cout << "\nTesting failure code returning." << endl;
            }

            Obj mX;  const Obj& X = mX;

            // Default 32k stack is used.

            ASSERT(0 == mX.prepare(&errorMsg,
                                   &errorOffset,
                                   PATTERN,
                                   Obj::k_FLAG_JIT,
                                   0));
            ASSERT(0 == X.jitStackSize());
            ASSERT(0 == X.match(SUBJECT, SUBJECT_LENGTH));

            // Smallest allocated stack is used.

            ASSERT(0 == mX.prepare(&errorMsg,
                                   &errorOffset,
                                   PATTERN,
                                   Obj::k_FLAG_JIT,
                                   1));
            if (Obj::isJitAvailable()) {
                ASSERT(1 == X.jitStackSize());
                ASSERT(2 == X.match(SUBJECT, SUBJECT_LENGTH));
            } else {
                ASSERT(0 == X.jitStackSize());
                ASSERT(0 == X.match(SUBJECT, SUBJECT_LENGTH));
            }

            // Allocated 32k stack is used.

            ASSERT(0 == mX.prepare(&errorMsg,
                                   &errorOffset,
                                   PATTERN,
                                   Obj::k_FLAG_JIT,
                                   32768));
            if (Obj::isJitAvailable()) {
                ASSERT(32768 == X.jitStackSize());
            } else {
                ASSERT(0     == X.jitStackSize());
            }
            ASSERT(0 == X.match(SUBJECT, SUBJECT_LENGTH));
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'isJitAvailable' METHOD
        //
        // Concerns:
        //: 1 The 'isJitAvailable' returns correct hardcoded value in
        //:   accordance with hardware platform.
        //
        // Plan:
        //: 1 Call 'isJitAvailable' method and compare returned result with the
        //:   hardcoded expected value.  (C-1)
        //
        // Testing:
        //   bool isJitAvailable();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'isJitAvailable' METHOD" << endl
                          << "===============================" << endl;

#if defined(BSLS_PLATFORM_CPU_SPARC_V9)
        ASSERT(false == Obj::isJitAvailable());
#elif defined(BSLS_PLATFORM_CPU_ARM) && defined(BSLS_PLATFORM_CPU_64_BIT)
        ASSERT(false == Obj::isJitAvailable());
#else
        ASSERT(true == Obj::isJitAvailable());
#endif

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING ALLOCATOR PROPAGATION
        //
        // Concerns:
        //: 1 We want to make sure that the allocator is propagated properly
        //:   and that all memory allocation is done by the supplied allocator.
        //
        // Plan:
        //: 1 Create a set of patterns using a test allocator, and use a
        //:   default allocator guard to measure the amount of memory allocated
        //:   via the default allocator.  Verify that all memory is allocated
        //:   by the test allocator while exercising the pattern.  (C-1)
        //
        // Testing:
        //   ALLOCATOR PROPAGATION
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ALLOCATION PROPAGATION" << endl
                          << "==============================" << endl;

        bslma::TestAllocator allocator0(veryVeryVeryVerbose);
        bslma::TestAllocator allocator1(veryVeryVeryVerbose);
        bslma::TestAllocator allocator2(veryVeryVeryVerbose);

        bslma::TestAllocator *Z0 = &allocator0;
        bslma::TestAllocator *Z1 = &allocator1;
        bslma::TestAllocator *Z2 = &allocator2;

        bslma::DefaultAllocatorGuard allocGuard(Z0);

        {
            Obj mX(Z1);  const Obj& X = mX;

            bsl::string errorMsg(Z2);          // do not use default allocator!
            size_t      errorOffset;

            const char PATTERN[]         = "(?P<pkgName>[a-z]+)_([A-Za-z]+)";

            ASSERT(0 == mX.prepare(&errorMsg,
                                   &errorOffset,
                                   PATTERN,
                                   Obj::k_FLAG_MULTILINE));

            const char   TEST_STRING[]   = "bbasm_SecurityCache\n"
                                           "bdlt_Date\n";
            const size_t TEST_STRING_LEN = sizeof(TEST_STRING) - 1;

            bsl::pair<size_t, size_t> match;
            ASSERT(0 == X.match(&match, TEST_STRING, TEST_STRING_LEN));

            {
                const char  EXPECTED_MATCH[] = "bbasm_SecurityCache";
                bsl::string realMatch(&TEST_STRING[match.first],
                        match.second, Z2);     // do not use default allocator!
                ASSERTV(realMatch, EXPECTED_MATCH == realMatch);
            }

            const size_t startPosition = match.second;
            X.match(&match, TEST_STRING, TEST_STRING_LEN, startPosition);

            {
                const char  EXPECTED_MATCH[] = "bdlt_Date";
                bsl::string realMatch(&TEST_STRING[match.first],
                        match.second, Z2);     // do not use default allocator!
                ASSERTV(realMatch, EXPECTED_MATCH == realMatch);
            }

            bsl::vector<pair<size_t, size_t> > vMatch(Z2);
                                               // do not use default allocator!
            X.match(&vMatch, TEST_STRING, TEST_STRING_LEN);

            const size_t LEN = 3;
            const char   EXPECTED_MATCHES[LEN][30] = { "bbasm_SecurityCache",
                                                       "bbasm",
                                                       "SecurityCache" };

            ASSERTV(vMatch.size(), LEN == (int)vMatch.size());
            for (size_t i = 0; i < LEN; ++i) {
                bsl::string realMatch(&TEST_STRING[vMatch[i].first],
                        vMatch[i].second, Z2); // do not use default allocator!

                ASSERTV(i, realMatch, EXPECTED_MATCHES[i] == realMatch);
            }
        }
        ASSERT(0 == Z0->numAllocations());
        ASSERT(0 != Z1->numAllocations());

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING SUBPATTERNS
        //   This will test the 'numSubpatterns' and 'subpatternIndex'
        //   accessors.  It will also test the vector 'match' and 'matchRaw'
        //   functions to verify that it returns the captured substrings
        //   correctly.
        //
        // Concerns:
        //: 1 We want to make sure that subpatterns are recognized correctly
        //:   and also that captured substrings can be retrieved.
        //
        // Plan:
        //: 1 Create a set of patterns, with increasing number of subpatterns.
        //:   The pattern used will be a regular expression with the capability
        //:   of splitting a BDE-style class name into package name & class
        //:   name, and also split the class name into separate words.  For
        //:   example, 'bbasm_SecurityDataChunkResolver' will be split into
        //:   package name 'bbasm' and class name 'SecurityDataChunkResolver'.
        //:   Further, the class name will be split into separate words
        //:   'Security', 'Data', 'Chunk', and 'Resolver'.  Each word is
        //:   represented by a sub-pattern in the regular expression.  Since
        //:   the set contains patterns with increasing number of subpatterns,
        //:   the number of words that a pattern can identify will also
        //:   increase.  The test data contains 'numWords' values from 0 to 4.
        //:   This also tests "nested" subpatterns (the 'words' sub-patterns
        //:   are nested inside the 'class-name' sub-pattern).  (C-1)
        //
        // Testing:
        //   int numSubpatterns() const;
        //   int subpatternIndex(const char*) const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl << "TESTING SUBPATTERNS" << endl
                                  << "===================" << endl;

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        static const struct {
            int         d_lineNum;   // source line number
            const char *d_pattern;   // pattern
            int         d_numWords;  // number of words recognized
        } PATTERNS[] = {
            //line  pattern                                        numWords
            //----  -------                                        --------
            { L_,   "(?P<pkg>[a-z]+)_"
                    "(?P<name>"
                    "([A-Z][a-z]*)+)",                             0        },
            { L_,   "(?P<pkg>[a-z]+)_"
                    "(?P<name>"
                    "(?P<word1>[A-Z][a-z]*)"
                    "([A-Z][a-z]*)*)",                             1        },
            { L_,   "(?P<pkg>[a-z]+)_"
                    "(?P<name>"
                    "(?P<word1>[A-Z][a-z]*)"
                    "(?P<word2>[A-Z][a-z]*)"
                    "([A-Z][a-z]*)*)",                             2        },
            { L_,   "(?P<pkg>[a-z]+)_"
                    "(?P<name>"
                    "(?P<word1>[A-Z][a-z]*)"
                    "(?P<word2>[A-Z][a-z]*)"
                    "(?P<word3>[A-Z][a-z]*)"
                    "([A-Z][a-z]*)*)",                             3        },
            { L_,   "(?P<pkg>[a-z]+)_"
                    "(?P<name>"
                    "(?P<word1>[A-Z][a-z]*)"
                    "(?P<word2>[A-Z][a-z]*)"
                    "(?P<word3>[A-Z][a-z]*)"
                    "(?P<word4>[A-Z][a-z]*)"
                    "([A-Z][a-z]*)*)",                             4        },
        };
        enum { NUM_PATTERNS = sizeof PATTERNS / sizeof *PATTERNS };

        // subpattern names
        const char SPN_PKG[]      = "pkg";
        const char SPN_NAME[]     = "name";
        const char SPN_WORD[4][6] = { "word1", "word2", "word3", "word4" };

        // subject
        const char   SUBJECT[]   = "bbasm_SecurityDataChunkResolver";
        const size_t SUBJECT_LEN = sizeof(SUBJECT) - 1;

        // captured substrings
        const char CS_PKG[]       = "bbasm";
        const char CS_NAME[]      = "SecurityDataChunkResolver";
        const char CS_WORD[4][10] = { "Security",
                                      "Data",
                                      "Chunk",
                                      "Resolver" };

        if (verbose) {
            cout << "\nTesting with increasing number of subpatterns." << endl;
        }

        for (int i = 0; i < NUM_PATTERNS; ++i) {
            const int   LINE      = PATTERNS[i].d_lineNum;
            const char *PATTERN   = PATTERNS[i].d_pattern;
            const int   NUM_WORDS = PATTERNS[i].d_numWords;

            if (veryVerbose) {
                cout << "\n\tPreparing expression from "; P(LINE);
                cout << "\t  with "; P(PATTERN)
                cout << "\t  and ";  P(NUM_WORDS)
            }

            Obj mX(&ta); const Obj& X = mX;

            bsl::string errorMsg;
            size_t      errorOffset;

            int retCode = mX.prepare(&errorMsg, &errorOffset, PATTERN, 0);
            ASSERTV(LINE, errorMsg, errorOffset, 0 == retCode);

            const int NUM_SUBPATTERNS  = NUM_WORDS + 3;
            const int SP_PKG_INDEX     = 1;
            const int SP_NAME_INDEX    = 2;
            const int SP_WORD_INDEX[4] = { 3, 4, 5, 6 };

            if (veryVeryVerbose) {
                T_ T_
                P_(X.numSubpatterns())
                P_(X.subpatternIndex(SPN_PKG))
                P(X.subpatternIndex(SPN_NAME))
            }

            ASSERTV(LINE, X.numSubpatterns(),
                                        NUM_SUBPATTERNS == X.numSubpatterns());
            ASSERTV(LINE, X.subpatternIndex(SPN_PKG),
                                  SP_PKG_INDEX  == X.subpatternIndex(SPN_PKG));
            ASSERTV(LINE, X.subpatternIndex(SPN_NAME),
                                 SP_NAME_INDEX == X.subpatternIndex(SPN_NAME));

            // Test names that do not identify sub-patterns.

            bsl::string BAD_SPN_NAME(SPN_NAME);  BAD_SPN_NAME += "xYz";
            bsl::string BAD_SPN_PKG(SPN_PKG);    BAD_SPN_PKG  += "AbC";

            ASSERTV(LINE, -1 == X.subpatternIndex(BAD_SPN_NAME.c_str()));
            ASSERTV(LINE, -1 == X.subpatternIndex(BAD_SPN_PKG.c_str()));

            for (int j = 0; j < NUM_WORDS; ++j) {
                const char *SPN_WORDJ      = SPN_WORD[j];
                const int   SP_WORD_INDEXJ = SP_WORD_INDEX[j];

                if (veryVeryVerbose) {
                    T_ T_ P_(j) P(X.subpatternIndex(SPN_WORDJ))
                }

                ASSERTV(LINE, j, X.subpatternIndex(SPN_WORDJ),
                               SP_WORD_INDEXJ == X.subpatternIndex(SPN_WORDJ));

                bsl::string BAD_SPN_WORDJ(SPN_WORDJ);  BAD_SPN_WORDJ += "#%$";

                ASSERTV(LINE, -1 == X.subpatternIndex(BAD_SPN_WORDJ.c_str()));
            }

            if (veryVerbose) {
                cout << "\n\tMatching with "; P(SUBJECT);
            }

            if (veryVeryVerbose) {
                cout << "\n\tMatching with offsets based 'match'/'matchRaw'"
                     << endl;
            }
            {
                // 'match' is tested on the first iteration and 'matchRaw' on
                // the second one.

                for (int i = 0; i < 2; ++i) {
                    vector<pair<size_t, size_t> > vMatch;

                    if (NUM_WORDS%2) {
                        // Grow the vector to make sure it shrinks back to
                        // exactly 'NUM_SUBPATTERNS'+1.  Only do this for 1/2
                        // the cases.  For the other half, we check that the
                        // vector grows to exactly 'NUM_SUBPATTERNS'+1.

                        for (int j = 0; j < NUM_SUBPATTERNS + 10; ++j) {
                            vMatch.push_back(make_pair(0, 0));
                        }
                    }

                    if (i == 0) {
                        retCode = X.match(&vMatch, SUBJECT, SUBJECT_LEN);
                    } else {
                        retCode = X.matchRaw(&vMatch, SUBJECT, SUBJECT_LEN);
                    }

                    ASSERTV(LINE, 0 == retCode);
                    ASSERTV(LINE, vMatch.size(),
                                      NUM_SUBPATTERNS+1 == (int)vMatch.size());

                    // captured substrings
                    const string csPkg(&SUBJECT[vMatch[SP_PKG_INDEX].first],
                                                vMatch[SP_PKG_INDEX].second);
                    const string csName(&SUBJECT[vMatch[SP_NAME_INDEX].first],
                                                 vMatch[SP_NAME_INDEX].second);

                    if (veryVeryVerbose) {
                        T_ T_ P_(csPkg) P(csName)
                    }

                    ASSERTV(LINE, csPkg,  CS_PKG  == csPkg);
                    ASSERTV(LINE, csName, CS_NAME == csName);

                    for (int j = 0; j < NUM_WORDS; ++j) {
                        const char *CS_WORDJ       = CS_WORD[j];
                        const int   SP_WORD_INDEXJ = SP_WORD_INDEX[j];

                        const string csWordj(
                                        &SUBJECT[vMatch[SP_WORD_INDEXJ].first],
                                        vMatch[SP_WORD_INDEXJ].second);

                        if (veryVeryVerbose) {
                            T_ T_ P_(j) P(csWordj)
                        }

                        ASSERTV(LINE, csWordj,  CS_WORDJ == csWordj);
                    }

                    if (4 == NUM_WORDS) {
                        // If NUM_WORDS is 4, that means the last subpattern
                        // was not matched (only 4 words in the subject).  So
                        // make sure that the last element in the vector
                        // contains (as per doc):
                        // pair<size_t,size_t>(k_INVALID_OFFSET, 0).

                        const pair<size_t, size_t> NOT_FOUND(
                                                         Obj::k_INVALID_OFFSET,
                                                         0);
                        const pair<size_t, size_t> lastElement =
                                                       vMatch[vMatch.size()-1];

                        if (veryVeryVerbose) {
                            T_ T_ P_(lastElement.first) P(lastElement.second)
                        }

                        ASSERTV(LINE, lastElement.first, lastElement.second,
                                NOT_FOUND == lastElement);

                        ASSERTV(LINE, lastElement.first,
                                Obj::k_INVALID_OFFSET == lastElement.first);
                    }
                    else {
                        // Check that the last (unnamed) substring at the end
                        // was captured correctly.  It should be the last word
                        // in the subject (CS_WORD[3]).

                        const pair<size_t, size_t> lastElement =
                                                       vMatch[vMatch.size()-1];

                        const string  csLastWord(&SUBJECT[lastElement.first],
                                                 lastElement.second);

                        if (veryVeryVerbose) {
                            T_ T_ P(csLastWord)
                        }

                        ASSERTV(LINE, csLastWord, CS_WORD[3] == csLastWord);
                    }
                }
            }

            if (veryVeryVerbose) {
                cout
                    << "\n\tMatching with 'StringRef' based 'match'/'matchRaw'"
                    << endl;
            }
            {
                // 'match' is tested on the first iteration and 'matchRaw' on
                // the second one.

                for (int i = 0; i < 2; ++i) {
                    vector<bslstl::StringRef> vMatch;

                    if (NUM_WORDS%2) {
                        // Grow the vector to make sure it shrinks back to
                        // exactly 'NUM_SUBPATTERNS'+1.  Only do this for 1/2
                        // the cases.  For the other half, we check that the
                        // vector grows to exactly 'NUM_SUBPATTERNS'+1.

                        for (int j = 0; j < NUM_SUBPATTERNS + 10; ++j) {
                            vMatch.push_back((bslstl::StringRef()));
                        }
                    }

                    if (i == 0) {
                        retCode = X.match(&vMatch, SUBJECT, SUBJECT_LEN);
                    } else {
                        retCode = X.matchRaw(&vMatch, SUBJECT, SUBJECT_LEN);
                    }

                    ASSERTV(LINE, 0 == retCode);
                    ASSERTV(LINE, vMatch.size(),
                                      NUM_SUBPATTERNS+1 == (int)vMatch.size());

                    // captured substrings
                    const bslstl::StringRef& csPkg  = vMatch[SP_PKG_INDEX];
                    const bslstl::StringRef& csName = vMatch[SP_NAME_INDEX];

                    if (veryVeryVerbose) {
                        T_ T_ P_(csPkg) P(csName)
                    }

                    ASSERTV(LINE, csPkg,  CS_PKG  == csPkg);
                    ASSERTV(LINE, csName, CS_NAME == csName);

                    for (int j = 0; j < NUM_WORDS; ++j) {
                        const char *CS_WORDJ       = CS_WORD[j];
                        const int   SP_WORD_INDEXJ = SP_WORD_INDEX[j];

                        const bslstl::StringRef csWordj =
                                                        vMatch[SP_WORD_INDEXJ];

                        if (veryVeryVerbose) {
                            T_ T_ P_(j) P(csWordj)
                        }

                        ASSERTV(LINE, csWordj,  CS_WORDJ == csWordj);
                    }

                    if (4 == NUM_WORDS) {
                        // If NUM_WORDS is 4, that means the last subpattern
                        // was not matched (only 4 words in the subject).  So
                        // make sure that the last element in the vector
                        // contains (as per doc): empty 'StringRef'.

                        const bslstl::StringRef  NOT_FOUND;
                        const bslstl::StringRef& lastElement =
                                                       vMatch[vMatch.size()-1];

                        if (veryVeryVerbose) {
                            T_ T_ P(lastElement)
                        }

                        ASSERTV(LINE, lastElement, NOT_FOUND == lastElement);
                    }
                    else {
                        // Check that the last (unnamed) substring at the end
                        // was captured correctly.  It should be the last word
                        // in the subject (CS_WORD[3]).

                        const bslstl::StringRef csLastWord =
                                                       vMatch[vMatch.size()-1];

                        if (veryVeryVerbose) {
                            T_ T_ P(csLastWord)
                        }

                        ASSERTV(LINE, csLastWord, CS_WORD[3] == csLastWord);
                    }
                }
            }
        }

        if (verbose) cout << "\nEnd of Subpatterns Test." << endl;
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING k_FLAG_DOTMATCHESALL FLAG
        //   This will test the 'k_FLAG_DOTMATCHESALL' option when compiling
        //   regular expressions.
        //
        // Concerns:
        //: 1 We want to make sure that the dot metacharacter '.' matches all
        //:   characters including newlines ('\n') when this flag is specified.
        //:   We also want to check that not specifying this flag disables the
        //:   matching of newlines.
        //
        // Plan:
        //: 1 For a given pattern string containing newlines, create two
        //:   regular expression objects - one with 'k_FLAG_DOTMATCHESALL'
        //:   specified and another without.
        //:
        //: 2 For each object, exercise the match function using subjects of
        //:   the form "<preamble>\n<pattern-match>\n<postamble>".  Note that
        //:   in some cases the pattern to be matched will contain newlines.
        //:   Select test data with increasing preamble/postamble length (from
        //:   0 to 3).  Verify that the object with 'k_FLAG_DOTMATCHESALL'
        //:   always succeeds and also that the object without
        //:   'k_FLAG_DOTMATCHESALL' always fails.  (C-1)
        //:
        //: 3 Finally, exercise the match function using a subject that matches
        //:   the pattern exactly on a single line (i.e.  without any
        //:   preamble/postamble/newline characters).  Verify that both objects
        //:   succeed.  (C-1)
        //
        // Testing:
        //   k_FLAG_DOTMATCHESALL
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING k_FLAG_DOTMATCHESALL FLAG" << endl
                          << "=================================" << endl;

        const char PATTERN[]             = "b.*e";
        const char SUBJECT_SINGLE_LINE[] = "bbasm_SecurityCache";

        static const struct {
            int         d_lineNum;          // source line number
            const char *d_subject;          // subject string
            size_t      d_preambleLength;   // number of chars in preamble
            size_t      d_postambleLength;  // number of chars in postamble
        } DATA[] = {
            //line  subject                            preamble   postamble
            //----  -------                            --------   ---------
            { L_,   "bbasm\n_Se\ncurityCache\n",           0,         1     },
            { L_,   "\nbbasm\n_Se\ncurityCache\n",         1,         1     },
            { L_,   "a\nbbasm\n_Se\ncurityCache\na",       2,         2     },
            { L_,   "a\nbbasm\n_Se\ncurityCache\na",       2,         2     },
            { L_,   "az\nbbasm\n_Se\ncurityCache\n",       3,         1     },
            { L_,   "\nbbasm\n_Se\ncurityCache\nab",       1,         3     },
            { L_,   "az\nbbasm\n_Se\ncurityCache\nab",     3,         3     },
            { L_,   "azc\nbbasm\n_Se\ncurityCache\n",      4,         1     },
            { L_,   "\nbbasm\n_Se\ncurityCache\nabc",      1,         4     },
            { L_,   "azc\nbbasm\n_Se\ncurityCache\nabc",   4,         4     },
        };
        const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        Obj mX(&ta); const Obj& X = mX;   // has   'k_FLAG_DOTMATCHESALL'
        Obj mY(&ta); const Obj& Y = mY;   // !have 'k_FLAG_DOTMATCHESALL'

        if (verbose) {
            cout << "\nPreparing the regular expression objects." << endl;

            if (veryVerbose) {
                T_ P(PATTERN);
            }
        }

        bsl::string errorMsg;
        size_t      errorOffset;

        int retCode = mX.prepare(&errorMsg,
                                 &errorOffset,
                                 PATTERN,
                                 Obj::k_FLAG_DOTMATCHESALL);
        ASSERTV(errorMsg, errorOffset, 0 == retCode);
        ASSERTV(X.flags(), Obj::k_FLAG_DOTMATCHESALL == X.flags());

        retCode = mY.prepare(&errorMsg, &errorOffset, PATTERN, 0);
        ASSERTV(errorMsg, errorOffset, 0 == retCode);
        ASSERTV(Y.flags(), 0 == Y.flags());

        if (verbose) cout << "\nTrying multiline subjects." << endl;

        for (size_t i = 0; i < NUM_DATA; ++i) {
            const int    LINE          = DATA[i].d_lineNum;
            const char  *SUBJECT       = DATA[i].d_subject;
            const size_t PREAMBLE_LEN  = DATA[i].d_preambleLength;
            const size_t POSTAMBLE_LEN = DATA[i].d_postambleLength;
            const size_t SUBJECT_LEN   = bsl::strlen(SUBJECT);
            const size_t MATCH_OFFSET  = PREAMBLE_LEN;
            const size_t MATCH_LENGTH  = SUBJECT_LEN
                                         - PREAMBLE_LEN
                                         - POSTAMBLE_LEN;

            if (veryVerbose) {
                T_ P_(LINE) P_(SUBJECT) P_(SUBJECT_LEN)
                   P_(PREAMBLE_LEN) P(POSTAMBLE_LEN)
            }

            if (veryVeryVerbose) {
                cout << "\t\tUsing regular match function." << endl;
            }

            pair<size_t, size_t> match;

            retCode = X.match(&match, SUBJECT, SUBJECT_LEN);

            ASSERTV(LINE, retCode,      0            == retCode);
            ASSERTV(LINE, match.first,  MATCH_OFFSET == match.first);
            ASSERTV(LINE, match.second, MATCH_LENGTH == match.second);

            retCode = Y.match(&match, SUBJECT, SUBJECT_LEN);

            ASSERTV(LINE, retCode, 0 != retCode);

            if (veryVeryVerbose) {
                cout << "\t\tUsing vector match function." << endl;
            }

            vector<pair<size_t, size_t> > vMatch;

            retCode = X.match(&vMatch, SUBJECT, SUBJECT_LEN);

            ASSERTV(LINE, retCode, 0 == retCode);
            ASSERTV(LINE, retCode, 1 == vMatch.size());
            ASSERTV(LINE, vMatch[0].first,  MATCH_OFFSET == vMatch[0].first);
            ASSERTV(LINE, vMatch[0].second, MATCH_LENGTH == vMatch[0].second);

            retCode = Y.match(&vMatch, SUBJECT, SUBJECT_LEN);
            ASSERTV(LINE, retCode, 0 != retCode);
        }

        if (verbose)  cout << "\nTrying single line subject." << endl;
        {
            const char   *SUBJECT     = SUBJECT_SINGLE_LINE;
            const size_t  SUBJECT_LEN = bsl::strlen(SUBJECT);

            if (veryVerbose) {
                T_ P_(SUBJECT) P(SUBJECT_LEN)
            }

            if (veryVeryVerbose) {
                cout << "\t\tUsing regular match function." << endl;
            }

            pair<size_t, size_t> match;

            retCode = X.match(&match, SUBJECT, SUBJECT_LEN);

            ASSERTV(retCode,      0           == retCode);
            ASSERTV(match.first,  0           == match.first);
            ASSERTV(match.second, SUBJECT_LEN == match.second);

            retCode = Y.match(&match, SUBJECT, SUBJECT_LEN);

            ASSERTV(retCode,      0           == retCode);
            ASSERTV(match.first,  0           == match.first);
            ASSERTV(match.second, SUBJECT_LEN == match.second);

            if (veryVeryVerbose) {
                cout << "\t\tUsing vector match function." << endl;
            }

            vector<pair<size_t, size_t> > vMatch;

            retCode = X.match(&vMatch, SUBJECT, SUBJECT_LEN);

            ASSERTV(retCode,          0           == retCode);
            ASSERTV(vMatch.size(),    1           == vMatch.size());
            ASSERTV(vMatch[0].first,  0           == vMatch[0].first);
            ASSERTV(vMatch[0].second, SUBJECT_LEN == vMatch[0].second);

            retCode = Y.match(&vMatch, SUBJECT, SUBJECT_LEN);

            ASSERTV(retCode,          0           == retCode);
            ASSERTV(vMatch.size(),    1           == vMatch.size());
            ASSERTV(vMatch[0].first,  0           == vMatch[0].first);
            ASSERTV(vMatch[0].second, SUBJECT_LEN == vMatch[0].second);
        }

        if (verbose) cout << "\nEnd of Multiline Flag Test." << endl;
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING k_FLAG_UTF8 FLAG
        //   This will test the 'k_FLAG_UTF8' option when compiling regular
        //   expressions.
        //
        // Concerns:
        //: 1 We want to make sure that UTF8 byte sequences are treated as
        //:   single UTF8 characters when this flag is specified.  We also want
        //:   to make sure that the same UTF8 byte sequences are treated as
        //:   regular characters when this flag is not specified.
        //
        // Plan:
        //: 1 Create a set of UTF8 patterns.  For each pattern, create two
        //:   regular expression objects - one with 'k_FLAG_UTF8' specified and
        //:   another without 'k_FLAG_UTF8' specified.
        //:
        //: 2 For each object, exercise the match function using a UTF8 subject
        //:   that matches the pattern being tested.  Verify that the object
        //:   with 'k_FLAG_UTF8' succeeds and the object without 'k_FLAG_UTF8'
        //:   fails.  Next, exercise the match function using a non-UTF8
        //:   subject that matches the pattern being tested.  Verify that the
        //:   object with 'k_FLAG_UTF8' fails and the object without
        //:   'k_FLAG_UTF8' succeeds.  (C-1)
        //
        // Testing:
        //   k_FLAG_UTF8
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING k_FLAG_UTF8 FLAG" << endl
                          << "========================" << endl;

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        static const struct {
            int            d_lineNum;         // source line number
            const char    *d_pattern;         // pattern string
            unsigned char  d_utf8Subject[2];  // utf8 subject
            unsigned char  d_regularSubject;  // regular subject
        } DATA[] = {
            //line   pattern       utf8Subject       regularSubject
            //----   -------       -----------       --------------
            { L_,    "\\xC0",      { 0xC3, 0x80 },   0xC0           },
            { L_,    "\\xC5",      { 0xC3, 0x85 },   0xC5           },
            { L_,    "\\xD3",      { 0xC3, 0x93 },   0xD3           },
            { L_,    "\\xFF",      { 0xC3, 0xBF },   0xFF           },
        };
        const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nRunning test data." << endl;

        for (size_t i = 0; i < NUM_DATA; ++i) {
            const int   LINE    = DATA[i].d_lineNum;
            const char *PATTERN = DATA[i].d_pattern;

            if (veryVerbose) {
                cout << "\tPreparing the regular expression objects.  ";
                P_(LINE) P(PATTERN)
            }

            Obj mX(&ta); const Obj& X = mX;  // has   'k_FLAG_UTF8'
            Obj mY(&ta); const Obj& Y = mY;  // !have 'k_FLAG_UTF8'

            bsl::string errorMsg;
            size_t      errorOffset;

            int retCode = mX.prepare(&errorMsg,
                                     &errorOffset,
                                     PATTERN,
                                     Obj::k_FLAG_UTF8);

            ASSERTV(errorMsg, errorOffset, 0 == retCode);
            ASSERTV(X.flags(), Obj::k_FLAG_UTF8 == X.flags());

            retCode = mY.prepare(&errorMsg, &errorOffset, PATTERN, 0);

            ASSERTV(errorMsg, errorOffset, 0 == retCode);
            ASSERTV(Y.flags(), 0 == Y.flags());

            {
                if (veryVeryVerbose) {
                    cout << "\t\tTesting with UTF8 subject." << endl;
                }

                const unsigned char *UTF8_SUBJECT = DATA[i].d_utf8Subject;

                retCode = X.match((const char*)UTF8_SUBJECT, 2);
                ASSERTV(LINE, retCode, 0 == retCode);

                retCode = Y.match((const char*)UTF8_SUBJECT, 2);
                ASSERTV(LINE, retCode, 0 != retCode);

                retCode = X.match(string_view((const char*)UTF8_SUBJECT, 2));
                ASSERTV(LINE, retCode, 0 == retCode);

                retCode = Y.match(string_view((const char*)UTF8_SUBJECT, 2));
                ASSERTV(LINE, retCode, 0 != retCode);

                bsl::pair<size_t, size_t> p;

                retCode = X.match(&p, (const char*)UTF8_SUBJECT, 2);
                ASSERTV(LINE, retCode, 0 == retCode);

                retCode = Y.match(&p, (const char*)UTF8_SUBJECT, 2);
                ASSERTV(LINE, retCode, 0 != retCode);

                string_view sv;

                retCode = X.match(&sv, (const char*)UTF8_SUBJECT, 2);
                ASSERTV(LINE, retCode, 0 == retCode);

                retCode = Y.match(&sv, (const char*)UTF8_SUBJECT, 2);
                ASSERTV(LINE, retCode, 0 != retCode);

                retCode = X.match(&sv,
                                  string_view((const char*)UTF8_SUBJECT, 2));
                ASSERTV(LINE, retCode, 0 == retCode);

                retCode = Y.match(&sv,
                                  string_view((const char*)UTF8_SUBJECT, 2));
                ASSERTV(LINE, retCode, 0 != retCode);

                bsl::vector<bsl::pair<size_t, size_t> > vp;

                retCode = X.match(&vp, (const char*)UTF8_SUBJECT, 2);
                ASSERTV(LINE, retCode, 0 == retCode);

                retCode = Y.match(&vp, (const char*)UTF8_SUBJECT, 2);
                ASSERTV(LINE, retCode, 0 != retCode);

                bsl::vector<bslstl::StringRef> vsr;

                retCode = X.match(&vsr, (const char*)UTF8_SUBJECT, 2);
                ASSERTV(LINE, retCode, 0 == retCode);

                retCode = Y.match(&vsr, (const char*)UTF8_SUBJECT, 2);
                ASSERTV(LINE, retCode, 0 != retCode);

                bsl::vector<bsl::string_view> vsv1;

                retCode = X.match(&vsv1,
                                  string_view( (const char*)UTF8_SUBJECT, 2));
                ASSERTV(LINE, retCode, 0 == retCode);

                retCode = Y.match(&vsv1,
                                  string_view( (const char*)UTF8_SUBJECT, 2));
                ASSERTV(LINE, retCode, 0 != retCode);

                std::vector<bsl::string_view> vsv2;

                retCode = X.match(&vsv2,
                                  string_view( (const char*)UTF8_SUBJECT, 2));
                ASSERTV(LINE, retCode, 0 == retCode);

                retCode = Y.match(&vsv2,
                                  string_view( (const char*)UTF8_SUBJECT, 2));
                ASSERTV(LINE, retCode, 0 != retCode);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                std::pmr::vector<bsl::string_view> vsv3;

                retCode = X.match(&vsv3,
                                  string_view( (const char*)UTF8_SUBJECT, 2));
                ASSERTV(LINE, retCode, 0 == retCode);

                retCode = Y.match(&vsv3,
                                  string_view( (const char*)UTF8_SUBJECT, 2));
                ASSERTV(LINE, retCode, 0 != retCode);
#endif
          }

            {
                if (veryVeryVerbose) {
                    cout << "\t\tTesting with non-UTF8 subject." << endl;
                }

                const unsigned char REGULAR_SUBJECT = DATA[i].d_regularSubject;

                retCode = X.match((const char*)&REGULAR_SUBJECT, 1);
                ASSERTV(LINE, retCode, 0 != retCode);

                retCode = Y.match((const char*)&REGULAR_SUBJECT, 1);
                ASSERTV(LINE, retCode, 0 == retCode);

                retCode = X.match(
                                string_view((const char*)&REGULAR_SUBJECT, 1));
                ASSERTV(LINE, retCode, 0 != retCode);
                retCode = Y.match(
                                string_view((const char*)&REGULAR_SUBJECT, 1));
                ASSERTV(LINE, retCode, 0 == retCode);

                bsl::pair<size_t, size_t> p;

                retCode = X.match(&p, (const char*)&REGULAR_SUBJECT, 1);
                ASSERTV(LINE, retCode, 0 != retCode);

                retCode = Y.match(&p, (const char*)&REGULAR_SUBJECT, 1);
                ASSERTV(LINE, retCode, 0 == retCode);
                string_view sv;

                retCode = X.match(&sv, (const char*)&REGULAR_SUBJECT, 1);
                ASSERTV(LINE, retCode, 0 != retCode);

                retCode = Y.match(&sv, (const char*)&REGULAR_SUBJECT, 1);
                ASSERTV(LINE, retCode, 0 == retCode);

                retCode = X.match(
                                &sv,
                                string_view((const char*)&REGULAR_SUBJECT, 1));
                ASSERTV(LINE, retCode, 0 != retCode);

                retCode = Y.match(
                                &sv,
                                string_view((const char*)&REGULAR_SUBJECT, 1));
                ASSERTV(LINE, retCode, 0 == retCode);

                bsl::vector<bsl::pair<size_t, size_t> > vp;

                retCode = X.match(&vp, (const char*)&REGULAR_SUBJECT, 1);
                ASSERTV(LINE, retCode, 0 != retCode);

                retCode = Y.match(&vp, (const char*)&REGULAR_SUBJECT, 1);
                ASSERTV(LINE, retCode, 0 == retCode);

                bsl::vector<bslstl::StringRef> vsr;

                retCode = X.match(&vsr, (const char*)&REGULAR_SUBJECT, 1);
                ASSERTV(LINE, retCode, 0 != retCode);

                retCode = Y.match(&vsr, (const char*)&REGULAR_SUBJECT, 1);
                ASSERTV(LINE, retCode, 0 == retCode);

                bsl::vector<bsl::string_view> vsv1;

                retCode = X.match(
                               &vsv1,
                               string_view( (const char*)&REGULAR_SUBJECT, 1));
                ASSERTV(LINE, retCode, 0 != retCode);

                retCode = Y.match(
                               &vsv1,
                               string_view( (const char*)&REGULAR_SUBJECT, 1));
                ASSERTV(LINE, retCode, 0 == retCode);

                std::vector<bsl::string_view> vsv2;

                retCode = X.match(
                               &vsv2,
                               string_view( (const char*)&REGULAR_SUBJECT, 1));
                ASSERTV(LINE, retCode, 0 != retCode);

                retCode = Y.match(
                               &vsv2,
                               string_view( (const char*)&REGULAR_SUBJECT, 1));
                ASSERTV(LINE, retCode, 0 == retCode);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                std::pmr::vector<bsl::string_view> vsv3;

                retCode = X.match(
                               &vsv3,
                               string_view( (const char*)&REGULAR_SUBJECT, 1));
                ASSERTV(LINE, retCode, 0 != retCode);

                retCode = Y.match(
                               &vsv3,
                               string_view( (const char*)&REGULAR_SUBJECT, 1));
                ASSERTV(LINE, retCode, 0 == retCode);
#endif
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING k_FLAG_MULTILINE FLAG
        //   This will test the 'k_FLAG_MULTILINE' option when compiling
        //   regular expressions.
        //
        // Concerns:
        //: 1 We want to make sure that '^' and '$' match 'beginning-of-line'
        //:   and 'end-of-line' respectively when this flag is specified.  We
        //:   also want to make sure that '^' and '$' match
        //:   'beginning-of-string' and 'end-of-string' respectively when this
        //:   flag is not specified.
        //
        // Plan:
        //: 1 For a given pattern string starting with '^' and ending with '$',
        //:   create two regular expression objects - one with
        //:   'k_FLAG_MULTILINE' specified and another without
        //:   'k_FLAG_MULTILINE' specified.
        //:
        //: 2 For each object, exercise the match function using subjects of
        //:   the form "<preamble>\n<pattern-match>\n<postamble>".  Select test
        //:   data with increasing preamble/postamble length (from 0 to 3).
        //:   Verify that the object with 'k_FLAG_MULTILINE' always succeeds
        //:   and that the object without 'k_FLAG_MULTILINE' always fails.
        //:   (C-1)
        //:
        //: 3 Finally, exercise the match function using a subject that matches
        //:   the pattern exactly on a single line (i.e.  without any
        //:   preamble/postamble/newline characters).  Verify that both objects
        //:   succeed.  (C-1)
        //
        // Testing:
        //   k_FLAG_MULTILINE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING k_FLAG_MULTILINE FLAG" << endl
                          << "=============================" << endl;

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        const char PATTERN[]             = "^bbasm_SecurityCache$";
        const char SUBJECT_SINGLE_LINE[] = "bbasm_SecurityCache";

        static const struct {
            int         d_lineNum;          // source line number
            const char *d_subject;          // subject string
            size_t      d_preambleLength;   // number of chars in preamble
            size_t      d_postambleLength;  // number of chars in postamble
        } DATA[] = {
            //line  subject                            preamble   postamble
            //----  -------                            --------   ---------
            { L_,   "\nbbasm_SecurityCache\n",         0,         0         },
            { L_,   "a\nbbasm_SecurityCache\n",        1,         0         },
            { L_,   "\nbbasm_SecurityCache\na",        0,         1         },
            { L_,   "a\nbbasm_SecurityCache\na",       1,         1         },
            { L_,   "ab\nbbasm_SecurityCache\n",       2,         0         },
            { L_,   "\nbbasm_SecurityCache\nab",       0,         2         },
            { L_,   "ab\nbbasm_SecurityCache\nab",     2,         2         },
            { L_,   "abc\nbbasm_SecurityCache\n",      3,         0         },
            { L_,   "\nbbasm_SecurityCache\nabc",      0,         3         },
            { L_,   "abc\nbbasm_SecurityCache\nabc",   3,         3         },
        };
        const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        Obj mX(&ta); const Obj& X = mX;  // has 'k_FLAG_MULTILINE'
        Obj mY(&ta); const Obj& Y = mY;  // !have 'k_FLAG_MULTILINE'

        if (verbose) {
            cout << "\nPreparing the regular expression objects." << endl;

            if (veryVerbose) {
                T_ P(PATTERN);
            }
        }

        bsl::string errorMsg;
        size_t      errorOffset;

        int retCode = mX.prepare(&errorMsg,
                                 &errorOffset,
                                 PATTERN,
                                 Obj::k_FLAG_MULTILINE);
        ASSERTV(errorMsg, errorOffset, 0 == retCode);
        ASSERTV(X.flags(), Obj::k_FLAG_MULTILINE == X.flags());

        retCode = mY.prepare(&errorMsg, &errorOffset, PATTERN, 0);
        ASSERTV(errorMsg, errorOffset, 0 == retCode);
        ASSERTV(Y.flags(), 0 == Y.flags());

        if (verbose) cout << "\nTrying multi-line subjects." << endl;

        for (size_t i = 0; i < NUM_DATA; ++i) {
            const int     LINE          = DATA[i].d_lineNum;
            const char   *SUBJECT       = DATA[i].d_subject;
            const size_t  PREAMBLE_LEN  = DATA[i].d_preambleLength;
            const size_t  POSTAMBLE_LEN = DATA[i].d_postambleLength;
            const size_t  SUBJECT_LEN   = strlen(SUBJECT);
            const size_t  MATCH_OFFSET  = PREAMBLE_LEN + 1;
            const size_t  MATCH_LENGTH  = SUBJECT_LEN
                                          - PREAMBLE_LEN
                                          - POSTAMBLE_LEN
                                          - 2;

            if (veryVerbose) {
                T_ P_(LINE) P_(SUBJECT) P_(SUBJECT_LEN)
                   P_(PREAMBLE_LEN) P(POSTAMBLE_LEN)
            }

            if (veryVeryVerbose) {
                cout << "\t\tUsing regular match function." << endl;
            }

            pair<size_t, size_t> match;

            retCode = X.match(&match, SUBJECT, SUBJECT_LEN);

            ASSERTV(LINE, retCode,      0            == retCode);
            ASSERTV(LINE, match.first,  MATCH_OFFSET == match.first);
            ASSERTV(LINE, match.second, MATCH_LENGTH == match.second);

            retCode = Y.match(&match, SUBJECT, SUBJECT_LEN);

            ASSERTV(LINE, retCode, 0 != retCode);

            if (veryVeryVerbose) {
                cout << "\t\tUsing vector match function." << endl;
            }

            vector<pair<size_t, size_t> > vMatch;

            retCode = X.match(&vMatch, SUBJECT, SUBJECT_LEN);

            ASSERTV(LINE, retCode, 0 == retCode);
            ASSERTV(LINE, vMatch[0].first,
                         MATCH_OFFSET == vMatch[0].first);
            ASSERTV(LINE, vMatch[0].second,
                         MATCH_LENGTH == vMatch[0].second);

            retCode = Y.match(&vMatch, SUBJECT, SUBJECT_LEN);

            ASSERTV(LINE, retCode, 0 != retCode);
        }

        if (verbose) cout << "\nTrying single line subject." << endl;
        {
            const char   *SUBJECT     = SUBJECT_SINGLE_LINE;
            const size_t  SUBJECT_LEN = strlen(SUBJECT);

            if (veryVerbose) {
                T_ P_(SUBJECT) P(SUBJECT_LEN)
            }

            if (veryVeryVerbose) {
                cout << "\t\tUsing regular match function." << endl;
            }

            pair<size_t, size_t> match;

            retCode = X.match(&match, SUBJECT, SUBJECT_LEN);

            ASSERTV(retCode,      0           == retCode);
            ASSERTV(match.first,  0           == match.first);
            ASSERTV(match.second, SUBJECT_LEN == match.second);

            retCode = Y.match(&match, SUBJECT, SUBJECT_LEN);

            ASSERTV(retCode,      0           == retCode);
            ASSERTV(match.first,  0           == match.first);
            ASSERTV(match.second, SUBJECT_LEN == match.second);

            if (veryVeryVerbose) {
                cout << "\t\tUsing vector match function." << endl;
            }

            vector<pair<size_t, size_t> > vMatch;

            retCode = X.match(&vMatch, SUBJECT, SUBJECT_LEN);

            ASSERTV(retCode,          0           == retCode);
            ASSERTV(vMatch[0].first,  0           == vMatch[0].first);
            ASSERTV(vMatch[0].second, SUBJECT_LEN == vMatch[0].second);

            retCode = Y.match(&vMatch, SUBJECT, SUBJECT_LEN);

            ASSERTV(retCode,          0           == retCode);
            ASSERTV(vMatch[0].first,  0           == vMatch[0].first);
            ASSERTV(vMatch[0].second, SUBJECT_LEN == vMatch[0].second);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING k_FLAG_CASELESS FLAG
        //   This will test the 'k_FLAG_CASELESS' option when compiling regular
        //   expressions.
        //
        // Concerns:
        //: 1 We want to make sure that caseless matching is performed when
        //:   this flag is specified, and also that caseless matching is *not*
        //:   performed when this flag is not specified.
        //
        // Plan:
        //: 1 For a given pattern string containing both upper-case and
        //:   lower-case letters, create two regular expression objects - one
        //:   with 'k_FLAG_CASELESS' specified and another without
        //:   'k_FLAG_CASELESS' specified.
        //:
        //: 2 For each object, exercise the match function using subjects that
        //:   use different cases from the pattern.  Verify that the object
        //:   with 'k_FLAG_CASELESS' always succeeds and also that the object
        //:   without 'k_FLAG_CASELESS' always fails.  (C-1)
        //:
        //: 3 Finally, exercise the match function using a subject that uses
        //:   the same case as the pattern.  Verify that both objects succeed.
        //:   (C-1)
        //
        // Testing:
        //   k_FLAG_CASELESS
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING k_FLAG_CASELESS FLAG" << endl
                          << "============================" << endl;

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        const char PATTERN[]           = "bbasm_SecurityCache";
        const char SUBJECT_SAME_CASE[] = "bbasm_SecurityCache";

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_subject;  // subject string
        } DATA[] = {
            //line  subject
            //----  ---------------------
            { L_,   "BBASM_SECURITYCACHE"  },
            { L_,   "bbasm_securitycache"  },
            { L_,   "bBaSm_sEcUrItYcAcHe"  },
            { L_,   "BbAsM_SeCuRiTyCaChE"  },
        };
        const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        Obj mX(&ta); const Obj& X = mX;  // has   'k_FLAG_CASELESS'
        Obj mY(&ta); const Obj& Y = mY;  // !have 'k_FLAG_CASELESS'

        if (verbose) {
            cout << "\nPreparing the regular expression objects." << endl;

            if (veryVerbose) {
                T_ P(PATTERN);
            }
        }

        bsl::string errorMsg;
        size_t      errorOffset;

        int retCode = mX.prepare(&errorMsg,
                                 &errorOffset,
                                 PATTERN,
                                 Obj::k_FLAG_CASELESS);

        ASSERTV(errorMsg, errorOffset, 0 == retCode);
        ASSERTV(X.flags(), Obj::k_FLAG_CASELESS == X.flags());

        retCode = mY.prepare(&errorMsg, &errorOffset, PATTERN, 0);

        ASSERTV(errorMsg, errorOffset, 0 == retCode);
        ASSERTV(Y.flags(), 0 == Y.flags());

        if (verbose) cout << "\nTrying different cases." << endl;

        for (size_t i = 0; i < NUM_DATA; ++i) {
            const int     LINE    = DATA[i].d_lineNum;
            const char   *SUBJECT = DATA[i].d_subject;
            const size_t  LEN     = strlen(SUBJECT);

            if (veryVerbose) {
                T_ P_(LINE) P_(SUBJECT) P(LEN)
            }

            if (veryVeryVerbose) {
                cout << "\t\tUsing regular match function." << endl;
            }

            pair<size_t, size_t> match;

            retCode = X.match(&match, SUBJECT, LEN);

            ASSERTV(LINE, retCode,      0   == retCode);
            ASSERTV(LINE, match.first,  0   == match.first);
            ASSERTV(LINE, match.second, LEN == match.second);

            retCode = Y.match(&match, SUBJECT, LEN);

            ASSERTV(LINE, retCode, 0 != retCode);

            if (veryVeryVerbose) {
                cout << "\t\tUsing vector match function." << endl;
            }

            vector<pair<size_t, size_t> > vMatch;

            retCode = X.match(&vMatch, SUBJECT, LEN);

            ASSERTV(LINE, retCode,          0   == retCode);
            ASSERTV(LINE, vMatch[0].first,  0   == vMatch[0].first);
            ASSERTV(LINE, vMatch[0].second, LEN == vMatch[0].second);

            retCode = Y.match(&vMatch, SUBJECT, LEN);

            ASSERTV(LINE, retCode, 0 != retCode);
        }

        if (verbose) cout << "\nTrying same case as pattern." << endl;
        {
            const char   *SUBJECT = SUBJECT_SAME_CASE;
            const size_t  LEN     = strlen(SUBJECT);

            if (veryVerbose) {
                T_ P_(SUBJECT) P(LEN)
            }

            if (veryVeryVerbose) {
                cout << "\t\tUsing regular match function." << endl;
            }

            pair<size_t, size_t> match;

            retCode = X.match(&match, SUBJECT, LEN);

            ASSERTV(retCode,      0   == retCode);
            ASSERTV(match.first,  0   == match.first);
            ASSERTV(match.second, LEN == match.second);

            retCode = Y.match(&match, SUBJECT, LEN);

            ASSERTV(retCode,      0   == retCode);
            ASSERTV(match.first,  0   == match.first);
            ASSERTV(match.second, LEN == match.second);

            if (veryVeryVerbose) {
                cout << "\t\tUsing vector match function." << endl;
            }

            vector<pair<size_t, size_t> > vMatch;

            retCode = X.match(&vMatch, SUBJECT, LEN);

            ASSERTV(retCode,          0   == retCode);
            ASSERTV(vMatch[0].first,  0   == vMatch[0].first);
            ASSERTV(vMatch[0].second, LEN == vMatch[0].second);

            retCode = Y.match(&vMatch, SUBJECT, LEN);

            ASSERTV(retCode,          0   == retCode);
            ASSERTV(vMatch[0].first,  0   == vMatch[0].first);
            ASSERTV(vMatch[0].second, LEN == vMatch[0].second);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'flags' METHOD
        //   This will test that the options passed to the 'prepare' method are
        //   correctly propagated to the object data member.
        //
        // Concerns:
        //: 1 Options passed to the 'prepare' method are stored as an object
        //:   data member and returned by the 'flags' method.
        //
        // Plan:
        //: 1 Call 'prepare' with a different set of flags and verify that the
        //:   'flags' method return correct value.  (C-1)
        //
        // Testing:
        //   int flags() const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'flags' METHOD" << endl
                          << "======================" << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator ta("test",    veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard guard(&da);

        const char PATTERN[]         = "(abc)+";
        const char INVALID_PATTERN[] = "(abcdef";

        if (verbose) cout << "\nTesting a valid pattern." << endl;
        {
            Obj mX(&ta); const Obj& X = mX;

            ASSERT(false == X.isPrepared());
            ASSERTV(X.flags(), 0 == X.flags());

            int retCode = mX.prepare(0, 0, PATTERN, Obj::k_FLAG_MULTILINE, 0);

            ASSERTV(retCode,        0                     == retCode);
            ASSERTV(X.isPrepared(), true                  == X.isPrepared());
            ASSERTV(X.flags(),      Obj::k_FLAG_MULTILINE == X.flags());

            retCode = mX.prepare(0, 0, PATTERN, Obj::k_FLAG_CASELESS, 0);

            ASSERTV(retCode,        0                    == retCode);
            ASSERTV(X.isPrepared(), true                 == X.isPrepared());
            ASSERTV(X.flags(),      Obj::k_FLAG_CASELESS == X.flags());

            retCode = mX.prepare(0, 0, PATTERN, Obj::k_FLAG_UTF8, 0);

            ASSERTV(retCode,        0                == retCode);
            ASSERTV(X.isPrepared(), true             == X.isPrepared());
            ASSERTV(X.flags(),      Obj::k_FLAG_UTF8 == X.flags());

            retCode = mX.prepare(0, 0, PATTERN, Obj::k_FLAG_DOTMATCHESALL, 0);

            ASSERTV(retCode,        0    == retCode);
            ASSERTV(X.isPrepared(), true == X.isPrepared());
            ASSERTV(X.flags(),      Obj::k_FLAG_DOTMATCHESALL == X.flags());

            retCode = mX.prepare(0, 0, PATTERN, Obj::k_FLAG_JIT, 0);

            ASSERTV(retCode,        0               == retCode);
            ASSERTV(X.isPrepared(), true            == X.isPrepared());
            ASSERTV(X.flags(),      Obj::k_FLAG_JIT == X.flags());

            const int flags = Obj::k_FLAG_MULTILINE
                            | Obj::k_FLAG_CASELESS
                            | Obj::k_FLAG_UTF8
                            | Obj::k_FLAG_DOTMATCHESALL
                            | Obj::k_FLAG_JIT;

            retCode = mX.prepare(0, 0, PATTERN, flags, 0);

            ASSERTV(retCode,        0     == retCode);
            ASSERTV(X.isPrepared(), true  == X.isPrepared());
            ASSERTV(X.flags(),      flags == X.flags());

            mX.clear();

            ASSERT(false == X.isPrepared());
            ASSERTV(X.flags(), 0 == X.flags());

            retCode = mX.prepare(0, 0, INVALID_PATTERN, flags, 0);

            ASSERTV(retCode,        0     != retCode);
            ASSERTV(X.isPrepared(), false == X.isPrepared());
            ASSERTV(X.flags(),      flags == X.flags());
        }
        ASSERTV(da.numAllocations(), 0 == da.numAllocations());
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING EXTENDED 'match' AND 'matchRaw' METHODS
        //
        // Concerns:
        //: 1 Although we are not testing the implementation of the PCRE
        //:   library, we want to check that the interface is plugged in
        //:   correctly and works as documented.  In particular, we want to
        //:   check that the 'subject', 'subjectLength', and 'subjectStart'
        //:   arguments are passed to PCRE correctly and the resulting
        //:   'ovector' is copied correctly to the 'result'.  Also we want to
        //:   make sure that a failure code is returned when the subject does
        //:   not match the pattern.
        //
        // Plan:
        //: 1 Prepare a regular expression object using a given 'PATTERN'
        //:   string, create a set of subjects that contain a single match for
        //:   'PATTERN'.  The set should contain subjects of increasing length,
        //:   and also increasing match offsets ('matchOffset').
        //:
        //: 2 Exercise the 'match' and 'matchRaw' methods using 'subjectStart'
        //:   values in the range [0..'subjectLength'].  Check that the methods
        //:   succeed when 'subjectStart' <= 'matchOffset' and they fail when
        //:   'subjectStart' > 'matchOffset'.  For each successful call to
        //:   'match' or 'matchRaw', check that 'result' contains the correct
        //:   'StringRef' for the captured string.  Note that captured
        //:   substrings are tested in later test case.
        //:
        //: 3 Finally, exercise the special case where 'subjectLength' is 0.
        //
        // Testing:
        //   int match(bslstl::StringRef*, ...) const;
        //   int match(bsl::vector<bslstl::StringRef>*, ...) const;
        //   int matchRaw(bslstl::StringRef *result, ...) const;
        //   int matchRaw(bsl::vector<bslstl::StringRef> *result, ...) const;
        // --------------------------------------------------------------------
        if (verbose)
            cout << endl
                 << "TESTING EXTENDED 'match' AND 'matchRaw' METHODS" << endl
                 << "===============================================" << endl;

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        const char PATTERN[] = "(a(b(c)?)?)";  // matches 'a', 'ab', or 'abc'

        static const struct {
            int         d_lineNum;      // source line number
            const char *d_subject;      // subject string
            size_t      d_matchOffset;  // match offset
            const char *d_matchString;  // matched string
        } DATA[] = {
            //line   subject       matchOffset       matchString
            //----   -------       -----------       -----------

            // subject length = 1
            { L_,    "a",          0,                "a"         },

            // subject length = 2
            { L_,    "ab",         0,                "ab"        },
            { L_,    "aZ",         0,                "a"         },
            { L_,    "ba",         1,                "a"         },
            { L_,    "Za",         1,                "a"         },

            // // subject length = 3
            { L_,    "abb",        0,                "ab"        },
            { L_,    "abc",        0,                "abc"       },
            { L_,    "abZ",        0,                "ab"        },
            { L_,    "acZ",        0,                "a"         },
            { L_,    "aZZ",        0,                "a"         },
            { L_,    "ZaZ",        1,                "a"         },
            { L_,    "Zab",        1,                "ab"        },
            { L_,    "ZZa",        2,                "a"         },

            // // subject length = 4
            { L_,    "aZZZ",       0,                "a"         },
            { L_,    "abZZ",       0,                "ab"        },
            { L_,    "abcZ",       0,                "abc"       },
            { L_,    "ZaZZ",       1,                "a"         },
            { L_,    "ZabZ",       1,                "ab"        },
            { L_,    "Zabc",       1,                "abc"       },
            { L_,    "ZZaZ",       2,                "a"         },
            { L_,    "ZZab",       2,                "ab"        },
            { L_,    "ZZZa",       3,                "a"         },
        };
        const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        Obj mX(&ta); const Obj& X = mX;

        bsl::string errorMsg;
        size_t      errorOffset;

        int retCodeView;
        int retCode = mX.prepare(&errorMsg, &errorOffset, PATTERN, 0, 0);
        int retCodeRaw;
        int retCodeRawView;
        ASSERTV(errorMsg, errorOffset, 0 == retCode);

        if (verbose) {
            cout << "\nTesting non-empty subjects." << endl;

            if (veryVerbose) {
                T_ P(PATTERN)
            }
        }

        for (size_t i = 0; i < NUM_DATA; ++i) {
            const int     LINE         = DATA[i].d_lineNum;
            const char   *SUBJECT      = DATA[i].d_subject;
            const size_t  MATCH_OFFSET = DATA[i].d_matchOffset;
            const char   *MATCH_STRING = DATA[i].d_matchString;
            const size_t  SUBJECT_LEN  = strlen(SUBJECT);

            if (veryVerbose) {
                T_ P_(LINE) P_(SUBJECT) P(MATCH_STRING)
            }

            string_view               matchView;
            bslstl::StringRef         match;
            string_view               matchRawView;
            bslstl::StringRef         matchRaw;
            vector<string_view>       vMatchView;
            vector<bslstl::StringRef> vMatch;
            vector<string_view>       vMatchRawView;
            vector<bslstl::StringRef> vMatchRaw;

            for (size_t subjectStart = 0; subjectStart <= SUBJECT_LEN;
                                                              ++subjectStart) {
                if (veryVeryVerbose) {
                    T_ T_ P(subjectStart)
                }

                retCodeView    = X.match(   &matchView,
                                            string_view(SUBJECT, SUBJECT_LEN),
                                            subjectStart);
                retCode        = X.match(   &match,
                                            SUBJECT, SUBJECT_LEN,
                                            subjectStart);
                retCodeRawView = X.matchRaw(&matchRawView,
                                            string_view(SUBJECT, SUBJECT_LEN),
                                            subjectStart);
                retCodeRaw     = X.matchRaw(&matchRaw,
                                            SUBJECT, SUBJECT_LEN,
                                            subjectStart);

                if (subjectStart <= MATCH_OFFSET) {
                    ASSERTV(LINE, subjectStart, 0 == retCodeView);
                    ASSERTV(LINE, subjectStart, 0 == retCode);
                    ASSERTV(LINE, subjectStart, 0 == retCodeRawView);
                    ASSERTV(LINE, subjectStart, 0 == retCodeRaw);
                    ASSERTV(LINE, subjectStart, MATCH_STRING == matchView);
                    ASSERTV(LINE, subjectStart, MATCH_STRING == match);
                    ASSERTV(LINE, subjectStart, MATCH_STRING == matchRawView);
                    ASSERTV(LINE, subjectStart, MATCH_STRING == matchRaw);
                }
                else {
                    ASSERTV(LINE, subjectStart, 0 != retCodeView);
                    ASSERTV(LINE, subjectStart, 0 != retCode);
                    ASSERTV(LINE, subjectStart, 0 != retCodeRawView);
                    ASSERTV(LINE, subjectStart, 0 != retCodeRaw);
                }

                retCodeView    = X.match(   &vMatchView,
                                            string_view(SUBJECT, SUBJECT_LEN),
                                            subjectStart);
                retCode        = X.match(   &vMatch,
                                            SUBJECT, SUBJECT_LEN,
                                            subjectStart);
                retCodeRawView = X.matchRaw(&vMatchRawView,
                                            string_view(SUBJECT, SUBJECT_LEN),
                                            subjectStart);
                retCodeRaw     = X.matchRaw(&vMatchRaw,
                                            SUBJECT, SUBJECT_LEN,
                                            subjectStart);

                if (subjectStart <= MATCH_OFFSET) {
                    ASSERTV(LINE, subjectStart, 0 == retCodeView);
                    ASSERTV(LINE, subjectStart, 0 == retCode);
                    ASSERTV(LINE, subjectStart, 0 == retCodeRawView);
                    ASSERTV(LINE, subjectStart, 0 == retCodeRaw);
                    ASSERTV(LINE, subjectStart, MATCH_STRING == vMatchView[0]);
                    ASSERTV(LINE, subjectStart, MATCH_STRING == vMatch[0]);
                    ASSERTV(LINE, subjectStart, MATCH_STRING ==
                                                             vMatchRawView[0]);
                    ASSERTV(LINE, subjectStart, MATCH_STRING == vMatchRaw[0]);
                }
                else {
                    ASSERTV(LINE, subjectStart, 0 != retCodeView);
                    ASSERTV(LINE, subjectStart, 0 != retCode);
                    ASSERTV(LINE, subjectStart, 0 != retCodeRawView);
                    ASSERTV(LINE, subjectStart, 0 != retCodeRaw);
                }

            }
        }

        if (verbose) cout << "\nTesting empty subjects." << endl;
        {
            const char GOOD_PATTERN[]     = "(abc)*";
            const char NOT_GOOD_PATTERN[] = "(abc)+";

            Obj mGood(&ta);    const Obj& GOOD     = mGood;
            Obj mNotGood(&ta); const Obj& NOT_GOOD = mNotGood;

            if (veryVerbose) {
                cout << "\tPreparing regular expression objects." << endl;
            }

            int retCode = mGood.prepare(&errorMsg,
                                        &errorOffset,
                                        GOOD_PATTERN,
                                        0);
            ASSERTV(errorMsg, errorOffset, 0 == retCode);

            retCode = mNotGood.prepare(&errorMsg,
                                       &errorOffset,
                                       NOT_GOOD_PATTERN,
                                       0);
            ASSERTV(errorMsg, errorOffset, 0 == retCode);

            if (veryVerbose) {
                cout << "\tTesting good pattern." << endl;
            }

            string_view               matchView;
            string_view               match;

            vector<string_view>       vMatchView;
            vector<bslstl::StringRef> vMatch;

            retCode = GOOD.match(&matchView, string_view("", 0));
            ASSERT(0 == retCode);
            ASSERTV(match,   "" == matchView);

            retCode = GOOD.match(&match, "", 0);
            ASSERT(0 == retCode);
            ASSERTV(match,   "" == match);

            retCode = GOOD.matchRaw(&matchView, string_view("", 0));
            ASSERT(0 == retCode);
            ASSERTV(match,   "" == matchView);

            retCode = GOOD.matchRaw(&match, "", 0);
            ASSERT(0 == retCode);
            ASSERTV(match,   "" == match);

            retCode = GOOD.match(&vMatchView, string_view("", 0));
            ASSERT(0 == retCode);
            ASSERTV(vMatchView[0],  "" == vMatchView[0]);

            retCode = GOOD.match(&vMatch, "", 0);
            ASSERT(0 == retCode);
            ASSERTV(vMatch[0],  "" == vMatch[0]);

            retCode = GOOD.matchRaw(&vMatchView, string_view("", 0));
            ASSERT(0 == retCode);
            ASSERTV(vMatchView[0],  "" == vMatchView[0]);

            retCode = GOOD.matchRaw(&vMatch, "", 0);
            ASSERT(0 == retCode);
            ASSERTV(vMatch[0],  "" == vMatch[0]);

            if (veryVerbose) {
                cout << "\tTesting not so good pattern." << endl;
            }

            retCode = NOT_GOOD.match(&matchView, string_view("", 0));
            ASSERT(0 != retCode);

            retCode = NOT_GOOD.match(&match, "", 0);
            ASSERT(0 != retCode);

            retCode = NOT_GOOD.matchRaw(&matchView, string_view("", 0));
            ASSERT(0 != retCode);

            retCode = NOT_GOOD.matchRaw(&match, "", 0);
            ASSERT(0 != retCode);

            retCode = NOT_GOOD.match(&vMatchView, string_view("", 0));
            ASSERT(0 != retCode);

            retCode = NOT_GOOD.match(&vMatch, "", 0);
            ASSERT(0 != retCode);

            retCode = NOT_GOOD.matchRaw(&vMatchView, string_view("", 0));
            ASSERT(0 != retCode);

            retCode = NOT_GOOD.matchRaw(&vMatch, "", 0);
            ASSERT(0 != retCode);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj mX(&ta); const Obj& X = mX;

            bsl::string errorMsg;
            size_t      errorOffset;
            const char  PATTERN[] = "(abc)*";
            const char  SUBJECT[] = "XXXabcZZZ";

            string_view                    r, *zr = 0;
            string_view                    p, *zp = 0;

            bsl::vector<string_view>       z, *zz = 0;
            bsl::vector<bslstl::StringRef>       v, *zv = 0;

            (void)zp;
            (void)zv;

            ASSERT(0 == mX.prepare(&errorMsg, &errorOffset, PATTERN, 0));

            // 'match' taking 'StringRef'
            {
                ASSERT_SAFE_PASS(X.match(   &r, string_view(SUBJECT,  9),  1));
                ASSERT_SAFE_PASS(X.match(   &p, SUBJECT,  9,               1));
                ASSERT_SAFE_PASS(X.matchRaw(&r, string_view(SUBJECT,  9),  1));
                ASSERT_SAFE_PASS(X.matchRaw(&p, SUBJECT,  9,               1));
                ASSERT_FAIL(X.match(        zr, string_view(SUBJECT,  9),  1));
                ASSERT_FAIL(X.match(        zp, SUBJECT,  9,               1));
                ASSERT_FAIL(X.matchRaw(     zr, string_view(SUBJECT,  9),  1));
                ASSERT_FAIL(X.matchRaw(     zp, SUBJECT,  9,               1));

                ASSERT_SAFE_PASS(X.match(   &r, string_view(SUBJECT,  9),  1));
                ASSERT_SAFE_PASS(X.match(   &p, SUBJECT,  9,               1));
                ASSERT_SAFE_PASS(X.matchRaw(&r, string_view(SUBJECT,  9),  1));
                ASSERT_SAFE_PASS(X.matchRaw(&p, SUBJECT,  9,               1));
                // string_view can not be passed (0,  9)
                ASSERT_FAIL(X.match(        &p,  0,  9,                    1));
                ASSERT_FAIL(X.matchRaw(     &p,  0,  9,                    1));

                ASSERT_SAFE_PASS(X.match(   &r,  string_view(0,  0),  0));
                ASSERT_SAFE_PASS(X.match(   &p,  0,  0,               0));
                ASSERT_SAFE_PASS(X.matchRaw(&r,  string_view(0,  0),  0));
                ASSERT_SAFE_PASS(X.matchRaw(&p,  0,  0,               0));
                // string_view can not be passed (0,  1)
                ASSERT_FAIL(X.match(        &p,       0,  1,  0));
                ASSERT_FAIL(X.matchRaw(     &p,       0,  1,  0));

                ASSERT_SAFE_PASS(X.match(   &r, string_view(SUBJECT,  0),  0));
                ASSERT_SAFE_PASS(X.match(   &p, SUBJECT,  0,               0));
                ASSERT_SAFE_PASS(X.matchRaw(&r, string_view(SUBJECT,  0),  0));
                ASSERT_SAFE_PASS(X.matchRaw(&p, SUBJECT,  0,               0));
                ASSERT_FAIL(X.match(        &r, string_view(SUBJECT,  0), -1));
                ASSERT_FAIL(X.match(        &p, SUBJECT,  0,              -1));
                ASSERT_FAIL(X.matchRaw(     &r, string_view(SUBJECT,  0), -1));
                ASSERT_FAIL(X.matchRaw(     &p, SUBJECT,  0,              -1));

                ASSERT_SAFE_PASS(X.match(   &r, string_view(SUBJECT,  1),  1));
                ASSERT_SAFE_PASS(X.match(   &p, SUBJECT,  1,               1));
                ASSERT_SAFE_PASS(X.matchRaw(&r, string_view(SUBJECT,  1),  1));
                ASSERT_SAFE_PASS(X.matchRaw(&p, SUBJECT,  1,               1));
                ASSERT_FAIL(X.match(        &r, string_view(SUBJECT,  1),  2));
                ASSERT_FAIL(X.match(        &p, SUBJECT,  1,               2));
                ASSERT_FAIL(X.matchRaw(     &r, string_view(SUBJECT,  1),  2));
                ASSERT_FAIL(X.matchRaw(     &p, SUBJECT,  1,               2));
            }

            // 'match' taking 'bsl::vector' of 'StringRef'
            {
                ASSERT_SAFE_PASS(X.match(   &z, string_view(SUBJECT,  9),  1));
                ASSERT_SAFE_PASS(X.match(   &v, SUBJECT,  9,               1));
                ASSERT_SAFE_PASS(X.matchRaw(&z, string_view(SUBJECT,  9),  1));
                ASSERT_SAFE_PASS(X.matchRaw(&v, SUBJECT,  9,               1));
                ASSERT_FAIL(X.match(        zz, string_view(SUBJECT,  9),  1));
                ASSERT_FAIL(X.match(        zv, SUBJECT,  9,               1));
                ASSERT_FAIL(X.matchRaw(     zz, string_view(SUBJECT,  9),  1));
                ASSERT_FAIL(X.matchRaw(     zv, SUBJECT,  9,               1));

                ASSERT_SAFE_PASS(X.match(   &z, string_view(SUBJECT,  9),  1));
                ASSERT_SAFE_PASS(X.match(   &v, SUBJECT,  9,               1));
                ASSERT_SAFE_PASS(X.matchRaw(&z, string_view(SUBJECT,  9),  1));
                ASSERT_SAFE_PASS(X.matchRaw(&v, SUBJECT,  9,               1));
                // string_view can not be passed (0,  9)
                ASSERT_FAIL(X.match(        &v,  0,  9,                    1));
                ASSERT_FAIL(X.matchRaw(     &v,  0,  9,                    1));

                ASSERT_SAFE_PASS(X.match(   &z,  string_view(0,  0),       0));
                ASSERT_SAFE_PASS(X.match(   &v,  0,  0,                    0));
                ASSERT_SAFE_PASS(X.matchRaw(&z,  string_view(0,  0),       0));
                ASSERT_SAFE_PASS(X.matchRaw(&v,  0,  0,                    0));
                // string_view can not be passed (0,  1)
                ASSERT_FAIL(X.match(        &v,  0,  1,                    0));
                ASSERT_FAIL(X.matchRaw(     &v,  0,  1,                    0));

                ASSERT_SAFE_PASS(X.match(   &z, string_view(SUBJECT,  0),  0));
                ASSERT_SAFE_PASS(X.match(   &v, SUBJECT,  0,               0));
                ASSERT_SAFE_PASS(X.matchRaw(&z, string_view(SUBJECT,  0),  0));
                ASSERT_SAFE_PASS(X.matchRaw(&v, SUBJECT,  0,               0));
                ASSERT_FAIL(X.match(        &z, string_view(SUBJECT,  0), -1));
                ASSERT_FAIL(X.match(        &v, SUBJECT,  0,              -1));
                ASSERT_FAIL(X.matchRaw(     &z, string_view(SUBJECT,  0), -1));
                ASSERT_FAIL(X.matchRaw(     &v, SUBJECT,  0,              -1));

                ASSERT_SAFE_PASS(X.match(   &z, string_view(SUBJECT,  1),  1));
                ASSERT_SAFE_PASS(X.match(   &v, SUBJECT,  1,               1));
                ASSERT_SAFE_PASS(X.matchRaw(&z, string_view(SUBJECT,  1),  1));
                ASSERT_SAFE_PASS(X.matchRaw(&v, SUBJECT,  1,               1));
                ASSERT_FAIL(X.match(        &z, string_view(SUBJECT,  1),  2));
                ASSERT_FAIL(X.match(        &v, SUBJECT,  1,               2));
                ASSERT_FAIL(X.matchRaw(     &z, string_view(SUBJECT,  1),  2));
                ASSERT_FAIL(X.matchRaw(     &v, SUBJECT,  1,               2));
            }

            // restore object to unprepared state
            {
                mX.clear();

                ASSERT_FAIL(X.match(        &r, string_view(SUBJECT,  9),  1));
                ASSERT_FAIL(X.match(        &p, SUBJECT,  9,               1));
                ASSERT_FAIL(X.matchRaw(     &r, string_view(SUBJECT,  9),  1));
                ASSERT_FAIL(X.matchRaw(     &p, SUBJECT,  9,               1));

                ASSERT_FAIL(X.match(        &z, string_view(SUBJECT,  9),  1));
                ASSERT_FAIL(X.match(        &v, SUBJECT,  9,               1));
                ASSERT_FAIL(X.matchRaw(     &z, string_view(SUBJECT,  9),  1));
                ASSERT_FAIL(X.matchRaw(     &v, SUBJECT,  9,               1));
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'match' AND 'matchRaw' METHODS
        //
        // Concerns:
        //: 1 Although we are not testing the implementation of the PCRE
        //:   library, we want to check that the interface is plugged in
        //:   correctly and works as documented.  In particular, we want to
        //:   check that the 'subject', 'subjectLength', and 'subjectStart'
        //:   arguments are passed to PCRE correctly and the resulting
        //:   'ovector' is copied correctly to the 'result'.  Also we want to
        //:   make sure that a failure code is returned when the subject does
        //:   not match the pattern.
        //
        // Plan:
        //: 1 Prepare a regular expression object using a given 'PATTERN'
        //:   string, create a set of subjects that contain a match for
        //:   'PATTERN'.  The set should contain subjects of increasing length,
        //:   and also increasing match offsets ('matchOffset').
        //:
        //: 2 Exercise the 'match' and 'matchRaw' methods using 'subjectStart'
        //:   values in the range [0..'subjectLength'].  Check that the methods
        //:   succeed when 'subjectStart' <= 'matchOffset' and they fail when
        //:   'subjectStart' > 'matchOffset'.  For each successful call to
        //:   'match' or 'matchRaw', check that 'result' contains the correct
        //:   offset and length for the captured string.  Note that captured
        //:   substrings are tested in later test case.
        //:
        //: 3 Finally, exercise the special case where 'subjectLength' is 0.
        //
        // Testing:
        //   int match(const bsl::string_view&, ...) const;
        //   int match(const char *subject, ...) const;
        //   int match(bsl::pair<size_t, size_t>*, ...) const;
        //   int match(bsl::vector<bsl::pair<size_t, size_t> >*, ...) const;
        //   int match(bsl::string_view*, ...) const;
        //   int match(bsl::vector<bslstl::string_view>*, ...) const;
        //   int match(bsl::vector<bsl::string_view> *result, ...) const;
        //   int match(std::vector<bsl::string_view> *result, ...) const;
        //   int match(std::pmr::vector<bsl::string_view> *result, ...) const;
        //   int matchRaw(const char *subject, ...) const;
        //   int matchRaw(bsl::pair<size_t, size_t> *result, ...) const;
        //   int matchRaw(vector<bsl::pair<size_t, size_t> > *result,...)const;
        //   int matchRaw(bsl::vector<bsl::string_view> *result, ...) const;
        //   int matchRaw(bsl::vector<bsl::string_view> *result, ...) const;
        //   int matchRaw(std::pmr::vector<bsl::string_view> *result,...)const;
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'match' AND 'matchRaw' METHODS" << endl
                          << "======================================" << endl;

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        const char PATTERN[] = "(a(b(c)?)?)";  // matches 'a', 'ab', or 'abc'

        static const struct {
            int         d_lineNum;      // source line number
            const char *d_subject;      // subject string
            size_t      d_matchOffset;  // match offset
            size_t      d_matchLength;  // match length
        } DATA[] = {
            //line   subject             matchOffset   matchLength
            //----   -------             -----------   -----------

            // subject length = 1
            { L_,    "a",                0,            1            },

            // subject length = 2
            { L_,    "aZ",               0,            1            },
            { L_,    "ab",               0,            2            },
            { L_,    "Za",               1,            1            },

            // subject length = 3
            { L_,    "aZZ",              0,            1            },
            { L_,    "abZ",              0,            2            },
            { L_,    "abc",              0,            3            },
            { L_,    "ZaZ",              1,            1            },
            { L_,    "Zab",              1,            2            },
            { L_,    "ZZa",              2,            1            },

            // subject length = 4
            { L_,    "aZZZ",             0,            1            },
            { L_,    "abZZ",             0,            2            },
            { L_,    "abcZ",             0,            3            },
            { L_,    "ZaZZ",             1,            1            },
            { L_,    "ZabZ",             1,            2            },
            { L_,    "Zabc",             1,            3            },
            { L_,    "ZZaZ",             2,            1            },
            { L_,    "ZZab",             2,            2            },
            { L_,    "ZZZa",             3,            1            },

            // subject length = 5
            { L_,    "aZZZZ",            0,            1            },
            { L_,    "abZZZ",            0,            2            },
            { L_,    "abcZZ",            0,            3            },
            { L_,    "ZaZZZ",            1,            1            },
            { L_,    "ZabZZ",            1,            2            },
            { L_,    "ZabcZ",            1,            3            },
            { L_,    "ZZaZZ",            2,            1            },
            { L_,    "ZZabZ",            2,            2            },
            { L_,    "ZZabc",            2,            3            },
            { L_,    "ZZZaZ",            3,            1            },
            { L_,    "ZZZab",            3,            2            },
            { L_,    "ZZZZa",            4,            1            },

            // subject length = 6
            { L_,    "aZZZZZ",           0,            1            },
            { L_,    "abZZZZ",           0,            2            },
            { L_,    "abcZZZ",           0,            3            },
            { L_,    "ZaZZZZ",           1,            1            },
            { L_,    "ZabZZZ",           1,            2            },
            { L_,    "ZabcZZ",           1,            3            },
            { L_,    "ZZaZZZ",           2,            1            },
            { L_,    "ZZabZZ",           2,            2            },
            { L_,    "ZZabcZ",           2,            3            },
            { L_,    "ZZZaZZ",           3,            1            },
            { L_,    "ZZZabZ",           3,            2            },
            { L_,    "ZZZabc",           3,            3            },
            { L_,    "ZZZZaZ",           4,            1            },
            { L_,    "ZZZZab",           4,            2            },
            { L_,    "ZZZZZa",           5,            1            },
        };
        const size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        Obj mX(&ta); const Obj& X = mX;

        bsl::string errorMsg;
        size_t      errorOffset;

        int retCode = mX.prepare(&errorMsg, &errorOffset, PATTERN, 0, 0);

        ASSERTV(errorMsg, errorOffset, 0 == retCode);

        if (verbose) {
            cout << "\nTesting non-empty subjects." << endl;

            if (veryVerbose) {
                T_ P(PATTERN)
            }
        }

        for (size_t i = 0; i < NUM_DATA; ++i) {
            const int     LINE         = DATA[i].d_lineNum;
            const char   *SUBJECT      = DATA[i].d_subject;
            const size_t  MATCH_OFFSET = DATA[i].d_matchOffset;
            const size_t  MATCH_LENGTH = DATA[i].d_matchLength;
            const size_t  SUBJECT_LEN  = strlen(SUBJECT);
            const string_view EXPECTED = string_view(SUBJECT + MATCH_OFFSET,
                                                      MATCH_LENGTH);

            if (veryVerbose) {
                T_ P_(LINE) P_(SUBJECT) P_(MATCH_OFFSET) P(MATCH_LENGTH)
            }

            int retCode1;
            int retCodePr;
            int retCodeSr;
            int retCodeSv;
            int retCodeVPr;
            int retCodeVSr;
            int retCodeVSv;
            int retCodeVSv1;
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            int retCodeVSv2;
#endif
            bsl::pair<size_t, size_t>           pr;
            bslstl::StringRef                   sr;
            bsl::string_view                    sv;
            bsl::vector<pair<size_t, size_t> >  vpr;
            bsl::vector<bslstl::StringRef>      vsr;
            bsl::vector<bsl::string_view>       vsv;
            std::vector<bsl::string_view>       vsv1;
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            std::pmr::vector<bsl::string_view>  vsv2;
#endif
            for (size_t subjectStart = 0; subjectStart <= SUBJECT_LEN;
                                                              ++subjectStart) {
                if (veryVeryVerbose) {
                    T_ T_ P(subjectStart)
                }

                retCode     = X.match(       string_view(SUBJECT, SUBJECT_LEN),
                                             subjectStart);
                retCode1    = X.match(       SUBJECT, SUBJECT_LEN,
                                             subjectStart);
                retCodePr   = X.match(&pr,   SUBJECT, SUBJECT_LEN,
                                             subjectStart);
                retCodeSr   = X.match(&sr,   SUBJECT, SUBJECT_LEN,
                                             subjectStart);
                retCodeSv   = X.match(&sv,   string_view(SUBJECT, SUBJECT_LEN),
                                             subjectStart);
                retCodeVPr  = X.match(&vpr,  SUBJECT, SUBJECT_LEN,
                                             subjectStart);
                retCodeVSr  = X.match(&vsr,  SUBJECT, SUBJECT_LEN,
                                             subjectStart);
                retCodeVSv  = X.match(&vsv,  string_view(SUBJECT, SUBJECT_LEN),
                                             subjectStart);
                retCodeVSv1 = X.match(&vsv1, string_view(SUBJECT, SUBJECT_LEN),
                                             subjectStart);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                retCodeVSv2 = X.match(&vsv2, string_view(SUBJECT, SUBJECT_LEN),
                                             subjectStart);
#endif
                if (subjectStart <= MATCH_OFFSET) {
                    ASSERTV(LINE, subjectStart, 0 == retCode);
                    ASSERTV(LINE, subjectStart, 0 == retCode1);
                    ASSERTV(LINE, subjectStart, 0 == retCodePr);
                    ASSERTV(LINE, subjectStart,                 pr.first,
                                                MATCH_OFFSET == pr.first);
                    ASSERTV(LINE, subjectStart,                 pr.second,
                                                MATCH_LENGTH == pr.second);
                    ASSERTV(LINE, subjectStart, 0 == retCodeSr);
                    ASSERTV(LINE, subjectStart, EXPECTED, sr, EXPECTED == sr);
                    ASSERTV(LINE, subjectStart, 0 == retCodeSv);
                    ASSERTV(LINE, subjectStart, EXPECTED, sv, EXPECTED == sv);
                    ASSERTV(LINE, subjectStart, 0 == retCodeVPr);
                    ASSERTV(LINE, subjectStart,                 vpr[0].first,
                                                MATCH_OFFSET == vpr[0].first);
                    ASSERTV(LINE, subjectStart,                 vpr[0].second,
                                                MATCH_LENGTH == vpr[0].second);
                    ASSERTV(LINE, subjectStart, 0 == retCodeVSr);
                    ASSERTV(LINE, subjectStart, EXPECTED,   vsr[0],
                                                EXPECTED == vsr[0]);
                    ASSERTV(LINE, subjectStart, 0 == retCodeVSv);
                    ASSERTV(LINE, subjectStart, EXPECTED,   vsv[0],
                                                EXPECTED == vsv[0]);
                    ASSERTV(LINE, subjectStart, 0 == retCodeVSv1);
                    ASSERTV(LINE, subjectStart, EXPECTED,   vsv1[0],
                                                EXPECTED == vsv1[0]);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                    ASSERTV(LINE, subjectStart, 0 == retCodeVSv2);
                    ASSERTV(LINE, subjectStart, EXPECTED,   vsv2[0],
                                                EXPECTED == vsv2[0]);
#endif
                }
                else {
                    ASSERTV(LINE, subjectStart, 0 != retCode);
                    ASSERTV(LINE, subjectStart, 0 != retCode1);
                    ASSERTV(LINE, subjectStart, 0 != retCodePr);
                    ASSERTV(LINE, subjectStart, 0 != retCodeSr);
                    ASSERTV(LINE, subjectStart, 0 != retCodeSv);
                    ASSERTV(LINE, subjectStart, 0 != retCodeVPr);
                    ASSERTV(LINE, subjectStart, 0 != retCodeVSr);
                    ASSERTV(LINE, subjectStart, 0 != retCodeVSv);
                    ASSERTV(LINE, subjectStart, 0 != retCodeVSv1);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                    ASSERTV(LINE, subjectStart, 0 != retCodeVSv2);
#endif
                }

                retCode     = X.matchRaw(       string_view(SUBJECT,
                                                            SUBJECT_LEN),
                                                subjectStart);
                retCode1    = X.matchRaw(       SUBJECT, SUBJECT_LEN,
                                                subjectStart);
                retCodePr   = X.matchRaw(&pr,   SUBJECT, SUBJECT_LEN,
                                                subjectStart);
                retCodeSr   = X.matchRaw(&sr,   SUBJECT, SUBJECT_LEN,
                                                subjectStart);
                retCodeSv   = X.matchRaw(&sv,   string_view(SUBJECT,
                                                            SUBJECT_LEN),
                                                subjectStart);
                retCodeVPr  = X.matchRaw(&vpr,  SUBJECT, SUBJECT_LEN,
                                                subjectStart);
                retCodeVSr  = X.matchRaw(&vsr,  SUBJECT, SUBJECT_LEN,
                                                subjectStart);
                retCodeVSv  = X.matchRaw(&vsv,  string_view(SUBJECT,
                                                            SUBJECT_LEN),
                                                subjectStart);
                retCodeVSv1 = X.matchRaw(&vsv1, string_view(SUBJECT,
                                                            SUBJECT_LEN),
                                                subjectStart);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                retCodeVSv2 = X.matchRaw(&vsv2, string_view(SUBJECT,
                                                            SUBJECT_LEN),
                                                subjectStart);
#endif
                if (subjectStart <= MATCH_OFFSET) {
                    ASSERTV(LINE, subjectStart, 0 == retCode);
                    ASSERTV(LINE, subjectStart, 0 == retCode1);
                    ASSERTV(LINE, subjectStart, 0 == retCodePr);
                    ASSERTV(LINE, subjectStart,                 pr.first,
                                                MATCH_OFFSET == pr.first);
                    ASSERTV(LINE, subjectStart,                 pr.second,
                                                MATCH_LENGTH == pr.second);
                    ASSERTV(LINE, subjectStart, 0 == retCodeSr);
                    ASSERTV(LINE, subjectStart, EXPECTED, sr, EXPECTED == sr);
                    ASSERTV(LINE, subjectStart, 0 == retCodeSv);
                    ASSERTV(LINE, subjectStart, EXPECTED, sv, EXPECTED == sv);
                    ASSERTV(LINE, subjectStart, 0 == retCodeVPr);
                    ASSERTV(LINE, subjectStart,                 vpr[0].first,
                                                MATCH_OFFSET == vpr[0].first);
                    ASSERTV(LINE, subjectStart,                 vpr[0].second,
                                                MATCH_LENGTH == vpr[0].second);
                    ASSERTV(LINE, subjectStart, 0 == retCodeVSr);
                    ASSERTV(LINE, subjectStart, EXPECTED,   vsr[0],
                                                EXPECTED == vsr[0]);
                    ASSERTV(LINE, subjectStart, 0 == retCodeVSv);
                    ASSERTV(LINE, subjectStart, EXPECTED,   vsv[0],
                                                EXPECTED == vsv[0]);
                    ASSERTV(LINE, subjectStart, 0 == retCodeVSv1);
                    ASSERTV(LINE, subjectStart, EXPECTED,   vsv1[0],
                                                EXPECTED == vsv1[0]);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                    ASSERTV(LINE, subjectStart, 0 == retCodeVSv2);
                    ASSERTV(LINE, subjectStart, EXPECTED,   vsv2[0],
                                                EXPECTED == vsv2[0]);
#endif
                }
                else {
                    ASSERTV(LINE, subjectStart, 0 != retCode);
                    ASSERTV(LINE, subjectStart, 0 != retCode1);
                    ASSERTV(LINE, subjectStart, 0 != retCodePr);
                    ASSERTV(LINE, subjectStart, 0 != retCodeSr);
                    ASSERTV(LINE, subjectStart, 0 != retCodeSv);
                    ASSERTV(LINE, subjectStart, 0 != retCodeVPr);
                    ASSERTV(LINE, subjectStart, 0 != retCodeVSr);
                    ASSERTV(LINE, subjectStart, 0 != retCodeVSv);
                    ASSERTV(LINE, subjectStart, 0 != retCodeVSv1);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                    ASSERTV(LINE, subjectStart, 0 != retCodeVSv2);
#endif
                }
            }
        }

        if (verbose) {
            cout << "\nTesting empty subjects." << endl;
        }

        {
            const char GOOD_PATTERN[]     = "(abc)*";
            const char NOT_GOOD_PATTERN[] = "(abc)+";

            Obj mGood(&ta);    const Obj& GOOD     = mGood;
            Obj mNotGood(&ta); const Obj& NOT_GOOD = mNotGood;

            if (veryVerbose) {
                cout << "\tPreparing regular expression objects." << endl;
            }

            int retCode = mGood.prepare(&errorMsg,
                                        &errorOffset,
                                        GOOD_PATTERN,
                                        0,
                                        0);
            ASSERTV(errorMsg, errorOffset, 0 == retCode);

            retCode = mNotGood.prepare(&errorMsg,
                                       &errorOffset,
                                       NOT_GOOD_PATTERN,
                                       0,
                                       0);
            ASSERTV(errorMsg, errorOffset, 0 == retCode);

            if (veryVerbose) {
                cout << "\tTesting good pattern." << endl;
            }

            retCode = GOOD.match(string_view("", 0));
            ASSERT(0 == retCode);

            retCode = GOOD.match("", 0);
            ASSERT(0 == retCode);

            retCode = GOOD.matchRaw("", 0);
            ASSERT(0 == retCode);

            retCode = GOOD.matchRaw(string_view("", 0));
            ASSERT(0 == retCode);

            bsl::pair<size_t, size_t>           pr;
            bslstl::StringRef                   sr;
            bsl::string_view                    sv;
            bsl::vector<pair<size_t, size_t> >  vpr;
            bsl::vector<bslstl::StringRef>      vsr;
            bsl::vector<bsl::string_view>       vsv;
            std::vector<bsl::string_view>       vsv1;
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            std::pmr::vector<bsl::string_view>  vsv2;
#endif
            retCode = GOOD.match(&pr, "", 0);
            ASSERT(0 == retCode);
            ASSERTV(pr.first,   0 == pr.first);
            ASSERTV(pr.second , 0 == pr.second);

            retCode = GOOD.match(&sr, "", 0);
            ASSERT(0 == retCode);
            ASSERTV(sr, sr.empty());

            retCode = GOOD.match(&sv, string_view("", 0));
            ASSERT(0 == retCode);
            ASSERTV(sv, sv.empty());

            retCode = GOOD.match(&vpr, "", 0);
            ASSERT(0 == retCode);
            ASSERTV(vpr[0].first,   0 == vpr[0].first);
            ASSERTV(vpr[0].second , 0 == vpr[0].second);

            retCode = GOOD.match(&vsr, "", 0);
            ASSERT(0 == retCode);
            ASSERTV(vsr[0], bslstl::StringRef("", 0) == vsr[0]);

            retCode = GOOD.match(&vsv, string_view("", 0));
            ASSERT(0 == retCode);
            ASSERTV(vsv[0], string_view("", 0) == vsv[0]);

            retCode = GOOD.match(&vsv1, string_view("", 0));
            ASSERT(0 == retCode);
            ASSERTV(vsv1[0], string_view("", 0) == vsv1[0]);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            retCode = GOOD.match(&vsv2, string_view("", 0));
            ASSERT(0 == retCode);
            ASSERTV(vsv2[0], string_view("", 0) == vsv2[0]);
#endif

            retCode = GOOD.matchRaw(&pr, "", 0);
            ASSERT(0 == retCode);
            ASSERTV(pr.first,   0 == pr.first);
            ASSERTV(pr.second , 0 == pr.second);

            retCode = GOOD.matchRaw(&sr, "", 0);
            ASSERT(0 == retCode);
            ASSERTV(sr, sr.empty());

            retCode = GOOD.matchRaw(&vpr, "", 0);
            ASSERT(0 == retCode);
            ASSERTV(vpr[0].first,   0 == vpr[0].first);
            ASSERTV(vpr[0].second , 0 == vpr[0].second);

            retCode = GOOD.matchRaw(&vsr, "", 0);
            ASSERT(0 == retCode);
            ASSERTV(vsr[0], bslstl::StringRef("", 0) == vsr[0]);

            if (veryVerbose) {
                cout << "\tTesting not so good pattern." << endl;
            }

            retCode = NOT_GOOD.match(   string_view("", 0));
            ASSERT(0 != retCode);
            retCode = NOT_GOOD.matchRaw(string_view("", 0));
            ASSERT(0 != retCode);

            retCode = NOT_GOOD.match(   "", 0);
            ASSERT(0 != retCode);
            retCode = NOT_GOOD.matchRaw("", 0);
            ASSERT(0 != retCode);

            retCode = NOT_GOOD.match(   &pr,    "", 0);
            ASSERT(0 != retCode);
            retCode = NOT_GOOD.matchRaw(&pr,    "", 0);
            ASSERT(0 != retCode);

            retCode = NOT_GOOD.match(   &sr,    "", 0);
            ASSERT(0 != retCode);
            retCode = NOT_GOOD.matchRaw(&sr,    "", 0);
            ASSERT(0 != retCode);

            retCode = NOT_GOOD.match(   &sv,    string_view("", 0));
            ASSERT(0 != retCode);
            retCode = NOT_GOOD.matchRaw(&sv,    string_view("", 0));
            ASSERT(0 != retCode);

            retCode = NOT_GOOD.match(   &vpr,   "", 0);
            ASSERT(0 != retCode);
            retCode = NOT_GOOD.matchRaw(&vpr,   "", 0);
            ASSERT(0 != retCode);

            retCode = NOT_GOOD.match(   &vsr,   "", 0);
            ASSERT(0 != retCode);
            retCode = NOT_GOOD.matchRaw(&vsr,   "", 0);
            ASSERT(0 != retCode);

            retCode = NOT_GOOD.match(   &vsv,  string_view("", 0));
            ASSERT(0 != retCode);
            retCode = NOT_GOOD.matchRaw(&vsv,  string_view("", 0));
            ASSERT(0 != retCode);

            retCode = NOT_GOOD.match(   &vsv1, string_view("", 0));
            ASSERT(0 != retCode);
            retCode = NOT_GOOD.matchRaw(&vsv1, string_view("", 0));
            ASSERT(0 != retCode);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            retCode = NOT_GOOD.match(   &vsv2, string_view("", 0));
            ASSERT(0 != retCode);
            retCode = NOT_GOOD.matchRaw(&vsv2, string_view("", 0));
            ASSERT(0 != retCode);
#endif
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj mX(&ta); const Obj& X = mX;

            bsl::string errorMsg;
            size_t      errorOffset;
            const char  PATTERN[] = "(abc)*";
            const char  SUBJECT[] = "XXXabcZZZ";

            ASSERT(0 == mX.prepare(&errorMsg, &errorOffset, PATTERN, 0, 0));

            // basic 'match
            {
                ASSERT_SAFE_PASS(X.match(   string_view(SUBJECT,  9),  1));
                ASSERT_SAFE_PASS(X.match(   SUBJECT,  9,               1));
                ASSERT_SAFE_PASS(X.matchRaw(string_view(SUBJECT,  9),  1));
                ASSERT_SAFE_PASS(X.matchRaw(SUBJECT,  9,               1));
                // string_view can not be created with (0,  9)
                ASSERT_FAIL(X.match(        0,  9,                     1));
                ASSERT_FAIL(X.matchRaw(     0,  9,                     1));

                ASSERT_SAFE_PASS(X.match(   string_view(0,  0)));
                ASSERT_SAFE_PASS(X.match(   0,  0));
                ASSERT_SAFE_PASS(X.matchRaw(string_view(0,  0)));
                ASSERT_SAFE_PASS(X.matchRaw(0,  0));
                // string_view can not be created with (0,  1)
                ASSERT_FAIL(X.match(        0,  1));
                ASSERT_FAIL(X.matchRaw(     0,  1));

                ASSERT_SAFE_PASS(X.match(   string_view(SUBJECT,  0),  0));
                ASSERT_SAFE_PASS(X.match(   SUBJECT,  0,               0));
                ASSERT_SAFE_PASS(X.matchRaw(string_view(SUBJECT,  0),  0));
                ASSERT_SAFE_PASS(X.matchRaw(SUBJECT,  0,               0));
                ASSERT_FAIL(X.match(        string_view(SUBJECT,  0), -1));
                ASSERT_FAIL(X.match(        SUBJECT,  0,              -1));
                ASSERT_FAIL(X.matchRaw(     string_view(SUBJECT,  0), -1));
                ASSERT_FAIL(X.matchRaw(     SUBJECT,  0,              -1));

                ASSERT_SAFE_PASS(X.match(   string_view(SUBJECT,  1),  1));
                ASSERT_SAFE_PASS(X.match(   SUBJECT,  1,               1));
                ASSERT_SAFE_PASS(X.matchRaw(string_view(SUBJECT,  1),  1));
                ASSERT_SAFE_PASS(X.matchRaw(SUBJECT,  1,               1));
                ASSERT_FAIL(X.match(        string_view(SUBJECT,  1),  2));
                ASSERT_FAIL(X.match(        SUBJECT,  1,               2));
                ASSERT_FAIL(X.matchRaw(     string_view(SUBJECT,  1),  2));
                ASSERT_FAIL(X.matchRaw(     SUBJECT,  1,               2));
            }

            // 'match' taking 'bsl::string_view'
            {
                bsl::string_view p, *zp = 0; (void)zp;

                ASSERT_SAFE_PASS(X.match(   &p, SUBJECT,  9,               1));
                ASSERT_SAFE_PASS(X.match(   &p, string_view(SUBJECT,  9),  1));
                ASSERT_SAFE_PASS(X.matchRaw(&p, SUBJECT,  9,               1));
                ASSERT_SAFE_PASS(X.matchRaw(&p, string_view(SUBJECT,  9),  1));
                ASSERT_FAIL(X.match(        zp, SUBJECT,  9,               1));
                ASSERT_FAIL(X.match(        zp, string_view(SUBJECT,  9),  1));
                ASSERT_FAIL(X.matchRaw(     zp, SUBJECT,  9,               1));
                ASSERT_FAIL(X.matchRaw(     zp, string_view(SUBJECT,  9),  1));

                ASSERT_SAFE_PASS(X.match(   &p, SUBJECT,  9,               1));
                ASSERT_SAFE_PASS(X.match(   &p, string_view(SUBJECT,  9),  1));
                ASSERT_SAFE_PASS(X.matchRaw(&p, SUBJECT,  9,               1));
                ASSERT_SAFE_PASS(X.matchRaw(&p, string_view(SUBJECT,  9),  1));
                // string_view can not be created with (0,  9)
                ASSERT_FAIL(X.match(        &p, 0,  9,                     1));
                ASSERT_FAIL(X.matchRaw(     &p, 0,  9,                     1));

                ASSERT_SAFE_PASS(X.match(   &p, 0,  0,                     0));
                ASSERT_SAFE_PASS(X.match(   &p, string_view(0,  0),        0));
                ASSERT_SAFE_PASS(X.matchRaw(&p, 0,  0,                     0));
                ASSERT_SAFE_PASS(X.matchRaw(&p, string_view(0,  0),        0));
                // string_view can not be created with (0,  1)
                ASSERT_FAIL(X.match(        &p, 0,  1,                     0));
                ASSERT_FAIL(X.matchRaw(     &p, 0,  1,                     0));

                ASSERT_SAFE_PASS(X.match(   &p, SUBJECT,  0,               0));
                ASSERT_SAFE_PASS(X.match(   &p, string_view(SUBJECT,  0),  0));
                ASSERT_SAFE_PASS(X.matchRaw(&p, SUBJECT,  0,               0));
                ASSERT_SAFE_PASS(X.matchRaw(&p, string_view(SUBJECT,  0),  0));
                ASSERT_FAIL(X.match(        &p, SUBJECT,  0,              -1));
                ASSERT_FAIL(X.match(        &p, string_view(SUBJECT,  0), -1));
                ASSERT_FAIL(X.matchRaw(     &p, SUBJECT,  0,              -1));
                ASSERT_FAIL(X.matchRaw(     &p, string_view(SUBJECT,  0), -1));

                ASSERT_SAFE_PASS(X.match(   &p, SUBJECT,  1,               1));
                ASSERT_SAFE_PASS(X.match(   &p, string_view(SUBJECT,  1),  1));
                ASSERT_SAFE_PASS(X.matchRaw(&p, SUBJECT,  1,               1));
                ASSERT_SAFE_PASS(X.matchRaw(&p, string_view(SUBJECT,  1),  1));
                ASSERT_FAIL(X.match(        &p, SUBJECT,  1,               2));
                ASSERT_FAIL(X.match(        &p, string_view(SUBJECT,  1),  2));
                ASSERT_FAIL(X.matchRaw(     &p, SUBJECT,  1,               2));
                ASSERT_FAIL(X.matchRaw(     &p, string_view(SUBJECT,  1),  2));
            }

            // 'match' taking 'bsl::pair'
            {
                bsl::pair<size_t, size_t> p, *zp = 0; (void)zp;

                ASSERT_SAFE_PASS(X.match(   &p, SUBJECT,  9,               1));
                ASSERT_SAFE_PASS(X.matchRaw(&p, SUBJECT,  9,               1));
                ASSERT_FAIL(X.match(        zp, SUBJECT,  9,               1));
                ASSERT_FAIL(X.matchRaw(     zp, SUBJECT,  9,               1));

                ASSERT_SAFE_PASS(X.match(   &p, SUBJECT,  9,               1));
                ASSERT_SAFE_PASS(X.matchRaw(&p, SUBJECT,  9,               1));
                ASSERT_FAIL(X.match(        &p,  0,  9,                    1));
                ASSERT_FAIL(X.matchRaw(     &p,  0,  9,                    1));

                ASSERT_SAFE_PASS(X.match(   &p,  0,  0,                    0));
                ASSERT_SAFE_PASS(X.matchRaw(&p,  0,  0,                    0));
                ASSERT_FAIL(X.match(        &p,  0,  1,                    0));
                ASSERT_FAIL(X.matchRaw(     &p,  0,  1,                    0));

                ASSERT_SAFE_PASS(X.match(   &p, SUBJECT,  0,               0));
                ASSERT_SAFE_PASS(X.matchRaw(&p, SUBJECT,  0,               0));
                ASSERT_FAIL(X.match(        &p, SUBJECT,  0,              -1));
                ASSERT_FAIL(X.matchRaw(     &p, SUBJECT,  0,              -1));

                ASSERT_SAFE_PASS(X.match(   &p, SUBJECT,  1,               1));
                ASSERT_SAFE_PASS(X.matchRaw(&p, SUBJECT,  1,               1));
                ASSERT_FAIL(X.match(        &p, SUBJECT,  1,               2));
                ASSERT_FAIL(X.matchRaw(     &p, SUBJECT,  1,               2));
            }

            // 'match' taking 'bsl::vector<bsl::pair<size_t, size_t>'
            {
                bsl::vector<bsl::pair<size_t, size_t> > v, *zv = 0; (void)zv;

                ASSERT_SAFE_PASS(X.match(   &v, SUBJECT,  9,               1));
                ASSERT_SAFE_PASS(X.matchRaw(&v, SUBJECT,  9,               1));
                ASSERT_FAIL(X.match(        zv, SUBJECT,  9,               1));
                ASSERT_FAIL(X.matchRaw(     zv, SUBJECT,  9,               1));

                ASSERT_SAFE_PASS(X.match(   &v, SUBJECT,  9,               1));
                ASSERT_SAFE_PASS(X.matchRaw(&v, SUBJECT,  9,               1));
                ASSERT_FAIL(X.match(        &v,  0,  9,                    1));
                ASSERT_FAIL(X.matchRaw(     &v,  0,  9,                    1));

                ASSERT_SAFE_PASS(X.match(   &v,  0,  0,                    0));
                ASSERT_SAFE_PASS(X.matchRaw(&v,  0,  0,                    0));
                ASSERT_FAIL(X.match(        &v,  0,  1,                    0));
                ASSERT_FAIL(X.matchRaw(     &v,  0,  1,                    0));

                ASSERT_SAFE_PASS(X.match(   &v, SUBJECT,  0,               0));
                ASSERT_SAFE_PASS(X.matchRaw(&v, SUBJECT,  0,               0));
                ASSERT_FAIL(X.match(        &v, SUBJECT,  0,              -1));
                ASSERT_FAIL(X.matchRaw(     &v, SUBJECT,  0,              -1));

                ASSERT_SAFE_PASS(X.match(   &v, SUBJECT,  1,               1));
                ASSERT_SAFE_PASS(X.matchRaw(&v, SUBJECT,  1,               1));
                ASSERT_FAIL(X.match(        &v, SUBJECT,  1,               2));
                ASSERT_FAIL(X.matchRaw(     &v, SUBJECT,  1,               2));
            }

            // 'match' taking 'bsl::vector<bslstl::StringRef>'
            {
                bsl::vector<bslstl::StringRef> v, *zv = 0; (void)zv;

                ASSERT_SAFE_PASS(X.match(   &v, SUBJECT,  9,               1));
                ASSERT_SAFE_PASS(X.matchRaw(&v, SUBJECT,  9,               1));
                ASSERT_FAIL(X.match(        zv, SUBJECT,  9,               1));
                ASSERT_FAIL(X.matchRaw(     zv, SUBJECT,  9,               1));

                ASSERT_SAFE_PASS(X.match(   &v, SUBJECT,  9,               1));
                ASSERT_SAFE_PASS(X.matchRaw(&v, SUBJECT,  9,               1));
                ASSERT_FAIL(X.match(        &v,  0,  9,                    1));
                ASSERT_FAIL(X.matchRaw(     &v,  0,  9,                    1));

                ASSERT_SAFE_PASS(X.match(   &v,  0,  0,                    0));
                ASSERT_SAFE_PASS(X.matchRaw(&v,  0,  0,                    0));
                ASSERT_FAIL(X.match(        &v,  0,  1,                    0));
                ASSERT_FAIL(X.matchRaw(     &v,  0,  1,                    0));

                ASSERT_SAFE_PASS(X.match(   &v, SUBJECT,  0,               0));
                ASSERT_SAFE_PASS(X.matchRaw(&v, SUBJECT,  0,               0));
                ASSERT_FAIL(X.match(        &v, SUBJECT,  0,              -1));
                ASSERT_FAIL(X.matchRaw(     &v, SUBJECT,  0,              -1));

                ASSERT_SAFE_PASS(X.match(   &v, SUBJECT,  1,               1));
                ASSERT_SAFE_PASS(X.matchRaw(&v, SUBJECT,  1,               1));
                ASSERT_FAIL(X.match(        &v, SUBJECT,  1,               2));
                ASSERT_FAIL(X.matchRaw(     &v, SUBJECT,  1,               2));
            }

            // 'match' taking 'bsl::vector<bsl::string_view>'
            {
                bsl::vector<bsl::string_view> v, *zv = 0; (void)zv;

                ASSERT_SAFE_PASS(X.match(   &v, string_view(SUBJECT,  9),  1));
                ASSERT_SAFE_PASS(X.matchRaw(&v, string_view(SUBJECT,  9),  1));
                ASSERT_FAIL(X.match(        zv, string_view(SUBJECT,  9),  1));
                ASSERT_FAIL(X.matchRaw(     zv, string_view(SUBJECT,  9),  1));

                ASSERT_SAFE_PASS(X.match(   &v, string_view(SUBJECT,  0),  0));
                ASSERT_SAFE_PASS(X.matchRaw(&v, string_view(SUBJECT,  0),  0));
                ASSERT_FAIL(X.match(        &v, string_view(SUBJECT,  0), -1));
                ASSERT_FAIL(X.matchRaw(     &v, string_view(SUBJECT,  0), -1));

                ASSERT_SAFE_PASS(X.match(   &v, string_view(SUBJECT,  1),  1));
                ASSERT_SAFE_PASS(X.matchRaw(&v, string_view(SUBJECT,  1),  1));
                ASSERT_FAIL(X.match(        &v, string_view(SUBJECT,  1),  2));
                ASSERT_FAIL(X.matchRaw(     &v, string_view(SUBJECT,  1),  2));
            }

            // 'match' taking 'std::vector<bsl::string_view>'
            {
                std::vector<bsl::string_view> v, *zv = 0; (void)zv;

                ASSERT_SAFE_PASS(X.match(   &v, string_view(SUBJECT,  9),  1));
                ASSERT_SAFE_PASS(X.matchRaw(&v, string_view(SUBJECT,  9),  1));
                ASSERT_FAIL(X.match(        zv, string_view(SUBJECT,  9),  1));
                ASSERT_FAIL(X.matchRaw(     zv, string_view(SUBJECT,  9),  1));

                ASSERT_SAFE_PASS(X.match(   &v, string_view(SUBJECT,  0),  0));
                ASSERT_SAFE_PASS(X.matchRaw(&v, string_view(SUBJECT,  0),  0));
                ASSERT_FAIL(X.match(        &v, string_view(SUBJECT,  0), -1));
                ASSERT_FAIL(X.matchRaw(     &v, string_view(SUBJECT,  0), -1));

                ASSERT_SAFE_PASS(X.match(   &v, string_view(SUBJECT,  1),  1));
                ASSERT_SAFE_PASS(X.matchRaw(&v, string_view(SUBJECT,  1),  1));
                ASSERT_FAIL(X.match(        &v, string_view(SUBJECT,  1),  2));
                ASSERT_FAIL(X.matchRaw(     &v, string_view(SUBJECT,  1),  2));
            }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
            // 'match' taking 'std::pmr::vector<bsl::string_view>'
            {
                std::pmr::vector<bsl::string_view> v, *zv = 0; (void)zv;

                ASSERT_SAFE_PASS(X.match(   &v, string_view(SUBJECT,  9),  1));
                ASSERT_SAFE_PASS(X.matchRaw(&v, string_view(SUBJECT,  9),  1));
                ASSERT_FAIL(X.match(        zv, string_view(SUBJECT,  9),  1));
                ASSERT_FAIL(X.matchRaw(     zv, string_view(SUBJECT,  9),  1));

                ASSERT_SAFE_PASS(X.match(   &v, string_view(SUBJECT,  0),  0));
                ASSERT_SAFE_PASS(X.matchRaw(&v, string_view(SUBJECT,  0),  0));
                ASSERT_FAIL(X.match(        &v, string_view(SUBJECT,  0), -1));
                ASSERT_FAIL(X.matchRaw(     &v, string_view(SUBJECT,  0), -1));

                ASSERT_SAFE_PASS(X.match(   &v, string_view(SUBJECT,  1),  1));
                ASSERT_SAFE_PASS(X.matchRaw(&v, string_view(SUBJECT,  1),  1));
                ASSERT_FAIL(X.match(        &v, string_view(SUBJECT,  1),  2));
                ASSERT_FAIL(X.matchRaw(     &v, string_view(SUBJECT,  1),  2));
            }
#endif

            // restore object to unprepared state
            {
                bsl::pair<size_t, size_t>           p;
                bsl::string_view                    s;
                bsl::vector<pair<size_t, size_t> >  vp;
                bsl::vector<bslstl::StringRef>      vsr;
                bsl::vector<bsl::string_view>       vsv;
                std::vector<bsl::string_view>       vsv1;
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                std::pmr::vector<bsl::string_view>  vsv2;
#endif

                mX.clear();

                // basic 'match'
                ASSERT_FAIL(X.match(          string_view(SUBJECT,  9), 1));
                ASSERT_FAIL(X.match(          SUBJECT,  9,              1));
                ASSERT_FAIL(X.matchRaw(       string_view(SUBJECT,  9), 1));
                ASSERT_FAIL(X.matchRaw(       SUBJECT,  9,              1));

                // 'match' taking 'bsl::pair'
                ASSERT_FAIL(X.match(   &p,    SUBJECT,  9,              1));
                ASSERT_FAIL(X.matchRaw(&p,    SUBJECT,  9,              1));

                // 'match' taking 'bsl::string_view'
                ASSERT_FAIL(X.match(   &s,    SUBJECT,  9,              1));
                ASSERT_FAIL(X.match(   &s,    string_view(SUBJECT,  9), 1));
                ASSERT_FAIL(X.matchRaw(&s,    SUBJECT,  9,              1));
                ASSERT_FAIL(X.matchRaw(&s,    string_view(SUBJECT,  9), 1));

                // 'match' taking 'bsl::vector<bsl::pair<size_t, size_t>'
                ASSERT_FAIL(X.match(   &vp,   SUBJECT,  9,              1));
                ASSERT_FAIL(X.matchRaw(&vp,   SUBJECT,  9,              1));

                // 'match' taking 'bsl::vector<bslstl::StringRef>'
                ASSERT_FAIL(X.match(   &vsr,  SUBJECT,  9,              1));
                ASSERT_FAIL(X.matchRaw(&vsr,  SUBJECT,  9,              1));

                // 'match' taking 'bsl::vector<bsl::string_view>'
                ASSERT_FAIL(X.match(   &vsv,  string_view(SUBJECT,  9), 1));
                ASSERT_FAIL(X.matchRaw(&vsv,  string_view(SUBJECT,  9), 1));

                // 'match' taking 'std::vector<bsl::string_view>'
                ASSERT_FAIL(X.match(   &vsv1, string_view(SUBJECT,  9), 1));
                ASSERT_FAIL(X.matchRaw(&vsv1, string_view(SUBJECT,  9), 1));

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                // 'match' taking 'std::pmr::vector<bsl::string_view>'
                ASSERT_FAIL(X.match(   &vsv2, string_view(SUBJECT,  9), 1));
                ASSERT_FAIL(X.matchRaw(&vsv2, string_view(SUBJECT,  9), 1));
#endif
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING MANIPULATORS AND ACCESSORS
        //   We test that the object can prepare and clear regular expression
        //   patterns used for subsequent matching.
        //
        // Concerns:
        //: 1 The object correctly handles valid and invalid patterns.
        //:
        //: 2 The compiled patterns are correctly cleared by either 'clear'
        //:   method or when calling 'prepare' with different pattern.
        //:
        //: 3 The object correctly reports the state of the pattern via
        //:   'isPrepared' accessor.
        //:
        //: 4 The object correctly reports about JIT stack absence via
        //:   'jitStackSize' accessor.
        //:
        //: 5 The 'clear' method places the object in the "unprepared"
        //:   state, regardless of the current pattern state.
        //
        // Plan:
        //: 1 Construct a regular expression object and verify that the object
        //:   after construction is in the "unprepared" state. (C-3)
        //:
        //: 2 Call 'prepare' method with valid and invalid patterns and verify
        //:   that the correct patterns are compiled and the object is put in
        //:   the "prepared" state or corresponding error code is returned.
        //:   (C-2)
        //:
        //: 3 Subsequently call 'prepare' method and verify that on success
        //:   previously compiled pattern is cleared.  For invalid pattern the
        //:   object cleares previously compiled pattern and reports an error.
        //:   (C-2,3)
        //:
        //: 4 Call 'clear' method and verify that the object goes into
        //:   "unprepared" state and the accessors under the test return
        //:   correct values.  (C-5)
        //:
        //: 5 Verify that memory is supplied by the allocator, passed at
        //:   construction.
        //
        // Testing:
        //   void clear();
        //   int prepare(bsl::string*, size_t *, const char *, int, size_t);
        //   bool isPrepared() const;
        //   const bsl::string& pattern() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING MANIPULATORS AND ACCESSORS" << endl
                          << "==================================" << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator ta("test",    veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard guard(&da);

        const char PATTERN1[]        = "(abc)+";
        const char PATTERN2[]        = "(def)+";
        const char INVALID_PATTERN[] = "(abcdef";

        if (verbose) cout << "\nTesting a valid pattern." << endl;
        {
            Obj mX(&ta); const Obj& X = mX;

            ASSERT(false == X.isPrepared());

            bsl::string errorMsg;
            size_t      errorOffset = 0;

            int retCode = mX.prepare(&errorMsg, &errorOffset, PATTERN1, 0, 0);

            ASSERTV(retCode,        0        == retCode);
            ASSERTV(X.isPrepared(), true     == X.isPrepared());
            ASSERTV(X.pattern(),    PATTERN1 == X.pattern());
            ASSERTV(errorOffset,    0        == errorOffset);
            ASSERTV(errorMsg,       ""       == errorMsg);
        }
        ASSERTV(da.numAllocations(), 0 == da.numAllocations());

        if (verbose) cout << "\nTesting an invalid pattern." << endl;
        {
            Obj mX(&ta); const Obj& X = mX;

            ASSERT(false == X.isPrepared());

            // Separate allocator for the error string
            bslma::TestAllocator sa("string", veryVeryVeryVerbose);

            bsl::string errorMsg(&sa);
            size_t      errorOffset = 0;

            int retCode = mX.prepare(&errorMsg,
                                     &errorOffset,
                                     INVALID_PATTERN,
                                     0,
                                     0);

            ASSERTV(retCode,        0               != retCode);
            ASSERTV(X.isPrepared(), false           == X.isPrepared());
            ASSERTV(X.pattern(),    INVALID_PATTERN == X.pattern());
            ASSERTV(errorOffset,    7               == errorOffset);
            ASSERTV(errorMsg,       ""              != errorMsg);
        }
        {
            Obj mX(&ta); const Obj& X = mX;

            ASSERT(false == X.isPrepared());

            // Separate allocator for the error string
            bslma::TestAllocator sa("string", veryVeryVeryVerbose);

            bsl::string errorMsg(&sa);

            int retCode = mX.prepare(&errorMsg, 0, INVALID_PATTERN, 0, 0);

            ASSERTV(retCode,        0               != retCode);
            ASSERTV(X.isPrepared(), false           == X.isPrepared());
            ASSERTV(X.pattern(),    INVALID_PATTERN == X.pattern());
            ASSERTV(errorMsg,       ""              != errorMsg);
        }
        {
            Obj mX(&ta); const Obj& X = mX;

            ASSERT(false == X.isPrepared());

            size_t errorOffset = 0;

            int retCode = mX.prepare(0, &errorOffset, INVALID_PATTERN, 0, 0);

            ASSERTV(retCode,        0               != retCode);
            ASSERTV(X.isPrepared(), false           == X.isPrepared());
            ASSERTV(X.pattern(),    INVALID_PATTERN == X.pattern());
            ASSERTV(errorOffset,    7               == errorOffset);
        }

        if (verbose) cout << "\nTesting sequential 'prepare'." << endl;
        {
            Obj mX(&ta); const Obj& X = mX;

            ASSERT(false == X.isPrepared());

            int retCode = mX.prepare(0, 0, PATTERN1, 0, 0);

            ASSERTV(retCode,        0        == retCode);
            ASSERTV(X.isPrepared(), true     == X.isPrepared());
            ASSERTV(X.pattern(),    PATTERN1 == X.pattern());

            retCode = mX.prepare(0, 0, PATTERN2, 0, 0);

            ASSERTV(retCode,        0        == retCode);
            ASSERTV(X.isPrepared(), true     == X.isPrepared());
            ASSERTV(X.pattern(),    PATTERN2 == X.pattern());

            retCode = mX.prepare(0, 0, INVALID_PATTERN, 0, 0);

            ASSERTV(retCode,        0                != retCode);
            ASSERTV(X.isPrepared(), false            == X.isPrepared());
            ASSERTV(X.pattern(),    INVALID_PATTERN  == X.pattern());
        }

        if (verbose) cout << "\nTesting 'clear'." << endl;
        {
            Obj mX(&ta); const Obj& X = mX;

            ASSERT(false == X.isPrepared());

            mX.clear();

            ASSERTV(X.isPrepared(), false == X.isPrepared());
            ASSERTV(X.pattern(),    ""    == X.pattern());

            int retCode = mX.prepare(0, 0, PATTERN1, 0, 0);

            ASSERTV(retCode,        0        == retCode);
            ASSERTV(X.isPrepared(), true     == X.isPrepared());
            ASSERTV(X.pattern(),    PATTERN1 == X.pattern());

            mX.clear();

            ASSERTV(X.isPrepared(), false == X.isPrepared());
            ASSERTV(X.pattern(),    ""    == X.pattern());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            const int INVALID_FLAG = -1;
            (void) INVALID_FLAG;

            Obj mX(&ta);

            ASSERT_SAFE_PASS(mX.prepare(0, 0, PATTERN1, 0, 0));
            ASSERT_FAIL(mX.prepare(0, 0, 0,        0, 0));

            ASSERT_SAFE_PASS(mX.prepare(0, 0, PATTERN1, 0,               0));
            ASSERT_SAFE_PASS(mX.prepare(0, 0, PATTERN1, Obj::k_FLAG_JIT, 0));
            ASSERT_FAIL(mX.prepare(0, 0, PATTERN1, INVALID_FLAG,    0));
        }

        ASSERTV(da.numAllocations(), 0 == da.numAllocations());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS
        //   This test will verify that the primary manipulators are working as
        //   expected.
        //
        // Concerns:
        //: 1 The objects can be constructed.
        //:
        //: 2 The memory comes from the supplied allocator or from default
        //:   allocator if the allocator is not specified.
        //:
        //: 3 The object destroys all allocated memory at destruction.
        //
        // Plan:
        //: 1 Create several 'DatumError' objects using the value constructors
        //:   and verify that allocator is installed correctly.  (C-1,2)
        //: 2 Let objects go out the scope to verify destructor behavior.
        //:   (C-3)
        //
        // Testing:
        //   RegEx(bslma::Allocator *basicAllocator);
        //   ~RegEx();
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS" << endl
                          << "====================" << endl;

        if (verbose) cout << "\nAllocator installation test." << endl;
        {
            if (verbose) cout << "\tConstructor with default allocator."
                              << endl;
            {
                bslma::TestAllocator da("default", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard guard(&da);

                ASSERT(0 == da.numAllocations());

                Obj        mX;
                const Obj& X = mX;

                ASSERT(false == X.isPrepared());

                ASSERT(0 != da.numAllocations());
                ASSERT(0 != da.numBytesInUse());
            }

            if (verbose) cout
                            << "\tConstructor with explicit default allocator."
                            << endl;
            {
                bslma::TestAllocator da("default", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard guard(&da);

                ASSERT(0 == da.numAllocations());

                Obj        mX(static_cast<bslma::Allocator *>(0));
                const Obj& X = mX;

                ASSERT(false == X.isPrepared());

                ASSERT(0 != da.numAllocations());
                ASSERT(0 != da.numBytesInUse());
            }

            if (verbose) cout << "\tConstructor with object allocator."
                              << endl;
            {
                bslma::TestAllocator da("default", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard guard(&da);
                ASSERT(0 == da.numAllocations());

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                ASSERT(0 == oa.numAllocations());

                Obj        mX(&oa);
                const Obj& X = mX;

                ASSERT(false == X.isPrepared());

                ASSERT(0 == da.numAllocations());
                ASSERT(0 == da.numBytesInUse());
                ASSERT(0 != oa.numAllocations());
                ASSERT(0 != oa.numBytesInUse());
            }
        }
        if (verbose) cout << "\nDestructor test." << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                Obj        mX(&oa);
                const Obj& X = mX;

                ASSERT(false == X.isPrepared());
                ASSERT(0 != oa.numAllocations());
                ASSERT(0 != oa.numBytesInUse());
            }
            ASSERT(0 == oa.numBytesInUse());
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
        //: 1 Create a regular expression object.  (C-1)
        //:
        //: 2 Verify that it is in the unprepared state.  (C-1)
        //:
        //: 3 Prepare the object with a pattern.  (C-1)
        //:
        //: 4 Verify that it is in the prepared state and that all the basic
        //:   accessors return the correct values.  (C-1)
        //:
        //: 5 Verify that the regular and the vector match routines work
        //:   correctly, with a 0 start position and also a non-zero start
        //:   position.  (C-1)
        //:
        //: 6 Clear the object to free its resources.  (C-1)
        //:
        //: 7 Verify that the object has gone back to the unprepared state.
        //:   (C-1)
        //
        // Testing:
        //   BREATING TEST
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bslma::TestAllocator ta(veryVeryVeryVerbose);

        Obj mX(&ta); const Obj& X = mX;

        if (verbose) cout << "\nVerifying unprepared state." << endl;

        ASSERT(false == X.isPrepared());

        if (verbose) cout << "\nPreparing the regular expression." << endl;

        bsl::string errorMsg;
        size_t      errorOffset;

        const char PATTERN[]         = "(?P<pkgName>[a-z]+)_([A-Za-z]+)";
        const char SUBPATTERN_NAME[] = "pkgName";

        if (veryVerbose) {
            T_ P_(PATTERN) P(SUBPATTERN_NAME)
        }

        int retCode = mX.prepare(&errorMsg,
                                 &errorOffset,
                                 PATTERN,
                                 Obj::k_FLAG_MULTILINE);

        ASSERTV(errorMsg, errorOffset, 0 == retCode);

        if (verbose) cout << "\nVerifying prepared state." << endl;

        ASSERT(true                  == X.isPrepared());
        ASSERT(Obj::k_FLAG_MULTILINE == X.flags());
        ASSERT(2                     == X.numSubpatterns());
        ASSERT(PATTERN               == X.pattern());
        ASSERT(1                     == X.subpatternIndex(SUBPATTERN_NAME));

        if (verbose) {
            cout << "\nVerifying regular match routine with 0 start "
                 << "position." << endl;
        }

        const char   TEST_STRING[]   = "bbasm_SecurityCache\n"
                                       "tweut_StringRef\n";
        const size_t TEST_STRING_LEN = sizeof(TEST_STRING) - 1;

        pair<size_t, size_t> match;

        {
            retCode = X.match(&match, TEST_STRING, TEST_STRING_LEN);
            ASSERT(0 == retCode);

            const char EXPECTED_MATCH[] = "bbasm_SecurityCache";

            string realMatch(&TEST_STRING[match.first], match.second);

            ASSERTV(realMatch, EXPECTED_MATCH == realMatch);
        }

        if (verbose) {
            cout << "\nVerifying regular match routine with non-zero start "
                 << "position." << endl;
        }

        {
            const size_t startPosition = match.second;

            retCode = X.match(&match,
                              TEST_STRING,
                              TEST_STRING_LEN,
                              startPosition);

            ASSERT(0 == retCode);

            const char EXPECTED_MATCH[] = "tweut_StringRef";

            string realMatch(&TEST_STRING[match.first], match.second);

            ASSERTV(realMatch, EXPECTED_MATCH == realMatch);
        }

        if (verbose) {
            cout << "\nVerifying vector match routine with 0 start "
                 << "position." << endl;
        }

        vector<pair<size_t, size_t> > vMatch;

        {
            retCode = X.match(&vMatch, TEST_STRING, TEST_STRING_LEN);

            ASSERT(0 == retCode);

            const int  LEN = 3;
            const char EXPECTED_MATCHES[LEN][30] = { "bbasm_SecurityCache",
                                                     "bbasm",
                                                     "SecurityCache" };

            ASSERTV(vMatch.size(), LEN == (int)vMatch.size());

            for (int i = 0; i < LEN; ++i) {
                string realMatch(&TEST_STRING[vMatch[i].first],
                                 vMatch[i].second);

                ASSERTV(i, realMatch, EXPECTED_MATCHES[i] == realMatch);
            }
        }

        if (verbose) {
            cout << "\nVerifying vector match routine with non-zero start "
                 << "position." << endl;
        }

        {
            const size_t startPosition = vMatch[0].second;

            retCode = X.match(&vMatch,
                              TEST_STRING,
                              TEST_STRING_LEN,
                              startPosition);
            ASSERT(0 == retCode);

            const int  LEN = 3;
            const char EXPECTED_MATCHES[LEN][30] = { "tweut_StringRef",
                                                     "tweut",
                                                     "StringRef" };

            ASSERTV(vMatch.size(), LEN == vMatch.size());

            for (int i = 0; i < LEN; ++i) {
                string realMatch(&TEST_STRING[vMatch[i].first],
                                 vMatch[i].second);

                ASSERTV(i, realMatch, EXPECTED_MATCHES[i] == realMatch);
            }
        }

        if (verbose) cout << "\nClearing regular expression." << endl;

        mX.clear();

        if (verbose) cout << "\nVerifying unprepared state." << endl;

        ASSERT(false == X.isPrepared());

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST 1
        //
        // Concerns:
        //: 1 JIT compiling optimization speed up pattern matching.
        //:
        //: 2 JIT compiling optimization slow down pattern preparation.
        //
        // Plan:
        //: 1 Using 'bsls_stopwatch' measure the run time of the 'match' method
        //:   with and without JIT compiling support.  Compare the results and
        //:   verify that 'match' with JIT support is faster.  (C-1)
        //:
        //: 2 Using 'bsls_stopwatch' measure the run time of the 'prepare'
        //:   method with and without JIT compiling support.  Compare the
        //:   results and verify that 'prepare' with JIT support is slower.
        //:   (C-2)
        //
        // Testing:
        //  PERFORMANCE TEST 1
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "PERFORMANCE TEST 1" << endl
                          << "==================" << endl;

        const char *SIMPLE_PATTERN     = "X(abc)*Z";
        const char *EMAIL_PATTERN      = "[A-Za-z0-9._-]+@[[A-Za-z0-9.-]+";
        const char *IP_ADDRESS_PATTERN = "(?:[0-9]{1,3}\\.){3}[0-9]{1,3}";

        static const struct {
            int         d_lineNum;      // source line number
            const char *d_pattern;      // pattern string
            const char *d_subject;      // subject string
        } DATA[] = {
            //line  pattern              subject
            //----  -------              -------
            { L_,   SIMPLE_PATTERN,      "XXXabcabcZZZ"              },
            { L_,   EMAIL_PATTERN,       "john.dow@bloomberg.net" },
            { L_,   IP_ADDRESS_PATTERN,  "255.255.255.255"        },
        };

        const size_t NUM_DATA = sizeof DATA / sizeof *DATA;
        const int    NUM_MATCHES = 100000;
        bsl::string  errorMsg;
        size_t       errorOffset;

        for (size_t i = 0; i < NUM_DATA; ++i) {
            const int     LINE        = DATA[i].d_lineNum;
            const char   *PATTERN     = DATA[i].d_pattern;
            const char   *SUBJECT     = DATA[i].d_subject;
            const size_t  SUBJECT_LEN = strlen(SUBJECT);

            bsls::Stopwatch timer;
            Obj             mX;
            const Obj&      X = mX;

            if (verbose) {
                cout << "\nTesting '" << PATTERN << "'. pattern" << endl;
            }

            // Testing 'match' without JIT compilation support.

            int retCode = mX.prepare(&errorMsg,
                                     &errorOffset,
                                     PATTERN,
                                     0,
                                     0);
            ASSERTV(LINE, errorMsg, errorOffset, 0 == retCode);


            Obj         mY;
            const Obj&  Y = mY;

            {
                PrepareJob job(&mY, PATTERN, 0, 0);

                bslmt::ThreadUtil::Handle handle;

                int rc = bslmt::ThreadUtil::create(&handle,
                                                   testPrepareFunction,
                                                   &job);
                ASSERTV(rc, 0 == rc);

                rc = bslmt::ThreadUtil::join(handle);
                ASSERTV(rc, 0 == rc);
            }

            pair<size_t, size_t> result;
            X.match(&result, SUBJECT, SUBJECT_LEN, 0);

            if (veryVeryVerbose) {
                P_(PATTERN) P_(SUBJECT) P_(result.first) P(result.second);
            }

            ASSERTV(LINE, 0 == X.match(SUBJECT, SUBJECT_LEN, 0));

            timer.start();
            for (int i = 0; i < NUM_MATCHES; ++i) {
               X.match(SUBJECT, SUBJECT_LEN, 0);
            }
            timer.stop();
            double matchTimeX = timer.elapsedTime();

            timer.reset();
            timer.start();
            for (int i = 0; i < NUM_MATCHES; ++i) {
               Y.match(SUBJECT, SUBJECT_LEN, 0);
            }
            timer.stop();
            double matchTimeY = timer.elapsedTime();

            timer.reset();
            mX.clear();
            mY.clear();

            // Testing 'prepare' without JIT compilation support.

            timer.start();
            for (int i = 0; i < NUM_MATCHES; ++i) {
                retCode = mX.prepare(&errorMsg,
                                     &errorOffset,
                                     PATTERN,
                                     0,
                                     0);
                ASSERTV(LINE, errorMsg, errorOffset, 0 == retCode);

                ASSERTV(LINE, 0 == X.match(SUBJECT, SUBJECT_LEN, 0));
            }
            timer.stop();
            double prepareTime = timer.elapsedTime();

            timer.reset();
            mX.clear();

            // Testing 'match' with JIT compilation support.

            retCode = mX.prepare(&errorMsg,
                                 &errorOffset,
                                 PATTERN,
                                 Obj::k_FLAG_JIT,
                                 0);
            ASSERTV(LINE, errorMsg, errorOffset, 0 == retCode);
            ASSERTV(LINE, 0 == X.match(SUBJECT, SUBJECT_LEN, 0));

            {
                PrepareJob job(&mY, PATTERN, Obj::k_FLAG_JIT, 0);

                bslmt::ThreadUtil::Handle handle;

                int rc = bslmt::ThreadUtil::create(&handle,
                                                   testPrepareFunction,
                                                   &job);
                ASSERTV(rc, 0 == rc);

                rc = bslmt::ThreadUtil::join(handle);
                ASSERTV(rc, 0 == rc);
            }

            timer.start();
            for (int i = 0; i < NUM_MATCHES; ++i) {
               X.match(SUBJECT, SUBJECT_LEN, 0);
            }
            timer.stop();
            double matchJitTimeX = timer.elapsedTime();

            timer.reset();
            timer.start();
            for (int i = 0; i < NUM_MATCHES; ++i) {
               Y.match(SUBJECT, SUBJECT_LEN, 0);
            }
            timer.stop();
            double matchJitTimeY = timer.elapsedTime();

            timer.reset();
            mX.clear();
            mY.clear();

            // Testing 'prepare' with JIT compilation support.

            timer.start();
            for (int i = 0; i < NUM_MATCHES; ++i) {
                retCode = mX.prepare(&errorMsg,
                                     &errorOffset,
                                     PATTERN,
                                     Obj::k_FLAG_JIT,
                                     0);
                ASSERTV(LINE, errorMsg, errorOffset, 0 == retCode);

                ASSERTV(LINE, 0 == X.match(SUBJECT, SUBJECT_LEN, 0));
            }
            timer.stop();
            double prepareJitTime = timer.elapsedTime();

            if (veryVerbose) {
                cout << "\tResults:" << endl
                     << "\t\t'match'          time: " << matchTimeX << endl
                     << "\t\t'match'(JIT)     time: " << matchJitTimeX << endl
                     << endl
                     << "\t\t'match'(mt)      time: " << matchTimeY << endl
                     << "\t\t'match'(mt; JIT) time: " << matchJitTimeY << endl
                     << endl
                     << "\t\t'prepare'        time: " << prepareTime << endl
                     << "\t\t'prepare'(JIT)   time: " << prepareJitTime
                     << endl;
            }

            ASSERTV(LINE,
                    matchTimeX,
                    matchJitTimeX,
                    matchTimeX > matchJitTimeX);
            ASSERTV(LINE,
                    prepareTime,
                    prepareJitTime,
                    prepareTime < prepareJitTime);
        }
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST 2
        //
        // Concerns:
        //: Bypassing UTF staring validity check speed up pattern matching.
        //
        // Plan:
        //: 1 Using 'bsls_stopwatch' measure the run time of the 'match' method
        //:   with and without UTF string validity check.  Compare the results
        //:   and verify that 'match' without UTF string validity check is
        //:   faster.
        //
        // Testing:
        //  PERFORMANCE TEST 2
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "PERFORMANCE TEST 2" << endl
                          << "==================" << endl;

        const int       NUM_MATCHES = 10000;
        const int       SUBJECT_LENGTH = 10000;
        bsls::Stopwatch timer;

        bsl::string     errorMsg;
        size_t          errorOffset;

        const char PATTERN[] = "[abc]+";
        char       SUBJECT[SUBJECT_LENGTH];
        for (int i = 0; i < SUBJECT_LENGTH; ++i) {
            SUBJECT[i] = 'a';
        }
        SUBJECT[1] = 'd';

        Obj        mX;
        const Obj& X = mX;

        if (verbose) {
            cout << "Testing 'match' with UTF string validity check."
                 << endl;
        }

        int retCode = mX.prepare(&errorMsg,
                                 &errorOffset,
                                 PATTERN,
                                 Obj::k_FLAG_UTF8,
                                 0);
        ASSERTV(errorMsg, errorOffset, 0 == retCode);
        ASSERT(0 == X.match(SUBJECT, SUBJECT_LENGTH, 0));
        timer.start();
        for (int i = 0; i < NUM_MATCHES; ++i) {
           X.match(SUBJECT, SUBJECT_LENGTH, 0);
        }
        timer.stop();
        double matchTime = timer.elapsedTime();

        timer.reset();

        if (verbose) {
            cout << "Testing 'match' without UTF string validity check."
                 << endl;
        }

        ASSERT(0 == X.matchRaw(SUBJECT, SUBJECT_LENGTH, 0));
        timer.start();
        for (int i = 0; i < NUM_MATCHES; ++i) {
           X.matchRaw(SUBJECT, SUBJECT_LENGTH, 0);
        }
        timer.stop();
        double matchRawTime = timer.elapsedTime();

        if (veryVeryVerbose) {
            cout << "\nResults:" << endl
                 << "\t'match'    time: " << matchTime << endl
                 << "\t'matchRaw' time: " << matchRawTime << endl;
        }

        ASSERTV(matchTime, matchRawTime, matchTime > matchRawTime);
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
// Copyright 2016 Bloomberg Finance L.P.
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
