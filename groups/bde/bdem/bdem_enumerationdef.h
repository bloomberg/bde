// bdem_enumerationdef.h                                              -*-C++-*-
#ifndef INCLUDED_BDEM_ENUMERATIONDEF
#define INCLUDED_BDEM_ENUMERATIONDEF

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a definition for a set of enumerated values.
//
//@CLASSES:
//           bdem_EnumerationDef: a definition for a set of enumerated values
//  bdem_EnumerationDefIterator: an iterator over an enumeration definition
//
//@SEE_ALSO: bdem_schema
//
//@AUTHOR: Tom Marshall, Henry Verschell
//
//@DESCRIPTION: This component provides a collaborative type,
// 'bdem_EnumerationDef', that is an integral part of a fully-value semantic
// schema object.  A 'bdem_EnumerationDef' object defines an enumeration
// (i.e., a set of enumerated values).  The values in a enumeration are (id,
// name) pairs, where the id is an integer, and the name is a null-terminated
// C-string.  Both the id and name must be unique within a single enumeration.
// In general, clients should *not* construct a 'bdem_EnumerationDef' directly,
// but instead obtain an enumeration definition from a 'bdem_Schema' (see
// 'bdem_schema' for more information).  Note that an enumeration definition
// can be used to constrain the values of a field of type 'STRING', 'INT',
// 'STRING_ARRAY', or 'INT_ARRAY' in a record (see 'bdem_recorddef' for
// more information on constraints).
//
///Usage
///-----
// The following usage examples demonstrate how one could construct,
// manipulate, and access a enumeration definition in isolation.  In general,
// clients should *not* create a 'bdem_Enumeration' directly, but obtain one
// from a schema (see 'bdem_schema').
//
///Directly Creating a 'bdem_RecordDef' (*Not Recommended*)
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following section demonstrates how to create a 'bdem_EnumerationDef' in
// isolation -- this is *not* the recommended usage of 'bdem_EnumerationDef'.
// Clients should not create a 'bdem_EnumerationDef' directly, but obtain one
// from a schema (see 'bdem_schema').  For the purpose of this usage example,
// we define a dummy stand-in type for 'bdem_Schema'.  This type is used in
// name-only by 'bdem_EnumerationDef', so its definition is not necessary:
//..
//  namespace BloombergLP {
//
//  class bdem_Schema {
//      // Dummy type.
//  };
//
//  }
//..
// Now we create an enumeration definition with an index of 0, and a name of
// "COLOR", that will contain an enumeration of color identifiers (e.g., red,
// green, blue):
//..
//  bslma_Allocator           *allocator = bslma_Default::allocator();
//  bdema_SequentialAllocator  seqAllocator(allocator);
//  bdem_Schema                dummySchema;
//
//  bdem_EnumerationDef colorEnumDef(&dummySchema, 0, "COLOR", &seqAllocator);
//..
//
///Manipulating and Accessing a 'bdem_EnumerationDef'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// The following section demonstrates how to add enumerated values to an
// enumeration definition, and then access those values.  We will use the
// enumeration definition, 'colorEnumDef' created in the preceding section.
// In practice clients should obtain a enumeration definition from a schema
// (see 'bdem_schema').
//
// We start by examining the properties of an empty enumeration definition.
// Note that 'enumerationName' and 'enumerationIndex' are supplied at
// construction:
//..
//  assert(0 == colorEnumDef.enumerationIndex());
//  assert(0 == bsl::strcmp("COLOR", colorEnumDef.enumerationName()));
//  assert(0 == colorEnumDef.numEnumerators());
//  assert(&dummySchema == &colorEnumDef.schema());
//..
// Next we add 3 different values to the enumeration.  Since we do not
// explicitly specify an ID for the color values, one is assigned
// automatically.  By default, the first added enumerator is given the ID 0,
// subsequent values (if no ID is explicitly provided) are given the current
// maximum ID plus 1:
//..
//  int redId   = colorEnumDef.addEnumerator("RED");
//  int greenId = colorEnumDef.addEnumerator("GREEN");
//  int blueId  = colorEnumDef.addEnumerator("BLUE");
//..
// We verify that the IDs of the new values:
//..
//  assert(3 == colorEnumDef.numEnumerators());
//  assert(2 == colorEnumDef.maxId());
//
//  assert(0 == redId);
//  assert(1 == greenId);
//  assert(2 == blueId);
//..
// Next we add a enumerator, and explicitly provide an ID (in this case -5):
//..
//  int yellowId = colorEnumDef.addEnumerator("YELLOW", -5);
//
//  assert(4 == colorEnumDef.numEnumerators());
//  assert(2 == colorEnumDef.maxId());
//
//  assert(-5 == yellowId);
//..
// Now we add a final color.  Because we do not provide an ID, it is given an
// ID one greater than the current maximum ID (which is 2):
//..
//  int purpleId = colorEnumDef.addEnumerator("PURPLE");
//
//  assert(3 == purpleId);
//  assert(5 == colorEnumDef.numEnumerators());
//  assert(4 == colorEnumDef.maxId());
//..
// We can perform lookups, either by name, or by ID:
//..
//  assert(2 == colorEnumDef.lookupId("BLUE"));
//  assert(0 == bsl::strcmp("BLUE", colorEnumDef.lookupName(2)));
//..
// We can use a 'bdem_EnumerationDefIterator' to efficiently iterate over the
// values in the enumeration definition:
//..
//  bdem_EnumerationDefIterator enumIt = colorEnumDef.begin();
//  for ( ; enumIt != colorEnumDef.end(); ++enumIt) {
//      bsl::cout << "[ " << enumIt.name() << " " << enumIt.id() << " ]"
//                << bsl::endl;
//  }
//..
// The console output of the preceding loop will be:
//..
//  [ YELLOW -5 ]
//  [ RED 0 ]
//  [ GREEN 1 ]
//  [ BLUE 2 ]
//  [ PURPLE 3 ]
//..
// Finally we can use the accessors 'nextLargerId' (or 'nextLargerName') to
// find the next larger ID (or name) after the ID supplied (or name).  Note
// that these methods can be used to iterate over the elements of an
// enumeration definition, in either ID or name order.  Each call to
// 'nextLargerId' and 'nextLargerName' has 'O(log(N))' complexity (where 'N'
// is the number of enumerators).
//
// The next larger ID in 'colorEnumDef' after 0 is 1 ("GREEN"):
//..
//  bsl::pair<const char *, int> enumerator = colorEnumDef.nextLargerId(0);
//  assert(0 == bsl::strcmp("GREEN", enumerator.first));
//  assert(1 == enumerator.second);
//..
// The next larger name in 'colorEnumDef' after "RED" is "YELLOW" (-5):
//..
//  enumerator = colorEnumDef.nextLargerName("RED");
//  assert(0  == bsl::strcmp("YELLOW", enumerator.first));
//  assert(-5 == enumerator.second);
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_ELEMTYPE
#include <bdem_elemtype.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

class bdem_Schema;
class bdema_SequentialAllocator;

                      // =====================================
                      // class bdem_EnumerationDef_NameCompare
                      // =====================================

struct bdem_EnumerationDef_NameCompare
                     : bsl::binary_function<const char *, const char *, bool> {
    // Component-private struct: DO NOT USE OUTSIDE OF THIS COMPONENT.
    // This binary function object class provides a less-than operation that
    // compares 'const char' pointers as strings.  It is used for sorting
    // 'const char *' values.

    bool operator() (const char *lhs, const char *rhs) const
        // Return 'true' if the specified 'lhs' is lexicographically less
        // than the specified 'rhs', and return 'false' otherwise.  The
        // behavior is undefined unless 'lhs' and 'rhs' are non-zero.
    {
        return bsl::strcmp(lhs, rhs) < 0;
    }
};

                   // =================================
                   // class bdem_EnumerationDefIterator
                   // =================================

class bdem_EnumerationDefIterator {
    // This class provides an STL-style iterator over the elements of an
    // enumeration definition.  The behavior of the 'operator*', 'id', and
    // 'name' methods is undefined unless the iterator is at a valid
    // position in the set of enumerated values (i.e., not the "end") and the
    // enumeration definition has not been modified since the iterator was
    // constructed.  Note that the order of iteration may differ between
    // equivalent 'bdem_EnumerationDef' objects (see
    // 'bdem_EnumerationDef::areEquivalent').

    // PRIVATE TYPES
    typedef bsl::map<int, const char *>::const_iterator Imp;
        // Underlying iterator type.

    // DATA
    Imp d_imp;  // iterator implementation

    // FRIEND
    friend class bdem_EnumerationDef;
    friend bool operator==(const bdem_EnumerationDefIterator&,
                           const bdem_EnumerationDefIterator&);

  private:
    // PRIVATE CREATORS
    bdem_EnumerationDefIterator(const Imp& position);
        // Create a enumeration definition iterator at the specified
        // 'position'.

  public:
    // CREATORS
    bdem_EnumerationDefIterator();
        // Create an uninitialized iterator.

    bdem_EnumerationDefIterator(const bdem_EnumerationDefIterator& original);
        // Create an iterator having the same value as the specified
        // 'original' one.

    //! ~bdem_EnumerationDefIterator();
        // Destroy this iterator.  Note that this method's definition is
        // compiler generated.

    // MANIPULATORS
    bdem_EnumerationDefIterator& operator=(
                                       const bdem_EnumerationDefIterator& rhs);
        // Assign to this iterator the value of the specified 'rhs' one, and
        // return a modifiable reference to this iterator.

    bdem_EnumerationDefIterator& operator++();
        // Advance this iterator to the next enumeration definition and return
        // a modifiable reference to this iterator.  The behavior is undefined
        // unless the iterator is at a valid position in the
        // list.

    // ACCESSORS
    bsl::pair<int, const char *> operator*() const;
        // Return the enumeration value, as an (id, name) pair, at which this
        // iterator is positioned.  The behavior is undefined unless this
        // iterator is at a valid position in the enumeration definition.

    int id() const;
        // Return the id of the enumeration value at which this iterator is
        // positioned.  The behavior is undefined unless this iterator is at a
        // valid position in the enumeration definition.

    const char *name() const;
        // Return the name of the enumeration value at which this iterator is
        // positioned.  The behavior is undefined unless this iterator is at a
        // valid position in the enumeration definition.
};

// FREE OPERATORS
bool operator==(const bdem_EnumerationDefIterator& lhs,
                const bdem_EnumerationDefIterator& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' iterators
    // have the same value and 'false' otherwise.  Two iterators have
    // the same value if they refer to the same position in the same
    // enumeration definition, or if both iterators are at an invalid position
    // in the enumeration definition (i.e., the "end" of the enumeration, or
    // the default constructed value).

bool operator!=(const bdem_EnumerationDefIterator& lhs,
                const bdem_EnumerationDefIterator& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' iterators
    // do not have the same value and 'false' otherwise.  Two iterators do not
    // have the same value if do not refer to the same position in the
    // enumeration definition.

                       // =========================
                       // class bdem_EnumerationDef
                       // =========================

class bdem_EnumerationDef {
    // This class represents a enumeration definition, which is an integral
    // part of a fully value-semantic schema object.  An enumeration
    // definition describes a bi-directional 1-to-1 mapping between a set of
    // (unique) null-terminated strings and a set of (unique) integer IDs.
    // The values in a enumeration are (id, name) pairs.  Typical clients
    // should *not* construct a 'bdem_EnumerationDef' directly, but instead
    // obtain an enumeration definition from a schema (see 'bdem_schema' for
    // more information).  Performing a lookup, either by name, or by ID, is a
    // logarithmic-time operation.  Note that an enumeration definition may be
    // used to constrain the value of a field within a record.  An enumeration
    // definition can be used to constraint fields of type 'STRING', 'INT',
    // 'STRING_ARRAY', or 'INT_ARRAY', and allow a bi-directional run-time
    // translation between names and integer IDs, and/or to restrict the
    // allowable set of strings or integers that may be stored in an
    // associated field (see 'bdem_recorddef' for more information on
    // constraints).

    // PRIVATE TYPES
    typedef bsl::map<const char *,
                     int,
                     bdem_EnumerationDef_NameCompare> CharptrIntMap;
        // alias for 'const char *' -> 'int' map

    typedef bsl::map<int, const char *> IntCharptrMap;
        // alias for 'int' -> 'const char *' map

    // DATA
    bdem_Schema   *d_schema_p;       // pointer to parent schema
    int            d_enumIndex;      // index of this object in schema
    const char    *d_name_p;         // my name (held, not owned)
    CharptrIntMap  d_enumByNameMap;  // name-to-ID map of enums
    IntCharptrMap  d_enumByIdMap;    // ID-to-name map of enums
    int            d_maxId;          // maximum id value in this EnumerationDef

    // NOT IMPLEMENTED
    bdem_EnumerationDef(const bdem_EnumerationDef&);
    bdem_EnumerationDef& operator=(const bdem_EnumerationDef&);

  public:
    // CLASS METHODS
    static bool areEquivalent(const bdem_EnumerationDef& lhs,
                              const bdem_EnumerationDef& rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' enumeration
        // definitions have the the same enumerators (i.e., each enumerator
        // has the same name and ID), and 'false' otherwise.  Note that
        // corresponding enumerators need not appear in the same order during
        // iteration (see 'bdem_EnumerationDefIterator').

    static bool isSuperset(const bdem_EnumerationDef& super,
                           const bdem_EnumerationDef& sub);
        // Return 'true' if the specified 'super' enumeration is a superset of
        // the specified 'sub' enumeration (i.e., for each enumerator in 'sub'
        // there exists an enumerator in 'super' having the same name and ID),
        // and 'false' otherwise.  Note that corresponding enumerators need
        // not appear in the same order during iteration (see
        // 'bdem_EnumerationDefIterator').

    static bool canHaveEnumConstraint(bdem_ElemType::Type type);
        // Return 'true' if the specified data 'type' is permitted to have an
        // enumeration constraint, and 'false' otherwise.  The types that are
        // permitted to have enumeration constraints are 'INT', 'STRING',
        // 'INT_ARRAY', and 'STRING_ARRAY'.

    // CREATORS
    bdem_EnumerationDef(bdem_Schema               *schema,
                        int                        index,
                        const char                *name,
                        bdema_SequentialAllocator *writeOnceAllocator);
        // Create an empty enumeration definition having the specified
        // 'index', 'name', and (opaque) 'schema', using the specified
        // 'writeOnceAllocator' to supply memory.  Clients of 'bdem', in
        // general, should *not* construct a 'bdem_RecordDef' directly, but
        // obtain one through a 'bdem_Schema' object (see 'bdem_schema' for
        // more information).  If 'name' is 0, the record definition is
        // unnamed.  'writeOnceAllocator' will be used to supply memory that
        // will not be released by this object.  The behavior is undefined
        // unless both 'schema' and 'name' are not 0, and 'name'remains valid
        // and unmodified for the lifetime of this object.  Note that this
        // operation does *not* modify 'schema' to refer to the newly
        // constructed object.

    ~bdem_EnumerationDef();
        // Destroy this enumeration definition.  Do not deallocate memory that
        // was allocated from the write-once allocator supplied at
        // construction.

    // MANIPULATORS
    int addEnumerator(const char *name);
        // Add the specified 'name' to the set of enumerators for this
        // enumeration; return its integer ID on success, and
        // 'bdetu_Unset<int>::unsetValue()' otherwise.  An enumerator added
        // in this way is automatically assigned an integer ID one larger than
        // the ID of the the maximum ID in this enumeration, or 0 if it is
        // the first enumerator added to this enumeration.   This operation
        // will fail (return 'bdetu_Unset<int>::unsetValue()') if 'name'
        // already exists in this enumeration, or if 'maxId()' is 'INT_MAX'.

    int addEnumerator(const char *name, int id);
        // Add the specified 'name' to the set of enumerators for this
        // enumeration, assign it the specified integer 'id'.  Return 'id'
        // on success and 'bdetu_Unset<int>::unsetValue()' otherwise.
        // This function will fail (return 'bdetu_Unset<int>::unsetValue()')
        // unless 'name' is non-null, it is unique among the enumerators in
        // this enumeration definition, 'id != bdetu_Unset<int>::unsetValue()',
        // and 'id' is distinct from any integer ID currently used by an
        // enumerator in this enumeration.

    void alphabetize(int firstId = 0);
        // Re-assign the IDs of all the current enumerators in this enumeration
        // consecutively in alphabetical order by name (using the order
        // defined by 'strcmp'), starting from the specified 'firstId'.
        // The next enumerator added will be assigned an ID of 'firstId +
        // numEnumerators()'.  Note that assigning IDs alphabetically is
        // consistent with the ASN.1 formula for assigning integer IDs to
        // enumerators.

    // ACCESSORS
    bdem_EnumerationDefIterator begin() const;
        // Return an iterator positioned at the beginning of the set of
        // enumerators represented by this object.

    bdem_EnumerationDefIterator end() const;
        // Return an iterator positioned one past the final position in the set
        // of enumerators represented by this object.

    int enumerationIndex() const;
        // Return the index for this enumeration definition (established at
        // construction).  Note that a enumeration index is generally useful
        // in identifying an enumeration definition within a schema (see
        // 'bdem_schema').

    const char *enumerationName() const;
        // Return the non-modifiable name of this enumeration definition
        // (established at construction), or 0 this enumeration definition is
        // unnamed.  Note that an enumeration name is generally useful in
        // identifying an enumeration definition within a schema (see
        // 'bdem_schema').

    const char *lookupName(int id) const;
        // Return the string representation of the enumerator having the
        // specified 'id', or 0 if there is no such enumerator.  Note that the
        // returned string is owned by, and is valid through the lifetime of
        // this enumeration definition object.

    const char *lookupName(const char *name) const;
        // Return the string representation of the enumerator having the
        // specified 'name', or 0 if there is no such enumerator.  Note that
        // the returned string is owned by, and is valid through the lifetime
        // of this enumeration definition object.

    int lookupId(const char *name) const;
        // Return the integer ID of the enumerator corresponding to the
        // specified 'name' or 'bdetu_Unset<int>::unsetValue()' if there is no
        // corresponding enumerator.

    int maxId() const;
        // Return the maximum ID that has beed added to this enumeration.  The
        // behavior is undefined unless '1 <= numEnumerators'.

    int numEnumerators() const;
        // Return the number of enumerators that have been added to this
        // enumeration.

    bsl::pair<const char *, int> nextLargerName(const char *name) const;
        // Return, as a 'bsl::pair', the name and id of either (1) the
        // enumerator in this enumeration definition whose name is
        // lexicographically the smallest one greater than the specified
        // (non-null)' name', if one exists, (2) the name and id of the
        // enumerator in this enumeration definition having the
        // lexicographically smallest name, if 'name' is 0 and this object is
        // not empty, or (3) 0 and 'bdetu_Unset<int>' otherwise.  This
        // function has complexity 'O[log(n)]', where 'n' is
        // 'numEnumerators()'.  Note that this method can be used to iterate
        // through the enumerators in alphabetical order (see also the
        // alphabetize' method).

    bsl::pair<const char *, int> nextLargerId(int id) const;
        // Return, as a 'bsl::pair', the name and id of the enumerator in this
        // enumeration definition having the smallest id greater than the
        // specified 'id', if one exists, and 0 and 'bdetu_Unset<int>'
        // otherwise.  This function has complexity 'O[log(n)]', where 'n' is
        // 'numEnumerators()'.  Note that this method can be used to iterate
        // through the enumerators in alphabetical order.

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

    const bdem_Schema& schema() const;
        // Return a reference to this enumeration definition's non-modifiable
        // schema (established at construction).

    bslma_Allocator *writeOnceAllocator() const;
        // Return the address of the modifiable write-once allocator supplied
        // when this object was constructed.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                   // ---------------------------------
                   // class bdem_EnumerationDefIterator
                   // ---------------------------------

// PRIVATE CREATORS
inline
bdem_EnumerationDefIterator::bdem_EnumerationDefIterator(const Imp& position)
: d_imp(position)
{
}

// CREATORS
inline
bdem_EnumerationDefIterator::bdem_EnumerationDefIterator()
: d_imp()
{
}

inline
bdem_EnumerationDefIterator::bdem_EnumerationDefIterator(
                           const bdem_EnumerationDefIterator& original)
: d_imp(original.d_imp)
{
}

// MANIPULATORS
inline
bdem_EnumerationDefIterator&
bdem_EnumerationDefIterator::operator=(const bdem_EnumerationDefIterator& rhs)
{
    d_imp = rhs.d_imp;
    return *this;
}

inline
bdem_EnumerationDefIterator&
bdem_EnumerationDefIterator::operator++()
{
    ++d_imp;
    return *this;
}

// ACCESSORS
inline
bsl::pair<int, const char *>
bdem_EnumerationDefIterator::operator*() const
{
    return *d_imp;
}

inline
int bdem_EnumerationDefIterator::id() const
{
    return d_imp->first;
}

inline
const char *bdem_EnumerationDefIterator::name() const
{
    return d_imp->second;
}

// FREE OPERATORS
inline
bool operator==(const bdem_EnumerationDefIterator& lhs,
                const bdem_EnumerationDefIterator& rhs)
{
    return lhs.d_imp == rhs.d_imp;
}

inline
bool operator!=(const bdem_EnumerationDefIterator& lhs,
                const bdem_EnumerationDefIterator& rhs)
{
    return !(lhs == rhs);
}

                        // -------------------------
                        // class bdem_EnumerationDef
                        // -------------------------

// CLASS METHODS
inline
bool bdem_EnumerationDef::canHaveEnumConstraint(bdem_ElemType::Type type)
{
    return type == bdem_ElemType::BDEM_INT
        || type == bdem_ElemType::BDEM_STRING
        || type == bdem_ElemType::BDEM_INT_ARRAY
        || type == bdem_ElemType::BDEM_STRING_ARRAY;
}

// MANIPULATORS
inline
int bdem_EnumerationDef::addEnumerator(const char *name)
{
    // If this is the first enumerated value, give it the id 0, otherwise give
    // it the id 'max() + 1'
    return addEnumerator(name, numEnumerators() ? d_maxId + 1 : 0);
}

// ACCESSORS
inline
bdem_EnumerationDefIterator bdem_EnumerationDef::begin() const
{
    return bdem_EnumerationDefIterator(d_enumByIdMap.begin());
}

inline
bdem_EnumerationDefIterator bdem_EnumerationDef::end() const
{
    return bdem_EnumerationDefIterator(d_enumByIdMap.end());
}

inline
int bdem_EnumerationDef::enumerationIndex() const
{
    return d_enumIndex;
}

inline
const char *bdem_EnumerationDef::enumerationName() const
{
    return d_name_p;
}

inline
int bdem_EnumerationDef::numEnumerators() const
{
    return (int)d_enumByNameMap.size();
}

inline
int bdem_EnumerationDef::maxId() const
{
    return d_maxId;
}

inline
const bdem_Schema& bdem_EnumerationDef::schema() const
{
    return *d_schema_p;
}

inline
bslma_Allocator *bdem_EnumerationDef::writeOnceAllocator() const
{
    return d_enumByNameMap.get_allocator().mechanism();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
