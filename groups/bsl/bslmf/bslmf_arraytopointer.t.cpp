// bslmf_arraytopointer.t.cpp                                         -*-C++-*-

#include <bslmf_arraytopointer.h>
#include <bslmf_issame.h>

#include <bsls_platform.h>            // for testing only

#include <stdlib.h>    // atoi()
#include <iostream>
#include <typeinfo>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
//                                  TBD
//-----------------------------------------------------------------------------
// [ 1] bslmf::ArrayToPointer
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
#define ASSERT_SAME(X,Y) { ASSERT((1==bsl::is_same<X,Y>::value)); }
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

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

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
        if (verbose) cout << endl
                          << "bslmf::ArrayToConstPointer" << endl
                          << "==========================" << endl;

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

        if (verbose) cout << endl
                          << "bslmf::ArrayToPointer" << endl
                          << "====================" << endl;

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
