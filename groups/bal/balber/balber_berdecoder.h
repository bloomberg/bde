// balber_berdecoder.h                                                -*-C++-*-
#ifndef INCLUDED_BALBER_BERDECODER
#define INCLUDED_BALBER_BERDECODER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a BER decoder class.
//
//@CLASSES:
//  balber::BerDecoder: BER decoder
//
//@SEE_ALSO: balber_berencoder, bdem_bdemdecoder, balxml_decoder
//
//@DESCRIPTION: This component defines a single class, `balber::BerDecoder`,
// that contains a parameterized `decode` function.  The `decode` function
// decodes data read from a specified stream and loads the corresponding object
// to an object of the parameterized type.  The `decode` method is overloaded
// for two types of input streams:
// * `bsl::streambuf`
// * `bsl::istream`
//
// This class decodes objects based on the X.690 BER specification and is
// restricted to types supported by the `bdlat` framework.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Decoding an Employee Record
/// - - - - - - - - - - - - - - - - - - -
// Suppose that an "employee record" consists of a sequence of attributes --
// `name`, `age`, and `salary` -- that are of types `bsl::string`, `int`, and
// `float`, respectively.  Furthermore, we have a need to BER encode employee
// records as a sequence of values (for out-of-process consumption).
//
// Assume that we have defined a `usage::EmployeeRecord` class to represent
// employee record values, and assume that we have provided the `bdlat`
// specializations that allow the `balber` codec components to represent class
// values as a sequence of BER primitive values.  See
// {`bdlat_sequencefunctions`|Usage} for details of creating specializations
// for a sequence type.
//
// First, we create an employee record object having typical values:
// ```
// usage::EmployeeRecord bob("Bob", 56, 1234.00);
// assert("Bob"   == bob.name());
// assert(  56    == bob.age());
// assert(1234.00 == bob.salary());
// ```
// Next, we create a `balber::Encoder` object and use it to encode our `bob`
// object.  Here, to facilitate the examination of our results, the BER
// encoding data is delivered to a `bslsb::MemOutStreamBuf` object:
// ```
// bdlsb::MemOutStreamBuf osb;
// balber::BerEncoder     encoder;
// int                    rc = encoder.encode(&osb, bob);
// assert( 0 == rc);
// assert(18 == osb.length());
// ```
// Now, we create a `bdlsb::FixedMemInStreamBuf` object to manage our access
// to the data portion of the `bdlsb::MemOutStreamBuf` (where our BER encoding
// resides), decode the values found there, and use them to set the value
// of an `usage::EmployeeRecord` object.
// ```
// balber::BerDecoderOptions  options;
// balber::BerDecoder         decoder(&options);
// bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
// usage::EmployeeRecord      obj;
//
// rc = decoder.decode(&isb, &obj);
// assert(0 == rc);
// ```
// Finally, we confirm that the object defined by the BER encoding has the
// same value as the original object.
// ```
// assert(bob.name()   == obj.name());
// assert(bob.age()    == obj.age());
// assert(bob.salary() == obj.salary());
// ```

#include <balscm_version.h>

#include <balber_berconstants.h>
#include <balber_berdecoderoptions.h>
#include <balber_beruniversaltagnumber.h>
#include <balber_berutil.h>

#include <bdlat_arrayfunctions.h>
#include <bdlat_choicefunctions.h>
#include <bdlat_customizedtypefunctions.h>
#include <bdlat_enumfunctions.h>
#include <bdlat_enumutil.h>
#include <bdlat_formattingmode.h>
#include <bdlat_nullablevaluefunctions.h>
#include <bdlat_sequencefunctions.h>
#include <bdlat_typecategory.h>
#include <bdlat_valuetypefunctions.h>

#include <bdlb_variant.h>

#include <bdlsb_memoutstreambuf.h>

#include <bslma_allocator.h>

#include <bsls_assert.h>
#include <bsls_keyword.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_review.h>

#include <bsl_istream.h>
#include <bsl_ostream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace balber {

class BerDecoder_Node;
class BerDecoder_NodeVisitor;
class BerDecoder_UniversalElementVisitor;

                              // ================
                              // class BerDecoder
                              // ================

/// This class contains the parameterized `decode` functions that decode
/// data (in BER format) from an incoming stream into `bdlat` types.
class BerDecoder {

  private:
    // PRIVATE TYPES

    /// This class provides stream for logging using
    /// `bdlsb::MemOutStreamBuf` as a streambuf.  The logging stream is
    /// created on demand, i.e., during the first attempt to log message.
    class MemOutStream : public bsl::ostream {

        bdlsb::MemOutStreamBuf d_sb;

        // NOT IMPLEMENTED
        MemOutStream(const MemOutStream&);             // = delete;
        MemOutStream& operator=(const MemOutStream&);  // = delete;

      public:
        // CREATORS

        /// Create a stream object.  Optionally specify a `basicAllocator`
        /// used to supply memory.  If `basicAllocator` is 0, the currently
        /// installed default allocator is used.
        MemOutStream(bslma::Allocator *basicAllocator = 0);

        /// Destroy this stream and release memory back to the allocator.
        ///
        /// Although the compiler should generate this destructor
        /// implicitly, xlC 8 breaks when the destructor is called by name
        /// unless it is explicitly declared.
        ~MemOutStream() BSLS_KEYWORD_OVERRIDE;

        // MANIPULATORS

        /// Reset the internal streambuf to the empty state.
        void reset();

        // ACCESSORS

        /// Return a pointer to the memory containing the formatted values
        /// formatted to this stream.  The data is not null-terminated
        /// unless a null character was appended onto this stream.
        const char *data() const;

        /// Return the length of the formatted data, including null
        /// characters appended to the stream, if any.
        int length() const;
    };

  public:
    // PUBLIC TYPES
    enum ErrorSeverity {
        e_BER_SUCCESS = 0x00
      , e_BER_ERROR   = 0x02

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , BDEM_BER_SUCCESS = e_BER_SUCCESS
      , BDEM_BER_ERROR   = e_BER_ERROR
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
    };

  private:
    // DATA
    const BerDecoderOptions         *d_options;      // held, not owned
    bslma::Allocator                *d_allocator;    // held, not owned

    bsls::ObjectBuffer<MemOutStream> d_logArea;      // placeholder for
                                                     // 'MemOutStream'

    MemOutStream                    *d_logStream;    // if not zero,
                                                     // log stream was created
                                                     // at the moment of first
                                                     // logging and must be
                                                     // destroyed

    ErrorSeverity                    d_severity;     // error severity level
    bsl::streambuf                  *d_streamBuf;    // held, not owned
    int                              d_currentDepth; // current depth

    int                              d_numUnknownElementsSkipped;
                                                     // number of unknown
                                                     // elements skipped

    BerDecoder_Node                 *d_topNode;      // last node

    int                              d_arrayLengthHint;
                                                     // length hint

    // NOT IMPLEMENTED
    BerDecoder(const BerDecoder&);             // = delete;
    BerDecoder& operator=(const BerDecoder&);  // = delete;

    // FRIENDS
    friend class BerDecoder_Node;

  private:
    // PRIVATE MANIPULATORS

    /// Log the specified `msg`, upgrade the severity level, and return
    /// `e_BER_ERROR`.
    ErrorSeverity logError(const char *msg);

    /// Log the specified `msg` and upgrade the severity level.
    void logErrorImp(const char *msg);

    /// Log the specified `prefix` and `msg` and return `errorSeverity()`.
    ErrorSeverity logMsg(const char *prefix, const char *msg);

    /// Return the stream used for logging.  If stream has not been created
    /// yet, it will be created during this call.
    bsl::ostream& logStream();

  public:
    // CREATORS

    /// Construct a decoder object.  Optionally specify decoder `options`.
    /// If `options` is 0, `BerDecoderOptions()` is used.  Optionally
    /// specify a `basicAllocator` used to supply memory.  If
    /// `basicAllocator` is 0, the currently installed default allocator is
    /// used.
    BerDecoder(const BerDecoderOptions *options = 0,
               bslma::Allocator        *basicAllocator = 0);

    /// Destroy this object.  This destruction has no effect on objects
    /// pointed-to by the pointers provided at construction.
    ~BerDecoder();

    // MANIPULATORS

    /// Decode an object of parameterized `TYPE` from the specified
    /// `streamBuf` and load the result into the specified `variable`.
    /// Return 0 on success, and a non-zero value otherwise.
    template <typename TYPE>
    int decode(bsl::streambuf *streamBuf, TYPE *variable);

    /// Decode an object of parameterized `TYPE` from the specified `stream`
    /// and load the result into the specified modifiable `variable`.
    /// Return 0 on success, and a non-zero value otherwise.  If the
    /// decoding fails `stream` will be invalidated.
    template <typename TYPE>
    int decode(bsl::istream& stream, TYPE *variable);

    /// Set the number of unknown elements skipped by the decoder during the
    /// current decoding operation to the specified `value`.  The behavior
    /// is undefined unless `0 <= value`.
    void setNumUnknownElementsSkipped(int value);

    // ACCESSORS

    /// Return the address of the BER decoder options.
    const BerDecoderOptions *decoderOptions() const;

    /// Return 'true' if the maximum depth level is exceeded and 'false'
    /// otherwise.
    bool maxDepthExceeded() const;

    /// Return the number of unknown elements that were skipped during the
    /// previous decoding operation.  Note that unknown elements are skipped
    /// only if `true == options()->skipUnknownElements()`.
    int numUnknownElementsSkipped() const;

    /// Return the severity of the most severe log or error message
    /// encountered during the last call to the `decode` method.  The
    /// severity is reset each time `decode` is called.
    ErrorSeverity  errorSeverity() const;

    /// Return a string containing any error or trace messages that were
    /// logged during the last call to the `decode` method.  The log is
    /// reset each time `decode` is called.
    bslstl::StringRef loggedMessages() const;
};


                       // =============================
                       // private class BerDecoder_Node
                       // =============================

/// This class provides current context for BER decoding process and
/// represents a node for BER element.  The BER element consists of element
/// tag, length field, body field and optional end of tag.  The class also
/// provides various methods to read the different parts of BER element such
/// as tag header (tag itself and length fields), body for any type of data,
/// and optional tag trailer.
class BerDecoder_Node {

    // DATA
    BerDecoder             *d_decoder;              // decoder,
                                                    // held, not owned
    BerDecoder_Node        *d_parent;               // parent node,
                                                    // held, not owned
    BerConstants::TagClass  d_tagClass;             // tag class
    BerConstants::TagType   d_tagType;              // tag type
    int                     d_tagNumber;            // tag id or number
    int                     d_expectedLength;       // body length
    int                     d_consumedHeaderBytes;  // header bytes read
    int                     d_consumedBodyBytes;    // body bytes read
    int                     d_consumedTailBytes;    // trailer bytes read
    int                     d_formattingMode;       // formatting mode
    const char             *d_fieldName;            // name of the field

    // NOT IMPLEMENTED
    BerDecoder_Node(BerDecoder_Node&);            // = delete;
    BerDecoder_Node& operator=(BerDecoder_Node&); // = delete;

  private:
    // PRIVATE MANIPULATORS

    /// Family of methods to decode current element into the specified
    /// `variable` of category `bdlat_TypeCategory`.  Return zero on
    /// success, and a non-zero value otherwise. the tag header is already
    /// read at the moment of call and input stream is positioned at the
    /// first byte of the body field.
    int decode(bsl::vector<char> *variable, bdlat_TypeCategory::Array);
    int decode(bsl::vector<unsigned char> *variable,
               bdlat_TypeCategory::Array);
    template <typename TYPE>
    int decode(TYPE *variable, bdlat_TypeCategory::Array);
    template <typename TYPE>
    int decode(TYPE *variable, bdlat_TypeCategory::Choice);
    template <typename TYPE>
    int decode(TYPE *variable, bdlat_TypeCategory::NullableValue);
    template <typename TYPE>
    int decode(TYPE *variable, bdlat_TypeCategory::CustomizedType);
    template <typename TYPE>
    int decode(TYPE *variable, bdlat_TypeCategory::Enumeration);
    template <typename TYPE>
    int decode(TYPE *variable, bdlat_TypeCategory::Sequence);
    template <typename TYPE>
    int decode(TYPE *variable, bdlat_TypeCategory::Simple);
    template <typename TYPE>
    int decode(TYPE *variable, bdlat_TypeCategory::DynamicType);

    /// Decode the current element, an array, into specified `variable`.
    /// Return zero on success, and a non-zero value otherwise.
    template <typename TYPE>
    int decodeArray(TYPE *variable);

    /// Decode the current element, which is a choice object, into specified
    /// `variable`.  Return zero on success, and a non-zero value otherwise.
    template <typename TYPE>
    int decodeChoice(TYPE *variable);

  public:
    // CREATORS
    BerDecoder_Node(BerDecoder *decoder);

    template <typename TYPE>
    BerDecoder_Node(BerDecoder *decoder, const TYPE *variable);

    ~BerDecoder_Node();

    // MANIPULATORS
    template <typename TYPE>
    int operator()(TYPE *object, bslmf::Nil);

    template <typename TYPE, typename ANY_CATEGORY>
    int operator()(TYPE *object, ANY_CATEGORY category);

    template <typename TYPE>
    int operator()(TYPE *object);

    /// Print the content of node to the specified stream `out`.  `depth` is
    /// the value `d_decoder->currentDepth` assumed after node was created.
    void print(bsl::ostream&  out,
               int            depth,
               int            spacePerLevel = 0,
               const char    *prefixText = 0) const;

    /// Print the chain of nodes to the specified `out` stream, starting
    /// from this node and iterating to the parent node, then its parent,
    /// etc.
    void printStack(bsl::ostream& out) const;

    /// Set formatting mode specified by `formattingMode`.
    void setFormattingMode(int formattingMode);

    /// Set object field name associated with this node to the specified
    /// `name`.
    void setFieldName(const char *name);

    /// Set the node severity to `e_BER_ERROR`, print the error message
    /// specified by `msg` to the decoder's log, print the stack of nodes to
    /// the decoder's log, and return a non-zero value.
    int logError(const char *msg);

    /// Read the node tag field containing tag class, tag type and tag
    /// number, and the node length field.  Return zero on success, and a
    /// non-zero value otherwise.
    int readTagHeader();

    /// Read the node end-of-octets field, if such exists, so the stream
    /// will be positioned at the start of next node.  Return zero on
    /// success and a non-zero value otherwise.
    int readTagTrailer();

    /// Return `true` if current node has more embedded elements and return
    /// `false` otherwise.
    bool hasMore();

    /// Skip the field body.  The identifier octet and length have already
    /// been extracted.  Return zero on success, and a non-zero value
    /// otherwise.  Note that method must be called when input stream is
    /// positioned at the first byte of the body field.
    int skipField();

    /// Load the node body content into the specified `variable`.  Return 0
    /// on success, and a non-zero value otherwise.
    int readVectorChar(bsl::vector<char> *variable);

    /// Load the node body content into the specified `variable`.  Return 0
    /// on success, and a non-zero value otherwise.
    int readVectorUnsignedChar(bsl::vector<unsigned char> *variable);

    // ACCESSORS

    /// Return the address of the parent node.
    BerDecoder_Node *parent() const;

    /// Return the BER tag class for this node.
    BerConstants::TagClass tagClass() const;

    /// Return the BER tag type for this node.
    BerConstants::TagType tagType() const;

    /// Return the BER tag number for this node.
    int tagNumber() const;

    /// Return formatting mode for this node.
    int formattingMode() const;

    /// Return field name for this node.
    const char *fieldName() const;

    /// Return expected length of the body or -1 when the length is indefinite.
    int length() const;

    /// Return the position of node tag from the beginning of input stream.
    int startPos() const;
};

                    // ====================================
                    // private class BerDecoder_NodeVisitor
                    // ====================================

/// This class is used as a visitor for visiting contained objects during
/// decoding.
class BerDecoder_NodeVisitor {

    // DATA
    BerDecoder_Node *d_node;  // current node, held, not owned

    // NOT IMPLEMENTED
    BerDecoder_NodeVisitor(const BerDecoder_NodeVisitor&);         // = delete;
    BerDecoder_NodeVisitor& operator=(const BerDecoder_NodeVisitor&);
                                                                   // = delete;

  public:
    // CREATORS
    BerDecoder_NodeVisitor(BerDecoder_Node *node);

    //! ~BerDecoder_NodeVisitor() = default;

    // MANIPULATORS
    template <typename TYPE, typename INFO>
    int operator()(TYPE *variable, const INFO& info);
};

              // ================================================
              // private class BerDecoder_UniversalElementVisitor
              // ================================================

/// This `class` is used as a visitor for visiting the top-level element and
/// also array elements during decoding.  This class is required so that the
/// universal tag number of the element can be determined when the element
/// is visited.
class BerDecoder_UniversalElementVisitor {

    // DATA
    BerDecoder_Node d_node;  // a new node

    // NOT IMPLEMENTED
    BerDecoder_UniversalElementVisitor(
                               const BerDecoder_UniversalElementVisitor&);
                                                                   // = delete;
    BerDecoder_UniversalElementVisitor& operator=(
                               const BerDecoder_UniversalElementVisitor&);
                                                                   // = delete;

  public:
    // CREATORS
    BerDecoder_UniversalElementVisitor(BerDecoder *d_decoder);

    //! ~BerDecoder_UniversalElementVisitor() = default;

    // MANIPULATORS
    template <typename TYPE>
    int operator()(TYPE *variable);
};

                          // =======================
                          // class BerDecoder_Zeroer
                          // =======================

/// This class is a deleter that just zeroes out a given pointer upon
/// destruction, for making code exception-safe.
class BerDecoder_Zeroer {

    // DATA
    const BerDecoderOptions **d_options_p;  // address of pointer to zero
                                            // out upon destruction

  public:
    // CREATORS
    BerDecoder_Zeroer(const BerDecoderOptions **options)
    : d_options_p(options)
    {
    }

    ~BerDecoder_Zeroer()
    {
        *d_options_p = 0;
    }
};

}  // close package namespace

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                   // --------------------------------------
                   // class balber::BerDecoder::MemOutStream
                   // --------------------------------------

// CREATORS
inline
balber::BerDecoder::MemOutStream::MemOutStream(
                                              bslma::Allocator *basicAllocator)
: bsl::ostream(0)
, d_sb(bslma::Default::allocator(basicAllocator))
{
    rdbuf(&d_sb);
}

// MANIPULATORS
inline
void balber::BerDecoder::MemOutStream::reset()
{
    d_sb.reset();
}

// ACCESSORS
inline
const char *balber::BerDecoder::MemOutStream::data() const
{
    return d_sb.data();
}

inline
int balber::BerDecoder::MemOutStream::length() const
{
    return (int)d_sb.length();
}

namespace balber {
                              // ----------------
                              // class BerDecoder
                              // ----------------

// MANIPULATORS
inline BerDecoder::ErrorSeverity
BerDecoder::logError(const char *msg)
{
    // This is inline just so compilers see it cannot return SUCCESS, thereby
    // improving flow analysis and choking off spurious warnings.

    logErrorImp(msg);
    return e_BER_ERROR;
}

inline
bsl::ostream& BerDecoder::logStream()
{
    if (0 == d_logStream) {
        d_logStream = new(d_logArea.buffer()) MemOutStream(d_allocator);
    }
    return *d_logStream;
}

template <typename TYPE>
inline
int BerDecoder::decode(bsl::istream& stream, TYPE *variable)
{
    if (!stream.good()) {
        return -1;                                                    // RETURN
    }

    if (0 != this->decode(stream.rdbuf(), variable)) {
        stream.setstate(bsl::ios_base::failbit);
        return -1;                                                    // RETURN
    }

    return 0;
}

template <typename TYPE>
int BerDecoder::decode(bsl::streambuf *streamBuf, TYPE *variable)
{
    BSLS_ASSERT(0 == d_streamBuf);

    d_streamBuf                 = streamBuf;
    d_currentDepth              = 0;
    d_severity                  = e_BER_SUCCESS;
    d_numUnknownElementsSkipped = 0;

    if (d_logStream != 0) {
        d_logStream->reset();
    }

    d_topNode = 0;

    bdlat_ValueTypeFunctions::reset(variable);

    int rc = d_severity;

    if (! d_options) {
        // Create temporary options object
        BerDecoderOptions                  options; d_options = &options;
        BerDecoder_Zeroer                  zeroer(&d_options);
        BerDecoder_UniversalElementVisitor visitor(this);
        rc = visitor(variable);
    }
    else {
        BerDecoder_UniversalElementVisitor visitor(this);
        rc = visitor(variable);
    }

    d_streamBuf = 0;
    return rc;
}

inline
void BerDecoder::setNumUnknownElementsSkipped(int value)
{
    BSLS_ASSERT(0 <= value);

    d_numUnknownElementsSkipped = value;
}

// ACCESSORS
inline
const BerDecoderOptions *BerDecoder::decoderOptions() const
{
    return d_options;
}

inline
BerDecoder::ErrorSeverity BerDecoder::errorSeverity() const
{
    return d_severity;
}

inline
bslstl::StringRef BerDecoder::loggedMessages() const
{
    if (d_logStream) {
        return bslstl::StringRef(d_logStream->data(),
                                 d_logStream->length());              // RETURN
    }

    return bslstl::StringRef();
}

inline
bool BerDecoder::maxDepthExceeded() const
{
    return d_currentDepth > d_options->maxDepth();
}

inline
int BerDecoder::numUnknownElementsSkipped() const
{
    return d_numUnknownElementsSkipped;
}

                       // -----------------------------
                       // private class BerDecoder_Node
                       // -----------------------------

// CREATORS
inline
BerDecoder_Node::BerDecoder_Node(BerDecoder *decoder)
: d_decoder            (decoder)
, d_parent             (d_decoder->d_topNode)
, d_tagClass           (BerConstants::e_UNIVERSAL)
, d_tagType            (BerConstants::e_PRIMITIVE)
, d_tagNumber          (0)
, d_expectedLength     (0)
, d_consumedHeaderBytes(0)
, d_consumedBodyBytes  (0)
, d_consumedTailBytes  (0)
, d_formattingMode     (bdlat_FormattingMode::e_DEFAULT)
, d_fieldName          (0)
{
    ++d_decoder->d_currentDepth;
    if (d_parent) {
        d_formattingMode = d_parent->d_formattingMode;
    }
    d_decoder->d_topNode = this;
}

inline
BerDecoder_Node::~BerDecoder_Node()
{
    if (d_parent) {
        d_parent->d_consumedBodyBytes += d_consumedHeaderBytes
                                      +  d_consumedBodyBytes
                                      +  d_consumedTailBytes;
    }

    d_decoder->d_topNode = d_parent;
    --d_decoder->d_currentDepth;
}

// MANIPULATORS
inline
bool
BerDecoder_Node::hasMore()
{
    BSLS_ASSERT(d_tagType == BerConstants::e_CONSTRUCTED);

    if (BerUtil::k_INDEFINITE_LENGTH == d_expectedLength) {
        return 0 != d_decoder->d_streamBuf->sgetc();                  // RETURN
    }

    return d_expectedLength > d_consumedBodyBytes;
}

// ACCESSORS
inline
BerDecoder_Node*BerDecoder_Node::parent() const
{
    return d_parent;
}

inline
BerConstants::TagClass BerDecoder_Node::tagClass() const
{
    return d_tagClass;
}

inline
BerConstants::TagType BerDecoder_Node::tagType() const
{
    return d_tagType;
}

inline
int BerDecoder_Node::tagNumber() const
{
    return d_tagNumber;
}

inline
int BerDecoder_Node::formattingMode() const
{
    return d_formattingMode;
}

inline
const char *BerDecoder_Node::fieldName() const
{
    return d_fieldName;
}

inline
int BerDecoder_Node::length() const
{
    return d_expectedLength;
}

// MANIPULATORS
inline
void BerDecoder_Node::setFormattingMode(int formattingMode)
{
    d_formattingMode = formattingMode;
}

inline
void BerDecoder_Node::setFieldName(const char *name)
{
    d_fieldName = name;
}

template <typename TYPE>
inline
int BerDecoder_Node::operator()(TYPE *, bslmf::Nil)
{
    BSLS_ASSERT(0 && "Should never execute this function");

    return -1;
}

template <typename TYPE, typename ANY_CATEGORY>
inline
int BerDecoder_Node::operator()(TYPE *object, ANY_CATEGORY category)
{
    return this->decode(object, category);
}

template <typename TYPE>
inline
int BerDecoder_Node::operator()(TYPE *object)
{
    typedef typename bdlat_TypeCategory::Select<TYPE>::Type Tag;
    return this->decode(object, Tag());
}

// PRIVATE MANIPULATORS
template <typename TYPE>
int
BerDecoder_Node::decode(TYPE *variable, bdlat_TypeCategory::Choice)
{
    // A misunderstanding of X.694 (clause 20.4), an XML choice (not anonymous)
    // element is encoded as a sequence (outer) with 1 element (inner).
    // However, if the element is anonymous (i.e., untagged), then there is no
    // inner tag.  This behavior is kept for backward compatibility.

    if (d_tagType != BerConstants::e_CONSTRUCTED) {
        return logError("Expected CONSTRUCTED tag type for choice");  // RETURN
    }

    bool isUntagged = d_formattingMode & bdlat_FormattingMode::e_UNTAGGED;

    int rc = BerDecoder::e_BER_SUCCESS;

    if (!isUntagged) {

        // 'typename' will be taken from predecessor node.
        BerDecoder_Node innerNode(d_decoder);
        rc = innerNode.readTagHeader();
        if (rc != BerDecoder::e_BER_SUCCESS) {
            return rc;  // error message is already logged            // RETURN
        }

        if (innerNode.tagClass() != BerConstants::e_CONTEXT_SPECIFIC) {
            return innerNode.logError(
                    "Expected CONTEXT tag class for tagged choice");  // RETURN
        }

        if (innerNode.tagType() != BerConstants::e_CONSTRUCTED) {
            return innerNode.logError(
                 "Expected CONSTRUCTED tag type for tagged choice");  // RETURN
        }

        if (innerNode.tagNumber() != 0) {
            return innerNode.logError(
                    "Expected 0 as a tag number for tagged choice");  // RETURN
        }

        if (innerNode.hasMore()) {
            // if shouldContinue returns false, then there is no selection
            rc  = innerNode.decodeChoice(variable);
            if (rc != BerDecoder::e_BER_SUCCESS) {
                return rc;  // error message is already logged        // RETURN
            }
        }

        rc = innerNode.readTagTrailer();
    }
    else if (this->hasMore()) {

       // if shouldContinue returns false, then there is no selection
       rc = this->decodeChoice(variable);
    }

    return rc;
}

template <typename TYPE>
int
BerDecoder_Node::decode(TYPE *variable, bdlat_TypeCategory::NullableValue)
{
    int rc = BerDecoder::e_BER_SUCCESS;

    if (d_formattingMode & bdlat_FormattingMode::e_NILLABLE) {
        // nillable is encoded in BER as a sequence with one optional element

        if (d_tagType != BerConstants::e_CONSTRUCTED) {
            return logError(
                      "Expected CONSTRUCTED tag type for nullable");  // RETURN
        }

        if (hasMore()) {

            // If 'hasMore' returns false, then the nullable value is null.
            BerDecoder_Node innerNode(d_decoder);
            rc = innerNode.readTagHeader();
            if (rc != BerDecoder::e_BER_SUCCESS) {
                return rc;  // error message is already logged        // RETURN
            }

            if (innerNode.tagClass() != BerConstants::e_CONTEXT_SPECIFIC) {
                return innerNode.logError("Expected CONTEXT tag class for "
                                          "inner nillable");          // RETURN
            }

            if (innerNode.tagNumber() != 0) {
                return innerNode.logError(
                     "Expected 0 as tag number for inner nillable");  // RETURN
            }

            bdlat_NullableValueFunctions::makeValue(variable);

            rc = bdlat_NullableValueFunctions::manipulateValue(variable,
                                                               innerNode);
            if (rc != BerDecoder::e_BER_SUCCESS) {
                return rc;  // error message is already logged        // RETURN
            }

            rc = innerNode.readTagTrailer();

        }  // this->hasMore()
        else {
            bdlat_ValueTypeFunctions::reset(variable);
        }
    }
    else {  // not 'bdlat_FormattingMode::e_NILLABLE'
        bdlat_NullableValueFunctions::makeValue(variable);
        rc = bdlat_NullableValueFunctions::manipulateValue(variable, *this);
    }

    return rc;
}

template <typename TYPE>
int
BerDecoder_Node::decode(TYPE *variable, bdlat_TypeCategory::CustomizedType)
{
    typedef typename
    bdlat_CustomizedTypeFunctions::BaseType<TYPE>::Type BaseType;

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"
#ifndef BSLS_PLATFORM_CMP_CLANG
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#endif

    BaseType base;

    typedef typename bdlat_TypeCategory::Select<BaseType>::Type BaseTag;
    int rc = this->decode(&base, BaseTag());

    if (rc != BerDecoder::e_BER_SUCCESS) {
        return rc;  // error message is already logged                // RETURN
    }

    if (bdlat_CustomizedTypeFunctions::convertFromBaseType(variable,
                                                           base) != 0) {
        return logError(
                  "Error converting from base type for customized");  // RETURN
    }

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

    return BerDecoder::e_BER_SUCCESS;
}

template <typename TYPE>
int
BerDecoder_Node::decode(TYPE *variable, bdlat_TypeCategory::Enumeration)
{
    int value = 0;
    int rc    = this->decode(&value, bdlat_TypeCategory::Simple());

    if (rc != BerDecoder::e_BER_SUCCESS) {
        return rc;  // error message is already logged                // RETURN
    }

    if (0 != bdlat::EnumUtil::fromIntOrFallbackIfEnabled(variable, value)) {
        return logError("Error converting enumeration value");        // RETURN
    }

    return BerDecoder::e_BER_SUCCESS;
}

template <typename TYPE>
inline
int BerDecoder_Node::decode(TYPE *variable, bdlat_TypeCategory::Simple)
{
    if (d_tagType != BerConstants::e_PRIMITIVE) {
        return logError(
                     "Expected PRIMITIVE tag type for simple type");  // RETURN
    }

    if (BerUtil::getValue(d_decoder->d_streamBuf,
                          variable,
                          d_expectedLength,
                          *d_decoder->d_options) != 0) {
        return logError("Error reading value for simple type");       // RETURN
    }

    d_consumedBodyBytes = d_expectedLength;

    return BerDecoder::e_BER_SUCCESS;
}

template <typename TYPE>
int
BerDecoder_Node::decode(TYPE *variable, bdlat_TypeCategory::Sequence)
{
    if (d_tagType != BerConstants::e_CONSTRUCTED) {
        return logError(
                      "Expected CONSTRUCTED tag type for sequence");  // RETURN
    }

    while (this->hasMore()) {

        BerDecoder_Node innerNode(d_decoder);

        int rc = innerNode.readTagHeader();
        if (rc != BerDecoder::e_BER_SUCCESS) {
            return rc;  // error message is already logged            // RETURN
        }

        if (innerNode.tagClass() != BerConstants::e_CONTEXT_SPECIFIC) {
            return innerNode.logError(
                      "Expected CONTEXT tag class inside sequence");  // RETURN
        }

        if (bdlat_SequenceFunctions::hasAttribute(*variable,
                                                  innerNode.tagNumber())) {

            BerDecoder_NodeVisitor visitor(&innerNode);

            rc = bdlat_SequenceFunctions::manipulateAttribute(
                                                        variable,
                                                        visitor,
                                                        innerNode.tagNumber());
        }
        else if (BerConstants::k_ARRAY_LENGTH_HINT_TAG_NUMBER ==
                                                       innerNode.tagNumber()) {
            rc = innerNode.decode(&d_decoder->d_arrayLengthHint,
                                  bdlat_TypeCategory::Simple());
        }
        else {
            rc = innerNode.skipField();
            d_decoder->setNumUnknownElementsSkipped(
                                   d_decoder->numUnknownElementsSkipped() + 1);
        }

        if (rc != BerDecoder::e_BER_SUCCESS) {
            return rc;  // error message is already logged            // RETURN
        }

        rc = innerNode.readTagTrailer();
        if (rc != BerDecoder::e_BER_SUCCESS) {
            return rc;  // error message is already logged            // RETURN
        }
    }

    return BerDecoder::e_BER_SUCCESS;
}

template <typename TYPE>
inline
int BerDecoder_Node::decode(TYPE *variable, bdlat_TypeCategory::Array)
{
    // Note: 'bsl::vector<char>' and 'bsl::vector<unsigned char>' are
    // handled as special cases in the CPP file.

    return this->decodeArray(variable);
}

template <typename TYPE>
inline
int BerDecoder_Node::decode(TYPE *variable, bdlat_TypeCategory::DynamicType)
{
    return bdlat_TypeCategoryUtil::manipulateByCategory(variable, *this);
}

template <typename TYPE>
int BerDecoder_Node::decodeChoice(TYPE *variable)
{
    BerDecoder_Node innerNode(d_decoder);

    int rc = innerNode.readTagHeader();
    if (rc != BerDecoder::e_BER_SUCCESS) {
        return rc;  // error message is already logged                // RETURN
    }

    if (innerNode.tagClass() != BerConstants::e_CONTEXT_SPECIFIC) {
        return innerNode.logError(
                        "Expected CONTEXT tag class for internal ");  // RETURN
    }

    if (bdlat_ChoiceFunctions::hasSelection(*variable,
                                            innerNode.tagNumber())) {

        if (0 != bdlat_ChoiceFunctions::makeSelection(variable,
                                                      innerNode.tagNumber())) {
            return innerNode.logError(
                                 "Unable to make choice selection");  // RETURN
        }

        BerDecoder_NodeVisitor visitor(&innerNode);

        rc = bdlat_ChoiceFunctions::manipulateSelection(variable, visitor);
    }
    else {
        rc = innerNode.skipField();
        d_decoder->setNumUnknownElementsSkipped(
                                   d_decoder->numUnknownElementsSkipped() + 1);
    }

    if (rc != BerDecoder::e_BER_SUCCESS) {
        return rc;  // error message is already logged                // RETURN
    }

    return innerNode.readTagTrailer();
}

template <typename TYPE>
int
BerDecoder_Node::decodeArray(TYPE *variable)
{
    if (d_tagType != BerConstants::e_CONSTRUCTED) {
        return logError("Expected CONSTRUCTED tag class for array");  // RETURN
    }

    const int maxSize = d_decoder->decoderOptions()->maxSequenceSize();

    const int arrayLengthHint = d_decoder->d_arrayLengthHint;
    if (0 < arrayLengthHint) {
        bdlat_ArrayFunctions::reserve(variable,
                                        arrayLengthHint <= maxSize
                                      ? arrayLengthHint
                                      : maxSize);
        d_decoder->d_arrayLengthHint = 0;
    }

    int i = static_cast<int>(bdlat_ArrayFunctions::size(*variable));
    while (this->hasMore()) {
        int j = i + 1;

        if (j > maxSize) {
            return logError("Array size exceeds the limit");          // RETURN
        }

        bdlat_ArrayFunctions::resize(variable, j);

        BerDecoder_UniversalElementVisitor visitor(d_decoder);
        int rc = bdlat_ArrayFunctions::manipulateElement(variable, visitor, i);
        if (rc != BerDecoder::e_BER_SUCCESS) {
            return logError("Error in decoding array element");       // RETURN
        }
        i = j;
    }

    return BerDecoder::e_BER_SUCCESS;
}

                    // ------------------------------------
                    // private class BerDecoder_NodeVisitor
                    // ------------------------------------

// CREATORS
inline
BerDecoder_NodeVisitor::
BerDecoder_NodeVisitor(BerDecoder_Node *node)
: d_node(node)
{
}

// MANIPULATORS
template <typename TYPE, typename INFO>
inline
int BerDecoder_NodeVisitor::operator()(TYPE *variable, const INFO& info)
{
    d_node->setFormattingMode(info.formattingMode());
    d_node->setFieldName(info.name());

    return d_node->operator()(variable);
}

              // ------------------------------------------------
              // private class BerDecoder_UniversalElementVisitor
              // ------------------------------------------------

// CREATORS
inline
BerDecoder_UniversalElementVisitor::
BerDecoder_UniversalElementVisitor(BerDecoder *decoder)
: d_node(decoder)
{
}

// MANIPULATORS
template <typename TYPE>
int BerDecoder_UniversalElementVisitor::operator()(TYPE *variable)
{
    int                          alternateTag      = -1;
    BerUniversalTagNumber::Value expectedTagNumber =
                         BerUniversalTagNumber::select(*variable,
                                                       d_node.formattingMode(),
                                                       &alternateTag);

    int rc = d_node.readTagHeader();
    if (rc != BerDecoder::e_BER_SUCCESS) {
        return rc;  // error message is already logged                // RETURN
    }

    if (d_node.tagClass() != BerConstants::e_UNIVERSAL) {
        return d_node.logError("Expected UNIVERSAL tag class");       // RETURN
    }

    if (d_node.tagNumber() != static_cast<int>(expectedTagNumber)) {
        if (-1 == alternateTag || d_node.tagNumber() != alternateTag) {
            return d_node.logError("Unexpected tag number");          // RETURN
        }
    }

    rc = d_node(variable);

    if (rc != BerDecoder::e_BER_SUCCESS) {
        return rc;                                                    // RETURN
    }

    rc = d_node.readTagTrailer();

    return rc;
}

}  // close package namespace
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
