// bdec2_intmap.h                -*-C++-*-
#ifndef INCLUDED_BDEC2_INTMAP
#define INCLUDED_BDEC2_INTMAP

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide associations of unique 'int' to 'T' values.
//
//@DEPRECATED: Use 'bsl::map<int, T>' instead.
//
//@CLASSES:
//        bdec2_IntMap: memory manager for a map of 'int' to 'T' values
//    bdec2_IntMapIter: sequential read-only accessor of map members
//   bdec2_IntMapManip: sequential selective remover of map members
//
//@AUTHOR: Jeffrey Mendelsohn (jmendelsohn)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component implements an efficient map class for elements
// of type 'int' to 'T' values.  Iterators and manipulators provide efficient
// access and filtering (i.e., selectively removing) of members, respectively.
//
// 'T' may not be 'bslma_Allocator' or 'bslma_Allocator *' and may not be
// 'const'.  The type 'T' must have a public copy constructor, a public
// destructor, and the assignment operator.
//
// 'T' may optionally have a 'bslma_Allocator *' argument to its copy
// constructor.  If this is the case, the
// 'bslalg_TypeTraitUsesBslmaAllocator' trait must be used.  In this case,
// the signature for thecopy constructor should be
//..
//     T(const T& original, bslma_Allocator *basicAllocator = 0);
//..
// Otherwise the signatures for the copy constructor should be:
//..
//     T(const T& original);
//..
// The bdex_OutStream operator<<(bdex_OutStream&, const T&) and
// bdex_InStream operator>>(bdex_InStream&, T&) are optional.  If they exist,
// the corresponding map >> and << operators may be used.
//
// The bsl::ostream& operator<<(bsl::ostream&, const T&) for the map parameter
// 'T' is optional.  If it exists, the map's ostream operator<< may be used.
//
// 'T' may optionally have 'operator=='.  If this operator is defined, the
// map's equality and inequality operators == and != may be used.  The map does
// not use the inequality operator of 'T'.
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
//
///MAP USAGE
///---------
// The following snippets of code illustrate how to create and use a map.
// First create an empty 'bdec2_IntMap' 's' and populate it with elements:
//..
//      const int K1 = 100;
//      const int K2 = 200;
//      const int K3 = 300;
//
//      const char *V1 = "a";
//      const char *V2 = "ab";
//      const char *V3 = "abc";
//
//      bdec2_IntMap<bsl::string> s;   assert( 0 == s.length());
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
//
///ITERATOR USAGE
///--------------
// The following shows the standard iterator pattern:
//..
//      for (bdec2_IntMapIter<bsl::string> it(s); it; ++it) {
//          bsl::cout << it.key() << ' ' << it.value() << bsl::endl;
//      }
//..
//
///MANIPULATOR USAGE
///-----------------
// The following illustrates the (less-common) manipulator pattern:
//..
//      bdec2_IntMapManip<bsl::string> manip(&s);
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

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDECI_HASHTABLE
#include <bdeci_hashtable.h>
#endif

#ifndef INCLUDED_BDEIMP_INTHASH
#include <bdeimp_inthash.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BSL_NEW
#include <bsl_new.h>         // placement syntax
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif


namespace BloombergLP {

template <class VALUE>
class bdec2_IntMapIter;

template <class VALUE>
class bdec2_IntMapManip;

                       // ========================
                       // struct bdec2_IntMap_Node
                       // ========================

// This struct should be private to the class bdec2_IntMap.  Due to compiler
// issues (e.g., IBM AIX xlC) it is not.  Do not use this struct outside of
// this component.

template <class VALUE>
struct bdec2_IntMap_Node {
    // This 'struct' implements the storage and functionality required for a
    // map node.

    int                      d_key;
    bsls_ObjectBuffer<VALUE> d_valueStorage;

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdec2_IntMap_Node,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    bdec2_IntMap_Node(int                               key,
                      const VALUE&                      value,
                      bslma_Allocator                  *allocator)
    : d_key(key)
    {
        bslalg_ScalarPrimitives::copyConstruct(&d_valueStorage.object(),
                                                value,
                                                allocator);
    }

    bdec2_IntMap_Node(const bdec2_IntMap_Node<VALUE>&  original,
                      bslma_Allocator                 *allocator = 0)
    : d_key(original.d_key)
    {
        BSLS_ASSERT_SAFE(allocator);  // allocator is *required*

        bslalg_ScalarPrimitives::copyConstruct(&d_valueStorage.object(),
                                                original.value(),
                                                allocator);
    }

    ~bdec2_IntMap_Node()
    {
        d_valueStorage.object().~VALUE();
    }

    bdec2_IntMap_Node& operator=(const bdec2_IntMap_Node& rhs)
    {
        d_key = rhs.d_key;
        d_valueStorage.object() = rhs.d_valueStorage.object();
        return *this;
    }

    int& key() { return d_key; }
    VALUE& value() { return d_valueStorage.object(); }

    const int& key() const { return d_key; }
    const VALUE& value() const { return d_valueStorage.object(); }

    bool operator==(const bdec2_IntMap_Node& rhs) const
                   { return d_key == rhs.d_key; }
    bool operator!=(const bdec2_IntMap_Node& rhs) const
                   { return d_key != rhs.d_key; }
};

                       // ========================
                       // struct bdec2_IntMap_Hash
                       // ========================

// This struct should be private to the class bdec2_IntMap.  Due to compiler
// issues (e.g., IBM AIX xlC) it is not.  Do not use this struct outside of
// this component.

template <class VALUE>
struct bdec2_IntMap_Hash {
    // This 'struct' implements the hash function for the Node.
    static int hash(const bdec2_IntMap_Node<VALUE>& node, int size)
                            { return bdeimp_IntHash::hash(node.d_key, size); }
};

                           // ==================
                           // class bdec2_IntMap
                           // ==================

template <class VALUE>
class bdec2_IntMap {
    // This class implements an efficient map of 'int' to 'VALUE' values.  It
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
    bdeci_Hashtable<bdec2_IntMap_Node<VALUE>,
                    bdec2_IntMap_Hash<VALUE> >  d_hashtable;    // hash table
    bslma_Allocator                            *d_allocator_p;  // holds memory
                                                                // allocator

    // FRIENDS
    friend class bdec2_IntMapIter<VALUE>;
    friend class bdec2_IntMapManip<VALUE>;

  private:
    // PRIVATE ACCESSORS
    bsl::ostream&
             indent(bsl::ostream& stream, int level, int spacesPerLevel) const;
        // Emit the product of the specified 'level' and 'spacesPerLevel' to
        // the specified output 'stream' and return a reference to the
        // modifiable 'stream'.  If the product is negative, this function has
        // no effect.  The behavior is undefined unless 0 <= spacesPerLevel.
  public:
    // TYPES
    struct InitialCapacity {
        // Enable uniform use of an optional integral constructor argument to
        // specify the initial internal capacity (in elements).  For example,
        //..
        //   bdec2_IntMap x(bdec2_IntMap::InitialCapacity(8));
        //..
        // defines an instance, 'x', with an initial capacity of 8 elements,
        // but with a logical length of 0 elements.

        int d_i;
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
    explicit
    bdec2_IntMap(bslma_Allocator *basicAllocator = 0);
        // Create an empty map.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    explicit
    bdec2_IntMap(const InitialCapacity&  numElements,
                 bslma_Allocator        *basicAllocator = 0);
        // Create an empty map with sufficient initial capacity to accommodate
        // up to the specified 'numElements' values without subsequent
        // reallocation.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined unless
        // 0 <= numElements.

    bdec2_IntMap(const int       *keys,
                 const VALUE     *values,
                 int              numElements,
                 bslma_Allocator *basicAllocator = 0);
        // Create a map initialized to having the specified 'numElements'
        // associations from respective elements of the specified 'keys' array
        // and the specified 'values' array.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless 0 <= numElements.

    bdec2_IntMap(const bdec2_IntMap<VALUE>&  original,
                 bslma_Allocator            *basicAllocator = 0);
        // Create a map initialized to the value of the specified 'original'
        // map.  Optionally specify a 'basicAllocator' used to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.

    ~bdec2_IntMap();
        // Destroy this object.

    // MANIPULATORS
    bdec2_IntMap<VALUE>& operator=(const bdec2_IntMap<VALUE>& rhs);
        // Assign to this map the value of the specified 'rhs' map.  Return a
        // reference to this map.

    VALUE *add(int key, const VALUE& value);
        // Add to this map the association from the specified 'key' to the
        // specified 'value'.  If the 'key' is already a member, this method
        // has no effect.  Return a pointer to the, newly created,  modifiable
        // value if 'key' was not already a member of this map, and 0
        // otherwise.  The pointer will remain valid as long as this map is not
        // destroyed or modified (e.g., via 'add', 'set', or 'remove').

    VALUE *lookup(int key);
        // Return a pointer to the modifiable value corresponding to the
        // specified 'key' if 'key' is a member of this map, and 0 otherwise.
        // The pointer will remain valid as long as this map is not destroyed
        // or modified (e.g., via 'add', 'set', or 'remove').

    int remove(int key);
        // Remove the specified 'key' from this map.  Return 1 if the 'key' was
        // already a member of this map, and 0 otherwise.

    void removeAll();
        // Remove all members of this map.

    void reserveCapacity(int numElements);
        // Reserve sufficient internal capacity to accommodate up to the
        // specified 'numElements' values - excluding capacity required by the
        // string values - without reallocation.  The behavior is undefined
        // unless 0 <= numElements.

    VALUE *set(int key, const VALUE& value);
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
    int isEqual(const bdec2_IntMap<VALUE>& rhs) const;
        // Return 1 if this map and the specified 'rhs' map have the same
        // value, and 0 otherwise.  Two maps have the same value if they have
        // the same length and all the elements of one map are members of the
        // other map.

    int isMember(int key) const;
        // Return 1 if the specified 'key' is an element of this map, and 0
        // otherwise.

    int length() const;
        // Return the number of elements in this map.

    const VALUE *lookup(int key) const;
        // Return a pointer to the non-modifiable value corresponding to the
        // specified 'key' if 'key' is a member of this map, and 0 otherwise.
        // The pointer will remain valid as long as this map is not destroyed
        // or modified (e.g., via 'add', 'set', or 'remove').

    const int *lookupKey(int key) const;
        // Return a pointer to the non-modifiable key corresponding to the
        // specified 'key' if 'key' is a member of this map, and 0 otherwise.
        // The pointer will remain valid as long as this map is not destroyed
        // or modified (e.g., via 'add', 'set', or 'remove').

    bsl::ostream& print(bsl::ostream& stream,
                   int      level,
                   int      spacesPerLevel) const;
        // Format the element values in this map to the specified output
        // 'stream' at the (absolute value of the) specified indentation
        // 'level' using the specified 'spacesPerLevel' of indentation.
        // Making 'level' negative suppress indentation for the first line
        // only.  The behavior is undefined unless 0 <= spacesPerLevel.  Note
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
        bdec2_IntMapIter<VALUE> iter(*this);
        while (iter) {
            stream << '(' << iter.key() << "," << iter.value() << ") ";
            ++iter;
        }
        return stream << ']';
    }
};

// FREE OPERATORS
template <class VALUE>
inline
bool operator==(const bdec2_IntMap<VALUE>& lhs,
                                               const bdec2_IntMap<VALUE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' maps have the same value,
    // and 'false' otherwise.  Two maps have the same value if they have the
    // same length and all the elements of one map are members of the other
    // map.

template <class VALUE>
inline
bool operator!=(const bdec2_IntMap<VALUE>& lhs,
                                               const bdec2_IntMap<VALUE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' maps do not have the same
    // value, and 'false' otherwise.  Two maps do not have the same value if
    // they differ in length or there exists an element of one map that is not
    // a member of the other map.

template <class VALUE>
inline
bsl::ostream& operator<<(bsl::ostream&              stream,
                         const bdec2_IntMap<VALUE>& map);
    // Write the specified 'map' to the specified output 'stream' and return a
    // reference to the modifiable 'stream'.

                         // ======================
                         // class bdec2_IntMapIter
                         // ======================

template <class VALUE>
class bdec2_IntMapIter {
    // Provide read-only, sequential access to the members of a map of
    // 'int' elements.  The iterator becomes invalid if any manipulator
    // of the underlying map is used.  Note that the order of iteration is
    // implementation dependent.

    bdeci_HashtableSlotIter<bdec2_IntMap_Node<VALUE>,
                            bdec2_IntMap_Hash<VALUE> > d_iter;
    int d_nextSlotIndex;  // next slot index
    int d_numSlots;       // number of slots

    // NOT IMPLEMENTED
    bdec2_IntMapIter(const bdec2_IntMapIter&);
    bdec2_IntMapIter& operator=(const bdec2_IntMapIter&);

    bool operator==(const bdec2_IntMapIter&) const;
    bool operator!=(const bdec2_IntMapIter&) const;

    template<class OTHER>
    bool operator==(const bdec2_IntMapIter<OTHER>&) const;
    template<class OTHER>
    bool operator!=(const bdec2_IntMapIter<OTHER>&) const;

  public:
    // CREATORS
    bdec2_IntMapIter(const bdec2_IntMap<VALUE>& map);
        // Create an iterator for the specified map.

    ~bdec2_IntMapIter();
        // Destroy this object.

    // MANIPULATORS
    void operator++();
        // Advance the iterator to refer to the next member of its associated
        // map.  The behavior is undefined unless the current map member is
        // valid.  Note that the order of iteration is implementation
        // dependent.

    // ACCESSORS
    operator const void *() const;
        // Return non-zero if the current member of the map associated with
        // this iterator is valid, and 0 otherwise.

    const int& key() const;
        // Return a reference to the non-modifiable 'key' of the current member
        // of the map associated with this iterator.  The behavior is undefined
        // unless the current map member is valid.

    const VALUE& value() const;
        // Return a reference to the non-modifiable 'value' of the current
        // member of the map associated with this iterator.  The behavior is
        // undefined unless the current map member is valid.
};

                         // =======================
                         // class bdec2_IntMapManip
                         // =======================

template <class VALUE>
class bdec2_IntMapManip {
    // Provide sequential access with selective-remove (filtering) capability
    // to members of a map of 'int' elements.  This manipulator object
    // becomes invalid if any manipulator method of the underlying map or other
    // manipulator is used.  Note that the order of iteration is implementation
    // dependent.

    bdeci_HashtableSlotManip<bdec2_IntMap_Node<VALUE>,
                             bdec2_IntMap_Hash<VALUE> > d_manip;
    int d_nextSlotIndex;  // next slot index
    int d_numSlots;       // number of slots

    // NOT IMPLEMENTED
    bdec2_IntMapManip(const bdec2_IntMapManip&);
    bdec2_IntMapManip& operator=(const bdec2_IntMapManip&);

    bool operator==(const bdec2_IntMapManip&) const;
    bool operator!=(const bdec2_IntMapManip&) const;

    template<class OTHER>
    bool operator==(const bdec2_IntMapManip<OTHER>&) const;
    template<class OTHER>
    bool operator!=(const bdec2_IntMapManip<OTHER>&) const;

  public:
    // CREATORS
    bdec2_IntMapManip(bdec2_IntMap<VALUE> *map);
        // Create a manipulator for the specified 'map'.  Note that use of a
        // manipulator invalidates iterators and other manipulators operating
        // on the same map.

    ~bdec2_IntMapManip();
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

    const int& key();
        // Return a reference to the non-modifiable 'key' of the current member
        // of the map associated with this manipulator.  The behavior is
        // undefined unless the current map member is valid.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                              // ------------
                              // bdec2_IntMap
                              // ------------

// CREATORS
template <class VALUE>
bdec2_IntMap<VALUE>::bdec2_IntMap(bslma_Allocator *basicAllocator)
: d_hashtable(basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

template <class VALUE>
bdec2_IntMap<VALUE>::bdec2_IntMap(const InitialCapacity&  numElements,
                                  bslma_Allocator        *basicAllocator)
: d_hashtable((typename bdeci_Hashtable<bdec2_IntMap_Node<VALUE>,
                              bdec2_IntMap_Hash<VALUE> >::
                                             InitialCapacity)(numElements.d_i),
              basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

template <class VALUE>
bdec2_IntMap<VALUE>
             ::bdec2_IntMap(const bdec2_IntMap<VALUE>&  original,
                            bslma_Allocator            *basicAllocator)
: d_hashtable((typename bdeci_Hashtable<bdec2_IntMap_Node<VALUE>,
                              bdec2_IntMap_Hash<VALUE> >::
                          InitialCapacity)(original.d_hashtable.numElements()),
              basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    d_hashtable = original.d_hashtable;
}

template <class VALUE>
inline
bdec2_IntMap<VALUE>::~bdec2_IntMap()
{
}

template <class VALUE>
inline
bdec2_IntMap<VALUE>&
bdec2_IntMap<VALUE>::operator=(const bdec2_IntMap<VALUE>& rhs)
{
    d_hashtable = rhs.d_hashtable;
    return *this;
}

template <class VALUE>
inline
VALUE *bdec2_IntMap<VALUE>::add(int key, const VALUE& value)
{
    bdec2_IntMap_Node<VALUE> *node =
             d_hashtable.addUnique(bdec2_IntMap_Node<VALUE>(key,
                                                            value,
                                                            d_allocator_p));
    return node ? &node->value() : 0;
}

template <class VALUE>
inline
VALUE *bdec2_IntMap<VALUE>::lookup(int key)
{
    bdec2_IntMap_Node<VALUE> *node =
                d_hashtable.lookup(bdec2_IntMap_Node<VALUE>(key,
                                                            VALUE(),
                                                            d_allocator_p));
    return node ? &node->value() : 0;
}

template <class VALUE>
inline
int bdec2_IntMap<VALUE>::remove(int key)
{
    return d_hashtable.removeFirst(bdec2_IntMap_Node<VALUE>(key,
                                                            VALUE(),
                                                            d_allocator_p));
}

template <class VALUE>
inline
void bdec2_IntMap<VALUE>::removeAll()
{
    d_hashtable.removeAll();
}

template <class VALUE>
inline
void bdec2_IntMap<VALUE>::reserveCapacity(int numElements)
{
    d_hashtable.reserveCapacity(numElements);
}

template <class VALUE>
inline
VALUE *bdec2_IntMap<VALUE>::set(int key, const VALUE& value)
{
    bdec2_IntMap_Node<VALUE> *node =
                   d_hashtable.set(bdec2_IntMap_Node<VALUE>(key,
                                                            value,
                                                            d_allocator_p));
    return &node->value();
}

template <class VALUE>
inline
int bdec2_IntMap<VALUE>::length() const
{
    return d_hashtable.numElements();
}

template <class VALUE>
inline
int bdec2_IntMap<VALUE>::isMember(int key) const
{
    return d_hashtable.isMember(bdec2_IntMap_Node<VALUE>(key,
                                                         VALUE(),
                                                         d_allocator_p));
}

template <class VALUE>
inline
const VALUE *bdec2_IntMap<VALUE>::lookup(int key) const
{
    const bdec2_IntMap_Node<VALUE> *node =
                d_hashtable.lookup(bdec2_IntMap_Node<VALUE>(key,
                                                            VALUE(),
                                                            d_allocator_p));
    return node ? &node->value() : 0;
}

template <class VALUE>
inline
const int *bdec2_IntMap<VALUE>::lookupKey(int key) const
{
    const bdec2_IntMap_Node<VALUE> *node =
                d_hashtable.lookup(bdec2_IntMap_Node<VALUE>(key,
                                                            VALUE(),
                                                            d_allocator_p));
    return node ? &node->key() : 0;
}

template <class VALUE>
template <class STREAM>
STREAM& bdec2_IntMap<VALUE>::bdexStreamOut(STREAM& stream, int) const
{
    stream.putLength(length());

    const int numSlots = d_hashtable.numSlots();
    for (int si = 0; si < numSlots; ++si) {
        bdeci_HashtableSlotIter<bdec2_IntMap_Node<VALUE>,
                              bdec2_IntMap_Hash<VALUE> > iter(d_hashtable, si);
        while (iter) {
            stream << iter().key();
            stream << iter().value();
            ++iter;
        }
    }
    return stream;
}

// FREE OPERATORS
template <class VALUE>
inline
bool operator==(const bdec2_IntMap<VALUE>& lhs, const bdec2_IntMap<VALUE>& rhs)
{
    return lhs.isEqual(rhs);
}

template <class VALUE>
inline
bool operator!=(const bdec2_IntMap<VALUE>& lhs, const bdec2_IntMap<VALUE>& rhs)
{
    return !(lhs == rhs);
}

template <class VALUE>
inline
bsl::ostream& operator<<(bsl::ostream&              stream,
                         const bdec2_IntMap<VALUE>& map)
{
    return map.streamOut(stream);
}

                            // ----------------
                            // bdec2_IntMapIter
                            // ----------------

template <class VALUE>
bdec2_IntMapIter<VALUE>::bdec2_IntMapIter(const bdec2_IntMap<VALUE>& map)
: d_iter(map.d_hashtable, 0)
, d_nextSlotIndex(1)
, d_numSlots(map.d_hashtable.numSlots())
{
    while (!d_iter && d_nextSlotIndex < d_numSlots) {
        d_iter.setSlot(d_nextSlotIndex);
        ++d_nextSlotIndex;
    }
}

template <class VALUE>
inline
bdec2_IntMapIter<VALUE>::~bdec2_IntMapIter()
{
}

template <class VALUE>
void bdec2_IntMapIter<VALUE>::operator++()
{
    ++d_iter;
    while (!d_iter && d_nextSlotIndex < d_numSlots) {
        d_iter.setSlot(d_nextSlotIndex);
        ++d_nextSlotIndex;
    }
}

template <class VALUE>
inline
bdec2_IntMapIter<VALUE>::operator const void *() const
{
    return d_iter;
}

template <class VALUE>
inline
const int& bdec2_IntMapIter<VALUE>::key() const
{
    return d_iter().key();
}

template <class VALUE>
inline
const VALUE& bdec2_IntMapIter<VALUE>::value() const
{
    return d_iter().value();
}

                            // -----------------
                            // bdec2_IntMapManip
                            // -----------------

template <class VALUE>
bdec2_IntMapManip<VALUE>::bdec2_IntMapManip(bdec2_IntMap<VALUE> *map)
: d_manip(&map->d_hashtable, 0)
, d_nextSlotIndex(1)
, d_numSlots(map->d_hashtable.numSlots())
{
    while (!d_manip && d_nextSlotIndex < d_numSlots) {
        d_manip.setSlot(d_nextSlotIndex);
        ++d_nextSlotIndex;
    }
}

template <class VALUE>
inline
bdec2_IntMapManip<VALUE>::~bdec2_IntMapManip()
{
}

template <class VALUE>
void bdec2_IntMapManip<VALUE>::advance()
{
    d_manip.advance();
    while (!d_manip && d_nextSlotIndex < d_numSlots) {
        d_manip.setSlot(d_nextSlotIndex);
        ++d_nextSlotIndex;
    }
}

template <class VALUE>
inline
void bdec2_IntMapManip<VALUE>::remove()
{
    d_manip.remove();
}

template <class VALUE>
inline
VALUE& bdec2_IntMapManip<VALUE>::value()
{
    return d_manip().value();
}

template <class VALUE>
inline
bdec2_IntMapManip<VALUE>::operator const void *() const
{
    return d_manip;
}

template <class VALUE>
inline
const int& bdec2_IntMapManip<VALUE>::key()
{
    return d_manip().key();
}

// ===========================================================================
//                      NON-INLINE FUNCTION DEFINITIONS
// ===========================================================================
// TBD these are inlined

// PRIVATE ACCESSORS
template <class VALUE>
bsl::ostream& bdec2_IntMap<VALUE>::indent(bsl::ostream& stream,
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
template <class VALUE>
inline
int bdec2_IntMap<VALUE>::maxSupportedBdexVersion()
{
    return 1;  // Required by BDE policy; versions start at 1.
}

template <class VALUE>
inline
int bdec2_IntMap<VALUE>::maxSupportedVersion()
{
    return maxSupportedBdexVersion();
}

// CREATORS
template <class VALUE>
bdec2_IntMap<VALUE>::bdec2_IntMap(const int       *keys,
                                  const VALUE     *values,
                                  int              numElements,
                                  bslma_Allocator *basicAllocator)
: d_hashtable(bdeci_Hashtable<bdec2_IntMap_Node<VALUE>,
                              bdec2_IntMap_Hash<VALUE> >::
                                                  InitialCapacity(numElements),
              basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    for (int i = 0; i < numElements; ++i) {
        add(keys[i], values[i]);
    }
}

// MANIPULATORS
template <class VALUE>
template <class STREAM>
STREAM& bdec2_IntMap<VALUE>::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on schema version (starting with 1).
          case 1: {
            int newLength;

            stream.getLength(newLength);
            if (!stream) {
                return stream;                                  // RETURN
            }

            if (newLength < 0) {
                stream.invalidate();
                return stream;                                  // RETURN
            }

            d_hashtable.removeAll();
            d_hashtable.reserveCapacity(newLength);
            for (int i = 0; i < newLength; ++i) {
                int key;
                VALUE value;

                stream >> key;
                if (!stream) {
                    return stream;                              // RETURN
                }

                stream >> value;
                if (!stream) {
                    return stream;                              // RETURN
                }

                if (0 == add(key, value)) {
                    stream.invalidate();
                    return stream;                              // RETURN
                }
            }

          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;                                              // RETURN
}

// ACCESSORS
template <class VALUE>
int bdec2_IntMap<VALUE>::isEqual(const bdec2_IntMap<VALUE>& rhs) const
{
    if (length() != rhs.length()) {
        return 0;
    }
    const int rhsNumSlots = rhs.d_hashtable.numSlots();

    if (d_hashtable.numSlots() == rhsNumSlots) {

        // Since the underlying hashtables have the same logical size and the
        // identical hash function is used in both maps, an element of one map
        // must be in the same slot as the other map.

        for (int si = 0; si < rhsNumSlots; ++si) {
            bdeci_HashtableSlotIter<bdec2_IntMap_Node<VALUE>,
                                    bdec2_IntMap_Hash<VALUE> >
                                                       ri(rhs.d_hashtable, si);
            while (ri) {
                bdeci_HashtableSlotIter<bdec2_IntMap_Node<VALUE>,
                                        bdec2_IntMap_Hash<VALUE> >
                                                       li(d_hashtable, si);
                while (li && ri() != li()) {
                    ++li;
                }
                if (!li) {
                    return 0;
                }
                if (!(li().value() == ri().value())) {
                    return 0;
                }
                ++ri;
            }
        }
    }
    else {
        for (int si = 0; si < rhsNumSlots; ++si) {
            bdeci_HashtableSlotIter<bdec2_IntMap_Node<VALUE>,
                                    bdec2_IntMap_Hash<VALUE> >
                                                     iter(rhs.d_hashtable, si);
            while (iter) {
                const bdec2_IntMap_Node<VALUE> *n = d_hashtable.lookup(iter());
                if (!n) {
                    return 0;
                }
                if (!(iter().value() == n->value())) {
                    return 0;
                }
                ++iter;
            }
        }
    }
    return 1;
}

template <class VALUE>
bsl::ostream& bdec2_IntMap<VALUE>::
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
        bdeci_HashtableSlotIter<bdec2_IntMap_Node<VALUE>,
                              bdec2_IntMap_Hash<VALUE> > iter(d_hashtable, si);
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
