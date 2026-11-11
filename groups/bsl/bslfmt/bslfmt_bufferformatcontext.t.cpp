// bslfmt_bufferformatcontext.t.cpp                                   -*-C++-*-
#include <bslfmt_bufferformatcontext.h>

#include <bslfmt_formatterintegral.h>
#include <bslfmt_mockformatcontext.h>
#include <bslfmt_mockparsecontext.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bslmf_issame.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_libraryfeatures.h>

#include <bslstl_string.h>
#include <bslstl_utility.h>

#include <stdio.h>
#include <stdlib.h>  // `atoi`

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is a format context that appends its output to a
// string and that forwards its argument lookups to another format context.
// The test driver verifies the types the class defines, that what is written
// through the iterator returned by `out` is appended to the buffer supplied at
// construction, and that `arg` returns the arguments of the context supplied
// at construction, including when that context is itself a
// `BufferFormatContext`.
// ----------------------------------------------------------------------------
// CLASS `BufferFormatContext`
// TYPES
// [ 2] char_type
// [ 2] iterator
// [ 2] Arg
//
// CREATORS
// [ 3] BufferFormatContext(basic_string<t_CHAR> *, const t_FORMAT_CTX *);
//
// MANIPULATORS
// [ 3] void advance_to(iterator it);
// [ 3] iterator out();
//
// ACCESSORS
// [ 4] Arg arg(std::size_t id) const;
// ----------------------------------------------------------------------------
// [ 5] USAGE EXAMPLE
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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace u {

                            // ================
                            // struct IntReader
                            // ================

/// This visitor records whether the formatting argument it visits holds no
/// value, or an `int`, and the value of that `int`.
struct IntReader {
    // DATA
    bool d_isMonostate;  // `true` if a `bsl::monostate` was visited
    bool d_isInt;        // `true` if an `int` was visited
    int  d_value;        // the visited `int`

    // CREATORS

    /// Create a visitor that has visited nothing.
    IntReader()
    : d_isMonostate(false)
    , d_isInt(false)
    , d_value(0)
    {
    }

    // MANIPULATORS

    /// Record that an argument holding no value was visited.
    void operator()(bsl::monostate)
    {
        d_isMonostate = true;
    }

    /// Record that an argument holding the specified `value` was visited.
    void operator()(int value)
    {
        d_isInt = true;
        d_value = value;
    }

    /// Ignore an argument of any other type.
    template <class t_TYPE>
    void operator()(const t_TYPE&)
    {
    }
};

/// Return a `bsl::basic_string<t_CHAR>` holding the characters of the
/// specified null-terminated `text`, each converted to `t_CHAR`.
template <class t_CHAR>
bsl::basic_string<t_CHAR> widen(const char *text)
{
    bsl::basic_string<t_CHAR> result;
    for (; *text; ++text) {
        result.push_back(static_cast<t_CHAR>(*text));
    }
    return result;
}

/// Verify that the types defined by `bslfmt::BufferFormatContext` for the
/// (template parameter) `t_CHAR` character type are as expected.
template <class t_CHAR>
void testTypes()
{
    typedef bslfmt::MockFormatContext<t_CHAR>           Source;
    typedef bslfmt::BufferFormatContext<t_CHAR, Source> Obj;
    typedef bslfmt::BufferFormatContext<t_CHAR, Obj>    NestedObj;
    typedef bsl::back_insert_iterator<bsl::basic_string<t_CHAR> >
                                                            Iterator;

    ASSERT((bsl::is_same<t_CHAR,   typename Obj::char_type>::value));
    ASSERT((bsl::is_same<Iterator, typename Obj::iterator>::value));

    ASSERT((bsl::is_same<typename Source::Arg, typename Obj::Arg>::value));

    ASSERT((bsl::is_same<typename Source::Arg,
                         typename NestedObj::Arg>::value));
}

/// Verify that what is written to a `bslfmt::BufferFormatContext` for the
/// (template parameter) `t_CHAR` character type is appended to its buffer.
template <class t_CHAR>
void testOutput()
{
    typedef bslfmt::MockFormatContext<t_CHAR>           Source;
    typedef bslfmt::BufferFormatContext<t_CHAR, Source> Obj;
    typedef bsl::basic_string<t_CHAR>                   String;

    const String ABC = widen<t_CHAR>("abc");
    const String DE  = widen<t_CHAR>("de");

    Source source(0);

    if (veryVerbose) printf("\t\tEmpty buffer.\n");
    {
        String buffer;
        Obj    mX(&buffer, &source);

        ASSERT(buffer.empty());

        typename Obj::iterator out = mX.out();
        out = bsl::copy(ABC.begin(), ABC.end(), out);
        mX.advance_to(out);

        ASSERT(ABC == buffer);

        // A fresh iterator continues where the previous one stopped.

        out = mX.out();
        out = bsl::copy(DE.begin(), DE.end(), out);

        ASSERT(widen<t_CHAR>("abcde") == buffer);
    }

    if (veryVerbose) printf("\t\tNon-empty buffer.\n");
    {
        String buffer(DE);
        Obj    mX(&buffer, &source);

        ASSERT(DE == buffer);

        typename Obj::iterator out = mX.out();
        *out = static_cast<t_CHAR>('x');
        ++out;
        mX.advance_to(out);

        ASSERT(widen<t_CHAR>("dex") == buffer);
    }

    if (veryVerbose) printf("\t\t`advance_to` writes nothing.\n");
    {
        String buffer(ABC);
        Obj    mX(&buffer, &source);

        mX.advance_to(mX.out());
        mX.advance_to(mX.out());

        ASSERT(ABC == buffer);
    }
}

/// Verify that `arg` of a `bslfmt::BufferFormatContext` for the (template
/// parameter) `t_CHAR` character type returns the arguments of the context
/// supplied at construction.
template <class t_CHAR>
void testArg()
{
    typedef bslfmt::MockFormatContext<t_CHAR>           Source;
    typedef bslfmt::BufferFormatContext<t_CHAR, Source> Obj;
    typedef bslfmt::BufferFormatContext<t_CHAR, Obj>    NestedObj;
    typedef bsl::basic_string<t_CHAR>                   String;

    static const struct {
        int  d_line;
        int  d_id;
        bool d_isInt;
        int  d_value;
    } DATA[] = {
        //LINE  ID  IS_INT  VALUE
        //----  --  ------  -----
        { L_,   0,  true,     10 },
        { L_,   1,  true,     20 },
        { L_,   2,  true,     30 },
        { L_,   3,  false,     0 },
        { L_,   9,  false,     0 },
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    Source source(10, 20, 30);

    String          buffer;
    const Obj       X(&buffer, &source);
    const NestedObj Y(&buffer, &X);

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int  LINE   = DATA[ti].d_line;
        const int  ID     = DATA[ti].d_id;
        const bool IS_INT = DATA[ti].d_isInt;
        const int  VALUE  = DATA[ti].d_value;

        if (veryVeryVerbose) { T_ T_ P_(LINE) P(ID) }

        {
            typename Obj::Arg arg = X.arg(ID);

            IntReader reader;
            arg.visit(reader);

            ASSERTV(LINE, IS_INT == reader.d_isInt);
            ASSERTV(LINE, IS_INT != reader.d_isMonostate);
            ASSERTV(LINE, reader.d_value, VALUE == reader.d_value);
        }

        {
            typename NestedObj::Arg arg = Y.arg(ID);

            IntReader reader;
            arg.visit(reader);

            ASSERTV(LINE, IS_INT == reader.d_isInt);
            ASSERTV(LINE, IS_INT != reader.d_isMonostate);
            ASSERTV(LINE, reader.d_value, VALUE == reader.d_value);
        }
    }

    ASSERT(buffer.empty());
}

}  // close namespace u

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.

namespace usage_example_1 {

///Example 1: Buffering Formatted Output
///- - - - - - - - - - - - - - - - - - -
// Suppose we wish to render an integer whose width is supplied by a format
// argument, and examine the result before writing it to the actual output.
//
// First, we parse a specification that takes the width from the first
// argument:
// ```
    void run()
    {
        bslfmt::MockParseContext<char> parseContext("{}}", 1);

        bsl::formatter<int, char> formatter;
        parseContext.advance_to(formatter.parse(parseContext));
// ```
// Then, we create the format context that holds the arguments, the buffer, and
// a buffer context that writes to the buffer and obtains its arguments from
// the format context:
// ```
        typedef bslfmt::MockFormatContext<char>                  FormatContext;
        typedef bslfmt::BufferFormatContext<char, FormatContext> Context;

        FormatContext formatContext(5);
        bsl::string   buffer;
        Context       bufferContext(&buffer, &formatContext);
// ```
// Now, we format a value into the buffer context:
// ```
        bufferContext.advance_to(
                           bsl::as_const(formatter).format(42, bufferContext));
// ```
// Finally, we observe that the value has been written to the buffer, padded to
// the width that the format context supplied:
// ```
        ASSERT("   42" == buffer);
    }
// ```

}  // close namespace usage_example_1

#endif  // !BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    (void) veryVerbose;
    (void) veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: No global memory is allocated after `main` starts.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT
    (void) test;
    if (verbose) printf("Component is not defined when the standard library "
                        "provides C++23 `std::format` ranges support.\n");
#else
    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
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
      case 4: {
        // --------------------------------------------------------------------
        // TESTING `arg`
        //
        // Concerns:
        // 1. `arg` returns the argument having the given index held by the
        //    context supplied at construction.
        //
        // 2. `arg` returns an argument holding no value for an index for
        //    which the context supplied at construction has no argument.
        //
        // 3. The arguments are forwarded through any number of nested
        //    `BufferFormatContext` objects.
        //
        // 4. `arg` is a `const` method that does not write to the buffer.
        //
        // 5. The above holds for both `char` and `wchar_t`.
        //
        // Plan:
        // 1. Using the table-driven technique, call `arg` on a `const` object
        //    whose source context holds three `int` arguments, for indices
        //    both in and out of range, and visit the result to verify its
        //    value.  Repeat for a `const` object whose source context is the
        //    first object.  Verify that the buffer remains empty.  Do this
        //    for both `char` and `wchar_t`.  (C-1..5)
        //
        // Testing:
        //   Arg arg(std::size_t id) const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `arg`"
                            "\n=============\n");

        if (verbose) printf("\t`char`\n");
        u::testArg<char>();

        if (verbose) printf("\t`wchar_t`\n");
        u::testArg<wchar_t>();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CREATOR AND MANIPULATORS
        //
        // Concerns:
        // 1. Constructing an object does not modify the buffer.
        //
        // 2. Characters written through the iterator returned by `out` are
        //    appended to the buffer supplied at construction, after any
        //    characters it already held.
        //
        // 3. An iterator obtained by a later call to `out` continues where
        //    the previous one stopped.
        //
        // 4. `advance_to` does not modify the buffer.
        //
        // 5. The above holds for both `char` and `wchar_t`.
        //
        // Plan:
        // 1. Construct objects for empty and non-empty buffers, write through
        //    iterators returned by `out`, call `advance_to`, and verify the
        //    contents of the buffer after each step.  Do this for both `char`
        //    and `wchar_t`.  (C-1..5)
        //
        // Testing:
        //   BufferFormatContext(basic_string<t_CHAR> *, const t_FORMAT_CTX *);
        //   void advance_to(iterator it);
        //   iterator out();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CREATOR AND MANIPULATORS"
                            "\n================================\n");

        if (verbose) printf("\t`char`\n");
        u::testOutput<char>();

        if (verbose) printf("\t`wchar_t`\n");
        u::testOutput<wchar_t>();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING TYPES
        //
        // Concerns:
        // 1. `char_type` is the character type.
        //
        // 2. `iterator` is a back insert iterator into a string of the
        //    character type.
        //
        // 3. `Arg` is the argument type of the source context, including when
        //    the source context is itself a `BufferFormatContext`.
        //
        // 4. The above holds for both `char` and `wchar_t`.
        //
        // Plan:
        // 1. Compare each type with its expected type using `bsl::is_same`,
        //    for both `char` and `wchar_t`.  (C-1..4)
        //
        // Testing:
        //   char_type
        //   iterator
        //   Arg
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TYPES"
                            "\n=============\n");

        if (verbose) printf("\t`char`\n");
        u::testTypes<char>();

        if (verbose) printf("\t`wchar_t`\n");
        u::testTypes<wchar_t>();
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Write to an object, and verify the contents of its buffer and
        //    the value of an argument it forwards.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        typedef bslfmt::MockFormatContext<char>           Source;
        typedef bslfmt::BufferFormatContext<char, Source> Obj;

        Source      source(7);
        bsl::string buffer;
        Obj         mX(&buffer, &source);  const Obj& X = mX;

        Obj::iterator out = mX.out();
        *out = 'a';
        ++out;
        mX.advance_to(out);

        ASSERT("a" == buffer);

        Obj::Arg     arg = X.arg(0);
        u::IntReader reader;
        arg.visit(reader);

        ASSERT(reader.d_isInt);
        ASSERT(7 == reader.d_value);
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }
#endif  // !BSLS_LIBRARYFEATURES_HAS_CPP23_RANGE_FORMAT

    // CONCERN: In no case does memory come from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-Zero test status = %d.\n", testStatus);
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
