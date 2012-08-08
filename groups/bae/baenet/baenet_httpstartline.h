// baenet_httpstartline.h   -*-C++-*-   GENERATED FILE -- DO NOT EDIT
#ifndef INCLUDED_BAENET_HTTPSTARTLINE
#define INCLUDED_BAENET_HTTPSTARTLINE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT_RCSID(baenet_httpstartline_h,"$Id$ $CSID$ $CCId$")
BDES_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide value-semantic attribute classes
//
//@AUTHOR: Rohan Bhindwale (rbhindwale@bloomberg.net)
//
//@SEE_ALSO: RFC 2616
//
//@DESCRIPTION:  This component provides a value-semantic type to represent an
// HTTP start line, as defined in RFC 2616.  It is essentially a choice between
// 'baenet_HttpRequestLine' and 'baenet_HttpStatusLine'.

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#include <bdeat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDEAT_SELECTIONINFO
#include <bdeat_selectioninfo.h>
#endif

#ifndef INCLUDED_BDEAT_TYPETRAITS
#include <bdeat_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BAENET_HTTPREQUESTLINE
#include <baenet_httprequestline.h>
#endif

#ifndef INCLUDED_BAENET_HTTPSTATUSLINE
#include <baenet_httpstatusline.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {


                       // ==============================                       
                       // class baenet_HttpStartLine                       
                       // ==============================                       

class baenet_HttpStartLine {

    // INSTANCE DATA
    union {
        bsls_ObjectBuffer< baenet_HttpRequestLine > d_requestLine;
        bsls_ObjectBuffer< baenet_HttpStatusLine >  d_statusLine;
    };

    int                                                 d_selectionId;
    bslma_Allocator                                    *d_allocator_p;

  public:
    // TYPES
    enum {
        SELECTION_ID_UNDEFINED    = -1

      , SELECTION_ID_REQUEST_LINE = 0
      , SELECTION_ID_STATUS_LINE  = 1
    };

    enum {
        NUM_SELECTIONS = 2
    };

    enum {
        SELECTION_INDEX_REQUEST_LINE = 0
      , SELECTION_INDEX_STATUS_LINE  = 1
    };

    // CONSTANTS
    static const char CLASS_NAME[];

    static const bdeat_SelectionInfo SELECTION_INFO_ARRAY[];

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    static const bdeat_SelectionInfo *lookupSelectionInfo(int id);
        // Return selection information for the selection indicated by the
        // specified 'id' if the selection exists, and 0 otherwise.

    static const bdeat_SelectionInfo *lookupSelectionInfo(
                                                    const char *name,
                                                    int         nameLength);
        // Return selection information for the selection indicated by the
        // specified 'name' of the specified 'nameLength' if the selection
        // exists, and 0 otherwise.

    // CREATORS
    explicit baenet_HttpStartLine(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'baenet_HttpStartLine' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    baenet_HttpStartLine(const baenet_HttpStartLine& original,
                            bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'baenet_HttpStartLine' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~baenet_HttpStartLine();
        // Destroy this object.

    // MANIPULATORS
    baenet_HttpStartLine& operator=(const baenet_HttpStartLine& rhs);
        // Assign to this object the value of the specified 'rhs' object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

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

    baenet_HttpRequestLine& makeRequestLine();
    baenet_HttpRequestLine& makeRequestLine(const baenet_HttpRequestLine& value);
        // Set the value of this object to be a "RequestLine" value. 
        // Optionally specify the 'value' of the "RequestLine".  If 'value' is
        // not specified, the default "RequestLine" value is used.

    baenet_HttpStatusLine& makeStatusLine();
    baenet_HttpStatusLine& makeStatusLine(const baenet_HttpStatusLine& value);
        // Set the value of this object to be a "StatusLine" value.  Optionally
        // specify the 'value' of the "StatusLine".  If 'value' is not
        // specified, the default "StatusLine" value is used.

    template<class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator' on the address of the modifiable
        // selection, supplying 'manipulator' with the corresponding selection
        // information structure.  Return the value returned from the
        // invocation of 'manipulator' if this object has a defined selection,
        // and -1 otherwise.

    baenet_HttpRequestLine& requestLine();
        // Return a reference to the modifiable "RequestLine" selection of this
        // object if "RequestLine" is the current selection.  The behavior is
        // undefined unless "RequestLine" is the selection of this object.

    baenet_HttpStatusLine& statusLine();
        // Return a reference to the modifiable "StatusLine" selection of this
        // object if "StatusLine" is the current selection.  The behavior is
        // undefined unless "StatusLine" is the selection of this object.

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

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    int selectionId() const;
        // Return the id of the current selection if the selection is defined,
        // and -1 otherwise.

    template<class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' on the non-modifiable selection,
        // supplying 'accessor' with the corresponding selection information
        // structure.  Return the value returned from the invocation of
        // 'accessor' if this object has a defined selection, and -1 otherwise.

    const baenet_HttpRequestLine& requestLine() const;
        // Return a reference to the non-modifiable "RequestLine" selection of
        // this object if "RequestLine" is the current selection.  The behavior
        // is undefined unless "RequestLine" is the selection of this object.

    const baenet_HttpStatusLine& statusLine() const;
        // Return a reference to the non-modifiable "StatusLine" selection of
        // this object if "StatusLine" is the current selection.  The behavior
        // is undefined unless "StatusLine" is the selection of this object.

    bool isRequestLineValue() const;
        // Return 'true' if the value of this object is a "RequestLine" value,
        // and return 'false' otherwise.

    bool isStatusLineValue() const;
        // Return 'true' if the value of this object is a "StatusLine" value,
        // and return 'false' otherwise.

    bool isUndefinedValue() const;
        // Return 'true' if the value of this object is undefined, and 'false'
        // otherwise.

    const char *selectionName() const;
        // Return the symbolic name of the current selection of this object.
};

// FREE OPERATORS
inline
bool operator==(const baenet_HttpStartLine& lhs,
                const baenet_HttpStartLine& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'baenet_HttpStartLine' objects have
    // the same value if either the selections in both objects have the same
    // ids and the same values, or both selections are undefined.

inline
bool operator!=(const baenet_HttpStartLine& lhs,
                const baenet_HttpStartLine& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same values, as determined by 'operator==', and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream&                  stream,
                         const baenet_HttpStartLine&    rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.


// TRAITS

BDEAT_DECL_CHOICE_WITH_ALLOCATOR_BITWISEMOVEABLE_TRAITS(baenet_HttpStartLine)

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================


                       // ------------------------------                       
                       // class baenet_HttpStartLine                       
                       // ------------------------------                       

// CLASS METHODS
inline
int baenet_HttpStartLine::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// CREATORS
inline
baenet_HttpStartLine::baenet_HttpStartLine(bslma_Allocator *basicAllocator)
: d_selectionId(SELECTION_ID_UNDEFINED)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

inline
baenet_HttpStartLine::~baenet_HttpStartLine()
{
    reset();
}

// MANIPULATORS
template <class STREAM>
STREAM& baenet_HttpStartLine::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            short selectionId;
            stream.getInt16(selectionId);
            if (!stream) {
                return stream;
            }
            switch (selectionId) {
              case SELECTION_ID_REQUEST_LINE: {
                makeRequestLine();
                bdex_InStreamFunctions::streamIn(
                    stream, d_requestLine.object(), 1);
              } break;
              case SELECTION_ID_STATUS_LINE: {
                makeStatusLine();
                bdex_InStreamFunctions::streamIn(
                    stream, d_statusLine.object(), 1);
              } break;
              case SELECTION_ID_UNDEFINED: {
                reset();
              } break;
              default:
                stream.invalidate();
            }
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

template <class MANIPULATOR>
int baenet_HttpStartLine::manipulateSelection(MANIPULATOR& manipulator)
{
    switch (d_selectionId) {
      case baenet_HttpStartLine::SELECTION_ID_REQUEST_LINE:
        return manipulator(&d_requestLine.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_REQUEST_LINE]);
      case baenet_HttpStartLine::SELECTION_ID_STATUS_LINE:
        return manipulator(&d_statusLine.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_STATUS_LINE]);
      default:
        BSLS_ASSERT(baenet_HttpStartLine::SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
baenet_HttpRequestLine& baenet_HttpStartLine::requestLine()
{
    BSLS_ASSERT(SELECTION_ID_REQUEST_LINE == d_selectionId);
    return d_requestLine.object();
}

inline
baenet_HttpStatusLine& baenet_HttpStartLine::statusLine()
{
    BSLS_ASSERT(SELECTION_ID_STATUS_LINE == d_selectionId);
    return d_statusLine.object();
}

// ACCESSORS
template <class STREAM>
STREAM& baenet_HttpStartLine::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
            stream.putInt16(d_selectionId);
            switch (d_selectionId) {
              case SELECTION_ID_REQUEST_LINE: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_requestLine.object(), 1);
              } break;
              case SELECTION_ID_STATUS_LINE: {
                bdex_OutStreamFunctions::streamOut(
                    stream, d_statusLine.object(), 1);
              } break;
              default:
                BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
            }
      } break;
    }
    return stream;
}

inline
int baenet_HttpStartLine::selectionId() const
{
    return d_selectionId;
}

template <class ACCESSOR>
int baenet_HttpStartLine::accessSelection(ACCESSOR& accessor) const
{
    switch (d_selectionId) {
      case SELECTION_ID_REQUEST_LINE:
        return accessor(d_requestLine.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_REQUEST_LINE]);
      case SELECTION_ID_STATUS_LINE:
        return accessor(d_statusLine.object(),
                SELECTION_INFO_ARRAY[SELECTION_INDEX_STATUS_LINE]);
      default:
        BSLS_ASSERT(SELECTION_ID_UNDEFINED == d_selectionId);
        return -1;
    }
}

inline
const baenet_HttpRequestLine& baenet_HttpStartLine::requestLine() const
{
    BSLS_ASSERT(SELECTION_ID_REQUEST_LINE == d_selectionId);
    return d_requestLine.object();
}

inline
const baenet_HttpStatusLine& baenet_HttpStartLine::statusLine() const
{
    BSLS_ASSERT(SELECTION_ID_STATUS_LINE == d_selectionId);
    return d_statusLine.object();
}

inline
bool baenet_HttpStartLine::isRequestLineValue() const
{
    return SELECTION_ID_REQUEST_LINE == d_selectionId;
}

inline
bool baenet_HttpStartLine::isStatusLineValue() const
{
    return SELECTION_ID_STATUS_LINE == d_selectionId;
}

inline
bool baenet_HttpStartLine::isUndefinedValue() const
{
    return SELECTION_ID_UNDEFINED == d_selectionId;
}

// FREE FUNCTIONS

inline
bool operator==(
        const baenet_HttpStartLine& lhs,
        const baenet_HttpStartLine& rhs)
{
    typedef baenet_HttpStartLine Class;
    if (lhs.selectionId() == rhs.selectionId()) {
        switch (rhs.selectionId()) {
          case Class::SELECTION_ID_REQUEST_LINE:
            return lhs.requestLine() == rhs.requestLine();
          case Class::SELECTION_ID_STATUS_LINE:
            return lhs.statusLine() == rhs.statusLine();
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
bool operator!=(
        const baenet_HttpStartLine& lhs,
        const baenet_HttpStartLine& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(
        bsl::ostream& stream,
        const baenet_HttpStartLine& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP
#endif

// GENERATED BY BLP_BAS_CODEGEN_3.4.4 Wed Feb 10 17:14:02 2010
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
