// btemt_queryprocessorconfiguration.h   -*-C++-*-
#ifndef INCLUDED_BTEMT_QUERYPROCESSORCONFIGURATION
#define INCLUDED_BTEMT_QUERYPROCESSORCONFIGURATION

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a constrained-attribute class for query processors.
//
//@AUTHOR: Tom Marshall (tmarshal)
//
//@SEE_ALSO: btemt_ChannelPoolConfiguration btemt_ChannelPool
//
//@CLASSES:
// btemt_QueryProcessorConfiguration:
//     configuration parameters for a query processor.
//
//@DESCRIPTION: This component provides a constrained-attribute class that
// contains a set of attributes (objects and parameters) of particular use to
// query processors.  The constraints are actively maintained by the class.  In
// particular, the "set" methods for constrained values will fail if their
// arguments are not consistent with the constraints.  Also, the constructor
// does not take any constrained arguments, but rather sets those values to
// valid defaults unconditionally.  This behavior avoids "silent failures",
// since the constructor cannot explicitly return a status value.
//
// The attributes contained by a 'btemt_QueryProcessorConfiguration' object and
// the attribute constraints are given, respectively, in two tables below.  The
// attributes are as follows:
//..
//   TYPE                 NAME                DESCRIPTION
//   ------------------   --------------      ---------------------------------
//    int                 maxConnections      maximum number of connections
//                                            that can be managed by a query
//                                            processor
//    int                 maxIoThreads        maximum number of IO threads
//                                            managed by a channel pool
//    int                 minProcThreads      minimum number of processing
//                                            threads
//    int                 maxProcThreads      maximum number of processing
//                                            threads
//    double              idleTimeout         idle timeout for processing
//                                            threads
//    int                 maxWriteCache       maximum number of bytes to be
//                                            cached in the outgoing buffer
//                                            for a client channel
//    double              readTimeout         timeout for "read" operations
//    double              metricsInterval     periodic-update interval for
//                                            metrics
//    int                 messageSizeOut      page size for the outgoing
//                                            messages
//    int                 messageSizeIn       page size for the outgoing
//                                            messages
//..
// The constraints are as follows:
//..
//    NAME                 CONSTRAINT
//   +--------------------+---------------------------------------------+
//   | maxConnections     | 0 <= maxConnections                         |
//   +--------------------+---------------------------------------------+
//   | maxIoThreads       | 0 <  maxIoThreads                           |
//   +--------------------+---------------------------------------------+
//   | minProcThreads     | 0 <  minProcThreads <= maxProcThreads       |
//   | maxProcThreads     |                                             |
//   +--------------------+---------------------------------------------+
//   | idleTimeout        | 0 <= idleTimeout                            |
//   +--------------------+---------------------------------------------+
//   | maxWriteCache      | 0 <= maxWriteCache                          |
//   +--------------------+---------------------------------------------+
//   | readTimeout        | 0 <= readTimeout                            |
//   +--------------------+---------------------------------------------+
//   | metricsInterval    | 0 <= metricsInterval                        |
//   +--------------------+---------------------------------------------+
//   | messageSizeOut     | 0 <  maxMessageSizeOut                      |
//   +--------------------+---------------------------------------------+
//   | messageSizeIn      | 0 <  maxMessageSizeIn                       |
//   +--------------------+---------------------------------------------+
//..
//
///Thread Safety
///-------------
// This constrained-attribute component is *thread-safe* but not
// *thread-enabled*, and requires explicit synchronization in the user space.
// Note that any of the contained user-defined callbacks may be invoked from
// *any* thread, and the user must account for that.
//
///USAGE
///-----
// The following snippets of code illustrate how to use a
// 'btemt_QueryProcessorConfiguration' object.  First, create a configuration
// object 'cpc'.  Note that it is necessarily configured to valid but
// unpublished defaults.
//..
//    btemt_QueryProcessorConfiguration cpc;
//..
// Next, set each attribute.  Note that each of the "in" and "out" message size
// triplets must be set atomically (i.e., with a single three-argument "set"
// method.  Note also that each "set" method will fail if the argument or
// triplet of arguments is not valid, and so each method returns a status
// value.
//..
//    assert(0 == cpc.setIncomingMessageSize(4));
//    assert(4 == cpc.incomingMessageSize());
//
//    assert(0 == cpc.setOutgoingMessageSize(6));
//    assert(6 == cpc.outgoingMessageSize());
//
//    assert(0 == cpc.setMaxConnections(100));
//    assert(100 == cpc.maxConnections());
//
//    assert(0 == cpc.setMaxIoThreads(4));
//    assert(4 == cpc.maxIoThreads());
//
//    assert(0 == cpc.setMaxWriteCache(1024));
//    assert(1024 == cpc.maxWriteCache());
//
//    assert(0 == cpc.setReadTimeout(3.5));
//    assert(3.5 == cpc.readTimeout());
//
//    assert(0 == cpc.setMetricsInterval(5.25));
//    assert(5.25 == cpc.metricsInterval());
//
//    assert(0 == cpc.setProcessingThreads(3, 5));
//    assert(3 == cpc.minProcessingThreads());
//    assert(5 == cpc.maxProcessingThreads());
//
//    assert(0 == cpc.setIdleTimeout(3.6));
//    assert(3.6 == cpc.idleTimeout());
//
//..
// The configuration object is now validly configured with our choice of
// parameters.  If, however, we attempt to set an invalid configuration, the
// "set" method will fail (with a non-zero return status), and the
// configuration will be left unchanged.
//..
//    assert(0 != cpc.setIncomingMessageSize(-256));
//    assert(4 == cpc.incomingMessageSize());
//..
// Finally, we can print the configuration value to 'stdout'.
//..
//    cout << cpc;
//..
// This produces the following (multi-line) output:
//..
// [
//         maxConnections         : 100
//         maxIoThreads           : 4
//         maxWriteCache          : 1024
//         readTimeout            : 3.5
//         metricsInterval        : 5.25
//         incomingMessageSize    : 4
//         outgoingMessageSize    : 6
//         minProcThreads         : 3
//         maxProcThreads         : 5
//         idleTimeout            : 3.6
// ]
//..

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

namespace BloombergLP {

class btemt_Message;

class btemt_QueryProcessorConfiguration {
    // This class provides constrained configuration parameters for a channel
    // pool.  The constraints are maintained as class invariants; it is not
    // possible to obtain an invalid object through this interface.
    //
    // More generally, this class supports a complete set of *value* *semantic*
    // operations, including copy construction, assignment, equality
    // comparison, 'ostream' printing, and 'bdex' serialization.  (A precise
    // operational definition of when two instances have the same value can be
    // found in the description of 'operator==' for the class.)  This class is
    // *exception* *neutral* with no guarantee of rollback: if an exception is
    // thrown during the invocation of a method on a pre-existing instance, the
    // object is left in a valid state, but its value is undefined.  In no
    // event is memory leaked.  Finally, *aliasing* (e.g., using all or part of
    // an object as both source and destination) is supported in all cases.

  private:
    int             d_maxConnections;  // maximum number of connections
                                       // that can be managed by a query
                                       // processor
    int             d_maxIoThreads;    // maximum number of threads managed
                                       // by a query processor
    int             d_maxWriteCache;   // maximum number of bytes to be
                                       // cached in the outgoing buffer for a
                                       // client channel
    double          d_readTimeout;     // timeout for "read" operations
    double          d_metricsInterval; // periodic-update interval for
                                       // metrics
    int             d_messageSizeOut;  // page size for the outgoing
                                       // messages
    int             d_messageSizeIn;   // page size for the outgoing
                                       // messages

    int             d_minProcThreads;  // minimum number of processing threads
    int             d_maxProcThreads;  // maximum number of processing threads
    double          d_idleTimeout;     // idle timeout for processing threads

    friend bsl::ostream& operator<<(bsl::ostream&,
                         const btemt_QueryProcessorConfiguration&);

    friend bool operator==(const btemt_QueryProcessorConfiguration&,
                           const btemt_QueryProcessorConfiguration&);

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    static int maxSupportedVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)
        //
        // DEPRECATED: replaced by 'maxSupportedBdexVersion()'

    // CREATORS
    btemt_QueryProcessorConfiguration();
        // Create a channel pool configuration constrained-attribute object
        // having valid default values for all attributes.

    btemt_QueryProcessorConfiguration(
                            const btemt_QueryProcessorConfiguration& original);
        // Create a channel pool configuration constrained-attribute object
        // having the value of the specified 'original' object.

    ~btemt_QueryProcessorConfiguration();
        // Destroy this channel pool configuration constrained-attribute
        // object.

    // MANIPULATORS
    btemt_QueryProcessorConfiguration&
                       operator=(const btemt_QueryProcessorConfiguration& rhs);
        // Assign to this channel pool configuration constrained-attribute
        // object the value of the specified 'rhs' object.

    int setIncomingMessageSize(int size);
        // Set the page size for incoming message to the specified 'size' if
        // 0 < size.  Return 0 on success, and a non-zero value (with no effect
        // on the state of this object) otherwise.

    int setOutgoingMessageSize(int size);
        // Set the page size for incoming message to the specified 'size' if
        // 0 < size.  Return 0 on success, and a non-zero value (with no effect
        // on the state of this object) otherwise.

    int setMaxConnections(int maxConnections);
        // Set the maximum number of connections attribute of this object to
        // the specified 'maxConnections' if 0 <= maxConnections.  Return 0 on
        // success, and a non-zero value (with no effect on the state of this
        // object) otherwise.

    int setMaxIoThreads(int maxIoThreads);
        // Set the maximum number of threads attribute of this object to the
        // specified 'maxIoThreads' if 0 <= maxIoThreads.  Return 0 on success,
        // and a non-zero value (with no effect on the state of this object)
        // otherwise.

    int setMaxWriteCache(int numBytes);
        // Set the maximum write cache size attribute of this object to the
        // specified 'numBytes' if 0 <= numBytes.  Return 0 on success, and a
        // non-zero value (with no effect on the state of this object)
        // otherwise.

    int setMetricsInterval(double metricsInterval);
        // Set the metrics interval attribute of this object to the specified
        // 'metricsInterval' value if 0 <= metricsInterval.  Return 0 on
        // success, and a non-zero value (with no effect on the state of this
        // object) otherwise.

    int setReadTimeout(double readTimeout);
        // Set the read timeout attribute of this object to the specified
        // 'readTimeout' value if 0 <= readTimeout.  Return 0 on success, and a
        // non-zero value (with no effect on the state of this object)
        // otherwise.

    int setProcessingThreads(int minThreads, int maxThreads);
        // Set the minimum and maximum number of processing threads to
        // 'minThreads' and 'maxThreads' respectively.  Return 0 on success and
        // a non-zero value with no effect on the state of this object
        // otherwise.

    int setIdleTimeout(double timeout);

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    // ACCESSORS
    int incomingMessageSize() const;
        // Return the page size for incoming messages.

    int outgoingMessageSize() const;
        // Return the page size for outgoing messages.

    int typicalOutgoingMessageSize() const;
        // Return the typical outgoing-message-size attribute of this object.

    int maxOutgoingMessageSize() const;
        // Return the maximum outgoing-message-size attribute of this object.

    int maxConnections() const;
        // Return the maximum number of connections attribute of this object.

    int maxIoThreads() const;
        // Return the maximum number of threads attribute of this object.

    int maxWriteCache() const;
        // Return the maximum write cache size (in bytes) attribute of this
        // object.

    const double& metricsInterval() const;
        // Return the metrics interval attribute of this object.

    const double& readTimeout() const;
        // Return the read timeout attribute of this object.

    int minProcessingThreads() const;
    int maxProcessingThreads() const;
    double idleTimeout() const;

    bsl::ostream& streamOut(bsl::ostream& stream) const;
        // Write the specified 'configuration' value to the specified 'output'
        // stream in a reasonable multi-line format.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the modifiable
        // 'stream'.  If 'version' is not supported, 'stream' is unmodified.
        // Note that 'version' is not written to 'stream'.  See the 'bdex'
        // package-level documentation for more information on 'bdex' streaming
        // of value-semantic types and containers.
};

// FREE OPERATORS
inline
bool operator==(const btemt_QueryProcessorConfiguration& lhs,
                const btemt_QueryProcessorConfiguration& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects objects
    // have the same value if each respective attribute has the same value.

inline
bool operator!=(const btemt_QueryProcessorConfiguration& lhs,
                const btemt_QueryProcessorConfiguration& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

bsl::ostream& operator<<(bsl::ostream&                         output,
                       const btemt_QueryProcessorConfiguration& configuration);
    // Write the specified 'configuration' value to the specified 'output'
    // stream in a reasonable multi-line format.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// CREATORS
inline
btemt_QueryProcessorConfiguration::btemt_QueryProcessorConfiguration()
: d_maxConnections(1)
, d_maxIoThreads(1)
, d_maxWriteCache(4196)
, d_readTimeout(30)
, d_metricsInterval(30)
, d_messageSizeOut(4196)
, d_messageSizeIn(4196)
, d_minProcThreads(1)
, d_maxProcThreads(1)
, d_idleTimeout(30.0)

{
}

inline
btemt_QueryProcessorConfiguration::btemt_QueryProcessorConfiguration(
                             const btemt_QueryProcessorConfiguration& original)
: d_maxConnections(original.d_maxConnections)
, d_maxIoThreads(original.d_maxIoThreads)
, d_maxWriteCache(original.d_maxWriteCache)
, d_readTimeout(original.d_readTimeout)
, d_metricsInterval(original.d_metricsInterval)
, d_messageSizeOut(original.d_messageSizeOut)
, d_messageSizeIn(original.d_messageSizeIn)
, d_minProcThreads(original.d_minProcThreads)
, d_maxProcThreads(original.d_maxProcThreads)
, d_idleTimeout(original.d_idleTimeout)
{
}

// MANIPULATORS
inline
btemt_QueryProcessorConfiguration&
btemt_QueryProcessorConfiguration::operator=(
                                  const btemt_QueryProcessorConfiguration& rhs)
{
    d_maxConnections    = rhs.d_maxConnections;
    d_maxIoThreads      = rhs.d_maxIoThreads;
    d_maxWriteCache     = rhs.d_maxWriteCache;
    d_readTimeout       = rhs.d_readTimeout;
    d_metricsInterval   = rhs.d_metricsInterval;
    d_messageSizeOut    = rhs.d_messageSizeOut;
    d_messageSizeIn     = rhs.d_messageSizeIn;
    d_minProcThreads    = rhs.d_minProcThreads;
    d_maxProcThreads    = rhs.d_maxProcThreads;
    d_idleTimeout       = rhs.d_idleTimeout;

    return *this;
}

inline
int btemt_QueryProcessorConfiguration::setIncomingMessageSize(int size)
{
    if (0 < size) {
        d_messageSizeIn = size;
        return 0;
    }
    return -1;
}

inline
int btemt_QueryProcessorConfiguration::setOutgoingMessageSize(int size)
{
    if (0 < size) {
        d_messageSizeOut = size;
        return 0;
    }
    return -1;
}

inline
int btemt_QueryProcessorConfiguration::setMaxConnections(int maxConnections)
{
    if (0 <= maxConnections) {
        d_maxConnections = maxConnections;
        return 0;
    }
    return -1;
}

inline
int btemt_QueryProcessorConfiguration::setMaxIoThreads(int maxIoThreads)
{
    if (0 <= maxIoThreads) {
        d_maxIoThreads = maxIoThreads;
        return 0;
    }
    return -1;
}

inline
int btemt_QueryProcessorConfiguration::setMaxWriteCache(int numBytes)
{
    if (0 <= numBytes) {
        d_maxWriteCache = numBytes;
        return 0;
    }
    return -1;
}

inline
int btemt_QueryProcessorConfiguration::setMetricsInterval(
                                             double metricsInterval)
{
    if (0 <= metricsInterval) {
        d_metricsInterval = metricsInterval;
        return 0;
    }
    return -1;
}

inline
int
btemt_QueryProcessorConfiguration::setReadTimeout(double readTimeout)
{
    if (0 <= readTimeout) {
        d_readTimeout = readTimeout;
        return 0;
    }
    return -1;
}

inline
int btemt_QueryProcessorConfiguration::setProcessingThreads(
        int minThreads, int maxThreads)
{
    if (0 < minThreads && minThreads <= maxThreads) {
        d_minProcThreads = minThreads;
        d_maxProcThreads = maxThreads;
        return 0;
    }
    return -1;
}

inline
int btemt_QueryProcessorConfiguration::setIdleTimeout(double timeout)
{
    if (0 < timeout) {
        d_idleTimeout = timeout;
        return 0;
    }
    return -1;
}

template <class STREAM>
STREAM& btemt_QueryProcessorConfiguration::bdexStreamIn(STREAM& stream,
                                                     int     version)
{
    if (stream) {
        switch (version) { // switch on the version
            case 1: {
                int maxConnections;
                stream.getInt32(maxConnections);
                if (!stream || 0 > maxConnections) {
                    stream.invalidate();
                    return stream;
                }

                int maxIoThreads;
                stream.getInt32(maxIoThreads);
                if (!stream || 0 > maxIoThreads) {
                    stream.invalidate();
                    return stream;
                }

                int maxWriteCache;
                stream.getInt32(maxWriteCache);
                if (!stream || 0 > maxWriteCache) {
                    stream.invalidate();
                    return stream;
                }

                double readTimeout;
                stream.getFloat64(readTimeout);

                if (!stream || 0 > readTimeout) {
                    stream.invalidate();
                    return stream;
                }

                double metricsInterval;
                stream.getFloat64(metricsInterval);

                if (!stream || 0 > metricsInterval) {
                    stream.invalidate();
                    return stream;
                }

                int messageSizeOut;
                stream.getInt32(messageSizeOut);
                if (!stream || 0 >= messageSizeOut) {
                    stream.invalidate();
                    return stream;
                }

                int messageSizeIn;
                stream.getInt32(messageSizeIn);
                if (!stream || 0 >= messageSizeIn) {
                    stream.invalidate();
                    return stream;
                }

                int minProcThreads;
                stream.getInt32(minProcThreads);
                if (!stream || 0 >= minProcThreads) {
                    stream.invalidate();
                    return stream;
                }

                int maxProcThreads;
                stream.getInt32(maxProcThreads);
                if (!stream || minProcThreads > maxProcThreads) {
                    stream.invalidate();
                    return stream;
                }

                double idleTimeout;
                stream.getFloat64(idleTimeout);

                if (!stream || 0 > idleTimeout) {
                    stream.invalidate();
                    return stream;
                }

                d_maxConnections = maxConnections;
                d_maxIoThreads = maxIoThreads;
                d_maxWriteCache = maxWriteCache;
                d_readTimeout = readTimeout;
                d_metricsInterval = metricsInterval;
                d_messageSizeOut = messageSizeOut;
                d_messageSizeIn = messageSizeIn;
                d_minProcThreads = minProcThreads;
                d_maxProcThreads = maxProcThreads;
                d_idleTimeout = idleTimeout;
            } break;
            default: {
                stream.invalidate();
            }
        }
    }
    return stream;
}

// ACCESSORS
inline
int btemt_QueryProcessorConfiguration::incomingMessageSize() const
{
    return d_messageSizeIn;
}

inline
int btemt_QueryProcessorConfiguration::outgoingMessageSize() const
{
    return d_messageSizeOut;
}

inline
int btemt_QueryProcessorConfiguration::maxConnections() const
{
    return d_maxConnections;
}

inline
int btemt_QueryProcessorConfiguration::maxIoThreads() const
{
    return d_maxIoThreads;
}

inline
int btemt_QueryProcessorConfiguration::maxWriteCache() const
{
    return d_maxWriteCache;
}

inline
const double& btemt_QueryProcessorConfiguration::metricsInterval() const
{
    return d_metricsInterval;
}

inline
const double& btemt_QueryProcessorConfiguration::readTimeout() const
{
    return d_readTimeout;
}

inline
int btemt_QueryProcessorConfiguration::minProcessingThreads() const
{
    return d_minProcThreads;
}

inline
int btemt_QueryProcessorConfiguration::maxProcessingThreads() const {
    return d_maxProcThreads;
}

inline
double btemt_QueryProcessorConfiguration::idleTimeout() const {
    return d_idleTimeout;
}

inline
bsl::ostream& btemt_QueryProcessorConfiguration::
                                          streamOut(bsl::ostream& stream) const
{
    return stream << *this;
}

template <class STREAM>
STREAM& btemt_QueryProcessorConfiguration::bdexStreamOut(STREAM& stream,
                                                         int     version) const
{
    switch (version) {
      case 1: {
        stream.putInt32(d_maxConnections);
        stream.putInt32(d_maxIoThreads);
        stream.putInt32(d_maxWriteCache);

        stream.putFloat64(d_readTimeout);
        stream.putFloat64(d_metricsInterval);

        stream.putInt32(d_messageSizeOut);
        stream.putInt32(d_messageSizeIn);

        stream.putInt32(d_minProcThreads);
        stream.putInt32(d_maxProcThreads);
        stream.putFloat64(d_idleTimeout);

      } break;
    }
    return stream;
}

// FREE OPERATORS
inline
bool operator==(const btemt_QueryProcessorConfiguration& lhs,
                const btemt_QueryProcessorConfiguration& rhs)
{
    return
        lhs.d_maxConnections    == rhs.d_maxConnections    &&
        lhs.d_maxIoThreads      == rhs.d_maxIoThreads      &&
        lhs.d_maxWriteCache     == rhs.d_maxWriteCache     &&
        lhs.d_readTimeout       == rhs.d_readTimeout       &&
        lhs.d_metricsInterval   == rhs.d_metricsInterval   &&
        lhs.d_messageSizeOut    == rhs.d_messageSizeOut    &&
        lhs.d_messageSizeIn     == rhs.d_messageSizeIn     &&
        lhs.d_minProcThreads    == rhs.d_minProcThreads    &&
        lhs.d_maxProcThreads    == rhs.d_maxProcThreads    &&
        lhs.d_idleTimeout       == rhs.d_idleTimeout;
}

inline
bool operator!=(const btemt_QueryProcessorConfiguration& lhs,
                const btemt_QueryProcessorConfiguration& rhs)
{
    return !(lhs == rhs);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
