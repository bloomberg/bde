// baenet_httprequestheader.h   -*-C++-*-
#ifndef INCLUDED_BAENET_HTTPREQUESTHEADER
#define INCLUDED_BAENET_HTTPREQUESTHEADER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")



//@PURPOSE: Provide a value-semantic HTTP request header.
//
//@CLASSES: baenet_HttpRequestHeader: value-semantic HTTP request header
//
//@SEE_ALSO: baenet_httpresponseheader
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION:  This component provides an implementation of the
// 'baenet_HttpHeader' protocol for a value-semantic HTTP request header.
// This header contains all the request header fields defined in RFC2616.
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.
// Suppose we wanted to print the values of each field in the header.  We can
// use the following accessor:
//..
//  struct PrintField {
//      template <typename TYPE>
//      int operator()(const TYPE&  fieldValue,
//                     const char  *fieldName,
//                     int          fieldNameLength)
//      {
//          bsl::cout << bdeut_StringRef(fieldName, fieldNameLength)
//                    << ": " << fieldValue << bsl::endl;
//          return 0;
//      }
//  }
//..
// This accessor can be used as follows:
//..
//  PrintField printField;
//
//  requestHeader.accessFields(printField);
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAENET_HTTPHEADER
#include <baenet_httpheader.h>
#endif

#ifndef INCLUDED_BAENET_HTTPBASICHEADERFIELDS
#include <baenet_httpbasicheaderfields.h>
#endif

#ifndef INCLUDED_BAENET_HTTPREQUESTHEADERFIELDS
#include <baenet_httprequestheaderfields.h>
#endif

#ifndef INCLUDED_BAENET_HTTPEXTENDEDHEADERFIELDS
#include <baenet_httpextendedheaderfields.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_ATTRIBUTEINFO
#include <bdeat_attributeinfo.h>
#endif

#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#include <bdeat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

struct baenet_HttpRequestHeader_Trait : bslalg_TypeTraitUsesBslmaAllocator,
                                        bdeu_TypeTraitHasPrintMethod { };

class baenet_HttpRequestHeader : public baenet_HttpHeader {
    // HTTP request header.

    // PRIVATE DATA MEMBERS
    baenet_HttpBasicHeaderFields    d_basicFields;    // basic fields
    baenet_HttpRequestHeaderFields  d_requestFields;  // request fields
    baenet_HttpExtendedHeaderFields d_extendedFields; // extended fields

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(baenet_HttpRequestHeader,
                                 baenet_HttpRequestHeader_Trait);

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    // CREATORS
    explicit baenet_HttpRequestHeader(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'baenet_HttpRequestHeader' having the
        // default value.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    baenet_HttpRequestHeader(
                          const baenet_HttpRequestHeader&  original,
                          bslma_Allocator                 *basicAllocator = 0);
        // Create an object of type 'baenet_HttpRequestHeader' having the value
        // of the specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    virtual ~baenet_HttpRequestHeader();
        // Destroy this object.

    // MANIPULATORS
    baenet_HttpRequestHeader& operator=(const baenet_HttpRequestHeader& rhs);
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
        // Reset this object to the default value (i.e., its value upon
        // default construction).

    baenet_HttpBasicHeaderFields& basicFields();
        // Return a reference to the modifiable "BasicFields" attribute of this
        // object.

    baenet_HttpRequestHeaderFields& requestFields();
        // Return a reference to the modifiable "RequestFields" attribute of
        // this object.

    baenet_HttpExtendedHeaderFields& extendedFields();
        // Return a reference to the modifiable "ExtendedFields" attribute of
        // this object.

    // ACCESSORS
    template <typename ACCESSOR>
    int accessFields(ACCESSOR& accessor) const;
        // Invoke the specified 'accessor' sequentially on each
        // (non-modifiable) attribute of this object, supplying 'accessor' with
        // the corresponding attribute information structure or field name and
        // length until such invocation returns a non-zero value.  Return the
        // value from the last invocation of 'accessor' (i.e., the invocation
        // that terminated the sequence).

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

    const baenet_HttpRequestHeaderFields& requestFields() const;
        // Return a reference to the non-modifiable "RequestFields"
        // attribute of this object.

    const baenet_HttpExtendedHeaderFields& extendedFields() const;
        // Return a reference to the non-modifiable "ExtendedFields"
        // attribute of this object.

    // MANIPULATORS (FROM 'baenet_HttpHeader')
    virtual int addField(const bdeut_StringRef& fieldName,
                         const bdeut_StringRef& fieldValue);
        // Add the specified 'fieldValue' to the field with the specified
        // 'fieldName'.  Return 0 on success, and a non-zero value otherwise.

    // ACCESSORS (FROM 'baenet_HttpHeader')
    virtual const baenet_HttpBasicHeaderFields& basicFields() const;
        // Return a reference to the non-modifiable basic header fields.
};

// FREE OPERATORS
inline
bool operator==(const baenet_HttpRequestHeader& lhs,
                const baenet_HttpRequestHeader& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.  Two attribute objects have the
    // same value if each respective attribute has the same value.

inline
bool operator!=(const baenet_HttpRequestHeader& lhs,
                const baenet_HttpRequestHeader& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.  Two attribute objects do
    // not have the same value if one or more respective attributes differ in
    // values.

inline
bsl::ostream& operator<<(bsl::ostream&                   stream,
                         const baenet_HttpRequestHeader& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

              // ===============================================
              // class baenet_HttpRequestHeader_AccessFieldProxy
              // ===============================================

template <typename ACCESSOR>
class baenet_HttpRequestHeader_AccessFieldProxy {
    // Proxy class to filter out INFO_TYPE and adding field name & field name
    // length, before passing the the field accessor.

    // PRIVATE DATA MEMBERS
    ACCESSOR& d_accessor;

    // NOT IMPLEMENTED
    baenet_HttpRequestHeader_AccessFieldProxy(
                             const baenet_HttpRequestHeader_AccessFieldProxy&);
    baenet_HttpRequestHeader_AccessFieldProxy& operator=(
                             const baenet_HttpRequestHeader_AccessFieldProxy&);

  public:
    // CREATORS
    explicit baenet_HttpRequestHeader_AccessFieldProxy(ACCESSOR& accessor)
    : d_accessor(accessor)
    {
    }

    // ~baenet_HttpRequestHeader_AccessFieldProxy()
        // Destroy this object.  Note that this trivial destructor is generated
        // by the compiler.

    // MANIPULATORS
    template <typename TYPE>
    int operator()(const TYPE&  object,
                   const char  *fieldName,
                   int          fieldNameLength)
    {
        return d_accessor(object, fieldName, fieldNameLength);
    }

    template <typename TYPE, typename INFO_TYPE>
    int operator()(const TYPE& object, const INFO_TYPE& info)
    {
        return d_accessor(object, info.name(), info.nameLength());
    }
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// The following inlined functions are invoked from other inline functions.

inline
int baenet_HttpRequestHeader::maxSupportedBdexVersion()
{
    return 1;  // versions start at 1.
}

// ---------------------------------------------------------------------------

// CREATORS
inline
baenet_HttpRequestHeader::baenet_HttpRequestHeader(
                                               bslma_Allocator *basicAllocator)
: d_basicFields(bslma_Default::allocator(basicAllocator))
, d_requestFields(bslma_Default::allocator(basicAllocator))
, d_extendedFields(bslma_Default::allocator(basicAllocator))
{
}

inline
baenet_HttpRequestHeader::baenet_HttpRequestHeader(
        const baenet_HttpRequestHeader& original,
        bslma_Allocator *basicAllocator)
: d_basicFields(original.d_basicFields,
                bslma_Default::allocator(basicAllocator))
, d_requestFields(original.d_requestFields,
                  bslma_Default::allocator(basicAllocator))
, d_extendedFields(original.d_extendedFields,
                   bslma_Default::allocator(basicAllocator))
{
}

inline
baenet_HttpRequestHeader::~baenet_HttpRequestHeader()
{
}

// MANIPULATORS
inline
baenet_HttpRequestHeader&
baenet_HttpRequestHeader::operator=(const baenet_HttpRequestHeader& rhs)
{
    if (this != &rhs) {
        d_basicFields = rhs.d_basicFields;
        d_requestFields = rhs.d_requestFields;
        d_extendedFields = rhs.d_extendedFields;
    }
    return *this;
}

template <class STREAM>
STREAM& baenet_HttpRequestHeader::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdex_InStreamFunctions::streamIn(
                  stream, d_basicFields, 1);
            bdex_InStreamFunctions::streamIn(
                  stream, d_requestFields, 1);
            bdex_InStreamFunctions::streamIn(
                  stream, d_extendedFields, 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void baenet_HttpRequestHeader::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_basicFields);
    bdeat_ValueTypeFunctions::reset(&d_requestFields);
    bdeat_ValueTypeFunctions::reset(&d_extendedFields);
}

inline
baenet_HttpBasicHeaderFields& baenet_HttpRequestHeader::basicFields()
{
    return d_basicFields;
}

inline
baenet_HttpRequestHeaderFields& baenet_HttpRequestHeader::requestFields()
{
    return d_requestFields;
}

inline
baenet_HttpExtendedHeaderFields& baenet_HttpRequestHeader::extendedFields()
{
    return d_extendedFields;
}

// ACCESSORS
template <typename ACCESSOR>
int baenet_HttpRequestHeader::accessFields(ACCESSOR& accessor) const
{
    int ret;

    baenet_HttpRequestHeader_AccessFieldProxy<ACCESSOR> proxy(accessor);

    ret = d_basicFields.accessAttributes(proxy);
    if (0 != ret) {
        return ret;
    }

    ret = d_requestFields.accessAttributes(proxy);
    if (0 != ret) {
        return ret;
    }

    typedef baenet_HttpExtendedHeaderFields::ConstFieldIterator
                                                       ConstFieldIterator;
    typedef baenet_HttpExtendedHeaderFields::ConstFieldValueIterator
                                                       ConstFieldValueIterator;

    for (ConstFieldIterator itf  = d_extendedFields.beginFields();
                            itf != d_extendedFields.endFields();
                          ++itf) {
        const char *fieldName       = itf->first.data();
        const int   fieldNameLength = itf->first.length();

        for (ConstFieldValueIterator itv  = itf->second.begin();
                                     itv != itf->second.end();
                                   ++itv) {
            ret = proxy(*itv, fieldName, fieldNameLength);
            if (0 != ret) {
                return ret;
            }
        }
    }

    return 0;
}

template <class STREAM>
STREAM& baenet_HttpRequestHeader::bdexStreamOut(STREAM& stream,
                                                int     version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream, d_basicFields, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_requestFields, 1);
        bdex_OutStreamFunctions::streamOut(stream, d_extendedFields, 1);
      } break;
      default: {
        enum { UNSUPPORTED_BDEX_VERSION };
        BSLS_ASSERT_SAFE(UNSUPPORTED_BDEX_VERSION);
        stream.invalidate();
      } break;
    }
    return stream;
}

inline
const baenet_HttpRequestHeaderFields&
baenet_HttpRequestHeader::requestFields() const
{
    return d_requestFields;
}

inline
const baenet_HttpExtendedHeaderFields&
baenet_HttpRequestHeader::extendedFields() const
{
    return d_extendedFields;
}

// ACCESSORS (FROM 'baenet_HttpHeader')

inline
const baenet_HttpBasicHeaderFields&
baenet_HttpRequestHeader::basicFields() const
{
    return d_basicFields;
}

// FREE OPERATORS
inline
bool operator==(const baenet_HttpRequestHeader& lhs,
                const baenet_HttpRequestHeader& rhs)
{
    return  lhs.basicFields()    == rhs.basicFields()
         && lhs.requestFields()  == rhs.requestFields()
         && lhs.extendedFields() == rhs.extendedFields();
}

inline
bool operator!=(const baenet_HttpRequestHeader& lhs,
                const baenet_HttpRequestHeader& rhs)
{
    return  lhs.basicFields()    != rhs.basicFields()
         || lhs.requestFields()  != rhs.requestFields()
         || lhs.extendedFields() != rhs.extendedFields();
}

inline
bsl::ostream& operator<<(bsl::ostream&                   stream,
                         const baenet_HttpRequestHeader& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP;

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
