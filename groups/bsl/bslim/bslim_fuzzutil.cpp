// bslim_fuzzutil.cpp                                                 -*-C++-*-
#include <bslim_fuzzutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslim_fuzzutil_cpp,"$Id$ $CSID$")


namespace BloombergLP {
namespace bslim {
namespace {
// LOCAL METHODS
template <class CONTAINER>
void consumeRandomLengthCharSequence(CONTAINER    *output,
                                     FuzzDataView *fuzzDataView,
                                     bsl::size_t   maxLength)
    // Load into the specified 'output' a sequence of characters of length from
    // 0 to the specified 'maxLength'.  If the specified 'fuzzDataView' has
    // fewer bytes than 'maxLength', load at most 'fuzzDataView->length()'
    // bytes into 'output'.  If the buffer in 'fuzzDataView' contains two
    // successive backslash characters, then in 'output' they will be converted
    // to a single backslash ('\\') character; if the buffer contains a single
    // backslash character, the construction is terminated, and the following
    // byte, if one is present, will be consumed.  Note that more than
    // 'maxLength' bytes may be consumed from the buffer to produce the
    // 'output'.
{
    bsl::size_t length = bsl::min(maxLength, fuzzDataView->length());

    output->resize(length);

    typename CONTAINER::iterator outIt = output->begin();

    const bsl::uint8_t *end = fuzzDataView->begin() + length;
    const bsl::uint8_t *it  = fuzzDataView->begin();

    const bsl::uint8_t slash = static_cast<bsl::uint8_t>('\\');

    for (; it < end; ++it) {
        if (*it == slash) {
            if (fuzzDataView->end() != end) {
                ++end;
            }
            if (++it == end) {
                break;
            }
            if (*it != slash) {
                ++it;
                break;
            }
        }
        *outIt++ = static_cast<char>(*it);
    }
    output->resize(outIt - output->begin());
    fuzzDataView->removePrefix(it - fuzzDataView->begin());
}
}  // close unnamed namespace


                        // ---------------
                        // struct FuzzUtil
                        // ---------------

void FuzzUtil::consumeRandomLengthChars(bsl::vector<char> *output,
                                        FuzzDataView      *fuzzDataView,
                                        bsl::size_t        maxLength)
{
    consumeRandomLengthCharSequence(output, fuzzDataView, maxLength);
}

void FuzzUtil::consumeRandomLengthChars(std::vector<char> *output,
                                        FuzzDataView      *fuzzDataView,
                                        bsl::size_t        maxLength)
{
    consumeRandomLengthCharSequence(output, fuzzDataView, maxLength);
}
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
void FuzzUtil::consumeRandomLengthChars(std::pmr::vector<char> *output,
                                        FuzzDataView           *fuzzDataView,
                                        bsl::size_t             maxLength)
{
    consumeRandomLengthCharSequence(output, fuzzDataView, maxLength);
}
#endif

void FuzzUtil::consumeRandomLengthString(bsl::string  *output,
                                         FuzzDataView *fuzzDataView,
                                         bsl::size_t   maxLength)
{
    consumeRandomLengthCharSequence(output, fuzzDataView, maxLength);
}

void FuzzUtil::consumeRandomLengthString(std::string  *output,
                                         FuzzDataView *fuzzDataView,
                                         bsl::size_t   maxLength)
{
    consumeRandomLengthCharSequence(output, fuzzDataView, maxLength);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
void FuzzUtil::consumeRandomLengthString(std::pmr::string *output,
                                         FuzzDataView     *fuzzDataView,
                                         bsl::size_t       maxLength)
{
    consumeRandomLengthCharSequence(output, fuzzDataView, maxLength);
}
#endif

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
