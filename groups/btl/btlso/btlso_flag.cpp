// btlso_flag.cpp            -*-C++-*-
#include <btlso_flag.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_flag_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

const char *bteso_Flag::toAscii(bteso_Flag::Flag value)
{
    switch (value) {
        case k_ASYNC_INTERRUPT: return "ASYNC_INTERRUPT";
        default: return "(* UNKNOWN *)";
    }
}

const char *bteso_Flag::toAscii(bteso_Flag::BlockingMode value)
{
    switch (value) {
        case e_BLOCKING_MODE: return "BLOCKING_MODE";
        case e_NONBLOCKING_MODE: return "NONBLOCKING_MODE";
        default: return "(* UNKNOWN *)";
    }
}

const char *bteso_Flag::toAscii(bteso_Flag::ShutdownType value)
{
    switch (value) {
        case e_SHUTDOWN_RECEIVE: return "SHUTDOWN_RECEIVE";
        case e_SHUTDOWN_SEND: return "SHUTDOWN_SEND";
        case e_SHUTDOWN_BOTH: return "SHUTDOWN_BOTH";
        default: return "(* UNKNOWN *)";
    }
}

const char *bteso_Flag::toAscii(bteso_Flag::IOWaitType value)
{
    switch (value) {
        case e_IO_READ: return "IO_READ";
        case e_IO_WRITE: return "IO_WRITE";
        case e_IO_RW: return "IO_RW";
        default: return "(* UNKNOWN *)";
    }
}

bsl::ostream& bteso_Flag::streamOut(bsl::ostream&    stream,
                                    bteso_Flag::Flag rhs)
{
    return stream << bteso_Flag::toAscii(rhs);
}

bsl::ostream&  bteso_Flag::streamOut(bsl::ostream& stream,
                                     bteso_Flag::BlockingMode rhs)
{
    return stream << bteso_Flag::toAscii(rhs);
}

bsl::ostream&  bteso_Flag::streamOut(bsl::ostream& stream,
                                     bteso_Flag::ShutdownType rhs)
{
    return stream << bteso_Flag::toAscii(rhs);
}

bsl::ostream&  bteso_Flag::streamOut(bsl::ostream& stream,
                                     bteso_Flag::IOWaitType rhs)
{
    return stream << bteso_Flag::toAscii(rhs);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
