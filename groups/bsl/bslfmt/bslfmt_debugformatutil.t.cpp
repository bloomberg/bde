// bslfmt_debugformatutil.t.cpp                                       -*-C++-*-
#include <bslfmt_debugformatutil.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>

#include <stdio.h>   // `printf`
#include <stdlib.h>  // `atoi`

using namespace BloombergLP;
using namespace bslfmt;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test provides a trait detecting formatters that support
// debug formatting, and a utility function that calls `set_debug_format()` on
// such a formatter and does nothing otherwise.
// ----------------------------------------------------------------------------
// TRAITS
// [ 2] SupportsDebugFormat<t_FORMATTER>::value
//
// CLASS METHODS
// [ 3] void setDebugFormat(t_FORMATTER *formatter);
// ----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE
// [ 1] BREATHING TEST

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------
namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);
        fflush(stdout);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV BSLS_BSLTESTUTIL_ASSERTV

#define P BSLS_BSLTESTUTIL_P    // Print identifier and value.
#define T_ BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                          GLOBAL TEST TYPES
// ----------------------------------------------------------------------------

namespace {

/// A formatter having the member and the nested trait declaration.
struct TaggedFormatter {
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TaggedFormatter, SupportsDebugFormat);

    // PUBLIC DATA
    bool d_called;

    // CREATORS
    TaggedFormatter()
    : d_called(false)
    {
    }

    // MANIPULATORS
    void set_debug_format() { d_called = true; }
};

/// A formatter inheriting the member, without its own trait declaration.
struct DerivedFormatter : TaggedFormatter {
};

/// A formatter inheriting the member, with its own trait declaration.
struct DerivedDeclaredFormatter : TaggedFormatter {
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(DerivedDeclaredFormatter,
                                   SupportsDebugFormat);
};

/// A formatter having the member, without the trait declaration.
struct UntaggedFormatter {
    // PUBLIC DATA
    bool d_called;

    // CREATORS
    UntaggedFormatter()
    : d_called(false)
    {
    }

    // MANIPULATORS
    void set_debug_format() { d_called = true; }
};

/// A formatter having neither the member nor the trait declaration.
struct PlainFormatter {
    // PUBLIC DATA
    bool d_called;

    // CREATORS
    PlainFormatter()
    : d_called(false)
    {
    }
};

}  // close unnamed namespace

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.

namespace usage_example_1 {

///Example 1: Requesting Debug Formatting Of An Element
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are writing a formatter for a container and wish its elements to
// be rendered in the debug presentation, as the standard requires.
//
// First, we have two element formatters: one that has a debug presentation and
// advertises it and one that has none.
// ```
    struct StringLikeFormatter {
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(StringLikeFormatter,
                                       bslfmt::SupportsDebugFormat);

        // PUBLIC DATA
        bool d_debug;

        // CREATORS
        StringLikeFormatter() : d_debug(false) {}

        // MANIPULATORS
        void set_debug_format() { d_debug = true; }
    };

    struct IntLikeFormatter {
        // PUBLIC DATA
        bool d_debug;

        // CREATORS
        IntLikeFormatter() : d_debug(false) {}
    };
// ```
// Now, we ask each of them to switch to debug formatting:
// ```
    void run()
    {
        StringLikeFormatter stringFormatter;
        IntLikeFormatter    intFormatter;

        bslfmt::DebugFormatUtil::setDebugFormat(&stringFormatter);
        bslfmt::DebugFormatUtil::setDebugFormat(&intFormatter);
// ```
// Finally, we observe that the string-like formatter has switched and the
// integer-like formatter has been left alone:
// ```
        ASSERT(true  == stringFormatter.d_debug);
        ASSERT(false == intFormatter.d_debug);
    }
// ```

}  // close namespace usage_example_1

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int  test        = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose     = argc > 2;
    const bool veryVerbose = argc > 3;

    printf("TEST %s CASE %d \n", __FILE__, test);

    bslma::TestAllocator globalAllocator("global", veryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        // 1. The usage example provided in the component header file
        //    compiles, links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate the usage example from the header into the test
        //    driver, remove leading comment characters, and replace `assert`
        //    with `ASSERT`.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        usage_example_1::run();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING `setDebugFormat`
        //
        // Concerns:
        // 1. `setDebugFormat` calls `set_debug_format()` on a formatter that
        //    has one.
        //
        // 2. `setDebugFormat` compiles, and does nothing, for a formatter that
        //    does not.
        //
        // Plan:
        // 1. Call `setDebugFormat` on one formatter of each kind and observe
        //    whether the member ran.  (C-1..2)
        //
        // Testing:
        //   void setDebugFormat(t_FORMATTER *formatter);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `setDebugFormat`"
                            "\n========================\n");

        {
            TaggedFormatter formatter;
            DebugFormatUtil::setDebugFormat(&formatter);
            ASSERT(formatter.d_called);
        }
        {
            DerivedDeclaredFormatter formatter;
            DebugFormatUtil::setDebugFormat(&formatter);
            ASSERT(formatter.d_called);
        }
        {
            // A derived formatter that does not repeat the declaration is
            // rescued by the `decltype` detection, and only by it.
            DerivedFormatter formatter;
            DebugFormatUtil::setDebugFormat(&formatter);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
            ASSERT(formatter.d_called);
#else
            ASSERT(!formatter.d_called);
#endif
        }
        {
            // The call must compile and be a no-op.  There is no member to
            // observe, so the point of the test is that this line builds.
            PlainFormatter formatter;
            DebugFormatUtil::setDebugFormat(&formatter);
            ASSERT(!formatter.d_called);
        }
#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
        {
            UntaggedFormatter formatter;
            DebugFormatUtil::setDebugFormat(&formatter);
            ASSERT(formatter.d_called);
        }
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING DETECTION
        //
        // Concerns:
        // 1. A formatter associated with the trait is detected.
        //
        // 2. A formatter with no debug mode is not detected.
        //
        // 3. Where `decltype` is available, a formatter having the member but
        //    no declaration is detected as well.
        //
        // 4. A nested trait association is not inherited: a derived formatter
        //    that does not repeat the declaration is detected only through the
        //    `decltype` path.
        //
        // Plan:
        // 1. Check `SupportsDebugFormat<T>::value` for one type of
        //    each kind.  (C-1..4)
        //
        // Testing:
        //   SupportsDebugFormat<t_FORMATTER>::value
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING DETECTION"
                            "\n=================\n");

        ASSERT((SupportsDebugFormat<TaggedFormatter>::value));
        ASSERT((SupportsDebugFormat<DerivedDeclaredFormatter>::value));
        ASSERT(!(SupportsDebugFormat<PlainFormatter>::value));
        ASSERT(!(SupportsDebugFormat<int>::value));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
        ASSERT((SupportsDebugFormat<UntaggedFormatter>::value));
        ASSERT((SupportsDebugFormat<DerivedFormatter>::value));
#else
        ASSERT(!(SupportsDebugFormat<UntaggedFormatter>::value));
        ASSERT(!(SupportsDebugFormat<DerivedFormatter>::value));
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        // 1. The component is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Request debug formatting of a supporting and a non-supporting
        //    formatter.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST\n==============\n");

        TaggedFormatter supported;
        PlainFormatter   unsupported;

        DebugFormatUtil::setDebugFormat(&supported);
        DebugFormatUtil::setDebugFormat(&unsupported);

        ASSERT(supported.d_called);
      } break;
      default: {
        printf("WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        printf("Error, non-zero test status = %d .\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2026 Bloomberg Finance L.P.
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
