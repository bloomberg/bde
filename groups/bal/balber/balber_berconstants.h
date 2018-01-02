// balber_berconstants.h                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALBER_BERCONSTANTS
#define INCLUDED_BALBER_BERCONSTANTS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide namespace for BER-related constants.
//
//@CLASSES:
//  balber::BerConstants: namespace for BER-related constants
//
//@SEE_ALSO: BER Specification (ITU-T X.690)
//
//@DESCRIPTION: The 'balber::BerConstants' 'struct' defined in this component
// defines two enumerations, 'balber::TagClass' and 'balber::TagType', that
// contain symbolic constants for the corresponding tag classes and tag types,
// as defined in the BER specification (X.690).
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif


namespace BloombergLP {
namespace balber {

                            // ===================
                            // struct BerConstants
                            // ===================

struct BerConstants {
    // This 'struct' contains enumerations of the constants used by the BER
    // encoder and decoder.

    enum TagClass {
        // Enumeration of the four possible BER tag classes.

        e_UNIVERSAL         = 0x00  // the universal tag class
      , e_APPLICATION       = 0x40  // the application tag class
      , e_CONTEXT_SPECIFIC  = 0x80  // the context-specific tag class
      , e_PRIVATE           = 0xC0  // the private tag class

    };

    enum TagType {
        // Enumeration of the two possible BER tag types.

        e_PRIMITIVE   = 0x00  // the primitive tag type
      , e_CONSTRUCTED = 0x20  // the constructed tag type

    };
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&          stream,
                         BerConstants::TagClass tagClass);
    // Format the specified 'tagClass' to the specified output 'stream' and
    // return a reference providing modifiable access to 'stream'.

bsl::ostream& operator<<(bsl::ostream&         stream,
                         BerConstants::TagType tagType);
    // Format the specified 'tagType' to the specified output 'stream' and
    // return a reference providing modifiable access to 'stream'.

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
