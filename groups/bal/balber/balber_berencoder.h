// balber_berencoder.h                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALBER_BERENCODER
#define INCLUDED_BALBER_BERENCODER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a BER encoder class.
//
//@CLASSES:
//   balber::BerEncoder: BER encoder
//
//@SEE_ALSO: balber_berdecoder, bdem_bdemencoder, balxml_encoder
//
//@DESCRIPTION: This component defines a single class, 'balber::BerEncoder',
// that contains a parameterized 'encode' function.  The 'encode' function
// encodes data read from a specified stream and loads the corresponding object
// to an object of the parameterized type.  The 'encode' method is overloaded
// for two types of input streams:
//: o 'bsl::streambuf'
//: o 'bsl::istream'
//
// This component encodes objects based on the X.690 BER specification.  It can
// only be used with types supported by the 'bdlat' framework.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Encoding an Employee Record
/// - - - - - - - - - - - - - - - - - - -
// Suppose that an "employee record" consists of a sequence of attributes --
// 'name', 'age', and 'salary' -- that are of types 'bsl::string', 'int', and
// 'float', respectively.  Furthermore, we have a need to BER encode employee
// records as a sequence of values (for out-of-process consumption).
//
// Assume that we have defined a 'usage::EmployeeRecord' class to represent
// employee record values, and assume that we have provided the 'bdlat'
// specializations that allow the 'balber' codec components to represent class
// values as a sequence of BER primitive values.  See
// {'bdlat_sequencefunctions'|Usage} for details of creating specializations
// for a sequence type.
//
// First, we create an employee record object having typical values:
//..
//  usage::EmployeeRecord bob("Bob", 56, 1234.00);
//  assert("Bob"   == bob.name());
//  assert(  56    == bob.age());
//  assert(1234.00 == bob.salary());
//..
// Now, we create a 'balber::Encoder' object and use it to encode our 'bob'
// object.  Here, to facilitate the examination of our results, the BER
// encoding data is delivered to a 'bslsb::MemOutStreamBuf' object:
//..
//  bdlsb::MemOutStreamBuf osb;
//  balber::BerEncoder     encoder;
//  int                    rc = encoder.encode(&osb, bob);
//  assert( 0 == rc);
//  assert(18 == osb.length());
//..
// Finally, we confirm that the generated BER encoding has the expected layout
// and values.  We create an 'bdlsb::FixedMemInStreamBuf' to manage our access
// to the data portion of the 'bdlsb::MemOutStreamBuf' where our BER encoding
// resides:
//..
//  bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());
//..
// The 'balber_berutil' component provides functions that allow us to decode
// the descriptive fields and values of the BER encoded sequence:
//..
//  balber::BerConstants::TagClass tagClass;
//  balber::BerConstants::TagType  tagType;
//  int                            tagNumber;
//  int                            accumNumBytesConsumed = 0;
//  int                            length;
//
//  rc = balber::BerUtil::getIdentifierOctets(&isb,
//                                            &tagClass,
//                                            &tagType,
//                                            &tagNumber,
//                                            &accumNumBytesConsumed);
//  assert(0                                             == rc);
//  assert(balber::BerConstants::e_UNIVERSAL             == tagClass);
//  assert(balber::BerConstants::e_CONSTRUCTED           == tagType);
//  assert(balber::BerUniversalTagNumber::e_BER_SEQUENCE == tagNumber);
//
//  rc = balber::BerUtil::getLength(&isb, &length, &accumNumBytesConsumed);
//  assert(0                                    == rc);
//  assert(balber::BerUtil::e_INDEFINITE_LENGTH == length);
//..
// The 'UNIVERSAL' value in 'tagClass' indicates that the 'tagNumber' value
// represents a type in the BER standard, a 'BER_SEQUENCE', as we requested of
// the infrastructure (see the 'IsSequence' specialization above).  The
// 'tagType' value of 'CONSTRUCTED' indicates that this is a non-primitive
// type.  The 'INDEFINITE' value for length is typical for sequence encodings.
// In these cases, the end-of-data is indicated by a sequence to two null
// bytes.
//
// We now examine the tags and values corresponding to each of the data members
// of 'usage::EmployeeRecord' class.  For each of these the 'tagClass' is
// 'CONTEXT_SPECIFIC' (i.e., member of a larger construct) and the 'tagType' is
// 'PRIMITIVE' ('bsl::string', 'int', and 'float' each correspond to a
// primitive BER type.  The 'tagNumber' for each field was defined (in the
// elided definiton) to correspond the position of the field in the
// 'usage::EmployeeRecord' class.
//..
//  rc = balber::BerUtil::getIdentifierOctets(&isb,
//                                            &tagClass,
//                                            &tagType,
//                                            &tagNumber,
//                                            &accumNumBytesConsumed);
//  assert(0                                        == rc);
//  assert(balber::BerConstants::e_CONTEXT_SPECIFIC == tagClass);
//  assert(balber::BerConstants::e_PRIMITIVE        == tagType);
//  assert(1                                        == tagNumber);
//
//  bsl::string name;
//  rc = balber::BerUtil::getValue(&isb, &name, &accumNumBytesConsumed);
//  assert(0     == rc);
//  assert("Bob" == name);
//
//  rc = balber::BerUtil::getIdentifierOctets(&isb,
//                                            &tagClass,
//                                            &tagType,
//                                            &tagNumber,
//                                            &accumNumBytesConsumed);
//  assert(0                                        == rc);
//  assert(balber::BerConstants::e_CONTEXT_SPECIFIC == tagClass);
//  assert(balber::BerConstants::e_PRIMITIVE        == tagType);
//  assert(2                                        == tagNumber);
//
//  int age;
//  rc = balber::BerUtil::getValue(&isb, &age, &accumNumBytesConsumed);
//  assert(0  == rc);
//  assert(56 == age);
//
//  rc = balber::BerUtil::getIdentifierOctets(&isb,
//                                            &tagClass,
//                                            &tagType,
//                                            &tagNumber,
//                                            &accumNumBytesConsumed);
//  assert(0 == rc);
//  assert(balber::BerConstants::e_CONTEXT_SPECIFIC == tagClass);
//  assert(balber::BerConstants::e_PRIMITIVE        == tagType);
//  assert(3                                        == tagNumber);
//
//  float salary;
//  rc = balber::BerUtil::getValue(&isb, &salary, &accumNumBytesConsumed);
//  assert(0       == rc);
//  assert(1234.00 == salary);
//..
// Lastly, we confirm that end-of-data sequence (two null bytes) are found we
// expect them and that we have entirely consumed the data that we generated by
// our encoding.
//..
//  rc = balber::BerUtil::getEndOfContentOctets(&isb, &accumNumBytesConsumed);
//  assert(0            == rc);
//  assert(osb.length() == static_cast<bsl::size_t>(accumNumBytesConsumed));
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALBER_BERCONSTANTS
#include <balber_berconstants.h>
#endif

#ifndef INCLUDED_BALBER_BERENCODEROPTIONS
#include <balber_berencoderoptions.h>
#endif

#ifndef INCLUDED_BALBER_BERUNIVERSALTAGNUMBER
#include <balber_beruniversaltagnumber.h>
#endif

#ifndef INCLUDED_BALBER_BERUTIL
#include <balber_berutil.h>
#endif

#ifndef INCLUDED_BDLAT_ARRAYFUNCTIONS
#include <bdlat_arrayfunctions.h>
#endif

#ifndef INCLUDED_BDLAT_ATTRIBUTEINFO
#include <bdlat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDLAT_CHOICEFUNCTIONS
#include <bdlat_choicefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_CUSTOMIZEDTYPEFUNCTIONS
#include <bdlat_customizedtypefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_ENUMFUNCTIONS
#include <bdlat_enumfunctions.h>
#endif

#ifndef INCLUDED_BDLAT_FORMATTINGMODE
#include <bdlat_formattingmode.h>
#endif

#ifndef INCLUDED_BDLAT_NULLABLEVALUEFUNCTIONS
#include <bdlat_nullablevaluefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_SEQUENCEFUNCTIONS
#include <bdlat_sequencefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_TYPECATEGORY
#include <bdlat_typecategory.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BDLSB_MEMOUTSTREAMBUF
#include <bdlsb_memoutstreambuf.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_TYPEINFO
#include <bsl_typeinfo.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {
namespace balber {

struct BerEncoder_encodeProxy;
class  BerEncoder_Visitor;
class  BerEncoder_UniversalElementVisitor;
class  BerEncoder_LevelGuard;

                              // ================
                              // class BerEncoder
                              // ================

class BerEncoder {
    // This class contains the parameterized 'encode' functions that encode
    // 'bdlat' types to an outgoing stream in BER format.

  private:
    // FRIENDS
    friend struct BerEncoder_encodeProxy;
    friend class  BerEncoder_Visitor;
    friend class  BerEncoder_UniversalElementVisitor;
    friend class  BerEncoder_LevelGuard;

    // PRIVATE TYPES
    class MemOutStream : public bsl::ostream {
        // This class provides stream for logging using
        // 'bdlsb::MemOutStreamBuf' as a streambuf.  The logging stream is
        // created on demand, i.e., during the first attempt to log message.

        // DATA
        bdlsb::MemOutStreamBuf d_sb;

        // NOT IMPLEMENTED
        MemOutStream(const MemOutStream&);             // = delete;
        MemOutStream& operator=(const MemOutStream&);  // = delete;

      public:
        // CREATORS
        MemOutStream(bslma::Allocator *basicAllocator = 0);
            // Create a 'MemOutStream' object Optionally specify a
            // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
            // 0, the currently installed default allocator is used.

        virtual ~MemOutStream();
            // Destroy this stream and release memory back to the allocator.
            //
            // Although the compiler should generate this destructor
            // implicitly, xlC 8 breaks when the destructor is called by name
            // unless it is explicitly declared.

        // MANIPULATORS
        void reset();
            // Reset the internal streambuf to empty.

        // ACCESSORS
        const char *data() const;
            // Return the address of the memory containing the values formatted
            // to this stream.  The data is not null-terminated unless a null
            // character was appended onto this stream.

        int length() const;
            // Return the length of of the formatted data, including null
            // characters appended to the stream, if any.
    };

  public:
    // PUBLIC TYPES
    enum ErrorSeverity {
        e_BER_SUCCESS = 0x00
      , e_BER_ERROR   = 0x02

    };

  private:
    // DATA
    const BerEncoderOptions          *d_options;        // held, not owned
    bslma::Allocator                 *d_allocator;      // held, not owned

    bsls::ObjectBuffer<MemOutStream>  d_logArea;
        // placeholder for MemOutStream

    MemOutStream                     *d_logStream;
        // if not zero, log stream was created at the moment of first logging
        // and must be destroyed

    ErrorSeverity                     d_severity;       // error severity

    bsl::streambuf                   *d_streamBuf;      // held, not owned
    int                               d_currentDepth;   // current depth

    // NOT IMPLEMENTED
    BerEncoder(const BerEncoder&);             // = delete;
    BerEncoder& operator=(const BerEncoder&);  // = delete;

    // PRIVATE MANIPULATORS
    ErrorSeverity logMsg(const char             *msg,
                         BerConstants::TagClass  tagClass,
                         int                     tagNumber,
                         const char             *name  =  0,
                         int                     index = -1);
        // Log the specified 'msg' using the specified 'tagClass', 'tagNumber',
        // name, and 'index', and return 'errorSeverity()'.

    ErrorSeverity logError(BerConstants::TagClass  tagClass,
                           int                     tagNumber,
                           const char             *name  =  0,
                           int                     index = -1);
        // Log error and upgrade the severity level.  Return 'errorSeverity()'.

    bsl::ostream& logStream();
        // Return the stream for logging.  Note the if stream has not been
        // created yet, it will be created during this call.

    int encodeImpl(const bsl::vector<char>&  value,
                   BerConstants::TagClass    tagClass,
                   int                       tagNumber,
                   int                       formattingMode,
                   bdlat_TypeCategory::Array );

    template <typename TYPE>
    int encodeArrayImpl(const TYPE&            value,
                        BerConstants::TagClass tagClass,
                        int                    tagNumber,
                        int                    formattingMode);

    template <typename TYPE>
    int encodeImpl(const TYPE&                value,
                   BerConstants::TagClass     tagClass,
                   int                        tagNumber,
                   int                        formattingMode,
                   bdlat_TypeCategory::Choice );

    template <typename TYPE>
    int encodeImpl(const TYPE&                       value,
                   BerConstants::TagClass            tagClass,
                   int                               tagNumber,
                   int                               formattingMode,
                   bdlat_TypeCategory::NullableValue );

    template <typename TYPE>
    int encodeImpl(const TYPE&                        value,
                   BerConstants::TagClass             tagClass,
                   int                                tagNumber,
                   int                                formattingMode,
                   bdlat_TypeCategory::CustomizedType );

    template <typename TYPE>
    int encodeImpl(const TYPE&                     value,
                   BerConstants::TagClass          tagClass,
                   int                             tagNumber,
                   int                             formattingMode,
                   bdlat_TypeCategory::Enumeration );

    template <typename TYPE>
    int encodeImpl(const TYPE&                  value,
                   BerConstants::TagClass       tagClass,
                   int                          tagNumber,
                   int                          formattingMode,
                   bdlat_TypeCategory::Sequence );

    template <typename TYPE>
    int encodeImpl(const TYPE&                value,
                   BerConstants::TagClass     tagClass,
                   int                        tagNumber,
                   int                        formattingMode,
                   bdlat_TypeCategory::Simple );

    template <typename TYPE>
    int encodeImpl(const TYPE&               value,
                   BerConstants::TagClass    tagClass,
                   int                       tagNumber,
                   int                       formattingMode,
                   bdlat_TypeCategory::Array );

    template <typename TYPE>
    int encodeImpl(const TYPE&                     value,
                   BerConstants::TagClass          tagClass,
                   int                             tagNumber,
                   int                             formattingMode,
                   bdlat_TypeCategory::DynamicType );

  public:
    // CREATORS
    BerEncoder(const BerEncoderOptions *options        = 0,
               bslma::Allocator        *basicAllocator = 0);
        // Construct a encoder object.  Optionally specify decoder 'options'.
        // If 'options' is 0, 'BerEncoderOptions()' is used.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~BerEncoder();
        // Destroy this object.  This destruction has no effect on objects
        // pointed-to by the pointers provided at construction.

    template <typename TYPE>
    int encode(bsl::streambuf *streamBuf, const TYPE& value);
        // Encode the specified non-modifiable 'value' to the specified
        // 'streamBuf'.  Return 0 on success, and a non-zero value otherwise.

    template <typename TYPE>
    int encode(bsl::ostream& stream, const TYPE& value);
        // Encode the specified non-modifiable 'value' to the specified
        // 'stream'.  Return 0 on success, and a non-zero value otherwise.  If
        // the encoding fails 'stream' will be invalidated.

    // ACCESSORS
    const BerEncoderOptions *options() const;
        // Return address of the options.

    ErrorSeverity  errorSeverity() const;
        // Return the severity of the most severe warning or error encountered
        // during the last call to the 'encode' method.  The severity is reset
        // each time 'encode' is called.

    bslstl::StringRef loggedMessages() const;
        // Return a string containing any error, warning, or trace messages
        // that were logged during the last call to the 'encode' method.  The
        // log is reset each time 'encode' is called.
};

                    // ===================================
                    // private class BerEncoder_LevelGuard
                    // ===================================

class BerEncoder_LevelGuard {
    // This class serves the purpose to automatically increment-decrement the
    // current depth level.

    // DATA
    BerEncoder *d_encoder;

    // NOT IMPLEMENTED
    BerEncoder_LevelGuard(BerEncoder_LevelGuard&);            // = delete;
    BerEncoder_LevelGuard& operator=(BerEncoder_LevelGuard&); // = delete;

  public:
    // CREATORS
    BerEncoder_LevelGuard(BerEncoder *encoder);
    ~BerEncoder_LevelGuard();
};

                      // ================================
                      // private class BerEncoder_Visitor
                      // ================================

class BerEncoder_Visitor {
    // This class is used as a visitor for visiting contained objects during
    // encoding.  Produces always BER elements with CONTEXT_SPECIFIC BER tag.

    // DATA
    BerEncoder             *d_encoder;     // encoder to write data to
    BerEncoder_LevelGuard   d_levelGuard;

    // NOT IMPLEMENTED
    BerEncoder_Visitor(const BerEncoder_Visitor&);             // = delete;
    BerEncoder_Visitor& operator=(const BerEncoder_Visitor&);  // = delete;

  public:
    // CREATORS
    BerEncoder_Visitor(BerEncoder *encoder);
    ~BerEncoder_Visitor();

    // MANIPULATORS
    template <typename TYPE, typename INFO>
    int operator()(const TYPE& value, const INFO& info);
};

              // ================================================
              // private class BerEncoder_UniversalElementVisitor
              // ================================================

class BerEncoder_UniversalElementVisitor {
    // This class is used as a visitor for visiting the top-level element and
    // also array elements during encoding.  This class is required so that the
    // universal tag number of the element can be determined when the element
    // is visited.

    // PRIVATE DATA MEMBERS
    BerEncoder            *d_encoder;         // streambuf to write data to
    int                    d_formattingMode;  // formatting mode to use
    BerEncoder_LevelGuard  d_levelGuard;

  private:
    // NOT IMPLEMENTED
    BerEncoder_UniversalElementVisitor(
                                    const BerEncoder_UniversalElementVisitor&);
                                                                   // = delete;
    BerEncoder_UniversalElementVisitor& operator=(
                                    const BerEncoder_UniversalElementVisitor&);
                                                                   // = delete;
  public:
    // CREATORS
    BerEncoder_UniversalElementVisitor(BerEncoder *encoder,
                                       int         formattingMode);

    ~BerEncoder_UniversalElementVisitor();

    // MANIPULATORS
    template <typename TYPE>
    int operator()(const TYPE& value);
};

// ============================================================================
//                               PROXY CLASSES
// ============================================================================

                       // =============================
                       // struct BerEncoder_encodeProxy
                       // =============================

struct BerEncoder_encodeProxy {
    // Component-private struct.  Provides accessor that that keeps current
    // context and can be used in different 'bdlat' Category Functions.

    // DATA MEMBERS
    BerEncoder             *d_encoder;
    BerConstants::TagClass  d_tagClass;
    int                     d_tagNumber;
    int                     d_formattingMode;

    // CREATORS Creators have been omitted to allow simple static
    // initialization of this struct.

    // FUNCTIONS
    template <typename TYPE>
    int operator()(const TYPE& object, bslmf::Nil);

    template <typename TYPE, typename ANY_CATEGORY>
    int operator()(const TYPE& object, ANY_CATEGORY category);

    template <typename TYPE>
    int operator()(const TYPE& object);
};

}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                   // --------------------------------------
                   // class balber::BerEncoder::MemOutStream
                   // --------------------------------------

inline
balber::BerEncoder::MemOutStream::MemOutStream(
                                              bslma::Allocator *basicAllocator)
: bsl::ostream(0)
, d_sb(bslma::Default::allocator(basicAllocator))
{
    rdbuf(&d_sb);
}

// MANIPULATORS
inline
void balber::BerEncoder::MemOutStream::reset()
{
    d_sb.reset();
}

// ACCESSORS
inline
const char *balber::BerEncoder::MemOutStream::data() const
{
    return d_sb.data();
}

inline
int balber::BerEncoder::MemOutStream::length() const
{
    return static_cast<int>(d_sb.length());
}

namespace balber {

                        // ----------------------------
                        // class BerEncoder::LevelGuard
                        // ----------------------------

inline
BerEncoder_LevelGuard::BerEncoder_LevelGuard(BerEncoder *encoder)
: d_encoder (encoder)
{
    ++d_encoder->d_currentDepth;
}

inline
BerEncoder_LevelGuard::~BerEncoder_LevelGuard()
{
    --d_encoder->d_currentDepth;
}

                       // -----------------------------
                       // struct BerEncoder_encodeProxy
                       // -----------------------------

template <typename TYPE>
inline
int BerEncoder_encodeProxy::operator()(const TYPE&, bslmf::Nil)
{
    BSLS_ASSERT_SAFE(0);
    return -1;
}

template <typename TYPE, typename ANY_CATEGORY>
inline
int BerEncoder_encodeProxy::operator()(const TYPE&  object,
                                       ANY_CATEGORY category)
{
    return d_encoder->encodeImpl(object,
                                 d_tagClass,
                                 d_tagNumber,
                                 d_formattingMode,
                                 category);
}

template <typename TYPE>
inline
int BerEncoder_encodeProxy::operator()(const TYPE& object)
{
    typedef typename
    bdlat_TypeCategory::Select<TYPE>::Type TypeCategory;

    return this->operator()(object, TypeCategory());
}

                              // ----------------
                              // class BerEncoder
                              // ----------------

// ACCESSORS
inline
const BerEncoderOptions *BerEncoder::options() const
{
    return d_options;
}

inline
BerEncoder::ErrorSeverity BerEncoder::errorSeverity() const
{
    return d_severity;
}

inline
bslstl::StringRef BerEncoder::loggedMessages() const
{
    if (d_logStream) {
        return bslstl::StringRef(d_logStream->data(), d_logStream->length());
    }

    return bslstl::StringRef();
}

inline
bsl::ostream& BerEncoder::logStream()
{
    if (d_logStream == 0) {
        d_logStream = new(d_logArea.buffer()) MemOutStream(d_allocator);
    }
    return *d_logStream;
}

template <typename TYPE>
int BerEncoder::encode(bsl::streambuf *streamBuf, const TYPE& value)
{
    BSLS_ASSERT(!d_streamBuf);

    d_streamBuf = streamBuf;
    d_severity  = e_BER_SUCCESS;

    if (d_logStream != 0) {
        d_logStream->reset();
    }

    d_currentDepth = 0;

    int rc;

    if (! d_options) {
        BerEncoderOptions options;  // temporary options object
        d_options = &options;
        BerEncoder_UniversalElementVisitor visitor(
                                              this,
                                              bdlat_FormattingMode::e_DEFAULT);

        rc = visitor(value);
        d_options = 0;
    }
    else {
        BerEncoder_UniversalElementVisitor visitor(
                                              this,
                                              bdlat_FormattingMode::e_DEFAULT);
        rc = visitor(value);
    }

    d_streamBuf = 0;

    streamBuf->pubsync();

    return rc;
}

template <typename TYPE>
int BerEncoder::encode(bsl::ostream& stream, const TYPE& value)
{
    if (!stream.good()) {
        return -1;
    }

    if (0 != this->encode(stream.rdbuf(), value)) {
        stream.setstate(bsl::ios_base::failbit);
        return -1;
    }
    return 0;
}

// PRIVATE MANIPULATORS
template <typename TYPE>
int BerEncoder::encodeImpl(const TYPE&                value,
                           BerConstants::TagClass     tagClass,
                           int                        tagNumber,
                           int                        formattingMode,
                           bdlat_TypeCategory::Choice )
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    const BerConstants::TagType tagType = BerConstants::e_CONSTRUCTED;

    int rc = BerUtil::putIdentifierOctets(d_streamBuf,
                                          tagClass,
                                          tagType,
                                          tagNumber);
    if (rc | BerUtil::putIndefiniteLengthOctet(d_streamBuf)) {
        return k_FAILURE;                                             // RETURN
    }

    const bool isUntagged = formattingMode
                          & bdlat_FormattingMode::e_UNTAGGED;

    if (!isUntagged) {
        // According to X.694 (clause 20.4), an XML choice (not anonymous)
        // element is encoded as a sequence with 1 element.

        rc = BerUtil::putIdentifierOctets(d_streamBuf,
                                          BerConstants::e_CONTEXT_SPECIFIC,
                                          tagType,
                                          0);
        if (rc | BerUtil::putIndefiniteLengthOctet(d_streamBuf)) {
            return k_FAILURE;
        }
    }

    const int selectionId = bdlat_ChoiceFunctions::selectionId(value);

    if (bdlat_ChoiceFunctions::k_UNDEFINED_SELECTION_ID != selectionId) {

        BerEncoder_Visitor visitor(this);

        if (0 != bdlat_ChoiceFunctions::accessSelection(value, visitor)) {
            return k_FAILURE;                                         // RETURN
        }
    }

    if (!isUntagged) {
        // According to X.694 (clause 20.4), an XML choice (not anonymous)
        // element is encoded as a sequence with 1 element.

        // Don't waste time checking the result of this call -- the only thing
        // that can go wrong is eof, which will happen again when we call it
        // again below.
        BerUtil::putEndOfContentOctets(d_streamBuf);
    }

    return BerUtil::putEndOfContentOctets(d_streamBuf);
}

template <typename TYPE>
int BerEncoder::encodeImpl(const TYPE&                       value,
                           BerConstants::TagClass            tagClass,
                           int                               tagNumber,
                           int                               formattingMode,
                           bdlat_TypeCategory::NullableValue )
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    bool isNillable = formattingMode & bdlat_FormattingMode::e_NILLABLE;

    if (isNillable) {

        // nillable is encoded in BER as a sequence with one optional element

        int rc = BerUtil::putIdentifierOctets(d_streamBuf,
                                              tagClass,
                                              BerConstants::e_CONSTRUCTED,
                                              tagNumber);
        if (rc | BerUtil::putIndefiniteLengthOctet(d_streamBuf)) {
            return k_FAILURE;
        }

        if (!bdlat_NullableValueFunctions::isNull(value)) {

            BerEncoder_encodeProxy proxy1 = {
                                 this,
                                 BerConstants::e_CONTEXT_SPECIFIC, // tagClass
                                 0,                                // tagNumber
                                 formattingMode };

            if (0 != bdlat_NullableValueFunctions::accessValue(value,
                                                               proxy1)) {
                return k_FAILURE;
            }
        } // end of bdlat_NullableValueFunctions::isNull(...)

        return BerUtil::putEndOfContentOctets(d_streamBuf);
    } // end of isNillable

    if (!bdlat_NullableValueFunctions::isNull(value)) {

        BerEncoder_encodeProxy proxy2 = { this,
                                          tagClass,
                                          tagNumber,
                                          formattingMode };

        if (0 != bdlat_NullableValueFunctions::accessValue(value, proxy2)) {
                return k_FAILURE;
        }
    }

    return k_SUCCESS;
}

template <typename TYPE>
int BerEncoder::encodeImpl(const TYPE&                        value,
                           BerConstants::TagClass             tagClass,
                           int                                tagNumber,
                           int                                formattingMode,
                           bdlat_TypeCategory::CustomizedType )
{
    typedef typename
    bdlat_CustomizedTypeFunctions::BaseType<TYPE>::Type BaseType;

    typedef typename
    bdlat_TypeCategory::Select<BaseType>::Type          BaseTypeCategory;

    int rc = encodeImpl(
                      bdlat_CustomizedTypeFunctions::convertToBaseType(value),
                      tagClass,
                      tagNumber,
                      formattingMode,
                      BaseTypeCategory());

    return rc;
}

template <typename TYPE>
int BerEncoder::encodeImpl(const TYPE&                     value,
                           BerConstants::TagClass          tagClass,
                           int                             tagNumber,
                           int                             ,
                           bdlat_TypeCategory::Enumeration )
{
    int rc = BerUtil::putIdentifierOctets(d_streamBuf,
                                          tagClass,
                                          BerConstants::e_PRIMITIVE,
                                          tagNumber);

    int intValue;
    bdlat_EnumFunctions::toInt(&intValue, value);

    rc |= BerUtil::putValue(d_streamBuf, intValue);

    return rc;
}

template <typename TYPE>
int BerEncoder::encodeImpl(const TYPE&                  value,
                           BerConstants::TagClass       tagClass,
                           int                          tagNumber,
                           int                          ,
                           bdlat_TypeCategory::Sequence )
{
    BerEncoder_Visitor visitor(this);

    int rc = BerUtil::putIdentifierOctets(d_streamBuf,
                                          tagClass,
                                          BerConstants::e_CONSTRUCTED,
                                          tagNumber);
    rc |= BerUtil::putIndefiniteLengthOctet(d_streamBuf);
    if (rc) {
        return rc;
    }

    rc = bdlat_SequenceFunctions::accessAttributes(value, visitor);
    rc |= BerUtil::putEndOfContentOctets(d_streamBuf);

    return rc;
}

template <typename TYPE>
int BerEncoder::encodeImpl(const TYPE&                     value,
                                BerConstants::TagClass     tagClass,
                                int                        tagNumber,
                                int                        ,
                                bdlat_TypeCategory::Simple )
{
    int rc = BerUtil::putIdentifierOctets(d_streamBuf,
                                          tagClass,
                                          BerConstants::e_PRIMITIVE,
                                          tagNumber);
    rc |= BerUtil::putValue(d_streamBuf, value, d_options);

    return rc;
}

template <typename TYPE>
inline
int BerEncoder::encodeImpl(const TYPE&               value,
                           BerConstants::TagClass    tagClass,
                           int                       tagNumber,
                           int                       formattingMode,
                           bdlat_TypeCategory::Array )
{
    enum { k_SUCCESS = 0,  k_FAILURE = -1 };

    if (d_currentDepth <= 1 || tagClass == BerConstants::e_UNIVERSAL) {
        return k_FAILURE;
    }
    // Note: bsl::vector<char> is handled as a special case in the CPP file.
    return this->encodeArrayImpl(value,
                                 tagClass,
                                 tagNumber,
                                 formattingMode);
}

template <typename TYPE>
int
BerEncoder::encodeArrayImpl(const TYPE&             value,
                            BerConstants::TagClass  tagClass,
                            int                     tagNumber,
                            int                     formattingMode)
{
    enum { k_FAILURE = -1, k_SUCCESS = 0 };

    const int size = static_cast<int>(bdlat_ArrayFunctions::size(value));

    if (0 == size && d_options && !d_options->encodeEmptyArrays()) {
        return k_SUCCESS;                                             // RETURN
    }

    const BerConstants::TagType tagType = BerConstants::e_CONSTRUCTED;

    int rc = BerUtil::putIdentifierOctets(d_streamBuf,
                                          tagClass,
                                          tagType,
                                          tagNumber);
    rc |= BerUtil::putIndefiniteLengthOctet(d_streamBuf);
    if (rc) {
        return k_FAILURE;                                             // RETURN
    }

    BerEncoder_UniversalElementVisitor visitor(this, formattingMode);

    for (int i = 0; i < size; ++i) {
        if (0 != bdlat_ArrayFunctions::accessElement(value, visitor, i)) {

            this->logError(tagClass,
                           tagNumber,
                           0,  // bdlat_TypeName::name(value),
                           i);

            return k_FAILURE;                                         // RETURN
        }
    }

    return BerUtil::putEndOfContentOctets(d_streamBuf);
}

template <typename TYPE>
inline
int BerEncoder::encodeImpl(const TYPE&                          value,
                                BerConstants::TagClass          tagClass,
                                int                             tagNumber,
                                int                             formattingMode,
                                bdlat_TypeCategory::DynamicType )
{
    BerEncoder_encodeProxy proxy = { this,
                                     tagClass,
                                     tagNumber,
                                     formattingMode
                                   };

    return bdlat_TypeCategoryUtil::accessByCategory(value, proxy);
}

                      // --------------------------------
                      // private class BerEncoder_Visitor
                      // --------------------------------

// CREATORS
inline
BerEncoder_Visitor::BerEncoder_Visitor(BerEncoder *encoder)
: d_encoder(encoder)
, d_levelGuard(encoder)
{
}

inline
BerEncoder_Visitor::~BerEncoder_Visitor()
{
}

// MANIPULATORS
template <typename TYPE, typename INFO>
inline
int BerEncoder_Visitor::operator()(const TYPE& value, const INFO& info)
{
    typedef typename
    bdlat_TypeCategory::Select<TYPE>::Type TypeCategory;

    int rc = d_encoder->encodeImpl(value,
                                   BerConstants::e_CONTEXT_SPECIFIC,
                                   info.id(),
                                   info.formattingMode(),
                                   TypeCategory());

    if (rc) {
        d_encoder->logError(BerConstants::e_CONTEXT_SPECIFIC,
                            info.id(),
                            info.name());
    }

    return rc;
}

              // ------------------------------------------------
              // private class BerEncoder_UniversalElementVisitor
              // ------------------------------------------------

// CREATORS
inline
BerEncoder_UniversalElementVisitor::
BerEncoder_UniversalElementVisitor(BerEncoder *encoder,
                                   int         formattingMode)
: d_encoder(encoder)
, d_formattingMode(formattingMode)
, d_levelGuard(encoder)
{
}

inline
BerEncoder_UniversalElementVisitor::
~BerEncoder_UniversalElementVisitor()
{
}

// MANIPULATORS
template <typename TYPE>
int BerEncoder_UniversalElementVisitor::operator()(const TYPE& value)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    typedef typename
    bdlat_TypeCategory::Select<TYPE>::Type TypeCategory;

    BerUniversalTagNumber::Value tagNumber = BerUniversalTagNumber::select(
                                                         value,
                                                         d_formattingMode,
                                                         d_encoder->options());

    if (d_encoder->encodeImpl(value,
                              BerConstants::e_UNIVERSAL,
                              static_cast<int>(tagNumber),
                              d_formattingMode,
                              TypeCategory())) {
        d_encoder->logError(BerConstants::e_UNIVERSAL,
                            tagNumber,
                            0  // bdlat_TypeName::name(value)
                           );
        return k_FAILURE;
    }

    return k_SUCCESS;
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
