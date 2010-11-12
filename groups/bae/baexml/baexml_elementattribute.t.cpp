// baexml_elementattribute.t.cpp                  -*-C++-*-

#include <baexml_elementattribute.h>

#include <baexml_namespaceregistry.h>
#include <baexml_prefixstack.h>

#include <bsl_iostream.h>
#include <bsl_cstdlib.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//

//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
                          << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\n";           \
                aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J    \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\t" << #M << ": " << M << "\n";         \
               aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J     \
                         << "\t" << #K << ": " << K << "\t" << #L << ": "  \
                         << L << "\t" << #M << ": " << M << "\t" << #N     \
                         << ": " << N << "\n"; aSsErT(1, #X, __LINE__); } }

// Allow compilation of individual test-cases (for test drivers that take a
// very long time to compile).  Specify '-DSINGLE_TEST=<testcase>' to compile
// only the '<testcase>' test case.
#define TEST_IS_ENABLED(num) (! defined(SINGLE_TEST) || SINGLE_TEST == (num))

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl; // Print ID and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;  // Quote ID literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << flush; // P(X) w/o '\n'
#define L_ __LINE__                                // current Line number
#define T_ bsl::cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

// The following function parses an XML-style attribute assignment in the form
// "qname='value'", where 'qname' can be a qualified name in the form,
// "prefix:localName".  The prefix (if any) must be registered in the specified
// 'prefixStack' object.  Either single or double quotes may be used to
// enclose the attribute value.  The parsed attribute is stored in the
// specified 'attribute' object of type 'baexml_ElementAttribute'.  Note that
// this function modifies the input string by inserting null characters,
// rather than copying the component parts into allocated memory.  This is a
// realistic interface for a function used within an XML parser that has
// already copied the XML stream into allocated memory.  Also note that this
// function does not interpret character escapes such as "&amp;".
//..
    int parseAttribute(baexml_ElementAttribute  *attribute,
                       char                     *attributeString,
                       const baexml_PrefixStack *prefixStack)
    {
//..
// First, find the end of the qualified name, i.e., the '=' character:
//..
        char *qname    = attributeString;
        char *equalPtr = bsl::strchr(qname, '=');
        if (0 == equalPtr) {
            return -1;
        }
//..
// Then find out which quote character is used to start the value string
//..
        char quote = *(equalPtr + 1);
        if (quote != '\'' && quote != '"') {
            return -1;
        }
//..
// The value string starts after the opening quote and extends until a
// matching quote:
//..
        char *value    = equalPtr + 2;
        char *endValue = bsl::strchr(value, quote);
        if (0 == endValue) {
            return -1;
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
        baexml_NamespaceRegistry registry;
        baexml_PrefixStack prefixes(&registry);
        int cal = prefixes.pushPrefix("cal",
                                  "http://www.bloomberg.com/schemas/calendar");
        int math = prefixes.pushPrefix("math",
                                      "http://www.bloomberg.com/schemas/math");
        int app = prefixes.pushPrefix("",  // Default namespace
                                      "http://www.bloomberg.com/schemas/app");
//..
// Now we can parse an attribute string and the 'baexml_ElementAttribute'
// object will provide detailed information about it.
//..
        char attrStr1[] = "cal:date='12-07-2006'";
        baexml_ElementAttribute attribute1;
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
        baexml_ElementAttribute attribute2;
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
// A 'baexml_ElementAttribute' does not need to be generated by parsing XML
// code.  If a specific facet of the object is set to a non-null value, then
// that value will be returned by the corresponding accessor even if it is
// inconsistent with the other values in the object.  For example, the
// following constructs a valid 'baexml_ElementAttribute' object, even though
// the prefix value does not agree with the qualified name:
//..
        baexml_ElementAttribute attribute3(&prefixes,
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
// feature.  It allows parsers to construct 'baexml_Attribute' objects before
// all information is known, e.g., before the namespace is registered with the
// prefix stack.  Consistency checking also reduces performance.
//..
        return 0;
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
