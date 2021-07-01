// balxml_utf8readerwrapper.cpp                                       -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//                            IMPLEMENTATION NOTES
//
//                              // Error State
//
// Error state exists in 3 objects with a 'Utf8ReaderWrapper':
//: 1 'int d_utf8StreamBuf.errorStatus();' this is the error state of UTF-8
//:   stream buf.  It will be 0 as long as input is being read successfully,
//:   then will become positive on an honest EOF, or negative if we've reached
//:   a UTF-8 error.
//:   o Note that though 'd_utf8StreamBuf' contains a buffer and may have
//:     discovered a UTF-8 error kilobytes ahead of where the reader is reading
//:     from it, 'd_utf8StreamBuf.errorStatus()' will be 0 until the reader
//:     actually attempts to read the bad UTF-8 of hits EOF.
//:
//: 2 'const balxml::ErrorInfo& heldReader()->errorInfo();' this reports the
//:   error as perceived by the held reader.  Normally XML syntax errors will
//:   be reported here.
//:
//: 3 'balxml::ErrorInfo this->d_errorInfo': if we encounter an error and
//:   'd_utf8StreamBuf' is reporting a UTF-8 error, we create an error report
//:   here that reports both the syntax error and the position as perceived by
//:   the held reader, but also describes the nature of the UTF-8 error as
//:   reported by '1' above.  Note that this variable is used for no other
//:   purpose.
//
// There is also a boolean 'd_useHeldErrorInfo' that indicates whether
// 'this->errorInfo()' is to return '2' or '3' above.
// ----------------------------------------------------------------------------

#include <balxml_utf8readerwrapper.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balxml_reader_cpp,"$Id$ $CSID$")

#include <balxml_elementattribute.h>

#include <balxml_elementattribute.h>
#include <balxml_namespaceregistry.h>
#include <balxml_prefixstack.h>

#include <bslma_default.h>

#include <bsl_string.h>

namespace {
namespace u {

inline
const char *nonNullStr(const char *str)
    // Return the specified 'str' is 'str != 0', and "" otherwise.
{
    return str ? str : "";
}

}  // close namespace u
}  // close unnamed namespace

namespace BloombergLP {
namespace balxml {

                                // ------------
                                // class Reader
                                // ------------

// PRIVATE MANIPULATORS
inline
int Utf8ReaderWrapper::doOpen(const char *url, const char *encoding)
{
    // The 'open' functions are documented as returning 0 on success and
    // non-zero on errors.  'heldReader()->open' may begin reading the opened
    // file, and encounter a UTF-8 error, in which case we return a
    // 'bdlde::Utf8Util::ErrorStatus' value, but we may propagate negative
    // statuses from the held reader open for other reasons, so we don't
    // document the UTF-8 behavior to avoid the client getting confused and
    // thinking that ALL negative statuses mean UTF-8 errors.

    // Returning the 'bdlde::Utf8Util::ErrorStatus' values is useful in
    // testing.

    url      = u::nonNullStr(url);
    encoding = u::nonNullStr(encoding);

    int rc = heldReader()->open(&d_utf8StreamBuf, url, encoding);
    if (0 == rc) {
        return 0;                                                     // RETURN
    }

    const int utf8Rc = d_utf8StreamBuf.errorStatus();

    // 'utf8Rc' will be 0 until the reader either attempts to read some invalid
    // UTF-8 or reaches EOF.
    //
    //: o (0 == utf8Rc): no error
    //: o (0 < utf8Rc): non-UTF-8 error, such as an honest EOF
    //: o (utf8Rc < 0): UTF-8 error, an enum of type
    //:   'bdlde::Utf8Util::ErrorStatus'

    if (0 <= utf8Rc) {
        return rc;                                                    // RETURN
    }

    this->reportUtf8Error(utf8Rc);

    return utf8Rc;
}

void Utf8ReaderWrapper::reportUtf8Error(int utf8Rc)
{
    BSLS_ASSERT(utf8Rc < 0);

    // So probably the held reader thinks it hit EOF somewhere it wasn't
    // expecting, resulting in an XML syntax error.  So we want to concatenate
    // our UTF-8 error message to the held reader's complaint.

    const bsl::string& heldMsg = heldReader()->errorInfo().message();
    bsl::string msg;
    msg.reserve(heldMsg.length() + 256);

    msg = heldMsg;
    msg += " -- ";
    msg += bdlde::Utf8CheckingInStreamBufWrapper::toAscii(utf8Rc);

    d_errorInfo.setError(ErrorInfo::e_FATAL_ERROR,
                         heldReader()->getLineNumber(),
                         heldReader()->getColumnNumber(),
                         heldReader()->nodeBaseUri(),
                         msg);
    d_useHeldErrorInfo = false;
}

// PUBLIC CREATORS
Utf8ReaderWrapper::Utf8ReaderWrapper(Reader           *reader,
                                     bslma::Allocator *basicAllocator)
: d_utf8StreamBuf(basicAllocator)
, d_fixedStreamBuf(0, 0)
, d_stream()
, d_reader_p(reader)
, d_errorInfo(basicAllocator)
, d_useHeldErrorInfo(true)
{}

Utf8ReaderWrapper::~Utf8ReaderWrapper()
{
    this->close();
}

// MANIPULATORS

                              // ** setup methods **

void Utf8ReaderWrapper::setOptions(unsigned int flags)
{
    heldReader()->setOptions(flags);
}

void Utf8ReaderWrapper::setPrefixStack(PrefixStack *prefixes)
{
    heldReader()->setPrefixStack(prefixes);
}

void Utf8ReaderWrapper::setResolver(XmlResolverFunctor resolver)
{
    heldReader()->setResolver(resolver);
}

                            // ** open/close methods **

void Utf8ReaderWrapper::close()
{
    d_useHeldErrorInfo = true;

    if (this->isOpen()) {
        heldReader()->close();
        d_utf8StreamBuf.reset(0);
        d_fixedStreamBuf.pubsetbuf("", 0);
        d_stream.close();
        d_errorInfo.reset();
    }
}

int Utf8ReaderWrapper::open(const char *filename, const char *encoding)
{
    this->close();

    filename = u::nonNullStr(filename);
    d_stream.open(filename, bsl::ios_base::in | bsl::ios_base::binary);
    if (!d_stream.is_open()) {
        return -1;                                                    // RETURN
    }

    d_utf8StreamBuf.reset(d_stream.rdbuf());

    return this->doOpen(filename, encoding);
}

int Utf8ReaderWrapper::open(const char  *buffer,
                            bsl::size_t  size,
                            const char  *url,
                            const char  *encoding)
{
    this->close();

    if (buffer == 0 || size == 0) {
        return -1;                                                    // RETURN
    }

    d_fixedStreamBuf.pubsetbuf(buffer, size);
    d_utf8StreamBuf.reset(&d_fixedStreamBuf);

    return this->doOpen(url, encoding);
}

int Utf8ReaderWrapper::open(bsl::streambuf *stream,
                            const char     *url,
                            const char     *encoding)
{
    this->close();

    d_utf8StreamBuf.reset(stream);

    return this->doOpen(url, encoding);
}

                                // ** navigation **

int Utf8ReaderWrapper::advanceToNextNode()
{
    d_useHeldErrorInfo = true;

    // Note that this function is documented as returning 0 on success, 1 on
    // EOF, and a negative value on errors.  If invalid UTF-8 is encounted, we
    // return a status from 'bdlde::Utf8Util::ErrorStatus', but we do not
    // document this because we return negative statuses for other reasons too,
    // and we don't want the caller getting confused and assuming that all
    // negative statuses returned mean invalid UTF-8.

    // The fact that we return 'bdlde::Utf8Util::ErrorStatus' statuses is
    // useful for testing.

    const int rc = heldReader()->advanceToNextNode();
    if (0 == rc) {
        return 0;                                                     // RETURN
    }

    const int utf8Rc = d_utf8StreamBuf.errorStatus();

    // 'utf8Rc' will be 0 until the reader either attempts to read some invalid
    // UTF-8 or reaches EOF.
    //
    //: o (0 == utf8Rc): no error
    //: o (0 < utf8Rc): non-UTF-8 error, such as an honest EOF
    //: o (utf8Rc < 0): UTF-8 error, an enum of type
    //:   'bdlde::Utf8Util::ErrorStatus'

    if (0 <= utf8Rc) {
        return rc;                                                    // RETURN
    }

    this->reportUtf8Error(utf8Rc);

    return utf8Rc;
}

// ACCESSORS
bslma::Allocator *Utf8ReaderWrapper::allocator() const
{
    return d_errorInfo.source().get_allocator().mechanism();
}

const char *Utf8ReaderWrapper::documentEncoding() const
{
    return heldReader()->documentEncoding();
}

const ErrorInfo& Utf8ReaderWrapper::errorInfo() const
{
    return d_useHeldErrorInfo ? heldReader()->errorInfo()
                              : d_errorInfo;
}

int Utf8ReaderWrapper::getColumnNumber() const
{
    return heldReader()->getColumnNumber();
}

int Utf8ReaderWrapper::getLineNumber() const
{
    return heldReader()->getLineNumber();
}

bool Utf8ReaderWrapper::isEmptyElement() const
{
    return heldReader()->isEmptyElement();
}

bool Utf8ReaderWrapper::isOpen() const
{
    return heldReader()->isOpen();
}

int Utf8ReaderWrapper::lookupAttribute(ElementAttribute *attribute,
                                       int               index) const
{
    return heldReader()->lookupAttribute(attribute, index);
}

int Utf8ReaderWrapper::lookupAttribute(ElementAttribute *attribute,
                                       const char       *qname) const
{
    return heldReader()->lookupAttribute(attribute, qname);
}

int Utf8ReaderWrapper::lookupAttribute(ElementAttribute *attribute,
                                       const char       *localName,
                                       const char       *namespaceUri) const
{
    return heldReader()->lookupAttribute(attribute, localName, namespaceUri);
}

int Utf8ReaderWrapper::lookupAttribute(ElementAttribute *attribute,
                                       const char       *localName,
                                       int               namespaceId) const
{
    return heldReader()->lookupAttribute(attribute, localName, namespaceId);
}

const char *Utf8ReaderWrapper::nodeBaseUri() const
{
    return heldReader()->nodeBaseUri();
}

int Utf8ReaderWrapper::nodeDepth() const
{
    return heldReader()->nodeDepth();
}

bool Utf8ReaderWrapper::nodeHasValue() const
{
    return heldReader()->nodeHasValue();
}

const char *Utf8ReaderWrapper::nodeLocalName() const
{
    return heldReader()->nodeLocalName();
}

const char *Utf8ReaderWrapper::nodeName() const
{
    return heldReader()->nodeName();
}

int Utf8ReaderWrapper::nodeNamespaceId() const
{
    return heldReader()->nodeNamespaceId();
}

const char *Utf8ReaderWrapper::nodeNamespaceUri() const
{
    return heldReader()->nodeNamespaceUri();
}

const char *Utf8ReaderWrapper::nodePrefix() const
{
    return heldReader()->nodePrefix();
}

Utf8ReaderWrapper::NodeType Utf8ReaderWrapper::nodeType() const
{
    return heldReader()->nodeType();
}

const char *Utf8ReaderWrapper::nodeValue() const
{
    return heldReader()->nodeValue();
}

int Utf8ReaderWrapper::numAttributes() const
{
    return heldReader()->numAttributes();
}

unsigned int Utf8ReaderWrapper::options() const
{
    return heldReader()->options();
}

PrefixStack *Utf8ReaderWrapper::prefixStack() const
{
    return heldReader()->prefixStack();
}

Utf8ReaderWrapper::XmlResolverFunctor Utf8ReaderWrapper::resolver() const
{
    return heldReader()->resolver();
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
