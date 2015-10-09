// ball_attribute.h                                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_ATTRIBUTE
#define INCLUDED_BALL_ATTRIBUTE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a representation of (literal) name/value pairs.
//
//@CLASSES:
//     ball::Attribute: (literal) name/value pair
//
//@SEE_ALSO: ball_attributeset
//
//@DESCRIPTION: This component implements an unusual in-core value-semantic
// class, 'ball::Attribute'.  Each instance of this type represents an
// attribute that consists of a (literal) name (held but not owned), and an
// associated value (owned) that can be an 'int', a 64-bit integer, or a
// 'bsl::string'.
//
// IMPORTANT: The attribute name, whose type is 'const char *', must therefore
// remain valid throughout the life time of the 'ball::Attribute' object and
// that of any other 'ball::Attribute' objects that are copied or assigned from
// the original object.  It is recommended that only null-terminated C-string
// literals be used for names.
//
///Usage
///-----
// The following code creates three attributes having the same name, but
// different attribute value types.
//..
//    ball::Attribute a1("day", "Sunday");
//    ball::Attribute a2("day", 7);
//    ball::Attribute a3("day", static_cast<bsls::Types::Int64>(7));
//..
// The names of the attributes can be found by calling the 'name' method:
//..
//    assert(0 == bsl::strcmp("day", a1.name()));
//    assert(0 == bsl::strcmp("day", a2.name()));
//    assert(0 == bsl::strcmp("day", a3.name()));
//..
// The 'value' method returns a non-modifiable reference to the
// 'bdlb::Variant' object that manages the value of the attribute:
//..
//    assert(true     == a1.value().is<bsl::string>());
//    assert("Sunday" == a1.value().the<bsl::string>());
//
//    assert(true     == a2.value().is<int>());
//    assert(7        == a2.value().the<int>());
//
//    assert(true     == a3.value().is<bsls::Types::Int64>());
//    assert(7        == a3.value().the<bsls::Types::Int64>());
//..
// Note that the name string that is passed to the constructor of
// 'ball::Attribute' *must* remain valid and unchanged after the
// 'ball::Attribute' object is created.  In the next example, we create a
// temporary buffer to store the name string, and then use the buffer to
// create an attribute.  Note that any subsequent changes to this temporary
// buffer will also modify the name of the attribute:
//..
//    char buffer[] = "Hello";
//    ball::Attribute a4(buffer, 1);                   // BAD IDEA!!!
//    bsl::strcpy(buffer, "World");
//    assert(0 == bsl::strcmp("World", a4.name()));
//..
// The 'ball::Attribute' class also provides a constructor that takes a value
// of type 'ball::Attribute::Value':
//..
//    ball::Attribute::Value value;
//    value.assign<bsl::string>("Sunday");
//    ball::Attribute a5("day", value);
//    assert(a5 == a1);
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BDLB_VARIANT
#include <bdlb_variant.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

namespace BloombergLP {

namespace ball {
                        // ===============
                        // class Attribute
                        // ===============

class Attribute {
    // An 'Attribute' object contains an attribute name which is not managed
    // and an attribute value which is managed.

  public:
    // TYPES
    typedef bdlb::Variant<int,
                          bsls::Types::Int64,
                          bsl::string> Value;

  private:
    // DATA
    const char  *d_name;       // attribute name

    Value        d_value;      // attribute value

    mutable int  d_hashValue;  // hash value (-1 means unset)

    mutable int  d_hashSize;   // hash size from which the hash value was
                               // calculated

    // FRIENDS
    friend bool operator==(const Attribute&, const Attribute&);
    friend bool operator!=(const Attribute&, const Attribute&);
    friend bsl::ostream& operator<<(bsl::ostream&, const Attribute&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Attribute, bslma::UsesBslmaAllocator);

    // CLASS METHODS
    static int hash(const Attribute& attribute, int size);
        // Return a hash value calculated from the specified 'attribute' using
        // the specified 'size' as the number of slots.  The hash value is
        // guaranteed to be in the range [0 ..size - 1].  The behavior is
        // undefined unless '0 <= size'.

    // CREATORS
    Attribute(const char       *name,
              int               value,
              bslma::Allocator *basicAllocator = 0 );
        // Create a 'Attribute' object having the specified (literal) 'name'
        // and (32-bit integer) 'value'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator will be used.  Note that 'name' is not
        // managed by this object and therefore must remain valid while in use
        // by any 'Attribute' object.

    Attribute(const char         *name,
              bsls::Types::Int64  value,
              bslma::Allocator   *basicAllocator = 0 );
        // Create a 'Attribute' object having the specified (literal) 'name'
        // and (64-bit integer) 'value'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator will be used.  Note that 'name' is not
        // managed by this object and therefore must remain valid while in use
        // by any 'Attribute' object.

    Attribute(const char               *name,
              const bslstl::StringRef&  value,
              bslma::Allocator         *basicAllocator = 0 );
        // Create a 'Attribute' object having the specified (literal) 'name'
        // and (character string)'value'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator will be used.  Note that
        // 'name' is not managed by this object and therefore must remain valid
        // while in use by any 'Attribute' object.

    Attribute(const char       *name,
              const Value&      value,
              bslma::Allocator *basicAllocator = 0 );
        // Create a 'Attribute' object having the specified (literal) 'name'
        // and 'value'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator will be used.  Note that 'name' is not managed by this
        // object and therefore must remain valid while in use by any
        // 'Attribute' object.

    Attribute(const Attribute&  original,
              bslma::Allocator *basicAllocator = 0);
        // Create a 'Attribute' object having the same (literal) name and
        // attribute value as the specified 'original' object.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator
        // will be used.

    ~Attribute();
        // Destroy this attribute object.

    // MANIPULATORS
    Attribute& operator=(const Attribute& rhs);
        // Assign the value of the specified 'rhs' object to this object.

    void setName(const char *name);
        // Set the attribute name of this object to the specified (literal)
        // 'name'.  Note that 'name' is not managed by this object and
        // therefore must remain valid while in use by any 'Attribute' object.

    void setValue(const Value& value);
    void setValue(int value);
    void setValue(bsls::Types::Int64 value);
    void setValue(const bslstl::StringRef& value);
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
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.
};

// FREE OPERATORS
bool operator==(const Attribute& lhs, const Attribute& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'Attribute' objects have the same
    // value if they have the same name (but not necessarily the identical
    // representation in memory), the same attribute value type, and the same
    // attribute value.

bool operator!=(const Attribute& lhs, const Attribute& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'Attribute' objects do not have
    // the same value if any of their respective names (value, not address),
    // attribute value types, or attribute values differ.

bsl::ostream& operator<<(bsl::ostream& output, const Attribute& attribute);
    // Write the value of the specified 'attribute' to the specified 'output'
    // stream.  Return the specified 'output' stream.

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                        // ---------------
                        // class Attribute
                        // ---------------

// CREATORS
inline
Attribute::Attribute(const char       *name,
                     int               value,
                     bslma::Allocator *basicAllocator)
: d_name(name)
, d_value(basicAllocator)
, d_hashValue(-1)
{
    d_value.assign<int>(value);
}

inline
Attribute::Attribute(const char         *name,
                     bsls::Types::Int64  value,
                     bslma::Allocator   *basicAllocator)
: d_name(name)
, d_value(basicAllocator)
, d_hashValue(-1)
{
    d_value.assign<bsls::Types::Int64>(value);
}

inline
Attribute::Attribute(const char               *name,
                     const bslstl::StringRef&  value,
                     bslma::Allocator         *basicAllocator)
: d_name(name)
, d_value(basicAllocator)
, d_hashValue(-1)
{
    d_value.assign<bsl::string>(bsl::string(value.data(), value.length()));
}

inline
Attribute::Attribute(const char       *name,
                     const Value&      value,
                     bslma::Allocator *basicAllocator)
: d_name(name)
, d_value(value, basicAllocator)
, d_hashValue(-1)
{
}

inline
Attribute::Attribute(const Attribute&  original,
                     bslma::Allocator *basicAllocator)
: d_name(original.d_name)
, d_value(original.d_value, basicAllocator)
, d_hashValue(original.d_hashValue)
, d_hashSize(original.d_hashSize)
{
}

inline
Attribute::~Attribute()
{
}

// MANIPULATORS
inline
Attribute& Attribute::operator=(const Attribute& rhs)
{
    d_name      = rhs.d_name;
    d_value     = rhs.d_value;
    d_hashValue = rhs.d_hashValue;
    d_hashSize  = rhs.d_hashSize;
    return *this;
}

inline
void Attribute::setName(const char *name)
{
    d_name = name;
    d_hashValue = -1;
}

inline
void Attribute::setValue(const Value& value)
{
    d_value = value;
    d_hashValue = -1;
}

inline
void Attribute::setValue(int value)
{
    d_value.assign(value);
    d_hashValue = -1;
}

inline
void Attribute::setValue(bsls::Types::Int64 value)
{
    d_value.assign(value);
    d_hashValue = -1;
}

inline
void Attribute::setValue(const bslstl::StringRef& value)
{
    d_value.assign(bsl::string(value.data(), value.length()));
    d_hashValue = -1;
}

// ACCESSORS
inline
const char *Attribute::name() const
{
    return d_name;
}

inline
const Attribute::Value& Attribute::value() const
{
    return d_value;
}
}  // close package namespace

// FREE OPERATORS
inline
bool ball::operator==(const Attribute& lhs, const Attribute& rhs)
{
    return 0 == bsl::strcmp(lhs.d_name, rhs.d_name)
        && lhs.d_value == rhs.d_value;
}

inline
bool ball::operator!=(const Attribute& lhs, const Attribute& rhs)
{
    return !(lhs == rhs);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
