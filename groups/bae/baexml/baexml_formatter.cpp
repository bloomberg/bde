// baexml_formatter.cpp                                               -*-C++-*-
#include <baexml_formatter.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baexml_formatter_cpp,"$Id$ $CSID$")

#include <bdeu_print.h>

#include <bsls_assert.h>

#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_utility.h>

namespace BloombergLP {

                        // ===================================
                        // class baexml_Formatter::ElemContext
                        // ===================================

#ifdef BDE_BUILD_TARGET_SAFE2
baexml_Formatter::ElemContext::ElemContext(const bdeut_StringRef& tag,
                                           WhitespaceType         ws)
: d_ws(ws), d_tagLen(bsl::min(tag.length(), 255))
{
    int len = bsl::min(int(BAEXML_TRUNCATED_TAG_LEN), tag.length());
    bsl::memcpy(d_tag, tag.data(), len);
}

bool baexml_Formatter::ElemContext::matchTag(const bdeut_StringRef& tag) const
{
    if (d_tagLen != bsl::min(tag.length(), 255)) {
        // Lengths don't match
        return false;
    }

    int len = bsl::min(int(BAEXML_TRUNCATED_TAG_LEN), tag.length());
    return 0 == bsl::memcmp(d_tag, tag.data(), len);
}
#endif

                        // ======================
                        // class baexml_Formatter
                        // ======================

// CREATORS
baexml_Formatter::baexml_Formatter(bsl::streambuf  *output,
                                   int              indentLevel,
                                   int              spacesPerLevel,
                                   int              wrapColumn,
                                   bslma_Allocator *basic_allocator)
: d_outputStreamObj(output)
, d_outputStream(d_outputStreamObj)
, d_indentLevel(indentLevel)
, d_spacesPerLevel(spacesPerLevel)
, d_column(0)
, d_wrapColumn(wrapColumn)
, d_elementNesting(basic_allocator)
, d_state(BAEXML_AT_START)
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

baexml_Formatter::baexml_Formatter(bsl::ostream&    output,
                                   int              indentLevel,
                                   int              spacesPerLevel,
                                   int              wrapColumn,
                                   bslma_Allocator *basic_allocator)
: d_outputStreamObj(0)
, d_outputStream(output)
, d_indentLevel(indentLevel)
, d_spacesPerLevel(spacesPerLevel)
, d_column(0)
, d_wrapColumn(wrapColumn)
, d_elementNesting(basic_allocator)
, d_state(BAEXML_AT_START)
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
void baexml_Formatter::indent()
{
    if (d_wrapColumn < 0) {
        return;
    }

    if (0 != d_column) {
        d_outputStream << '\n';
    }

    bdeu_Print::indent(d_outputStream, d_indentLevel, d_spacesPerLevel);
    d_column = d_indentLevel * d_spacesPerLevel;
}

void baexml_Formatter::doAddAttribute(const bdeut_StringRef& name,
                                      const bdeut_StringRef& value)
{
    BSLS_ASSERT(BAEXML_IN_TAG == d_state);
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

void baexml_Formatter::doAddData(const bdeut_StringRef& value, bool addSpace)
{
    BSLS_ASSERT(BAEXML_BETWEEN_TAGS == d_state);

    int valueLen = value.length();
    if (d_wrapColumn >= 0) {
        if (d_isFirstData && d_column > 0 &&
            BAEXML_NEWLINE_INDENT == d_elementNesting.back().ws()) {
            addNewline();
        }

        if (0 == valueLen) {
            return;
        }

        WhitespaceType ws = d_elementNesting.back().ws();
        if (addSpace && BAEXML_PRESERVE_WHITESPACE != ws) {
            if (d_column + valueLen >= d_wrapColumn || 0 == d_column) {
                if (BAEXML_WORDWRAP == ws
                 && d_column > 0 && d_wrapColumn >= 0) {
                    addNewline();
                }
                else {
                    indent();
                    d_elementNesting.back().setWs(BAEXML_NEWLINE_INDENT);
                    // Force BAEXML_WORDWRAP_INDENT to become
                    // BAEXML_NEWLINE_INDENT now that there is line wrapping
                    // for the data, so that the closing tag doesn't share its
                    // line with data.
                    // TBD: this is not documented in the interface.
                }
                d_isFirstDataAtLine = true;
            }
        }
        else if (!addSpace && d_isFirstData && BAEXML_NEWLINE_INDENT == ws) {
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
void baexml_Formatter::openElement(const bdeut_StringRef& name,
                                   WhitespaceType         ws)
{
// TBD: Why ?
//     BSLS_ASSERT(d_state != BAEXML_AT_END);

    closeTagIfOpen();

    indent();
    d_outputStream << '<' << name;
    d_column += 1 + name.length();

    if (d_wrapColumn >= 0) {
        d_elementNesting.push_back(ElemContext(name, ws));
    }
    ++d_indentLevel;
    d_state = BAEXML_IN_TAG;
    d_isFirstData = true;
    d_isFirstDataAtLine = true;
}

void baexml_Formatter::closeElement(const bdeut_StringRef& name)
{
    BSLS_ASSERT(BAEXML_IN_TAG == d_state || BAEXML_BETWEEN_TAGS == d_state);
    BSLS_ASSERT(d_wrapColumn < 0 || ! d_elementNesting.empty());
#ifdef BDE_BUILD_TARGET_SAFE2
    BSLS_ASSERT(d_wrapColumn < 0 || d_elementNesting.back().matchTag(name));
#endif

    --d_indentLevel;

    if (BAEXML_IN_TAG == d_state) {
        // Empty element (may have attributes but no data).
        d_outputStream << "/>";
        d_column += 2;
    }
    else {
        if (d_wrapColumn > 0
         && (0 == d_column ||
             BAEXML_NEWLINE_INDENT == d_elementNesting.back().ws())) {
            // Indent this line.
            indent();
        }
        d_outputStream << "</" << name << '>';
        d_column += 3 + name.length();
    }

    d_isFirstData = false;

    if (d_wrapColumn < 0) {
        // Compact mode: 'd_elementNesting' is not used.
        // Use indent level to determine if we are at the end.
        d_state = 0 == d_indentLevel ? BAEXML_AT_END : BAEXML_BETWEEN_TAGS;
    }
    else {
        // Non-compact mode: Add newline at end of element.
        d_outputStream << '\n';
        d_column = 0;
        d_isFirstDataAtLine = true;

        // Pop element stack and compute new state.
        d_elementNesting.pop_back();
        d_state = d_elementNesting.empty() ? BAEXML_AT_END
                                           : BAEXML_BETWEEN_TAGS;
    }

    if (BAEXML_AT_END == d_state) {
        d_outputStream.flush();
    }
}

void baexml_Formatter::addHeader(const bdeut_StringRef& encoding)
{
    BSLS_ASSERT(BAEXML_AT_START == d_state);
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
    d_state = BAEXML_AFTER_START_NO_TAG;
}

void baexml_Formatter::addComment(const bdeut_StringRef& comment,
                                  bool                   forceNewline)
{
    if (BAEXML_AT_START == d_state) {
        d_state = BAEXML_AFTER_START_NO_TAG;
    }
    if (BAEXML_AFTER_START_NO_TAG != d_state) {
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

void baexml_Formatter::reset()
{
    d_outputStream.clear(); // Clear error condition(s)
    d_column = 0;
    d_state = BAEXML_AT_START;
    d_indentLevel -= d_elementNesting.size();
    d_elementNesting.clear();

    d_isFirstData = true;
    d_isFirstDataAtLine = true;
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
