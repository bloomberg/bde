// bsls_nameof.t.cpp                                                  -*-C++-*-

#include <bsls_nameof.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_stopwatch.h>

#include <cstring>
#include <string>

#include <stdio.h>       // sprintf()
#include <stdlib.h>      // atoi()

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test provides an in-core pointer-semantic object.
//-----------------------------------------------------------------------------
// CREATORS
//
// MANIPULATORS
//
// ACCESSORS
//
// FREE OPERATORS
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [11] USAGE

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU

// ============================================================================
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

// First, your test driver must have the following 'using' statements so that
// the template class 'NameOf' and the template function 'nameOfType' can be
// referred to concisely, without having to qualify them with namespaces on
// each call.  Note that if you've already said 'using namespace BloombergLP'
// you don't have to give the 'BloombergLP::' qualifiers here:
//..
    using BloombergLP::bsls::NameOf;
    using BloombergLP::bsls::nameOfType;
//..
// Next, we define some types in the unnamed namespace:
//..
    namespace {
//
    struct MyType {
        int  d_i;
        char d_c;
    };
//
    union MyUnion {
        int  d_i;
        char d_buffer[100];
    };
//
    }  // close unnamed namespace
//..

static int array[100];

static bool eq(const char *lhs, const char *rhs)
{
    return !std::strcmp(lhs, rhs);
}

namespace BloombergLP {
namespace bsls {

template <class TYPE>
struct Pretty {
    // DATA
    const char *d_funcName;

    // CREATOR
    Pretty()
        // Initialize this object with the function name of the c'tor.
    {
#if   defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
        d_funcName = __PRETTY_FUNCTION__;
#elif defined(BSLS_PLATFORM_CMP_SUN)
# if  BSLS_PLATFORM_CMP_VERSION >= 20768
        d_funcName = __PRETTY_FUNCTION__;
# else
        d_funcName = "unknown_type";
# endif
#elif defined(BSLS_PLATFORM_CMP_IBM)
        d_funcName = __FUNCTION__;
#elif defined(BSLS_PLATFORM_CMP_MSVC)
        d_funcName = __FUNCSIG__;
#else
# error No function signature macro defined.
#endif
    }

    // ACCESSOR
    operator const char *() const
        // Return the function name of the c'tor.
    {
        return d_funcName;
    }
};

}  // close package namespace
}  // close enterprise namespace

//=============================================================================
//                             MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE:
        //
        // Concerns:
        //   Demonstrate the usage of this component.
        //
        // Plan:
        //   Run the usage example to be sure it works.
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 20768
        // This component doesn't work on Solaris CC before 5.12.

        break;
#endif

// Next, we see that the 'NameOf' template class, when created with a type, can
// be implicitly cast to a 'const char *' which points to a description of the
// type.
//..
    ASSERT(!std::strcmp("double", NameOf<double>()));
    ASSERT(!std::strcmp("int",    NameOf<int>()));
//..
// Then, we see that when 'NameOf' is passed a 'typedef' or template parameter,
// it resolves it to the original type:
//..
    typedef int Woof;
//
    ASSERT(!std::strcmp("int",    NameOf<Woof>()));
//..
// Next, we introduce the 'nameOfType' template function, which takes as any
// variable as an argument, and returns a 'const char *' pointing to a
// description of the type of the variable.
//..
    int ii = 2;
//
    ASSERT(!std::strcmp("int",    nameOfType(ii)));
//..
// Then, we see that 'NameOf' and 'nameOfType' will strip 'BloombergLP::'
// namespace qualifiers, as well as anonymous namespace qualifiers.
//..
    typedef BloombergLP::bsls::Stopwatch SW;
//
    const SW      sw;
    const MyType  mt = { 2, 'a' };
    MyUnion       mu;
    mu.d_i = 7;
//
    ASSERT(!std::strcmp("bsls::Stopwatch", NameOf<SW>()));
    ASSERT(!std::strcmp("bsls::Stopwatch",
                                      NameOf<BloombergLP::bsls::Stopwatch>()));
    ASSERT(!std::strcmp("bsls::Stopwatch", nameOfType(sw)));
//
    ASSERT(!std::strcmp("MyType",          NameOf<MyType>()));
    ASSERT(!std::strcmp("MyType",          nameOfType(mt)));
//
    ASSERT(!std::strcmp("MyUnion",         NameOf<MyUnion>()));
    ASSERT(!std::strcmp("MyUnion",         nameOfType(mu)));
//..
// There is a problem with template code not knowing how to implicitly cast the
// 'NameOf' type to 'const char *' for initializing or comparing with
// 'std::string's.  To facilitate, 'NameOf' provides a 'const char *' 'name'
// accessor, to avoid the user having to do a more verbose 'static cast'.
//..
    const std::string swName = "bsls::Stopwatch";
    ASSERT(swName == static_cast<const char *>(NameOf<SW>()));
    ASSERT(swName == NameOf<SW>().name());
//
    const std::string swNameB = NameOf<SW>().name();
    ASSERT(swNameB == swName);
if (verbose) {
    printf("NameOf<SW>() = \"%s\"\n", NameOf<SW>().name());
    printf("NameOfType(4 + 3) = \"%s\"\n", nameOfType(4 + 3));
}
//..
// Note that 'nameOfType' naturally returns a 'const char *' and needs no help
// casting.  Note also that 'bsls::debugprint' is able to figure out how to
// cast 'NameOf' directly to 'const char *' with no problems, as can iostreams,
// so there is no problem with putting a 'NameOf' in a 'LOOP_ASSERT' or
// 'ASSERTV'.  It is anticipated that displaying by the BDE 'ASSERTV',
// 'LOOP_ASSERT, and 'P' macros will be the primary use of this component.
//..
if (verbose) {
    printf("NameOf<double>() = ");
    BloombergLP::bsls::debugprint(NameOf<double>());
    printf("\n");
}
//
    typedef double DTYPE;
    DTYPE x = 7.3;
//
    LOOP_ASSERT(NameOf<DTYPE>(), x > 7);
//
    std::string myStr;              // Assign, not init, of string doesn't need
    myStr = NameOf<DTYPE>();        // '.name()'.
    ASSERT("double" == myStr);
//..
// Which produces:
//..
//  NameOf<SW>() = "bsls::Stopwatch"
//..
// Finally, we see that 'NameOf' and 'nameOfType' will simplifiy
// 'std::basic_string<...>' declarations to 'std::string'.
//..
    const std::string s = "std::string";
//
    ASSERT(s == NameOf<std::basic_string<char> >().name());
    ASSERT(s == NameOf<std::string>().name());
    ASSERT(s == nameOfType(s));
//
    typedef NameOf<std::string> Nos;
//
    const std::string s2 = "bsls::NameOf<std::string>";
//
    ASSERT(s2 == NameOf<NameOf<std::basic_string<char> > >().name());
    ASSERT(s2 == NameOf<NameOf<std::string> >().name());
    ASSERT(s2 == NameOf<Nos>().name());
    ASSERT(s2 == nameOfType(Nos()));
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Concerns:
        //   Exercise each method lightly.
        //
        // Plan:
        //   Call the 'NameOf' c'tor and the local 'nameOfType' function on
        //   various types and observe the results.
        //
        // Testing:
        //   This Test Case exercises basic functionality.
        // --------------------------------------------------------------------

        if (verbose) printf("BREATHING TEST\n"
                            "==============\n");

        using BloombergLP::bsls::NameOf;
        using BloombergLP::bsls::nameOfType;
        using BloombergLP::bsls::Pretty;

        int             ii;
        MyType          mt;
        MyUnion         mu;
        bsls::Stopwatch sw;

        if (verbose) {
            P(Pretty<int>());
            P(Pretty<std::string>());
            P(Pretty<MyType>());
            P(Pretty<NameOf<std::string> >());
            P(Pretty<NameOf<bsls::Stopwatch> >());

            P(NameOf<int>());
            P(NameOf<MyType>());
            P(NameOf<MyUnion>());
            P(NameOf<std::string>());
            P(NameOf<NameOf<int> >());
            P(NameOf<bsls::Stopwatch>());
            P(NameOf<NameOf<std::string> >());
            P(NameOf<NameOf<bsls::Stopwatch> >());

            P(nameOfType(ii));
            P(nameOfType(mt));
            P(nameOfType(mu));
            P(nameOfType(sw));
            P(nameOfType(array));
            P(nameOfType(""));
            P(nameOfType(&nameOfType<int>));
            P(nameOfType(NameOf<int>()));
            P(nameOfType(sw));
            P(nameOfType(&NameOf<std::string>::name));

#if   defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
            P(nameOfType(__PRETTY_FUNCTION__));
#elif defined(BSLS_PLATFORM_CMP_SUN)
# if  BSLS_PLATFORM_CMP_VERSION >= 20768
            P(nameOfType(__PRETTY_FUNCTION__));
# else
            P(nameOfType(__FUNCTION__));;
# endif
#elif defined(BSLS_PLATFORM_CMP_IBM)
            P(nameOfType(__FUNCTION__));
#elif defined(BSLS_PLATFORM_CMP_MSVC)
            P(nameOfType(__FUNCSIG__));
#else
# error No function signature macro defined.
#endif

            P(BSLS_PLATFORM_CMP_VERSION);
        }

        // Ensure that all calls with a given type return the same ptr.

        const char *pc = NameOf<int>();
        for (int jj = 0; jj < 10; ++jj) {
            ASSERT(NameOf<int>()        == pc);
            ASSERT(NameOf<int>().name() == pc);
            ASSERT(nameOfType(ii)       == pc);
            ASSERT(nameOfType(jj)       == pc);
            ASSERT(nameOfType(jj + 11)  == pc);
        }

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 20768
        // This component doesn't work on Solaris CC before 5.12.

        ASSERT(eq("unknown_type", NameOf<int>()));

        break;
#endif

        std::string s;
        s = NameOf<int>();

        ASSERTV(NameOf<float>().name(),
                                  eq(NameOf<float>().name(), NameOf<float>()));

        ASSERTV(nameOfType(ii), NameOf<int>(),
                                        eq(nameOfType(ii), NameOf<int>()));
        ASSERTV(nameOfType(mt), NameOf<MyType>(),
                                        eq(nameOfType(mt), NameOf<MyType>()));
        ASSERTV(nameOfType(mu), NameOf<MyUnion>(),
                                        eq(nameOfType(mu), NameOf<MyUnion>()));

        ASSERTV(NameOf<int>(),        eq("int",        NameOf<int>()));
        ASSERTV(NameOf<MyType>(),     eq("MyType",     NameOf<MyType>()));
        ASSERTV(NameOf<MyUnion>(),    eq("MyUnion",    NameOf<MyUnion>()));
        ASSERTV(NameOf<std::string>(),eq("std::string",NameOf<std::string>()));
        ASSERTV(NameOf<bsls::Stopwatch>(),
                             eq("bsls::Stopwatch", NameOf<bsls::Stopwatch>()));
        ASSERTV(NameOf<NameOf<int> >(),
                              eq("bsls::NameOf<int>", NameOf<NameOf<int> >()));

        ASSERTV(NameOf<NameOf<MyType> >(),
                        eq("bsls::NameOf<MyType>", NameOf<NameOf<MyType> >()));
        ASSERTV(NameOf<NameOf<bsls::Stopwatch> >(),
                                            eq("bsls::NameOf<bsls::Stopwatch>",
                                          NameOf<NameOf<bsls::Stopwatch> >()));
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
