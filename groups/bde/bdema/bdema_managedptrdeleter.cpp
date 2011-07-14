// bdema_managedptr.cpp                                               -*-C++-*-
#include <bdema_managedptr.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdema_managedptrdeleter_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_ios.h>
#include <bsl_ostream.h>

#include <bsls_assert.h>


namespace BloombergLP {

                       // -------------------------
                       // class bdema_ManagedPtrDeleter
                       // -------------------------

// CREATORS
//bdema_ManagedPtrDeleter::bdema_ManagedPtrDeleter(int timeout, bool useLingeringFlag)
//: d_timeout(timeout)
//, d_useLingeringFlag(useLingeringFlag)
//{
//}

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
bdema_ManagedPtrDeleter::~bdema_ManagedPtrDeleter()
{
    // actually, we have no detectable constraint vioalations to test for.
}
#endif

// MANIPULATORS

// ACCESSORS
                                  // Aspects

bsl::ostream& bdema_ManagedPtrDeleter::print(bsl::ostream& stream,
                                         int           level,
                                         int           spacesPerLevel) const
{
    const bsl::ios_base::fmtflags fmtFlags = stream.flags();
    stream << bsl::boolalpha;

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.print(d_object_p,  "object" );
    printer.print(d_factory_p, "factory");
    printer.print(d_deleter,   "deleter");
    printer.end();

    stream.flags(fmtFlags);

    return stream;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&              stream,
                         const bdema_ManagedPtrDeleter& object)
{
    const bsl::ios_base::fmtflags fmtFlags = stream.flags();
    stream << bsl::boolalpha;

    bslim::Printer printer(&stream, 0, -1);
    printer.start();
    printer.print(object.object(),  0);
    printer.print(object.factory(), 0);
    printer.print(object.deleter(), 0);
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
