// baenet_httpextendedheaderfields.h                                  -*-C++-*-
#ifndef INCLUDED_BAENET_HTTPEXTENDEDHEADERFIELDS
#define INCLUDED_BAENET_HTTPEXTENDEDHEADERFIELDS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a container for extended HTTP header fields.
//
//@CLASSES:
//  baenet_HttpExtendedHeaderFields: container for extended HTTP header fields
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION:  This component provides the 'baenet_HttpExtendedHeaderFields'
// class, which is a value-semantic container for extended HTTP header fields.
// The container is essentially a multi-map of field names to field values.  A
// significant property of this container is that the order of same-field
// elements are preserved.
//
///Iterators
///---------
// The 'baenet_HttpExtendedHeaderFields' class provides two types of iterators,
// with immutable ("const") variants of both.  The iterators are listed in the
// table below:
//..
//  Purpose                       Mutable Type          ImmutableType
//  -------                       ------------          -------------
//  Iterate through the fields    FieldIterator         ConstFieldIterator
//  in the header.
//
//  Iterate through the values    FieldValueIterator    ConstFieldValueIterator
//  for a particular field.
//..
//
///FieldIterator And ConstFieldIterator
///------------------------------------
// These iterators are "Forward Iterator"s and are used to iterate through the
// fields in a 'baenet_HttpExtendedHeaderFields' object.  More information
// about "Forward Iterator"s can be obtained from
// 'http://www.sgi.com/tech/stl/ForwardIterator.html'.
//
// The value type of 'FieldIterator' and 'ConstFieldIterator' contains the
// following members:
//..
//  Member    Description
//  ------    -----------
//  first     A reference to an 'bsl::string' object containing the name of the
//            field.
//
//  second    A reference to an implementation-defined container containing the
//            values of the field.  The type of this object is a model of the
//            "Forward Container" concept.  More information about the
//            "Forward Container" concept can be obtained from
//            'http://www.sgi.com/tech/stl/ForwardContainer.html'.  For
//            convenience, the 'iterator' type for this container is typedef'ed
//            as 'FieldValueIterator', and the 'const_iterator' type for this
//            container typedef'ed as 'ConstFieldValueIterator'.
//..
//
///FieldValueIterator And ConstFieldValueIterator
///----------------------------------------------
// These iterators are "Forward Iterator"s and are used to iterate through the
// values of a field.  More information about "Forward Iterator"s can be
// obtained from 'http://www.sgi.com/tech/stl/ForwardIterator.html'.
//
// Note that these typedefs are shorthand for the following types:
//..
//  Typedef                   Shorthand For
//  -------                   -------------
//
//  FieldValueIterator        FieldIterator::value_type::second_type::iterator
//  ConstFieldValueIterator   ConstFieldIterator::value_type::second_type
//                                                             ::const_iterator
//..
//
// The value type of 'FieldValueIterator' and 'ConstFieldValueIterator' is
// 'bsl::string'.
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.  We
// use the 'addFieldValue' method to add fields:
//..
//  baenet_HttpExtendedHeaderFields fields;
//  fields.addFieldValue("CustomFieldB", "Value 1");
//  fields.addFieldValue("SOAPAction", "http://www.bloomberg.com/sqrt");
//  fields.addFieldValue("CustomFieldA", "Value 2");
//  fields.addFieldValue("CustomFieldA", "Value 3");
//  fields.addFieldValue("CustomFieldB", "Value 4");
//..
// Now we will iterate through the fields and the values of each field:
//..
// typedef baenet_HttpExtendedHeaderFields::FieldIterator      FieldIterator;
// typedef baenet_HttpExtendedHeaderFields::FieldValueIterator ValueIterator;
//
// for (FieldIterator fi = fields.beginFields(); fi != fields.endFields();
//                                                                      ++fi) {
//     for (ValueIterator vi = fi->second.begin(); vi != fi->second.end();
//                                                                      ++vi) {
//         bsl::cout << "Field Name = " << (*fi).first << ", Value = "
//                   << *vi << bsl::endl;
//     }
// }
//..
// Note that the order of fields are not guaranteed to be maintained, but the
// order of the values for each field is maintained.  The following will be
// printed on standard output:
//..
//  Field Name = CustomFieldA, Value = Value 2
//  Field Name = CustomFieldA, Value = Value 3
//  Field Name = CustomFieldB, Value = Value 1
//  Field Name = CustomFieldB, Value = Value 4
//  Field Name = SOAPAction, Value = http://www.bloomberg.com/sqrt
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEAT_VALUETYPEFUNCTIONS
#include <bdeat_valuetypefunctions.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BDEU_PRINTMETHODS
#include <bdeu_printmethods.h>
#endif

#ifndef INCLUDED_BDEU_STRING
#include <bdeu_string.h>
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

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

struct baenet_HttpExtendedHeaderFields_Trait
                                          : bslalg_TypeTraitUsesBslmaAllocator,
                                            bdeu_TypeTraitHasPrintMethod { };

                   // =====================================
                   // class baenet_HttpExtendedHeaderFields
                   // =====================================

class baenet_HttpExtendedHeaderFields {
    // This class is a value-semantic container for extended HTTP header
    // fields.  Extended header fields is basically a multi-map of case
    // insensitive 'bsl::string' to 'bsl::string'.  However, as per
    // RFC2616/4.2, the order in which the header fields with the same field
    // name are added is preserved.

    // PRIVATE TYPES
    struct CaseInsensitiveLessThan {
        bool operator()(const bsl::string& lhs, const bsl::string& rhs) const
        {
            return bdeu_String::lowerCaseCmp(lhs, rhs) == -1 ? true : false;
        }
    };

    typedef bsl::vector<bsl::string> FieldValues;

    typedef bsl::map<bsl::string,
                     FieldValues,
                     CaseInsensitiveLessThan> FieldMap;
        // TBD: this would be good to go into a separate reusable component
        // TBD: (multi-map that preserves order of same-key elements)

    // PRIVATE DATA MEMBERS
    FieldMap d_fieldMap;

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(baenet_HttpExtendedHeaderFields,
                                 baenet_HttpExtendedHeaderFields_Trait);

    // TYPES
    typedef FieldMap::iterator       FieldIterator;
    typedef FieldMap::const_iterator ConstFieldIterator;
        // Mutable and immutable iterator types for iterating through fields in
        // an extended header.  See the component-level documentation for a
        // description of this iterator type.

    typedef FieldValues::iterator       FieldValueIterator;
    typedef FieldValues::const_iterator ConstFieldValueIterator;
        // Mutable and immutable iterator types for iterating through field
        // values for a particular field.  See the component-level
        // documentation for a description of these iterator types.

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    // CREATORS
    explicit baenet_HttpExtendedHeaderFields(
                                          bslma_Allocator *basicAllocator = 0);
        // Construct an extended header fields container and use the specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    baenet_HttpExtendedHeaderFields(
                   const baenet_HttpExtendedHeaderFields&  original,
                   bslma_Allocator                        *basicAllocator = 0);
        // Construct a copy of the specified 'original', and use the specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    ~baenet_HttpExtendedHeaderFields();
        // Destroy this object.

    // MANIPULATORS
    baenet_HttpExtendedHeaderFields& operator=(
                                   const baenet_HttpExtendedHeaderFields& rhs);
        // Assign the specified 'rhs' to this object.  Return a reference to
        // this modifiable object.

    FieldValueIterator addFieldValue(const bsl::string& fieldName);
    FieldValueIterator addFieldValue(const bsl::string& fieldName,
                                     const bsl::string& fieldValue);
        // Add a new field with the specified 'fieldName' and the optionally
        // specified 'fieldValue'.  Return a mutable field value iterator to
        // the field value.  Note that if 'fieldValue' is not specified, the
        // field value will be an empty string.

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

    FieldIterator beginFields();
        // Return a mutable iterator positioned at the first field in this
        // container.  If there are no fields in this container, the returned
        // iterator is positioned at that returned by 'endFields'.

    FieldIterator endFields();
        // Return a mutable iterator positioned one element past the last field
        // in this container.  If there are no fields in this container, the
        // returned iterator is positioned at that returned by 'beginFields'.

    FieldIterator findField(const bsl::string& fieldName);
        // Return a mutable iterator positioned at the field with the specified
        // 'fieldName'.  If there is no field with the specified 'fieldName' in
        // this container, the returned iterator is positioned at that returned
        // by 'endFields'.  Note that the search is not case sensitive.

    void reset();
        // Remove all fields from this object.

    // ACCESSORS
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    ConstFieldIterator beginFields() const;
        // Return an immutable iterator positioned at the first field in this
        // container.  If there are no fields in this container, the returned
        // iterator is positioned at that returned by 'endFields'.

    ConstFieldIterator endFields() const;
        // Return an immutable iterator positioned one element past the last
        // field in this container.  If there are no fields in this container,
        // the returned iterator is positioned at that returned by
        // 'beginFields'.

    ConstFieldIterator findField(const bsl::string& fieldName) const;
        // Return an immutable iterator positioned at the field with the
        // specified 'fieldName'.  If there is no field with the specified
        // 'fieldName' in this container, the returned iterator is positioned
        // at that returned by 'endFields'.  Note that the search is not case
        // sensitive.

    int numFields() const;
        // Return the number of fields in this container.  Note that each field
        // can have more than one value.

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
};

// FREE OPERATORS
bool operator==(const baenet_HttpExtendedHeaderFields& lhs,
                const baenet_HttpExtendedHeaderFields& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects have
    // the same value, and 'false' otherwise.

inline
bool operator!=(const baenet_HttpExtendedHeaderFields& lhs,
                const baenet_HttpExtendedHeaderFields& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' attribute objects do not
    // have the same value, and 'false' otherwise.

inline
bsl::ostream& operator<<(bsl::ostream&                          stream,
                         const baenet_HttpExtendedHeaderFields& rhs);
    // Format the specified 'rhs' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                   // -------------------------------------
                   // class baenet_HttpExtendedHeaderFields
                   // -------------------------------------

// CLASS METHODS
inline
int baenet_HttpExtendedHeaderFields::maxSupportedBdexVersion()
{
    return 1;
}

// CREATORS
inline
baenet_HttpExtendedHeaderFields::baenet_HttpExtendedHeaderFields(
                                               bslma_Allocator *basicAllocator)
: d_fieldMap(CaseInsensitiveLessThan(), basicAllocator)
{
}

inline
baenet_HttpExtendedHeaderFields::baenet_HttpExtendedHeaderFields(
                        const baenet_HttpExtendedHeaderFields&  original,
                        bslma_Allocator                        *basicAllocator)
: d_fieldMap(original.d_fieldMap, basicAllocator)
{
}

inline
baenet_HttpExtendedHeaderFields::~baenet_HttpExtendedHeaderFields()
{
}

// MANIPULATORS
inline
baenet_HttpExtendedHeaderFields&
baenet_HttpExtendedHeaderFields::operator=(
                                    const baenet_HttpExtendedHeaderFields& rhs)
{
    if (this != &rhs) {
        d_fieldMap = rhs.d_fieldMap;
    }

    return *this;
}

inline
baenet_HttpExtendedHeaderFields::FieldValueIterator
baenet_HttpExtendedHeaderFields::addFieldValue(const bsl::string& fieldName)
{
    return addFieldValue(fieldName, "");
}

inline
baenet_HttpExtendedHeaderFields::FieldValueIterator
baenet_HttpExtendedHeaderFields::addFieldValue(const bsl::string& fieldName,
                                               const bsl::string& fieldValue)
{
    FieldValues& values = d_fieldMap[fieldName];
    return values.insert(values.end(), fieldValue);
}

template <class STREAM>
STREAM& baenet_HttpExtendedHeaderFields::bdexStreamIn(STREAM& stream,
                                                      int     version)
{
    switch (version) {
      case 1: {
        typedef bsl::pair<FieldMap::iterator, bool> InsertResult;

        reset();

        int                  numFieldNames;
        FieldMap::value_type field;

        bdex_InStreamFunctions::streamIn(stream, numFieldNames, 1);

        for (int i = 0; stream && i < numFieldNames; ++i) {
            bdex_InStreamFunctions::streamIn(stream, field.first, 1);

            if (!stream) {
                break;
            }

            InsertResult result = d_fieldMap.insert(field);

            if (!result.second) {
                stream.invalidate();
                break;
            }

            bdex_InStreamFunctions::streamIn(stream, result.first->second, 1);
        }
      } break;
      default: {
        stream.invalidate();
      } break;
    }
    return stream;
}

inline
baenet_HttpExtendedHeaderFields::FieldIterator
baenet_HttpExtendedHeaderFields::beginFields()
{
    return d_fieldMap.begin();
}

inline
baenet_HttpExtendedHeaderFields::FieldIterator
baenet_HttpExtendedHeaderFields::endFields()
{
    return d_fieldMap.end();
}

inline
baenet_HttpExtendedHeaderFields::FieldIterator
baenet_HttpExtendedHeaderFields::findField(const bsl::string& fieldName)
{
    return d_fieldMap.find(fieldName);
}

inline
void baenet_HttpExtendedHeaderFields::reset()
{
    d_fieldMap.clear();
}

// ACCESSORS
template <class STREAM>
STREAM& baenet_HttpExtendedHeaderFields::bdexStreamOut(STREAM& stream,
                                                       int     version) const
{
    switch (version) {
      case 1: {
        const int numFieldNames = d_fieldMap.size();

        bdex_OutStreamFunctions::streamOut(stream, numFieldNames, 1);

        typedef FieldMap::const_iterator NameIterator;

        for (NameIterator it  = d_fieldMap.begin();
                          it != d_fieldMap.end() && stream;
                        ++it) {
            bdex_OutStreamFunctions::streamOut(stream, it->first,  1);
            bdex_OutStreamFunctions::streamOut(stream, it->second, 1);
        }
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
baenet_HttpExtendedHeaderFields::ConstFieldIterator
baenet_HttpExtendedHeaderFields::beginFields() const
{
    return d_fieldMap.begin();
}

inline
baenet_HttpExtendedHeaderFields::ConstFieldIterator
baenet_HttpExtendedHeaderFields::endFields() const
{
    return d_fieldMap.end();
}

inline
baenet_HttpExtendedHeaderFields::ConstFieldIterator
baenet_HttpExtendedHeaderFields::findField(const bsl::string& fieldName) const
{
    return d_fieldMap.find(fieldName);
}

inline
int baenet_HttpExtendedHeaderFields::numFields() const
{
    return static_cast<int>(d_fieldMap.size());
}

inline
bsl::ostream& baenet_HttpExtendedHeaderFields::print(
                                            bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    return bdeu_PrintMethods::print(stream, d_fieldMap, level, spacesPerLevel);
}

// FREE OPERATORS
inline
bool operator!=(const baenet_HttpExtendedHeaderFields& lhs,
                const baenet_HttpExtendedHeaderFields& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream&                          stream,
                         const baenet_HttpExtendedHeaderFields& rhs)
{
    return rhs.print(stream, 0, -1);
}

                     // ==================================
                     // namespace bdeat_ValueTypeFunctions
                     // ==================================

namespace bdeat_ValueTypeFunctions {

    inline
    void reset(baenet_HttpExtendedHeaderFields *object)
    {
        object->reset();
    }

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
