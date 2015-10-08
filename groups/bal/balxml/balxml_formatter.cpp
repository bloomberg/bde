// balxml_formatter.cpp                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_formatter.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balxml_formatter_cpp,"$Id$ $CSID$")

#include <bdlb_print.h>

#include <bsls_assert.h>

#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_utility.h>

namespace BloombergLP {

                    // ====================================
                    // class balxml::Formatter::ElemContext
                    // ====================================

// ACCESSORS
#ifdef BDE_BUILD_TARGET_SAFE2
bool balxml::Formatter::ElemContext::matchTag(
                                            const bslstl::StringRef& tag) const
{
    if (d_tagLen != bsl::min(tag.length(), 255)) {
        // Lengths don't match
        return false;
    }

    int len = bsl::min(int(k_TRUNCATED_TAG_LEN), tag.length());
    return 0 == bsl::memcmp(d_tag, tag.data(), len);
}
#endif

namespace balxml {

                              // ===============
                              // class Formatter
                              // ===============

// CREATORS
Formatter::Formatter(bsl::streambuf   *output,
                     int               indentLevel,
                     int               spacesPerLevel,
                     int               wrapColumn,
                     bslma::Allocator *basic_allocator)
: d_outputStreamObj(output)
, d_outputStream(d_outputStreamObj)
, d_indentLevel(indentLevel)
, d_spacesPerLevel(spacesPerLevel)
, d_column(0)
, d_wrapColumn(wrapColumn)
, d_elementNesting(basic_allocator)
, d_state(e_AT_START)
, d_isFirstData(true)
, d_isFirstDataAtLine(true)
{
    if (d_wrapColumn < 0) {
        // In compact mode, we don't use the 'd_elementNesting' stack.  In
        // this case, we rely on 'd_indentLevel' to determine the depth of the
        // element stack.
        d_indentLevel = 0;
    }
}

Formatter::Formatter(bsl::ostream&     output,
                     int               indentLevel,
                     int               spacesPerLevel,
                     int               wrapColumn,
                     bslma::Allocator *basic_allocator)
: d_outputStreamObj(0)
, d_outputStream(output)
, d_indentLevel(indentLevel)
, d_spacesPerLevel(spacesPerLevel)
, d_column(0)
, d_wrapColumn(wrapColumn)
, d_elementNesting(basic_allocator)
, d_state(e_AT_START)
, d_isFirstData(true)
, d_isFirstDataAtLine(true)
{
    if (d_wrapColumn < 0) {
        // In compact mode, we don't use the 'd_elementNesting' stack.  In
        // this case, we rely on 'd_indentLevel' to determine the depth of the
        // element stack.
        d_indentLevel = 0;
    }
}

// PRIVATE MANIPULATORS
void Formatter::indent()
{
    if (d_wrapColumn < 0) {
        return;                                                       // RETURN
    }

    if (0 != d_column) {
        d_outputStream << '\n';
    }

    bdlb::Print::indent(d_outputStream, d_indentLevel, d_spacesPerLevel);
    d_column = d_indentLevel * d_spacesPerLevel;
}

void Formatter::doAddAttribute(const bslstl::StringRef& name,
                               const bslstl::StringRef& value)
{
    BSLS_ASSERT(e_IN_TAG == d_state);
    BSLS_ASSERT(0 != name.length());

    int attrLen = name.length() + value.length() + 4;
        // 4 accounts for ' ', '=', '"', '"'
    if (d_column + attrLen + 2 >= d_wrapColumn) {
                          // 2 accounts for possible "/>"
        indent();
        // No need to count ' ' after the element name
        --attrLen;
    }
    else {
        d_outputStream << ' ';
    }

    d_outputStream << name << "=\"" << value << '"';
    d_column += attrLen;
}

void Formatter::doAddData(const bslstl::StringRef& value, bool addSpace)
{
    BSLS_ASSERT(e_BETWEEN_TAGS == d_state);

    int valueLen = value.length();
    if (d_wrapColumn >= 0) {
        if (d_isFirstData && d_column > 0 &&
            e_NEWLINE_INDENT == d_elementNesting.back().ws()) {
            addNewline();
        }

        if (0 == valueLen) {
            return;                                                   // RETURN
        }

        WhitespaceType ws = d_elementNesting.back().ws();
        if (addSpace && e_PRESERVE_WHITESPACE != ws) {
            if (d_column + valueLen >= d_wrapColumn || 0 == d_column) {
                if (e_WORDWRAP == ws
                 && d_column > 0 && d_wrapColumn >= 0) {
                    addNewline();
                }
                else {
                    indent();
                    d_elementNesting.back().setWs(e_NEWLINE_INDENT);
                    // Force BAEXML_WORDWRAP_INDENT to become
                    // BAEXML_NEWLINE_INDENT now that there is line wrapping
                    // for the data, so that the closing tag doesn't share its
                    // line with data.
                    // TBD: this is not documented in the interface.
                }
                d_isFirstDataAtLine = true;
            }
        }
        else if (!addSpace && d_isFirstData && e_NEWLINE_INDENT == ws) {
            indent();
        } // else do nothing if BAEXML_PRESERVE_WHITESPACE or if addSpace is
          // false

        if (addSpace && !d_isFirstDataAtLine) {
            d_outputStream << ' ';
            ++d_column;
        }
    } // End if (do wrapping)

    d_outputStream << value;
    d_column += valueLen;
    d_isFirstData = false;
    d_isFirstDataAtLine = false;
}

// MANIPULATORS
void Formatter::openElement(const bslstl::StringRef& name,
                            WhitespaceType           ws)
{
// TBD: Why ?
//     BSLS_ASSERT(d_state != e_AT_END);

    closeTagIfOpen();

    indent();
    d_outputStream << '<' << name;
    d_column += 1 + name.length();

    if (d_wrapColumn >= 0) {
        d_elementNesting.push_back(ElemContext(name, ws));
    }
    ++d_indentLevel;
    d_state = e_IN_TAG;
    d_isFirstData = true;
    d_isFirstDataAtLine = true;
}

void Formatter::closeElement(const bslstl::StringRef& name)
{
    BSLS_ASSERT(e_IN_TAG == d_state || e_BETWEEN_TAGS == d_state);
    BSLS_ASSERT(d_wrapColumn < 0 || ! d_elementNesting.empty());
#ifdef BDE_BUILD_TARGET_SAFE2
    BSLS_ASSERT(d_wrapColumn < 0 || d_elementNesting.back().matchTag(name));
#endif

    --d_indentLevel;

    if (e_IN_TAG == d_state) {
        // Empty element (may have attributes but no data).
        d_outputStream << "/>";
        d_column += 2;
    }
    else {
        if (d_wrapColumn > 0
         && (0 == d_column ||
             e_NEWLINE_INDENT == d_elementNesting.back().ws())) {
            // Indent this line.
            indent();
        }
        d_outputStream << "</" << name << '>';
        d_column += 3 + name.length();
    }

    d_isFirstData = false;

    if (d_wrapColumn < 0) {
        // Compact mode: 'd_elementNesting' is not used.  Use indent level to
        // determine if we are at the end.
        d_state = 0 == d_indentLevel ? e_AT_END : e_BETWEEN_TAGS;
    }
    else {
        // Non-compact mode: Add newline at end of element.
        d_outputStream << '\n';
        d_column = 0;
        d_isFirstDataAtLine = true;

        // Pop element stack and compute new state.
        d_elementNesting.pop_back();
        d_state = d_elementNesting.empty() ? e_AT_END
                                           : e_BETWEEN_TAGS;
    }

    if (e_AT_END == d_state) {
        d_outputStream.flush();
    }
}

void Formatter::addHeader(const bslstl::StringRef& encoding)
{
    BSLS_ASSERT(e_AT_START == d_state);
    // TBD escape encoding?
    static const char startHeader[] = "<?xml version=\"1.0\" encoding=\"";
    static const char endHeader[]   = "\" ?>";
    d_outputStream << startHeader << encoding << endHeader;
    if (d_wrapColumn >= 0) {
        d_outputStream << '\n';
        d_column = 0;
    }
    else {
        d_column += (sizeof(startHeader) + sizeof(endHeader) - 2 +
                     encoding.length());
    }
    d_state = e_AFTER_START_NO_TAG;
}

void Formatter::addComment(const bslstl::StringRef& comment,
                           bool                     forceNewline)
{
    if (e_AT_START == d_state) {
        d_state = e_AFTER_START_NO_TAG;
    }
    if (e_AFTER_START_NO_TAG != d_state) {
        closeTagIfOpen();
    }
    bool isOnSeparateLine = false;
    if ((forceNewline || 0 == d_column) && d_wrapColumn >= 0) {
        indent();
        isOnSeparateLine = true;
    }
    else {
        d_outputStream << ' ';
        ++d_column;
    }
    d_outputStream << "<!-- " << comment << " -->";
    if (isOnSeparateLine) {
        d_outputStream << '\n';
        d_column = 0;
    }
    else {
        d_column += comment.length() + 9;
    }
}

void Formatter::reset()
{
    d_outputStream.clear(); // Clear error condition(s)
    d_column = 0;
    d_state = e_AT_START;
    d_indentLevel -= d_elementNesting.size();
    d_elementNesting.clear();

    d_isFirstData = true;
    d_isFirstDataAtLine = true;
}
}  // close package namespace

}  // close enterprise namespace

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
