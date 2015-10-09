// balxml_util.t.cpp                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_util.h>

#include <bslim_testutil.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                Overview
//                                --------
// The basic idea of testing balxml::Util::extractNamespaceFromXsd to test Lock
// and bdlqq::QLock is to create thread poll and manipulate various test data
// in parallel, then to check the the integrity of data is preserved.
// ----------------------------------------------------------------------------
// CLASS balxml::Util
// [ 3] Testing various schemas given by streams
// [ 2] Testing various schemas given by strings
// [ 1] BREATHING TEST and USAGE EXAMPLE
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

#define NL "\n"

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------
static  const char * targetNs = "http://localhost:2000/calc.wsdl";

static  const char * goodSchema =
"<?xml version='1.0' encoding='UTF-8'?>"
" <schema targetNamespace='http://localhost:2000/calc.wsdl' "
"        elementFormDefault='qualified'"
"        xmlns='http://www.w3.org/2001/XMLSchema'"
"        xmlns:xs='http://www.w3.org/2001/XMLSchema'"
"        xmlns:bdem='http://bloomberg.com/schemas/bdem'"
"        xmlns:m_bascalc='http://localhost:2000/calc.wsdl'"
"        xmlns:tns='http://localhost:2000/calc.wsdl'"
"   <xs:complexType name='Options'>"
"   <xs:sequence>"
"      <xs:element name='MaxDepth' type='xs:int'"
"                  minOccurs='0' maxOccurs='1'"
"                  default='32'"
"                  bdem:allowsDirectManipulation='0'>"
"      </xs:element>"
"    </xs:sequence>"
"  </xs:complexType>"
"  <complexType name='Configuration'>"
"    <sequence>"
"      <element name='Options' type='m_bascalc:Options'/>"
"    </sequence>"
"  </complexType>"
"  <element name='Configuration' type='m_bascalc:Configuration'/>"
"</schema>"
;

static  const char * badSchema1 =
"<?xml version='1.0' encoding='UTF-8'?>"
" <schema elementFormDefault='qualified'"
"        xmlns='http://www.w3.org/2001/XMLSchema'"
"        xmlns:xs='http://www.w3.org/2001/XMLSchema'"
"        xmlns:bdem='http://bloomberg.com/schemas/bdem'"
"        xmlns:m_bascalc='http://localhost:2000/calc.wsdl'"
"        xmlns:tns='http://localhost:2000/calc.wsdl'"
"   <xs:complexType name='Options'>"
"   <xs:sequence>"
"      <xs:element name='MaxDepth' type='xs:int'"
"                  minOccurs='0' maxOccurs='1'"
"                  default='32'"
"                  bdem:allowsDirectManipulation='0'>"
"      </xs:element>"
"    </xs:sequence>"
"  </xs:complexType>"
"  <complexType name='Configuration'>"
"    <sequence>"
"      <element name='Options' type='m_bascalc:Options'/>"
"    </sequence>"
"  </complexType>"
"  <element name='Configuration' type='m_bascalc:Configuration'/>"
"</schema>"
;

static  const char * badSchema2 = "";

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

struct TestData {
    const char *d_schema;
    bool        d_result;
    const char *d_tns;
};

TestData data[] = {
    {
        "<schema elementFormDefault='qualified'></schema>",
        false,
        ""
    },
    {
        "<schema targetNamespace='zzz'></schema>",
        true,
        "zzz"
    },
    {
        "<nonschema targetNamespace='zzz'></nonschema>",
        true,
        "zzz"
    },
    {
        "<!-- targetNamespace='yyy' -->"
        "<schema targetNamespace='zzz'></schema>",
        true,
        "zzz"
    },
    {
        "<schema targetNamespace='zzz'",
        true,
        "zzz"
    }
};


// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test)
    {
    case 0:  // Zero is always the leading case.
    case 3:
        {
            // ----------------------------------------------------------------
            // [ 2] Testing various schemas given by strings
            // ----------------------------------------------------------------

            if (verbose) bsl::cout << bsl::endl
                << "Testing various schemas given by streams" << bsl::endl
                << "========================================" << bsl::endl;


            for (int i=0; i < sizeof(data)/sizeof(data[0]); ++i) {

                bsl::string xsd(data[i].d_schema);
                bsl::string tnsExpected(data[i].d_tns);
                bsl::string tnsReal;

                bsl::istringstream is(xsd);

                bool rc = balxml::Util::extractNamespaceFromXsd(is.rdbuf(),
                                                               &tnsReal);

                LOOP3_ASSERT(i, rc, data[i].d_result, rc == data[i].d_result);

                if (rc) {
                    LOOP3_ASSERT(i,
                                 tnsExpected,
                                 tnsReal,
                                 tnsExpected == tnsReal);
                }
            }
        }
        break;

    case 2:
        {
            // ----------------------------------------------------------------
            // [ 2] Testing various schemas given by strings
            // ----------------------------------------------------------------

            if (verbose) bsl::cout << bsl::endl
                << "Testing various schemas given by strings" << bsl::endl
                << "========================================" << bsl::endl;


            for (int i=0; i < sizeof(data)/sizeof(data[0]); ++i) {

                bsl::string xsd(data[i].d_schema);
                bsl::string tnsExpected(data[i].d_tns);
                bsl::string tnsReal;

                bool rc = balxml::Util::extractNamespaceFromXsd(xsd, &tnsReal);

                LOOP3_ASSERT(i, rc, data[i].d_result, rc == data[i].d_result);

                if (rc) {
                    LOOP3_ASSERT(i,
                                 tnsExpected,
                                 tnsReal,
                                 tnsExpected == tnsReal);
                }
            }
        }
        break;

    case 1:
        {
            // ----------------------------------------------------------------
            // BREATHING TEST and USAGE EXAMPLE
            // ----------------------------------------------------------------

            if (verbose) bsl::cout << bsl::endl
                << "BREATHING TEST and USAGE EXAMPLE" << bsl::endl
                << "================================" << bsl::endl;

            bsl::string strGood (goodSchema);
            bsl::string strBad1 (badSchema1);

            bsl::string resultNs;
            bool rc;

            rc= balxml::Util::extractNamespaceFromXsd(strGood, &resultNs);
            ASSERT (rc);
            ASSERT (resultNs == targetNs);

            resultNs.clear();
            rc = balxml::Util::extractNamespaceFromXsd(strBad1, &resultNs);
            ASSERT (!rc);
        }
        break;
    default:
        {
            bsl::cerr << "WARNING: CASE `"
                << test
                << "' NOT FOUND."
                << bsl::endl;
            testStatus = -1;
        }
    }

    if (testStatus > 0)
    {
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
