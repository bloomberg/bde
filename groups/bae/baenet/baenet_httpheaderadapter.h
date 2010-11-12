// baenet_httpheaderadapter.h   -*-C++-*-
#ifndef INCLUDED_BAENET_HTTPHEADERADAPTER
#define INCLUDED_BAENET_HTTPHEADERADAPTER

//@PURPOSE: Provide a generic implementation of 'baenet_HttpHeader' protocol.
//
//@CLASSES:
//  baenet_HttpHeaderAdapter: generic 'baenet_HttpHeader' implementation
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@SEE_ALSO: baenet_httpheader
//
//@DESCRIPTION:
//  TBD: component level doc
//
///Usage
///-----
// TBD: usage example
// DEPRECATED: DO NOT USE!!!!

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAENET_HTTPHEADER
#include <baenet_httpheader.h>
#endif

#ifndef INCLUDED_BAENET_HTTPPARSERUTIL
#include <baenet_httpparserutil.h>
#endif

#ifndef INCLUDED_BDEALG_TYPETRAITS
#include <bdealg_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_ARRAYFUNCTIONS
#include <bdeat_arrayfunctions.h>
#endif

#ifndef INCLUDED_BDEAT_NULLABLEVALUEFUNCTIONS
#include <bdeat_nullablevaluefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_TYPECATEGORY
#include <bdeat_typecategory.h>
#endif

#ifndef INCLUDED_BDEUT_STRINGREF
#include <bdeut_stringref.h>
#endif

#ifndef INCLUDED_BDEMF_IF
#include <bdemf_if.h>
#endif

namespace BloombergLP {

class bdema_Allocator;
class baenet_HttpBasicHeaderFields;

                    // ====================================
                    // class baenet_HttpHeaderAdapter<TYPE>
                    // ====================================

template <typename TYPE>
class baenet_HttpHeaderAdapter : public baenet_HttpHeader {
    // This template implements the 'baenet_HttpHeader' protocol for a header
    // of the parameterized 'TYPE'.
    //
    // TBD: doc the concept for 'TYPE'
    // t.manipulateNewField
    // t.basicFields
    // Default ctor

    // PRIVATE TYPES
    typedef typename
    bdemf_If<bdealg_HasTrait<TYPE, bdealg_TypeTraitUsesBdemaAllocator>::VALUE,
             bdealg_TypeTraitUsesBdemaAllocator,
             bdemf_Nil>::Type PassThroughTrait;

    // PRIVATE DATA MEMBERS
    TYPE d_header;  // header object

  private:
    // NOT IMPLEMENTED
    baenet_HttpHeaderAdapter(const baenet_HttpHeaderAdapter&);
    baenet_HttpHeaderAdapter& operator=(const baenet_HttpHeaderAdapter&);

  public:
    // TRAITS
    BDEALG_DECLARE_NESTED_TRAITS(baenet_HttpHeaderAdapter, PassThroughTrait);

    // CREATORS
    baenet_HttpHeaderAdapter();
        // Construct a header adapter object.  If the parameterized 'TYPE' uses
        // 'bdema' allocators, then the currently installed default allocator
        // will be used.

    explicit baenet_HttpHeaderAdapter(bdema_Allocator *basicAllocator);
        // Construct a header adapter object using the specified
        // 'basicAllocator' to supply memory.  Note that this constructor will
        // fail to compile unless the parameterized 'TYPE' uses 'bdema'
        // allocators.  If 'basicAllocator' is 0, then the currently installed
        // default allocator will be used.

    virtual ~baenet_HttpHeaderAdapter();
        // Destroy this object.

    // MANIPULATORS
    virtual int addField(const bdeut_StringRef& fieldName,
                         const bdeut_StringRef& fieldValue);
        // Add the specified 'fieldValue' to the field with the specified
        // 'fieldName'.  Return 0 on success, and a non-zero value otherwise.

    TYPE& header();
        // Return a reference to the modifiable header object.

    // ACCESSORS
    virtual const baenet_HttpBasicHeaderFields& basicFields() const;
        // Return a reference to the non-modifiable basic header fields.

    const TYPE& header() const;
        // Return a reference to the non-modifiable header object.
};

// ---  Anything below this line is implementation specific.  Do not use.  ----

                  // =======================================
                  // class baenet_HttpHeaderAdapter_AddField
                  // =======================================

class baenet_HttpHeaderAdapter_AddField {
    // This visitor will add a value to the visited field.

    // PRIVATE DATA MEMBERS
    bool                   d_failed;        // set to true if parsing failed
    const bdeut_StringRef *d_fieldValue_p;  // held, not owned

    // PRIVATE MANIPULATORS
    template <typename TYPE>
    int appendElementToArray(TYPE                   *object,
                             const bdeut_StringRef&  str);
        // Append, to the specified 'object' array, an element obtained by
        // parsing the specified 'str'.  Return 0 on success, and a non-zero
        // value otherwise.

    template <typename TYPE>
    int execute(TYPE *object, bdeat_TypeCategory::Array);
    template <typename TYPE>
    int execute(TYPE *object, bdeat_TypeCategory::NullableValue);
    template <typename TYPE, typename ANY_CATEGORY>
    int execute(TYPE *object, ANY_CATEGORY);
        // Add the associated field value to the specified 'object'.  Return 0
        // on success, and a non-zero value otherwise.  Note that the second
        // argument indicates the type category of the parameterized 'TYPE',
        // and is used for overloading purposes.

  private:
    // NOT IMPLEMENTED
    baenet_HttpHeaderAdapter_AddField(
                                     const baenet_HttpHeaderAdapter_AddField&);
    baenet_HttpHeaderAdapter_AddField& operator=(
                                     const baenet_HttpHeaderAdapter_AddField&);

  public:
    // CREATORS
    explicit baenet_HttpHeaderAdapter_AddField(
                                            const bdeut_StringRef *fieldValue);
        // Construct a visitor that will add the specified 'fieldValue' to the
        // visited field.

    ~baenet_HttpHeaderAdapter_AddField();
        // Destroy this object.

    // MANIPULATORS
    template <typename TYPE>
    int operator()(TYPE *object);
    template <typename TYPE, typename INFO_TYPE>
    int operator()(TYPE *object, const INFO_TYPE&);
        // Add the associated field value to the specified object.  Return 0 on
        // success, and a non-zero value otherwise.  Note that the second
        // argument, if specified, is ignored.

    // ACCESSORS
    bool failed() const;
        // Return true if parsing failed.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                    // ------------------------------------
                    // class baenet_HttpHeaderAdapter<TYPE>
                    // ------------------------------------

// CREATORS

template <typename TYPE>
inline
baenet_HttpHeaderAdapter<TYPE>::baenet_HttpHeaderAdapter()
{
}

template <typename TYPE>
inline
baenet_HttpHeaderAdapter<TYPE>::baenet_HttpHeaderAdapter(
                                               bdema_Allocator *basicAllocator)
: d_header(basicAllocator)
{
}

template <typename TYPE>
inline
baenet_HttpHeaderAdapter<TYPE>::~baenet_HttpHeaderAdapter()
{
}

// MANIPULATORS

template <typename TYPE>
// virtual
int baenet_HttpHeaderAdapter<TYPE>::addField(const bdeut_StringRef& fieldName,
                                             const bdeut_StringRef& fieldValue)
{
    enum { SUCCESS = 0, FAILURE = -1, IGNORED = 0 };

    baenet_HttpHeaderAdapter_AddField visitor(&fieldValue);

    if (0 != d_header.manipulateNewFieldValue(visitor,
                                              fieldName.data(),
                                              fieldName.length())) {
        if (visitor.failed()) {
            return FAILURE;
        }

        return IGNORED;
    }

    return SUCCESS;
}

template <typename TYPE>
inline
TYPE& baenet_HttpHeaderAdapter<TYPE>::header()
{
    return d_header;
}

// ACCESSORS

template <typename TYPE>
// virtual
const baenet_HttpBasicHeaderFields&
baenet_HttpHeaderAdapter<TYPE>::basicFields() const
{
    return d_header.basicFields();
}

template <typename TYPE>
inline
const TYPE& baenet_HttpHeaderAdapter<TYPE>::header() const
{
    return d_header;
}

                  // ---------------------------------------
                  // class baenet_HttpHeaderAdapter_AddField
                  // ---------------------------------------

// PRIVATE MANIPULATORS

template <typename TYPE>
inline
int baenet_HttpHeaderAdapter_AddField::appendElementToArray(
                                                TYPE                   *object,
                                                const bdeut_StringRef&  str)
{
    int i = bdeat_ArrayFunctions::size(*object);

    bdeat_ArrayFunctions::resize(object, i + 1);

    baenet_HttpHeaderAdapter_AddField loadElement(&str);

    return bdeat_ArrayFunctions::manipulateElement(object, loadElement, i);
}

template <typename TYPE>
int baenet_HttpHeaderAdapter_AddField::execute(TYPE *object,
                                               bdeat_TypeCategory::Array)
{
    enum { FAILURE = -1 };

    const char *begin = d_fieldValue_p->begin();
    const char *p     = begin;
    const char *end   = d_fieldValue_p->end();

    while (p != end) {
        if ('(' == *p) {
            baenet_HttpParserUtil::skipCommentsAndFoldedWhitespace(&p, end);
        }
        else if ('\"' == *p) {
            baenet_HttpParserUtil::skipQuotedString(&p, end);
        }
        else if ('[' == *p) {
            baenet_HttpParserUtil::skipDomainLiteral(&p, end);
        }
        else if (',' == *p) {
            bdeut_StringRef element(begin, p);

            if (0 != appendElementToArray(object, element)) {
                return FAILURE;
            }

            ++p;
            begin = p;
        }
        else {
            ++p;
        }
    }

    bdeut_StringRef element(begin, p);

    return appendElementToArray(object, element);
}

template <typename TYPE>
inline
int baenet_HttpHeaderAdapter_AddField::execute(
                                             TYPE *object,
                                             bdeat_TypeCategory::NullableValue)
{
    bdeat_NullableValueFunctions::makeValue(object);
    return bdeat_NullableValueFunctions::manipulateValue(object, *this);
}

template <typename TYPE, typename ANY_CATEGORY>
inline
int baenet_HttpHeaderAdapter_AddField::execute(TYPE *object, ANY_CATEGORY)
{
    return baenet_HttpParserUtil::parseFieldValue(object, *d_fieldValue_p);
}

// CREATORS

inline
baenet_HttpHeaderAdapter_AddField::baenet_HttpHeaderAdapter_AddField(
                                             const bdeut_StringRef *fieldValue)
: d_failed(false)
, d_fieldValue_p(fieldValue)
{
}

inline
baenet_HttpHeaderAdapter_AddField::~baenet_HttpHeaderAdapter_AddField()
{
}

// MANIPULATORS

template <typename TYPE>
inline
int baenet_HttpHeaderAdapter_AddField::operator()(TYPE *object)
{
    typedef typename
    bdeat_TypeCategory::Select<TYPE>::Type TypeCategory;

    int ret = execute(object, TypeCategory());

    if (0 != ret) {
        d_failed = true;
    }

    return ret;
}

template <typename TYPE, typename INFO_TYPE>
inline
int baenet_HttpHeaderAdapter_AddField::operator()(TYPE *object,
                                                  const INFO_TYPE&)
{
    return (*this)(object);
}

// ACCESSORS

inline
bool baenet_HttpHeaderAdapter_AddField::failed() const
{
    return d_failed;
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
