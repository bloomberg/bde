// ball_predicateset.h                                                -*-C++-*-
#ifndef INCLUDED_BALL_PREDICATESET
#define INCLUDED_BALL_PREDICATESET

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a container for predicates.
//
//@CLASSES:
//    ball::PredicateSet: a container for predicates
//
//@SEE_ALSO: ball_predicate, ball_rule
//
//@AUTHOR: Gang Chen (gchen20)
//
//@DESCRIPTION: This component implements a value-semantic container class
// that manages a set of 'ball::Predicate' objects.
//
///Usage
///-----
// The following code fragments illustrate how to work with a predicate set.
//
// We first create an empty predicate set:
//..
//  ball::PredicateSet predicateSet;
//..
// We then add two predicates to the predicate set:
//..
//  ball::Predicate p1("uuid", 4044457);
//  assert(1 == predicateSet.addPredicate(p1));
//  assert(1 == predicateSet.addPredicate(ball::Predicate("uuid", 3133246)));
//..
// Predicates can be looked up (by value) via the 'isMember' method:
//..
//  assert(true == predicateSet.isMember(p1));
//  assert(true == predicateSet.isMember(ball::Predicate("uuid", 3133246)));
//..
// Predicate values in a predicate set are unique:
//..
//  assert(0 == predicateSet.addPredicate(ball::Predicate("uuid", 3133246)));
//..
// Predicates can also be removed from the predicate set by the
// 'removePredicate' method:
//..
//  assert(1 == predicateSet.removePredicate(p1));
//  assert(false == predicateSet.isMember(p1));
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_ATTRIBUTE
#include <ball_attribute.h>
#endif

#ifndef INCLUDED_BALL_PREDICATE
#include <ball_predicate.h>
#endif

#ifndef INCLUDED_BDLXXXX_INSTREAMFUNCTIONS
#include <bdlxxxx_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDLXXXX_OUTSTREAMFUNCTIONS
#include <bdlxxxx_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_UNORDERED_SET
#include <bsl_unordered_set.h>
#endif

namespace BloombergLP {


namespace ball {class AttributeContainerList;

                   // =======================
                   // class PredicateSet
                   // =======================

class PredicateSet {
    // This class implements a value-semantic collection of unique predicates.
    // (Note that a predicate is a compound entity that, as a whole, must be
    // unique although individual parts need not be.)  Additionally, the
    // 'evaluate' accessor can be used to determine if every predicate in the
    // set evaluates to 'true' in the context of a given attribute map.

    // PRIVATE TYPES
    struct PredicateHash
        // A hash functor for 'Predicate'
    {
      private:
        static int s_hashtableSize;  // default hashtable size for which the
                                     // hash value is calculated
      public:
        int operator()(const Predicate& predicate) const
            // Return the hash value of the specified 'predicate'.
        {
            return Predicate::hash(predicate, s_hashtableSize);
        }
    };

    typedef bsl::unordered_set<Predicate, PredicateHash> SetType;

    // PRIVATE MEMBERS
    static int               s_initialSize;     // the initial size of the set

    SetType                  d_predicateSet;    // the set of predicates

    // FRIENDS
    friend bool operator==(const PredicateSet&, const PredicateSet&);
    friend bool operator!=(const PredicateSet&, const PredicateSet&);
    friend bsl::ostream& operator<<(bsl::ostream&, const PredicateSet&);

  public:
    // CLASS METHODS
    static int hash(const PredicateSet& set, int size);
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
    explicit PredicateSet(bslma::Allocator *basicAllocator = 0);
        // Create an empty 'PredicateSet' object.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator will be used.

    PredicateSet(const PredicateSet&  original,
                      bslma::Allocator         *basicAllocator = 0);
        // Create a 'PredicateSet' object having the same value as the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator will be used.

    ~PredicateSet();
        // Destroy this predicate set.

    // MANIPULATORS
    PredicateSet& operator=(const PredicateSet& rhs);
        // Assign the value of the specified 'rhs' object to this object.

    int addPredicate(const Predicate& value);
        // Add a predicate having the specified 'value' to this object.
        // Return 1 on success and 0 if a predicate having the same value
        // already exists in this object.

    int removePredicate(const Predicate& value);
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
    bool evaluate(const AttributeContainerList& containerList) const;
        // Return 'true' if for every predicate maintained by this object, an
        // attribute with the same name and value exists in the specified
        // 'containerList', or if this object has no predicates; otherwise
        // return 'false'.

    int numPredicates() const;
        // Return the number of predicates managed by this object.

    bool isMember(const Predicate& value) const;
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
bool operator==(const PredicateSet& lhs,
                const PredicateSet& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'PredicateSet' objects have the
    // same value if they have the same number of predicates and every
    // predicate value that appears in one object also appears in the other.

bool operator!=(const PredicateSet& lhs,
                const PredicateSet& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'PredicateSet' objects do
    // not have the same value if they do not have the same number of
    // predicates or there is at least one predicate value that appears in
    // one object, but not in the other.

bsl::ostream& operator<<(bsl::ostream&            output,
                         const PredicateSet& predicateSet);
    // Write the value of the specified 'predicateSet' to the specified
    // 'output' stream.  Return the specified 'output' stream.

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                   // -----------------------
                   // class PredicateSet
                   // -----------------------

// CLASS METHODS
inline
int PredicateSet::maxSupportedBdexVersion()
{
    return 1;  // Required by BDE policy; versions start at 1.
}

// CREATORS
inline
PredicateSet::PredicateSet(bslma::Allocator *basicAllocator)
: d_predicateSet(s_initialSize,                        // initial size
                 PredicateHash(),                      // hash functor
                 bsl::equal_to<Predicate>(),      // equal functor
                 basicAllocator)
{
}

inline
PredicateSet::PredicateSet(const PredicateSet&  original,
                                     bslma::Allocator         *basicAllocator)
: d_predicateSet(original.d_predicateSet, basicAllocator)
{
}

inline
PredicateSet::~PredicateSet()
{
}

// MANIPULATORS
inline
int PredicateSet::addPredicate(const Predicate& value)
{
    return d_predicateSet.insert(value).second;
}

inline
int PredicateSet::removePredicate(const Predicate& value)
{
    return static_cast<int>(d_predicateSet.erase(value));
}

inline
void PredicateSet::removeAllPredicates()
{
    d_predicateSet.clear();
}

template <class STREAM>
STREAM& PredicateSet::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {
          case 1: {
            int size;
            Predicate predicate("", 0);  // no default ctor; dummy data

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
int PredicateSet::numPredicates() const
{
    return static_cast<int>(d_predicateSet.size());
}

inline
bool PredicateSet::isMember(const Predicate& value)
const
{
    return d_predicateSet.find(value) != d_predicateSet.end();
}

inline
PredicateSet::const_iterator PredicateSet::begin() const
{
    return d_predicateSet.begin();
}

inline
PredicateSet::const_iterator PredicateSet::end() const
{
    return d_predicateSet.end();
}

template <class STREAM>
STREAM& PredicateSet::bdexStreamOut(STREAM& stream, int version) const
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
}  // close package namespace

// FREE OPERATORS
inline
bsl::ostream& ball::operator<<(bsl::ostream&            output,
                         const PredicateSet& predicateSet)
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
