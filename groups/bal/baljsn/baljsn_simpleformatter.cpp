// baljsn_simpleformatter.cpp                                         -*-C++-*-
#include <baljsn_simpleformatter.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_simpleformatter_cpp,"$Id$ $CSID$")

#include <baljsn_encoderoptions.h>
#include <baljsn_printutil.h>

namespace BloombergLP {
namespace baljsn {

                          // ---------------
                          // class SimpleFormatter
                          // ---------------

// CREATORS
SimpleFormatter::SimpleFormatter(bsl::ostream&     stream,
                                 bslma::Allocator *basicAllocator)
: d_outputStream(stream)
, d_useComma(false)
, d_started(false)
, d_memberNameSupplied(false)
, d_callSequence(basicAllocator)
, d_indentLevel(d_encoderOptions.initialIndentLevel())
{
    // Add a dummy value so we don't have to check whether 'd_callSequence' is
    // empty in 'openObject' when we access its last element.

    d_callSequence.append(false);
}

SimpleFormatter::SimpleFormatter(bsl::ostream&          stream,
                                 const EncoderOptions&  encoderOptions,
                                 bslma::Allocator      *basicAllocator)
: d_outputStream(stream)
, d_useComma(false)
, d_started(false)
, d_memberNameSupplied(false)
, d_callSequence(basicAllocator)
, d_encoderOptions(encoderOptions)
, d_indentLevel(d_encoderOptions.initialIndentLevel())
{
    // Add a dummy value so we don't have to check whether 'd_callSequence' is
    // empty in 'openObject' when we access its last element.

    d_callSequence.append(false);
}

SimpleFormatter::SimpleFormatter(const SimpleFormatter&  original,
                                 bslma::Allocator       *basicAllocator)
: d_outputStream(original.d_outputStream)
, d_useComma(original.d_useComma)
, d_started(original.d_started)
, d_memberNameSupplied(original.d_memberNameSupplied)
, d_callSequence(original.d_callSequence, basicAllocator)
, d_indentLevel(original.d_indentLevel)
{
}

SimpleFormatter::~SimpleFormatter()
{
    // NOTE: Do not assert that we've balanced all open/close calls, in case an
    // exception was thrown during processing and we're being destroyed
    // mid-formatting.
}

// MANIPULATORS
void SimpleFormatter::closeArray(ArrayFormattingStyle formattingStyle)
{
    BSLS_ASSERT(isFormattingArray());
    BSLS_ASSERT_SAFE(!isFormattingObject());

    if (usePrettyStyle() && e_REGULAR_ARRAY_FORMAT == formattingStyle) {
        --d_indentLevel;

        // 'd_useComma' will be 'true' if there's an element in the array, and
        // 'false' otherwise.
        if (d_useComma) {
            d_outputStream << '\n';
        }

        indent();
    }
    d_callSequence.remove(d_callSequence.length() - 1);

    d_useComma = true;

    d_outputStream << ']';
}

void SimpleFormatter::closeObject()
{
    BSLS_ASSERT(isNameNeeded());
    BSLS_ASSERT_SAFE(!isFormattingArray());
    if (usePrettyStyle()) {
        --d_indentLevel;

        // 'd_useComma' will be 'true' if there's an element in the object, and
        // 'false' otherwise.
        if (d_useComma) {
            d_outputStream << '\n';
        }

        indent();
    }
    d_callSequence.remove(d_callSequence.length() - 1);

    d_useComma = true;

    d_outputStream << '}';
}

void SimpleFormatter::openArray(ArrayFormattingStyle formattingStyle)
{
    BSLS_ASSERT(!isNameNeeded());

    bool needIndent = usePrettyStyle() && !d_memberNameSupplied;

    printComma();
    followWithComma(false);

    if (needIndent) {
        indent();
    }

    d_outputStream << '[';

    if (usePrettyStyle() && e_REGULAR_ARRAY_FORMAT == formattingStyle) {
        d_outputStream << '\n';
        ++d_indentLevel;
    }
    d_callSequence.append(true);

    BSLS_ASSERT_SAFE(isFormattingArray());
    BSLS_ASSERT_SAFE(!isFormattingObject());
}

void SimpleFormatter::openArray(const bslstl::StringRef& name,
                                ArrayFormattingStyle     formattingStyle)
{
    BSLS_ASSERT(isNameNeeded());

    printComma();
    followWithComma(false);

    printName(name);

    d_outputStream << '[';

    if (usePrettyStyle() && e_REGULAR_ARRAY_FORMAT == formattingStyle) {
        d_outputStream << '\n';
        ++d_indentLevel;
    }
    d_callSequence.append(true);

    BSLS_ASSERT_SAFE(isFormattingArray());
    BSLS_ASSERT_SAFE(!isFormattingObject());
}

void SimpleFormatter::openObject()
{
    BSLS_ASSERT(!isNameNeeded());

    bool needIndent = usePrettyStyle() && !d_memberNameSupplied;

    printComma();
    followWithComma(false);

    if (needIndent) {
        indent();
    }

    d_outputStream << '{';

    if (usePrettyStyle()) {
        d_outputStream << '\n';
        ++d_indentLevel;
    }
    d_callSequence.append(false);

    BSLS_ASSERT_SAFE(!isFormattingArray());
    BSLS_ASSERT_SAFE(isFormattingObject());
}

void SimpleFormatter::openObject(const bslstl::StringRef& name)
{
    BSLS_ASSERT(isNameNeeded());

    printComma();
    followWithComma(false);

    printName(name);

    d_outputStream << '{';

    if (usePrettyStyle()) {
        d_outputStream << '\n';
        ++d_indentLevel;
    }
    d_callSequence.append(false);

    BSLS_ASSERT_SAFE(!isFormattingArray());
    BSLS_ASSERT_SAFE(isFormattingObject());
}

}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
