// bdlmxxx_enumerationdef.h                                           -*-C++-*-
#ifndef INCLUDED_BDLMXXX_ENUMERATIONDEF
#define INCLUDED_BDLMXXX_ENUMERATIONDEF

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a definition for a set of enumerated values.
//
//@CLASSES:
//           bdlmxxx::EnumerationDef: a definition for a set of enumerated values
//  bdlmxxx::EnumerationDefIterator: an iterator over an enumeration definition
//
//@SEE_ALSO: bdlmxxx_schema
//
//@AUTHOR: Tom Marshall, Henry Verschell
//
//@DESCRIPTION: This component provides a collaborative type,
// 'bdlmxxx::EnumerationDef', that is an integral part of a fully-value semantic
// schema object.  A 'bdlmxxx::EnumerationDef' object defines an enumeration
// (i.e., a set of enumerated values).  The values in a enumeration are (id,
// name) pairs, where the id is an integer, and the name is a null-terminated
// C-string.  Both the id and name must be unique within a single enumeration.
// In general, clients should *not* construct a 'bdlmxxx::EnumerationDef' directly,
// but instead obtain an enumeration definition from a 'bdlmxxx::Schema' (see
// 'bdlmxxx_schema' for more information).  Note that an enumeration definition
// can be used to constrain the values of a field of type 'STRING', 'INT',
// 'STRING_ARRAY', or 'INT_ARRAY' in a record (see 'bdlmxxx_recorddef' for
// more information on constraints).
//
///Usage
///-----
// The following usage examples demonstrate how one could construct,
// manipulate, and access a enumeration definition in isolation.  In general,
// clients should *not* create a 'bdem::Enumeration' directly, but obtain one
// from a schema (see 'bdlmxxx_schema').
//
///Directly Creating a 'bdlmxxx::RecordDef' (*Not Recommended*)
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following section demonstrates how to create a 'bdlmxxx::EnumerationDef' in
// isolation -- this is *not* the recommended usage of 'bdlmxxx::EnumerationDef'.
// Clients should not create a 'bdlmxxx::EnumerationDef' directly, but obtain one
// from a schema (see 'bdlmxxx_schema').  For the purpose of this usage example,
// we define a dummy stand-in type for 'bdlmxxx::Schema'.  This type is used in
// name-only by 'bdlmxxx::EnumerationDef', so its definition is not necessary:
//..
//  namespace BloombergLP {
//
//  class bdlmxxx::Schema {
//      // Dummy type.
//  };
//
//  }
//..
// Now we create an enumeration definition with an index of 0, and a name of
// "COLOR", that will contain an enumeration of color identifiers (e.g., red,
// green, blue):
//..
//  bslma::Allocator          *allocator = bslma::Default::allocator();
//  bdlma::SequentialAllocator  seqAllocator(allocator);
//  bdlmxxx::Schema                dummySchema;
//
//  bdlmxxx::EnumerationDef colorEnumDef(&dummySchema, 0, "COLOR", &seqAllocator);
//..
//
///Manipulating and Accessing a 'bdlmxxx::EnumerationDef'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// The following section demonstrates how to add enumerated values to an
// enumeration definition, and then access those values.  We will use the
// enumeration definition, 'colorEnumDef' created in the preceding section.
// In practice clients should obtain a enumeration definition from a schema
// (see 'bdlmxxx_schema').
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
// We can use a 'bdlmxxx::EnumerationDefIterator' to efficiently iterate over the
// values in the enumeration definition:
//..
//  bdlmxxx::EnumerationDefIterator enumIt = colorEnumDef.begin();
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

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMTYPE
#include <bdlmxxx_elemtype.h>
#endif

#ifndef INCLUDED_BDLB_CSTRINGLESS
#include <bdlb_cstringless.h>
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


namespace bdlmxxx {class Schema;
}  // close package namespace
namespace bdlma { class SequentialAllocator; }

namespace bdlmxxx {
                   // =================================
                   // class EnumerationDefIterator
                   // =================================

class EnumerationDefIterator {
    // This class provides an STL-style iterator over the elements of an
    // enumeration definition.  The behavior of the 'operator*', 'id', and
    // 'name' methods is undefined unless the iterator is at a valid
    // position in the set of enumerated values (i.e., not the "end") and the
    // enumeration definition has not been modified since the iterator was
    // constructed.  Note that the order of iteration may differ between
    // equivalent 'EnumerationDef' objects (see
    // 'EnumerationDef::areEquivalent').

    // PRIVATE TYPES
    typedef bsl::map<int, const char *>::const_iterator Imp;
        // Underlying iterator type.

    // DATA
    Imp d_imp;  // iterator implementation

    // FRIEND
    friend class EnumerationDef;
    friend bool operator==(const EnumerationDefIterator&,
                           const EnumerationDefIterator&);

  private:
    // PRIVATE CREATORS
    EnumerationDefIterator(const Imp& position);
        // Create a enumeration definition iterator at the specified
        // 'position'.

  public:
    // CREATORS
    EnumerationDefIterator();
        // Create an uninitialized iterator.

    EnumerationDefIterator(const EnumerationDefIterator& original);
        // Create an iterator having the same value as the specified
        // 'original' one.

    //! ~EnumerationDefIterator();
        // Destroy this iterator.  Note that this method's definition is
        // compiler generated.

    // MANIPULATORS
    EnumerationDefIterator& operator=(
                                       const EnumerationDefIterator& rhs);
        // Assign to this iterator the value of the specified 'rhs' one, and
        // return a modifiable reference to this iterator.

    EnumerationDefIterator& operator++();
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
bool operator==(const EnumerationDefIterator& lhs,
                const EnumerationDefIterator& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' iterators
    // have the same value and 'false' otherwise.  Two iterators have
    // the same value if they refer to the same position in the same
    // enumeration definition, or if both iterators are at an invalid position
    // in the enumeration definition (i.e., the "end" of the enumeration, or
    // the default constructed value).

bool operator!=(const EnumerationDefIterator& lhs,
                const EnumerationDefIterator& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' iterators
    // do not have the same value and 'false' otherwise.  Two iterators do not
    // have the same value if do not refer to the same position in the
    // enumeration definition.

                       // =========================
                       // class EnumerationDef
                       // =========================

class EnumerationDef {
    // This class represents a enumeration definition, which is an integral
    // part of a fully value-semantic schema object.  An enumeration
    // definition describes a bi-directional 1-to-1 mapping between a set of
    // (unique) null-terminated strings and a set of (unique) integer IDs.
    // The values in a enumeration are (id, name) pairs.  Typical clients
    // should *not* construct a 'EnumerationDef' directly, but instead
    // obtain an enumeration definition from a schema (see 'bdlmxxx_schema' for
    // more information).  Performing a lookup, either by name, or by ID, is a
    // logarithmic-time operation.  Note that an enumeration definition may be
    // used to constrain the value of a field within a record.  An enumeration
    // definition can be used to constraint fields of type 'STRING', 'INT',
    // 'STRING_ARRAY', or 'INT_ARRAY', and allow a bi-directional run-time
    // translation between names and integer IDs, and/or to restrict the
    // allowable set of strings or integers that may be stored in an
    // associated field (see 'bdlmxxx_recorddef' for more information on
    // constraints).

    // PRIVATE TYPES
    typedef bsl::map<const char *,
                     int,
                     bdlb::CStringLess> CharptrIntMap;
        // alias for 'const char *' -> 'int' map

    typedef bsl::map<int, const char *> IntCharptrMap;
        // alias for 'int' -> 'const char *' map

    // DATA
    Schema   *d_schema_p;       // pointer to parent schema
    int            d_enumIndex;      // index of this object in schema
    const char    *d_name_p;         // my name (held, not owned)
    CharptrIntMap  d_enumByNameMap;  // name-to-ID map of enums
    IntCharptrMap  d_enumByIdMap;    // ID-to-name map of enums
    int            d_maxId;          // maximum id value in this EnumerationDef

    // NOT IMPLEMENTED
    EnumerationDef(const EnumerationDef&);
    EnumerationDef& operator=(const EnumerationDef&);

  public:
    // CLASS METHODS
    static bool areEquivalent(const EnumerationDef& lhs,
                              const EnumerationDef& rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' enumeration
        // definitions have the the same enumerators (i.e., each enumerator
        // has the same name and ID), and 'false' otherwise.  Note that
        // corresponding enumerators need not appear in the same order during
        // iteration (see 'EnumerationDefIterator').

    static bool isSuperset(const EnumerationDef& super,
                           const EnumerationDef& sub);
        // Return 'true' if the specified 'super' enumeration is a superset of
        // the specified 'sub' enumeration (i.e., for each enumerator in 'sub'
        // there exists an enumerator in 'super' having the same name and ID),
        // and 'false' otherwise.  Note that corresponding enumerators need
        // not appear in the same order during iteration (see
        // 'EnumerationDefIterator').

    static bool canHaveEnumConstraint(ElemType::Type type);
        // Return 'true' if the specified data 'type' is permitted to have an
        // enumeration constraint, and 'false' otherwise.  The types that are
        // permitted to have enumeration constraints are 'INT', 'STRING',
        // 'INT_ARRAY', and 'STRING_ARRAY'.

    // CREATORS
    EnumerationDef(Schema               *schema,
                        int                        index,
                        const char                *name,
                        bdlma::SequentialAllocator *writeOnceAllocator);
        // Create an empty enumeration definition having the specified
        // 'index', 'name', and (opaque) 'schema', using the specified
        // 'writeOnceAllocator' to supply memory.  Clients of 'bdem', in
        // general, should *not* construct a 'RecordDef' directly, but
        // obtain one through a 'Schema' object (see 'bdlmxxx_schema' for
        // more information).  If 'name' is 0, the record definition is
        // unnamed.  'writeOnceAllocator' will be used to supply memory that
        // will not be released by this object.  The behavior is undefined
        // unless both 'schema' and 'name' are not 0, and 'name'remains valid
        // and unmodified for the lifetime of this object.  Note that this
        // operation does *not* modify 'schema' to refer to the newly
        // constructed object.

    ~EnumerationDef();
        // Destroy this enumeration definition.  Do not deallocate memory that
        // was allocated from the write-once allocator supplied at
        // construction.

    // MANIPULATORS
    int addEnumerator(const char *name);
        // Add the specified 'name' to the set of enumerators for this
        // enumeration; return its integer ID on success, and
        // 'bdltuxxx::Unset<int>::unsetValue()' otherwise.  An enumerator added
        // in this way is automatically assigned an integer ID one larger than
        // the ID of the the maximum ID in this enumeration, or 0 if it is
        // the first enumerator added to this enumeration.   This operation
        // will fail (return 'bdltuxxx::Unset<int>::unsetValue()') if 'name'
        // already exists in this enumeration, or if 'maxId()' is 'INT_MAX'.

    int addEnumerator(const char *name, int id);
        // Add the specified 'name' to the set of enumerators for this
        // enumeration, assign it the specified integer 'id'.  Return 'id'
        // on success and 'bdltuxxx::Unset<int>::unsetValue()' otherwise.
        // This function will fail (return 'bdltuxxx::Unset<int>::unsetValue()')
        // unless 'name' is non-null, it is unique among the enumerators in
        // this enumeration definition, 'id != bdltuxxx::Unset<int>::unsetValue()',
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
    EnumerationDefIterator begin() const;
        // Return an iterator positioned at the beginning of the set of
        // enumerators represented by this object.

    EnumerationDefIterator end() const;
        // Return an iterator positioned one past the final position in the set
        // of enumerators represented by this object.

    int enumerationIndex() const;
        // Return the index for this enumeration definition (established at
        // construction).  Note that a enumeration index is generally useful
        // in identifying an enumeration definition within a schema (see
        // 'bdlmxxx_schema').

    const char *enumerationName() const;
        // Return the non-modifiable name of this enumeration definition
        // (established at construction), or 0 this enumeration definition is
        // unnamed.  Note that an enumeration name is generally useful in
        // identifying an enumeration definition within a schema (see
        // 'bdlmxxx_schema').

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
        // specified 'name' or 'bdltuxxx::Unset<int>::unsetValue()' if there is no
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
        // not empty, or (3) 0 and 'bdltuxxx::Unset<int>' otherwise.  This
        // function has complexity 'O[log(n)]', where 'n' is
        // 'numEnumerators()'.  Note that this method can be used to iterate
        // through the enumerators in alphabetical order (see also the
        // alphabetize' method).

    bsl::pair<const char *, int> nextLargerId(int id) const;
        // Return, as a 'bsl::pair', the name and id of the enumerator in this
        // enumeration definition having the smallest id greater than the
        // specified 'id', if one exists, and 0 and 'bdltuxxx::Unset<int>'
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

    const Schema& schema() const;
        // Return a reference to this enumeration definition's non-modifiable
        // schema (established at construction).

    bslma::Allocator *writeOnceAllocator() const;
        // Return the address of the modifiable write-once allocator supplied
        // when this object was constructed.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                   // ---------------------------------
                   // class EnumerationDefIterator
                   // ---------------------------------

// PRIVATE CREATORS
inline
EnumerationDefIterator::EnumerationDefIterator(const Imp& position)
: d_imp(position)
{
}

// CREATORS
inline
EnumerationDefIterator::EnumerationDefIterator()
: d_imp()
{
}

inline
EnumerationDefIterator::EnumerationDefIterator(
                           const EnumerationDefIterator& original)
: d_imp(original.d_imp)
{
}

// MANIPULATORS
inline
EnumerationDefIterator&
EnumerationDefIterator::operator=(const EnumerationDefIterator& rhs)
{
    d_imp = rhs.d_imp;
    return *this;
}

inline
EnumerationDefIterator&
EnumerationDefIterator::operator++()
{
    ++d_imp;
    return *this;
}

// ACCESSORS
inline
bsl::pair<int, const char *>
EnumerationDefIterator::operator*() const
{
    return *d_imp;
}

inline
int EnumerationDefIterator::id() const
{
    return d_imp->first;
}

inline
const char *EnumerationDefIterator::name() const
{
    return d_imp->second;
}
}  // close package namespace

// FREE OPERATORS
inline
bool bdlmxxx::operator==(const EnumerationDefIterator& lhs,
                const EnumerationDefIterator& rhs)
{
    return lhs.d_imp == rhs.d_imp;
}

inline
bool bdlmxxx::operator!=(const EnumerationDefIterator& lhs,
                const EnumerationDefIterator& rhs)
{
    return !(lhs == rhs);
}

namespace bdlmxxx {
                        // -------------------------
                        // class EnumerationDef
                        // -------------------------

// CLASS METHODS
inline
bool EnumerationDef::canHaveEnumConstraint(ElemType::Type type)
{
    return type == ElemType::BDEM_INT
        || type == ElemType::BDEM_STRING
        || type == ElemType::BDEM_INT_ARRAY
        || type == ElemType::BDEM_STRING_ARRAY;
}

// MANIPULATORS
inline
int EnumerationDef::addEnumerator(const char *name)
{
    // If this is the first enumerated value, give it the id 0, otherwise give
    // it the id 'max() + 1'
    return addEnumerator(name, numEnumerators() ? d_maxId + 1 : 0);
}

// ACCESSORS
inline
EnumerationDefIterator EnumerationDef::begin() const
{
    return EnumerationDefIterator(d_enumByIdMap.begin());
}

inline
EnumerationDefIterator EnumerationDef::end() const
{
    return EnumerationDefIterator(d_enumByIdMap.end());
}

inline
int EnumerationDef::enumerationIndex() const
{
    return d_enumIndex;
}

inline
const char *EnumerationDef::enumerationName() const
{
    return d_name_p;
}

inline
int EnumerationDef::numEnumerators() const
{
    return (int)d_enumByNameMap.size();
}

inline
int EnumerationDef::maxId() const
{
    return d_maxId;
}

inline
const Schema& EnumerationDef::schema() const
{
    return *d_schema_p;
}

inline
bslma::Allocator *EnumerationDef::writeOnceAllocator() const
{
    return d_enumByNameMap.get_allocator().mechanism();
}
}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
