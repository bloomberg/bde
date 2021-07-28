// balxml_formatter_compactimpl.h                                     -*-C++-*-
#ifndef INCLUDED_BALXML_FORMATTER_COMPACTIMPL
#define INCLUDED_BALXML_FORMATTER_COMPACTIMPL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a minimal-whitespace implementation for 'balxml_formatter'.
//
//@CLASSES:
//  balxml::Formatter_CompactImplState: state of formatter state machine
//  balxml::Formatter_CompactImplStateId: labels for formatter state
//  balxml::Formatter_CompactImplUtil: actions of formatter state machine
//
//@DESCRIPTION: This private, subordinate component to 'balxml_formatter'
// provides an in-core value semantic attribute class,
// 'balxml::Formatter_CompactImplState', and a utility 'struct',
// 'balxml::Formatter_CompactImplUtil', that implements XML printing operations
// using the state value type.  These two classes work in conjunction to
// implement a state machine for printing an XML document having minimal
// whitespace, given a sequence of tokens to emit.  The class
// 'balxml::Formatter_CompactImplStateId' enumerates the set of labels for
// distinct states of 'balxml::Formatter_CompactImplState', upon which most
// control-flow decisions of 'balxml::Formatter_CompactImplUtil' are
// based.

#include <balscm_version.h>

#include <balxml_encoderoptions.h>
#include <balxml_formatterwhitespacetype.h>
#include <balxml_typesprintutil.h>

#include <bsl_ostream.h>
#include <bsl_string_view.h>

namespace BloombergLP {
namespace balxml {

                    // ===================================
                    // struct Formatter_CompactImplStateId
                    // ===================================

struct Formatter_CompactImplStateId {
    // This 'struct' provides a namespace for enumerating a set of labels for
    // distinct states of 'Formatter_CompactImplState'.

    // TYPES
    enum Enum {
        e_AT_START,
            // This state indicates that the current write position of the
            // formatter is at the start of the document.  The formatter is
            // only allowed to add an XML header when in this state.

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
            // formatter is either 1) after a closing tag, or 2) after a
            // complete opening tag and before any data for the tag.  In this
            // state, data printing operations do *not* need to emit delimiting
            // whitespace for their content.  Note that comments are not
            // considered data.  For example:
            //..
            //  1| <someTag>
            //   `----------^
            //..
            // or:
            //..
            //  1| <someTag><!-- comment -->
            //   `--------------------------^
            //..
            // or:
            //..
            //  1| <someTag></someTag>
            //   `--------------------^
            //..

        e_TRAILING_DATA_BETWEEN_TAGS
            // This state indicates that the current write position of the
            // formatter is after one or more data tokens for the
            // currently-open tag.  In this state, data printing operations
            // need to emit delimiting whitespace for their content.  For
            // example:
            //..
            //  1| <someTag>data
            //   `--------------^
            //..
            // or:
            //..
            //  1| <someTag>some list data
            //   `------------------------^
            //..
    };
};

                      // ================================
                      // class Formatter_CompactImplState
                      // ================================

class Formatter_CompactImplState {
    // This class provides an in-core, value-semantic attribute type that
    // maintains all of the state information needed to print an XML document
    // with minimal whitespace using the operations provided by
    // 'Formatter_CompactImplUtil'.

  public:
    // TYPES
    typedef Formatter_CompactImplStateId Id;

  private:
    // DATA
    Id::Enum d_id;
        // the canonical "state" in the state machine, upon which most
        // control-flow decisions are based when printing

    int      d_indentLevel;
        // the current open XML tag nesting depth

    int      d_initialIndentLevel;
        // the XML tag nesting depth to use at the start of the document, and
        // the value to which 'd_indentLevel' is set upon 'reset', which no
        // printing operations modify

    int      d_spacesPerLevel;
        // an option that the formatter does not use in compact mode, but
        // which is accessible to clients in any mode

    int      d_column;
        // an approximation of the current column number, which does not take
        // into account the number of characters used to print any data, and
        // which the formatter does not use in compact mode, but is accessible
        // to clients in any mode

  public:
    // CREATORS
    Formatter_CompactImplState();
        // Create a 'Formatter_CompactImplState' having an 'id' attribute of
        // 'Id::e_AT_START', and 'indentLevel', 'initialIndentLevel',
        // 'spacesPerLevel', and 'column' attributes of 0.

    Formatter_CompactImplState(int indentLevel, int spacesPerLevel);
        // Create a 'Formatter_CompactImplState' having an 'id' attribute of
        // 'Id::e_AT_START', the specified 'indentLevel' and 'spacesPerLevel',
        // as well as an 'initialIndentLevel' attribute equal to 'indentLevel',
        // and a 'column' attribute of 0.

    Formatter_CompactImplState(Id::Enum id,
                               int      indentLevel,
                               int      initialIndentLevel,
                               int      spacesPerLevel,
                               int      column);
        // Create a 'Formatter_CompactImplState' having the specified 'id',
        // 'indentLevel', 'initialIndentLevel', 'spacesPerLevel', and 'column'
        // attributes.

    // MANIPULATORS
    int& column();
        // Return a reference providing modifiable access to the 'column'
        // attribute of this object.

    Id::Enum& id();
        // Return a reference providing modifiable access to the 'id' attribute
        // of this object.

    int& indentLevel();
        // Return a reference providing modifiable access to the 'indentLevel'
        // attribute of this object.

    int& initialIndentLevel();
        // Return a reference providing modifiable access to the
        // 'initialIndentLevel' attribute of this object.

    int& spacesPerLevel();
        // Return a reference providing modifiable access to the
        // 'spacesPerLevel' attribute of this object.

    // ACCESSORS
    const int& column() const;
        // Return a reference providing non-modifiable access to the 'column'
        // attribute of this object.

    const Id::Enum& id() const;
        // Return a reference providing non-modifiable access to the 'id'
        // attribute of this object.

    const int& indentLevel() const;
        // Return a reference providing non-modifiable access to the
        // 'indentLevel' attribute of this object.

    const int& initialIndentLevel() const;
        // Return a reference providing non-modifiable access to the
        // 'initialIndentLevel' attribute of this object.

    const int& spacesPerLevel() const;
        // Return a reference providing non-modifiable access to the
        // 'spacesPerLevel' attribute of this object.
};

                      // ================================
                      // struct Formatter_CompactImplUtil
                      // ================================

struct Formatter_CompactImplUtil {
    // This utility 'struct' provides a namespace for a suite of operations
    // used to pretty-print XML documents given a sequence of tokens to emit.
    // Together with 'Formatter_CompactImplState', this 'struct' provides an
    // implementation of a state machine for such pretty-printing.

    // TYPES
    typedef Formatter_CompactImplState   State;
    typedef Formatter_CompactImplStateId StateId;
    typedef FormatterWhitespaceType      WhitespaceType;

  private:
    // PRIVATE CLASS METHODS
    static void addCommentImpl(bsl::ostream&            stream,
                               State                   *state,
                               const bsl::string_view&  comment,
                               const bsl::string_view&  openMarker,
                               const bsl::string_view&  closeMarker);
        // Write the specified 'openMarker', 'comment', and 'closeMarker' into
        // the specified 'stream', with formatting depending on the specified
        // 'state', and update the 'state' accordingly.  Note that if an
        // element-opening tag is not completed with a '>', this function will
        // add '>'.

  public:
    // CLASS METHODS
    template <class VALUE_TYPE>
    static bsl::ostream& addAttribute(
                   bsl::ostream&            stream,
                   State                   *state,
                   const bsl::string_view&  name,
                   const VALUE_TYPE&        value,
                   int                      valueFormattingMode = 0,
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
        // specify a 'valueFormattingMode' and 'encoderOptions' to control the
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
        // update 'state' accordingly.  Return the 'stream'.  'value' can be of
        // the following types: 'char', 'short', 'int', 'bsls::Types::Int64',
        // 'float', 'double', 'bsl::string', 'bdlt::Datetime', 'bdlt::Date',
        // and 'bdlt::Time'.  Perform no line-wrapping or indentation as if the
        // whitespace constraint were always 'BAEXML_PRESERVE_WHITESPACE' in
        // 'openElement', with the only exception that an initial newline and
        // an initial indent is added when 'openElement' specifies
        // 'BAEXML_NEWLINE_INDENT' option.  If 'value' is of type
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
        // format a new XML document as if the formatter were just constructed.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                      // --------------------------------
                      // class Formatter_CompactImplState
                      // --------------------------------

// CREATORS
inline
Formatter_CompactImplState::Formatter_CompactImplState()
: d_id()
, d_indentLevel()
, d_initialIndentLevel()
, d_spacesPerLevel()
, d_column()
{
}

inline
Formatter_CompactImplState::Formatter_CompactImplState(int indentLevel,
                                                       int spacesPerLevel)
: d_id()
, d_indentLevel(indentLevel)
, d_initialIndentLevel(indentLevel)
, d_spacesPerLevel(spacesPerLevel)
, d_column()
{
}

inline
Formatter_CompactImplState::Formatter_CompactImplState(
                                                   Id::Enum id,
                                                   int      indentLevel,
                                                   int      initialIndentLevel,
                                                   int      spacesPerLevel,
                                                   int      column)
: d_id(id)
, d_indentLevel(indentLevel)
, d_initialIndentLevel(initialIndentLevel)
, d_spacesPerLevel(spacesPerLevel)
, d_column(column)
{
}

// MANIPULATORS
inline
int& Formatter_CompactImplState::column()
{
    return d_column;
}

inline
Formatter_CompactImplStateId::Enum& Formatter_CompactImplState::id()
{
    return d_id;
}

inline
int& Formatter_CompactImplState::indentLevel()
{
    return d_indentLevel;
}

inline
int& Formatter_CompactImplState::initialIndentLevel()
{
    return d_initialIndentLevel;
}

inline
int& Formatter_CompactImplState::spacesPerLevel()
{
    return d_spacesPerLevel;
}

// ACCESSORS
inline
const int& Formatter_CompactImplState::column() const
{
    return d_column;
}

inline
const Formatter_CompactImplStateId::Enum&
Formatter_CompactImplState::id() const
{
    return d_id;
}

inline
const int& Formatter_CompactImplState::indentLevel() const
{
    return d_indentLevel;
}

inline
const int& Formatter_CompactImplState::initialIndentLevel() const
{
    return d_initialIndentLevel;
}

inline
const int& Formatter_CompactImplState::spacesPerLevel() const
{
    return d_spacesPerLevel;
}

                      // --------------------------------
                      // struct Formatter_CompactImplUtil
                      // --------------------------------

// CLASS METHODS
template <class VALUE_TYPE>
bsl::ostream& Formatter_CompactImplUtil::addAttribute(
                                       bsl::ostream&            stream,
                                       State                   *state,
                                       const bsl::string_view&  name,
                                       const VALUE_TYPE&        value,
                                       int                      formattingMode,
                                       const EncoderOptions&    encoderOptions)
{

    stream << ' ' << name << "=\"";
    TypesPrintUtil::print(stream, value, formattingMode, &encoderOptions);
    stream << '"';

    // Minimum output if value is empty.
    state->column() += static_cast<int>(name.length()) + 4;

    return stream;
}


template <class VALUE_TYPE>
bsl::ostream& Formatter_CompactImplUtil::addData(
                                         bsl::ostream&          stream,
                                         State                 *state,
                                         const VALUE_TYPE&      value,
                                         int                    formattingMode,
                                         const EncoderOptions&  encoderOptions)
{
    // Step 1: Print a sequence of conditional tokens to 'stream' and update
    //         the column number and indentation level of 'state'.

    if (StateId::e_IN_TAG == state->id()) {
        stream << '>';
        state->column() += 1;
    }

    TypesPrintUtil::print(stream, value, formattingMode, &encoderOptions);
    state->column() += 1;

    // Step 2: Update the ID of 'state'.

    state->id() = StateId::e_TRAILING_DATA_BETWEEN_TAGS;

    return stream;
}

template <class TYPE>
bsl::ostream& Formatter_CompactImplUtil::addElementAndData(
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
bsl::ostream& Formatter_CompactImplUtil::addListData(
                                         bsl::ostream&          stream,
                                         State                 *state,
                                         const VALUE_TYPE&      value,
                                         int                    formattingMode,
                                         const EncoderOptions&  encoderOptions)
{
    // Step 1: Print a sequence of conditional tokens to 'stream' and update
    //         the column number and indentation level of 'state'.

    if (StateId::e_IN_TAG == state->id()) {
        stream << '>';
        state->column() += 1;
    }

    if (StateId::e_FIRST_DATA_BETWEEN_TAGS != state->id() &&
        StateId::e_IN_TAG != state->id()) {
        stream << ' ';
        state->column() += 1;
    }

    TypesPrintUtil::print(stream, value, formattingMode, &encoderOptions);
    state->column() += 1; // assume value is not empty

    // Step 2: Update the ID of 'state'.

    state->id() = StateId::e_TRAILING_DATA_BETWEEN_TAGS;

    return stream;
}

inline
void Formatter_CompactImplUtil::reset(State *state)
{
    state->column()      = 0;
    state->id()          = StateId::e_AT_START;
    state->indentLevel() = state->initialIndentLevel();
}

}  // close package namespace
}  // close enterprise namespace

#endif // INCLUDED_BALXML_FORMATTER_COMPACTIMPL

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
