// bslim_gtestutil.cpp                                                -*-C++-*-

#include <bslim_gtestutil.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsl_cctype.h>    // 'isprint'

// ----------------------------------------------------------------------------
//                              IMPLEMENTATION NOTE
//
// Suppose we start with the implementation of
// 'bsl::PrintTo(bsl::optional<TYPE>, ...)' as
//..
//  if (value.has_value()) {
//      PrintTo(*value, stream);
//  }
//..
// This doesn't work.  Some types, such as fundamental types, are not defined
// in the 'testing' namespace, but the 'PrintTo' functions for them are.  For
// that reason, ADL wasn't finding the 'PrintTo' for them.  Trying:
//..
//  if (value.has_value()) {
//      testing::PrintTo(*value, stream);
//  }
//..
// was unacceptable, because 'TYPE' may be some type defined in a namespace
// other than 'testing', where the 'PrintTo' is also defined in that other
// namespace, so it won't be found.
//
// We tried:
//..
//  if (value.has_value()) {
//      using namespace testing;
//      PrintTo(*value, stream);
//  }
//..
// Unfortunately, when looking for matches to 'PrintTo', the compiler first
// looks for any 'PrintTo' in each enclosing namespace within which the call
// occurs, and when it finds 'bsl::PrintTo(bsl::optional<TYPE>, ...)' it quits
// looking, before examining the global namespace into which the using
// directive would bring the names from the 'testing' namespace in this case.
// There were possible ways around this, but additionally, the
// 'using namespace testing' only draws in declarations made prior to the
// 'using' statement, and an important requirement of BDE programming is that
// includes can occur in any order.  We also tried:
//..
//  if (value.has_value()) {
//      using testing::PrintTo;
//      PrintTo(*value, stream);
//  }
//..
// But again, this only draws 'testing::PrintTo's that have previously been
// declared into consideration.
//
// If, earlier in 'bslim_gtestutil.h', we do a template forward declare:
//..
//  namespace testing {
//  template <class TYPE>
//  void PrintTo(const TYPE&, stream *);
//  }  // close namespace testing
//..
// and then do a 'using testing::PrintTo', the forward template declaration
// only declares the template function, it does not forward declare
// non-template overloads declared after the 'using', and thus, the 'using'
// fails to draw them in.
//
// The solution is, when the call is made, do:
//..
//  if (value.has_value()) {
//      PrintTo(*value, testing::bslim_Gtestutil_TestingStreamHolder(stream));
//  }
//..
// and the stream holder being in the 'testing' namespace will draw in all the
// 'PrintTo's defined in 'testing' through ADL, and '*value' will draw in any
// 'PrintTo's defined the namespace of 'TYPE', also through ADL.  The stream
// holder will then implicitly convert to 'stream *'.
// ----------------------------------------------------------------------------

// FREE OPERATORS
void bsl::PrintTo(const bsl::string& value, bsl::ostream *stream)
{
    *stream << '"' << value << '"';
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
            if (!bsl::isprint(c)) {
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

    *stream << '"';
}

void bsl::PrintTo(const BloombergLP::bslstl::StringRef&  value,
                  bsl::ostream                          *stream)
{
    *stream << '"' << value << '"';
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
