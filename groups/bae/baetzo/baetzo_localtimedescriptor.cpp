// baetzo_localtimedescriptor.cpp                                     -*-C++-*-
#include <baetzo_localtimedescriptor.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baetzo_localtimedescriptor_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ios.h>
#include <bsl_ostream.h>

namespace BloombergLP {

                        // --------------------------------
                        // class baetzo_LocalTimeDescriptor
                        // --------------------------------

// ACCESSORS
bsl::ostream& baetzo_LocalTimeDescriptor::print(
                                            bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    const bsl::ios_base::fmtflags fmtFlags = stream.flags();
    stream << bsl::boolalpha;

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.print(d_utcOffsetInSeconds,  "utcOffsetInSeconds");
    printer.print(d_dstInEffectFlag,     "dstInEffectFlag");
    printer.print(d_description.c_str(), "description");
    printer.end();

    stream.flags(fmtFlags);

    return stream;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                     stream,
                         const baetzo_LocalTimeDescriptor& object)
{
    const bsl::ios_base::fmtflags fmtFlags = stream.flags();
    stream << bsl::boolalpha;

    bslim::Printer printer(&stream, 0, -1);
    printer.start();
    printer.print(object.utcOffsetInSeconds(),  0);
    printer.print(object.dstInEffectFlag(),     0);
    printer.print(object.description().c_str(), 0);
    printer.end();

    stream.flags(fmtFlags);

    return stream;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
