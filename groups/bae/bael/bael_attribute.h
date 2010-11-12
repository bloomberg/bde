// bael_attribute.h                                                   -*-C++-*-
#ifndef INCLUDED_BAEL_ATTRIBUTE
#define INCLUDED_BAEL_ATTRIBUTE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a representation of (literal) name/value pairs.
//
//@CLASSES:
//     bael_Attribute: (literal) name/value pair
//
//@SEE_ALSO: bael_attributeset
//
//@AUTHOR: Gang Chen (gchen20)
//
//@DESCRIPTION: This component implements an unusual in-core value-semantic
// class, 'bael_Attribute'.  Each instance of this type represents an attribute
// that consists of a (literal) name (held but not owned), and an associated
// value (owned) that can be an 'int', a 64-bit integer, or a 'bsl::string'.
//
// IMPORTANT: The attribute name, whose type is 'const char *', must therefore
// remain valid throughout the life time of the 'bael_Attribute' object and
// that of any other 'bael_Attribute' objects that are copied or assigned from
// the original object.  It is recommended that only null-terminated c string
// literals be used for names.
//
///Usage
///-----
// The following code creates three attributes having the same name, but
// different attribute value types.
//..
//    bael_Attribute a1("day", "Sunday");
//    bael_Attribute a2("day", 7);
//    bael_Attribute a3("day", static_cast<bsls_PlatformUtil::Int64>(7));
//..
// The names of the attributes can be found by calling the 'name' method:
//..
//    assert(0 == bsl::strcmp("day", a1.name()));
//    assert(0 == bsl::strcmp("day", a2.name()));
//    assert(0 == bsl::strcmp("day", a3.name()));
//..
// The 'value' method returns a non-modifiable reference to the
// 'bdeut_Variant' object that manages the value of the attribute:
//..
//    assert(true     == a1.value().is<bsl::string>());
//    assert("Sunday" == a1.value().the<bsl::string>());
//
//    assert(true     == a2.value().is<int>());
//    assert(7        == a2.value().the<int>());
//
//    assert(true     == a3.value().is<bsls_PlatformUtil::Int64>());
//    assert(7        == a3.value().the<bsls_PlatformUtil::Int64>());
//..
// Note that the name string that is passed to the constructor of
// 'bael_Attribute' *must* remain valid and unchanged after the
// 'bael_Attribute' object is created.  In the next example, we create a
// temporary buffer to store the name string, and then use the buffer to
// create an attribute.  Note that any subsequent changes to this temporary
// buffer will also modify the name of the attribute:
//..
//    char buffer[] = "Hello";
//    bael_Attribute a4(buffer, 1);                   // BAD IDEA!!!
//    bsl::strcpy(buffer, "World");
//    assert(0 == bsl::strcmp("World", a4.name()));
//..
// The 'bael_Attribute' class also provides a constructor that takes a value
// of type 'bael_Attribute::Value':
//..
//    bael_Attribute::Value value;
//    value.assign<bsl::string>("Sunday");
//    bael_Attribute a5("day", value);
//    assert(a5 == a1);
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BDEUT_VARIANT
#include <bdeut_variant.h>
#endif

#ifndef INCLUDED_BDEUT_STRINGREF
#include <bdeut_stringref.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

                        // ====================
                        // class bael_Attribute
                        // ====================

class bael_Attribute {
    // A 'bael_Attribute' object contains an attribute name which is not
    // managed and an attribute value which is managed.

  public:
    // TYPES
    typedef bdeut_Variant<int,
                          bsls_PlatformUtil::Int64,
                          bsl::string> Value;

  private:
    // DATA
    const char  *d_name;       // attribute name

    Value        d_value;      // attribute value

    mutable int  d_hashValue;  // hash value (-1 means unset)

    mutable int  d_hashSize;   // hash size from which the hash value was
                               // calculated

    // FRIENDS
    friend bool operator==(const bael_Attribute&, const bael_Attribute&);
    friend bool operator!=(const bael_Attribute&, const bael_Attribute&);
    friend bsl::ostream& operator<<(bsl::ostream&, const bael_Attribute&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bael_Attribute,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CLASS METHODS
    static int hash(const bael_Attribute& attribute, int size);
        // Return a hash value calculated from the specified 'attribute' using
        // the specified 'size' as the number of slots.  The hash value is
        // guaranteed to be in the range [0 ..size - 1].  The behavior is
        // undefined unless '0 <= size'.

    // CREATORS
    bael_Attribute(const char      *name,
                   int              value,
                   bslma_Allocator *basicAllocator = 0 );
        // Create a 'bael_Attribute' object having the specified (literal)
        // 'name' and (32-bit integer) 'value'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator will be used.  Note that
        // 'name' is not managed by this object and therefore must remain
        // valid while in use by any 'bael_Attribute' object.

    bael_Attribute(const char               *name,
                   bsls_PlatformUtil::Int64  value,
                   bslma_Allocator          *basicAllocator = 0 );
        // Create a 'bael_Attribute' object having the specified (literal)
        // 'name' and (64-bit integer) 'value'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator will be used.  Note that
        // 'name' is not managed by this object and therefore must remain
        // valid while in use by any 'bael_Attribute' object.

    bael_Attribute(const char             *name,
                   const bdeut_StringRef&  value,
                   bslma_Allocator        *basicAllocator = 0 );
        // Create a 'bael_Attribute' object having the specified (literal)
        // 'name' and (character string)'value'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator will be used.  Note that
        // 'name' is not managed by this object and therefore must remain
        // valid while in use by any 'bael_Attribute' object.

    bael_Attribute(const char      *name,
                   const Value&     value,
                   bslma_Allocator *basicAllocator = 0 );
        // Create a 'bael_Attribute' object having the specified (literal)
        // 'name' and 'value'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator will be used.  Note that 'name' is not managed by
        // this object and therefore must remain valid while in use by any
        // 'bael_Attribute' object.

    bael_Attribute(const bael_Attribute&  original,
                   bslma_Allocator       *basicAllocator = 0);
        // Create a 'bael_Attribute' object having the same (literal) name and
        // attribute value as the specified 'original' object.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator
        // will be used.

    ~bael_Attribute();
        // Destroy this attribute object.

    // MANIPULATORS
    bael_Attribute& operator=(const bael_Attribute& rhs);
        // Assign the value of the specified 'rhs' object to this object.

    void setName(const char *name);
        // Set the attribute name of this object to the specified (literal)
        // 'name'.  Note that 'name' is not managed by this object and
        // therefore must remain valid while in use by any 'bael_Attribute'
        // object.

    void setValue(const Value& value);
    void setValue(int value);
    void setValue(bsls_PlatformUtil::Int64 value);
    void setValue(const bdeut_StringRef& value);
        // Set the attribute value of this object to the specified 'value'.

    // ACCESSORS
    const char *name() const;
        // Return the name of this object.

    const Value& value() const;
        // Return a reference to the non-modifiable attribute value of this
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
};

// FREE OPERATORS
bool operator==(const bael_Attribute& lhs,
                const bael_Attribute& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'bael_Attribute' objects have the
    // same value if they have the same name (but not necessarily the
    // identical representation in memory), the same attribute value type, and
    // the same attribute value.

bool operator!=(const bael_Attribute& lhs,
                const bael_Attribute& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'bael_Attribute' objects do not
    // have the same value if any of their respective names (value, not
    // address), attribute value types, or attribute values differ.

bsl::ostream& operator<<(bsl::ostream&         output,
                         const bael_Attribute& attribute);
    // Write the value of the specified 'attribute' to the specified
    // 'output' stream.  Return the specified 'output' stream.

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

// CREATORS
inline
bael_Attribute::bael_Attribute(const char      *name,
                               int              value,
                               bslma_Allocator *basicAllocator)
: d_name(name)
, d_value(basicAllocator)
, d_hashValue(-1)
{
    d_value.assign<int>(value);
}

inline
bael_Attribute::bael_Attribute(const char               *name,
                               bsls_PlatformUtil::Int64 value,
                               bslma_Allocator          *basicAllocator)
: d_name(name)
, d_value(basicAllocator)
, d_hashValue(-1)
{
    d_value.assign<bsls_PlatformUtil::Int64>(value);
}

inline
bael_Attribute::bael_Attribute(const char             *name,
                               const bdeut_StringRef&  value,
                               bslma_Allocator        *basicAllocator)
: d_name(name)
, d_value(basicAllocator)
, d_hashValue(-1)
{
    d_value.assign<bsl::string>(value);
}

inline
bael_Attribute::bael_Attribute(const char      *name,
                               const Value&     value,
                               bslma_Allocator *basicAllocator)
: d_name(name)
, d_value(value, basicAllocator)
, d_hashValue(-1)
{
}

inline
bael_Attribute::bael_Attribute(const bael_Attribute&  original,
                               bslma_Allocator       *basicAllocator)
: d_name(original.d_name)
, d_value(original.d_value, basicAllocator)
, d_hashValue(original.d_hashValue)
, d_hashSize(original.d_hashSize)
{
}

inline
bael_Attribute::~bael_Attribute()
{
}

// MANIPULATORS
inline
bael_Attribute& bael_Attribute::operator=(const bael_Attribute& rhs)
{
    d_name      = rhs.d_name;
    d_value     = rhs.d_value;
    d_hashValue = rhs.d_hashValue;
    d_hashSize  = rhs.d_hashSize;
    return *this;
}

inline
void bael_Attribute::setName(const char *name)
{
    d_name = name;
    d_hashValue = -1;
}

inline
void bael_Attribute::setValue(const Value& value)
{
    d_value = value;
    d_hashValue = -1;
}

inline
void bael_Attribute::setValue(int value)
{
    d_value.assign(value);
    d_hashValue = -1;
}

inline
void bael_Attribute::setValue(bsls_PlatformUtil::Int64 value)
{
    d_value.assign(value);
    d_hashValue = -1;
}

inline
void bael_Attribute::setValue(const bdeut_StringRef& value)
{
    d_value.assign(bsl::string(value));
    d_hashValue = -1;
}

// ACCESSORS
inline
const char *bael_Attribute::name() const
{
    return d_name;
}

inline
const bael_Attribute::Value& bael_Attribute::value() const
{
    return d_value;
}

// FREE OPERATORS
inline
bool operator==(const bael_Attribute& lhs,
                const bael_Attribute& rhs)
{
    return 0 == bsl::strcmp(lhs.d_name, rhs.d_name)
        && lhs.d_value == rhs.d_value;
}

inline
bool operator!=(const bael_Attribute& lhs,
                const bael_Attribute& rhs)
{
    return !(lhs == rhs);
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
