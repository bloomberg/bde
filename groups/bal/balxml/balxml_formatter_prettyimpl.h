// balxml_formatter_prettyimpl.h                                      -*-C++-*-
#ifndef INCLUDED_BALXML_FORMATTER_PRETTYIMPL
#define INCLUDED_BALXML_FORMATTER_PRETTYIMPL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide pretty-printing implementation for 'balxml_formatter'.
//
//@CLASSES:
//  balxml::Formatter_PrettyImplState: state of formatter state machine
//  balxml::Formatter_PrettyImplStateId: labels for formatter state
//  balxml::Formatter_PrettyImplUtil: actions of formatter state machine
//
//@DESCRIPTION: This private, subordinate component to 'balxml_formatter'
// provides an in-core value semantic attribute class,
// 'balxml::Formatter_PrettyImplState', and a utility 'struct',
// 'balxml::Formatter_PrettyImplUtil', that implements XML pretty-printing
// operations using the state value type.  These two classes work in
// conjunction to implement a state machine for pretty-printing an XML document
// given a sequence of tokens to emit.  The class
// 'balxml::Formatter_PrettyImplStateId' enumerates the set of labels for
// distinct states of 'balxml::Formatter_PrettyImplState', upon which most
// control-flow decisions of 'balxml::Formatter_PrettyImplUtil' are based.

#include <balscm_version.h>

#include <balxml_encoderoptions.h>
#include <balxml_formatterwhitespacetype.h>
#include <balxml_typesprintutil.h>

#include <bdlma_localsequentialallocator.h>

#include <bdlsb_memoutstreambuf.h>

#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsl_ios.h>
#include <bsl_ostream.h>
#include <bsl_string_view.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace balxml {

                     // ==================================
                     // struct Formatter_PrettyImplStateId
                     // ==================================

struct Formatter_PrettyImplStateId {
    // This 'struct' provides a namespace for enumerating the set of labels for
    // distinct states of 'Formatter_PrettyImplState'.

    // TYPES
    enum Enum {
        e_AT_START,
            // This state indicates that the current write position of the
            // formatter is at the start of the document.  The formatter is
            // only allowed to add an XML header when in this state.

        e_AFTER_START_NO_TAG,
            // This state indicates that the current write position of the
            // formatter is after some tokens have been emitted, such as an XML
            // header and/or some comments, but before any XML tags have been
            // emitted.  The formatter is not allowed to emit an XML header
            // when in this state.

        e_IN_TAG,
            // This state indicates that the current write position of the
            // formatter is immediately after the name of an opening tag, or
            // otherwise immediately after the value of an attribute of an
            // opening tag.  In this state, most token printing operations,
            // other than adding attributes, need to emit a ">" character to
            // close the currently open tag before emitting their content.  For
            // example:
            //..
            //  1| <someTag
            //   `---------^
            //
            //   * Note that there is no '>' character yet
            //..
            // or:
            //..
            //  1| <someTag attr="value" otherAttr="42"
            //   `-------------------------------------^
            //..

        e_FIRST_DATA_BETWEEN_TAGS,
            // This state indicates that the current write position of the
            // formatter is after a complete opening tag, before any data for
            // the tag, and that there are tokens already emitted on the
            // current line (i.e., that the current write position is not at
            // the start of the line).  In this state, whether or not token
            // printing operations need to emit a new line and indentation
            // generally depends on the whitespace mode set for the
            // currently-open tag.  Note that comments are not considered data.
            // For example:
            //..
            //  1| <someTag>
            //   `----------^
            //..
            // or:
            //..
            //  1| <someTag> <!-- comment -->
            //   `---------------------------^
            //..
            // or:
            //..
            //  1| <someTag attr="value">
            //  2|   <!-- comment -->
            //   `-------------------^
            //..

        e_FIRST_DATA_AT_LINE_BETWEEN_TAGS,
            // This state indicates that the current write position of the
            // formatter is at column 0 (i.e., that it is at the start of a new
            // line) and either 1) after a closing tag, or 2) after a complete
            // opening tag and optional data.  In this state, most token
            // printing operations need to emit indentation before their
            // content.  For example:
            //..
            //  1| <someTag>
            //  2|
            //   `^
            //..
            // or:
            //..
            //  1| <someTag>
            //  2| </someTag>
            //  3|
            //   `^
            //
            //   * Note that the 'closeElement' operation, which is used to
            //     print closing tags, e.g., "</example>", *always* writes a
            //     newline character after the closing tag.
            //..
            // or:
            //..
            //  1| <someTag>
            //  2|   some list data
            //  3|   some more list data
            //  4|
            //   `^
            //..

        e_TRAILING_DATA_BETWEEN_TAGS,
            // This state indicates that the current write position of the
            // formatter is after one or more data tokens for the
            // currently-open tag, and that there are tokens already emitted on
            // the current line (i.e., that the current write position is not
            // at the start of a new line).  In this state, data printing
            // operations must put delimiting whitespace before their data.
            // What whitespace they emit may depend on the whitespace mode of
            // the currently-open tag.  For example:
            //..
            // 1| <someTag> someData
            //  `-------------------^
            //..
            // or
            //..
            // 1| <someTag>
            // 2|   some list data
            // 3|   some more list data
            //  `----------------------^
            //..

        e_AT_END
            // This state indicates that the current write position of the
            // formatter is immediately after the top-level closing tag of the
            // document.
    };
};

                      // ===============================
                      // class Formatter_PrettyImplState
                      // ===============================

class Formatter_PrettyImplState {
    // This class provides an in-core, value-semantic attribute type that
    // maintains all of the state information needed to pretty-print an XML
    // document using the operations provided by 'Formatter_PrettyImplUtil'.

  public:
    // TYPES
    typedef bsl::allocator<char>        allocator_type;
    typedef Formatter_PrettyImplStateId Id;
    typedef FormatterWhitespaceType     WhitespaceType;

  private:
    // DATA
    Id::Enum                          d_id;
        // the canonical "state" in the state machine, upon which most
        // control-flow decisions are based when printing

    int                               d_indentLevel;
        // number of indentations to perform when printing an element on a new
        // line

    int                               d_spacesPerLevel;
        // number of spaces to print per level of indentation

    int                               d_column;
        // the current column number

    int                               d_wrapColumn;
        // the column number at which an element will be printed on the next
        // line and optionally indented depending on the requested whitespace
        // mode

    bsl::vector<WhitespaceType::Enum> d_elementNesting;
        // a stack of names of currently nested elements with the whitespace
        // handling constraint for each element in the stack

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Formatter_PrettyImplState,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    Formatter_PrettyImplState();
    explicit Formatter_PrettyImplState(const allocator_type& allocator);
        // Create a 'Formatter_PrettyImplState' having an 'id' attribute of
        // 'Id::e_AT_START', 'indentLevel', 'spacesPerLevel', 'column', and
        // 'wrapColumn' attributes of 0, and an empty 'elementNesting'
        // attribute.  Optionally specify an 'allocator' (e.g., the address of
        // a 'bslma::Allocator' object) to supply memory; otherwise, the
        // default allocator is used.

    Formatter_PrettyImplState(
                           int                   indentLevel,
                           int                   spacesPerLevel,
                           int                   wrapColumn,
                           const allocator_type& allocator = allocator_type());
        // Create a 'Formatter_PrettyImplState' having an 'id' attribute of
        // 'Id::e_AT_START', the specified 'indentLevel', the specified
        // 'spacesPerLevel', and the specified 'wrapColumn', a 'column'
        // attribute of 0, and an empty 'elementNesting' attribute.  Optionally
        // specify an 'allocator' (e.g., the address of a 'bslma::Allocator'
        // object) to supply memory; otherwise, the default allocator is used.

    Formatter_PrettyImplState(
        Id::Enum                                 id,
        int                                      indentLevel,
        int                                      spacesPerLevel,
        int                                      column,
        int                                      wrapColumn,
        const bsl::vector<WhitespaceType::Enum>& elementNesting,
        const allocator_type&                    allocator = allocator_type());
        // Create a 'Formatter_PrettyImplState' having the specified 'id',
        // 'indentLevel', 'spacesPerLevel', 'column', 'wrapColumn', and
        // 'elementNesting' attributes.  Optionally specify an 'allocator'
        // (e.g., the address of a 'bslma::Allocator' object) to supply memory;
        // otherwise, the default allocator is used.

    Formatter_PrettyImplState(
                const Formatter_PrettyImplState& original,
                const allocator_type&            allocator = allocator_type());
        // Create a 'Formatter_PrettyImplState' object having the same value as
        // the specified 'original' object.  Optionally specify an 'allocator'
        // (e.g., the address of a 'bslma::Allocator' object) to supply memory;
        // otherwise, the default allocator is used.

    // MANIPULATORS
    int& column();
        // Return a reference providing modifiable access to the 'column'
        // attribute of this object.

    bsl::vector<WhitespaceType::Enum>& elementNesting();
        // Return a reference providing modifiable access to the
        // 'elementNesting' attribute of this object.

    Id::Enum& id();
        // Return a reference providing modifiable access to the 'id' attribute
        // of this object.

    int& indentLevel();
        // Return a reference providing modifiable access to the 'indentLevel'
        // attribute of this object.

    int& spacesPerLevel();
        // Return a reference providing modifiable access to the
        // 'spacesPerLevel' attribute of this object.

    int& wrapColumn();
        // Return a reference providing modifiable access to the 'wrapColumn'
        // attribute of this object.

    // ACCESSORS
    const int& column() const;
        // Return a reference providing non-modifiable access to the 'column'
        // attribute of this object.

    const bsl::vector<WhitespaceType::Enum>& elementNesting() const ;
        // Return a reference providing non-modifiable access to the
        // 'elementNesting' attribute of this object.

    allocator_type get_allocator() const;
        // Return the allocator associated with this object.

    const Id::Enum& id() const;
        // Return a reference providing non-modifiable access to the 'id'
        // attribute of this object.

    const int& indentLevel() const;
        // Return a reference providing non-modifiable access to the
        // 'indentLevel' attribute of this object.

    const int& spacesPerLevel() const;
        // Return a reference providing non-modifiable access to the
        // 'spacesPerLevel' attribute of this object.

    const int& wrapColumn() const;
        // Return a reference providing non-modifiable access to the
        // 'wrapColumn' attribute of this object.
};

                       // ==============================
                       // class Formatter_PrettyImplUtil
                       // ==============================

struct Formatter_PrettyImplUtil {
    // This utility 'struct' provides a namespace for a suite of operations
    // used to pretty-print XML documents given a sequence of tokens to emit.
    // Together with 'Formatter_PrettyImplState', this 'struct' provides an
    // implementation of a state machine for such pretty-printing.

    // TYPES
    typedef Formatter_PrettyImplState   State;
    typedef Formatter_PrettyImplStateId StateId;
    typedef FormatterWhitespaceType     WhitespaceType;

  private:
    // PRIVATE TYPES
    enum {
        k_VALUE_STRING_BUFFER_SIZE = 256,

        k_INFINITE_WRAP_COLUMN = 0
    };

    typedef bdlma::LocalSequentialAllocator<k_VALUE_STRING_BUFFER_SIZE>
        BufferedAllocator;

    // PRIVATE CLASS METHODS
    static void addAttributeImpl(bsl::ostream&            stream,
                                 State                   *state,
                                 const bsl::string_view&  name,
                                 const bsl::string_view&  value);
        // Add an attribute of the specified 'name' and 'value' to the
        // currently open element in the specified 'stream', with formatting
        // depending on the specified 'state', and update the 'state'
        // accordingly.  Precede this name="value" pair with a single space.
        // Wrap line (write the attribute on next line with proper
        // indentation), if the length of name="value" is too long.  'value' is
        // truncated at any invalid UTF-8 byte-sequence or any control
        // character.  The list of invalid control characters includes
        // characters in the range '[0x00, 0x20)' and '0x7F' (DEL) but does not
        // include '0x9', '0xA', and '0x0D'.  The five special characters:
        // apostrophe, double quote, ampersand, less than, and greater than are
        // escaped in the output XML.  The behavior is undefined unless the
        // last manipulator was 'openElement' or 'addAttribute'.

    static void addCommentImpl(bsl::ostream&            stream,
                               State                   *state,
                               const bsl::string_view&  comment,
                               const bsl::string_view&  openMarker,
                               const bsl::string_view&  closeMarker);
        // Write the specified 'openMarker', 'comment', and then 'closeMarker'
        // into the specified 'stream', with formatting depending on the
        // specified 'state', and update the 'state' accordingly.  If an
        // element-opening tag is not completed with a '>', 'addCommentImpl'
        // will add '>'.

    static void addCommentOnNewLineImpl(bsl::ostream&            stream,
                                        State                   *state,
                                        const bsl::string_view&  comment,
                                        const bsl::string_view&  openMarker,
                                        const bsl::string_view&  closeMarker);
        // Write the specified 'openMarker', 'comment', and then 'closeMarker'
        // into the specified 'stream' on their own line, with formatting
        // depending on the specified 'state', and update the 'state'
        // accordingly.  If an element-opening tag is not completed with a '>',
        // 'addCommentImpl' will add '>'.

    static void addDataImpl(bsl::ostream&            stream,
                            State                   *state,
                            const bsl::string_view&  value);
    static void addListDataImpl(bsl::ostream&            stream,
                                State                   *state,
                                const bsl::string_view&  value);
        // Add the specified 'value' as the data content to the specified
        // 'stream', with formatting depending on the specified 'state', and
        // update 'state' accordingly.  'addListData' prefixes the 'value' with
        // a space('0x20') unless the data being added is the first data on a
        // line.  In the case of 'addData', perform no line-wrapping or
        // indentation as if the whitespace constraint were always
        // 'BAEXML_PRESERVE_WHITESPACE' in 'openElement', with the only
        // exception that an initial newline and an initial indent is added
        // when 'openElement' specifies 'BAEXML_NEWLINE_INDENT' option.  In the
        // case of 'addListData', when adding the data makes the line too long,
        // perform line-wrapping and indentation as determined by the
        // whitespace constraint used when the current element is opened with
        // 'openElement'.  'value' is truncated at any invalid UTF-8
        // byte-sequence or any control character.  The list of invalid control
        // characters includes characters in the range '[0x00, 0x20)' and
        // '0x7F' (DEL) but does not include '0x9', '0xA', and '0x0D'.  The
        // five special characters: apostrophe, double quote, ampersand, less
        // than, and greater than are escaped in the output XML.  The behavior
        // is undefined if the call is made when there are no opened elements.

  public:
    // CLASS METHODS
    template <class VALUE_TYPE>
    static bsl::ostream& addAttribute(
                   bsl::ostream&            stream,
                   State                   *state,
                   const bsl::string_view&  name,
                   const VALUE_TYPE&        value,
                   int                      formattingMode = 0,
                   const EncoderOptions&    encoderOptions = EncoderOptions());
        // Add an attribute of the specified 'name' and specified 'value' to
        // the currently open element in the specified 'stream', with
        // formatting depending on the specified 'state', and update the
        // 'state' accordingly.  Return the 'stream'.  'value' can be of the
        // following types: 'char', 'short', 'int', 'bsls::Types::Int64',
        // 'float', 'double', 'bsl::string', 'bdlt::Datetime', 'bdlt::Date',
        // and 'bdlt::Time'.  Precede this name="value" pair with a single
        // space.  Wrap line (write the attribute on next line with proper
        // indentation), if the length of name="value" is too long.  Optionally
        // specify 'formattingMode' and 'encoderOptions' to control the
        // formatting of 'value'.  If 'value' is of type 'bsl::string', it is
        // truncated at any invalid UTF-8 byte-sequence or any control
        // character.  The list of invalid control characters includes
        // characters in the range '[0x00, 0x20)' and '0x7F' (DEL) but does not
        // include '0x9', '0xA', and '0x0D'.  The five special characters:
        // apostrophe, double quote, ampersand, less than, and greater than are
        // escaped in the output XML.  If 'value' is of type 'char', it is cast
        // to a signed byte value with a range '[ -128 ..  127 ]'.  The
        // behavior is undefined unless the last manipulator was 'openElement'
        // or 'addAttribute'.

    static bsl::ostream& addBlankLine(bsl::ostream& stream, State *state);
        // Insert one or two newline characters into the specified 'stream'
        // stream such that a blank line results, depending on the specified
        // 'state', and update the 'state' accordingly.  Return the 'stream'.
        // If the last output was a newline, then only one newline is added,
        // otherwise two newlines are added.  If following a call to
        // 'openElement', or 'addAttribute', add a closing '>' to the opened
        // tag.

    static bsl::ostream& addComment(
                                 bsl::ostream&            stream,
                                 State                   *state,
                                 const bsl::string_view&  comment,
                                 bool                     forceNewline = true);
        // !DEPRECATED!: Use 'addValidComment' instead.
        //
        // Write the specified 'comment' into the specified 'stream', with
        // formatting depending on the specified 'state', and update the
        // 'state' accordingly.  Return the 'stream'.  The optionally specified
        // 'forceNewline', if true, forces to start a new line solely for the
        // comment if it's not on a new line already.  Otherwise, comments
        // continue on current line.  If an element-opening tag is not
        // completed with a '>', 'addComment' will add '>'.

    template <class VALUE_TYPE>
    static bsl::ostream& addData(
                     bsl::ostream&          stream,
                     State                 *state,
                     const VALUE_TYPE&      value,
                     int                    formattingMode = 0,
                     const EncoderOptions&  encoderOptions = EncoderOptions());
        // Add the specified 'value' as the data content to the specified
        // 'stream', with formatting depending on the specified 'state', and
        // update 'state' accordingly.  Return the 'stream'.  Return the
        // 'stream'.  'value' can be of the following types: 'char', 'short',
        // 'int', 'bsls::Types::Int64', 'float', 'double', 'bsl::string',
        // 'bdlt::Datetime', 'bdlt::Date', and 'bdlt::Time'.  Perform no
        // line-wrapping or indentation as if the whitespace constraint were
        // always 'BAEXML_PRESERVE_WHITESPACE' in 'openElement', with the only
        // exception that an initial newline and an initial indent is added
        // when 'openElement' specifies 'BAEXML_NEWLINE_INDENT' option.  If
        // 'value' is of type 'bsl::string', it is truncated at any invalid
        // UTF-8 byte-sequence or any control character.  The list of invalid
        // control characters includes characters in the range '[0x00, 0x20)'
        // and '0x7F' (DEL) but does not include '0x9', '0xA', and '0x0D'.  The
        // five special characters: apostrophe, double quote, ampersand, less
        // than, and greater than are escaped in the output XML.  If 'value' is
        // of type 'char', it is cast to a signed byte value with a range of '[
        // -128 ..  127 ]'.  Optionally specify the 'formattingMode' and
        // 'encoderOptions' to specify the format used to encode 'value'.  The
        // behavior is undefined if the call is made when there are no opened
        // elements.

    template <class TYPE>
    static bsl::ostream& addElementAndData(
                   bsl::ostream&            stream,
                   State                   *state,
                   const bsl::string_view&  name,
                   const TYPE&              value,
                   int                      formattingMode = 0,
                   const EncoderOptions&    encoderOptions = EncoderOptions());
        // Add element of the specified 'name' and the specified 'value' as the
        // data content to the specified 'stream', with formatting depending on
        // the specified 'state' and the optionally specified 'encoderOptions',
        // and update 'state' accordingly.  Return the 'stream'.  This has the
        // same effect as calling the following sequence: 'openElement(name);
        // addData(value), closeElement(name);'.  Optionally specify the
        // 'formattingMode'.

    static bsl::ostream& addHeader(bsl::ostream&            stream,
                                   State                   *state,
                                   const bsl::string_view&  encoding);
        // Add XML header with optionally specified 'encoding' to the specified
        // 'stream', with formatting depending on the specified 'state', and
        // update 'state' accordingly.  Return the 'stream'.  Version is always
        // "1.0".  The behavior is undefined unless 'addHeader' is the first
        // manipulator (with the exception of 'rawOutputStream') after
        // construction or 'reset'.

    template <class VALUE_TYPE>
    static bsl::ostream& addListData(
                     bsl::ostream&          stream,
                     State                 *state,
                     const VALUE_TYPE&      value,
                     int                    formattingMode = 0,
                     const EncoderOptions&  encoderOptions = EncoderOptions());
        // Add the specified 'value' as the data content to the specified
        // 'stream', with formatting depending on the specified 'state', and
        // update 'state' accordingly.  Return the 'stream'.  'value' can be of
        // the following types: 'char', 'short', 'int', 'bsls::Types::Int64',
        // 'float', 'double', 'bsl::string', 'bdlt::Datetime', 'bdlt::Date',
        // and 'bdlt::Time'.  Prefix the 'value' with a space('0x20') unless
        // the data being added is the first data on a line.  When adding the
        // data makes the line too long, perform line-wrapping and indentation
        // as determined by the whitespace constraint used when the current
        // element is opened with 'openElement'.  If 'value' is of type
        // 'bsl::string', it is truncated at any invalid UTF-8 byte-sequence or
        // any control character.  The list of invalid control characters
        // includes characters in the range '[0x00, 0x20)' and '0x7F' (DEL) but
        // does not include '0x9', '0xA', and '0x0D'.  The five special
        // characters: apostrophe, double quote, ampersand, less than, and
        // greater than are escaped in the output XML.  If 'value' is of type
        // 'char', it is cast to a signed byte value with a range of '[ -128 ..
        // 127 ]'.  Optionally specify the 'formattingMode' and
        // 'encoderOptions' to specify the format used to encode 'value'.  The
        // behavior is undefined if the call is made when there are no opened
        // elements.

    static bsl::ostream& addNewline(bsl::ostream& stream, State *state);
        // Insert a literal newline into the XML output of the specified
        // 'stream', with formatting depending on the specified 'state', and
        // update 'state' accordingly.  Return the 'stream'.  If following a
        // call to 'openElement', or 'addAttribute', add a closing '>' to the
        // opened tag.

    static int addValidComment(
                     bsl::ostream&            stream,
                     State                   *state,
                     const bsl::string_view&  comment,
                     bool                     forceNewline            = true,
                     bool                     omitEnclosingWhitespace = false);
        // Write the specified 'comment' into the specified 'stream', with
        // formatting depending on the specified 'state', and update the
        // 'state' accordingly.  If the optionally specified 'forceNewline' is
        // 'true' then a new line is inserted for comments not already on a new
        // line.  Also optionally specify an 'omitEnclosingWhitespace' that
        // specifies if a space character should be omitted before and after
        // 'comment'.  If 'omitEnclosingWhitespace' is not specified then a
        // space character is inserted before and after 'comment'.  Return 0 on
        // success, and non-zero value otherwise.  Note that a non-zero return
        // value is returned if either 'comment' contains '--' or if
        // 'omitEnclosingWhitespace' is 'true' and 'comment' ends with '-'.
        // Also note that if an element-opening tag is not completed with a
        // '>', 'addValidComment' will add '>'.

    static bsl::ostream& closeElement(bsl::ostream&            stream,
                                      State                   *state,
                                      const bsl::string_view&  name);
        // Decrement the indent level and add the closing tag for the element
        // of the specified 'name' to the specified 'stream', with formatting
        // depending on the specified 'state', and update 'state' accordingly.
        // Return the 'stream'.  If the element does not have content, write
        // '/>' and a newline into stream.  Otherwise, write '</name>' and a
        // newline.  If this '</name>' does not share the same line with data,
        // or it follows another element's closing tag, indent properly before
        // writing '</name>' and the newline.  If 'name' is root element, flush
        // the output stream.  The behavior is undefined if 'name' is not the
        // most recently opened element that's yet to be closed.

    static bsl::ostream& flush(bsl::ostream& stream, State *state);
        // Insert the closing '>' if there is an incomplete tag, and flush the
        // specified output 'stream', with formatting depending on the
        // specified 'state', and update 'state' accordingly.  Return the
        // 'stream'.

    static bsl::ostream&
    openElement(bsl::ostream&            stream,
                State                   *state,
                const bsl::string_view&  name,
                WhitespaceType::Enum     whitespaceMode =
                    WhitespaceType::e_PRESERVE_WHITESPACE);
        // Open an element of the specified 'name' at current indent level with
        // the optionally specified whitespace constraint 'whitespaceMode' for
        // its textual data to the specified 'stream', with formatting
        // depending on the specified 'state', and update 'state' accordingly,
        // incrementing the indent level.  Return the 'stream'.
        // 'whitespaceMode' constrains how textual data is written with
        // 'addListData' for the current element, but not its nested elements.
        // The behavior is undefined if 'openElement' is called after the root
        // element is closed and there is no subsequent call to 'reset'.

    static void reset(State *state);
        // Reset the specified formatter 'state' such that it can be used to
        // format a new XML document as if the formatter were just constructed
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                      // -------------------------------
                      // class Formatter_PrettyImplState
                      // -------------------------------

// CREATORS
inline
Formatter_PrettyImplState::Formatter_PrettyImplState()
: d_id(Id::e_AT_START)
, d_indentLevel(0)
, d_spacesPerLevel(0)
, d_column(0)
, d_wrapColumn(0)
, d_elementNesting()
{
}

inline
Formatter_PrettyImplState::Formatter_PrettyImplState(
                                               const allocator_type& allocator)
: d_id(Id::e_AT_START)
, d_indentLevel(0)
, d_spacesPerLevel(0)
, d_column(0)
, d_wrapColumn(0)
, d_elementNesting(allocator)
{
}

inline
Formatter_PrettyImplState::Formatter_PrettyImplState(
                                    const Formatter_PrettyImplState& original,
                                    const allocator_type&            allocator)
: d_id(original.d_id)
, d_indentLevel(original.d_indentLevel)
, d_spacesPerLevel(original.d_spacesPerLevel)
, d_column(original.d_column)
, d_wrapColumn(original.d_wrapColumn)
, d_elementNesting(original.d_elementNesting, allocator)
{
}

inline
Formatter_PrettyImplState::Formatter_PrettyImplState(
                                          int                   indentLevel,
                                          int                   spacesPerLevel,
                                          int                   wrapColumn,
                                          const allocator_type& allocator)
: d_id(Id::e_AT_START)
, d_indentLevel(indentLevel)
, d_spacesPerLevel(spacesPerLevel)
, d_column()
, d_wrapColumn(wrapColumn)
, d_elementNesting(allocator)
{
}

inline
Formatter_PrettyImplState::Formatter_PrettyImplState(
                       Id::Enum                                 id,
                       int                                      indentLevel,
                       int                                      spacesPerLevel,
                       int                                      column,
                       int                                      wrapColumn,
                       const bsl::vector<WhitespaceType::Enum>& elementNesting,
                       const allocator_type&                    allocator)
: d_id(id)
, d_indentLevel(indentLevel)
, d_spacesPerLevel(spacesPerLevel)
, d_column(column)
, d_wrapColumn(wrapColumn)
, d_elementNesting(elementNesting, allocator)
{
}

// MANIPULATORS
inline
int& Formatter_PrettyImplState::column()
{
    return d_column;
}

inline
bsl::vector<FormatterWhitespaceType::Enum>&
Formatter_PrettyImplState::elementNesting()
{
    return d_elementNesting;
}

inline
Formatter_PrettyImplStateId::Enum& Formatter_PrettyImplState::id()
{
    return d_id;
}

inline
int& Formatter_PrettyImplState::indentLevel()
{
    return d_indentLevel;
}

inline
int& Formatter_PrettyImplState::spacesPerLevel()
{
    return d_spacesPerLevel;
}

inline
int& Formatter_PrettyImplState::wrapColumn()
{
    return d_wrapColumn;
}

// ACCESSORS
inline
const int& Formatter_PrettyImplState::column() const
{
    return d_column;
}

inline
const bsl::vector<FormatterWhitespaceType::Enum>&
Formatter_PrettyImplState::elementNesting() const
{
    return d_elementNesting;
}

inline
Formatter_PrettyImplState::allocator_type
Formatter_PrettyImplState::get_allocator() const
{
    return d_elementNesting.get_allocator();
}

inline
const Formatter_PrettyImplStateId::Enum& Formatter_PrettyImplState::id() const
{
    return d_id;
}

inline
const int& Formatter_PrettyImplState::indentLevel() const
{
    return d_indentLevel;
}

inline
const int& Formatter_PrettyImplState::spacesPerLevel() const
{
    return d_spacesPerLevel;
}

inline
const int& Formatter_PrettyImplState::wrapColumn() const
{
    return d_wrapColumn;
}

                       // ------------------------------
                       // class Formatter_PrettyImplUtil
                       // ------------------------------

// CLASS METHODS
template <class VALUE_TYPE>
bsl::ostream& Formatter_PrettyImplUtil::addAttribute(
                                       bsl::ostream&            stream,
                                       State                   *state,
                                       const bsl::string_view&  name,
                                       const VALUE_TYPE&        value,
                                       int                      formattingMode,
                                       const EncoderOptions&    encoderOptions)
{
    BufferedAllocator      allocator;
    bdlsb::MemOutStreamBuf sb(&allocator);
    bsl::ostream           ss(&sb);

    TypesPrintUtil::print(ss, value, formattingMode, &encoderOptions);
    if (!ss.good()) {
        stream.setstate(bsl::ios_base::failbit);
        return stream;                                                // RETURN
    }

    const bsl::string_view valueString(sb.data(), sb.length());

    addAttributeImpl(stream, state, name, valueString);

    return stream;
}


template <class VALUE_TYPE>
bsl::ostream& Formatter_PrettyImplUtil::addData(
                                         bsl::ostream&          stream,
                                         State                 *state,
                                         const VALUE_TYPE&      valueData,
                                         int                    formattingMode,
                                         const EncoderOptions&  encoderOptions)
{
    BufferedAllocator      allocator;
    bdlsb::MemOutStreamBuf sb(&allocator);
    bsl::ostream           ss(&sb);

    TypesPrintUtil::print(ss, valueData, formattingMode, &encoderOptions);
    if (!ss.good()) {
        stream.setstate(bsl::ios_base::failbit);
        return stream;                                                // RETURN
    }

    const bsl::string_view valueString(sb.data(), sb.length());
    addDataImpl(stream, state, valueString);
    return stream;
}

template <class TYPE>
bsl::ostream& Formatter_PrettyImplUtil::addElementAndData(
                                       bsl::ostream&            stream,
                                       State                   *state,
                                       const bsl::string_view&  name,
                                       const TYPE&              value,
                                       int                      formattingMode,
                                       const EncoderOptions&    encoderOptions)
{
    openElement(stream, state, name, WhitespaceType::e_PRESERVE_WHITESPACE);
    addData(stream, state, value, formattingMode, encoderOptions);
    closeElement(stream, state, name);
    return stream;
}

template <class VALUE_TYPE>
bsl::ostream& Formatter_PrettyImplUtil::addListData(
                                         bsl::ostream&          stream,
                                         State                 *state,
                                         const VALUE_TYPE&      value,
                                         int                    formattingMode,
                                         const EncoderOptions&  encoderOptions)
{
    BufferedAllocator      allocator;
    bdlsb::MemOutStreamBuf sb(&allocator);
    bsl::ostream           ss(&sb);

    TypesPrintUtil::print(ss, value, formattingMode, &encoderOptions);
    if (!ss.good()) {
        stream.setstate(bsl::ios_base::failbit);
        return stream;                                                // RETURN
    }

    const bsl::string_view valueString(sb.data(), sb.length());
    addListDataImpl(stream, state, valueString);
    return stream;
}

inline
void Formatter_PrettyImplUtil::reset(State *state)
{
    state->column() = 0;
    state->id()     = StateId::e_AT_START;
    state->indentLevel() -= static_cast<int>(state->elementNesting().size());
    state->elementNesting().clear();
}

}  // close package namespace
}  // close enterprise namespace

#endif // INCLUDED_BALXML_FORMATTER_PRETTYIMPL

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
