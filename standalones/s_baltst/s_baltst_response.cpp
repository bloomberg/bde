// s_baltst_response.cpp           *DO NOT EDIT*           @generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_response_cpp, "$Id$ $CSID$")

#include <s_baltst_response.h>

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <bsl_string.h>
#include <s_baltst_featuretestmessage.h>
#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>
#include <bsl_utility.h>

namespace BloombergLP {
namespace s_baltst {

                               // --------------
                               // class Response
                               // --------------

// CONSTANTS

const char Response::CLASS_NAME[] = "Response";

const bdlat_SelectionInfo Response::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_RESPONSE_DATA,
        "responseData",
        sizeof("responseData") - 1,
        "",
        bdlat_FormattingMode::e_TEXT
    },
    {
        SELECTION_ID_FEATURE_RESPONSE,
        "featureResponse",
        sizeof("featureResponse") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *Response::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 2; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    Response::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *Response::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_RESPONSE_DATA:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_RESPONSE_DATA];
      case SELECTION_ID_FEATURE_RESPONSE:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_FEATURE_RESPONSE];
      default:
        return 0;
    }
}

// CREATORS

Response::Response(
    const Response& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_RESPONSE_DATA: {
        new (d_responseData.buffer())
            bsl::string(
                original.d_responseData.object(), d_allocator_p);
      } break;
      case SELECTION_ID_FEATURE_RESPONSE: {
        new (d_featureResponse.buffer())
            s_baltst::FeatureTestMessage(
                original.d_featureResponse.object(), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Response::Response(Response&& original) noexcept
: d_selectionId(original.d_selectionId)
, d_allocator_p(original.d_allocator_p)
{
    switch (d_selectionId) {
      case SELECTION_ID_RESPONSE_DATA: {
        new (d_responseData.buffer())
            bsl::string(
                bsl::move(original.d_responseData.object()), d_allocator_p);
      } break;
      case SELECTION_ID_FEATURE_RESPONSE: {
        new (d_featureResponse.buffer())
            s_baltst::FeatureTestMessage(
                bsl::move(original.d_featureResponse.object()), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

Response::Response(
    Response&& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_RESPONSE_DATA: {
        new (d_responseData.buffer())
            bsl::string(
                bsl::move(original.d_responseData.object()), d_allocator_p);
      } break;
      case SELECTION_ID_FEATURE_RESPONSE: {
        new (d_featureResponse.buffer())
            s_baltst::FeatureTestMessage(
                bsl::move(original.d_featureResponse.object()), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

// MANIPULATORS

Response&
Response::operator=(const Response& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_RESPONSE_DATA: {
            makeResponseData(rhs.d_responseData.object());
          } break;
          case SELECTION_ID_FEATURE_RESPONSE: {
            makeFeatureResponse(rhs.d_featureResponse.object());
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Response&
Response::operator=(Response&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_RESPONSE_DATA: {
            makeResponseData(bsl::move(rhs.d_responseData.object()));
          } break;
          case SELECTION_ID_FEATURE_RESPONSE: {
            makeFeatureResponse(bsl::move(rhs.d_featureResponse.object()));
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}
#endif

void Response::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_RESPONSE_DATA: {
        typedef bsl::string Type;
        d_responseData.object().~Type();
      } break;
      case SELECTION_ID_FEATURE_RESPONSE: {
        typedef s_baltst::FeatureTestMessage Type;
        d_featureResponse.object().~Type();
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int Response::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_RESPONSE_DATA: {
        makeResponseData();
      } break;
      case SELECTION_ID_FEATURE_RESPONSE: {
        makeFeatureResponse();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int Response::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

bsl::string& Response::makeResponseData()
{
    if (SELECTION_ID_RESPONSE_DATA == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_responseData.object());
    }
    else {
        reset();
        new (d_responseData.buffer())
                bsl::string(d_allocator_p);
        d_selectionId = SELECTION_ID_RESPONSE_DATA;
    }

    return d_responseData.object();
}

bsl::string& Response::makeResponseData(const bsl::string& value)
{
    if (SELECTION_ID_RESPONSE_DATA == d_selectionId) {
        d_responseData.object() = value;
    }
    else {
        reset();
        new (d_responseData.buffer())
                bsl::string(value, d_allocator_p);
        d_selectionId = SELECTION_ID_RESPONSE_DATA;
    }

    return d_responseData.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
bsl::string& Response::makeResponseData(bsl::string&& value)
{
    if (SELECTION_ID_RESPONSE_DATA == d_selectionId) {
        d_responseData.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_responseData.buffer())
                bsl::string(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_RESPONSE_DATA;
    }

    return d_responseData.object();
}
#endif

s_baltst::FeatureTestMessage& Response::makeFeatureResponse()
{
    if (SELECTION_ID_FEATURE_RESPONSE == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_featureResponse.object());
    }
    else {
        reset();
        new (d_featureResponse.buffer())
                s_baltst::FeatureTestMessage(d_allocator_p);
        d_selectionId = SELECTION_ID_FEATURE_RESPONSE;
    }

    return d_featureResponse.object();
}

s_baltst::FeatureTestMessage& Response::makeFeatureResponse(const s_baltst::FeatureTestMessage& value)
{
    if (SELECTION_ID_FEATURE_RESPONSE == d_selectionId) {
        d_featureResponse.object() = value;
    }
    else {
        reset();
        new (d_featureResponse.buffer())
                s_baltst::FeatureTestMessage(value, d_allocator_p);
        d_selectionId = SELECTION_ID_FEATURE_RESPONSE;
    }

    return d_featureResponse.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
s_baltst::FeatureTestMessage& Response::makeFeatureResponse(s_baltst::FeatureTestMessage&& value)
{
    if (SELECTION_ID_FEATURE_RESPONSE == d_selectionId) {
        d_featureResponse.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_featureResponse.buffer())
                s_baltst::FeatureTestMessage(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_FEATURE_RESPONSE;
    }

    return d_featureResponse.object();
}
#endif

// ACCESSORS

bsl::ostream& Response::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
      case SELECTION_ID_RESPONSE_DATA: {
        printer.printAttribute("responseData", d_responseData.object());
      }  break;
      case SELECTION_ID_FEATURE_RESPONSE: {
        printer.printAttribute("featureResponse", d_featureResponse.object());
      }  break;
      default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}


const char *Response::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_RESPONSE_DATA:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_RESPONSE_DATA].name();
      case SELECTION_ID_FEATURE_RESPONSE:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_FEATURE_RESPONSE].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}
}  // close package namespace
}  // close enterprise namespace

// GENERATED BY @BLP_BAS_CODEGEN_VERSION@
// USING bas_codegen.pl s_baltst_response.xsd --mode msg --includedir . --msgComponent response --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2022 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
