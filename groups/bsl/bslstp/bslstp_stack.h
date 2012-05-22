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

// Contents originally from stl/_stack.h

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

#ifndef INCLUDED_BSLSTP_STACK
#define INCLUDED_BSLSTP_STACK

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

//# ifndef _STLP_OUTERMOST_HEADER_ID
//#  define _STLP_OUTERMOST_HEADER_ID 0x60
//#  include <bslstp_stl_prolog.h>
//# endif

#ifndef INCLUDED_BSLSTL_DEQUE
#include <bslstl_deque.h>
#endif

namespace bsl {

template <class _Tp, class _Sequence = deque<_Tp> >
class stack {

  typedef stack<_Tp, _Sequence> _Self;

public:
  typedef typename _Sequence::value_type      value_type;
  typedef typename _Sequence::size_type       size_type;
  typedef          _Sequence                  container_type;

  typedef typename _Sequence::reference       reference;
  typedef typename _Sequence::const_reference const_reference;
protected:
  _Sequence _M_s;
public:
  stack() : _M_s() {}
  explicit stack(const _Sequence& __s) : _M_s(__s) {}

  // Constructors with allocator.
  explicit stack(BloombergLP::bslma::Allocator *__alloc) : _M_s(__alloc) {}
  stack(const _Sequence& __s, BloombergLP::bslma::Allocator *__alloc)
      : _M_s(__s, __alloc) {}
  stack(const stack& original, BloombergLP::bslma::Allocator *__alloc = 0)
      : _M_s(original._M_s, __alloc) {}

  /*explicit stack(__full_move_source<_Self> src)
    : _M_s(_FullMoveSource<_Sequence>(src.get()._M_s)) {
  }*/


  bool empty() const { return _M_s.empty(); }
  size_type size() const { return _M_s.size(); }
  reference top() { return _M_s.back(); }
  const_reference top() const { return _M_s.back(); }
  void push(const value_type& __x) { _M_s.push_back(__x); }
  void pop() { _M_s.pop_back(); }
  const _Sequence& _Get_s() const { return _M_s; }
};

//# ifndef _STLP_STACK_ARGS
//#  define _STLP_STACK_ARGS _Tp, _Sequence
//#  define _STLP_STACK_HEADER_ARGS class _Tp, class _Sequence
//# else
//#  define _STLP_STACK_HEADER_ARGS class _Tp
//# endif

template < class _Tp , class _Sequence >
inline
bool operator==(const stack<_Tp, _Sequence>& __x,
                const stack<_Tp, _Sequence>& __y)
{
  return __x._Get_s() == __y._Get_s();
}

template < class _Tp , class _Sequence >
inline
bool operator<(const stack<_Tp, _Sequence>& __x,
               const stack<_Tp, _Sequence>& __y)
{
  return __x._Get_s() < __y._Get_s();
}

template < class _Tp , class _Sequence >
inline
bool operator!=(const stack<_Tp, _Sequence>& __x,
                const stack<_Tp, _Sequence>& __y) {return !(__x == __y);}

template < class _Tp , class _Sequence >
inline
bool operator>(const stack<_Tp, _Sequence>& __x,
               const stack<_Tp, _Sequence>& __y)  {return __y < __x;}

template < class _Tp , class _Sequence >
inline
bool operator<=(const stack<_Tp, _Sequence>& __x,
                const stack<_Tp, _Sequence>& __y) { return !(__y < __x);}

template < class _Tp , class _Sequence >
inline
bool operator>=(const stack<_Tp, _Sequence>& __x,
                const stack<_Tp, _Sequence>& __y) { return !(__x < __y);}

}  // close namespace bsl

//#  undef _STLP_STACK_ARGS
//#  undef _STLP_STACK_HEADER_ARGS

//# if (_STLP_OUTERMOST_HEADER_ID == 0x60)
//#  include <bslstp_stl_epilog.h>
//#  undef _STLP_OUTERMOST_HEADER_ID
//# endif

#endif /* INCLUDED_BSLSTP_STACK */

// Local Variables:
// mode:C++
// End:
