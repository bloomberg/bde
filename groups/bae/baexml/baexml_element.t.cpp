// baexml_element.t.cpp  -*-C++-*-
#include <baexml_element.h>

#include <bcema_blob.h>
#include <bcema_blobutil.h>
#include <bcema_pooledblobbufferfactory.h>
#include <bcesb_blobstreambuf.h>

#include <bsl_cstring.h>     // strlen()
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
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
//                          HELPER FUNCTIONS
//-----------------------------------------------------------------------------

void makeBlob(bcema_Blob *data, const bsl::string& str)
{
    bcesb_OutBlobStreamBuf osb(data);
    bsl::ostream           os(&osb);

    os << str << bsl::flush;
}

bsl::string blobToString(const bcema_Blob& data)
{
    bcesb_InBlobStreamBuf isb(&data);
    bsl::istream          is(&isb);
    bsl::string           result;

    bsl::getline(is, result, '\0');
    return result;
}

void printElement(bsl::ostream&         stream,
                  const baexml_Element& element)
{
    bcema_Blob data;
    element.extractData(&data);
    {
        bcesb_InBlobStreamBuf isb(&data);
        stream << (&isb) << bsl::flush;
    }
}

bsl::ostream& operator<<(bsl::ostream& strm, const baexml_Element& elem)
{
    printElement(strm, elem);
    return strm;
}

void printElementDetailed(bsl::ostream&                stream,
                          const baexml_ElementConstRef& element,
                          const bsl::string&           name)
{
    bcema_Blob data;
    element.extractData(&data);
    {
        bcesb_InBlobStreamBuf isb(&data);
        stream << '\n' << name << " =\n" << (&isb) << bsl::endl;
    }
    int numSubElements = element.numSubElements();
    for (int i = 0; i < numSubElements; ++i) {
        bsl::stringstream ss;
        ss << name << "[" << i << "]";
        bsl::string subElementName = ss.str();
        printElementDetailed(stream, element[i], subElementName);
    }
}

//=============================================================================
//                           USAGE EXAMPLE
//-----------------------------------------------------------------------------

// The following snippets of code illustrate the usage of this component.  The
// main usage of this component is to manipulate XML elements or rearrange
// them.  This example will demonstrate converting an XML document in 'bas' XML
// format to SOAP format.
//
// Suppose we receive an XML response that looks like the following:
//..
    // Listing 1
    // ---------
    const char *portResponseMsg =
        "<?xml version='1.0' encoding='utf-8'?>"
        "<PortResponse>"
            "<OpName>"
                "<MyResponseElement>"
                    "<SomeData>"
                        "<FieldValue>qwerty</FieldValue>"
                    "</SomeData>"
                    "<OtherData>abcdef</OtherData>"
                "</MyResponseElement>"
            "</OpName>"
        "</PortResponse>";
//..
// This is how a typical response looks like from a 'bas' service that was
// generated from a WSDL file using the 'bas_codegen.pl' service generator.
// The corresponding SOAP response looks something like:
//..
    // Listing 2
    // ---------
    const char *soapEnvelopeMsg =
        "<soap:Envelope"
                   " xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'"
                   " xmlns:xsd='http://www.w3.org/2001/XMLSchema'"
                   " xmlns:soap='http://schemas.xmlsoap.org/soap/envelope/'>"
            "<soap:Body>"
                "<MyResponseElement xmlns='http://myservice/ns'>"
                    "<SomeData>"
                        "<FieldValue>qwerty</FieldValue>"
                    "</SomeData>"
                    "<OtherData>abcdef</OtherData>"
                "</MyResponseElement>"
            "</soap:Body>"
        "</soap:Envelope>";
//..
// In order to convert from 'Listing 1' to 'Listing 2', we need to extract the
// 'MyResponseElement' element and insert it as a sub-element of the
// 'soap:Body' element.  As an additional requirement, we need to add the
// 'xmlns' attribute to the 'MyResponseElement' element.
//
// This component allows us to efficiently and intuitively implement this
// conversion.  The reason it is efficient is because the 'baexml_Element'
// class is blob-based, so there is no deep copy of the underlying data.
//
// In this example, we will use the following "scratch-data" to aid the
// conversion:
//..
    // Listing 3
    // ---------
    const char *scratchDataText =
        "<ScratchData>"
            "<soap:Envelope"
                   " xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance'"
                   " xmlns:xsd='http://www.w3.org/2001/XMLSchema'"
                   " xmlns:soap='http://schemas.xmlsoap.org/soap/envelope/'>"
                "<soap:Body>"
                "</soap:Body>"
            "</soap:Envelope>"
            "<SomeResponse xmlns='http://myservice/ns'></SomeResponse>"
            "<MyResponseElement xmlns='http://myservice/ns'>"
        "</MyResponseElement>"
            "<OtherResponse xmlns='http://myservice/ns'></OtherResponse>"
            "<FourthResponse xmlns='http://myservice/ns'></FourthResponse>"
        "</ScratchData>";
//..
// Note that our "scratch-data" contains a soap envelope with an empty soap
// body, and a few response elements with the 'xmlns' attribute defined.  Since
// we do not know which response element to use yet, we will walk through our
// data in 'Listing 1' and pick an appropriate element from the scratch data.
//
// For efficiency reasons, we will allocate this scratch data only once, into a
// 'bcema_Blob', and reuse the blob buffers for each request.  Our 'convert'
// function will take this global scratch data as a third argument:
//..
    int convert(bcema_Blob            *soapEnvelopeData,
                const bcema_Blob&      portResponseData,
                const baexml_Element&  globalScratchData)
        // Convert from the specified 'portResponseData' to the specified
        // 'soapEnvelopeData', using the specified 'globalScratchData'.  Return
        // 0 on success, and a non-negative value otherwise.
    {
        enum { SUCCESS = 0, FAILURE = -1 };
//..
// In this example, 'portResponseData' will contain the data from 'Listing 1'
// and 'soapEnvelopeData' will be filled with the data from 'Listing 2'.  The
// 'globalScratchData' is assumed to be an 'baexml_Element' object that has
// been loaded with the contents of 'Listing 3'.  The first thing we will do
// is make a local 'scratchData' so that we can manipulate it locally.  Note
// that this will only do a shallow copy of the scratch data blob, so it
// should be efficient:
//..
        baexml_Element scratchData = globalScratchData;
//..
// Next, we create an 'baexml_ElementRef' to refer to the 'soap:Envelope'
// element in the scratch data.  For convenience, we will also create a
// reference to the 'soap:Body' element:
//..
        baexml_ElementRef soapEnvelope = scratchData[0]; // TBD: lookup by name
        baexml_ElementRef soapBody     = soapEnvelope[0];
//..
// Now, we will construct an 'baexml_Element' object, loading the data from
// 'portResponseData'.  Note that we are only interested in 3 levels of the
// tree (to get to the 'MyResponseElement' element).  Once we get to this
// element, we will loop from 'scratchData[1]' through to
// 'scratchData[scratchData.numSubElements()-1]' to find an appropriate element
// that has the 'xmlns' attribute filled in:
//..
        baexml_Element portResponse;

        if (0 != portResponse.load(portResponseData, 3)) {
            return FAILURE;
        }

        if (1 != portResponse.numSubElements()
         || 1 != portResponse[0].numSubElements()) {
            return FAILURE;
        }

        baexml_ElementRef opName          = portResponse[0];
        baexml_ElementRef responseElement = opName[0];
        baexml_ElementRef responseElementWithNs;

        bsl::string responseElementName;

        responseElement.extractElementName(&responseElementName);

        for (int i = 1; i < scratchData.numSubElements(); ++i) {
            bsl::string testName;
            scratchData[i].extractElementName(&testName);
            if (testName == responseElementName) {
                responseElementWithNs = scratchData[i];
                break;
            }
        }

        if (!responseElementWithNs) {
            return FAILURE;
        }
//..
// Now that we have found the corresponding response element with the 'xmlns'
// attribute, we can start moving data around.  Note that although new sub
// elements are inserted, there is no deep copying of data, so it will still be
// efficient:
//..
        bcema_Blob content;

        responseElement.extractContent(&content);
        responseElementWithNs.setContent(content);
        soapBody.insertSubElement(0, responseElementWithNs);

        soapEnvelope.extractData(soapEnvelopeData);
//..
// At this point, the 'soapEnvelopeData' blob will contain the contents of
// 'Listing 2', so we can return successfully:
//..
        return SUCCESS;
    }
//..

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef baexml_Element Obj;

using namespace bsl;  // automatically added by script


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // DRQS TEST - This case tests the fix for DRQS 16530688
        // --------------------------------------------------------------------

        if (verbose) cout << "\nDRQS TEST"
                          << "\n=========" << endl;

        bcema_PooledBlobBufferFactory factory(30);

        Obj mX;
        bcema_Blob data(&factory);
        makeBlob(&data,
                 "<?xml version='1.0'?>\n"
                 "<tns:suggestion xmlns:tns='urn:test'>\n"
                 "   <tns:text>Run {BDE&lt;GO></tns:text>\n"
                 "</tns:suggestion>\n");

        int result = mX.load(data);
        ASSERT(0 == result);

        if (veryVerbose) P(mX);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUSAGE EXAMPLE"
                          << "\n=============" << endl;

        bcema_PooledBlobBufferFactory factory(30);

        bcema_Blob portResponseData(&factory);
        makeBlob(&portResponseData, portResponseMsg);

        bcema_Blob scratchData(&factory);
        makeBlob(&scratchData, scratchDataText);

        baexml_Element scratchElement;
        scratchElement.load(scratchData);

        bcema_Blob soapEnvelopeData;
        convert(&soapEnvelopeData, portResponseData, scratchElement);

        bsl::string result = blobToString(soapEnvelopeData);
        LOOP2_ASSERT(soapEnvelopeMsg, result, soapEnvelopeMsg == result);

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // COPY OPERATIONS
        // --------------------------------------------------------------------

        if (verbose) cout << "\nCOPY OPERATIONS TEST"
                          << "\n====================" << endl;

        Obj mX; const Obj& X = mX;
        Obj mY(X);
        Obj mZ;
        mZ = X;

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        bcema_PooledBlobBufferFactory factory(30);

        Obj mX;
        bcema_Blob data(&factory);
        makeBlob(&data,
                 "<?xml version='1.0'?>\n"
                 "<Envelope>\n"
                 "   <Header>\n"
                 "       <Field>abc</Field>\n"
                 "       <Value>def</Value>\n"
                 "   </Header>\n"
                 "   <Body>xyz</Body>\n"
                 "   <Complicated>\n"
                 "       <Deep>\n"
                 "           <ThirdLevel>qwerty</ThirdLevel>\n"
                 "           <AnotherThird>\n"
                 "               <Fourth>poiuy</Fourth>\n"
                 "               <AnotherFourth>blah</AnotherFourth>\n"
                 "           </AnotherThird>\n"
                 "       </Deep>\n"
                 "   </Complicated>\n"
                 "</Envelope>\n");

        int result = mX.load(data, 2);
        ASSERT(0 == result);

        int numElem = mX.numSubElements();

        if (veryVerbose) cout << "\nInput"
                              << "\n=====\n"
                              << mX << endl
                              << "\nTesting insertSubElement"
                              << "\n========================" << endl;

        int numElem0 = mX[0].numSubElements();

        mX[0].insertSubElement(1, mX[1]);
        ASSERT(mX[0].numSubElements() == ++numElem0);
        // XXX ASSERT(mX[0][1] == mX[1]);

        if (veryVerbose) P(mX);

        Obj mY = mX[0];
        mX.insertSubElement(2, mY);
        mX.insertSubElement(0, mY);

        ASSERT(mX.numSubElements() == (numElem += 2));
        ASSERT(mY.numSubElements() == numElem0);

        if (veryVerbose) {
            P(mX);
            cout              << "\nTesting removeSubElement"
                              << "\n========================" << endl;
        }

        mX[1].removeSubElement(0);
        ASSERT(mX[1].numSubElements() == numElem0 - 1);

        mX[3].removeSubElement(1);
        mX[3].removeSubElement(1);
        ASSERT(mX[3].numSubElements() == 1);

        {
            bsl::string name3_0;
            mX[3][0].extractElementName(&name3_0);
            ASSERT(name3_0 == "Field");
        }

        if (veryVerbose) {
            P(mX);
            cout              << "\nTesting removeAllSubElements"
                              << "\n============================" << endl;
        }

        mX[3][0].removeAllSubElements();
        ASSERT(mX[3][0].numSubElements() == 0);

        if (veryVerbose) {
            P(mX);
            cout              << "\nTesting self-insertion"
                              << "\n======================" << endl;
        }

        mX[0].insertSubElement(0, mX[0]);
        mX[0].insertSubElement(2, mX[0]);

        if (veryVerbose) {
            cout << "mX[0] = ";
            printElement(cout, mX[0]);
            cout << endl << endl;
        }

        if (veryVeryVerbose) {
            cout << "====== Detailed =======\n\n";
            printElementDetailed(cout, mX, "mX");
            cout << endl << endl;
        }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "."
                  << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
