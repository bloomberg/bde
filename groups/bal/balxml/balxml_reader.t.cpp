// balxml_reader.t.cpp                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_reader.h>

#include <bslim_testutil.h>

#include <balxml_errorinfo.h>
#include <balxml_namespaceregistry.h>
#include <balxml_prefixstack.h>
#include <balxml_elementattribute.h>

#include <bsl_cstring.h>     // strlen()
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// ----------------------------------------------------------------------------
// FREE OPERATORS
// [ 2] bsl::ostream& operator(bsl::ostream&, balxml::Reader::NodeType);
// ----------------------------------------------------------------------------
// [ 3] USAGE EXMAPLE
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

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace UsageExample {
// We want the example of the protocol usage to come first in the header file,
// preceding the example of the protocol implementation.  But class declaration
// have to precede its usage, so the examples are swapped in the test driver.

// Assume we have a very simple user directory.  The XML below describes a
// typical entry, it has a top level element, with one embedded entry
// describing a user.
//
//  <?xml version='1.0' encoding='UTF-8'?>
//  <directory-entry xmlns:dir='http://bloomberg.com/schemas/directory'>
//      <name>John Smith</name>
//      <phone dir:phonetype='cell'>212-318-2000</phone>
//      <address/>
//  </directory-entry>
//
///Example 2: The protocol implementation
/// - - - - - - - - - - - - - - - - - - -
// We have to implement all pure virtual functions of the 'balxml::Reader'
// protocol, but to make the example easier to read and shorter we will stub
// some methods.  Moreover, we will provide fake implementations of the methods
// used in this example, so our implementation will not handle the given XML
// fragment, but iterate through some supposititious XML structure.
//
// First, let's introduce an array of "helper" structs.  This array will be
// filled in with data capable of describing the information contained in the
// user directory XML above:
//..
    struct TestNode {
        // A struct that contains information capable of describing an XML
        // node.

        // TYPES
        struct Attribute {
            // This struct represents the qualified name and value of an XML
            // attribute.

            const char *d_qname;  // qualified name of the attribute
            const char *d_value;  // value of the attribute
        };

        enum {
            k_NUM_ATTRIBUTES = 5
        };

        // DATA
        balxml::Reader::NodeType  d_type;
            // type of the node

        const char               *d_qname;
            // qualified name of the node

        const char               *d_nodeValue;
            // value of the XML node (if it's null, 'hasValue()' returns
            // 'false')

        int                       d_depthChange;
            // adjustment for the depth level of 'TestReader', valid values are
            // -1, 0 or 1

        bool                      d_isEmpty;
            // flag indicating whether the element is empty

        Attribute d_attributes[k_NUM_ATTRIBUTES];
            // array of attributes
    };

    static const TestNode fakeDocument[] = {
        // 'fakeDocument' is an array of 'TestNode' objects, that will be used
        // by the 'TestReader' to traverse and describe the user directory XML
        // above.

        { balxml::Reader::e_NODE_TYPE_NONE,
          0                , 0                               ,  0,
          false, {}                                                          },

        { balxml::Reader::e_NODE_TYPE_XML_DECLARATION,
          "xml"            , "version='1.0' encoding='UTF-8'", +1,
          false, {}                                                          },

        { balxml::Reader::e_NODE_TYPE_ELEMENT,
          "directory-entry" , 0                              ,  0,
          false, {"xmlns:dir"    , "http://bloomberg.com/schemas/directory"} },

        { balxml::Reader::e_NODE_TYPE_ELEMENT,
          "name"           , 0                               , +1,
          false, {}                                                          },

        { balxml::Reader::e_NODE_TYPE_TEXT,
          0                , "John Smith"                    , +1,
          false, {}                                                          },

        { balxml::Reader::e_NODE_TYPE_END_ELEMENT,
          "name"           , 0                               , -1,
          false, {}                                                          },

        { balxml::Reader::e_NODE_TYPE_ELEMENT,
          "phone"          , 0                               ,  0,
          false, {"dir:phonetype", "cell"}                                   },

        { balxml::Reader::e_NODE_TYPE_TEXT,
          0                , "212-318-2000"                  , +1,
          false, {}                                                          },

        { balxml::Reader::e_NODE_TYPE_END_ELEMENT,
          "phone"          , 0                               , -1,
          false, {}                                                          },

        { balxml::Reader::e_NODE_TYPE_ELEMENT,
          "address"       , 0                                ,  0,
          true,  {}                                                          },

        { balxml::Reader::e_NODE_TYPE_END_ELEMENT,
          "directory-entry", 0                               , -1,
          false, {}                                                          },

        { balxml::Reader::e_NODE_TYPE_NONE,
          0                , 0                               ,  0,
          false, {}                                                          },
    };
//..
// Now, create a class that implements the 'balxml::Reader' interface.  Note
// that documentation for class methods is omitted to reduce the text of the
// usage example.  If necessary, it can be seen in the 'balxml::Reader' class
// declaration.
//..
                                  // ================
                                  // class TestReader
                                  // ================

    class TestReader : public balxml::Reader {
      private:
        // DATA
        balxml::ErrorInfo    d_errorInfo;    // current error information

        balxml::PrefixStack *d_prefixes;     // prefix stack (held, not owned)

        XmlResolverFunctor   d_resolver;     // place holder, not actually used

        bool                 d_isOpen;       // flag indicating whether the
                                             // reader is open

        bsl::string          d_encoding;     // document encoding

        int                  d_nodeDepth;    // level of the current node

        const TestNode      *d_currentNode;  // node being handled (held, not
                                             // owned)

        // PRIVATE CLASS METHODS
        void setEncoding(const char *encoding);
        void adjustPrefixStack();

      public:
        // CREATORS
        TestReader();
        virtual ~TestReader();

        // MANIPULATORS
        virtual void setResolver(XmlResolverFunctor resolver);

        virtual void setPrefixStack(balxml::PrefixStack *prefixes);

        virtual int open(const char *filename, const char *encoding = 0);
        virtual int open(const char *buffer,
                         size_t      size,
                         const char *url = 0,
                         const char *encoding = 0);
        virtual int open(bsl::streambuf *stream,
                         const char     *url = 0,
                         const char     *encoding = 0);

        virtual void close();

        virtual int advanceToNextNode();

        virtual int lookupAttribute(balxml::ElementAttribute *attribute,
                                    int                       index) const;
        virtual int lookupAttribute(balxml::ElementAttribute *attribute,
                                    const char               *qname) const;
        virtual int lookupAttribute(
                                 balxml::ElementAttribute *attribute,
                                 const char               *localName,
                                 const char               *namespaceUri) const;
        virtual int lookupAttribute(
                                  balxml::ElementAttribute *attribute,
                                  const char               *localName,
                                  int                       namespaceId) const;

        virtual void setOptions(unsigned int flags);

        // ACCESSORS
        virtual const char *documentEncoding() const;
        virtual XmlResolverFunctor resolver() const;
        virtual bool isOpen() const;
        virtual const balxml::ErrorInfo& errorInfo() const;
        virtual int getLineNumber() const;
        virtual int getColumnNumber() const;
        virtual balxml::PrefixStack *prefixStack() const;
        virtual NodeType nodeType() const;
        virtual const char *nodeName() const;
        virtual const char *nodeLocalName() const;
        virtual const char *nodePrefix() const;
        virtual int nodeNamespaceId() const;
        virtual const char *nodeNamespaceUri() const;
        virtual const char *nodeBaseUri() const;
        virtual bool nodeHasValue() const;
        virtual const char *nodeValue() const;
        virtual int nodeDepth() const;
        virtual int numAttributes() const;
        virtual bool isEmptyElement() const;
        virtual unsigned int options() const;
    };

                                  // ----------------
                                  // class TestReader
                                  // ----------------

    // PRIVATE CLASS METHODS
    inline
    void TestReader::setEncoding(const char *encoding)
    {
        d_encoding =
                   (0 == encoding || '\0' == encoding[0]) ? "UTF-8" : encoding;
    }

    inline
    void TestReader::adjustPrefixStack()
    {
        // Each time this object reads a 'e_NODE_TYPE_ELEMENT' node, it must
        // push a namespace prefix onto the prefix stack to handle in-scope
        // namespace calculations that happen inside XML documents where inner
        // namespaces can override outer ones.

        if (balxml::Reader::e_NODE_TYPE_ELEMENT == d_currentNode->d_type) {
            for (int ii = 0; ii < TestNode::k_NUM_ATTRIBUTES; ++ii) {
                const char *prefix = d_currentNode->d_attributes[ii].d_qname;

                if (!prefix || bsl::strncmp("xmlns", prefix, 5)) {
                    continue;
                }

                if (':' == prefix[5]) {
                    d_prefixes->pushPrefix(
                        prefix + 6, d_currentNode->d_attributes[ii].d_value);
                }
                else {
                    // default namespace
                    d_prefixes->pushPrefix(
                        "", d_currentNode->d_attributes[ii].d_value);
                }
            }
        }
        else if (balxml::Reader::e_NODE_TYPE_NONE == d_currentNode->d_type) {
            d_prefixes->reset();
        }
    }

    // PUBLIC CREATORS
    TestReader::TestReader()
    : d_errorInfo()
    , d_prefixes(0)
    , d_resolver()
    , d_isOpen(false)
    , d_encoding()
    , d_nodeDepth(0)
    , d_currentNode(0)
    {
    }

    TestReader::~TestReader()
    {
    }

    // MANIPULATORS
    void TestReader::setResolver(XmlResolverFunctor resolver)
    {
        d_resolver = resolver;
    }

    void TestReader::setPrefixStack(balxml::PrefixStack *prefixes)
    {
        ASSERT(!d_isOpen);

        d_prefixes = prefixes;
    }

    int TestReader::open(const char * /* filename */,
                         const char * /* encoding */)
    {
        return -1;  // STUB
    }

    int TestReader::open(const char * /* buffer */,
                         size_t       /* size */,
                         const char * /* url */,
                         const char *encoding)
    {
        if (d_isOpen) {
            return false;                                             // RETURN
        }
        d_isOpen    = true;
        d_nodeDepth = 0;
//..
// Note that we do not use the supplied buffer, but direct the internal
// iterator to the fake structure:
//..
        d_currentNode = fakeDocument;

        setEncoding(encoding);
        return 0;
    }

    int TestReader::open(bsl::streambuf * /* stream */,
                         const char     * /* url */,
                         const char     * /* encoding */)
    {
        return -1;  // STUB
    }

    void TestReader::close()
    {
        if (d_prefixes) {
            d_prefixes->reset();
        }

        d_isOpen = false;
        d_encoding.clear();
        d_nodeDepth   = 0;
        d_currentNode = 0;
    }

    int TestReader::advanceToNextNode()
    {
        if (!d_currentNode) {
            return -1;                                                // RETURN
        }

        const TestNode *nextNode = d_currentNode + 1;

        if (balxml::Reader::e_NODE_TYPE_NONE == nextNode->d_type) {
            // The document ends when the type of the next node is
            // 'e_NODE_TYPE_NONE'.
            d_prefixes->reset();
            return 1;                                                 // RETURN
        }

        d_currentNode = nextNode;

        if (d_prefixes && 1 == d_nodeDepth) {
            // A 'TestReader' only recognizes namespace URIs that have the
            // prefix "xmlns:" on the top-level element. A 'TestReader' adds
            // such URIs to its prefix stack. It treats namespace URI
            // declarations on any other elements like normal attributes, and
            // resets its prefix stack once the top level element closes.
            adjustPrefixStack();
        }

        d_nodeDepth += d_currentNode->d_depthChange;

        return 0;
    }

    int TestReader::lookupAttribute(balxml::ElementAttribute *attribute,
                                    int                       index) const
    {
        if (!d_currentNode ||
            index < 0 ||
            index >= TestNode::k_NUM_ATTRIBUTES) {
            return 1;                                                 // RETURN
        }

        const char *qname = d_currentNode->d_attributes[index].d_qname;
        if ('\0' == qname[0]) {
            return 1;                                                 // RETURN
        }

        attribute->reset(
            d_prefixes, qname, d_currentNode->d_attributes[index].d_value);
        return 0;
    }

    int TestReader::lookupAttribute(
                                  balxml::ElementAttribute * /* attribute */,
                                  const char               * /* qname */) const
    {
        return -1;  // STUB
    }

    int TestReader::lookupAttribute(
                           balxml::ElementAttribute * /* attribute */,
                           const char               * /* localName */,
                           const char               * /* namespaceUri */) const
    {
        return -1;  // STUB
    }

    int TestReader::lookupAttribute(
                            balxml::ElementAttribute * /* attribute */,
                            const char               * /* localName */,
                            int                        /* namespaceId */) const
    {
        return -1;  // STUB
    }

    void TestReader::setOptions(unsigned int /* flags */)
    {
        return;  // STUB
    }

    // ACCESSORS
    const char *TestReader::documentEncoding() const
    {
        return d_encoding.c_str();
    }

    TestReader::XmlResolverFunctor TestReader::resolver() const
    {
        return d_resolver;
    }

    bool TestReader::isOpen() const
    {
        return d_isOpen;
    }

    const balxml::ErrorInfo& TestReader::errorInfo() const
    {
        return d_errorInfo;
    }

    int TestReader::getLineNumber() const
    {
        return 0;  // STUB
    }

    int TestReader::getColumnNumber() const
    {
        return 0;  // STUB
    }

    balxml::PrefixStack *TestReader::prefixStack() const
    {
        return d_prefixes;
    }

    TestReader::NodeType TestReader::nodeType() const
    {
        if (!d_currentNode || !d_isOpen) {
            return e_NODE_TYPE_NONE;                                  // RETURN
        }

        return d_currentNode->d_type;
    }

    const char *TestReader::nodeName() const
    {
        if (!d_currentNode || !d_isOpen) {
            return 0;                                                 // RETURN
        }

        return d_currentNode->d_qname;
    }

    const char *TestReader::nodeLocalName() const
    {
        if (!d_currentNode || !d_isOpen) {
            return 0;                                                 // RETURN
        }

        // This simple 'TestReader' does not understand XML that contains
        // qualified node names. This means the local name of a node is always
        // equal to its qualified name, so this function simply returns
        // 'd_qname'.
        return d_currentNode->d_qname;
    }

    const char *TestReader::nodePrefix() const
    {
        return "";  // STUB
    }

    int TestReader::nodeNamespaceId() const
    {
        return -1;  // STUB
    }

    const char *TestReader::nodeNamespaceUri() const
    {
        return "";  // STUB
    }

    const char *TestReader::nodeBaseUri() const
    {
        return "";  // STUB
    }

    bool TestReader::nodeHasValue() const
    {
        if (!d_currentNode || !d_isOpen) {
            return false;                                             // RETURN
        }

        if (0 == d_currentNode->d_nodeValue) {
            return false;                                             // RETURN
        }

        return ('\0' != d_currentNode->d_nodeValue[0]);
    }

    const char *TestReader::nodeValue() const
    {
        if (!d_currentNode || !d_isOpen) {
            return 0;                                                 // RETURN
        }

        return d_currentNode->d_nodeValue;
    }

    int TestReader::nodeDepth() const
    {
        return d_nodeDepth;
    }

    int TestReader::numAttributes() const
    {
        for (int index = 0; index < TestNode::k_NUM_ATTRIBUTES; ++index) {
            if (0 == d_currentNode->d_attributes[index].d_qname) {
                return index;                                         // RETURN
            }
        }

        return TestNode::k_NUM_ATTRIBUTES;
    }

    bool TestReader::isEmptyElement() const
    {
        return d_currentNode->d_isEmpty;
    }

    unsigned int TestReader::options() const
    {
        return 0;
    }
//..
// Finally, our implementation of 'balxml::Reader' is complete. We may use this
// implementation as the 'TestReader' in the first example.

int usageExample()
{
///Example 1: The protocol usage
///- - - - - - - - - - - - - - -
// The following string describes xml for a very simple user directory.
// The top level element contains one xml namespace attribute, with one
// embedded entry describing a user.
//..
    const char TEST_XML_STRING[] =
       "<?xml version='1.0' encoding='UTF-8'?>\n"
       "<directory-entry xmlns:dir='http://bloomberg.com/schemas/directory'>\n"
       "    <name>John Smith</name>\n"
       "    <phone dir:phonetype='cell'>212-318-2000</phone>\n"
       "    <address/>\n"
       "</directory-entry>\n";
//..
// Suppose we need to extract the name of the user and his cellphone number
// from this entry.
// In order to read the XML, we first need to construct a
// 'balxml::NamespaceRegistry' object, a 'balxml::PrefixStack' object, and a
// 'TestReader' object, where 'TestReader' is an implementation of
// 'balxml::Reader'.
//..
    balxml::NamespaceRegistry namespaces;
    balxml::PrefixStack       prefixStack(&namespaces);
    TestReader                testReader;
    balxml::Reader&           reader = testReader;
//..
// The reader uses a 'balxml::PrefixStack' to manage namespace prefixes.
// Installing a stack for an open reader leads to undefined behavior.  So, we
// want to ensure that our reader is not open before installation.
//..
    ASSERT(false == reader.isOpen());

    reader.setPrefixStack(&prefixStack);

    ASSERT(&prefixStack == reader.prefixStack());
//..
// Next, we call the 'open' method to setup the reader for parsing using the
// data contained in the XML string.
//..
    reader.open(TEST_XML_STRING, sizeof(TEST_XML_STRING) -1, 0, "UTF-8");
//..
// Confirm that the 'bdem::Reader' has opened properly.
//..
    ASSERT(true == reader.isOpen());
//..
// Then, iterate through the nodes to find the elements that are interesting to
// us. First, we'll find the user's name:
//..
    int         rc = 0;
    bsl::string name;
    bsl::string number;

    do {
        rc = reader.advanceToNextNode();
        ASSERT(0 == rc);
    } while (bsl::strcmp(reader.nodeName(), "name"));

    rc = reader.advanceToNextNode();

    ASSERT(0                                == rc);
    ASSERT(3                                == reader.nodeDepth());
    ASSERT(balxml::Reader::e_NODE_TYPE_TEXT == reader.nodeType());
    ASSERT(true                             == reader.nodeHasValue());

    name.assign(reader.nodeValue());
//..
// Next, advance to the user's phone number:
//..
    do {
        rc = reader.advanceToNextNode();
        ASSERT(0 == rc);
    } while (bsl::strcmp(reader.nodeName(), "phone"));

    ASSERT(false == reader.isEmptyElement());
    ASSERT(1     == reader.numAttributes());

    balxml::ElementAttribute elemAttr;

    rc = reader.lookupAttribute(&elemAttr, 0);
    ASSERT(0     == rc);
    ASSERT(false == elemAttr.isNull());

    if (!bsl::strcmp(elemAttr.value(), "cell")) {
        rc = reader.advanceToNextNode();

        ASSERT(0                                == rc);
        ASSERT(balxml::Reader::e_NODE_TYPE_TEXT == reader.nodeType());
        ASSERT(true                             == reader.nodeHasValue());

        number.assign(reader.nodeValue());
    }
//..
// Now, verify the extracted data:
//..
    ASSERT("John Smith"   == name);
    ASSERT("212-318-2000" == number);
//..
// Finally, close the reader:
//..
    reader.close();
    ASSERT(false == reader.isOpen());
//..
// And exit from the function:
//..
    return 0;
}
//..

}  // close namespace UsageExample


// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int        test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int     verbose = argc > 2;
    int veryVerbose = argc > 3;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // This is a simple implementation of the 'balxml::Reader' interface.
        //
        // Concerns:
        //   That the usage example compiles and runs correctly.
        //   That a class can be derived from 'balxml::Reader' and that it can
        //       override all the methods.
        //
        // Plan:
        //   Copy the usage example from the component-level documentation.
        //   Replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   balxml::Reader();  // Constructor
        //   ~balxml::Reader(); // Destructor
        //   setPrefixStack(balxml::PrefixStack *prefixes);
        //   prefixStack();
        //   isOpen();
        //   documentEncoding();
        //   nodeType();
        //   nodeName();
        //   nodeHasValue());
        //   nodeValue());
        //   nodeDepth());
        //   numAttributes());
        //   isEmptyElement());
        //   advanceToNextNode();
        //   lookupAttribute(ElemAtt a, int index);
        //   lookupAttribute(ElemAtt a, char *qname);
        //   lookupAttribute(ElemAtt a, char *localname, char *nsUri);
        //   lookupAttribute(ElemAtt a, char *localname, int nsId);
        //
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nUSAGE EXAMPLE"
                               << "\n=============" << bsl::endl;

        UsageExample::usageExample();

      } break;
      case 2: {
        // ------------------------------------------------------------------
        // NODETYPE STREAMING
        //
        // Concerns:
        //: 1 Streaming a NodeType enumeration value results in the same
        //:   string as calling 'nodeTypeAsString()'.
        //
        // Plan:
        //: 1 Stream various NodeType values to a string stream and compare
        //:   the results to 'nodeTypeAsString()'. (C-1)
        //
        // Testing:
        //   bsl::ostream& operator(bsl::ostream&, balxml::Reader::NodeType);
        // ------------------------------------------------------------------

        if (verbose) bsl::cout      << bsl::endl
            << "NODETYPE STREAMING" << bsl::endl
            << "==================" << bsl::endl;

        static const balxml::Reader::NodeType DATA[] = {
            balxml::Reader::e_NODE_TYPE_NONE,
            balxml::Reader::e_NODE_TYPE_ELEMENT,
            balxml::Reader::e_NODE_TYPE_TEXT,
            balxml::Reader::e_NODE_TYPE_CDATA,
            balxml::Reader::e_NODE_TYPE_ENTITY_REFERENCE,
            balxml::Reader::e_NODE_TYPE_ENTITY,
            balxml::Reader::e_NODE_TYPE_PROCESSING_INSTRUCTION,
            balxml::Reader::e_NODE_TYPE_COMMENT,
            balxml::Reader::e_NODE_TYPE_DOCUMENT,
            balxml::Reader::e_NODE_TYPE_DOCUMENT_TYPE,
            balxml::Reader::e_NODE_TYPE_DOCUMENT_FRAGMENT,
            balxml::Reader::e_NODE_TYPE_NOTATION,
            balxml::Reader::e_NODE_TYPE_WHITESPACE,
            balxml::Reader::e_NODE_TYPE_SIGNIFICANT_WHITESPACE,
            balxml::Reader::e_NODE_TYPE_END_ELEMENT,
            balxml::Reader::e_NODE_TYPE_END_ENTITY,
            balxml::Reader::e_NODE_TYPE_XML_DECLARATION,
            balxml::Reader::NodeType(31)   // out of range value
        };
        static const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            balxml::Reader::NodeType NT = DATA[i];
            if (veryVerbose) {
                T_ bsl::cout << "Streaming " << NT << bsl::endl;
            }
            bsl::ostringstream os;
            os << NT;
            LOOP_ASSERT(NT, balxml::Reader::nodeTypeAsString(NT) == os.str());
        }
      } break;
      case 1: {
            // ------------------------------------------------------------
            // Basic Test: Derived class can be instantiated
            // -------------------------------------------------------------

            if (verbose) bsl::cout << bsl::endl
                << "Basic Test" << bsl::endl
                << "=====================" << bsl::endl;

            UsageExample::TestReader reader;
            reader.open ("somefilename", "UTF-8");
            reader.close();
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
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
