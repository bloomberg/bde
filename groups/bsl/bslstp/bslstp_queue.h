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

// Contents originally from stl/_queue.h

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

#ifndef INCLUDED_BSLSTP_QUEUE
#define INCLUDED_BSLSTP_QUEUE

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_DEQUE
#include <bslstl_deque.h>
#endif

#ifndef INCLUDED_BSLSTL_VECTOR
#include <bslstl_vector.h>
#endif

#ifndef INCLUDED_BSLALG_CONSTRUCTORPROXY
#include <bslalg_constructorproxy.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_EXCEPTIONUTIL
#include <bsls_exceptionutil.h>
#endif

#ifndef INCLUDED_ALGORITHM
#include <algorithm>
#define INCLUDED_ALGORITHM
#endif

#ifndef INCLUDED_FUNCTIONAL
#include <functional>
#define INCLUDED_FUNCTIONAL
#endif

// bslalg::ConstructorProxy needs an allocator, even if default.
#define _STLP_BSLMA_DFLT        (BloombergLP::bslma::Allocator*)0
#define _STLP_ADD_BSLMA_DFLT  , (BloombergLP::bslma::Allocator*)0

//_STLP_BEGIN_NAMESPACE
namespace bsl {

template <class _Tp, class _Sequence = deque<_Tp> >
class queue {
  typedef queue<_Tp, _Sequence> _Self;
public:
  typedef typename _Sequence::value_type      value_type;
  typedef typename _Sequence::size_type       size_type;
  typedef          _Sequence                  container_type;

  typedef typename _Sequence::reference       reference;
  typedef typename _Sequence::const_reference const_reference;

  typedef typename _Sequence::allocator_type  allocator_type;

protected:
  BloombergLP::bslalg::ConstructorProxy<_Sequence> _M_s;

  _Sequence& _Get_s() { return _M_s.object(); }
  const _Sequence& _Get_s() const { return _M_s.object(); }

public:
  BSLALG_DECLARE_NESTED_TRAITS(queue,
                               BloombergLP::bslalg_TypeTraits<_Sequence>);

  queue() : _M_s(_STLP_BSLMA_DFLT) {}
  explicit queue(const _Sequence& __c) : _M_s(__c) {}
  queue(const queue& original)
    : _M_s(original._M_s _STLP_ADD_BSLMA_DFLT)
    {}
  queue& operator=(const queue& original)
    { _M_s.object() = original._M_s.object(); return *this; }

  // Constructors with allocator.
  explicit queue(const allocator_type& __alloc)
    : _M_s(__alloc.mechanism())
    {}
  queue(const _Sequence& __s, const allocator_type& __alloc)
    : _M_s(__s, __alloc.mechanism())
    {}
  queue(const queue& original, const allocator_type& __alloc)
    : _M_s(original._M_s, __alloc.mechanism())
    {}

  /*explicit queue(__full_move_source<_Self> src)
    : _M_s(_FullMoveSource<_Sequence>(src.get()._M_s)) {
  }*/

  bool empty() const { return _Get_s().empty(); }
  size_type size() const { return _Get_s().size(); }
  reference front() { return _Get_s().front(); }
  const_reference front() const { return _Get_s().front(); }
  reference back() { return _Get_s().back(); }
  const_reference back() const { return _Get_s().back(); }
  void push(const value_type& __x) { _Get_s().push_back(__x); }
  void pop() { _Get_s().pop_front(); }
};

//# ifndef _STLP_QUEUE_ARGS
//#  define _STLP_QUEUE_ARGS _Tp, _Sequence
//#  define _STLP_QUEUE_HEADER_ARGS class _Tp, class _Sequence
//# else
//#  define _STLP_QUEUE_HEADER_ARGS class _Tp
//# endif

template < class _Tp, class _Sequence >
inline
bool operator==(const queue<_Tp, _Sequence>& __x,
                const queue<_Tp, _Sequence>& __y)
{
  return __x._Get_s() == __y._Get_s();
}

template < class _Tp, class _Sequence >
inline
bool operator<(const queue<_Tp, _Sequence>& __x,
               const queue<_Tp, _Sequence>& __y)
{
  return __x._Get_s() < __y._Get_s();
}

template < class _Tp, class _Sequence >
inline
bool operator!=(const queue<_Tp, _Sequence>& __x,
                const queue<_Tp, _Sequence>& __y)
{
    return !(__x == __y);
}

template < class _Tp, class _Sequence >
inline
bool operator>(const queue<_Tp, _Sequence>& __x,
               const queue<_Tp, _Sequence>& __y)
{
    return __y < __x;
}

template < class _Tp, class _Sequence >
inline
bool operator<=(const queue<_Tp, _Sequence>& __x,
                const queue<_Tp, _Sequence>& __y)
{
    return !(__y < __x);
}

template < class _Tp, class _Sequence >
inline
bool operator>=(const queue<_Tp, _Sequence>& __x,
                const queue<_Tp, _Sequence>& __y) { return !(__x < __y);}


template <class _Sequence, class _Compare>
struct priority_queue_TypeTraits
  : BloombergLP::bslalg_TypeTraits<_Sequence>
  , BloombergLP::bslalg_TypeTraits<_Compare>
{
};

template <class _Tp, class _Sequence = vector<_Tp>,
          class _Compare = std::less<typename _Sequence::value_type> >
class  priority_queue
{
  typedef priority_queue<_Tp, _Sequence, _Compare> _Self;
public:
  typedef typename _Sequence::value_type      value_type;
  typedef typename _Sequence::size_type       size_type;
  typedef          _Sequence                  container_type;

  typedef typename _Sequence::reference       reference;
  typedef typename _Sequence::const_reference const_reference;

  typedef typename _Sequence::allocator_type  allocator_type;

protected:
  BloombergLP::bslalg::ConstructorProxy<_Sequence> _M_s;
  BloombergLP::bslalg::ConstructorProxy<_Compare>  comp;

  _Sequence& _Get_s() { return _M_s.object(); }
  const _Sequence& _Get_s() const { return _M_s.object(); }
  _Compare& _comp() { return comp.object(); }
  const _Compare& _comp() const { return comp.object(); }

  typedef priority_queue_TypeTraits<_Sequence,_Compare> _Traits;
public:
  BSLALG_DECLARE_NESTED_TRAITS(_Self, _Traits);

public:
  priority_queue() : _M_s(_STLP_BSLMA_DFLT), comp(_STLP_BSLMA_DFLT) {}
  explicit priority_queue(const _Compare& __x)
    : _M_s(_STLP_BSLMA_DFLT), comp(__x _STLP_ADD_BSLMA_DFLT) {}
  priority_queue(const _Compare& __x, const _Sequence& __s)
    : _M_s(__s _STLP_ADD_BSLMA_DFLT), comp(__x _STLP_ADD_BSLMA_DFLT)
    { std::make_heap(_Get_s().begin(), _Get_s().end(), _comp()); }
  priority_queue(const priority_queue& original)
    : _M_s(original._M_s _STLP_ADD_BSLMA_DFLT), comp(original.comp _STLP_ADD_BSLMA_DFLT)
    { std::make_heap(_Get_s().begin(), _Get_s().end(), _comp()); }
  priority_queue& operator=(const priority_queue& original)
    { _M_s.object() = original._M_s.object();
      comp.object() = original.comp.object();
      std::make_heap(_Get_s().begin(), _Get_s().end(), _comp());
      return *this; }

//  explicit priority_queue(__partial_move_source<_Self> src)
//    : _M_s(_AsPartialMoveSource(src.get()._Get_s()) _STLP_ADD_BSLMA_DFLT)
//    , comp(_AsPartialMoveSource(src.get()._comp()) _STLP_ADD_BSLMA_DFLT)
//    {}

  // Constructors with allocator.
  explicit priority_queue(const allocator_type& alloc)
    : _M_s(alloc.mechanism()), comp(alloc.mechanism())
    {}
  explicit priority_queue(const _Compare& __x, const allocator_type& alloc)
    :  _M_s(alloc.mechanism()), comp(__x, alloc.mechanism())
    {}
  priority_queue(const _Compare& __x, const _Sequence& __s,
                 const allocator_type& alloc)
    : _M_s(__s, alloc.mechanism()), comp(__x, alloc.mechanism())
    { std::make_heap(_Get_s().begin(), _Get_s().end(), _comp()); }
  priority_queue(const priority_queue& original, const allocator_type& alloc)
    :  _M_s(original._M_s, alloc.mechanism()), comp(original.comp, alloc.mechanism())
    { make_heap(_Get_s().begin(), _Get_s().end(), _comp()); }



  template <class _InputIterator>
  priority_queue(_InputIterator __first, _InputIterator __last)
    : _M_s(__first, __last _STLP_ADD_BSLMA_DFLT)
    , comp(_Compare() _STLP_ADD_BSLMA_DFLT)
    { std::make_heap(_Get_s().begin(), _Get_s().end(), _comp()); }

  template <class _InputIterator>
  priority_queue(_InputIterator __first,
                 _InputIterator __last, const _Compare& __x)
    : _M_s(__first, __last _STLP_ADD_BSLMA_DFLT)
    , comp(__x _STLP_ADD_BSLMA_DFLT)
    { std::make_heap(_Get_s().begin(), _Get_s().end(), _comp()); }

  template <class _InputIterator>
  priority_queue(_InputIterator __first, _InputIterator __last,
                 const _Compare& __x, const _Sequence& __s)
  : _M_s(__s _STLP_ADD_BSLMA_DFLT), comp(__x _STLP_ADD_BSLMA_DFLT)
  {
    _Get_s().insert(_Get_s().end(), __first, __last);
    std::make_heap(_Get_s().begin(), _Get_s().end(), _comp());
  }

  // Constructor templates with allocator.
  template <class _InputIterator>
  priority_queue(_InputIterator __first, _InputIterator __last,
                 const allocator_type& alloc)
    : _M_s(__first, __last, alloc.mechanism()), comp(alloc.mechanism())
    { std::make_heap(_Get_s().begin(), _Get_s().end(), _comp()); }

  template <class _InputIterator>
  priority_queue(_InputIterator __first,
                 _InputIterator __last, const _Compare& __x,
          const allocator_type& alloc)
    : _M_s(__first, __last, alloc.mechanism()), comp(__x, alloc.mechanism())
    { std::make_heap(_Get_s().begin(), _Get_s().end(), _comp()); }

  template <class _InputIterator>
  priority_queue(_InputIterator __first, _InputIterator __last,
                 const _Compare& __x, const _Sequence& __s,
                 const allocator_type& alloc)
    : _M_s(__s.alloc.mechanism()), comp(__x, alloc.mechanism())
    {
      _Get_s().insert(_Get_s().end(), __first, __last);
      std::make_heap(_Get_s().begin(), _Get_s().end(), _comp());
    }


  bool empty() const { return _Get_s().empty(); }
  size_type size() const { return _Get_s().size(); }
  const_reference top() const { return _Get_s().front(); }
  void push(const value_type& __x) {
    BSLS_TRY {
      _Get_s().push_back(__x);
      std::push_heap(_Get_s().begin(), _Get_s().end(), _comp());
    }
    BSLS_CATCH(...) {
        _Get_s().clear();
        BSLS_RETHROW;
    }
  }
  void pop() {
    BSLS_TRY {
      std::pop_heap(_Get_s().begin(), _Get_s().end(), _comp());
      _Get_s().pop_back();
    }
    BSLS_CATCH(...) {
        _Get_s().clear();
        BSLS_RETHROW;
    }
  }
};

}  // close namespace bsl

#  undef _STLP_BSLMA_DFLT
#  undef _STLP_ADD_BSLMA_DFLT
//#  undef _STLP_QUEUE_ARGS
//#  undef _STLP_QUEUE_HEADER_ARGS

#endif /* INCLUDED_BSLSTP_QUEUE */

// Local Variables:
// mode:C++
// End:
