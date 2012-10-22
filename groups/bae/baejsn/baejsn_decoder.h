// baejsn_decoder.h                                                   -*-C++-*-
#ifndef INCLUDED_BAEJSN_DECODER
#define INCLUDED_BAEJSN_DECODER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a JSON decoder that decodes into 'bdeat' compatible types
//
//@CLASSES:
//  baejsn_Decoder: JSON decoder for 'bdeat' compliant types
//
//@SEE_ALSO: baejsn_encoder, baejsn_parserutil
//
//@AUTHOR: Raymond Chiu (schiu49)
//
//@DESCRIPTION: This component provides utility functions for decoding a JSON
// string.

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEJSN_PARSERUTIL
#include <baejsn_parserutil.h>
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

#ifndef INCLUDED_BDEAT_FORMATTINGMODE
#include <bdeat_formattingmode.h>
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

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
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

namespace BloombergLP {

                        // ====================
                        // class baejsn_Decoder
                        // ====================

class baejsn_Decoder
{
    // DATA
    bsl::ostringstream  d_logStream;       // stream to store error message 
    bsl::streambuf     *d_streamBuf;       // held, not owned

    // FRIENDS
    friend struct baejsn_Decoder_DecodeImpProxy;
    friend struct baejsn_Decoder_ElementVisitor;

    // PRIVATE MANIPULATORS
    int decodeBinaryArray(std::vector<char> *value);

    template <typename TYPE>
    int decodeImp(TYPE *value, bdeat_TypeCategory::DynamicType);

    template <typename TYPE>
    int decodeImp(TYPE *value, bdeat_TypeCategory::Sequence);

    template <typename TYPE>
    int decodeImp(TYPE *value, bdeat_TypeCategory::Choice);

    template <typename TYPE>
    int decodeImp(TYPE *value, bdeat_TypeCategory::Enumeration);

    template <typename TYPE>
    int decodeImp(TYPE *value, bdeat_TypeCategory::CustomizedType);

    template <typename TYPE>
    int decodeImp(TYPE *value, bdeat_TypeCategory::Simple);

    template <typename TYPE>
    int decodeImp(TYPE *value, bdeat_TypeCategory::Array);

    template <typename TYPE>
    int decodeImp(TYPE *value, bdeat_TypeCategory::NullableValue);

    template <typename TYPE, typename ANY_CATEGORY>
    int decodeImp(TYPE *object, ANY_CATEGORY category);

  public:
    // CREATORS
    baejsn_Decoder(bslma_Allocator *basicAllocator = 0);
        // Construct a decoder object using the optionally specified
        // 'basicAllocator'.  If 'basicAllocator' is 0, the default allocator
        // is used.

    template <typename TYPE>
    int decode(bsl::streambuf *streamBuf, TYPE *variable);
        // Decode an object of parameterized 'TYPE' from the specified
        // 'streamBuf' and load the result into the specified modifiable
        // 'variable'.  Return 0 on success, and a non-zero value otherwise.

    template <typename TYPE>
    int decode(bsl::istream& stream, TYPE *variable);
        // Decode an object of parameterized 'TYPE' from the specified 'stream'
        // and load the result into the specified modifiable 'variable'.
        // Return 0 on success, and a non-zero value otherwise.  Note that
        // 'stream' will be invalidated if the decoding fails.

    // ACCESSORS
    bsl::string loggedMessages() const;
        // Return a string containing any error, warning, or trace messages
        // that were logged during the last call to the 'decode' method.  The
        // log is reset each time 'decode' is called.
};

                 // ====================================
                 // struct baejsn_Decoder_ElementVisitor
                 // ====================================

struct baejsn_Decoder_ElementVisitor {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.

    // DATA
    baejsn_Decoder *d_decoder_p;

    // CREATORS
    // Creators have been omitted to allow simple static initialization of
    // this struct.

    // MANIPULATORS
    template <typename TYPE>
    int operator()(TYPE *value);

    template <typename TYPE, typename INFO>
    int operator()(TYPE *value, const INFO&);
};

                 // ====================================
                 // struct baejsn_Decoder_DecodeImpProxy
                 // ====================================

struct baejsn_Decoder_DecodeImpProxy {
    // COMPONENT-PRIVATE CLASS.  DO NOT USE OUTSIDE OF THIS COMPONENT.

    // DATA
    baejsn_Decoder *d_decoder_p;

    // CREATORS
    // Creators have been omitted to allow simple static initialization of
    // this struct.

    // MANIPULATORS
    template <typename TYPE>
    int operator()(TYPE *, bslmf_Nil);

    template <typename TYPE, typename ANY_CATEGORY>
    int operator()(TYPE *object, ANY_CATEGORY category);
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // --------------------
                        // class baejsn_Decoder
                        // --------------------

// PRIVATE MANIPULATORS
template <typename TYPE>
inline
int baejsn_Decoder::decodeImp(TYPE *value, bdeat_TypeCategory::DynamicType)
{
    baejsn_Decoder_DecodeImpProxy proxy = { this };
    return bdeat_TypeCategoryUtil::manipulateByCategory(value, proxy);
}

template <typename TYPE>
int baejsn_Decoder::decodeImp(TYPE *value, bdeat_TypeCategory::Sequence)
{
    if (0 != baejsn_ParserUtil::advancePastWhitespaceAndToken(d_streamBuf,
                                                              '{')) {
        d_logStream << "Could not decode sequence, missing starting {";
        return 1;                                                     // RETURN
    }

    if (0 != baejsn_ParserUtil::advancePastWhitespaceAndToken(d_streamBuf,
                                                              '}')) {
        bool hasMore = false;
        do {
            baejsn_ParserUtil::skipSpaces(d_streamBuf);

            bsl::string attributeName;
            if (0 != baejsn_ParserUtil::getString(d_streamBuf,
                                                  &attributeName)) {
                d_logStream << "Could not decode sequence, "
                               "missing element string after ',' or '{'";
                return 1;                                             // RETURN
            }

            if (attributeName.empty()) {
                d_logStream << "Could not decode sequence, "
                               "empty attribute names are not permitted";
                return 1;                                             // RETURN
            }

            if (0 != baejsn_ParserUtil::advancePastWhitespaceAndToken(
                                                                   d_streamBuf,
                                                                   ':')) {
                d_logStream << "Could not decode sequence, "
                               "missing ':' after attribute name";
                return 1;                                             // RETURN
            }

            baejsn_Decoder_ElementVisitor visitor = { this };
            if (0 != bdeat_SequenceFunctions::manipulateAttribute(
                                                     value,
                                                     visitor,
                                                     attributeName.data(),
                                                     attributeName.length())) {
                d_logStream << "Could not decode sequence, "
                               "unknown attribute id " << attributeName;
                return 1;                                             // RETURN
            }

            hasMore =
                0 == baejsn_ParserUtil::advancePastWhitespaceAndToken(
                                                                   d_streamBuf,
                                                                   ',');
        } while (hasMore);

        if (0 != baejsn_ParserUtil::advancePastWhitespaceAndToken(d_streamBuf,
                                                                  '}')) {
            d_logStream << "Could not decode sequence, "
                           "missing terminator '}' or seperator ','";
            return 1;                                                 // RETURN
        }
    }

    return 0;
}

template <typename TYPE>
inline
int baejsn_Decoder::decodeImp(TYPE *value, bdeat_TypeCategory::Choice)
{
    if (0 != baejsn_ParserUtil::advancePastWhitespaceAndToken(d_streamBuf,
                                                              '{')) {
        d_logStream << "Could not decode choice, missing starting {";
        return 1;                                                     // RETURN
    }

    baejsn_ParserUtil::skipSpaces(d_streamBuf);

    bsl::string attributeName;

    if (0 != baejsn_ParserUtil::getString(d_streamBuf, &attributeName)) {
        d_logStream << "Could not decode choice, missing attribute name";
        return 1;                                                     // RETURN
    }

    if (0 != bdeat_ChoiceFunctions::makeSelection(value,
                                                  attributeName.data(),
                                                  attributeName.length())) {
        d_logStream << "Could not deocde choice, bad attribute name '"
                    << attributeName << "'";
        return 1;                                                     // RETURN
    }

    if (0 != baejsn_ParserUtil::advancePastWhitespaceAndToken(d_streamBuf,
                                                              ':')) {
        d_logStream << "Could not decode choice, "
                       "missing ':' after selection name";
        return 1;                                                     // RETURN
    }

    baejsn_Decoder_ElementVisitor visitor = { this };
    if (0 != bdeat_ChoiceFunctions::manipulateSelection(value, visitor)) {
        d_logStream << "Could not decode choice, attribute '"
                    << attributeName << "' was not decoded";
        return 1;                                                     // RETURN
    }

    if (0 != baejsn_ParserUtil::advancePastWhitespaceAndToken(d_streamBuf,
                                                              '}')) {
        d_logStream << "Could not decode choice, "
                       "missing terminator '}' or seperator ','";
        return 1;                                                     // RETURN
    }
    return 0;
}

template <typename TYPE>
inline
int baejsn_Decoder::decodeImp(TYPE *value, bdeat_TypeCategory::Enumeration)
{
    bsl::string valueString;

    if (0 != decodeImp(&valueString, bdeat_TypeCategory::Simple())) {
        d_logStream << "Error decoding enumeration";
        return 1;                                                     // RETURN
    }

    const int rc = bdeat_EnumFunctions::fromString(value,
                                                   valueString.data(),
                                                   valueString.length());

    if (rc) {
        d_logStream << "Could not decode Enum String, value not allowed \""
                    << valueString << "\"";
        return 1;                                                     // RETURN
    }
    return 0;
}

template <typename TYPE>
inline
int baejsn_Decoder::decodeImp(TYPE *value, bdeat_TypeCategory::CustomizedType)
{
    typename bdeat_CustomizedTypeFunctions::BaseType<TYPE>::Type valueBaseType;

    if (0 != decodeImp(&valueBaseType, bdeat_TypeCategory::Simple())) {
        d_logStream << "Error decoding customized type";
        return 1;                                                     // RETURN
    }

    const int rc = bdeat_CustomizedTypeFunctions::convertFromBaseType(
                                                                value,
                                                                valueBaseType);
    if (rc) {
        d_logStream << "Could not decode Enum Customized, "
                       "value not allowed \"" << valueBaseType << "\"";
        return 1;                                                     // RETURN
    }
    return 0;
}

template <typename TYPE>
inline
int baejsn_Decoder::decodeImp(TYPE *value, bdeat_TypeCategory::Simple)
{
    return baejsn_ParserUtil::getValue(d_streamBuf, value);
}

template <>
inline
int baejsn_Decoder::decodeImp(bsl::vector<char> *value,
                              bdeat_TypeCategory::Array)
{
    return decodeBinaryArray(value);
}

template <typename TYPE>
int baejsn_Decoder::decodeImp(TYPE *value, bdeat_TypeCategory::Array)
{
    if (0 != baejsn_ParserUtil::advancePastWhitespaceAndToken(d_streamBuf,
                                                              '[')) {
        d_logStream << "Could not decode vector, missing start [";
        return 1;                                                     // RETURN
    }

    if (0 != baejsn_ParserUtil::advancePastWhitespaceAndToken(d_streamBuf,
                                                              ']')) {

        int i = static_cast<int>(bdeat_ArrayFunctions::size(*value));

        do {
            ++i;
            bdeat_ArrayFunctions::resize(value, i);

            baejsn_Decoder_ElementVisitor visitor = { this };
            if (0 != bdeat_ArrayFunctions::manipulateElement(value,
                                                             visitor,
                                                             i)) {
                d_logStream << "Could not element '" << i << "\'";
                return 1;                                             // RETURN
            }
        } while (0 != baejsn_ParserUtil::advancePastWhitespaceAndToken(
                                                                   d_streamBuf,
                                                                   ','));

        if (0 != baejsn_ParserUtil::advancePastWhitespaceAndToken(d_streamBuf,
                                                                  ']')) {
            d_logStream << "Could not decode vector, missing end ]";
            return 1;                                                 // RETURN
        }
    }
    return 0;
}

template <typename TYPE>
inline
int baejsn_Decoder::decodeImp(TYPE *value, bdeat_TypeCategory::NullableValue)
{
    baejsn_ParserUtil::skipSpaces(d_streamBuf);

    if (0 == baejsn_ParserUtil::eatToken(d_streamBuf, "null")) {
        return 0;
    }

    bdeat_NullableValueFunctions::makeValue(value);

    baejsn_Decoder_ElementVisitor visitor = { this };
    return bdeat_NullableValueFunctions::manipulateValue(value, visitor);
}

template <typename TYPE, typename ANY_CATEGORY>
inline
int baejsn_Decoder::decodeImp(TYPE *, ANY_CATEGORY)
{
    BSLS_ASSERT_OPT(0 && "Unreachable");
    return 1;                                                         // RETURN
}

// CREATORS
inline
baejsn_Decoder::baejsn_Decoder(bslma_Allocator *basicAllocator)
: d_logStream(basicAllocator)
, d_streamBuf(0)
{
}


// MANIPULATORS
template <typename TYPE>
int baejsn_Decoder::decode(bsl::streambuf *streamBuf, TYPE *variable)
{
    BSLS_ASSERT(0 == d_streamBuf);
    BSLS_ASSERT(streamBuf);
    BSLS_ASSERT(variable);
    BSLMF_ASSERT(bdeat_SequenceFunctions::IsSequence<TYPE>::VALUE
              || bdeat_ChoiceFunctions::IsChoice<TYPE>::VALUE);

    d_streamBuf = streamBuf;

    d_logStream.clear();
    d_logStream.str("");

    bdeat_ValueTypeFunctions::reset(variable);

    typedef typename bdeat_TypeCategory::Select<TYPE>::Type TypeCategory;

    const int rc = decodeImp(variable, TypeCategory());

    d_streamBuf = 0;
    return rc;
}

template <typename TYPE>
int baejsn_Decoder::decode(bsl::istream& stream, TYPE *value)
{
    if (!stream.good()) {
        return -1;                                                    // RETURN
    }

    if (0 != decode(stream.rdbuf(), value)) {
        stream.setstate(bsl::ios_base::failbit);
        return -1;                                                    // RETURN
    }

    return 0;
}

// ACCESSORS
inline
bsl::string baejsn_Decoder::loggedMessages() const
{
    return d_logStream.str();
}


                    // ------------------------------------
                    // struct baejsn_Decoder_ElementVisitor
                    // ------------------------------------

template <typename TYPE>
inline
int baejsn_Decoder_ElementVisitor::operator()(TYPE *value)
{
    typedef typename bdeat_TypeCategory::Select<TYPE>::Type TypeCategory;
    return d_decoder_p->decodeImp(value, TypeCategory());
}

template <typename TYPE, typename INFO>
inline
int baejsn_Decoder_ElementVisitor::operator()(TYPE *value, const INFO&)
{
    return (*this)(value);
}

                    // ------------------------------------
                    // struct baejsn_Decoder_DecodeImpProxy
                    // ------------------------------------

// MANIPULATORS
template <typename TYPE>
inline
int baejsn_Decoder_DecodeImpProxy::operator()(TYPE *, bslmf_Nil)
{
    BSLS_ASSERT_OPT(0 && "Unreachable");
    return -1;
}

template <typename TYPE, typename ANY_CATEGORY>
inline
int baejsn_Decoder_DecodeImpProxy::operator()(TYPE         *object,
                                              ANY_CATEGORY  category)
{
    return d_decoder_p->decodeImp(object, category);
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
