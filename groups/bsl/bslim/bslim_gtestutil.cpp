// bslim_gtestutil.cpp                                                -*-C++-*-

#include <bslim_gtestutil.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsl_c_ctype.h>    // 'isprint'

// FREE OPERATORS
void bsl::PrintTo(const bsl::string& value, bsl::ostream *stream)
{
    *stream << '"' << value << '"' << bsl::flush;
}

void bsl::PrintTo(const bsl::wstring& value, bsl::ostream *stream)
{
    BSLMF_ASSERT((bsl::is_same<wchar_t, bsl::wstring::value_type>::value));
    BSLMF_ASSERT(sizeof(unsigned int) >= sizeof(wchar_t));

    *stream << '"';

    const bsl::wstring::const_iterator end = value.cend();
    for  (bsl::wstring::const_iterator it  = value.cbegin(); it < end; ++it) {
        const wchar_t currentWChar = *it;

        char c = 0;
        if (currentWChar > 0 && currentWChar < 128) {
            c = static_cast<char>(currentWChar);
            if (!isprint(c)) {
                c = 0;
            }
        }
        if (c) {
            *stream << c;
        }
        else {
            enum {
                k_NUM_NIBBLES = sizeof(wchar_t) * 2,
                k_NIBBLE_MASK = 0xf
            };

            *stream << "\\x";
            for (int shift = 4 * (k_NUM_NIBBLES - 1); 0 <= shift; shift -= 4) {
                unsigned int ic = (currentWChar >> shift) & k_NIBBLE_MASK;
                BSLS_ASSERT(ic < 16);

                ic = ic >= 10 ? 'a' + (ic - 10)
                              : '0' + ic;

                *stream << static_cast<char>(ic);
            }
        }
    }

    *stream << '"' << bsl::flush;
}

namespace BloombergLP {
namespace bslim {

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
