// btlso_socketoptions.cpp                                            -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_socketoptions_cpp,"$Id$ $CSID$")

#include <btlso_socketoptions.h>

#include <bdlat_valuetypefunctions.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>

#include <bdlb_nullablevalue.h>
#include <btlso_lingeroptions.h>

#include <bsls_assert.h>

#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>

namespace BloombergLP {

namespace btlso {
                       // -------------------------
                       // class SocketOptions
                       // -------------------------

// CREATORS
SocketOptions::SocketOptions()
: d_linger()
, d_sendBufferSize()
, d_receiveBufferSize()
, d_minimumSendBufferSize()
, d_minimumReceiveBufferSize()
, d_sendTimeout()
, d_receiveTimeout()
, d_debugFlag()
, d_allowBroadcasting()
, d_reuseAddress()
, d_keepAlive()
, d_bypassNormalRouting()
, d_leaveOutOfBandDataInline()
, d_tcpNoDelay()
{
}

SocketOptions::SocketOptions(const SocketOptions& original)
: d_linger(original.d_linger)
, d_sendBufferSize(original.d_sendBufferSize)
, d_receiveBufferSize(original.d_receiveBufferSize)
, d_minimumSendBufferSize(original.d_minimumSendBufferSize)
, d_minimumReceiveBufferSize(original.d_minimumReceiveBufferSize)
, d_sendTimeout(original.d_sendTimeout)
, d_receiveTimeout(original.d_receiveTimeout)
, d_debugFlag(original.d_debugFlag)
, d_allowBroadcasting(original.d_allowBroadcasting)
, d_reuseAddress(original.d_reuseAddress)
, d_keepAlive(original.d_keepAlive)
, d_bypassNormalRouting(original.d_bypassNormalRouting)
, d_leaveOutOfBandDataInline(original.d_leaveOutOfBandDataInline)
, d_tcpNoDelay(original.d_tcpNoDelay)
{
}

SocketOptions::~SocketOptions()
{
}

// MANIPULATORS
SocketOptions&
SocketOptions::operator=(const SocketOptions& rhs)
{
    if (this != &rhs) {
        d_debugFlag = rhs.d_debugFlag;
        d_allowBroadcasting = rhs.d_allowBroadcasting;
        d_reuseAddress = rhs.d_reuseAddress;
        d_keepAlive = rhs.d_keepAlive;
        d_bypassNormalRouting = rhs.d_bypassNormalRouting;
        d_linger = rhs.d_linger;
        d_leaveOutOfBandDataInline = rhs.d_leaveOutOfBandDataInline;
        d_sendBufferSize = rhs.d_sendBufferSize;
        d_receiveBufferSize = rhs.d_receiveBufferSize;
        d_minimumSendBufferSize = rhs.d_minimumSendBufferSize;
        d_minimumReceiveBufferSize = rhs.d_minimumReceiveBufferSize;
        d_sendTimeout = rhs.d_sendTimeout;
        d_receiveTimeout = rhs.d_receiveTimeout;
        d_tcpNoDelay = rhs.d_tcpNoDelay;
    }
    return *this;
}

void SocketOptions::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_debugFlag);
    bdeat_ValueTypeFunctions::reset(&d_allowBroadcasting);
    bdeat_ValueTypeFunctions::reset(&d_reuseAddress);
    bdeat_ValueTypeFunctions::reset(&d_keepAlive);
    bdeat_ValueTypeFunctions::reset(&d_bypassNormalRouting);
    bdeat_ValueTypeFunctions::reset(&d_linger);
    bdeat_ValueTypeFunctions::reset(&d_leaveOutOfBandDataInline);
    bdeat_ValueTypeFunctions::reset(&d_sendBufferSize);
    bdeat_ValueTypeFunctions::reset(&d_receiveBufferSize);
    bdeat_ValueTypeFunctions::reset(&d_minimumSendBufferSize);
    bdeat_ValueTypeFunctions::reset(&d_minimumReceiveBufferSize);
    bdeat_ValueTypeFunctions::reset(&d_sendTimeout);
    bdeat_ValueTypeFunctions::reset(&d_receiveTimeout);
    bdeat_ValueTypeFunctions::reset(&d_tcpNoDelay);
}

// ACCESSORS
bsl::ostream& SocketOptions::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "DebugFlag = ";
        bdlb::PrintMethods::print(stream, d_debugFlag,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "AllowBroadcasting = ";
        bdlb::PrintMethods::print(stream, d_allowBroadcasting,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "ReuseAddress = ";
        bdlb::PrintMethods::print(stream, d_reuseAddress,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "KeepAlive = ";
        bdlb::PrintMethods::print(stream, d_keepAlive,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "BypassNormalRouting = ";
        bdlb::PrintMethods::print(stream, d_bypassNormalRouting,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Linger = ";
        bdlb::PrintMethods::print(stream, d_linger,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "LeaveOutOfBandDataInline = ";
        bdlb::PrintMethods::print(stream, d_leaveOutOfBandDataInline,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "SendBufferSize = ";
        bdlb::PrintMethods::print(stream, d_sendBufferSize,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "ReceiveBufferSize = ";
        bdlb::PrintMethods::print(stream, d_receiveBufferSize,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "MinimumSendBufferSize = ";
        bdlb::PrintMethods::print(stream, d_minimumSendBufferSize,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "MinimumReceiveBufferSize = ";
        bdlb::PrintMethods::print(stream, d_minimumReceiveBufferSize,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "SendTimeout = ";
        bdlb::PrintMethods::print(stream, d_sendTimeout,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "ReceiveTimeout = ";
        bdlb::PrintMethods::print(stream, d_receiveTimeout,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "TcpNoDelay = ";
        bdlb::PrintMethods::print(stream, d_tcpNoDelay,
                                 -levelPlus1, spacesPerLevel);

        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "DebugFlag = ";
        bdlb::PrintMethods::print(stream, d_debugFlag,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "AllowBroadcasting = ";
        bdlb::PrintMethods::print(stream, d_allowBroadcasting,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "ReuseAddress = ";
        bdlb::PrintMethods::print(stream, d_reuseAddress,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "KeepAlive = ";
        bdlb::PrintMethods::print(stream, d_keepAlive,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "BypassNormalRouting = ";
        bdlb::PrintMethods::print(stream, d_bypassNormalRouting,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Linger = ";
        bdlb::PrintMethods::print(stream, d_linger,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "LeaveOutOfBandDataInline = ";
        bdlb::PrintMethods::print(stream, d_leaveOutOfBandDataInline,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "SendBufferSize = ";
        bdlb::PrintMethods::print(stream, d_sendBufferSize,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "ReceiveBufferSize = ";
        bdlb::PrintMethods::print(stream, d_receiveBufferSize,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "MinimumSendBufferSize = ";
        bdlb::PrintMethods::print(stream, d_minimumSendBufferSize,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "MinimumReceiveBufferSize = ";
        bdlb::PrintMethods::print(stream, d_minimumReceiveBufferSize,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "SendTimeout = ";
        bdlb::PrintMethods::print(stream, d_sendTimeout,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "ReceiveTimeout = ";
        bdlb::PrintMethods::print(stream, d_receiveTimeout,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "TcpNoDelay = ";
        bdlb::PrintMethods::print(stream, d_tcpNoDelay,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream;
}
}  // close package namespace

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
