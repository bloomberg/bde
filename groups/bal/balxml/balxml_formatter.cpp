// balxml_formatter.cpp                                               -*-C++-*-
#include <balxml_formatter.h>

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balxml_formatter_cpp,"$Id$ $CSID$")

#include <bdlb_print.h>

namespace BloombergLP {
namespace balxml {

                           // ---------------------
                           // class Formatter_State
                           // ---------------------

// PRIVATE CREATORS
Formatter_State::Formatter_State(int                   indentLevel,
                                 int                   spacesPerLevel,
                                 int                   wrapColumn,
                                 const allocator_type& allocator)
: d_mode(k_COMPACT_MODE_WRAP_COLUMN == wrapColumn ? Mode::e_COMPACT
                                                  : Mode::e_PRETTY)
, d_allocator(allocator)
{
    switch (d_mode) {
      case Mode::e_COMPACT: {
        new (d_compact.buffer()) Compact(indentLevel, spacesPerLevel);
      } break;
      case Mode::e_PRETTY: {
        new (d_pretty.buffer())
            Pretty(indentLevel, spacesPerLevel, wrapColumn, allocator);
      } break;
    }
}

// CREATORS
Formatter_State::Formatter_State(const Formatter_State& original,
                                 const allocator_type&  allocator)
: d_mode(original.d_mode)
, d_allocator(allocator)
{
    switch (original.d_mode) {
      case Mode::e_COMPACT: {
        new (d_compact.buffer()) Compact(original.d_compact.object());
      } break;
      case Mode::e_PRETTY: {
        new (d_pretty.buffer()) Pretty(original.d_pretty.object(), allocator);
      } break;
    }
}

Formatter_State::~Formatter_State()
{
    switch (d_mode) {
      case Mode::e_COMPACT: {
        d_compact.object().~Compact();
      } break;
      case Mode::e_PRETTY: {
        d_pretty.object().~Pretty();
      } break;
    }
}

// MANIPULATORS
Formatter_State& Formatter_State::operator=(const Formatter_State& rhs)
{
    switch (d_mode) {
      case Mode::e_COMPACT: {
        d_compact.object().~Compact();
      } break;
      case Mode::e_PRETTY: {
        d_pretty.object().~Pretty();
      } break;
    }

    d_mode = rhs.d_mode;

    switch (rhs.d_mode) {
      case Mode::e_COMPACT: {
        new (d_compact.buffer()) Compact(rhs.d_compact.object());
      } break;
      case Mode::e_PRETTY: {
        new (d_pretty.buffer())
            Pretty(rhs.d_pretty.object(), d_allocator);
      } break;
    }

    return *this;
}

                              // ===============
                              // class Formatter
                              // ===============

// CLASS DATA
#ifdef BDE_VERIFY
#pragma bde_verify push
#pragma bde_verify - MN03
#pragma bde_verify - UC01
#endif

const FormatterWhitespaceType::Enum Formatter::e_PRESERVE_WHITESPACE;
const FormatterWhitespaceType::Enum Formatter::e_WORDWRAP;
const FormatterWhitespaceType::Enum Formatter::e_WORDWRAP_INDENT;
const FormatterWhitespaceType::Enum Formatter::e_NEWLINE_INDENT;
const FormatterWhitespaceType::Enum Formatter::BAEXML_NEWLINE_INDENT;

#ifdef BDE_VERIFY
#pragma bde_verify pop
#endif

// CREATORS
Formatter::Formatter(bsl::streambuf   *output,
                     int               indentLevel,
                     int               spacesPerLevel,
                     int               wrapColumn,
                     bslma::Allocator *basicAllocator)
: d_streamHolder(output)
, d_state(indentLevel, spacesPerLevel, wrapColumn, basicAllocator)
, d_encoderOptions(basicAllocator)
{
}

Formatter::Formatter(bsl::ostream&     output,
                     int               indentLevel,
                     int               spacesPerLevel,
                     int               wrapColumn,
                     bslma::Allocator *basicAllocator)
: d_streamHolder(&output)
, d_state(indentLevel, spacesPerLevel, wrapColumn, basicAllocator)
, d_encoderOptions(basicAllocator)
{
}

Formatter::Formatter(bsl::streambuf        *output,
                     const EncoderOptions&  encoderOptions,
                     int                    indentLevel,
                     int                    spacesPerLevel,
                     int                    wrapColumn,
                     bslma::Allocator      *basicAllocator)
: d_streamHolder(output)
, d_state(indentLevel, spacesPerLevel, wrapColumn, basicAllocator)
, d_encoderOptions(encoderOptions, basicAllocator)
{
}

Formatter::Formatter(bsl::ostream&          output,
                     const EncoderOptions&  encoderOptions,
                     int                    indentLevel,
                     int                    spacesPerLevel,
                     int                    wrapColumn,
                     bslma::Allocator      *basicAllocator)
: d_streamHolder(&output)
, d_state(indentLevel, spacesPerLevel, wrapColumn, basicAllocator)
, d_encoderOptions(encoderOptions, basicAllocator)
{
}

// MANIPULATORS
void Formatter::addComment(const bsl::string_view& comment, bool forceNewline)
{
    switch (d_state.mode()) {
      case Mode::e_COMPACT: {
        CompactUtil::addComment(*d_streamHolder.stream(),
                                &d_state.compact(),
                                comment,
                                forceNewline);
      } break;
      case Mode::e_PRETTY: {
        PrettyUtil::addComment(*d_streamHolder.stream(),
                               &d_state.pretty(),
                               comment,
                               forceNewline);
      } break;
    }
}

void Formatter::addHeader(const bsl::string_view& encoding)
{
    switch (d_state.mode()) {
      case Mode::e_COMPACT: {
        CompactUtil::addHeader(
            *d_streamHolder.stream(), &d_state.compact(), encoding);
      } break;
      case Mode::e_PRETTY: {
        PrettyUtil::addHeader(
            *d_streamHolder.stream(), &d_state.pretty(), encoding);
      } break;
    }
}

int Formatter::addValidComment(const bsl::string_view& comment,
                               bool                    forceNewline,
                               bool                    omitEnclosingWhitespace)
{
    int result = 0;

    switch (d_state.mode()) {
      case Mode::e_COMPACT: {
        result = CompactUtil::addValidComment(*d_streamHolder.stream(),
                                              &d_state.compact(),
                                              comment,
                                              forceNewline,
                                              omitEnclosingWhitespace);
      } break;
      case Mode::e_PRETTY: {
        result = PrettyUtil::addValidComment(*d_streamHolder.stream(),
                                             &d_state.pretty(),
                                             comment,
                                             forceNewline,
                                             omitEnclosingWhitespace);
      } break;
    }

    return result;
}

void Formatter::closeElement(const bsl::string_view& name)
{
    switch (d_state.mode()) {
      case Mode::e_COMPACT: {
        CompactUtil::closeElement(
            *d_streamHolder.stream(), &d_state.compact(), name);
      } break;
      case Mode::e_PRETTY: {
        PrettyUtil::closeElement(
            *d_streamHolder.stream(), &d_state.pretty(), name);
      } break;
    }
}

void Formatter::openElement(const bsl::string_view& name,
                            WhitespaceType          whitespaceMode)
{
    switch (d_state.mode()) {
      case Mode::e_COMPACT: {
        CompactUtil::openElement(*d_streamHolder.stream(),
                                 &d_state.compact(),
                                 name,
                                 whitespaceMode);
      } break;
      case Mode::e_PRETTY: {
        PrettyUtil::openElement(
            *d_streamHolder.stream(), &d_state.pretty(), name, whitespaceMode);
      } break;
    }
}

void Formatter::reset()
{
    d_streamHolder.stream()->clear();  // Clear error condition(s)

    switch (d_state.mode()) {
      case Mode::e_COMPACT: {
        CompactUtil::reset(&d_state.compact());
      } break;
      case Mode::e_PRETTY: {
        PrettyUtil::reset(&d_state.pretty());
      } break;
    }
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
