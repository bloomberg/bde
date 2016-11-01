// bslmf_util.t.cpp                                                   -*-C++-*-
#include <bslmf_util.h>

#include <bsls_objectbuffer.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>

#include <new>   // required to support use of "placement new"

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// 'bslmf::Util' is a utility class, where each function will be tested in a
// separate test case.  Any significant test machinery will be tested before
// any function whose test case relies upon it.
//-----------------------------------------------------------------------------
// CLASS METHODS
// [ ] template <class T> forward(T&& value)
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [ ] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#   define BSLMF_UTIL_FORWARD_REF(T) const T&
#   define BSLMF_UTIL_FORWARD(T,V) \
        ::BloombergLP::bslmf::Util::forward(V)
#else
#   define BSLMF_UTIL_FORWARD_REF(T) T&&
#   define BSLMF_UTIL_FORWARD(T,V)       \
        ::BloombergLP::bsls::Util::forward<T>(V)
#endif

struct CUtil {
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=2
    template <class TARGET, class ...Args>
    static void construct(TARGET *address, Args&&...args)
    {
      new (address) TARGET(bslmf::Util::forward<Args>(args)...);
    }
#else
    template <class TARGET, class Arg1>
    static void construct(TARGET *address, BSLMF_UTIL_FORWARD_REF(Arg1) a1)
    {
      new (address) TARGET(BSLMF_UTIL_FORWARD(Arg1,a1));
    }
    template <class TARGET, class Arg1, class Arg2>
    static void construct(TARGET *address,
                          BSLMF_UTIL_FORWARD_REF(Arg1) a1,
                          BSLMF_UTIL_FORWARD_REF(Arg2) a2)
    {
      new (address) TARGET(BSLMF_UTIL_FORWARD(Arg1,a1),
                           BSLMF_UTIL_FORWARD(Arg2,a2));
    }
#endif
};

class Obj {
    bool d_copied;
    bool d_moved;

  public:
    Obj()
    : d_copied(false)
    , d_moved(false)
    {
    }

    Obj(const Obj&)
    : d_copied(true)
    , d_moved(false)
    {
    }

    Obj(bslmf::MovableRef<Obj>)
    : d_copied(false)
    , d_moved(true)
    {
    }

    bool copied() const { return d_copied; }
    bool moved() const { return d_moved; }
};

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    (void)veryVerbose;
    (void)veryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This test exercises basic functionality but *tests* *nothing*.
        //
        // Concerns:
        //: 1 That the functions exist with the documented signatures.
        //: 2 That the basic functionality works as documented.
        //
        // Plan:
        //: 1 Exercise each function in turn and devise an elementary test
        //:   sequence to ensure that the basic functionality is as documented.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        Obj mA; const Obj& A = mA;
        ASSERT(false == A.copied());
        ASSERT(false == A.moved());

        bsls::ObjectBuffer<Obj> buf;
        Obj& mX = buf.object(); const Obj& X = mX;

        CUtil::construct(buf.address(),  A);
        ASSERT( true == X.copied());
        ASSERT(false == X.moved());

        CUtil::construct(buf.address(), mA);
        ASSERT( true == X.copied());
        ASSERT(false == X.moved());

        CUtil::construct(buf.address(), bslmf::MovableRefUtil::move(mA));
        ASSERT(false == X.copied());
        ASSERT( true == X.moved());

        ASSERT(sizeof(bool) == sizeof(bslmf::Util::declval<Obj>().copied()));
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
