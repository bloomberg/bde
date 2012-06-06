// bael_predicate.h                                                   -*-C++-*-
#ifndef INCLUDED_BAEL_PREDICATE
#define INCLUDED_BAEL_PREDICATE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a predicate object that consists of a name/value pair.
//
//@CLASSES:
//    bael_Predicate: a predicate in the form of an attribute name/value pair
//
//@SEE_ALSO: bael_predicateset, bael_rule
//
//@AUTHOR: Gang Chen (gchen20)
//
//@DESCRIPTION: This component implements a value-semantic predicate object
// that consists of a name and a value.  The value can be an 'int', a 64-bit
// integer, or a 'bsl::string'.  Both the name and value are managed by this
// object.
//
///Usage
///-----
// The following code illustrates how to create predicates:
//..
//    bael_Predicate p1("uuid", 4044457);
//    bael_Predicate p2("name", "Gang Chen");
//..
// The names of the predicates can be found by calling the 'name' method:
//..
//    assert(0 == bsl::strcmp("uuid", p1.name()));
//    assert(0 == bsl::strcmp("name", p2.name()));
//..
// The 'value' method returns a non-modifiable reference to the
// 'bdeut_Variant' object that manages the value of the predicate:
//..
//    assert(true        == p1.value().is<int>());
//    assert(4044457     == p1.value().the<int>());
//    assert(true        == p2.value().is<bsl::string>());
//    assert("Gang Chen" == p2.value().the<bsl::string>());
//..
// Note that the name string that is passed to the constructor of
// 'bael_Predicate' can be safely modified or even destroyed after the
// 'bael_Predicate' object is created.  In the next example, we create a
// temporary buffer to store the name string, and then use the buffer to
// create a predicate.  Once the predicate is created, the content of the
// buffer can be altered without affecting the name of the predicate:
//..
//    char buffer[] = "Hello";
//    bael_Predicate p3(buffer, 1);
//    bsl::strcpy(buffer, "World");
//    assert(0 == bsl::strcmp("Hello", p3.name()));
//..
// The 'bael_Attribute' class also provides a constructor that takes a value
// of type 'bael_Attribute::Value':
//..
//    bael_Attribute::Value value;
//    value.assign<int>(4044457);
//
//    bael_Predicate p4("uuid", value);
//    assert(p4 == p1);
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEL_ATTRIBUTE
#include <bael_attribute.h>
#endif

#ifndef INCLUDED_BDEUT_STRINGREF
#include <bdeut_stringref.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

                         // ====================
                         // class bael_Predicate
                         // ====================

class bael_Predicate {
    // This class contains an attribute name and an attribute value, both of
    // which are managed.  A 'bael_Predicate' object contains a
    // 'bael_Attribute' subobject, so that an equivalent 'bael_Attribute'
    // object can be obtained without any overhead when needed.  Since the
    // 'bael_Attribute' subobject does not manage the attribute name,
    // 'bael_Predicate' stores the attribute name in another member variable.

    // DATA
    bsl::string     d_nameStr;    // the attribute name

    bael_Attribute  d_attribute;  // the 'bael_Attribute' subobject, whose
                                  // attribute name points to 'd_nameStr'

    // FRIENDS
    friend bool operator==(const bael_Predicate&, const bael_Predicate&);
    friend bool operator!=(const bael_Predicate&, const bael_Predicate&);
    friend bsl::ostream& operator<<(bsl::ostream&, const bael_Predicate&);

  public:
    // CLASS METHODS
    static int hash(const bael_Predicate& predicate, int size);
        // Return a hash value calculated from the specified 'predicate' using
        // the specified 'size' as the number of slots.  The hash value is
        // guaranteed to be in the range [0 .. size - 1].

    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported
        // by this class.  (See the 'bde' package-group-level documentation
        // for more information on 'bdex' streaming of value-semantic types
        // and containers.)

    // CREATORS
    bael_Predicate(const bdeut_StringRef&  name,
                   int                     value,
                   bslma_Allocator        *basicAllocator = 0 );
        // Create a 'bael_Predicate' object having the specified 'name' and
        // 32-bit integer 'value'.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    bael_Predicate(const bdeut_StringRef&    name,
                   bsls_PlatformUtil::Int64  value,
                   bslma_Allocator          *basicAllocator = 0 );
        // Create a 'bael_Predicate' object having the specified 'name' and
        // 64-bit integer 'value'.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    bael_Predicate(const bdeut_StringRef&  name,
                   const char             *value,
                   bslma_Allocator        *basicAllocator = 0 );
        // Create a 'bael_Predicate' object having the specified 'name' and
        // character string 'value'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    bael_Predicate(const bdeut_StringRef&        name,
                   const bael_Attribute::Value&  value,
                   bslma_Allocator              *basicAllocator = 0 );
        // Create a 'bael_Attribute' object having the specified (literal)
        // 'name' and 'value'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    bael_Predicate(const bael_Predicate&  original,
                   bslma_Allocator       *basicAllocator = 0);
        // Create a 'bael_Predicate' object having the same name and attribute
        // value as the specified 'original' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    // MANIPULATORS
    bael_Predicate& operator=(const bael_Predicate& rhs);
        // Assign the value of the specified 'rhs' object to this object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a
        // reference to the modifiable 'stream'.  If 'stream' is initially
        // invalid, this operation has no effect.  If 'stream' becomes invalid
        // during this operation, this object is valid, but its value is
        // undefined.  If 'version' is not supported, 'stream' is marked
        // invalid and this object is unaltered.  Note that no version is read
        // from 'stream'.

    void setName(const bdeut_StringRef& name);
        // Set the attribute name of this object to the specified (literal)
        // 'name'.

    void setValue(const bael_Attribute::Value& value);
        // Set the attribute value of this object to the specified 'value'.

    // ACCESSORS
    const char *name() const;
        // Return the name of this object.

    const bael_Attribute::Value& value() const;
        // Return a reference to the non-modifiable attribute value of this
        // object.

    const bael_Attribute& attribute() const;
        // Return a reference to the non-modifiable attribute managed by this
        // object.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // (absolute value of) the optionally specified indentation 'level'
        // and return a reference to 'stream'.  If 'level' is specified,
        // optionally specify 'spacesPerLevel', the number of spaces per
        // indentation level for this and all of its nested objects.  If
        // 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, format the entire output on one line,
        // suppressing all but the initial indentation (as governed by
        // 'level').  If 'stream' is not valid on entry, this operation has no
        // effect.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the modifiable
        // 'stream'.  If 'version' is not supported, 'stream' is unmodified.
        // Note that 'version' is not written to 'stream'.
};

// FREE OPERATORS
bool operator==(const bael_Predicate& lhs, const bael_Predicate& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'bael_Predicate' objects have the
    // same value if they have the same name (but not necessarily the
    // identical representation in memory), the same attribute value type, and
    // the same attribute value.

bool operator!=(const bael_Predicate& lhs, const bael_Predicate& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'bael_Predicate' objects do not
    // have the same value if any of their respective names (value, not
    // address), attribute value types, or attribute values differ.

bsl::ostream& operator<<(bsl::ostream&         stream,
                         const bael_Predicate& attribute);
    // Write the value of the specified 'attribute' to the specified output
    // 'stream', and return a reference to the modifiable 'stream'.

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                         // --------------------
                         // class bael_Predicate
                         // --------------------

//CLASS METHODS
inline
int bael_Predicate::hash(const bael_Predicate& predicate, int size)
{
    return bael_Attribute::hash(predicate.d_attribute, size);
}

inline
int bael_Predicate::maxSupportedBdexVersion()
{
    return 1;  // Required by BDE policy; versions start at 1.
}

// CREATORS
inline
bael_Predicate::bael_Predicate(const bdeut_StringRef&  name,
                               int                     value,
                               bslma_Allocator        *basicAllocator)
: d_nameStr(name.data(), name.length(), basicAllocator)
, d_attribute(d_nameStr.c_str(), value, basicAllocator)
{
}

inline
bael_Predicate::bael_Predicate(const bdeut_StringRef&    name,
                               bsls_PlatformUtil::Int64  value,
                               bslma_Allocator          *basicAllocator)
: d_nameStr(name.data(), name.length(), basicAllocator)
, d_attribute(d_nameStr.c_str(), value, basicAllocator)
{
}

inline
bael_Predicate::bael_Predicate(const bdeut_StringRef&  name,
                               const char             *value,
                               bslma_Allocator        *basicAllocator)
: d_nameStr(name.data(), name.length(), basicAllocator)
, d_attribute(d_nameStr.c_str(), value, basicAllocator)
{
}

inline
bael_Predicate::bael_Predicate(const bdeut_StringRef&        name,
                               const bael_Attribute::Value&  value,
                               bslma_Allocator              *basicAllocator)
: d_nameStr(name.data(), name.length(), basicAllocator)
, d_attribute(d_nameStr.c_str(), value, basicAllocator)
{
}

inline
bael_Predicate::bael_Predicate(const bael_Predicate&  original,
                               bslma_Allocator       *basicAllocator)
: d_nameStr(original.d_nameStr, basicAllocator)
, d_attribute(d_nameStr.c_str(), original.d_attribute.value(), basicAllocator)
{
}

// MANIPULATORS
inline
bael_Predicate& bael_Predicate::operator=(const bael_Predicate& rhs)
{
    d_nameStr   = rhs.d_nameStr;
    d_attribute = rhs.d_attribute;
    d_attribute.setName(d_nameStr.c_str());
    return *this;
}

inline
void bael_Predicate::setName(const bdeut_StringRef& name)
{
    d_nameStr.assign(name.data(), name.length());
    d_attribute.setName(d_nameStr.c_str());
}

inline
void bael_Predicate::setValue(const bael_Attribute::Value& value)
{
    d_attribute.setValue(value);
}

template <class STREAM>
STREAM& bael_Predicate::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_InStreamFunctions::streamIn(stream, d_nameStr, 0);
            if (!stream) {
                return stream;                                        // RETURN
            }
            d_attribute.setName(d_nameStr.c_str());

            bael_Attribute::Value value;
            bdex_InStreamFunctions::streamIn(stream, value, 1);
            if (!stream) {
                return stream;                                        // RETURN
            }
            d_attribute.setValue(value);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }

    return stream;
}

// ACCESSORS
inline
const char *bael_Predicate::name() const
{
    return d_attribute.name();
}

inline
const bael_Attribute::Value& bael_Predicate::value() const
{
    return d_attribute.value();
}

inline
const bael_Attribute& bael_Predicate::attribute() const
{
    return d_attribute;
}

template <class STREAM>
STREAM& bael_Predicate::bdexStreamOut(STREAM& stream, int version) const
{
    if (stream) {
        switch (version) {
          case 1: {
            bdex_OutStreamFunctions::streamOut(stream, d_nameStr, 0);
            bdex_OutStreamFunctions::streamOut(stream, d_attribute.value(), 1);
          } break;
          default: {
            stream.invalidate();
          }
        }
    }

    return stream;
}

// FREE OPERATORS
inline
bool operator==(const bael_Predicate& lhs, const bael_Predicate& rhs)
{
    return lhs.d_nameStr           == rhs.d_nameStr
        && lhs.d_attribute.value() == rhs.d_attribute.value();
}

inline
bool operator!=(const bael_Predicate& lhs, const bael_Predicate& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bael_Predicate& predicate)
{
    return predicate.print(stream, 0, -1);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
