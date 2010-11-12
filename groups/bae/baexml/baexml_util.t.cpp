// baexml_util.t.cpp  -*-C++-*-

#include <baexml_util.h>

#include <bsl_cstring.h>     // strlen()
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The basic idea of testing baexml_Util::extractNamespaceFromXsd
// to test Lock and bcemt_QLock is to create
// thread poll and manipulate various test data in parallel, then to
// check the the integrity of data is preserved.
//-----------------------------------------------------------------------------
// CLASS baexml_Util
// [ 3] Testing various schemas given by streams
// [ 2] Testing various schemas given by strings
// [ 1] BREATHING TEST and USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t"  \
                          << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\n";\
               aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
                                              // Print identifier and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
                                              // Quote identifier literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
                                              // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------

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


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

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

                bool rc = baexml_Util::extractNamespaceFromXsd(is.rdbuf(),
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

                bool rc = baexml_Util::extractNamespaceFromXsd(xsd, &tnsReal);

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

            rc= baexml_Util::extractNamespaceFromXsd(strGood, &resultNs);
            ASSERT (rc);
            ASSERT (resultNs == targetNs);

            resultNs.clear();
            rc = baexml_Util::extractNamespaceFromXsd(strBad1, &resultNs);
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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
