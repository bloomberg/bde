// btemt_channelpoolconfiguration.h                                   -*-C++-*-
#ifndef INCLUDED_BTEMT_CHANNELPOOLCONFIGURATION
#define INCLUDED_BTEMT_CHANNELPOOLCONFIGURATION

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a constrained-attribute class for channel pools.
//
//@AUTHOR: Tom Marshall (tmarshal)
//
//@CLASSES:
// btemt_ChannelPoolConfiguration: configuration parameters for a channel pool
//
//@DESCRIPTION: This component provides a constrained-attribute class that
// contains a set of attributes (objects and parameters) of particular use to
// channel pools.  The constraints are actively maintained by the class.  In
// particular, the "set" methods for constrained values will fail if their
// arguments are not consistent with the constraints.  Also, the constructor
// does not take any constrained arguments, but rather sets those values to
// valid defaults unconditionally.  This behavior avoids "silent failures",
// since the constructor cannot explicitly return a status value.
//
// The attributes contained by a 'btemt_ChannelPoolConfiguration' object and
// the attribute constraints are given, respectively, in two tables below.  The
// attributes are as follows:
//..
//   TYPE             NAME                DESCRIPTION
//   --------------   --------------      --------------------------------
//   int              maxConnections      maximum number of connections
//                                        that can be managed by a channel
//                                        pool.
//   int              maxThreads          maximum number of threads managed
//                                        by a channel pool
//   double           readTimeout         timeout for "read" operations; if
//                                        this value is 0, the read timeout
//                                        will be disabled
//   double           metricsInterval     periodic-update interval for metrics
//   int              minMessageSizeOut   output message strategy hint
//   int              typMessageSizeOut   output message strategy hint
//   int              maxMessageSizeOut   output message strategy hint
//   int              minMessageSizeIn    input message strategy hint
//   int              typMessageSizeIn    input message strategy hint
//   int              maxMessageSizeIn    input message strategy hint
//   int              writeCacheLowWat    High and low watermarks (in
//   int              writeCacheHiWat     bytes) for a channel's write
//                                        cache.  Once high watermark
//                                        is reached, the channel pool
//                                        will no longer accept messages
//                                        for the channel until there
//                                        is write space available.
//                                        A channel state callback will
//                                        result once the cached data size
//                                        is lower than the low watermark
//                                        value.
//   int              threadStackSize     the stack size of threads
//                                        managed by this pool in bytes.
//   bool             collectTimeMetrics  indicates whether the configured
//                                        channel pool will collect metrics.
//                                        If this value is 'true', the channel
//                                        pool will collect metrics
//                                        categorizing the time spent
//                                        processing data, and if this value
//                                        is 'false', those metrics will not
//                                        be collected.  The default value is
//                                        'true'.
//
// DEPRECATED (no longer used by channel pool):
//
//   int              workloadThreshold   threshold of the workload for an
//                                        event manager after which
//                                        a new thread will be created,
//                                        or an alert generated if
//                                        no new thread can be created.
//..
// The constraints are as follows:
//..
//    NAME                 CONSTRAINT
//   +--------------------+---------------------------------------------+
//   | maxConnections     | 0 <= maxConnections                         |
//   +--------------------+---------------------------------------------+
//   | maxThreads         | 0 <= maxThreads                             |
//   +--------------------+---------------------------------------------+
//   | writeCacheLowWat   | 0 <= writeCacheLowWat                       |
//   | writeCacheHiWat    | writeCacheLowWat <= writeCacheLowWat        |
//   +--------------------+---------------------------------------------+
//   | readTimeout        | 0 <= readTimeout                            |
//   +--------------------+---------------------------------------------+
//   | metricsInterval    | 0 <= metricsInterval                        |
//   +--------------------+---------------------------------------------+
//   | minMessageSizeOut  | 0 <= minMessageSizeOut <= typMessageSizeOut |
//   | typMessageSizeOut  |   <= maxMessageSizeOut                      |
//   | maxMessageSizeOut  |                                             |
//   +--------------------+---------------------------------------------+
//   | minMessageSizeIn   | 0 <= minMessageSizeIn <= typMessageSizeIn   |
//   | typMessageSizeIn   |   <= maxMessageSizeIn                       |
//   | maxMessageSizeIn   |                                             |
//   +--------------------+---------------------------------------------+
//   | threadStackSize    | 0 <= threadStackSize                        |
//   +--------------------+---------------------------------------------+
//   | workloadThreshold  | 0 <= workloadThreshold                      |
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
// 'btemt_ChannelPoolConfiguration' object.  First, create a configuration
// object 'cpc'.  Note that it is necessarily configured to valid but
// unpublished defaults.
//..
//    btemt_ChannelPoolConfiguration cpc;
//..
// Next, set each attribute.  Note that each of the "in" and "out" message size
// triplets must be set atomically (i.e., with a single three-argument "set"
// method).  Note also that each "set" method will fail if the argument or
// triplet of arguments is not valid, and so each method returns a status
// value.
//..
//    assert(0 == cpc.setIncomingMessageSizes(1, 2, 3));
//    assert(1 == cpc.minIncomingMessageSize());
//    assert(2 == cpc.typicalIncomingMessageSize());
//    assert(3 == cpc.maxIncomingMessageSize());
//
//    assert(0 == cpc.setOutgoingMessageSizes(4, 5, 6));
//    assert(4 == cpc.minOutgoingMessageSize());
//    assert(5 == cpc.typicalOutgoingMessageSize());
//    assert(6 == cpc.maxOutgoingMessageSize());
//
//    assert(0 == cpc.setMaxConnections(100));
//    assert(100 == cpc.maxConnections());
//
//    assert(0 == cpc.setMaxThreads(200));
//    assert(200 == cpc.maxThreads());
//
//    assert(0 == cpc.setWriteCacheWatermarks(0, 1024));
//    assert(0 == cpc.writeCacheLowWatermark());
//    assert(1024 == cpc.writeCacheHiWatermark());
//
//    assert(0 == cpc.setReadTimeout(3.5));
//    assert(3.5 == cpc.readTimeout());
//
//    assert(0 == cpc.setMetricsInterval(5.25));
//    assert(5.25 == cpc.metricsInterval());
//
//    assert(0 == cpc.setThreadStackSize(1024));
//    assert(1024 == cpc.threadStackSize());
//
//..
// The configuration object is now validly configured with our choice of
// parameters.  If, however, we attempt to set an invalid configuration, the
// "set" method will fail (with a non-zero return status), and the
// configuration will be left unchanged.
//..
//    assert(0 != cpc.setIncomingMessageSizes(8, 4, 256));
//    assert(1 == cpc.minIncomingMessageSize());
//    assert(2 == cpc.typicalIncomingMessageSize());
//    assert(3 == cpc.maxIncomingMessageSize());
//..
// Finally, we can print the configuration value to 'stdout'.
//..
//    cout << cpc;
//..
// This produces the following (multi-line) output:
//..
// [
//         maxConnections         : 100
//         maxThreads             : 200
//         writeCacheLowWat       : 0
//         writeCacheHiWat        : 1024
//         readTimeout            : (3, 500000000)
//         metricsInterval        : (5, 250000000)
//         minOutgoingMessageSize : 4
//         typOutgoingMessageSize : 5
//         maxOutgoingMessageSize : 6
//         minIncomingMessageSize : 1
//         typIncomingMessageSize : 2
//         maxIncomingMessageSize : 3
//         threadStackSize        : 1024
//         collectTimeMetrics     : 1
// ]
//..

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#include <bdeat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#include <bdeat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>
#endif

#ifndef INCLUDED_BDEUT_NULLABLEVALUE
#include <bdeut_nullablevalue.h>
#endif

namespace BloombergLP {

class btemt_Message;

                   // ====================================
                   // class btemt_ChannelPoolConfiguration
                   // ====================================

class btemt_ChannelPoolConfiguration {
    // This class provides constrained configuration parameters for a channel
    // pool.  The constraints are maintained as class invariants; it is not
    // possible to obtain an invalid object through this interface.  See the
    // component-level documentation for a description of the channel pool
    // attributes.
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
    enum { BTEMT_DEFAULT_THREAD_STACK_SIZE  = 1024 * 1024 };

    // Resource limits
    int                   d_maxConnections;    // maximum number of connections
                                               // that can be managed by a
                                               // channel pool.
    int                   d_maxThreads;        // maximum number of threads
                                               // managed by a channel pool

    int                   d_writeCacheLowWat;  // watermarks for the write
    int                   d_writeCacheHiWat;   // buffer for a managed channel

    // Timeouts
    double                d_readTimeout;
    double                d_metricsInterval;   // periodic-update interval for
                                               // metrics

    // Message sizes for incoming and outgoing messages and the configuration
    // flags indicating the strategy that should be used.  The "typical" size
    // must be between the minimum and the maximum sizes.

    int                   d_minMessageSizeOut;
    int                   d_typMessageSizeOut;
    int                   d_maxMessageSizeOut;

    int                   d_minMessageSizeIn;
    int                   d_typMessageSizeIn;
    int                   d_maxMessageSizeIn;

    int                   d_threadStackSize;   // stack size

    bool                  d_collectTimeMetrics;

    bdeut_NullableValue<int> d_numNewThreads;

    friend bsl::ostream& operator<<(bsl::ostream&,
                         const btemt_ChannelPoolConfiguration&);

    friend bool operator==(const btemt_ChannelPoolConfiguration&,
                           const btemt_ChannelPoolConfiguration&);

  public:
    // TYPES
    enum {
        NUM_ATTRIBUTES = 15 // the number of attributes in this class
    };

    enum {
        ATTRIBUTE_INDEX_MAX_CONNECTIONS      = 0,
            // index for 'MaxConnections' attribute
        ATTRIBUTE_INDEX_MAX_THREADS          = 1,
            // index for 'MaxThreads' attribute
        ATTRIBUTE_INDEX_READ_TIMEOUT         = 2,
            // index for 'ReadTimeout' attribute
        ATTRIBUTE_INDEX_METRICS_INTERVAL     = 3,
            // index for 'MetricsInterval' attribute
        ATTRIBUTE_INDEX_MIN_MESSAGE_SIZE_OUT = 4,
            // index for 'MinMessageSizeOut' attribute
        ATTRIBUTE_INDEX_TYP_MESSAGE_SIZE_OUT = 5,
            // index for 'TypMessageSizeOut' attribute
        ATTRIBUTE_INDEX_MAX_MESSAGE_SIZE_OUT = 6,
            // index for 'MaxMessageSizeOut' attribute
        ATTRIBUTE_INDEX_MIN_MESSAGE_SIZE_IN  = 7,
            // index for 'MinMessageSizeIn' attribute
        ATTRIBUTE_INDEX_TYP_MESSAGE_SIZE_IN  = 8,
            // index for 'TypMessageSizeIn' attribute
        ATTRIBUTE_INDEX_MAX_MESSAGE_SIZE_IN  = 9,
            // index for 'MaxMessageSizeIn' attribute
        ATTRIBUTE_INDEX_WRITE_CACHE_LOW_WAT  = 10,
            // index for 'WriteCacheLowWat' attribute
        ATTRIBUTE_INDEX_WRITE_CACHE_HI_WAT   = 11,
            // index for 'WriteCacheHiWat' attribute
        ATTRIBUTE_INDEX_THREAD_STACK_SIZE    = 12,
            // index for 'ThreadStackSize' attribute
        ATTRIBUTE_INDEX_COLLECT_TIME_METRICS = 13,
            // index for 'CollectTimeMetrics' attribute
        ATTRIBUTE_INDEX_NUM_NEW_THREADS      = 14
            // index for 'NumNewThreads' attribute
    };

    enum {
        ATTRIBUTE_ID_MAX_CONNECTIONS         = 1,
            // id for 'MaxConnections' attribute
        ATTRIBUTE_ID_MAX_THREADS             = 2,
            // id for 'MaxThreads' attribute
        ATTRIBUTE_ID_READ_TIMEOUT            = 3,
            // id for 'ReadTimeout' attribute
        ATTRIBUTE_ID_METRICS_INTERVAL        = 4,
            // id for 'MetricsInterval' attribute
        ATTRIBUTE_ID_MIN_MESSAGE_SIZE_OUT    = 5,
            // id for 'MinMessageSizeOut' attribute
        ATTRIBUTE_ID_TYP_MESSAGE_SIZE_OUT    = 6,
            // id for 'TypMessageSizeOut' attribute
        ATTRIBUTE_ID_MAX_MESSAGE_SIZE_OUT    = 7,
            // id for 'MaxMessageSizeOut' attribute
        ATTRIBUTE_ID_MIN_MESSAGE_SIZE_IN     = 8,
            // id for 'MinMessageSizeIn' attribute
        ATTRIBUTE_ID_TYP_MESSAGE_SIZE_IN     = 9,
            // id for 'TypMessageSizeIn' attribute
        ATTRIBUTE_ID_MAX_MESSAGE_SIZE_IN     = 10,
            // id for 'MaxMessageSizeIn' attribute
        ATTRIBUTE_ID_WRITE_CACHE_LOW_WAT     = 11,
            // id for 'WriteCacheLowWat' attribute
        ATTRIBUTE_ID_WRITE_CACHE_HI_WAT      = 12,
            // id for 'WriteCacheHiWat' attribute
        ATTRIBUTE_ID_THREAD_STACK_SIZE       = 13,
            // id for 'ThreadStackSize' attribute
        ATTRIBUTE_ID_COLLECT_TIME_METRICS    = 14,
            // id for 'CollectTimeMetrics' attribute
        ATTRIBUTE_ID_NUM_NEW_THREADS      = 15
            // id for 'NumNewThreads' attribute
    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class

    static const bdeat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(btemt_ChannelPoolConfiguration,
                                  bdeat_TypeTraitBasicSequence,
                                  bdeu_TypeTraitHasPrintMethod);

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
        // *DEPRECATED* Replaced by 'maxSupportedBdexVersion()'

    static const bdeat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdeat_AttributeInfo *lookupAttributeInfo(
                                                       const char *name,
                                                       int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    btemt_ChannelPoolConfiguration();
        // Create a channel pool configuration constrained-attribute object
        // having valid default values for all attributes.

    btemt_ChannelPoolConfiguration(
                              const btemt_ChannelPoolConfiguration& original);
        // Create a channel pool configuration constrained-attribute object
        // having the value of the specified 'original' object.

    ~btemt_ChannelPoolConfiguration();
        // Destroy this channel pool configuration constrained-attribute
        // object.

    // MANIPULATORS
    btemt_ChannelPoolConfiguration&
                          operator=(const btemt_ChannelPoolConfiguration& rhs);
        // Assign to this channel pool configuration constrained-attribute
        // object the value of the specified 'rhs' object.

    int setIncomingMessageSizes(int min, int typical, int max);
        // Set the triplet of incoming-message-size attributes of this object
        // to the specified 'min', 'typical', and 'max' values if
        // 0 <= min <= typical <= max.  Return 0 on success, and a non-zero
        // value (with no effect on the state of this object) otherwise.

    int setOutgoingMessageSizes(int min, int typical, int max);
        // Set the triplet of outgoing-message-size attributes of this object
        // to the specified 'min', 'typical', and 'max' values if
        // 0 <= min <= typical <= max.  Return 0 on success, and a non-zero
        // value (with no effect on the state of this object) otherwise.

    int setMaxConnections(int maxConnections);
        // Set the maximum number of connections attribute of this object to
        // the specified 'maxConnections' if 0 <= maxConnections.  Return 0 on
        // success, and a non-zero value (with no effect on the state of this
        // object) otherwise.

    int setMaxThreads(int maxThreads);
        // Set the maximum number of threads attribute of this object to the
        // specified 'maxThreads' if 0 <= maxThreads.  Return 0 on success, and
        // a non-zero value (with no effect on the state of this object)
        // otherwise.

    int setNumNewThreads(int numNewThreads);
        // Set the number of new threads attribute of this object to the
        // specified 'numNewThreads' if '0 <= numNewThreads'.  Return 0 on
        // success, and a non-zero value (with no effect on the state of this
        // object) otherwise.

    int setMaxWriteCache(int numBytes);
        // Set the maximum write cache size attribute of this object to the
        // specified 'numBytes' if 0 <= numBytes.  Return 0 on success, and a
        // non-zero value (with no effect on the state of this object)
        // otherwise.
        //
        // *OBSOLETE* Use 'setWriteCacheWatermarks' instead.

   int setMetricsInterval(double metricsInterval);
        // Set the metrics interval attribute of this object to the specified
        // 'metricsInterval' value if 0 <= metricsInterval.  Return 0 on
        // success, and a non-zero value (with no effect on the state of this
        // object) otherwise.

    int setReadTimeout(double readTimeout);
        // Set the read timeout attribute of this object to the specified
        // 'readTimeout' value if 0 <= readTimeout.  Return 0 on success, and a
        // non-zero value (with no effect on the state of this object)
        // otherwise.  A value of 0 will disable the read timeout.

    int setThreadStackSize(int stackSize);
        // Set the thread stack size attribute of this object to the specified
        // 'stackSize' value if 0 <= stackSize.  Return 0 on success, and a
        // non-zero value (with no effect on the state of this object)
        // otherwise.

    int setWorkloadThreshold(int threshold);
        // Set the workload threshold attribute of this object to the specified
        // 'threshold' value if 0 <= threshold.  Return 0 on success, and a
        // non-zero value (with no effect on the state of this object)
        // otherwise.
        //
        // *DEPRECATED* Workload thresholds are now ignored by the channel
        // pool, since all threads are statically created.

    int setWriteCacheWatermarks(int lowWatermark, int hiWatermark);
        // Set the write cache watermarks to specified 'lowWatermark' and
        // 'hiWatermark' values.  Return 0 on success, and a non-zero value
        // (with no effect on the state of this object) otherwise.

    int setCollectTimeMetrics(bool collectTimeMetricsFlag);
        // Set to the specified 'collectTimeMetricsFlag' whether the configured
        // channel pool will collect time metrics.  Return 0.  If
        // 'collectTimeMetricsFlag' is 'true' the configured channel pool will
        // collect metrics categorizing the time spent processing data.  Note
        // that, if this value is 'false', the channel pool cannot use that
        // estimate of work-load when it attempts to distribute work amongst
        // its managed threads.

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

    template<class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the last
        // invocation of 'manipulator' (i.e., the invocation that terminated
        // the sequence).

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int id);
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'id', supplying
        // 'manipulator' with the corresponding attribute information
        // structure.  Return the value returned from the invocation of
        // 'manipulator' if 'id' identifies an attribute of this class, and -1
        // otherwise.

    template<class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *name,
                            int           nameLength);
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'name' of the
        // specified 'nameLength', supplying 'manipulator' with the
        // corresponding attribute information structure.  Return the value
        // returned from the invocation of 'manipulator' if 'name' identifies
        // an attribute of this class, and -1 otherwise.

    // ACCESSORS
    int minIncomingMessageSize() const;
        // Return the minimum incoming-message-size attribute of this object.

    int typicalIncomingMessageSize() const;
        // Return the typical incoming-message-size attribute of this object.

    int maxIncomingMessageSize() const;
        // Return the maximum incoming-message-size attribute of this object.

    int minOutgoingMessageSize() const;
        // Return the minimum outgoing-message-size attribute of this object.

    int typicalOutgoingMessageSize() const;
        // Return the typical outgoing-message-size attribute of this object.

    int maxOutgoingMessageSize() const;
        // Return the maximum outgoing-message-size attribute of this object.

    int maxConnections() const;
        // Return the maximum number of connections attribute of this object.

    int maxThreads() const;
        // Return the maximum number of threads attribute of this object.

    const bdeut_NullableValue<int>& numNewThreads() const;
        // Return a reference to the non-modifiable number of new threads
        // attribute of this object.

    bool collectTimeMetrics() const;
        // Return 'true' if the configured channel pool will collect time
        // metrics, and 'false' otherwise.  If this value is 'true', the
        // channel pool will collect metrics categorizing the time spent
        // processing data.  Note that, if this value is 'false', the channel
        // pool cannot use that estimate of work-load when it attempts to
        // distribute work amongst its managed threads.

    int maxWriteCache() const;
        // Return the maximum write cache size (in bytes) attribute of this
        // object.
        //
        // *OBSOLETE* Use 'writeCacheHiWatermark' instead.

    const double& metricsInterval() const;
        // Return the metrics interval attribute of this object.

    const double& readTimeout() const;
        // Return the read timeout attribute of this object.  A value of 0
        // indicates the read timeout should be disabled.

    int workloadThreshold() const;
        // Return the workload threshold attribute of this object.
        //
        // *DEPRECATED* Workload thresholds are now ignored by the channel
        // pool, since all threads are statically created.

    int writeCacheLowWatermark() const;
    int writeCacheHiWatermark() const;
        // Return the watermarks for the write cache.

    int threadStackSize() const;
        // Return the thread stack size attribute of this object.

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

    template<class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor' with
        // the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the last
        // invocation of 'accessor' (i.e., the invocation that terminated the
        // sequence).

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int id) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute of
        // this object indicated by the specified 'id', supplying 'accessor'
        // with the corresponding attribute information structure.  Return the
        // value returned from the invocation of 'accessor' if 'id' identifies
        // an attribute of this class, and -1 otherwise.

    template<class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *name,
                        int         nameLength) const;
        // Invoke the specified 'accessor' on the (non-modifiable) attribute of
        // this object indicated by the specified 'name' of the specified
        // 'nameLength', supplying 'accessor' with the corresponding attribute
        // information structure.  Return the value returned from the
        // invocation of 'accessor' if 'name' identifies an attribute of this
        // class, and -1 otherwise.
};

// FREE OPERATORS
bool operator==(const btemt_ChannelPoolConfiguration& lhs,
                const btemt_ChannelPoolConfiguration& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects objects
    // have the same value if each respective attribute has the same value.

inline
bool operator!=(const btemt_ChannelPoolConfiguration& lhs,
                const btemt_ChannelPoolConfiguration& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

bsl::ostream& operator<<(bsl::ostream&                         output,
                         const btemt_ChannelPoolConfiguration& configuration);
    // Write the specified 'configuration' value to the specified 'output'
    // stream in a reasonable multi-line format.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                   // ------------------------------------
                   // class btemt_ChannelPoolConfiguration
                   // ------------------------------------
// MANIPULATORS
inline
int btemt_ChannelPoolConfiguration::setIncomingMessageSizes(int min,
                                                            int typical,
                                                            int max)
{
    if (0 <= min && min <= typical && typical <= max) {
        d_minMessageSizeIn = min;
        d_typMessageSizeIn = typical;
        d_maxMessageSizeIn = max;
        return 0;
    }
    return -1;
}

inline
int btemt_ChannelPoolConfiguration::setOutgoingMessageSizes(int min,
                                                            int typical,
                                                            int max)
{
    if (0 <= min && min <= typical && typical <= max) {
        d_minMessageSizeOut = min;
        d_typMessageSizeOut = typical;
        d_maxMessageSizeOut = max;
        return 0;
    }
    return -1;
}

inline
int btemt_ChannelPoolConfiguration::setMaxConnections(int maxConnections)
{
    if (0 <= maxConnections) {
        d_maxConnections = maxConnections;
        return 0;
    }
    return -1;
}

inline
int btemt_ChannelPoolConfiguration::setMaxThreads(int maxThreads)
{
    if (0 <= maxThreads) {
        d_maxThreads = maxThreads;
        return 0;
    }
    return -1;
}

inline
int btemt_ChannelPoolConfiguration::setNumNewThreads(int numNewThreads)
{
    if (0 <= numNewThreads) {
        d_numNewThreads = numNewThreads;
        return 0;
    }
    return -1;
}

inline
int btemt_ChannelPoolConfiguration::setMaxWriteCache(int numBytes)
{
    if (0 <= numBytes) {
        d_writeCacheHiWat = numBytes;
        return 0;
    }
    return -1;
}

inline
int btemt_ChannelPoolConfiguration::setThreadStackSize(
     int stackSize)
{
    if (0 <= stackSize) {
        d_threadStackSize = stackSize;
        return 0;
    }
    return -1;
}

inline
int btemt_ChannelPoolConfiguration::setWriteCacheWatermarks(
     int lowWatermark, int hiWatermark)
{
    if (0 <= lowWatermark && lowWatermark <= hiWatermark) {
        d_writeCacheLowWat = lowWatermark;
        d_writeCacheHiWat = hiWatermark;
        return 0;
    }
    return -1;
}

inline
int btemt_ChannelPoolConfiguration::setMetricsInterval(
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
btemt_ChannelPoolConfiguration::setReadTimeout(double readTimeout)
{
    if (0 <= readTimeout) {
        d_readTimeout = readTimeout;
        return 0;
    }
    return -1;
}

inline
int btemt_ChannelPoolConfiguration::setWorkloadThreshold(int threshold)
{
    return (0 <= threshold) ? 0 : 1;
}

inline
int btemt_ChannelPoolConfiguration::setCollectTimeMetrics(
                                                   bool collectTimeMetricsFlag)
{
    d_collectTimeMetrics = collectTimeMetricsFlag;
    return 0;
}

template <class STREAM>
STREAM& btemt_ChannelPoolConfiguration::bdexStreamIn(STREAM& stream,
                                                     int     version)
{
    if (stream) {
        switch (version) { // switch on the version
            case 6: {
                int maxConnections;
                stream.getInt32(maxConnections);
                if (!stream || 0 > maxConnections) {
                    stream.invalidate();
                    return stream;
                }

                int maxThreads;
                stream.getInt32(maxThreads);
                if (!stream || 0 > maxThreads) {
                    stream.invalidate();
                    return stream;
                }

                int writeCacheLowWat;
                stream.getInt32(writeCacheLowWat);
                if (!stream || 0 > writeCacheLowWat) {
                    stream.invalidate();
                    return stream;
                }

                int writeCacheHiWat;
                stream.getInt32(writeCacheHiWat);
                if (!stream ||  writeCacheHiWat < writeCacheLowWat) {
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

                int minMessageSizeOut;
                stream.getInt32(minMessageSizeOut);
                if (!stream || 0 > minMessageSizeOut) {
                    stream.invalidate();
                    return stream;
                }

                int typMessageSizeOut;
                stream.getInt32(typMessageSizeOut);
                if (!stream || minMessageSizeOut > typMessageSizeOut) {
                    stream.invalidate();
                    return stream;
                }

                int maxMessageSizeOut;
                stream.getInt32(maxMessageSizeOut);
                if (!stream || typMessageSizeOut > maxMessageSizeOut) {
                    stream.invalidate();
                    return stream;
                }

                int minMessageSizeIn;
                stream.getInt32(minMessageSizeIn);
                if (!stream || 0 > minMessageSizeIn) {
                    stream.invalidate();
                    return stream;
                }

                int typMessageSizeIn;
                stream.getInt32(typMessageSizeIn);
                if (!stream || minMessageSizeIn > typMessageSizeIn) {
                    stream.invalidate();
                    return stream;
                }

                int maxMessageSizeIn;
                stream.getInt32(maxMessageSizeIn);
                if (!stream || typMessageSizeIn > maxMessageSizeIn) {
                    stream.invalidate();
                    return stream;
                }

                int threadStackSize;
                stream.getInt32(threadStackSize);
                if (!stream || 0 > threadStackSize) {
                    stream.invalidate();
                    return stream;
                }

                unsigned char collectTimeMetrics;
                stream.getUint8(collectTimeMetrics);
                if (!stream) {
                    stream.invalidate();
                    return stream;
                }

                bdeut_NullableValue<int> numNewThreads;
                numNewThreads.bdexStreamIn(stream, 1);
                if (!stream) {
                    stream.invalidate();
                    return stream;
                }

                d_maxConnections     = maxConnections;
                d_maxThreads         = maxThreads;
                d_writeCacheLowWat   = writeCacheLowWat;
                d_writeCacheHiWat    = writeCacheHiWat;
                d_readTimeout        = readTimeout;
                d_metricsInterval    = metricsInterval;
                d_minMessageSizeOut  = minMessageSizeOut;
                d_typMessageSizeOut  = typMessageSizeOut;
                d_maxMessageSizeOut  = maxMessageSizeOut;
                d_minMessageSizeIn   = minMessageSizeIn;
                d_typMessageSizeIn   = typMessageSizeIn;
                d_maxMessageSizeIn   = maxMessageSizeIn;
                d_threadStackSize    = threadStackSize;
                d_collectTimeMetrics = collectTimeMetrics;
            } break;
            case 5: {
                int maxConnections;
                stream.getInt32(maxConnections);
                if (!stream || 0 > maxConnections) {
                    stream.invalidate();
                    return stream;
                }

                int maxThreads;
                stream.getInt32(maxThreads);
                if (!stream || 0 > maxThreads) {
                    stream.invalidate();
                    return stream;
                }

                int writeCacheLowWat;
                stream.getInt32(writeCacheLowWat);
                if (!stream || 0 > writeCacheLowWat) {
                    stream.invalidate();
                    return stream;
                }

                int writeCacheHiWat;
                stream.getInt32(writeCacheHiWat);
                if (!stream ||  writeCacheHiWat < writeCacheLowWat) {
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

                int minMessageSizeOut;
                stream.getInt32(minMessageSizeOut);
                if (!stream || 0 > minMessageSizeOut) {
                    stream.invalidate();
                    return stream;
                }

                int typMessageSizeOut;
                stream.getInt32(typMessageSizeOut);
                if (!stream || minMessageSizeOut > typMessageSizeOut) {
                    stream.invalidate();
                    return stream;
                }

                int maxMessageSizeOut;
                stream.getInt32(maxMessageSizeOut);
                if (!stream || typMessageSizeOut > maxMessageSizeOut) {
                    stream.invalidate();
                    return stream;
                }

                int minMessageSizeIn;
                stream.getInt32(minMessageSizeIn);
                if (!stream || 0 > minMessageSizeIn) {
                    stream.invalidate();
                    return stream;
                }

                int typMessageSizeIn;
                stream.getInt32(typMessageSizeIn);
                if (!stream || minMessageSizeIn > typMessageSizeIn) {
                    stream.invalidate();
                    return stream;
                }

                int maxMessageSizeIn;
                stream.getInt32(maxMessageSizeIn);
                if (!stream || typMessageSizeIn > maxMessageSizeIn) {
                    stream.invalidate();
                    return stream;
                }

                int threadStackSize;
                stream.getInt32(threadStackSize);
                if (!stream || 0 > threadStackSize) {
                    stream.invalidate();
                    return stream;
                }

                unsigned char collectTimeMetrics;
                stream.getUint8(collectTimeMetrics);
                if (!stream) {
                    stream.invalidate();
                    return stream;
                }

                d_maxConnections     = maxConnections;
                d_maxThreads         = maxThreads;
                d_writeCacheLowWat   = writeCacheLowWat;
                d_writeCacheHiWat    = writeCacheHiWat;
                d_readTimeout        = readTimeout;
                d_metricsInterval    = metricsInterval;
                d_minMessageSizeOut  = minMessageSizeOut;
                d_typMessageSizeOut  = typMessageSizeOut;
                d_maxMessageSizeOut  = maxMessageSizeOut;
                d_minMessageSizeIn   = minMessageSizeIn;
                d_typMessageSizeIn   = typMessageSizeIn;
                d_maxMessageSizeIn   = maxMessageSizeIn;
                d_threadStackSize    = threadStackSize;
                d_collectTimeMetrics = collectTimeMetrics;
            } break;
            case 4: {
                int maxConnections;
                stream.getInt32(maxConnections);
                if (!stream || 0 > maxConnections) {
                    stream.invalidate();
                    return stream;
                }

                int maxThreads;
                stream.getInt32(maxThreads);
                if (!stream || 0 > maxThreads) {
                    stream.invalidate();
                    return stream;
                }

                int writeCacheLowWat;
                stream.getInt32(writeCacheLowWat);
                if (!stream || 0 > writeCacheLowWat) {
                    stream.invalidate();
                    return stream;
                }

                int writeCacheHiWat;
                stream.getInt32(writeCacheHiWat);
                if (!stream ||  writeCacheHiWat < writeCacheLowWat) {
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

                int minMessageSizeOut;
                stream.getInt32(minMessageSizeOut);
                if (!stream || 0 > minMessageSizeOut) {
                    stream.invalidate();
                    return stream;
                }

                int typMessageSizeOut;
                stream.getInt32(typMessageSizeOut);
                if (!stream || minMessageSizeOut > typMessageSizeOut) {
                    stream.invalidate();
                    return stream;
                }

                int maxMessageSizeOut;
                stream.getInt32(maxMessageSizeOut);
                if (!stream || typMessageSizeOut > maxMessageSizeOut) {
                    stream.invalidate();
                    return stream;
                }

                int minMessageSizeIn;
                stream.getInt32(minMessageSizeIn);
                if (!stream || 0 > minMessageSizeIn) {
                    stream.invalidate();
                    return stream;
                }

                int typMessageSizeIn;
                stream.getInt32(typMessageSizeIn);
                if (!stream || minMessageSizeIn > typMessageSizeIn) {
                    stream.invalidate();
                    return stream;
                }

                int maxMessageSizeIn;
                stream.getInt32(maxMessageSizeIn);
                if (!stream || typMessageSizeIn > maxMessageSizeIn) {
                    stream.invalidate();
                    return stream;
                }

                int threadStackSize;
                stream.getInt32(threadStackSize);
                if (!stream || 0 > threadStackSize) {
                    stream.invalidate();
                    return stream;
                }

                d_maxConnections     = maxConnections;
                d_maxThreads         = maxThreads;
                d_writeCacheLowWat   = writeCacheLowWat;
                d_writeCacheHiWat    = writeCacheHiWat;
                d_readTimeout        = readTimeout;
                d_metricsInterval    = metricsInterval;
                d_minMessageSizeOut  = minMessageSizeOut;
                d_typMessageSizeOut  = typMessageSizeOut;
                d_maxMessageSizeOut  = maxMessageSizeOut;
                d_minMessageSizeIn   = minMessageSizeIn;
                d_typMessageSizeIn   = typMessageSizeIn;
                d_maxMessageSizeIn   = maxMessageSizeIn;
                d_threadStackSize    = threadStackSize;
                d_collectTimeMetrics = true;

            } break;
            case 3: {
                int maxConnections;
                stream.getInt32(maxConnections);
                if (!stream || 0 > maxConnections) {
                    stream.invalidate();
                    return stream;
                }

                int maxThreads;
                stream.getInt32(maxThreads);
                if (!stream || 0 > maxThreads) {
                    stream.invalidate();
                    return stream;
                }

                int writeCacheLowWat;
                stream.getInt32(writeCacheLowWat);
                if (!stream || 0 > writeCacheLowWat) {
                    stream.invalidate();
                    return stream;
                }

                int writeCacheHiWat;
                stream.getInt32(writeCacheHiWat);
                if (!stream ||  writeCacheHiWat < writeCacheLowWat) {
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

                int minMessageSizeOut;
                stream.getInt32(minMessageSizeOut);
                if (!stream || 0 > minMessageSizeOut) {
                    stream.invalidate();
                    return stream;
                }

                int typMessageSizeOut;
                stream.getInt32(typMessageSizeOut);
                if (!stream || minMessageSizeOut > typMessageSizeOut) {
                    stream.invalidate();
                    return stream;
                }

                int maxMessageSizeOut;
                stream.getInt32(maxMessageSizeOut);
                if (!stream || typMessageSizeOut > maxMessageSizeOut) {
                    stream.invalidate();
                    return stream;
                }

                int minMessageSizeIn;
                stream.getInt32(minMessageSizeIn);
                if (!stream || 0 > minMessageSizeIn) {
                    stream.invalidate();
                    return stream;
                }

                int typMessageSizeIn;
                stream.getInt32(typMessageSizeIn);
                if (!stream || minMessageSizeIn > typMessageSizeIn) {
                    stream.invalidate();
                    return stream;
                }

                int maxMessageSizeIn;
                stream.getInt32(maxMessageSizeIn);
                if (!stream || typMessageSizeIn > maxMessageSizeIn) {
                    stream.invalidate();
                    return stream;
                }

                d_maxConnections     = maxConnections;
                d_maxThreads         = maxThreads;
                d_writeCacheLowWat   = writeCacheLowWat;
                d_writeCacheHiWat    = writeCacheHiWat;
                d_readTimeout        = readTimeout;
                d_metricsInterval    = metricsInterval;
                d_minMessageSizeOut  = minMessageSizeOut;
                d_typMessageSizeOut  = typMessageSizeOut;
                d_maxMessageSizeOut  = maxMessageSizeOut;
                d_minMessageSizeIn   = minMessageSizeIn;
                d_typMessageSizeIn   = typMessageSizeIn;
                d_maxMessageSizeIn   = maxMessageSizeIn;
                d_threadStackSize    = BTEMT_DEFAULT_THREAD_STACK_SIZE;
                d_collectTimeMetrics = true;

            } break;
            case 2: {
                int maxConnections;
                stream.getInt32(maxConnections);
                if (!stream || 0 > maxConnections) {
                    stream.invalidate();
                    return stream;
                }

                int maxThreads;
                stream.getInt32(maxThreads);
                if (!stream || 0 > maxThreads) {
                    stream.invalidate();
                    return stream;
                }

                int writeCacheLowWat;
                stream.getInt32(writeCacheLowWat);
                if (!stream || 0 > writeCacheLowWat) {
                    stream.invalidate();
                    return stream;
                }

                int writeCacheHiWat;
                stream.getInt32(writeCacheHiWat);
                if (!stream ||  writeCacheHiWat < writeCacheLowWat) {
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

                int minMessageSizeOut;
                stream.getInt32(minMessageSizeOut);
                if (!stream || 0 > minMessageSizeOut) {
                    stream.invalidate();
                    return stream;
                }

                int typMessageSizeOut;
                stream.getInt32(typMessageSizeOut);
                if (!stream || minMessageSizeOut > typMessageSizeOut) {
                    stream.invalidate();
                    return stream;
                }

                int maxMessageSizeOut;
                stream.getInt32(maxMessageSizeOut);
                if (!stream || typMessageSizeOut > maxMessageSizeOut) {
                    stream.invalidate();
                    return stream;
                }

                int minMessageSizeIn;
                stream.getInt32(minMessageSizeIn);
                if (!stream || 0 > minMessageSizeIn) {
                    stream.invalidate();
                    return stream;
                }

                int typMessageSizeIn;
                stream.getInt32(typMessageSizeIn);
                if (!stream || minMessageSizeIn > typMessageSizeIn) {
                    stream.invalidate();
                    return stream;
                }

                int maxMessageSizeIn;
                stream.getInt32(maxMessageSizeIn);
                if (!stream || typMessageSizeIn > maxMessageSizeIn) {
                    stream.invalidate();
                    return stream;
                }

                int threshold;
                stream.getInt32(threshold);
                if (!stream || 0 > threshold) {
                    stream.invalidate();
                    return stream;
                }

                d_maxConnections     = maxConnections;
                d_maxThreads         = maxThreads;
                d_writeCacheLowWat   = writeCacheLowWat;
                d_writeCacheHiWat    = writeCacheHiWat;
                d_readTimeout        = readTimeout;
                d_metricsInterval    = metricsInterval;
                d_minMessageSizeOut  = minMessageSizeOut;
                d_typMessageSizeOut  = typMessageSizeOut;
                d_maxMessageSizeOut  = maxMessageSizeOut;
                d_minMessageSizeIn   = minMessageSizeIn;
                d_typMessageSizeIn   = typMessageSizeIn;
                d_maxMessageSizeIn   = maxMessageSizeIn;
                d_threadStackSize    = BTEMT_DEFAULT_THREAD_STACK_SIZE;
                d_collectTimeMetrics = true;
                (void) threshold; // *DEPRECATED*  Ignored

            } break;
            case 1: {
                int maxConnections;
                stream.getInt32(maxConnections);
                if (!stream || 0 > maxConnections) {
                    stream.invalidate();
                    return stream;
                }

                int maxThreads;
                stream.getInt32(maxThreads);
                if (!stream || 0 > maxThreads) {
                    stream.invalidate();
                    return stream;
                }

                int writeCacheLowWat = 0;
                int writeCacheHiWat;
                stream.getInt32(writeCacheHiWat);
                if (!stream ||  writeCacheHiWat < writeCacheLowWat) {
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

                int minMessageSizeOut;
                stream.getInt32(minMessageSizeOut);
                if (!stream || 0 > minMessageSizeOut) {
                    stream.invalidate();
                    return stream;
                }

                int typMessageSizeOut;
                stream.getInt32(typMessageSizeOut);
                if (!stream || minMessageSizeOut > typMessageSizeOut) {
                    stream.invalidate();
                    return stream;
                }

                int maxMessageSizeOut;
                stream.getInt32(maxMessageSizeOut);
                if (!stream || typMessageSizeOut > maxMessageSizeOut) {
                    stream.invalidate();
                    return stream;
                }

                int minMessageSizeIn;
                stream.getInt32(minMessageSizeIn);
                if (!stream || 0 > minMessageSizeIn) {
                    stream.invalidate();
                    return stream;
                }

                int typMessageSizeIn;
                stream.getInt32(typMessageSizeIn);
                if (!stream || minMessageSizeIn > typMessageSizeIn) {
                    stream.invalidate();
                    return stream;
                }

                int maxMessageSizeIn;
                stream.getInt32(maxMessageSizeIn);
                if (!stream || typMessageSizeIn > maxMessageSizeIn) {
                    stream.invalidate();
                    return stream;
                }

                int threshold;
                stream.getInt32(threshold);
                if (!stream || 0 > threshold) {
                    stream.invalidate();
                    return stream;
                }

                d_maxConnections     = maxConnections;
                d_maxThreads         = maxThreads;
                d_writeCacheLowWat   = writeCacheLowWat;
                d_writeCacheHiWat    = writeCacheHiWat;
                d_readTimeout        = readTimeout;
                d_metricsInterval    = metricsInterval;
                d_minMessageSizeOut  = minMessageSizeOut;
                d_typMessageSizeOut  = typMessageSizeOut;
                d_maxMessageSizeOut  = maxMessageSizeOut;
                d_minMessageSizeIn   = minMessageSizeIn;
                d_typMessageSizeIn   = typMessageSizeIn;
                d_maxMessageSizeIn   = maxMessageSizeIn;
                d_threadStackSize    = BTEMT_DEFAULT_THREAD_STACK_SIZE;
                d_collectTimeMetrics = true;
                (void) threshold; // *DEPRECATED*  Ignored

            } break;
            default: {
                stream.invalidate();
            }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int btemt_ChannelPoolConfiguration::manipulateAttributes(
                                                      MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_maxConnections,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_CONNECTIONS]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_maxThreads,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_THREADS]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_readTimeout,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_READ_TIMEOUT]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_metricsInterval,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_METRICS_INTERVAL]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_minMessageSizeOut,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MIN_MESSAGE_SIZE_OUT]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_typMessageSizeOut,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TYP_MESSAGE_SIZE_OUT]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_maxMessageSizeOut,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_MESSAGE_SIZE_OUT]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_minMessageSizeIn,
                    ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MIN_MESSAGE_SIZE_IN]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_typMessageSizeIn,
                    ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TYP_MESSAGE_SIZE_IN]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_maxMessageSizeIn,
                    ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_MESSAGE_SIZE_IN]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_writeCacheLowWat,
                    ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_CACHE_LOW_WAT]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_writeCacheHiWat,
                     ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_CACHE_HI_WAT]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_threadStackSize,
                     ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_THREAD_STACK_SIZE]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_collectTimeMetrics,
                  ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COLLECT_TIME_METRICS]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = manipulator(&d_numNewThreads,
                      ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NUM_NEW_THREADS]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
int btemt_ChannelPoolConfiguration::manipulateAttribute(
                                                      MANIPULATOR& manipulator,
                                                      int          id)
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_MAX_CONNECTIONS: {
        return manipulator(&d_maxConnections,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_CONNECTIONS]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_MAX_THREADS: {
        return manipulator(&d_maxThreads,
                            ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_THREADS]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_READ_TIMEOUT: {
        return manipulator(&d_readTimeout,
                           ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_READ_TIMEOUT]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_METRICS_INTERVAL: {
        return manipulator(&d_metricsInterval,
                       ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_METRICS_INTERVAL]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_MIN_MESSAGE_SIZE_OUT: {
        return manipulator(&d_minMessageSizeOut,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MIN_MESSAGE_SIZE_OUT]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_TYP_MESSAGE_SIZE_OUT: {
        return manipulator(&d_typMessageSizeOut,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TYP_MESSAGE_SIZE_OUT]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_MAX_MESSAGE_SIZE_OUT: {
        return manipulator(&d_maxMessageSizeOut,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_MESSAGE_SIZE_OUT]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_MIN_MESSAGE_SIZE_IN: {
        return manipulator(&d_minMessageSizeIn,
                    ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MIN_MESSAGE_SIZE_IN]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_TYP_MESSAGE_SIZE_IN: {
        return manipulator(&d_typMessageSizeIn,
                    ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TYP_MESSAGE_SIZE_IN]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_MAX_MESSAGE_SIZE_IN: {
        return manipulator(&d_maxMessageSizeIn,
                    ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_MESSAGE_SIZE_IN]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_WRITE_CACHE_LOW_WAT: {
        return manipulator(&d_writeCacheLowWat,
                    ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_CACHE_LOW_WAT]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_WRITE_CACHE_HI_WAT: {
        return manipulator(&d_writeCacheHiWat,
                     ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_CACHE_HI_WAT]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_THREAD_STACK_SIZE: {
        return manipulator(&d_threadStackSize,
                     ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_THREAD_STACK_SIZE]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_COLLECT_TIME_METRICS: {
        return manipulator(&d_collectTimeMetrics,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COLLECT_TIME_METRICS]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_NUM_NEW_THREADS: {
        return manipulator(
                        &d_numNewThreads,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NUM_NEW_THREADS]);
                                                                    // RETURN
      } break;

      default:
        return NOT_FOUND;
    }
}

template <class MANIPULATOR>
inline
int btemt_ChannelPoolConfiguration::manipulateAttribute(
                                                     MANIPULATOR&  manipulator,
                                                     const char   *name,
                                                     int           nameLength)
{
    enum { NOT_FOUND = -1 };

    const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
    if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

// ACCESSORS
inline
int btemt_ChannelPoolConfiguration::minIncomingMessageSize() const
{
    return d_minMessageSizeIn;
}

inline
int btemt_ChannelPoolConfiguration::typicalIncomingMessageSize() const
{
    return d_typMessageSizeIn;
}

inline
int btemt_ChannelPoolConfiguration::maxIncomingMessageSize() const
{
    return d_maxMessageSizeIn;
}

inline
int btemt_ChannelPoolConfiguration::minOutgoingMessageSize() const
{
    return d_minMessageSizeOut;
}

inline
int btemt_ChannelPoolConfiguration::typicalOutgoingMessageSize() const
{
    return d_typMessageSizeOut;
}

inline
int btemt_ChannelPoolConfiguration::maxOutgoingMessageSize() const
{
    return d_maxMessageSizeOut;
}

inline
int btemt_ChannelPoolConfiguration::maxConnections() const
{
    return d_maxConnections;
}

inline
int btemt_ChannelPoolConfiguration::maxThreads() const
{
    return d_maxThreads;
}

inline
const bdeut_NullableValue<int>&
btemt_ChannelPoolConfiguration::numNewThreads() const
{
    return d_numNewThreads;
}

inline
int btemt_ChannelPoolConfiguration::maxWriteCache() const
{
    return d_writeCacheHiWat;
}

inline
const double& btemt_ChannelPoolConfiguration::metricsInterval() const
{
    return d_metricsInterval;
}

inline
const double& btemt_ChannelPoolConfiguration::readTimeout() const
{
    return d_readTimeout;
}

inline
bsl::ostream& btemt_ChannelPoolConfiguration::
                                          streamOut(bsl::ostream& stream) const
{
    return stream << *this;
}

template <class STREAM>
STREAM& btemt_ChannelPoolConfiguration::bdexStreamOut(STREAM& stream,
                                                      int     version) const
{
    switch (version) {
      case 6: {
        stream.putInt32(d_maxConnections);
        stream.putInt32(d_maxThreads);
        stream.putInt32(d_writeCacheLowWat);
        stream.putInt32(d_writeCacheHiWat);
        stream.putFloat64(d_readTimeout);
        stream.putFloat64(d_metricsInterval);

        stream.putInt32(d_minMessageSizeOut);
        stream.putInt32(d_typMessageSizeOut);
        stream.putInt32(d_maxMessageSizeOut);
        stream.putInt32(d_minMessageSizeIn);
        stream.putInt32(d_typMessageSizeIn);
        stream.putInt32(d_maxMessageSizeIn);
        stream.putInt32(d_threadStackSize);
        stream.putUint8(d_collectTimeMetrics);
        d_numNewThreads.bdexStreamOut(stream, 1);
      } break;
      case 5: {
        stream.putInt32(d_maxConnections);
        stream.putInt32(d_maxThreads);
        stream.putInt32(d_writeCacheLowWat);
        stream.putInt32(d_writeCacheHiWat);
        stream.putFloat64(d_readTimeout);
        stream.putFloat64(d_metricsInterval);

        stream.putInt32(d_minMessageSizeOut);
        stream.putInt32(d_typMessageSizeOut);
        stream.putInt32(d_maxMessageSizeOut);
        stream.putInt32(d_minMessageSizeIn);
        stream.putInt32(d_typMessageSizeIn);
        stream.putInt32(d_maxMessageSizeIn);
        stream.putInt32(d_threadStackSize);
        stream.putUint8(d_collectTimeMetrics);
      } break;
      case 4: {
        stream.putInt32(d_maxConnections);
        stream.putInt32(d_maxThreads);
        stream.putInt32(d_writeCacheLowWat);
        stream.putInt32(d_writeCacheHiWat);
        stream.putFloat64(d_readTimeout);
        stream.putFloat64(d_metricsInterval);

        stream.putInt32(d_minMessageSizeOut);
        stream.putInt32(d_typMessageSizeOut);
        stream.putInt32(d_maxMessageSizeOut);
        stream.putInt32(d_minMessageSizeIn);
        stream.putInt32(d_typMessageSizeIn);
        stream.putInt32(d_maxMessageSizeIn);
        stream.putInt32(d_threadStackSize);
      } break;
      case 3: {
        stream.putInt32(d_maxConnections);
        stream.putInt32(d_maxThreads);
        stream.putInt32(d_writeCacheLowWat);
        stream.putInt32(d_writeCacheHiWat);
        stream.putFloat64(d_readTimeout);
        stream.putFloat64(d_metricsInterval);

        stream.putInt32(d_minMessageSizeOut);
        stream.putInt32(d_typMessageSizeOut);
        stream.putInt32(d_maxMessageSizeOut);
        stream.putInt32(d_minMessageSizeIn);
        stream.putInt32(d_typMessageSizeIn);
        stream.putInt32(d_maxMessageSizeIn);
      } break;
      case 2: {
        stream.putInt32(d_maxConnections);
        stream.putInt32(d_maxThreads);
        stream.putInt32(d_writeCacheLowWat);
        stream.putInt32(d_writeCacheHiWat);
        stream.putFloat64(d_readTimeout);
        stream.putFloat64(d_metricsInterval);

        stream.putInt32(d_minMessageSizeOut);
        stream.putInt32(d_typMessageSizeOut);
        stream.putInt32(d_maxMessageSizeOut);
        stream.putInt32(d_minMessageSizeIn);
        stream.putInt32(d_typMessageSizeIn);
        stream.putInt32(d_maxMessageSizeIn);
        stream.putInt32(100);
      } break;
      case 1: {
        stream.putInt32(d_maxConnections);
        stream.putInt32(d_maxThreads);
        stream.putInt32(d_writeCacheHiWat);
        stream.putFloat64(d_readTimeout);
        stream.putFloat64(d_metricsInterval);

        stream.putInt32(d_minMessageSizeOut);
        stream.putInt32(d_typMessageSizeOut);
        stream.putInt32(d_maxMessageSizeOut);
        stream.putInt32(d_minMessageSizeIn);
        stream.putInt32(d_typMessageSizeIn);
        stream.putInt32(d_maxMessageSizeIn);
        stream.putInt32(100);
      } break;
    }
    return stream;
}

inline
int btemt_ChannelPoolConfiguration::workloadThreshold() const
{
    return 100;
}

inline
int btemt_ChannelPoolConfiguration::writeCacheLowWatermark() const {
    return d_writeCacheLowWat;
}

inline
int btemt_ChannelPoolConfiguration::writeCacheHiWatermark() const {
    return d_writeCacheHiWat;
}

inline
int btemt_ChannelPoolConfiguration::threadStackSize() const {
    return d_threadStackSize;
}

inline
bool btemt_ChannelPoolConfiguration::collectTimeMetrics() const {
    return d_collectTimeMetrics;
}

template <class ACCESSOR>
int btemt_ChannelPoolConfiguration::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_maxConnections,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_CONNECTIONS]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_maxThreads,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_THREADS]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_readTimeout,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_READ_TIMEOUT]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_metricsInterval,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_METRICS_INTERVAL]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_minMessageSizeOut,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MIN_MESSAGE_SIZE_OUT]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_typMessageSizeOut,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TYP_MESSAGE_SIZE_OUT]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_maxMessageSizeOut,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_MESSAGE_SIZE_OUT]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_minMessageSizeIn,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MIN_MESSAGE_SIZE_IN]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_typMessageSizeIn,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TYP_MESSAGE_SIZE_IN]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_maxMessageSizeIn,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_MESSAGE_SIZE_IN]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_writeCacheLowWat,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_CACHE_LOW_WAT]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_writeCacheHiWat,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_CACHE_HI_WAT]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_threadStackSize,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_THREAD_STACK_SIZE]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_collectTimeMetrics,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COLLECT_TIME_METRICS]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    ret = accessor(d_numNewThreads,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NUM_NEW_THREADS]);
    if (ret) {
        return ret;                                                 // RETURN
    }

    return ret;
}

template <class ACCESSOR>
int
btemt_ChannelPoolConfiguration::accessAttribute(ACCESSOR& accessor, int id)
                                                                          const
{
    enum { NOT_FOUND = -1 };

    switch (id) {
      case ATTRIBUTE_ID_MAX_CONNECTIONS: {
        return accessor(d_maxConnections,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_CONNECTIONS]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_MAX_THREADS: {
        return accessor(d_maxThreads,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_THREADS]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_READ_TIMEOUT: {
        return accessor(d_readTimeout,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_READ_TIMEOUT]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_METRICS_INTERVAL: {
        return accessor(d_metricsInterval,
                       ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_METRICS_INTERVAL]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_MIN_MESSAGE_SIZE_OUT: {
        return accessor(d_minMessageSizeOut,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MIN_MESSAGE_SIZE_OUT]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_TYP_MESSAGE_SIZE_OUT: {
        return accessor(d_typMessageSizeOut,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TYP_MESSAGE_SIZE_OUT]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_MAX_MESSAGE_SIZE_OUT: {
        return accessor(d_maxMessageSizeOut,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_MESSAGE_SIZE_OUT]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_MIN_MESSAGE_SIZE_IN: {
        return accessor(d_minMessageSizeIn,
                    ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MIN_MESSAGE_SIZE_IN]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_TYP_MESSAGE_SIZE_IN: {
        return accessor(d_typMessageSizeIn,
                    ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TYP_MESSAGE_SIZE_IN]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_MAX_MESSAGE_SIZE_IN: {
        return accessor(d_maxMessageSizeIn,
                    ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_MAX_MESSAGE_SIZE_IN]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_WRITE_CACHE_LOW_WAT: {
        return accessor(d_writeCacheLowWat,
                    ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_CACHE_LOW_WAT]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_WRITE_CACHE_HI_WAT: {
        return accessor(d_writeCacheHiWat,
                     ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_WRITE_CACHE_HI_WAT]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_THREAD_STACK_SIZE: {
        return accessor(d_threadStackSize,
                     ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_THREAD_STACK_SIZE]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_COLLECT_TIME_METRICS: {
        return accessor(d_collectTimeMetrics,
                   ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_COLLECT_TIME_METRICS]);
                                                                    // RETURN
      } break;
      case ATTRIBUTE_ID_NUM_NEW_THREADS: {
        return accessor(d_numNewThreads,
                        ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_NUM_NEW_THREADS]);
                                                                    // RETURN
      } break;

      default:
        return NOT_FOUND;
    }
}

template <class ACCESSOR>
inline
int btemt_ChannelPoolConfiguration::accessAttribute(ACCESSOR&   accessor,
                                                    const char *name,
                                                    int         nameLength)
                                                                          const
{
    enum { NOT_FOUND = -1 };

     const bdeat_AttributeInfo *attributeInfo =
           lookupAttributeInfo(name, nameLength);
     if (0 == attributeInfo) {
        return NOT_FOUND;                                           // RETURN
     }

     return accessAttribute(accessor, attributeInfo->d_id);
}

// FREE OPERATORS
inline
bool operator!=(const btemt_ChannelPoolConfiguration& lhs,
                const btemt_ChannelPoolConfiguration& rhs)
{
    return !(lhs == rhs);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
