// s_baltst_request.h            *DO NOT EDIT*             @generated -*-C++-*-
#ifndef INCLUDED_S_BALTST_REQUEST
#define INCLUDED_S_BALTST_REQUEST

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_request_h, "$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes

#include <bslalg_typetraits.h>

#include <bdlat_attributeinfo.h>

#include <bdlat_selectioninfo.h>

#include <bdlat_typetraits.h>

#include <bsls_objectbuffer.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <s_baltst_featuretestmessage.h>

#include <s_baltst_simplerequest.h>

#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace s_baltst { class Request; }
namespace s_baltst {

                               // =============
                               // class Request
                               // =============

class Request {

    // INSTANCE DATA
    union {
        bsls::ObjectBuffer< s_baltst::SimpleRequest >      d_simpleRequest;
        bsls::ObjectBuffer< s_baltst::FeatureTestMessage > d_featureRequest;
    };

    int                                                    d_selectionId;
    bslma::Allocator                                      *d_allocator_p;

    // PRIVATE ACCESSORS
    bool isEqualTo(const Request& rhs) const;

  public:
    // TYPES

    enum {
        SELECTION_ID_UNDEFINED       = -1
      , SELECTION_ID_SIMPLE_REQUEST  = 0
      , SELECTION_ID_FEATURE_REQUEST = 1
    };

    enum {
        NUM_SELECTIONS = 2
    };

    enum {
        SELECTION_INDEX_SIMPLE_REQUEST  = 0
      , SELECTION_INDEX_FEATURE_REQUEST = 1
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdlat_SelectionInfo SELECTION_INFO_ARRAY[];

    // CLASS METHODS

    /// Return selection information for the selection indicated by the
    /// specified `id` if the selection exists, and 0 otherwise.
    static const bdlat_SelectionInfo *lookupSelectionInfo(int id);

    /// Return selection information for the selection indicated by the
    /// specified `name` of the specified `nameLength` if the selection
    /// exists, and 0 otherwise.
    static const bdlat_SelectionInfo *lookupSelectionInfo(
                                                       const char *name,
                                                       int         nameLength);

    // CREATORS

    /// Create an object of type `Request` having the default value.  Use
    /// the optionally specified `basicAllocator` to supply memory.  If
    /// `basicAllocator` is 0, the currently installed default allocator is
    /// used.
    explicit Request(bslma::Allocator *basicAllocator = 0);

    /// Create an object of type `Request` having the value of the specified
    /// `original` object.  Use the optionally specified `basicAllocator` to
    /// supply memory.  If `basicAllocator` is 0, the currently installed
    /// default allocator is used.
    Request(const Request& original,
           bslma::Allocator *basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Create an object of type `Request` having the value of the specified
    /// `original` object.  After performing this action, the `original`
    /// object will be left in a valid, but unspecified state.
    Request(Request&& original) noexcept;

    /// Create an object of type `Request` having the value of the specified
    /// `original` object.  After performing this action, the `original`
    /// object will be left in a valid, but unspecified state.  Use the
    /// optionally specified `basicAllocator` to supply memory.  If
    /// `basicAllocator` is 0, the currently installed default allocator is
    /// used.
    Request(Request&& original,
           bslma::Allocator *basicAllocator);
#endif

    /// Destroy this object.
    ~Request();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    Request& operator=(const Request& rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    /// Assign to this object the value of the specified `rhs` object.
    /// After performing this action, the `rhs` object will be left in a
    /// valid, but unspecified state.
    Request& operator=(Request&& rhs);
#endif

    /// Reset this object to the default value (i.e., its value upon default
    /// construction).
    void reset();

    /// Set the value of this object to be the default for the selection
    /// indicated by the specified `selectionId`.  Return 0 on success, and
    /// non-zero value otherwise (i.e., the selection is not found).
    int makeSelection(int selectionId);

    /// Set the value of this object to be the default for the selection
    /// indicated by the specified `name` of the specified `nameLength`.
    /// Return 0 on success, and non-zero value otherwise (i.e., the
    /// selection is not found).
    int makeSelection(const char *name, int nameLength);

    s_baltst::SimpleRequest& makeSimpleRequest();
    s_baltst::SimpleRequest& makeSimpleRequest(const s_baltst::SimpleRequest& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    s_baltst::SimpleRequest& makeSimpleRequest(s_baltst::SimpleRequest&& value);
#endif
        // Set the value of this object to be a "SimpleRequest" value.
        // Optionally specify the 'value' of the "SimpleRequest".  If 'value'
        // is not specified, the default "SimpleRequest" value is used.

    s_baltst::FeatureTestMessage& makeFeatureRequest();
    s_baltst::FeatureTestMessage& makeFeatureRequest(const s_baltst::FeatureTestMessage& value);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    s_baltst::FeatureTestMessage& makeFeatureRequest(s_baltst::FeatureTestMessage&& value);
#endif
        // Set the value of this object to be a "FeatureRequest" value.
        // Optionally specify the 'value' of the "FeatureRequest".  If 'value'
        // is not specified, the default "FeatureRequest" value is used.

    /// Invoke the specified `manipulator` on the address of the modifiable
    /// selection, supplying `manipulator` with the corresponding selection
    /// information structure.  Return the value returned from the
    /// invocation of `manipulator` if this object has a defined selection,
    /// and -1 otherwise.
    template <typename t_MANIPULATOR>
    int manipulateSelection(t_MANIPULATOR& manipulator);

    /// Return a reference to the modifiable "SimpleRequest" selection of
    /// this object if "SimpleRequest" is the current selection.  The
    /// behavior is undefined unless "SimpleRequest" is the selection of
    /// this object.
    s_baltst::SimpleRequest& simpleRequest();

    /// Return a reference to the modifiable "FeatureRequest" selection of
    /// this object if "FeatureRequest" is the current selection.  The
    /// behavior is undefined unless "FeatureRequest" is the selection of
    /// this object.
    s_baltst::FeatureTestMessage& featureRequest();

    // ACCESSORS

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;

    /// Return the id of the current selection if the selection is defined,
    /// and -1 otherwise.
    int selectionId() const;

    /// Invoke the specified `accessor` on the non-modifiable selection,
    /// supplying `accessor` with the corresponding selection information
    /// structure.  Return the value returned from the invocation of
    /// `accessor` if this object has a defined selection, and -1 otherwise.
    template <typename t_ACCESSOR>
    int accessSelection(t_ACCESSOR& accessor) const;

    /// Return a reference to the non-modifiable "SimpleRequest" selection
    /// of this object if "SimpleRequest" is the current selection.  The
    /// behavior is undefined unless "SimpleRequest" is the selection of
    /// this object.
    const s_baltst::SimpleRequest& simpleRequest() const;

    /// Return a reference to the non-modifiable "FeatureRequest" selection
    /// of this object if "FeatureRequest" is the current selection.  The
    /// behavior is undefined unless "FeatureRequest" is the selection of
    /// this object.
    const s_baltst::FeatureTestMessage& featureRequest() const;

    /// Return `true` if the value of this object is a "SimpleRequest"
    /// value, and return `false` otherwise.
    bool isSimpleRequestValue() const;

    /// Return `true` if the value of this object is a "FeatureRequest"
    /// value, and return `false` otherwise.
    bool isFeatureRequestValue() const;

    /// Return `true` if the value of this object is undefined, and `false`
    /// otherwise.
    bool isUndefinedValue() const;

    /// Return the symbolic name of the current selection of this object.
    const char *selectionName() const;

    // HIDDEN FRIENDS

    /// Return `true` if the specified `lhs` and `rhs` objects have the same
    /// value, and `false` otherwise.  Two `Request` objects have the same
    /// value if either the selections in both objects have the same ids and
    /// the same values, or both selections are undefined.
    friend bool operator==(const Request& lhs, const Request& rhs)
    {
        return lhs.isEqualTo(rhs);
    }

    /// Return `true` if the specified `lhs` and `rhs` objects do not have
    /// the same values, as determined by `operator==`, and `false`
    /// otherwise.
    friend bool operator!=(const Request& lhs, const Request& rhs)
    {
        return !(lhs == rhs);
    }

    /// Format the specified `rhs` to the specified output `stream` and
    /// return a reference to the modifiable `stream`.
    friend bsl::ostream& operator<<(bsl::ostream& stream, const Request& rhs)
    {
        return rhs.print(stream, 0, -1);
    }
};

}  // close package namespace

// TRAITS

BDLAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(s_baltst::Request)

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

namespace s_baltst {

                               // -------------
                               // class Request
                               // -------------

// CLASS METHODS
// PRIVATE ACCESSORS
inline
bool Request::isEqualTo(const Request& rhs) const
{
    typedef Request Class;
    if (this->selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_SIMPLE_REQUEST:
            return this->simpleRequest() == rhs.simpleRequest();
          case Class::SELECTION_ID_FEATURE_REQUEST:
            return this->featureRequest() == rhs.featureRequest();
          default:
            BSLS_ASSERT(Class::SELECTION_ID_UNDEFINED == rhs.selectionId());
            return true;
        }
    }
    else {
        return false;
    }
}

// CREATORS
inline
Request::Request(bslma::Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
Request::~Request()
{
    reset();
}

// MANIPULATORS
template <typename t_MANIPULATOR>
int Request::manipulateSelection(t_MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case Request::SELECTION_ID_SIMPLE_REQUEST:
        return manipulator(&d_simpleRequest.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SIMPLE_REQUEST]);
      case Request::SELECTION_ID_FEATURE_REQUEST:
        return manipulator(&d_featureRequest.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_FEATURE_REQUEST]);
      default:
        BSLS_ASSERT(Request::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
s_baltst::SimpleRequest& Request::simpleRequest()
{
    BSLS_ASSERT(SELECTION_ID_SIMPLE_REQUEST == d_selectionId);
    return d_simpleRequest.object();
}

inline
s_baltst::FeatureTestMessage& Request::featureRequest()
{
    BSLS_ASSERT(SELECTION_ID_FEATURE_REQUEST == d_selectionId);
    return d_featureRequest.object();
}

// ACCESSORS
inline
int Request::selectionId() const
{
    return d_selectionId;
}

template <typename t_ACCESSOR>
int Request::accessSelection(t_ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_SIMPLE_REQUEST:
        return accessor(d_simpleRequest.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_SIMPLE_REQUEST]);
      case SELECTION_ID_FEATURE_REQUEST:
        return accessor(d_featureRequest.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_FEATURE_REQUEST]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const s_baltst::SimpleRequest& Request::simpleRequest() const
{
    BSLS_ASSERT(SELECTION_ID_SIMPLE_REQUEST == d_selectionId);
    return d_simpleRequest.object();
}

inline
const s_baltst::FeatureTestMessage& Request::featureRequest() const
{
    BSLS_ASSERT(SELECTION_ID_FEATURE_REQUEST == d_selectionId);
    return d_featureRequest.object();
}

inline
bool Request::isSimpleRequestValue() const
{
    return SELECTION_ID_SIMPLE_REQUEST == d_selectionId;
}

inline
bool Request::isFeatureRequestValue() const
{
    return SELECTION_ID_FEATURE_REQUEST == d_selectionId;
}

inline
bool Request::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}
}  // close package namespace

// FREE FUNCTIONS

}  // close enterprise namespace
#endif

// GENERATED BY BLP_BAS_CODEGEN_2025.08.21
// USING bas_codegen.pl s_baltst_request.xsd --mode msg --includedir . --msgComponent request --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2025 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
