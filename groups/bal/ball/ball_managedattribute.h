// ball_managedattribute.h                                            -*-C++-*-
#ifndef INCLUDED_BALL_MANAGEDATTRIBUTE
#define INCLUDED_BALL_MANAGEDATTRIBUTE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a wrapper for 'ball::Attribute' with managed name storage.
//
//@CLASSES:
//  ball::ManagedAttribute: wrapper for 'ball::Attribute' with managed storage
//
//@SEE_ALSO: ball_attribute
//
//@DESCRIPTION: This component implements a wrapper for 'ball::Attribute',
// 'ball::ManagedAttribute', that manages the lifetime of the attribute name.
// Note that 'ball::Attribute' does *not* manage the lifetime of its name (see
// {'ball_attribute'}).
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Basic Properties of 'ball::ManagedAttribute'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example creates 'ball::ManagedAttribute' objects and shows basic
// properties of those objects:
//..
//  ball::ManagedAttribute p1("uuid", 4044457);
//  ball::ManagedAttribute p2("name", "Bloomberg");
//
//  assert("uuid" == p1.key());
//  assert("name" == p2.key());
//
//  assert(true        == p1.value().is<int>());
//  assert(4044457     == p1.value().the<int>());
//  assert(true        == p2.value().is<bsl::string>());
//  assert("Bloomberg" == p2.value().the<bsl::string>());
//..
// Finally, we show that 'ball::ManagedAttribute' manages the storage for the
// attribute name after construction:
//..
//  char buffer[] = "Hello";
//  ball::ManagedAttribute p3(buffer, 1);
//  bsl::strcpy(buffer, "World");
//  assert("Hello" == p3.key()));
//..

#include <balscm_version.h>

#include <ball_attribute.h>

#include <bdlb_variant.h>

#include <bslma_allocator.h>
#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_keyword.h>
#include <bsls_types.h>

#include <bsl_string.h>

namespace BloombergLP {
namespace ball {

                        // ======================
                        // class ManagedAttribute
                        // ======================

class ManagedAttribute {
    // A 'ball::ManagedAttribute' object contains a 'ball::Attribute' object
    // and provides storage for the attribute's name.

  private:
    // DATA
    bsl::string  d_name;        // storage for attribute's name
    Attribute    d_attribute;   // attribute

    // FRIENDS
    friend bool operator==(const ManagedAttribute&, const ManagedAttribute&);
    friend bool operator!=(const ManagedAttribute&, const ManagedAttribute&);
    friend bsl::ostream& operator<<(bsl::ostream&, const ManagedAttribute&);

  public:
    // TYPES
    typedef bsl::allocator<char> allocator_type;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(ManagedAttribute,
                                   bslma::UsesBslmaAllocator);

    // CLASS METHODS
    static int hash(const ManagedAttribute& attribute, int size);
        // Return a hash value calculated from the specified 'attribute' using
        // the specified 'size' as the number of slots.  The hash value is
        // guaranteed to be in the range '[0 .. size - 1]'.  The behavior is
        // undefined unless '0 < size'.

    // CREATORS
    explicit ManagedAttribute(
                           const Attribute&      attribute,
                           const allocator_type& allocator = allocator_type());
        // Create a 'ManagedAttribute' object having the value of the specified
        // 'attribute'.  Optionally specify an 'allocator' (e.g., the address
        // of a 'bslma::Allocator' object) to supply memory; otherwise, the
        // default allocator is used.

    ManagedAttribute(const bsl::string_view& name,
                     const bsl::string_view& value,
                     const allocator_type&   allocator = allocator_type());
        // Create a 'ManagedAttribute' object having the specified 'name' and
        // string 'value'.  Optionally specify an 'allocator' (e.g., the
        // address of a 'bslma::Allocator' object) to supply memory; otherwise,
        // the default allocator is used.

    ManagedAttribute(const bsl::string_view&  name,
                     const char              *value,
                     const allocator_type&    allocator = allocator_type());
        // Create a 'ManagedAttribute' object having the specified 'name' and
        // the C-style 'value' string.  Optionally specify an 'allocator'
        // (e.g., the address of a 'bslma::Allocator' object) to supply memory;
        // otherwise, the default allocator is used.

    ManagedAttribute(const bsl::string_view&  name,
                     int                      value,
                     const allocator_type&    allocator = allocator_type());
    ManagedAttribute(const bsl::string_view&  name,
                     long                     value,
                     const allocator_type&    allocator = allocator_type());
    ManagedAttribute(const bsl::string_view&  name,
                     long long                value,
                     const allocator_type&    allocator = allocator_type());
    ManagedAttribute(const bsl::string_view&  name,
                     unsigned int             value,
                     const allocator_type&    allocator = allocator_type());
    ManagedAttribute(const bsl::string_view&  name,
                     unsigned long            value,
                     const allocator_type&    allocator = allocator_type());
    ManagedAttribute(const bsl::string_view&  name,
                     unsigned long long       value,
                     const allocator_type&    allocator = allocator_type());
        // Create a 'ManagedAttribute' object having the specified 'name' and
        // 'value'.  Optionally specify an 'allocator' (e.g., the address of a
        // 'bslma::Allocator' object) to supply memory; otherwise, the default
        // allocator is used.

    ManagedAttribute(const bsl::string_view&  name,
                     const void              *value,
                     const allocator_type&    allocator = allocator_type());
        // Create a 'ManagedAttribute' object having the specified 'name' and
        // the pointer to the specified 'value' of cv-qualified 'void' type.
        // Optionally specify an 'allocator' (e.g., the address of a
        // 'bslma::Allocator' object) to supply memory; otherwise, the default
        // allocator is used.

    ManagedAttribute(const bsl::string_view& name,
                     const Attribute::Value& value,
                     const allocator_type&   allocator = allocator_type());
        // Create a 'ManagedAttribute' object having the specified 'name' and
        // 'value'.  Optionally specify an 'allocator' (e.g., the address of a
        // 'bslma::Allocator' object) to supply memory; otherwise, the default
        // allocator is used.

    ManagedAttribute(const ManagedAttribute&  original,
                     const allocator_type&    allocator = allocator_type());
        // Create a 'ManagedAttribute' object having the same value as the
        // specified 'original' object.  Optionally specify an 'allocator'
        // (e.g., the address of a 'bslma::Allocator' object) to supply memory;
        // otherwise, the default allocator is used.

    //! ~ManagedAttribute() = default;
        // Destroy this object.

    // MANIPULATORS
    ManagedAttribute& operator=(const ManagedAttribute& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a non-'const' reference to this object.

    void setName(const bsl::string_view& name);
        // Set the attribute name of this object to the specified 'name'.

    void setValue(int                      value);
    void setValue(long                     value);
    void setValue(long long                value);
    void setValue(unsigned int             value);
    void setValue(unsigned long            value);
    void setValue(unsigned long long       value);
    void setValue(const bsl::string_view&  value);
    void setValue(const Attribute::Value&  value);
    void setValue(const char              *value);
    void setValue(const void              *value);
        // Set the attribute value of this object to the specified 'value'.

    // ACCESSORS
    const Attribute& attribute() const;
        // Return a 'const' reference to the attribute of this object.

    allocator_type get_allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the default
        // allocator in effect at construction is used.

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

    const bsl::string& key() const;
        // Return a 'const' reference to the attribute name of this object.

    const char *name() const;
        // Return the attribute name of this object.  Note that this accessor
        // should not be used to get the attribute name if the name string
        // contains embedded zeros.
        //
        // !DEPRECATED!: Use 'key()' instead.

    const Attribute::Value& value() const;
        // Return a 'const' reference to the attribute value of this object.
};

// FREE OPERATORS
bool operator==(const ManagedAttribute& lhs, const ManagedAttribute& rhs);
bool operator==(const ManagedAttribute& lhs, const Attribute& rhs);
bool operator==(const Attribute& lhs, const ManagedAttribute& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'ManagedAttribute' objects have the
    // same value if they have the same name, same attribute value type, and
    // the same attribute value.

bool operator!=(const ManagedAttribute& lhs, const ManagedAttribute& rhs);
bool operator!=(const ManagedAttribute& lhs, const Attribute& rhs);
bool operator!=(const Attribute& lhs, const ManagedAttribute& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'ManagedAttribute' objects do
    // not have the same value if any of their respective names, attribute
    // value types, or attribute values differ.

bsl::ostream& operator<<(bsl::ostream&           output,
                         const ManagedAttribute& attribute);
    // Write the value of the specified 'attribute' to the specified 'output'
    // stream.  Return the specified 'output' stream.

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                        // ----------------------
                        // class ManagedAttribute
                        // ----------------------

//CLASS METHODS
inline
int ManagedAttribute::hash(const ManagedAttribute& attribute, int size)
{
    return Attribute::hash(attribute.d_attribute, size);
}

// CREATORS
inline
ManagedAttribute::ManagedAttribute(const Attribute&      attribute,
                                   const allocator_type& allocator)
: d_name(attribute.name(), allocator)
, d_attribute(d_name.c_str(), attribute.value(), allocator)
{
}

inline
ManagedAttribute::ManagedAttribute(const bsl::string_view&  name,
                                   const bsl::string_view&  value,
                                   const allocator_type&    allocator)
: d_name(name, allocator)
, d_attribute(d_name.c_str(), value, allocator)
{
}

inline
ManagedAttribute::ManagedAttribute(const bsl::string_view&  name,
                                   const char              *value,
                                   const allocator_type&    allocator)
: d_name(name, allocator)
, d_attribute(d_name.c_str(), value, allocator)
{
}

inline
ManagedAttribute::ManagedAttribute(const bsl::string_view&  name,
                                   int                      value,
                                   const allocator_type&    allocator)
: d_name(name, allocator)
, d_attribute(d_name.c_str(), value, allocator)
{
}

inline
ManagedAttribute::ManagedAttribute(const bsl::string_view&  name,
                                   long                     value,
                                   const allocator_type&    allocator)
: d_name(name, allocator)
, d_attribute(d_name.c_str(), value, allocator)
{
}

inline
ManagedAttribute::ManagedAttribute(const bsl::string_view&  name,
                                   long long                value,
                                   const allocator_type&    allocator)
: d_name(name, allocator)
, d_attribute(d_name.c_str(), value, allocator)
{
}

inline
ManagedAttribute::ManagedAttribute(const bsl::string_view&  name,
                                   unsigned int             value,
                                   const allocator_type&    allocator)
: d_name(name, allocator)
, d_attribute(d_name.c_str(), value, allocator)
{
}

inline
ManagedAttribute::ManagedAttribute(const bsl::string_view&  name,
                                   unsigned long            value,
                                   const allocator_type&    allocator)
: d_name(name, allocator)
, d_attribute(d_name.c_str(), value, allocator)
{
}

inline
ManagedAttribute::ManagedAttribute(const bsl::string_view&  name,
                                   unsigned long long       value,
                                   const allocator_type&    allocator)
: d_name(name, allocator)
, d_attribute(d_name.c_str(), value, allocator)
{
}

inline
ManagedAttribute::ManagedAttribute(const bsl::string_view&  name,
                                   const void              *value,
                                   const allocator_type&    allocator)
: d_name(name, allocator)
, d_attribute(d_name.c_str(), value, allocator)
{
}

inline
ManagedAttribute::ManagedAttribute(const bsl::string_view&  name,
                                   const Attribute::Value&  value,
                                   const allocator_type&    allocator)
: d_name(name, allocator)
, d_attribute(d_name.c_str(), value, allocator)
{
}

inline
ManagedAttribute::ManagedAttribute(const ManagedAttribute&  original,
                                   const allocator_type&    allocator)
: d_name(original.d_name, allocator)
, d_attribute(d_name.c_str(), original.d_attribute.value(), allocator)
{
}

// MANIPULATORS
inline
ManagedAttribute& ManagedAttribute::operator=(const ManagedAttribute& rhs)
{
    d_name       = rhs.d_name;
    d_attribute.setName(d_name.c_str());
    d_attribute.setValue(rhs.d_attribute.value());
    return *this;
}

inline
void ManagedAttribute::setName(const bsl::string_view& name)
{
    d_name.assign(name);
    d_attribute.setName(d_name.c_str());
}

inline
void ManagedAttribute::setValue(int value)
{
    d_attribute.setValue(value);
}

inline
void ManagedAttribute::setValue(long value)
{
    d_attribute.setValue(value);
}

inline
void ManagedAttribute::setValue(long long value)
{
    d_attribute.setValue(value);
}

inline
void ManagedAttribute::setValue(unsigned int value)
{
    d_attribute.setValue(value);
}

inline
void ManagedAttribute::setValue(unsigned long value)
{
    d_attribute.setValue(value);
}

inline
void ManagedAttribute::setValue(unsigned long long value)
{
    d_attribute.setValue(value);
}

inline
void ManagedAttribute::setValue(const bsl::string_view& value)
{
    d_attribute.setValue(value);
}

inline
void ManagedAttribute::setValue(const Attribute::Value& value)
{
    d_attribute.setValue(value);
}

inline
void ManagedAttribute::setValue(const char *value)
{
    d_attribute.setValue(value);
}

inline
void ManagedAttribute::setValue(const void *value)
{
    d_attribute.setValue(value);
}

// ACCESSORS
inline
const Attribute& ManagedAttribute::attribute() const
{
    return d_attribute;
}

inline
const bsl::string& ManagedAttribute::key() const
{
    return d_name;
}

inline
const char *ManagedAttribute::name() const
{
    return d_attribute.name();
}

inline
const Attribute::Value& ManagedAttribute::value() const
{
    return d_attribute.value();
}

                                  // Aspects

inline
ManagedAttribute::allocator_type ManagedAttribute::get_allocator() const
{
    return d_name.get_allocator();
}

}  // close package namespace

// FREE OPERATORS
inline
bool ball::operator==(const ManagedAttribute& lhs,
                      const ManagedAttribute& rhs)
{
    return (lhs.d_name == rhs.d_name
        && lhs.d_attribute.value() == rhs.d_attribute.value());
}

inline
bool ball::operator==(const ManagedAttribute& lhs,
                      const Attribute&        rhs)
{
    return (lhs.key() == rhs.name() && lhs.value() == rhs.value());
}

inline
bool ball::operator==(const Attribute&        lhs,
                      const ManagedAttribute& rhs)
{
    return (lhs.name() == rhs.key() && lhs.value() == rhs.value());
}

inline
bool ball::operator!=(const ManagedAttribute& lhs,
                      const ManagedAttribute& rhs)
{
    return !(lhs == rhs);
}

inline
bool ball::operator!=(const ManagedAttribute& lhs,
                      const Attribute&        rhs)
{
    return !(lhs == rhs);
}

inline
bool ball::operator!=(const Attribute&        lhs,
                      const ManagedAttribute& rhs)
{
    return !(lhs == rhs);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
