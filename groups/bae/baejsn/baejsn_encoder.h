// baejsn_encoder.h                                                   -*-C++-*-
#ifndef INCLUDED_BAEJSN_ENCODER
#define INCLUDED_BAEJSN_ENCODER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a JSON encoder class
//
//@CLASSES:
//
//@SEE_ALSO:
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

#include <stdint.h>

#include <iomanip>
#include <sstream>
#include <limits>
#include <set>
#include <vector>

#include <bslmf_if.h>
#include <bdeut_nullablevalue.h>
#include <bdeat_attributeinfo.h>
#include <bdeat_sequencefunctions.h>
#include <bdeat_choicefunctions.h>
#include <bdeat_enumfunctions.h>
#include <bdeat_typecategory.h>
#include <bdeat_formattingmode.h>
#include <bdeat_valuetypefunctions.h>
#include <bsl_sstream.h>

#ifndef INCLUDED_BDEDE_BASE64ENCODER
#include <bdede_base64encoder.h>
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

#include <iostream>  // TBD: Remove later

namespace BloombergLP {

class baejsn_Encoder {
    // DATA
    //bslma_Allocator                 *d_allocator;    // held, not owned

    bsls_ObjectBuffer<bsl::ostringstream>  d_logArea;      // placeholder for
                                                     // MemOutStream

    bsl::ostringstream              *d_logStream;    // if not zero,
                                                     // log stream was
                                                     // created at the
                                                     // moment of first
                                                     // logging and must
                                                     // be destroyed

    bsl::ostream                  d_outputStream;    // held, not owned

    // FRIEND
    friend struct baejsn_Encoder_EncodeImpProxy;
    friend struct baejsn_Encoder_ElementVisitor;
    friend struct baejsn_Encoder_SequenceVisitor;

  private:
    // PRIVATE MANIPULATORS
    bsl::ostream& logStream();
        // Return the stream for logging.  Note the if stream has not
        // been created yet, it will be created during this call.

  public:
    void encodeSimple(short  value) { d_outputStream << value; }
    void encodeSimple(int  value) { d_outputStream << value; }
    void encodeSimple(bsls::Types::Int64  value) { d_outputStream << value; }

    void encodeSimple(unsigned char value) { d_outputStream << static_cast<int>(value); }
    void encodeSimple(unsigned short value) { d_outputStream << value; }
    void encodeSimple(unsigned int value) { d_outputStream << value; }
    void encodeSimple(bsls::Types::Uint64 value) { d_outputStream << value; }

    void encodeSimple(float  value) { encodeFloat(value); }
    void encodeSimple(double value) { encodeFloat(value); }

    void encodeSimple(const bdet_Time& value) { encodeIso8601(value); }
    void encodeSimple(const bdet_Date& value) { encodeIso8601(value); }
    void encodeSimple(const bdet_Datetime& value) { encodeIso8601(value); }
    void encodeSimple(const bdet_TimeTz& value) { encodeIso8601(value); }
    void encodeSimple(const bdet_DateTz& value) { encodeIso8601(value); }
    void encodeSimple(const bdet_DatetimeTz& value) { encodeIso8601(value); }

    template <class TYPE>
    int encodeIso8601(const TYPE& value) {
        char buffer[bdepu_Iso8601::BDEPU_MAX_DATETIME_STRLEN + 1];
        bdepu_Iso8601::generate(buffer, value, sizeof buffer);
        encodeSimple(buffer);
        return 0;
    }

    template <typename TYPE>
    void encodeFloat(TYPE value)
    {
        std::streamsize         prec  = d_outputStream.precision();
        std::ios_base::fmtflags flags = d_outputStream.flags();

        d_outputStream.precision(std::numeric_limits<TYPE>::digits10);
        d_outputStream.setf(std::ios::scientific, std::ios::floatfield);

        d_outputStream << value;

        d_outputStream.precision(prec);
        d_outputStream.flags(flags);
    }

    void encodeSimple(const bool value) { d_outputStream << (value ? "true" : "false"); }

    void encodeSimple(const char value);
    void encodeSimple(const bsl::string & value);
    void encodeSimple(const char * value) { encode(std::string(value)); }

    void encodeArray(const bsl::vector<char>& value)
    {
        bsl::string base64String;
        bdede_Base64Encoder encoder(0);
        base64String.resize(bdede_Base64Encoder::encodedLength(value.size(), 0));
        int numOut;
        int numIn;
        int rc = encoder.convert(base64String.begin(), &numOut, &numIn, value.begin(), value.end());
        rc = encoder.endConvert(base64String.begin() + numOut);

        //base64String.resize(numOut);

        encodeSimple(base64String);
        //d_outputStream << '[';

        //for (typename std::vector<TYPE>::const_iterator it = value.begin(); it != value.end(); ++it)
        //{
        //    if (it != value.begin())
        //        d_outputStream << ',';

        //    encodeHex(*it);
        //}

        //d_outputStream << ']';
    }

    template <typename TYPE>
    void encodeArray(const TYPE& value);

    //void encodeHex(const std::vector<char> & value);

    //template <typename TYPE>
    //void encodeHex(const std::vector<TYPE> & value)
    //{
    //    d_outputStream << '[';

    //    for (typename std::vector<TYPE>::const_iterator it = value.begin(); it != value.end(); ++it)
    //    {
    //        if (it != value.begin())
    //            d_outputStream << ',';

    //        encodeHex(*it);
    //    }

    //    d_outputStream << ']';
    //}

    //void encode(const bcem_Aggregate & value);

    template <typename TYPE>
    void encode(const TYPE & value)
    {
        // TBD: A JSON string must contain an object.  i.e., the 'TYPE' *must*
        // be a Sequence or Choice on the first invocation of 'decode'.  This
        // 'decode' method is currently used to decode other JSON values.
        // Consequently, the decoder will allow a JSON string that contains
        // other JSON value instead of an object and should be fixed.

        typedef typename
        bdeat_TypeCategory::Select<TYPE>::Type TypeCategory;
        // TBD: return value
        //return encodeChooser(value, TypeCategory());
        encodeChooser(value, TypeCategory());
    }

    template <typename TYPE>
    void encodeSequence(const TYPE & value);

    template <typename TYPE>
    void encodeChoice(const TYPE & value);

    template <typename TYPE>
    void encodeEnumString(const TYPE & value)
    {
        std::string valueString;
        bdeat_EnumFunctions::toString(&valueString, value);
        encode(valueString);
    }

    template <typename TYPE>
    void encodeEnumCustomized(const TYPE & value)
    {
        encode(bdeat_CustomizedTypeFunctions::convertToBaseType(value));
    }

    template <typename TYPE>
    void encodeNullable(const TYPE & value);

  private:
    template <typename TYPE>
    void encodeChooser(const TYPE & value, bdeat_TypeCategory::Sequence)
    {
        // TBD: need to specialize for empty sequence.
        //typedef typename bslmf_If< TYPE::NUM_ATTRIBUTES != 0, IsSequence, IsSequenceEmpty >::Type TypeTag;
        //encodeChooserSequence(value, TypeTag());
        encodeSequence(value);
    }

    template <typename TYPE>
    void encodeChooser(const TYPE & value, bdeat_TypeCategory::Choice) { encodeChoice(value); }

    template <typename TYPE>
    void encodeChooser(const TYPE & value, bdeat_TypeCategory::Enumeration) { encodeEnumString(value); }

    template <typename TYPE>
    void encodeChooser(const TYPE & value, bdeat_TypeCategory::CustomizedType) { encodeEnumCustomized(value); }

    template <typename TYPE>
    void encodeChooser(const TYPE &value, bdeat_TypeCategory::DynamicType);

    template <typename TYPE>
    void encodeChooser(const TYPE &value, bdeat_TypeCategory::Simple) { encodeSimple(value); }

    template <typename TYPE>
    void encodeChooser(const TYPE& value, bdeat_TypeCategory::Array) { encodeArray(value); }

    template <typename TYPE>
    void encodeChooser(const TYPE& value, bdeat_TypeCategory::NullableValue) { encodeNullable(value);}

    //template <typename TYPE, typename TYPE_CATEGORY>
    //int encodeChooser(const TYPE *, const TYPE_CATEGORY&);

    struct HasEncodeValue
    {
        struct IsChoice { enum { VALUE = 1 }; };
        struct IsOther  { enum { VALUE = 1 }; };

        template <typename TYPE, typename ATTRIBUTE_OR_SELECTION>
        int operator() (const std::vector<TYPE> & value, const ATTRIBUTE_OR_SELECTION &)
        {
            return value.empty() ? 0 : 1;
        }

        template <typename TYPE, typename ATTRIBUTE_OR_SELECTION>
        int operator() (const bdeut_NullableValue<TYPE> & value, const ATTRIBUTE_OR_SELECTION &)
        {
            return value.isNull() ? 0 : 1;
        }

        template <typename TYPE, typename ATTRIBUTE_OR_SELECTION>
        int operator() (const TYPE & value, const ATTRIBUTE_OR_SELECTION &)
        {
            typedef typename bslmf_If< bdeat_ChoiceFunctions::IsChoice<TYPE>::VALUE, IsChoice, IsOther>::Type TypeTag;

            return hasValue(value, TypeTag()) ? 1 : 0;
        }

      private:
        template <typename TYPE>
        bool hasValue (const TYPE &value, IsChoice)
        {
            return value.selectionId() >= 0;
        }

        template <typename TYPE>
        bool hasValue (const TYPE &, IsOther)
        {
            return true;
        }
    };

  public:
    // CREATORS
    baejsn_Encoder();
        // Create a encoder object.

    template <typename TYPE>
    int encode(bsl::streambuf *streamBuf, const TYPE& variable);
        // Encode an object of parameterized 'TYPE' from the specified
        // 'streamBuf' and load the result into the specified modifiable
        // 'variable'.  Return 0 on success, and a non-zero value otherwise.

    template <typename TYPE>
    int encode(bsl::ostream& stream, const TYPE& variable);
        // Encode an object of parameterized 'TYPE' from the specified 'stream'
        // and load the result into the specified modifiable 'variable'.
        // Return 0 on success, and a non-zero value otherwise.  Note that
        // 'stream' will be invalidated if the decoding fails.

    bsl::string loggedMessages() const;
        // Return a string containing any error, warning, or trace messages
        // that were logged during the last call to the 'encode' method.  The
        // log is reset each time 'encode' is called.
};

                 // ====================================
                 // struct baejsn_Encoder_ElementVisitor
                 // ====================================

struct baejsn_Encoder_ElementVisitor {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.

    // DATA
    baejsn_Encoder *d_encoder;

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
                 // struct baejsn_Encoder_SequenceVisitor
                 // ====================================

struct baejsn_Encoder_SequenceVisitor {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.

    // DATA
    baejsn_Encoder *d_encoder;
    bool            d_firstPassFlag;

    // CREATORS
    baejsn_Encoder_SequenceVisitor(baejsn_Encoder *encoder);

    // MANIPULATORS
    template <typename TYPE, typename ATTRIBUTE_OR_SELECTION>
    int operator()(const TYPE& value, const ATTRIBUTE_OR_SELECTION &info);
};

                 // ====================================
                 // struct baejsn_Encoder_EncodeImpProxy
                 // ====================================

struct baejsn_Encoder_EncodeImpProxy {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.

    // DATA
    baejsn_Encoder *d_encoder;

    // CREATORS
    // Creators have been omitted to allow simple static initialization of
    // this struct.

    // MANIPULATORS
    template <typename TYPE>
    int operator()(const TYPE&, bslmf_Nil);

    template <typename TYPE, typename ANY_CATEGORY>
    int operator()(const TYPE& object, ANY_CATEGORY category);
};

// PRIVATE MANIPULATORS
template <typename TYPE>
inline
void baejsn_Encoder::encodeChooser(const TYPE& value, bdeat_TypeCategory::DynamicType)
{
    baejsn_Encoder_EncodeImpProxy proxy = { this };
    // TBD: return
    bdeat_TypeCategoryUtil::accessByCategory(value, proxy);
}

                            // --------------------
                            // class baejsn_Encoder
                            // --------------------

//template <typename TYPE>
//inline
//int baejsn_Encoder::encode(TYPE *value)
//{
//    // TBD: A JSON string must contain an object.  i.e., the 'TYPE' *must*
//    // be a Sequence or Choice on the first invocation of 'encode'.  This
//    // 'encode' method is currently used to encode other JSON values.
//    // Consequently, the decoder will allow a JSON string that contains
//    // other JSON value instead of an object and should be fixed.
//
//    typedef typename
//    bdeat_TypeCategory::Select<TYPE>::Type TypeCategory;
//    return decodeChooser(value, TypeCategory());
//}

// PRIVATE MANIPULATORS
template <typename TYPE>
void baejsn_Encoder::encodeArray(const TYPE& value)
{
    d_outputStream << '[';

    bsl::size_t size = bdeat_ArrayFunctions::size(value);

    for (bsl::size_t i = 0; i < size; ++i)
    {
        if (0 != i) {
            d_outputStream << ',';
        }

        baejsn_Encoder_ElementVisitor visitor = { this };
        if (0 != bdeat_ArrayFunctions::accessElement(value, visitor, i)) {
            return;
        }
    }

    d_outputStream << ']';
}

template <typename TYPE>
void baejsn_Encoder::encodeSequence(const TYPE & value)
{
    d_outputStream << '{';

    baejsn_Encoder_SequenceVisitor visitor(this);
    if (0 != bdeat_SequenceFunctions::accessAttributes(value, visitor)) {
        return;
    }
    //for (int i = 0, coded = 0; i < TYPE::NUM_ATTRIBUTES; ++i)
    //{
    //    const bdeat_AttributeInfo & attribute = TYPE::ATTRIBUTE_INFO_ARRAY[i];

    //    HasEncodeValue hasEncodeValueSelector;
    //    if (value.accessAttribute(hasEncodeValueSelector, attribute.id()) <= 0)
    //        continue;

    //    if (coded)
    //        d_outputStream << ',';

    //    encode(attribute.name());

    //    d_outputStream << ':';

    //    if (value.accessAttribute(*this, attribute.id()) < 0)
    //        d_outputStream << "null";

    //   ++coded;
    //}

    d_outputStream << '}';
}

template <typename TYPE>
void baejsn_Encoder::encodeChoice(const TYPE & value)
{
    //const bdeat_SelectionInfo * selection = value.lookupSelectionInfo(value.selectionId());

    if (bdeat_ChoiceFunctions::BDEAT_UNDEFINED_SELECTION_ID !=
                                     bdeat_ChoiceFunctions::selectionId(value))
    {
        d_outputStream << '{';

        //encode(selection->name());

        //d_outputStream << ':';

        baejsn_Encoder_ElementVisitor visitor = { this };
        if (0 != bdeat_ChoiceFunctions::accessSelection(value, visitor)) {
            return;
        }
        //if (value.accessSelection(*this) < 0)
        //    d_outputStream << "null";

        d_outputStream << '}';
    }
    else {
        d_outputStream << "null";
    }
}

template <typename TYPE>
void baejsn_Encoder::encodeNullable(const TYPE & value)
{
    if (bdeat_NullableValueFunctions::isNull(value)) {
        d_outputStream << "null";
        return;
    }

    baejsn_Encoder_ElementVisitor visitor = { this };
    bdeat_NullableValueFunctions::accessValue(value, visitor);
    return;
}

template <typename TYPE>
int baejsn_Encoder::encode(bsl::streambuf *streamBuf, const TYPE& variable)
{
    //BSLS_ASSERT(0 == d_streamBuf);
    BSLS_ASSERT(streamBuf);

    d_outputStream.rdbuf(streamBuf);

    //if (d_logStream != 0) {
    //    d_logStream->reset();
    //}

    //int rc = encode(variable);
    encode(variable);

    d_outputStream.rdbuf(0);
    return 0;
}

template <typename TYPE>
int baejsn_Encoder::encode(bsl::ostream& stream, const TYPE& value)
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

// CREATORS
inline
baejsn_Encoder::baejsn_Encoder()
: d_outputStream(0)
{
}

                    // -------------------------------------
                    // struct baejsn_Encoder_SequenceVisitor
                    // -------------------------------------

inline
baejsn_Encoder_SequenceVisitor::baejsn_Encoder_SequenceVisitor(
                                                       baejsn_Encoder *encoder)
: d_encoder(encoder)
, d_firstPassFlag(true)
{
}

template <typename TYPE, typename ATTRIBUTE_OR_SELECTION>
inline
int baejsn_Encoder_SequenceVisitor::operator()(const TYPE &value,
                                               const ATTRIBUTE_OR_SELECTION &info)
{
    if (!d_firstPassFlag) {
        d_encoder->encode(',');
    }
    d_encoder->encode(info.name());
    d_encoder->d_outputStream << ':';
    d_encoder->encode(value);

    // TBD: Check return value of encode before setting d_firstPassFlag.
    d_firstPassFlag = true;

    return 0;
}

                    // ------------------------------------
                    // struct baejsn_Encoder_ElementVisitor
                    // ------------------------------------

template <typename TYPE>
inline
int baejsn_Encoder_ElementVisitor::operator()(const TYPE &value)
{
    d_encoder->encode(value);
    return 0;
}

template <typename TYPE, typename ATTRIBUTE_OR_SELECTION>
inline
int baejsn_Encoder_ElementVisitor::operator()(const TYPE &value,
                                              const ATTRIBUTE_OR_SELECTION &info)
{
    d_encoder->encode(info.name());
    d_encoder->d_outputStream << ':';
    d_encoder->encode(value);

    return 0;
}

                    // ------------------------------------
                    // struct baejsn_Encoder_EncodeImpProxy
                    // ------------------------------------

// MANIPULATORS
template <typename TYPE>
inline
int baejsn_Encoder_EncodeImpProxy::operator()(const TYPE&, bslmf_Nil)
{
    BSLS_ASSERT_SAFE(0);
    return -1;
}

template <typename TYPE, typename ANY_CATEGORY>
inline
int baejsn_Encoder_EncodeImpProxy::operator()(const TYPE&  object,
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
