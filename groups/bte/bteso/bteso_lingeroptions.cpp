// bteso_lingeroptions.cpp                                            -*-C++-*-
#include <bteso_lingeroptions.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bteso_lingeroptions_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ios.h>
#include <bsl_ostream.h>

#include <bsls_assert.h>


namespace BloombergLP {

                       // -------------------------
                       // class bteso_LingerOptions
                       // -------------------------

// CREATORS
bteso_LingerOptions::bteso_LingerOptions()
: d_timeout(0)
, d_useLingeringFlag(false)
{
}

bteso_LingerOptions::bteso_LingerOptions(int timeout, bool useLingeringFlag)
: d_timeout(timeout)
, d_useLingeringFlag(useLingeringFlag)
{
}

bteso_LingerOptions::bteso_LingerOptions(const bteso_LingerOptions& original)
: d_timeout(original.d_timeout)
, d_useLingeringFlag(original.d_useLingeringFlag)
{
}

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
bteso_LingerOptions::~bteso_LingerOptions()
{
    BSLS_ASSERT_SAFE(0 <= d_timeout);
}
#endif

// MANIPULATORS
void bteso_LingerOptions::reset()
{
    d_timeout          = 0;
    d_useLingeringFlag = 0;
}

// ACCESSORS
                                  // Aspects

bsl::ostream& bteso_LingerOptions::print(bsl::ostream& stream,
                                         int           level,
                                         int           spacesPerLevel) const
{
    const bsl::ios_base::fmtflags fmtFlags = stream.flags();
    stream << bsl::boolalpha;

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.print(d_timeout,          "timeout");
    printer.print(d_useLingeringFlag, "useLingeringFlag");
    printer.end();

    stream.flags(fmtFlags);

    return stream;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&              stream,
                         const bteso_LingerOptions& object)
{
    const bsl::ios_base::fmtflags fmtFlags = stream.flags();
    stream << bsl::boolalpha;

    bslim::Printer printer(&stream, 0, -1);
    printer.start();
    printer.print(object.timeout(),          0);
    printer.print(object.useLingeringFlag(), 0);
    printer.end();

    stream.flags(fmtFlags);

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
