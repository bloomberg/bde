// bael_defaultattributecontainer.h                                   -*-C++-*-
#ifndef INCLUDED_BAEL_DEFAULTATTRIBUTECONTAINER
#define INCLUDED_BAEL_DEFAULTATTRIBUTECONTAINER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a default container for storing attribute name/value pairs.
//
//@CLASSES:
//     bael_DefaultAttributeContainer: a collection of unique attributes
//
//@SEE_ALSO: bael_attributecontainer
//
//@AUTHOR: Gang Chen (gchen20), Mike Verschell (hverschell)
//
//@DESCRIPTION: This component provides a default implementation of the
// 'bael_AttributeContainer' protocol, providing a 'hash_set' based container
// of 'bael_Attribute' values.  Each attribute within the default attribute
// container holds a (case-sensitive) name and a value, which may be an 'int',
// a 64-bit integer, or a 'bsl::string'.
//
///Thread Safety
///-------------
// 'bael_DefaultAttributeContainer' is *const* *thread-safe*, meaning that
// accessors may be invoked concurrently from different threads, but it is not
// safe to access or modify a 'bael_DefaultAttributeContainer' in one thread
// while another thread modifies the same object.
//
///Usage
///-----
// The following code snippets illustrate the basic usage of a
// 'bael_DefaultAttributeContainer'.  A 'bael_DefaultAttributeContainer'
// initially has no attributes when created by the default constructor:
//..
//    bael_DefaultAttributeContainer attributeContainer;
//..
// Let's now create some attributes and add them to the attribute map:
//..
//    bael_Attribute a1("uuid", 1111);
//    bael_Attribute a2("sid", "111-1");
//    assert(true == attributeContainer.addAttribute(a1));
//    assert(true == attributeContainer.addAttribute(a2));
//..
// New attributes with a name that already exists in the map can be added, as
// long as they have a different value:
//..
//    bael_Attribute a3("uuid", 2222);
//    bael_Attribute a4("sid", "222-2");
//    assert(true == attributeContainer.addAttribute(a3));
//    assert(true == attributeContainer.addAttribute(a4));
//..
// But attributes having the same name and value cannot be added:
//..
//    bael_Attribute a5("uuid", 1111);                 // same as 'a1'
//    assert(false == attributeContainer.addAttribute(a5));
//..
// Note that the attribute name is case-sensitive:
//..
//    bael_Attribute a6("UUID", 1111);
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
// Finally, the 'bael_DefaultAttributeContainer' class provides an iterator:
//..
//    bael_DefaultAttributeContainer::const_iterator iter =
//                                                  attributeContainer.begin();
//    for ( ; iter != attributeContainer.end(); ++iter ) {
//        bsl::cout << *iter << bsl::endl;
//    }
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEL_ATTRIBUTE
#include <bael_attribute.h>
#endif

#ifndef INCLUDED_BAEL_ATTRIBUTECONTAINER
#include <bael_attributecontainer.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_HASH_SET
#include <bsl_hash_set.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

                    // ====================================
                    // class bael_DefaultAttributeContainer
                    // ====================================

class bael_DefaultAttributeContainer : public bael_AttributeContainer {
    // A 'bael_DefaultAttributeContainer' object contains a collection of
    // (unique) attributes values.

    // PRIVATE TYPES
    struct AttributeHash
        // A hash functor for 'bael_Attribute'
    {
      private:
        static int s_hashtableSize;  // default hashtable size for which the
                                     // hash value is calculated
      public:
        int operator()(const bael_Attribute& attribute) const
            // Return the hash value of the specified 'attribute'.
        {
            return bael_Attribute::hash(attribute, s_hashtableSize);
        }
    };

    // CLASS DATA
    static int                   s_initialSize;   // initial size of the
                                                  // attribute map

    // DATA
    bsl::hash_set<bael_Attribute, AttributeHash>
                                 d_attributeSet;  // hash table that stores
                                                  // all the attributes
                                                  // managed by this object

  public:
    // TYPES
    typedef bsl::hash_set<bael_Attribute, AttributeHash>::const_iterator
                                 const_iterator;  // type of iterator for
                                                  // iterating through the
                                                  // non-modifiable attributes
                                                  // managed by this object

    // CREATORS
    explicit bael_DefaultAttributeContainer(
                                          bslma_Allocator *basicAllocator = 0);
        // Create an empty 'bael_DefaultAttributeContainer' object.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator
        // will be used.

    bael_DefaultAttributeContainer(
                    const bael_DefaultAttributeContainer&  original,
                    bslma_Allocator                       *basicAllocator = 0);
        // Create a 'bael_DefaultAttributeContainer' object having the same
        // value as the specified 'original' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator will be used.

    virtual ~bael_DefaultAttributeContainer();
        // Destroy this object.

    // MANIPULATORS
    bael_DefaultAttributeContainer& operator=(
                                    const bael_DefaultAttributeContainer& rhs);
        // Assign the value of the specified 'rhs' object to this object.

    bool addAttribute(const bael_Attribute& value);
        // Add an attribute having the specified 'value' to this object.
        // Return 'true' on success and 'false' if an attribute having the
        // same value already exists in this object.

    bool removeAttribute(const bael_Attribute& value);
        // Remove the attribute having the specified 'value' from this object.
        // Return the 'true' on success and 'false' if the attribute having
        // the specified 'value' does not exist in this object.

    void removeAllAttributes();
        // Remove every attribute in this attribute set.

    // ACCESSORS
    int numAttributes() const;
        // Return the number of attributes managed by this object.

    virtual bool hasValue(const bael_Attribute& value) const;
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
bool operator==(const bael_DefaultAttributeContainer& lhs,
                const bael_DefaultAttributeContainer& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'bael_DefaultAttributeContainer'
    // objects have the same value if they contain the same number of (unique)
    // attributes, and every attribute that appears in one object also
    // appears in the other.

bool operator!=(const bael_DefaultAttributeContainer& lhs,
                const bael_DefaultAttributeContainer& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two
    // 'bael_DefaultAttributeContainer' objects do not have the same value if
    // they contain differing numbers of attributes or if there is at least
    // one attribute that appears in one object, but not in the other.

bsl::ostream& operator<<(
                 bsl::ostream&                         output,
                 const bael_DefaultAttributeContainer& attributeContainer);
    // Write the value of the specified 'attributeContainer' to the specified
    // 'output' stream in some single-line, human readable format.  Return the
    // specified 'output' stream.

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                    // ------------------------------------
                    // class bael_DefaultAttributeContainer
                    // ------------------------------------

// CREATORS
inline
bael_DefaultAttributeContainer::bael_DefaultAttributeContainer(
                                               bslma_Allocator* basicAllocator)
: d_attributeSet(s_initialSize,                    // initial size
                 AttributeHash(),                  // hash functor
                 bsl::equal_to<bael_Attribute>(),  // equal functor
                 basicAllocator)
{
}

inline
bael_DefaultAttributeContainer::bael_DefaultAttributeContainer(
                        const bael_DefaultAttributeContainer&   original,
                        bslma_Allocator                        *basicAllocator)
: d_attributeSet(original.d_attributeSet,
                 basicAllocator)
{
}

inline
bael_DefaultAttributeContainer::~bael_DefaultAttributeContainer()
{
}

// MANIPULATORS
inline
bool bael_DefaultAttributeContainer::addAttribute(const bael_Attribute& value)
{
    return d_attributeSet.insert(value).second;
}

inline
bool bael_DefaultAttributeContainer::removeAttribute(
                                                 const bael_Attribute& value)
{
    return d_attributeSet.erase(value) != 0;
}

inline
void bael_DefaultAttributeContainer::removeAllAttributes()
{
    d_attributeSet.clear();
}

// ACCESSORS
inline
int bael_DefaultAttributeContainer::numAttributes() const
{
    return static_cast<int>(d_attributeSet.size());
}

inline
bael_DefaultAttributeContainer::const_iterator
bael_DefaultAttributeContainer::begin() const
{
    return d_attributeSet.begin();
}

inline
bael_DefaultAttributeContainer::const_iterator
bael_DefaultAttributeContainer::end() const
{
    return d_attributeSet.end();
}

// FREE OPERATORS
inline
bsl::ostream& operator<<(
                      bsl::ostream&                         output,
                      const bael_DefaultAttributeContainer& attributeContainer)
{
    return attributeContainer.print(output, 0, -1);
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
