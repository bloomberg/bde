// baexml_minireader.cpp                  -*-C++-*-
#include <baexml_minireader.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baexml_minireader_cpp,"$Id$ $CSID$")

#include <bdeu_string.h>

#include <bsls_assert.h>

#include <bsl_algorithm.h>  // for swap
#include <bsl_cctype.h>

// IMPLEMENTATION NOTES
// --------------------

// This section will provide a little background on how this component is
// implemented.  This component presents a parser that has the following
// control flow:
//..
//          +------------------------------<----------------------------------+
//          |                                                                 |
//          |                                                                 ^
//          |                                                                 |
//          |                    '?'                                          |
//          |                  +--->- scanProcessingInstruction ------------>-+
//          |                  |      - set type PROCESSING_INSTRUCTION       |
//          |                  |      - set nodeName  (ex xml)                |
//          |                  |      - set nodeValue (ex version='1.0')      ^
//          |                  |      - sets state to ST_TAG_END              |
//          |                  |                                              |
//          |                  |                               '<!--'         |
//          |                  ^                              +--- COMMENT ->-+
//          |                  |                              |  - set type   |
//          |                  |                              ^    COMMENT    |
//          |                  |                              |  - set value  ^
//          |                  |                              |    to comment |
//          |                  |                              |  - set state  |
//          V                  |                              |    ST_TAG_END |
//          |                  |  '!'                         |               |
//          |                  +---->- scanExclaimConstruct --+               |
//          |                  |                              |               |
//          |                  |                              |               |
//          |                  |                              V               |
//          |                  |                              |               |
//          |                  |                              |'<![CDATA['    |
//          |                  |                              +----- CDATA ->-+
//          |                  |                                 - set type   |
//          |                  ^                                   CDATA      |
//          |                  |                                 - set value  |
//          |                  |                                   to escaped ^
//          |                  |                                   data       |
//          |                  |                                 - set state  |
//          |                  |                                   ST_TAG_END |
//          |                  |                                              |
//          |                  | default                                      |
//  INITIAL | scanOpenTag -----+----->----- scanStartElement ------->---------+
//     |    |     |            |            - set type ELEMENT                |
//     |    |     |            |            - set nodeName                    |
//     |    |     |            |              (ex 'Request')                  |
//     |    |     |            |            - scanAttrs                       |
//     |    |     |            |            - check for empty element         |
//     |    |     |            |               - set flags to EMPTY           |
//     |    |     |            v            - set state to ST_TAG_END         |
//     |    |     |            |                                              |
//     |    |     |            |                                              |
//     |    |     | '<'        |  '/'                                         |
//     |    |     |            +------->--- scanEndElement ---------->--------+
//     |    |     |                         - END_ELEMENT
//     |    |     |                         - set state to ST_TAG_END
//     |    |     |                         - decrement num of active nodes
//     |    |     |
//     |    |     ^
//     |-<--+     |
//     |          |
//     |          |---------<-------------------+
//     |          |                             |
//     v          |                             |
//  scanNode -->--+                             |
//     |          |                             |
//     |          |                             | '<'
//     |          v                             |
//     |          | default                     ^
//     |          |                             |
//     |          |                             |
//     |          +---->--- scanText ---->------+
//     |               - WHITESPACE / TEXT      |
//     |               - if TEXT                |
//     |                  - Replace Char        |
//     |                    References          V EOF
//     |                                        |
//     |                                        |
//     |                                        |
//     |-------------------<--------------------+
//     |
//     v
//    END
//..

namespace {

inline
const char* nonNullStr(const char *s)
    // Return 's' if 's' != 0, or "" otherwise.  Never returns a null pointer.
{
    return s ? s : "";
}

int unicodeToUtf8(char *output, unsigned val)
    // Convert the specified unicode 'val' from 32-bit unicode to UTF-8 format
    // and write the characters to the character array at the specified
    // 'output' address.  Return the number of characters output or 0 if 'val'
    // is not in the legal range.
{
    /*
     * Borrowed from LibXml2
     *
     * We are supposed to handle UTF8, check it's valid
     * From rfc2044: encoding of the Unicode values on UTF-8:
     *
     * UCS-4 range (hex.)           UTF-8 octet sequence (binary)
     * 0000 0000-0000 007F   0xxxxxxx
     * 0000 0080-0000 07FF   110xxxxx 10xxxxxx
     * 0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx
     * 0001 0000-0010 FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
     * 0011 0000-FFFF FFFF   not valid
     */

    char *start = output;

    if  (val < 0x80U) {
        *output++ = val;
    } else  if (val < 0x800U) {
        *output++= (val >>  6)           | 0xC0;
        *output++= (val         & 0x3FU) | 0x80;
    }
    else if (val < 0x10000U) {
        *output++= (val  >> 12)          | 0xE0;
        *output++= ((val >>  6) & 0x3FU) | 0x80;
        *output++= (val         & 0x3FU) | 0x80;
    }
    else if (val < 0x110000U) {
        *output++= (val  >> 18)          | 0xF0;
        *output++= ((val >> 12) & 0x3FU) | 0x80;
        *output++= ((val >>  6) & 0x3FU) | 0x80;
        *output++= (val         & 0x3FU) | 0x80;
    }
    else {
        return 0;  // out of Unicode range
    }

    return output - start;
}

// Perform an in-place replacement of XML character references with the
// specified null-terminated 'text' with the corresponding ASCII
// character.  The following pre-defined character entities are
// recognized:
//..
//  Entity  Char    Name
//  ======  ====    ====
//  &lt;    <       Less-than
//  &gt;    >       Greater-than
//  &amp;   &       Ampersand
//  &apos;  '       Apostrophe
//  &quot;  "       Quote
//..
// In addition, this function recognizes numeric character entities of the
// form '&#dd;', where 'dd' is a character value in decimal notation.  For
// example, '&#92;' is a character reference for the backslash character
// (\).  This function only recognizes numeric character entities in the
// range 1-255 ('&#00' is forbidden by both the XML 1.0 and 1.1
// standards).  Unrecognized character entities (whether symbolic or
// numeric) are ignored and remain in the text unchanged.
//
// The transformed text (including the null terminator at the end) is
// written back to the location specified by 'text'.  It will always have
// a length no longer than the original text.
void replaceCharReferences(char *text)
{
    struct Entity {
        char     d_name[6];
        int      d_len;
        unsigned d_ch;
    };

    static const Entity NONE = { "",      0, '&'  };
    static const Entity LT   = { "lt;",   3, '<'  };
    static const Entity GT   = { "gt;",   3, '>'  };
    static const Entity AMP  = { "amp;",  4, '&'  };
    static const Entity APOS = { "apos;", 5, '\'' };
    static const Entity QUOT = { "quot;", 5, '"'  };

    // Skip initial segment up to first ampersand.
    char *output = bsl::strchr(text, '&');
    if (! output) {
        return; // No ampersands
    }

    // Loop through rest of input, looking for ampersands.
    // Loop invariant: *input == '&'
    const char* input = output;
    do {
        Entity Numeric;

        const Entity* entity = &NONE;
        switch (input[1]) {
          case 'l': entity = &LT; break;
          case 'g': entity = &GT; break;
          case 'a': entity = ('m' == input[2] ? &AMP : &APOS); break;
          case 'q': entity = &QUOT; break;
          case '#': {
              // Numeric entity
              char *endptr = 0;
              unsigned c;
              if ('x' == input[2]) {
                  c = bsl::strtoul(input + 3, &endptr, 16);
              }
              else {
                  c = bsl::strtoul(input + 2, &endptr, 10);
              }

              // TBD, if c is 0 or is out of unicode range, how can we report
              // an error?

              if (endptr && ';' == *endptr && 0 < c) {
                  // String has already been matched.  Consume the input
                  // and set the Numeric entity to match an empty string.
                  input = endptr;
                  Numeric.d_len = 0;
                  Numeric.d_ch = c;
                  entity = &Numeric;
              }
          } break;
          default: break;
        }

        if (&Numeric == entity) {

            output += unicodeToUtf8(output, entity->d_ch);
            input += entity->d_len + 1; // Always consumes at least 1 char.
        }
        else if (0 == bsl::memcmp(input + 1, entity->d_name, entity->d_len)) {

            *output++ = entity->d_ch;
            input += entity->d_len + 1; // Always consumes at least 1 char.
        }
        else {
            // Entity not found.  Consume the ampersand unchanged.
            *output++ = *input++;
        }

        // Copy input to output up to (but not including) the next ampersand.
        const char *ampersand = bsl::strchr(input, '&');
        int len = ampersand ? ampersand - input : bsl::strlen(input);
        bsl::memmove(output, input, len);
        output += len;
        input = ampersand;

    } while (input);

    *output = '\0';
}

}  // close unnamed namespace

namespace BloombergLP  {

                  // -----------------------------
                  // class baexml_MiniReader::Node
                  // -----------------------------

baexml_MiniReader::Node::Node(bslma_Allocator *basicAllocator)
: d_type          (BAEXML_NODE_TYPE_NONE)
, d_qualifiedName (0)
, d_prefix        (0)
, d_localName     (0)
, d_value         (0)
, d_namespaceId   (-1)
, d_namespaceUri  (0)
, d_flags         (BAEXML_NODE_NO_FLAGS)
, d_attributes    (basicAllocator)
, d_attrCount     ()
, d_namespaceCount(0)
, d_startPos      (-1)
, d_endPos        (-1)
{
}

baexml_MiniReader::Node::Node(const Node& other,
                              bslma_Allocator *basicAllocator)
: d_type          (other.d_type)
, d_qualifiedName (other.d_qualifiedName)
, d_prefix        (other.d_prefix)
, d_localName     (other.d_localName)
, d_value         (other.d_value)
, d_namespaceId   (other.d_namespaceId)
, d_namespaceUri  (other.d_namespaceUri)
, d_flags         (other.d_flags)
, d_attributes    (other.d_attributes, basicAllocator)
, d_attrCount     (other.d_attrCount)
, d_namespaceCount(other.d_namespaceCount)
, d_startPos      (other.d_startPos)
, d_endPos        (other.d_endPos)
{
}

void
baexml_MiniReader::Node::reset()
{
    d_type           = BAEXML_NODE_TYPE_NONE;
    d_qualifiedName  = 0;
    d_prefix         = 0;
    d_localName      = 0;
    d_value          = 0;
    d_namespaceId    = -1;
    d_namespaceUri   = 0;
    d_flags          = BAEXML_NODE_NO_FLAGS;
    d_attrCount      = 0;
    d_namespaceCount = 0;
    d_startPos       = -1;
    d_endPos         = -1;
}

void
baexml_MiniReader::Node::swap(Node& other)
{
    bsl::swap(d_type, other.d_type);
    bsl::swap(d_qualifiedName, other.d_qualifiedName);
    bsl::swap(d_prefix, other.d_prefix);
    bsl::swap(d_localName, other.d_localName);
    bsl::swap(d_value, other.d_value);
    bsl::swap(d_namespaceId, other.d_namespaceId);
    bsl::swap(d_namespaceUri, other.d_namespaceUri);
    bsl::swap(d_flags, other.d_flags);
    d_attributes.swap(other.d_attributes);
    bsl::swap(d_attrCount, other.d_attrCount);
    bsl::swap(d_namespaceCount, other.d_namespaceCount);
    bsl::swap(d_startPos, other.d_startPos);
    bsl::swap(d_endPos, other.d_endPos);
}

void
baexml_MiniReader::Node::addAttribute(const Attribute& attr)
{
    if (d_attrCount < d_attributes.size()) {
        d_attributes[d_attrCount] = attr;
    } else {
        d_attributes.push_back(attr);
    }

    ++d_attrCount;

    if ((attr.flags() & Attribute::BAEXML_ATTR_IS_NSDECL) != 0) {
        ++d_namespaceCount;
    }
}

                          // -----------------------
                          // class baexml_MiniReader
                          // -----------------------

// CREATORS
baexml_MiniReader::baexml_MiniReader(bslma_Allocator *basicAllocator)
: d_state           (ST_CLOSED)
, d_flags           (0)
, d_readSize        (BAEXML_DEFAULT_BUFSIZE)
, d_parseBuf        (basicAllocator)
, d_streamOffset    (0)
, d_stream          ()
, d_streamBuf       (0)
, d_memStream       (0)
, d_memSize         (0)
, d_startPtr        (0)
, d_endPtr          (0)
, d_scanPtr         (0)
, d_markPtr         (0)
, d_attrNamePtr     (0)
, d_attrValPtr      (0)
, d_lineNum         (0)
, d_linePtr         (0)
, d_errorInfo       (basicAllocator)
, d_resolver        ()
, d_ownNamespaces   (basicAllocator)
, d_ownPrefixes     (&d_ownNamespaces, basicAllocator)
, d_prefixes        (&d_ownPrefixes)
, d_currentNode     (basicAllocator)
, d_activeNodesCount(0)
, d_activeNodes     (basicAllocator)  // always push dummy node
, d_baseURL         (basicAllocator)
, d_encoding        (basicAllocator)
, d_dummyStr        ("", basicAllocator)
, d_options         (0)
{
    d_activeNodes.resize(BAEXML_DEFAULT_DEPTH);
    d_parseBuf.resize(d_readSize);
}

baexml_MiniReader::baexml_MiniReader(int              bufSize,
                                     bslma_Allocator *basicAllocator)
: d_state           (ST_CLOSED)
, d_flags           (0)
, d_readSize        (bufSize)
, d_parseBuf        (basicAllocator)
, d_streamOffset    (0)
, d_stream          ()
, d_streamBuf       (0)
, d_memStream       (0)
, d_memSize         (0)
, d_startPtr        (0)
, d_endPtr          (0)
, d_scanPtr         (0)
, d_markPtr         (0)
, d_attrNamePtr     (0)
, d_attrValPtr      (0)
, d_lineNum         (0)
, d_linePtr         (0)
, d_errorInfo       (basicAllocator)
, d_resolver        ()
, d_ownNamespaces   (basicAllocator)
, d_ownPrefixes     (&d_ownNamespaces, basicAllocator)
, d_prefixes        (&d_ownPrefixes)
, d_currentNode     (basicAllocator)
, d_activeNodesCount(0)
, d_activeNodes     (basicAllocator)  // always push dummy node
, d_baseURL         (basicAllocator)
, d_encoding        (basicAllocator)
, d_dummyStr        ("", basicAllocator)
, d_options         (0)
{
    if (-20000 >= d_readSize && (-20000 - BAEXML_MIN_BUFSIZE) < d_readSize) {
        // TBD: This condition was added as a special case for stress testing
        // pointer rebase, however because readInput resizes d_parseBuf to be
        // BAEXML_MIN_BUFSIZE setting d_readSize to anything less then
        // BAEXML_MIN_BUFSIZE is meaningless.
        d_readSize = -20000 - d_readSize;
    }
    else if (d_readSize < BAEXML_MIN_BUFSIZE) {
        d_readSize = BAEXML_MIN_BUFSIZE;
    } else if (d_readSize > BAEXML_MAX_BUFSIZE ) {
        d_readSize = BAEXML_MAX_BUFSIZE;
    }

    d_activeNodes.resize(BAEXML_DEFAULT_DEPTH);
    d_parseBuf.resize(d_readSize);
}

baexml_MiniReader::~baexml_MiniReader()
{
    close ();
}

// MANIPULATORS
int
baexml_MiniReader::setError(baexml_ErrorInfo::Severity error,
                            const bsl::string&         msg)
{
    Node&  node = currentNode();

    if (node.d_startPos == -1) {
        node.d_startPos = getCurrentPosition();
    }

    if (node.d_endPos == -1) {
        node.d_endPos = getCurrentPosition();
    }

    int rc = 0;
    if ((int)error >= (int)baexml_ErrorInfo::BAEXML_ERROR) {
        d_state = ST_ERROR;
        rc = -1;
    }

    d_errorInfo.setError(error,
                         getLineNumber(),
                         getColumnNumber(),
                         d_baseURL,
                         msg);
    return rc;
}

int
baexml_MiniReader::setParseError(const char *errText,
                                 const char *startFragment,
                                 const char *endFragment)
{
    bsl::string msg(errText);
    if (startFragment != 0) {

        msg.append(": '");
        if (endFragment != 0) {
            msg.append(startFragment, endFragment);
        } else {
            msg.append(startFragment);
        }
        msg.append("'");
    }
    return setError(baexml_ErrorInfo::BAEXML_ERROR, msg);
}

void
baexml_MiniReader::setOptions(unsigned int flags)
{
    d_options = flags;
}

unsigned int
baexml_MiniReader::options() const
{
    return d_options;
}

void
baexml_MiniReader::setResolver(XmlResolverFunctor resolver)
{
    d_resolver = resolver;
}

baexml_MiniReader::XmlResolverFunctor
baexml_MiniReader::resolver() const
{
    return d_resolver;
}

baexml_PrefixStack *
baexml_MiniReader::prefixStack() const
{
    return (d_prefixes == &d_ownPrefixes ? 0 : d_prefixes);
}

void
baexml_MiniReader::setPrefixStack(baexml_PrefixStack *prefixes)
{
    d_prefixes = prefixes;
    if (d_prefixes == 0) {
        d_prefixes = &d_ownPrefixes;
    }
}

void
baexml_MiniReader::close()
{
    d_stream.close();

    d_streamOffset = 0;
    d_streamBuf = 0;
    d_memStream = 0;
    d_memSize   = 0;
    d_flags    |= FLG_READ_EOF;
    d_state     = ST_CLOSED;
}

int
baexml_MiniReader::doOpen(const char *url, const char *encoding)
{
    // reset active nodes stack
    d_activeNodesCount = 0;
    currentNode().reset();

    d_errorInfo.reset();

    d_ownPrefixes.reset();
    d_ownNamespaces.reset();

    // clear input source
    d_parseBuf.resize(d_readSize);
    d_parseBuf[0] = '\0';
    d_streamOffset = 0;
    d_flags       = 0;

    d_startPtr   = &d_parseBuf.front();
    d_endPtr     = d_startPtr;
    d_scanPtr    = d_startPtr;
    d_markPtr    = d_startPtr;

    d_lineNum    = 0;
    d_linePtr    = d_startPtr;

    d_attrNamePtr = 0;
    d_state       = ST_INITIAL;

    d_baseURL  = nonNullStr(url);
    d_encoding = nonNullStr(encoding);

    return (readInput() > 0) ? 0 : -1;
}

int
baexml_MiniReader::open(const char *buffer,
                        size_t      size,
                        const char *url,
                        const char *encoding)
{
    if (d_state != ST_CLOSED) {
        return -1;
    }

    if (buffer == 0 || size == 0) {
        return -1;
    }

    d_memStream = buffer;
    d_memSize = size;

    return doOpen(url, encoding);
}

int
baexml_MiniReader::open(const char *filename,
                        const char *encoding)
{
    if (d_state != ST_CLOSED) {
        return -1;
    }

    d_stream.open(nonNullStr(filename));

    if (!d_stream.is_open()) {
        close();
        return -1;
    }

    return open(d_stream.rdbuf(), filename, encoding);
}

int
baexml_MiniReader::open(bsl::streambuf *streambuf,
                        const char     *url,
                        const char     *encoding)
{
    if (d_state != ST_CLOSED) {
        return -1;
    }

    if  (streambuf == 0) {
        return -1;
    }

    d_streamBuf = streambuf;

    return doOpen(url, encoding);
}

// ACCESSORS
const baexml_ErrorInfo&
baexml_MiniReader::errorInfo () const
{
    return d_errorInfo;
}

bool
baexml_MiniReader::isOpen() const
{
    return (d_state != ST_CLOSED);
}

int
baexml_MiniReader::getLineNumber() const
{
    return d_lineNum+1;
}

int
baexml_MiniReader::getColumnNumber() const
{
    return (d_scanPtr - d_linePtr) + 1;
}

baexml_Reader::NodeType
baexml_MiniReader::nodeType() const
{
    return currentNode().d_type;
}

const char *
baexml_MiniReader::nodeName() const
{
    return currentNode().d_qualifiedName;
}

const char *
baexml_MiniReader::nodeLocalName() const
{
    return currentNode().d_localName;
}

const char *
baexml_MiniReader::nodePrefix() const
{
    return currentNode().d_prefix;
}

const char *
baexml_MiniReader::nodeNamespaceUri() const
{
    return currentNode().d_namespaceUri;
}

int
baexml_MiniReader::nodeNamespaceId() const
{
    return currentNode().d_namespaceId;
}

const char *
baexml_MiniReader::nodeValue() const
{
    return currentNode().d_value;
}

bool
baexml_MiniReader::nodeHasValue() const
{
    switch (nodeType()) {
      case BAEXML_NODE_TYPE_TEXT:
      case BAEXML_NODE_TYPE_CDATA:
      case BAEXML_NODE_TYPE_PROCESSING_INSTRUCTION:
      case BAEXML_NODE_TYPE_COMMENT:
      case BAEXML_NODE_TYPE_DOCUMENT_TYPE:
      case BAEXML_NODE_TYPE_WHITESPACE:
      case BAEXML_NODE_TYPE_SIGNIFICANT_WHITESPACE:
      case BAEXML_NODE_TYPE_XML_DECLARATION: {
          return true;
      }
      default: {
      } break;
    }
    return false;
}

const char *
baexml_MiniReader::nodeBaseUri() const
{
    return d_baseURL.c_str();
}

const char *
baexml_MiniReader::documentEncoding() const
{
    return d_encoding.c_str();
}

int
baexml_MiniReader::nodeDepth() const
{
    int rc = d_activeNodesCount;
    switch (nodeType()) {
      case BAEXML_NODE_TYPE_END_ELEMENT:
      case BAEXML_NODE_TYPE_NONE: {
      } break;
      case BAEXML_NODE_TYPE_ELEMENT:
      default: {
        ++rc;
      } break;
    }
    return rc;
}

int
baexml_MiniReader::numAttributes() const
{
    return currentNode().d_attrCount;
}

bool
baexml_MiniReader::isEmptyElement() const
{
    if (nodeType() != BAEXML_NODE_TYPE_ELEMENT)
    {
        return false;
    }
    return ((currentNode().d_flags & Node::BAEXML_NODE_EMPTY) != 0);
}

int
baexml_MiniReader::lookupAttribute(
                         baexml_ElementAttribute *attribute,
                         int                      index) const
{
    const Node&  node = currentNode();

    if (((size_t) index) <  node.d_attrCount) {
        *attribute = node.d_attributes[index];
        return 0; // success
    }

    return 1; //not found
}

int
baexml_MiniReader::lookupAttribute(
                           baexml_ElementAttribute  *attribute,
                           const char               *qname) const
{
    const Node&  node = currentNode();

    AttributeVector::const_iterator it1 = node.d_attributes.begin();
    AttributeVector::const_iterator it2 = it1 + node.d_attrCount;

    for (; it1 != it2; ++it1) {

        if (0 == bsl::strcmp(qname, (*it1).qualifiedName())) {
            *attribute = *it1;
            return 0; // success;
        }
    }

    return 1; //not found
}

int
baexml_MiniReader::lookupAttribute(
                           baexml_ElementAttribute *attribute,
                           const char              *localName,
                           const char              *namespaceUri) const
{
    const Node&  node = currentNode();

    AttributeVector::const_iterator it1 = node.d_attributes.begin();
    AttributeVector::const_iterator it2 = it1 + node.d_attrCount;

    for (; it1 != it2; ++it1) {

        if (0 == bsl::strcmp(localName, (*it1).localName()) &&
            0 == bsl::strcmp(namespaceUri, (*it1).namespaceUri()))
        {
            *attribute = *it1;
            return 0; // success;
        }
    }

    return 1; //not found
}

int
baexml_MiniReader::lookupAttribute(
                           baexml_ElementAttribute *attribute,
                           const char              *localName,
                           int                      namespaceId) const
{
    const Node&  node = currentNode();

    AttributeVector::const_iterator it1 = node.d_attributes.begin();
    AttributeVector::const_iterator it2 = it1 + node.d_attrCount;

    for (; it1 != it2; ++it1) {

        if (0 == bsl::strcmp(localName, (*it1).localName()) &&
            namespaceId == (*it1).namespaceId())
        {
            *attribute = *it1;
            return 0; // success;
        }
    }

    return 1; //not found
}

int
baexml_MiniReader::advanceToNextNode()
{
    int rc = 0;
    // Return codes:
    // -1 (< 0)  errors, stop parsing
    // 0         node has been found
    // 1         EOF
    // 2         skip node and continue

    d_markPtr = d_scanPtr;

    do {
        switch (d_state) {
          case ST_INITIAL: {
            rc = scanNode();
          } break;
          case ST_TAG_BEGIN: {
            preAdvance();
            rc = scanOpenTag();
          } break;
          case ST_TAG_END: {
            preAdvance();
            rc = scanNode();
          } break;
          case ST_EOF: {
            preAdvance();
            rc = 1;
          } break;
          case ST_ERROR:
          case ST_CLOSED: {
            return -1;
          }
          default: {
            BSLS_ASSERT(0);
            return -1;
          }
        }
    } while (rc == 2);

    if (rc == 1) {  // EOF

        if (d_activeNodesCount > 0) {

            return setParseError("No End Element tags for the Element",
                    d_activeNodes[d_activeNodesCount-1].first.c_str(),
                    0);
        }

        if ((d_flags & FLG_ROOT_CLOSED) == 0) {  // Root element not closed

            return setParseError("Root Element not found",
                    0,
                    0);
        }
    }

    return rc;
}

//----------------------------------------------------------------------
//   PRIVATE methods
//----------------------------------------------------------------------
int
baexml_MiniReader::skipSpaces()
{
    while (1) {

        // skip SPACE, TAB, CR chars
        size_t len = bsl::strspn(d_scanPtr, "\r\t ");
        d_scanPtr +=len;

        if (checkForNewLine()) {
            ++d_scanPtr;          //skip NL
            continue;
        }

        if (d_scanPtr < d_endPtr) {
            break;
        }

        if (readInput() == 0) {
            return 0;
        }
    }
    return *d_scanPtr;
}

int
baexml_MiniReader::scanForSymbol(char symbol)
{
    char strSet [] = { symbol, '\n', '\0' };

    while (1) {
        // find 'symbol' or NL
        size_t len = bsl::strcspn(d_scanPtr, strSet);
        d_scanPtr +=len;

        if (symbol == *d_scanPtr) {
            return symbol;
        }

        if (checkForNewLine()) {
            ++d_scanPtr;        //skip NL
            continue;
        }

        if (d_scanPtr < d_endPtr) {
            break;
        }

        if (readInput() == 0) {
            return 0;
        }
    }

    return *d_scanPtr;
}

int
baexml_MiniReader::scanForSymbolOrSpace(char symbol)
{
    char strSet[] = { symbol, '\n', '\r', '\t' , ' ', '\0' };

    while (1) {
        // find 'symbol' or space
        size_t len = bsl::strcspn(d_scanPtr, strSet);
        d_scanPtr +=len;

        if (d_scanPtr < d_endPtr) {
            break;
        }

        if (readInput() == 0) {
            return 0;
        }
    }
    return *d_scanPtr;
}

int
baexml_MiniReader::scanForSymbolOrSpace(char symbol1, char symbol2)
{
    char strSet [] = {
        symbol1, symbol2,  '\n', '\r', '\t' , ' ', '\0'
    };

    while (1) {
        // find 'symbol1' or 'symbol2' or space
        size_t len = bsl::strcspn(d_scanPtr, strSet);
        d_scanPtr +=len;

        if (d_scanPtr < d_endPtr) {
            break;
        }

        if (readInput() == 0) {
            return 0;
        }
    }
    return *d_scanPtr;
}

int
baexml_MiniReader::scanForString(const char *str)
{
    size_t len = bsl::strlen(str);
    while(1) {
        int ch = scanForSymbol(str[0]);
        if (ch == 0) {
            return ch;
        }

        while ((d_endPtr - d_scanPtr) < (int)len ) {

            if (readInput()== 0) {
                d_scanPtr = d_endPtr;
                return 0;
            }
        }

        if (bsl::memcmp(d_scanPtr, str, len) == 0) {
            break;
        }
        ++d_scanPtr;
    }
    return *d_scanPtr;
}

bool
baexml_MiniReader::skipIfMatch(const char *str)
{
    size_t len = bsl::strlen(str);

    while ((d_endPtr - d_scanPtr) < (int)len ) {
        if (readInput()== 0) {
            return false;
        }
    }

    if (bsl::memcmp(d_scanPtr, str, len) == 0) {
        d_scanPtr += len;
        return true;
    }

    return false;
}

void
baexml_MiniReader::preAdvance()
{
    switch (currentNode().d_type) {
      case BAEXML_NODE_TYPE_NONE: {
      } break;
      case BAEXML_NODE_TYPE_END_ELEMENT: {
        BSLS_ASSERT(d_activeNodesCount > 0);

        d_prefixes->popPrefixes(d_activeNodes[d_activeNodesCount-1].second);
        if (--d_activeNodesCount == 0) {        // pop END_ELEMENT
            d_flags |= FLG_ROOT_CLOSED;
        }
      } break;
      case BAEXML_NODE_TYPE_ELEMENT: {
        if (isEmptyElement()) {
            d_prefixes->popPrefixes(currentNode().d_namespaceCount);
            if (0 == d_activeNodesCount) {
                d_flags |= FLG_ROOT_CLOSED;
            }
        }
        else {
            if (d_activeNodes.size() == d_activeNodesCount) {
                d_activeNodes.resize(d_activeNodesCount+2);
            }
            Element& elem = d_activeNodes[d_activeNodesCount];
            elem.first = currentNode().d_qualifiedName;
            elem.second = currentNode().d_namespaceCount;
            ++d_activeNodesCount;
        }
      } break;
      case BAEXML_NODE_TYPE_CDATA:
      case BAEXML_NODE_TYPE_TEXT:
      default: {
      } break;
    }
    currentNode().reset();
    d_errorInfo.reset();
}

int
baexml_MiniReader::scanNode()
{
    int ch = peekChar();
    switch(ch) {
      case 0: {
        d_state = ST_EOF;
        return 1;
      }
      case '<': {        // open tag
        getChar();
        return scanOpenTag();
      }
      default: {
      } break;
    }
    return scanText();
}

int
baexml_MiniReader::scanText()
{
    // at this moment the current position is set to
    // the character following  '>'
    Node& node = currentNode();

    node.d_startPos = getCurrentPosition();
    node.d_value = d_scanPtr;
    node.d_type = BAEXML_NODE_TYPE_WHITESPACE;

    int ch = skipSpaces();

    if (ch == '<') {

        node.d_endPos = getCurrentPosition();

        // consume separating character with replacing it by zero
        // this will make node value as C-string
        getCharAndSet(0);
        d_state = ST_TAG_BEGIN;
        return 0;
    }

    if (d_state == ST_INITIAL) {
        return setParseError("No root element", 0 , 0);
    }

    if (ch == 0) {

        node.d_endPos = getCurrentPosition();
        d_state = ST_EOF;
        return 0;
    }

    ch = scanForSymbol('<');
    if (ch == '<') {

        node.d_endPos = getCurrentPosition();

        // consume separating character with replacing it by zero
        // this will make node value as C-string
        getCharAndSet(0);
        node.d_type = BAEXML_NODE_TYPE_TEXT;
        d_state = ST_TAG_BEGIN;

        replaceCharReferences(const_cast<char *>(node.d_value));
        return 0;
    }

    BSLS_ASSERT(ch==0);

    if (node.d_value == d_endPtr) {
        node.reset();
        node.d_startPos = getCurrentPosition();
        node.d_endPos = getCurrentPosition();
        d_state = ST_EOF;
        return 1;
    }

    return setParseError("Text out of root element", d_scanPtr, 0);
}

int
baexml_MiniReader::scanOpenTag()
{
    // at this moment the current position is set to
    // the character following  '<'
    Node& node = currentNode();
    node.d_startPos = getCurrentPosition()-1;

    int ch = peekChar();

    switch(ch) {
      case 0:           // End of Data
        setError(baexml_ErrorInfo::BAEXML_ERROR,
                 "Syntax error - Unexpected End of Data");
        return -1;

      case '?':         // Processing Instruction
        getChar();
        return scanProcessingInstruction();

      case '!':         // Comments, CDATA, DTD declarations
        getChar();
        return scanExclaimConstruct();

      case '/':         // End Element
        getChar();
        return scanEndElement();

    default:
        break;
    }

    if (bsl::isspace(static_cast<unsigned char>(ch))) {
        return setParseError("Invalid tag character",
                             d_scanPtr,
                             d_scanPtr+1);
    }

    if ((d_flags & FLG_ROOT_CLOSED) != 0) {
        return setParseError("Only one root element is allowed",
                             d_scanPtr,
                             d_scanPtr+1);
    }

    return scanStartElement();
}

int
baexml_MiniReader::scanExclaimConstruct()
{
    // at this moment the current position is set to
    // the character following "<!"
    Node& node = currentNode();

    if (skipIfMatch("--")) {  // comment

        node.d_type = BAEXML_NODE_TYPE_COMMENT;
        node.d_value = d_scanPtr;

        if (scanForString("-->") == 0) { // not found
            return setParseError("No closing tag for comment",
                                 node.d_value,
                                 d_scanPtr);

        }
        getCharAndSet(0);   // consume '-'
        getChar();          // consume '-'
        getChar();          // consume '>'

        node.d_endPos = getCurrentPosition();
        d_state = ST_TAG_END;
        return 0;
    }

    if (skipIfMatch("[CDATA[")) { // CDATA section

        node.d_type = BAEXML_NODE_TYPE_CDATA;
        node.d_value = d_scanPtr;

        if (scanForString("]]>") == 0) { // not found
            return setParseError("No closing tag for CDATA",
                                 node.d_value,
                                 d_scanPtr);
        }
        getCharAndSet(0);   // consume ']'
        getChar();          // consume ']'
        getChar();          // consume '>'

        // Note that in CDATA section we should not
        // replace entities and char references.

        node.d_endPos = getCurrentPosition();
        d_state = ST_TAG_END;
        return 0;
    }

    // here is the place to add support of
    // <!XXXX > (old DTD-style constructs)

    node.d_value = d_scanPtr;

    if (!skipIfMatch("NOTATION") &&
        !skipIfMatch("ENTITY")   &&
        !skipIfMatch("ATTRLIST") &&
        !skipIfMatch("ELEMENT")  &&
        !skipIfMatch("DOCTYPE")) {

        return setParseError("Unrecognized construst" ,
                             node.d_value,
                             d_scanPtr);

    }

    if (scanForSymbol('>') == 0) { // not found
        return setParseError("No closing tag for " ,
                             node.d_value,
                             d_scanPtr);
    }

    getCharAndSet(0);   // consume '>'
    node.d_endPos = getCurrentPosition();

    d_state = ST_TAG_END;
    return 2;     //
}

int
baexml_MiniReader::scanProcessingInstruction()
{
    // at this moment the current position is set to
    // the character following "<?"
    Node& node = currentNode();

    node.d_type = BAEXML_NODE_TYPE_PROCESSING_INSTRUCTION;
    node.d_qualifiedName = d_scanPtr;

    int ch = scanForSymbolOrSpace('?');

    if (ch == 0) {  // not found
        return setParseError("Invalid PI name",
                             node.d_qualifiedName,
                             0);
    }

    // consume separating character with replacing it by zero
    // this will make PI name as C-string
    getCharAndSet(0);

    if (bsl::isspace(static_cast<unsigned char>(ch))) {
        ch = skipSpaces();
    }

    if (ch != '?') {            // we have PI with value
        node.d_value = d_scanPtr;

        ch = scanForSymbol('?');
        if (ch != '?') {       // not found , must be end
            return setParseError("Invalid PI value",
                                  node.d_value,
                                  0);
        }

        // consume separating character with replacing it by zero
        // this will make PI value as C-string
        getCharAndSet(0);
    }

    ch = getChar();
    if (ch != '>') {            // Not closing tag
        return setParseError("No closing tag for PI",
                              node.d_qualifiedName,
                              0);
    }

    if (bsl::strcmp("xml", node.d_qualifiedName) == 0) {
        node.d_type = BAEXML_NODE_TYPE_XML_DECLARATION;
        if (d_state != ST_INITIAL) {
            return setParseError("The XML declaration is unexpected",
                                 0, 0);
        }
    }
    node.d_endPos = getCurrentPosition();
    d_state = ST_TAG_END;
    return 0;
}

int
baexml_MiniReader::scanEndElement()
{
    // at this moment the current position is set to
    // the character following "</"
    Node& node = currentNode();

    node.d_type = BAEXML_NODE_TYPE_END_ELEMENT;
    node.d_qualifiedName = d_scanPtr;

    int ch = scanForSymbolOrSpace('>');
    if (ch == 0) {
        return setParseError("Unexpected end of document, expected >",
                             node.d_qualifiedName,
                             0);
    }

    // consume separating character with replacing it by zero
    // this will make qualified name as C-string
    getCharAndSet(0);

    if (bsl::isspace(static_cast<unsigned char>(ch))) {
        skipSpaces();
        ch = getChar();
    }

    if (ch != '>') {
        return setParseError("No '>' for Element",
                             node.d_qualifiedName,
                             0);
    }

    node.d_endPos = getCurrentPosition();
    d_state = ST_TAG_END;

    if (d_activeNodesCount == 0) {

        return setParseError("no opening tag for closing tag",
                             node.d_qualifiedName,
                             0);
    }

    Element& elem = d_activeNodes[d_activeNodesCount-1];

    if (elem.first == node.d_qualifiedName) {
        return updateElementInfo();
    }

    return setParseError("Opening and closing tag mismatch'",
                          node.d_qualifiedName,
                         0);
}

int
baexml_MiniReader::scanStartElement()
{
    // at this moment the current position is set to
    // the character following "<"
    Node& node = currentNode();

    node.d_type = BAEXML_NODE_TYPE_ELEMENT;
    node.d_qualifiedName = d_scanPtr;

    int ch = scanForSymbolOrSpace('/', '>');
    if (ch == 0) {
        return setParseError("Unexpected end of document, expected >",
                             node.d_qualifiedName,
                             0);
    }

    // consume separating character with replacing it by zero
    // this will make qualified name as C-string
    getCharAndSet(0);

    // Check for attributes.
    if (bsl::isspace(static_cast<unsigned char>(ch))) {
        int rc = scanAttributes();
        if (rc != 0) {
            return rc;
        }
        ch = getChar();
    }

    if (ch == '/') {
        node.d_flags |= Node::BAEXML_NODE_EMPTY;
        ch = getChar();
    }

    if (ch != '>') {
        return setParseError("No '>' for Element",
                             node.d_qualifiedName,
                             0);
    }

    node.d_endPos = getCurrentPosition();
    d_state = ST_TAG_END;
    return updateElementInfo();
}

int
baexml_MiniReader::updateElementInfo()
{
    Node& node = currentNode();
    char* colon = (char*) bsl::strchr(node.d_qualifiedName, ':');

    if (colon == 0) {
        //only localName
        node.d_localName = node.d_qualifiedName;
        node.d_namespaceId = d_prefixes->lookupNamespaceId("");
    } else {
        //  prefix:localName
        node.d_localName = colon + 1;
        *colon = 0;   // temporary set terminating zero
        node.d_namespaceId = d_prefixes->lookupNamespaceId(
                                          node.d_qualifiedName);

        node.d_prefix = d_prefixes->lookupNamespacePrefix(
                                            node.d_qualifiedName);
        *colon =':';  // restore
        if (node.d_namespaceId == -1) {

            // prepare a message
            bsl::string msg("Undefined namespace for prefix: '");
            msg.append(node.d_qualifiedName);
            msg.append("'");

            //******************************************
            // temporary fix for BAS<->SOAP converters.
            // If there is no user provided PrefixStack,
            // assume we do not process namespaces,
            // ignore undefined namespaces and consider
            // this case as warning rather then error.
            //******************************************
            if (d_prefixes != &d_ownPrefixes) {
                // user provided PrefixStack
                return setError(baexml_ErrorInfo::BAEXML_ERROR, msg);
            }

           // issue warning and continue
           setError(baexml_ErrorInfo::BAEXML_WARNING, msg);
        }
    }
    node.d_namespaceUri =
        d_prefixes->lookupNamespaceUri(node.d_namespaceId);

    return 0;
}

int
baexml_MiniReader::scanAttributes()
{
    int rc = 0;
    int separator = ' ';
    while (rc == 0) {
        int ch = skipSpaces();
        if (ch == 0) {
            return setParseError("Unexpected end of document, expected >",
                                 currentNode().d_qualifiedName,
                                 0);
        }

        // must be '/', '>'  or attribute name
        if (ch == '/' || ch == '>') {
            break;
        }

        // Here we are at the begging attribute name
        d_attrNamePtr = d_scanPtr;
        ch = scanForSymbolOrSpace('=', '>');
        if (ch == 0) {
            return setParseError("Invalid Attribute Name",
                                 d_attrNamePtr,
                                 0);
        }

        if (! bsl::isspace(static_cast<unsigned char>(separator))) {
            return setParseError("No space before attribute ",
                                 d_attrNamePtr, d_scanPtr);
        }

        // consume separating character with replacing it by zero
        // this will make attribute qualified name as C-string
        getCharAndSet(0);

        // Space after attribute name.
        if (bsl::isspace(static_cast<unsigned char>(ch))) {
            ch = skipSpaces();    // skip spaces and
            if ('=' == ch) {
                getChar();        // consume '=' (will check below)
            }
        }

        if (ch != '=') {         // must be '=' after attribute name
            return setParseError("No '=' after Attribute Name",
                                 d_attrNamePtr,
                                 0);
        }

        ch = skipSpaces();      // ch must be (") or (')
        getChar();              // consume (") or (')
        switch(ch) {
          case '"':
          case '\'':
            break;
          default:
            return setParseError(
                "Attribute value must start with ' or \"",
                0, 0);
        }

        d_attrValPtr = d_scanPtr;
        int ch2 = scanForSymbol(ch);
        if (ch2 != ch) {       // not the same delimiter
            return setParseError("Attribute value must end with ' or \"",
                                 0, 0);
        }
        // consume terminating character with replacing it by zero
        // this will make attribute qualified name as C-string
        getCharAndSet(0);

        rc = addAttribute();

        separator = peekChar(); // Get separator between attributes

    } // end while

    if (rc == 0) {
        rc = updateAttributes();
    }
    return rc;
}

int
baexml_MiniReader::updateAttributes()
{
    Node&  node = currentNode();
    AttributeVector::iterator it1 = node.d_attributes.begin();
    AttributeVector::iterator it2 = it1 + node.d_attrCount;

    for (; it1 != it2; ++it1) {

        Attribute& attr =*it1;
        int flags = attr.flags();

        if (0 != (flags & Attribute::BAEXML_ATTR_IS_NSDECL)){
            continue;
        }

        const char *qName        = attr.qualifiedName();
        const char *localName    = attr.localName();
        const char *value        = attr.value();
        const char *prefix       = "";
        const char *namespaceUri = "";
        int         namespaceId  = -1;

        const char *colon = attr.prefix();

        if (*colon == ':')  {                 // prefix:localName
            *const_cast<char *>(colon) = 0;       // replace ':' with 0

            prefix = d_prefixes->lookupNamespacePrefix(qName);

            *const_cast<char *>(colon) = ':';     // restore original ':'

            namespaceId = d_prefixes->lookupNamespaceId(prefix);
            if (namespaceId < 0) {

                bsl::string msg("Prefix is not defined: '");
                msg.append(qName);
                msg.append("'");
                setError(baexml_ErrorInfo::BAEXML_WARNING, msg);

                //******************************************
                // temporary fix for BAS<->SOAP converters.
                // If there is no user provided PrefixStack,
                // assume we do not process namespaces,
                // ignore undefined namespaces and consider
                // this case as warning rather then error.
                //******************************************
                if (d_prefixes != &d_ownPrefixes) {
                    // user provided PrefixStack
                    return setError(baexml_ErrorInfo::BAEXML_ERROR, msg);
                }

                // issue warning and continue
                setError(baexml_ErrorInfo::BAEXML_WARNING, msg);
            }

            namespaceUri = d_prefixes->lookupNamespaceUri(namespaceId);
        }

        attr.reset(d_prefixes,
                   qName,
                   value,
                   prefix,
                   localName,
                   namespaceId,
                   namespaceUri,
                   flags);
    }
    return 0; //OK
}

int
baexml_MiniReader::addAttribute()
{
    int         flags = 0;
    const char *prefix = "";
    const char *localName = "";
    const char *namespaceUri = "";
    int         namespaceId = INT_MIN;

    replaceCharReferences(d_attrValPtr);

    char* colon = bsl::strchr(d_attrNamePtr, ':');

    if (colon == 0) {
        //  only localName
        if (bsl::strcmp(d_attrNamePtr, "xmlns") == 0) {
            localName = "";
            flags |= Attribute::BAEXML_ATTR_IS_NSDECL;
        } else {
            localName = d_attrNamePtr;
        }
    } else {
        //  prefix:localName
        localName = colon + 1;
        *colon = 0;            // temporary set terminating zero
        if (!bsl::strcmp(d_attrNamePtr, "xmlns")) {
            flags |= Attribute::BAEXML_ATTR_IS_NSDECL;
        }
        else if (!bsl::strcmp(d_attrNamePtr, "xsi")) {
            flags |= Attribute::BAEXML_ATTR_IS_XSIDECL;
        }
        else {
            prefix = colon;    // later it will be corrected
        }
        *colon = ':';          // restore
    }

    if (flags & Attribute::BAEXML_ATTR_IS_NSDECL) {

        prefix = "xmlns";
        namespaceUri = "http://www.w3.org/2000/xmlns/";
        namespaceId = d_prefixes->lookupNamespaceId(prefix);

        d_prefixes->pushPrefix(localName, d_attrValPtr);
    }
    else if (flags & Attribute::BAEXML_ATTR_IS_XSIDECL) {

        prefix = "xsi";
        namespaceUri = "http://www.w3.org/2001/XMLSchema-instance";
        namespaceId = d_prefixes->lookupNamespaceId(prefix);
    }

    Attribute attr(d_prefixes,
                   d_attrNamePtr,
                   d_attrValPtr,
                   prefix,
                   localName,
                   namespaceId,
                   namespaceUri,
                   flags);

    currentNode().addAttribute(attr);
    return 0;
}

void
baexml_MiniReader::rebasePointers(const char *newBase, size_t newLength)
{
    // adjust current node
    d_currentNode.d_qualifiedName =
        rebasePointer(d_currentNode.d_qualifiedName, newBase);

    d_currentNode.d_prefix =
        rebasePointer(d_currentNode.d_prefix, newBase);

    d_currentNode.d_localName =
        rebasePointer(d_currentNode.d_localName, newBase);

    d_currentNode.d_value =
        rebasePointer(d_currentNode.d_value, newBase);

    d_currentNode.d_namespaceUri =
        rebasePointer(d_currentNode.d_namespaceUri, newBase);

    // adjust attributes in current node
    AttributeVector::iterator it1 = d_currentNode.d_attributes.begin();
    AttributeVector::iterator it2 = it1 + d_currentNode.d_attrCount;

    for (; it1 != it2; ++it1) {

        Attribute& attr = *it1;

        int flags = attr.flags();
        const char *qName        = attr.qualifiedName();
        const char *localName    = attr.localName();
        const char *value        = attr.value();
        const char *prefix       = attr.prefix();
        const char *namespaceUri = attr.namespaceUri();
        int         namespaceId  = attr.namespaceId();

        qName        = rebasePointer(qName, newBase);
        localName    = rebasePointer(localName, newBase);
        value        = rebasePointer(value, newBase);
        prefix       = rebasePointer(prefix, newBase);
        namespaceUri = rebasePointer(namespaceUri, newBase);

        attr.reset(d_prefixes,
                qName,
                value,
                prefix,
                localName,
                namespaceId,
                namespaceUri,
                flags);
    }

    // adjust members
    d_attrNamePtr = const_cast<char *>(rebasePointer(d_attrNamePtr, newBase));
    d_attrValPtr  = const_cast<char *>(rebasePointer(d_attrValPtr, newBase));

    // adjust scan info ptrs
    d_linePtr = const_cast<char *>(rebasePointer(d_linePtr, newBase));
    d_scanPtr = const_cast<char *>(rebasePointer(d_scanPtr, newBase));
    d_markPtr = const_cast<char *>(rebasePointer(d_markPtr, newBase));

    d_startPtr = const_cast<char *>(newBase);
    d_endPtr   = d_startPtr + newLength;
}

int
baexml_MiniReader::readInput()
{
    if ((d_flags & FLG_READ_EOF) != 0) {
        return 0;
    }

    size_t numConsumed = d_markPtr - d_startPtr;
    size_t numLeft = d_endPtr - d_markPtr;

    // adjust the position of buffer in input stream
    d_streamOffset += numConsumed;

    // shift left unprocessed bytes
    if (numLeft != 0 && d_startPtr != d_markPtr) {
        bsl::memmove(d_startPtr, d_markPtr, numLeft);
    }

    size_t chunkSize = d_parseBuf.size() - numLeft;

    if (chunkSize < BAEXML_MIN_BUFSIZE) {
        chunkSize = BAEXML_MIN_BUFSIZE;
    }

    if (d_parseBuf.size() < (numLeft + chunkSize + 1)) {
        d_parseBuf.resize(numLeft + chunkSize + 1);
    }

    size_t numRead = 0;
    char  *buffer = &d_parseBuf.at(numLeft);

    if (d_streamBuf != 0) {
        numRead = d_streamBuf->sgetn(buffer, chunkSize);
    }
    else if (d_memStream != 0 && d_memSize != 0) {

        numRead = (chunkSize < d_memSize) ? chunkSize : d_memSize;
        bsl::memmove(buffer, d_memStream, numRead);

        d_memStream += numRead;
        d_memSize -=numRead;
    }
    d_parseBuf[numLeft + numRead] = '\0';

    const char *newBase = &d_parseBuf.front();

    rebasePointers(newBase, numLeft + numRead);

    if (numRead == 0) {
        d_flags |= FLG_READ_EOF;
    }
    return numRead;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
