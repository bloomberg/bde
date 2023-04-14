// bslstp_hashmap.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLSTP_HASHMAP
#define INCLUDED_BSLSTP_HASHMAP

//@PURPOSE: Provide containers indexed by a hashed key value.
//
//@INTERNAL_DEPRECATED: Do not use directly.
//
//@CLASSES:
//        bsl::hash_map: map key to value uniquely
//   bsl::hash_multimap: map key to value(s)
//
//@SEE_ALSO: bsl_unordered_map
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_hash_map.h>' instead.
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

// Contents originally from stl/_hash_map.h

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
#error "bslstp_hashmap is not for publication"
#endif
#include <bslscm_version.h>

#include <bslstp_alloc.h>

#include <bslstp_exfunctional.h>
#include <bslstp_hash.h>
#include <bslstp_hashtable.h>
#include <bslstp_iterator.h>

#include <bslalg_typetraithasstliterators.h>

#include <bslma_usesbslmaallocator.h>

#include <functional>

namespace bsl {


template <class _Key, class _Tp,
          class _HashFcn =
                      typename ::BloombergLP::bslstp::HashSelector<_Key>::Type,
          class _EqualKey = typename bsl::ComparatorSelector<_Key>::Type,
          class _Alloc = bsl::allocator< pair < const _Key, _Tp > > >
class hash_map
{
private:
  typedef pair<const _Key, _Tp>                             _Ht_pair;
  typedef hashtable<_Ht_pair, _Key, _HashFcn,
                    select1st<_Ht_pair>, _EqualKey, _Alloc> _Ht;
  typedef hash_map<_Key, _Tp, _HashFcn, _EqualKey, _Alloc>  _Self;
public:
  typedef typename _Ht::key_type key_type;
  typedef _Tp data_type;
  typedef _Tp mapped_type;
  typedef typename _Ht::value_type _value_type;
  typedef typename _Ht::value_type value_type;
  typedef typename _Ht::hasher hasher;
  typedef typename _Ht::key_equal key_equal;

  typedef typename _Ht::size_type size_type;
  typedef typename _Ht::difference_type difference_type;
  typedef typename _Ht::pointer pointer;
  typedef typename _Ht::const_pointer const_pointer;
  typedef typename _Ht::reference reference;
  typedef typename _Ht::const_reference const_reference;

  typedef typename _Ht::iterator iterator;
  typedef typename _Ht::const_iterator const_iterator;

  typedef typename _Ht::allocator_type allocator_type;

  hasher hash_funct() const { return _M_ht.hash_funct(); }
  key_equal key_eq() const { return _M_ht.key_eq(); }
  allocator_type get_allocator() const { return _M_ht.get_allocator(); }

private:
  _Ht _M_ht;
public:
  hash_map() : _M_ht(100, hasher(), key_equal(), allocator_type()) {}
  explicit hash_map(size_type __n)
    : _M_ht(__n, hasher(), key_equal(), allocator_type()) {}
  hash_map(size_type __n, const hasher& __hf)
    : _M_ht(__n, __hf, key_equal(), allocator_type()) {}

  // Constructors with alternative allocator.
  explicit hash_map(const allocator_type& __a)
    : _M_ht(100, hasher(), key_equal(), __a) {}
  hash_map(size_type __n, const allocator_type& __a)
    : _M_ht(__n, hasher(), key_equal(), __a) {}
  hash_map(size_type __n, const hasher& __hf, const allocator_type& __a)
    : _M_ht(__n, __hf, key_equal(), __a) {}

  hash_map(size_type __n, const hasher& __hf, const key_equal& __eql,
           const allocator_type& __a = allocator_type())
    : _M_ht(__n, __hf, __eql, __a) {}

  template <class _InputIterator>
  hash_map(_InputIterator __f, _InputIterator __l)
    : _M_ht(100, hasher(), key_equal(), allocator_type())
    { _M_ht.insert_unique(__f, __l); }
  template <class _InputIterator>
  hash_map(_InputIterator __f, _InputIterator __l, size_type __n)
    : _M_ht(__n, hasher(), key_equal(), allocator_type())
    { _M_ht.insert_unique(__f, __l); }
  template <class _InputIterator>
  hash_map(_InputIterator __f, _InputIterator __l, size_type __n,
           const hasher& __hf)
    : _M_ht(__n, __hf, key_equal(), allocator_type())
    { _M_ht.insert_unique(__f, __l); }

  // Constructors with alternative allocator.
  template <class _InputIterator>
  hash_map(_InputIterator __f, _InputIterator __l, const allocator_type& __a)
    : _M_ht(100, hasher(), key_equal(), __a)
    { _M_ht.insert_unique(__f, __l); }
  template <class _InputIterator>
  hash_map(_InputIterator __f, _InputIterator __l, size_type __n,
           const allocator_type& __a)
    : _M_ht(__n, hasher(), key_equal(), __a)
    { _M_ht.insert_unique(__f, __l); }
  template <class _InputIterator>
  hash_map(_InputIterator __f, _InputIterator __l, size_type __n,
           const hasher& __hf, const allocator_type& __a)
    : _M_ht(__n, __hf, key_equal(), __a)
    { _M_ht.insert_unique(__f, __l); }

  template <class _InputIterator>
  hash_map(_InputIterator __f, _InputIterator __l, size_type __n,
           const hasher& __hf, const key_equal& __eql,
           const allocator_type& __a = allocator_type())
    : _M_ht(__n, __hf, __eql, __a)
    { _M_ht.insert_unique(__f, __l); }


  // Copy-construct with alternative allocator.
  // NOTE: Default copy-constructor is generated by the compiler
  hash_map(const _Self& __x, const allocator_type& __a)
      : _M_ht(__x._M_ht, __a) { }

public:
  size_type size() const { return _M_ht.size(); }
  size_type max_size() const { return _M_ht.max_size(); }
  bool empty() const { return _M_ht.empty(); }
  void swap(_Self& __hs) { _M_ht.swap(__hs._M_ht); }
  iterator begin() { return _M_ht.begin(); }
  iterator end() { return _M_ht.end(); }
  const_iterator begin() const { return _M_ht.begin(); }
  const_iterator end() const { return _M_ht.end(); }

public:
  pair<iterator,bool> insert(const value_type& __obj)
    { return _M_ht.insert_unique(__obj); }
  template <class _InputIterator>
  void insert(_InputIterator __f, _InputIterator __l)
    { _M_ht.insert_unique(__f,__l); }
  pair<iterator,bool> insert_noresize(const value_type& __obj)
    { return _M_ht.insert_unique_noresize(__obj); }

  iterator find(const key_type& __key) { return _M_ht.find(__key); }
  const_iterator find(const key_type& __key) const
  {
    return _M_ht.find(__key);
  }

  _Tp& operator[](const key_type& __key) {
    iterator __it = _M_ht.find(__key);
    if (__it == _M_ht.end())
    {
        // This must be two statements because some compilers (e.g. SunPro 5)
        // get confused when constructing a temporary using the default
        // constructor.  (Calls destructor too many times.)

        // MODIFIED BY ARTHUR
        //_Tp v = _STLP_DEFAULT_CONSTRUCTED(_Tp);
        return _M_ht._M_insert(_value_type(__key, _Tp())).second;     // RETURN
    }
    else
        return (*__it).second;                                        // RETURN
  }

  size_type count(const key_type& __key) const { return _M_ht.count(__key); }

  pair<iterator, iterator> equal_range(const key_type& __key)
    { return _M_ht.equal_range(__key); }
  pair<const_iterator, const_iterator>
  equal_range(const key_type& __key) const
    { return _M_ht.equal_range(__key); }

  size_type erase(const key_type& __key) {return _M_ht.erase(__key); }
  void erase(iterator __it) { _M_ht.erase(__it); }
  void erase(iterator __f, iterator __l) { _M_ht.erase(__f, __l); }
  void clear() { _M_ht.clear(); }

  void resize(size_type __hint) { _M_ht.resize(__hint); }
  size_type bucket_count() const { return _M_ht.bucket_count(); }
  size_type max_bucket_count() const { return _M_ht.max_bucket_count(); }
  size_type elems_in_bucket(size_type __n) const
    { return _M_ht.elems_in_bucket(__n); }
  static bool _M_equal (const _Self& __x, const _Self& __y) {
    return _Ht::_M_equal(__x._M_ht,__y._M_ht);
  }
};

template <class _Key, class _Tp, class _HashFcn, class _EqualKey, class _Alloc>
inline
void swap(hash_map<_Key, _Tp, _HashFcn, _EqualKey, _Alloc>& lhs,
          hash_map<_Key, _Tp, _HashFcn, _EqualKey, _Alloc>& rhs)
{
    lhs.swap(rhs);
}

template <class _Key, class _Tp,
          class _HashFcn =
                      typename ::BloombergLP::bslstp::HashSelector<_Key>::Type,
          class _EqualKey = typename bsl::ComparatorSelector<_Key>::Type,
          class _Alloc = bsl::allocator< pair < const _Key, _Tp> > >
class hash_multimap
{
private:
  typedef pair<const _Key, _Tp>                                 _Ht_pair;
  typedef hashtable<_Ht_pair, _Key, _HashFcn,
                    select1st<_Ht_pair>, _EqualKey, _Alloc>     _Ht;
  typedef hash_multimap<_Key, _Tp, _HashFcn, _EqualKey, _Alloc> _Self;
public:
  typedef typename _Ht::key_type key_type;
  typedef _Tp data_type;
  typedef _Tp mapped_type;
  typedef typename _Ht::value_type _value_type;
  typedef _value_type value_type;
  typedef typename _Ht::hasher hasher;
  typedef typename _Ht::key_equal key_equal;

  typedef typename _Ht::size_type size_type;
  typedef typename _Ht::difference_type difference_type;
  typedef typename _Ht::pointer pointer;
  typedef typename _Ht::const_pointer const_pointer;
  typedef typename _Ht::reference reference;
  typedef typename _Ht::const_reference const_reference;

  typedef typename _Ht::iterator iterator;
  typedef typename _Ht::const_iterator const_iterator;

  typedef typename _Ht::allocator_type allocator_type;

  hasher hash_funct() const { return _M_ht.hash_funct(); }
  key_equal key_eq() const { return _M_ht.key_eq(); }
  allocator_type get_allocator() const { return _M_ht.get_allocator(); }

private:
  _Ht _M_ht;
public:
  hash_multimap() : _M_ht(100, hasher(), key_equal(), allocator_type()) {}
  explicit hash_multimap(size_type __n)
    : _M_ht(__n, hasher(), key_equal(), allocator_type()) {}
  hash_multimap(size_type __n, const hasher& __hf)
    : _M_ht(__n, __hf, key_equal(), allocator_type()) {}

  // Constructors with alternative allocator.
  explicit hash_multimap(const allocator_type& __a)
    : _M_ht(100, hasher(), key_equal(), __a) {}
  hash_multimap(size_type __n, const allocator_type& __a)
    : _M_ht(__n, hasher(), key_equal(), __a) {}
  hash_multimap(size_type __n, const hasher& __hf, const allocator_type& __a)
    : _M_ht(__n, __hf, key_equal(), __a) {}

  hash_multimap(size_type __n, const hasher& __hf, const key_equal& __eql,
                const allocator_type& __a = allocator_type())
    : _M_ht(__n, __hf, __eql, __a) {}

  template <class _InputIterator>
  hash_multimap(_InputIterator __f, _InputIterator __l)
    : _M_ht(100, hasher(), key_equal(), allocator_type())
    { _M_ht.insert_equal(__f, __l); }
  template <class _InputIterator>
  hash_multimap(_InputIterator __f, _InputIterator __l, size_type __n)
    : _M_ht(__n, hasher(), key_equal(), allocator_type())
    { _M_ht.insert_equal(__f, __l); }
  template <class _InputIterator>
  hash_multimap(_InputIterator __f, _InputIterator __l, size_type __n,
                const hasher& __hf)
    : _M_ht(__n, __hf, key_equal(), allocator_type())
    { _M_ht.insert_equal(__f, __l); }

  // Constructors with alternative allocator.
  template <class _InputIterator>
  hash_multimap(_InputIterator __f, _InputIterator __l,
                const allocator_type& __a)
    : _M_ht(100, hasher(), key_equal(), __a)
    { _M_ht.insert_equal(__f, __l); }
  template <class _InputIterator>
  hash_multimap(_InputIterator __f, _InputIterator __l, size_type __n,
                const allocator_type& __a)
    : _M_ht(__n, hasher(), key_equal(), __a)
    { _M_ht.insert_equal(__f, __l); }
  template <class _InputIterator>
  hash_multimap(_InputIterator __f, _InputIterator __l, size_type __n,
                const hasher& __hf, const allocator_type& __a)
    : _M_ht(__n, __hf, key_equal(), __a)
    { _M_ht.insert_equal(__f, __l); }

  template <class _InputIterator>
  hash_multimap(_InputIterator __f, _InputIterator __l, size_type __n,
                const hasher& __hf, const key_equal& __eql,
                const allocator_type& __a = allocator_type())
    : _M_ht(__n, __hf, __eql, __a)
    { _M_ht.insert_equal(__f, __l); }


  // Copy-construct with alternative allocator.
  // NOTE: Default copy-constructor is generated by the compiler
  hash_multimap(const _Self& __x, const allocator_type& __a)
      : _M_ht(__x._M_ht, __a) { }

public:
  size_type size() const { return _M_ht.size(); }
  size_type max_size() const { return _M_ht.max_size(); }
  bool empty() const { return _M_ht.empty(); }
  void swap(_Self& __hs) { _M_ht.swap(__hs._M_ht); }

  iterator begin() { return _M_ht.begin(); }
  iterator end() { return _M_ht.end(); }
  const_iterator begin() const { return _M_ht.begin(); }
  const_iterator end() const { return _M_ht.end(); }

public:
  iterator insert(const value_type& __obj)
    { return _M_ht.insert_equal(__obj); }
  template <class _InputIterator>
  void insert(_InputIterator __f, _InputIterator __l)
    { _M_ht.insert_equal(__f,__l); }
  iterator insert_noresize(const value_type& __obj)
    { return _M_ht.insert_equal_noresize(__obj); }

  iterator find(const key_type& __key) { return _M_ht.find(__key); }
  const_iterator find(const key_type& __key) const
    { return _M_ht.find(__key); }

  size_type count(const key_type& __key) const { return _M_ht.count(__key); }

  pair<iterator, iterator> equal_range(const key_type& __key)
    { return _M_ht.equal_range(__key); }
  pair<const_iterator, const_iterator>
  equal_range(const key_type& __key) const
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

template <class _Key, class _Tp, class _HashFcn, class _EqualKey, class _Alloc>
inline
void swap(hash_multimap<_Key, _Tp, _HashFcn, _EqualKey, _Alloc>& lhs,
          hash_multimap<_Key, _Tp, _HashFcn, _EqualKey, _Alloc>& rhs)
{
    lhs.swap(rhs);
}

template <class _Key, class _Tp, class _HashFcn, class _EqlKey, class _Alloc>
inline
bool operator==(const hash_map<_Key,_Tp,_HashFcn,_EqlKey,_Alloc>& __hm1,
                const hash_map<_Key,_Tp,_HashFcn,_EqlKey,_Alloc>& __hm2)
{
  return hash_map<_Key,_Tp,_HashFcn,_EqlKey,_Alloc>::_M_equal(__hm1, __hm2);
}

template <class _Key, class _Tp, class _HashFcn, class _EqlKey, class _Alloc>
inline
bool operator!=(const hash_map<_Key,_Tp,_HashFcn,_EqlKey,_Alloc>& __hm1,
                const hash_map<_Key,_Tp,_HashFcn,_EqlKey,_Alloc>& __hm2) {
  return !(__hm1 == __hm2);
}

template <class _Key, class _Tp, class _HashFcn, class _EqlKey, class _Alloc>
inline
bool operator==(const hash_multimap<_Key,_Tp,_HashFcn,_EqlKey,_Alloc>& __hm1,
                const hash_multimap<_Key,_Tp,_HashFcn,_EqlKey,_Alloc>& __hm2)
{
  return hash_multimap<_Key,
                       _Tp,
                       _HashFcn,
                       _EqlKey,
                       _Alloc>::_M_equal(__hm1, __hm2);
}

template <class _Key, class _Tp, class _HashFcn, class _EqlKey, class _Alloc>
inline
bool operator!=(const hash_multimap<_Key,_Tp,_HashFcn,_EqlKey,_Alloc>& __hm1,
                const hash_multimap<_Key,_Tp,_HashFcn,_EqlKey,_Alloc>& __hm2)
{
  return !(__hm1 == __hm2);
}

}  // close namespace bsl

namespace std {

// Specialization of insert_iterator so that it will work for hash_map
// and hash_multimap.

template <class _Key, class _Tp, class _HashFn,  class _EqKey, class _Alloc>
class insert_iterator<
        bsl::hash_map<_Key, _Tp, _HashFn, _EqKey, _Alloc> > {
protected:
  typedef bsl::hash_map<_Key, _Tp, _HashFn, _EqKey, _Alloc> _Container;
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

template <class _Key, class _Tp, class _HashFn,  class _EqKey, class _Alloc>
class insert_iterator<bsl::hash_multimap<_Key, _Tp, _HashFn, _EqKey, _Alloc> >
{
protected:
  typedef bsl::hash_multimap<_Key, _Tp, _HashFn, _EqKey, _Alloc> _Container;
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
//:     '_Alloc' is convertible from 'bslma::Allocator*'.

namespace BloombergLP {

namespace bslalg {

template <class _Key,
          class _Tp,
          class _HashFcn,
          class _EqualKey,
          class _Alloc>
struct HasStlIterators<bsl::hash_map<_Key, _Tp, _HashFcn, _EqualKey, _Alloc> >
    : bsl::true_type
{};

template <class _Key,
          class _Tp,
          class _HashFcn,
          class _EqualKey,
          class _Alloc>
struct HasStlIterators<bsl::hash_multimap<_Key, _Tp, _HashFcn, _EqualKey, _Alloc> >
    : bsl::true_type
{};

}  // close namespace bslalg

namespace bslma {

template <class _Key,
          class _Tp,
          class _HashFcn,
          class _EqualKey,
          class _Alloc>
struct UsesBslmaAllocator<bsl::hash_map<_Key, _Tp, _HashFcn, _EqualKey, _Alloc> >
    : bsl::is_convertible<Allocator*, _Alloc>
{};

template <class _Key,
          class _Tp,
          class _HashFcn,
          class _EqualKey,
          class _Alloc>
struct UsesBslmaAllocator<bsl::hash_multimap<_Key, _Tp, _HashFcn, _EqualKey, _Alloc> >
    : bsl::is_convertible<Allocator*, _Alloc>
{};

}  // close namespace bslma

}  // close enterprise namespace

#endif /* INCLUDED_BSLSTP_HASHMAP */

// Local Variables:
// mode:C++
// End:
