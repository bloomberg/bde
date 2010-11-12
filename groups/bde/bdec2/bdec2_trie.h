// bdec2_trie.h              -*-C++-*-
#ifndef INCLUDED_BDEC2_TRIE
#define INCLUDED_BDEC2_TRIE

//@PURPOSE: Provide a longest-prefix match container.
//
//@CLASSES:
//   bdec2_Trie_Node: local implementation class for bdec2_Trie
//   bdec2_TrieIter: iterator class for bdec2_Trie
//   bdec2_Trie: trie based implementation of the container
//
//@SEE_ALSO: 
//
//@AUTHOR: Dino Oliva (doliva1)
//
//@DESCRIPTION: This component implements a longest-prefix match container
// based on tries.
//
///USAGE
///-----
// The following snippets of code illustrate how to create and use a
// bdec2_Trie. First create an empty trie 't' and populate it with elements:
//..
//      bdec2_Trie<char, int> t;  assert(t.isEmpty());
//                                assert(0 == t.length());
//  
//      t.add("foo", 5);          assert(1 == t.length());
//      t.add("bar", 6);          assert(2 == t.length());
//..
// Now lookup elements in trie 't':
//..
//      bdec2_TrieIter<char, int> it;
//  
//      it = t.lookup("foo");     assert(t.end() != it);
//                                assert(5 == it.getValue());
//  
//      it = t.lookup("bar");     assert(t.end() != it);
//                                assert(6 == it.getValue());
//  
//      it = t.lookup("fooloo");  assert(t.end() != it);
//                                assert(5 == it.getValue());
//  
//      it = t.lookup("barla");   assert(t.end() != it);
//                                assert(6 == it.getValue());
//  
//      it = t.lookup("baz");     assert(t.end() == it);
//..
// Finally, remove elements from trie 't':
//..
//      t.remove("foo");
//      it = t.lookup("foo");     assert(t.end() == it);
//  
//      it = t.lookup("bar");     assert(t.end() != it);
//                                assert(6 == it.getValue());
//  
//      it = t.lookup("fooloo");  assert(t.end() == it);
//  
//      it = t.lookup("barla");   assert(t.end() != it);
//                                assert(6 == it.getValue());
//..
///ITERATOR USAGE
///--------------
// The following code shows the standard iterator pattern:
//..
//      for (bdec2_TrieIter<char, int> it = t.begin(); it != t.end(); ++it) {
//          const std::vector<char>& key = it.getKey();
//          int value = it.getValue();
//          printKey(std::cout, key.begin(), key.end());
//          std::cout << " = " << value << std::endl;
//      }
//..
//
// invariant -
// 1) any tree w/no value must have at least 2 subtrees
// 2) all leaves have values
// now to compact - 2 cases:
// remove(in)
//       i (no value)        it
//      / |            ==> 
//    it  in 
// but
//       i (value)           i
//      / |            ==>   |
//    it  in                 it
//
// or
//       i                     i
//    ... |            ==> ...  |
//         in                   inn
//         /
//       inn

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEMA_DEFAULT
#include <bdema_default.h>
#endif

#ifndef INCLUDED_BDEMA_POOL
#include <bdema_pool.h>
#endif

#ifndef INCLUDED_BDES_ASSERT
#include <bdes_assert.h>
#endif

#ifndef INCLUDED_BDEU_PRINT
#include <bdeu_print.h>
#endif

#ifndef INCLUDED_OSTREAM
#include <ostream>
#define INCLUDED_OSTREAM
#endif

#ifndef INCLUDED_STRINGS_H
#include <strings.h>
#define INCLUDED_STRINGS_H
#endif

#ifndef INCLUDED_STRING
#include <string>
#define INCLUDED_STRING
#endif

#ifndef INCLUDED_STACK
#include <stack>
#define INCLUDED_STACK
#endif

#ifndef INCLUDED_VECTOR
#include <vector>
#define INCLUDED_VECTOR
#endif

#ifndef INCLUDED_UTILITY
#include <utility>
#define INCLUDED_UTILITY
#endif

namespace BloombergLP {

class bdema_Allocator;

template<class KEY_ELEMENT, class VALUE> class bdec2_Trie;
template<class KEY_ELEMENT, class VALUE> class bdec2_TrieIter;

                        // =====================
                        // class bdec2_Trie_Node
                        // =====================

template<class KEY_ELEMENT, class VALUE> 
class bdec2_Trie_Node {
  private:
    std::vector<KEY_ELEMENT>       d_key;       // key of this node

    VALUE                         *d_value_p;   // pointer to the value 
                                                // associated with this key,
                                                // if there is one, and
                                                // NULL otherwise
 
    std::vector<bdec2_Trie_Node *> d_children;  // children of this node 
                                                // (held, not owned).

    bdema_Pool&                    d_valuePool; // memory pool for values

    // FRIENDS
    friend class bdec2_Trie<KEY_ELEMENT, VALUE>;
    friend class bdec2_TrieIter<KEY_ELEMENT, VALUE>;
    template<class KEY_ELEMENT1, class VALUE1>
    friend bool operator==(const bdec2_Trie_Node<KEY_ELEMENT1, VALUE1>&,
                           const bdec2_Trie_Node<KEY_ELEMENT1, VALUE1>&);

  public:
    // CREATORS
    template <class InputIterator>
    bdec2_Trie_Node(InputIterator    begin,
                    InputIterator    end,
                    const VALUE     *value,
                    bdema_Pool&      valuePool);
        // Create a new trie node where the key is specified as the
        // KEY_ELEMENTs between 'begin' and 'end', and the value associated
        // with this key is 'value'.  The memory allocated for copying value is
        // supplied by the 'valuePool'.

    bdec2_Trie_Node(const bdec2_Trie_Node& original, bdema_Pool& valuePool);
        // Create a trie node object having the value of the specified
        // 'original' trie node.  The memory allocated for copying value is 
        // supplied by the 'valuePool'.

    ~bdec2_Trie_Node();
        // Destroy this object.

    // MANIPULATORS
    bdec2_Trie_Node& operator=(const bdec2_Trie_Node& rhs);
        // Assign to this trie node the value of the specified 'rhs' trie node.

    // ACCESSORS
    std::ostream& print(std::ostream& stream,
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
};

                        // ====================
                        // class bdec2_TrieIter
                        // ====================

template<class KEY_ELEMENT, class VALUE> 
class bdec2_TrieIter {
  private:
    std::stack<bdec2_Trie_Node<KEY_ELEMENT, VALUE> *> d_stack; // stack of tries
                                                               // that need to
                                                               // be iterated
                                                               // over.

    void push(bdec2_Trie_Node<KEY_ELEMENT, VALUE> *node);
        // Push 'node' onto the stack of nodes to be iterated over if there is
        // a value in the node and recursively 'push' all of 'node's children
        // otherwise. Note that this guarantees that the iterator only ever
        // points to trie nodes that have values.

    // FRIENDS
    friend class bdec2_Trie<KEY_ELEMENT, VALUE>;
    template<class KEY_ELEMENT1, class VALUE1> 
    friend bool operator==(const bdec2_TrieIter<KEY_ELEMENT1, VALUE1>&,
                           const bdec2_TrieIter<KEY_ELEMENT1, VALUE1>&);

  public:
    // CREATORS
    bdec2_TrieIter();
        // Create an iterator for the specified trie.
    bdec2_TrieIter(const bdec2_TrieIter& original);
        // Create an iterator having the value of the specified 'original'
        // iterator. 
    ~bdec2_TrieIter();
        // Destroy this object.

    // MANIPULATORS
    bdec2_TrieIter& operator=(const bdec2_TrieIter& rhs);
        // Assign to this iterator the value of the specified 'rhs' iterator.

    bdec2_TrieIter& operator++();
        // Advance the iterator to refer to the next member of its associated
        // map.  The behavior is undefined unless the current map member is
        // valid.  Note that the order of iteration is implementation
        // dependent.

    void setValue(const VALUE& value);
        // Set the value currently pointed to by the iterator.

    // ACCESSORS
    const std::vector<KEY_ELEMENT>& getKey() const;
        // Get a reference to the key currently pointed to by the iterator.

    const VALUE& getValue() const;
        // Get a reference to the value currently pointed to by the iterator.

    typedef 
      std::pair<const std::vector<KEY_ELEMENT> *, const VALUE *> 
      Association;
        // dpo: where should this typedef go?
    Association getAssociation() const;
        // Get a pair containing 1) a pointer to the key currently pointed
        // to by the iterator and 2) a pointer to the value currently pointed
        // to by the iterator. 
};

                        // ================
                        // class bdec2_Trie
                        // ================

template<class KEY_ELEMENT, class VALUE>
class bdec2_Trie {
  private:

    bdec2_Trie_Node<KEY_ELEMENT, VALUE>   *d_root_p;      // trie node
                                                          // containing the
                                                          // key/value
                                                          // associations

    int                                   d_numElements;  // number of mappings
                                                          // in this trie

    bdema_Pool                            d_valuePool;    // pool of memory for
                                                          // allocating VALUEs

    bdema_Pool                            d_trieNodePool; // pool of memory for
                                                          // allocating node's

    static bool equivalent(const bdec2_Trie_Node<KEY_ELEMENT, VALUE>&,
                           const bdec2_Trie_Node<KEY_ELEMENT, VALUE>&);
        // Return 'true' if the specified 'lhs' and 'rhs' trie nodes are the
        // same value and their children contain pointers to 'equivalent' trie
        // nodes and 'true' otherwise.  Note that the order of the children
        // does not affect whether the trie nodes are equivalent.

    static std::ostream& deepPrint(const bdec2_Trie_Node<KEY_ELEMENT, VALUE>&,
                                   std::ostream&,
                                   int,
                                   int);
        // Format 'node' to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream' and recursively format all of the children of
        // 'node'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.

    bdec2_Trie_Node<KEY_ELEMENT, VALUE> *
    deepCopy(const bdec2_Trie_Node<KEY_ELEMENT, VALUE>& node);
        // Construct a copy of 'node' and recursively copy all of 'node's 
        // 'd_children'. Uses 'd_trieNodePool' to allocate copies of children 
        // and 'd_valuePool' to allocate copies of values.

    void deepDelete(bdec2_Trie_Node<KEY_ELEMENT, VALUE> *node);
        // Destroy 'node' and all of the nodes pointed to by 'node's
        // children. Uses 'd_trieNodePool' to deallocate child node's 
        // and 'd_valuePool' to deallocate values.

    template <class InputIterator>
    bdec2_Trie_Node<KEY_ELEMENT, VALUE> *
    add(bool                                *wasAdded,
        bdec2_Trie_Node<KEY_ELEMENT, VALUE> *node,
        InputIterator                        keyBegin,
        InputIterator                        keyEnd,
        int                                  index,
        const VALUE&                         value,
        bool                                 addOnMatch);
        // Add an association between the key specified by 'keyBegin' 
        // and 'keyEnd' and 'value' to the trie node 'node'.  The matching of
        // the keys starts at offset 'index' and it is assumed that the
        // specified key and the key in 'node' have been verified to be
        // identical up to 'index' already.  The output argument 'wasAdded' is
        // set to 'false' if there is already a value associated with the
        // specified key in the trie node, otherwise it is set to 'true'.
        // Return a pointer to a trie node modified to add the specified
        // key/value association to the trie node 'node'  if there is no value
        // associated with the specified key, and return the node unmodified
        // otherwise. Note that 'node' is assumed to be non-zero.

    template <class InputIterator>
    bdec2_Trie_Node<KEY_ELEMENT, VALUE> *
    remove(bool                                *wasRemoved,
           bdec2_Trie_Node<KEY_ELEMENT, VALUE> *node,
           InputIterator                        keyBegin,
           InputIterator                        keyEnd,
           int                                  index);
        // Remove the association between the key specified by 'keyBegin' 
        // and 'keyEnd' and a value in the trie node if such an association
        // exists.  The matching of the keys starts at offset 'index' and it
        // is assumed that the specified key and the key in 'node' have been
        // verified to be identical up to 'index' already.  The output argument
        // 'wasRemoved' is set to 'false' if there is no associated value with
        // the specified key in the trie node otherwise it is set to 'true'.
        // Return a pointer to the trie node modified to remove the specified
        // key/value association if such an association exits otherwise return
        // node unmodified. Note that node is assumed to be non-zero and that
        // 'remove' may return zero.

    template <class InputIterator>
    bdec2_Trie_Node<KEY_ELEMENT, VALUE> *lookup(
        bool                                 *allMatched,
        bdec2_Trie_Node<KEY_ELEMENT, VALUE>&  node,
        InputIterator                         keyBegin,
        InputIterator                         keyEnd,
        int                                   index) const;
        // Return the trie node containing the key/value association of
        // longest prefix match for key specified by 'keyBegin' and 'keyEnd'
        // found in 'node' and 0 if no such prefix exists.  The matching of the
        // keys starts at offset 'index' and it is assumed that the specified 
        // key and the key in 'node' have been verified to be identical up to 
        // 'index' already.

    // FRIENDS
    template<class KEY_ELEMENT1, class VALUE1>
    friend bool operator==(const bdec2_Trie<KEY_ELEMENT1, VALUE1>&,
                           const bdec2_Trie<KEY_ELEMENT1, VALUE1>&);

    template<class KEY_ELEMENT1, class VALUE1>
    friend std::ostream& operator<<(
        std::ostream& stream,
        const bdec2_Trie<KEY_ELEMENT1, VALUE1>& trie);

  public:
    typedef bdec2_TrieIter<KEY_ELEMENT, VALUE> iterator;

    // CREATORS
    bdec2_Trie(bdema_Allocator *basicAllocator = 0);
        // Create a new trie with no mappings. Optionally specify a
        // 'basicAllocator' used to supply memory via 'd_valuePool' 
        // and 'd_trieNodePool'.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    bdec2_Trie(const bdec2_Trie<KEY_ELEMENT, VALUE>& original,
               bdema_Allocator                      *basicAllocator = 0);
        // Create a trie having the value of the specified 'original'
        // trie. Optionally specify a 'basicAllocator' used to supply
        // memory via 'd_valuePool' and 'd_trieNodePool'.  If
        // 'basicAllocator' is 0, the currently installed default allocator
        // is used.

    ~bdec2_Trie();
        // Destroy this object.

    // MANIPULATORS
    bdec2_Trie& operator=(const bdec2_Trie& rhs);
        // Assign to this trie the value of the specified 'rhs' trie.

    template <class InputIterator>
    bool add(InputIterator keyBegin, InputIterator keyEnd, const VALUE& value);
        // Will add the association of the key specified by 'keyBegin' and
        // 'keyEnd'  with 'value' to 'this' trie.   This method will return
        // false if there is already an association between the specified key
        // and a value and will return 'true' otherwise.

    bool add(const char *key, const VALUE& value);
        // Will add the association of 'key' with 'value' to 'this' trie.
        // This method will return false if there is already an association
        // between 'key' and a value and will return 'true' otherwise.

    template <class InputIterator>
    bool remove(InputIterator keyBegin, InputIterator keyEnd);
        // Remove the association between the key specified by 'keyBegin' and
        // 'keyEnd' it's value' from the trie.  Return 'false' if there is no
        // such key'value association in the trie otherwise return 'true'.

    bool remove(const char *key);
        // Remove the association between 'key' and it's value from the trie.
        // Return 'false' if there is no such key/value association in the
        // trie, otherwise return 'true'.

    void removeAll();
        // Remove all associations contained within the trie.

    // ACCESSORS
    bdec2_TrieIter<KEY_ELEMENT, VALUE> begin() const;
        // Return an iterator pointing to the first key/value association 
        // of this trie.  If there are no key/value associations, return
        // the end iterator.

    bdec2_TrieIter<KEY_ELEMENT, VALUE> end() const;
        // Return an iterator positioned one element past the last key/value
        // association in the trie. 

    bool isEmpty() const;
        // Return 'true' if there are no key/value associations contained 
        // within the trie otherwise return 'false'.

    int length() const;
        // Return the number of key/value associations contained within the
        // trie.

    template <class InputIterator>
    bdec2_TrieIter<KEY_ELEMENT, VALUE> lookup(
        InputIterator keyBegin,
        InputIterator keyEnd) const;
        // Return the iterator pointing to the key/value association of
        // longest prefix match for the key specified by 'keyBegin' and
        // 'keyEnd' found in the trie and the end iterator if no such
        // prefix exists.

    bdec2_TrieIter<KEY_ELEMENT, VALUE> lookup(
        const char *key) const;
        // Return the iterator pointing to the key/value association of
        // longest prefix match for 'key' found in the trie and the end 
        // iterator if no such prefix exists.

    template <class InputIterator>
    bdec2_TrieIter<KEY_ELEMENT, VALUE> lookup(
        bool *allMatched, 
        InputIterator keyBegin,
        InputIterator keyEnd) const;
        // Return the iterator pointing to the key/value association of
        // longest prefix match for the key specified by 'keyBegin' and
        // 'keyEnd' found in the trie and the end iterator if no such
        // prefix exists.

    bdec2_TrieIter<KEY_ELEMENT, VALUE> lookup(
        bool *allMatched, 
        const char *key) const;
        // Return the iterator pointing to the key/value association of
        // longest prefix match for 'key' found in the trie and the end 
        // iterator if no such prefix exists.

    std::ostream& print(std::ostream& stream,
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
};

// FREE OPERATORS 
template <class InputIterator>
inline
std::ostream &printKey(std::ostream&, InputIterator begin, InputIterator end);
    // Format a key consisting of KEY_ELEMENTs between 'begin' and 'end' to
    // the specified output stream 'stream'.

template <class InputIterator>
inline
int checkPrefix(bool         *input1DoneFlag,
                bool         *input2DoneFlag,
                InputIterator input1Begin,
                InputIterator input1End,
                InputIterator input2Begin,
                InputIterator input2End);
    // dpo: this one needs to be commented ...

template<class KEY_ELEMENT, class VALUE>
inline
bool operator==(const bdec2_Trie_Node<KEY_ELEMENT, VALUE>& lhs,
                const bdec2_Trie_Node<KEY_ELEMENT, VALUE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' trie nodes are the same
    // value and 'false' otherwise.  Note that for trie nodes, they are the
    // same value, if their keys are the same and the associated values are
    // the same.

template<class KEY_ELEMENT, class VALUE>
inline
bool operator!=(const bdec2_Trie_Node<KEY_ELEMENT, VALUE>& lhs,
                const bdec2_Trie_Node<KEY_ELEMENT, VALUE>& rhs);
    // Return 'false' if the specified 'lhs' and 'rhs' trie nodes are the same
    // value and 'true' otherwise.

template<class KEY_ELEMENT, class VALUE>
inline
bool operator==(const bdec2_TrieIter<KEY_ELEMENT, VALUE>& lhs,
                const bdec2_TrieIter<KEY_ELEMENT, VALUE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' trie iterators are the
    // same value and 'false' otherwise.

template<class KEY_ELEMENT, class VALUE>
inline
bool operator!=(const bdec2_TrieIter<KEY_ELEMENT, VALUE>& lhs,
                const bdec2_TrieIter<KEY_ELEMENT, VALUE>& rhs);
    // Return 'false' if the specified 'lhs' and 'rhs' trie iterators are the
    // same value and 'true' otherwise.

template<class KEY_ELEMENT, class VALUE>
inline
bdec2_TrieIter<KEY_ELEMENT, VALUE> operator++(
    bdec2_TrieIter<KEY_ELEMENT, VALUE>& it,
    int);
    // Advance the iterator to refer to the next member of its associated
    // map.  The behavior is undefined unless the current map member is
    // valid.  Note that the order of iteration is implementation
    // dependent.

template<class KEY_ELEMENT, class VALUE>
inline
bool operator==(const bdec2_Trie<KEY_ELEMENT, VALUE>& lhs,
                const bdec2_Trie<KEY_ELEMENT, VALUE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' tries are the same value
    // and 'false' otherwise.  Note that for tries are the same value, if
    // they have the same number of elements and their roots contain the same
    // key/value associations.

template<class KEY_ELEMENT, class VALUE>
inline
bool operator!=(const bdec2_Trie<KEY_ELEMENT, VALUE>& lhs,
                const bdec2_Trie<KEY_ELEMENT, VALUE>& rhs);
    // Return 'false' if the specified 'lhs' and 'rhs' tries are the same
    // value and 'true' otherwise.

template<class KEY_ELEMENT, class VALUE>
inline
std::ostream& operator<<(std::ostream&                         stream,
                         const bdec2_Trie<KEY_ELEMENT, VALUE>& trie);
    // Write the specified 'trie' to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                          // ---------------------
                          // class bdec2_Trie_Node
                          // ---------------------

// CREATORS

template<class KEY_ELEMENT, class VALUE>
template <class InputIterator>
inline
bdec2_Trie_Node<KEY_ELEMENT, VALUE>::bdec2_Trie_Node(InputIterator begin,
                                                     InputIterator end,
                                                     const VALUE  *value,
                                                     bdema_Pool&   valuePool)
: d_key(begin, end)
, d_children()
, d_valuePool(valuePool)
{
    if (value) {
        d_value_p = new (d_valuePool) VALUE(*value);
    }
    else {
        d_value_p = 0;
    }
}

template<class KEY_ELEMENT, class VALUE>
inline
bdec2_Trie_Node<KEY_ELEMENT, VALUE>::bdec2_Trie_Node(
    const bdec2_Trie_Node& original,
    bdema_Pool&            valuePool)
: d_key(original.d_key)
, d_valuePool(valuePool)
{
    if (original.d_value_p) {
        d_value_p = new (d_valuePool) VALUE(*original.d_value_p);
    } else {
        d_value_p = 0;
    }
}

template<class KEY_ELEMENT, class VALUE>
inline
bdec2_Trie_Node<KEY_ELEMENT, VALUE>::~bdec2_Trie_Node()
{
    if (d_value_p) {
        d_valuePool.deleteObject(d_value_p);
    }
}

// MANIPULATORS

template<class KEY_ELEMENT, class VALUE>
inline
bdec2_Trie_Node<KEY_ELEMENT, VALUE>&
bdec2_Trie_Node<KEY_ELEMENT, VALUE>::operator=(
    const bdec2_Trie_Node<KEY_ELEMENT, VALUE>& rhs)
{
    if (this != &rhs) {
        d_children.erase(d_children.begin(), d_children.end());
        d_key = rhs.d_key;
        if (rhs.d_value_p) {
            d_value_p = new (d_valuePool) VALUE(*rhs.d_value_p);
        }
        else {
            d_value_p = 0;
        }
    }
    return *this;
}

// ACCESSORS
template<class KEY_ELEMENT, class VALUE>
inline
std::ostream& bdec2_Trie_Node<KEY_ELEMENT, VALUE>::print(
    std::ostream& stream,
    int           level,
    int           spacesPerLevel)
    const
{
    if (stream.bad()) {
        return stream;
    }

    if (level > 0) {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    stream << "(";

    printKey(stream, d_key.begin(), d_key.end());
   
    if (d_value_p) {                              // print value, if
        stream << ", " << *d_value_p << ")";      // there is one
    }
    else {
        stream << ", <<novalue>>)";
    }

    stream << ((spacesPerLevel >= 0) ? '\n' : ' ');

    return stream;
}


                          // --------------------
                          // class bdec2_TrieIter
                          // --------------------

// CREATORS

template<class KEY_ELEMENT, class VALUE> 
inline
bdec2_TrieIter<KEY_ELEMENT, VALUE>::bdec2_TrieIter()
{
}

template<class KEY_ELEMENT, class VALUE> 
inline
bdec2_TrieIter<KEY_ELEMENT, VALUE>::bdec2_TrieIter(
    const bdec2_TrieIter<KEY_ELEMENT, VALUE>& trieIter)
: d_stack(trieIter.d_stack)
{
}

template<class KEY_ELEMENT, class VALUE> 
inline
bdec2_TrieIter<KEY_ELEMENT, VALUE>::~bdec2_TrieIter()
{
}

// MANIPULATORS

template<class KEY_ELEMENT, class VALUE> 
inline
void bdec2_TrieIter<KEY_ELEMENT, VALUE>::push(
    bdec2_Trie_Node<KEY_ELEMENT, VALUE> *node)
{
    if (!node) return;
    d_stack.push(node);
    while (!d_stack.empty() && (d_stack.top()->d_value_p == 0)) {
        bdec2_Trie_Node<KEY_ELEMENT, VALUE> *temp = d_stack.top();
        d_stack.pop();
        typename std::vector<bdec2_Trie_Node<KEY_ELEMENT, VALUE> *>
                    ::iterator it = temp->d_children.begin();
        while (it != temp->d_children.end()) {
            push(*it);
            ++it;
        }
    }
}

template<class KEY_ELEMENT, class VALUE> 
inline
bdec2_TrieIter<KEY_ELEMENT, VALUE>&
bdec2_TrieIter<KEY_ELEMENT, VALUE>::operator=(
    const bdec2_TrieIter<KEY_ELEMENT, VALUE>& rhs)
{
    if (this != &rhs) {
        d_stack = rhs.d_stack;
    }
    return *this;
}

template<class KEY_ELEMENT, class VALUE> 
inline
bdec2_TrieIter<KEY_ELEMENT, VALUE>&
bdec2_TrieIter<KEY_ELEMENT, VALUE>::operator++()
{
    if (!d_stack.empty()) {
        bdec2_Trie_Node<KEY_ELEMENT, VALUE> *temp = d_stack.top();
        d_stack.pop();
        typename std::vector<bdec2_Trie_Node<KEY_ELEMENT, VALUE> *>
                    ::iterator it = temp->d_children.begin();
        for (; it != temp->d_children.end(); ++it) {
            push(*it);
        }
    }
    return *this;
}

template<class KEY_ELEMENT, class VALUE> 
inline
void bdec2_TrieIter<KEY_ELEMENT, VALUE>::setValue(const VALUE& value)
{
    BDE_ASSERT_H(d_stack.top() != 0);
    BDE_ASSERT_H(d_stack.top()->d_value_p != 0);
    d_stack.top()->d_valuePool.deleteObject(d_stack.top()->d_value_p);
    d_stack.top()->d_value_p = new (d_stack.top()->d_valuePool) VALUE(value);
}

// ACCESSORS

template<class KEY_ELEMENT, class VALUE> 
inline
const std::vector<KEY_ELEMENT>& bdec2_TrieIter<KEY_ELEMENT, VALUE>::getKey()
    const
{
    BDE_ASSERT_H(d_stack.top() != 0);
    return d_stack.top()->d_key;
}

template<class KEY_ELEMENT, class VALUE> 
inline
const VALUE& bdec2_TrieIter<KEY_ELEMENT, VALUE>::getValue() const
{
    BDE_ASSERT_H(d_stack.top() != 0);
    BDE_ASSERT_H(d_stack.top()->d_value_p != 0);
    return *d_stack.top()->d_value_p;
}

template<class KEY_ELEMENT, class VALUE> 
inline
typename bdec2_TrieIter<KEY_ELEMENT, VALUE>::Association
bdec2_TrieIter<KEY_ELEMENT, VALUE>::getAssociation() const
{
    BDE_ASSERT_H(d_stack.top() != 0);
    BDE_ASSERT_H(d_stack.top()->d_value_p != 0);

    const std::vector<KEY_ELEMENT> *key = &(d_stack.top()->d_key);
    const VALUE *value = d_stack.top()->d_value_p;
    typename bdec2_TrieIter<KEY_ELEMENT, VALUE>::Association p(key, value);
    return p;
}

                          // ----------------
                          // class bdec2_Trie
                          // ----------------

// PRIVATE METHODS

template<class KEY_ELEMENT, class VALUE>
inline
bool bdec2_Trie<KEY_ELEMENT, VALUE>::equivalent(
    const bdec2_Trie_Node<KEY_ELEMENT, VALUE>& lhs,
    const bdec2_Trie_Node<KEY_ELEMENT, VALUE>& rhs)
{

    if (lhs.d_key != rhs.d_key) {
        return false;
    }
    typename std::vector<bdec2_Trie_Node<KEY_ELEMENT, VALUE> *>
                ::const_iterator lhsIt = lhs.d_children.begin();
    for (; lhsIt != lhs.d_children.end(); ++lhsIt) {
        bool wasFound = false;
        typename std::vector<bdec2_Trie_Node<KEY_ELEMENT, VALUE> *>
                    ::const_iterator rhsIt = rhs.d_children.begin();
        for (; rhsIt != rhs.d_children.end(); ++rhsIt) {
            if (equivalent(**lhsIt, **rhsIt)) {
                wasFound = true;
                break;
            }
        }
        if (!wasFound) {
            return false;
        }
    }
    return true;
}

template<class KEY_ELEMENT, class VALUE>
inline std::ostream& bdec2_Trie<KEY_ELEMENT, VALUE>::deepPrint(
    const bdec2_Trie_Node<KEY_ELEMENT, VALUE>& node,
    std::ostream&                              stream,
    int                                        level,
    int                                        spacesPerLevel)
{
    if (stream.bad()) {
        return stream;
    }

    char NL;

    if (spacesPerLevel >= 0) {
        node.print(stream, level, -spacesPerLevel);
        NL = '\n';
    } 
    else {
        node.print(stream, level, spacesPerLevel);
        NL = ' ';
    }

    if (level < 0) {
        level = -level;
    }

    stream << NL ;

    if (node.d_children.begin() != node.d_children.end()) {
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "[" << NL;
        typename std::vector<bdec2_Trie_Node<KEY_ELEMENT, VALUE> *>
                    ::const_iterator it = node.d_children.begin();
        for (; it !=node. d_children.end(); ++it) {
            deepPrint(**it, stream, level+1, spacesPerLevel);
        }
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]" << NL;
    }
    return stream;
}


template<class KEY_ELEMENT, class VALUE>
inline
bdec2_Trie_Node<KEY_ELEMENT, VALUE> *
bdec2_Trie<KEY_ELEMENT, VALUE>::deepCopy(
    const bdec2_Trie_Node<KEY_ELEMENT, VALUE>& node)
{
    bdec2_Trie_Node<KEY_ELEMENT, VALUE> *
        newNode = new (d_trieNodePool)
                  bdec2_Trie_Node<KEY_ELEMENT, VALUE>(node, d_valuePool);
    typename std::vector<bdec2_Trie_Node<KEY_ELEMENT, VALUE> *>
                ::const_iterator it = node.d_children.begin();
    while (it != node.d_children.end()) {
        if (*it) {
            newNode->d_children.push_back(deepCopy(**it));
        }
        ++it;
    }
    return newNode;
}

template<class KEY_ELEMENT, class VALUE>
inline
void bdec2_Trie<KEY_ELEMENT, VALUE>::deepDelete(
    bdec2_Trie_Node<KEY_ELEMENT, VALUE> *node)
{
    if (!node) {
        return;
    }
    typename std::vector<bdec2_Trie_Node<KEY_ELEMENT, VALUE> *>
                ::const_iterator it = node->d_children.begin();
    while (it != node->d_children.end()) {
        deepDelete(*it);
        ++it;
    }
    d_trieNodePool.deleteObject(node);
}

template<class KEY_ELEMENT, class VALUE>
template <class InputIterator>
inline
bdec2_Trie_Node<KEY_ELEMENT, VALUE> *bdec2_Trie<KEY_ELEMENT, VALUE>::add(
    bool                                *wasAdded,
    bdec2_Trie_Node<KEY_ELEMENT, VALUE> *node, 
    InputIterator                        keyBegin,
    InputIterator                        keyEnd,
    int                                  index, 
    const VALUE&                         value,
    bool                                 addOnNoMatch)
{
    BDE_ASSERT_H(wasAdded);
    BDE_ASSERT_H(node);
    *wasAdded = false;
    bool keyDoneFlag;
    bool nodeKeyDoneFlag;
    int  numMatched =
        checkPrefix(&keyDoneFlag, &nodeKeyDoneFlag,
                    keyBegin + index, keyEnd,
                    node->d_key.begin() + index, node->d_key.end());
    // key is equal to d_key
    if (keyDoneFlag && nodeKeyDoneFlag) {
        if (!node->d_value_p) {
            node->d_value_p = new (node->d_valuePool) VALUE(value);
            *wasAdded = true;
        }
        return node;
    }
    // key is a prefix of d_key
    if (keyDoneFlag) { 
        bdec2_Trie_Node<KEY_ELEMENT, VALUE> *
            newNode = new (d_trieNodePool)
                      bdec2_Trie_Node<KEY_ELEMENT, VALUE>(keyBegin,
                                                          keyEnd,
                                                          &value,
                                                          d_valuePool);
        newNode->d_children.push_back(node);
        *wasAdded = true;
        return newNode;
    }
    // d_key is a prefix of key - add key into the children
    if (nodeKeyDoneFlag) {
        typename std::vector<bdec2_Trie_Node<KEY_ELEMENT, VALUE> *>
                    ::iterator it = node->d_children.begin();
        for (; it != node->d_children.end(); ++it) {
            bdec2_Trie_Node<KEY_ELEMENT, VALUE> *
                result = add(wasAdded, *it, keyBegin, keyEnd,
                             index+numMatched, value, false);
            if (*wasAdded) {
                *it = result;
                return node;
            }
        }
        // didn't find a place to add the node, add new node to the back
        bdec2_Trie_Node<KEY_ELEMENT, VALUE> *
            newNode = new (d_trieNodePool)
                      bdec2_Trie_Node<KEY_ELEMENT, VALUE>(keyBegin,
                                                          keyEnd,
                                                          &value,
                                                          d_valuePool);
        node->d_children.push_back(newNode);
        *wasAdded = true;
        return node;
    }
    // key and d_key share a common prefix
    if (numMatched || addOnNoMatch) {
        bdec2_Trie_Node<KEY_ELEMENT, VALUE> *
            commonNode = new (d_trieNodePool)
                         bdec2_Trie_Node<KEY_ELEMENT, VALUE>(
                             keyBegin,
                             keyBegin+index+numMatched,
                             0,
                             d_valuePool);
        bdec2_Trie_Node<KEY_ELEMENT, VALUE> *
            newNode = new (d_trieNodePool)
                      bdec2_Trie_Node<KEY_ELEMENT, VALUE>(keyBegin,
                                                          keyEnd,
                                                          &value,
                                                          d_valuePool);
        commonNode->d_children.push_back(node);
        commonNode->d_children.push_back(newNode);
        *wasAdded = true;
        return commonNode;
    }
    // key and d_key are unrelated
    return node;
}

template<class KEY_ELEMENT, class VALUE>
template <class InputIterator>
inline
bdec2_Trie_Node<KEY_ELEMENT, VALUE> *bdec2_Trie<KEY_ELEMENT, VALUE>::remove(
    bool                                *wasRemoved,
    bdec2_Trie_Node<KEY_ELEMENT, VALUE> *node,
    InputIterator                        keyBegin,
    InputIterator                        keyEnd,
    int                                  index)
{
    BDE_ASSERT_H(wasRemoved);
    BDE_ASSERT_H(node);
    *wasRemoved = false;

    bool nodeKeyDoneFlag;
    bool keyDoneFlag;
    int  numMatched = 
         checkPrefix(&nodeKeyDoneFlag, &keyDoneFlag,
                     node->d_key.begin() + index, node->d_key.end(),
                     keyBegin + index, keyEnd);

    // d_key is the same as key
    if (nodeKeyDoneFlag && keyDoneFlag) {
        if (node->d_value_p) {
            *wasRemoved = true;
            node->d_valuePool.deleteObject(node->d_value_p);
            node->d_value_p = 0;
            if (node->d_children.size() == 1) {
                bdec2_Trie_Node<KEY_ELEMENT, VALUE> *
                    temp = *node->d_children.begin();
                d_trieNodePool.deleteObject(node);
                return temp;
            }
            if (node->d_children.empty()) {
                d_trieNodePool.deleteObject(node);
                return 0;
            }
        }
    }

    // d_key is a prefix of key
    else
    if (nodeKeyDoneFlag) {
        typename std::vector<bdec2_Trie_Node<KEY_ELEMENT, VALUE> *>
                    ::iterator it = node->d_children.begin();
        for (; it != node->d_children.end(); ++it) {
            bdec2_Trie_Node<KEY_ELEMENT, VALUE> *
                temp = remove(wasRemoved, *it, keyBegin, keyEnd,
                              node->d_key.size());
            if (!temp) {
                node->d_children.erase(it);
            } 
            else { 
                *it = temp;
            }
            if (*wasRemoved) {
                break;
            }
        }
        if (!node->d_value_p && node->d_children.size() == 1) {
            bdec2_Trie_Node<KEY_ELEMENT, VALUE> *
                temp = *node->d_children.begin();
            node->d_children.erase(node->d_children.begin());
            d_trieNodePool.deleteObject(node);
            return temp;
        }
        if (!node->d_value_p && node->d_children.empty()) {
            d_trieNodePool.deleteObject(node);
            return 0;
        } 

    }
    return node;
}

template<class KEY_ELEMENT, class VALUE>
template <class InputIterator>
inline
bdec2_Trie_Node<KEY_ELEMENT, VALUE> *bdec2_Trie<KEY_ELEMENT, VALUE>::lookup(
    bool                                 *allMatched,
    bdec2_Trie_Node<KEY_ELEMENT, VALUE>&  node,
    InputIterator                         keyBegin,
    InputIterator                         keyEnd,
    int                                   index) const
{
    BDE_ASSERT_H(*allMatched);
    bool nodeKeyDoneFlag;
    bool keyDoneFlag;
    int  numMatched = 
        checkPrefix(&nodeKeyDoneFlag, &keyDoneFlag,
                    node.d_key.begin() + index, node.d_key.end(),
                    keyBegin + index, keyEnd);
    if (nodeKeyDoneFlag) {
        if (keyDoneFlag && node.d_value_p) {
            *allMatched = true;
            return &node;
        }
        typename std::vector<bdec2_Trie_Node<KEY_ELEMENT, VALUE> *>
                    ::iterator it = node.d_children.begin();
        for (; it != node.d_children.end(); ++it) {
            BDE_ASSERT_H(*it); //dpo: should we have this here?
            bdec2_Trie_Node<KEY_ELEMENT, VALUE> *result =
                lookup(allMatched, **it, keyBegin, keyEnd, node.d_key.size());
            if (result) {
                return result;
            }
        }
        // didn't find a better match
        if (node.d_value_p) {
            *allMatched = false;
            return &node;
        }
    }
    *allMatched = false;
    return 0;
}

// CREATORS

template<class KEY_ELEMENT, class VALUE>
inline
bdec2_Trie<KEY_ELEMENT, VALUE>::bdec2_Trie(bdema_Allocator *basicAllocator)
: d_root_p(0)
, d_numElements(0)
, d_valuePool(sizeof(VALUE), basicAllocator)
, d_trieNodePool(sizeof(bdec2_Trie_Node<KEY_ELEMENT, VALUE>), basicAllocator)
{
}

template<class KEY_ELEMENT, class VALUE>
inline
bdec2_Trie<KEY_ELEMENT, VALUE>::bdec2_Trie(
    const bdec2_Trie<KEY_ELEMENT, VALUE>& original,
    bdema_Allocator                      *basicAllocator)
: d_root_p(0)
, d_numElements(original.d_numElements)
, d_valuePool(sizeof(VALUE), basicAllocator)
, d_trieNodePool(sizeof(bdec2_Trie_Node<KEY_ELEMENT, VALUE>), basicAllocator)
{
    if (original.d_root_p) {
        d_root_p = deepCopy(*original.d_root_p);
    }
}

template<class KEY_ELEMENT, class VALUE>
inline
bdec2_Trie<KEY_ELEMENT, VALUE>::~bdec2_Trie()
{
    if (d_root_p) {
        deepDelete(d_root_p);
    }
}

// MANIPULATORS

template<class KEY_ELEMENT, class VALUE>
inline
bdec2_Trie<KEY_ELEMENT, VALUE>& bdec2_Trie<KEY_ELEMENT, VALUE>::operator=(
    const bdec2_Trie<KEY_ELEMENT, VALUE>& rhs)
{
    if (this != &rhs) {
        d_numElements = rhs.d_numElements;
        if (d_root_p) {
            deepDelete(d_root_p);
        }
        if (rhs.d_root_p) {
            d_root_p = deepCopy(*rhs.d_root_p);
        }
        else {
            d_root_p = 0;
        }
    }
    return *this;
}

template<class KEY_ELEMENT, class VALUE>
template <class InputIterator>
inline
bool bdec2_Trie<KEY_ELEMENT, VALUE>::add(InputIterator keyBegin,
                                         InputIterator keyEnd,
                                         const VALUE&  value)
{
    bool wasAdded;
    if (!d_root_p) {
        d_root_p =
            new (d_trieNodePool)
            bdec2_Trie_Node<KEY_ELEMENT, VALUE>(keyBegin,
                                                keyEnd,
                                                &value,
                                                d_valuePool);
        wasAdded = true;
    }
    else {
        d_root_p = add(&wasAdded, d_root_p,
                       keyBegin, keyEnd,
                       0, value, true);
    }
    if (wasAdded) {
        ++d_numElements;
        return true;
    } 
    else {
        return false;
    }
}

template<class KEY_ELEMENT, class VALUE>
inline
bool bdec2_Trie<KEY_ELEMENT, VALUE>::add(const char *key, const VALUE& value)
{
    BDE_ASSERT_H(key);
    std::vector<KEY_ELEMENT> vkey(key, key+strlen(key));
    return add(vkey.begin(), vkey.end(), value);
}

template<class KEY_ELEMENT, class VALUE>
template <class InputIterator>
inline
bool bdec2_Trie<KEY_ELEMENT, VALUE>::remove(InputIterator keyBegin,
                                            InputIterator keyEnd)
{
    if (!d_root_p) {
        return false;
    }
    else {
        bool wasRemoved = false;
        d_root_p = remove(&wasRemoved, d_root_p, keyBegin, keyEnd, 0);
        if (wasRemoved) {
            --d_numElements;
            return true;
        }
        else {
            return false;
        }
    }
}

template<class KEY_ELEMENT, class VALUE>
inline
bool bdec2_Trie<KEY_ELEMENT, VALUE>::remove(const char *key)
{
    BDE_ASSERT_H(key);
    std::vector<KEY_ELEMENT> vkey(key, key+strlen(key));
    return remove(vkey.begin(), vkey.end());
}

template<class KEY_ELEMENT, class VALUE>
inline
void bdec2_Trie<KEY_ELEMENT, VALUE>::removeAll()
{
    if (d_root_p) {
        d_numElements = 0;
        deepDelete(d_root_p);
        d_root_p = 0;
    }
}


// ACCESSORS

template<class KEY_ELEMENT, class VALUE>
inline
bdec2_TrieIter<KEY_ELEMENT, VALUE> bdec2_Trie<KEY_ELEMENT, VALUE>::begin() const
{
    bdec2_TrieIter<KEY_ELEMENT, VALUE> it;
    if (d_root_p) {
        it.push(d_root_p);
    }
    return it;
}

template<class KEY_ELEMENT, class VALUE>
inline
bdec2_TrieIter<KEY_ELEMENT, VALUE> bdec2_Trie<KEY_ELEMENT, VALUE>::end() const
{
    bdec2_TrieIter<KEY_ELEMENT, VALUE> temp;
    return temp;
}

template<class KEY_ELEMENT, class VALUE>
inline
bool bdec2_Trie<KEY_ELEMENT, VALUE>::isEmpty() const
{
    return 0 == d_numElements;
}

template<class KEY_ELEMENT, class VALUE>
inline
int bdec2_Trie<KEY_ELEMENT, VALUE>::length() const
{
    return d_numElements;
}

template<class KEY_ELEMENT, class VALUE>
template <class InputIterator>
inline
bdec2_TrieIter<KEY_ELEMENT, VALUE>
bdec2_Trie<KEY_ELEMENT, VALUE>::lookup(
    InputIterator keyBegin,
    InputIterator keyEnd) const
{
    bdec2_TrieIter<KEY_ELEMENT, VALUE> it;
    bool allMatched = false;
    if (d_root_p) {
        bdec2_Trie_Node<KEY_ELEMENT, VALUE> *
            result = lookup(&allMatched, *d_root_p, keyBegin, keyEnd, 0);
        if (result) it.push(result);
    }
    return it;
}

template<class KEY_ELEMENT, class VALUE>
inline
bdec2_TrieIter<KEY_ELEMENT, VALUE> bdec2_Trie<KEY_ELEMENT, VALUE>::lookup(
    const char *key) const
{
    BDE_ASSERT_H(key);
    std::vector<KEY_ELEMENT> vkey(key, key+strlen(key));
    return lookup(vkey.begin(), vkey.end());
}

template<class KEY_ELEMENT, class VALUE>
template <class InputIterator>
inline
bdec2_TrieIter<KEY_ELEMENT, VALUE>
bdec2_Trie<KEY_ELEMENT, VALUE>::lookup(
    bool *allMatched,
    InputIterator keyBegin,
    InputIterator keyEnd) const
{
    BDE_ASSERT_H(allMatched);
    bdec2_TrieIter<KEY_ELEMENT, VALUE> it;
    if (d_root_p) {
        bdec2_Trie_Node<KEY_ELEMENT, VALUE> *
            result = lookup(allMatched, *d_root_p, keyBegin, keyEnd, 0);
        if (result) it.push(result);
    }
    return it;
}

template<class KEY_ELEMENT, class VALUE>
inline
bdec2_TrieIter<KEY_ELEMENT, VALUE> bdec2_Trie<KEY_ELEMENT, VALUE>::lookup(
    bool *allMatched,
    const char *key) const
{
    BDE_ASSERT_H(key);
    BDE_ASSERT_H(allMatched);
    std::vector<KEY_ELEMENT> vkey(key, key+strlen(key));
    return lookup(allMatched, vkey.begin(), vkey.end());
}

template<class KEY_ELEMENT, class VALUE>
inline
std::ostream& bdec2_Trie<KEY_ELEMENT, VALUE>::print(
    std::ostream& stream,
    int           level,
    int           spacesPerLevel)
    const
{
    if (d_root_p) {
        deepPrint(*d_root_p, stream, level, spacesPerLevel);
    }
    return stream;
}

// FREE OPERATORS DEFINITIONS

template <class InputIterator>
inline
std::ostream &printKey(std::ostream& stream,
                       InputIterator keyBegin,
                       InputIterator keyEnd)
{
    while (keyBegin != keyEnd) {
        stream << *keyBegin;
        ++ keyBegin;
    }
    return stream;
}

template <class InputIterator>
inline
int checkPrefix(bool         *input1DoneFlag,
                bool         *input2DoneFlag,
                InputIterator input1Begin,
                InputIterator input1End,
                InputIterator input2Begin,
                InputIterator input2End)
{
    BDE_ASSERT_H(input1DoneFlag != 0);
    BDE_ASSERT_H(input2DoneFlag != 0);
    *input1DoneFlag = false;
    *input2DoneFlag = false;
    int numMatched = 0;
    int i = 0;
    while ((input1Begin != input1End) && (input2Begin != input2End)) {
        if (*input1Begin != *input2Begin) {
            return numMatched;
        }
        ++numMatched;
        ++input1Begin;
        ++input2Begin;
    }
    *input1DoneFlag = (input1Begin == input1End);
    *input2DoneFlag = (input2Begin == input2End);
    return numMatched;
}

template<class KEY_ELEMENT, class VALUE>
inline
bool operator==(const bdec2_Trie_Node<KEY_ELEMENT, VALUE>& lhs,
                const bdec2_Trie_Node<KEY_ELEMENT, VALUE>& rhs)
{
    typename std::vector<KEY_ELEMENT>
                ::const_iterator lhsKeyIt = lhs.d_key.begin();
    typename std::vector<KEY_ELEMENT>
                ::const_iterator rhsKeyIt = rhs.d_key.begin(); 
    while ((lhsKeyIt != lhs.d_key.end()) && (rhsKeyIt != rhs.d_key.end())) {
        if (*lhsKeyIt != *rhsKeyIt) {
            break;
        }
         ++lhsKeyIt;
         ++rhsKeyIt;
    }
    if ((lhsKeyIt != lhs.d_key.end()) || (rhsKeyIt != rhs.d_key.end())) {
        return false;
    }
    if (lhs.d_value_p != rhs.d_value_p) {
        return false;
    }
    if (lhs.d_value_p && (*lhs.d_value_p != *rhs.d_value_p)) {
        return false;
    }
    return true;
}

template<class KEY_ELEMENT, class VALUE> 
inline
bool operator!=(const bdec2_Trie_Node<KEY_ELEMENT, VALUE>& lhs,
                const bdec2_Trie_Node<KEY_ELEMENT, VALUE>& rhs)
{
    return !(lhs == rhs);
}

template<class KEY_ELEMENT, class VALUE> 
inline
bool operator==(const bdec2_TrieIter<KEY_ELEMENT, VALUE>& lhs,
                const bdec2_TrieIter<KEY_ELEMENT, VALUE>& rhs)
{
    return lhs.d_stack == rhs.d_stack;
}

template<class KEY_ELEMENT, class VALUE> 
inline
bool operator!=(const bdec2_TrieIter<KEY_ELEMENT, VALUE>& lhs,
                const bdec2_TrieIter<KEY_ELEMENT, VALUE>& rhs)
{
    return !(lhs == rhs);
}

template<class KEY_ELEMENT, class VALUE> 
inline
bdec2_TrieIter<KEY_ELEMENT, VALUE> operator++(
    bdec2_TrieIter<KEY_ELEMENT, VALUE>& it,
    int)
{
    bdec2_TrieIter<KEY_ELEMENT, VALUE> temp(it);
    ++it;
    return temp;
}

template<class KEY_ELEMENT, class VALUE>
inline
bool operator==(const bdec2_Trie<KEY_ELEMENT, VALUE>& lhs,
                const bdec2_Trie<KEY_ELEMENT, VALUE>& rhs)
{
    if (lhs.d_numElements != rhs.d_numElements) {
        return false;
    }
    if (!lhs.d_root_p && !lhs.d_root_p) {
        return true;
    }
    if (lhs.d_root_p && lhs.d_root_p) {
        return bdec2_Trie<KEY_ELEMENT, VALUE>::equivalent(*(lhs.d_root_p),
                                                          *(rhs.d_root_p));
    }
    return false;
}

template<class KEY_ELEMENT, class VALUE>
inline
bool operator!=(const bdec2_Trie<KEY_ELEMENT, VALUE>& lhs,
                const bdec2_Trie<KEY_ELEMENT, VALUE>& rhs)
{
    return !(lhs == rhs);
}

template<class KEY_ELEMENT, class VALUE>
inline
std::ostream& operator<<(std::ostream& stream,
                         const bdec2_Trie<KEY_ELEMENT, VALUE>& trie)
{
    trie.print(stream);
    return stream;
}

}  // close namespace BloombergLP

#endif // INCLUDED_TRIE

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------

