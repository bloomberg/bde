/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Copyright (c) 1997
 * Moscow Center for SPARC Technology
 *
 * Copyright (c) 1999
 * Boris Fomitchev
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */

/* NOTE: This is an internal header file, included by other STL headers.
 *   You should not attempt to use it directly.
 */

#ifndef INCLUDED_BSLSTP_TREE
#define INCLUDED_BSLSTP_TREE

/*

Red-black tree class, designed for use in implementing STL
associative containers (set, multiset, map, and multimap). The
insertion and deletion algorithms are based on those in Cormen,
Leiserson, and Rivest, Introduction to Algorithms (MIT Press, 1990),
except that

(1) the header cell is maintained with links not only to the root
but also to the leftmost node of the tree, to enable constant time
begin(), and to the rightmost node of the tree, to enable linear time
performance when used with the generic set algorithms (set_union,
etc.);

(2) when a node being deleted has two children its successor node is
relinked into its place, rather than copied, so that the only
iterators invalidated are those referring to the deleted node.

*/

#ifndef INCLUDED_BSLSTP_ALLOC
#include <bslstp_alloc.h>
#endif

// Needed for the const and nonconst traits.  Remove later.
#ifndef INCLUDED_BSLSTP_ITERATOR
#include <bslstp_iterator.h>
#endif

#ifndef INCLUDED_BSLSTL_PAIR
#include <bslstl_pair.h>
#endif

#ifndef INCLUDED_BSLSTL_ITERATOR
#include <bslstl_iterator.h>
#endif

#ifndef INCLUDED_BSLSTL_UTIL
#include <bslstl_util.h>
#endif

#ifndef INCLUDED_BSLALG_HASTRAIT
#include <bslalg_hastrait.h>
#endif

#ifndef INCLUDED_BSLALG_PASSTHROUGHTRAIT
#include <bslalg_passthroughtrait.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARDESTRUCTIONPRIMITIVES
#include <bslalg_scalardestructionprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISECOPYABLE
#include <bslalg_typetraitbitwisecopyable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITSGROUPSTLORDERED
#include <bslalg_typetraitsgroupstlordered.h>
#endif

#ifndef INCLUDED_BSLS_EXCEPTIONUTIL
#include <bsls_exceptionutil.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_ALGORITHM
#include <algorithm>
#define INCLUDED_ALGORITHM
#endif

//_STLP_BEGIN_NAMESPACE
namespace bsl {

typedef bool _Rb_tree_Color_type;
//const _Rb_tree_Color_type BSLSTP_STL_TREE_RED = false;
//const _Rb_tree_Color_type BSLSTP_STL_TREE_BLACK = true;

#define BSLSTP_STL_TREE_RED false
#define BSLSTP_STL_TREE_BLACK true

struct _Rb_tree_node_base
{
  typedef _Rb_tree_Color_type _Color_type;
  typedef _Rb_tree_node_base* _Base_ptr;

  _Color_type _M_color;
  _Base_ptr _M_parent;
  _Base_ptr _M_left;
  _Base_ptr _M_right;

  static _Base_ptr  _S_minimum(_Base_ptr __x)
  {
    while (__x->_M_left != 0) __x = __x->_M_left;
    return __x;
  }

  static _Base_ptr  _S_maximum(_Base_ptr __x)
  {
    while (__x->_M_right != 0) __x = __x->_M_right;
    return __x;
  }
};

template <class _Value> struct _Rb_tree_node : public _Rb_tree_node_base
{
  _Value _M_value_field;
};

struct _Rb_tree_base_iterator;

template <class _Dummy> class _Rb_global {
public:
  typedef _Rb_tree_node_base* _Base_ptr;
  // those used to be global functions
  static void  _Rebalance(_Rb_tree_node_base* __x, _Rb_tree_node_base*& __root);
  static _Rb_tree_node_base* _Rebalance_for_erase(_Rb_tree_node_base* __z,
                                                  _Rb_tree_node_base*& __root,
                                                  _Rb_tree_node_base*& __leftmost,
                                                  _Rb_tree_node_base*& __rightmost);
  // those are from _Rb_tree_base_iterator - moved here to reduce code bloat
  // moved here to reduce code bloat without templatizing _Rb_tree_base_iterator
  static _Rb_tree_node_base*   _M_increment(_Rb_tree_node_base*);
  static _Rb_tree_node_base*   _M_decrement(_Rb_tree_node_base*);
  static void  _Rotate_left(_Rb_tree_node_base* __x, _Rb_tree_node_base*& __root);
  static void  _Rotate_right(_Rb_tree_node_base* __x, _Rb_tree_node_base*& __root);
};

typedef _Rb_global<bool> _Rb_global_inst;

struct _Rb_tree_base_iterator
{
  typedef _Rb_tree_node_base*               _Base_ptr;
//  typedef bidirectional_iterator_tag        iterator_category;
//  typedef ptrdiff_t                         difference_type;
  _Base_ptr _M_node;

  // TRAIT
  BSLALG_DECLARE_NESTED_TRAITS(_Rb_tree_base_iterator,
                               BloombergLP::bslalg_TypeTraitBitwiseCopyable);

  bool operator==(const _Rb_tree_base_iterator& __y) const {
    return _M_node == __y._M_node;
  }
  bool operator!=(const _Rb_tree_base_iterator& __y) const {
    return _M_node != __y._M_node;
  }
};


template <class _Value, class _Traits> struct _Rb_tree_iterator
        : public _Rb_tree_base_iterator
          // just to keep studio12-v4 happy, since algorithms takes only
          // iterators inheriting from 'std::iterator'
        , public std::iterator<bidirectional_iterator_tag, _Value>
{
  typedef _Value value_type;
  typedef bidirectional_iterator_tag        iterator_category;
  typedef ptrdiff_t                         difference_type;
  typedef typename _Traits::reference  reference;
  typedef typename _Traits::pointer    pointer;
  typedef _Rb_tree_iterator<_Value, _Traits> _Self;
  typedef _Rb_tree_node<_Value>* _Link_type;

  typedef BloombergLP::bslalg_PassthroughTrait<_Rb_tree_base_iterator,
                                  BloombergLP::bslalg_TypeTraitBitwiseCopyable>
                                                                     BaseTrait;

  // TRAITS
  BSLALG_DECLARE_NESTED_TRAITS(_Self,
                               BaseTrait);

  _Rb_tree_iterator() { _M_node = 0; }
  _Rb_tree_iterator(_Link_type __x) { _M_node = __x; }
  _Rb_tree_iterator(const _Rb_tree_iterator<_Value,
                    _Nonconst_traits<_Value> >& __it) { _M_node = __it._M_node; }

  reference operator*() const {
    return _Link_type(_M_node)->_M_value_field;
  }

  pointer operator -> ( ) const { return & ( operator * ( ) ) ; }

  _Self& operator++() { _M_node = _Rb_global_inst::_M_increment(_M_node); return *this; }
  _Self operator++(int) {
    _Self __tmp = *this;
    _M_node = _Rb_global_inst::_M_increment(_M_node);
    return __tmp;
  }

  _Self& operator--() { _M_node = _Rb_global_inst::_M_decrement(_M_node); return *this; }
  _Self operator--(int) {
    _Self __tmp = *this;
    _M_node = _Rb_global_inst::_M_decrement(_M_node);
    return __tmp;
  }
};

// Base class to help EH

template <class _Tp, class _Alloc> struct _Rb_tree_base
{
  typedef _Rb_tree_node<_Tp> _Node;
  typedef typename _Alloc_traits<_Tp, _Alloc>::allocator_type allocator_type;

  _Rb_tree_base(const allocator_type& __a) :
    _M_header(__a, (_Node*)0) {
      _M_header._M_data = _M_header.allocate(1);
  }
  ~_Rb_tree_base() {
          if (_M_header._M_data != 0)
    _M_header.deallocate(_M_header._M_data,1);
  }
  allocator_type get_allocator() const {
    return _M_header.allocator();
  }
protected:
  typedef typename _Alloc_traits<_Node, _Alloc>::allocator_type _M_node_allocator_type;
  _STLP_alloc_proxy<_Node*, _Node, _M_node_allocator_type> _M_header;
};


template <class _Key, class _Value, class _KeyOfValue, class _Compare,
          class _Alloc = bsl::allocator<_Value> > class _Rb_tree : public _Rb_tree_base<_Value, _Alloc> {
  typedef _Rb_tree_base<_Value, _Alloc> _Base;
  typedef _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc> _Self;

  struct QuickSwap;
  friend struct QuickSwap;

  struct QuickSwap
  {
      // Function object to quickly swap two trees with identical
      // allocators and allocation modes.
      void operator()(_Self& v1, _Self& v2) const
          // Swap contents of 'v1' and 'v2'.  Undefined unless
          // 'v1.get_allocator() == v2.get_allocator()'.
      {
          // MODIFIED BY PABLO
          //_STLP_STD::swap(v1._M_header._M_data, v2._M_header._M_data);
          //_STLP_STD::swap(v1._M_node_count,     v2._M_node_count);
          //_STLP_STD::swap(v1._M_key_compare,    v2._M_key_compare);
          typedef BloombergLP::bslalg_ScalarPrimitives primitive;

          primitive::swap(v1._M_header._M_data, v2._M_header._M_data);
          primitive::swap(v1._M_node_count,     v2._M_node_count);
          primitive::swap(v1._M_key_compare,    v2._M_key_compare);
      }
  };

protected:
  typedef _Rb_tree_node_base* _Base_ptr;
  typedef _Rb_tree_node<_Value> _Node;
  typedef _Rb_tree_Color_type _Color_type;
public:
  typedef _Key key_type;
  typedef _Value value_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef _Rb_tree_node<_Value>* _Link_type;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef bidirectional_iterator_tag _Iterator_category;
  typedef typename _Base::allocator_type allocator_type;

  typedef BloombergLP::bslalg_TypeTraitsGroupStlOrdered<_Value,_Compare,_Alloc>
      TreeTypeTraits;
  BSLALG_DECLARE_NESTED_TRAITS(_Rb_tree, TreeTypeTraits);

protected:

  _Link_type _M_create_node(const value_type& __x)
  {
    _Link_type __tmp = this->_M_header.allocate(1);
    BSLS_TRY {
      BloombergLP::bslalg_ScalarPrimitives::copyConstruct(
          &__tmp->_M_value_field, __x,
          this->_M_header.bslmaAllocator());
    }
    BSLS_CATCH(...) {
        this->_M_header.deallocate(__tmp,1);
        BSLS_RETHROW;
    }
    return __tmp;
  }

  _Link_type _M_clone_node(_Link_type __x)
  {
    _Link_type __tmp = _M_create_node(__x->_M_value_field);
    __tmp->_M_color = __x->_M_color;
    __tmp->_M_left = 0;
    __tmp->_M_right = 0;
    return __tmp;
  }

protected:
  size_type _M_node_count; // keeps track of size of tree
  _Compare _M_key_compare;

  // The casts to '(void *&)' below suppress warnings: "conversion from 'X' to
  // a more strictly aligned type 'Y' may cause misaligned access".  Note that
  // the casts in question (to '(_Link_type&)') are safe downcasts.

  _Link_type& _M_root() const
    { return (_Link_type&)(void *&) this->_M_header._M_data->_M_parent; }
  _Link_type& _M_leftmost() const
    { return (_Link_type&)(void *&) this->_M_header._M_data->_M_left; }
  _Link_type& _M_rightmost() const
    { return (_Link_type&)(void *&) this->_M_header._M_data->_M_right; }

  static _Link_type&  _S_left(_Link_type __x)
    { return (_Link_type&)(void *&)(__x->_M_left); }
  static _Link_type&  _S_right(_Link_type __x)
    { return (_Link_type&)(void *&)(__x->_M_right); }
  static _Link_type&  _S_parent(_Link_type __x)
    { return (_Link_type&)(void *&)(__x->_M_parent); }
  static reference   _S_value(_Link_type __x)
    { return __x->_M_value_field; }
  static const _Key&  _S_key(_Link_type __x)
    { return _KeyOfValue()(_S_value(__x)); }
  static _Color_type&  _S_color(_Link_type __x)
    { return (_Color_type&)(__x->_M_color); }

  static _Link_type&  _S_left(_Base_ptr __x)
    { return (_Link_type&)(void *&)(__x->_M_left); }
  static _Link_type&  _S_right(_Base_ptr __x)
    { return (_Link_type&)(void *&)(__x->_M_right); }
  static _Link_type&  _S_parent(_Base_ptr __x)
    { return (_Link_type&)(void *&)(__x->_M_parent); }
  static reference   _S_value(_Base_ptr __x)
    { return ((_Link_type)__x)->_M_value_field; }
  static const _Key&  _S_key(_Base_ptr __x)
    { return _KeyOfValue()(_S_value(_Link_type(__x)));}
  static _Color_type&  _S_color(_Base_ptr __x)
    { return (_Color_type&)(_Link_type(__x)->_M_color); }

  static _Link_type   _S_minimum(_Link_type __x)
    { return (_Link_type)  _Rb_tree_node_base::_S_minimum(__x); }

  static _Link_type   _S_maximum(_Link_type __x)
    { return (_Link_type) _Rb_tree_node_base::_S_maximum(__x); }

public:
  typedef _Rb_tree_iterator<value_type, _Nonconst_traits<value_type> > iterator;
  typedef _Rb_tree_iterator<value_type, _Const_traits<value_type> > const_iterator;

  // MODIFIED BY ARTHUR
  //_STLP_DECLARE_BIDIRECTIONAL_REVERSE_ITERATORS;
  typedef bsl::reverse_iterator<iterator>       reverse_iterator;
  typedef bsl::reverse_iterator<const_iterator> const_reverse_iterator;

private:
  iterator _M_insert(_Base_ptr __x, _Base_ptr __y, const value_type& __v, _Base_ptr __w = 0);
  _Link_type _M_copy(_Link_type __x, _Link_type __p);
  void _M_erase(_Link_type __x);

public:
                                // allocation/deallocation
  _Rb_tree()
    : _Rb_tree_base<_Value, _Alloc>(allocator_type()), _M_node_count(0), _M_key_compare(_Compare())
    { _M_empty_initialize(); }

  _Rb_tree(const _Compare& __comp)
    : _Rb_tree_base<_Value, _Alloc>(allocator_type()), _M_node_count(0), _M_key_compare(__comp)
    { _M_empty_initialize(); }

  _Rb_tree(const _Compare& __comp, const allocator_type& __a)
    : _Rb_tree_base<_Value, _Alloc>(__a), _M_node_count(0), _M_key_compare(__comp)
    { _M_empty_initialize(); }

  _Rb_tree(const _Self& __x)
    : _Rb_tree_base<_Value, _Alloc>(BloombergLP::bslstl_Util::copyContainerAllocator(__x.get_allocator())),
      _M_node_count(0), _M_key_compare(__x._M_key_compare)
  {
    if (__x._M_root() == 0)
      _M_empty_initialize();
    else {
      _S_color(this->_M_header._M_data) = BSLSTP_STL_TREE_RED;
      _M_root() = _M_copy(__x._M_root(), this->_M_header._M_data);
      _M_leftmost() = _S_minimum(_M_root());
      _M_rightmost() = _S_maximum(_M_root());
    }
    _M_node_count = __x._M_node_count;
  }

  // Copy-construct with alternative allocator.
  _Rb_tree(const _Self& __x, const allocator_type& __a)
    : _Rb_tree_base<_Value, _Alloc>(__a),
      _M_node_count(0), _M_key_compare(__x._M_key_compare)
  {
    if (__x._M_root() == 0)
      _M_empty_initialize();
    else {
      _S_color(this->_M_header._M_data) = BSLSTP_STL_TREE_RED;
      _M_root() = _M_copy(__x._M_root(), this->_M_header._M_data);
      _M_leftmost() = _S_minimum(_M_root());
      _M_rightmost() = _S_maximum(_M_root());
    }
    _M_node_count = __x._M_node_count;
  }

  /*explicit _Rb_tree(__full_move_source<_Self> src)
          : _Rb_tree_base<_Value, _Alloc>(_FullMoveSource<_Rb_tree_base<_Value, _Alloc> >(src.get())) {
  }*/

//  explicit _Rb_tree(__partial_move_source<_Self> src)
//          : _Rb_tree_base<_Value, _Alloc>(src.get()) {
//          src.get()._M_header._M_data = 0;
//          src.get()._M_node_count = 0;
//  }

  ~_Rb_tree() { clear(); }
  _Self& operator=(const _Self& __x);

private:
  void _M_empty_initialize() {
    _S_color(this->_M_header._M_data) = BSLSTP_STL_TREE_RED; // used to distinguish header from
                                          // __root, in iterator.operator++
    _M_root() = 0;
    _M_leftmost() = this->_M_header._M_data;
    _M_rightmost() = this->_M_header._M_data;
  }

public:
                                // accessors:
  _Compare key_comp() const { return _M_key_compare; }

  iterator begin() { return iterator(_M_leftmost()); }
  const_iterator begin() const { return const_iterator(_M_leftmost()); }
  iterator end() { return iterator(this->_M_header._M_data); }
  const_iterator end() const { return const_iterator(this->_M_header._M_data); }

  reverse_iterator rbegin() { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(end());
  }
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const {
    return const_reverse_iterator(begin());
  }
  bool empty() const { return _M_node_count == 0; }
  size_type size() const { return _M_node_count; }
  size_type max_size() const { return size_type(-1); }

  void swap(_Self& __t) {
    BloombergLP::bslstl_Util::swapContainers(*this, __t, QuickSwap());
  }

public:
                                // insert/erase
  pair<iterator,bool> insert_unique(const value_type& __x);
  iterator insert_equal(const value_type& __x);

  iterator insert_unique(iterator __position, const value_type& __x);
  iterator insert_equal(iterator __position, const value_type& __x);

  template<class _II> void insert_equal(_II __first, _II __last) {
    for ( ; __first != __last; ++__first)
      insert_equal(*__first);
  }
  template<class _II> void insert_unique(_II __first, _II __last) {
    for ( ; __first != __last; ++__first)
      insert_unique(*__first);
  }

  void erase(iterator __position) {
    _Link_type __y =
      (_Link_type) _Rb_global_inst::_Rebalance_for_erase(__position._M_node,
                                                         this->_M_header._M_data->_M_parent,
                                                         this->_M_header._M_data->_M_left,
                                                         this->_M_header._M_data->_M_right);
// MODIFIED BY ARTHUR
//    bsl::_bslstp_Destroy(&__y->_M_value_field);
    BloombergLP::bslalg_ScalarDestructionPrimitives::destroy(&__y->_M_value_field);
    this->_M_header.deallocate(__y,1);
    --_M_node_count;
  }

  size_type erase(const key_type& __x) {
    pair<iterator,iterator> __p = equal_range(__x);
    size_type __n = bsl::distance(__p.first, __p.second);
    erase(__p.first, __p.second);
    return __n;
  }

  void erase(iterator __first, iterator __last) {
    if (__first == begin() && __last == end())
      clear();
    else
      while (__first != __last) erase(__first++);
  }

  void erase(const key_type* __first, const key_type* __last) {
    while (__first != __last) erase(*__first++);
  }

  void clear() {
    if (_M_node_count != 0) {
      _M_erase(_M_root());
      _M_leftmost() = this->_M_header._M_data;
      _M_root() = 0;
      _M_rightmost() = this->_M_header._M_data;
      _M_node_count = 0;
    }
  }

public:
                                // set operations:

  template <class _KT> iterator find(const _KT& __x) { return iterator(_M_find(__x)); }
  template <class _KT> const_iterator find(const _KT& __x) const { return const_iterator(_M_find(__x)); }
private:
  template <class _KT> _Rb_tree_node<_Value>* _M_find(const _KT& __k) const
  {
    _Link_type __y = this->_M_header._M_data;      // Last node which is not less than __k.
    _Link_type __x = _M_root();      // Current node.

    while (__x != 0)
      if (!_M_key_compare(_S_key(__x), __k))
        __y = __x, __x = _S_left(__x);
      else
        __x = _S_right(__x);
    if (__y == this->_M_header._M_data || _M_key_compare(__k, _S_key(__y)))
      __y = this->_M_header._M_data;
    return __y;
  }

  _Link_type _M_lower_bound(const key_type& __k) const {
    _Link_type __y = this->_M_header._M_data; /* Last node which is not less than __k. */
    _Link_type __x = _M_root(); /* Current node. */

    while (__x != 0)
      if (!_M_key_compare(_S_key(__x), __k))
        __y = __x, __x = _S_left(__x);
      else
        __x = _S_right(__x);

    return __y;
  }

  _Link_type _M_upper_bound(const key_type& __k) const {
    _Link_type __y = this->_M_header._M_data; /* Last node which is greater than __k. */
    _Link_type __x = _M_root(); /* Current node. */

    while (__x != 0)
      if (_M_key_compare(__k, _S_key(__x)))
        __y = __x, __x = _S_left(__x);
      else
        __x = _S_right(__x);

    return __y;
  }

public:
  size_type count(const key_type& __x) const;
  iterator lower_bound(const key_type& __x) { return iterator(_M_lower_bound(__x)); }
  const_iterator lower_bound(const key_type& __x) const { return const_iterator(_M_lower_bound(__x)); }
  iterator upper_bound(const key_type& __x) { return iterator(_M_upper_bound(__x)); }
  const_iterator upper_bound(const key_type& __x) const { return const_iterator(_M_upper_bound(__x)); }
  pair<iterator,iterator> equal_range(const key_type& __x) {
    return pair<iterator, iterator>(lower_bound(__x), upper_bound(__x));
  }
  pair<const_iterator, const_iterator> equal_range(const key_type& __x) const {
    return pair<const_iterator,const_iterator>(lower_bound(__x),
                                               upper_bound(__x));
  }

public:
                                // Debugging.
  bool __rb_verify() const;
};

}  // close namespace bsl

// BEGIN FORMER CONTENTS OF bslstp_tree.c
/*
 *
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Copyright (c) 1997
 * Moscow Center for SPARC Technology
 *
 * Copyright (c) 1999
 * Boris Fomitchev
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 * Modified CRP 7/10/00 for improved conformance / efficiency on insert_unique /
 * insert_equal with valid hint -- efficiency is improved all around, and it is
 * should now be standard conforming for complexity on insert point immediately
 * after hint (amortized constant time).
 *
 */
// fbp: these defines are for outline methods definitions.
// needed for definitions to be portable. Should not be used in method bodies.
//#  define __iterator__  _STLP_TYPENAME_ON_RETURN_TYPE _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::iterator
//#  define __size_type__  _STLP_TYPENAME_ON_RETURN_TYPE _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::size_type

//#if defined ( _STLP_DEBUG)
//#  define _Rb_tree __WORKAROUND_DBG_RENAME(Rb_tree)
//#endif

namespace bsl {

template <class _Dummy> void
_Rb_global<_Dummy>::_Rotate_left(_Rb_tree_node_base* __x, _Rb_tree_node_base*& __root)
{
  _Rb_tree_node_base* __y = __x->_M_right;
  __x->_M_right = __y->_M_left;
  if (__y->_M_left !=0)
    __y->_M_left->_M_parent = __x;
  __y->_M_parent = __x->_M_parent;

  if (__x == __root)
    __root = __y;
  else if (__x == __x->_M_parent->_M_left)
    __x->_M_parent->_M_left = __y;
  else
    __x->_M_parent->_M_right = __y;
  __y->_M_left = __x;
  __x->_M_parent = __y;
}

template <class _Dummy> void
_Rb_global<_Dummy>::_Rotate_right(_Rb_tree_node_base* __x, _Rb_tree_node_base*& __root)
{
  _Rb_tree_node_base* __y = __x->_M_left;
  __x->_M_left = __y->_M_right;
  if (__y->_M_right != 0)
    __y->_M_right->_M_parent = __x;
  __y->_M_parent = __x->_M_parent;

  if (__x == __root)
    __root = __y;
  else if (__x == __x->_M_parent->_M_right)
    __x->_M_parent->_M_right = __y;
  else
    __x->_M_parent->_M_left = __y;
  __y->_M_right = __x;
  __x->_M_parent = __y;
}

template <class _Dummy> void
_Rb_global<_Dummy>::_Rebalance(_Rb_tree_node_base* __x,
                               _Rb_tree_node_base*& __root)
{
  __x->_M_color = BSLSTP_STL_TREE_RED;
  while (__x != __root && __x->_M_parent->_M_color == BSLSTP_STL_TREE_RED) {
    if (__x->_M_parent == __x->_M_parent->_M_parent->_M_left) {
      _Rb_tree_node_base* __y = __x->_M_parent->_M_parent->_M_right;
      if (__y && __y->_M_color == BSLSTP_STL_TREE_RED) {
        __x->_M_parent->_M_color = BSLSTP_STL_TREE_BLACK;
        __y->_M_color = BSLSTP_STL_TREE_BLACK;
        __x->_M_parent->_M_parent->_M_color = BSLSTP_STL_TREE_RED;
        __x = __x->_M_parent->_M_parent;
      }
      else {
        if (__x == __x->_M_parent->_M_right) {
          __x = __x->_M_parent;
          _Rotate_left(__x, __root);
        }
        __x->_M_parent->_M_color = BSLSTP_STL_TREE_BLACK;
        __x->_M_parent->_M_parent->_M_color = BSLSTP_STL_TREE_RED;
        _Rotate_right(__x->_M_parent->_M_parent, __root);
      }
    }
    else {
      _Rb_tree_node_base* __y = __x->_M_parent->_M_parent->_M_left;
      if (__y && __y->_M_color == BSLSTP_STL_TREE_RED) {
        __x->_M_parent->_M_color = BSLSTP_STL_TREE_BLACK;
        __y->_M_color = BSLSTP_STL_TREE_BLACK;
        __x->_M_parent->_M_parent->_M_color = BSLSTP_STL_TREE_RED;
        __x = __x->_M_parent->_M_parent;
      }
      else {
        if (__x == __x->_M_parent->_M_left) {
          __x = __x->_M_parent;
          _Rotate_right(__x, __root);
        }
        __x->_M_parent->_M_color = BSLSTP_STL_TREE_BLACK;
        __x->_M_parent->_M_parent->_M_color = BSLSTP_STL_TREE_RED;
        _Rotate_left(__x->_M_parent->_M_parent, __root);
      }
    }
  }
  __root->_M_color = BSLSTP_STL_TREE_BLACK;
}

template <class _Dummy> _Rb_tree_node_base*
_Rb_global<_Dummy>::_Rebalance_for_erase(_Rb_tree_node_base* __z,
                                         _Rb_tree_node_base*& __root,
                                         _Rb_tree_node_base*& __leftmost,
                                         _Rb_tree_node_base*& __rightmost)
{
  _Rb_tree_node_base* __y = __z;
  _Rb_tree_node_base* __x = 0;
  _Rb_tree_node_base* __x_parent = 0;
  if (__y->_M_left == 0)     // __z has at most one non-null child. y == z.
    __x = __y->_M_right;     // __x might be null.
  else
    if (__y->_M_right == 0)  // __z has exactly one non-null child. y == z.
      __x = __y->_M_left;    // __x is not null.
    else {                   // __z has two non-null children.  Set __y to
      __y = __y->_M_right;   //   __z's successor.  __x might be null.
      while (__y->_M_left != 0)
        __y = __y->_M_left;
      __x = __y->_M_right;
    }
  if (__y != __z) {          // relink y in place of z.  y is z's successor
    __z->_M_left->_M_parent = __y;
    __y->_M_left = __z->_M_left;
    if (__y != __z->_M_right) {
      __x_parent = __y->_M_parent;
      if (__x) __x->_M_parent = __y->_M_parent;
      __y->_M_parent->_M_left = __x;      // __y must be a child of _M_left
      __y->_M_right = __z->_M_right;
      __z->_M_right->_M_parent = __y;
    }
    else
      __x_parent = __y;
    if (__root == __z)
      __root = __y;
    else if (__z->_M_parent->_M_left == __z)
      __z->_M_parent->_M_left = __y;
    else
      __z->_M_parent->_M_right = __y;
    __y->_M_parent = __z->_M_parent;


// MODIFIED BY PABLO
//    _STLP_STD::swap(__y->_M_color, __z->_M_color);
    BloombergLP::bslalg_ScalarPrimitives::swap(__y->_M_color, __z->_M_color);

    __y = __z;
    // __y now points to node to be actually deleted
  }
  else {                        // __y == __z
    __x_parent = __y->_M_parent;
    if (__x) __x->_M_parent = __y->_M_parent;
    if (__root == __z)
      __root = __x;
    else
      if (__z->_M_parent->_M_left == __z)
        __z->_M_parent->_M_left = __x;
      else
        __z->_M_parent->_M_right = __x;
    if (__leftmost == __z) {
      if (__z->_M_right == 0)        // __z->_M_left must be null also
        __leftmost = __z->_M_parent;
    // makes __leftmost == _M_header if __z == __root
      else
        __leftmost = _Rb_tree_node_base::_S_minimum(__x);
    }
    if (__rightmost == __z) {
      if (__z->_M_left == 0)         // __z->_M_right must be null also
        __rightmost = __z->_M_parent;
    // makes __rightmost == _M_header if __z == __root
      else                      // __x == __z->_M_left
        __rightmost = _Rb_tree_node_base::_S_maximum(__x);
    }
  }
  if (__y->_M_color != BSLSTP_STL_TREE_RED) {
    while (__x != __root && (__x == 0 || __x->_M_color == BSLSTP_STL_TREE_BLACK))
      if (__x == __x_parent->_M_left) {
        _Rb_tree_node_base* __w = __x_parent->_M_right;
        if (__w->_M_color == BSLSTP_STL_TREE_RED) {
          __w->_M_color = BSLSTP_STL_TREE_BLACK;
          __x_parent->_M_color = BSLSTP_STL_TREE_RED;
          _Rotate_left(__x_parent, __root);
          __w = __x_parent->_M_right;
        }
        if ((__w->_M_left == 0 ||
             __w->_M_left->_M_color == BSLSTP_STL_TREE_BLACK) && (__w->_M_right == 0 ||
             __w->_M_right->_M_color == BSLSTP_STL_TREE_BLACK)) {
          __w->_M_color = BSLSTP_STL_TREE_RED;
          __x = __x_parent;
          __x_parent = __x_parent->_M_parent;
        } else {
          if (__w->_M_right == 0 ||
              __w->_M_right->_M_color == BSLSTP_STL_TREE_BLACK) {
            if (__w->_M_left) __w->_M_left->_M_color = BSLSTP_STL_TREE_BLACK;
            __w->_M_color = BSLSTP_STL_TREE_RED;
            _Rotate_right(__w, __root);
            __w = __x_parent->_M_right;
          }
          __w->_M_color = __x_parent->_M_color;
          __x_parent->_M_color = BSLSTP_STL_TREE_BLACK;
          if (__w->_M_right) __w->_M_right->_M_color = BSLSTP_STL_TREE_BLACK;
          _Rotate_left(__x_parent, __root);
          break;
        }
      } else {                  // same as above, with _M_right <-> _M_left.
        _Rb_tree_node_base* __w = __x_parent->_M_left;
        if (__w->_M_color == BSLSTP_STL_TREE_RED) {
          __w->_M_color = BSLSTP_STL_TREE_BLACK;
          __x_parent->_M_color = BSLSTP_STL_TREE_RED;
          _Rotate_right(__x_parent, __root);
          __w = __x_parent->_M_left;
        }
        if ((__w->_M_right == 0 ||
             __w->_M_right->_M_color == BSLSTP_STL_TREE_BLACK) && (__w->_M_left == 0 ||
             __w->_M_left->_M_color == BSLSTP_STL_TREE_BLACK)) {
          __w->_M_color = BSLSTP_STL_TREE_RED;
          __x = __x_parent;
          __x_parent = __x_parent->_M_parent;
        } else {
          if (__w->_M_left == 0 ||
              __w->_M_left->_M_color == BSLSTP_STL_TREE_BLACK) {
            if (__w->_M_right) __w->_M_right->_M_color = BSLSTP_STL_TREE_BLACK;
            __w->_M_color = BSLSTP_STL_TREE_RED;
            _Rotate_left(__w, __root);
            __w = __x_parent->_M_left;
          }
          __w->_M_color = __x_parent->_M_color;
          __x_parent->_M_color = BSLSTP_STL_TREE_BLACK;
          if (__w->_M_left) __w->_M_left->_M_color = BSLSTP_STL_TREE_BLACK;
          _Rotate_right(__x_parent, __root);
          break;
        }
      }
    if (__x) __x->_M_color = BSLSTP_STL_TREE_BLACK;
  }
  return __y;
}

template <class _Dummy> _Rb_tree_node_base*
_Rb_global<_Dummy>::_M_decrement(_Rb_tree_node_base* _M_node)
{
  if (_M_node->_M_color == BSLSTP_STL_TREE_RED && _M_node->_M_parent->_M_parent == _M_node)
    _M_node = _M_node->_M_right;
  else if (_M_node->_M_left != 0) {
    _Base_ptr __y = _M_node->_M_left;
    while (__y->_M_right != 0)
      __y = __y->_M_right;
    _M_node = __y;
  }
  else {
    _Base_ptr __y = _M_node->_M_parent;
    while (_M_node == __y->_M_left) {
      _M_node = __y;
      __y = __y->_M_parent;
    }
    _M_node = __y;
  }
  return _M_node;
}

template <class _Dummy> _Rb_tree_node_base*
_Rb_global<_Dummy>::_M_increment(_Rb_tree_node_base* _M_node)
{
  if (_M_node->_M_right != 0) {
    _M_node = _M_node->_M_right;
    while (_M_node->_M_left != 0)
      _M_node = _M_node->_M_left;
  }
  else {
    _Base_ptr __y = _M_node->_M_parent;
    while (_M_node == __y->_M_right) {
      _M_node = __y;
      __y = __y->_M_parent;
    }
    if (_M_node->_M_right != __y)
      _M_node = __y;
  }
  return _M_node;
}

template <class _Key, class _Value, class _KeyOfValue,
          class _Compare, class _Alloc>
          _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>&
          _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc> ::operator=(const _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>& __x)
{
  if (this != &__x) {
                                // Note that _Key may be a constant type.
    clear();
    _M_node_count = 0;
    _M_key_compare = __x._M_key_compare;
    if (__x._M_root() == 0) {
      _M_root() = 0;
      _M_leftmost() = this->_M_header._M_data;
      _M_rightmost() = this->_M_header._M_data;
    }
    else {
      _M_root() = _M_copy(__x._M_root(), this->_M_header._M_data);
      _M_leftmost() = _S_minimum(_M_root());
      _M_rightmost() = _S_maximum(_M_root());
      _M_node_count = __x._M_node_count;
    }
  }
  return *this;
}

// CRP 7/10/00 inserted argument __w_, which is another hint (meant to
// act like __x_ and ignore a portion of the if conditions -- specify
// __w_ != 0 to bypass comparison as false or __x_ != 0 to bypass
// comparison as true)
template <class _Key, class _Value, class _KeyOfValue,
          class _Compare, class _Alloc>
typename _Rb_tree < _Key , _Value , _KeyOfValue , _Compare , _Alloc > :: iterator
_Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc> ::_M_insert(_Rb_tree_node_base* __x_, _Rb_tree_node_base* __y_, const _Value& __v,
  _Rb_tree_node_base* __w_)
{
  _Link_type __w = (_Link_type) __w_;
  _Link_type __x = (_Link_type) __x_;
  _Link_type __y = (_Link_type) __y_;
  _Link_type __z;

  if ( __y == this->_M_header._M_data ||
       ( __w == 0 && // If w != 0, the remainder fails to false
         ( __x != 0 ||     // If x != 0, the remainder succeeds to true
           _M_key_compare( _KeyOfValue()(__v), _S_key(__y) ) )
         )
       ) {

    __z = _M_create_node(__v);
    _S_left(__y) = __z;               // also makes _M_leftmost() = __z
                                      //    when __y == _M_header
    if (__y == this->_M_header._M_data) {
      _M_root() = __z;
      _M_rightmost() = __z;
    }
    else if (__y == _M_leftmost())
      _M_leftmost() = __z;   // maintain _M_leftmost() pointing to min node
  }
  else {
    __z = _M_create_node(__v);
    _S_right(__y) = __z;
    if (__y == _M_rightmost())
      _M_rightmost() = __z;  // maintain _M_rightmost() pointing to max node
  }
  _S_parent(__z) = __y;
  _S_left(__z) = 0;
  _S_right(__z) = 0;
  _Rb_global_inst::_Rebalance(__z, this->_M_header._M_data->_M_parent);
  ++_M_node_count;
  return iterator(__z);
}

template <class _Key, class _Value, class _KeyOfValue,
          class _Compare, class _Alloc>
typename _Rb_tree < _Key , _Value , _KeyOfValue , _Compare , _Alloc > :: iterator
_Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc> ::insert_equal(const _Value& __v)
{
  _Link_type __y = this->_M_header._M_data;
  _Link_type __x = _M_root();
  while (__x != 0) {
    __y = __x;
    __x = _M_key_compare(_KeyOfValue()(__v), _S_key(__x)) ?
            _S_left(__x) : _S_right(__x);
  }
  return _M_insert(__x, __y, __v);
}


template <class _Key, class _Value, class _KeyOfValue,
          class _Compare, class _Alloc> pair< _Rb_tree_iterator<_Value, _Nonconst_traits<_Value> >, bool>
_Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc> ::insert_unique(const _Value& __v)
{
  _Link_type __y = this->_M_header._M_data;
  _Link_type __x = _M_root();
  bool __comp = true;
  while (__x != 0) {
    __y = __x;
    __comp = _M_key_compare(_KeyOfValue()(__v), _S_key(__x));
    __x = __comp ? _S_left(__x) : _S_right(__x);
  }
  iterator __j = iterator(__y);
  if (__comp) {
    if (__j == begin())
      return pair<iterator,bool>(_M_insert(/* __x*/ __y, __y, __v), true);
    else
      --__j;
  }
  if (_M_key_compare(_S_key(__j._M_node), _KeyOfValue()(__v)))
    return pair<iterator,bool>(_M_insert(__x, __y, __v), true);
  return pair<iterator,bool>(__j, false);
}

// Modifications CRP 7/10/00 as noted to improve conformance and
// efficiency.
template <class _Key, class _Value, class _KeyOfValue,
          class _Compare, class _Alloc>
typename _Rb_tree < _Key , _Value , _KeyOfValue , _Compare , _Alloc > :: iterator
_Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc> ::insert_unique(iterator __position, const _Value& __v)
{
  if (__position._M_node == this->_M_header._M_data->_M_left) { // begin()

    // if the container is empty, fall back on insert_unique.
    if (size() <= 0)
      return insert_unique(__v).first;

    if ( _M_key_compare(_KeyOfValue()(__v), _S_key(__position._M_node)))
      return _M_insert(__position._M_node, __position._M_node, __v);
    // first argument just needs to be non-null
    else
      {
        bool __comp_pos_v = _M_key_compare( _S_key(__position._M_node), _KeyOfValue()(__v) );

        if (__comp_pos_v == false)  // compare > and compare < both false so compare equal
          return __position;
        //Below __comp_pos_v == true

        // Standard-conformance - does the insertion point fall immediately AFTER
        // the hint?
        iterator __after = __position;
        ++__after;

        // Check for only one member -- in that case, __position points to itself,
        // and attempting to increment will cause an infinite loop.
        if (__after._M_node == this->_M_header._M_data)
          // Check guarantees exactly one member, so comparison was already
          // performed and we know the result; skip repeating it in _M_insert
          // by specifying a non-zero fourth argument.
          return _M_insert(0, __position._M_node, __v, __position._M_node);


        // All other cases:

        // Optimization to catch insert-equivalent -- save comparison results,
        // and we get this for free.
        if(_M_key_compare( _KeyOfValue()(__v), _S_key(__after._M_node) )) {
          if (_S_right(__position._M_node) == 0)
            return _M_insert(0, __position._M_node, __v, __position._M_node);
          else
            return _M_insert(__after._M_node, __after._M_node, __v);
        } else {
            return insert_unique(__v).first;
        }
      }

  } else if (__position._M_node == this->_M_header._M_data) { // end()
    if (_M_key_compare(_S_key(_M_rightmost()), _KeyOfValue()(__v)))
      // pass along to _M_insert that it can skip comparing
      // v, Key ; since compare Key, v was true, compare v, Key must be false.
      return _M_insert(0, _M_rightmost(), __v, __position._M_node); // Last argument only needs to be non-null
    else
      return insert_unique(__v).first;
  } else {
    iterator __before = __position;
    --__before;

    bool __comp_v_pos = _M_key_compare(_KeyOfValue()(__v), _S_key(__position._M_node));

    if (__comp_v_pos
      && _M_key_compare( _S_key(__before._M_node), _KeyOfValue()(__v) )) {

      if (_S_right(__before._M_node) == 0)
        return _M_insert(0, __before._M_node, __v, __before._M_node); // Last argument only needs to be non-null
      else
        return _M_insert(__position._M_node, __position._M_node, __v);
    // first argument just needs to be non-null
    } else
      {
        // Does the insertion point fall immediately AFTER the hint?
        iterator __after = __position;
        ++__after;

        // Optimization to catch equivalent cases and avoid unnecessary comparisons
        bool __comp_pos_v = !__comp_v_pos;  // Stored this result earlier
        // If the earlier comparison was true, this comparison doesn't need to be
        // performed because it must be false.  However, if the earlier comparison
        // was false, we need to perform this one because in the equal case, both will
        // be false.
        if (!__comp_v_pos) __comp_pos_v = _M_key_compare(_S_key(__position._M_node), _KeyOfValue()(__v));

        if ( (!__comp_v_pos) // comp_v_pos true implies comp_v_pos false
             && __comp_pos_v
             && (__after._M_node == this->_M_header._M_data ||
                _M_key_compare( _KeyOfValue()(__v), _S_key(__after._M_node) ))) {

          if (_S_right(__position._M_node) == 0)
            return _M_insert(0, __position._M_node, __v, __position._M_node);
          else
            return _M_insert(__after._M_node, __after._M_node, __v);
        } else {
          // Test for equivalent case
          if (__comp_v_pos == __comp_pos_v)
            return __position;
          else
            return insert_unique(__v).first;
        }
      }
  }
}


template <class _Key, class _Value, class _KeyOfValue,
          class _Compare, class _Alloc>
typename _Rb_tree < _Key , _Value , _KeyOfValue , _Compare , _Alloc > :: iterator
_Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc> ::insert_equal(iterator __position, const _Value& __v)
{
  if (__position._M_node == this->_M_header._M_data->_M_left) { // begin()

    // Check for zero members
    if (size() <= 0)
        return insert_equal(__v);

    if (!_M_key_compare(_S_key(__position._M_node), _KeyOfValue()(__v)))
      return _M_insert(__position._M_node, __position._M_node, __v);
    else    {
      // Check for only one member
      if (__position._M_node->_M_left == __position._M_node)
        // Unlike insert_unique, can't avoid doing a comparison here.
        return _M_insert(0, __position._M_node, __v);

      // All other cases:
      // Standard-conformance - does the insertion point fall immediately AFTER
      // the hint?
      iterator __after = __position;
      ++__after;

      // Already know that compare(pos, v) must be true!
      // Therefore, we want to know if compare(after, v) is false.
      // (i.e., we now pos < v, now we want to know if v <= after)
      // If not, invalid hint.
      if ( __after._M_node==this->_M_header._M_data ||
           !_M_key_compare( _S_key(__after._M_node), _KeyOfValue()(__v) ) ) {
        if (_S_right(__position._M_node) == 0)
          return _M_insert(0, __position._M_node, __v, __position._M_node);
        else
          return _M_insert(__after._M_node, __after._M_node, __v);
      } else // Invalid hint
        return insert_equal(__v);
    }
  } else if (__position._M_node == this->_M_header._M_data) {// end()
    if (!_M_key_compare(_KeyOfValue()(__v), _S_key(_M_rightmost())))
      return _M_insert(0, _M_rightmost(), __v, __position._M_node); // Last argument only needs to be non-null
    else
      return insert_equal(__v);
  } else {
    iterator __before = __position;
    --__before;
    // store the result of the comparison between pos and v so
    // that we don't have to do it again later.  Note that this reverses the shortcut
    // on the if, possibly harming efficiency in comparisons; I think the harm will
    // be negligible, and to do what I want to do (save the result of a comparison so
    // that it can be re-used) there is no alternative.  Test here is for before <= v <= pos.
    bool __comp_pos_v = _M_key_compare(_S_key(__position._M_node), _KeyOfValue()(__v));
    if (!__comp_pos_v
        && !_M_key_compare(_KeyOfValue()(__v), _S_key(__before._M_node))) {
      if (_S_right(__before._M_node) == 0)
        return _M_insert(0, __before._M_node, __v, __before._M_node); // Last argument only needs to be non-null
      else
        return _M_insert(__position._M_node, __position._M_node, __v);
    } else  {
      // Does the insertion point fall immediately AFTER the hint?
      // Test for pos < v <= after
      iterator __after = __position;
      ++__after;

      if (__comp_pos_v
          && ( __after._M_node==this->_M_header._M_data
               || !_M_key_compare( _S_key(__after._M_node), _KeyOfValue()(__v) ) ) ) {
        if (_S_right(__position._M_node) == 0)
          return _M_insert(0, __position._M_node, __v, __position._M_node);
        else
          return _M_insert(__after._M_node, __after._M_node, __v);
      } else // Invalid hint
        return insert_equal(__v);
    }
  }
}

template <class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc> _Rb_tree_node<_Value>*
_Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc> ::_M_copy(_Rb_tree_node<_Value>* __x, _Rb_tree_node<_Value>* __p)
{
                        // structural copy.  __x and __p must be non-null.
  _Link_type __top = _M_clone_node(__x);
  __top->_M_parent = __p;

  BSLS_TRY {
    if (__x->_M_right)
      __top->_M_right = _M_copy(_S_right(__x), __top);
    __p = __top;
    __x = _S_left(__x);

    while (__x != 0) {
      _Link_type __y = _M_clone_node(__x);
      __p->_M_left = __y;
      __y->_M_parent = __p;
      if (__x->_M_right)
        __y->_M_right = _M_copy(_S_right(__x), __y);
      __p = __y;
      __x = _S_left(__x);
    }
  }
  BSLS_CATCH(...) {
    _M_erase(__top);
    BSLS_RETHROW;
  }

  return __top;
}

// this has to stay out-of-line : it's recursive
template <class _Key, class _Value, class _KeyOfValue,
          class _Compare, class _Alloc> void
_Rb_tree<_Key,_Value,_KeyOfValue,
  _Compare,_Alloc>::_M_erase(_Rb_tree_node<_Value>* __x)
{
                                // erase without rebalancing
  while (__x != 0) {
    _M_erase(_S_right(__x));
    _Link_type __y = _S_left(__x);
    // MODIFIED BY ARTHUR
    // bsl::_bslstp_Destroy(&__x->_M_value_field);
    BloombergLP::bslalg_ScalarDestructionPrimitives::destroy(&__x->_M_value_field);
    this->_M_header.deallocate(__x,1);
    __x = __y;
  }
}

template <class _Key, class _Value, class _KeyOfValue,
          class _Compare, class _Alloc>
typename _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>::size_type
_Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc> ::count(const _Key& __k) const
{
  pair<const_iterator, const_iterator> __p = equal_range(__k);
  size_type __n = bsl::distance(__p.first, __p.second);
  return __n;
}

inline
int __black_count(_Rb_tree_node_base* __node, _Rb_tree_node_base* __root)
{
  if (__node == 0)
    return 0;
  else {
    int __bc = __node->_M_color == BSLSTP_STL_TREE_BLACK ? 1 : 0;
    if (__node == __root)
      return __bc;
    else
      return __bc + __black_count(__node->_M_parent, __root);
  }
}

template <class _Key, class _Value, class _KeyOfValue,
          class _Compare, class _Alloc> bool _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>::__rb_verify() const
{
  if (_M_node_count == 0 || begin() == end())
    return _M_node_count == 0 && begin() == end() && this->_M_header._M_data->_M_left == this->_M_header._M_data
      && this->_M_header._M_data->_M_right == this->_M_header._M_data;

  int __len = __black_count(_M_leftmost(), _M_root());
  for (const_iterator __it = begin(); __it != end(); ++__it) {
    _Link_type __x = (_Link_type) __it._M_node;
    _Link_type __L = _S_left(__x);
    _Link_type __R = _S_right(__x);

    if (__x->_M_color == BSLSTP_STL_TREE_RED)
      if ((__L && __L->_M_color == BSLSTP_STL_TREE_RED) ||
          (__R && __R->_M_color == BSLSTP_STL_TREE_RED))
        return false;

    if (__L && _M_key_compare(_S_key(__x), _S_key(__L)))
      return false;
    if (__R && _M_key_compare(_S_key(__R), _S_key(__x)))
      return false;

    if (!__L && !__R && __black_count(__x, _M_root()) != __len)
      return false;
  }

  if (_M_leftmost() != _Rb_tree_node_base::_S_minimum(_M_root()))
    return false;
  if (_M_rightmost() != _Rb_tree_node_base::_S_maximum(_M_root()))
    return false;

  return true;
}

template <class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
inline
bool  operator==(const _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>& __x,
                 const _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>& __y)
{
  return __x.size() == __y.size() &&
    std::equal(__x.begin(), __x.end(), __y.begin());
}

template <class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
inline
bool operator<(const _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>& __x,
               const _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>& __y) {
  return std::lexicographical_compare(__x.begin(), __x.end(),
          __y.begin(), __y.end());
}

template <class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
inline
bool operator!=(const _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>& __x,
                const _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>& __y)
{
    return !(__x == __y);
}

template <class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
inline
bool operator>(const _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>& __x,
               const _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>& __y)
{
    return __y < __x;
}

template <class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
inline
bool operator<=(const _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>& __x,
                const _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>& __y)
{
    return !(__y < __x);
}

template <class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
inline
bool operator>=(const _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>& __x,
                const _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>& __y)
{
    return !(__x < __y);
}

//template <class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
//struct __partial_move_traits< _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc> > {
//  typedef __true_type supported;
//};
//
//template <class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
//struct __action_on_move< _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc> > {
//  typedef __true_type swap;
//};



//#define _STLP_TEMPLATE_HEADER template <class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc>
//
//# define _STLP_TEMPLATE_CONTAINER _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>
//# include <bslstp_stl_relops_cont.h>
//# undef _STLP_TEMPLATE_CONTAINER
//
//#undef _STLP_TEMPLATE_HEADER

}  // close namespace bsl

//# undef __iterator__
//# undef iterator
//# undef __size_type__

// Local Variables:
// mode:C++
// End:
// END FORMER CONTENTS OF bslstp_stl_tree.c

//# undef _Rb_tree

//#if defined (_STLP_DEBUG)
//# include <stl/debug/bslstp_stl_tree.h>
//#endif

namespace bsl {
// Class rb_tree is not part of the C++ standard.  It is provided for
// compatibility with the HP STL.

template <class _Key, class _Value, class _KeyOfValue, class _Compare,
          class _Alloc = bsl::allocator<_Value> >
struct rb_tree : public _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc> {
  typedef _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc> _Base;
  typedef typename _Base::allocator_type allocator_type;

  rb_tree()
     : _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>(_Compare(), allocator_type()) {}
  rb_tree(const _Compare& __comp,
          const allocator_type& __a = allocator_type())
    : _Rb_tree<_Key, _Value, _KeyOfValue, _Compare, _Alloc>(__comp, __a) {}
  ~rb_tree() {}
};
}  // close namespace bsl

#endif /* INCLUDED_BSLSTP_TREE */

// Local Variables:
// mode:C++
// End:
