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
//@DESCRIPTION: This component provides utility functions for encoding a
// 'bdeat'-compliant object into JSON format.  In particular, the
// 'baejsn_Encoder' 'class' contains a parameterized 'encode' function that
// encodes a specified 'bdeat' object into a specified stream.  There are two
// overloaded versions of this function:
//..
//    o writes to an 'bsl::streambuf'
//    o writes to an 'bsl::ostream'
//..
//
///Encoding Format for Simple Type
///-------------------------------
// The following table describes how various Simple type is encoded.
//..
//  Simple Type          JSON Type  Notes
//  -----------          ---------  -----
//  char                 string     string with 1 character
//  unsigned char        number
//  int                  number
//  unsigned int         number
//  bsls::Types::Int64   number
//  bsls::Types::Uint64  number
//  float                number     number in scientific notation
//  double               number     number in scientific notation
//  char *               string
//  bsl::string          string
//  bdet_Date            string     ISO 8601 format
//  bdet_DateTz          string     ISO 8601 format
//  bdet_Time            string     ISO 8601 format
//  bdet_TimeTz          string     ISO 8601 format
//  bdet_DatetimeTz      string     ISO 8601 format
//  bdet_DatetimeTz      string     ISO 8601 format
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Encoding a 'bcem_Aggregate' Object into JSON
///-------------------------------------------------------
// Suppose we want to encode a 'bcem_Aggregate' object into JSON.
//
// First, we create a schema that we will use to configure a 'bcem_Aggregate':
//..
//  bcema_SharedPtr<bdem_Schema> schema(new bdem_Schema);
//
//  bdem_RecordDef *address = schema->createRecord("Address");
//  address->appendField(bdem_ElemType::BDEM_STRING, "street");
//  address->appendField(bdem_ElemType::BDEM_STRING, "city");
//  address->appendField(bdem_ElemType::BDEM_STRING, "state");
//
//  bdem_RecordDef *employee = schema->createRecord("Employee");
//  employee->appendField(bdem_ElemType::BDEM_STRING, "name");
//  employee->appendField(bdem_ElemType::BDEM_LIST, address, "homeAddress");
//  employee->appendField(bdem_ElemType::BDEM_INT, "age");
//..
// Then, we create a 'bcem_Aggregate' object using the schema and populate it
// with values:
//..
//  bcem_Aggregate bob(schema, "Employee");
//
//  bob["name"].setValue("Bob");
//  bob["homeAddress"]["street"].setValue("Some Street");
//  bob["homeAddress"]["city"].setValue("Some City");
//  bob["homeAddress"]["state"].setValue("Some State");
//  bob["age"].setValue(21);
//..
// Next, we create a 'baejsn_Encoder':
//..
//  baejsn_Encoder encoder;
//..
// Now, we encode the object.
//..
//  bsl::ostringstream oss;
//  encoder.encode(oss, bob);
//..
// Finally, we print the encoded string:
//..
//  cout << oss.str();
//..
// The output should look like the following:
//..
//  {"name":"Bob","homeAddress":{"street":"Some Street","city":"Some City",
//  "state":"Some State"},"age":21}
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEJSN_PRINTUTIL
#include <baejsn_printutil.h>
#endif

#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#include <bdeat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDEAT_CHOICEFUNCTIONS
#include <bdeat_choicefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_CUSTOMIZEDTYPEFUNCTIONS
#include <bdeat_customizedtypefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_ENUMFUNCTIONS
#include <bdeat_enumfunctions.h>
#endif

#ifndef INCLUDED_BDEAT_CUSTOMIZEDTYPEFUNCTIONS
#include <bdeat_customizedtypefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_SEQUENCEFUNCTIONS
#include <bdeat_sequencefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_TYPECATEGORY
#include <bdeat_typecategory.h>
#endif

#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#include <bdeat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_SSTREAM
#include <bsl_sstream.h>
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

                        // ====================
                        // class baejsn_Encoder
                        // ====================

class baejsn_Encoder {
    // FRIENDS
    friend struct baejsn_Encoder_EncodeImpl;

  private:
    // DATA
    bsl::ostringstream d_logStream;  // stream used for logging

  private:
    // PRIVATE MANIPULATORS
    bsl::ostream& logStream();
        // Return the stream for logging.  Note the if stream has not
        // been created yet, it will be created during this call.

  public:
    // CREATORS
    explicit baejsn_Encoder(bslma::Allocator *basicAllocator = 0);
        // Create a encoder object.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    //! ~baejsn_Encoder() = default;
        // Destroy this object.

    // MANIPULATORS
    template <typename TYPE>
    int encode(bsl::streambuf *streamBuf, const TYPE& value);
        // Encode the specified 'value' of parameterized 'TYPE' into the
        // specified 'streamBuf'.  Return 0 on success, and a non-zero value
        // otherwise.

    template <typename TYPE>
    int encode(bsl::ostream& stream, const TYPE& value);
        // Encode the specified 'value' of parameterized 'TYPE' into the
        // specified 'streamBuf'.  Return 0 on success, and a non-zero value
        // otherwise.  Note that 'stream' will be invalidated if the encoding
        // failed.

    // ACCESSORS
    bsl::string loggedMessages() const;
        // Return a string containing any error, warning, or trace messages
        // that were logged during the last call to the 'encode' method.  The
        // log is reset each time 'encode' is called.
};

                        // ===============================
                        // class baejsn_Encoder_EncodeImpl
                        // ===============================

class baejsn_Encoder_EncodeImpl {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.  This
    // class implements the the parameterized 'encode' functions that encode
    // 'bdeat' types in JSON format.

    // DATA
    baejsn_Encoder *d_encoder;       // encoder (held, not owned)
    bsl::ostream    d_outputStream;  // stream for output

    // FRIENDS
    friend struct baejsn_Encoder_DynamicTypeDispatcher;
    friend struct baejsn_Encoder_ElementVisitor;
    friend class baejsn_Encoder_SequenceVisitor;

  private:
    // PRIVATE MANIPULATORS
    bsl::ostream& logStream();
        // Return the stream for logging.  Note that if stream has not been
        // created yet, it will be created during this call.

    bsl::ostream& outputStream();
        // Return the stream for output.

    int encodeImp(const bsl::vector<char>& value, bdeat_TypeCategory::Array);
    template <typename TYPE>
    int encodeImp(const TYPE& value, bdeat_TypeCategory::Array);
    template <typename TYPE>
    int encodeImp(const TYPE& value, bdeat_TypeCategory::Choice);
    template <typename TYPE>
    int encodeImp(const TYPE& value, bdeat_TypeCategory::CustomizedType);
    template <typename TYPE>
    int encodeImp(const TYPE& value, bdeat_TypeCategory::DynamicType);
    template <typename TYPE>
    int encodeImp(const TYPE& value, bdeat_TypeCategory::Enumeration);
    template <typename TYPE>
    int encodeImp(const TYPE& value, bdeat_TypeCategory::NullableValue);
    template <typename TYPE>
    int encodeImp(const TYPE& value, bdeat_TypeCategory::Sequence);
    template <typename TYPE>
    int encodeImp(const TYPE& value, bdeat_TypeCategory::Simple);
        // Dispatch the encoding functions for the specified 'value' based on
        // the type of the unnamed "dispatch" argument.

  public:
    // CREATORS
    baejsn_Encoder_EncodeImpl(baejsn_Encoder *encoder,
                              bsl::streambuf *streambuf);
        // Create a 'baejsn_Encoder_EncodeImpl' object.

    // MANIPULATORS
    template <typename TYPE>
    int encode(const TYPE & value);
        // Encode the specified 'value' into JSON.
};

                 // ====================================
                 // struct baejsn_Encoder_ElementVisitor
                 // ====================================

struct baejsn_Encoder_ElementVisitor {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.  This
    // functor class encode an element in a 'bdeat' Array or Choice object.

    // DATA
    baejsn_Encoder_EncodeImpl *d_encoder;  // encoder (held, not owned)

    // CREATORS
    // Creators have been omitted to allow simple static initialization of
    // this struct.

    // MANIPULATORS
    template <typename TYPE>
    int operator()(const TYPE& value);
        // Encode the specified 'value'.

    template <typename TYPE, typename INFO>
    int operator()(const TYPE& value, const INFO &info);
        // Encode the specified 'value' described by the specified 'info'.
};

                 // ====================================
                 // class baejsn_Encoder_SequenceVisitor
                 // ====================================

class baejsn_Encoder_SequenceVisitor {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.  This
    // functor class encode an element in a 'bdeat' Sequence object.  It should
    // be passed as an argument to the
    // 'bdeat_SequenceFunctions::accessAttributes' function.

    // DATA
    baejsn_Encoder_EncodeImpl *d_encoder;          // encoder (held, not owned)
    bool                       d_firstElementFlag; // indicate if an element
                                                   // has been printed

  private:
    // PRIVATE CLASS METHODS
    template <class TYPE>
    static bool isAttributeNull(const TYPE&, bslmf::MetaInt<0>);
    template <class TYPE>
    static bool isAttributeNull(const TYPE& value, bslmf::MetaInt<1>);
    template <class TYPE>
    static bool isAttributeNull(const TYPE& value);
        // Return 'true' if the specified 'value' is a Nullable type and is
        // null, and 'false' otherwise.

  public:
    // CREATORS
    explicit baejsn_Encoder_SequenceVisitor(
                                           baejsn_Encoder_EncodeImpl *encoder);
        // Create a 'baejsn_Encoder_SequenceVisitor' object.

    // MANIPULATORS
    template <typename TYPE, typename INFO>
    int operator()(const TYPE& value, const INFO &info);
        // Encode the specified 'value' described by the specified 'info'.
};

                 // ===========================================
                 // struct baejsn_Encoder_DynamicTypeDispatcher
                 // ===========================================

struct baejsn_Encoder_DynamicTypeDispatcher {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.  This
    // class is used to dispatch the appropriate 'encodeImp' method for a
    // Dynamic type.

    // DATA
    baejsn_Encoder_EncodeImpl *d_encoder;  // encoder (held, not owned)

    // CREATORS
    // Creators have been omitted to allow simple static initialization of
    // this struct.

    // MANIPULATORS
    template <typename TYPE>
    int operator()(const TYPE&, bslmf_Nil);
    template <typename TYPE, typename ANY_CATEGORY>
    int operator()(const TYPE& value, ANY_CATEGORY category);
        // Encode the specified 'value' with the specified bdeat 'category'.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                            // --------------------
                            // class baejsn_Encoder
                            // --------------------

// PRIVATE MANIPULATORS
inline
bsl::ostream& baejsn_Encoder::logStream()
{
    return d_logStream;
}

// CREATORS
inline
baejsn_Encoder::baejsn_Encoder(bslma::Allocator *basicAllocator)
: d_logStream(basicAllocator)
{
}

// MANIPULATORS
template <typename TYPE>
int baejsn_Encoder::encode(bsl::streambuf *streamBuf, const TYPE& value)
{
    BSLS_ASSERT(streamBuf);

    bdeat_TypeCategory::Value category =
                                    bdeat_TypeCategoryFunctions::select(value);
    if (bdeat_TypeCategory::BDEAT_SEQUENCE_CATEGORY != category
     && bdeat_TypeCategory::BDEAT_CHOICE_CATEGORY != category
     && bdeat_TypeCategory::BDEAT_ARRAY_CATEGORY != category) {
        logStream()
                  << "Encoded object must be a Sequence, Choice or Array type."
                  << bsl::endl;
        return -1;                                                    // RETURN
    }

    d_logStream.clear();
    d_logStream.str("");

    baejsn_Encoder_EncodeImpl encoderImpl(this, streamBuf);
    return encoderImpl.encode(value);
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
bsl::string baejsn_Encoder::loggedMessages() const
{
    return d_logStream.str();
}

                        // -------------------------------
                        // class baejsn_Encoder_EncodeImpl
                        // -------------------------------

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

template <typename TYPE>
inline
int baejsn_Encoder_EncodeImpl::encodeImp(const TYPE& value,
                                         bdeat_TypeCategory::Sequence)
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
inline
int baejsn_Encoder_EncodeImpl::encodeImp(const TYPE& value,
                                         bdeat_TypeCategory::Choice)
{
    if (bdeat_ChoiceFunctions::BDEAT_UNDEFINED_SELECTION_ID !=
                                   bdeat_ChoiceFunctions::selectionId(value)) {
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
inline
int baejsn_Encoder_EncodeImpl::encodeImp(const TYPE& value,
                                         bdeat_TypeCategory::Enumeration)
{
    bsl::string valueString;
    bdeat_EnumFunctions::toString(&valueString, value);
    return encode(valueString);
}

template <typename TYPE>
inline
int baejsn_Encoder_EncodeImpl::encodeImp(const TYPE& value,
                                         bdeat_TypeCategory::CustomizedType)
{
    return encode(bdeat_CustomizedTypeFunctions::convertToBaseType(value));
}

template <typename TYPE>
inline
int baejsn_Encoder_EncodeImpl::encodeImp(const TYPE& value,
                                         bdeat_TypeCategory::DynamicType)
{
    baejsn_Encoder_DynamicTypeDispatcher proxy = { this };
    return bdeat_TypeCategoryUtil::accessByCategory(value, proxy);
}


template <typename TYPE>
inline
int baejsn_Encoder_EncodeImpl::encodeImp(const TYPE& value,
                                         bdeat_TypeCategory::Simple)
{
    return baejsn_PrintUtil::printValue(outputStream(), value);
}

template <typename TYPE>
int baejsn_Encoder_EncodeImpl::encodeImp(const TYPE& value,
                                         bdeat_TypeCategory::Array)
{
    outputStream() << '[';

    bsl::size_t size = bdeat_ArrayFunctions::size(value);

    if (0 < size) {
        baejsn_Encoder_ElementVisitor visitor = { this };

        int rc = bdeat_ArrayFunctions::accessElement(value, visitor, 0);
        if (0 != rc) {
            return rc;                                                // RETURN
        }

        for (bsl::size_t i = 1; i < size; ++i) {
            outputStream() << ',';
            rc = bdeat_ArrayFunctions::accessElement(value, visitor, i);
            if (0 != rc) {
                return rc;                                            // RETURN
            }
        }
    }

    outputStream() << ']';
    return 0;
}

template <typename TYPE>
inline
int baejsn_Encoder_EncodeImpl::encodeImp(const TYPE& value,
                                         bdeat_TypeCategory::NullableValue)
{
    if (bdeat_NullableValueFunctions::isNull(value)) {
        outputStream() << "null";
        return 0;                                                     // RETURN
    }

    baejsn_Encoder_ElementVisitor visitor = { this };
    return bdeat_NullableValueFunctions::accessValue(value, visitor);
}

template <typename TYPE>
inline
int baejsn_Encoder_EncodeImpl::encode(const TYPE& value)
{
    typedef typename
    bdeat_TypeCategory::Select<TYPE>::Type TypeCategory;
    return encodeImp(value, TypeCategory());
}

// CREATORS
inline
baejsn_Encoder_EncodeImpl::baejsn_Encoder_EncodeImpl(baejsn_Encoder *encoder,
                                                     bsl::streambuf *streambuf)
: d_encoder(encoder)
, d_outputStream(streambuf)
{
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
, d_firstElementFlag(true)
{
}

// MANIPULATORS
template <typename TYPE, typename INFO>
inline
int baejsn_Encoder_SequenceVisitor::operator()(const TYPE& value,
                                               const INFO& info)
{
    // Determine if 'value' is null and do not encode 'value' if it is.

    if (isAttributeNull(value)) {
        return 0;                                                     // RETURN
    }

    if (!d_firstElementFlag) {
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

    d_firstElementFlag = false;

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

template <typename TYPE, typename INFO>
inline
int baejsn_Encoder_ElementVisitor::operator()(const TYPE& value,
                                              const INFO& info)
{
    int rc = d_encoder->encode(info.name());
    if (0 != rc) {
        d_encoder->logStream()
            << "Unable to encode the name of the selection, '"
            << info.name()
            << "'."
            << bsl::endl;
        return rc;                                                    // RETURN
    }
    d_encoder->outputStream() << ':';
    rc = d_encoder->encode(value);
    if (0 != rc) {
        d_encoder->logStream()
            << "Unable to encode the value of the selection, '"
            << info.name()
            << "'."
            << bsl::endl;
        return rc;                                                    // RETURN
    }
    return 0;
}

                    // -------------------------------------------
                    // struct baejsn_Encoder_DynamicTypeDispatcher
                    // -------------------------------------------

// MANIPULATORS
template <typename TYPE>
inline
int baejsn_Encoder_DynamicTypeDispatcher::operator()(const TYPE&, bslmf_Nil)
{
    BSLS_ASSERT_OPT(!"Should be unreachable!");

    return -1;
}

template <typename TYPE, typename ANY_CATEGORY>
inline
int baejsn_Encoder_DynamicTypeDispatcher::operator()(const TYPE&  value,
                                                     ANY_CATEGORY category)
{
    d_encoder->encodeImp(value, category);
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
