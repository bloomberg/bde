// bdem_berconstants.cpp                  -*-C++-*-
#include <bdem_berconstants.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_berconstants_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {

// FREE OPERATORS

bsl::ostream& operator<<(bsl::ostream&               stream,
                         bdem_BerConstants::TagClass tagClass)
{
    switch (tagClass) {
      case bdem_BerConstants::BDEM_UNIVERSAL:
          return stream << "UNIVERSAL";
      case bdem_BerConstants::BDEM_APPLICATION:
          return stream << "APPLICATION";
      case bdem_BerConstants::BDEM_CONTEXT_SPECIFIC:
          return stream << "CONTEXT_SPECIFIC";
      case bdem_BerConstants::BDEM_PRIVATE:
          return stream << "PRIVATE";
    }

    BSLS_ASSERT(!"encountered out-of-bound enumerated value");

    return stream;
}

bsl::ostream& operator<<(bsl::ostream&              stream,
                         bdem_BerConstants::TagType tagType)
{
    switch (tagType) {
      case bdem_BerConstants::BDEM_PRIMITIVE:
          return stream << "PRIMITIVE";
      case bdem_BerConstants::BDEM_CONSTRUCTED:
          return stream << "CONSTRUCTED";
    }

    BSLS_ASSERT(!"encountered out-of-bound enumerated value");

    return stream;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
