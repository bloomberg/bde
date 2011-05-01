// bteso_socketoptions.cpp                                            -*-C++-*-

#include <bdes_ident.h>
BDES_IDENT_RCSID(bteso_socketoptions_cpp,"$Id$ $CSID$")

#include <bteso_socketoptions.h>

#include <bdeat_valuetypefunctions.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>

#include <bdeut_nullablevalue.h>
#include <bteso_lingeroptions.h>

#include <bsls_assert.h>

#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>

namespace BloombergLP {

                       // -------------------------
                       // class bteso_SocketOptions
                       // -------------------------

// CREATORS
bteso_SocketOptions::bteso_SocketOptions()
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
{
}

bteso_SocketOptions::bteso_SocketOptions(const bteso_SocketOptions& original)
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
{
}

bteso_SocketOptions::~bteso_SocketOptions()
{
}

// MANIPULATORS
bteso_SocketOptions&
bteso_SocketOptions::operator=(const bteso_SocketOptions& rhs)
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
    }
    return *this;
}

void bteso_SocketOptions::reset()
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
}

// ACCESSORS
bsl::ostream& bteso_SocketOptions::print(
    bsl::ostream& stream,
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
        stream << "DebugFlag = ";
        bdeu_PrintMethods::print(stream, d_debugFlag,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "AllowBroadcasting = ";
        bdeu_PrintMethods::print(stream, d_allowBroadcasting,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "ReuseAddress = ";
        bdeu_PrintMethods::print(stream, d_reuseAddress,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "KeepAlive = ";
        bdeu_PrintMethods::print(stream, d_keepAlive,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "BypassNormalRouting = ";
        bdeu_PrintMethods::print(stream, d_bypassNormalRouting,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Linger = ";
        bdeu_PrintMethods::print(stream, d_linger,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "LeaveOutOfBandDataInline = ";
        bdeu_PrintMethods::print(stream, d_leaveOutOfBandDataInline,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "SendBufferSize = ";
        bdeu_PrintMethods::print(stream, d_sendBufferSize,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "ReceiveBufferSize = ";
        bdeu_PrintMethods::print(stream, d_receiveBufferSize,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "MinimumSendBufferSize = ";
        bdeu_PrintMethods::print(stream, d_minimumSendBufferSize,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "MinimumReceiveBufferSize = ";
        bdeu_PrintMethods::print(stream, d_minimumReceiveBufferSize,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "SendTimeout = ";
        bdeu_PrintMethods::print(stream, d_sendTimeout,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "ReceiveTimeout = ";
        bdeu_PrintMethods::print(stream, d_receiveTimeout,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "DebugFlag = ";
        bdeu_PrintMethods::print(stream, d_debugFlag,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "AllowBroadcasting = ";
        bdeu_PrintMethods::print(stream, d_allowBroadcasting,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "ReuseAddress = ";
        bdeu_PrintMethods::print(stream, d_reuseAddress,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "KeepAlive = ";
        bdeu_PrintMethods::print(stream, d_keepAlive,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "BypassNormalRouting = ";
        bdeu_PrintMethods::print(stream, d_bypassNormalRouting,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Linger = ";
        bdeu_PrintMethods::print(stream, d_linger,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "LeaveOutOfBandDataInline = ";
        bdeu_PrintMethods::print(stream, d_leaveOutOfBandDataInline,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "SendBufferSize = ";
        bdeu_PrintMethods::print(stream, d_sendBufferSize,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "ReceiveBufferSize = ";
        bdeu_PrintMethods::print(stream, d_receiveBufferSize,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "MinimumSendBufferSize = ";
        bdeu_PrintMethods::print(stream, d_minimumSendBufferSize,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "MinimumReceiveBufferSize = ";
        bdeu_PrintMethods::print(stream, d_minimumReceiveBufferSize,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "SendTimeout = ";
        bdeu_PrintMethods::print(stream, d_sendTimeout,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "ReceiveTimeout = ";
        bdeu_PrintMethods::print(stream, d_receiveTimeout,
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
