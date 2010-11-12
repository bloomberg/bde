// bdeimp_strhash.t.cpp            -*-C++-*-

#include <bdeimp_strhash.h>

#include <bsls_platformutil.h>
#include <bsls_stopwatch.h>

#include <bsl_cstdlib.h>     // 'bsl::atoi()'
#include <bsl_cstring.h>     // 'bsl::strlen'
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test consists of two 'static' member functions (pure
// procedures) that compute hash values for strings.  The general plan is that
// the functions are tested against a set of tabulated test vectors.  Both
// functions *should* have the same underlying implementation, so tests are
// included to verify that the functions return identical values for equivalent
// inputs.
//-----------------------------------------------------------------------------
// [ 1] static int hash(char *string, int size);     CONCERN: value
// [ 1] static int hash(char *string, length, size); CONCERN:   "
// [ 2] static int hash(char *string, length, size); CONCERN: embedded '\0'
// [ 3] static int hash(char *string, int size);     CONCERN: distribution
// [ 3] static int hash(char *string, length, size); CONCERN:   "
// [-1] static int hash(char *string, int size);     CONCERN: timing
// [-1] static int hash(char *string, length, size); CONCERN:   "
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

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

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef bdeimp_StrHash Obj;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // TESTING HASH FUNCTIONS (DISTRIBUTION)
        //
        // Concerns:
        //   That the hash functions generate a good distribution of return
        //   values over a wide range of 'size' values; specifically, that for
        //   a wide selection of strings, the return values are reasonably
        //   distributed.
        //
        // Plan:
        //   Perform a large number of hashes and verify the distribution.
        //
        // Testing:
        //   static int hash(char *string, int size);     CONCERN: distribution
        //   static int hash(char *string, length, size); CONCERN:   "
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'hash' distribution" << endl
                                  << "===========================" << endl;

        static const char *SPECS[] = {
            "!=",
            "(",
            "(A",
            "(C-style)",
            "(as",
            "(but",
            "(e.g.",
            "(if",
            "(in",
            "(not",
            "(or",
            "(single-line)",
            "(the",
            ")",
            ").",
            "*",
            "*aliasing*",
            "*exception*",
            "*neutral*",
            "*semantic*",
            "*value*",
            "+",
            "-",
            ".",
            "..",
            "0",
            "0.",
            "1",
            "1]",
            ":",
            "<",
            "<=",
            "==",
            "ACCESSORS",
            "All",
            "Append",
            "Assign",
            "C-style",
            "CREATORS",
            "Create",
            "DISABLED",
            "Destroy",
            "Enables",
            "FREE",
            "FRIEND",
            "Finally",
            "First",
            "Hence",
            "If",
            "In",
            "Insert",
            "MANIPULATORS",
            "More",
            "N",
            "Note",
            "Now",
            "O(N)",
            "OPERATORS",
            "Optionally",
            "PRIVATE",
            "Remove",
            "Replace",
            "Return",
            "Specifically:",
            "TYPES",
            "The",
            "Then",
            "This",
            "Two",
            "Use",
            "Write",
            "[0",
            "[cstr()",
            "\0",
            "a",
            "able",
            "above",
            "accessed",
            "accessing",
            "address",
            "alias",
            "aliasing",
            "all",
            "allocated",
            "allocator",
            "allocator.",
            "always",
            "an",
            "and",
            "any",
            "append",
            "are",
            "as",
            "assign",
            "assignment",
            "at",
            "basicAllocator",
            "bba",
            "bdet_Date",
            "bdex",
            "be",
            "becomes",
            "beginning",
            "behavior",
            "below",
            "between",
            "beyond",
            "both",
            "but",
            "by",
            "bytes",
            "can",
            "cannot",
            "capacity",
            "cases.",
            "char",
            "character",
            "character.",
            "characters",
            "characters.",
            "class",
            "class.)",
            "code",
            "comparison",
            "complete",
            "component",
            "concatenate",
            "concatenation",
            "consisting",
            "construction",
            "container",
            "containing",
            "contiguous",
            "continuing",
            "conversion",
            "copy",
            "correctly",
            "create",
            "creation.",
            "creator",
            "criterion).",
            "cstr()",
            "currently",
            "dabb",
            "dd",
            "definition",
            "delete",
            "dependent",
            "description",
            "destination)",
            "destroyed",
            "differ",
            "different",
            "discarded.",
            "do",
            "does",
            "down",
            "dstIndex",
            "dstNumCharacters",
            "due",
            "during",
            "each",
            "effect.",
            "efficiency.",
            "efficient",
            "element",
            "elements",
            "else",
            "empty",
            "end",
            "equal",
            "equality",
            "equivalent",
            "event",
            "exception",
            "excluding",
            "first",
            "following",
            "following:",
            "for",
            "format",
            "found",
            "from",
            "function",
            "gap",
            "generally",
            "global",
            "greater",
            "guarantee",
            "handles",
            "has",
            "have",
            "having",
            "heap",
            "heap.",
            "hold",
            "how",
            "idxB",
            "idxC",
            "if",
            "illustrate",
            "implemented)",
            "implements",
            "implicit",
            "in",
            "in-place",
            "in-place.",
            "including",
            "increasing",
            "index",
            "indicated",
            "initial",
            "initially",
            "input",
            "insert",
            "instance",
            "instances",
            "int",
            "integer",
            "interior",
            "internal",
            "into",
            "invalid",
            "invocation",
            "is",
            "issue",
            "it",
            "iterate",
            "its",
            "itself",
            "itself)",
            "leaked.",
            "least",
            "left",
            "lenA",
            "lenB",
            "lenC",
            "length",
            "length()",
            "length().",
            "length()]",
            "lengths",
            "less",
            "lexicographic",
            "lexicographically",
            "lhs",
            "lhsData",
            "lhsLength",
            "listed",
            "location",
            "logically",
            "long",
            "machine",
            "may",
            "mechanism",
            "memory",
            "memory)",
            "memory.",
            "method",
            "methods.",
            "modifiable",
            "modified",
            "more",
            "necessarily",
            "necessary",
            "neutral",
            "neutral.",
            "new",
            "newLength",
            "newLength.",
            "no",
            "non-modifiable",
            "not",
            "not)",
            "not.",
            "now",
            "null",
            "null-terminated",
            "numCharacters",
            "numPreserve",
            "number",
            "object",
            "object.",
            "occurs",
            "of",
            "on",
            "one",
            "operation",
            "operational",
            "operations",
            "operator==",
            "operators",
            "opposed",
            "optimized",
            "or",
            "order",
            "original",
            "original.length().",
            "ostream",
            "other",
            "otherwise.",
            "output",
            "outside",
            "over",
            "part",
            "past",
            "plus",
            "pointer",
            "populate",
            "position",
            "position.",
            "positions",
            "positions.",
            "possible",
            "pre-existing",
            "precise",
            "preserve",
            "printing",
            "produce",
            "produced)",
            "produced).",
            "produces",
            "promotion",
            "provided",
            "range",
            "read",
            "reasonable",
            "reference",
            "remain",
            "remove",
            "replace",
            "representation",
            "represents",
            "reserved",
            "respective",
            "resulting",
            "return",
            "returned",
            "returned.",
            "rhs",
            "rhsData",
            "rhsLength",
            "rhsLength.",
            "rollback:",
            "routine",
            "s",
            "s.length()",
            "safe",
            "same",
            "same.",
            "serialization.",
            "set",
            "shifted",
            "size",
            "snippets",
            "some",
            "source",
            "space",
            "specified",
            "specify",
            "splice",
            "srcIndex",
            "srcNumCharacters",
            "srcString",
            "srcString.length()",
            "srcString.length().",
            "srcString[0]",
            "standard",
            "starting",
            "state",
            "stdout",
            "storage",
            "store",
            "stored",
            "strcmp",
            "stream",
            "string",
            "string.",
            "strings",
            "strings.",
            "strings:",
            "strlen(cstr())",
            "strlen(lhsData)",
            "strlen(rhsData)",
            "strlen(srcString).",
            "substring",
            "substrings",
            "sufficient",
            "supplied",
            "supply",
            "supported",
            "supports",
            "terminated",
            "terminating",
            "than",
            "than):",
            "that",
            "the",
            "their",
            "them",
            "there",
            "they",
            "this",
            "those",
            "thrown",
            "to",
            "to:",
            "two",
            "undefined",
            "undefined.",
            "unless",
            "up",
            "uppercase",
            "use",
            "used",
            "used.",
            "using",
            "valid",
            "value",
            "values",
            "values.",
            "via",
            "violate",
            "was",
            "well",
            "when",
            "where",
            "whether",
            "which",
            "will",
            "with",
            "word",
            "would",
            "zero",
            0        // null string required as last element
        };

        if (veryVerbose)
            cout << "\tTesting 'hash(const char *value, int size)'." << endl;
        {
            int hit[256], count, i;

            count = 0;
            for (i = 0; SPECS[i]; ++i, ++count);

            for (i = 0; i < 256; ++i) {
                hit[i] = 0;
            }
            for (i = 0; SPECS[i]; ++i) {
                hit[Obj::hash(SPECS[i], 256)]++;
            }

            for (i = 0; i < 256; ++i) {
                LOOP_ASSERT(i, count / (256 * 4) <= hit[i]);
                LOOP_ASSERT(i, 4 * count / 256 + 2 >= hit[i]);
            }
        }
        {
            int hit[65536], count, i;

            count = 0;
            for (i = 0; SPECS[i]; ++i, ++count);

            for (i = 0; i < 65536; ++i) {
                hit[i] = 0;
            }
            for (i = 0; SPECS[i]; ++i) {
                hit[Obj::hash(SPECS[i], 65536)]++;
            }

            for (i = 0; i < 65536; ++i) {
                LOOP_ASSERT(i, count / (65536 * 4) <= hit[i]);
                LOOP_ASSERT(i, 4 * count / 65536 + 2 >= hit[i]);
            }
        }

        if (veryVerbose)
            cout << "\tTesting "
                    "'hash(const char *value, int length, int size)'." << endl;
        {
            int hit[256], count, i;

            count = 0;
            for (i = 0; SPECS[i]; ++i, ++count);

            for (i = 0; i < 256; ++i) {
                hit[i] = 0;
            }

            for (i = 0; SPECS[i]; ++i) {
                hit[Obj::hash(SPECS[i], bsl::strlen(SPECS[i]), 256)]++;
            }

            for (i = 0; i < 256; ++i) {
                LOOP_ASSERT(i, count / (256 * 4) <= hit[i]);
                LOOP_ASSERT(i, 4 * count / 256 + 2 >= hit[i]);
            }
        }
        {
            int hit[65536], count, i;

            count = 0;
            for (i = 0; SPECS[i]; ++i, ++count);

            for (i = 0; i < 65536; ++i) {
                hit[i] = 0;
            }

            for (i = 0; SPECS[i]; ++i) {
                hit[Obj::hash(SPECS[i], bsl::strlen(SPECS[i]), 65536)]++;
            }

            for (i = 0; i < 65536; ++i) {
                LOOP_ASSERT(i, count / (65536 * 4) <= hit[i]);
                LOOP_ASSERT(i, 4 * count / 65536 + 2 >= hit[i]);
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING HASH FUNCTION WITH LENGTH (EMBEDDED NULL)
        //
        // Concerns:
        //   That the hash function taking a 'stringLength' parameter works as
        //   expected on strings that contain embedded null characters.
        //
        // Plan:
        //   Specify a set of test vectors that contain '\0' characters and
        //   verify the return values from the hash function.
        //
        // Testing:
        //   static int hash(char *string, length, size); CONCERN:embedded '\0'
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing 'hash' against strings containing '\\0'" << endl
                 << "==============================================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // string
            int         d_length;   // string length
            int         d_size;     // size of hash table
            int         d_hash;     // expected output
        } DATA[] = {
            // line  string          length  size  exp
            // ----  ----------      ------  ----  ---
            {  L_,   "\0a"           , 2,    256,  133  },
            {  L_,   "a\0a"          , 3,    256,  241  },
            {  L_,   "aa\0a"         , 4,    256,   45  },
            {  L_,   "aaaa\0"        , 5,    256,  239  },
            {  L_,   "a\0aa\0a"      , 6,    256,   38  },
            {  L_,   "aa\0a\0aa"     , 7,    256,  208  },
            {  L_,   "aaa\0\0aaa"    , 8,    256,  114  },
            {  L_,   "a\0aaa\0aaa"   , 9,    256,  196  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (veryVerbose)
            cout << "\tTesting "
                    "'hash(const char *value, int length, int size)'." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_lineNum;
                const char *SPEC   = DATA[ti].d_spec_p;
                const int   LENGTH = DATA[ti].d_length;
                const int   SIZE   = DATA[ti].d_size;
                const int   HASH   = DATA[ti].d_hash;

                const int hash = Obj::hash(SPEC, LENGTH, SIZE);

                if (veryVeryVerbose)
                    cout << SPEC << ", " << SIZE << " ---> " << hash << endl;

                LOOP_ASSERT(LINE, HASH == hash);

                {
                    const int hash = Obj::hash(SPEC, SIZE);
                    LOOP_ASSERT(LINE, HASH != hash);
                }
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING HASH FUNCTIONS (VALUE)
        //
        // Concerns:
        //   That the hash return values are consistent across all platforms
        //   for given inputs.  That the two hash functions return identical
        //   values for identical inputs.
        //
        // Plan:
        //   Specify a set of test vectors and independently verify the return
        //   values from the two hash functions.  The tests implicitly verify
        //   equivalence of the hash values produced by the two functions.
        //
        // Testing:
        //   static int hash(char *string, int size);     CONCERN: value
        //   static int hash(char *string, length, size); CONCERN:   "
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'hash' value" << endl
                                  << "====================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // string
            int         d_size;     // size of hash table
            int         d_hash;     // expected output
        } DATA[] = {
            // line  string                size    exp
            // ----  ----------            ------  -----
            {  L_,   ""                  ,   256,      0  },
            {  L_,   "a"                 ,   256,     76  },
            {  L_,   "aa"                ,   256,    168  },
            {  L_,   "aaa"               ,   256,    148  },
            {  L_,   "aaaa"              ,   256,    208  },
            {  L_,   "aaaaa"             ,   256,     92  },
            {  L_,   "aaaaaa"            ,   256,    120  },
            {  L_,   "aaaaaaa"           ,   256,    164  },
            {  L_,   "aaaaaaaa"          ,   256,     96  },
            {  L_,   "b"                 ,   256,     89  },
            {  L_,   "bb"                ,   256,     94  },
            {  L_,   "bbb"               ,   256,    107  },
            {  L_,   "bbbb"              ,   256,    212  },
            {  L_,   "bbbbb"             ,   256,    157  },
            {  L_,   "bbbbbb"            ,   256,     82  },
            {  L_,   "bbbbbbb"           ,   256,    207  },
            {  L_,   "bbbbbbbb"          ,   256,     40  },
            {  L_,   ""                  , 65536,      0  },
            {  L_,   "a"                 , 65536,  40524  },
            {  L_,   "aa"                , 65536,  59816  },
            {  L_,   "aaa"               , 65536,  59540  },
            {  L_,   "aaaa"              , 65536,  26064  },
            {  L_,   "aaaaa"             , 65536,  41820  },
            {  L_,   "aaaaaa"            , 65536,  35704  },
            {  L_,   "aaaaaaa"           , 65536,  63908  },
            {  L_,   "aaaaaaaa"          , 65536,   8288  },
            {  L_,   "b"                 , 65536,   1113  },
            {  L_,   "bb"                , 65536,  44126  },
            {  L_,   "bbb"               , 65536,  39531  },
            {  L_,   "bbbb"              , 65536,  23508  },
            {  L_,   "bbbbb"             , 65536,   8093  },
            {  L_,   "bbbbbb"            , 65536,  11602  },
            {  L_,   "bbbbbbb"           , 65536,  24271  },
            {  L_,   "bbbbbbbb"          , 65536,  45096  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (veryVerbose)
            cout << "\tTesting 'hash(const char *value, int size)'." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE = DATA[ti].d_lineNum;
                const char *SPEC = DATA[ti].d_spec_p;
                const int   SIZE = DATA[ti].d_size;
                const int   HASH = DATA[ti].d_hash;

                const int hash = Obj::hash(SPEC, SIZE);

                if (veryVeryVerbose)
                    cout << SPEC << ", " << SIZE << " ---> " << hash << endl;

                LOOP_ASSERT(LINE, HASH == hash);
            }
        }

        if (veryVerbose)
            cout << "\tTesting "
                    "'hash(const char *value, int length, int size)'." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE = DATA[ti].d_lineNum;
                const char *SPEC = DATA[ti].d_spec_p;
                const int   SIZE = DATA[ti].d_size;
                const int   HASH = DATA[ti].d_hash;

                const int hash = Obj::hash(SPEC, bsl::strlen(SPEC), SIZE);

                if (veryVeryVerbose)
                    cout << SPEC << ", " << SIZE << " ---> " << hash << endl;

                LOOP_ASSERT(LINE, HASH == hash);
            }
        }

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // TESTING HASH FUNCTIONS (TIMING)
        //
        // Concerns:
        //   Compare the timings to those of the more thorough 'bdeu_hashutil'.
        //
        // Plan:
        //   Perform a large number of hashes and measure the timing by a
        //   'bsls_Stopwatch'.
        //
        // Testing:
        //   static int hash(char *string, int size);     CONCERN: timing
        //   static int hash(char *string, length, size); CONCERN:   "
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'hash' timing" << endl
                                  << "=====================" << endl;

        char mText[] = "Now is the time for all good men to come to "
                       "the aid of their country";
        const char *TEXT = mText;
        const int LENGTH = sizeof mText;

        enum { ITERATIONS = 1000000 };  // 1M

        if (veryVerbose)
            cout << "\tTesting 'hash(const char *value, int size)'." << endl;
        {
            unsigned int value = 0;
            for (int length = 2; length < LENGTH; length *= 2) {
                char c = TEXT[length + 1];
                mText[length + 1] = '\0';

                bsls_Stopwatch timer;
                timer.start();
                for (int i = 0; i < ITERATIONS; ++i) {
                    value += Obj::hash(TEXT, length);
                }
                timer.stop();

                // veryVeryVerbose by default
                cout << "Hashing 1M values (seconds): "
                     << timer.elapsedTime()
                     << "\t(key: " << TEXT
                     << ", size: " << length << ")" << endl;

                mText[length + 1] = c;
            }
        }

        if (veryVerbose)
            cout << "\tTesting "
                    "'hash(const char *value, int length, int size)'." << endl;
        {
            unsigned int value = 0;
            for (int length = 2; length < LENGTH; length *= 2) {
                char c = TEXT[length + 1];
                mText[length + 1] = '\0';

                bsls_Stopwatch timer;
                timer.start();
                for (int i = 0; i < ITERATIONS; ++i) {
                    value += Obj::hash(TEXT, length + 1, length);
                }
                timer.stop();

                // veryVeryVerbose by default
                cout << "Hashing 1M values (seconds): "
                     << timer.elapsedTime()
                     << "\t(key: " << TEXT
                     << ", size: " << length << ")" << endl;

                mText[length + 1] = c;
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
