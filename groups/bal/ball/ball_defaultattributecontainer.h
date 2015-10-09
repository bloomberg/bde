// ball_defaultattributecontainer.h                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALL_DEFAULTATTRIBUTECONTAINER
#define INCLUDED_BALL_DEFAULTATTRIBUTECONTAINER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a default container for storing attribute name/value pairs.
//
//@CLASSES:
//  ball::DefaultAttributeContainer: a collection of unique attributes
//
//@SEE_ALSO: ball_attributecontainer
//
//@DESCRIPTION: This component provides a default implementation of the
// 'ball::AttributeContainer' protocol, 'ball::DefaultAttributeContainer'
// providing an 'unordered_set'-based container of 'ball::Attribute' values.
// Each attribute within the default attribute container holds a
// (case-sensitive) name and a value, which may be an 'int', a 64-bit integer,
// or a 'bsl::string'.
//
///Thread Safety
///-------------
// 'ball::DefaultAttributeContainer' is *const* *thread-safe*, meaning that
// accessors may be invoked concurrently from different threads, but it is not
// safe to access or modify a 'ball::DefaultAttributeContainer' in one thread
// while another thread modifies the same object.
//
///Usage
///-----
// The following code snippets illustrate the basic usage of a
// 'ball::DefaultAttributeContainer'.  A 'ball::DefaultAttributeContainer'
// initially has no attributes when created by the default constructor:
//..
//    ball::DefaultAttributeContainer attributeContainer;
//..
// Let's now create some attributes and add them to the attribute map:
//..
//    ball::Attribute a1("uuid", 1111);
//    ball::Attribute a2("sid", "111-1");
//    assert(true == attributeContainer.addAttribute(a1));
//    assert(true == attributeContainer.addAttribute(a2));
//..
// New attributes with a name that already exists in the map can be added, as
// long as they have a different value:
//..
//    ball::Attribute a3("uuid", 2222);
//    ball::Attribute a4("sid", "222-2");
//    assert(true == attributeContainer.addAttribute(a3));
//    assert(true == attributeContainer.addAttribute(a4));
//..
// But attributes having the same name and value cannot be added:
//..
//    ball::Attribute a5("uuid", 1111);                 // same as 'a1'
//    assert(false == attributeContainer.addAttribute(a5));
//..
// Note that the attribute name is case-sensitive:
//..
//    ball::Attribute a6("UUID", 1111);
//    assert(true == attributeContainer.addAttribute(a6));
//..
// Existing attributes can be looked up by the 'hasValue' method:
//..
//    assert(true == defaultattributecontainer.hasValue(a1));
//    assert(true == defaultattributecontainer.hasValue(a2));
//    assert(true == defaultattributecontainer.hasValue(a3));
//    assert(true == defaultattributecontainer.hasValue(a4));
//    assert(true == defaultattributecontainer.hasValue(a5));
//    assert(true == defaultattributecontainer.hasValue(a6));
//..
// Or removed by the 'removeAttribute' method:
//..
//    defaultattributecontainer.removeAttribute(a1);
//    assert(false == attributeContainer.hasValue(a1));
//..
// Finally, the 'ball::DefaultAttributeContainer' class provides an iterator:
//..
//    ball::DefaultAttributeContainer::const_iterator iter =
//                                                  attributeContainer.begin();
//    for ( ; iter != attributeContainer.end(); ++iter ) {
//        bsl::cout << *iter << bsl::endl;
//    }
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_ATTRIBUTE
#include <ball_attribute.h>
#endif

#ifndef INCLUDED_BALL_ATTRIBUTECONTAINER
#include <ball_attributecontainer.h>
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

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_UNORDERED_SET
#include <bsl_unordered_set.h>
#endif

namespace BloombergLP {

namespace ball {
                    // ===============================
                    // class DefaultAttributeContainer
                    // ===============================

class DefaultAttributeContainer : public AttributeContainer {
    // A 'DefaultAttributeContainer' object contains a collection of (unique)
    // attributes values.

    // PRIVATE TYPES
    struct AttributeHash
        // A hash functor for 'Attribute'
    {
      private:
        static int s_hashtableSize;  // default hashtable size for which the
                                     // hash value is calculated
      public:
        int operator()(const Attribute& attribute) const
            // Return the hash value of the specified 'attribute'.
        {
            return Attribute::hash(attribute, s_hashtableSize);
        }
    };

    // CLASS DATA
    static int                   s_initialSize;   // initial size of the
                                                  // attribute map

    // DATA
    bsl::unordered_set<Attribute, AttributeHash>
                                 d_attributeSet;  // hash table that stores
                                                  // all the attributes
                                                  // managed by this object

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(DefaultAttributeContainer,
                                   bslma::UsesBslmaAllocator);

    // TYPES
    typedef bsl::unordered_set<Attribute, AttributeHash>::const_iterator
                                 const_iterator;  // type of iterator for
                                                  // iterating through the
                                                  // non-modifiable attributes
                                                  // managed by this object

    // CREATORS
    explicit DefaultAttributeContainer(bslma::Allocator *basicAllocator = 0);
        // Create an empty 'DefaultAttributeContainer' object.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator
        // will be used.

    DefaultAttributeContainer(
                    const DefaultAttributeContainer&  original,
                    bslma::Allocator                 *basicAllocator = 0);
        // Create a 'DefaultAttributeContainer' object having the same value as
        // the specified 'original' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator will be used.

    virtual ~DefaultAttributeContainer();
        // Destroy this object.

    // MANIPULATORS
    DefaultAttributeContainer& operator=(
                                    const DefaultAttributeContainer& rhs);
        // Assign the value of the specified 'rhs' object to this object.

    bool addAttribute(const Attribute& value);
        // Add an attribute having the specified 'value' to this object.
        // Return 'true' on success and 'false' if an attribute having the
        // same 'value' already exists in this object.

    bool removeAttribute(const Attribute& value);
        // Remove the attribute having the specified 'value' from this object.
        // Return the 'true' on success and 'false' if the attribute having the
        // 'value' does not exist in this object.

    void removeAllAttributes();
        // Remove every attribute in this attribute set.

    // ACCESSORS
    int numAttributes() const;
        // Return the number of attributes managed by this object.

    virtual bool hasValue(const Attribute& value) const;
        // Return 'true' if the attribute having specified 'value' exists in
        // this object, and 'false' otherwise.

    const_iterator begin() const;
        // Return an iterator pointing at the beginning of the (unordered)
        // sequence of attributes managed by this map, or 'end()' if
        // 'numAttributes()' is 0.

    const_iterator end() const;
        // Return an iterator pointing at one past the end of the map.

    virtual bsl::ostream& print(bsl::ostream& stream,
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
bool operator==(const DefaultAttributeContainer& lhs,
                const DefaultAttributeContainer& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'DefaultAttributeContainer' objects
    // have the same value if they contain the same number of (unique)
    // attributes, and every attribute that appears in one object also appears
    // in the other.

bool operator!=(const DefaultAttributeContainer& lhs,
                const DefaultAttributeContainer& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'DefaultAttributeContainer'
    // objects do not have the same value if they contain differing numbers of
    // attributes or if there is at least one attribute that appears in one
    // object, but not in the other.

bsl::ostream& operator<<(bsl::ostream&                    output,
                         const DefaultAttributeContainer& attributeContainer);
    // Write the value of the specified 'attributeContainer' to the specified
    // 'output' stream in some single-line, human readable format.  Return the
    // 'output' stream.

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                    // -------------------------------
                    // class DefaultAttributeContainer
                    // -------------------------------

// CREATORS
inline
DefaultAttributeContainer::DefaultAttributeContainer(
                                              bslma::Allocator *basicAllocator)
: d_attributeSet(s_initialSize,                    // initial size
                 AttributeHash(),                  // hash functor
                 bsl::equal_to<Attribute>(),       // equal functor
                 basicAllocator)
{
}

inline
DefaultAttributeContainer::DefaultAttributeContainer(
                            const DefaultAttributeContainer&  original,
                            bslma::Allocator                 *basicAllocator)
: d_attributeSet(original.d_attributeSet, basicAllocator)
{
}

inline
DefaultAttributeContainer::~DefaultAttributeContainer()
{
}

// MANIPULATORS
inline
bool DefaultAttributeContainer::addAttribute(const Attribute& value)
{
    return d_attributeSet.insert(value).second;
}

inline
bool DefaultAttributeContainer::removeAttribute(const Attribute& value)
{
    return d_attributeSet.erase(value) != 0;
}

inline
void DefaultAttributeContainer::removeAllAttributes()
{
    d_attributeSet.clear();
}

// ACCESSORS
inline
int DefaultAttributeContainer::numAttributes() const
{
    return static_cast<int>(d_attributeSet.size());
}

inline
DefaultAttributeContainer::const_iterator
DefaultAttributeContainer::begin() const
{
    return d_attributeSet.begin();
}

inline
DefaultAttributeContainer::const_iterator
DefaultAttributeContainer::end() const
{
    return d_attributeSet.end();
}
}  // close package namespace

// FREE OPERATORS
inline
bsl::ostream& ball::operator<<(
                           bsl::ostream&                    output,
                           const DefaultAttributeContainer& attributeContainer)
{
    return attributeContainer.print(output, 0, -1);
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
