// bslstl_stringargumentdata.t.cpp                                    -*-C++-*-

#include <bslstl_stringargumentdata.h>

#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test provides an in-core pointer-semantic object.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] bslstl_StringArgumentData();
// [ 2] bslstl_StringArgumentData(const char *begin, const char *end);
// [ 7] bslstl_StringArgumentData(const bslstl_StringArgumentData& other);
//
// MANIPULATORS
// [ 9] operator=(const bslstl_StringArgumentData& other);
//
// ACCESSORS
// [ 3] const_iterator begin() const;
// [ 3] const_iterator end() const;
//
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [10] USAGE EXAMPLE

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
// NOTE: This implementation of LOOP_ASSERT macros must use printf since
//       cout uses new and must not be called during exception testing.

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                    \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                  \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\t"); \
                printf("%s", #L ": "); dbg_print(L); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\t"); \
                printf("%s", #L ": "); dbg_print(L); printf("\t"); \
                printf("%s", #M ": "); dbg_print(M); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X) printf("<| " #X " |>\n");      // Quote identifier literally.
#define P(X) dbg_print(#X " = ", X, "\n");  // Print identifier and value.
#define P_(X) dbg_print(#X " = ", X, ", "); // P(X) without '\n'
#define L_ __LINE__                         // current Line number
#define T_ putchar('\t');                   // Print a tab (w/o newline)

//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Fundamental-type-specific print functions.
inline void dbg_print(bool b) { printf(b ? "true" : "false"); fflush(stdout); }
inline void dbg_print(char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(unsigned char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(signed char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(short val) { printf("%d", (int)val); fflush(stdout); }
inline void dbg_print(unsigned short val) {
    printf("%d", (int)val); fflush(stdout);
}
inline void dbg_print(int val) { printf("%d", val); fflush(stdout); }
inline void dbg_print(unsigned int val) { printf("%u", val); fflush(stdout); }
inline void dbg_print(long val) { printf("%ld", val); fflush(stdout); }
inline void dbg_print(unsigned long val) {
    printf("%lu", val); fflush(stdout);
}
inline void dbg_print(long long val) { printf("%lld", val); fflush(stdout); }
inline void dbg_print(unsigned long long val) {
    printf("%llu", val); fflush(stdout);
}
inline void dbg_print(float val) {
    printf("'%f'", (double)val); fflush(stdout);
}
inline void dbg_print(double val) { printf("'%f'", val); fflush(stdout); }
inline void dbg_print(long double val) {
    printf("'%Lf'", val); fflush(stdout);
}
inline void dbg_print(const char* s) { printf("\"%s\"", s); fflush(stdout); }
inline void dbg_print(char* s) { printf("\"%s\"", s); fflush(stdout); }
inline void dbg_print(void* p) { printf("%p", p); fflush(stdout); }

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

size_t computeHash(bslstl_StringArgumentData<char> str)
{
    size_t hash = 3069134613U;

    for (const char *p = str.begin(); p != str.end(); ++p)
        hash = (hash << 5) ^ (hash >> 27) ^ *p;

    return hash;
}

//=============================================================================
//                             MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 10: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Usage Example"
                            "\n=====================\n");

        const char str[] = "C string";
        size_t hash = computeHash(bslstl_StringArgumentData<char>(
                                                      str, str + sizeof(str)));
        ASSERT(hash == 3354902561U);

        printf("%u\n", hash);
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        //
        // Testing:
        //   operator=(const bslstl_StringArgumentData& other);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting the copy-assignment operator"
                            "\n====================================\n");

        static const struct {
            int         d_line;
            const char *d_str;
        } DATA[] = {
            //line string
            //---- ------

            { L_,  ""                           },
            { L_,  "a"                          },
            { L_,  "aa"                         },
            { L_,  "aaa"                        },
            { L_,  "aaaa"                       },
            { L_,  "aaaaa"                      },
            { L_,  "aaaaaa"                     },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i != NUM_DATA; ++i) {
            const int LINE = DATA[i].d_line;
            const char *STR = DATA[i].d_str;

            bslstl_StringArgumentData<char> X(STR, STR + strlen(STR));
            bslstl_StringArgumentData<char> Y;

            Y = X;

            LOOP_ASSERT(LINE, Y.begin() == STR);
            LOOP_ASSERT(LINE, Y.end() - Y.begin() == strlen(STR));
            LOOP_ASSERT(LINE, *Y.end() == '\0');
        }
      } break;
      case 8: {
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //
        // Testing:
        //   bslstl_StringArgumentData(const bslstl_StringArgumentData& other);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting the copy constructor"
                            "\n============================\n");

        static const struct {
            int         d_line;
            const char *d_str;
        } DATA[] = {
            //line string
            //---- ------

            { L_,  ""                           },
            { L_,  "a"                          },
            { L_,  "aa"                         },
            { L_,  "aaa"                        },
            { L_,  "aaaa"                       },
            { L_,  "aaaaa"                      },
            { L_,  "aaaaaa"                     },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i != NUM_DATA; ++i) {
            const int LINE = DATA[i].d_line;
            const char *STR = DATA[i].d_str;

            bslstl_StringArgumentData<char> X(STR, STR + strlen(STR));
            bslstl_StringArgumentData<char> Y(X);

            LOOP_ASSERT(LINE, Y.begin() == STR);
            LOOP_ASSERT(LINE, Y.end() - Y.begin() == strlen(STR));
            LOOP_ASSERT(LINE, *Y.end() == '\0');
        }
      } break;
      case 6: {
      } break;
      case 5: {
      } break;
      case 4: {
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING DIRECT ACCESSORS:
        //
        // Testing:
        //   const_iterator begin() const;
        //   const_iterator end() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting direct accessors"
                            "\n========================\n");

        static const struct {
            int         d_line;
            const char *d_str;
        } DATA[] = {
            //line string
            //---- ------

            { L_,  ""                           },
            { L_,  "a"                          },
            { L_,  "aa"                         },
            { L_,  "aaa"                        },
            { L_,  "aaaa"                       },
            { L_,  "aaaaa"                      },
            { L_,  "aaaaaa"                     },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i != NUM_DATA; ++i) {
            const int LINE = DATA[i].d_line;
            const char *STR = DATA[i].d_str;

            bslstl_StringArgumentData<char> Y(STR, STR + strlen(STR));
            LOOP_ASSERT(LINE, Y.begin() == STR);
            LOOP_ASSERT(LINE, Y.end() - Y.begin() == strlen(STR));
            LOOP_ASSERT(LINE, *Y.end() == '\0');
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BASIC MANIPULATORS:
        //
        // Testing:
        //   bslstl_StringArgumentData();
        //   bslstl_StringArgumentData(const char *begin, const char *end);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting basic manupulators"
                            "\n==========================\n");

        if (veryVerbose) printf("\tTesting the default constructor\n");

        bslstl_StringArgumentData<char> X;
        ASSERT(X.begin() == 0);
        ASSERT(X.end() == 0);

        if (veryVerbose) printf("\tTesting the constructor with parameters\n");

        static const struct {
            int         d_line;
            const char *d_str;
        } DATA[] = {
            //line string
            //---- ------

            { L_,  ""                           },
            { L_,  "a"                          },
            { L_,  "aa"                         },
            { L_,  "aaa"                        },
            { L_,  "aaaa"                       },
            { L_,  "aaaaa"                      },
            { L_,  "aaaaaa"                     },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i != NUM_DATA; ++i) {
            const int LINE = DATA[i].d_line;
            const char *STR = DATA[i].d_str;

            bslstl_StringArgumentData<char> Y(STR, STR + strlen(STR));
            LOOP_ASSERT(LINE, Y.begin() == STR);
            LOOP_ASSERT(LINE, Y.end() - Y.begin() == strlen(STR));
            LOOP_ASSERT(LINE, *Y.end() == '\0');
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   We want to makes sure that the basic functionality of
        //   'bslstl_StringArgumentData' works.
        //
        // Testing:
        //   This "test" *exercises* basic functionality.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bslstl_StringArgumentData<char> X;
        ASSERT(X.begin() == 0);
        ASSERT(X.end() == 0);

        const char *str = "abced";
        bslstl_StringArgumentData<char> Y(str, str + strlen(str));

        ASSERT(Y.begin() != Y.end());
        ASSERT(Y.begin() == str);
        ASSERT(Y.end() == str + strlen(str));
      } break;

      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}
