// bslmf_arraytopointer.t.cpp                                         -*-C++-*-

#include <bslmf_arraytopointer.h>
#include <bslmf_issame.h>

#include <bsls_bsltestutil.h>
#include <bsls_platform.h>            // for testing only

#include <stdio.h>      // print, fprintf
#include <stdlib.h>     // atoi
#include <typeinfo>

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
//                                  TBD
//-----------------------------------------------------------------------------
// [ 1] bslmf::ArrayToPointer
// [ 2] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

#define ASSERT_SAME(X,Y) { ASSERT((1==bsl::is_same<X,Y>::value)); }

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum Enum {};

struct Struct {
    int a[4];
};

union Union {
    int  i;
    char a[sizeof(int)];
};

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

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    setbuf(stdout, NULL);       // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example illustrating use of 'bsl::is_array'.
        //
        // Concerns:
        //
        // Plan:
        //
        // Tactics:
        //   - Add-Hoc Data Selection Method
        //   - Brute-Force implementation technique
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE\n"
                            "\n=============\n");

///Usage
///-----
// For example:
//..
        ASSERT(1 == (bsl::is_same<bslmf::ArrayToPointer<int[5]>::Type
                                 , int*>::value));
        ASSERT(1 == (bsl::is_same<bslmf::ArrayToPointer<int*>::Type
                                 , int*>::value));
        ASSERT(1 == (bsl::is_same<bslmf::ArrayToPointer<int(*)[5]>::Type
                                 , int(*)[5]>::value));
        P(typeid(bslmf::ArrayToPointer<int(*)[5]>::Type).name());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate 'bslmf::ArrayToConstPointer' with various types and
        //   verify that their 'Type' member aliases the correctly const-
        //   qualified decayed type.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'bslmf::ArrayToConstPointer'\n"
                            "\n====================================\n");

        ASSERT_SAME(bslmf::ArrayToPointer<char const    [1]>::Type,
                                          char const     *       );

        ASSERT_SAME(bslmf::ArrayToPointer<char       (&)[1]>::Type,
                                          char           *       );

        ASSERT_SAME(bslmf::ArrayToPointer<char             >::Type,
                                          char                   );

        ASSERT_SAME(bslmf::ArrayToPointer<char const       >::Type,
                                          char const             );

        ASSERT_SAME(bslmf::ArrayToPointer<char           * >::Type,
                                          char           *       );

        ASSERT_SAME(bslmf::ArrayToPointer<void       *   [2]>::Type,
                                          void       *    *       );
        ASSERT_SAME(bslmf::ArrayToPointer<void const *   [2]>::Type,
                                          void const *    *       );
        ASSERT_SAME(bslmf::ArrayToPointer<void       *(&)[2]>::Type,
                                          void       *    *       );
        ASSERT_SAME(bslmf::ArrayToPointer<void       *      >::Type,
                                          void       *            );
        ASSERT_SAME(bslmf::ArrayToPointer<void              >::Type,
                                          void                    );

        ASSERT_SAME(bslmf::ArrayToPointer<int                [3]>::Type,
                                          int                 *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int const          [3]>::Type,
                                          int const           *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int volatile       [3]>::Type,
                                          int volatile        *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int const volatile [3]>::Type,
                                          int const volatile  *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int                   >::Type,
                                          int                         );
        ASSERT_SAME(bslmf::ArrayToPointer<int const             >::Type,
                                          int const                   );
        ASSERT_SAME(bslmf::ArrayToPointer<int volatile          >::Type,
                                          int volatile                );
        ASSERT_SAME(bslmf::ArrayToPointer<int const volatile    >::Type,
                                          int const volatile          );

        ASSERT_SAME(bslmf::ArrayToPointer<int                (&)[4]>::Type,
                                          int                    *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int const          (&)[4]>::Type,
                                          int const              *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int volatile       (&)[4]>::Type,
                                          int volatile           *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int const volatile (&)[4]>::Type,
                                          int const volatile     *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int                 &    >::Type,
                                          int                 &          );
        ASSERT_SAME(bslmf::ArrayToPointer<int const           &    >::Type,
                                          int const           &          );
        ASSERT_SAME(bslmf::ArrayToPointer<int volatile        &    >::Type,
                                          int volatile        &          );
        ASSERT_SAME(bslmf::ArrayToPointer<int const volatile  &    >::Type,
                                          int const volatile  &          );

        ASSERT_SAME(bslmf::ArrayToPointer<int                 * [5]>::Type,
                                          int                 *  *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int const           * [5]>::Type,
                                          int const           *  *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int volatile        * [5]>::Type,
                                          int volatile        *  *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int const volatile  * [5]>::Type,
                                          int const volatile  *  *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int                (*)[5]>::Type,
                                          int                (*)[5]      );
        ASSERT_SAME(bslmf::ArrayToPointer<int const          (*)[5]>::Type,
                                          int const          (*)[5]      );
        ASSERT_SAME(bslmf::ArrayToPointer<int volatile       (*)[5]>::Type,
                                          int volatile       (*)[5]      );
        ASSERT_SAME(bslmf::ArrayToPointer<int const volatile (*)[5]>::Type,
                                          int const volatile (*)[5]      );
        ASSERT_SAME(bslmf::ArrayToPointer<int                 *    >::Type,
                                          int                 *          );
        ASSERT_SAME(bslmf::ArrayToPointer<int const           *    >::Type,
                                          int const           *          );
        ASSERT_SAME(bslmf::ArrayToPointer<int volatile        *    >::Type,
                                          int volatile        *          );
        ASSERT_SAME(bslmf::ArrayToPointer<int const volatile  *    >::Type,
                                          int const volatile  *          );

        ASSERT_SAME(bslmf::ArrayToPointer<int         [6][6]>::Type,
                                          int      (*)[6]         );
        ASSERT_SAME(bslmf::ArrayToPointer<int const   [6][6]>::Type,
                                          int const(*)[6]         );
        ASSERT_SAME(bslmf::ArrayToPointer<int      (&)[6][6]>::Type,
                                          int      (*)[6]         );
        ASSERT_SAME(bslmf::ArrayToPointer<int       * [6][6]>::Type,
                                          int    * (*)[6]         );
        ASSERT_SAME(bslmf::ArrayToPointer<int      (*)[6][6]>::Type,
                                          int      (*)[6][6]      );

        ASSERT_SAME(bslmf::ArrayToPointer<int *const    [6][6]>::Type,
                                          int *const (*)[6]         );
        ASSERT_SAME(bslmf::ArrayToPointer<int *const (&)[6][6]>::Type,
                                          int *const (*)[6]         );
        ASSERT_SAME(bslmf::ArrayToPointer<int *const (*)[6][6]>::Type,
                                          int *const (*)[6][6]      );

        ASSERT_SAME(bslmf::ArrayToPointer<void *[]>::Type,
                                          void * *      );

        ASSERT_SAME(bslmf::ArrayToPointer<int                []>::Type,
                                          int                *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int const          []>::Type,
                                          int const          *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int volatile       []>::Type,
                                          int volatile       *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int const volatile []>::Type,
                                          int const volatile *       );

#ifndef BSLS_PLATFORM_CMP_MSVC
        ASSERT_SAME(bslmf::ArrayToPointer<int                (&)[]>::Type,
                                          int                   *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int const          (&)[]>::Type,
                                          int const             *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int volatile       (&)[]>::Type,
                                          int volatile          *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int const volatile (&)[]>::Type,
                                          int const volatile    *       );
#endif

        ASSERT_SAME(bslmf::ArrayToPointer<int                 * []>::Type,
                                          int                 * *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int const           * []>::Type,
                                          int const           * *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int volatile        * []>::Type,
                                          int volatile        * *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int const volatile  * []>::Type,
                                          int const volatile  * *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int                (*)[]>::Type,
                                          int                (*)[]      );
        ASSERT_SAME(bslmf::ArrayToPointer<int const          (*)[]>::Type,
                                          int const          (*)[]      );
        ASSERT_SAME(bslmf::ArrayToPointer<int volatile       (*)[]>::Type,
                                          int volatile       (*)[]      );
        ASSERT_SAME(bslmf::ArrayToPointer<int const volatile (*)[]>::Type,
                                          int const volatile (*)[]      );

        ASSERT_SAME(bslmf::ArrayToPointer<int          [][7]>::Type,
                                          int         (*)[7]      );
        ASSERT_SAME(bslmf::ArrayToPointer<int const    [][7]>::Type,
                                          int const   (*)[7]      );
#ifndef BSLS_PLATFORM_CMP_MSVC
        ASSERT_SAME(bslmf::ArrayToPointer<int       (&)[][7]>::Type,
                                          int       (*)  [7]      );
#endif
        ASSERT_SAME(bslmf::ArrayToPointer<int        * [][7]>::Type,
                                          int        *(*)[7]      );
        ASSERT_SAME(bslmf::ArrayToPointer<int       (*)[][7]>::Type,
                                          int       (*)[][7]      );

        ASSERT_SAME(bslmf::ArrayToPointer<int *const    [][7]>::Type,
                                          int *const   (*)[7]      );
#ifndef BSLS_PLATFORM_CMP_MSVC
        ASSERT_SAME(bslmf::ArrayToPointer<int *const (&)[][7]>::Type,
                                          int *const (*)  [7]      );
#endif
        ASSERT_SAME(bslmf::ArrayToPointer<int *const (*)[][7]>::Type,
                                          int *const (*)[][7]      );

        ASSERT_SAME(bslmf::ArrayToPointer<Enum          [8]>::Type,
                                          Enum           *       );
        ASSERT_SAME(bslmf::ArrayToPointer<Enum       (&)[8]>::Type,
                                          Enum           *       );
        ASSERT_SAME(bslmf::ArrayToPointer<Enum const (&)[8]>::Type,
                                          Enum const     *       );
        ASSERT_SAME(bslmf::ArrayToPointer<Enum             >::Type,
                                          Enum                   );

        ASSERT_SAME(bslmf::ArrayToPointer<Struct    [8]>::Type,
                                          Struct     *       );
        ASSERT_SAME(bslmf::ArrayToPointer<Struct (&)[8]>::Type,
                                          Struct     *       );
        ASSERT_SAME(bslmf::ArrayToPointer<Struct       >::Type,
                                          Struct             );

        ASSERT_SAME(bslmf::ArrayToPointer<Union    [8]>::Type,
                                          Union     *       );
        ASSERT_SAME(bslmf::ArrayToPointer<Union (&)[8]>::Type,
                                          Union     *       );
        ASSERT_SAME(bslmf::ArrayToPointer<Union       >::Type,
                                          Union             );

        ASSERT_SAME(bslmf::ArrayToPointer<int  Struct::*    >::Type,
                                          int  Struct::*          );
        ASSERT_SAME(bslmf::ArrayToPointer<int (Struct::*)[9]>::Type,
                                          int (Struct::*)[9]      );
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate 'bslmf::ArrayToPointer' with various types and verify
        //   that their 'Type' member aliases the correctly decayed type.
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'bslmf::ArrayToPointer'\n"
                            "\n===============================\n");

        ASSERT_SAME(bslmf::ArrayToPointer<const char [6]>::Type, const char*);
        if (verbose) {
            //P(bslmf::ArrayToPointer<const char [6]>::ID);
#if !(defined(BSLS_PLATFORM_CMP_CLANG) &&                                     \
      (BSLS_PLATFORM_CMP_VER_MAJOR < 3 ||                                     \
        (BSLS_PLATFORM_CMP_VER_MAJOR == 3 && BSLS_PLATFORM_CMP_VER_MINOR < 5)))

            // Note that there is a bug in the Clang 3.1 compiler that causes
            // the front end to assert on the following 'typeid' expression:
            P(typeid(const char [6]).name());
#endif
            P(typeid(bslmf::ArrayToPointer<const char [6]>::Type).name());
        }

        ASSERT_SAME(bslmf::ArrayToPointer<char const [1]>::Type,
                    char const                      *);

        ASSERT_SAME(bslmf::ArrayToPointer<char       (&)[1]>::Type,
                    char           *       );

        ASSERT_SAME(bslmf::ArrayToPointer<char >::Type,
                    char                       );

        ASSERT_SAME(bslmf::ArrayToPointer<char const >::Type,
                    char                       const );

        ASSERT_SAME(bslmf::ArrayToPointer<char *>::Type,
                    char                       *);

        ASSERT_SAME(bslmf::ArrayToPointer<void  *[2]>::Type,
                    void                       **);
        ASSERT_SAME(bslmf::ArrayToPointer<void const  *[2]>::Type,
                    void const                       **);
        ASSERT_SAME(bslmf::ArrayToPointer<void       *(&)[2]>::Type,
                    void       *    *       );
        ASSERT_SAME(bslmf::ArrayToPointer<void *>::Type,
                    void                       *);
        ASSERT_SAME(bslmf::ArrayToPointer<void >::Type,
                    void                       );

        ASSERT_SAME(bslmf::ArrayToPointer<int  [3]>::Type,
                    int                       *);
        ASSERT_SAME(bslmf::ArrayToPointer<int const [3]>::Type,
                    int const                      *);
        ASSERT_SAME(bslmf::ArrayToPointer<int volatile  [3]>::Type,
                    int volatile                       *);
        ASSERT_SAME(bslmf::ArrayToPointer<int const volatile  [3]>::Type,
                    int const volatile                       *);
        ASSERT_SAME(bslmf::ArrayToPointer<int >::Type,
                    int                       );
        ASSERT_SAME(bslmf::ArrayToPointer<int const >::Type,
                    int                       const );
        ASSERT_SAME(bslmf::ArrayToPointer<int volatile >::Type,
                    int volatile                       );
        ASSERT_SAME(bslmf::ArrayToPointer<int const volatile >::Type,
                    int const volatile                       );

        ASSERT_SAME(bslmf::ArrayToPointer<int                (&)[4]>::Type,
                    int                    *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int const          (&)[4]>::Type,
                    int const              *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int volatile       (&)[4]>::Type,
                    int volatile           *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int const volatile (&)[4]>::Type,
                    int const volatile     *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int                 &    >::Type,
                    int                 &          );
        ASSERT_SAME(bslmf::ArrayToPointer<int const           &    >::Type,
                    int const           &          );
        ASSERT_SAME(bslmf::ArrayToPointer<int volatile        &    >::Type,
                    int volatile        &          );
        ASSERT_SAME(bslmf::ArrayToPointer<int const volatile  &    >::Type,
                    int const volatile  &          );

        ASSERT_SAME(bslmf::ArrayToPointer<int  *[5]>::Type,
                    int                       **);
        ASSERT_SAME(bslmf::ArrayToPointer<int const  *[5]>::Type,
                    int const                       **);
        ASSERT_SAME(bslmf::ArrayToPointer<int volatile  *[5]>::Type,
                    int volatile                       **);
        ASSERT_SAME(bslmf::ArrayToPointer<int const volatile  *[5]>::Type,
                    int const volatile                       **);
        ASSERT_SAME(bslmf::ArrayToPointer<int                (*)[5]>::Type,
                    int                (*)[5]      );
        ASSERT_SAME(bslmf::ArrayToPointer<int const          (*)[5]>::Type,
                    int const          (*)[5]      );
        ASSERT_SAME(bslmf::ArrayToPointer<int volatile       (*)[5]>::Type,
                    int volatile       (*)[5]      );
        ASSERT_SAME(bslmf::ArrayToPointer<int const volatile (*)[5]>::Type,
                    int const volatile (*)[5]      );
        ASSERT_SAME(bslmf::ArrayToPointer<int *>::Type,
                    int                       *);
        ASSERT_SAME(bslmf::ArrayToPointer<int const *>::Type,
                    int const                       *);
        ASSERT_SAME(bslmf::ArrayToPointer<int volatile *>::Type,
                    int volatile                       *);
        ASSERT_SAME(bslmf::ArrayToPointer<int const volatile *>::Type,
                    int const volatile                       *);

        ASSERT_SAME(bslmf::ArrayToPointer<int         [6][6]>::Type,
                    int      (*)[6]         );
        ASSERT_SAME(bslmf::ArrayToPointer<int   const [6][6]>::Type,
                    int const(*)[6]         );
        ASSERT_SAME(bslmf::ArrayToPointer<int      (&)[6][6]>::Type,
                    int      (*)[6]         );
        ASSERT_SAME(bslmf::ArrayToPointer<int        *[6][6]>::Type,
                    int    * (*)[6]         );
        ASSERT_SAME(bslmf::ArrayToPointer<int      (*)[6][6]>::Type,
                    int      (*)[6][6]      );

        ASSERT_SAME(bslmf::ArrayToPointer<int    *const [6][6]>::Type,
                    int *const (*)[6]         );
        ASSERT_SAME(bslmf::ArrayToPointer<int *const (&)[6][6]>::Type,
                    int *const (*)[6]         );
        ASSERT_SAME(bslmf::ArrayToPointer<int *const (*)[6][6]>::Type,
                    int *const (*)[6][6]      );

        ASSERT_SAME(bslmf::ArrayToPointer<void  *[]>::Type,
                    void                       **);

        ASSERT_SAME(bslmf::ArrayToPointer<int  []>::Type,
                    int                       *);
        ASSERT_SAME(bslmf::ArrayToPointer<int const []>::Type,
                    int const                      *);
        ASSERT_SAME(bslmf::ArrayToPointer<int volatile  []>::Type,
                    int volatile                       *);
        ASSERT_SAME(bslmf::ArrayToPointer<int const volatile  []>::Type,
                    int const volatile                       *);

#ifndef BSLS_PLATFORM_CMP_MSVC
        ASSERT_SAME(bslmf::ArrayToPointer<int                (&)[]>::Type,
                    int                   *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int const          (&)[]>::Type,
                    int const             *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int volatile       (&)[]>::Type,
                    int volatile          *       );
        ASSERT_SAME(bslmf::ArrayToPointer<int const volatile (&)[]>::Type,
                    int const volatile    *       );
#endif

        ASSERT_SAME(bslmf::ArrayToPointer<int  *[]>::Type,
                    int                       **);
        ASSERT_SAME(bslmf::ArrayToPointer<int const  *[]>::Type,
                    int const                       **);
        ASSERT_SAME(bslmf::ArrayToPointer<int volatile  *[]>::Type,
                    int volatile                       **);
        ASSERT_SAME(bslmf::ArrayToPointer<int const volatile  *[]>::Type,
                    int const volatile                       **);
        ASSERT_SAME(bslmf::ArrayToPointer<int                (*)[]>::Type,
                    int                (*)[]      );
        ASSERT_SAME(bslmf::ArrayToPointer<int const          (*)[]>::Type,
                    int const          (*)[]      );
        ASSERT_SAME(bslmf::ArrayToPointer<int volatile       (*)[]>::Type,
                    int volatile       (*)[]      );
        ASSERT_SAME(bslmf::ArrayToPointer<int const volatile (*)[]>::Type,
                    int const volatile (*)[]      );

        ASSERT_SAME(bslmf::ArrayToPointer<int          [][7]>::Type,
                    int         (*)[7]      );
        ASSERT_SAME(bslmf::ArrayToPointer<int    const [][7]>::Type,
                    int const   (*)[7]      );
#ifndef BSLS_PLATFORM_CMP_MSVC
        ASSERT_SAME(bslmf::ArrayToPointer<int       (&)[][7]>::Type,
                    int       (*)  [7]      );
#endif
        ASSERT_SAME(bslmf::ArrayToPointer<int         *[][7]>::Type,
                    int        *(*)[7]      );
        ASSERT_SAME(bslmf::ArrayToPointer<int       (*)[][7]>::Type,
                    int       (*)[][7]      );

        ASSERT_SAME(bslmf::ArrayToPointer<int    *const [][7]>::Type,
                    int *const   (*)[7]      );
#ifndef BSLS_PLATFORM_CMP_MSVC
        ASSERT_SAME(bslmf::ArrayToPointer<int *const (&)[][7]>::Type,
                    int *const (*)  [7]      );
#endif
        ASSERT_SAME(bslmf::ArrayToPointer<int *const (*)[][7]>::Type,
                    int *const (*)[][7]      );

        ASSERT_SAME(bslmf::ArrayToPointer<Enum  [8]>::Type,
                    Enum                       *);
        ASSERT_SAME(bslmf::ArrayToPointer<Enum       (&)[8]>::Type,
                    Enum           *       );
        ASSERT_SAME(bslmf::ArrayToPointer<Enum const (&)[8]>::Type,
                    Enum const     *       );
        ASSERT_SAME(bslmf::ArrayToPointer<Enum >::Type,
                    Enum                       );

        ASSERT_SAME(bslmf::ArrayToPointer<Struct  [8]>::Type,
                    Struct                       *);
        ASSERT_SAME(bslmf::ArrayToPointer<Struct (&)[8]>::Type,
                    Struct     *       );
        ASSERT_SAME(bslmf::ArrayToPointer<Struct >::Type,
                    Struct                       );

        ASSERT_SAME(bslmf::ArrayToPointer<Union  [8]>::Type,
                    Union                       *);
        ASSERT_SAME(bslmf::ArrayToPointer<Union (&)[8]>::Type,
                    Union     *       );
        ASSERT_SAME(bslmf::ArrayToPointer<Union >::Type,
                    Union                       );

        ASSERT_SAME(bslmf::ArrayToPointer<int  Struct:: *>::Type,
                    int  Struct::                       *);
        ASSERT_SAME(bslmf::ArrayToPointer<int (Struct::*)[9]>::Type,
                    int (Struct::*)[9]      );
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

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
