// ball_predicate.h                                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_PREDICATE
#define INCLUDED_BALL_PREDICATE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a predicate object that consists of a name/value pair.
//
//@CLASSES:
//  ball::Predicate: a predicate in the form of an attribute name/value pair
//
//@SEE_ALSO: ball_predicateset, ball_rule
//
//@DESCRIPTION: This component implements a value-semantic predicate object,
// 'ball::Predicate', that consists of a name and a value.  The value can be an
// 'int', a 64-bit integer, or a 'bsl::string'.  Both the name and value are
// managed by this object.
//
///Usage
///-----
// The following code illustrates how to create predicates:
//..
//    ball::Predicate p1("uuid", 4044457);
//    ball::Predicate p2("name", "Gang Chen");
//..
// The names of the predicates can be found by calling the 'name' method:
//..
//    assert(0 == bsl::strcmp("uuid", p1.name()));
//    assert(0 == bsl::strcmp("name", p2.name()));
//..
// The 'value' method returns a non-modifiable reference to the
// 'bdlb::Variant' object that manages the value of the predicate:
//..
//    assert(true        == p1.value().is<int>());
//    assert(4044457     == p1.value().the<int>());
//    assert(true        == p2.value().is<bsl::string>());
//    assert("Gang Chen" == p2.value().the<bsl::string>());
//..
// Note that the name string that is passed to the constructor of
// 'ball::Predicate' can be safely modified or even destroyed after the
// 'ball::Predicate' object is created.  In the next example, we create a
// temporary buffer to store the name string, and then use the buffer to
// create a predicate.  Once the predicate is created, the content of the
// buffer can be altered without affecting the name of the predicate:
//..
//    char buffer[] = "Hello";
//    ball::Predicate p3(buffer, 1);
//    bsl::strcpy(buffer, "World");
//    assert(0 == bsl::strcmp("Hello", p3.name()));
//..
// The 'ball::Attribute' class also provides a constructor that takes a value
// of type 'ball::Attribute::Value':
//..
//    ball::Attribute::Value value;
//    value.assign<int>(4044457);
//
//    ball::Predicate p4("uuid", value);
//    assert(p4 == p1);
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_ATTRIBUTE
#include <ball_attribute.h>
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

namespace BloombergLP {

namespace ball {
                         // ===============
                         // class Predicate
                         // ===============

class Predicate {
    // This class contains an attribute name and an attribute value, both of
    // which are managed.  A 'Predicate' object contains a 'Attribute'
    // subobject, so that an equivalent 'Attribute' object can be obtained
    // without any overhead when needed.  Since the 'Attribute' subobject does
    // not manage the attribute name, 'Predicate' stores the attribute name in
    // another member variable.

    // DATA
    bsl::string d_nameStr;    // the attribute name

    Attribute   d_attribute;  // the 'Attribute' subobject, whose attribute
                              // name points to 'd_nameStr'

    // FRIENDS
    friend bool operator==(const Predicate&, const Predicate&);
    friend bool operator!=(const Predicate&, const Predicate&);
    friend bsl::ostream& operator<<(bsl::ostream&, const Predicate&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Predicate, bslma::UsesBslmaAllocator);

    // CLASS METHODS
    static int hash(const Predicate& predicate, int size);
        // Return a hash value calculated from the specified 'predicate' using
        // the specified 'size' as the number of slots.  The hash value is
        // guaranteed to be in the range [0 .. size - 1].

    // CREATORS
    Predicate(const bslstl::StringRef&  name,
              int                       value,
              bslma::Allocator         *basicAllocator = 0 );
        // Create a 'Predicate' object having the specified 'name' and 32-bit
        // integer 'value'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    Predicate(const bslstl::StringRef&  name,
              bsls::Types::Int64        value,
              bslma::Allocator         *basicAllocator = 0 );
        // Create a 'Predicate' object having the specified 'name' and 64-bit
        // integer 'value'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    Predicate(const bslstl::StringRef&  name,
              const char               *value,
              bslma::Allocator         *basicAllocator = 0 );
        // Create a 'Predicate' object having the specified 'name' and
        // character string 'value'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    Predicate(const bslstl::StringRef&  name,
              const Attribute::Value&   value,
              bslma::Allocator         *basicAllocator = 0 );
        // Create a 'Attribute' object having the specified (literal) 'name'
        // and 'value'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    Predicate(const Predicate&  original,
              bslma::Allocator *basicAllocator = 0);
        // Create a 'Predicate' object having the same name and attribute
        // value as the specified 'original' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    // MANIPULATORS
    Predicate& operator=(const Predicate& rhs);
        // Assign the value of the specified 'rhs' object to this object.

    void setName(const bslstl::StringRef& name);
        // Set the attribute name of this object to the specified (literal)
        // 'name'.

    void setValue(const Attribute::Value& value);
        // Set the attribute value of this object to the specified 'value'.

    // ACCESSORS
    const char *name() const;
        // Return the name of this object.

    const Attribute::Value& value() const;
        // Return a reference to the non-modifiable attribute value of this
        // object.

    const Attribute& attribute() const;
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

};

// FREE OPERATORS
bool operator==(const Predicate& lhs, const Predicate& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'Predicate' objects have the same
    // value if they have the same name (but not necessarily the identical
    // representation in memory), the same attribute value type, and the same
    // attribute value.

bool operator!=(const Predicate& lhs, const Predicate& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'Predicate' objects do not have
    // the same value if any of their respective names (value, not address),
    // attribute value types, or attribute values differ.

bsl::ostream& operator<<(bsl::ostream&    stream,
                         const Predicate& predicate);
    // Write the value of the specified 'predicate' to the specified output
    // 'stream', and return a reference to the modifiable 'stream'.

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                         // ---------------
                         // class Predicate
                         // ---------------

//CLASS METHODS
inline
int Predicate::hash(const Predicate& predicate, int size)
{
    return Attribute::hash(predicate.d_attribute, size);
}

// CREATORS
inline
Predicate::Predicate(const bslstl::StringRef&  name,
                     int                       value,
                     bslma::Allocator         *basicAllocator)
: d_nameStr(name.data(), name.length(), basicAllocator)
, d_attribute(d_nameStr.c_str(), value, basicAllocator)
{
}

inline
Predicate::Predicate(const bslstl::StringRef&  name,
                     bsls::Types::Int64        value,
                     bslma::Allocator         *basicAllocator)
: d_nameStr(name.data(), name.length(), basicAllocator)
, d_attribute(d_nameStr.c_str(), value, basicAllocator)
{
}

inline
Predicate::Predicate(const bslstl::StringRef&  name,
                     const char               *value,
                     bslma::Allocator         *basicAllocator)
: d_nameStr(name.data(), name.length(), basicAllocator)
, d_attribute(d_nameStr.c_str(), value, basicAllocator)
{
}

inline
Predicate::Predicate(const bslstl::StringRef&  name,
                     const Attribute::Value&   value,
                     bslma::Allocator         *basicAllocator)
: d_nameStr(name.data(), name.length(), basicAllocator)
, d_attribute(d_nameStr.c_str(), value, basicAllocator)
{
}

inline
Predicate::Predicate(const Predicate&  original,
                     bslma::Allocator *basicAllocator)
: d_nameStr(original.d_nameStr, basicAllocator)
, d_attribute(d_nameStr.c_str(), original.d_attribute.value(), basicAllocator)
{
}

// MANIPULATORS
inline
Predicate& Predicate::operator=(const Predicate& rhs)
{
    d_nameStr   = rhs.d_nameStr;
    d_attribute = rhs.d_attribute;
    d_attribute.setName(d_nameStr.c_str());
    return *this;
}

inline
void Predicate::setName(const bslstl::StringRef& name)
{
    d_nameStr.assign(name.data(), name.length());
    d_attribute.setName(d_nameStr.c_str());
}

inline
void Predicate::setValue(const Attribute::Value& value)
{
    d_attribute.setValue(value);
}


// ACCESSORS
inline
const char *Predicate::name() const
{
    return d_attribute.name();
}

inline
const Attribute::Value& Predicate::value() const
{
    return d_attribute.value();
}

inline
const Attribute& Predicate::attribute() const
{
    return d_attribute;
}

}  // close package namespace

// FREE OPERATORS
inline
bool ball::operator==(const Predicate& lhs, const Predicate& rhs)
{
    return lhs.d_nameStr           == rhs.d_nameStr
        && lhs.d_attribute.value() == rhs.d_attribute.value();
}

inline
bool ball::operator!=(const Predicate& lhs, const Predicate& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& ball::operator<<(bsl::ostream&    stream,
                               const Predicate& predicate)
{
    return predicate.print(stream, 0, -1);
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
