// bslfmt_print_ostream_imp.cpp                                       -*-C++-*-
#include <bslfmt_print_ostream_imp.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslfmt_print_ostream_imp_cpp,"$Id$ $CSID$")

namespace BloombergLP {
namespace bslfmt {

void vprint_nonunicode(std::ostream&    stream,
                       bsl::string_view fmt,
                       bsl::format_args args)
{
    using std::ios_base;
    std::ostream::sentry ok(stream);
    if (ok)
    {
        bsl::string out = bsl::vformat(
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT
                                       stream.getloc(),
#endif
                                                        fmt, args);
        try {
            using std::streamsize;
            streamsize toWrite = static_cast<streamsize>(out.length());
            if (stream.rdbuf()->sputn(out.data(), toWrite) != toWrite) {
                stream.setstate(ios_base::badbit);
            }
        }
        catch(...) {
            stream.setstate(ios_base::badbit);
        }
    }
}

void vprint_unicode(std::ostream&    stream,
                    bsl::string_view fmt,
                    bsl::format_args args)
{
    bslfmt::vprint_nonunicode(stream, fmt, args);
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
