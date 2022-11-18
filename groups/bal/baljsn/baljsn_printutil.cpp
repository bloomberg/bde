// baljsn_printutil.cpp                                               -*-C++-*-
#include <baljsn_printutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_printutil_cpp,"$Id$ $CSID$")

#include <bdlde_base64encoder.h>
#include <bdlde_utf8util.h>

#include <bsls_annotation.h>

#include <bsl_sstream.h>

namespace BloombergLP {
namespace baljsn {

                              // ---------------
                              // class PrintUtil
                              // ---------------

int PrintUtil::printValue(bsl::ostream&         stream,
                          bdldfp::Decimal64     value,
                          const EncoderOptions *options)
{
    switch (bdldfp::DecimalUtil::classify(value)) {
      case FP_INFINITE: {
        if (options && options->encodeInfAndNaNAsStrings()) {
            stream <<
                  (value == bsl::numeric_limits<bdldfp::Decimal64>::infinity()
                  ? "\"+inf\""
                  : "\"-inf\"");
        }
        else {
            return -1;                                                // RETURN
        }
      } break;
      case FP_NAN: {
        if (options && options->encodeInfAndNaNAsStrings()) {
            stream << "\"nan\"";
        }
        else {
            return -1;                                                // RETURN
        }
      } break;
      default: {
        if (options && options->encodeQuotedDecimal64()) {
            stream.put('"');
            stream << value;
            stream.put('"');
        }
        else {
            stream << value;
        }
        if (stream.bad()) {
            return -1;                                                // RETURN
        }
      }
    }
    return 0;
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
