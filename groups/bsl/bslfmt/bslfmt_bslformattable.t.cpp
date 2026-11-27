// bslfmt_bslformattable.t.cpp                                        -*-C++-*-
#include <bslfmt_bslformattable.h>

#include <bslfmt_formatterbase.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
#include <format>
#endif

#include <stdio.h>   // `printf`
#include <stdlib.h>  // `atoi`

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test provides the concept `bslfmt::BslFormattable`.  We
// verify the result of the concept for a set of test types whose formatters,
// defined in this test driver, either meet or violate (in one way each) the
// requirements of a formatter, and for a delegating formatter constrained on
// the concept.  When the standard library provides `<format>` we also verify
// that the concept is not satisfied for a type having only a `std::formatter`.
//
// The concept is defined only if concepts are supported.
//-----------------------------------------------------------------------------
// CONCEPTS
// [ 2] bslfmt::BslFormattable<t_TYPE, t_CHAR>
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE
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

#define LOOP_ASSERT BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q BSLS_BSLTESTUTIL_Q    // Quote identifier literally.
#define P BSLS_BSLTESTUTIL_P    // Print identifier and value.
#define P_ BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                               TEST TYPES
//-----------------------------------------------------------------------------

/// A type having a valid `bsl::formatter` for `char` (but not for `wchar_t`).
struct Formattable {};

/// A type having no formatter.
struct NotFormattable {};

/// A type whose `bsl::formatter` has no `parse` member.
struct NoParse {};

/// A type whose `bsl::formatter` is not copyable.
struct NotCopyable {};

/// A type whose `bsl::formatter` has a `format` member returning a type other
/// than the iterator of the format context.
struct BadFormat {};

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
/// A type having a `std::formatter`, but no `bsl::formatter`.
struct StdOnly {};
#endif

namespace bsl {

template <>
struct formatter<Formattable, char> {
    template <class t_PARSE_CONTEXT>
    typename t_PARSE_CONTEXT::iterator parse(t_PARSE_CONTEXT& context)
    {
        return context.begin();
    }

    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                         const Formattable&,
                                         t_FORMAT_CONTEXT& context) const
    {
        return context.out();
    }
};

template <>
struct formatter<NoParse, char> {
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                         const NoParse&,
                                         t_FORMAT_CONTEXT& context) const
    {
        return context.out();
    }
};

template <>
struct formatter<NotCopyable, char> {
  private:
    // NOT IMPLEMENTED
    formatter(const formatter&) BSLS_KEYWORD_DELETED;

  public:
    formatter() {}

    template <class t_PARSE_CONTEXT>
    typename t_PARSE_CONTEXT::iterator parse(t_PARSE_CONTEXT& context)
    {
        return context.begin();
    }

    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                         const NotCopyable&,
                                         t_FORMAT_CONTEXT& context) const
    {
        return context.out();
    }
};

template <>
struct formatter<BadFormat, char> {
    template <class t_PARSE_CONTEXT>
    typename t_PARSE_CONTEXT::iterator parse(t_PARSE_CONTEXT& context)
    {
        return context.begin();
    }

    template <class t_FORMAT_CONTEXT>
    int format(const BadFormat&, t_FORMAT_CONTEXT&) const
    {
        return 0;
    }
};

}  // close namespace bsl

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
namespace std {

template <>
struct formatter<StdOnly, char> {
    template <class t_PARSE_CONTEXT>
    constexpr typename t_PARSE_CONTEXT::iterator parse(
                                                      t_PARSE_CONTEXT& context)
    {
        return context.begin();
    }

    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                         const StdOnly&,
                                         t_FORMAT_CONTEXT& context) const
    {
        return context.out();
    }
};

}  // close namespace std
#endif

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Example 1: Constraining a Delegating Formatter
/// - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a class template, `Box`, that holds a single value, and we
// want to format a `Box` by formatting its value.
//
// First, we define `Box`:
// ```
    template <class t_TYPE>
    struct Box {
        t_TYPE d_value;
    };
// ```
// Then, we define a `bsl::formatter` for `Box` that delegates to the
// `bsl::formatter` of the value type.  We constrain the specialization on
// `bslfmt::BslFormattable`, so that it is enabled only if that delegation is
// possible.  Since concepts may not be available (older compilers/standards)
// we protect the constraint with the preprocessor:
// ```
    namespace bsl {

    template <class t_TYPE, class t_CHAR>
#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONCEPTS
        requires BloombergLP::bslfmt::BslFormattable<t_TYPE, t_CHAR>
#endif
    struct formatter<Box<t_TYPE>, t_CHAR> {
        // DATA
        formatter<t_TYPE, t_CHAR> d_formatter;

        // MANIPULATORS
        template <class t_PARSE_CONTEXT>
        BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
                                                      t_PARSE_CONTEXT& context)
        {
            return d_formatter.parse(context);
        }

        // ACCESSORS
        template <class t_FORMAT_CONTEXT>
        typename t_FORMAT_CONTEXT::iterator format(
                                        const Box<t_TYPE>& box,
                                        t_FORMAT_CONTEXT&  context) const
        {
            return d_formatter.format(box.d_value, context);
        }
    };

    }  // close namespace bsl
// ```
// Now, we define a type that has no formatter:
// ```
    struct NoFormatter {};
// ```
// Finally, we observe that a `Box` of that type, and a `Box` of such a `Box`,
// are not formattable: the constrained specialization is not used for them,
// rather than failing to compile when instantiated:
// ```
//  #ifdef BSLS_COMPILERFEATURES_SUPPORT_CONCEPTS
//  assert(!(bslfmt::BslFormattable<Box<NoFormatter>,      char>));
//  assert(!(bslfmt::BslFormattable<Box<Box<NoFormatter> >, char>));
//  #endif
// ```

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;  (void)    veryVerbose;
    const bool     veryVeryVerbose = argc > 4;  (void)veryVeryVerbose;
    const bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: No global memory is allocated after `main` starts.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Confirm no static initialization locked the global allocator
    ASSERT(&globalAllocator == bslma::Default::globalAllocator());

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    // Confirm no static initialization locked the default allocator
    ASSERT(&defaultAllocator == bslma::Default::defaultAllocator());

    switch (test) {  case 0:
      case 3: {
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

        if (verbose) puts("\nUSAGE EXAMPLE"
                          "\n=============");

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONCEPTS
        ASSERT(!(bslfmt::BslFormattable<Box<NoFormatter>,      char>));
        ASSERT(!(bslfmt::BslFormattable<Box<Box<NoFormatter> >, char>));
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CONCEPT `bslfmt::BslFormattable`
        //
        // Concerns:
        // 1. The concept is satisfied for a type having a valid
        //    `bsl::formatter` for the given character type, regardless of the
        //    cv-qualification and reference of the type.
        //
        // 2. The concept is not satisfied for a type having no
        //    `bsl::formatter`, or a `bsl::formatter` that is not copyable,
        //    has no `parse` member, or has a `format` member with the wrong
        //    return type.
        //
        // 3. The concept is not satisfied for a type having a
        //    `std::formatter` but no `bsl::formatter`.
        //
        // 4. A delegating formatter constrained on the concept is enabled if
        //    and only if the type it delegates to satisfies the concept, also
        //    when nested.
        //
        // Plan:
        // 1. Verify the result of the concept for each test type, with `char`
        //    and `wchar_t`.  (C-1..3)
        //
        // 2. Verify the result of the concept for `Box` (from the usage
        //    example) of formattable and not formattable types, and of a
        //    `Box` of such a `Box`.  (C-4)
        //
        // Testing:
        //   bslfmt::BslFormattable<t_TYPE, t_CHAR>
        // --------------------------------------------------------------------

        if (verbose) puts("\nCONCEPT `bslfmt::BslFormattable`"
                          "\n================================");

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONCEPTS
        ASSERT( (bslfmt::BslFormattable<Formattable,           char>));
        ASSERT( (bslfmt::BslFormattable<const Formattable,     char>));
        ASSERT( (bslfmt::BslFormattable<const Formattable&,    char>));
        ASSERT( (bslfmt::BslFormattable<Formattable&&,         char>));
        ASSERT(!(bslfmt::BslFormattable<Formattable,        wchar_t>));
        ASSERT(!(bslfmt::BslFormattable<NotFormattable,        char>));
        ASSERT(!(bslfmt::BslFormattable<NoParse,               char>));
        ASSERT(!(bslfmt::BslFormattable<NotCopyable,           char>));
        ASSERT(!(bslfmt::BslFormattable<BadFormat,             char>));
# ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
        ASSERT( (std::default_initializable<std::formatter<StdOnly, char> >));
        ASSERT(!(bslfmt::BslFormattable<StdOnly,               char>));
# endif

        ASSERT( (bslfmt::BslFormattable<Box<Formattable>,         char>));
        ASSERT( (bslfmt::BslFormattable<Box<Box<Formattable> >,   char>));
        ASSERT(!(bslfmt::BslFormattable<Box<Formattable>,      wchar_t>));
        ASSERT(!(bslfmt::BslFormattable<Box<NotFormattable>,      char>));
        ASSERT(!(bslfmt::BslFormattable<Box<Box<NotFormattable> >, char>));
#else
        if (verbose) puts("\tConcepts are not supported; skipping.");
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        // 1. That basic functionality appears to work as advertised before
        //    beginning testing in earnest.
        //
        // Plan:
        // 1. Demonstrate a formattable and a not formattable type.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) puts("\nBREATHING TEST"
                          "\n==============");

#ifdef BSLS_COMPILERFEATURES_SUPPORT_CONCEPTS
        ASSERT( (bslfmt::BslFormattable<Formattable,    char>));
        ASSERT(!(bslfmt::BslFormattable<NotFormattable, char>));
#endif
      } break;
      default: {
        printf("WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

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
