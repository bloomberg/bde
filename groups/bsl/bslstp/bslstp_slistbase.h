// bslstp_slistbase.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLSTP_SLISTBASE
#define INCLUDED_BSLSTP_SLISTBASE

//@PURPOSE: Provide facility for STLPort implementation.
//
//@INTERNAL_DEPRECATED: Do not use directly.
//
//@CLASSES:
//
//@SEE_ALSO: bslstp_slist
//
//@DESCRIPTION: This component is for internal use only.
//
// Note that the functions in this component are based on STLPort's
// implementation, with copyright notice as follows:
// ```
/*
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
// ```
//
// /Usage
// /-----
//  This component is for internal use only.

#ifdef BDE_OPENSOURCE_PUBLICATION // STP
#error "bslstp_slistbase is not for publication"
#endif

#include <cstddef>  // for std::size_t

namespace bsl {

struct _Slist_node_base
{
  _Slist_node_base* _M_next;
};

inline
_Slist_node_base* __slist_make_link(_Slist_node_base *__prev_node,
                                    _Slist_node_base *__new_node)
{
  __new_node->_M_next = __prev_node->_M_next;
  __prev_node->_M_next = __new_node;
  return __new_node;
}


template <class _Dummy>
class _Sl_global {
public:
  // those used to be global functions
  // moved here to reduce code bloat without templatizing _Slist_iterator_base
  static std::size_t  size(_Slist_node_base* __node);
  static _Slist_node_base*  __reverse(_Slist_node_base* __node);
  static void  __splice_after(_Slist_node_base *__pos,
                              _Slist_node_base *__before_first,
                              _Slist_node_base *__before_last);

  static void  __splice_after(_Slist_node_base* __pos, _Slist_node_base* __head);

  static _Slist_node_base*  __previous(_Slist_node_base       *__head,
                                       const _Slist_node_base *__node);
  static const _Slist_node_base*  __previous(const _Slist_node_base *__head,
                                             const _Slist_node_base *__node) {
    return _Sl_global<_Dummy>::__previous(const_cast<_Slist_node_base*>(__head), __node);
  }
};

typedef _Sl_global<bool> _Sl_global_inst;

}  // close namespace bsl

// BEGIN FORMER CONTENTS OF bslstp_slistbase.c
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

// #ifndef INCLUDED_BSLSTP_STL_SLIST_BASE
// # include <bslstp_stl_slist_base.h>
// #endif

namespace bsl {


template <class _Dummy>
_Slist_node_base*
_Sl_global<_Dummy>::__previous(_Slist_node_base       *__head,
                               const _Slist_node_base *__node)
{
  while (__head && __head->_M_next != __node)
    __head = __head->_M_next;
  return __head;
}

template <class _Dummy>
void
_Sl_global<_Dummy>::__splice_after(_Slist_node_base* __pos, _Slist_node_base* __head)
{
  _Slist_node_base* __before_last = __previous(__head, 0);
  if (__before_last != __head) {
    _Slist_node_base* __after = __pos->_M_next;
    __pos->_M_next = __head->_M_next;
    __head->_M_next = 0;
    __before_last->_M_next = __after;
  }
}

template <class _Dummy>
void
_Sl_global<_Dummy>::__splice_after(_Slist_node_base *__pos,
                                   _Slist_node_base *__before_first,
                                   _Slist_node_base *__before_last)
{
  if (__pos != __before_first && __pos != __before_last) {
    _Slist_node_base* __first = __before_first->_M_next;
    _Slist_node_base* __after = __pos->_M_next;
    __before_first->_M_next = __before_last->_M_next;
    __pos->_M_next = __first;
    __before_last->_M_next = __after;
  }
}

template <class _Dummy>
_Slist_node_base*
_Sl_global<_Dummy>::__reverse(_Slist_node_base* __node)
{
  _Slist_node_base* __result = __node;
  __node = __node->_M_next;
  __result->_M_next = 0;
  while(__node) {
    _Slist_node_base* __next = __node->_M_next;
    __node->_M_next = __result;
    __result = __node;
    __node = __next;
  }
  return __result;
}

template <class _Dummy>
std::size_t
_Sl_global<_Dummy>::size(_Slist_node_base* __node)
{
  size_t __result = 0;
  for ( ; __node != 0; __node = __node->_M_next)
    ++__result;
  return __result;
}


}  // close namespace bsl


// Local Variables:
// mode:C++
// End:
// END FORMER CONTENTS OF bslstp_slistbase.c

#endif /* INCLUDED_BSLSTP_SLISTBASE */

// Local Variables:
// mode:C++
// End:
