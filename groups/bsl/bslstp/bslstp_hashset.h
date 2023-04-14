// bslstp_hashset.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLSTP_HASHSET
#define INCLUDED_BSLSTP_HASHSET

//@PURPOSE: Provide containers hashed by value.
//
//@INTERNAL_DEPRECATED: Do not use directly.
//
//@CLASSES:
//        bsl::hash_set: container of unique values
//   bsl::hash_multiset: container of possibly non-unique values
//
//@SEE_ALSO: bsl_unordered_set
//
//@DESCRIPTION: This component is for internal use only.
//
// Note that the functions in this component are based on STLPort's
// implementation, with copyright notice as follows:
//..
/*
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
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

// Contents originally from stl/_hash_set.h

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
//..
//
///Usage
///-----
// This component is for internal use only.

#ifdef BDE_OPENSOURCE_PUBLICATION // STP
#error "bslstp_hashset is not for publication"
#endif
#include <bslscm_version.h>

#include <bslstp_exfunctional.h>
#include <bslstp_hash.h>
#include <bslstp_hashtable.h>

#include <bslalg_typetraithasstliterators.h>

#include <bslma_usesbslmaallocator.h>

#include <functional>

namespace bsl {

template <class _Value,
          class _HashFcn =
                    typename ::BloombergLP::bslstp::HashSelector<_Value>::Type,
          class _EqualKey = typename bsl::ComparatorSelector<_Value>::Type,
          class _Alloc = bsl::allocator<_Value> >
class hash_set
{
private:
  typedef hashtable<_Value, _Value, _HashFcn,
                    ::BloombergLP::bslstp::Identity<_Value>,
                    _EqualKey, _Alloc> _Ht;
  typedef hash_set<_Value, _HashFcn, _EqualKey, _Alloc> _Self;
  typedef typename _Ht::iterator _ht_iterator;
public:
  typedef typename _Ht::key_type key_type;
  typedef typename _Ht::value_type value_type;
  typedef typename _Ht::hasher hasher;
  typedef typename _Ht::key_equal key_equal;

  typedef typename _Ht::size_type size_type;
  typedef typename _Ht::difference_type difference_type;
  typedef typename _Ht::pointer         pointer;
  typedef typename _Ht::const_pointer   const_pointer;
  typedef typename _Ht::reference       reference;
  typedef typename _Ht::const_reference const_reference;

  // SunPro bug
  typedef typename _Ht::const_iterator const_iterator;
  typedef const_iterator iterator;

  typedef typename _Ht::allocator_type allocator_type;

  hasher hash_funct() const { return _M_ht.hash_funct(); }
  key_equal key_eq() const { return _M_ht.key_eq(); }
  allocator_type get_allocator() const { return _M_ht.get_allocator(); }

private:
  _Ht _M_ht;

public:
  hash_set()
    : _M_ht(100, hasher(), key_equal(), allocator_type()) {}
  explicit hash_set(size_type __n)
    : _M_ht(__n, hasher(), key_equal(), allocator_type()) {}
  hash_set(size_type __n, const hasher& __hf)
    : _M_ht(__n, __hf, key_equal(), allocator_type()) {}

  // Constructors with alternative allocator.
  explicit hash_set(const allocator_type& __a)
    : _M_ht(100, hasher(), key_equal(), __a) {}
  hash_set(size_type __n, const allocator_type& __a)
    : _M_ht(__n, hasher(), key_equal(), __a) {}
  hash_set(size_type __n, const hasher& __hf, const allocator_type& __a)
    : _M_ht(__n, __hf, key_equal(), __a) {}

  hash_set(size_type __n, const hasher& __hf, const key_equal& __eql,
           const allocator_type& __a = allocator_type())
    : _M_ht(__n, __hf, __eql, __a) {}

  template <class _InputIterator>
  hash_set(_InputIterator __f, _InputIterator __l)
    : _M_ht(100, hasher(), key_equal(), allocator_type())
    { _M_ht.insert_unique(__f, __l); }
  template <class _InputIterator>
  hash_set(_InputIterator __f, _InputIterator __l, size_type __n)
    : _M_ht(__n, hasher(), key_equal(), allocator_type())
    { _M_ht.insert_unique(__f, __l); }
  template <class _InputIterator>
  hash_set(_InputIterator __f, _InputIterator __l, size_type __n,
           const hasher& __hf)
    : _M_ht(__n, __hf, key_equal(), allocator_type())
    { _M_ht.insert_unique(__f, __l); }

  // Constructors with alternative allocator.
  template <class _InputIterator>
  hash_set(_InputIterator __f, _InputIterator __l, const allocator_type& __a)
    : _M_ht(100, hasher(), key_equal(), __a)
    { _M_ht.insert_unique(__f, __l); }
  template <class _InputIterator>
  hash_set(_InputIterator __f, _InputIterator __l, size_type __n, const allocator_type& __a)
    : _M_ht(__n, hasher(), key_equal(), __a)
    { _M_ht.insert_unique(__f, __l); }
  template <class _InputIterator>
  hash_set(_InputIterator __f, _InputIterator __l, size_type __n,
           const hasher& __hf, const allocator_type& __a)
    : _M_ht(__n, __hf, key_equal(), __a)
    { _M_ht.insert_unique(__f, __l); }

  template <class _InputIterator>
  hash_set(_InputIterator __f, _InputIterator __l, size_type __n,
           const hasher& __hf, const key_equal& __eql,
           const allocator_type& __a = allocator_type())
    : _M_ht(__n, __hf, __eql, __a)
    { _M_ht.insert_unique(__f, __l); }

  // Copy-construct with alternative allocator.
  // NOTE: Default copy-constructor is generated by the compiler
  hash_set(const _Self& __x, const allocator_type& __a)
      : _M_ht(__x._M_ht, __a) { }

public:
  size_type size() const { return _M_ht.size(); }
  size_type max_size() const { return _M_ht.max_size(); }
  bool empty() const { return _M_ht.empty(); }
  void swap(_Self& __hs) { _M_ht.swap(__hs._M_ht); }

  iterator begin() const { return _M_ht.begin(); }
  iterator end() const { return _M_ht.end(); }

public:
  pair<iterator, bool> insert(const value_type& __obj)
    {
      pair<_ht_iterator, bool> __p = _M_ht.insert_unique(__obj);
      return pair<iterator,bool>(reinterpret_cast<const iterator&>(__p.first), __p.second);
    }
  template <class _InputIterator>
  void insert(_InputIterator __f, _InputIterator __l)
    { _M_ht.insert_unique(__f,__l); }
  pair<iterator, bool> insert_noresize(const value_type& __obj)
  {
    pair<_ht_iterator, bool> __p =
      _M_ht.insert_unique_noresize(__obj);
    return pair<iterator, bool>(__p.first, __p.second);
  }

  template <class _KT>
  iterator find(const _KT& __key) const { return _M_ht.find(__key); }
  size_type count(const key_type& __key) const { return _M_ht.count(__key); }

  pair<iterator, iterator> equal_range(const key_type& __key) const
    { return _M_ht.equal_range(__key); }

  size_type erase(const key_type& __key) {return _M_ht.erase(__key); }
  void erase(iterator __it) { _M_ht.erase(__it); }
  void erase(iterator __f, iterator __l) { _M_ht.erase(__f, __l); }
  void clear() { _M_ht.clear(); }

public:
  void resize(size_type __hint) { _M_ht.resize(__hint); }
  size_type bucket_count() const { return _M_ht.bucket_count(); }
  size_type max_bucket_count() const { return _M_ht.max_bucket_count(); }
  size_type elems_in_bucket(size_type __n) const
    { return _M_ht.elems_in_bucket(__n); }

  static bool _M_equal (const _Self& __x, const _Self& __y) {
    return _Ht::_M_equal(__x._M_ht,__y._M_ht);
  }

};

template <class _Value, class _HashFcn, class _EqualKey, class _Alloc>
inline
void swap(hash_set<_Value, _HashFcn, _EqualKey, _Alloc>& lhs,
          hash_set<_Value, _HashFcn, _EqualKey, _Alloc>& rhs)
{
    lhs.swap(rhs);
}

template <class _Value,
          class _HashFcn =
                    typename ::BloombergLP::bslstp::HashSelector<_Value>::Type,
          class _EqualKey = typename bsl::ComparatorSelector<_Value>::Type,
          class _Alloc = bsl::allocator<_Value> >
class hash_multiset
{
private:
  typedef hashtable<_Value, _Value, _HashFcn,
                    ::BloombergLP::bslstp::Identity<_Value>,
                    _EqualKey, _Alloc> _Ht;
  typedef hash_multiset<_Value, _HashFcn, _EqualKey, _Alloc> _Self;

public:
  typedef typename _Ht::key_type key_type;
  typedef typename _Ht::value_type value_type;
  typedef typename _Ht::hasher hasher;
  typedef typename _Ht::key_equal key_equal;

  typedef typename _Ht::size_type size_type;
  typedef typename _Ht::difference_type difference_type;
  typedef typename _Ht::pointer       pointer;
  typedef typename _Ht::const_pointer const_pointer;
  typedef typename _Ht::reference reference;
  typedef typename _Ht::const_reference const_reference;

  typedef typename _Ht::const_iterator const_iterator;
  // SunPro bug
  typedef const_iterator iterator;

  typedef typename _Ht::allocator_type allocator_type;

  hasher hash_funct() const { return _M_ht.hash_funct(); }
  key_equal key_eq() const { return _M_ht.key_eq(); }
  allocator_type get_allocator() const { return _M_ht.get_allocator(); }

private:
  _Ht _M_ht;

public:
  hash_multiset()
    : _M_ht(100, hasher(), key_equal(), allocator_type()) {}
  explicit hash_multiset(size_type __n)
    : _M_ht(__n, hasher(), key_equal(), allocator_type()) {}
  hash_multiset(size_type __n, const hasher& __hf)
      : _M_ht(__n, __hf, key_equal(), allocator_type()) {}

  // Constructors with alternative allocator.
  explicit hash_multiset(const allocator_type& __a)
    : _M_ht(100, hasher(), key_equal(), __a) {}
  hash_multiset(size_type __n, const allocator_type& __a)
    : _M_ht(__n, hasher(), key_equal(), __a) {}
  hash_multiset(size_type __n, const hasher& __hf, const allocator_type& __a)
    : _M_ht(__n, __hf, key_equal(), __a) {}

  hash_multiset(size_type __n, const hasher& __hf, const key_equal& __eql)
    : _M_ht(__n, __hf, __eql, allocator_type()) {}
  hash_multiset(size_type __n, const hasher& __hf, const key_equal& __eql,
                const allocator_type& __a)
    : _M_ht(__n, __hf, __eql, __a) {}

  template <class _InputIterator>
  hash_multiset(_InputIterator __f, _InputIterator __l)
    : _M_ht(100, hasher(), key_equal(), allocator_type())
    { _M_ht.insert_equal(__f, __l); }
  template <class _InputIterator>
  hash_multiset(_InputIterator __f, _InputIterator __l, size_type __n)
    : _M_ht(__n, hasher(), key_equal(), allocator_type())
    { _M_ht.insert_equal(__f, __l); }
  template <class _InputIterator>
  hash_multiset(_InputIterator __f, _InputIterator __l, size_type __n,
                const hasher& __hf)
    : _M_ht(__n, __hf, key_equal(), allocator_type())
    { _M_ht.insert_equal(__f, __l); }

  // Constructors with alternative allocator.
  template <class _InputIterator>
  hash_multiset(_InputIterator __f, _InputIterator __l,
                const allocator_type& __a)
    : _M_ht(100, hasher(), key_equal(), __a)
    { _M_ht.insert_equal(__f, __l); }
  template <class _InputIterator>
  hash_multiset(_InputIterator __f, _InputIterator __l, size_type __n,
                const allocator_type& __a)
    : _M_ht(__n, hasher(), key_equal(), __a)
    { _M_ht.insert_equal(__f, __l); }
  template <class _InputIterator>
  hash_multiset(_InputIterator __f, _InputIterator __l, size_type __n,
                const hasher& __hf, const allocator_type& __a)
    : _M_ht(__n, __hf, key_equal(), __a)
    { _M_ht.insert_equal(__f, __l); }

  template <class _InputIterator>
  hash_multiset(_InputIterator __f, _InputIterator __l, size_type __n,
                const hasher& __hf, const key_equal& __eql,
                const allocator_type& __a = allocator_type())
    : _M_ht(__n, __hf, __eql, __a)
    { _M_ht.insert_equal(__f, __l); }

  // Copy-construct with alternative allocator.
  // NOTE: Default copy-constructor is generated by the compiler
  hash_multiset(const _Self& __x, const allocator_type& __a)
      : _M_ht(__x._M_ht, __a) { }

public:
  size_type size() const { return _M_ht.size(); }
  size_type max_size() const { return _M_ht.max_size(); }
  bool empty() const { return _M_ht.empty(); }
  void swap(_Self& hs) { _M_ht.swap(hs._M_ht); }

  iterator begin() const { return _M_ht.begin(); }
  iterator end() const { return _M_ht.end(); }

public:
  iterator insert(const value_type& __obj)
    { return _M_ht.insert_equal(__obj); }
  template <class _InputIterator>
  void insert(_InputIterator __f, _InputIterator __l)
    { _M_ht.insert_equal(__f,__l); }
  iterator insert_noresize(const value_type& __obj)
    { return _M_ht.insert_equal_noresize(__obj); }

  template <class _KT>
  iterator find(const _KT& __key) const { return _M_ht.find(__key); }

  size_type count(const key_type& __key) const { return _M_ht.count(__key); }

  pair<iterator, iterator> equal_range(const key_type& __key) const
    { return _M_ht.equal_range(__key); }

  size_type erase(const key_type& __key) {return _M_ht.erase(__key); }
  void erase(iterator __it) { _M_ht.erase(__it); }
  void erase(iterator __f, iterator __l) { _M_ht.erase(__f, __l); }
  void clear() { _M_ht.clear(); }

public:
  void resize(size_type __hint) { _M_ht.resize(__hint); }
  size_type bucket_count() const { return _M_ht.bucket_count(); }
  size_type max_bucket_count() const { return _M_ht.max_bucket_count(); }
  size_type elems_in_bucket(size_type __n) const
    { return _M_ht.elems_in_bucket(__n); }
  static bool _M_equal (const _Self& __x, const _Self& __y) {
    return _Ht::_M_equal(__x._M_ht,__y._M_ht);
  }
};

template <class _Value, class _HashFcn, class _EqualKey, class _Alloc>
inline
void swap(hash_multiset<_Value, _HashFcn, _EqualKey, _Alloc>& lhs,
          hash_multiset<_Value, _HashFcn, _EqualKey, _Alloc>& rhs)
{
    lhs.swap(rhs);
}

template <class _Value, class _HashFcn, class _EqualKey, class _Alloc>
inline bool
operator==(const hash_set<_Value,_HashFcn,_EqualKey,_Alloc>& __hm1,
           const hash_set<_Value,_HashFcn,_EqualKey,_Alloc>& __hm2)
{
  return hash_set<_Value,_HashFcn,_EqualKey,_Alloc>::_M_equal(__hm1, __hm2);
}

template <class _Value, class _HashFcn, class _EqualKey, class _Alloc>
inline bool
operator!=(const hash_set<_Value,_HashFcn,_EqualKey,_Alloc>& __hm1,
           const hash_set<_Value,_HashFcn,_EqualKey,_Alloc>& __hm2) {
  return !(__hm1 == __hm2);
}

template <class _Value, class _HashFcn, class _EqualKey, class _Alloc>
inline bool
operator==(const hash_multiset<_Value,_HashFcn,_EqualKey,_Alloc>& __hm1,
           const hash_multiset<_Value,_HashFcn,_EqualKey,_Alloc>& __hm2)
{
  return hash_multiset<_Value,_HashFcn,_EqualKey,_Alloc>::_M_equal(__hm1, __hm2);
}

template <class _Value, class _HashFcn, class _EqualKey, class _Alloc>
inline bool
operator!=(const hash_multiset<_Value,_HashFcn,_EqualKey,_Alloc>& __hm1,
           const hash_multiset<_Value,_HashFcn,_EqualKey,_Alloc>& __hm2) {
  return !(__hm1 == __hm2);
}

}  // close namespace bsl

namespace std {

// Specialization of insert_iterator so that it will work for hash_set
// and hash_multiset.

template <class _Value, class _HashFcn, class _EqualKey, class _Alloc>
class insert_iterator<bsl::hash_set<_Value, _HashFcn, _EqualKey, _Alloc> > {
protected:
  typedef bsl::hash_set<_Value, _HashFcn, _EqualKey, _Alloc> _Container;
  _Container* container;
public:
  typedef _Container          container_type;
  typedef output_iterator_tag iterator_category;
  typedef void                value_type;
  typedef void                difference_type;
  typedef void                pointer;
  typedef void                reference;

  insert_iterator(_Container& __x) : container(&__x) {}
  insert_iterator(_Container& __x, typename _Container::iterator)
    : container(&__x) {}
  insert_iterator<_Container>&
  operator=(const typename _Container::value_type& __val) {
    container->insert(__val);
    return *this;
  }
  insert_iterator<_Container>& operator*() { return *this; }
  insert_iterator<_Container>& operator++() { return *this; }
  insert_iterator<_Container>& operator++(int) { return *this; }
};

template <class _Value, class _HashFcn, class _EqualKey, class _Alloc>
class insert_iterator<bsl::hash_multiset<_Value, _HashFcn, _EqualKey, _Alloc> > {
protected:
  typedef bsl::hash_multiset<_Value, _HashFcn, _EqualKey, _Alloc> _Container;
  _Container* container;
  typename _Container::iterator iter;
public:
  typedef _Container          container_type;
  typedef output_iterator_tag iterator_category;
  typedef void                value_type;
  typedef void                difference_type;
  typedef void                pointer;
  typedef void                reference;

  insert_iterator(_Container& __x) : container(&__x) {}
  insert_iterator(_Container& __x, typename _Container::iterator)
    : container(&__x) {}
  insert_iterator<_Container>&
  operator=(const typename _Container::value_type& __val) {
    container->insert(__val);
    return *this;
  }
  insert_iterator<_Container>& operator*() { return *this; }
  insert_iterator<_Container>& operator++() { return *this; }
  insert_iterator<_Container>& operator++(int) { return *this; }
};

}  // close namespace std

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// Type traits for STL *ordered* containers:
//: o An ordered container defines STL iterators.
//: o An ordered container uses 'bslma' allocators if the parameterized
//:     'ALLOCATOR' is convertible from 'bslma::Allocator*'.

namespace BloombergLP {

namespace bslalg {

template <class _Value,
          class _HashFcn,
          class _EqualKey,
          class _Alloc>
struct HasStlIterators<bsl::hash_set<_Value, _HashFcn, _EqualKey, _Alloc> >
    : bsl::true_type
{};

template <class _Value,
          class _HashFcn,
          class _EqualKey,
          class _Alloc>
struct HasStlIterators<bsl::hash_multiset<_Value, _HashFcn, _EqualKey, _Alloc> >
    : bsl::true_type
{};

}  // close namespace bslalg

namespace bslma {

template <class _Value,
          class _HashFcn,
          class _EqualKey,
          class _Alloc>
struct UsesBslmaAllocator<bsl::hash_set<_Value, _HashFcn, _EqualKey, _Alloc> >
    : bsl::is_convertible<Allocator*, _Alloc>
{};

template <class _Value,
          class _HashFcn,
          class _EqualKey,
          class _Alloc>
struct UsesBslmaAllocator<bsl::hash_multiset<_Value, _HashFcn, _EqualKey, _Alloc> >
    : bsl::is_convertible<Allocator*, _Alloc>
{};

}  // close namespace bslma

}  // close enterprise namespace

#endif /* INCLUDED_BSLSTP_HASHSET */

// Local Variables:
// mode:C++
// End:
