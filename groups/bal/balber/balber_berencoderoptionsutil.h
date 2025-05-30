// balber_berencoderoptionsutil.h                                     -*-C++-*-
#ifndef INCLUDED_BALBER_BERENCODEROPTIONSUTIL
#define INCLUDED_BALBER_BERENCODEROPTIONSUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility for configuring `balber::BerEncoderOptions`.
//
//@CLASSES:
//  balber::BerEncoderOptionsUtil: utility for setting `BerEncoderOptions`
//
//@SEE_ALSO: balber_berencoder, balber_berencoderoptions
//
//@DESCRIPTION: This component provides a `struct` of utility functions,
// `balber::BerEncoderOptionsUtil`, for configuring `balber::BerEncoderOptions`
// object.  In particular, this utility can be used to set the recommended
// options needed for encoding BER messages that are time-efficient to decode.
// This utility can also be used to set a `balber::BerEncoderOptions` object to
// its default state.
//
///Modes
///-----
// This utility defines the mode `balber::BerEncoderOptionsUtil::e_DEFAULT`
// that sets the attributes to the values used in
// `balber::BerEncoderOptions()`.
//
// This utility defines the mode
// `balber::BerEncoderOptionsUtil::e_FAST_20250615` that applies the default
// attribute values except for the following overrides:
// ```
// encodeDateAndTimeTypesAsBinary     true
// encodeArrayLengthHints             true
// ```
// WARNING: It is only safe to use this mode when communicating with a balber
// BER decoder built after June 15, 2025.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Setting `balber::EncoderOptions` for Efficiency
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Every call to one of the `encode` functions of `balber::BerEncoder` requires
// the user to provide a `balber::BerEncoderOptions` object.  The `setMode`
// function of this utility provides a convenient way to set the option
// attributes to a combination that is deemed efficient for future decoding of
// the message.
//
// First, create a `balber::BerEncoderOptions` object:
// ```
// balber::BerEncoderOptions options;
// ```
// Now, set the option values for efficiency:
// ```
// balber::BerEncoderOptionsUtil::setMode(
//                             &options,
//                             balber::BerEncoderOptionsUtil::e_FAST_20250615);
// ```
// Finally, `options` can be adjusted to the set of default attributes:
// ```
// balber::BerEncoderOptionsUtil::setMode(
//                                   &options,
//                                   balber::BerEncoderOptionsUtil::e_DEFAULT);
// ```

#include <balscm_version.h>

namespace BloombergLP {
namespace balber {

class BerEncoderOptions;

                            // ===========================
                            // class BerEncoderOptionsUtil
                            // ===========================

/// This `struct` provides a namespace for functions that set
/// `BerEncoderOptions` to particular configurations.
struct BerEncoderOptionsUtil {

  public:
    // TYPES
    enum Mode {
        e_DEFAULT       = 0  // set to default state
      , e_FAST_20250615 = 1  // set for efficient decoding of encoded data
    };

    // CLASS METHODS

    /// Set the attributes of the specified `options` to the configuration
    /// associated with the specified `mode`.  See {Modes} for details.
    static void setMode(BerEncoderOptions *options, Mode mode);
};

}  // close package namespace
}  // close enterprise namespace

#endif

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
