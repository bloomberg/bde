// ball_managedattributeset.h                                         -*-C++-*-
#ifndef INCLUDED_BALL_MANAGEDATTRIBUTESET
#define INCLUDED_BALL_MANAGEDATTRIBUTESET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a container for managed attributes.
//
//@CLASSES:
//  ball::ManagedAttributeSet: a container for managed attributes
//
//@SEE_ALSO: ball_managedattribute, ball_rule
//
//@DESCRIPTION: This component implements a value-semantic container class,
// `ball::ManagedAttributeSet`, that manages a set of `ball::ManagedAttribute`
// objects.
//
// This component participates in the implementation of "Rule-Based Logging".
// For more information on how to use that feature, please see the
// package-level documentation and usage examples for "Rule-Based Logging".
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Basic Properties of `ball::ManagedAttributeSet`
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example shows basic operations on a managed attribute set.
//
// First, we create an empty attribute set:
// ```
// ball::ManagedAttributeSet attributeSet;
// ```
// Then, we add two attributes to the attribute set:
// ```
// ball::ManagedAttribute p1("uuid", 4044457);
// assert(attributeSet.addAttribute(p1));
// assert(attributeSet.addAttribute(ball::ManagedAttribute("uuid", 3133246)));
// ```
// Next, we look up (by value) via the `isMember` method:
// ```
// assert(attributeSet.isMember(p1));
// assert(attributeSet.isMember(ball::ManagedAttribute("uuid", 3133246)));
// ```
// Then, we add duplicated value and observe the status of the operation:
// ```
// assert(!attributeSet.addAttribute(ball::ManagedAttribute("uuid",
//                                                          3133246)));
// ```
// Finally, we remove an attribute and check that it is not a member of the
// attribute set:
// ```
// assert(attributeSet.removeAttribute(p1));
// assert(!attributeSet.isMember(p1));
// ```

#include <balscm_version.h>

#include <ball_attribute.h>
#include <ball_managedattribute.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsl_functional.h>
#include <bsl_unordered_set.h>

namespace BloombergLP {
namespace ball {

class AttributeContainerList;

                        // =========================
                        // class ManagedAttributeSet
                        // =========================

/// This class implements a value-semantic collection of unique attributes.
/// (Note that an attribute is a compound entity that, as a whole, must be
/// unique although individual parts need not be.)  Additionally, the
/// `evaluate` accessor can be used to determine if every attribute in the
/// set is present in the specified attribute container list.
class ManagedAttributeSet {

    // PRIVATE TYPES

    /// A hash functor for `ManagedAttribute`.
    struct AttributeHash {

      private:
        // CLASS DATA
        static int s_hashtableSize;  // default hashtable size for which the
                                     // hash value is calculated
      public:
        // ACCESSORS

        /// Return the hash value of the specified `attribute`.
        int operator()(const ManagedAttribute& attribute) const
        {
            return ManagedAttribute::hash(attribute, s_hashtableSize);
        }
    };

    /// This `typedef` is an alias for the container of managed attributes
    /// used by this object.
    typedef bsl::unordered_set<ManagedAttribute, AttributeHash> SetType;

    // CLASS DATA
    static int s_initialSize;     // the initial size of the set

    // DATA
    SetType    d_attributeSet;    // the set of attributes

    // FRIENDS
    friend bool operator==(const ManagedAttributeSet&,
                           const ManagedAttributeSet&);
    friend bool operator!=(const ManagedAttributeSet&,
                           const ManagedAttributeSet&);
    friend bsl::ostream& operator<<(bsl::ostream&, const ManagedAttributeSet&);

  public:
    // TYPES
    typedef bsl::allocator<char> allocator_type;

    typedef SetType::const_iterator const_iterator;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(ManagedAttributeSet,
                                   bslma::UsesBslmaAllocator);

    // CLASS METHODS

    /// Return a hash value calculated from the specified `set` using the
    /// specified `size` as the number of slots.  The hash value is
    /// guaranteed to be in the range `[0 .. size - 1]`.  The behavior is
    /// undefined unless `0 < size`.
    static int hash(const ManagedAttributeSet& set, int size);

    // CREATORS

    /// Create an empty `ManagedAttributeSet` object.  Optionally specify an
    /// `allocator` (e.g., the address of a `bslma::Allocator` object) to
    /// supply memory; otherwise, the default allocator is used.
    ManagedAttributeSet();
    explicit ManagedAttributeSet(const allocator_type& allocator);

    /// Create a `ManagedAttributeSet` object having the same value as the
    /// specified `original` object.  Optionally specify an `allocator`
    /// (e.g., the address of a `bslma::Allocator` object) to supply memory;
    /// otherwise, the default allocator is used.
    ManagedAttributeSet(
                      const ManagedAttributeSet& original,
                      const allocator_type&      allocator = allocator_type());

    //! ~ManagedAttributeSet() = default;
        // Destroy this attribute set.

    // MANIPULATORS

    /// Assign the value of the specified `rhs` to this object, and return a
    /// reference providing modifiable access to this object.
    ManagedAttributeSet& operator=(const ManagedAttributeSet& rhs);

    /// Add an attribute having the specified `value` to this object.
    /// Return `true` on success and `false` if an attribute having the
    /// same `value` already exists in this object.
    bool addAttribute(const ManagedAttribute& value);

    /// Add an attribute having the specified `value` to this object.
    /// Return 1 on success and 0 if an attribute having the same value
    /// already exists in this object.
    /// **DEPRECATED**: Use `addAttribute` instead.
    int addPredicate(const ManagedAttribute& value);

    /// Remove all attributes from this attribute set.
    void removeAll();

    /// **DEPRECATED**: Use `removeAll` instead.
    void removeAllPredicates();

    /// Remove the attribute having the specified `value` from this object.
    /// Return `true` on success and `false` if an attribute having the
    /// `value` does not exist in this object.
    bool removeAttribute(const ManagedAttribute& value);

    /// Remove the attribute having the specified `value` from this object.
    /// Return the number of attributes removed (i.e., 1 on success and 0 if
    /// an attribute having `value` does not exist in this object).
    /// **DEPRECATED**: Use `removeAttribute` instead.
    int removePredicate(const ManagedAttribute& value);

    // ACCESSORS

    /// Return `true` if for every attribute maintained by this object, an
    /// attribute with the same name and value exists in the specified
    /// `containerList`, or if this object has no attributes; otherwise
    /// return `false`.
    bool evaluate(const AttributeContainerList& containerList) const;

    /// Return the allocator used by this object to supply memory.  Note
    /// that if no allocator was supplied at construction the default
    /// allocator in effect at construction is used.
    allocator_type get_allocator() const;

    /// Return `true` if an attribute having specified `value` exists in
    /// this object, and `false` otherwise.
    bool isMember(const ManagedAttribute& value) const;

    /// Return the number of attributes managed by this object.
    int numAttributes() const;

    /// **DEPRECATED**: Use `numAttributes` instead.
    int numPredicates() const;

    /// Return an iterator referring to the first member of this attribute
    /// set.
    const_iterator begin() const;

    /// Return an iterator referring to one past the last member of this
    /// attribute set.
    const_iterator end() const;

    /// Format this object to the specified output `stream` at the (absolute
    /// value of) the optionally specified indentation `level` and return a
    /// reference to `stream`.  If `level` is specified, optionally specify
    /// `spacesPerLevel`, the number of spaces per indentation level for
    /// this and all of its nested objects.  If `level` is negative,
    /// suppress indentation of the first line.  If `spacesPerLevel` is
    /// negative, format the entire output on one line, suppressing all but
    /// the initial indentation (as governed by `level`).  If `stream` is
    /// not valid on entry, this operation has no effect.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` objects have the same
/// value, and `false` otherwise.  Two `ManagedAttributeSet` objects have
/// the same value if they have the same number of attributes and every
/// attribute value that appears in one object also appears in the other.
bool operator==(const ManagedAttributeSet& lhs,
                const ManagedAttributeSet& rhs);

/// Return `true` if the specified `lhs` and `rhs` objects do not have the
/// same value, and `false` otherwise.  Two `ManagedAttributeSet` objects do
/// not have the same value if they do not have the same number of
/// attributes or there is at least one attribute value that appears in one
/// object, but not in the other.
bool operator!=(const ManagedAttributeSet& lhs,
                const ManagedAttributeSet& rhs);

/// Write the value of the specified `attributeSet` to the specified
/// `output` stream.  Return the specified `output` stream.
bsl::ostream& operator<<(bsl::ostream&              output,
                         const ManagedAttributeSet& attributeSet);

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                        // -------------------------
                        // class ManagedAttributeSet
                        // -------------------------

// CREATORS
inline
ManagedAttributeSet::ManagedAttributeSet()
: d_attributeSet(s_initialSize,                      // initial size
                 AttributeHash(),                    // hash functor
                 bsl::equal_to<ManagedAttribute>())  // equal functor
{
}

inline
ManagedAttributeSet::ManagedAttributeSet(const allocator_type& allocator)
: d_attributeSet(s_initialSize,                      // initial size
                 AttributeHash(),                    // hash functor
                 bsl::equal_to<ManagedAttribute>(),  // equal functor
                 allocator.mechanism())
{
}

inline
ManagedAttributeSet::ManagedAttributeSet(const ManagedAttributeSet&  original,
                                         const allocator_type&       allocator)
: d_attributeSet(original.d_attributeSet, allocator)
{
}

// MANIPULATORS
inline
bool ManagedAttributeSet::addAttribute(const ManagedAttribute& value)
{
    return d_attributeSet.insert(value).second;
}

inline
int ManagedAttributeSet::addPredicate(const ManagedAttribute& value)
{
    return addAttribute(value);
}

inline
bool ManagedAttributeSet::removeAttribute(const ManagedAttribute& value)
{
    return !!d_attributeSet.erase(value);
}

inline
int ManagedAttributeSet::removePredicate(const ManagedAttribute& value)
{
    return static_cast<int>(d_attributeSet.erase(value));
}

inline
void ManagedAttributeSet::removeAll()
{
    d_attributeSet.clear();
}

inline
void ManagedAttributeSet::removeAllPredicates()
{
    removeAll();
}

// ACCESSORS
inline
ManagedAttributeSet::allocator_type
ManagedAttributeSet::get_allocator() const
{
    return d_attributeSet.get_allocator();
}

inline
bool ManagedAttributeSet::isMember(const ManagedAttribute& value) const
{
    return d_attributeSet.find(value) != d_attributeSet.end();
}

inline
int ManagedAttributeSet::numAttributes() const
{
    return static_cast<int>(d_attributeSet.size());
}

inline
int ManagedAttributeSet::numPredicates() const
{
    return numAttributes();
}

inline
ManagedAttributeSet::const_iterator ManagedAttributeSet::begin() const
{
    return d_attributeSet.begin();
}

inline
ManagedAttributeSet::const_iterator ManagedAttributeSet::end() const
{
    return d_attributeSet.end();
}

}  // close package namespace

// FREE OPERATORS
inline
bsl::ostream& ball::operator<<(bsl::ostream&              output,
                               const ManagedAttributeSet& attributeSet)
{
    attributeSet.print(output, 0, -1);
    return output;
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
