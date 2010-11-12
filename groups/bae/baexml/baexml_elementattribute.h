// baexml_elementattribute.h                  -*-C++-*-
#ifndef INCLUDED_BAEXML_ELEMENTATTRIBUTE
#define INCLUDED_BAEXML_ELEMENTATTRIBUTE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide the properties of an attribute in an XML element tag
//
//@CLASSES:
// baexml_ElementAttribute: Properties of an attribute in an XML element tag.
//
//@SEE_ALSO: baexml_reader, baexml_prefixstack
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component provides a class, 'baexml_ElementAttribute',
// that encapsulates the name, namespace, and value of an attribute within an
// element in an XML document, along with a flag indicating whether the value
// comes from the default value in the document's schema.  There are accessors
// to return the attribute name as a qualified name, namespace prefix,
// namespace URI, namespace ID (in a namespace registry), and local name
// (without the prefix).
//
// If a namespace prefix stack and a qualified name are provided at
// construction (or via the 'reset' method), then all of the other facets of
// the name can be computed automatically.  For efficiency reasons, these
// various decompositions of the attribute name are computed only when needed
// and cached within the 'baexml_ElementAttribute' structure.  The caller can
// supply any or all of these computed facets in the constructor or 'reset'
// method.  Explicitly supplying a facet prevents it from being computed, thus
// saving processing time.  If the object is constructed with explicit values
// for all facets of the name, then the prefix stack is never used and may be
// null.  If an explicitly-supplied facet differs from the value that would
// have been computed otherwise, then the facets returned by the accessors
// will be inconsistent with one-another.  This inconsistency is deliberately
// permitted so that an program may construct a 'baexml_ElementAttribute'
// even when some parts of of the name are not yet known, e.g., if the prefix
// URI is known but the qualified name is not.  If the qualified name facet is
// zero, then no other facets will be computed -- their original values will
// be returned from the accessors.
//
///Not Value Semantic
///------------------
// 'baexml_ElementAttribute' is not a value-semantic class.  It provides copy
// construction and assignment, but does not provide equality or persistence
// operators.  Copying a 'baexml_ElementAttribute' object copies all of its
// pointer facets but does not make copies of the pointed-to strings or
// objects.  The presence of copy operations makes it possible to use
// 'baexml_ElementAttribute' with container class templates.
//
// A 'baexml_ElementAttribute' object does not own any of its pointer facets
// and it performs no memory management.  This means that a
// 'baexml_ElementAttribute' object can be invalidated by modifying or
// deleting any of the pointed-to entities.  Calling an accessor on an invalid
// 'baexml_ElementAttribute' is unsafe because the accessor may return an
// invalid pointer.  However an invalid object may be safely destroyed or
// reset.  Components that set or return a 'baexml_ElementAttribute' object
// should provide clear documentation describing the events that will cause
// the resulting 'baexml_ElementAttribute' object to become invalid (see
// 'baexml_Reader').
//
///Thread Safety
///-------------
// It is safe to access or modify two 'baexml_ElementAttribute' objects
// simultaneously, each from a separate thread.  It is safe to access a single
// 'baexml_ElementAttribute' object simultaneously from two or more separate
// threads, provided no other thread is simultaneously modifying the object.
// It is not safe to access or modify a 'baexml_ElementAttribute' object in
// one thread while another thread modifies the same object.
//
// A 'baexml_ElementAttribute' object holds pointers to objects that it does
// not own and which can be modified independently of the
// 'baexml_ElementAttribute' object.  It is not safe to modify or delete any
// of these object in one thread while accessing or modifying the
// 'baexml_ElementAttribute' object in another thread.
//
///Usage
///-----
// The following function parses an XML-style attribute assignment in the form
// "qname='value'", where 'qname' can be a qualified name in the form,
// "prefix:localName".  The prefix (if any) must be registered in the specified
// 'prefixStack' object.  Either single or double quotes may be used to
// enclose the attribute value.  The parsed attribute is stored in the
// specified 'attribute' object of type 'baexml_ElementAttribute'.  Note that
// this function modifies the input string by inserting null characters,
// rather than copying the component parts into allocated memory.  This is a
// realistic interface for a function used within an XML parser that has
// already copied the XML stream into allocated memory.  Also note that this
// function does not interpret character escapes such as "&amp;".
//..
//  int parseAttribute(baexml_ElementAttribute  *attribute,
//                     char                     *attributeString,
//                     const baexml_PrefixStack *prefixStack)
//  {
//..
// First, find the end of the qualified name, i.e., the '=' character:
//..
//      char *qname    = attributeString;
//      char *equalPtr = bsl::strchr(qname, '=');
//      if (0 == equalPtr) {
//          return -1;
//      }
//..
// Then find out which quote character is used to start the value string
//..
//      char quote = *(equalPtr + 1);
//      if (quote != '\'' && quote != '"') {
//          return -1;
//      }
//..
// The value string starts after the opening quote and extends until a
// matching quote:
//..
//      char *value    = equalPtr + 2;
//      char *endValue = bsl::strchr(value, quote);
//      if (0 == endValue) {
//          return -1;
//      }
//..
// Once we have successfully parsed the string, chop it into pieces by putting
// a null terminator at the end of the qualified name and at the end of the
// value string:
//..
//      *equalPtr = '\0';    // Terminate qualified name
//      *endValue = '\0';    // Terminate value string
//..
// Use the prefix stack, qname, and value to set the attribute object.  All
// other arguments are defaulted and will be computed as needed.
//..
//      attribute->reset(prefixStack, qname, value);
//      return 0;
//  }
//..
// Before calling the 'parseAttribute' function, it is necessary to create a
// namespace registry and prefix stack, as well as to register one or more
// prefixes:
//..
//  int main()
//  {
//      baexml_NamespaceRegistry registry;
//      baexml_PrefixStack prefixes(&registry);
//      int cal = prefixes.pushPrefix("cal",
//                                "http://www.bloomberg.com/schemas/calendar");
//      int math = prefixes.pushPrefix("math",
//                                    "http://www.bloomberg.com/schemas/math");
//      int app = prefixes.pushPrefix("",  // Default namespace
//                                    "http://www.bloomberg.com/schemas/app");
//..
// Now we can parse an attribute string and the 'baexml_ElementAttribute'
// object will provide detailed information about it.
//..
//      char attrStr1[] = "cal:date='12-07-2006'";
//      baexml_ElementAttribute attribute1;
//      int ret = parseAttribute(&attribute1, attrStr1, &prefixes);
//
//      assert(0 == bsl::strcmp("cal:date", attribute1.qualifiedName()));
//      assert(0 == bsl::strcmp("12-07-2006", attribute1.value()));
//      assert(0 == bsl::strcmp("date", attribute1.localName()));
//      assert(0 == bsl::strcmp("cal", attribute1.prefix()));
//      assert(cal == attribute1.namespaceId());
//      assert(0 == bsl::strcmp("http://www.bloomberg.com/schemas/calendar",
//                              attribute1.namespaceUri()));
//      assert(0 == attribute1.flags());
//..
// Results are slightly different when the attribute name has no prefix:
//..
//      char attrStr2[] = "name=\"Bloomberg, L.P.\"";
//      baexml_ElementAttribute attribute2;
//      ret = parseAttribute(&attribute2, attrStr2, &prefixes);
//
//      assert(0 == bsl::strcmp("name", attribute2.qualifiedName()));
//      assert(0 == bsl::strcmp("Bloomberg, L.P.", attribute2.value()));
//      assert(0 == bsl::strcmp("name", attribute2.localName()));
//..
// As per the XML namespace standard, an attribute with no namespace prefix
// does NOT inherit the default namespace but rather has NO namespace:
//..
//      assert(0 == bsl::strcmp("", attribute2.prefix()));
//      assert(-1 == attribute2.namespaceId());
//      assert(0 == bsl::strcmp("", attribute2.namespaceUri()));
//      assert(0 == attribute2.flags());
//..
// A 'baexml_ElementAttribute' does not need to be generated by parsing XML
// code.  If a specific facet of the object is set to a non-null value, then
// that value will be returned by the corresponding accessor even if it is
// inconsistent with the other values in the object.  For example, the
// following constructs a valid 'baexml_ElementAttribute' object, even though
// the prefix value does not agree with the qualified name:
//..
//      baexml_ElementAttribute attribute3(&prefixes,
//                                         "math:product", "4.5", "cal");
//..
// There is no consistency checking, and the mismatched prefix is simply
// returned by the accessor:
//..
//      assert(0 == bsl::strcmp("math:product", attribute3.qualifiedName()));
//      assert(0 == bsl::strcmp("product", attribute3.localName()));
//      assert(0 == bsl::strcmp("cal", attribute3.prefix()));
//..
// Note that the ability to create inconsistent objects is a deliberate
// feature.  It allows parsers to construct 'baexml_Attribute' objects before
// all information is known, e.g., before the namespace is registered with the
// prefix stack.  Consistency checking also reduces performance.
//..
//      return 0;
//  }
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSL_CLIMITS
#include <bsl_climits.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

namespace BloombergLP {

class baexml_PrefixStack;

                        // =============================
                        // class baexml_ElementAttribute
                        // =============================

class baexml_ElementAttribute {
    // Class to represent the properties of an attribute in an XML element
    // tag.  Note that this class is not value semantic and does not own any
    // of its pointer values.  The owner of the arguments used to set the
    // value of a 'baexml_ElementAttribute' is responsible for ensuring that
    // the values remain valid or else must document the conditions that will
    // make the values invalid.  Some facets are computed the first time that
    // they are needed.  To avoid extra processing, the caller may supply
    // otherwise-computed facets at construction or by calling 'reset'.
    // Facets provided by the caller are not checked to ensure that they are
    // consistent with one another.

    // MEMBER VARIABLES
    // mutable members may be used to cache computed results.
    const baexml_PrefixStack *d_prefixStack;   // Held, not owned
    const char               *d_qualifiedName; // Held, not owned
    const char               *d_value;         // Held, not owned
    mutable const char       *d_prefix;        // Held, not owned
    mutable const char       *d_localName;     // Held, not owned
    mutable int               d_namespaceId;
    mutable const char       *d_namespaceUri;  // Held, not owned
    unsigned                  d_flags;

  public:
    // FLAGS
    // The flags property should be set to the bitwise-OR of one or more of
    // the following values:
    enum {
        BAEXML_ATTR_NO_FLAGS   = 0x0000,
            // No flags set.

        BAEXML_ATTR_IS_DEFAULT = 0x0001,
            // Set this bit if the value for this attribute object was
            // generated from the default attribute value in the DTD or
            // schema.

        BAEXML_ATTR_IS_NSDECL  = 0x0002
            // Set this bit if this attribute represents a namespace
            // declaration.
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , ATTR_NO_FLAGS   = BAEXML_ATTR_NO_FLAGS
      , ATTR_IS_DEFAULT = BAEXML_ATTR_IS_DEFAULT
      , ATTR_IS_NSDECL  = BAEXML_ATTR_IS_NSDECL
#endif
    };

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(baexml_PrefixStack,
                                 bslalg_TypeTraitBitwiseCopyable);

    // PUBLIC CREATORS
    baexml_ElementAttribute();
        // Construct a null attribute object: Set all string facets to zero,
        // the namespace ID to 'INT_MIN', and flags to zero.

    baexml_ElementAttribute(const baexml_PrefixStack *prefixStack,
                            const char               *qualifiedName,
                            const char               *value,
                            const char               *prefix = 0,
                            const char               *localName = 0,
                            int                       namespaceId = INT_MIN,
                            const char               *namespaceUri = 0,
                            unsigned                  flags = 0);
        // Construct an attribute object with the specified, 'prefixStack',
        // 'qualifiedName', and 'value', with optionally specified 'prefix',
        // 'localName', 'namespaceId', 'namespaceUri', and 'flags'.  Except
        // for 'flags', if any of the optional arguments are null (for
        // pointers) or 'INT_MIN' (for integer arguments), then the
        // corresponding facet will computed from the other arguments on an
        // as-needed basis.  If all of the optional arguments are given
        // non-null, non-'INT_MIN' values, then 'prefixStack' is unused and
        // may be null.  Arguments are permitted to be inconsistent with one
        // another (e.g., 'prefix' may not match the beginning of
        // 'qualifiedName') and will produce an object for with facets that
        // are inconsistent with one another.  The constructed object will
        // become invalid if any of the supplied pointers is invalidated
        // during its lifetime (or before it is reset).  An invalid object may
        // be destroyed or reset, but any other access yields undefined
        // behavior.

#ifdef DOXYGEN
    // For efficiency, use compiler-generated copy constructor, destructor,
    // and assignment, but document them as though they were explicitly
    // declared:

    baexml_ElementAttribute(const baexml_ElementAttribute& other);
        // Construct a copy of the specified 'other' attribute object.  The
        // copy will have pointers to the same strings and prefix table as
        // 'other'.

    ~baexml_ElementAttribute();
        // Destroy this attribute object.  The targets of the pointers used to
        // construct or reset this object are not owned by this object and are
        // not deallocated.  It is safe to destroy a 'baexml_ElementAttribute'
        // object that is in an invalid state.

    baexml_ElementAttribute operator=(const baexml_ElementAttribute& rhs);
        // Assign this attribute object the value of 'rhs'.  This object will
        // have pointers to the same strings and prefix table as 'other'.
#endif

    void reset();
        // Reset this object to the default-constructed state.

    void reset(const baexml_PrefixStack *prefixStack,
               const char               *qualifiedName,
               const char               *value,
               const char               *prefix = 0,
               const char               *localName = 0,
               int                       namespaceId = INT_MIN,
               const char               *namespaceUri = 0,
               unsigned                  flags = 0);
        // Reset this attribute object with the specified, 'prefixStack',
        // 'qualifiedName', and 'value', with optionally specified 'prefix',
        // 'localName', 'namespaceId', 'namespaceUri', and 'flags'.  Except
        // for 'flags', if any of the optional arguments are null (for
        // pointers) or 'INT_MIN' (for integer arguments), then the
        // corresponding facet will computed from the other arguments on an
        // as-needed basis.  If all of the optional arguments are given
        // non-null, non-'INT_MIN' values, then 'prefixStack' is unused and
        // may be null.  Arguments are permitted to be inconsistent with one
        // another (e.g., 'prefix' may not match the beginning of
        // 'qualifiedName') and will produce an object for with facets that
        // are inconsistent with one another.  This object will become invalid
        // if any of the supplied pointers is invalidated during its lifetime
        // (or before it is reset).  An invalid object may be destroyed or
        // reset, but any other access yields undefined behavior.

    // ACCESSORS
    const baexml_PrefixStack *prefixStack() const;
        // Return the value of 'prefixStack' specified at the last call to
        // 'reset' or the constructor or 0 if no prefix stack was specified.

    const char *qualifiedName() const;
        // Return the value of 'qualifiedName' specified at the last call to
        // 'reset' or the constructor or 0 if no qualified name was specified.

    const char *value() const;
        // Return the value of 'value' specified at the last call to 'reset'
        // or the constructor or 0 if no value was specified.

    const char *prefix() const;
        // Return the value of 'prefix' specified at the last call to 'reset'
        // or the constructor, if non-zero, else return a copy of the prefix
        // portion of the qualified name.  More precisely, return a copy of
        // the portion of 'qualifiedName()' up to, but not including, the
        // colon.  Return an empty string if the qualified name has no colon
        // or if 'prefixStack()' returns zero.

    const char *localName() const;
        // Return the value of 'localName' specified at the last call to
        // 'reset' or the constructor, if non-zero, else return the local part
        // of the qualified name.  More precisely, return the portion of
        // 'qualifiedName()' after the colon, if any, or the entire
        // 'qualifiedName()' if there is no colon.

    int namespaceId() const;
        // Return the value of 'localName' specified at the last call to
        // 'reset' or the constructor, if not 'INT_MIN', else return the ID
        // returned by the prefix stack for the current value of 'prefix()'.
        // Return -1 if 'prefix()' is an empty string, 'prefixStack()' is
        // zero, or 'prefix()' is not active in the prefix stack.  Note that,
        // as per the XML namespace standard, an empty prefix does *NOT* refer
        // to the default namespace, but rather refers to *NO* namespace.

    const char *namespaceUri() const;
        // Return the value of 'namespaceUri' specified at the last call to
        // 'reset' or the constructor, if non-zero, else return the URI
        // returned by the prefix stack for the current value of 'prefix()'.
        // Return an empty string if 'prefix()' is an empty string,
        // 'prefixStack()' is zero, or 'prefix()' is not active in the prefix
        // stack.  Note that, as per the XML namespace standard, an empty
        // prefix does *NOT* refer to the default namespace, but rather refers
        // to *NO* namespace.

    unsigned flags() const;
        // Return the value of 'flags' specified at the last call to 'reset'
        // or the constructor, if non-zero, or zero otherwise.  Pre-defined
        // flags are enumerated in the class definition.

    bool isNull() const;
        // Return true if this object is null.  More precisely, return true if
        // 'qualifiedName()' returns 0.  A default-constructed object will be
        // null, as will an object that was reset with no arguments.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // (absolute value of) the optionally specified indentation 'level'
        // and return a reference to 'stream'.  If 'level' is specified,
        // optionally specify 'spacesPerLevel', the number of spaces per
        // indentation level for this and all of its nested objects.  If
        // 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, format the entire output on one line.
        // If 'stream' is not valid on entry, this operation has no effect.
        // Facet values that have not yet been computed are represented by
        // "<null>" in the resulting stream.
};

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream&                  os,
                         const baexml_ElementAttribute& attribute);
    // Write the contents of the specified 'attribute' object to the specified
    // 'os' in human-readable form.  Attributes that have not yet been
    // computed are not computed by this function.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// ACCESSORS
inline
const baexml_PrefixStack *baexml_ElementAttribute::prefixStack() const
{
    return d_prefixStack;
}

inline
const char *baexml_ElementAttribute::qualifiedName() const
{
    return d_qualifiedName;
}

inline
const char *baexml_ElementAttribute::value() const
{
    return d_value;
}

inline
unsigned baexml_ElementAttribute::flags() const
{
    return d_flags;
}

inline
bool baexml_ElementAttribute::isNull() const
{
    return 0 == d_qualifiedName;
}

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream&                  os,
                         const baexml_ElementAttribute& attribute)
{
    return attribute.print(os);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
