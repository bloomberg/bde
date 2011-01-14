// bael_predicateset.h                                                -*-C++-*-
#ifndef INCLUDED_BAEL_PREDICATESET
#define INCLUDED_BAEL_PREDICATESET

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a container for predicates.
//
//@CLASSES:
//    bael_PredicateSet: a container for predicates
//
//@SEE_ALSO: bael_predicate, bael_rule
//
//@AUTHOR: Gang Chen (gchen20)
//
//@DESCRIPTION: This component implements a value-semantic container class
// that manages a set of 'bael_Predicate' objects.
//
///Usage
///-----
// The following code fragments illustrate how to work with a predicate set.
//
// We first create an empty predicate set:
//..
//  bael_PredicateSet predicateSet;
//..
// We then add two predicates to the predicate set:
//..
//  bael_Predicate p1("uuid", 4044457);
//  assert(1 == predicateSet.addPredicate(p1));
//  assert(1 == predicateSet.addPredicate(bael_Predicate("uuid", 3133246)));
//..
// Predicates can be looked up (by value) via the 'isMember' method:
//..
//  assert(true == predicateSet.isMember(p1));
//  assert(true == predicateSet.isMember(bael_Predicate("uuid", 3133246)));
//..
// Predicate values in a predicate set are unique:
//..
//  assert(0 == predicateSet.addPredicate(bael_Predicate("uuid", 3133246)));
//..
// Predicates can also be removed from the predicate set by the
// 'removePredicate' method:
//..
//  assert(1 == predicateSet.removePredicate(p1));
//  assert(false == predicateSet.isMember(p1));
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEL_ATTRIBUTE
#include <bael_attribute.h>
#endif

#ifndef INCLUDED_BAEL_PREDICATE
#include <bael_predicate.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_HASH_SET
#include <bsl_hash_set.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

class bael_AttributeContainerList;

                   // =======================
                   // class bael_PredicateSet
                   // =======================

class bael_PredicateSet {
    // This class implements a value-semantic collection of unique predicates.
    // (Note that a predicate is a compound entity that, as a whole, must be
    // unique although individual parts need not be.)  Additionally, the
    // 'evaluate' accessor can be used to determine if every predicate in the
    // set evaluates to 'true' in the context of a given attribute map.

    // PRIVATE TYPES
    struct PredicateHash
        // A hash functor for 'bael_Predicate'
    {
      private:
        static int s_hashtableSize;  // default hashtable size for which the
                                     // hash value is calculated
      public:
        int operator()(const bael_Predicate& predicate) const
            // Return the hash value of the specified 'predicate'.
        {
            return bael_Predicate::hash(predicate, s_hashtableSize);
        }
    };

    typedef bsl::hash_set<bael_Predicate, PredicateHash> SetType;

    // PRIVATE MEMBERS
    static int               s_initialSize;     // the initial size of the set

    SetType                  d_predicateSet;    // the set of predicates

    // FRIENDS
    friend bool operator==(const bael_PredicateSet&, const bael_PredicateSet&);
    friend bool operator!=(const bael_PredicateSet&, const bael_PredicateSet&);
    friend bsl::ostream& operator<<(bsl::ostream&, const bael_PredicateSet&);

  public:
    // CLASS METHODS
    static int hash(const bael_PredicateSet& set, int size);
        // Return a hash value calculated from the specified 'set' using the
        // specified 'size' as the number of slots.  The hash value is
        // guaranteed to be in the range [0, size).

    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported
        // by this class.  (See the 'bde' package-group-level documentation
        // for more information on 'bdex' streaming of value-semantic types
        // and containers.)

    // TYPES
    typedef SetType::const_iterator const_iterator;

    // CREATORS
    explicit bael_PredicateSet(bslma_Allocator *basicAllocator = 0);
        // Create an empty 'bael_PredicateSet' object.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator will be used.

    bael_PredicateSet(const bael_PredicateSet&  original,
                      bslma_Allocator          *basicAllocator = 0);
        // Create a 'bael_PredicateSet' object having the same value as the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator will be used.

    ~bael_PredicateSet();
        // Destroy this predicate set.

    // MANIPULATORS
    bael_PredicateSet& operator=(const bael_PredicateSet& rhs);
        // Assign the value of the specified 'rhs' object to this object.

    int addPredicate(const bael_Predicate& value);
        // Add a predicate having the specified 'value' to this object.
        // Return 1 on success and 0 if a predicate having the same value
        // already exists in this object.

    int removePredicate(const bael_Predicate& value);
        // Remove the predicate having the specified 'value' from this object.
        // Return the number of predicates being removed (i.e., 1 on success
        // and 0 if the predicate having the specified 'value' does not exist
        // in this object).

    void removeAllPredicates();
        // Remove every predicate in this predicate set.

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

    // ACCESSORS
    bool evaluate(const bael_AttributeContainerList& containerList) const;
        // Return 'true' if for every predicate maintained by this object, an
        // attribute with the same name and value exists in the specified
        // 'containerList', or if this object has no predicates; otherwise
        // return 'false'.

    int numPredicates() const;
        // Return the number of predicates managed by this object.

    bool isMember(const bael_Predicate& value) const;
        // Return 'true' if the predicate having specified 'value' exists in
        // this object, and 'false' otherwise.

    const_iterator begin() const;
        // Return an iterator pointing at the first member of the predicate
        // set.

    const_iterator end() const;
        // Return an iterator pointing at one past the last member of the
        // predicate set.

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
bool operator==(const bael_PredicateSet& lhs,
                const bael_PredicateSet& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'bael_PredicateSet' objects have the
    // same value if they have the same number of predicates and every
    // predicate value that appears in one object also appears in the other.

bool operator!=(const bael_PredicateSet& lhs,
                const bael_PredicateSet& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'bael_PredicateSet' objects do
    // not have the same value if they do not have the same number of
    // predicates or there is at least one predicate value that appears in
    // one object, but not in the other.

bsl::ostream& operator<<(bsl::ostream&            output,
                         const bael_PredicateSet& predicateSet);
    // Write the value of the specified 'predicateSet' to the specified
    // 'output' stream.  Return the specified 'output' stream.

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                   // -----------------------
                   // class bael_PredicateSet
                   // -----------------------

// CLASS METHODS
inline
int bael_PredicateSet::maxSupportedBdexVersion()
{
    return 1;  // Required by BDE policy; versions start at 1.
}

// CREATORS
inline
bael_PredicateSet::bael_PredicateSet(bslma_Allocator *basicAllocator)
: d_predicateSet(s_initialSize,                        // initial size
                 PredicateHash(),                      // hash functor
                 bsl::equal_to<bael_Predicate>(),      // equal functor
                 basicAllocator)
{
}

inline
bael_PredicateSet::bael_PredicateSet(const bael_PredicateSet&   original,
                                     bslma_Allocator           *basicAllocator)
: d_predicateSet(original.d_predicateSet, basicAllocator)
{
}

inline
bael_PredicateSet::~bael_PredicateSet()
{
}

// MANIPULATORS
inline
int bael_PredicateSet::addPredicate(const bael_Predicate& value)
{
    return d_predicateSet.insert(value).second;
}

inline
int bael_PredicateSet::removePredicate(const bael_Predicate& value)
{
    return static_cast<int>(d_predicateSet.erase(value));
}

inline
void bael_PredicateSet::removeAllPredicates()
{
    d_predicateSet.clear();
}

template <class STREAM>
STREAM& bael_PredicateSet::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            int size;
            bael_Predicate predicate("", 0);  // no default ctor; dummy data

            bdex_InStreamFunctions::streamIn(stream, size, 0);
            if (!stream) {
                return stream;                                        // RETURN
            }

            if (0 > size) {
                stream.invalidate();
                return stream;                                        // RETURN
            }

            removeAllPredicates();
            for (int i = 0; i < size; ++i) {
                bdex_InStreamFunctions::streamIn(stream, predicate, 1);
                if (!stream) {
                    return stream;                                    // RETURN
                }

                addPredicate(predicate);
            }
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
int bael_PredicateSet::numPredicates() const
{
    return static_cast<int>(d_predicateSet.size());
}

inline
bool bael_PredicateSet::isMember(const bael_Predicate& value)
const
{
    return d_predicateSet.find(value) != d_predicateSet.end();
}

inline
bael_PredicateSet::const_iterator bael_PredicateSet::begin() const
{
    return d_predicateSet.begin();
}

inline
bael_PredicateSet::const_iterator bael_PredicateSet::end() const
{
    return d_predicateSet.end();
}

template <class STREAM>
STREAM& bael_PredicateSet::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        bdex_OutStreamFunctions::streamOut(stream,
                                           (int)d_predicateSet.size(),
                                           0);
        for (const_iterator iter = d_predicateSet.begin();
             iter != d_predicateSet.end();
             ++iter) {
            bdex_OutStreamFunctions::streamOut(stream, *iter, 1);
        }
      } break;
    }
    return stream;
}

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream&            output,
                         const bael_PredicateSet& predicateSet)
{
    predicateSet.print(output, 0, -1);
    return output;
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
