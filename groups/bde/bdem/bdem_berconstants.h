// bdem_berconstants.h                  -*-C++-*-
#ifndef INCLUDED_BDEM_BERCONSTANTS
#define INCLUDED_BDEM_BERCONSTANTS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide namespace for storing BER-related constants
//
//@CLASSES:
//     bdem_BerConstants: namespace for BER-related constants
//
//@SEE_ALSO: BER Specification (ITU-T X.690)
//
//@AUTHOR: Rohan Bhindwale (rbhindwa), Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: The 'bdem_BerConstants' 'struct' defined in this component
// defines two enumerations 'TagClass' and 'TagType' that contain symbolic
// constants for the corresponding tag classes and tag types, as defined in the
// BER specification (X.690).
//
///Usage
///-----
// No usage example necessary.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
#undef UNIVERSAL
#undef APPLICATION
#undef CONTEXT_SPECIFIC
#undef PRIVATE
#undef PRIMITIVE
#undef CONSTRUCTED
#endif // BDE_OMIT_INTERNAL_DEPRECATED

namespace BloombergLP {

struct bdem_BerConstants {
    // This 'struct' contains enumerations of the constants used by the BER
    // encoder and decoder.

    enum TagClass {
        // Enumeration of the four possible BER tag classes.

        BDEM_UNIVERSAL         = 0x00,  // the universal tag class
        BDEM_APPLICATION       = 0x40,  // the application tag class
        BDEM_CONTEXT_SPECIFIC  = 0x80,  // the context-specific tag class
        BDEM_PRIVATE           = 0xC0   // the private tag class

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , UNIVERSAL        = BDEM_UNIVERSAL
      , APPLICATION      = BDEM_APPLICATION
      , CONTEXT_SPECIFIC = BDEM_CONTEXT_SPECIFIC
      , PRIVATE          = BDEM_PRIVATE
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    enum TagType {
        // Enumeration of the two possible BER tag types.

        BDEM_PRIMITIVE   = 0x00,  // the primitive tag type
        BDEM_CONSTRUCTED = 0x20   // the constructed tag type

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , PRIMITIVE   = BDEM_PRIMITIVE
      , CONSTRUCTED = BDEM_CONSTRUCTED
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&               stream,
                         bdem_BerConstants::TagClass tagClass);
    // Format the specified 'tagClass' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

bsl::ostream& operator<<(bsl::ostream&              stream,
                         bdem_BerConstants::TagType tagType);
    // Format the specified 'tagType' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
