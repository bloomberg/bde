// baejsn_encoder.h                                                   -*-C++-*-
#ifndef INCLUDED_BAEJSN_ENCODER
#define INCLUDED_BAEJSN_ENCODER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a JSON encoder class.
//
//@CLASSES:
// baejsn_Encoder: JSON encoder utility class
//
//@SEE_ALSO: baejsn_decoder
//
//@AUTHOR: Raymond Chiu (schiu49)
//
//@DESCRIPTION: This component provides utility functions for
// decoding a JSON string.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BAEJSN_PARSERUTIL
#include <baejsn_parserutil.h>
#endif

#ifndef INCLUDED_BSL_IOMANIP
#include <bsl_iomanip.h>
#endif

#ifndef INCLUDED_BSL_SSTREAM
#include <bsl_sstream.h>
#endif

#ifndef INCLUDED_BSL_LIMITS
#include <bsl_limits.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#include <bdeat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDEAT_SEQUENCEFUNCTIONS
#include <bdeat_sequencefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_CHOICEFUNCTIONS
#include <bdeat_choicefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_ENUMFUNCTIONS
#include <bdeat_enumfunctions.h>
#endif

#ifndef INCLUDED_BDEAT_TYPECATEGORY
#include <bdeat_typecategory.h>
#endif

#ifndef INCLUDED_BDEAT_FORMATTINGMODE
#include <bdeat_formattingmode.h>
#endif

#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#include <bdeat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BSL_SSTREAM
#include <bsl_sstream.h>
#endif

#ifndef INCLUDED_BDESB_MEMOUTSTREAMBUF
#include <bdesb_memoutstreambuf.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

                        // ====================
                        // class baejsn_Encoder
                        // ====================

class baejsn_Encoder {
    // FRIENDS
    friend struct baejsn_Encoder_EncodeImpl;

  private:
    // PRIVATE TYPES
    class MemOutStream : public bsl::ostream
    {
        // This class provides stream for logging using 'bdesb_MemOutStreamBuf'
        // as a streambuf.  The logging stream is created on demand, i.e.,
        // during the first attempt to log message.

        // DATA
        bdesb_MemOutStreamBuf d_sb;

      private:
        // NOT IMPLEMENTED
        MemOutStream(const MemOutStream&);
        MemOutStream& operator=(const MemOutStream&);

      public:
        // CREATORS
        explicit MemOutStream(bslma_Allocator *basicAllocator = 0);
            // Create a new stream using the specified 'basicAllocator'.

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
            // Return a pointer to the memory containing the formatted values
            // formatted to this stream.  The data is not null-terminated
            // unless a null character was appended onto this stream.

        int length() const;
            // Return the length of of the formatted data, including null
            // characters appended to the stream, if any.
    };

  private:
    // DATA
    bslma_Allocator                 *d_allocator;    // held, not owned

    bsls_ObjectBuffer<MemOutStream>  d_logArea;      // placeholder for
                                                     // MemOutStream

    MemOutStream                    *d_logStream;    // if not zero, log stream
                                                     // was created at the
                                                     // moment of first logging
                                                     // and must be destroyed

  private:
    // PRIVATE MANIPULATORS
    bsl::ostream& logStream();
        // Return the stream for logging.  Note the if stream has not
        // been created yet, it will be created during this call.


    template <class TYPE>
    int encodeObject(bsl::streambuf *streamBuf, const TYPE& value);
        // Verify the specified 'value' of (template parameter) type 'TYPE' is
        // a Sequence or Choice and encode 'value' into the specified
        // 'streamBuf'.  Return 0 on success, and a non-zero value otherwise.

  public:
    // CREATORS
    explicit baejsn_Encoder(bslma::Allocator *basicAllocator = 0);
        // Create a encoder object.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~baejsn_Encoder();
        // Destroy this object.

    // MANIPULATORS
    template <typename TYPE>
    int encode(bsl::streambuf *streamBuf, const TYPE& value);
        // Encode the specifed 'value' of parameterized 'TYPE' into the
        // specified 'streamBuf'.  Return 0 on success, and a non-zero value
        // otherwise.

    template <typename TYPE>
    int encode(bsl::ostream& stream, const TYPE& value);
        // Encode the specifed 'value' of parameterized 'TYPE' into the
        // specified 'streamBuf'.  Return 0 on success, and a non-zero value
        // otherwise.  Note that 'stream' will be invalidated if the encoding
        // failed.

    // ACCESSORS
    bslstl::StringRef loggedMessages() const;
        // Return a string containing any error, warning, or trace messages
        // that were logged during the last call to the 'encode' method.  The
        // log is reset each time 'encode' is called.
};

                        // ===============================
                        // class baejsn_Encoder_EncodeImpl
                        // ===============================

class baejsn_Encoder_EncodeImpl {
    // DATA
    baejsn_Encoder *d_encoder;
    bsl::ostream    d_outputStream;

    // FRIENDS
    friend struct baejsn_Encoder_DynamicTypeChooser;
    friend struct baejsn_Encoder_ElementVisitor;
    friend struct baejsn_Encoder_SequenceVisitor;

  private:
    // PRIVATE MANIPULATORS
    bsl::ostream& logStream();
        // Return the stream for logging.  Note the if stream has not
        // been created yet, it will be created during this call.

    bsl::ostream& outputStream();
        // Return the stream for output.

  public:
    // CREATORS
    baejsn_Encoder_EncodeImpl(baejsn_Encoder *encoder,
                              bsl::streambuf *streambuf);
        // Create a 'baejsn_Encoder_EncodeImpl' object.

    // MANIPULATORS
    int encodeSimple(const bool value);
    int encodeSimple(const char value);
    int encodeSimple(short  value);
    int encodeSimple(int  value);
    int encodeSimple(bsls::Types::Int64  value);

    int encodeSimple(unsigned char value);
    int encodeSimple(unsigned short value);
    int encodeSimple(unsigned int value);
    int encodeSimple(bsls::Types::Uint64 value);

    int encodeSimple(float  value);
    int encodeSimple(double value);

    int encodeSimple(const bsl::string & value);
    int encodeSimple(const char *value);

    int encodeSimple(const bdet_Time& value);
    int encodeSimple(const bdet_Date& value);
    int encodeSimple(const bdet_Datetime& value);
    int encodeSimple(const bdet_TimeTz& value);
    int encodeSimple(const bdet_DateTz& value);
    int encodeSimple(const bdet_DatetimeTz& value);


    template <class TYPE>
    int encodeIso8601(const TYPE& value);

    template <typename TYPE>
    int encodeFloat(TYPE value);

    int encodeArray(const bsl::vector<char>& value);

    template <typename TYPE>
    int encodeArray(const TYPE& value);

    template <typename TYPE>
    int encodeSequence(const TYPE & value);

    template <typename TYPE>
    int encodeChoice(const TYPE & value);

    template <typename TYPE>
    int encodeEnumeration(const TYPE & value);

    template <typename TYPE>
    int encodeCustomized(const TYPE & value);

    template <typename TYPE>
    int encodeNullable(const TYPE & value);

    template <typename TYPE>
    int encodeChooser(const TYPE & value, bdeat_TypeCategory::Sequence);

    template <typename TYPE>
    int encodeChooser(const TYPE & value, bdeat_TypeCategory::Choice);

    template <typename TYPE>
    int encodeChooser(const TYPE & value, bdeat_TypeCategory::Enumeration);

    template <typename TYPE>
    int encodeChooser(const TYPE & value, bdeat_TypeCategory::CustomizedType);

    template <typename TYPE>
    int encodeChooser(const TYPE &value, bdeat_TypeCategory::DynamicType);

    template <typename TYPE>
    int encodeChooser(const TYPE &value, bdeat_TypeCategory::Simple);

    template <typename TYPE>
    int encodeChooser(const TYPE& value, bdeat_TypeCategory::Array);

    template <typename TYPE>
    int encodeChooser(const TYPE& value, bdeat_TypeCategory::NullableValue);

    template <typename TYPE>
    int encode(const TYPE & value);
};

                 // ====================================
                 // struct baejsn_Encoder_ElementVisitor
                 // ====================================

struct baejsn_Encoder_ElementVisitor {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.

    // DATA
    baejsn_Encoder_EncodeImpl *d_encoder;

    // CREATORS
    // Creators have been omitted to allow simple static initialization of
    // this struct.

    // MANIPULATORS
    template <typename TYPE>
    int operator()(const TYPE& value);

    template <typename TYPE, typename ATTRIBUTE_OR_SELECTION>
    int operator()(const TYPE& value, const ATTRIBUTE_OR_SELECTION &info);
};

                 // ====================================
                 // class baejsn_Encoder_SequenceVisitor
                 // ====================================

class baejsn_Encoder_SequenceVisitor {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.

    // DATA
    baejsn_Encoder_EncodeImpl *d_encoder;
    bool                       d_firstPassFlag;

  private:
    // PRIVATE CLASS METHODS
    template <class TYPE>
    static bool isAttributeNull(const TYPE&, bslmf::MetaInt<0>);

    template <class TYPE>
    static bool isAttributeNull(const TYPE& value, bslmf::MetaInt<1>);

    template <class TYPE>
    static bool isAttributeNull(const TYPE& value);

  public:
    // CREATORS
    explicit baejsn_Encoder_SequenceVisitor(
                                           baejsn_Encoder_EncodeImpl *encoder);

    // MANIPULATORS
    template <typename TYPE, typename ATTRIBUTE_OR_SELECTION>
    int operator()(const TYPE& value, const ATTRIBUTE_OR_SELECTION &info);
};

                 // =========================================
                 // struct baejsn_Encoder_DynamicTypeChooser
                 // =========================================

struct baejsn_Encoder_DynamicTypeChooser {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.

    // DATA
    baejsn_Encoder_EncodeImpl *d_encoder;

    // CREATORS
    // Creators have been omitted to allow simple static initialization of
    // this struct.

    // MANIPULATORS
    template <typename TYPE>
    int operator()(const TYPE&, bslmf_Nil);

    template <typename TYPE, typename ANY_CATEGORY>
    int operator()(const TYPE& object, ANY_CATEGORY category);
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                   // ----------------------------------
                   // class baejsn_Encoder::MemOutStream
                   // ----------------------------------

inline
baejsn_Encoder::MemOutStream::MemOutStream(bslma_Allocator *basicAllocator)
: bsl::ostream(0)
, d_sb(bslma_Default::allocator(basicAllocator))
{
    rdbuf(&d_sb);
}

// MANIPULATORS
inline
void baejsn_Encoder::MemOutStream::reset()
{
    d_sb.reset();
}

// ACCESSORS
inline
const char* baejsn_Encoder::MemOutStream::data() const
{
    return d_sb.data();
}

inline
int baejsn_Encoder::MemOutStream::length() const
{
    return (int)d_sb.length();
}

                            // --------------------
                            // class baejsn_Encoder
                            // --------------------

// PRIVATE MANIPULATORS
template <typename TYPE>
int baejsn_Encoder::encodeObject(bsl::streambuf *streamBuf, const TYPE& value)
{
    bdeat_TypeCategory::Value category =
                                    bdeat_TypeCategoryFunctions::select(value);
    if (bdeat_TypeCategory::BDEAT_SEQUENCE_CATEGORY != category
     && bdeat_TypeCategory::BDEAT_CHOICE_CATEGORY != category) {
        logStream() << "Encoded object must be a Sequence or Choice type"
                    << bsl::endl;
        return -1;                                                    // RETURN
    }
    baejsn_Encoder_EncodeImpl encoder(this, streamBuf);
    return encoder.encode(value);
}

// MANIPULATORS
template <typename TYPE>
int baejsn_Encoder::encode(bsl::streambuf *streamBuf, const TYPE& value)
{
    BSLS_ASSERT(streamBuf);

    if (d_logStream != 0) {
        d_logStream->reset();
    }

    return encodeObject(streamBuf, value);
}

template <typename TYPE>
int baejsn_Encoder::encode(bsl::ostream& stream, const TYPE& value)
{
    if (!stream.good()) {
        logStream() << "Invalid stream." << bsl::endl;
        return -1;                                                    // RETURN
    }

    int rc = this->encode(stream.rdbuf(), value);
    if (0 != rc) {
        stream.setstate(bsl::ios_base::failbit);
        return rc;                                                    // RETURN
    }

    return 0;
}

// ACCESSORS
inline
bslstl::StringRef baejsn_Encoder::loggedMessages() const
{
    if (d_logStream) {
        return bslstl::StringRef(d_logStream->data(), d_logStream->length());
                                                                      // RETURN
    }
    return bslstl::StringRef();
}

                            // -------------------------------
                            // class baejsn_Encoder_EncodeImpl
                            // -------------------------------

inline
baejsn_Encoder_EncodeImpl::baejsn_Encoder_EncodeImpl(baejsn_Encoder *encoder,
                                                     bsl::streambuf *streambuf)
: d_encoder(encoder)
, d_outputStream(streambuf)
{
}

// PRIVATE MANIPULATORS
inline
bsl::ostream& baejsn_Encoder_EncodeImpl::logStream()
{
    return d_encoder->logStream();
}

inline
bsl::ostream& baejsn_Encoder_EncodeImpl::outputStream()
{
    return d_outputStream;
}

// MANIPULATORS
inline
int baejsn_Encoder_EncodeImpl::encodeSimple(const bool value)
{
    outputStream() << (value ? "true" : "false");
    return 0;
}

inline
int baejsn_Encoder_EncodeImpl::encodeSimple(short  value)
{
    outputStream() << value;
    return 0;
}

inline
int baejsn_Encoder_EncodeImpl::encodeSimple(int  value)
{
    outputStream() << value;
    return 0;
}

inline
int baejsn_Encoder_EncodeImpl::encodeSimple(bsls::Types::Int64  value)
{
    outputStream() << value;
    return 0;
}

inline
int baejsn_Encoder_EncodeImpl::encodeSimple(unsigned char value)
{
    outputStream() << static_cast<int>(value);
    return 0;
}

inline
int baejsn_Encoder_EncodeImpl::encodeSimple(unsigned short value)
{
    outputStream() << value;
    return 0;
}

inline
int baejsn_Encoder_EncodeImpl::encodeSimple(unsigned int value)
{
    outputStream() << value;
    return 0;
}

inline
int baejsn_Encoder_EncodeImpl::encodeSimple(bsls::Types::Uint64 value)
{
    outputStream() << value;
    return 0;
}

inline
int baejsn_Encoder_EncodeImpl::encodeSimple(float  value)
{
    encodeFloat(value);
    return 0;
}

inline
int baejsn_Encoder_EncodeImpl::encodeSimple(double value)
{
    encodeFloat(value);
    return 0;
}


inline
int baejsn_Encoder_EncodeImpl::encodeSimple(const char * value)
{
    return encodeSimple(bsl::string(value));
}

inline
int baejsn_Encoder_EncodeImpl::encodeSimple(const bdet_Time& value)
{
    return encodeIso8601(value);
}

inline
int baejsn_Encoder_EncodeImpl::encodeSimple(const bdet_Date& value)
{
    return encodeIso8601(value);
}

inline
int baejsn_Encoder_EncodeImpl::encodeSimple(const bdet_Datetime& value)
{
    return encodeIso8601(value);
}

inline
int baejsn_Encoder_EncodeImpl::encodeSimple(const bdet_TimeTz& value)
{
    return encodeIso8601(value);
}

inline
int baejsn_Encoder_EncodeImpl::encodeSimple(const bdet_DateTz& value)
{
    return encodeIso8601(value);
}

inline
int baejsn_Encoder_EncodeImpl::encodeSimple(const bdet_DatetimeTz& value)
{
    return encodeIso8601(value);
}

template <typename TYPE>
int baejsn_Encoder_EncodeImpl::encodeArray(const TYPE& value)
{
    outputStream() << '[';

    bsl::size_t size = bdeat_ArrayFunctions::size(value);

    for (bsl::size_t i = 0; i < size; ++i)
    {
        if (0 != i) {
            outputStream() << ',';
        }

        baejsn_Encoder_ElementVisitor visitor = { this };
        int rc = bdeat_ArrayFunctions::accessElement(value, visitor, i);
        if (0 != rc) {
            return rc;                                                // RETURN
        }
    }

    outputStream() << ']';
    return 0;
}

template <typename TYPE>
int baejsn_Encoder_EncodeImpl::encodeSequence(const TYPE & value)
{
    outputStream() << '{';

    baejsn_Encoder_SequenceVisitor visitor(this);
    int rc = bdeat_SequenceFunctions::accessAttributes(value, visitor);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    outputStream() << '}';
    return 0;
}

template <typename TYPE>
int baejsn_Encoder_EncodeImpl::encodeChoice(const TYPE & value)
{
    if (bdeat_ChoiceFunctions::BDEAT_UNDEFINED_SELECTION_ID !=
                                     bdeat_ChoiceFunctions::selectionId(value))
    {
        outputStream() << '{';

        baejsn_Encoder_ElementVisitor visitor = { this };
        if (0 != bdeat_ChoiceFunctions::accessSelection(value, visitor)) {
            return -1;                                                // RETURN
        }

        outputStream() << '}';
    }
    else {
        logStream() << "Undefined selection for Choice object" << bsl::endl;
        return -1;                                                    // RETURN
    }
    return 0;
}

template <typename TYPE>
int baejsn_Encoder_EncodeImpl::encodeNullable(const TYPE & value)
{
    if (bdeat_NullableValueFunctions::isNull(value)) {
        outputStream() << "null";
        return 0;                                                     // RETURN
    }

    baejsn_Encoder_ElementVisitor visitor = { this };
    return bdeat_NullableValueFunctions::accessValue(value, visitor);
}

template <class TYPE>
inline
int baejsn_Encoder_EncodeImpl::encodeIso8601(const TYPE& value)
{
    char buffer[bdepu_Iso8601::BDEPU_MAX_DATETIME_STRLEN + 1];
    bdepu_Iso8601::generate(buffer, value, sizeof buffer);
    return encodeSimple(buffer);
}

template <typename TYPE>
inline
int baejsn_Encoder_EncodeImpl::encodeFloat(TYPE value)
{
    bsl::streamsize         prec  = outputStream().precision();
    bsl::ios_base::fmtflags flags = outputStream().flags();

    outputStream().precision(bsl::numeric_limits<TYPE>::digits10);
    outputStream().setf(bsl::ios::scientific, bsl::ios::floatfield);

    outputStream() << value;

    outputStream().precision(prec);
    outputStream().flags(flags);
    return 0;
}

template <typename TYPE>
inline
int baejsn_Encoder_EncodeImpl::encodeEnumeration(const TYPE & value)
{
    bsl::string valueString;
    bdeat_EnumFunctions::toString(&valueString, value);
    return encode(valueString);
}

template <typename TYPE>
inline
int baejsn_Encoder_EncodeImpl::encodeCustomized(const TYPE & value)
{
    return encode(bdeat_CustomizedTypeFunctions::convertToBaseType(value));
}

template <typename TYPE>
inline
int baejsn_Encoder_EncodeImpl::encodeChooser(const TYPE& value,
                                  bdeat_TypeCategory::Sequence)
{
    return encodeSequence(value);
}

template <typename TYPE>
inline
int baejsn_Encoder_EncodeImpl::encodeChooser(const TYPE& value,
                                  bdeat_TypeCategory::Choice)
{
    return encodeChoice(value);
}

template <typename TYPE>
inline
int baejsn_Encoder_EncodeImpl::encodeChooser(const TYPE& value,
                                  bdeat_TypeCategory::Enumeration)
{
    return encodeEnumeration(value);
}

template <typename TYPE>
inline
int baejsn_Encoder_EncodeImpl::encodeChooser(const TYPE& value,
                                  bdeat_TypeCategory::CustomizedType)
{
    return encodeCustomized(value);
}

template <typename TYPE>
inline
int baejsn_Encoder_EncodeImpl::encodeChooser(const TYPE& value,
                                  bdeat_TypeCategory::DynamicType)
{
    baejsn_Encoder_DynamicTypeChooser proxy = { this };
    return bdeat_TypeCategoryUtil::accessByCategory(value, proxy);
}


template <typename TYPE>
inline
int baejsn_Encoder_EncodeImpl::encodeChooser(const TYPE& value,
                                  bdeat_TypeCategory::Simple)
{
    return encodeSimple(value);
}

template <typename TYPE>
inline
int baejsn_Encoder_EncodeImpl::encodeChooser(const TYPE& value,
                                  bdeat_TypeCategory::Array)
{
    return encodeArray(value);
}

template <typename TYPE>
inline
int baejsn_Encoder_EncodeImpl::encodeChooser(const TYPE& value,
                                  bdeat_TypeCategory::NullableValue)
{
    return encodeNullable(value);
}

template <typename TYPE>
inline
int baejsn_Encoder_EncodeImpl::encode(const TYPE& value)
{
    typedef typename
    bdeat_TypeCategory::Select<TYPE>::Type TypeCategory;
    return encodeChooser(value, TypeCategory());
}

                    // -------------------------------------
                    // struct baejsn_Encoder_SequenceVisitor
                    // -------------------------------------

// PRIVATE CLASS METHODS
template <class TYPE>
inline
bool baejsn_Encoder_SequenceVisitor::isAttributeNull(const TYPE&,
                                                     bslmf::MetaInt<0>)
{
    return false;
}

template <class TYPE>
inline
bool baejsn_Encoder_SequenceVisitor::isAttributeNull(const TYPE& value,
                                                     bslmf::MetaInt<1>)
{
    if (bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY ==
                              bdeat_TypeCategoryFunctions::select(value)) {
        return bdeat_NullableValueFunctions::isNull(value);           // RETURN
    }
    return false;
}

template <class TYPE>
inline
bool baejsn_Encoder_SequenceVisitor::isAttributeNull(const TYPE& value)
{
    return isAttributeNull(value,
                           bslmf::MetaInt<bdeat_NullableValueFunctions
                                        ::IsNullableValue<TYPE>::VALUE>());
}

// CREATORS
inline
baejsn_Encoder_SequenceVisitor::baejsn_Encoder_SequenceVisitor(
                                            baejsn_Encoder_EncodeImpl *encoder)
: d_encoder(encoder)
, d_firstPassFlag(true)
{
}

// MANIPULATORS
template <typename TYPE, typename ATTRIBUTE_OR_SELECTION>
inline
int baejsn_Encoder_SequenceVisitor::operator()(
                                           const TYPE&                   value,
                                           const ATTRIBUTE_OR_SELECTION& info)
{
    // Determine if 'value' is null and do not encode 'value' if it is.

    if (isAttributeNull(value)) {
        return 0;                                                     // RETURN
    }

    if (!d_firstPassFlag) {
        d_encoder->outputStream() << ',';
    }
    int rc = d_encoder->encode(info.name());
    if (0 != rc) {
        d_encoder->logStream()
            << "Unable to encode the name of the attribute '"
            << info.name()
            << "'."
            << bsl::endl;
        return rc;                                                    // RETURN
    }
    d_encoder->outputStream() << ':';
    rc = d_encoder->encode(value);
    if (0 != rc) {
        d_encoder->logStream()
            << "Unable to encode the value of the attribute '"
            << info.name()
            << "'."
            << bsl::endl;
        return rc;                                                    // RETURN
    }

    d_firstPassFlag = false;

    return 0;
}

                    // ------------------------------------
                    // struct baejsn_Encoder_ElementVisitor
                    // ------------------------------------

template <typename TYPE>
inline
int baejsn_Encoder_ElementVisitor::operator()(const TYPE &value)
{
    return d_encoder->encode(value);
}

template <typename TYPE, typename ATTRIBUTE_OR_SELECTION>
inline
int baejsn_Encoder_ElementVisitor::operator()(
                                            const TYPE&                   value,
                                            const ATTRIBUTE_OR_SELECTION& info)
{
    int rc = d_encoder->encode(info.name());
    if (0 != rc) {
        d_encoder->logStream()
            << "Unable to encode the name of the selection, '"
            << info.name()
            << "'."
            << bsl::endl;
        return rc;
    }
    d_encoder->outputStream() << ':';
    rc = d_encoder->encode(value);
    if (0 != rc) {
        d_encoder->logStream()
            << "Unable to encode the value of the selection, '"
            << info.name()
            << "'."
            << bsl::endl;
        return rc;
    }
    return 0;
}

                    // -----------------------------------------
                    // struct baejsn_Encoder_DynamicTypeChooser
                    // -----------------------------------------

// MANIPULATORS
template <typename TYPE>
inline
int baejsn_Encoder_DynamicTypeChooser::operator()(const TYPE&, bslmf_Nil)
{
    BSLS_ASSERT_OPT(!"Should be unreachable!");

    return -1;
}

template <typename TYPE, typename ANY_CATEGORY>
inline
int baejsn_Encoder_DynamicTypeChooser::operator()(const TYPE&  object,
                                                   ANY_CATEGORY category)
{
    d_encoder->encodeChooser(object, category);
    return 0;
}


}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
