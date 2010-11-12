// btemt_queryprocessorconfiguration.cpp  -*-C++-*-
#include <btemt_queryprocessorconfiguration.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btemt_queryprocessorconfiguration_cpp,"$Id$ $CSID$")

#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {

                // =======================================
                // class btemt_QueryProcessorConfiguration
                // =======================================

                               // -------------
                               // CLASS METHODS
                               // -------------

int btemt_QueryProcessorConfiguration::maxSupportedBdexVersion() {
    return 1;
}

int btemt_QueryProcessorConfiguration::maxSupportedVersion() {
    return maxSupportedBdexVersion();
}

                                  // --------
                                  // CREATORS
                                  // --------

btemt_QueryProcessorConfiguration::~btemt_QueryProcessorConfiguration()
{
    BSLS_ASSERT(0 <= d_maxConnections);
    BSLS_ASSERT(0 <= d_maxIoThreads);
    BSLS_ASSERT(0 <= d_maxWriteCache);
    BSLS_ASSERT(0 <= d_readTimeout);
    BSLS_ASSERT(0 <= d_metricsInterval);
    BSLS_ASSERT(0 <= d_messageSizeOut);
    BSLS_ASSERT(0 <= d_messageSizeIn);
    BSLS_ASSERT(0 < d_minProcThreads);
    BSLS_ASSERT(d_minProcThreads    <= d_maxProcThreads);
    BSLS_ASSERT(0.0 <= d_idleTimeout);
}

                                // ------------
                                // MANIPULATORS
                                // ------------

                                 // ---------
                                 // ACCESSORS
                                 // ---------

                               // --------------
                               // FREE OPERATORS
                               // --------------

bsl::ostream& operator<<(bsl::ostream&                            output,
                         const btemt_QueryProcessorConfiguration& config)
{
    output << "[\n"
           << "\tmaxConnections         : " << config.d_maxConnections   <<"\n"
           << "\tmaxIoThreads           : " << config.d_maxIoThreads     <<"\n"
           << "\tmaxWriteCache          : " << config.d_maxWriteCache    <<"\n"
           << "\treadTimeout            : " << config.d_readTimeout      <<"\n"
           << "\tmetricsInterval        : " << config.d_metricsInterval  <<"\n"
           << "\toutgoingMessageSize    : " << config.d_messageSizeOut   <<"\n"
           << "\tincomingMessageSize    : " << config.d_messageSizeIn    <<"\n"
           << "\tminProcThreads         : " << config.d_minProcThreads   <<"\n"
           << "\tmaxProcThreads         : " << config.d_maxProcThreads   <<"\n"
           << "\tidleTimeout            : " << config.d_idleTimeout      <<"\n"
           << "]\n";

    return output;
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
