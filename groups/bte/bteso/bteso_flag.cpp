// bteso_flag.cpp            -*-C++-*-
#include <bteso_flag.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bteso_flag_cpp,"$Id$ $CSID$")

#include <bdes_bitutil.h>

#include <bsl_ostream.h>

namespace BloombergLP {

#define CASE(X) case(BTESO_ ## X): return #X

const char *bteso_Flag::toAscii(bteso_Flag::Flag value)
{
    switch (value) {
        CASE(ASYNC_INTERRUPT);
        default: return "(* UNKNOWN *)";
    }
}

const char *bteso_Flag::toAscii(bteso_Flag::BlockingMode value)
{
    switch (value) {
        CASE(BLOCKING_MODE);
        CASE(NONBLOCKING_MODE);
        default: return "(* UNKNOWN *)";
    }
}

const char *bteso_Flag::toAscii(bteso_Flag::ShutdownType value)
{
    switch (value) {
        CASE(SHUTDOWN_RECEIVE);
        CASE(SHUTDOWN_SEND);
        CASE(SHUTDOWN_BOTH);
        default: return "(* UNKNOWN *)";
    }
}

const char *bteso_Flag::toAscii(bteso_Flag::IOWaitType value)
{
    switch (value) {
        CASE(IO_READ);
        CASE(IO_WRITE);
        CASE(IO_RW);
        default: return "(* UNKNOWN *)";
    }
}

#undef CASE

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
