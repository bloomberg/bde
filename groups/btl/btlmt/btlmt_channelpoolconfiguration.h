// btlmt_channelpoolconfiguration.h                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLMT_CHANNELPOOLCONFIGURATION
#define INCLUDED_BTLMT_CHANNELPOOLCONFIGURATION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a class for configuring channel pools.
//
//@CLASSES:
//  btlmt::ChannelPoolConfiguration: configuration parameters for channel pool
//
//@DESCRIPTION: This component provides a constrained-attribute class,
// 'btlmt::ChannelPoolConfiguration', that contains a set of attributes
// (objects and parameters) of particular use to channel pools.  The
// constraints are actively maintained by the class.  In particular, the "set"
// methods for constrained values will fail if their arguments are not
// consistent with the constraints.  Also, the constructor does not take any
// constrained arguments, but rather sets those values to valid defaults
// unconditionally.  This behavior avoids "silent failures", since the
// constructor cannot explicitly return a status value.
//
// The attributes contained by a 'btlmt::ChannelPoolConfiguration' object and
// the attribute constraints are given, respectively, in two tables below.  The
// attributes are as follows:
//..
//   TYPE    NAME                DESCRIPTION                            DEFAULT
//   ----    --------------      --------------------------------       -------
//   int     maxConnections      maximum number of connections             1024
//                               that can be managed by a channel
//                               pool.
//
//   int     maxThreads          the number of threads managed                1
//                               by a channel pool
//
//   double  readTimeout         timeout for "read" operations; if           30
//                               this value is 0, the read timeout
//                               will be disabled
//
//   double  metricsInterval     periodic-update interval for                30
//                               metrics
//
//   int     minMessageSizeOut   output message strategy hint                 1
//
//   int     typMessageSizeOut   output message strategy hint                 1
//
//   int     maxMessageSizeOut   output message strategy hint              1 MB
//
//   int     minMessageSizeIn    input message strategy hint                  1
//
//   int     typMessageSizeIn    input message strategy hint                  1
//
//   int     maxMessageSizeIn    input message strategy hint               1024
//
//   int     writeCacheLowWat    High and low watermarks (in                  0
//   int     writeCacheHiWat     bytes) for a channel's write              1 MB
//                               cache.  Once high watermark
//                               is reached, the channel pool
//                               will no longer accept messages
//                               for the channel until there
//                               is write space available.
//                               A channel state callback will
//                               result once the cached data size
//                               is lower than the low watermark
//                               value.
//
//   int     threadStackSize     the stack size of threads                 1 MB
//                               managed by this pool in bytes.
//
//   bool    collectTimeMetrics  indicates whether the configured          true
//                               channel pool will collect metrics.
//                               If this value is 'true', the channel
//                               pool will collect metrics
//                               categorizing the time spent
//                               processing data, and if this value
//                               is 'false', those metrics will not
//                               be collected.
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
//..
//
///Thread Safety
///-------------
// This constrained-attribute component is *thread-safe* but not
// *thread-enabled*, and requires explicit synchronization in the user space.
// Note that any of the contained user-defined callbacks may be invoked from
// *any* thread, and the user must account for that.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The following snippets of code illustrate how to use a
// 'btlmt::ChannelPoolConfiguration' object.  First, create a configuration
// object 'cpc'.  Note that it is necessarily configured to have valid but
// unpublished defaults.
//..
//  btlmt::ChannelPoolConfiguration cpc;
//..
// Next, set each attribute.  Note that each of the "in" and "out" message size
// triplets must be set atomically (i.e., with a single three-argument "set"
// method).  Note also that each "set" method will fail if the argument or
// triplet of arguments is not valid, and so each method returns a status
// value.
//..
//  assert(0 == cpc.setIncomingMessageSizes(1, 2, 3));
//  assert(1 == cpc.minIncomingMessageSize());
//  assert(2 == cpc.typicalIncomingMessageSize());
//  assert(3 == cpc.maxIncomingMessageSize());
//
//  assert(0 == cpc.setOutgoingMessageSizes(4, 5, 6));
//  assert(4 == cpc.minOutgoingMessageSize());
//  assert(5 == cpc.typicalOutgoingMessageSize());
//  assert(6 == cpc.maxOutgoingMessageSize());
//
//  assert(0   == cpc.setMaxConnections(100));
//  assert(100 == cpc.maxConnections());
//
//  assert(0   == cpc.setMaxThreads(200));
//  assert(200 == cpc.maxThreads());
//
//  assert(0    == cpc.setWriteCacheWatermarks(0, 1024));
//  assert(0    == cpc.writeCacheLowWatermark());
//  assert(1024 == cpc.writeCacheHiWatermark());
//
//  assert(0   == cpc.setReadTimeout(3.5));
//  assert(3.5 == cpc.readTimeout());
//
//  assert(0    == cpc.setMetricsInterval(5.25));
//  assert(5.25 == cpc.metricsInterval());
//
//  assert(0    == cpc.setThreadStackSize(1024));
//  assert(1024 == cpc.threadStackSize());
//..
// The configuration object is now validly configured with our choice of
// parameters.  If, however, we attempt to set an invalid configuration, the
// "set" method will fail (with a non-zero return status), and the
// configuration will be left unchanged.
//..
//  assert(0 != cpc.setIncomingMessageSizes(8, 4, 256));
//  assert(1 == cpc.minIncomingMessageSize());
//  assert(2 == cpc.typicalIncomingMessageSize());
//  assert(3 == cpc.maxIncomingMessageSize());
//..
// Finally, we can print the configuration value to 'stdout'.
//..
//  cout << cpc;
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

#ifndef INCLUDED_BTLSCM_VERSION
#include <btlscm_version.h>
#endif

#ifndef INCLUDED_BDLAT_ATTRIBUTEINFO
#include <bdlat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDLAT_TYPETRAITS
#include <bdlat_typetraits.h>
#endif

#ifndef INCLUDED_BDLAT_VALUETYPEFUNCTIONS
#include <bdlat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BDLB_PRINTMETHODS
#include <bdlb_printmethods.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDLT_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

namespace BloombergLP {

namespace btlmt {

class Message;

                   // ==============================
                   // class ChannelPoolConfiguration
                   // ==============================

class ChannelPoolConfiguration {
    // This class provides constrained configuration parameters for a channel
    // pool.  The constraints are maintained as class invariants; it is not
    // possible to obtain an invalid object through this interface.  See the
    // component-level documentation for a description of the channel pool
    // attributes.
    //
    // More generally, this class supports a complete set of *value* *semantic*
    // operations, including copy construction, assignment, equality
    // comparison, 'ostream' printing, but not 'bdex' serialization.  (A
    // precise operational definition of when two instances have the same value
    // can be found in the description of 'operator==' for the class.)  This
    // class is *exception* *neutral* with no guarantee of rollback: if an
    // exception is thrown during the invocation of a method on a pre-existing
    // instance, the object is left in a valid state, but its value is
    // undefined.  In no event is memory leaked.  Finally, *aliasing* (e.g.,
    // using all or part of an object as both source and destination) is
    // supported in all cases.

  private:
    enum { k_DEFAULT_THREAD_STACK_SIZE  = 1024 * 1024 };

    // Resource limits
    int                   d_maxConnections;    // maximum number of connections
                                               // that can be managed by a
                                               // channel pool.

    int                   d_maxThreads;        // maximum number of threads
                                               // managed by a channel pool

    int                   d_writeCacheLowWat;  // watermarks for the write

    int                   d_writeCacheHiWat;   // buffer for a managed channel

    // Timeouts
    double                d_readTimeout;       // timeout interval to wait for
                                               // before informing clients
                                               // if data is not available to
                                               // be read

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

    friend bsl::ostream& operator<<(bsl::ostream&,
                                    const ChannelPoolConfiguration&);

    friend bool operator==(const ChannelPoolConfiguration&,
                           const ChannelPoolConfiguration&);

  public:
    // TYPES
    enum {
        k_NUM_ATTRIBUTES = 14 // the number of attributes in this class


    };

    enum {
        e_ATTRIBUTE_INDEX_MAX_CONNECTIONS      = 0,
            // index for 'MaxConnections' attribute

        e_ATTRIBUTE_INDEX_MAX_THREADS          = 1,
            // index for 'MaxThreads' attribute

        e_ATTRIBUTE_INDEX_READ_TIMEOUT         = 2,
            // index for 'ReadTimeout' attribute

        e_ATTRIBUTE_INDEX_METRICS_INTERVAL     = 3,
            // index for 'MetricsInterval' attribute

        e_ATTRIBUTE_INDEX_MIN_MESSAGE_SIZE_OUT = 4,
            // index for 'MinMessageSizeOut' attribute

        e_ATTRIBUTE_INDEX_TYP_MESSAGE_SIZE_OUT = 5,
            // index for 'TypMessageSizeOut' attribute

        e_ATTRIBUTE_INDEX_MAX_MESSAGE_SIZE_OUT = 6,
            // index for 'MaxMessageSizeOut' attribute

        e_ATTRIBUTE_INDEX_MIN_MESSAGE_SIZE_IN  = 7,
            // index for 'MinMessageSizeIn' attribute

        e_ATTRIBUTE_INDEX_TYP_MESSAGE_SIZE_IN  = 8,
            // index for 'TypMessageSizeIn' attribute

        e_ATTRIBUTE_INDEX_MAX_MESSAGE_SIZE_IN  = 9,
            // index for 'MaxMessageSizeIn' attribute

        e_ATTRIBUTE_INDEX_WRITE_CACHE_LOW_WAT  = 10,
            // index for 'WriteCacheLowWat' attribute

        e_ATTRIBUTE_INDEX_WRITE_CACHE_HI_WAT   = 11,
            // index for 'WriteCacheHiWat' attribute

        e_ATTRIBUTE_INDEX_THREAD_STACK_SIZE    = 12,
            // index for 'ThreadStackSize' attribute

        e_ATTRIBUTE_INDEX_COLLECT_TIME_METRICS = 13
            // index for 'CollectTimeMetrics' attribute


    };

    enum {
        e_ATTRIBUTE_ID_MAX_CONNECTIONS         = 1,
            // id for 'MaxConnections' attribute

        e_ATTRIBUTE_ID_MAX_THREADS             = 2,
            // id for 'MaxThreads' attribute

        e_ATTRIBUTE_ID_READ_TIMEOUT            = 3,
            // id for 'ReadTimeout' attribute

        e_ATTRIBUTE_ID_METRICS_INTERVAL        = 4,
            // id for 'MetricsInterval' attribute

        e_ATTRIBUTE_ID_MIN_MESSAGE_SIZE_OUT    = 5,
            // id for 'MinMessageSizeOut' attribute

        e_ATTRIBUTE_ID_TYP_MESSAGE_SIZE_OUT    = 6,
            // id for 'TypMessageSizeOut' attribute

        e_ATTRIBUTE_ID_MAX_MESSAGE_SIZE_OUT    = 7,
            // id for 'MaxMessageSizeOut' attribute

        e_ATTRIBUTE_ID_MIN_MESSAGE_SIZE_IN     = 8,
            // id for 'MinMessageSizeIn' attribute

        e_ATTRIBUTE_ID_TYP_MESSAGE_SIZE_IN     = 9,
            // id for 'TypMessageSizeIn' attribute

        e_ATTRIBUTE_ID_MAX_MESSAGE_SIZE_IN     = 10,
            // id for 'MaxMessageSizeIn' attribute

        e_ATTRIBUTE_ID_WRITE_CACHE_LOW_WAT     = 11,
            // id for 'WriteCacheLowWat' attribute

        e_ATTRIBUTE_ID_WRITE_CACHE_HI_WAT      = 12,
            // id for 'WriteCacheHiWat' attribute

        e_ATTRIBUTE_ID_THREAD_STACK_SIZE       = 13,
            // id for 'ThreadStackSize' attribute

        e_ATTRIBUTE_ID_COLLECT_TIME_METRICS    = 14
            // id for 'CollectTimeMetrics' attribute


    };

  public:
    // CONSTANTS
    static const char CLASS_NAME[];
        // the name of this class

    static const bdlat_AttributeInfo ATTRIBUTE_INFO_ARRAY[];
        // attribute information for each attribute

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(ChannelPoolConfiguration,
                                  bdlat_TypeTraitBasicSequence,
                                  bdlb::TypeTraitHasPrintMethod);

    // CLASS METHODS
    static const bdlat_AttributeInfo *lookupAttributeInfo(int id);
        // Return attribute information for the attribute indicated by the
        // specified 'id' if the attribute exists, and 0 otherwise.

    static const bdlat_AttributeInfo *lookupAttributeInfo(
                                                       const char *name,
                                                       int         nameLength);
        // Return attribute information for the attribute indicated by the
        // specified 'name' of the specified 'nameLength' if the attribute
        // exists, and 0 otherwise.

    // CREATORS
    ChannelPoolConfiguration();
        // Create a channel pool configuration constrained-attribute object
        // having valid default values for all attributes.

    ChannelPoolConfiguration(const ChannelPoolConfiguration& original);
        // Create a channel pool configuration constrained-attribute object
        // having the value of the specified 'original' object.

    ~ChannelPoolConfiguration();
        // Destroy this channel pool configuration constrained-attribute
        // object.

    // MANIPULATORS
    ChannelPoolConfiguration& operator=(const ChannelPoolConfiguration& rhs);
        // Assign to this channel pool configuration constrained-attribute
        // object the value of the specified 'rhs' object.

    int setIncomingMessageSizes(int min, int typical, int max);
        // Set the triplet of incoming-message-size attributes of this object
        // to the specified 'min', 'typical', and 'max' values if
        // '0 <= min <= typical <= max'.  Return 0 on success, and a non-zero
        // value (with no effect on the state of this object) otherwise.

    int setOutgoingMessageSizes(int min, int typical, int max);
        // Set the triplet of outgoing-message-size attributes of this object
        // to the specified 'min', 'typical', and 'max' values if
        // '0 <= min <= typical <= max'.  Return 0 on success, and a non-zero
        // value (with no effect on the state of this object) otherwise.

    int setMaxConnections(int maxConnections);
        // Set the maximum number of connections attribute of this object to
        // the specified 'maxConnections' if '0 <= maxConnections'.  Return 0
        // on success, and a non-zero value (with no effect on the state of
        // this object) otherwise.

    int setMaxThreads(int maxThreads);
        // Set the maximum number of threads attribute of this object to the
        // specified 'maxThreads' if '0 <= maxThreads'.  Return 0 on success,
        // and a non-zero value (with no effect on the state of this object)
        // otherwise.

   int setMetricsInterval(double metricsInterval);
        // Set the metrics interval attribute of this object to the specified
        // 'metricsInterval' value if '0 <= metricsInterval'.  Return 0 on
        // success, and a non-zero value (with no effect on the state of this
        // object) otherwise.

    int setReadTimeout(double readTimeout);
        // Set the read timeout attribute of this object to the specified
        // 'readTimeout' value if '0 <= readTimeout'.  Return 0 on success, and
        // a non-zero value (with no effect on the state of this object)
        // otherwise.  A value of 0 will disable the read timeout.

    int setThreadStackSize(int stackSize);
        // Set the thread stack size attribute of this object to the specified
        // 'stackSize' value if '0 <= stackSize'.  Return 0 on success, and a
        // non-zero value (with no effect on the state of this object)
        // otherwise.

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

    bool collectTimeMetrics() const;
        // Return 'true' if the configured channel pool will collect time
        // metrics, and 'false' otherwise.  If this value is 'true', the
        // channel pool will collect metrics categorizing the time spent
        // processing data.  Note that, if this value is 'false', the channel
        // pool cannot use that estimate of work-load when it attempts to
        // distribute work amongst its managed threads.

    const double& metricsInterval() const;
        // Return the metrics interval attribute of this object.

    const double& readTimeout() const;
        // Return the read timeout attribute of this object.  A value of 0
        // indicates the read timeout should be disabled.

    int writeCacheLowWatermark() const;
        // Return the low watermark for the write cache.

    int writeCacheHiWatermark() const;
        // Return the high watermark for the write cache.

    int threadStackSize() const;
        // Return the thread stack size attribute of this object.

    bsl::ostream& streamOut(bsl::ostream& stream) const;
        // Write the specified 'configuration' value to the specified 'output'
        // stream in a reasonable multi-line format.

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
bool operator==(const ChannelPoolConfiguration& lhs,
                const ChannelPoolConfiguration& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects objects
    // have the same value if each respective attribute has the same value.

inline
bool operator!=(const ChannelPoolConfiguration& lhs,
                const ChannelPoolConfiguration& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

bsl::ostream& operator<<(bsl::ostream&                   output,
                         const ChannelPoolConfiguration& configuration);
    // Write the specified 'configuration' value to the specified 'output'
    // stream in a reasonable multi-line format.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                   // ------------------------------
                   // class ChannelPoolConfiguration
                   // ------------------------------
// MANIPULATORS
inline
int ChannelPoolConfiguration::setIncomingMessageSizes(int min,
                                                      int typical,
                                                      int max)
{
    if (0 <= min && min <= typical && typical <= max) {
        d_minMessageSizeIn = min;
        d_typMessageSizeIn = typical;
        d_maxMessageSizeIn = max;
        return 0;                                                     // RETURN
    }
    return -1;
}

inline
int ChannelPoolConfiguration::setOutgoingMessageSizes(int min,
                                                      int typical,
                                                      int max)
{
    if (0 <= min && min <= typical && typical <= max) {
        d_minMessageSizeOut = min;
        d_typMessageSizeOut = typical;
        d_maxMessageSizeOut = max;
        return 0;                                                     // RETURN
    }
    return -1;
}

inline
int ChannelPoolConfiguration::setMaxConnections(int maxConnections)
{
    if (0 <= maxConnections) {
        d_maxConnections = maxConnections;
        return 0;                                                     // RETURN
    }
    return -1;
}

inline
int ChannelPoolConfiguration::setMaxThreads(int maxThreads)
{
    if (0 <= maxThreads) {
        d_maxThreads = maxThreads;
        return 0;                                                     // RETURN
    }
    return -1;
}

inline
int ChannelPoolConfiguration::setThreadStackSize(int stackSize)
{
    if (0 <= stackSize) {
        d_threadStackSize = stackSize;
        return 0;                                                     // RETURN
    }
    return -1;
}

inline
int ChannelPoolConfiguration::setWriteCacheWatermarks(int lowWatermark,
                                                      int hiWatermark)
{
    if (0 <= lowWatermark && lowWatermark <= hiWatermark) {
        d_writeCacheLowWat = lowWatermark;
        d_writeCacheHiWat = hiWatermark;
        return 0;                                                     // RETURN
    }
    return -1;
}

inline
int ChannelPoolConfiguration::setMetricsInterval(double metricsInterval)
{
    if (0 <= metricsInterval) {
        d_metricsInterval = metricsInterval;
        return 0;                                                     // RETURN
    }
    return -1;
}

inline
int
ChannelPoolConfiguration::setReadTimeout(double readTimeout)
{
    if (0 <= readTimeout) {
        d_readTimeout = readTimeout;
        return 0;                                                     // RETURN
    }
    return -1;
}

inline
int ChannelPoolConfiguration::setCollectTimeMetrics(
                                                   bool collectTimeMetricsFlag)
{
    d_collectTimeMetrics = collectTimeMetricsFlag;
    return 0;
}

template <class MANIPULATOR>
int ChannelPoolConfiguration::manipulateAttributes(MANIPULATOR& manipulator)
{
    int ret;

    ret = manipulator(&d_maxConnections,
                      ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_CONNECTIONS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_maxThreads,
                      ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_THREADS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(&d_readTimeout,
                      ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_READ_TIMEOUT]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(
                     &d_metricsInterval,
                     ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_METRICS_INTERVAL]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(
                 &d_minMessageSizeOut,
                 ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MIN_MESSAGE_SIZE_OUT]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(
                 &d_typMessageSizeOut,
                 ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TYP_MESSAGE_SIZE_OUT]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(
                 &d_maxMessageSizeOut,
                 ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_MESSAGE_SIZE_OUT]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(
                  &d_minMessageSizeIn,
                  ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MIN_MESSAGE_SIZE_IN]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(
                  &d_typMessageSizeIn,
                  ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TYP_MESSAGE_SIZE_IN]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(
                  &d_maxMessageSizeIn,
                  ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_MESSAGE_SIZE_IN]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(
                  &d_writeCacheLowWat,
                  ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_WRITE_CACHE_LOW_WAT]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(
                   &d_writeCacheHiWat,
                   ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_WRITE_CACHE_HI_WAT]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(
                    &d_threadStackSize,
                    ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_THREAD_STACK_SIZE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = manipulator(
                 &d_collectTimeMetrics,
                 ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_COLLECT_TIME_METRICS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class MANIPULATOR>
int ChannelPoolConfiguration::manipulateAttribute(MANIPULATOR& manipulator,
                                                  int          id)
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_MAX_CONNECTIONS: {
        return manipulator(
                      &d_maxConnections,
                      ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_CONNECTIONS]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_MAX_THREADS: {
        return manipulator(
                          &d_maxThreads,
                          ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_THREADS]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_READ_TIMEOUT: {
        return manipulator(
                         &d_readTimeout,
                         ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_READ_TIMEOUT]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_METRICS_INTERVAL: {
        return manipulator(
                     &d_metricsInterval,
                     ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_METRICS_INTERVAL]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_MIN_MESSAGE_SIZE_OUT: {
        return manipulator(
                 &d_minMessageSizeOut,
                 ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MIN_MESSAGE_SIZE_OUT]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_TYP_MESSAGE_SIZE_OUT: {
        return manipulator(
                 &d_typMessageSizeOut,
                 ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TYP_MESSAGE_SIZE_OUT]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_MAX_MESSAGE_SIZE_OUT: {
        return manipulator(
                 &d_maxMessageSizeOut,
                 ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_MESSAGE_SIZE_OUT]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_MIN_MESSAGE_SIZE_IN: {
        return manipulator(
                  &d_minMessageSizeIn,
                  ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MIN_MESSAGE_SIZE_IN]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_TYP_MESSAGE_SIZE_IN: {
        return manipulator(
                  &d_typMessageSizeIn,
                  ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TYP_MESSAGE_SIZE_IN]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_MAX_MESSAGE_SIZE_IN: {
        return manipulator(
                  &d_maxMessageSizeIn,
                  ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_MESSAGE_SIZE_IN]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_WRITE_CACHE_LOW_WAT: {
        return manipulator(
                  &d_writeCacheLowWat,
                  ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_WRITE_CACHE_LOW_WAT]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_WRITE_CACHE_HI_WAT: {
        return manipulator(
                   &d_writeCacheHiWat,
                   ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_WRITE_CACHE_HI_WAT]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_THREAD_STACK_SIZE: {
        return manipulator(
                    &d_threadStackSize,
                    ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_THREAD_STACK_SIZE]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_COLLECT_TIME_METRICS: {
        return manipulator(
                 &d_collectTimeMetrics,
                 ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_COLLECT_TIME_METRICS]);
                                                                      // RETURN
      } break;

      default:
        return k_NOT_FOUND;                                           // RETURN
    }
}

template <class MANIPULATOR>
inline
int ChannelPoolConfiguration::manipulateAttribute(MANIPULATOR&  manipulator,
                                                  const char   *name,
                                                  int           nameLength)
{
    enum { k_NOT_FOUND = -1 };

    const bdlat_AttributeInfo *attributeInfo = lookupAttributeInfo(name,
                                                                   nameLength);
    if (0 == attributeInfo) {
        return k_NOT_FOUND;                                           // RETURN
    }

    return manipulateAttribute(manipulator, attributeInfo->d_id);
}

// ACCESSORS
inline
int ChannelPoolConfiguration::minIncomingMessageSize() const
{
    return d_minMessageSizeIn;
}

inline
int ChannelPoolConfiguration::typicalIncomingMessageSize() const
{
    return d_typMessageSizeIn;
}

inline
int ChannelPoolConfiguration::maxIncomingMessageSize() const
{
    return d_maxMessageSizeIn;
}

inline
int ChannelPoolConfiguration::minOutgoingMessageSize() const
{
    return d_minMessageSizeOut;
}

inline
int ChannelPoolConfiguration::typicalOutgoingMessageSize() const
{
    return d_typMessageSizeOut;
}

inline
int ChannelPoolConfiguration::maxOutgoingMessageSize() const
{
    return d_maxMessageSizeOut;
}

inline
int ChannelPoolConfiguration::maxConnections() const
{
    return d_maxConnections;
}

inline
int ChannelPoolConfiguration::maxThreads() const
{
    return d_maxThreads;
}

inline
const double& ChannelPoolConfiguration::metricsInterval() const
{
    return d_metricsInterval;
}

inline
const double& ChannelPoolConfiguration::readTimeout() const
{
    return d_readTimeout;
}

inline
bsl::ostream& ChannelPoolConfiguration::streamOut(bsl::ostream& stream) const
{
    return stream << *this;
}

inline
int ChannelPoolConfiguration::writeCacheLowWatermark() const {
    return d_writeCacheLowWat;
}

inline
int ChannelPoolConfiguration::writeCacheHiWatermark() const {
    return d_writeCacheHiWat;
}

inline
int ChannelPoolConfiguration::threadStackSize() const {
    return d_threadStackSize;
}

inline
bool ChannelPoolConfiguration::collectTimeMetrics() const {
    return d_collectTimeMetrics;
}

template <class ACCESSOR>
int ChannelPoolConfiguration::accessAttributes(ACCESSOR& accessor) const
{
    int ret;

    ret = accessor(d_maxConnections,
                   ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_CONNECTIONS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_maxThreads,
                   ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_THREADS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_readTimeout,
                   ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_READ_TIMEOUT]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_metricsInterval,
                   ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_METRICS_INTERVAL]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(
                 d_minMessageSizeOut,
                 ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MIN_MESSAGE_SIZE_OUT]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(
                 d_typMessageSizeOut,
                 ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TYP_MESSAGE_SIZE_OUT]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(
                 d_maxMessageSizeOut,
                 ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_MESSAGE_SIZE_OUT]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(
                  d_minMessageSizeIn,
                  ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MIN_MESSAGE_SIZE_IN]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(
                  d_typMessageSizeIn,
                  ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TYP_MESSAGE_SIZE_IN]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(
                  d_maxMessageSizeIn,
                  ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_MESSAGE_SIZE_IN]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(
                  d_writeCacheLowWat,
                  ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_WRITE_CACHE_LOW_WAT]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_writeCacheHiWat,
                   ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_WRITE_CACHE_HI_WAT]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(d_threadStackSize,
                   ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_THREAD_STACK_SIZE]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    ret = accessor(
                 d_collectTimeMetrics,
                 ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_COLLECT_TIME_METRICS]);
    if (ret) {
        return ret;                                                   // RETURN
    }

    return ret;
}

template <class ACCESSOR>
int
ChannelPoolConfiguration::accessAttribute(ACCESSOR& accessor, int id) const
{
    enum { k_NOT_FOUND = -1 };

    switch (id) {
      case e_ATTRIBUTE_ID_MAX_CONNECTIONS: {
        return accessor(
                      d_maxConnections,
                      ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_CONNECTIONS]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_MAX_THREADS: {
        return accessor(d_maxThreads,
                        ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_THREADS]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_READ_TIMEOUT: {
        return accessor(d_readTimeout,
                        ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_READ_TIMEOUT]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_METRICS_INTERVAL: {
        return accessor(
                     d_metricsInterval,
                     ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_METRICS_INTERVAL]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_MIN_MESSAGE_SIZE_OUT: {
        return accessor(
                 d_minMessageSizeOut,
                 ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MIN_MESSAGE_SIZE_OUT]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_TYP_MESSAGE_SIZE_OUT: {
        return accessor(
                 d_typMessageSizeOut,
                 ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TYP_MESSAGE_SIZE_OUT]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_MAX_MESSAGE_SIZE_OUT: {
        return accessor(
                 d_maxMessageSizeOut,
                 ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_MESSAGE_SIZE_OUT]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_MIN_MESSAGE_SIZE_IN: {
        return accessor(
                  d_minMessageSizeIn,
                  ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MIN_MESSAGE_SIZE_IN]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_TYP_MESSAGE_SIZE_IN: {
        return accessor(
                  d_typMessageSizeIn,
                  ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_TYP_MESSAGE_SIZE_IN]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_MAX_MESSAGE_SIZE_IN: {
        return accessor(
                  d_maxMessageSizeIn,
                  ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_MAX_MESSAGE_SIZE_IN]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_WRITE_CACHE_LOW_WAT: {
        return accessor(
                  d_writeCacheLowWat,
                  ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_WRITE_CACHE_LOW_WAT]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_WRITE_CACHE_HI_WAT: {
        return accessor(
                   d_writeCacheHiWat,
                   ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_WRITE_CACHE_HI_WAT]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_THREAD_STACK_SIZE: {
        return accessor(
                    d_threadStackSize,
                    ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_THREAD_STACK_SIZE]);
                                                                      // RETURN
      } break;
      case e_ATTRIBUTE_ID_COLLECT_TIME_METRICS: {
        return accessor(
                 d_collectTimeMetrics,
                 ATTRIBUTE_INFO_ARRAY[e_ATTRIBUTE_INDEX_COLLECT_TIME_METRICS]);
                                                                      // RETURN
      } break;

      default:
        return k_NOT_FOUND;                                           // RETURN
    }
}

template <class ACCESSOR>
inline
int ChannelPoolConfiguration::accessAttribute(ACCESSOR&   accessor,
                                              const char *name,
                                              int         nameLength) const
{
    enum { k_NOT_FOUND = -1 };

     const bdlat_AttributeInfo *attributeInfo = lookupAttributeInfo(
                                                                   name,
                                                                   nameLength);
     if (0 == attributeInfo) {
        return k_NOT_FOUND;                                           // RETURN
     }

     return accessAttribute(accessor, attributeInfo->d_id);
}

}  // close package namespace

// FREE OPERATORS
inline
bool btlmt::operator!=(const ChannelPoolConfiguration& lhs,
                       const ChannelPoolConfiguration& rhs)
{
    return !(lhs == rhs);
}

}  // close enterprise namespace

#endif

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
