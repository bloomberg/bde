// balxml_elementattribute.t.cpp                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_elementattribute.h>

#include <bslim_testutil.h>

#include <balxml_namespaceregistry.h>
#include <balxml_prefixstack.h>

#include <bsl_iostream.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>  // for bsl::strchr

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//
//

// ----------------------------------------------------------------------------

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

// Allow compilation of individual test-cases (for test drivers that take a
// very long time to compile).  Specify '-DSINGLE_TEST=<testcase>' to compile
// only the '<testcase>' test case.
#define TEST_IS_ENABLED(num) (! defined(SINGLE_TEST) || SINGLE_TEST == (num))

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

// ============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                    CLASSES FOR TESTING USAGE EXAMPLES
// ----------------------------------------------------------------------------

// The following function parses an XML-style attribute assignment in the form
// "qname='value'", where 'qname' can be a qualified name in the form,
// "prefix:localName".  The prefix (if any) must be registered in the specified
// 'prefixStack' object.  Either single or double quotes may be used to
// enclose the attribute value.  The parsed attribute is stored in the
// specified 'attribute' object of type 'balxml::ElementAttribute'.  Note that
// this function modifies the input string by inserting null characters,
// rather than copying the component parts into allocated memory.  This is a
// realistic interface for a function used within an XML parser that has
// already copied the XML stream into allocated memory.  Also note that this
// function does not interpret character escapes such as "&amp;".
//..
    int parseAttribute(balxml::ElementAttribute  *attribute,
                       char                      *attributeString,
                       const balxml::PrefixStack *prefixStack)
    {
//..
// First, find the end of the qualified name, i.e., the '=' character:
//..
        char *qname    = attributeString;
        char *equalPtr = bsl::strchr(qname, '=');
        if (0 == equalPtr) {
            return -1;                                                // RETURN
        }
//..
// Then find out which quote character is used to start the value string
//..
        char quote = *(equalPtr + 1);
        if (quote != '\'' && quote != '"') {
            return -1;                                                // RETURN
        }
//..
// The value string starts after the opening quote and extends until a
// matching quote:
//..
        char *value    = equalPtr + 2;
        char *endValue = bsl::strchr(value, quote);
        if (0 == endValue) {
            return -1;                                                // RETURN
        }
//..
// Once we have successfully parsed the string, chop it into pieces by putting
// a null terminator at the end of the qualified name and at the end of the
// value string:
//..
        *equalPtr = '\0';    // Terminate qualified name
        *endValue = '\0';    // Terminate value string
//..
// Use the prefix stack, qname, and value to set the attribute object.  All
// other arguments are defaulted and will be computed as needed.
//..
        attribute->reset(prefixStack, qname, value);
        return 0;
    }
//..
// Before calling the 'parseAttribute' function, it is necessary to create a
// namespace registry and prefix stack, as well as to register one or more
// prefixes:
//..
    int usageExample()
    {
        balxml::NamespaceRegistry registry;
        balxml::PrefixStack prefixes(&registry);
        int cal = prefixes.pushPrefix("cal",
                                  "http://www.bloomberg.com/schemas/calendar");
        int math = prefixes.pushPrefix("math",
                                      "http://www.bloomberg.com/schemas/math");
        int app = prefixes.pushPrefix("",  // Default namespace
                                      "http://www.bloomberg.com/schemas/app");
//..
// Now we can parse an attribute string and the 'balxml::ElementAttribute'
// object will provide detailed information about it.
//..
        char attrStr1[] = "cal:date='12-07-2006'";
        balxml::ElementAttribute attribute1;
        ASSERT(attribute1.isNull());
        int ret = parseAttribute(&attribute1, attrStr1, &prefixes);

        ASSERT(! attribute1.isNull());
        ASSERT(0 == bsl::strcmp("cal:date", attribute1.qualifiedName()));
        ASSERT(0 == bsl::strcmp("12-07-2006", attribute1.value()));
        ASSERT(0 == bsl::strcmp("date", attribute1.localName()));
        ASSERT(0 == bsl::strcmp("cal", attribute1.prefix()));
        ASSERT(cal == attribute1.namespaceId());
        ASSERT(0 == bsl::strcmp("http://www.bloomberg.com/schemas/calendar",
                                attribute1.namespaceUri()));
        ASSERT(0 == attribute1.flags());
//..
// Results are slightly different when the attribute name has no prefix:
//..
        char attrStr2[] = "name=\"Bloomberg, L.P.\"";
        balxml::ElementAttribute attribute2;
        ASSERT(attribute2.isNull());
        ret = parseAttribute(&attribute2, attrStr2, &prefixes);

        ASSERT(! attribute2.isNull());
        ASSERT(0 == bsl::strcmp("name", attribute2.qualifiedName()));
        ASSERT(0 == bsl::strcmp("Bloomberg, L.P.", attribute2.value()));
        ASSERT(0 == bsl::strcmp("name", attribute2.localName()));
//..
// As per the XML namespace standard, an attribute with no namespace prefix
// does NOT inherit the default namespace but rather has NO namespace:
//..
        ASSERT(0 == bsl::strcmp("", attribute2.prefix()));
        ASSERT(-1 == attribute2.namespaceId());
        ASSERT(0 == bsl::strcmp("", attribute2.namespaceUri()));
        ASSERT(0 == attribute2.flags());
//..
// A 'balxml::ElementAttribute' does not need to be generated by parsing XML
// code.  If a specific facet of the object is set to a non-null value, then
// that value will be returned by the corresponding accessor even if it is
// inconsistent with the other values in the object.  For example, the
// following constructs a valid 'balxml::ElementAttribute' object, even though
// the prefix value does not agree with the qualified name:
//..
        balxml::ElementAttribute attribute3(&prefixes,
                                           "math:product", "4.5", "cal");
        ASSERT(! attribute3.isNull());
//..
// There is no consistency checking, and the mismatched prefix is simply
// returned by the accessor:
//..
        ASSERT(0 == bsl::strcmp("math:product", attribute3.qualifiedName()));
        ASSERT(0 == bsl::strcmp("product", attribute3.localName()));
        ASSERT(0 == bsl::strcmp("cal", attribute3.prefix()));
//..
// Note that the ability to create inconsistent objects is a deliberate
// feature.  It allows parsers to construct 'balxml::Attribute' objects before
// all information is known, e.g., before the namespace is registered with the
// prefix stack.  Consistency checking also reduces performance.
//..
        return 0;
    }
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nUSAGE EXAMPLE"
                               << "\n=============" << bsl::endl;

        usageExample();

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nBREATHING TEST"
                               << "\n==============" << bsl::endl;

      } break;

      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
