// bslim_gtestutil.cpp                                                -*-C++-*-

#include <bslim_gtestutil.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsl_c_ctype.h>    // 'isprint'

// FREE OPERATORS
void bsl::PrintTo(const bsl::string&  str,
                  bsl::ostream       *os_p)
{
    *os_p << '"' << str << '"' << bsl::flush;
}

void bsl::PrintTo(const bsl::wstring&  str,
                  bsl::ostream        *os_p)
{
    BSLMF_ASSERT((bsl::is_same<wchar_t, bsl::wstring::value_type>::value));
    BSLMF_ASSERT(sizeof(unsigned int) >= sizeof(wchar_t));

    *os_p << '"';

    const bsl::wstring::const_iterator end = str.cend();
    for  (bsl::wstring::const_iterator it  = str.cbegin(); it < end; ++it) {
        const wchar_t cur = *it;

        char c = 0;
        if (cur > 0 && cur < 128) {
            c = static_cast<char>(cur);
            if (!isprint(c)) {
                c = 0;
            }
        }
        if (c) {
            *os_p << c;
        }
        else {
            enum { k_NUM_NYBBLES = sizeof(wchar_t) * 2,
                   k_NYBBLE_MASK = 0xf };

            *os_p << "\\x";
            for (int shift = (k_NUM_NYBBLES - 1) * 4; 0 <= shift; shift -= 4) {
                unsigned int ic =   k_NYBBLE_MASK << shift;
                ic              &=  cur;
                ic              >>= shift;
                BSLS_ASSERT(ic < 16);

                ic = ic >= 10 ? 'a' + (ic - 10)
                              : '0' + ic;

                *os_p << static_cast<char>(ic);
            }
        }
    }

    *os_p << '"' << bsl::flush;
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
