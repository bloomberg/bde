// balber_berconstants.cpp                  -*-C++-*-
#include <balber_berconstants.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balber_berconstants_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {

// FREE OPERATORS

bsl::ostream& balber::operator<<(bsl::ostream&          stream,
                                 BerConstants::TagClass tagClass)
{
    switch (tagClass) {
      case BerConstants::e_UNIVERSAL:
          return stream << "UNIVERSAL";
      case BerConstants::e_APPLICATION:
          return stream << "APPLICATION";
      case BerConstants::e_CONTEXT_SPECIFIC:
          return stream << "CONTEXT_SPECIFIC";
      case BerConstants::e_PRIVATE:
          return stream << "PRIVATE";
    }

    BSLS_ASSERT(!"encountered out-of-bound enumerated value");

    return stream;
}

bsl::ostream& balber::operator<<(bsl::ostream&         stream,
                                 BerConstants::TagType tagType)
{
    switch (tagType) {
      case BerConstants::e_BDEM_PRIMITIVE:
          return stream << "PRIMITIVE";
      case BerConstants::e_BDEM_CONSTRUCTED:
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
