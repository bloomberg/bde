// s_baltst_response.h            *DO NOT EDIT*            @generated -*-C++-*-
#ifndef INCLUDED_S_BALTST_RESPONSE
#define INCLUDED_S_BALTST_RESPONSE

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_response_h, "$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>

#include <bdlat_selectioninfo.h>

#include <bdlat_typetraits.h>

#include <bsls_objectbuffer.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bsl_string.h>

#include <s_baltst_featuretestmessage.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace s_baltst { class Response; }
namespace s_baltst {

                               // ==============
                               // class Response
                               // ==============

class Response {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< bsl::string >                  d_responseData;
        bsls::ObjectBuffer< s_baltst::FeatureTestMessage > d_featureResponse;
    };

    int                                                    d_selectionId;
    bslma::Allocator                                      *d_allocator_p;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED        = -1
      , SELECTION_ID_RESPONSE_DATA    = 0
      , SELECTION_ID_FEATURE_RESPONSE = 1
    };

    enum {
        NUM_SELECTIONS = 2
    };

    enum {
        SELECTION_INDEX_RESPONSE_DATA    = 0
      , SELECTION_INDEX_FEATURE_RESPONSE = 1
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];

    // CLASS METHODS
    static const bdlat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdlat_SelectionInfo *lookupSelectionInfo(
                                                       const char *name,
                                                       int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit Response(bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Response' having the default value.  Use
        // the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    Response(const Response& original,
            bslma::Allocator *basicAllocator = 0);
        // Create an object of type 'Response' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    Response(Response&& original) noexcept;
        // Create an object of type 'Response' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.

    Response(Response&& original,
            bslma::Allocator *basicAllocator);
        // Create an object of type 'Response' having the value of the
        // specified 'original' object.  After performing this action, the
        // 'original' object will be left in a valid, but unspecified state.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.
#endif

    ~Response();
        // Destroy this object.

    // MANIPULATORS
    Response& operator=(const Response& rhs);
        // Assign to this object the value of the specified 'rhs' object.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    Response& operator=(Response&& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // After performing this action, the 'rhs' object will be left in a
        // valid, but unspecified state.
#endif

    void reset();
        // Reset this object to the default value (i.e., its value upon default
        // construction).

    int makeSelection(int selectionId);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'selectionId'.  Return 0 on success, and
        // non-zero value otherwise (i.e., the selection is not found).

    int makeSelection(const char *name, int nameLength);
        // Set the value of this object to be the default for the selection
        // indicated by the specified 'name' of the specified 'nameLength'.
        // Return 0 on success, and non-zero value otherwise (i.e., the
        // selection is not found).

    bsl::string& makeResponseData();
    bsl::string& makeResponseData(const bsl::string& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    bsl::string& makeResponseData(bsl::string&& value);
#endif
        // Set the value of this object to be a "ResponseData" value.
        // Optionally specify the 'value' of the "ResponseData".  If 'value' is
        // not specified, the default "ResponseData" value is used.

    s_baltst::FeatureTestMessage& makeFeatureResponse();
    s_baltst::FeatureTestMessage& makeFeatureResponse(const s_baltst::FeatureTestMessage& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    s_baltst::FeatureTestMessage& makeFeatureResponse(s_baltst::FeatureTestMessage&& value);
#endif
        // Set the value of this object to be a "FeatureResponse" value.
        // Optionally specify the 'value' of the "FeatureResponse".  If 'value'
        // is not specified, the default "FeatureResponse" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    bsl::string& responseData();
        // Return a reference to the modifiable "ResponseData" selection of
        // this object if "ResponseData" is the current selection.  The
        // behavior is undefined unless "ResponseData" is the selection of this
        // object.

    s_baltst::FeatureTestMessage& featureResponse();
        // Return a reference to the modifiable "FeatureResponse" selection of
        // this object if "FeatureResponse" is the current selection.  The
        // behavior is undefined unless "FeatureResponse" is the selection of
        // this object.

    // ACCESSORS
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

    int selectionId() const;
        // Return the id of the current selection if the selection is defined,
        // and -1 otherwise.

    template<class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' on the non-modifiable selection,
        // supplying 'accessor' with the corresponding selection information
        // structure.  Return the value returned from the invocation of
        // 'accessor' if this object has a defined selection, and -1 otherwise.

    const bsl::string& responseData() const;
        // Return a reference to the non-modifiable "ResponseData" selection of
        // this object if "ResponseData" is the current selection.  The
        // behavior is undefined unless "ResponseData" is the selection of this
        // object.

    const s_baltst::FeatureTestMessage& featureResponse() const;
        // Return a reference to the non-modifiable "FeatureResponse" selection
        // of this object if "FeatureResponse" is the current selection.  The
        // behavior is undefined unless "FeatureResponse" is the selection of
        // this object.

    bool isResponseDataValue() const;
        // Return 'true' if the value of this object is a "ResponseData" value,
        // and return 'false' otherwise.

    bool isFeatureResponseValue() const;
        // Return 'true' if the value of this object is a "FeatureResponse"
        // value, and return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const Response& lhs, const Response& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'Response' objects have the same
    // value if either the selections in both objects have the same ids and
    // the same values, or both selections are undefined.

inline
bool operator!=(const Response& lhs, const Response& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const Response& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::Response)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace s_baltst {

                               // --------------
                               // class Response
                               // --------------

// CLASS METHODS
// CREATORS
inline
Response::Response(bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
Response::~Response()
{
    reset();
}

// MANIPULATORS
template <class MANIPULATOR>
int Response::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case Response::SELECTION_ID_RESPONSE_DATA:
        return manipulator(&d_responseData.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_RESPONSE_DATA]);
      case Response::SELECTION_ID_FEATURE_RESPONSE:
        return manipulator(&d_featureResponse.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_FEATURE_RESPONSE]);
      default:
        BSLS_ASSERT(Response::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
bsl::string& Response::responseData()
{
    BSLS_ASSERT(SELECTION_ID_RESPONSE_DATA == d_selectionId);
    return d_responseData.object();
}

inline
s_baltst::FeatureTestMessage& Response::featureResponse()
{
    BSLS_ASSERT(SELECTION_ID_FEATURE_RESPONSE == d_selectionId);
    return d_featureResponse.object();
}

// ACCESSORS
inline
int Response::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int Response::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_RESPONSE_DATA:
        return accessor(d_responseData.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_RESPONSE_DATA]);
      case SELECTION_ID_FEATURE_RESPONSE:
        return accessor(d_featureResponse.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_FEATURE_RESPONSE]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const bsl::string& Response::responseData() const
{
    BSLS_ASSERT(SELECTION_ID_RESPONSE_DATA == d_selectionId);
    return d_responseData.object();
}

inline
const s_baltst::FeatureTestMessage& Response::featureResponse() const
{
    BSLS_ASSERT(SELECTION_ID_FEATURE_RESPONSE == d_selectionId);
    return d_featureResponse.object();
}

inline
bool Response::isResponseDataValue() const
{
    return SELECTION_ID_RESPONSE_DATA == d_selectionId;
}

inline
bool Response::isFeatureResponseValue() const
{
    return SELECTION_ID_FEATURE_RESPONSE == d_selectionId;
}

inline
bool Response::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}
}  // close package namespace

// FREE FUNCTIONS

inline
bool s_baltst::operator==(
        const s_baltst::Response& lhs,
        const s_baltst::Response& rhs)
{
    typedef s_baltst::Response Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_RESPONSE_DATA:
            return lhs.responseData() == rhs.responseData();
          case Class::SELECTION_ID_FEATURE_RESPONSE:
            return lhs.featureResponse() == rhs.featureResponse();
          default:
            BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;
        }
    }
    else {
        return false;
   }
}

inline
bool s_baltst::operator!=(
        const s_baltst::Response& lhs,
        const s_baltst::Response& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& s_baltst::operator<<(
        bsl::ostream& stream,
        const s_baltst::Response& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace
#endif

// GENERATED BY @BLP_BAS_CODEGEN_VERSION@
// USING bas_codegen.pl s_baltst_response.xsd --mode msg --includedir . --msgComponent response --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2022 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
