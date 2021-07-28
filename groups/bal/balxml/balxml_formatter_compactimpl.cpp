// balxml_formatter_compactimpl.cpp                                   -*-C++-*-
#include <balxml_formatter_compactimpl.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balxml_formatter_compactimpl_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

#include <bsl_algorithm.h>

namespace BloombergLP {
namespace balxml {

                      // -------------------------------
                      // class Formatter_CompactImplUtil
                      // -------------------------------

// PRIVATE CLASS METHODS
void Formatter_CompactImplUtil::addCommentImpl(
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
        state->column() += 1;
    }

    stream << ' ' << openMarker << comment << closeMarker;
    state->column() +=
        1 + static_cast<int>(openMarker.length() + comment.length() +
                             closeMarker.length());

    // Step 2: Update the ID of 'state'.

    if (StateId::e_AT_START == state->id() ||
        StateId::e_IN_TAG == state->id()) {
        state->id() = StateId::e_FIRST_DATA_BETWEEN_TAGS;
    }
}


// CLASS METHODS
bsl::ostream& Formatter_CompactImplUtil::addBlankLine(bsl::ostream&  stream,
                                                      State         *state)
{
    // Step 1: Print a sequence of conditional tokens to 'stream' and update
    //         the column number and indentation level of 'state'.

    if (StateId::e_IN_TAG == state->id()) {
        stream << '>';
        state->column() += 1;
    }

    if (0 < state->column()) {
        stream << '\n';
        state->column() = 0;
    }

    stream << '\n';
    state->column() = 0;

    // Step 2: Update the ID of 'state'.

    if (StateId::e_IN_TAG == state->id()) {
        state->id() = StateId::e_FIRST_DATA_BETWEEN_TAGS;
    }

    return stream;
}

bsl::ostream& Formatter_CompactImplUtil::addComment(
                                              bsl::ostream&            stream,
                                              State                   *state,
                                              const bsl::string_view&  comment,
                                              bool)
{
    const bsl::string_view openMarker = "<!-- ";
    const bsl::string_view closeMarker = " -->";

    addCommentImpl(stream, state, comment, openMarker, closeMarker);

    return stream;
}

bsl::ostream& Formatter_CompactImplUtil::addHeader(
                                             bsl::ostream&            stream,
                                             State                   *state,
                                             const bsl::string_view&  encoding)
{
    BSLS_ASSERT(StateId::e_AT_START == state->id());

    // Step 1: Print a sequence of conditional tokens to 'stream' and update
    //         the column number and indentation level of 'state'.

    const bsl::string_view startHeader = "<?xml version=\"1.0\" encoding=\"";
    const bsl::string_view endHeader   = "\" ?>";

    stream << startHeader << encoding << endHeader;
    state->column() += static_cast<int>(
        startHeader.length() + encoding.length() + endHeader.length());

    // Step 2: Update the ID of 'state'.

    state->id() = StateId::e_FIRST_DATA_BETWEEN_TAGS;

    return stream;
}

bsl::ostream& Formatter_CompactImplUtil::addNewline(bsl::ostream&  stream,
                                                    State         *state)
{
    // Step 1: Print a sequence of conditional tokens to 'stream' and update
    //         the column number and indentation level of 'state'.

    if (StateId::e_IN_TAG == state->id()) {
        stream << '>';
        state->column() += 1;
    }

    stream << '\n';
    state->column() = 0;

    // Step 2: Update the ID of 'state'.

    if (StateId::e_IN_TAG == state->id()) {
        state->id() = StateId::e_FIRST_DATA_BETWEEN_TAGS;
    }

    return stream;
}

int Formatter_CompactImplUtil::addValidComment(
                              bsl::ostream&            stream,
                              State                   *state,
                              const bsl::string_view&  comment,
                              bool,
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

    addCommentImpl(stream, state, comment, openMarker, closeMarker);

    return 0;
}

bsl::ostream& Formatter_CompactImplUtil::closeElement(
                                               bsl::ostream&            stream,
                                               State                   *state,
                                               const bsl::string_view&  name)
{
    BSLS_ASSERT(StateId::e_IN_TAG == state->id() ||
                StateId::e_FIRST_DATA_BETWEEN_TAGS == state->id() ||
                StateId::e_TRAILING_DATA_BETWEEN_TAGS == state->id());
    BSLS_ASSERT(0 != state->indentLevel());

    // Step 1: Print a sequence of conditional tokens to 'stream' and update
    //         the column number and indentation level of 'state'.

    if (StateId::e_IN_TAG == state->id()) {
        stream << "/>";
        state->column() += 2;
        state->indentLevel() -= 1;
    }
    else {
        stream << "</" << name << '>';
        state->column() += 3 + static_cast<int>(name.length());
        state->indentLevel() -= 1;
    }

    if (0 == state->indentLevel()) {
        stream.flush();
    }

    // Step 2: Update the ID of 'state'.

    state->id() = StateId::e_TRAILING_DATA_BETWEEN_TAGS;

    return stream;
}

bsl::ostream& Formatter_CompactImplUtil::flush(bsl::ostream&  stream,
                                               State         *state)
{
    // Step 1: Print a sequence of conditional tokens to 'stream' and update
    //         the column number and indentation level of 'state'.

    if (StateId::e_IN_TAG == state->id()) {
        stream << '>';
        state->column() += 1;
    }

    stream.flush();

    // Step 2: Update the ID of 'state'.

    if (StateId::e_IN_TAG == state->id()) {
        state->id() = StateId::e_FIRST_DATA_BETWEEN_TAGS;
    }

    return stream;
}

bsl::ostream& Formatter_CompactImplUtil::openElement(
                                               bsl::ostream&            stream,
                                               State                   *state,
                                               const bsl::string_view&  name,
                                               WhitespaceType::Enum)
{

    // Step 1: Print a sequence of conditional tokens to 'stream' and update
    //         the column number and indentation level of 'state'.

    if (StateId::e_IN_TAG == state->id()) {
        stream << '>';
        state->column() += 1;
    }

    stream << '<' << name;
    state->column() += 1 + static_cast<int>(name.length());
    state->indentLevel() += 1;

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
