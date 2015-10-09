// btlmt_channelpoolconfiguration.cpp                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <btlmt_channelpoolconfiguration.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlmt_channelpoolconfiguration_cpp,"$Id$ $CSID$")

#include <bdlat_formattingmode.h>
#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_cctype.h>
#include <bsl_ostream.h>

namespace BloombergLP {

                // ------------------------------
                // class ChannelPoolConfiguration
                // ------------------------------

const char btlmt::ChannelPoolConfiguration::CLASS_NAME[] =
                                             "btlmt::ChannelPoolConfiguration";
    // the name of this class

const bdlat_AttributeInfo
                    btlmt::ChannelPoolConfiguration::ATTRIBUTE_INFO_ARRAY[] = {
    {
        e_ATTRIBUTE_ID_MAX_CONNECTIONS,
        "MaxConnections",                     // name
        sizeof("MaxConnections") - 1,         // name length
        "",// annotation
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_MAX_THREADS,
        "MaxThreads",                     // name
        sizeof("MaxThreads") - 1,         // name length
        "",// annotation
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_READ_TIMEOUT,
        "ReadTimeout",                     // name
        sizeof("ReadTimeout") - 1,         // name length
        "",// annotation
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_METRICS_INTERVAL,
        "MetricsInterval",                     // name
        sizeof("MetricsInterval") - 1,         // name length
        "",// annotation
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_MIN_MESSAGE_SIZE_OUT,
        "MinMessageSizeOut",                     // name
        sizeof("MinMessageSizeOut") - 1,         // name length
        "",// annotation
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_TYP_MESSAGE_SIZE_OUT,
        "TypMessageSizeOut",                     // name
        sizeof("TypMessageSizeOut") - 1,         // name length
        "",// annotation
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_MAX_MESSAGE_SIZE_OUT,
        "MaxMessageSizeOut",                     // name
        sizeof("MaxMessageSizeOut") - 1,         // name length
        "",// annotation
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_MIN_MESSAGE_SIZE_IN,
        "MinMessageSizeIn",                     // name
        sizeof("MinMessageSizeIn") - 1,         // name length
        "",// annotation
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_TYP_MESSAGE_SIZE_IN,
        "TypMessageSizeIn",                     // name
        sizeof("TypMessageSizeIn") - 1,         // name length
        "",// annotation
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_MAX_MESSAGE_SIZE_IN,
        "MaxMessageSizeIn",                     // name
        sizeof("MaxMessageSizeIn") - 1,         // name length
        "",// annotation
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_WRITE_CACHE_LOW_WAT,
        "WriteCacheLowWat",                     // name
        sizeof("WriteCacheLowWat") - 1,         // name length
        "",// annotation
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_WRITE_CACHE_HI_WAT,
        "WriteCacheHiWat",                     // name
        sizeof("WriteCacheHiWat") - 1,         // name length
        "",// annotation
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_THREAD_STACK_SIZE,
        "ThreadStackSize",                     // name
        sizeof("ThreadStackSize") - 1,         // name length
        "",// annotation
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        e_ATTRIBUTE_ID_COLLECT_TIME_METRICS,
        "CollectTimeMetrics",                  // name
        sizeof("CollectTimeMetrics") - 1,      // name length
        "",// annotation
        bdlat_FormattingMode::e_DEFAULT
    }
};

namespace btlmt {

// CLASS METHODS
const bdlat_AttributeInfo *ChannelPoolConfiguration::lookupAttributeInfo(
                                                        const char *name,
                                                        int         nameLength)
{
    switch(nameLength) {
      case 10: {
        if (bsl::toupper(name[0])=='M'
         && bsl::toupper(name[1])=='A'
         && bsl::toupper(name[2])=='X'
         && bsl::toupper(name[3])=='T'
         && bsl::toupper(name[4])=='H'
         && bsl::toupper(name[5])=='R'
         && bsl::toupper(name[6])=='E'
         && bsl::toupper(name[7])=='A'
         && bsl::toupper(name[8])=='D'
         && bsl::toupper(name[9])=='S') {
            return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_THREADS];
                                                                      // RETURN
        }
      } break;
      case 11: {
        if (bsl::toupper(name[0])=='R'
         && bsl::toupper(name[1])=='E'
         && bsl::toupper(name[2])=='A'
         && bsl::toupper(name[3])=='D'
         && bsl::toupper(name[4])=='T'
         && bsl::toupper(name[5])=='I'
         && bsl::toupper(name[6])=='M'
         && bsl::toupper(name[7])=='E'
         && bsl::toupper(name[8])=='O'
         && bsl::toupper(name[9])=='U'
         && bsl::toupper(name[10])=='T') {
            return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_READ_TIMEOUT];
                                                                      // RETURN
        }
      } break;
      case 14: {
        if (bsl::toupper(name[0])=='M'
         && bsl::toupper(name[1])=='A'
         && bsl::toupper(name[2])=='X'
         && bsl::toupper(name[3])=='C'
         && bsl::toupper(name[4])=='O'
         && bsl::toupper(name[5])=='N'
         && bsl::toupper(name[6])=='N'
         && bsl::toupper(name[7])=='E'
         && bsl::toupper(name[8])=='C'
         && bsl::toupper(name[9])=='T'
         && bsl::toupper(name[10])=='I'
         && bsl::toupper(name[11])=='O'
         && bsl::toupper(name[12])=='N'
         && bsl::toupper(name[13])=='S') {
            return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_CONNECTIONS];
                                                                      // RETURN
        }
      } break;
      case 15: {
        switch(bsl::toupper(name[0])) {
          case 'M': {
            if (bsl::toupper(name[1])=='E'
             && bsl::toupper(name[2])=='T'
             && bsl::toupper(name[3])=='R'
             && bsl::toupper(name[4])=='I'
             && bsl::toupper(name[5])=='C'
             && bsl::toupper(name[6])=='S'
             && bsl::toupper(name[7])=='I'
             && bsl::toupper(name[8])=='N'
             && bsl::toupper(name[9])=='T'
             && bsl::toupper(name[10])=='E'
             && bsl::toupper(name[11])=='R'
             && bsl::toupper(name[12])=='V'
             && bsl::toupper(name[13])=='A'
             && bsl::toupper(name[14])=='L') {
                return
                    &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_METRICS_INTERVAL];
                                                                      // RETURN
            }
          } break;
          case 'W': {
            if (bsl::toupper(name[1])=='R'
             && bsl::toupper(name[2])=='I'
             && bsl::toupper(name[3])=='T'
             && bsl::toupper(name[4])=='E'
             && bsl::toupper(name[5])=='C'
             && bsl::toupper(name[6])=='A'
             && bsl::toupper(name[7])=='C'
             && bsl::toupper(name[8])=='H'
             && bsl::toupper(name[9])=='E'
             && bsl::toupper(name[10])=='H'
             && bsl::toupper(name[11])=='I'
             && bsl::toupper(name[12])=='W'
             && bsl::toupper(name[13])=='A'
             && bsl::toupper(name[14])=='T') {
                return
                   &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_WRITE_CACHE_HI_WAT];
                                                                      // RETURN
            }
          } break;
          case 'T': {
            if (bsl::toupper(name[1])=='H'
             && bsl::toupper(name[2])=='R'
             && bsl::toupper(name[3])=='E'
             && bsl::toupper(name[4])=='A'
             && bsl::toupper(name[5])=='D'
             && bsl::toupper(name[6])=='S'
             && bsl::toupper(name[7])=='T'
             && bsl::toupper(name[8])=='A'
             && bsl::toupper(name[9])=='C'
             && bsl::toupper(name[10])=='K'
             && bsl::toupper(name[11])=='S'
             && bsl::toupper(name[12])=='I'
             && bsl::toupper(name[13])=='Z'
             && bsl::toupper(name[14])=='E') {
                return
                    &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_THREAD_STACK_SIZE];
                                                                      // RETURN
            }
          } break;
        }
      } break;
      case 16: {
        switch(bsl::toupper(name[0])) {
          case 'M': {
            switch(bsl::toupper(name[1])) {
              case 'A': {
                if (bsl::toupper(name[2])=='X'
                 && bsl::toupper(name[3])=='M'
                 && bsl::toupper(name[4])=='E'
                 && bsl::toupper(name[5])=='S'
                 && bsl::toupper(name[6])=='S'
                 && bsl::toupper(name[7])=='A'
                 && bsl::toupper(name[8])=='G'
                 && bsl::toupper(name[9])=='E'
                 && bsl::toupper(name[10])=='S'
                 && bsl::toupper(name[11])=='I'
                 && bsl::toupper(name[12])=='Z'
                 && bsl::toupper(name[13])=='E'
                 && bsl::toupper(name[14])=='I'
                 && bsl::toupper(name[15])=='N') {
                   return
                  &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_MESSAGE_SIZE_IN];
                                                                      // RETURN
                }
              } break;
              case 'I': {
                if (bsl::toupper(name[2])=='N'
                 && bsl::toupper(name[3])=='M'
                 && bsl::toupper(name[4])=='E'
                 && bsl::toupper(name[5])=='S'
                 && bsl::toupper(name[6])=='S'
                 && bsl::toupper(name[7])=='A'
                 && bsl::toupper(name[8])=='G'
                 && bsl::toupper(name[9])=='E'
                 && bsl::toupper(name[10])=='S'
                 && bsl::toupper(name[11])=='I'
                 && bsl::toupper(name[12])=='Z'
                 && bsl::toupper(name[13])=='E'
                 && bsl::toupper(name[14])=='I'
                 && bsl::toupper(name[15])=='N') {
                    return
                  &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MIN_MESSAGE_SIZE_IN];
                                                                      // RETURN
                }
              } break;
            }
          } break;
          case 'T': {
            if (bsl::toupper(name[1])=='Y'
             && bsl::toupper(name[2])=='P'
             && bsl::toupper(name[3])=='M'
             && bsl::toupper(name[4])=='E'
             && bsl::toupper(name[5])=='S'
             && bsl::toupper(name[6])=='S'
             && bsl::toupper(name[7])=='A'
             && bsl::toupper(name[8])=='G'
             && bsl::toupper(name[9])=='E'
             && bsl::toupper(name[10])=='S'
             && bsl::toupper(name[11])=='I'
             && bsl::toupper(name[12])=='Z'
             && bsl::toupper(name[13])=='E'
             && bsl::toupper(name[14])=='I'
             && bsl::toupper(name[15])=='N') {
                return
                  &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TYP_MESSAGE_SIZE_IN];
                                                                      // RETURN
            }
          } break;
          case 'W': {
            if (bsl::toupper(name[1])=='R'
             && bsl::toupper(name[2])=='I'
             && bsl::toupper(name[3])=='T'
             && bsl::toupper(name[4])=='E'
             && bsl::toupper(name[5])=='C'
             && bsl::toupper(name[6])=='A'
             && bsl::toupper(name[7])=='C'
             && bsl::toupper(name[8])=='H'
             && bsl::toupper(name[9])=='E'
             && bsl::toupper(name[10])=='L'
             && bsl::toupper(name[11])=='O'
             && bsl::toupper(name[12])=='W'
             && bsl::toupper(name[13])=='W'
             && bsl::toupper(name[14])=='A'
             && bsl::toupper(name[15])=='T') {
                return
                  &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_WRITE_CACHE_LOW_WAT];
                                                                      // RETURN
            }
          } break;
        }
      } break;
      case 17: {
        switch(bsl::toupper(name[0])) {
          case 'M': {
            switch(bsl::toupper(name[1])) {
              case 'A': {
                if (bsl::toupper(name[2])=='X'
                 && bsl::toupper(name[3])=='M'
                 && bsl::toupper(name[4])=='E'
                 && bsl::toupper(name[5])=='S'
                 && bsl::toupper(name[6])=='S'
                 && bsl::toupper(name[7])=='A'
                 && bsl::toupper(name[8])=='G'
                 && bsl::toupper(name[9])=='E'
                 && bsl::toupper(name[10])=='S'
                 && bsl::toupper(name[11])=='I'
                 && bsl::toupper(name[12])=='Z'
                 && bsl::toupper(name[13])=='E'
                 && bsl::toupper(name[14])=='O'
                 && bsl::toupper(name[15])=='U'
                 && bsl::toupper(name[16])=='T') {
                    return
                 &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_MESSAGE_SIZE_OUT];
                                                                      // RETURN
                }
              } break;
              case 'I': {
                if (bsl::toupper(name[2])=='N'
                 && bsl::toupper(name[3])=='M'
                 && bsl::toupper(name[4])=='E'
                 && bsl::toupper(name[5])=='S'
                 && bsl::toupper(name[6])=='S'
                 && bsl::toupper(name[7])=='A'
                 && bsl::toupper(name[8])=='G'
                 && bsl::toupper(name[9])=='E'
                 && bsl::toupper(name[10])=='S'
                 && bsl::toupper(name[11])=='I'
                 && bsl::toupper(name[12])=='Z'
                 && bsl::toupper(name[13])=='E'
                 && bsl::toupper(name[14])=='O'
                 && bsl::toupper(name[15])=='U'
                 && bsl::toupper(name[16])=='T') {
                    return
                 &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MIN_MESSAGE_SIZE_OUT];
                                                                      // RETURN
                }
              } break;
            }
          } break;
          case 'T': {
            if (bsl::toupper(name[1])=='Y'
             && bsl::toupper(name[2])=='P'
             && bsl::toupper(name[3])=='M'
             && bsl::toupper(name[4])=='E'
             && bsl::toupper(name[5])=='S'
             && bsl::toupper(name[6])=='S'
             && bsl::toupper(name[7])=='A'
             && bsl::toupper(name[8])=='G'
             && bsl::toupper(name[9])=='E'
             && bsl::toupper(name[10])=='S'
             && bsl::toupper(name[11])=='I'
             && bsl::toupper(name[12])=='Z'
             && bsl::toupper(name[13])=='E'
             && bsl::toupper(name[14])=='O'
             && bsl::toupper(name[15])=='U'
             && bsl::toupper(name[16])=='T') {
                return
                 &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TYP_MESSAGE_SIZE_OUT];
                                                                      // RETURN
            }
          } break;
        }
      } break;
      case 18: {
        if (bsl::toupper(name[0])=='C'
         && bsl::toupper(name[1])=='O'
         && bsl::toupper(name[2])=='L'
         && bsl::toupper(name[3])=='L'
         && bsl::toupper(name[4])=='E'
         && bsl::toupper(name[5])=='C'
         && bsl::toupper(name[6])=='T'
         && bsl::toupper(name[7])=='T'
         && bsl::toupper(name[8])=='I'
         && bsl::toupper(name[9])=='M'
         && bsl::toupper(name[10])=='E'
         && bsl::toupper(name[11])=='M'
         && bsl::toupper(name[12])=='E'
         && bsl::toupper(name[13])=='T'
         && bsl::toupper(name[14])=='R'
         && bsl::toupper(name[15])=='I'
         && bsl::toupper(name[16])=='C'
         && bsl::toupper(name[17])=='S') {
            return &ATTRIBUTE_INFO_ARRAY[
                                       e_ATTRIBUTE_INDEX_COLLECT_TIME_METRICS];
                                                                      // RETURN
        }
      } break;
    }
    return 0;
}

const bdlat_AttributeInfo *
ChannelPoolConfiguration::lookupAttributeInfo(int id)
{
    switch (id) {
      case e_ATTRIBUTE_ID_MAX_CONNECTIONS: {
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_CONNECTIONS];
                                                                      // RETURN
      }
      case e_ATTRIBUTE_ID_MAX_THREADS: {
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_THREADS];  // RETURN
      }
      case e_ATTRIBUTE_ID_READ_TIMEOUT: {
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_READ_TIMEOUT]; // RETURN
      }
      case e_ATTRIBUTE_ID_METRICS_INTERVAL: {
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_METRICS_INTERVAL];
                                                                      // RETURN
      }
      case e_ATTRIBUTE_ID_MIN_MESSAGE_SIZE_OUT: {
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MIN_MESSAGE_SIZE_OUT];
                                                                      // RETURN
      }
      case e_ATTRIBUTE_ID_TYP_MESSAGE_SIZE_OUT: {
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TYP_MESSAGE_SIZE_OUT];
                                                                      // RETURN
      }
      case e_ATTRIBUTE_ID_MAX_MESSAGE_SIZE_OUT: {
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_MESSAGE_SIZE_OUT];
                                                                      // RETURN
      }
      case e_ATTRIBUTE_ID_MIN_MESSAGE_SIZE_IN: {
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MIN_MESSAGE_SIZE_IN];
                                                                      // RETURN
      }
      case e_ATTRIBUTE_ID_TYP_MESSAGE_SIZE_IN: {
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TYP_MESSAGE_SIZE_IN];
                                                                      // RETURN
      }
      case e_ATTRIBUTE_ID_MAX_MESSAGE_SIZE_IN: {
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_MESSAGE_SIZE_IN];
                                                                      // RETURN
      }
      case e_ATTRIBUTE_ID_WRITE_CACHE_LOW_WAT: {
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_WRITE_CACHE_LOW_WAT];
                                                                      // RETURN
      }
      case e_ATTRIBUTE_ID_WRITE_CACHE_HI_WAT: {
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_WRITE_CACHE_HI_WAT];
                                                                      // RETURN
      }
      case e_ATTRIBUTE_ID_THREAD_STACK_SIZE: {
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_THREAD_STACK_SIZE];
                                                                      // RETURN
      }
      case e_ATTRIBUTE_ID_COLLECT_TIME_METRICS: {
        return &ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_COLLECT_TIME_METRICS];
                                                                      // RETURN
      }

      default:
        return 0;                                                     // RETURN
    }
}

// CREATORS
ChannelPoolConfiguration::ChannelPoolConfiguration()
: d_maxConnections(1024)
, d_maxThreads(1)
, d_writeCacheLowWat(0)
, d_writeCacheHiWat(1 << 20)
, d_readTimeout(30)
, d_metricsInterval(30)
, d_minMessageSizeOut(1)
, d_typMessageSizeOut(1)
, d_maxMessageSizeOut(1 << 20)
, d_minMessageSizeIn(1)
, d_typMessageSizeIn(1)
, d_maxMessageSizeIn(1024)
, d_threadStackSize(k_DEFAULT_THREAD_STACK_SIZE)
, d_collectTimeMetrics(true)
{
}

ChannelPoolConfiguration::ChannelPoolConfiguration(
                                      const ChannelPoolConfiguration& original)
: d_maxConnections(original.d_maxConnections)
, d_maxThreads(original.d_maxThreads)
, d_writeCacheLowWat(original.d_writeCacheLowWat)
, d_writeCacheHiWat(original.d_writeCacheHiWat)
, d_readTimeout(original.d_readTimeout)
, d_metricsInterval(original.d_metricsInterval)
, d_minMessageSizeOut(original.d_minMessageSizeOut)
, d_typMessageSizeOut(original.d_typMessageSizeOut)
, d_maxMessageSizeOut(original.d_maxMessageSizeOut)
, d_minMessageSizeIn(original.d_minMessageSizeIn)
, d_typMessageSizeIn(original.d_typMessageSizeIn)
, d_maxMessageSizeIn(original.d_maxMessageSizeIn)
, d_threadStackSize(original.d_threadStackSize)
, d_collectTimeMetrics(original.d_collectTimeMetrics)
{
}

ChannelPoolConfiguration::~ChannelPoolConfiguration()
{
    BSLS_ASSERT(0 <= d_maxConnections);
    BSLS_ASSERT(0 <= d_maxThreads);
    BSLS_ASSERT(0 <= d_writeCacheLowWat);
    BSLS_ASSERT(d_writeCacheLowWat <= d_writeCacheHiWat);
    BSLS_ASSERT(0 <= d_readTimeout);
    BSLS_ASSERT(0 <= d_metricsInterval);
    BSLS_ASSERT(0 <= d_minMessageSizeOut
             && d_minMessageSizeOut <= d_typMessageSizeOut
             && d_typMessageSizeOut <= d_maxMessageSizeOut);
    BSLS_ASSERT(0 <= d_minMessageSizeIn
             && d_minMessageSizeIn <= d_typMessageSizeIn
             && d_typMessageSizeIn <= d_maxMessageSizeIn);
    BSLS_ASSERT(0 <= d_threadStackSize);
}

// MANIPULATORS
ChannelPoolConfiguration&
ChannelPoolConfiguration::operator=(const ChannelPoolConfiguration& rhs)
{
    if (this != &rhs) {
        d_maxConnections     = rhs.d_maxConnections;
        d_maxThreads         = rhs.d_maxThreads;
        d_writeCacheLowWat   = rhs.d_writeCacheLowWat;
        d_writeCacheHiWat    = rhs.d_writeCacheHiWat;
        d_readTimeout        = rhs.d_readTimeout;
        d_metricsInterval    = rhs.d_metricsInterval;
        d_minMessageSizeOut  = rhs.d_minMessageSizeOut;
        d_typMessageSizeOut  = rhs.d_typMessageSizeOut;
        d_maxMessageSizeOut  = rhs.d_maxMessageSizeOut;
        d_minMessageSizeIn   = rhs.d_minMessageSizeIn;
        d_typMessageSizeIn   = rhs.d_typMessageSizeIn;
        d_maxMessageSizeIn   = rhs.d_maxMessageSizeIn;
        d_threadStackSize    = rhs.d_threadStackSize;
        d_collectTimeMetrics = rhs.d_collectTimeMetrics;
    }
    return *this;
}

}  // close package namespace

// FREE OPERATORS
bool btlmt::operator==(const ChannelPoolConfiguration& lhs,
                       const ChannelPoolConfiguration& rhs)
{
    return lhs.d_maxConnections     == rhs.d_maxConnections
        && lhs.d_maxThreads         == rhs.d_maxThreads
        && lhs.d_writeCacheHiWat    == rhs.d_writeCacheHiWat
        && lhs.d_writeCacheLowWat   == rhs.d_writeCacheLowWat
        && lhs.d_readTimeout        == rhs.d_readTimeout
        && lhs.d_metricsInterval    == rhs.d_metricsInterval
        && lhs.d_minMessageSizeOut  == rhs.d_minMessageSizeOut
        && lhs.d_typMessageSizeOut  == rhs.d_typMessageSizeOut
        && lhs.d_maxMessageSizeOut  == rhs.d_maxMessageSizeOut
        && lhs.d_minMessageSizeIn   == rhs.d_minMessageSizeIn
        && lhs.d_typMessageSizeIn   == rhs.d_typMessageSizeIn
        && lhs.d_maxMessageSizeIn   == rhs.d_maxMessageSizeIn
        && lhs.d_threadStackSize    == rhs.d_threadStackSize
        && lhs.d_collectTimeMetrics == rhs.d_collectTimeMetrics;
}

bsl::ostream& btlmt::operator<<(bsl::ostream&                   output,
                                const ChannelPoolConfiguration& config)
{
    output << "[\n"
           << "\tmaxConnections         : " << config.d_maxConnections   <<"\n"
           << "\tmaxThreads             : " << config.d_maxThreads       <<"\n"
           << "\twriteCacheLowWat       : " << config.d_writeCacheLowWat <<"\n"
           << "\twriteCacheHiWat        : " << config.d_writeCacheHiWat  <<"\n"
           << "\treadTimeout            : " << config.d_readTimeout      <<"\n"
           << "\tmetricsInterval        : " << config.d_metricsInterval  <<"\n"
           << "\tminOutgoingMessageSize : " << config.d_minMessageSizeOut<<"\n"
           << "\ttypOutgoingMessageSize : " << config.d_typMessageSizeOut<<"\n"
           << "\tmaxOutgoingMessageSize : " << config.d_maxMessageSizeOut<<"\n"
           << "\tminIncomingMessageSize : " << config.d_minMessageSizeIn <<"\n"
           << "\ttypIncomingMessageSize : " << config.d_typMessageSizeIn <<"\n"
           << "\tmaxIncomingMessageSize : " << config.d_maxMessageSizeIn <<"\n"
           << "\tthreadStackSize        : " << config.d_threadStackSize  <<"\n"
           << "\tcollectTimeMetrics     : " << config.d_collectTimeMetrics
           << "\n]\n";

    return output;
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
