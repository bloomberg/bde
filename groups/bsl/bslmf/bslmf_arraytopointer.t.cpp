// bslmf_arraytopointer.t.cpp      -*-C++-*-

#include <bslmf_arraytopointer.h>
#include <bslmf_issame.h>

#include <bsls_platform.h>            // for testing only

#include <cstdlib>    // atoi()
#include <iostream>
#include <typeinfo>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// TBD
//-----------------------------------------------------------------------------
// [ 1] bslmf_ArrayToPointer
// [ 2] USAGE EXAMPLE
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
#define ASSERT_SAME(X,Y) { ASSERT((1==bslmf_IsSame<X,Y>::VALUE)); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }
//=============================================================================
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_() cout << '\t' << flush;           // Print tab w/o linefeed.

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
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example illustrating use of 'bslmf_IsArray'.
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

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

///Usage
///-----
// For example:
//..
        ASSERT(1 == (bslmf_IsSame<bslmf_ArrayToPointer<int[5]>::Type,
                                                       int*>::VALUE));
        ASSERT(1 == (bslmf_IsSame<bslmf_ArrayToPointer<int*>::Type,
                                                       int*>::VALUE));
        ASSERT(1 == (bslmf_IsSame<bslmf_ArrayToPointer<int(*)[5]>::Type,
                                                       int(*)[5]>::VALUE));
        P(typeid(bslmf_ArrayToPointer<int(*)[5]>::Type).name());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate 'bslmf_ArrayToConstPointer' with various types and
        //   verify that their 'Type' member aliases the correctly const-
        //   qualified decayed type.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "bslmf_ArrayToConstPointer" << endl
                          << "=========================" << endl;

        ASSERT_SAME(bslmf_ArrayToConstPointer<char const    [1]>::Type,
                                              char const     *       );

        ASSERT_SAME(bslmf_ArrayToConstPointer<char       (&)[1]>::Type,
                                              char const     *       );

        ASSERT_SAME(bslmf_ArrayToConstPointer<char             >::Type,
                                              char                   );

        ASSERT_SAME(bslmf_ArrayToConstPointer<char const       >::Type,
                                              char const             );

        ASSERT_SAME(bslmf_ArrayToConstPointer<char           * >::Type,
                                              char           *       );

        ASSERT_SAME(bslmf_ArrayToConstPointer<void       *     [2]>::Type,
                                              void       *const *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<void const *     [2]>::Type,
                                              void const *const *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<void       *  (&)[2]>::Type,
                                              void       *const *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<void       *        >::Type,
                                              void       *              );
        ASSERT_SAME(bslmf_ArrayToConstPointer<void                >::Type,
                                              void                      );

        ASSERT_SAME(bslmf_ArrayToConstPointer<int                [3]>::Type,
                                              int const           *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int const          [3]>::Type,
                                              int const           *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int volatile       [3]>::Type,
                                              int const volatile  *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int const volatile [3]>::Type,
                                              int const volatile  *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int                   >::Type,
                                              int                         );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int const             >::Type,
                                              int const                   );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int volatile          >::Type,
                                              int volatile                );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int const volatile    >::Type,
                                              int const volatile          );

        ASSERT_SAME(bslmf_ArrayToConstPointer<int                (&)[4]>::Type,
                                              int const              *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int const          (&)[4]>::Type,
                                              int const              *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int volatile       (&)[4]>::Type,
                                              int const volatile     *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int const volatile (&)[4]>::Type,
                                              int const volatile     *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int                 &    >::Type,
                                              int                 &          );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int const           &    >::Type,
                                              int const           &          );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int volatile        &    >::Type,
                                              int volatile        &          );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int const volatile  &    >::Type,
                                              int const volatile  &          );

        ASSERT_SAME(bslmf_ArrayToConstPointer<int          *        [5]>::Type,
                                              int          *const    *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int const    *        [5]>::Type,
                                              int const    *const    *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int volatile *        [5]>::Type,
                                              int volatile *const    *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int const volatile *  [5]>::Type,
                                              int const volatile *const *    );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int                (*)[5]>::Type,
                                              int                (*)[5]      );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int const          (*)[5]>::Type,
                                              int const          (*)[5]      );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int volatile       (*)[5]>::Type,
                                              int volatile       (*)[5]      );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int const volatile (*)[5]>::Type,
                                              int const volatile (*)[5]      );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int                 *    >::Type,
                                              int                 *          );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int const           *    >::Type,
                                              int const           *          );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int volatile        *    >::Type,
                                              int volatile        *          );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int const volatile  *    >::Type,
                                              int const volatile  *          );

        ASSERT_SAME(bslmf_ArrayToConstPointer<int         [6][6]>::Type,
                                              int const   (*)[6]      );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int const   [6][6]>::Type,
                                              int const   (*)[6]      );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int      (&)[6][6]>::Type,
                                              int const   (*)[6]      );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int *       [6][6]>::Type,
                                              int *const  (*)[6]      );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int      (*)[6][6]>::Type,
                                              int      (*)[6][6]      );

        ASSERT_SAME(bslmf_ArrayToConstPointer<int *const    [6][6]>::Type,
                                              int *const (*)[6]         );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int *const (&)[6][6]>::Type,
                                              int *const (*)[6]         );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int *const (*)[6][6]>::Type,
                                              int *const (*)[6][6]      );

        ASSERT_SAME(bslmf_ArrayToConstPointer<void *[]>::Type,
                                              void * const *);

        ASSERT_SAME(bslmf_ArrayToConstPointer<int                []>::Type,
                                              int const          *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int const          []>::Type,
                                              int const          *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int volatile       []>::Type,
                                              int const volatile *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int const volatile []>::Type,
                                              int const volatile *       );

#ifndef BSLS_PLATFORM__CMP_MSVC
        ASSERT_SAME(bslmf_ArrayToConstPointer<int                (&)[]>::Type,
                                              int const             *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int const          (&)[]>::Type,
                                              int const             *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int volatile       (&)[]>::Type,
                                              int const volatile    *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int const volatile (&)[]>::Type,
                                              int const volatile    *       );
#endif

        ASSERT_SAME(bslmf_ArrayToConstPointer<int           *       []>::Type,
                                              int           *const  *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int const     *       []>::Type,
                                              int const     *const  *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int volatile  *       []>::Type,
                                              int volatile  *const  *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int const volatile *  []>::Type,
                                              int const volatile * const *  );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int                (*)[]>::Type,
                                              int                (*)[]      );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int const          (*)[]>::Type,
                                              int const          (*)[]      );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int volatile       (*)[]>::Type,
                                              int volatile       (*)[]      );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int const volatile (*)[]>::Type,
                                              int const volatile (*)[]      );

        ASSERT_SAME(bslmf_ArrayToConstPointer<int          [][7]>::Type,
                                              int const   (*)[7]      );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int const    [][7]>::Type,
                                              int const   (*)[7]      );
#ifndef BSLS_PLATFORM__CMP_MSVC
        ASSERT_SAME(bslmf_ArrayToConstPointer<int       (&)[][7]>::Type,
                                              int const (*)  [7]      );
#endif
        ASSERT_SAME(bslmf_ArrayToConstPointer<int *        [][7]>::Type,
                                              int *const  (*)[7]      );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int       (*)[][7]>::Type,
                                              int       (*)[][7]      );

        ASSERT_SAME(bslmf_ArrayToConstPointer<int *const    [][7]>::Type,
                                              int *const   (*)[7]      );
#ifndef BSLS_PLATFORM__CMP_MSVC
        ASSERT_SAME(bslmf_ArrayToConstPointer<int *const (&)[][7]>::Type,
                                              int *const (*)  [7]      );
#endif
        ASSERT_SAME(bslmf_ArrayToConstPointer<int *const (*)[][7]>::Type,
                                              int *const (*)[][7]      );

        ASSERT_SAME(bslmf_ArrayToConstPointer<Enum          [8]>::Type,
                                              Enum const     *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<Enum       (&)[8]>::Type,
                                              Enum const     *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<Enum const (&)[8]>::Type,
                                              Enum const     *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<Enum             >::Type,
                                              Enum                   );

        ASSERT_SAME(bslmf_ArrayToConstPointer<Struct      [8]>::Type,
                                              Struct const *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<Struct   (&)[8]>::Type,
                                              Struct const *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<Struct         >::Type,
                                              Struct               );

        ASSERT_SAME(bslmf_ArrayToConstPointer<Union      [8]>::Type,
                                              Union const *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<Union   (&)[8]>::Type,
                                              Union const *       );
        ASSERT_SAME(bslmf_ArrayToConstPointer<Union         >::Type,
                                              Union               );

        ASSERT_SAME(bslmf_ArrayToConstPointer<int  Struct::*    >::Type,
                                              int  Struct::*          );
        ASSERT_SAME(bslmf_ArrayToConstPointer<int (Struct::*)[9]>::Type,
                                              int (Struct::*)[9]      );
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate 'bslmf_ArrayToPointer' with various types and verify
        //   that their 'Type' member aliases the correctly decayed type.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "bslmf_ArrayToPointer" << endl
                          << "====================" << endl;

        ASSERT_SAME(bslmf_ArrayToPointer<const char [6]>::Type, const char*);
        if (verbose) {
            //P(bslmf_ArrayToPointer<const char [6]>::ID);
            P(typeid(const char [6]).name());
            P(typeid(bslmf_ArrayToPointer<const char [6]>::Type).name());
        }

        ASSERT_SAME(bslmf_ArrayToPointer<char const    [1]>::Type,
                                         char const     *       );

        ASSERT_SAME(bslmf_ArrayToPointer<char       (&)[1]>::Type,
                                         char           *       );

        ASSERT_SAME(bslmf_ArrayToPointer<char             >::Type,
                                         char                   );

        ASSERT_SAME(bslmf_ArrayToPointer<char const       >::Type,
                                         char const             );

        ASSERT_SAME(bslmf_ArrayToPointer<char           * >::Type,
                                         char           *       );

        ASSERT_SAME(bslmf_ArrayToPointer<void       *   [2]>::Type,
                                         void       *    *       );
        ASSERT_SAME(bslmf_ArrayToPointer<void const *   [2]>::Type,
                                         void const *    *       );
        ASSERT_SAME(bslmf_ArrayToPointer<void       *(&)[2]>::Type,
                                         void       *    *       );
        ASSERT_SAME(bslmf_ArrayToPointer<void       *      >::Type,
                                         void       *            );
        ASSERT_SAME(bslmf_ArrayToPointer<void              >::Type,
                                         void                    );

        ASSERT_SAME(bslmf_ArrayToPointer<int                [3]>::Type,
                                         int                 *       );
        ASSERT_SAME(bslmf_ArrayToPointer<int const          [3]>::Type,
                                         int const           *       );
        ASSERT_SAME(bslmf_ArrayToPointer<int volatile       [3]>::Type,
                                         int volatile        *       );
        ASSERT_SAME(bslmf_ArrayToPointer<int const volatile [3]>::Type,
                                         int const volatile  *       );
        ASSERT_SAME(bslmf_ArrayToPointer<int                   >::Type,
                                         int                         );
        ASSERT_SAME(bslmf_ArrayToPointer<int const             >::Type,
                                         int const                   );
        ASSERT_SAME(bslmf_ArrayToPointer<int volatile          >::Type,
                                         int volatile                );
        ASSERT_SAME(bslmf_ArrayToPointer<int const volatile    >::Type,
                                         int const volatile          );

        ASSERT_SAME(bslmf_ArrayToPointer<int                (&)[4]>::Type,
                                         int                    *       );
        ASSERT_SAME(bslmf_ArrayToPointer<int const          (&)[4]>::Type,
                                         int const              *       );
        ASSERT_SAME(bslmf_ArrayToPointer<int volatile       (&)[4]>::Type,
                                         int volatile           *       );
        ASSERT_SAME(bslmf_ArrayToPointer<int const volatile (&)[4]>::Type,
                                         int const volatile     *       );
        ASSERT_SAME(bslmf_ArrayToPointer<int                 &    >::Type,
                                         int                 &          );
        ASSERT_SAME(bslmf_ArrayToPointer<int const           &    >::Type,
                                         int const           &          );
        ASSERT_SAME(bslmf_ArrayToPointer<int volatile        &    >::Type,
                                         int volatile        &          );
        ASSERT_SAME(bslmf_ArrayToPointer<int const volatile  &    >::Type,
                                         int const volatile  &          );

        ASSERT_SAME(bslmf_ArrayToPointer<int                 * [5]>::Type,
                                         int                 *  *       );
        ASSERT_SAME(bslmf_ArrayToPointer<int const           * [5]>::Type,
                                         int const           *  *       );
        ASSERT_SAME(bslmf_ArrayToPointer<int volatile        * [5]>::Type,
                                         int volatile        *  *       );
        ASSERT_SAME(bslmf_ArrayToPointer<int const volatile  * [5]>::Type,
                                         int const volatile  *  *       );
        ASSERT_SAME(bslmf_ArrayToPointer<int                (*)[5]>::Type,
                                         int                (*)[5]      );
        ASSERT_SAME(bslmf_ArrayToPointer<int const          (*)[5]>::Type,
                                         int const          (*)[5]      );
        ASSERT_SAME(bslmf_ArrayToPointer<int volatile       (*)[5]>::Type,
                                         int volatile       (*)[5]      );
        ASSERT_SAME(bslmf_ArrayToPointer<int const volatile (*)[5]>::Type,
                                         int const volatile (*)[5]      );
        ASSERT_SAME(bslmf_ArrayToPointer<int                 *    >::Type,
                                         int                 *          );
        ASSERT_SAME(bslmf_ArrayToPointer<int const           *    >::Type,
                                         int const           *          );
        ASSERT_SAME(bslmf_ArrayToPointer<int volatile        *    >::Type,
                                         int volatile        *          );
        ASSERT_SAME(bslmf_ArrayToPointer<int const volatile  *    >::Type,
                                         int const volatile  *          );

        ASSERT_SAME(bslmf_ArrayToPointer<int         [6][6]>::Type,
                                         int      (*)[6]         );
        ASSERT_SAME(bslmf_ArrayToPointer<int const   [6][6]>::Type,
                                         int const(*)[6]         );
        ASSERT_SAME(bslmf_ArrayToPointer<int      (&)[6][6]>::Type,
                                         int      (*)[6]         );
        ASSERT_SAME(bslmf_ArrayToPointer<int       * [6][6]>::Type,
                                         int    * (*)[6]         );
        ASSERT_SAME(bslmf_ArrayToPointer<int      (*)[6][6]>::Type,
                                         int      (*)[6][6]      );

        ASSERT_SAME(bslmf_ArrayToPointer<int *const    [6][6]>::Type,
                                         int *const (*)[6]         );
        ASSERT_SAME(bslmf_ArrayToPointer<int *const (&)[6][6]>::Type,
                                         int *const (*)[6]         );
        ASSERT_SAME(bslmf_ArrayToPointer<int *const (*)[6][6]>::Type,
                                         int *const (*)[6][6]      );

        ASSERT_SAME(bslmf_ArrayToPointer<void *[]>::Type,
                                         void * *      );

        ASSERT_SAME(bslmf_ArrayToPointer<int                []>::Type,
                                         int                *       );
        ASSERT_SAME(bslmf_ArrayToPointer<int const          []>::Type,
                                         int const          *       );
        ASSERT_SAME(bslmf_ArrayToPointer<int volatile       []>::Type,
                                         int volatile       *       );
        ASSERT_SAME(bslmf_ArrayToPointer<int const volatile []>::Type,
                                         int const volatile *       );

#ifndef BSLS_PLATFORM__CMP_MSVC
        ASSERT_SAME(bslmf_ArrayToPointer<int                (&)[]>::Type,
                                         int                   *       );
        ASSERT_SAME(bslmf_ArrayToPointer<int const          (&)[]>::Type,
                                         int const             *       );
        ASSERT_SAME(bslmf_ArrayToPointer<int volatile       (&)[]>::Type,
                                         int volatile          *       );
        ASSERT_SAME(bslmf_ArrayToPointer<int const volatile (&)[]>::Type,
                                         int const volatile    *       );
#endif

        ASSERT_SAME(bslmf_ArrayToPointer<int                 * []>::Type,
                                         int                 * *       );
        ASSERT_SAME(bslmf_ArrayToPointer<int const           * []>::Type,
                                         int const           * *       );
        ASSERT_SAME(bslmf_ArrayToPointer<int volatile        * []>::Type,
                                         int volatile        * *       );
        ASSERT_SAME(bslmf_ArrayToPointer<int const volatile  * []>::Type,
                                         int const volatile  * *       );
        ASSERT_SAME(bslmf_ArrayToPointer<int                (*)[]>::Type,
                                         int                (*)[]      );
        ASSERT_SAME(bslmf_ArrayToPointer<int const          (*)[]>::Type,
                                         int const          (*)[]      );
        ASSERT_SAME(bslmf_ArrayToPointer<int volatile       (*)[]>::Type,
                                         int volatile       (*)[]      );
        ASSERT_SAME(bslmf_ArrayToPointer<int const volatile (*)[]>::Type,
                                         int const volatile (*)[]      );

        ASSERT_SAME(bslmf_ArrayToPointer<int          [][7]>::Type,
                                         int         (*)[7]      );
        ASSERT_SAME(bslmf_ArrayToPointer<int const    [][7]>::Type,
                                         int const   (*)[7]      );
#ifndef BSLS_PLATFORM__CMP_MSVC
        ASSERT_SAME(bslmf_ArrayToPointer<int       (&)[][7]>::Type,
                                         int       (*)  [7]      );
#endif
        ASSERT_SAME(bslmf_ArrayToPointer<int        * [][7]>::Type,
                                         int        *(*)[7]      );
        ASSERT_SAME(bslmf_ArrayToPointer<int       (*)[][7]>::Type,
                                         int       (*)[][7]      );

        ASSERT_SAME(bslmf_ArrayToPointer<int *const    [][7]>::Type,
                                         int *const   (*)[7]      );
#ifndef BSLS_PLATFORM__CMP_MSVC
        ASSERT_SAME(bslmf_ArrayToPointer<int *const (&)[][7]>::Type,
                                         int *const (*)  [7]      );
#endif
        ASSERT_SAME(bslmf_ArrayToPointer<int *const (*)[][7]>::Type,
                                         int *const (*)[][7]      );

        ASSERT_SAME(bslmf_ArrayToPointer<Enum          [8]>::Type,
                                         Enum           *       );
        ASSERT_SAME(bslmf_ArrayToPointer<Enum       (&)[8]>::Type,
                                         Enum           *       );
        ASSERT_SAME(bslmf_ArrayToPointer<Enum const (&)[8]>::Type,
                                         Enum const     *       );
        ASSERT_SAME(bslmf_ArrayToPointer<Enum             >::Type,
                                         Enum                   );

        ASSERT_SAME(bslmf_ArrayToPointer<Struct    [8]>::Type,
                                         Struct     *       );
        ASSERT_SAME(bslmf_ArrayToPointer<Struct (&)[8]>::Type,
                                         Struct     *       );
        ASSERT_SAME(bslmf_ArrayToPointer<Struct       >::Type,
                                         Struct             );

        ASSERT_SAME(bslmf_ArrayToPointer<Union    [8]>::Type,
                                         Union     *       );
        ASSERT_SAME(bslmf_ArrayToPointer<Union (&)[8]>::Type,
                                         Union     *       );
        ASSERT_SAME(bslmf_ArrayToPointer<Union       >::Type,
                                         Union             );

        ASSERT_SAME(bslmf_ArrayToPointer<int  Struct::*    >::Type,
                                         int  Struct::*          );
        ASSERT_SAME(bslmf_ArrayToPointer<int (Struct::*)[9]>::Type,
                                         int (Struct::*)[9]      );
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = "
             << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
