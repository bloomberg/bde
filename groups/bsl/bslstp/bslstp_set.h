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

// Contents originally from stl/_set.h

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

#ifndef INCLUDED_BSLSTP_SET
#define INCLUDED_BSLSTP_SET

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLSTL_EXFUNCTIONAL
#include <bslstl_exfunctional.h>
#endif

#ifndef INCLUDED_BSLSTP_TREE
#include <bslstp_tree.h>
#endif

#ifndef INCLUDED_ALGORITHM
#include <algorithm>
#define INCLUDED_ALGORITHM
#endif

#ifndef INCLUDED_FUNCTIONAL
#include <functional>
#define INCLUDED_FUNCTIONAL
#endif

//#define set __WORKAROUND_RENAME(set)
//#define multiset __WORKAROUND_RENAME(multiset)

namespace bsl {


template <class _Key, class _Compare = std::less<_Key>,
          class _Alloc = bsl::allocator<_Key> >
class set {

public:
// typedefs:
  typedef _Key     key_type;
  typedef _Key     value_type;
  typedef _Compare key_compare;
  typedef _Compare value_compare;
private:
  typedef _Rb_tree<key_type, value_type,
    identity<value_type>, key_compare, _Alloc> _Rep_type;
  typedef set<_Key, _Compare, _Alloc> _Self;

public:
  typedef typename _Rep_type::pointer pointer;
  typedef typename _Rep_type::const_pointer const_pointer;
  typedef typename _Rep_type::reference reference;
  typedef typename _Rep_type::const_reference const_reference;
  typedef typename _Rep_type::const_iterator const_iterator;
  typedef const_iterator iterator;
  typedef typename _Rep_type::const_reverse_iterator reverse_iterator;
  typedef typename _Rep_type::const_reverse_iterator const_reverse_iterator;
  typedef typename _Rep_type::size_type size_type;
  typedef typename _Rep_type::difference_type difference_type;
  typedef typename _Rep_type::allocator_type allocator_type;

  BSLALG_DECLARE_NESTED_TRAITS(set, BloombergLP::bslalg_TypeTraits<_Rep_type>);

private:
  _Rep_type _M_t;  // red-black tree representing set
public:

  // allocation/deallocation

  set() : _M_t(_Compare(), allocator_type()) {}

  // Default-construct with alternative allocator.
  explicit  set(const allocator_type& __a)
    : _M_t(_Compare(), __a) {}

  explicit set(const _Compare& __comp,
               const allocator_type& __a = allocator_type())
    : _M_t(__comp, __a) {}

  template <class _InputIterator>
  set(_InputIterator __first, _InputIterator __last)
    : _M_t(_Compare(), allocator_type())
    { _M_t.insert_unique(__first, __last); }

  template <class _InputIterator>
  set(_InputIterator __first, _InputIterator __last, const _Compare& __comp,
      const allocator_type& __a = allocator_type())
    : _M_t(__comp, __a) { _M_t.insert_unique(__first, __last); }

  set(const _Self& __x) : _M_t(__x._M_t) {}

  // Copy-construct with alternative allocator.
  set(const _Self& __x, const allocator_type& __a) : _M_t(__x._M_t, __a) {}

  /*explicit set(__full_move_source<_Self> src)
    : _M_t(_FullMoveSource<_Rep_type>(src.get()._M_t)) {
  }*/

//  explicit set(__partial_move_source<_Self> src)
//    : _M_t(_AsPartialMoveSource(src.get()._M_t)) {
//  }

  _Self& operator=(const _Self& __x) {
    _M_t = __x._M_t;
    return *this;
  }

  // accessors:

  key_compare key_comp() const { return _M_t.key_comp(); }
  value_compare value_comp() const { return _M_t.key_comp(); }
  allocator_type get_allocator() const { return _M_t.get_allocator(); }

  iterator begin() const { return _M_t.begin(); }
  iterator end() const { return _M_t.end(); }
  reverse_iterator rbegin() const { return _M_t.rbegin(); }
  reverse_iterator rend() const { return _M_t.rend(); }
  bool empty() const { return _M_t.empty(); }
  size_type size() const { return _M_t.size(); }
  size_type max_size() const { return _M_t.max_size(); }
  void swap(_Self& __x) { _M_t.swap(__x._M_t); }

  // insert/erase
  pair<iterator,bool> insert(const value_type& __x) {
    typedef typename _Rep_type::iterator _Rep_iterator;
    pair<_Rep_iterator, bool> __p = _M_t.insert_unique(__x);
    return pair<iterator, bool>(reinterpret_cast<const iterator&>(__p.first), __p.second);
  }
  iterator insert(iterator __position, const value_type& __x) {
    typedef typename _Rep_type::iterator _Rep_iterator;
    return _M_t.insert_unique((_Rep_iterator&)__position, __x);
  }
  template <class _InputIterator>
  void insert(_InputIterator __first, _InputIterator __last) {
    _M_t.insert_unique(__first, __last);
  }
  void erase(iterator __position) {
    typedef typename _Rep_type::iterator _Rep_iterator;
    _M_t.erase((_Rep_iterator&)__position);
  }
  size_type erase(const key_type& __x) {
    return _M_t.erase(__x);
  }
  void erase(iterator __first, iterator __last) {
    typedef typename _Rep_type::iterator _Rep_iterator;
    _M_t.erase((_Rep_iterator&)__first, (_Rep_iterator&)__last);
  }
  void clear() { _M_t.clear(); }

  // set operations:
  template <class _KT>
  iterator find(const _KT& __x) const { return _M_t.find(__x); }
  size_type count(const key_type& __x) const {
    return _M_t.find(__x) == _M_t.end() ? 0 : 1 ;
  }
  iterator lower_bound(const key_type& __x) const {
    return _M_t.lower_bound(__x);
  }
  iterator upper_bound(const key_type& __x) const {
    return _M_t.upper_bound(__x);
  }
  pair<iterator,iterator> equal_range(const key_type& __x) const {
    return _M_t.equal_range(__x);
  }
};

template <class _Key, class _Compare, class _Alloc>
inline
void swap(set<_Key, _Compare, _Alloc>& lhs,
          set<_Key, _Compare, _Alloc>& rhs)
{
    lhs.swap(rhs);
}

template <class _Key, class _Compare = std::less<_Key>,
          class _Alloc = bsl::allocator<_Key> >
class multiset
{
  typedef multiset<_Key, _Compare, _Alloc> _Self;
public:
  // typedefs:

  typedef _Key     key_type;
  typedef _Key     value_type;
  typedef _Compare key_compare;
  typedef _Compare value_compare;
private:
  typedef _Rb_tree<key_type, value_type,
                  identity<value_type>, key_compare, _Alloc> _Rep_type;
public:
  typedef typename _Rep_type::pointer pointer;
  typedef typename _Rep_type::const_pointer const_pointer;
  typedef typename _Rep_type::reference reference;
  typedef typename _Rep_type::const_reference const_reference;
  typedef typename _Rep_type::const_iterator const_iterator;
  typedef const_iterator iterator;
  typedef typename _Rep_type::const_reverse_iterator reverse_iterator;
  typedef typename _Rep_type::const_reverse_iterator const_reverse_iterator;
  typedef typename _Rep_type::size_type size_type;
  typedef typename _Rep_type::difference_type difference_type;
  typedef typename _Rep_type::allocator_type allocator_type;

  BSLALG_DECLARE_NESTED_TRAITS(multiset,
                               BloombergLP::bslalg_TypeTraits<_Rep_type>);

private:
  _Rep_type _M_t;  // red-black tree representing multiset
public:
  // allocation/deallocation

  multiset() : _M_t(_Compare(), allocator_type()) {}

  // Default-construct with alternative allocator.
  explicit multiset(const allocator_type& __a)
    : _M_t(_Compare(), __a) {}

  explicit multiset(const _Compare& __comp,
                    const allocator_type& __a = allocator_type())
    : _M_t(__comp, __a) {}


  template <class _InputIterator>
  multiset(_InputIterator __first, _InputIterator __last)
    : _M_t(_Compare(), allocator_type())
    { _M_t.insert_equal(__first, __last); }

  template <class _InputIterator>
  multiset(_InputIterator __first, _InputIterator __last,
           const _Compare& __comp,
           const allocator_type& __a = allocator_type())
    : _M_t(__comp, __a) { _M_t.insert_equal(__first, __last); }


  multiset(const _Self& __x) : _M_t(__x._M_t) {}

  // Copy-construct with alternative allocator.
  multiset(const _Self& __x, const allocator_type& __a) : _M_t(__x._M_t, __a){}

  _Self& operator=(const _Self& __x) {
    _M_t = __x._M_t;
    return *this;
  }

  /*explicit multiset(__full_move_source<_Self> src)
    : _M_t(_FullMoveSource<_Rep_type>(src.get()._M_t)) {
  }*/

//  explicit multiset(__partial_move_source<_Self> src)
//    : _M_t(_AsPartialMoveSource(src.get()._M_t)) {
//  }

  // accessors:

  key_compare key_comp() const { return _M_t.key_comp(); }
  value_compare value_comp() const { return _M_t.key_comp(); }
  allocator_type get_allocator() const { return _M_t.get_allocator(); }

  iterator begin() const { return _M_t.begin(); }
  iterator end() const { return _M_t.end(); }
  reverse_iterator rbegin() const { return _M_t.rbegin(); }
  reverse_iterator rend() const { return _M_t.rend(); }
  bool empty() const { return _M_t.empty(); }
  size_type size() const { return _M_t.size(); }
  size_type max_size() const { return _M_t.max_size(); }
  void swap(_Self& __x) { _M_t.swap(__x._M_t); }

  // insert/erase
  iterator insert(const value_type& __x) {
    return _M_t.insert_equal(__x);
  }
  iterator insert(iterator __position, const value_type& __x) {
    typedef typename _Rep_type::iterator _Rep_iterator;
    return _M_t.insert_equal((_Rep_iterator&)__position, __x);
  }

  template <class _InputIterator>
  void insert(_InputIterator __first, _InputIterator __last) {
    _M_t.insert_equal(__first, __last);
  }
  void erase(iterator __position) {
    typedef typename _Rep_type::iterator _Rep_iterator;
    _M_t.erase((_Rep_iterator&)__position);
  }
  size_type erase(const key_type& __x) {
    return _M_t.erase(__x);
  }
  void erase(iterator __first, iterator __last) {
    typedef typename _Rep_type::iterator _Rep_iterator;
    _M_t.erase((_Rep_iterator&)__first, (_Rep_iterator&)__last);
  }
  void clear() { _M_t.clear(); }

  // multiset operations:

  template <class _KT>
  iterator find(const _KT& __x) const { return _M_t.find(__x); }
  size_type count(const key_type& __x) const { return _M_t.count(__x); }
  iterator lower_bound(const key_type& __x) const {
    return _M_t.lower_bound(__x);
  }
  iterator upper_bound(const key_type& __x) const {
    return _M_t.upper_bound(__x);
  }
  pair<iterator,iterator> equal_range(const key_type& __x) const {
    return _M_t.equal_range(__x);
  }
};

template <class _Key, class _Compare, class _Alloc>
inline
void swap(multiset<_Key, _Compare, _Alloc>& lhs,
          multiset<_Key, _Compare, _Alloc>& rhs)
{
    lhs.swap(rhs);
}

template <class _Key, class _Compare, class _Alloc>
inline
bool operator==(const set<_Key,_Compare,_Alloc>& __x,
                const set<_Key,_Compare,_Alloc>& __y) {
  return __x.size() == __y.size() &&
    std::equal(__x.begin(), __x.end(), __y.begin());
}

template <class _Key, class _Compare, class _Alloc>
inline
bool  operator<(const set<_Key,_Compare,_Alloc>& __x,
                const set<_Key,_Compare,_Alloc>& __y) {
  return std::lexicographical_compare(__x.begin(), __x.end(),
                                      __y.begin(), __y.end());
}

template <class _Key, class _Compare, class _Alloc>
inline
bool operator!=(const set<_Key,_Compare,_Alloc>& __x,
                const set<_Key,_Compare,_Alloc>& __y) {return !(__x == __y);}

template <class _Key, class _Compare, class _Alloc>
inline
bool operator>(const set<_Key,_Compare,_Alloc>& __x,
               const set<_Key,_Compare,_Alloc>& __y)  {return __y < __x;}

template <class _Key, class _Compare, class _Alloc>
inline
bool operator<=(const set<_Key,_Compare,_Alloc>& __x,
                const set<_Key,_Compare,_Alloc>& __y) { return !(__y < __x);}

template <class _Key, class _Compare, class _Alloc>
inline
bool operator>=(const set<_Key,_Compare,_Alloc>& __x,
                const set<_Key,_Compare,_Alloc>& __y) { return !(__x < __y);}

//template <class _Key, class _Compare, class _Alloc>
//struct __partial_move_traits<set<_Key,_Compare,_Alloc> > {
//  typedef __true_type supported;
//};
//
//template <class _Key, class _Compare, class _Alloc>
//struct __action_on_move<set<_Key,_Compare,_Alloc> > {
//  typedef __true_type swap;
//};

template <class _Key, class _Compare, class _Alloc>
inline
bool operator==(const multiset<_Key,_Compare,_Alloc>& __x,
                const multiset<_Key,_Compare,_Alloc>& __y) {
  return __x.size() == __y.size() &&
    std::equal(__x.begin(), __x.end(), __y.begin());
}

template <class _Key, class _Compare, class _Alloc>
inline
bool  operator<(const multiset<_Key,_Compare,_Alloc>& __x,
                const multiset<_Key,_Compare,_Alloc>& __y) {
  return std::lexicographical_compare(__x.begin(), __x.end(),
                                      __y.begin(), __y.end());
}

template <class _Key, class _Compare, class _Alloc>
inline
bool operator!=(const multiset<_Key,_Compare,_Alloc>& __x,
                const multiset<_Key,_Compare,_Alloc>& __y) {return !(__x == __y);}

template <class _Key, class _Compare, class _Alloc>
inline
bool operator>(const multiset<_Key,_Compare,_Alloc>& __x,
               const multiset<_Key,_Compare,_Alloc>& __y)  {return __y < __x;}

template <class _Key, class _Compare, class _Alloc>
inline
bool operator<=(const multiset<_Key,_Compare,_Alloc>& __x,
                const multiset<_Key,_Compare,_Alloc>& __y)
{
    return !(__y < __x);
}

template <class _Key, class _Compare, class _Alloc>
inline
bool operator>=(const multiset<_Key,_Compare,_Alloc>& __x,
                const multiset<_Key,_Compare,_Alloc>& __y)
{
    return !(__x < __y);
}

//template <class _Key, class _Compare, class _Alloc>
//struct __partial_move_traits<multiset<_Key,_Compare,_Alloc> > {
//  typedef __true_type supported;
//};
//
//template <class _Key, class _Compare, class _Alloc>
//struct __action_on_move<multiset<_Key,_Compare,_Alloc> > {
//  typedef __true_type swap;
//};




//# define _STLP_TEMPLATE_HEADER template <class _Key, class _Compare, class _Alloc>
//
//#  define _STLP_TEMPLATE_CONTAINER set<_Key,_Compare,_Alloc>
//#  include <bslstp_stl_relops_cont.h>
//#  undef  _STLP_TEMPLATE_CONTAINER
//
//#  define _STLP_TEMPLATE_CONTAINER multiset<_Key,_Compare,_Alloc>
//#  include <bslstp_stl_relops_cont.h>
//#  undef  _STLP_TEMPLATE_CONTAINER
//
//# undef  _STLP_TEMPLATE_HEADER

}  // close namespace bsl

// do a cleanup
//# undef set
//# undef multiset
//// provide a way to access full functionality
//# define __set__  __FULL_NAME(set)
//# define __multiset__  __FULL_NAME(multiset)

#endif /* INCLUDED_BSLSTP_SET */

// Local Variables:
// mode:C++
// End:
