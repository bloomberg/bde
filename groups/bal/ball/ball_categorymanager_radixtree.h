// ball_categorymanager_radixtree.h                                   -*-C++-*-
#ifndef INCLUDED_BALL_CATEGORYMANAGER_RADIXTREE
#define INCLUDED_BALL_CATEGORYMANAGER_RADIXTREE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a space-efficient associative container for string keys.
//
//@CLASSES:
//  ball::CategoryManager_RadixTree: space-efficient string-key assoc container
//
//@DESCRIPTION: This component implements `ball::CategoryManager_RadixTree`, a
// space-efficient associative container that stores key-value pairs where keys
// are strings (or string-like types).  A radix tree (also known as a
// compressed trie or prefix tree) achieves space efficiency by sharing common
// prefixes among keys, making it particularly suitable for storing large sets
// of strings with common prefixes.
//
///Features
///--------
// The `ball::CategoryManager_RadixTree` provides the following features:
//
// * Space-efficient storage of string keys with common prefixes
// * O(k) insertion, lookup, and removal where k is the key length
// * Support for custom value types
// * Visitor pattern for traversing all key-value pairs
// * Full allocator support for value-semantic behavior
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// Suppose we want to store a mapping of words to their definitions.  Using
// `ball::CategoryManager_RadixTree` allows us to efficiently store many words
// that share common prefixes.
//
// First, we create a radix tree:
// ```
// ball::CategoryManager_RadixTree<bsl::string> dictionary;
// ```
// Then, we insert some words and their definitions:
// ```
// dictionary.emplace("car",  "a road vehicle with four wheels");
// dictionary.emplace("card", "a piece of stiff paper");
// dictionary.emplace("care", "the provision of what is needed");
// dictionary.emplace("cat",  "a small domesticated carnivorous mammal");
// ```
// Next, we can look up definitions:
// ```
// bsl::optional<bsl::reference_wrapper<bsl::string> > definition =
//                                                 dictionary.find("car");
// assert(definition.has_value());
// assert(definition->get() == "a road vehicle with four wheels");
// ```
// We can also check if a key exists:
// ```
// assert(dictionary.contains("card"));
// assert(!dictionary.contains("dog"));
// ```
// We can find the longest prefix of a key that exists in the tree:
// ```
// ball::CategoryManager_RadixTree<bsl::string>::OptValueRef optValue;
// bsl::string_view prefix = dictionary.findLongestCommonPrefix(&optValue,
//                                                              "cards");
// assert(prefix == "card");
// assert(optValue.has_value() && optValue->get() == "a piece of stiff paper");
// prefix = dictionary.findLongestCommonPrefix(&optValue, "carpet");
// assert(prefix == "car");
// assert(optValue.has_value()
//     && optValue->get() == "a road vehicle with four wheels");
// prefix = dictionary.findLongestCommonPrefix(&optValue, "dog");
// assert(prefix == "");
// assert(!optValue.has_value());
// ```
// We can run a functor for all entries:
// ```
// struct Printer {
//     void operator()(const bsl::string_view& key,
//                     const bsl::string&      value) const {
//         bsl::cout << key << ": " << value << bsl::endl;
//     }
// };
// dictionary.forEach(Printer());
// ```
// We can run a functor for all entries with a given prefix:
// ```
// struct PrefixCollector {
//     bsl::vector<bsl::string> *d_vec_p;
//     PrefixCollector(bsl::vector<bsl::string> *vec) : d_vec_p(vec) {}
//     void operator()(const bsl::string_view& key,
//                     const bsl::string&      value) const {
//         d_vec_p->push_back(bsl::string(key));
//     }
// };
// bsl::vector<bsl::string> found;
// PrefixCollector          collector(&found);
// dictionary.forEachPrefix("car", collector);
// assert(found.size() == 3);
// bool foundCar  = false;
// bool foundCard = false;
// bool foundCare = false;
// for (bsl::size_t i = 0; i < found.size(); ++i) {
//     if (found[i] == "car") {
//         foundCar = true;
//     } else if (found[i] == "card") {
//         foundCard = true;
//     } else if (found[i] == "care") {
//         foundCare = true;
//     }
// }
// assert(foundCar);
// assert(foundCard);
// assert(foundCare);
// ```
// We can also mutate values for all entries with a given prefix:
// ```
// struct SuffixAppender {
//     void operator()(const bsl::string_view&, bsl::string& value) const {
//         value += "!";
//     }
// };
// dictionary.forEachPrefix("car", SuffixAppender());
// assert(dictionary.find("car")->get() ==
//                                         "a road vehicle with four wheels!");
// assert(dictionary.find("card")->get() ==         "a piece of stiff paper!");
// assert(dictionary.find("care")->get() ==
//                                         "the provision of what is needed!");
// ```
// Finally, we can remove entries:
// ```
// dictionary.erase("card"); assert(!dictionary.contains("card"));
// ```

#include <balscm_version.h>

#include <bslalg_constructorproxy.h>
#include <bslalg_swaputil.h>

#include <bslma_allocator.h>
#include <bslma_bslallocator.h>

#include <bslmf_movableref.h>

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>

#include <bsl_cstddef.h>
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_map.h>
#include <bsl_optional.h>
#include <bsl_string.h>
#include <bsl_string_view.h>
#include <bsl_utility.h>
#include <bsl_vector.h>

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// clang-format off
// Include version that can be compiled with C++03
// Generated on Fri Nov 28 18:11:57 2025
// Command line: sim_cpp11_features.pl ball_categorymanager_radixtree.h

# define COMPILING_BALL_CATEGORYMANAGER_RADIXTREE_H
# include <ball_categorymanager_radixtree_cpp03.h>
# undef COMPILING_BALL_CATEGORYMANAGER_RADIXTREE_H

// clang-format on
#else

namespace BloombergLP {
namespace ball {

                  // ====================================
                  // class CategoryManager_RadixTree_Node
                  // ====================================

/// This class template represents a node in the radix tree.  Each node stores
/// a prefix string, an optional value, and child nodes mapped with their
/// prefix-part starting character.  As the second underscore in the class name
/// indicates, this is a component-local class not intended for use outside of
// the radix tree implementation.
template <class t_VALUE>
class CategoryManager_RadixTree_Node {
  public:
    // PUBLIC TYPES

    /// Child nodes mapped by the starting their starting character of their
    /// prefix-part.
    typedef bsl::map<char, CategoryManager_RadixTree_Node> Children;

  private:
    // PRIVATE TYPES
    typedef bslalg::ConstructorProxy<bsl::optional<t_VALUE> > ValueProxy;

  private:
    // DATA
    bsl::string d_prefix;    // prefix-part of this node
    ValueProxy  d_value;     // optional value
    Children    d_children;  // child nodes mapped by prefix part 1st character

  public:
    // PUBLIC TYPES
    typedef bsl::allocator<> allocator_type;

    // CREATORS

    /// Create a `CategoryManager_RadixTree_Node` object with the specified
    /// `prefix` and no value.  Optionally specify an `allocator` (e.g., the
    /// address of a `bslma::Allocator` object) to supply memory; otherwise,
    /// the default allocator is used.
    explicit CategoryManager_RadixTree_Node(
                         const bsl::string_view& prefix,
                         const allocator_type&   allocator = allocator_type());

    /// Create a `CategoryManager_RadixTree_Node` object having the same value
    /// as the specified `original` object.  Use the default allocator to
    /// supply memory for this object.
    CategoryManager_RadixTree_Node(
                               const CategoryManager_RadixTree_Node& original);

    /// Create a `CategoryManager_RadixTree_Node` object having the same value
    /// as the specified `original` object, and use the specified `allocator`
    /// to supply memory for this new object.
    CategoryManager_RadixTree_Node(
                              const CategoryManager_RadixTree_Node& original,
                              const allocator_type&                 allocator);

    /// Create a `CategoryManager_RadixTree_Node` object having the same value
    /// as the specified `original` object by moving (in amortized constant
    /// time) the contents of `original` to the newly-created object.  The
    /// allocator associated with `original` is propagated for use in the
    /// newly-created object.  `original` is left in a valid but unspecified
    /// state.
    CategoryManager_RadixTree_Node(
            bslmf::MovableRef<CategoryManager_RadixTree_Node> original)
                                                     BSLS_KEYWORD_NOEXCEPT;

    /// Create a `CategoryManager_RadixTree_Node` object having the same value
    /// as the specified `original` object, and use the specified `allocator`
    /// to supply memory for this new object.  The contents of `original` are
    /// moved (in amortized constant time) to the newly-created object if
    /// `allocator == original.get_allocator()`, and are move-inserted (in
    /// linear time) using `allocator` otherwise.  `original` is left in a
    /// valid but unspecified state.
    CategoryManager_RadixTree_Node(
                  bslmf::MovableRef<CategoryManager_RadixTree_Node> original,
                  const allocator_type&                             allocator);

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.
    CategoryManager_RadixTree_Node& operator=(
                                    const CategoryManager_RadixTree_Node& rhs);

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  The
    /// contents of `rhs` are moved (in amortized constant time) to this object
    /// if `get_allocator() == rhs.get_allocator()`; otherwise, all elements in
    /// this object are either destroyed or move-assigned to, and each
    /// additional element in `rhs` is move-inserted into this object.  `rhs`
    /// is left in a valid but unspecified state.
    CategoryManager_RadixTree_Node& operator=(
                        bslmf::MovableRef<CategoryManager_RadixTree_Node> rhs);

    /// Return a reference providing modifiable access to the child nodes of
    /// this node.
    Children& children();

    /// Return a reference providing modifiable access to the prefix string of
    /// this node.  Note that only a part of the prefix is stored in a node;
    /// the full key is obtained by concatenating the prefixes of all nodes.
    bsl::string& prefix();

    /// Efficiently exchange the value of this object with the value of the
    /// specified `other` object.  This method provides the no-throw
    /// exception-safety guarantee if the two objects were created with the
    /// same allocator; otherwise, it provides the strong guarantee.
    void swap(CategoryManager_RadixTree_Node& other);

    /// Return a reference providing modifiable access to the optional value of
    /// this node.
    bsl::optional<t_VALUE>& value();

    // ACCESSORS

    /// Return a reference providing non-modifiable access to the child nodes
    /// of this node.
    const Children& children() const;

    /// Return a reference providing non-modifiable access to the prefix string
    /// of this node.
    const bsl::string& prefix() const;

    /// Return a reference providing non-modifiable access to the optional
    /// value of this node.
    const bsl::optional<t_VALUE>& value() const;

                                  // Aspects

    /// Return the allocator used by this object to supply memory.
    allocator_type get_allocator() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` nodes have the same value,
/// and `false` otherwise.  Two nodes have the same value if they have the same
/// prefix, the same value (or both have no value), and the same children.
template <class t_VALUE>
bool operator==(const CategoryManager_RadixTree_Node<t_VALUE>& lhs,
                const CategoryManager_RadixTree_Node<t_VALUE>& rhs);

/// Return `true` if the specified `lhs` and `rhs` nodes do not have the same
/// value, and `false` otherwise.
template <class t_VALUE>
bool operator!=(const CategoryManager_RadixTree_Node<t_VALUE>& lhs,
                const CategoryManager_RadixTree_Node<t_VALUE>& rhs);

              // ==============================================
              // class CategoryManager_RadixTree_ChildNodeGuard
              // ==============================================

/// RAII guard to remove a child node when an exception is thrown during value
/// emplacement in a `CategoryManager_RadixTree_Node`.  This guard ensures
/// exception safety by automatically removing a newly created child node from
/// its parent if the value construction throws an exception.
template <class t_VALUE>
class CategoryManager_RadixTree_ChildNodeGuard {
    // TYPES
  public:
    typedef CategoryManager_RadixTree_Node<t_VALUE> Node;
    typedef typename Node::Children::iterator       ChildIterator;

  private:
    // DATA
    Node          *d_parent_p;
    ChildIterator  d_child;
    bool           d_released;

  private:
    // NOT IMPLEMENTED
    CategoryManager_RadixTree_ChildNodeGuard(
                        const CategoryManager_RadixTree_ChildNodeGuard&)
                                                     BSLS_KEYWORD_DELETED;
    CategoryManager_RadixTree_ChildNodeGuard& operator=(
                        const CategoryManager_RadixTree_ChildNodeGuard&)
                                                     BSLS_KEYWORD_DELETED;

  public:
    // CREATORS

    /// Create a guard managing the specified `child` iterator in the
    /// specified `parent` node.  The guard will erase the child from the
    /// parent upon destruction unless `release()` is called.
    CategoryManager_RadixTree_ChildNodeGuard(Node          *parent,
                                             ChildIterator  child);

    /// Destroy this guard.  If `release()` has not been called, erase the
    /// child node from the parent (specified at construction).
    ~CategoryManager_RadixTree_ChildNodeGuard();

    // MANIPULATORS

    /// Release this guard, preventing the child node from being erased upon
    /// destruction.  This method should be called after the value has been
    /// successfully emplaced in the child node.
    void release();
};

                    // ===============================
                    // class CategoryManager_RadixTree
                    // ===============================

/// This class template implements a space-efficient associative container that
/// maps string keys to values of the specified `t_VALUE` type.  The container
/// uses a radix tree (compressed trie) data structure, which shares common
/// prefixes among keys.  The container provides O(k) insertion, lookup, and
/// removal operations, where k is the key length.
template <class t_VALUE>
class CategoryManager_RadixTree {
  public:
    // PUBLIC TYPES
    typedef bsl::allocator<> allocator_type;
    typedef t_VALUE          value_type;
    typedef bsl::size_t      size_type;

    /// Type for mutable access to the optional value.  Used also as return
    /// type for mutable finders where empty optional signifies "not found".
    typedef bsl::optional<bsl::reference_wrapper<      t_VALUE> > OptValueRef;

    /// Type for immutable access to the optional value.  Used also as return
    /// type for immutable finders where empty optional signifies "not found".
    typedef bsl::optional<bsl::reference_wrapper<const t_VALUE> > OptValueCRef;

    /// The return type of adding a value to the tree with `emplace()`.  It is
    /// *not* the usual `insert` return type, because this data structure does
    /// not provide an iterator.  The `.first` boolean is `true` if an element
    /// was inserted, and the `.second` data member is a reference to the
    /// (possibly newly created) value.  Notice that if the value existed
    /// (`.first == false`) the reference wrapper will still give access to the
    /// value of that node; the `.second` is always a valid mutable reference
    /// to the value belonging to the key used in the `emplace` call.
    typedef bsl::pair<bool, bsl::reference_wrapper<t_VALUE> >    EmplaceResult;

  private:
    // PRIVATE TYPES
    typedef CategoryManager_RadixTree_Node<t_VALUE>           Node;
    typedef CategoryManager_RadixTree_ChildNodeGuard<t_VALUE> ChildNodeGuard;

    // PRIVATE MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    /// Recursively insert a value into the subtree rooted at the specified
    /// `node` with the specified `remainingKey` and `args`.  Return a pair
    /// consisting of a reference to the inserted or existing value and a
    /// boolean indicating whether insertion took place (`true` if inserted,
    /// `false` if already existed).  Notice that the returned reference is
    /// always valid and refers to the value associated with `remainingKey`.
    template <class... Args>
    EmplaceResult emplaceImp(Node                    *node,
                             const bsl::string_view&  remainingKey,
                             Args&&...                args);
#endif

    /// Recursively erase all children of the specified `node` (but not the
    /// node itself). Return the number of entries removed.
    size_type eraseAllChildren(Node *node);

    /// Recursively erase the entry with the specified `remainingKey` from the
    /// subtree rooted at the specified `node`.  Return `true` if the entry was
    /// removed, and `false` otherwise.  Deleting an entry may not delete the
    /// node corresponding to `remainingKey` if it has children.  However if
    /// the node is deleted, this method will also recursively delete
    /// all now-unused (no value) nodes that lead to it.  Notice that this
    /// method erases one entry only, meaning that it does not erase entries
    /// with keys that have `remainingKey` as a prefix.  If a node exists with
    /// `remainingKey` but has no value (it is not an entry) this method will
    /// do nothing and return `false`.
    bool eraseImp(Node *node, const bsl::string_view& remainingKey);

    /// Recursively erase all entries with keys matching the specified
    /// `remainingPrefix` from the subtree rooted at the specified `node`.
    /// Return the number of entries removed.  While deleting the entries all
    /// nodes that become unused (no value and no children) are also deleted.
    size_type erasePrefixImp(Node                    *node,
                             const bsl::string_view&  remainingPrefix);

    /// Clean up a child node after an erase operation.  If the child at the
    /// specified `it` in the specified `node`'s children map (accessed via
    /// the specified `firstChar`) has no value and no children, remove it.
    /// If the child has no value but exactly one grandchild, merge the child
    /// with its grandchild to maintain a compact tree structure.
    void cleanupChildAfterErase(
                         Node                                  *node,
                         typename Node::Children::iterator      it,
                         char                                   firstChar);

  private:
    // PRIVATE CLASS METHODS

    /// Recursively visit all key-value pairs in the subtree rooted at the
    /// specified `node` with the specified `keyPrefix`, invoking the specified
    /// `functor` for each pair.  Note that this variant does not allow
    /// modifying the value by the functor.  Return the number of times the
    /// functor is called.
    template <class t_FUNCTOR>
    static void forEachImp(const Node              *node,
                           const bsl::string_view&  keyPrefix,
                           const t_FUNCTOR&         functor);

    /// Recursively visit all key-value pairs in the subtree rooted at the
    /// specified `node` with the specified `keyPrefix`, invoking the specified
    /// `functor` for each pair.  Note that this "manipulator" variant allows
    /// modifying the value by the functor.
    template <class t_FUNCTOR>
    static void forEachImp(Node                    *node,
                           const bsl::string_view&  keyPrefix,
                           const t_FUNCTOR&         functor);

    /// Recursively visit all key-value pairs in the subtree rooted at the
    /// specified 'node' with the specified 'key', invoking the specified
    /// 'functor' for each pair. This manipulator version allows modifying the
    /// value by the functor.  Return the number of times the functor was
    /// called.
    template <class t_FUNCTOR>
    static size_type forEachPrefixImp(Node                    *node,
                                      const bsl::string_view&  key,
                                      const t_FUNCTOR&         functor);

    /// Recursively visit all key-value pairs in the subtree rooted at the
    /// specified 'node' with the specified 'key', invoking the specified
    /// 'functor' for each pair. This accessor version does not allow modifying
    /// the value by the functor.  Return the number of times the functor was
    /// called.
    template <class t_FUNCTOR>
    static size_type forEachPrefixImp(const Node              *node,
                                      const bsl::string_view&  key,
                                      const t_FUNCTOR&         functor);

    /// Recursively print the subtree rooted at the specified `node` with the
    /// specified `keyPrefix` to the specified `stream`, using the specified
    /// `currLevel` for indentation and `spacesPerLevel` for spacing control,
    /// and when necessary the specified `depth` to indicate the depth on the
    /// tree (in single line printing).  This method is intended for debugging.
    static void printNodeImp(bsl::ostream&            stream,
                             int                      depth,
                             const Node              *node,
                             const bsl::string&       keyPrefix,
                             int                      currLevel,
                             int                      spacesPerLevel);

  private:
    // DATA
    Node      d_root;     // root node of the tree
    size_type d_size;     // number of entries in the tree

    // FRIENDS
    template <class t_TYPE>
    friend bool operator==(const CategoryManager_RadixTree<t_TYPE>&,
                           const CategoryManager_RadixTree<t_TYPE>&);

    template <class t_TYPE>
    friend void swap(CategoryManager_RadixTree<t_TYPE>& a,
                     CategoryManager_RadixTree<t_TYPE>& b);

  public:
    // CREATORS

    /// Create an empty `CategoryManager_RadixTree`.  Optionally specify
    /// an `allocator` (e.g., the address of a `bslma::Allocator` object)
    /// to supply memory; otherwise, the default allocator is used.
    CategoryManager_RadixTree();
    explicit CategoryManager_RadixTree(const allocator_type& allocator);

    /// Create a `CategoryManager_RadixTree` having the same value as the
    /// specified `original` object.  Optionally specify an `allocator`
    /// (e.g., the address of a `bslma::Allocator` object) to supply
    /// memory; otherwise, the default allocator is used.
    CategoryManager_RadixTree(
                const CategoryManager_RadixTree& original,
                const allocator_type&            allocator = allocator_type());

    /// Create a `CategoryManager_RadixTree` having the same value as the
    /// specified `original` object by moving (in amortized constant time) the
    /// contents of `original` to the newly-created object.  The allocator
    /// associated with `original` is propagated for use in the
    /// newly-created object.  `original` is left in a valid but
    /// unspecified state.
    CategoryManager_RadixTree(
               bslmf::MovableRef<CategoryManager_RadixTree> original)
                                                     BSLS_KEYWORD_NOEXCEPT;

    /// Create a `CategoryManager_RadixTree` having the same value as the
    /// specified `original` object that uses the specified `allocator` to
    /// supply memory.  The contents of `original` are moved (in amortized
    /// constant time) to the newly-created object if
    /// `allocator == original.get_allocator()`, and are move-inserted (in
    /// linear time) using `allocator` otherwise.  `original` is left in a
    /// valid but unspecified state.
    CategoryManager_RadixTree(
                       bslmf::MovableRef<CategoryManager_RadixTree> original,
                       const allocator_type&                        allocator);

    /// Destroy this object.
    //! ~CategoryManager_RadixTree() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.
    CategoryManager_RadixTree& operator=(const CategoryManager_RadixTree& rhs);

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  The
    /// contents of `rhs` are moved (in amortized constant time) to this object
    /// if `get_allocator() == rhs.get_allocator()`; otherwise, all elements in
    /// this object are either destroyed or move-assigned to, and each
    /// additional element in `rhs` is move-inserted into this object.  `rhs`
    /// is left in a valid but unspecified state.
    CategoryManager_RadixTree& operator=(
                             bslmf::MovableRef<CategoryManager_RadixTree> rhs);

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    /// Insert into this tree an entry with the specified `key` and a newly
    /// created `t_VALUE` object, constructed by forwarding `get_allocator()`
    /// (if required) and the specified (variable number of) `args` to the
    /// corresponding constructor of `t_VALUE`.  Return a pair consisting of a
    /// reference to the value associated with `key` (whether newly inserted or
    /// already existing) and a boolean indicating whether insertion took place
    /// (`true` if the key was not already present, `false` otherwise).  This
    /// method requires that `t_VALUE` be `emplace-constructible` from `args`.
    template <class... Args>
    EmplaceResult emplace(const bsl::string_view& key,
                          Args&&...               args);
#endif

    /// Remove all entries from this tree.  After this call `empty()` will
    /// return `true`.  After this call the tree will have 0 nodes.
    void clear();

    /// Remove from this tree the entry with the specified `key`.  Return
    /// `true` if the entry was removed (key existed), and `false` otherwise.
    bool erase(const bsl::string_view& key);

    /// Remove all children of the entry matching the specified `prefix`, but
    /// not the entry with the prefix itself.  If unused (no value, no
    /// children) nodes remain remove those, too.  If no entry exists for
    /// `prefix` remove nothing.  Return the number of entries removed.
    size_type eraseChildrenOfPrefix(const bsl::string_view& prefix);

    /// Remove from this tree all entries with keys that have the specified
    /// `prefix`, including the entry for the `prefix` itself if it exists.
    /// Return the number of entries removed.  Note that this method removes
    /// all nodes whose keys start with `prefix`, not just those that have a
    /// value.
    size_type erasePrefix(const bsl::string_view& prefix);

    /// Return an optional containing a reference to the modifiable value
    /// associated with the specified `key`, or an empty optional if the key is
    /// not found.  The returned reference remains valid until the tree is
    /// modified.
    OptValueRef find(const bsl::string_view& key);

    /// Return the longest prefix of the specified `key` that has an associated
    /// value in this tree, or an empty string view if no such prefix exists.
    /// If the optionally specified `value` is not null, load into `*value` a
    /// reference to the value associated with the returned prefix.  Note that
    /// an empty return value can mean either that no matching prefix exists,
    /// or that the empty string itself is the longest matching prefix (when
    /// the tree contains a value for the empty key).  Also note that the
    /// returned reference (if set) remains valid until the tree is modified.
    bsl::string_view findLongestCommonPrefix(OptValueRef             *value,
                                             const bsl::string_view&  key);

    /// Call the specified `functor` for each key-value pair in this tree.
    /// The `functor` should be a callable object that accepts two parameters:
    /// `const bsl::string_view&` for the key and `t_VALUE&` for the value.
    /// The order of visitation is unspecified.  Noteice that the functor is
    /// able to modify the value.
    template <class t_FUNCTOR>
    void forEach(const t_FUNCTOR& functor);

    /// Call the specified `functor` for each key-value pair whose key starts
    /// with the specified `prefix`.  The `functor` should be a callable object
    /// that accepts two parameters: `const bsl::string_view&` for the key and
    /// `t_VALUE&` for the value.  Return the number of times the functor was
    /// called.  The order of visitation is unspecified.  Noteice that the
    /// functor is able to modify the value.
    template <class t_FUNCTOR>
    size_type forEachPrefix(const bsl::string_view& prefix,
                            const t_FUNCTOR&        functor);

    /// Efficiently exchange the value of this object with the value of the
    /// specified `other` object.  This method provides the no-throw
    /// exception-safety guarantee.  The behavior is undefined unless this
    /// object was created with the same allocator as `other`.
    void swap(CategoryManager_RadixTree& other);

    // ACCESSORS

    /// Return `true` if this tree contains an entry for the specified `key`,
    /// and `false` otherwise.
    bool contains(const bsl::string_view& key) const;

    /// Return the total number of nodes in this tree, including internal
    /// nodes without values.  Note that this method has O(n) complexity where
    /// n is the number of nodes, and is intended for use in testing to verify
    /// tree structure invariants.  In user code use `size()` that tells the
    /// actual number of entries with values.
    size_type countNodes() const;

    /// Return `true` if this tree contains no entries, and `false` otherwise.
    bool empty() const;

    /// Return an optional containing a reference to the non-modifiable value
    /// associated with the specified `key`, or an empty optional if the key is
    /// not found.  The returned reference remains valid until the tree is
    /// modified.
    OptValueCRef find(const bsl::string_view& key) const;

    /// Return the longest prefix of the specified `key` that has an associated
    /// value in this tree, or an empty string view if no such prefix exists.
    /// If the optionally specified `value` is not null, load into `*value` a
    /// reference to the value associated with the returned prefix.  Note that
    /// an empty return value can mean either that no matching prefix exists,
    /// or that the empty string itself is the longest matching prefix (when
    /// the tree contains a value for the empty key).  Also note that the
    /// returned reference (if set) remains valid until the tree is modified.
    bsl::string_view findLongestCommonPrefix(
                                           OptValueCRef            *value,
                                           const bsl::string_view&  key) const;

    /// Call the specified `functor` for each key-value pair in this tree.
    /// The `functor` should be a callable object that accepts two parameters:
    /// `const bsl::string_view&` for the key and `const t_VALUE&` for the
    /// value.  The order of visitation is unspecified.  See also the
    /// manipulator variation that allows the functor to modify the value.
    template <class t_FUNCTOR>
    void forEach(const t_FUNCTOR& functor) const;

    /// Call the specified `functor` for each key-value pair whose key starts
    /// with the specified `prefix`.  The `functor` should be a callable object
    /// that accepts two parameters: `const bsl::string_view&` for the key and
    /// `const t_VALUE&` for the value.  Return the number of times the functor
    /// was called.  The order of visitation is unspecified.  See also the
    /// manipulator variation that allows the functor to modify the value.
    template <class t_FUNCTOR>
    size_type forEachPrefix(const bsl::string_view& prefix,
                            const t_FUNCTOR&        functor) const;

    /// Write the value of this object to the specified output `stream` in a
    /// human-readable format, and return a non-`const` reference to
    /// `stream`.  Optionally specify an initial indentation `level`, whose
    /// absolute value is incremented recursively for nested objects.  If
    /// `level` is specified, optionally specify `spacesPerLevel`, whose
    /// absolute value indicates the number of spaces per indentation level
    /// for this and all of its nested objects.  If `level` is negative,
    /// suppress indentation of the first line.  If `spacesPerLevel` is
    /// negative, format the entire output on one line, suppressing all but
    /// the initial indentation (as governed by `level`).  If `stream` is
    /// not valid on entry, this operation has no effect.  Note that the
    /// format is not fully specified, and may change without notice.
    bsl::ostream& printNodes(bsl::ostream& stream,
                             int           level = 0,
                             int           spacesPerLevel = 4) const;

    /// Return the number of entries in this tree.
    size_type size() const;

                                // Aspects

    /// Return the allocator used by this object to supply memory.  Note that
    /// if no allocator was supplied at construction the default allocator in
    /// effect at construction is used.
    allocator_type get_allocator() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` objects have the same
/// value, and `false` otherwise.  Two `CategoryManager_RadixTree` objects
/// have the same value if they have the same number of entries and each key
/// in `lhs` maps to the same value as in `rhs`.
template <class t_VALUE>
bool operator==(const CategoryManager_RadixTree<t_VALUE>& lhs,
                const CategoryManager_RadixTree<t_VALUE>& rhs);

/// Return `true` if the specified `lhs` and `rhs` objects do not have the
/// same value, and `false` otherwise.  Two `CategoryManager_RadixTree`
/// objects do not have the same value if they differ in their number of
/// entries or if any key maps to different values in the two objects.
template <class t_VALUE>
bool operator!=(const CategoryManager_RadixTree<t_VALUE>& lhs,
                const CategoryManager_RadixTree<t_VALUE>& rhs);

// FREE FUNCTIONS

/// Exchange the values of the specified `a` and `b` objects.  This function
/// provides the no-throw exception-safety guarantee if the two objects were
/// created with the same allocator and the basic guarantee otherwise.
template <class t_VALUE>
void swap(CategoryManager_RadixTree<t_VALUE>& a,
          CategoryManager_RadixTree<t_VALUE>& b);

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

              // ----------------------------------------------
              // class CategoryManager_RadixTree_ChildNodeGuard
              // ----------------------------------------------

// CREATORS
template <class t_VALUE>
inline
CategoryManager_RadixTree_ChildNodeGuard<t_VALUE>::
CategoryManager_RadixTree_ChildNodeGuard(Node          *parent,
                                         ChildIterator  child)
: d_parent_p(parent)
, d_child(child)
, d_released(false)
{
    BSLS_ASSERT(parent);
    BSLS_ASSERT(child != parent->children().end());
}

template <class t_VALUE>
inline
CategoryManager_RadixTree_ChildNodeGuard<t_VALUE>::
~CategoryManager_RadixTree_ChildNodeGuard()
{
    if (!d_released) {
        d_parent_p->children().erase(d_child);
    }
}

// MANIPULATORS
template <class t_VALUE>
inline
void CategoryManager_RadixTree_ChildNodeGuard<t_VALUE>::release()
{
    d_released = true;
}

                  // ------------------------------------
                  // class CategoryManager_RadixTree_Node
                  // ------------------------------------

// CREATORS
template <class t_VALUE>
inline
CategoryManager_RadixTree_Node<t_VALUE>::CategoryManager_RadixTree_Node(
                                            const bsl::string_view& prefix,
                                            const allocator_type&   allocator)
: d_prefix(prefix, allocator)
, d_value(allocator)
, d_children(allocator)
{
}

template <class t_VALUE>
inline
CategoryManager_RadixTree_Node<t_VALUE>::CategoryManager_RadixTree_Node(
                                const CategoryManager_RadixTree_Node& original)
: d_prefix(original.d_prefix)
, d_value(original.d_value, allocator_type())
, d_children(original.d_children)
{
}

template <class t_VALUE>
inline
CategoryManager_RadixTree_Node<t_VALUE>::CategoryManager_RadixTree_Node(
                               const CategoryManager_RadixTree_Node& original,
                               const allocator_type&                 allocator)
: d_prefix(original.d_prefix, allocator)
, d_value(original.d_value, allocator)
, d_children(original.d_children, allocator)
{
}

template <class t_VALUE>
inline
CategoryManager_RadixTree_Node<t_VALUE>::CategoryManager_RadixTree_Node(
              bslmf::MovableRef<CategoryManager_RadixTree_Node> original)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_prefix(bslmf::MovableRefUtil::move(
                             bslmf::MovableRefUtil::access(original).d_prefix))
, d_value(allocator_type())
, d_children(bslmf::MovableRefUtil::move(
                           bslmf::MovableRefUtil::access(original).d_children))
{
    d_value.object() =
                 bslmf::MovableRefUtil::move(
                     bslmf::MovableRefUtil::access(original).d_value.object());
}

template <class t_VALUE>
inline
CategoryManager_RadixTree_Node<t_VALUE>::CategoryManager_RadixTree_Node(
                   bslmf::MovableRef<CategoryManager_RadixTree_Node> original,
                   const allocator_type&                             allocator)
: d_prefix(bslmf::MovableRefUtil::move(
                             bslmf::MovableRefUtil::access(original).d_prefix),
           allocator)
, d_value(bslmf::MovableRefUtil::move(
                              bslmf::MovableRefUtil::access(original).d_value),
                              allocator)
, d_children(bslmf::MovableRefUtil::move(
                           bslmf::MovableRefUtil::access(original).d_children),
             allocator)
{
}

// MANIPULATORS
template <class t_VALUE>
inline
CategoryManager_RadixTree_Node<t_VALUE>&
CategoryManager_RadixTree_Node<t_VALUE>::operator=(
                                     const CategoryManager_RadixTree_Node& rhs)
{
    if (this != &rhs) {
        // Copy-and-swap for strong exception safety
        CategoryManager_RadixTree_Node temp(rhs, get_allocator());
        swap(temp);
    }
    return *this;
}

template <class t_VALUE>
inline
CategoryManager_RadixTree_Node<t_VALUE>&
CategoryManager_RadixTree_Node<t_VALUE>::operator=(
                         bslmf::MovableRef<CategoryManager_RadixTree_Node> rhs)
{
    CategoryManager_RadixTree_Node& lvalue = rhs;
    if (this != &lvalue) {
        if (get_allocator() == lvalue.get_allocator()) {
            // Same allocator - can swap efficiently
            swap(lvalue);
        }
        else {
            // Different allocators - must deep copy, use copy-and-swap
            CategoryManager_RadixTree_Node temp(lvalue, get_allocator());
            swap(temp);
        }
    }
    return *this;
}

template <class t_VALUE>
inline
typename CategoryManager_RadixTree_Node<t_VALUE>::Children&
CategoryManager_RadixTree_Node<t_VALUE>::children()
{
    return d_children;
}

template <class t_VALUE>
inline
bsl::string& CategoryManager_RadixTree_Node<t_VALUE>::prefix()
{
    return d_prefix;
}

template <class t_VALUE>
void
CategoryManager_RadixTree_Node<t_VALUE>::swap(
                                         CategoryManager_RadixTree_Node& other)
{
    BSLS_ASSERT(get_allocator() == other.get_allocator());

    bslalg::SwapUtil::swap(&d_prefix,         &other.d_prefix);
    bslalg::SwapUtil::swap(&d_value.object(), &other.d_value.object());
    bslalg::SwapUtil::swap(&d_children,       &other.d_children);
}

template <class t_VALUE>
inline
bsl::optional<t_VALUE>& CategoryManager_RadixTree_Node<t_VALUE>::value()
{
    return d_value.object();
}

// ACCESSORS
template <class t_VALUE>
inline
const typename CategoryManager_RadixTree_Node<t_VALUE>::Children&
CategoryManager_RadixTree_Node<t_VALUE>::children() const
{
    return d_children;
}

template <class t_VALUE>
inline
const bsl::string& CategoryManager_RadixTree_Node<t_VALUE>::prefix() const
{
    return d_prefix;
}

template <class t_VALUE>
inline
const bsl::optional<t_VALUE>&
CategoryManager_RadixTree_Node<t_VALUE>::value() const
{
    return d_value.object();
}

                                  // Aspects

template <class t_VALUE>
inline
typename CategoryManager_RadixTree_Node<t_VALUE>::allocator_type
CategoryManager_RadixTree_Node<t_VALUE>::get_allocator() const
{
    return d_prefix.get_allocator();
}

                   // -------------------------------
                   // class CategoryManager_RadixTree
                   // -------------------------------

// PRIVATE MANIPULATORS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_VALUE>
template <class... Args>
typename CategoryManager_RadixTree<t_VALUE>::EmplaceResult
CategoryManager_RadixTree<t_VALUE>::emplaceImp(
                                         Node                    *node,
                                         const bsl::string_view&  remainingKey,
                                         Args&&...                args)
{
    BSLS_ASSERT(node);

    if (remainingKey.empty()) {
        if (node->value().has_value()) {
            return EmplaceResult(false, node->value().value());       // RETURN
        }
        node->value().emplace(std::forward<Args>(args)...);
        return EmplaceResult(true, node->value().value());            // RETURN
    }

    const char firstChar = remainingKey[0];
    typename Node::Children::iterator it = node->children().find(firstChar);

    if (it == node->children().end()) {
        // No child with this first character, create new node
        typename Node::Children::iterator iter =
                       node->children().emplace(firstChar, remainingKey).first;
        ChildNodeGuard guard(node, iter);
        iter->second.value().emplace(std::forward<Args>(args)...);
        guard.release();
        return EmplaceResult(true, iter->second.value().value());     // RETURN
    }

    Node&                  child       = it->second;
    const bsl::string_view childPrefix = child.prefix();

    // Find common prefix length
    size_type minLen = bsl::min(remainingKey.size(), childPrefix.size());
    const bsl::string_view::const_iterator mismatchPos =
                                   bsl::mismatch(remainingKey.begin(),
                                                 remainingKey.begin() + minLen,
                                                 childPrefix.begin())
                                       .first;
    const size_type commonLen = mismatchPos - remainingKey.begin();

    if (commonLen == childPrefix.size()) {
        // Key shares full prefix with child, recurse into child
        return emplaceImp(&child,
                          remainingKey.substr(commonLen),
                          std::forward<Args>(args)...);               // RETURN
    }

    // Need to split the child node
    // Build the split structure first, without modifying the tree
    Node splitNode(childPrefix.substr(0, commonLen), get_allocator());

    // Create a copy of the child node with adjusted prefix
    // Copy constructor (will throw if allocation fails)
    Node childCopy(child);
    childCopy.prefix() = childPrefix.substr(commonLen);

    // Insert the child copy into split node (may throw)
    splitNode.children().emplace(childCopy.prefix()[0],
                                 bslmf::MovableRefUtil::move(childCopy));

    if (commonLen == remainingKey.size()) {
        // The split point is exactly our key - emplace value into split node
        splitNode.value().emplace(std::forward<Args>(args)...);

        // All operations succeeded - commit by moving split structure into the
        // tree
        it->second = bslmf::MovableRefUtil::move(splitNode);
        return EmplaceResult(true,
                             it->second.value().value());             // RETURN
    }

    // Insert remaining key under split node (may throw)
    const bsl::string_view newKey = remainingKey.substr(commonLen);
    const typename Node::Children::iterator newIter =
                      splitNode.children().emplace(newKey[0], newKey).first;
    newIter->second.value().emplace(std::forward<Args>(args)...);

    // All operations succeeded - commit by moving split structure into tree
    it->second = bslmf::MovableRefUtil::move(splitNode);
    return EmplaceResult(true, newIter->second.value().value());
}
#endif

template <class t_VALUE>
typename CategoryManager_RadixTree<t_VALUE>::size_type
CategoryManager_RadixTree<t_VALUE>::eraseAllChildren(
                       typename CategoryManager_RadixTree<t_VALUE>::Node *node)
{
    BSLS_ASSERT(node);

    size_type count = 0;

    typedef typename Node::Children::iterator Iter;
    for (Iter it = node->children().begin();
              it != node->children().end(); ) {
        count += eraseAllChildren(&it->second);
        if (it->second.value().has_value()) {
            it->second.value().reset();
            --d_size;
            ++count;
        }
        it = node->children().erase(it);
    }

    return count;
}

template <class t_VALUE>
void CategoryManager_RadixTree<t_VALUE>::cleanupChildAfterErase(
                                   Node                             *node,
                                   typename Node::Children::iterator it,
                                   char                              firstChar)
{
    BSLS_ASSERT(node);

    Node& child = it->second;

    if (!child.value().has_value() && child.children().empty()) {
        // Remove child completely
        node->children().erase(it);
    }
    else if (!child.value().has_value() && child.children().size() == 1) {
        // Merge child with its single grandchild
        // Build merged node without modifying original, then commit atomically
        const typename Node::Children::iterator grandIt =
                                                      child.children().begin();
        Node& grandchild = grandIt->second;

        // Build new prefix (may throw - but original tree unchanged)
        bsl::string mergedPrefix(child.prefix() + grandchild.prefix(),
                                 get_allocator());

        // Create merged node (may throw - but original tree unchanged)
        Node mergedNode(mergedPrefix, get_allocator());
        mergedNode.value() = bslmf::MovableRefUtil::move(grandchild.value());
        mergedNode.children() =
                            bslmf::MovableRefUtil::move(grandchild.children());

        // All operations succeeded - commit by replacing child in tree
        node->children().erase(it);
        node->children().emplace(firstChar,
                                 bslmf::MovableRefUtil::move(mergedNode));
    }
}

template <class t_VALUE>
bool CategoryManager_RadixTree<t_VALUE>::eraseImp(
                                         Node                    *node,
                                         const bsl::string_view&  remainingKey)
{
    BSLS_ASSERT(node);

    if (remainingKey.empty()) {
        if (!node->value().has_value()) {
            return false;                                             // RETURN
        }
        node->value().reset();
        return true;                                                  // RETURN
    }

    const char firstChar = remainingKey[0];
    typename Node::Children::iterator it = node->children().find(firstChar);

    if (it == node->children().end()) {
        return false;                                                 // RETURN
    }

    Node&            child       = it->second;
    bsl::string_view childPrefix = child.prefix();

    if (!remainingKey.starts_with(childPrefix)) {
        return false;                                                 // RETURN
    }

    // Recurse into child
    const bool erased = eraseImp(&child,
                                 remainingKey.substr(childPrefix.size()));

    if (!erased) {
        return false;                                                 // RETURN
    }

    // Post-order cleanup: merge or remove child if needed
    cleanupChildAfterErase(node, it, firstChar);

    return true;
}

template <class t_VALUE>
typename CategoryManager_RadixTree<t_VALUE>::size_type
CategoryManager_RadixTree<t_VALUE>::erasePrefixImp(
                                      Node                    *node,
                                      const bsl::string_view&  remainingPrefix)
{
    BSLS_ASSERT(node);

    if (remainingPrefix.empty()) {
        // Found the prefix node - recursively count and remove this subtree
        size_type count = 0;

        // Count and remove value at this node
        if (node->value().has_value()) {
            node->value().reset();
            --d_size;
            ++count;
        }

        // Recursively count and remove all children
        typedef typename Node::Children::iterator Iter;
        for (Iter it = node->children().begin();
                  it != node->children().end();
                ++it) {
            count += erasePrefixImp(&it->second, "");
        }

        node->children().clear();

        return count;                                                 // RETURN
    }

    const char firstChar = remainingPrefix[0];
    const typename Node::Children::iterator it =
                                              node->children().find(firstChar);

    if (it == node->children().end()) {
        return 0;                                                     // RETURN
    }

    Node&                  child       = it->second;
    const bsl::string_view childPrefix = child.prefix();

    if (remainingPrefix.starts_with(childPrefix)) {
        // Prefix matches this child's prefix completely, recurse
        const size_type count =
                    erasePrefixImp(&child,
                                   remainingPrefix.substr(childPrefix.size()));

        // After recursion, clean up child if needed
        cleanupChildAfterErase(node, it, firstChar);

        return count;                                                 // RETURN
    }
    else if (childPrefix.starts_with(remainingPrefix)) {
        // Child prefix starts with remaining prefix - remove entire child
        const size_type count = erasePrefixImp(&child, "");
        node->children().erase(it);
        return count;                                                 // RETURN
    }

    return 0;
}

// PRIVATE CLASS METHODS
template <class t_VALUE>
template <class t_FUNCTOR>
void
CategoryManager_RadixTree<t_VALUE>::forEachImp(
                                            const Node              *node,
                                            const bsl::string_view&  keyPrefix,
                                            const t_FUNCTOR&         functor)
{
    // Design Note: No return value with a count because it would always be
    // `size()` as we always run on all nodes with values.

    BSLS_ASSERT(node);

    const bsl::string fullKey = keyPrefix + node->prefix();

    // Call on `node` itself
    if (node->value().has_value()) {
        functor(fullKey, node->value().value());
    }

    // Recursively handle the children
    typedef typename Node::Children::const_iterator ConstIter;
    for (ConstIter it = node->children().begin();
                   it != node->children().end();
                 ++it) {
        forEachImp(&it->second, fullKey, functor);
    }
}

template <class t_VALUE>
template <class t_FUNCTOR>
void
CategoryManager_RadixTree<t_VALUE>::forEachImp(
                                            Node                    *node,
                                            const bsl::string_view&  keyPrefix,
                                            const t_FUNCTOR&         functor)
{
    // Design Note: No return value with a count because it would always be
    // `size()` as we always run on all nodes with values.

    BSLS_ASSERT(node);

    const bsl::string fullKey = keyPrefix + node->prefix();

    // Call on `node` itself
    if (node->value().has_value()) {
        functor(fullKey, node->value().value());
    }

    // Recursively handle the children
    typedef typename Node::Children::iterator Iter;
    for (Iter it = node->children().begin();
              it != node->children().end();
            ++it) {
        forEachImp(&it->second, fullKey, functor);
    }
}

template <class t_VALUE>
template <class t_FUNCTOR>
typename CategoryManager_RadixTree<t_VALUE>::size_type
CategoryManager_RadixTree<t_VALUE>::forEachPrefixImp(
                                              Node                    *node,
                                              const bsl::string_view&  key,
                                              const t_FUNCTOR&         functor)
{
    BSLS_ASSERT(node);

    size_type count = 0;

    // Call on `node` itself
    if (node->value()) {
        functor(key, *node->value());
        ++count;
    }

    // Recursively handle the children
    typedef typename Node::Children::iterator Iter;
    for (Iter it = node->children().begin();
              it != node->children().end();
            ++it) {
        const bsl::string nextKey = key + it->second.prefix();
        count += forEachPrefixImp(&it->second, nextKey, functor);
    }

    return count;
}

template <class t_VALUE>
template <class t_FUNCTOR>
typename CategoryManager_RadixTree<t_VALUE>::size_type
CategoryManager_RadixTree<t_VALUE>::forEachPrefixImp(
                                              const Node              *node,
                                              const bsl::string_view&  key,
                                              const t_FUNCTOR&         functor)
{
    BSLS_ASSERT(node);

    size_type count = 0;

    // Call on `node` itself
    if (node->value()) {
        functor(key, *node->value());
        ++count;
    }

    // Recursively handle the children
    typedef typename Node::Children::const_iterator Iter;
    for (Iter it = node->children().begin();
              it != node->children().end();
            ++it) {
        const bsl::string nextKey = key + it->second.prefix();
        count += forEachPrefixImp(&it->second, nextKey, functor);
    }

    return count;
}

template <class t_VALUE>
void
CategoryManager_RadixTree<t_VALUE>::printNodeImp(
                                            bsl::ostream&       stream,
                                            int                 depth,
                                            const Node         *node,
                                            const bsl::string&  keyPrefix,
                                            int                 currLevel,
                                            int                 spacesPerLevel)
{
    BSLS_ASSERT(node);

    const bool noFirstLineIndent = (currLevel < 0);
    const bool singleLineMode = (spacesPerLevel < 0);

    const int absLevel          = noFirstLineIndent ? -currLevel : currLevel;
    const int absSpacesPerLevel = singleLineMode
                                     ? -spacesPerLevel
                                     : spacesPerLevel;

    if (!noFirstLineIndent) {
        stream << bsl::string(absLevel * absSpacesPerLevel, ' ');
    }

    if (singleLineMode) {
        stream << '{' << depth << "} ";
    }

    stream << '"' << keyPrefix << '"';
    if (node->value().has_value()) {
        stream << ": ";
        stream << node->value().value();
    }
    else {
        stream << ": **NO-VALUE**";
    }

    stream << (singleLineMode ? ' ' : '\n');

    const int nextLevel = absLevel + 1;

    typedef typename Node::Children::const_iterator Iter;
    for (Iter it = node->children().begin();
              it != node->children().end();
            ++it) {
        printNodeImp(stream,
                     depth + 1,
                     &it->second,
                     keyPrefix + it->second.prefix(),
                     singleLineMode ? -nextLevel : nextLevel,
                     spacesPerLevel);
    }
}

// CREATORS
template <class t_VALUE>
inline
CategoryManager_RadixTree<t_VALUE>::CategoryManager_RadixTree()
: d_root("")
, d_size(0)
{
    // Notice that the root has the empty key, so if we add a value with an
    // empty key it will not make a child node, it'll be added to `d_root`.
}

template <class t_VALUE>
inline
CategoryManager_RadixTree<t_VALUE>::CategoryManager_RadixTree(
                                              const allocator_type& allocator)
: d_root("", allocator)
, d_size(0)
{
    // Notice that the root has the empty key, so if we add a value with an
    // empty key it will not make a child node, it'll be added to `d_root`.
}

template <class t_VALUE>
inline
CategoryManager_RadixTree<t_VALUE>::CategoryManager_RadixTree(
                                    const CategoryManager_RadixTree& original,
                                    const allocator_type&            allocator)
: d_root(original.d_root, allocator)
, d_size(original.d_size)
{
}

template <class t_VALUE>
inline
CategoryManager_RadixTree<t_VALUE>::CategoryManager_RadixTree(
              bslmf::MovableRef<CategoryManager_RadixTree> original)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_root(bslmf::MovableRefUtil::move(
                              bslmf::MovableRefUtil::access(original).d_root)),
  d_size(bslmf::MovableRefUtil::move(
                              bslmf::MovableRefUtil::access(original).d_size))
{
    bslmf::MovableRefUtil::access(original).d_size = 0;
}

template <class t_VALUE>
inline
CategoryManager_RadixTree<t_VALUE>::CategoryManager_RadixTree(
                        bslmf::MovableRef<CategoryManager_RadixTree> original,
                        const allocator_type&                        allocator)
: d_root(bslmf::MovableRefUtil::move(
                               bslmf::MovableRefUtil::access(original).d_root),
         allocator)
, d_size(bslmf::MovableRefUtil::move(
                              bslmf::MovableRefUtil::access(original).d_size))
{
    if (bslmf::MovableRefUtil::access(original).get_allocator() == allocator) {
        bslmf::MovableRefUtil::access(original).d_size = 0;
    }
}

// MANIPULATORS
template <class t_VALUE>
inline
CategoryManager_RadixTree<t_VALUE>&
CategoryManager_RadixTree<t_VALUE>::operator=(
                                          const CategoryManager_RadixTree& rhs)
{
    if (this != &rhs) {
        CategoryManager_RadixTree temp(rhs, get_allocator());
        swap(temp);
    }
    return *this;
}

template <class t_VALUE>
inline
CategoryManager_RadixTree<t_VALUE>&
CategoryManager_RadixTree<t_VALUE>::operator=(
                              bslmf::MovableRef<CategoryManager_RadixTree> rhs)
{
    CategoryManager_RadixTree& lvalue = rhs;
    if (this != &lvalue) {
        if (get_allocator() == lvalue.get_allocator()) {
            CategoryManager_RadixTree temp(bslmf::MovableRefUtil::move(rhs));
            swap(temp);
        }
        else {
            CategoryManager_RadixTree temp(bslmf::MovableRefUtil::move(rhs),
                                           get_allocator());
            swap(temp);
        }
    }
    return *this;
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class t_VALUE>
template <class... Args>
typename CategoryManager_RadixTree<t_VALUE>::EmplaceResult
CategoryManager_RadixTree<t_VALUE>::emplace(const bsl::string_view& key,
                                            Args&&...               args)
{
    const EmplaceResult result = emplaceImp(&d_root,
                                            key,
                                            std::forward<Args>(args)...);
    if (result.first) {
        ++d_size;
    }
    return result;
}
#endif

template <class t_VALUE>
inline
void CategoryManager_RadixTree<t_VALUE>::clear()
{
    d_root.children().clear();
    d_root.value().reset();
    d_size = 0;
}

template <class t_VALUE>
bool CategoryManager_RadixTree<t_VALUE>::erase(const bsl::string_view& key)
{
    const bool erased = eraseImp(&d_root, key);
    if (erased) {
        --d_size;
    }
    return erased;
}

template <class t_VALUE>
typename CategoryManager_RadixTree<t_VALUE>::size_type
CategoryManager_RadixTree<t_VALUE>::eraseChildrenOfPrefix(
                                                const bsl::string_view& prefix)
{
    Node      *node = &d_root;
    size_type  pos  = 0;

    while (pos < prefix.size()) {
        typedef typename Node::Children::iterator Iter;

        Iter it = node->children().find(prefix[pos]);
        if (it == node->children().end()) {
            return 0;                                                 // RETURN
        }

        Node &child = it->second;
        const bsl::string_view childPrefix = child.prefix();
        if (prefix.substr(pos, childPrefix.size()) == childPrefix) {
            node = &child;
            pos += childPrefix.size();
        } else {
            return 0;                                                 // RETURN
        }
    }
    // Now 'node' is the prefix node. Erase all its children, but not itself.
    return eraseAllChildren(node);
}

template <class t_VALUE>
typename CategoryManager_RadixTree<t_VALUE>::size_type
CategoryManager_RadixTree<t_VALUE>::erasePrefix(const bsl::string_view& prefix)
{
    // Empty prefix is a prefix of all nodes, so we just quickly clear it out
    if (prefix.empty()) {
        const size_type oldSize = d_size;
        clear();
        return oldSize;                                               // RETURN
    }

    return erasePrefixImp(&d_root, prefix);
}

template <class t_VALUE>
typename CategoryManager_RadixTree<t_VALUE>::OptValueRef
CategoryManager_RadixTree<t_VALUE>::find(const bsl::string_view& key)
{
    // Iterative implementation to avoid stack overflow
    Node             *currentNode  = &d_root;
    bsl::string_view  remainingKey = key;

    while (true) {
        if (remainingKey.empty()) {
            return currentNode->value().has_value()
                  ? typename CategoryManager_RadixTree::OptValueRef(
                                        bsl::ref(currentNode->value().value()))
                  : bsl::nullopt;                                     // RETURN
        }

        const typename Node::Children::iterator it =
                                 currentNode->children().find(remainingKey[0]);

        if (it == currentNode->children().end()) {
            return bsl::nullopt;                                      // RETURN
        }

        Node&                  child       = it->second;
        const bsl::string_view childPrefix = child.prefix();

        // Check if key matches child prefix
        if (!remainingKey.starts_with(childPrefix)) {
            return bsl::nullopt;                                      // RETURN
        }

        remainingKey.remove_prefix(childPrefix.size());
        currentNode = &child;
    }
}

template <class t_VALUE>
inline
bsl::string_view
CategoryManager_RadixTree<t_VALUE>::findLongestCommonPrefix(
                                                OptValueRef             *value,
                                                const bsl::string_view&  key)
{
    OptValueCRef constValue;
    const CategoryManager_RadixTree<t_VALUE>* constThis =
                            const_cast<const CategoryManager_RadixTree*>(this);
    const bsl::string_view result =
                          constThis->findLongestCommonPrefix(&constValue, key);

    if (value) {
        if (constValue.has_value()) {
            *value = bsl::ref(const_cast<t_VALUE&>(constValue.value().get()));
        } else {
            *value = bsl::nullopt;
        }
    }
    return result;
}

template <class t_VALUE>
template <class t_FUNCTOR>
void CategoryManager_RadixTree<t_VALUE>::forEach(const t_FUNCTOR& functor)
{
    if (d_size == 0) {
        return;                                                       // RETURN
    }

    forEachImp(&d_root, "", functor);
}

template <class t_VALUE>
template <class t_FUNCTOR>
typename CategoryManager_RadixTree<t_VALUE>::size_type
CategoryManager_RadixTree<t_VALUE>::forEachPrefix(
                                               const bsl::string_view& prefix,
                                               const t_FUNCTOR&        functor)
{
    Node        *node = &d_root;
    bsl::string  keySoFar;
    size_type    pos = 0;
    while (pos < prefix.size()) {
        bool found = false;
        typedef typename Node::Children::iterator Iter;
        for (Iter it = node->children().begin();
                  it != node->children().end();
                ++it) {
            const Node&        child       = it->second;
            const bsl::string& childPrefix = child.prefix();
            size_type i = 0;
            while (i < childPrefix.size()
                && pos + i < prefix.size()
                && prefix[pos + i] == childPrefix[i]) {
                ++i;
            }
            if (i == childPrefix.size()) {
                // Full child prefix match, keep descending
                node = &it->second;
                keySoFar.append(childPrefix);
                pos += i;
                found = true;
                break;                                                 // BREAK
            } else if (i == prefix.size() - pos) {
                // Prefix matches the start of childPrefix, descend into child
                node = &it->second;
                keySoFar.append(childPrefix.substr(0, i));
                pos += i;
                found = true;
                // Now, apply functor to all descendants of this child
                return forEachPrefixImp(node, prefix, functor);
            }
        }
        if (!found) {
            node = 0;
            break;                                                     // BREAK
        }
    }
    if (!node) {
        return 0;                                                     // RETURN
    }
    return forEachPrefixImp(node, prefix.substr(0, keySoFar.size()), functor);
}

template <class t_VALUE>
inline
void CategoryManager_RadixTree<t_VALUE>::swap(CategoryManager_RadixTree& other)
{
    BSLS_ASSERT(get_allocator() == other.get_allocator());

    bslalg::SwapUtil::swap(&d_root, &other.d_root);
    bslalg::SwapUtil::swap(&d_size, &other.d_size);
}

// ACCESSORS
template <class t_VALUE>
inline
bool CategoryManager_RadixTree<t_VALUE>::contains(
                                             const bsl::string_view& key) const
{
    // Iterative implementation to avoid stack overflow
    const Node       *currentNode  = &d_root;
    bsl::string_view  remainingKey = key;

    while (true) {
        if (remainingKey.empty()) {
            return currentNode->value().has_value();                  // RETURN
        }

        const typename Node::Children::const_iterator it =
                                 currentNode->children().find(remainingKey[0]);

        if (it == currentNode->children().end()) {
            return false;                                             // RETURN
        }

        const Node&            child       = it->second;
        const bsl::string_view childPrefix = child.prefix();

        // Check if key matches child prefix
        if (!remainingKey.starts_with(childPrefix)) {
            return false;                                             // RETURN
        }

        remainingKey.remove_prefix(childPrefix.size());
        currentNode = &child;
    }
}

template <class t_VALUE>
typename CategoryManager_RadixTree<t_VALUE>::size_type
CategoryManager_RadixTree<t_VALUE>::countNodes() const
{
    size_type count = 1;  // count root node

    // Recursively count all child nodes
    typedef typename Node::Children::const_iterator Iter;
    for (Iter it = d_root.children().begin();
              it != d_root.children().end();
            ++it) {
        // Count this child node
        ++count;

        // Recursively count all descendants of this child
        bsl::vector<const Node*> stack;
        stack.push_back(&it->second);

        while (!stack.empty()) {
            const Node* node = stack.back();
            stack.pop_back();

            for (Iter childIt = node->children().begin();
                      childIt != node->children().end();
                    ++childIt) {
                ++count;
                stack.push_back(&childIt->second);
            }
        }
    }

    return count;
}

template <class t_VALUE>
inline
bool CategoryManager_RadixTree<t_VALUE>::empty() const
{
    return 0 == d_size;
}

template <class t_VALUE>
typename CategoryManager_RadixTree<t_VALUE>::OptValueCRef
CategoryManager_RadixTree<t_VALUE>::find(const bsl::string_view& key) const
{
    // Iterative implementation to avoid stack overflow
    const Node       *currentNode  = &d_root;
    bsl::string_view  remainingKey = key;

    while (true) {
        if (remainingKey.empty()) {
            return currentNode->value().has_value()
                 ? typename CategoryManager_RadixTree::OptValueCRef(
                                       bsl::cref(currentNode->value().value()))
                 : bsl::nullopt;                                      // RETURN
        }

        const typename Node::Children::const_iterator it =
                                 currentNode->children().find(remainingKey[0]);

        if (it == currentNode->children().end()) {
            return bsl::nullopt;                                      // RETURN
        }

        const Node&            child       = it->second;
        const bsl::string_view childPrefix = child.prefix();

        // Check if key matches child prefix
        if (!remainingKey.starts_with(childPrefix)) {
            return bsl::nullopt;                                      // RETURN
        }

        remainingKey.remove_prefix(childPrefix.size());
        currentNode = &child;
    }
}

template <class t_VALUE>
inline
bsl::string_view CategoryManager_RadixTree<t_VALUE>::findLongestCommonPrefix(
                                            OptValueCRef            *value,
                                            const bsl::string_view&  key) const

{
    const Node  *node              = &d_root;
    size_type    matched           = 0;
    size_type    pos               = 0;
    size_type    lastMatchedLength = 0;
    OptValueCRef lastValueRef      = bsl::nullopt;

    // Handle the case where the root node has a value (empty-string key)
    if (node->value().has_value()) {
        lastMatchedLength = 0;
        lastValueRef      = bsl::cref(node->value().value());
    }

    while (pos < key.size()) {
        typedef typename Node::Children::const_iterator Iter;
        const Iter it = node->children().find(key[pos]);
        if (it == node->children().end()) {
            break;                                                     // BREAK
        }
        const Node& child = it->second;
        const bsl::string& childPrefix = child.prefix();
        size_type i = 0;
        while (i < childPrefix.size()
            && pos + i < key.size()
            && key[pos + i] == childPrefix[i]) {
            ++i;
        }
        // Defensive: if no progress is made, break to avoid infinite loop
        if (i == 0) {
            // No characters matched in prefix, cannot advance
            break;                                                     // BREAK
        }
        if (i < childPrefix.size()) {
            // Partial match, stop here
            break;                                                     // BREAK
        }
        // Full prefix match
        matched += i;
        pos += i;
        node = &child;
        if (node->value().has_value()) {
            lastMatchedLength = matched;
            lastValueRef      = bsl::cref(node->value().value());
        }
    }
    if (value) {
        *value = lastValueRef;
    }
    return key.substr(0, lastMatchedLength);
}

template <class t_VALUE>
template <class t_FUNCTOR>
void
CategoryManager_RadixTree<t_VALUE>::forEach(const t_FUNCTOR& functor) const
{
    if (d_size == 0) {
        return;                                                       // RETURN
    }

    forEachImp(&d_root, "", functor);
}

template <class t_VALUE>
template <class t_FUNCTOR>
typename CategoryManager_RadixTree<t_VALUE>::size_type
CategoryManager_RadixTree<t_VALUE>::forEachPrefix(
                                         const bsl::string_view& prefix,
                                         const t_FUNCTOR&        functor) const
{
    const Node  *node = &d_root;
    bsl::string  keySoFar;
    size_type    pos = 0;

    while (pos < prefix.size()) {
        bool found = false;
        typedef typename Node::Children::const_iterator Iter;
        for (Iter it = node->children().begin();
                  it != node->children().end();
                ++it) {
            const Node& child = it->second;
            const bsl::string& childPrefix = child.prefix();
            size_type i = 0;
            while (i < childPrefix.size()
                && pos + i < prefix.size()
                && prefix[pos + i] == childPrefix[i]) {
                ++i;
            }
            if (i == childPrefix.size()) {
                // Full child prefix match, keep descending
                node = &it->second;
                keySoFar.append(childPrefix);
                pos += i;
                found = true;
                break;                                                 // BREAK
            } else if (i == prefix.size() - pos) {
                // Prefix matches the start of childPrefix, descend into child
                node = &it->second;
                keySoFar.append(childPrefix.substr(0, i));
                pos += i;
                found = true;
                // Now, apply functor to all descendants of this child
                return forEachPrefixImp(node, prefix, functor);
            }
        }
        if (!found) {
            node = 0;
            break;                                                     // BREAK
        }
    }
    if (!node) {
        return 0;                                                     // RETURN
    }
    return forEachPrefixImp(node, prefix.substr(0, keySoFar.size()), functor);
}

template <class t_VALUE>
bsl::ostream& CategoryManager_RadixTree<t_VALUE>::printNodes(
                                            bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    printNodeImp(stream, 0, &d_root, "", level, spacesPerLevel);
    return stream;
}

template <class t_VALUE>
inline
typename CategoryManager_RadixTree<t_VALUE>::size_type
CategoryManager_RadixTree<t_VALUE>::size() const
{
    return d_size;
}

                                 // Aspects

template <class t_VALUE>
inline
typename CategoryManager_RadixTree<t_VALUE>::allocator_type
CategoryManager_RadixTree<t_VALUE>::get_allocator() const
{
    return d_root.children().get_allocator();
}

}  // close package namespace

                  // ------------------------------------
                  // class CategoryManager_RadixTree_Node
                  // ------------------------------------

// FREE OPERATORS
template <class t_VALUE>
bool ball::operator==(const CategoryManager_RadixTree_Node<t_VALUE>& lhs,
                      const CategoryManager_RadixTree_Node<t_VALUE>& rhs)
{
    return lhs.prefix()   == rhs.prefix()
        && lhs.value()    == rhs.value()
        && lhs.children() == rhs.children();
}

template <class t_VALUE>
inline
bool ball::operator!=(const CategoryManager_RadixTree_Node<t_VALUE>& lhs,
                      const CategoryManager_RadixTree_Node<t_VALUE>& rhs)
{
    return !(lhs == rhs);
}

                    // -------------------------------
                    // class CategoryManager_RadixTree
                    // -------------------------------

// FREE OPERATORS

template <class t_VALUE>
bool ball::operator==(const CategoryManager_RadixTree<t_VALUE>& lhs,
                      const CategoryManager_RadixTree<t_VALUE>& rhs)
{
    if (lhs.d_size != rhs.d_size) {
        return false;                                                 // RETURN
    }

    // Since the radix tree is always in its most compact form, two equal
    // trees will have identical structure.  Use direct node comparison.
    return lhs.d_root == rhs.d_root;
}

template <class t_VALUE>
inline
bool ball::operator!=(const CategoryManager_RadixTree<t_VALUE>& lhs,
                      const CategoryManager_RadixTree<t_VALUE>& rhs)
{
    return !(lhs == rhs);
}

// FREE FUNCTIONS
template <class t_VALUE>
inline
void ball::swap(CategoryManager_RadixTree<t_VALUE>& a,
                CategoryManager_RadixTree<t_VALUE>& b)
{
    bslalg::SwapUtil::swap(&a.d_root, &b.d_root);
    bslalg::SwapUtil::swap(&a.d_size, &b.d_size);
}
}  // close enterprise namespace

#endif // End C++11 code

#endif  // INCLUDED_BALL_CATEGORYMANAGER_RADIXTREE_H

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
