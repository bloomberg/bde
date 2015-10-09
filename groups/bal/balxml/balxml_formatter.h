// balxml_formatter.h                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALXML_FORMATTER
#define INCLUDED_BALXML_FORMATTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a simple interface for writing formatted XML.
//
//@CLASSES:
//  balxml::Formatter: provides formatted XML
//
//@SEE_ALSO:
//
//@DESCRIPTION: The 'balxml::Formatter' class provides methods to write a
// formatted XML to an underlining output stream.  These methods generate
// header, tags, data, attributes, comments in a human-readable, indented
// format.
//
// XML documents use a self-describing and simple syntax that consists of
// nested XML elements.  Each element is bounded by a pair of opening and
// closing tags.  Within the pair of tags, there can be more nested elements,
// or just plain text or numeric data in text format.  The opening tag of an
// element can also contain attributes in the form of name="value" pairs.
// This component provides methods to generate these XML ingredients and takes
// care of proper indentation and line wrapping.  Visit
// http://www.w3schools.com/xml/xml_syntax.asp for a complete tutorial.
//
///Usage
///-----
// Here is a basic example showing ten steps of how to create an XML document
// using this component's major manipulators:
//..
//  // 1. Create a formatter:
//  balxml::Formatter formatter(bsl::cout);
//
//  // 2. Add a header:
//  formatter.addHeader("UTF-8");
//
//  // 3. Open the root element,
//  //    Add attributes if there are any:
//  formatter.openElement("Fruits");
//
//  // 4. Open an element,
//  //    Add attributes if there are any:
//  formatter.openElement("Oranges");
//  formatter.addAttribute("farm", "Francis' Orchard"); // ' is escaped
//  formatter.addAttribute("size", 3.5);
//
//  // 5. If there are nested elements, recursively do steps 4 - 8:
//  // 6. Else, there are no more nested elements, add data:
//  formatter.openElement("pickDate");               // step 4
//  formatter.addData(bdlt::Date(2004, 8, 31));       // step 6
//  formatter.closeElement("pickDate");              // step 7
//  formatter.addElementAndData("Quantity", 12);     // step 8
//            // element "Quantity" has no attributes, can use
//            // shortcut 'addElementAndData' to complete steps
//            // 4, 6 and 7 in one shot.
//
//  // 7. Close the element:
//  formatter.closeElement("Oranges");
//
//  // 8. If there are more elements, repeat steps 4 - 8
//  formatter.openElement("Apples");                 // step 4
//  formatter.addAttribute("farm", "Fuji & Sons");   // '&' is escaped
//  formatter.addAttribute("size", 3);
//  formatter.closeElement("Apples");                // step 7
//
//  // 9. Close the root element:
//  formatter.closeElement("Fruits");
//..
// Indentation is correctly taken care of and the user need only concern
// her/himself with the correct ordering of the XML elements s/he's trying to
// write.  The output of the above example is:
//..
// +--bsl::cout---------------------------------------------------------------+
// |<?xml version="1.0" encoding="UTF-8" ?>                                   |
// |<Fruits>                                                                  |
// |    <Oranges farm="Francis&apos; Orchard" size="3.5">                     |
// |        <pickDate>2004-08-31</pickDate>                                   |
// |        <Quantity>12</Quantity>                                           |
// |    </Oranges>                                                            |
// |    <Apples farm="Fuji &amp; Sons" size="3"/>                             |
// |</Fruits>                                                                 |
// +--------------------------------------------------------------------------+
//..
// Following is a more complete usage example that uses most of the
// manipulators provided by balxml::Formatter:
//..
//          balxml::Formatter formatter(bsl::cout, 0, 4, 40);
//
//          formatter.addHeader("UTF-8");
//
//          formatter.openElement("Fruits");
//          formatter.openElement("Oranges");
//          formatter.addAttribute("farm", "Francis' Orchard");
//              // notice that the apostrophe in the string will be escaped
//          formatter.addAttribute("size", 3.5);
//
//          formatter.addElementAndData("Quantity", 12);
//
//          formatter.openElement("pickDate");
//          formatter.addData(bdlt::Date(2004, 8, 31));
//          formatter.closeElement("pickDate");
//
//          formatter.openElement("Feature");
//          formatter.addAttribute("shape", "round");
//          formatter.closeElement("Feature");
//
//          formatter.addComment("No wrapping for long comments");
//
//          formatter.closeElement("Oranges");
//
//          formatter.addBlankLine();
//
//          formatter.openElement("Apples");
//          formatter.addAttribute("farm", "Fuji & Sons");
//          formatter.addAttribute("size", 3);
//
//          formatter.openElement("pickDates",
//                                balxml::Formatter::BAEXML_NEWLINE_INDENT);
//          formatter.addListData(bdlt::Date(2005, 1, 17));
//          formatter.addListData(bdlt::Date(2005, 2, 21));
//          formatter.addListData(bdlt::Date(2005, 3, 25));
//          formatter.addListData(bdlt::Date(2005, 5, 30));
//          formatter.addListData(bdlt::Date(2005, 7, 4));
//          formatter.addListData(bdlt::Date(2005, 9, 5));
//          formatter.addListData(bdlt::Date(2005, 11, 24));
//          formatter.addListData(bdlt::Date(2005, 12, 25));
//
//          formatter.closeElement("pickDates");
//
//          formatter.openElement("Feature");
//          formatter.addAttribute("color", "red");
//          formatter.addAttribute("taste", "juicy");
//          formatter.closeElement("Feature");
//
//          formatter.closeElement("Apples");
//
//          formatter.closeElement("Fruits");
//
//          formatter.reset();
//          // reset the formatter for a new document in the same stream
//
//          formatter.addHeader();
//          formatter.openElement("Grains");
//
//          bsl::ostream& os = formatter.rawOutputStream();
//          os << "<free>anything that can mess up the XML doc</free>";
//          // Now coming back to the formatter, but can't do the following:
//          // formatter.addAttribute("country", "USA");
//          formatter.addData("Corn, Wheat, Oat");
//          formatter.closeElement("Grains");
//..
// Following are the two resulting documents, as separated by the call to
// reset(),
//..
// +--bsl::cout-----------------------------+
// |<?xml version="1.0" encoding="UTF-8" ?> |
// |<Fruits>                                |
// |    <Oranges                            |
// |         farm="Francis&apos; Orchard"   |
// |         size="3.5">                    |
// |        <Quantity>12</Quantity>         |
// |        <pickDate>2004-08-31</pickDate> |
// |        <Feature shape="round"/>        |
// |        <!-- No wrapping for long comments --> |
// |    </Oranges>                          |
// |                                        |
// |    <Apples farm="Fuji &amp; Sons"      |
// |         size="3">                      |
// |        <pickDates>                     |
// |            2005-01-17 2005-02-21       |
// |            2005-03-25 2005-05-30       |
// |            2005-07-04 2005-09-05       |
// |            2005-11-24 2005-12-25       |
// |        </pickDates>                    |
// |        <Feature color="red"            |
// |             taste="juicy"/>            |
// |    </Apples>                           |
// |</Fruits>                               |
// +----------------------------------------+
// +--bsl::cout-----------------------------+
// |<?xml version="1.0" encoding="UTF-8" ?> |
// |<Grains><free>anything that can mess up the XML doc</free>
// |              Corn, Wheat, Oat</Grains> |
// +----------------------------------------+
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALXML_TYPESPRINTUTIL
#include <balxml_typesprintutil.h>
#endif

#ifndef INCLUDED_BDLAT_FORMATTINGMODE
#include <bdlat_formattingmode.h>
#endif

#ifndef INCLUDED_BDLMA_BUFFEREDSEQUENTIALALLOCATOR
#include <bdlma_bufferedsequentialallocator.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BDLSB_MEMOUTSTREAMBUF
#include <bdlsb_memoutstreambuf.h>
#endif

#ifndef INCLUDED_BDLT_DATE
#include <bdlt_date.h>
#endif

#ifndef INCLUDED_BDLT_DATETIME
#include <bdlt_datetime.h>
#endif

#ifndef INCLUDED_BDLT_TIME
#include <bdlt_time.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>       // bsl::pair
#endif

namespace BloombergLP {

namespace balxml {
                              // ===============
                              // class Formatter
                              // ===============

class Formatter {
    // This class provides a set of XML-style formatting utilities that enable
    // transparent indentation and wrapping for users attempting to format data
    // with XML tags and attributes.  A formatter object is instantiated with a
    // pointer to an output stream or streambuf.  Users can then use the
    // provided utilities to write element tags, attributes, data in a valid
    // XML sequence into the underlying stream.
    //
    // This class has no features that would impair thread safety.  However, it
    // does not mediate between two threads attempting to access the same
    // stream.

  public:
    // CLASS TYPES
    enum WhitespaceType {
        // This describes options available when outputting textual data of an
        // element between its pair of opening and closing tags.

        e_PRESERVE_WHITESPACE,  // data is output as is

        e_WORDWRAP,             // data may be wrapped if output otherwise
                                // exceeds the wrap column

        e_WORDWRAP_INDENT,      // in addition to allowing wrapping, indent
                                // properly before continuing to output on the
                                // next line after wrapping

        e_NEWLINE_INDENT        // in addition to allowing wrapping and
                                // indentation, the tags do not share their
                                // respective lines with data


        // Current implementation does not provide the capability of analyzing
        // the internal whitespace in individual textual data and breaking the
        // line at such whitespace.  However, these options are meaningful
        // when individual data is output as part of a list, as in the case of
        // consecutive calls to 'addListData'.
    };

  private:
    typedef bsls::Types::Int64 Int64;

    enum State {
        e_AT_START,
        e_AFTER_START_NO_TAG,
        e_IN_TAG,
        e_BETWEEN_TAGS,
        e_AT_END
    };

    class ElemContext;
    friend class ElemContext;
    class ElemContext {
        // For use in element nesting stack.  Keep track of the whitespace
        // formatting mode and the tag (in safe mode) for an open element.
        WhitespaceType d_ws;
#ifdef BDE_BUILD_TARGET_SAFE_2
        // Use a fixed-length string to validate close tag against open tag.
        // If tag is longer than the maximum length, only the first
        // 'TRUNCATED_TAG_LEN' characters are checked.
        enum {
            k_TRUNCATED_TAG_LEN = 15
        };

        unsigned char d_tagLen;  // actual tag length, up to 255
        char          d_tag[k_TRUNCATED_TAG_LEN]; // truncated tag
#endif
      public:
        ElemContext(const bslstl::StringRef& tag, WhitespaceType ws);

        // Use compiler-generated copy constructor, assignment, and destructor.

        void setWs(WhitespaceType ws);
        WhitespaceType ws() const;
#ifdef BDE_BUILD_TARGET_SAFE_2
        bool matchTag(const bslstl::StringRef& tag) const;
#endif
    };

    // IMPORTANT: Do not re-order the following two declarations:
    bsl::ostream            d_outputStreamObj;  // Locally-constructed ostream
    bsl::ostream&           d_outputStream;     // Reference to active ostream
    int                     d_indentLevel;
    int                     d_spacesPerLevel;
    int                     d_column;
    int                     d_wrapColumn;
    bsl::vector<ElemContext> d_elementNesting;
                            // a stack of names of currently nested elements
                            // with the whitespace handling constraint for
                            // each element in the stack.
    State                   d_state;
    bool                    d_isFirstData;
                            // indicate whether the data to be added is the
                            // first data between its enclosing tags.
    bool                    d_isFirstDataAtLine;
                            // indicate whether the data to be added is the
                            // first data on a line.  This is meaningful only
                            // to addListData when it's called more than once
                            // between its enclosing tags.

    // NOT IMPLEMENTED
    Formatter(const Formatter&);
    Formatter& operator=(const Formatter&);

  private:
    // PRIVATE MANIPULATORS
    void doAddAttribute(const bslstl::StringRef& name,
                        const bslstl::StringRef& value);
        // Add an attribute of the specified 'name' that with the specified
        // 'value'.  Line is wrapped if the length of name="value" is too long
        // to fit on the current line.

    void doAddData(const bslstl::StringRef& value, bool addSpace);
        // Add the specified 'value' in the current element.  If the 'value' is
        // not the first data on a line, prefix the 'value' with a
        // space('0x20') if the specified 'addSpace' is true.  In case adding
        // the data makes the line too long, line may be wrapped only if
        // 'addSpace' is true and the current element is not opened with
        // 'BAEXML_PRESERVE_WHITESPACE'.  Indent after the wrapping only if the
        // current element is opened with 'BAEXML_WORDWRAP_INDENT' or
        // 'BAEXML_NEWLINE_INDENT'.

    void closeTagIfOpen();
        // Write '>' to stream to complete a just opened tag.  If the opening
        // tag was already completed with '>', do nothing.

    void indent();
        // Indent the current XML line to a column corresponding to the indent
        // level.  If cursor is currently at column 0, indent only, otherwise,
        // go to a new line and indent.

  public:
    // CREATORS
    Formatter(bsl::streambuf   *output,
              int               indentLevel =     0,
              int               spacesPerLevel =  4,
              int               wrapColumn =      80,
              bslma::Allocator *basic_allocator = 0);
    Formatter(bsl::ostream&     output,
              int               indentLevel =     0,
              int               spacesPerLevel =  4,
              int               wrapColumn =      80,
              bslma::Allocator *basic_allocator = 0);
        // Construct an object to format XML data into the specified 'output'
        // stream or streambuf.  Optionally specify initial 'indentLevel',
        // 'spacesPerLevel', and 'wrapColumn' for formatting.  An
        // 'indentLevel' of 0 (the default) indicates the root element will
        // have no indentation.  A 'wrapColumn' of 0 will cause the formatter
        // to behave as though the line length were infinite, but will still
        // insert newlines and indent when starting a new element.  A
        // 'wrapColumn' of -1 will cause output to be formatted in "compact"
        // mode -- with no added newlines or indentation.  The behavior is
        // undefined if the 'output' stream or streambuf is destroyed before
        // this object goes out of scope.

    ~Formatter();
        // Destroy this object.

    // MANIPULATORS
    template <class TYPE>
    void addAttribute(const bslstl::StringRef& name,
                      const TYPE&              value,
                      int                      formattingMode = 0);
        // Add an attribute the specified 'name' and specified 'value' to the
        // currently open element.  'value' can be of the following types:
        // 'char', 'short', 'int', 'bsls::Types::Int64', 'float', 'double',
        // 'bsl::string', 'bdlt::Datetime', 'bdlt::Date', and 'bdlt::Time'.
        // Precede this name="value" pair with a single space.  Wrap line
        // (write the attribute on next line with proper indentation), if the
        // length of name="value" is too long.  The behavior is undefined
        // unless the last manipulator was 'openElement' or 'addAttribute'.  If
        // 'value' is of type 'bsl::string', it is truncated at any invalid
        // UTF-8 byte-sequence or any control character '[0x00, 0x20)' except
        // '0x9', '0xA', and '0x0D', and escaped for five special characters:
        // apostrophe, double quote, ampersand, less than, and greater than.
        // If 'value' is of type 'char', it is cast to a signed byte value with
        // a range '[ -128 .. 127 ]'.

    void addBlankLine();
        // Insert one or two newline characters into the output stream such
        // that a blank line results.  If the last output was a newline, then
        // only one newline is added, otherwise two newlines are added.  If
        // following a call to 'openElement', or 'addAttribute', add a closing
        // '>' to the opened tag.

    void addComment(const bslstl::StringRef& comment,
                    bool                     forceNewline = true);
        // Write the specified 'comment' into the stream.  The specified
        // 'forceNewLine', if true, forces to start a new line solely for the
        // comment if it's not on a new line already.  Otherwise, comments
        // continue on current line.  If an element-opening tag is not
        // completed with a '>', 'addComment' will add '>'.

    template <class TYPE>
    void addData(const TYPE& value, int formattingMode = 0);

    template <class TYPE>
    void addListData(const TYPE& value, int formattingMode = 0);
        // Add the specified 'value' as the data content, where 'value' can be
        // of the following types: 'char', 'short', 'int',
        // 'bsls::Types::Int64', 'float', 'double', 'bsl::string',
        // 'bdlt::Datetime', 'bdlt::Date', and 'bdlt::Time'.  'addListData'
        // prefixes the 'value' with a space('0x20') unless the data being
        // added is the first data on a line.  In the case of 'addData',
        // perform no line-wrapping or indentation as if the whitespace
        // constraint were always 'BAEXML_PRESERVE_WHITESPACE' in
        // 'openElement', with the only exception that an initial newline and
        // an initial indent is added when 'openElement' specifies
        // 'BAEXML_NEWLINE_INDENT' option.  In the case of 'addListData', when
        // adding the data makes the line too long, perform line-wrapping and
        // indentation as determined by the whitespace constraint used when the
        // current element is opened with 'openElement'.  Behavior is undefined
        // if the call is made when there are no opened elements.  If 'value'
        // is of type 'bsl::string', it is truncated at invalid UTF-8
        // byte-sequence or any control character '[0x00, 0x20)' except '0x9',
        // '0xA', and '0xD', and escaped for five special characters:
        // apostrophe, double quote, ampersand, less than, and greater than.
        // If 'value' is of type 'char', it is cast to a signed byte value with
        // a range of '[ -128 .. 127 ]'.  Optionally specify the
        // 'formattingMode'.

    template <class TYPE>
    void addElementAndData(const bslstl::StringRef& name,
                           const TYPE&              value,
                           int                      formattingMode = 0);
        // Add element of the specified 'name' and the specified 'value' as the
        // data content.  This has the same effect as calling the following
        // sequence: 'openElement(name); addData(value), closeElement(name);'.
        // Optionally specify the 'formattingMode'.

    void addHeader(const bslstl::StringRef& encoding = "UTF-8");
        // Add XML header with optionally specified 'encoding'.  Version is
        // always "1.0".  Behavior is undefined unless 'addHeader' is the first
        // manipulator (with the exception of 'rawOutputStream') after
        // construction or 'reset'.

    void addNewline();
        // Insert a literal newline into the XML output.  If following a call
        // to 'openElement', or 'addAttribute', add a closing '>' to the opened
        // tag.

    void closeElement(const bslstl::StringRef& name);
        // Decrement the indent level and add the closing tag for the element
        // of the specified 'name'.  If the element does not have content,
        // write '/>' and a newline into stream.  Otherwise, write '</name>'
        // and a newline.  If this '</name>' does not share the same line with
        // data, or it follows another element's closing tag, indent properly
        // before writing '</name>' and the newline.  If 'name' is root
        // element, flush the output stream.  Behavior is undefined if 'name'
        // is not the most recently opened element that's yet to be closed.

    void flush();
        // Insert the closing '>' if there is an incomplete tag, and flush the
        // output stream.

    void openElement(const bslstl::StringRef& name,
                     WhitespaceType           ws = e_PRESERVE_WHITESPACE);
        // Open an element of the specified 'name' at current indent level with
        // the optionally specified whitespace constraint 'ws' for its textual
        // data and increment indent level.  'ws' constrains how textual data
        // is written with 'addListData' for the current element, but not its
        // nested elements.  Behavior is undefined if 'openELement' is called
        // after the root element is closed and there is no subsequent call to
        // 'reset'.

    bsl::ostream& rawOutputStream();
        // Return a reference to the underlining output stream.  This method is
        // provided in order to enable user to temporarily jump out of the
        // formatter and write user's own free-lance content directly to the
        // stream.

    void reset();
        // Reset the formatter such that it can be used to format a new XML
        // document as if the formatter were just constructed

    // ACCESSORS
    int outputColumn() const;
        // Return the current column position at a line where next output
        // starts.  This is unreliable if called after free-lance information
        // is written onto the stream returned by 'rawOutputStream'

    int indentLevel() const;
        // Return the current level of indentation.

    int spacesPerLevel() const;
        // Return the number of spaces per indentation level

    int status() const;
        // Return 0 if no errors have been detected since construction or
        // since the last call to 'reset', otherwise return a negative value.

    int wrapColumn() const;
        // Return the line width where line-wrapping takes place
};
}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                    // ------------------------------------
                    // class balxml::Formatter::ElemContext
                    // ------------------------------------

// CREATORS
#ifdef BDE_BUILD_TARGET_SAFE_2
inline
balxml::Formatter::ElemContext::ElemContext(const bslstl::StringRef& tag,
                                           WhitespaceType            ws)
: d_ws(ws), d_tagLen(bsl::min(tag.length(), 255))
{
    int len = bsl::min(int(k_TRUNCATED_TAG_LEN), tag.length());
    bsl::memcpy(d_tag, tag.data(), len);
}
#else
inline
balxml::Formatter::ElemContext::ElemContext(const bslstl::StringRef&,
                                            WhitespaceType           ws)
: d_ws(ws)
{
}
#endif

// MANIPULATORS
inline
void balxml::Formatter::ElemContext::setWs(WhitespaceType ws)
{
    d_ws = ws;
}

// ACCESSORS
inline
balxml::Formatter::WhitespaceType balxml::Formatter::ElemContext::ws() const
{
    return d_ws;
}

namespace balxml {
// PRIVATE MANIPULATORS
inline
void Formatter::closeTagIfOpen()
{
    if (e_IN_TAG == d_state) {
        d_outputStream << '>';
        ++d_column;
        d_state = e_BETWEEN_TAGS;
    }
}

// CREATORS
inline
Formatter::~Formatter()
{
}

// MANIPULATORS
template <class TYPE>
void Formatter::addAttribute(const bslstl::StringRef& name,
                             const TYPE&              value,
                             int                      formattingMode)
{
    if (d_wrapColumn > 0) {
        // Format attribute into string, to allow for intelligent line-wrapping
        const int BAEXML_FORMATTER_BUF_SIZE = 256;
        char      buffer[BAEXML_FORMATTER_BUF_SIZE];

        bdlma::BufferedSequentialAllocator allocator(
                                                    buffer,
                                                    BAEXML_FORMATTER_BUF_SIZE);

        bdlsb::MemOutStreamBuf sb(&allocator);
        bsl::ostream ss(&sb);

        TypesPrintUtil::print(ss, value, formattingMode);
        if (!ss.good()) {
            d_outputStream.setstate(bsl::ios_base::failbit);
            return;                                                   // RETURN
        }

        doAddAttribute(name, bslstl::StringRef(sb.data(), (int)sb.length()));
    }
    else {
        // Blast attribute to stream without line-wrapping
        d_outputStream << ' ' << name << "=\"";
        TypesPrintUtil::print(d_outputStream, value, formattingMode);
        d_outputStream << '"';
        d_column += name.length() + 4;  // Minimum output if value is empty
    }
}

template <class TYPE>
void Formatter::addData(const TYPE& value, int formattingMode)
{
    closeTagIfOpen();
    if (d_wrapColumn > 0) {
        // Format data into string, to allow for intelligent line-wrapping
        const int BAEXML_FORMATTER_BUF_SIZE = 256;
        char      buffer[BAEXML_FORMATTER_BUF_SIZE];

        bdlma::BufferedSequentialAllocator
                                  allocator(buffer, BAEXML_FORMATTER_BUF_SIZE);

        bdlsb::MemOutStreamBuf sb(&allocator);
        bsl::ostream ss(&sb);

        TypesPrintUtil::print(ss, value, formattingMode);
        if (!ss.good()) {
            d_outputStream.setstate(bsl::ios_base::failbit);
            return;                                                   // RETURN
        }

        doAddData(bslstl::StringRef(sb.data(), (int)sb.length()), false);
    }
    else {
        // Blast data to stream without line-wrapping
        TypesPrintUtil::print(d_outputStream, value, formattingMode);
        d_column += 1; // Assume value is not empty
        d_isFirstData = false;
        d_isFirstDataAtLine = false;
    }
}

template <class TYPE>
void Formatter::addListData(const TYPE& value, int formattingMode)
{
    closeTagIfOpen();
    if (d_wrapColumn > 0) {
        // Format data into string, to allow for intelligent line-wrapping
        const int BAEXML_FORMATTER_BUF_SIZE = 256;
        char      buffer[BAEXML_FORMATTER_BUF_SIZE];

        bdlma::BufferedSequentialAllocator
                                  allocator(buffer, BAEXML_FORMATTER_BUF_SIZE);

        bdlsb::MemOutStreamBuf sb(&allocator);
        bsl::ostream ss(&sb);

        TypesPrintUtil::print(ss, value, formattingMode);
        if (!ss.good()) {
            d_outputStream.setstate(bsl::ios_base::failbit);
            return;                                                   // RETURN
        }

        doAddData(bslstl::StringRef(sb.data(), (int)sb.length()), true);
    }
    else {
        // Blast data to stream without line-wrapping
        if (!d_isFirstData) {
            d_outputStream << ' ';
        }
        TypesPrintUtil::print(d_outputStream, value, formattingMode);
        d_column += 1; // Assume value is not empty
        d_isFirstData = false;
        d_isFirstDataAtLine = false;
    }
}

template <class TYPE>
inline
void Formatter::addElementAndData(const bslstl::StringRef& name,
                                  const TYPE&              value,
                                  int                      formattingMode)
{
    openElement(name);
    addData(value, formattingMode);
    closeElement(name);
}

inline
void Formatter::addBlankLine()
{
    closeTagIfOpen();
    if (d_column > 0) {
        d_outputStream << '\n';
    }
    d_outputStream << '\n';
    d_column = 0;
}

inline
void Formatter::addNewline()
{
    closeTagIfOpen();
    d_outputStream << '\n';
    d_column = 0;
}

inline
void Formatter::flush()
{
    closeTagIfOpen();
    d_outputStream.flush();
}

inline
bsl::ostream& Formatter::rawOutputStream()
{
    closeTagIfOpen();
    d_column = d_wrapColumn + 1;  // Make column invalid
    return d_outputStream;
}

// ACCESSORS
inline
int Formatter::outputColumn() const
{
    return d_column;
}

inline
int Formatter::indentLevel() const
{
    return d_indentLevel;
}

inline
int Formatter::spacesPerLevel() const
{
    return d_spacesPerLevel;
}

inline
int Formatter::status() const
{
    return d_outputStream.good() ? 0 : -1;
}

inline
int Formatter::wrapColumn() const
{
    return d_wrapColumn;
}
}  // close package namespace

}  // close enterprise namespace

#endif

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
