// s_baltst_request.cpp           *DO NOT EDIT*            @generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_request_cpp, "$Id$ $CSID$")

#include <s_baltst_request.h>

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <s_baltst_featuretestmessage.h>
#include <s_baltst_simplerequest.h>
#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>
#include <bsl_utility.h>

namespace BloombergLP {
namespace s_baltst {

                               // -------------
                               // class Request
                               // -------------

// CONSTANTS

const char Request::CLASS_NAME[] = "Request";

const bdlat_SelectionInfo Request::SELECTION_INFO_ARRAY[] = {
    {
        SELECTION_ID_SIMPLE_REQUEST,
        "simpleRequest",
        sizeof("simpleRequest") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    },
    {
        SELECTION_ID_FEATURE_REQUEST,
        "featureRequest",
        sizeof("featureRequest") - 1,
        "",
        bdlat_FormattingMode::e_DEFAULT
    }
};

// CLASS METHODS

const bdlat_SelectionInfo *Request::lookupSelectionInfo(
        const char *name,
        int         nameLength)
{
    for (int i = 0; i < 2; ++i) {
        const bdlat_SelectionInfo& selectionInfo =
                    Request::SELECTION_INFO_ARRAY[i];

        if (nameLength == selectionInfo.d_nameLength
        &&  0 == bsl::memcmp(selectionInfo.d_name_p, name, nameLength))
        {
            return &selectionInfo;
        }
    }

    return 0;
}

const bdlat_SelectionInfo *Request::lookupSelectionInfo(int id)
{
    switch (id) {
      case SELECTION_ID_SIMPLE_REQUEST:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_SIMPLE_REQUEST];
      case SELECTION_ID_FEATURE_REQUEST:
        return &SELECTION_INFO_ARRAY[SELECTION_INDEX_FEATURE_REQUEST];
      default:
        return 0;
    }
}

// CREATORS

Request::Request(
    const Request& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SIMPLE_REQUEST: {
        new (d_simpleRequest.buffer())
            s_baltst::SimpleRequest(
                original.d_simpleRequest.object(), d_allocator_p);
      } break;
      case SELECTION_ID_FEATURE_REQUEST: {
        new (d_featureRequest.buffer())
            s_baltst::FeatureTestMessage(
                original.d_featureRequest.object(), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
Request::Request(Request&& original) noexcept
: d_selectionId(original.d_selectionId)
, d_allocator_p(original.d_allocator_p)
{
    switch (d_selectionId) {
      case SELECTION_ID_SIMPLE_REQUEST: {
        new (d_simpleRequest.buffer())
            s_baltst::SimpleRequest(
                bsl::move(original.d_simpleRequest.object()), d_allocator_p);
      } break;
      case SELECTION_ID_FEATURE_REQUEST: {
        new (d_featureRequest.buffer())
            s_baltst::FeatureTestMessage(
                bsl::move(original.d_featureRequest.object()), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}

Request::Request(
    Request&& original,
    bslma::Allocator *basicAllocator)
: d_selectionId(original.d_selectionId)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    switch (d_selectionId) {
      case SELECTION_ID_SIMPLE_REQUEST: {
        new (d_simpleRequest.buffer())
            s_baltst::SimpleRequest(
                bsl::move(original.d_simpleRequest.object()), d_allocator_p);
      } break;
      case SELECTION_ID_FEATURE_REQUEST: {
        new (d_featureRequest.buffer())
            s_baltst::FeatureTestMessage(
                bsl::move(original.d_featureRequest.object()), d_allocator_p);
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }
}
#endif

// MANIPULATORS

Request&
Request::operator=(const Request& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SIMPLE_REQUEST: {
            makeSimpleRequest(rhs.d_simpleRequest.object());
          } break;
          case SELECTION_ID_FEATURE_REQUEST: {
            makeFeatureRequest(rhs.d_featureRequest.object());
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
Request&
Request::operator=(Request&& rhs)
{
    if (this != &rhs) {
        switch (rhs.d_selectionId) {
          case SELECTION_ID_SIMPLE_REQUEST: {
            makeSimpleRequest(bsl::move(rhs.d_simpleRequest.object()));
          } break;
          case SELECTION_ID_FEATURE_REQUEST: {
            makeFeatureRequest(bsl::move(rhs.d_featureRequest.object()));
          } break;
          default:
            BSLS_ASSERT(SELECTION_ID_UNDEFINED == rhs.d_selectionId);
            reset();
        }
    }

    return *this;
}
#endif

void Request::reset()
{
    switch (d_selectionId) {
      case SELECTION_ID_SIMPLE_REQUEST: {
        typedef s_baltst::SimpleRequest Type;
        d_simpleRequest.object().~Type();
      } break;
      case SELECTION_ID_FEATURE_REQUEST: {
        typedef s_baltst::FeatureTestMessage Type;
        d_featureRequest.object().~Type();
      } break;
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
    }

    d_selectionId = SELECTION_ID_UNDEFINED;
}

int Request::makeSelection(int selectionId)
{
    switch (selectionId) {
      case SELECTION_ID_SIMPLE_REQUEST: {
        makeSimpleRequest();
      } break;
      case SELECTION_ID_FEATURE_REQUEST: {
        makeFeatureRequest();
      } break;
      case SELECTION_ID_UNDEFINED: {
        reset();
      } break;
      default:
        return -1;
    }
    return 0;
}

int Request::makeSelection(const char *name, int nameLength)
{
    const bdlat_SelectionInfo *selectionInfo =
                                         lookupSelectionInfo(name, nameLength);
    if (0 == selectionInfo) {
       return -1;
    }

    return makeSelection(selectionInfo->d_id);
}

s_baltst::SimpleRequest& Request::makeSimpleRequest()
{
    if (SELECTION_ID_SIMPLE_REQUEST == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_simpleRequest.object());
    }
    else {
        reset();
        new (d_simpleRequest.buffer())
                s_baltst::SimpleRequest(d_allocator_p);
        d_selectionId = SELECTION_ID_SIMPLE_REQUEST;
    }

    return d_simpleRequest.object();
}

s_baltst::SimpleRequest& Request::makeSimpleRequest(const s_baltst::SimpleRequest& value)
{
    if (SELECTION_ID_SIMPLE_REQUEST == d_selectionId) {
        d_simpleRequest.object() = value;
    }
    else {
        reset();
        new (d_simpleRequest.buffer())
                s_baltst::SimpleRequest(value, d_allocator_p);
        d_selectionId = SELECTION_ID_SIMPLE_REQUEST;
    }

    return d_simpleRequest.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
s_baltst::SimpleRequest& Request::makeSimpleRequest(s_baltst::SimpleRequest&& value)
{
    if (SELECTION_ID_SIMPLE_REQUEST == d_selectionId) {
        d_simpleRequest.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_simpleRequest.buffer())
                s_baltst::SimpleRequest(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_SIMPLE_REQUEST;
    }

    return d_simpleRequest.object();
}
#endif

s_baltst::FeatureTestMessage& Request::makeFeatureRequest()
{
    if (SELECTION_ID_FEATURE_REQUEST == d_selectionId) {
        bdlat_ValueTypeFunctions::reset(&d_featureRequest.object());
    }
    else {
        reset();
        new (d_featureRequest.buffer())
                s_baltst::FeatureTestMessage(d_allocator_p);
        d_selectionId = SELECTION_ID_FEATURE_REQUEST;
    }

    return d_featureRequest.object();
}

s_baltst::FeatureTestMessage& Request::makeFeatureRequest(const s_baltst::FeatureTestMessage& value)
{
    if (SELECTION_ID_FEATURE_REQUEST == d_selectionId) {
        d_featureRequest.object() = value;
    }
    else {
        reset();
        new (d_featureRequest.buffer())
                s_baltst::FeatureTestMessage(value, d_allocator_p);
        d_selectionId = SELECTION_ID_FEATURE_REQUEST;
    }

    return d_featureRequest.object();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
s_baltst::FeatureTestMessage& Request::makeFeatureRequest(s_baltst::FeatureTestMessage&& value)
{
    if (SELECTION_ID_FEATURE_REQUEST == d_selectionId) {
        d_featureRequest.object() = bsl::move(value);
    }
    else {
        reset();
        new (d_featureRequest.buffer())
                s_baltst::FeatureTestMessage(bsl::move(value), d_allocator_p);
        d_selectionId = SELECTION_ID_FEATURE_REQUEST;
    }

    return d_featureRequest.object();
}
#endif

// ACCESSORS

bsl::ostream& Request::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    switch (d_selectionId) {
      case SELECTION_ID_SIMPLE_REQUEST: {
        printer.printAttribute("simpleRequest", d_simpleRequest.object());
      }  break;
      case SELECTION_ID_FEATURE_REQUEST: {
        printer.printAttribute("featureRequest", d_featureRequest.object());
      }  break;
      default:
        stream << "SELECTION UNDEFINED\n";
    }
    printer.end();
    return stream;
}


const char *Request::selectionName() const
{
    switch (d_selectionId) {
      case SELECTION_ID_SIMPLE_REQUEST:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_SIMPLE_REQUEST].name();
      case SELECTION_ID_FEATURE_REQUEST:
        return SELECTION_INFO_ARRAY[SELECTION_INDEX_FEATURE_REQUEST].name();
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return "(* UNDEFINED *)";
    }
}
}  // close package namespace
}  // close enterprise namespace

// GENERATED BY @BLP_BAS_CODEGEN_VERSION@
// USING bas_codegen.pl s_baltst_request.xsd --mode msg --includedir . --msgComponent request --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2022 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
