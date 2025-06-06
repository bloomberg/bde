// balber_berconstants.h                                              -*-C++-*-
#ifndef INCLUDED_BALBER_BERCONSTANTS
#define INCLUDED_BALBER_BERCONSTANTS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide namespace for BER-related constants.
//
//@CLASSES:
//  balber::BerConstants: namespace for BER-related constants
//
//@SEE_ALSO: BER Specification (ITU-T X.690)
//
//@DESCRIPTION: The `balber::BerConstants` `struct` defined in this component
// defines two enumerations, `balber::TagClass` and `balber::TagType`, that
// contain symbolic constants for the corresponding tag classes and tag types,
// as defined in the BER specification (X.690).  Also defined are constants
// used for encoding and decoding, such as the tag number for the array length
// hint.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// TBD

#include <balscm_version.h>

#include <bsl_iosfwd.h>

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
#undef UNIVERSAL
#undef APPLICATION
#undef CONTEXT_SPECIFIC
#undef PRIVATE
#undef PRIMITIVE
#undef CONSTRUCTED
#endif // BDE_OMIT_INTERNAL_DEPRECATED

namespace BloombergLP {
namespace balber {

                            // ===================
                            // struct BerConstants
                            // ===================

/// This `struct` contains enumerations of the constants used by the BER
/// encoder and decoder.
struct BerConstants {

    enum TagClass {
        // Enumeration of the four possible BER tag classes.

        e_UNIVERSAL         = 0x00  // the universal tag class
      , e_APPLICATION       = 0x40  // the application tag class
      , e_CONTEXT_SPECIFIC  = 0x80  // the context-specific tag class
      , e_PRIVATE           = 0xC0  // the private tag class

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , BDEM_UNIVERSAL         = e_UNIVERSAL
      , BDEM_APPLICATION       = e_APPLICATION
      , BDEM_CONTEXT_SPECIFIC  = e_CONTEXT_SPECIFIC
      , BDEM_PRIVATE           = e_PRIVATE

      , UNIVERSAL        = e_UNIVERSAL
      , APPLICATION      = e_APPLICATION
      , CONTEXT_SPECIFIC = e_CONTEXT_SPECIFIC
      , PRIVATE          = e_PRIVATE
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    enum TagType {
        // Enumeration of the two possible BER tag types.

        e_PRIMITIVE   = 0x00  // the primitive tag type
      , e_CONSTRUCTED = 0x20  // the constructed tag type

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , BDEM_PRIMITIVE   = e_PRIMITIVE
      , BDEM_CONSTRUCTED = e_CONSTRUCTED

      , PRIMITIVE   = e_PRIMITIVE
      , CONSTRUCTED = e_CONSTRUCTED
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    static const int k_ARRAY_LENGTH_HINT_TAG_NUMBER = 0x7fffffff;
};

// FREE OPERATORS

/// Format the specified `tagClass` to the specified output `stream` and
/// return a reference providing modifiable access to `stream`.
bsl::ostream& operator<<(bsl::ostream&          stream,
                         BerConstants::TagClass tagClass);

/// Format the specified `tagType` to the specified output `stream` and
/// return a reference providing modifiable access to `stream`.
bsl::ostream& operator<<(bsl::ostream&         stream,
                         BerConstants::TagType tagType);

}  // close package namespace
}  // close enterprise namespace
#endif

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
