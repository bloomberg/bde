// balxml_formatter.h                                                 -*-C++-*-
#ifndef INCLUDED_BALXML_FORMATTER
#define INCLUDED_BALXML_FORMATTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a simple interface for writing formatted XML.
//
//@CLASSES:
//  balxml::Formatter: provides formatted XML
//
//@SEE_ALSO:
//
//@DESCRIPTION: The `balxml::Formatter` class provides methods to write a
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
///Special Characters
///------------------
// XML defines five special characters that must not appear text; instead
// these characters are must be represented by multi-byte escape sequences.
// ```
// Special    (Hex)                 XML Escape
// Character  Value  Description    Sequence
// ---------  -----  -----------    ----------
// "          x22    quote          &quot;
// &          x26    ampersand      &amp;
// '          x27    apostrophe     &apos;
// <          x3C    less than      &lt;
// >          x3E    greater than   &gt;
// ```
// The following methods:
// * `addAttribute`,
// * `addData`, and
// * `addListData`
// implicitly convert each special character found in string input to the
// appropriate escape sequence in the resulting XML document.
//
///Valid Strings
///-------------
// Strings used to set element attributes and element data (see `addAttribute`,
// `addData`, and `addListData`) must consist of (valid) UTF-8 byte sequences
// excepting certain control characters.
// ```
//  Control
//  Characters  Description                 Allowed
//  ----------- --------------------------  -------
//  x09         HT  '\t' (horizontal tab)   true
//  x0A         LF  '\n' (new line)         true
//  x0D         CR  '\r' (carriage return)  true
//  x7F         DEL      (delete)           false
//
//  x01 .. 0x1F Other than HT, LF, and CR.  false
// ```
// Notice that range of 31 control characters between `0x01` and `0x1F`
// (inclusive) consist of three that are allowed and 28 that are not.
//
// The detection of an invalid character in an input stream stops the transfer
// of data to the output stream.  The output stream is put into a failed state.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// This example shows ten steps of how to create an XML document using this
// component's major manipulators:
// ```
// // 1. Create a formatter:
// balxml::Formatter formatter(bsl::cout);
//
// // 2. Add a header:
// formatter.addHeader("UTF-8");
//
// // 3. Open the root element,
// //    Add attributes if there are any:
// formatter.openElement("Fruits");
//
// // 4. Open an element,
// //    Add attributes if there are any:
// formatter.openElement("Oranges");
// formatter.addAttribute("farm", "Francis' Orchard"); // ' is escaped
// formatter.addAttribute("size", 3.5);
//
// // 5. If there are nested elements, recursively do steps 4 - 8:
// // 6. Else, there are no more nested elements, add data:
// formatter.openElement("pickDate");               // step 4
// formatter.addData(bdlt::Date(2004, 8, 31));       // step 6
// formatter.closeElement("pickDate");              // step 7
// formatter.addElementAndData("Quantity", 12);     // step 8
//           // element "Quantity" has no attributes, can use
//           // shortcut 'addElementAndData' to complete steps
//           // 4, 6 and 7 in one shot.
//
// // 7. Close the element:
// formatter.closeElement("Oranges");
//
// // 8. If there are more elements, repeat steps 4 - 8
// formatter.openElement("Apples");                 // step 4
// formatter.addAttribute("farm", "Fuji & Sons");   // '&' is escaped
// formatter.addAttribute("size", 3);
// formatter.closeElement("Apples");                // step 7
//
// // 9. Close the root element:
// formatter.closeElement("Fruits");
// ```
// Indentation is correctly taken care of and the user need only concern
// themselves with the correct ordering of the XML elements they're trying to
// write.  The output of the above example is:
// ```
// +--bsl::cout--------------------------------------------------------------+
// |<?xml version="1.0" encoding="UTF-8" ?>                                  |
// |<Fruits>                                                                 |
// |    <Oranges farm="Francis&apos; Orchard" size="3.5">                    |
// |        <pickDate>2004-08-31</pickDate>                                  |
// |        <Quantity>12</Quantity>                                          |
// |    </Oranges>                                                           |
// |    <Apples farm="Fuji &amp; Sons" size="3"/>                            |
// |</Fruits>                                                                |
// +-------------------------------------------------------------------------+
// ```
// Following is a more complete usage example that uses most of the
// manipulators provided by balxml::Formatter:
// ```
// balxml::Formatter formatter(bsl::cout, 0, 4, 40);
//
// formatter.addHeader("UTF-8");
//
// formatter.openElement("Fruits");
// formatter.openElement("Oranges");
// formatter.addAttribute("farm", "Francis' Orchard");
//     // notice that the apostrophe in the string will be escaped
// formatter.addAttribute("size", 3.5);
//
// formatter.addElementAndData("Quantity", 12);
//
// formatter.openElement("pickDate");
// formatter.addData(bdlt::Date(2004, 8, 31));
// formatter.closeElement("pickDate");
//
// formatter.openElement("Feature");
// formatter.addAttribute("shape", "round");
// formatter.closeElement("Feature");
//
// formatter.addComment("No wrapping for long comments");
//
// formatter.closeElement("Oranges");
//
// formatter.addBlankLine();
//
// formatter.openElement("Apples");
// formatter.addAttribute("farm", "Fuji & Sons");
// formatter.addAttribute("size", 3);
//
// formatter.openElement("pickDates",
//                       balxml::Formatter::BAEXML_NEWLINE_INDENT);
// formatter.addListData(bdlt::Date(2005, 1, 17));
// formatter.addListData(bdlt::Date(2005, 2, 21));
// formatter.addListData(bdlt::Date(2005, 3, 25));
// formatter.addListData(bdlt::Date(2005, 5, 30));
// formatter.addListData(bdlt::Date(2005, 7, 4));
// formatter.addListData(bdlt::Date(2005, 9, 5));
// formatter.addListData(bdlt::Date(2005, 11, 24));
// formatter.addListData(bdlt::Date(2005, 12, 25));
//
// formatter.closeElement("pickDates");
//
// formatter.openElement("Feature");
// formatter.addAttribute("color", "red");
// formatter.addAttribute("taste", "juicy");
// formatter.closeElement("Feature");
//
// formatter.closeElement("Apples");
//
// formatter.closeElement("Fruits");
//
// formatter.reset();
// // reset the formatter for a new document in the same stream
//
// formatter.addHeader();
// formatter.openElement("Grains");
//
// bsl::ostream& os = formatter.rawOutputStream();
// os << "<free>anything that can mess up the XML doc</free>";
// // Now coming back to the formatter, but can't do the following:
// // formatter.addAttribute("country", "USA");
// formatter.addData("Corn, Wheat, Oat");
// formatter.closeElement("Grains");
// ```
// Following are the two resulting documents, as separated by the call to
// reset(),
// ```
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
// ```

#include <balscm_version.h>

#include <balxml_encoderoptions.h>
#include <balxml_formatter_compactimpl.h>
#include <balxml_formatter_prettyimpl.h>
#include <balxml_formatterwhitespacetype.h>

#include <bslma_allocator.h>
#include <bslma_bslallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_assert.h>
#include <bsls_keyword.h>
#include <bsls_objectbuffer.h>

#include <bslstl_inplace.h>

#include <bsl_optional.h>
#include <bsl_ostream.h>
#include <bsl_streambuf.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace balxml {

class Formatter;

                        // ============================
                        // class Formatter_StreamHolder
                        // ============================

/// This component-private class provides a mechanism for holding the
/// `bsl::ostream` used by the `Formatter` to emit XML documents.  Objects
/// of `Formatter_StreamHolder` type can be constructed with either a
/// `bsl::streambuf *` or a `bsl::ostream *`.  If a stream holder is
/// constructed with a `bsl::streambuf *`, then it owns its held
/// `bsl::ostream`, which is constructed with the supplied stream buffer.
/// If a stream holder is constructed with a `bsl::ostream *`, its held
/// `bsl::ostream` is the supplied stream.
class Formatter_StreamHolder {

    // DATA

    // `bsl::ostream` if constructed with a `bsl::streambuf *`, and
    // disengaged otherwise
    bsl::optional<bsl::ostream>  d_ownStream;

    // the held `bsl::ostream`, which is equal to `&d_ownStream.value()` if
    // `d_ownStream` is engaged, and the `bsl::ostream *` supplied on
    // construction otherwise
    bsl::ostream                *d_stream_p;

    // NOT IMPLEMENTED
    Formatter_StreamHolder(const Formatter_StreamHolder&) BSLS_KEYWORD_DELETED;
    Formatter_StreamHolder& operator=(
                           const Formatter_StreamHolder&) BSLS_KEYWORD_DELETED;

  public:
    // CREATORS

    /// Create a `Formatter_StreamHolder` object that holds a `bsl::ostream`
    /// constructed from the specified `streamBuffer`.
    explicit Formatter_StreamHolder(bsl::streambuf *streamBuffer);

    /// Create a `Formatter_StreamHolder` that holds the specified `stream`.
    explicit Formatter_StreamHolder(bsl::ostream *stream);

    // MANIPULATORS

    /// Return the address that provides modifiable access to this object's
    /// held `bsl::ostream`.
    bsl::ostream *stream();

    // ACCESSORS

    /// Return the address that provides non-modifiable access to this
    /// object's held `bsl::ostream`.
    const bsl::ostream *stream() const;
};

                           // =====================
                           // struct Formatter_Mode
                           // =====================

/// This component-private utility `struct` provides a namespace for
/// enumerating the set of formatting modes that the `Formatter` can take.
struct Formatter_Mode {

    // TYPES
    enum Enum {
        e_COMPACT,
        e_PRETTY
    };
};

                           // =====================
                           // class Formatter_State
                           // =====================

/// This component-private, in-core, value-semantic class provides a variant
/// that can be inhabited by an object of either the component-private
/// `Formatter_CompactImplState` type or the component-private
/// `Formatter_PrettyImplState` type.
class Formatter_State {

  public:
    // TYPES
    typedef bsl::allocator<char> allocator_type;
    typedef Formatter_Mode       Mode;

  private:
    // PRIVATE TYPES
    typedef Formatter_CompactImplState Compact;
    typedef Formatter_PrettyImplState  Pretty;
    typedef FormatterWhitespaceType    WhitespaceType;

    enum { k_COMPACT_MODE_WRAP_COLUMN = -1 };

    // DATA
    Mode::Enum d_mode;
    union {
        bsls::ObjectBuffer<Compact> d_compact;
        bsls::ObjectBuffer<Pretty>  d_pretty;
    };
    allocator_type d_allocator;

    // PRIVATE CREATORS

    /// If the specified `wrapColumn` is -1, create a `Formatter_State`
    /// object having a `compact` selection, which is a
    /// `Formatter_CompactImplState` constructed with the specified
    /// `indentLevel` and `spacesPerLevel`.  Otherwise, create a
    /// `Formatter_State` object having a `pretty` selection, which is a
    /// `Formatter_PrettyImplState` constructed with the `indentLevel`,
    /// `spacesPerLevel`, and `wrapColumn`.  Optionally specify an
    /// `allocator` (e.g., the address of a `bslma::Allocator` object) to
    /// supply memory; otherwise, the default allocator is used.
    Formatter_State(int                   indentLevel,
                    int                   spacesPerLevel,
                    int                   wrapColumn,
                    const allocator_type& allocator = allocator_type());

    // PRIVATE MANIPULATORS

    /// Return a reference providing modifiable access to the `compact`
    /// selection of this object.  The behavior is undefined unless the
    /// current selection of this object is `compact`.
    Compact& compact();

    /// Return a reference providing modifiable access to the `pretty`
    /// selection of this object.  The behavior is undefined unless the
    /// current selection of this object is `pretty`.
    Pretty& pretty();

    // PRIVATE ACCESSORS

    /// Return a reference providing non-modifiable access to the `compact`
    /// selection of this object.  The behavior is undefined unless the
    /// current selection of this object is `compact`.
    const Compact& compact() const;

    /// Return `Mode::e_COMPACT` if the current selection of this object is
    /// `compact`, and return `Mode::e_PRETTY` otherwise.
    Mode::Enum mode() const;

    /// Return a reference providing non-modifiable access to the `pretty`
    /// selection of this object.  The behavior is undefined unless the
    /// current selection of this object is `pretty`.
    const Pretty& pretty() const;

    // FRIENDS
    friend class balxml::Formatter;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Formatter_State, bslma::UsesBslmaAllocator);

    // CREATORS

    /// Create a `Formatter_State` with a `compact` selection having the
    /// default value.  Optionally specify an `allocator` (e.g., the address
    /// of a `bslma::Allocator` object) to supply memory; otherwise, the
    /// default allocator is used.
    Formatter_State();
    explicit Formatter_State(const allocator_type& allocator);

    /// Create a `Formatter_State` object having the same value as the
    /// specified `original` object.  Optionally specify an `allocator`
    /// (e.g., the address of a `bslma::Allocator` object) to supply memory;
    /// otherwise, the default allocator is used.
    Formatter_State(const Formatter_State& original,
                    const allocator_type&  allocator = allocator_type());

    /// Destroy this object.
    ~Formatter_State();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` and return a
    /// reference to this object.
    Formatter_State& operator=(const Formatter_State& rhs);

    // ACCESSORS

    /// Return the allocator associated with this object.
    allocator_type get_allocator() const;
};

                              // ===============
                              // class Formatter
                              // ===============

/// This class provides a set of XML-style formatting utilities that enable
/// transparent indentation and wrapping for users attempting to format data
/// with XML tags and attributes.  A formatter object is instantiated with a
/// pointer to an output stream or streambuf.  Users can then use the
/// provided utilities to write element tags, attributes, data in a valid
/// XML sequence into the underlying stream.
///
/// This class has no features that would impair thread safety.  However, it
/// does not mediate between two threads attempting to access the same
/// stream.
class Formatter {

  public:
    // TYPES

    /// `WhitespaceType` describes options available when outputting textual
    /// data of an element between its pair of opening and closing tags.
    typedef FormatterWhitespaceType::Enum WhitespaceType;

    // PUBLIC CLASS DATA
#ifdef BDE_VERIFY
#pragma bde_verify push
#pragma bde_verify -MN03
#pragma bde_verify -UC01
#endif

    static const WhitespaceType e_PRESERVE_WHITESPACE =
        FormatterWhitespaceType::e_PRESERVE_WHITESPACE;
    // data is output as is

    static const WhitespaceType e_WORDWRAP =
        FormatterWhitespaceType::e_WORDWRAP;
    // data may be wrapped if output otherwise exceeds the wrap column

    static const WhitespaceType e_WORDWRAP_INDENT =
        FormatterWhitespaceType::e_WORDWRAP_INDENT;
    // in addition to allowing wrapping, indent properly before continuing to
    // output on the next line after wrapping

    static const WhitespaceType e_NEWLINE_INDENT =
        FormatterWhitespaceType::e_NEWLINE_INDENT;
    // in addition to allowing wrapping and indentation, the tags do not share
    // their respective lines with data

    static const WhitespaceType BAEXML_NEWLINE_INDENT =
        FormatterWhitespaceType::e_NEWLINE_INDENT;
    // @DEPRECATED: Use 'e_NEWLINE_INDENT' instead.

#ifdef BDE_VERIFY
#pragma bde_verify pop
#endif

  private:
    // PRIVATE TYPES
    typedef Formatter_CompactImplUtil CompactUtil;
    typedef Formatter_PrettyImplUtil  PrettyUtil;
    typedef Formatter_StreamHolder    StreamHolder;
    typedef Formatter_Mode            Mode;
    typedef Formatter_State           State;

    // DATA
    StreamHolder   d_streamHolder;
    State          d_state;
    EncoderOptions d_encoderOptions;

    // NOT IMPLEMENTED
    Formatter(const Formatter&);
    Formatter& operator=(const Formatter&);

  public:
    // CREATORS

    /// Construct an object to format XML data into the specified `output`
    /// stream or streambuf.  Optionally specify `encoderOptions`, initial
    /// `indentLevel`, `spacesPerLevel`, and `wrapColumn` for formatting.
    /// An `indentLevel` of 0 (the default) indicates the root element will
    /// have no indentation.  A `wrapColumn` of 0 will cause the formatter
    /// to behave as though the line length were infinite, but will still
    /// insert newlines and indent when starting a new element.  A
    /// `wrapColumn` of -1 will cause output to be formatted in "compact"
    /// mode -- with no added newlines or indentation.  Optionally specify a
    /// `basicAllocator` used to supply memory.  If `basicAllocator` is 0,
    /// the currently install default allocator is used.  The behavior is
    /// undefined if the `output` stream or streambuf is destroyed before
    Formatter(bsl::streambuf   *output,
              int               indentLevel    = 0,
              int               spacesPerLevel = 4,
              int               wrapColumn     = 80,
              bslma::Allocator *basicAllocator = 0);  // IMPLICIT
    Formatter(bsl::ostream&     output,
              int               indentLevel    = 0,
              int               spacesPerLevel = 4,
              int               wrapColumn     = 80,
              bslma::Allocator *basicAllocator = 0);  // IMPLICIT
    Formatter(bsl::streambuf        *output,
              const EncoderOptions&  encoderOptions,
              int                    indentLevel    = 0,
              int                    spacesPerLevel = 4,
              int                    wrapColumn     = 80,
              bslma::Allocator      *basicAllocator = 0);
    Formatter(bsl::ostream&          output,
              const EncoderOptions&  encoderOptions,
              int                    indentLevel    = 0,
              int                    spacesPerLevel = 4,
              int                    wrapColumn     = 80,
              bslma::Allocator      *basicAllocator = 0);

    // MANIPULATORS

    /// Add an attribute of the specified `name` and specified `value` to
    /// the currently open element.  `value` can be of the following types:
    /// `char`, `short`, `int`, `bsls::Types::Int64`, `float`, `double`,
    /// `bsl::string`, `bdlt::Datetime`, `bdlt::Date`, and `bdlt::Time`.
    /// Precede this name="value" pair with a single space.  Wrap line
    /// (write the attribute on next line with proper indentation), if the
    /// length of name="value" is too long.  Optionally specify a
    /// `formattingMode` used to control the formatting of the `value`.  If
    /// `value` is of type `bsl::string` or convertible to
    /// `bsl::string_view`, the presence of invalid input stops the transfer
    /// of data to the output specified on construction (see {Valid
    /// Strings}).  If `value` is of type `bsl::string` or convertible to
    /// `bsl::string_view`, any special characters in `value` are escaped
    /// (see {Special Characters}).  If `value` is of type `char`, it is
    /// cast to a signed byte value with a range `[ -128 .. 127 ]`.  The
    /// behavior is undefined unless the last manipulator was `openElement`
    /// or `addAttribute`.
    template <class TYPE>
    void addAttribute(const bsl::string_view& name,
                      const TYPE&             value,
                      int                     formattingMode = 0);

    /// Insert one or two newline characters into the output stream such
    /// that a blank line results.  If the last output was a newline, then
    /// only one newline is added, otherwise two newlines are added.  If
    /// following a call to `openElement`, or `addAttribute`, add a closing
    /// `>` to the opened tag.
    void addBlankLine();

    /// Write the specified `comment` into the stream.  The optionally
    /// specified `forceNewline`, if true, forces to start a new line solely
    /// for the comment if it's not on a new line already.  Otherwise,
    /// comments continue on current line.  If an element-opening tag is not
    /// completed with a `>`, `addComment` will add `>`.
    ///
    /// @DEPRECATED: Use `addValidComment` instead.
    void addComment(const bsl::string_view& comment, bool forceNewline = true);

    /// Add the specified `value` as the data content, where `value` can be
    /// of the following types: `char`, `short`, `int`,
    /// `bsls::Types::Int64`, `float`, `double`, `bsl::string`,
    /// `bdlt::Datetime`, `bdlt::Date`, and `bdlt::Time`.  Perform no
    /// line-wrapping or indentation as if the whitespace constraint were
    /// always `BAEXML_PRESERVE_WHITESPACE` in `openElement`, with the only
    /// exception that an initial newline and an initial indent is added
    /// when `openElement` specifies `BAEXML_NEWLINE_INDENT` option.  If
    /// `value` is of type `bsl::string` or convertible to
    /// `bsl::string_view`, the presence of invalid input stops the transfer
    /// of data to the output specified on construction (see {Valid
    /// Strings}).  If `value` is of type `bsl::string` or convertible to
    /// `bsl::string_view`, characters in `value` are escaped (see {Special
    /// Characters}).  If `value` is of type `char`, it is cast to a signed
    /// byte value with a range of `[ -128 .. 127 ]`.  Optionally specify
    /// the `formattingMode` to specify the format used to encode `value`.
    /// The behavior is undefined if the call is made when there are no
    /// opened elements.
    template <class TYPE>
    void addData(const TYPE& value, int formattingMode = 0);

    /// Add element of the specified `name` and the specified `value` as the
    /// data content.  This has the same effect as calling the following
    /// sequence: `openElement(name); addData(value), closeElement(name);`.
    /// Optionally specify the `formattingMode`.
    template <class TYPE>
    void addElementAndData(const bsl::string_view& name,
                           const TYPE&             value,
                           int                     formattingMode = 0);

    /// Add XML header with optionally specified `encoding`.  Version is
    /// always "1.0".  The behavior is undefined unless `addHeader` is the
    /// first manipulator (with the exception of `rawOutputStream`) after
    /// construction or `reset`.
    void addHeader(const bsl::string_view& encoding = "UTF-8");

    /// Add the specified `value` as the data content, where `value` can be
    /// of the following types: `char`, `short`, `int`,
    /// `bsls::Types::Int64`, `float`, `double`, `bsl::string`,
    /// `bdlt::Datetime`, `bdlt::Date`, and `bdlt::Time`.  Prefix the
    /// `value` with a space(`0x20`) unless the data being added is the
    /// first data on a line.  When adding the data makes the line too long,
    /// perform line-wrapping and indentation as determined by the
    /// whitespace constraint used when the current element is opened with
    /// `openElement`.  If `value` is of type `bsl::string` or convertible
    /// to `bsl::string_view`, the presence of invalid input stops the
    /// transfer of data to the output specified on construction (see {Valid
    /// Strings}).  If `value` is of type `bsl::string` or convertible to
    /// `bsl::string_view`, any special characters in `value` are escaped
    /// (see {Special Characters}).  If `value` is of type `char`, it is
    /// cast to a signed byte value with a range of `[ -128 .. 127 ]`.
    /// Optionally specify the `formattingMode` to specify the format used
    /// to encode `value`.  The behavior is undefined if the call is made
    /// when there are no opened elements.
    template <class TYPE>
    void addListData(const TYPE& value, int formattingMode = 0);

    /// Insert a literal newline into the XML output.  If following a call
    /// to `openElement`, or `addAttribute`, add a closing `>` to the opened
    /// tag.
    void addNewline();

    /// Write the specified `comment` into the stream.  Optionally specify
    /// `forceNewline` that specifies if a new line should be added before
    /// the comment if it is not already on a new line.  If `forceNewline`
    /// is not specified then a new line is inserted for comments not
    /// already on a new line.  Also optionally specify an
    /// `omitEnclosingWhitespace` that specifies if a space character should
    /// be omitted before and after `comment`.  If `omitEnclosingWhitespace`
    /// is not specified then a space character is inserted before and after
    /// `comment`.  Return 0 on success, and non-zero value otherwise.  Note
    /// that a non-zero return value is returned if either `comment`
    /// contains `--` or if `omitEnclosingWhitespace` is `true` and
    /// `comment` ends with `-`.  Also note that if an element-opening tag
    /// is not completed with a `>`, `addValidComment` will add `>`.
    int addValidComment(
                      const bsl::string_view& comment,
                      bool                    forceNewline            = true,
                      bool                    omitEnclosingWhitespace = false);

    /// Decrement the indent level and add the closing tag for the element
    /// of the specified `name`.  If the element does not have content,
    /// write `/>` and a newline into stream.  Otherwise, write `</name>`
    /// and a newline.  If this `</name>` does not share the same line with
    /// data, or it follows another element's closing tag, indent properly
    /// before writing `</name>` and the newline.  If `name` is root
    /// element, flush the output stream.  The behavior is undefined if
    /// `name` is not the most recently opened element that's yet to be
    /// closed.
    void closeElement(const bsl::string_view& name);

    /// Insert the closing `>` if there is an incomplete tag, and flush the
    /// output stream.
    void flush();

    /// Open an element of the specified `name` at current indent level with
    /// the optionally specified whitespace constraint `whitespaceMode` for
    /// its textual data and increment indent level.  `whitespaceMode`
    /// constrains how textual data is written with `addListData` for the
    /// current element, but not its nested elements.  The behavior is
    /// undefined if `openElement` is called after the root element is
    /// closed and there is no subsequent call to `reset`.
    void openElement(
               const bsl::string_view& name,
               WhitespaceType          whitespaceMode = e_PRESERVE_WHITESPACE);

    /// Return a reference to the underlining output stream.  This method is
    /// provided in order to enable user to temporarily jump out of the
    /// formatter and write user's own free-lance content directly to the
    /// stream.
    bsl::ostream& rawOutputStream();

    /// Reset the formatter such that it can be used to format a new XML
    /// document as if the formatter were just constructed
    void reset();

    // ACCESSORS

    /// Return the encoder options being used.
    const EncoderOptions& encoderOptions() const;

    /// Return the current level of indentation.
    int indentLevel() const;

    /// Return the current column position at a line where next output
    /// starts.  This is unreliable if called after free-lance information
    /// is written onto the stream returned by `rawOutputStream`
    int outputColumn() const;

    /// Return the number of spaces per indentation level.
    int spacesPerLevel() const;

    /// Return 0 if no errors have been detected since construction or
    /// since the last call to `reset`, otherwise return a negative value.
    int status() const;

    /// Return the line width where line-wrapping takes place.
    int wrapColumn() const;
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                        // ----------------------------
                        // class Formatter_StreamHolder
                        // ----------------------------

// CREATORS
inline
Formatter_StreamHolder::Formatter_StreamHolder(bsl::streambuf *streamBuffer)
: d_ownStream(bsl::in_place_t(), streamBuffer)
, d_stream_p(&d_ownStream.value())
{
}

inline
Formatter_StreamHolder::Formatter_StreamHolder(bsl::ostream *stream)
: d_ownStream()
, d_stream_p(stream)
{
}

// MANIPULATORS
inline
bsl::ostream *Formatter_StreamHolder::stream()
{
    return d_stream_p;
}

// ACCESSORS
inline
const bsl::ostream *Formatter_StreamHolder::stream() const
{
    return d_stream_p;
}

                           // ---------------------
                           // class Formatter_State
                           // ---------------------

// PRIVATE MANIPULATORS
inline
Formatter_CompactImplState& Formatter_State::compact()
{
    BSLS_ASSERT(Mode::e_COMPACT == d_mode);

    return d_compact.object();
}

inline
Formatter_PrettyImplState& Formatter_State::pretty()
{
    BSLS_ASSERT(Mode::e_PRETTY == d_mode);

    return d_pretty.object();
}

// PRIVATE ACCESSORS
inline
const Formatter_CompactImplState& Formatter_State::compact() const
{
    BSLS_ASSERT(Mode::e_COMPACT == d_mode);

    return d_compact.object();
}

inline
Formatter_Mode::Enum Formatter_State::mode() const
{
    return d_mode;
}

inline
const Formatter_PrettyImplState& Formatter_State::pretty() const
{
    BSLS_ASSERT(Mode::e_PRETTY == d_mode);

    return d_pretty.object();
}

// CREATORS
inline
Formatter_State::Formatter_State()
: d_mode(Mode::e_COMPACT)
, d_allocator()
{
    new (d_compact.buffer()) Compact();
}

inline
Formatter_State::Formatter_State(const allocator_type& allocator)
: d_mode(Mode::e_COMPACT)
, d_allocator(allocator)
{
    new (d_compact.buffer()) Compact();
}

// ACCESSORS
inline
Formatter_State::allocator_type Formatter_State::get_allocator() const
{
    return d_allocator;
}

                              // ---------------
                              // class Formatter
                              // ---------------

// MANIPULATORS
template <class TYPE>
void Formatter::addAttribute(const bsl::string_view& name,
                             const TYPE&             value,
                             int                     formattingMode)
{
    switch (d_state.mode()) {
      case Mode::e_COMPACT: {
        CompactUtil::addAttribute(*d_streamHolder.stream(),
                                  &d_state.compact(),
                                  name,
                                  value,
                                  formattingMode,
                                  d_encoderOptions);
      } break;
      case Mode::e_PRETTY: {
        PrettyUtil::addAttribute(*d_streamHolder.stream(),
                                 &d_state.pretty(),
                                 name,
                                 value,
                                 formattingMode,
                                 d_encoderOptions);
      } break;
    }
}

inline
void Formatter::addBlankLine()
{
    switch (d_state.mode()) {
      case Mode::e_COMPACT: {
        CompactUtil::addBlankLine(*d_streamHolder.stream(),
                                  &d_state.compact());
      } break;
      case Mode::e_PRETTY: {
        PrettyUtil::addBlankLine(*d_streamHolder.stream(), &d_state.pretty());
      } break;
    }
}

template <class TYPE>
void Formatter::addData(const TYPE& value, int formattingMode)
{
    switch (d_state.mode()) {
      case Mode::e_COMPACT: {
        CompactUtil::addData(*d_streamHolder.stream(),
                             &d_state.compact(),
                             value,
                             formattingMode,
                             d_encoderOptions);
      } break;
      case Mode::e_PRETTY: {
        PrettyUtil::addData(*d_streamHolder.stream(),
                            &d_state.pretty(),
                            value,
                            formattingMode,
                            d_encoderOptions);
      } break;
    }
}

template <class TYPE>
void Formatter::addElementAndData(const bsl::string_view& name,
                                  const TYPE&             value,
                                  int                     formattingMode)
{
    switch (d_state.mode()) {
      case Mode::e_COMPACT: {
        CompactUtil::addElementAndData(*d_streamHolder.stream(),
                                       &d_state.compact(),
                                       name,
                                       value,
                                       formattingMode,
                                       d_encoderOptions);
      } break;
      case Mode::e_PRETTY: {
        PrettyUtil::addElementAndData(*d_streamHolder.stream(),
                                      &d_state.pretty(),
                                      name,
                                      value,
                                      formattingMode,
                                      d_encoderOptions);
      } break;
    }
}

template <class TYPE>
void Formatter::addListData(const TYPE& value, int formattingMode)
{
    switch (d_state.mode()) {
      case Mode::e_COMPACT: {
        CompactUtil::addListData(*d_streamHolder.stream(),
                                 &d_state.compact(),
                                 value,
                                 formattingMode,
                                 d_encoderOptions);
      } break;
      case Mode::e_PRETTY: {
        PrettyUtil::addListData(*d_streamHolder.stream(),
                                &d_state.pretty(),
                                value,
                                formattingMode,
                                d_encoderOptions);
      } break;
    }
}

inline
void Formatter::addNewline()
{
    switch (d_state.mode()) {
      case Mode::e_COMPACT: {
        CompactUtil::addNewline(*d_streamHolder.stream(), &d_state.compact());
      } break;
      case Mode::e_PRETTY: {
        PrettyUtil::addNewline(*d_streamHolder.stream(), &d_state.pretty());
      } break;
    }
}

inline
void Formatter::flush()
{
    switch (d_state.mode()) {
      case Mode::e_COMPACT: {
        CompactUtil::flush(*d_streamHolder.stream(), &d_state.compact());
      } break;
      case Mode::e_PRETTY: {
        PrettyUtil::flush(*d_streamHolder.stream(), &d_state.pretty());
      } break;
    }
}

inline
bsl::ostream& Formatter::rawOutputStream()
{
    switch (d_state.mode()) {
      case Mode::e_COMPACT: {
        CompactUtil::flush(*d_streamHolder.stream(), &d_state.compact());
      } break;
      case Mode::e_PRETTY: {
        PrettyUtil::flush(*d_streamHolder.stream(), &d_state.pretty());
      } break;
    }

    return *d_streamHolder.stream();
}

// ACCESSORS
inline
const EncoderOptions& Formatter::encoderOptions() const
{
    return d_encoderOptions;
}

inline
int Formatter::indentLevel() const
{
    int result = 0;

    switch (d_state.mode()) {
      case Mode::e_COMPACT: {
        result = d_state.compact().indentLevel();
      } break;
      case Mode::e_PRETTY: {
        result = d_state.pretty().indentLevel();
      } break;
    }

    return result;
}

inline
int Formatter::outputColumn() const
{
    int result = 0;

    switch (d_state.mode()) {
      case Mode::e_COMPACT: {
        result = d_state.compact().column();
      } break;
      case Mode::e_PRETTY: {
        result = d_state.pretty().column();
      } break;
    }

    return result;
}

inline
int Formatter::spacesPerLevel() const
{
    int result = 0;

    switch (d_state.mode()) {
      case Mode::e_COMPACT: {
        result = d_state.compact().spacesPerLevel();
      } break;
      case Mode::e_PRETTY: {
        result = d_state.pretty().spacesPerLevel();
      } break;
    }

    return result;
}

inline
int Formatter::status() const
{
    return d_streamHolder.stream()->good() ? 0 : -1;
}

inline
int Formatter::wrapColumn() const
{
    int result = 0;

    switch (d_state.mode()) {
      case Mode::e_COMPACT: {
        result = State::k_COMPACT_MODE_WRAP_COLUMN;
      } break;
      case Mode::e_PRETTY: {
        result = d_state.pretty().wrapColumn();
      } break;
    }

    return result;
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
