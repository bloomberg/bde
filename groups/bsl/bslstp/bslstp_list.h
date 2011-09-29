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

// Content originally from stl/_list.h

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


#ifndef INCLUDED_BSLSTP_LIST
#define INCLUDED_BSLSTP_LIST

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

// used by relops
# ifndef  INCLUDED_ALGORITHM
#  include <algorithm>
#  define INCLUDED_ALGORITHM
# endif

# ifndef INCLUDED_BSLSTP_ALLOC
#  include <bslstp_alloc.h>
# endif

// Needed for the const and nonconst traits.  Remove later.
#ifndef INCLUDED_BSLSTP_ITERATOR
#include <bslstp_iterator.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_SCALARDESTRUCTIONPRIMITIVES
#include <bslalg_scalardestructionprimitives.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITSGROUPSTLSEQUENCE
#include <bslalg_typetraitsgroupstlsequence.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#include <bslmf_isfundamental.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_EXCEPTIONUTIL
#include <bsls_exceptionutil.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

#ifndef INCLUDED_BSLSTL_ITERATOR
#include <bslstl_iterator.h>
#endif

#ifndef INCLUDED_BSLSTL_UTIL
#include <bslstl_util.h>
#endif

#ifndef INCLUDED_FUNCTIONAL
#include <functional>
#define INCLUDED_FUNCTIONAL
#endif

//_STLP_BEGIN_NAMESPACE
namespace bsl {

//# undef list
//# define  list  __WORKAROUND_DBG_RENAME(list)

struct _List_node_base {
  _List_node_base* _M_next;
  _List_node_base* _M_prev;
};

template <class _Dummy>
class _List_global {
public:
  typedef _List_node_base _Node;
  static void   _Transfer(_List_node_base* __position,
                          _List_node_base* __first,
                          _List_node_base* __last);
};

typedef _List_global<bool> _List_global_inst;

template <class _Tp>
struct _List_node : public _List_node_base {
  _Tp _M_data;
};

struct _List_iterator_base {
//  typedef size_t                            size_type;
//  typedef ptrdiff_t                         difference_type;
//  typedef bidirectional_iterator_tag        iterator_category;

  _List_node_base* _M_node;

  _List_iterator_base(_List_node_base* __x) : _M_node(__x) {}
  _List_iterator_base() {}

  void _M_incr() { _M_node = _M_node->_M_next; }
  void _M_decr() { _M_node = _M_node->_M_prev; }
  bool operator==(const _List_iterator_base& __y ) const {
    return _M_node == __y._M_node;
  }
  bool operator!=(const _List_iterator_base& __y ) const {
    return _M_node != __y._M_node;
  }
};




template<class _Tp, class _Traits>
struct _List_iterator : public _List_iterator_base
                      , public std::iterator<bidirectional_iterator_tag, _Tp >
{
  typedef _Tp value_type;
  typedef typename _Traits::pointer         pointer;
  typedef typename _Traits::reference       reference;

  typedef _List_iterator<_Tp, _Nonconst_traits<_Tp> > iterator;
  typedef _List_iterator<_Tp, _Const_traits<_Tp> >    const_iterator;
  typedef _List_iterator<_Tp, _Traits>                       _Self;

  typedef bidirectional_iterator_tag iterator_category;
  typedef _List_node<_Tp> _Node;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

  _List_iterator(_Node* __x) : _List_iterator_base(__x) {}
  _List_iterator() {}
  _List_iterator(const iterator& __x) :  _List_iterator_base(__x._M_node) {}

  reference operator*() const { return ((_Node*)_M_node)->_M_data; }

  pointer operator -> ( ) const { return & ( operator * ( ) ) ; }

  _Self& operator++() {
    this->_M_incr();
    return *this;
  }
  _Self operator++(int) {
    _Self __tmp = *this;
    this->_M_incr();
    return __tmp;
  }
  _Self& operator--() {
    this->_M_decr();
    return *this;
  }
  _Self operator--(int) {
    _Self __tmp = *this;
    this->_M_decr();
    return __tmp;
  }
};


// Base class that encapsulates details of allocators and helps
// to simplify EH

template <class _Tp, class _Alloc>
class _List_base
{
protected:
  typedef _List_node<_Tp> _Node;
  typedef typename _Alloc_traits<_Node, _Alloc>::allocator_type
           _Node_allocator_type;
public:
  typedef typename _Alloc_traits<_Tp, _Alloc>::allocator_type
          allocator_type;

  allocator_type get_allocator() const {
    //return _STLP_CONVERT_ALLOCATOR((const _Node_allocator_type&)_M_node, _Tp);
    return (const _Node_allocator_type&)(_M_node.allocator());
  }

  _List_base(const allocator_type& __a) : _M_node(__a, (_Node*)0) {
    _Node* __n = _M_node.allocate(1);
    __n->_M_next = __n;
    __n->_M_prev = __n;
    _M_node._M_data = __n;
  }
  ~_List_base() {
    if (_M_node._M_data != 0) {
      clear();
      _M_node.deallocate(_M_node._M_data, 1);
    }
  }

  void clear();

public:
  _STLP_alloc_proxy<_Node*, _Node, _Node_allocator_type>  _M_node;
};

template <class _Tp, class _Alloc = bsl::allocator<_Tp> >
class list;

// helper functions to reduce code duplication
template <class _Tp, class _Alloc, class _Predicate>
void _S_remove_if(list<_Tp, _Alloc>& __that, _Predicate __pred);

template <class _Tp, class _Alloc, class _BinaryPredicate>
void _S_unique(list<_Tp, _Alloc>& __that, _BinaryPredicate __binary_pred);

template <class _Tp, class _Alloc, class _StrictWeakOrdering>
void _S_merge(list<_Tp, _Alloc>&  __that,
              list<_Tp, _Alloc>&  __x,
              _StrictWeakOrdering __comp);

template <class _Tp, class _Alloc, class _StrictWeakOrdering>
void _S_sort(list<_Tp, _Alloc>& __that, _StrictWeakOrdering __comp);

template <class _Tp, class _Alloc>
class list : public _List_base<_Tp, _Alloc>
{
  typedef _List_base<_Tp, _Alloc> _Base;
  typedef list<_Tp, _Alloc> _Self;

  struct QuickSwap;
  friend struct QuickSwap;

  struct QuickSwap
  {
      // Function object to quickly swap two lists with identical
      // allocators and allocation modes.
      void operator()(_Self& v1, _Self& v2) const
          // Swap contents of 'v1' and 'v2'.  Undefined unless
          // 'v1.get_allocator() == v2.get_allocator()'.
      {
          // MODIFIED BY ARTHUR
          //_STLP_STD::swap(v1._M_node._M_data, v2._M_node._M_data);
          typedef _List_node<_Tp> _Node;
          _Node *tmp         = v1._M_node._M_data;
          v1._M_node._M_data = v2._M_node._M_data;
          v2._M_node._M_data = tmp;
      }
  };

public:
  // Modified by Pablo Halpern to use 'pointer', 'const_pointer', 'size_type',
  // and 'difference_type' from 'allocator_type'.
  typedef typename _Base::allocator_type allocator_type;
  typedef _Tp value_type;
  typedef typename allocator_type::pointer pointer;
  typedef typename allocator_type::const_pointer const_pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef _List_node<_Tp> _Node;
  typedef typename allocator_type::size_type size_type;
  typedef typename allocator_type::difference_type difference_type;
  typedef bidirectional_iterator_tag _Iterator_category;

public:
  typedef _List_iterator<_Tp, _Nonconst_traits<_Tp> > iterator;
  typedef _List_iterator<_Tp, _Const_traits<_Tp> >    const_iterator;

  // MODIFIED BY ARTHUR
  //_STLP_DECLARE_BIDIRECTIONAL_REVERSE_ITERATORS;
  typedef bsl::reverse_iterator<iterator>       reverse_iterator;
  typedef bsl::reverse_iterator<const_iterator> const_reverse_iterator;

  typedef BloombergLP::bslalg_TypeTraitsGroupStlSequence<_Tp,_Alloc>
      ListTypeTraits;
  BSLALG_DECLARE_NESTED_TRAITS(list, ListTypeTraits);
      // Declare nested type traits for this class.

protected:
  _Node* _M_create_node(const _Tp& __x = _Tp())
  {
    _Node* __p = this->_M_node.allocate(1);
    BSLS_TRY {

      BloombergLP::bslalg_ScalarPrimitives::copyConstruct(
          BSLS_UTIL_ADDRESSOF(__p->_M_data), __x,
          this->_M_node.bslmaAllocator());

    }
    BSLS_CATCH(...)
    {
        this->_M_node.deallocate(__p, 1);
        BSLS_RETHROW;
    }
    return __p;
  }

public:
  explicit
  list(const allocator_type& __a = allocator_type()) :
    _List_base<_Tp, _Alloc>(__a) {}

  iterator begin()             { return iterator((_Node*)(this->_M_node._M_data->_M_next)); }
  const_iterator begin() const { return const_iterator((_Node*)(this->_M_node._M_data->_M_next)); }

  iterator end()             { return this->_M_node._M_data; }
  const_iterator end() const { return this->_M_node._M_data; }

  reverse_iterator rbegin()
    { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const
    { return const_reverse_iterator(end()); }

  reverse_iterator rend()
    { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const
    { return const_reverse_iterator(begin()); }

  bool empty() const { return this->_M_node._M_data->_M_next == this->_M_node._M_data; }
  size_type size() const {
    size_type __result = bsl::distance(begin(), end());
    return __result;
  }

  // Modified by Pablo Halpern
  //size_type max_size() const { return size_type(-1); }
  size_type max_size() const {
      const size_type v = this->_M_node.allocator().max_size();
      return v ? v - 1 : 0;  // Don't return less than 0.
  }

  reference front() { return *begin(); }
  const_reference front() const { return *begin(); }
  reference back() { return *(--end()); }
  const_reference back() const { return *(--end()); }

  void swap(list<_Tp, _Alloc>& __x) {
    BloombergLP::bslstl_Util::swapContainers(*this, __x, QuickSwap());
  }

  iterator insert(iterator __position, const _Tp& __x = _Tp()) {
    _Node* __tmp = _M_create_node(__x);
    _List_node_base* __n = __position._M_node;
    _List_node_base* __p = __n->_M_prev;
    __tmp->_M_next = __n;
    __tmp->_M_prev = __p;
    __p->_M_next = __tmp;
    __n->_M_prev = __tmp;
    return __tmp;
  }

  template <class _InputIterator>
  void insert(iterator __pos, _InputIterator __first, _InputIterator __last) {

    // MODIFIED BY ARTHUR
    // typedef typename _Is_integer<_InputIterator>::_Integral _Integral;

    enum { VALUE = BloombergLP::bslmf_IsFundamental<_InputIterator>::VALUE };
    _M_insert_dispatch(__pos, __first, __last, (BloombergLP::bslmf_MetaInt<VALUE> *)0);

  }

  // Check whether it's an integral type.  If so, it's not an iterator.
  template<class _Integer>
  void _M_insert_dispatch(iterator __pos, _Integer __n, _Integer __x,
                          BloombergLP::bslmf_MetaInt<1> *) {
    _M_fill_insert(__pos, (size_type) __n, (_Tp) __x);
  }
  template <class _InputIter>
  void
  _M_insert_dispatch(iterator __position,
                     _InputIter __first, _InputIter __last,
                     BloombergLP::bslmf_MetaInt<0> *)
  {
    for ( ; __first != __last; ++__first)
      insert(__position, *__first);
  }
  void insert(iterator __pos, size_type __n, const _Tp& __x) { _M_fill_insert(__pos, __n, __x); }

  void _M_fill_insert(iterator __pos, size_type __n, const _Tp& __x) {
    for ( ; __n > 0; --__n)
      insert(__pos, __x);
  }
  // PGH 5/12/05: Moved these functions to out-of-line to minimize impact of a
  // Sun CC 5.2 bug (failure to destroy some temporaries).
  void push_front(const _Tp& __x);
  void push_back(const _Tp& __x);

  iterator erase(iterator __position) {
    _List_node_base* __next_node = __position._M_node->_M_next;
    _List_node_base* __prev_node = __position._M_node->_M_prev;
    _Node* __n = (_Node*) __position._M_node;
    __prev_node->_M_next = __next_node;
    __next_node->_M_prev = __prev_node;
    BloombergLP::bslalg_ScalarDestructionPrimitives::destroy(
                                            BSLS_UTIL_ADDRESSOF(__n->_M_data));
    this->_M_node.deallocate(__n, 1);
    return iterator((_Node*)__next_node);
    }

  iterator erase(iterator __first, iterator __last) {
    while (__first != __last)
      erase(__first++);
    return __last;
  }

  void resize(size_type __new_size, const _Tp& __x = _Tp());
  void pop_front() { erase(begin()); }
  void pop_back() {
    iterator __tmp = end();
    erase(--__tmp);
  }
  explicit list(size_type __n, const _Tp& __val = _Tp(),
                const allocator_type& __a = allocator_type())
    : _List_base<_Tp, _Alloc>(__a)
    { this->insert(begin(), __n, __val); }

  // We don't need any dispatching tricks here, because insert does all of
  // that anyway.
  template <class _InputIterator>
  list(_InputIterator __first, _InputIterator __last,
       const allocator_type& __a = allocator_type())
    : _List_base<_Tp, _Alloc>(__a)
  { insert(begin(), __first, __last); }

  list(const _Self& __x)
  : _List_base<_Tp, _Alloc>(BloombergLP::bslstl_Util::copyContainerAllocator(__x.get_allocator()))
    { insert(begin(), __x.begin(), __x.end()); }

  // Copy-construct with alternative allocator.
  list(const _Self& __x, const allocator_type& __a)
  : _List_base<_Tp, _Alloc>(__a)
    { insert(begin(), __x.begin(), __x.end()); }

  /*explicit list(__full_move_source<_Self> src)
     : _List_base<_Tp, _Alloc>(_FullMoveSource<_List_base<_Tp, _Alloc> >(src.get())) {
  }*/

//  explicit list(__partial_move_source<_Self> src)
//   : _List_base<_Tp, _Alloc>(src.get()) {
//   src.get()._M_node._M_data = 0;
//  }

  ~list() { }

  list<_Tp, _Alloc>& operator=(const list<_Tp, _Alloc>& __x);

public:
  // assign(), a generalized assignment member function.  Two
  // versions: one that takes a count, and one that takes a range.
  // The range version is a member template, so we dispatch on whether
  // or not the type is an integer.

  void assign(size_type __n, const _Tp& __val) { _M_fill_assign(__n, __val); }

  void _M_fill_assign(size_type __n, const _Tp& __val);

  template <class _InputIterator>
  void assign(_InputIterator __first, _InputIterator __last) {

// MODIFIED BY ARTHUR
//    typedef typename _Is_integer<_InputIterator>::_Integral _Integral;

    enum { VALUE = BloombergLP::bslmf_IsFundamental<_InputIterator>::VALUE };
    _M_assign_dispatch(__first, __last, (BloombergLP::bslmf_MetaInt<VALUE> *)0);
  }

  template <class _Integer>
  void _M_assign_dispatch(_Integer __n, _Integer __val, BloombergLP::bslmf_MetaInt<1> *)
    { assign((size_type) __n, (_Tp) __val); }

  template <class _InputIterator>
  void _M_assign_dispatch(_InputIterator __first2, _InputIterator __last2,
                          BloombergLP::bslmf_MetaInt<0> *)
  {
    iterator __first1 = begin();
    iterator __last1 = end();
    for ( ; __first1 != __last1 && __first2 != __last2; ++__first1, ++__first2)
      *__first1 = *__first2;
    if (__first2 == __last2)
      erase(__first1, __last1);
    else
      insert(__last1, __first2, __last2);
  }

public:
  void splice(iterator __position, _Self& __x) {
    if (!__x.empty())
      _List_global_inst::_Transfer(__position._M_node, __x.begin()._M_node, __x.end()._M_node);
  }
  void splice(iterator __position, _Self&, iterator __i) {
    iterator __j = __i;
    ++__j;
    if (__position == __i || __position == __j) return;
    _List_global_inst::_Transfer(__position._M_node, __i._M_node, __j._M_node);
  }
  void splice(iterator __position, _Self&, iterator __first, iterator __last) {
    if (__first != __last)
      _List_global_inst::_Transfer(__position._M_node, __first._M_node, __last._M_node);
  }

  void remove(const _Tp& __val) {
    iterator __first = begin();
    iterator __last = end();
    while (__first != __last) {
      iterator __next = __first;
      ++__next;
      if (__val == *__first) erase(__first);
      __first = __next;
    }
  }

  void unique() {
    _S_unique(*this, ::std::equal_to<_Tp>());
  }

  void merge(_Self& __x) {
    _S_merge(*this, __x, ::std::less<_Tp>());
  }

  void reverse() {
    _List_node_base* __p = this->_M_node._M_data;
    _List_node_base* __tmp = __p;
    do {
      // MODIFIED BY ARTHUR
      // _STLP_STD::swap(__tmp->_M_next, __tmp->_M_prev);

      _List_node_base *tmpPtr = __tmp->_M_next;
      __tmp->_M_next          = __tmp->_M_prev;
      __tmp->_M_prev          = tmpPtr;

      __tmp = __tmp->_M_prev;     // Old next node is now prev.
    } while (__tmp != __p);
  }

  void sort() {
    _S_sort(*this, std::less<_Tp>());
  }

  template <class _Predicate> void remove_if(_Predicate __pred)  {
    _S_remove_if(*this, __pred);
  }
  template <class _BinaryPredicate>
    void unique(_BinaryPredicate __binary_pred) {
    _S_unique(*this, __binary_pred);
  }

  template <class _StrictWeakOrdering>
    void merge(list<_Tp, _Alloc>& __x,
               _StrictWeakOrdering __comp) {
    _S_merge(*this, __x, __comp);
  }

  template <class _StrictWeakOrdering>
    void sort(_StrictWeakOrdering __comp) {
    _S_sort(*this, __comp);
  }

};

template <class _Tp, class _Alloc>
inline
void swap(list<_Tp, _Alloc>& lhs, list<_Tp, _Alloc>& rhs)
{
    lhs.swap(rhs);
}

template <class _Tp, class _Alloc>
inline
void list<_Tp, _Alloc>::push_front(const _Tp& __x) { insert(begin(), __x); }

template <class _Tp, class _Alloc>
inline
void list<_Tp, _Alloc>::push_back(const _Tp& __x) { insert(end(), __x); }


template <class _Tp, class _Alloc>
inline
bool
operator==(const list<_Tp,_Alloc>& __x, const list<_Tp,_Alloc>& __y)
{
  typedef typename list<_Tp,_Alloc>::const_iterator const_iterator;
  const_iterator __end1 = __x.end();
  const_iterator __end2 = __y.end();

  const_iterator __i1 = __x.begin();
  const_iterator __i2 = __y.begin();
  while (__i1 != __end1 && __i2 != __end2 && *__i1 == *__i2) {
    ++__i1;
    ++__i2;
  }
  return __i1 == __end1 && __i2 == __end2;
}

template <class _Tp, class _Alloc>
inline
bool operator<(const list<_Tp, _Alloc>& __x,
               const list<_Tp, _Alloc>& __y) {
  return std::lexicographical_compare(__x.begin(), __x.end(),
          __y.begin(), __y.end());
}

template <class _Tp, class _Alloc>
inline
bool operator!=(const list<_Tp, _Alloc>& __x,
                const list<_Tp, _Alloc>& __y) {return !(__x == __y);}

template <class _Tp, class _Alloc>
inline
bool operator>(const list<_Tp, _Alloc>& __x,
               const list<_Tp, _Alloc>& __y)  {return __y < __x;}

template <class _Tp, class _Alloc>
inline
bool operator<=(const list<_Tp, _Alloc>& __x,
                const list<_Tp, _Alloc>& __y) { return !(__y < __x);}

template <class _Tp, class _Alloc>
inline
bool operator>=(const list<_Tp, _Alloc>& __x,
                const list<_Tp, _Alloc>& __y) { return !(__x < __y);}

//template <class _Tp, class _Alloc>
//struct __partial_move_traits<list<_Tp, _Alloc> > {
//  typedef __true_type supported;
//};
//
//template <class _Tp, class _Alloc>
//struct __action_on_move< list<_Tp, _Alloc> > {
//  typedef __true_type swap;
//};




//# define _STLP_EQUAL_OPERATOR_SPECIALIZED
//# define _STLP_TEMPLATE_HEADER    template <class _Tp, class _Alloc>
//# define _STLP_TEMPLATE_CONTAINER list<_Tp, _Alloc>
//# include <bslstp_stl_relops_cont.h>
//# undef _STLP_TEMPLATE_CONTAINER
//# undef _STLP_TEMPLATE_HEADER
//# undef _STLP_EQUAL_OPERATOR_SPECIALIZED

//_STLP_END_NAMESPACE

}  // close namespace bsl

// BEGIN FORMER CONTENTS OF bslstp_stl_list.c
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
 */

# ifndef INCLUDED_BSLS_OBJECTBUFFER
#   include <bsls_objectbuffer.h>
# endif
# ifndef INCLUDED_BSLALG_ARRAYPRIMITIVES
#   include <bslalg_arrayprimitives.h>
# endif
# ifndef INCLUDED_BSLMA_AUTODESTRUCTOR
#   include <bslma_autodestructor.h>
# endif

//# undef list
//# define  list  __WORKAROUND_DBG_RENAME(list)

namespace bsl {

template <class _Dummy>
void
_List_global<_Dummy>::_Transfer(_List_node_base* __position,
                                _List_node_base* __first,
                                _List_node_base* __last) {
  if (__position != __last) {
    // Remove [first, last) from its old position.
    ((_Node*) (__last->_M_prev))->_M_next = __position;
    ((_Node*) (__first->_M_prev))->_M_next    = __last;
    ((_Node*) (__position->_M_prev))->_M_next = __first;

    // Splice [first, last) into its new position.
    _Node* __tmp = (_Node*) (__position->_M_prev);
    __position->_M_prev = __last->_M_prev;
    __last->_M_prev      = __first->_M_prev;
    __first->_M_prev    = __tmp;
  }
}

template <class _Tp, class _Alloc>
void
_List_base<_Tp,_Alloc>::clear()
{
  _List_node<_Tp>* __cur = (_List_node<_Tp>*) this->_M_node._M_data->_M_next;
  while (__cur != this->_M_node._M_data) {
    _List_node<_Tp>* __tmp = __cur;
    __cur = (_List_node<_Tp>*) __cur->_M_next;
    BloombergLP::bslalg_ScalarDestructionPrimitives::destroy(
                                          BSLS_UTIL_ADDRESSOF(__tmp->_M_data));
    this->_M_node.deallocate(__tmp, 1);
  }
  this->_M_node._M_data->_M_next = this->_M_node._M_data;
  this->_M_node._M_data->_M_prev = this->_M_node._M_data;
}

template <class _Tp, class _Alloc>
void list<_Tp, _Alloc>::resize(size_type __new_size, const _Tp& __x)
{
  iterator __i = begin();
  size_type __len = 0;
  for ( ; __i != end() && __len < __new_size; ++__i, ++__len) {}

  if (__len == __new_size)
    erase(__i, end());
  else                          // __i == end()
    insert(end(), __new_size - __len, __x);
}

template <class _Tp, class _Alloc>
list<_Tp, _Alloc>& list<_Tp, _Alloc>::operator=(const list<_Tp, _Alloc>& __x)
{
  if (this != &__x) {
    iterator __first1 = begin();
    iterator __last1 = end();
    const_iterator __first2 = __x.begin();
    const_iterator __last2 = __x.end();
    while (__first1 != __last1 && __first2 != __last2)
      *__first1++ = *__first2++;
    if (__first2 == __last2)
      erase(__first1, __last1);
    else
      insert(__last1, __first2, __last2);
  }
  return *this;
}

template <class _Tp, class _Alloc>
void list<_Tp, _Alloc>::_M_fill_assign(size_type __n, const _Tp& __val) {
  iterator __i = begin();
  for ( ; __i != end() && __n > 0; ++__i, --__n)
    *__i = __val;
  if (__n > 0)
    insert(end(), __n, __val);
  else
    erase(__i, end());
}

template <class _Tp, class _Alloc, class _Predicate>
void _S_remove_if(list<_Tp, _Alloc>& __that, _Predicate __pred)  {
  typename list<_Tp, _Alloc>::iterator __first = __that.begin();
  typename list<_Tp, _Alloc>::iterator __last = __that.end();
  while (__first != __last) {
    typename list<_Tp, _Alloc>::iterator __next = __first;
    ++__next;
    if (__pred(*__first)) __that.erase(__first);
    __first = __next;
  }
}

template <class _Tp, class _Alloc, class _BinaryPredicate>
void _S_unique(list<_Tp, _Alloc>& __that, _BinaryPredicate __binary_pred) {
  typename list<_Tp, _Alloc>::iterator __first = __that.begin();
  typename list<_Tp, _Alloc>::iterator __last = __that.end();
  if (__first == __last) return;
  typename list<_Tp, _Alloc>::iterator __next = __first;
  while (++__next != __last) {
    if (__binary_pred(*__first, *__next))
      __that.erase(__next);
    else
      __first = __next;
    __next = __first;
  }
}

template <class _Tp, class _Alloc, class _StrictWeakOrdering>
void _S_merge(list<_Tp, _Alloc>& __that, list<_Tp, _Alloc>& __x,
              _StrictWeakOrdering __comp) {
  typedef typename list<_Tp, _Alloc>::iterator _Literator;
  _Literator __first1 = __that.begin();
  _Literator __last1 = __that.end();
  _Literator __first2 = __x.begin();
  _Literator __last2 = __x.end();
  while (__first1 != __last1 && __first2 != __last2)
    if (__comp(*__first2, *__first1)) {
      _Literator __next = __first2;
      _List_global_inst::_Transfer(__first1._M_node, __first2._M_node, (++__next)._M_node);
      __first2 = __next;
    }
    else
      ++__first1;
  if (__first2 != __last2) _List_global_inst::_Transfer(__last1._M_node, __first2._M_node, __last2._M_node);
}

template <class _Tp, class _Alloc, class _StrictWeakOrdering>
void _S_sort(list<_Tp, _Alloc>& __that, _StrictWeakOrdering __comp) {
  // Do nothing if the list has length 0 or 1.
  if (__that._M_node._M_data->_M_next != __that._M_node._M_data &&
      (__that._M_node._M_data->_M_next)->_M_next != __that._M_node._M_data) {
    typedef list<_Tp, _Alloc> _List;
    _List __carry(__that.get_allocator());

    // Create an array of 64 '_List' objects.  Since we cannot pass
    // constructor arguments to array element constructors, we instead use
    // an array of raw memory objects ('bsls_ObjectBuffer') and initialize
    // them with the desired allocator as a separate step.  After
    // initialization, we ensure that destructors are called by creating an
    // auto-destructor object.
    BloombergLP::bsls_ObjectBuffer<_List> __counterBuffers[64];
    _List* __counter = &__counterBuffers[0].object();
    BloombergLP::bslalg_ArrayPrimitives::uninitializedFillN(
        __counter, 64, __carry,
        __that._M_node.bslmaAllocator());
    BloombergLP::bslma_AutoDestructor<_List> __counterGuard(__counter, 64);
    int __fill = 0;
    while (!__that.empty()) {
      __carry.splice(__carry.begin(), __that, __that.begin());
      int __i = 0;
      while(__i < __fill && !__counter[__i].empty()) {
        _S_merge(__counter[__i], __carry, __comp);
        __carry.swap(__counter[__i++]);
      }
      __carry.swap(__counter[__i]);
      if (__i == __fill) ++__fill;
    }

    for (int __i = 1; __i < __fill; ++__i)
      _S_merge(__counter[__i], __counter[__i-1], __comp);
    __that.swap(__counter[__fill-1]);
  }
}

# undef  list
# undef  size_type

}  // close namespace bsl

#endif /* INCLUDED_BSLSTP_LIST */

// Local Variables:
// mode:C++
// End:
