// balxml_formatter_prettyimpl.t.cpp                                  -*-C++-*-
#include <balxml_formatter_prettyimpl.h>

#include <bdlsb_memoutstreambuf.h>

#include <bdlt_date.h>

#include <bsla_maybeunused.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_review.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_string_view.h>

using namespace BloombergLP;
using namespace bsl;

#ifdef BDE_VERIFY
#pragma bde_verify push
#pragma bde_verify -TP26
#endif

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The subordinate component to 'balxml_formatter' under test provides an
// in-core, value-semantic attribute type, 'balxml::Formatter_PrettyImplState',
// and a utility 'struct', 'balxml::Formatter_PrettyImplUtil', that compose to
// provide a state machine capable of pretty-printing XML documents given a
// sequence of tokens to emit.
//
// The facilities provided by this subordinate component are fully tested in
// the test driver of the primary component.  This test driver only contains a
// breathing test for the facilities provided in this component.
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST

#ifdef BDE_VERIFY
#pragma bde_verify pop
#endif

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)

// ============================================================================
//                           ENTITIES FOR TESTING
// ----------------------------------------------------------------------------

#define ASSERT_EQ(X,Y) ASSERTV(X, Y, X == Y)
#define ASSERT_NE(X,Y) ASSERTV(X, Y, X != Y)
#define ASSERT_LT(X,Y) ASSERTV(X, Y, X <  Y)
#define ASSERT_LE(X,Y) ASSERTV(X, Y, X <= Y)
#define ASSERT_GT(X,Y) ASSERTV(X, Y, X >  Y)
#define ASSERT_GE(X,Y) ASSERTV(X, Y, X >= Y)

namespace {
namespace u {

                              // ===============
                              // struct TestUtil
                              // ===============

struct TestUtil {
    // CLASS METHODS
    static bsl::string_view view(const bdlsb::MemOutStreamBuf& streamBuffer);
        // Return a string view to the contents of the specified
        // 'streamBuffer'.

    static bsl::string_view view(const char *cString);
        // Return a string view to the specified 'cString'.
};

// CLASS METHODS
bsl::string_view TestUtil::view(const bdlsb::MemOutStreamBuf& streamBuffer)
{
    return bsl::string_view(streamBuffer.data(), streamBuffer.length());
}

bsl::string_view TestUtil::view(const char *cString)
{
    return bsl::string_view(cString);
}

}  // close namespace u
}  // close unnamed namespace

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int test = argc > 1 ? atoi(argv[1]) : 0;

    BSLA_MAYBE_UNUSED const bool             verbose = argc > 2;
    BSLA_MAYBE_UNUSED const bool         veryVerbose = argc > 3;
    BSLA_MAYBE_UNUSED const bool     veryVeryVerbose = argc > 4;
    BSLA_MAYBE_UNUSED const bool veryVeryVeryVerbose = argc > 5;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The facilities in this component are sufficiently functional to
        //:   enable comprehensive testing in the primary, non-subordinate
        //:   component.
        //
        // Plan:
        //: 1 Write two documents to the same stream, resetting the state
        //:   machine in between documents, such that  the first document shows
        //:   the result for each of its print operations, while the second
        //:   document shows the result as a whole.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << bsl::endl
                      << "BREATHING TEST" << bsl::endl
                      << "==============" << bsl::endl;
        }

        typedef u::TestUtil Util;

        bdlsb::MemOutStreamBuf streambuf;
        bsl::ostream           stream(&streambuf);

        typedef balxml::Formatter_PrettyImplState Obj;
        typedef balxml::Formatter_PrettyImplUtil  ObjUtil;
        typedef balxml::FormatterWhitespaceType   WhitespaceType;

        Obj obj;
        obj.spacesPerLevel() = 4;
        obj.wrapColumn()     = 80;

        ObjUtil::addHeader(stream, &obj, "UTF-8");
        ASSERT_EQ(Util::view(streambuf),
                  Util::view("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"));

        streambuf.reset();
        ObjUtil::openElement(stream, &obj, "Fruits");
        ASSERT_EQ(Util::view(streambuf), Util::view("<Fruits"));

        streambuf.reset();
        ObjUtil::openElement(stream, &obj, "Oranges");
        ASSERT_EQ(Util::view(streambuf), Util::view(">\n    <Oranges"));

        streambuf.reset();
        ObjUtil::addAttribute(stream, &obj, "farm", "Frances' Orchard");
        ASSERT_EQ(Util::view(streambuf),
                  Util::view(" farm=\"Frances&apos; Orchard\""));

        streambuf.reset();
        ObjUtil::addAttribute(stream, &obj, "size", 3.5);
        ASSERT_EQ(Util::view(streambuf), Util::view(" size=\"3.5\""));

        streambuf.reset();
        ObjUtil::addNewline(stream, &obj);
        ASSERT_EQ(Util::view(streambuf), Util::view(">\n"));

        streambuf.reset();
        ObjUtil::openElement(stream, &obj, "pickDate");
        ASSERT_EQ(Util::view(streambuf), Util::view("        <pickDate"));

        streambuf.reset();
        ObjUtil::addData(stream, &obj, bdlt::Date(2004, 8, 31));
        ASSERT_EQ(Util::view(streambuf), Util::view(">2004-08-31"));

        streambuf.reset();
        ObjUtil::closeElement(stream, &obj, "pickDate");
        ASSERT_EQ(Util::view(streambuf), Util::view("</pickDate>\n"));

        streambuf.reset();
        ObjUtil::openElement(
            stream, &obj, "Features", WhitespaceType::e_NEWLINE_INDENT);
        ASSERT_EQ(Util::view(streambuf), Util::view("        <Features"));

        streambuf.reset();
        ObjUtil::addListData(stream, &obj, "Juicy");
        ASSERT_EQ(Util::view(streambuf),
                  Util::view(">\n" "            Juicy"));

        streambuf.reset();
        ObjUtil::addListData(stream, &obj, "Round");
        ASSERT_EQ(Util::view(streambuf), Util::view(" Round"));

        streambuf.reset();
        ObjUtil::addListData(
            stream, &obj, "Also shown on Florida license plates");
        ASSERT_EQ(Util::view(streambuf),
                  Util::view(" Also shown on Florida license plates"));

        streambuf.reset();
        ObjUtil::closeElement(stream, &obj, "Features");
        ASSERT_EQ(Util::view(streambuf),
                  Util::view("\n" "        </Features>\n"));

        streambuf.reset();
        ObjUtil::addComment(stream, &obj, "There's no more data for Oranges");
        ASSERT_EQ(
            Util::view(streambuf),
            Util::view("        <!-- There's no more data for Oranges -->\n"));

        streambuf.reset();
        ObjUtil::closeElement(stream, &obj, "Oranges");
        ASSERT_EQ(Util::view(streambuf), Util::view("    </Oranges>\n"));

        streambuf.reset();
        ObjUtil::closeElement(stream, &obj, "Fruits");
        ASSERT_EQ(Util::view(streambuf), Util::view("</Fruits>\n"));

        streambuf.reset();
        bsl::ostream stream2(&streambuf);

        ObjUtil::reset(&obj);

        ObjUtil::addHeader(stream2, &obj, "iso8896");
        ObjUtil::openElement(stream2, &obj, "Grains");
        ObjUtil::addBlankLine(stream2, &obj);

        stream2 << "<someTag> anything </someTag>" << bsl::endl;
        ObjUtil::closeElement(stream2, &obj, "Grains");

        ASSERT_EQ(Util::view(streambuf),
                  Util::view("<?xml version=\"1.0\" encoding=\"iso8896\" ?>\n"
                             "<Grains>\n"
                             "\n"
                             "<someTag> anything </someTag>\n"
                             "</Grains>\n"));

      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = "
                  << testStatus
                  << "."
                  << bsl::endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
