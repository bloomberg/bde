// bteso_lingeroptions.cpp                                            -*-C++-*-

#include <bdes_ident.h>
BDES_IDENT_RCSID(bteso_lingeroptions_cpp,"$Id$ $CSID$")

#include <bteso_lingeroptions.h>

#include <bdeat_valuetypefunctions.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>

#include <bsls_assert.h>

#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>

namespace BloombergLP {

                       // -------------------------
                       // class bteso_LingerOptions
                       // -------------------------

// CREATORS
bteso_LingerOptions::bteso_LingerOptions()
: d_timeout(0)
, d_useLingering(false)
{
}

bteso_LingerOptions::bteso_LingerOptions(const bteso_LingerOptions& original)
: d_timeout(original.d_timeout)
, d_useLingering(original.d_useLingering)
{
}

bteso_LingerOptions::~bteso_LingerOptions()
{
}

// MANIPULATORS
bteso_LingerOptions&
bteso_LingerOptions::operator=(const bteso_LingerOptions& rhs)
{
    if (this != &rhs) {
        d_useLingering = rhs.d_useLingering;
        d_timeout      = rhs.d_timeout;
    }
    return *this;
}

void bteso_LingerOptions::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_timeout);
    bdeat_ValueTypeFunctions::reset(&d_useLingering);
}

// ACCESSORS
bsl::ostream& bteso_LingerOptions::print(bsl::ostream& stream,
                                         int           level,
                                         int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "UseLingering = ";
        bdeu_PrintMethods::print(stream, d_useLingering,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Timeout = ";
        bdeu_PrintMethods::print(stream, d_timeout,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "UseLingering = ";
        bdeu_PrintMethods::print(stream, d_useLingering,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Timeout = ";
        bdeu_PrintMethods::print(stream, d_timeout,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream;
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
