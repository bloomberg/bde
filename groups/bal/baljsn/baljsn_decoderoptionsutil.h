// baljsn_decoderoptionsutil.h                                        -*-C++-*-
#ifndef INCLUDED_BALJSN_DECODEROPTIONSUTIL
#define INCLUDED_BALJSN_DECODEROPTIONSUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility for configuring `baljsn::DecoderOptions`.
//
//@CLASSES:
//  baljsn::DecoderOptionsUtil: utility for setting `baljsn::DecoderOptions`
//
//@SEE_ALSO: baljsn_decoder, baljsn_decoderoptions
//
//@DESCRIPTION: This component provides a `struct` of utility functions,
// `baljsn::DecoderOptionsUtil`, for configuring `baljsn::DecoderOptions`
// object.  In particular, this utility can be used to set the combination of
// options needed for strict compliance with the JSON grammar (see
// [](baljsn_decoder#Strict Conformance)).  This utility can also be used to
// set a `baljsn::DecoderOptions` object to its default state.
//
///Modes
///-----
// When a default constructed `baljsn::DecoderOptions` object is passed to the
// `decode` methods of a `baljsn::Decoder`, several convenient variances from
// the JSON grammar are tolerated in the JSON document without causing failure.
// Specifically:
// ```
// validateInputIsUtf8              false
// allowConsecutiveSeparators       true
// allowFormFeedAsWhitespace        true
// allowUnescapedControlCharacters  true
// ```
// See [](baljsn_decoderoptions#Attributes) for examples.  Should any of these
// variances be unacceptable, then one can flip individual options.  Strict
// compliance (see `bdljsn_jsontestsuiteutil`) with the JSON grammar requires
// that each option named above be flipped to the opposite value:
// ```
// validateInputIsUtf8              true
// allowConsecutiveSeparators       false
// allowFormFeedAsWhitespace        false
// allowUnescapedControlCharacters  false
// ```
// This utility defines the mode
// `baljsn::DecoderOptionsUtil::e_STRICT_20240423` to allow all four options to
// be set with a single call.
//
// Note that `baljsn::DecoderOptions` defines other options besides the four
// cited above.  Those are *not* changed by setting the
// `baljsn::DecoderOptionsUtil::e_STRICT_20240423` combination but are set when
// setting the options object using `baljsn::DecoderOptionsUtil::e_DEFAULT`.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Setting `baljsn::DecoderOptions` for Strictness
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Every call to one of the (non-deprecated) `decode` functions of
// `baljsn::Decoder` requires the user to provide a `baljsn::DecoderOptions`
// object that allows the user to fine-tune the rules used when decoding the
// JSON document.  The `setMode` function of this utility provides a convenient
// way to set the option attributes to a combination that is deemed "strict"
// (i.e., strictly complying with the rules of the JSON grammar).
//
// First, create a `baljsn::DecoderOptions` object:
// ```
// baljsn::DecoderOptions options;
// ```
// Now, set the option values for strict compliance:
// ```
// baljsn::DecoderOptionsUtil::setMode(
//                             &options,
//                             baljsn::DecoderOptionsUtil::e_STRICT_20240423);
// ```
// Finally, should there be a need, `options` can be adjusted to a laxer set of
// rules by adjusting individual attributes or, if the original set of default
// attributes is needed, by using `setMode`:
// ```
// baljsn::DecoderOptionsUtil::setMode(&options,
//                                     baljsn::DecoderOptionsUtil::e_DEFAULT);
// ```

#include <balscm_version.h>

namespace BloombergLP {
namespace baljsn {

class DecoderOptions;

                              // ========================
                              // class DecoderOptionsUtil
                              // ========================

/// This `struct` provides a namespace for functions that set
/// `DecoderOptions` to particular configurations.
struct DecoderOptionsUtil {

  public:
    // TYPES
    enum Mode {
        e_DEFAULT         = 0  // set to default state
      , e_STRICT_20240423 = 1  // set for strictness conformance per 2024-04-23
    };

    // CLASS METHODS

    /// Set the attributes of the specified `options` to the configuration
    /// associated with the specified `mode`.  See {Modes} for details.
    static void setMode(DecoderOptions *options, Mode mode);
};

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
