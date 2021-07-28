// balxml_formatter_prettyimpl.cpp                                    -*-C++-*-
#include <balxml_formatter_prettyimpl.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balxml_formatter_prettyimpl_cpp,"$Id$ $CSID$")

#include <bdlb_print.h>

#include <bsls_assert.h>

#include <bsl_algorithm.h>

namespace BloombergLP {
namespace balxml {

                       // ------------------------------
                       // class Formatter_PrettyImplUtil
                       // ------------------------------

// PRIVATE CLASS METHODS
void Formatter_PrettyImplUtil::addAttributeImpl(
                                               bsl::ostream&            stream,
                                               State                   *state,
                                               const bsl::string_view&  name,
                                               const bsl::string_view&  value)
{
    BSLS_ASSERT(StateId::e_IN_TAG == state->id());
    BSLS_ASSERT(0 != name.length());

    // 4 accounts for ' ', '=', '"', '"'
    const int attrLen = static_cast<int>(name.length() + value.length()) + 4;

    // 2 accounts for possible "/>"
    const bool willLineOverflow =
        state->wrapColumn() != k_INFINITE_WRAP_COLUMN &&
        state->column() + attrLen + 2 >= state->wrapColumn();

    if (willLineOverflow) {
        stream << '\n';
        bdlb::Print::indent(
            stream, state->indentLevel(), state->spacesPerLevel());
        stream << name << "=\"" << value << '"';

        state->column() = state->indentLevel() * state->spacesPerLevel() +
                          static_cast<int>(name.length() + value.length() + 3);
    }
    else {
        stream << ' ' << name << "=\"" << value << '"';
        state->column() += attrLen;
    }
}

void Formatter_PrettyImplUtil::addCommentImpl(
                                          bsl::ostream&            stream,
                                          State                   *state,
                                          const bsl::string_view&  comment,
                                          const bsl::string_view&  openMarker,
                                          const bsl::string_view&  closeMarker)
{
    // Step 1: Print a sequence of conditional tokens to 'stream' and update
    //         the column number and indentation level of 'state'.

    if (StateId::e_IN_TAG == state->id()) {
        stream << '>';
        ++state->column();
    }

    stream << ' ' << openMarker << comment << closeMarker;
    state->column() +=
        1 + static_cast<int>(openMarker.length() + comment.length() +
                             closeMarker.length());

    // Step 2: Update the ID of 'state'.

    if (StateId::e_IN_TAG == state->id()) {
        state->id() = StateId::e_FIRST_DATA_BETWEEN_TAGS;
    }
    else if (StateId::e_AT_START == state->id()) {
        state->id() = StateId::e_AFTER_START_NO_TAG;
    }
}

void Formatter_PrettyImplUtil::addCommentOnNewLineImpl(
                                          bsl::ostream&            stream,
                                          State                   *state,
                                          const bsl::string_view&  comment,
                                          const bsl::string_view&  openMarker,
                                          const bsl::string_view&  closeMarker)
{
    // Step 1: Print a sequence of conditional tokens to 'stream' and update
    //         the column number and indentation level of 'state'.

    if (StateId::e_IN_TAG == state->id()) {
        stream << ">";
        ++state->column();
    }

    if (0 != state->column()) {
        stream << '\n';
        state->column() += 1;
    }

    bdlb::Print::indent(stream, state->indentLevel(), state->spacesPerLevel());
    stream << openMarker << comment << closeMarker << '\n';
    state->column() = 0;

    // Step 2: Update the ID of 'state'.

    if (StateId::e_AT_START == state->id()) {
        state->id() = StateId::e_AFTER_START_NO_TAG;
    }
    else if (StateId::e_IN_TAG == state->id() ||
             StateId::e_FIRST_DATA_BETWEEN_TAGS == state->id() ||
             StateId::e_TRAILING_DATA_BETWEEN_TAGS == state->id()) {
        state->id() = StateId::e_FIRST_DATA_AT_LINE_BETWEEN_TAGS;
    }
}

void Formatter_PrettyImplUtil::addDataImpl(bsl::ostream&            stream,
                                           State                   *state,
                                           const bsl::string_view&  value)
{
    BSLS_ASSERT(StateId::e_FIRST_DATA_BETWEEN_TAGS == state->id() ||
                StateId::e_FIRST_DATA_AT_LINE_BETWEEN_TAGS == state->id() ||
                StateId::e_TRAILING_DATA_BETWEEN_TAGS == state->id() ||
                StateId::e_IN_TAG == state->id());

    // Step 1: Print a sequence of conditional tokens to 'stream' and update
    //         the column number and indentation level of 'state'.

    typedef WhitespaceType Ws;
    static const Ws::Enum  k_NEWLINE_INDENT = Ws::e_NEWLINE_INDENT;
    const Ws::Enum         ws               = state->elementNesting().back();
    const int              valueLen         = static_cast<int>(value.length());

    const bool isFirstData =
                     StateId::e_IN_TAG                          == state->id()
                  || StateId::e_FIRST_DATA_BETWEEN_TAGS         == state->id()
                  || StateId::e_FIRST_DATA_AT_LINE_BETWEEN_TAGS == state->id();

    if (StateId::e_IN_TAG == state->id()) {
        stream << '>';
        ++state->column();
    }

    if (isFirstData && k_NEWLINE_INDENT == ws && 0 != state->column()) {
        stream << '\n';
        state->column() = 0;
    }

    if (0 != valueLen && isFirstData && k_NEWLINE_INDENT == ws) {
        bdlb::Print::indent(
            stream, state->indentLevel(), state->spacesPerLevel());
        state->column() += state->indentLevel() * state->spacesPerLevel();
    }

    if (0 != valueLen) {
        stream << value;
        state->column() += valueLen;
    }

    // Step 2: Update the ID of 'state'.

    if (0 != valueLen) {
        state->id() = StateId::e_TRAILING_DATA_BETWEEN_TAGS;
    }
    else if (StateId::e_IN_TAG == state->id()) {
        state->id() = StateId::e_FIRST_DATA_BETWEEN_TAGS;
    }
}

void Formatter_PrettyImplUtil::addListDataImpl(bsl::ostream&            stream,
                                               State                   *state,
                                               const bsl::string_view&  value)
{
    BSLS_ASSERT(StateId::e_FIRST_DATA_BETWEEN_TAGS == state->id() ||
                StateId::e_FIRST_DATA_AT_LINE_BETWEEN_TAGS == state->id() ||
                StateId::e_TRAILING_DATA_BETWEEN_TAGS == state->id() ||
                StateId::e_IN_TAG == state->id());

    // Step 1: Print a sequence of conditional tokens to 'stream' and update
    //         the column number and indentation level of 'state'.

    if (StateId::e_IN_TAG == state->id()) {
        stream << '>';
        ++state->column();
    }

    typedef WhitespaceType Ws;
    static const Ws::Enum  k_PRESERVE_WHITESPACE = Ws::e_PRESERVE_WHITESPACE;
    static const Ws::Enum  k_WORDWRAP            = Ws::e_WORDWRAP;
    static const Ws::Enum  k_WORDWRAP_INDENT     = Ws::e_WORDWRAP_INDENT;
    static const Ws::Enum  k_NEWLINE_INDENT      = Ws::e_NEWLINE_INDENT;

    const Ws::Enum ws = state->elementNesting().back();

    const bool isFirstDataAtLine =
        StateId::e_FIRST_DATA_AT_LINE_BETWEEN_TAGS == state->id();
    const bool isFirstData = StateId::e_IN_TAG                  == state->id()
                          || StateId::e_FIRST_DATA_BETWEEN_TAGS == state->id();
    const bool isTrailingData =
        StateId::e_TRAILING_DATA_BETWEEN_TAGS == state->id();

    const int  valueLen = static_cast<int>(value.length());

    const bool willLineOverflow =
        state->wrapColumn() != k_INFINITE_WRAP_COLUMN &&
        state->wrapColumn() <= state->column() + valueLen;

    if (0 != state->column() && k_NEWLINE_INDENT == ws && isFirstData) {
        stream << '\n';
        state->column() = 0;
    }

    if (0 != valueLen && 0 != state->column() && willLineOverflow &&
        (k_NEWLINE_INDENT  == ws || k_WORDWRAP_INDENT == ws ||
         k_WORDWRAP        == ws)) {
        stream << '\n';
        state->column() = 0;
    }

    if (0 != valueLen &&
        ((k_PRESERVE_WHITESPACE != ws && isFirstDataAtLine) ||
         (k_NEWLINE_INDENT      == ws && isFirstData      ) ||
         (k_NEWLINE_INDENT      == ws && willLineOverflow ) ||
         (k_WORDWRAP_INDENT     == ws && willLineOverflow ))) {
        bdlb::Print::indent(
            stream, state->indentLevel(), state->spacesPerLevel());
        state->column() += state->indentLevel() * state->spacesPerLevel();

        // The presence of an indent before this list data indicates that it is
        // on its own line.  Set the whitespace mode of the current element to
        // 'WhitespaceType::e_NEWLINE_INDENT' in order to ensure that the
        // eventual closing tag for this element also appears on its own line,
        // as opposed to on the same line as the last data.

        state->elementNesting().back() = k_NEWLINE_INDENT;
    }

    if (0 != valueLen &&
        ((isTrailingData && k_PRESERVE_WHITESPACE == ws) ||
         (isTrailingData && !willLineOverflow          ))) {
        stream << ' ';
        state->column() += 1;
    }

    if (0 != valueLen) {
        stream << value;
        state->column() += valueLen;
    }

    // Step 2: Update the ID of 'state'.

    if (0 != valueLen) {
        state->id() = StateId::e_TRAILING_DATA_BETWEEN_TAGS;
    }
    else if (StateId::e_IN_TAG == state->id()) {
        state->id() = StateId::e_FIRST_DATA_BETWEEN_TAGS;
    }
}

// CLASS METHODS
bsl::ostream& Formatter_PrettyImplUtil::addBlankLine(bsl::ostream&  stream,
                                                     State         *state)
{
    // Step 1: Print a sequence of conditional tokens to 'stream' and update
    //         the column number and indentation level of 'state'.

    if (StateId::e_IN_TAG == state->id()) {
        stream << ">";
        ++state->column();
    }

    if (0 != state->column()) {
        stream << "\n";
        state->column() = 0;
    }

    stream << '\n';
    state->column() = 0;

    // Step 2: Update the ID of 'state'.

    if (StateId::e_IN_TAG == state->id() ||
        StateId::e_FIRST_DATA_BETWEEN_TAGS == state->id() ||
        StateId::e_TRAILING_DATA_BETWEEN_TAGS == state->id()) {
        state->id() = StateId::e_FIRST_DATA_AT_LINE_BETWEEN_TAGS;
    }

    return stream;
}

bsl::ostream& Formatter_PrettyImplUtil::addComment(
                                         bsl::ostream&            stream,
                                         State                   *state,
                                         const bsl::string_view&  comment,
                                         bool                     forceNewline)
{
    const bsl::string_view openMarker  = "<!-- ";
    const bsl::string_view closeMarker = " -->";

    if (forceNewline) {
        addCommentOnNewLineImpl(stream,
                                state,
                                comment,
                                openMarker,
                                closeMarker);
    }
    else {
        addCommentImpl(stream, state, comment, openMarker, closeMarker);
    }

    return stream;
}

bsl::ostream& Formatter_PrettyImplUtil::addHeader(
                                             bsl::ostream&            stream,
                                             State                   *state,
                                             const bsl::string_view&  encoding)
{
    BSLS_ASSERT(StateId::e_AT_START == state->id());

    // Step 1: Print a sequence of conditional tokens to 'stream' and update
    //         the column number and indentation level of 'state'.

    const bsl::string_view startHeader = "<?xml version=\"1.0\" encoding=\"";
    const bsl::string_view endHeader   = "\" ?>";

    stream << startHeader << encoding << endHeader << '\n';
    state->column() = 0;

    // Step 2: Update the ID of 'state'.

    state->id() = StateId::e_AFTER_START_NO_TAG;

    return stream;
}

bsl::ostream& Formatter_PrettyImplUtil::addNewline(bsl::ostream&  stream,
                                                   State         *state)
{
    // Step 1: Print a sequence of conditional tokens to 'stream' and update
    //         the column number and indentation level of 'state'.

    if (StateId::e_IN_TAG == state->id()) {
        stream << '>';
        ++state->column();
    }

    stream << '\n';
    state->column() = 0;

    // Step 2: Update the ID of 'state'.

    if (StateId::e_IN_TAG == state->id() ||
        StateId::e_FIRST_DATA_BETWEEN_TAGS == state->id() ||
        StateId::e_TRAILING_DATA_BETWEEN_TAGS == state->id()) {
        state->id() = StateId::e_FIRST_DATA_AT_LINE_BETWEEN_TAGS;
    }

    return stream;
}

int Formatter_PrettyImplUtil::addValidComment(
                              bsl::ostream&            stream,
                              State                   *state,
                              const bsl::string_view&  comment,
                              bool                     forceNewline,
                              bool                     omitEnclosingWhitespace)
{
    const char doubleHyphen[] = "--";
    // The string "--" (double-hyphen) must not occur within comments.  Also
    // the grammar does not allow a comment ending in "--->".
    if (comment.end() != bsl::search(comment.begin(),
                                     comment.end(),
                                     doubleHyphen,
                                     doubleHyphen + sizeof doubleHyphen - 1) ||
        (omitEnclosingWhitespace && !comment.empty() &&
         '-' == *comment.rbegin())) {
        return 1;                                                     // RETURN
    }

    const bsl::string_view openMarker =
        omitEnclosingWhitespace ? "<!--" : "<!-- ";
    const bsl::string_view closeMarker =
        omitEnclosingWhitespace ? "-->" : " -->";

    if (forceNewline) {
        addCommentOnNewLineImpl(stream,
                                state,
                                comment,
                                openMarker,
                                closeMarker);
    }
    else {
        addCommentImpl(stream, state, comment, openMarker, closeMarker);
    }

    return 0;
}

bsl::ostream& Formatter_PrettyImplUtil::closeElement(
                                               bsl::ostream&            stream,
                                               State                   *state,
                                               const bsl::string_view&  name)
{
    BSLS_ASSERT(StateId::e_FIRST_DATA_BETWEEN_TAGS == state->id() ||
                StateId::e_FIRST_DATA_AT_LINE_BETWEEN_TAGS == state->id() ||
                StateId::e_TRAILING_DATA_BETWEEN_TAGS == state->id() ||
                StateId::e_IN_TAG == state->id());
    BSLS_ASSERT(!state->elementNesting().empty());

    // Step 1: Print a sequence of conditional tokens to 'stream' and update
    //         the column number and indentation level of 'state'.

    typedef WhitespaceType Ws;
    static const Ws::Enum  k_NEWLINE_INDENT = Ws::e_NEWLINE_INDENT;

    const Ws::Enum ws = state->elementNesting().back();

    if (StateId::e_IN_TAG == state->id()) {
        stream << "/>";
        state->column() += 2;
    }

    if (StateId::e_IN_TAG == state->id() ||
        (0 != state->column() && k_NEWLINE_INDENT == ws)) {
        stream << '\n';
        state->column() = 0;
    }

    if (StateId::e_IN_TAG != state->id() &&
        (0 == state->column() || k_NEWLINE_INDENT == ws)) {
        bdlb::Print::indent(
            stream, state->indentLevel() - 1, state->spacesPerLevel());
        state->column() +=
            (state->indentLevel() - 1) * state->spacesPerLevel();
    }

    if (StateId::e_IN_TAG != state->id()) {
        stream << "</" << name << ">\n";
        state->column() = 0;
    }

    if (1 == state->elementNesting().size()) {
        stream.flush();
    }

    state->elementNesting().pop_back();
    state->column() = 0;
    --state->indentLevel();

    // Step 2: Update the ID of 'state'.

    if (state->elementNesting().empty()) {
        state->id() = StateId::e_AT_END;
    }
    else {
        state->id() = StateId::e_FIRST_DATA_AT_LINE_BETWEEN_TAGS;
    }

    return stream;
}

bsl::ostream& Formatter_PrettyImplUtil::flush(bsl::ostream&  stream,
                                              State         *state)
{
    // Step 1: Print a sequence of conditional tokens to 'stream' and update
    //         the column number and indentation level of 'state'.

    if (StateId::e_IN_TAG == state->id()) {
        stream << '>';
        ++state->column();
    }

    stream.flush();

    // Step 2: Update the ID of 'state'.

    if (StateId::e_IN_TAG == state->id()) {
        state->id() = StateId::e_FIRST_DATA_BETWEEN_TAGS;
    }

    return stream;
}

bsl::ostream& Formatter_PrettyImplUtil::openElement(
                                       bsl::ostream&            stream,
                                       State                   *state,
                                       const bsl::string_view&  name,
                                       WhitespaceType::Enum     whitespaceMode)
{
    // Step 1: Print a sequence of conditional tokens to 'stream' and update
    //         the column number and indentation level of 'state'.

    if (StateId::e_IN_TAG == state->id()) {
        stream << '>';
        ++state->column();
    }

    if (0 != state->column()) {
        stream << '\n';
        state->column() = 0;
    }

    bdlb::Print::indent(stream, state->indentLevel(), state->spacesPerLevel());
    stream << '<' << name;
    state->column() += state->indentLevel() * state->spacesPerLevel() + 1 +
                       static_cast<int>(name.length());

    state->elementNesting().push_back(whitespaceMode);
    ++state->indentLevel();

    // Step 2: Update the ID of 'state'.

    state->id() = StateId::e_IN_TAG;

    return stream;
}

}  // close package namespace
}  // close enterprise namespace

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
