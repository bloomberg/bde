// bdec2_map.h                -*-C++-*-
#ifndef INCLUDED_BDEC2_MAP
#define INCLUDED_BDEC2_MAP

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide associations of unique 'KEY' to 'VALUE' values.
//
//@DEPRECATED: Use 'bsl::map' instead.
//
//@CLASSES:
//        bdec2_Map: memory manager for a map of 'KEY' to 'VALUE' values
//    bdec2_MapIter: sequential read-only accessor of map members
//   bdec2_MapManip: sequential selective remover of map members
//
//@AUTHOR: Jeffrey Mendelsohn (jmendelsohn)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component implements an efficient map class for elements
// of type 'KEY' to 'VALUE' values.  Iterators and manipulators provide
// efficient access and filtering (i.e., selectively removing) of members,
// respectively.
//
///PERFORMANCE
///-----------
// The following characterizes the performance of representative operations
// using big-oh notation, O[f(N,M)], where the names 'N' and 'M' also refer to
// the number of respective elements in each container (i.e., its 'length()').
// Here the expected case, E[f(N)], is the expected cost of the operation
// assuming "normal" data.
//..
//     Operation           Worst Case          Expected Case
//     ---------           ----------          -------------
//     add                 O[N]                E[1]
//     remove              O[N]                E[1]
//     set                 O[N]                E[1]
//     isMember            O[N]                E[1]
//     lookup              O[N]                E[1]
//     length              O[1]
//..
///MAP USAGE
///---------
// The following snippets of code illustrate how to create and use a map.
// First create an empty 'bdec2_Map' 's' and populate it with elements:
//..
//      const int K1 = 100;
//      const int K2 = 200;
//      const int K3 = 300;
//
//      const char *V1 = "a";
//      const char *V2 = "ab";
//      const char *V3 = "abc";
//
//      bdec2_Map<int, bsl::string, bdeimp_IntHash> s;
//                                      assert( 0 == s.length());
//
//      s.add(K1, V1);                  assert( 1 == s.length());
//                                      assert( s.isMember(K1));
//                                      assert(!s.isMember(K2));
//                                      assert(!s.isMember(K3));
//
//      s.add(K2, V2);                  assert( 2 == s.length());
//                                      assert( s.isMember(K1));
//                                      assert( s.isMember(K2));
//                                      assert(!s.isMember(K3));
//
//      s.add(K3, V3);                  assert( 3 == s.length());
//                                      assert( s.isMember(K1));
//                                      assert( s.isMember(K2));
//                                      assert( s.isMember(K3));
//..
// Finally, remove the elements from the map 's':
//..
//      s.remove(K1);                   assert( 2 == s.length());
//                                      assert(!s.isMember(K1));
//                                      assert( s.isMember(K2));
//                                      assert( s.isMember(K3));
//
//      s.remove(K2);                   assert( 1 == s.length());
//                                      assert(!s.isMember(K1));
//                                      assert(!s.isMember(K2));
//                                      assert( s.isMember(K3));
//
//      s.remove(K3);                   assert( 0 == s.length());
//..
///ITERATOR USAGE
///--------------
// The following shows the standard iterator pattern:
//..
//      for (bdec2_MapIter<int, bsl::string, bdeimp_IntHash> it(s); it; ++it) {
//          bsl::cout << it.key() << ' ' << it.value() << bsl::endl;
//      }
//..
///MANIPULATOR USAGE
///-----------------
// The following illustrates the (less-common) manipulator pattern:
//..
//      bdec2_MapManip<int, bsl::string, bdeimp_IntHash> manip(&s);
//      while (manip) {
//          if (K3 >= manip.key()) {   // condition to keep current element
//              manip.advance();
//          }
//          else {                 // otherwise get rid of current element
//              manip.remove();
//          }
//      }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BDEIMP_INTHASH
#include <bdeimp_inthash.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLALG_CONSTRUCTORPROXY
#include <bslalg_constructorproxy.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDECI_HASHTABLE
#include <bdeci_hashtable.h>
#endif

#ifndef INCLUDED_BSL_NEW
#include <bsl_new.h>         // placement syntax
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif


namespace BloombergLP {

template <class KEY, class VALUE, class HASH>
class bdec2_MapIter;

template <class KEY, class VALUE, class HASH>
class bdec2_MapManip;

                         // ===========================
                         // local struct bdec2_Map_Node
                         // ===========================

template <class KEY, class VALUE>
struct bdec2_Map_Node {
    // This 'struct' implements the storage and functionality required for a
    // map node.  Note that this class should be internal to class 'bdec2_Map'.
    // Due to compiler issues (e.g., with IBM AIX and xlC), it is not.  Do not
    // use this class outside of this component.

    bslalg_ConstructorProxy<KEY>    d_keyStorage;
    bslalg_ConstructorProxy<VALUE>  d_valueStorage;

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdec2_Map_Node,
                                 bslalg_TypeTraitUsesBslmaAllocator);

  private:
    // NOT IMPLEMENTED
    bdec2_Map_Node(const bdec2_Map_Node<KEY, VALUE>&  original);

  public:
    // CREATORS
    bdec2_Map_Node(const KEY&       key,
                   const VALUE&     value,
                   bslma_Allocator *allocator)
    : d_keyStorage(key,     allocator)
    , d_valueStorage(value, allocator)
    {
    }

    bdec2_Map_Node(const bdec2_Map_Node<KEY, VALUE>&  original,
                   bslma_Allocator                   *allocator)
    : d_keyStorage(original.key(),     allocator)
    , d_valueStorage(original.value(), allocator)
    {
    }

    // MANIPULATORS
    bdec2_Map_Node& operator=(const bdec2_Map_Node& rhs)
    {
        d_keyStorage.object() = rhs.d_keyStorage.object();
        d_valueStorage.object() = rhs.d_valueStorage.object();
        return *this;
    }

    KEY& key() { return d_keyStorage.object(); }
    VALUE& value() { return d_valueStorage.object(); }

    // ACCESSORS
    const KEY& key() const { return d_keyStorage.object(); }
    const VALUE& value() const { return d_valueStorage.object(); }

    template <class T>
    bool operator==(const T& rhs) const {
        return d_keyStorage.object() == rhs; }
    bool operator==(const bdec2_Map_Node& rhs) const {
        return d_keyStorage.object() == rhs.d_keyStorage.object(); }
    bool operator!=(const bdec2_Map_Node& rhs) const {
        return d_keyStorage.object() != rhs.d_keyStorage.object(); }
};

                       // ================================
                       // local struct bdec2_Map_Node_Hash
                       // ================================

template <class KEY, class VALUE, class HASH>
struct bdec2_Map_Node_Hash {
    // Note that this struct should be internal to class 'bdec2_Map'.  Due to
    // compiler issues (e.g., with IBM AIX and xlC), it is not.  Do not use
    // this class outside of this component.

    // CLASS METHODS
    static int hash(const bdec2_Map_Node<KEY, VALUE>& node, int size);

    template <class T>
    static int hash(const T& key, int size) {
        return HASH::hash(key, size);
    }
};

// CLASS METHODS
template <class KEY, class VALUE, class HASH>
inline
int bdec2_Map_Node_Hash<KEY, VALUE, HASH>::
                         hash(const bdec2_Map_Node<KEY, VALUE>& node, int size)
{
    return HASH::hash(node.key(), size);
}

                             // ===============
                             // class bdec2_Map
                             // ===============

template <class KEY, class VALUE, class HASH>
class bdec2_Map {
    // This class implements an efficient map of 'KEY' to 'VALUE' values.  It
    // also provides access to the constant 'key' portion of every association.
    //
    // More generally, this class supports a complete map of *value*
    // *semantic* operations, including copy construction, assignment,
    // equality comparison, 'ostream' printing, and 'bdex' serialization.  (A
    // precise operational definition of when two instances have the same
    // value can be found in the description of 'operator==' for the class.)
    // This container is *exception* *neutral* with no guarantee of rollback:
    // if an exception is thrown during the invocation of a method on a
    // pre-existing instance, the class is left in a valid state, but its
    // value is undefined.  In no event is memory leaked.  Finally, *aliasing*
    // (e.g., using all or part of an object as both source and destination)
    // is supported in all cases.

    // DATA
    bdeci_Hashtable<bdec2_Map_Node<KEY, VALUE>,
                    bdec2_Map_Node_Hash<KEY, VALUE, HASH> > d_hashtable;
    bslma_Allocator                *d_allocator_p; // holds memory allocator

    // FRIENDS
    friend class bdec2_MapIter<KEY, VALUE, HASH>;
    friend class bdec2_MapManip<KEY, VALUE, HASH>;

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdec2_Map,
                                 bslalg_TypeTraitUsesBslmaAllocator);

  private:
    // PRIVATE ACCESSORS
    bsl::ostream&
    indent(bsl::ostream& stream, int level, int spacesPerLevel) const;
        // Emit the product of the specified 'level' and 'spacesPerLevel' to
        // the specified output 'stream' and return a reference to the
        // modifiable 'stream'.  If the product is negative, this function has
        // no effect.  The behavior is undefined unless '0 <= spacesPerLevel'.

  public:
    // TYPES
    struct InitialCapacity {
        // Enable uniform use of an optional integral constructor argument to
        // specify the initial internal capacity (in elements).  For example,
        //..
        //   bdec2_Map x(bdec2_Map::InitialCapacity(8));
        //..
        // defines an instance, 'x', with an initial capacity of 8 elements,
        // but with a logical length of 0 elements.

        int d_i;

        // CREATORS
        explicit InitialCapacity(int i) : d_i(i) { }
        ~InitialCapacity() { }
    };

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    static int maxSupportedVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)
        //
        // DEPRECATED: replaced by 'maxSupportedBdexVersion()'

    // CREATORS
    explicit bdec2_Map(bslma_Allocator *basicAllocator = 0);
        // Create an empty map.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the  currently installed
        // default allocator is used.

    explicit bdec2_Map(const InitialCapacity&  numElements,
                       bslma_Allocator        *basicAllocator = 0);
        // Create an empty map with sufficient initial capacity to accommodate
        // up to the specified 'numElements' values without subsequent
        // reallocation.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined unless
        // '0 <= numElements'.

    bdec2_Map(const KEY       *keys,
              const VALUE     *values,
              int              numElements,
              bslma_Allocator *basicAllocator = 0);
        // Create a map initialized to having the specified 'numElements'
        // associations from respective elements of the specified 'keys' array
        // and the specified 'values' array.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless '0 <= numElements'.  The behavior is undefined
        // unless '0 <= numElements'.

    bdec2_Map(const bdec2_Map<KEY, VALUE, HASH>&  original,
              bslma_Allocator                    *basicAllocator = 0);
        // Create a map initialized to the value of the specified 'original'
        // map.  Optionally specify a 'basicAllocator' used to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.  The behavior is undefined unless '0 <= numElements'.

    ~bdec2_Map();
        // Destroy this object.

    // MANIPULATORS
    bdec2_Map& operator=(const bdec2_Map<KEY, VALUE, HASH>& rhs);
        // Assign to this map the value of the specified 'rhs' map.  Return a
        // reference to this map.

    VALUE *add(const KEY& key, const VALUE& value);
        // Add to this map the association from the specified 'key' to the
        // specified 'value'.  If the 'key' is already a member, this method
        // has no effect.  Return a pointer to the, newly created,  modifiable
        // value if 'key' was not already a member of this map, and 0
        // otherwise.  The pointer will remain valid as long as this map is not
        // destroyed or modified (e.g., via 'add', 'set', or 'remove').

    VALUE *lookup(const KEY& key);
        // Return a pointer to the modifiable value corresponding to the
        // specified 'key' if 'key' is a member of this map, and 0 otherwise.
        // The pointer will remain valid as long as this map is not destroyed
        // or modified (e.g., via 'add', 'set', or 'remove').

    template <class T>
    VALUE *lookupRaw(const T& key) {
        bdec2_Map_Node<KEY, VALUE> *node = d_hashtable.lookupRaw(key);
        return node ? &node->value() : 0;
    }
        // TBD doc
        // Return a pointer to the modifiable value corresponding to the
        // specified 'key' if 'key' is a member of this map, and 0 otherwise.
        // The pointer will remain valid as long as this map is not destroyed
        // or modified (e.g., via 'add', 'set', or 'remove').

    int remove(const KEY& key);
        // Remove the specified 'key' from this map.  Return 1 if the 'key' was
        // already a member of this map, and 0 otherwise.

    void removeAll();
        // Remove all members of this map.

    void reserveCapacity(int numElements);
        // Reserve sufficient internal capacity to accommodate up to the
        // specified 'numElements' values - excluding capacity required by the
        // string values - without reallocation.  The behavior is undefined
        // unless '0 <= numElements'.

    VALUE *set(const KEY& key, const VALUE& value);
        // Add to this map the association from the specified 'key' to the
        // specified 'value'.  If the 'key' is already a member, this method
        // replaces the 'value' corresponding to 'key'.  Return a pointer to
        // the, perhaps created, modifiable value.  The pointer will remain
        // valid as long as this map is not destroyed or modified (e.g., via
        // 'add', 'set', or 'remove').

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If the
        // specified 'version' is not supported, 'stream' is marked invalid,
        // but this object is unaltered.  Note that no version is read from
        // 'stream'.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    // ACCESSORS
    int isEqual(const bdec2_Map<KEY, VALUE, HASH>& rhs) const;
        // Return 1 if this map and the specified 'rhs' map have the same
        // value, and 0 otherwise.  Two maps have the same value if they have
        // the same length and all the elements of one map are members of the
        // other map.

    int isMember(const KEY& key) const;
        // Return 1 if the specified 'key' is an element of this map, and 0
        // otherwise.

    int length() const;
        // Return the number of elements in this map.

    const VALUE *lookup(const KEY& key) const;
        // Return a pointer to the non-modifiable value corresponding to the
        // specified 'key' if 'key' is a member of this map, and 0 otherwise.
        // The pointer will remain valid as long as this map is not destroyed
        // or modified (e.g., via 'add', 'set', or 'remove').

    template <class T>
    const VALUE *lookupRaw(const T& key) const {
        const bdec2_Map_Node<KEY, VALUE> *node = d_hashtable.lookupRaw(key);
        return node ? &node->value() : 0;
    }
        // TBD doc
        // Return a pointer to the non-modifiable value corresponding to the
        // specified 'key' if 'key' is a member of this map, and 0 otherwise.
        // The pointer will remain valid as long as this map is not destroyed
        // or modified (e.g., via 'add', 'set', or 'remove').

    const KEY *lookupKey(const KEY& key) const;
        // Return a pointer to the non-modifiable key corresponding to the
        // specified 'key' if 'key' is a member of this map, and 0 otherwise.
        // The pointer will remain valid as long as this map is not destroyed
        // or modified (e.g., via 'add', 'set', or 'remove').

    template <class T>
    const KEY *lookupKeyRaw(const T& key) const {
        const bdec2_Map_Node<KEY, VALUE> *node = d_hashtable.lookupRaw(key);
        return node ? &node->key() : 0;
    }
        // TBD doc
        // Return a pointer to the non-modifiable key corresponding to the
        // specified 'key' if 'key' is a member of this map, and 0 otherwise.
        // The pointer will remain valid as long as this map is not destroyed
        // or modified (e.g., via 'add', 'set', or 'remove').

    bsl::ostream& print(bsl::ostream& stream,
                        int           level,
                        int           spacesPerLevel) const;
        // Format the element values in this map to the specified output
        // 'stream' at the (absolute value of the) specified indentation
        // 'level' using the specified 'spacesPerLevel' of indentation.
        // Making 'level' negative suppress indentation for the first line
        // only.  The behavior is undefined unless '0 <= spacesPerLevel'.  Note
        // that the order of the elements is implementation dependent.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' and return a
        // reference to the modifiable 'stream'.  Optionally specify an
        // explicit 'version' format; by default, the maximum supported version
        // is written to 'stream' and used as the format.  If 'version' is
        // specified, that format is used but *not* written to 'stream'.  If
        // 'version' is not supported, 'stream' is left unmodified.  (See the
        // package-group-level documentation for more information on 'bdex'
        // streaming of container types).

    bsl::ostream& streamOut(bsl::ostream& stream) const
    {
        stream << "[ ";
        bdec2_MapIter<KEY, VALUE, HASH> iter(*this);
        while (iter) {
            stream << '(' << iter.key() << "," << iter.value() << ") ";
            ++iter;
        }
        return stream << ']';
    }
};

// FREE OPERATORS
template <class KEY, class VALUE, class HASH>
inline
bool operator==(const bdec2_Map<KEY, VALUE, HASH>& lhs,
                const bdec2_Map<KEY, VALUE, HASH>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' maps have the same value,
    // and 'false' otherwise.  Two maps have the same value if they have the
    // same length and all the elements of one map are members of the other
    // map.

template <class KEY, class VALUE, class HASH>
inline
bool operator!=(const bdec2_Map<KEY, VALUE, HASH>& lhs,
                const bdec2_Map<KEY, VALUE, HASH>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' maps do not have the same
    // value, and 'false' otherwise.  Two maps do not have the same value if
    // they differ in length or there exists an element of one map that is not
    // a member of the other map.

template <class KEY, class VALUE, class HASH>
inline
bsl::ostream& operator<<(bsl::ostream&                      stream,
                         const bdec2_Map<KEY, VALUE, HASH>& map);
    // Write the specified 'map' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

                         // ===================
                         // class bdec2_MapIter
                         // ===================

template <class KEY, class VALUE, class HASH>
class bdec2_MapIter {
    // Provide read-only, sequential access to the members of a map of
    // 'KEY' elements.  The iterator becomes invalid if any manipulator
    // of the underlying map is used.  Note that the order of iteration is
    // implementation dependent.

    // DATA
    bdeci_HashtableSlotIter<bdec2_Map_Node<KEY, VALUE>,
                            bdec2_Map_Node_Hash<KEY, VALUE, HASH> > d_iter;
    int d_nextSlotIndex;  // next slot index
    int d_numSlots;       // number of slots

    // NOT IMPLEMENTED
    //bool operator==(const bdec2_MapIter&) const;
    //bool operator!=(const bdec2_MapIter&) const;

    //template<class KEY2, class VALUE2, class HASH2>
    //bool operator==(const bdec2_MapIter<KEY2,VALUE2,HASH2>&) const;
    //template<class KEY2, class VALUE2, class HASH2>
    //bool operator!=(const bdec2_MapIter<KEY2,VALUE2,HASH2>&) const;

  public:
    // CREATORS
    bdec2_MapIter(const bdec2_Map<KEY, VALUE, HASH>& map);
        // Create an iterator for the specified map.

    bdec2_MapIter(const bdec2_MapIter<KEY, VALUE, HASH>& original);
        // TBD doc

    ~bdec2_MapIter();
        // Destroy this object.

    // MANIPULATORS
    bdec2_MapIter<KEY, VALUE, HASH>&
                         operator=(const bdec2_MapIter<KEY, VALUE, HASH>& rhs);
        // TBD doc

    void operator++();
        // Advance the iterator to refer to the next member of its associated
        // map.  The behavior is undefined unless the current map member is
        // valid.  Note that the order of iteration is implementation
        // dependent.

    // ACCESSORS
    operator const void *() const;
        // Return non-zero if the current member of the map associated with
        // this iterator is valid, and 0 otherwise.

    const KEY& key() const;
        // Return a reference to the non-modifiable 'key' of the current member
        // of the map associated with this iterator.  The behavior is undefined
        // unless the current map member is valid.

    const VALUE& value() const;
        // Return a reference to the non-modifiable 'value' of the current
        // member of the map associated with this iterator.  The behavior is
        // undefined unless the current map member is valid.
};

                         // ====================
                         // class bdec2_MapManip
                         // ====================

template <class KEY, class VALUE, class HASH>
class bdec2_MapManip {
    // Provide sequential access with selective-remove (filtering) capability
    // to members of a map of 'KEY' elements.  This manipulator object
    // becomes invalid if any manipulator method of the underlying map or other
    // manipulator is used.  Note that the order of iteration is implementation
    // dependent.

    // DATA
    bdeci_HashtableSlotManip<bdec2_Map_Node<KEY, VALUE>,
                             bdec2_Map_Node_Hash<KEY, VALUE, HASH> > d_manip;
    int d_nextSlotIndex;  // next slot index
    int d_numSlots;       // number of slots

    // NOT IMPLEMENTED
    bdec2_MapManip(const bdec2_MapManip&);
    bdec2_MapManip& operator=(const bdec2_MapManip&);

    bool operator==(const bdec2_MapManip&) const;
    bool operator!=(const bdec2_MapManip&) const;

    template<class KEY2, class VALUE2, class HASH2>
    bool operator==(const bdec2_MapManip<KEY2,VALUE2,HASH2>&) const;
    template<class KEY2, class VALUE2, class HASH2>
    bool operator!=(const bdec2_MapManip<KEY2,VALUE2,HASH2>&) const;

  public:
    // CREATORS
    bdec2_MapManip(bdec2_Map<KEY, VALUE, HASH> *map);
        // Create a manipulator for the specified 'map'.  Note that use of a
        // manipulator invalidates iterators and other manipulators operating
        // on the same map.

    ~bdec2_MapManip();
        // Destroy this object.

    // MANIPULATORS
    void advance();
        // Advance this manipulator to refer to the next member of its
        // associated map.  The behavior is undefined unless the current map
        // member is valid.  Note that the order of iteration is implementation
        // dependent.

    void remove();
        // Remove the current member of this manipulator's associated map, in
        // effect advancing to the next member.  The behavior is undefined
        // unless the current map member is valid.

    VALUE& value();
        // Return a reference to the modifiable 'value' of the current member
        // of the map associated with this manipulator.  The behavior is
        // undefined unless the current map member is valid.

    // ACCESSORS
    operator const void *() const;
        // Return non-zero if the current member of the map associated with
        // this manipulator is valid, and 0 otherwise.

    const KEY& key();
        // Return a reference to the non-modifiable 'key' of the current member
        // of the map associated with this manipulator.  The behavior is
        // undefined unless the current map member is valid.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

// PRIVATE ACCESSORS
template <class KEY, class VALUE, class HASH>
bsl::ostream& bdec2_Map<KEY, VALUE, HASH>::indent(bsl::ostream& stream,
                                 int level,
                                 int spacesPerLevel) const
{
    int n = level * spacesPerLevel;
    while (n-- > 0) {
        stream << ' ';
    }
    return stream;
}

// CLASS METHODS
template <class KEY, class VALUE, class HASH>
inline
int bdec2_Map<KEY, VALUE, HASH>::maxSupportedBdexVersion()
{
    return 1;
}

template <class KEY, class VALUE, class HASH>
inline
int bdec2_Map<KEY, VALUE, HASH>::maxSupportedVersion()
{
    return maxSupportedBdexVersion();
}

// CREATORS
template <class KEY, class VALUE, class HASH>
bdec2_Map<KEY, VALUE, HASH>::bdec2_Map(bslma_Allocator *basicAllocator)
: d_hashtable(basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

template <class KEY, class VALUE, class HASH>
bdec2_Map<KEY, VALUE, HASH>::bdec2_Map(const InitialCapacity&  numElements,
                                       bslma_Allocator        *basicAllocator)
: d_hashtable((typename bdeci_Hashtable<bdec2_Map_Node<KEY, VALUE>,
                              bdec2_Map_Node_Hash<KEY, VALUE, HASH> >::
                                            InitialCapacity)(numElements.d_i),
              basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

template <class KEY, class VALUE, class HASH>
bdec2_Map<KEY, VALUE, HASH>::bdec2_Map(const KEY       *keys,
                                       const VALUE     *values,
                                       int              numElements,
                                       bslma_Allocator *basicAllocator)
: d_hashtable((typename bdeci_Hashtable<bdec2_Map_Node<KEY, VALUE>,
                              bdec2_Map_Node_Hash<KEY, VALUE, HASH> >::
                                                 InitialCapacity)(numElements),
              basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    for (int i = 0; i < numElements; ++i) {
        add(keys[i], values[i]);
    }
}

template <class KEY, class VALUE, class HASH>
bdec2_Map<KEY, VALUE, HASH>::
                  bdec2_Map(const bdec2_Map<KEY, VALUE, HASH>&  original,
                            bslma_Allocator                    *basicAllocator)
: d_hashtable((typename bdeci_Hashtable<bdec2_Map_Node<KEY, VALUE>,
                              bdec2_Map_Node_Hash<KEY, VALUE, HASH> >::
                          InitialCapacity)(original.d_hashtable.numElements()),
              basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    d_hashtable = original.d_hashtable;
}

template <class KEY, class VALUE, class HASH>
inline
bdec2_Map<KEY, VALUE, HASH>::~bdec2_Map()
{
}

// MANIPULATORS
template <class KEY, class VALUE, class HASH>
inline
bdec2_Map<KEY, VALUE, HASH>& bdec2_Map<KEY, VALUE, HASH>::
                              operator=(const bdec2_Map<KEY, VALUE, HASH>& rhs)
{
    d_hashtable = rhs.d_hashtable;
    return *this;
}

template <class KEY, class VALUE, class HASH>
inline
VALUE *bdec2_Map<KEY, VALUE, HASH>::add(const KEY& key, const VALUE& value)
{
    bdec2_Map_Node<KEY, VALUE> tempNode(key, value, d_allocator_p);

    bdec2_Map_Node<KEY, VALUE> *node = d_hashtable.addUnique(tempNode);
    return node ? &node->value() : 0;
}

template <class KEY, class VALUE, class HASH>
inline
VALUE *bdec2_Map<KEY, VALUE, HASH>::lookup(const KEY& key)
{
    bdec2_Map_Node<KEY, VALUE> tempNode(key, VALUE(), d_allocator_p);

    bdec2_Map_Node<KEY, VALUE> *node = d_hashtable.lookup(tempNode);
    return node ? &node->value() : 0;
}

template <class KEY, class VALUE, class HASH>
inline
int bdec2_Map<KEY, VALUE, HASH>::remove(const KEY& key)
{
    bdec2_Map_Node<KEY, VALUE> tempNode(key, VALUE(), d_allocator_p);

    return d_hashtable.removeFirst(tempNode);
}

template <class KEY, class VALUE, class HASH>
inline
void bdec2_Map<KEY, VALUE, HASH>::removeAll()
{
    d_hashtable.removeAll();
}

template <class KEY, class VALUE, class HASH>
inline
void bdec2_Map<KEY, VALUE, HASH>::reserveCapacity(int numElements)
{
    d_hashtable.reserveCapacity(numElements);
}

template <class KEY, class VALUE, class HASH>
inline
VALUE *bdec2_Map<KEY, VALUE, HASH>::set(const KEY& key, const VALUE& value)
{
    bdec2_Map_Node<KEY, VALUE> tempNode(key, value, d_allocator_p);

    bdec2_Map_Node<KEY, VALUE> *node = d_hashtable.set(tempNode);
    return &node->value();
}

template <class KEY, class VALUE, class HASH>
template <class STREAM>
STREAM& bdec2_Map<KEY, VALUE, HASH>::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on schema version (starting with 1).
          case 1: {
            int newLength;

            stream.getLength(newLength);
            if (!stream) {
                return stream;                                        // RETURN
            }

            if (newLength < 0) {
                stream.invalidate();
                return stream;                                        // RETURN
            }

            d_hashtable.removeAll();
            d_hashtable.reserveCapacity(newLength);
            for (int i = 0; i < newLength; ++i) {
                KEY key;
                VALUE value;

                stream >> key;  // TBD
                if (!stream) {
                    return stream;                                    // RETURN
                }

                stream >> value;  // TBD
                if (!stream) {
                    return stream;                                    // RETURN
                }

                if (0 == add(key, value)) {
                    stream.invalidate();
                    return stream;                                    // RETURN
                }
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
template <class KEY, class VALUE, class HASH>
inline
int bdec2_Map<KEY, VALUE, HASH>::length() const
{
    return d_hashtable.numElements();
}

template <class KEY, class VALUE, class HASH>
int bdec2_Map<KEY, VALUE, HASH>::
                          isEqual(const bdec2_Map<KEY, VALUE, HASH>& rhs) const
{
    if (length() != rhs.length()) {
        return 0;                                                     // RETURN
    }
    const int rhsNumSlots = rhs.d_hashtable.numSlots();

    if (d_hashtable.numSlots() == rhsNumSlots) {

        // Since the underlying hashtables have the same logical size and the
        // identical hash function is used in both maps, an element of one map
        // must be in the same slot as the other map.

        for (int si = 0; si < rhsNumSlots; ++si) {
            bdeci_HashtableSlotIter<bdec2_Map_Node<KEY, VALUE>,
                                    bdec2_Map_Node_Hash<KEY, VALUE, HASH> >
                                                       ri(rhs.d_hashtable, si);
            while (ri) {
                bdeci_HashtableSlotIter<bdec2_Map_Node<KEY, VALUE>,
                                        bdec2_Map_Node_Hash<KEY, VALUE, HASH> >
                                                       li(d_hashtable, si);
                while (li && ri() != li()) {
                    ++li;
                }
                if (!li) {
                    return 0;                                         // RETURN
                }
                if (!(li().value() == ri().value())) {
                    return 0;                                         // RETURN
                }
                ++ri;
            }
        }
    }
    else {
        for (int si = 0; si < rhsNumSlots; ++si) {
            bdeci_HashtableSlotIter<bdec2_Map_Node<KEY, VALUE>,
                                    bdec2_Map_Node_Hash<KEY, VALUE, HASH> >
                                                     iter(rhs.d_hashtable, si);
            while (iter) {
                const bdec2_Map_Node<KEY, VALUE> *n =
                                                    d_hashtable.lookup(iter());
                if (!n) {
                    return 0;                                         // RETURN
                }
                if (!(iter().value() == n->value())) {
                    return 0;                                         // RETURN
                }
                ++iter;
            }
        }
    }
    return 1;
}

template <class KEY, class VALUE, class HASH>
inline
int bdec2_Map<KEY, VALUE, HASH>::isMember(const KEY& key) const
{
    bdec2_Map_Node<KEY, VALUE> tempNode(key, VALUE(), d_allocator_p);

    return d_hashtable.isMember(tempNode);
}

template <class KEY, class VALUE, class HASH>
inline
const VALUE *bdec2_Map<KEY, VALUE, HASH>::lookup(const KEY& key) const
{
    bdec2_Map_Node<KEY, VALUE> tempNode(key, VALUE(), d_allocator_p);

    const bdec2_Map_Node<KEY, VALUE> *node = d_hashtable.lookup(tempNode);
    return node ? &node->value() : 0;
}

template <class KEY, class VALUE, class HASH>
inline
const KEY *bdec2_Map<KEY, VALUE, HASH>::lookupKey(const KEY& key) const
{
    bdec2_Map_Node<KEY, VALUE> tempNode(key, VALUE(), d_allocator_p);

    const bdec2_Map_Node<KEY, VALUE> *node = d_hashtable.lookup(tempNode);
    return node ? &node->key() : 0;
}

template <class KEY, class VALUE, class HASH>
bsl::ostream& bdec2_Map<KEY, VALUE, HASH>::
               print(bsl::ostream& stream, int level, int spacesPerLevel) const
{
    indent(stream, level, spacesPerLevel);
    stream << "[\n";
    if (level < 0) {
        level = -level;
    }
    int levelPlus1 = level + 1;
    const int numSlots = d_hashtable.numSlots();
    for (int si = 0; si < numSlots; ++si) {
        bdeci_HashtableSlotIter<bdec2_Map_Node<KEY, VALUE>,
                                bdec2_Map_Node_Hash<KEY, VALUE, HASH> >
                                                         iter(d_hashtable, si);
        while (iter) {
            indent(stream, levelPlus1, spacesPerLevel);
            stream << '('
                   << iter().key()
                   << ','
                   << iter().value()
                   << ")\n";
            ++iter;
        }
    }
    indent(stream, level, spacesPerLevel);
    return stream << "]\n";
}

template <class KEY, class VALUE, class HASH>
template <class STREAM>
STREAM& bdec2_Map<KEY, VALUE, HASH>::bdexStreamOut(STREAM& stream, int) const
{
    stream.putLength(length());

    const int numSlots = d_hashtable.numSlots();
    for (int si = 0; si < numSlots; ++si) {
      bdeci_HashtableSlotIter<bdec2_Map_Node<KEY, VALUE>,
                                    bdec2_Map_Node_Hash<KEY, VALUE, HASH> >
                                                         iter(d_hashtable, si);
      while (iter) {
          stream << iter().key();    // TBD
          stream << iter().value();  // TBD
          ++iter;
      }
    }
    return stream;
}

// FREE OPERATORS
template <class KEY, class VALUE, class HASH>
inline
bool operator==(const bdec2_Map<KEY, VALUE, HASH>& lhs,
                const bdec2_Map<KEY, VALUE, HASH>& rhs)
{
    return lhs.isEqual(rhs);
}

template <class KEY, class VALUE, class HASH>
inline
bool operator!=(const bdec2_Map<KEY, VALUE, HASH>& lhs,
                const bdec2_Map<KEY, VALUE, HASH>& rhs)
{
    return !(lhs == rhs);
}

template <class KEY, class VALUE, class HASH>
inline
bsl::ostream& operator<<(bsl::ostream&                      stream,
                         const bdec2_Map<KEY, VALUE, HASH>& map)
{
    return map.streamOut(stream);
}

                             // -------------------
                             // class bdec2_MapIter
                             // -------------------

// CREATORS
template <class KEY, class VALUE, class HASH>
bdec2_MapIter<KEY, VALUE, HASH>::
                          bdec2_MapIter(const bdec2_Map<KEY, VALUE, HASH>& map)
: d_iter(map.d_hashtable, 0)
, d_nextSlotIndex(1)
, d_numSlots(map.d_hashtable.numSlots())
{
    while (!d_iter && d_nextSlotIndex < d_numSlots) {
        d_iter.setSlot(d_nextSlotIndex);
        ++d_nextSlotIndex;
    }
}

template <class KEY, class VALUE, class HASH>
inline
bdec2_MapIter<KEY, VALUE, HASH>::
                 bdec2_MapIter(const bdec2_MapIter<KEY, VALUE, HASH>& original)
: d_iter(original.d_iter)
, d_nextSlotIndex(original.d_nextSlotIndex)
, d_numSlots(original.d_numSlots)
{
}

template <class KEY, class VALUE, class HASH>
inline
bdec2_MapIter<KEY, VALUE, HASH>::~bdec2_MapIter()
{
}

// MANIPULATORS
template <class KEY, class VALUE, class HASH>
inline
bdec2_MapIter<KEY, VALUE, HASH>& bdec2_MapIter<KEY, VALUE, HASH>::
                          operator=(const bdec2_MapIter<KEY, VALUE, HASH>& rhs)
{
    d_iter          = rhs.d_iter;
    d_nextSlotIndex = rhs.d_nextSlotIndex;
    d_numSlots      = rhs.d_numSlots;
    return *this;
}

template <class KEY, class VALUE, class HASH>
void bdec2_MapIter<KEY, VALUE, HASH>::operator++()
{
    ++d_iter;
    while (!d_iter && d_nextSlotIndex < d_numSlots) {
        d_iter.setSlot(d_nextSlotIndex);
        ++d_nextSlotIndex;
    }
}

// ACCESSORS
template <class KEY, class VALUE, class HASH>
inline
bdec2_MapIter<KEY, VALUE, HASH>::operator const void *() const
{
    return d_iter;
}

template <class KEY, class VALUE, class HASH>
inline
const KEY& bdec2_MapIter<KEY, VALUE, HASH>::key() const
{
    return d_iter().key();
}

template <class KEY, class VALUE, class HASH>
inline
const VALUE& bdec2_MapIter<KEY, VALUE, HASH>::value() const
{
    return d_iter().value();
}

                             // --------------------
                             // class bdec2_MapManip
                             // --------------------

// CREATORS
template <class KEY, class VALUE, class HASH>
bdec2_MapManip<KEY, VALUE, HASH>::
                               bdec2_MapManip(bdec2_Map<KEY, VALUE, HASH> *map)
: d_manip(&map->d_hashtable, 0)
, d_nextSlotIndex(1)
, d_numSlots(map->d_hashtable.numSlots())
{
    while (!d_manip && d_nextSlotIndex < d_numSlots) {
        d_manip.setSlot(d_nextSlotIndex);
        ++d_nextSlotIndex;
    }
}

template <class KEY, class VALUE, class HASH>
inline
bdec2_MapManip<KEY, VALUE, HASH>::~bdec2_MapManip()
{
}

// MANIPULATORS
template <class KEY, class VALUE, class HASH>
void bdec2_MapManip<KEY, VALUE, HASH>::advance()
{
    d_manip.advance();
    while (!d_manip && d_nextSlotIndex < d_numSlots) {
        d_manip.setSlot(d_nextSlotIndex);
        ++d_nextSlotIndex;
    }
}

template <class KEY, class VALUE, class HASH>
inline
void bdec2_MapManip<KEY, VALUE, HASH>::remove()
{
    d_manip.remove();
}

template <class KEY, class VALUE, class HASH>
inline
VALUE& bdec2_MapManip<KEY, VALUE, HASH>::value()
{
    return d_manip().value();
}

// ACCESSORS
template <class KEY, class VALUE, class HASH>
inline
bdec2_MapManip<KEY, VALUE, HASH>::operator const void *() const
{
    return d_manip;
}

template <class KEY, class VALUE, class HASH>
inline
const KEY& bdec2_MapManip<KEY, VALUE, HASH>::key()
{
    return d_manip().key();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
