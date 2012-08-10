/*
 * Copyright (c) 1999
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

// Contents originally from stl/_sstream.h

/*
 * Copyright (c) 1999
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

// This header defines classes basic_stringbuf, basic_istringstream,
// basic_ostringstream, and basic_stringstream.  These classes
// represent streamsbufs and streams whose sources or destinations are
// C++ strings.

#ifndef INCLUDED_BSLSTP_SSTREAM
#define INCLUDED_BSLSTP_SSTREAM

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTP_ALLOC
#include <bslstp_alloc.h>
#endif

#ifndef INCLUDED_BSLSTP_IOSFWD
#include <bslstp_iosfwd.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLSTL_ALLCOATOR
#include <bslstl_allocator.h>
#endif

#ifndef INCLUDED_BSLSTL_STRING
#include <bslstl_string.h>
#endif

#ifndef INCLUDED_STREAMBUF
#include <streambuf>
#define INCLUDED_STREAMBUF
#endif

#ifndef INCLUDED_ISTREAM
#include <istream>
#define INCLUDED_ISTREAM
#endif

#ifndef INCLUDED_OSTREAM
#include <ostream>
#define INCLUDED_OSTREAM
#endif

namespace bsl {

//----------------------------------------------------------------------
// This version of basic_stringbuf relies on the internal details of
// basic_string.  It relies on the fact that, in this implementation,
// basic_string's iterators are pointers.  It also assumes (as allowed
// by the standard) that _CharT is a POD type.

// We have a very small buffer for the put area, just so that we don't
// have to use append() for every sputc.  Conceptually, the buffer
// immediately follows the end of the underlying string.  We use this
// buffer when appending to write-only streambufs, but we don't use it
// for read-write streambufs.

template <class _CharT, class _Traits, class _Alloc>
class basic_stringbuf : public ::std::basic_streambuf<_CharT, _Traits>
{
public:                         // Typedefs.
  typedef _CharT                     char_type;
  typedef typename _Traits::int_type int_type;
  typedef typename _Traits::pos_type pos_type;
  typedef typename _Traits::off_type off_type;
  typedef _Traits                    traits_type;

  typedef typename _Alloc_traits<_CharT, _Alloc>::allocator_type
                                     allocator_type;

  typedef std::basic_streambuf<_CharT, _Traits>          _Base;
  typedef basic_stringbuf<_CharT, _Traits, _Alloc>  _Self;
  typedef basic_string<_CharT, _Traits, _Alloc>     _String;

  /* TODO:
  BSLALG_DECLARE_NESTED_TRAITS(
                  basic_stringbuf,
                  BloombergLP::bslalg::PassthroughTraitBslmaAllocator<_Alloc>);
                  */

public:                         // Constructors, destructor.
  explicit basic_stringbuf(::std::ios_base::openmode __mode
                                 = ::std::ios_base::in | ::std::ios_base::out);
  explicit basic_stringbuf(const _String& __s, std::ios_base::openmode __mode
                                     = std::ios_base::in | std::ios_base::out);
  // Constructors with allocators.
  explicit basic_stringbuf(const allocator_type& __a);
  explicit basic_stringbuf(std::ios_base::openmode __mode,
                           const allocator_type&   __a);
  explicit basic_stringbuf(const _String&        __s,
                           const allocator_type& __a);
  explicit basic_stringbuf(const _String&          __s,
                           std::ios_base::openmode __mode,
                           const allocator_type&   __a);
  virtual ~basic_stringbuf();

public:                         // Get or set the string.
  _String str() const {
      if (!(_M_mode & std::ios_base::in))
          // PGH 6/19/07: Apply invariant:
          // _M_append_buffer never called for input or input-output streams.
          _M_append_buffer();
      return _M_str;
  }
  void str(const _String& __s);

protected:                      // Overridden virtual member functions.
  virtual int_type underflow();
  virtual int_type uflow();
  virtual int_type pbackfail(int_type __c);
  virtual int_type overflow(int_type __c);
  int_type pbackfail() {return pbackfail(_Traits::eof());}
  int_type overflow() {return overflow(_Traits::eof());}

  virtual std::streamsize xsputn(const char_type* __s, ::std::streamsize __n);
  virtual std::streamsize _M_xsputnc(char_type __c, ::std::streamsize __n);

  virtual _Base* setbuf(_CharT* __buf, ::std::streamsize __n);
  virtual pos_type seekoff(off_type __off, std::ios_base::seekdir __dir,
                           std::ios_base::openmode __mode
                                     = std::ios_base::in | std::ios_base::out);
  virtual pos_type seekpos(pos_type __pos, std::ios_base::openmode __mode
                                     = std::ios_base::in | std::ios_base::out);

private:                        // Helper functions.
  // Append the internal buffer to the string if necessary.
  void _M_append_buffer() const;
  void _M_set_ptrs();

private:
  std::ios_base::openmode _M_mode;
  mutable basic_string<_CharT, _Traits, _Alloc> _M_str;

  enum _JustName { _S_BufSiz = 8 };
  _CharT _M_Buf[ 8 /* _S_BufSiz */];
};

//----------------------------------------------------------------------
// Class basic_istringstream, an input stream that uses a stringbuf.

template <class _CharT, class _Traits, class _Alloc>
class basic_istringstream : public ::std::basic_istream<_CharT, _Traits>
{
public:                         // Typedefs
  typedef typename _Traits::char_type   char_type;
  typedef typename _Traits::int_type    int_type;
  typedef typename _Traits::pos_type    pos_type;
  typedef typename _Traits::off_type    off_type;
  typedef _Traits                       traits_type;

  typedef typename _Alloc_traits<_CharT, _Alloc>::allocator_type
                                        allocator_type;

  typedef std::basic_ios<_CharT, _Traits>                _Basic_ios;
  typedef std::basic_istream<_CharT, _Traits>            _Base;
  typedef basic_string<_CharT, _Traits, _Alloc>     _String;
  typedef basic_stringbuf<_CharT, _Traits, _Alloc>  _Buf;

  /* TODO:
  BSLALG_DECLARE_NESTED_TRAITS(
                  basic_istringstream,
                  BloombergLP::bslalg::PassthroughTraitBslmaAllocator<_Alloc>);
                  */

public:                         // Constructors, destructor.
  basic_istringstream(std::ios_base::openmode __mode = std::ios_base::in);
  basic_istringstream(const _String&          __str,
                      std::ios_base::openmode __mode = std::ios_base::in);
  // Constructors with allocators.
  basic_istringstream(const allocator_type& __a);
  basic_istringstream(std::ios_base::openmode __mode,
                      const allocator_type&   __a);
  basic_istringstream(const _String&        __str,
                      const allocator_type& __a);
  basic_istringstream(const _String&          __str,
                      std::ios_base::openmode __mode,
                      const allocator_type&   __a);
  ~basic_istringstream();

public:                         // Member functions

  basic_stringbuf<_CharT, _Traits, _Alloc>* rdbuf() const
    { return const_cast<_Buf*>(&_M_buf); }

  _String str() const { return _M_buf.str(); }
  void str(const _String& __s) { _M_buf.str(__s); }

private:
  basic_stringbuf<_CharT, _Traits, _Alloc> _M_buf;
};


//----------------------------------------------------------------------
// Class basic_ostringstream, an output stream that uses a stringbuf.

template <class _CharT, class _Traits, class _Alloc>
class basic_ostringstream : public ::std::basic_ostream<_CharT, _Traits>
{
public:                         // Typedefs
  typedef typename _Traits::char_type   char_type;
  typedef typename _Traits::int_type    int_type;
  typedef typename _Traits::pos_type    pos_type;
  typedef typename _Traits::off_type    off_type;
  typedef _Traits                       traits_type;

  typedef typename _Alloc_traits<_CharT, _Alloc>::allocator_type
                                        allocator_type;

  typedef std::basic_ios<_CharT, _Traits>                _Basic_ios;
  typedef std::basic_ostream<_CharT, _Traits>            _Base;
  typedef basic_string<_CharT, _Traits, _Alloc>     _String;
  typedef basic_stringbuf<_CharT, _Traits, _Alloc>  _Buf;

  /* TODO:
  BSLALG_DECLARE_NESTED_TRAITS(
                  basic_ostringstream,
                  BloombergLP::bslalg::PassthroughTraitBslmaAllocator<_Alloc>);
                  */

public:                         // Constructors, destructor.
  basic_ostringstream(std::ios_base::openmode __mode = std::ios_base::out);
  basic_ostringstream(const _String&          __str,
                      std::ios_base::openmode __mode = std::ios_base::out);
  // Constructors with allocators.
  basic_ostringstream(const allocator_type& __a);
  basic_ostringstream(std::ios_base::openmode __mode,
                      const allocator_type&   __a);
  basic_ostringstream(const _String&        __str,
                      const allocator_type& __a);
  basic_ostringstream(const _String&          __str,
                      std::ios_base::openmode __mode,
                      const allocator_type&   __a);
  ~basic_ostringstream();

public:                         // Member functions.

  basic_stringbuf<_CharT, _Traits, _Alloc>* rdbuf() const
    { return const_cast<_Buf*>(&_M_buf); }

  _String str() const { return _M_buf.str(); }
    void str(const _String& __s) { _M_buf.str(__s); } // dwa 02/07/00 - BUG STOMPER DAVE


private:
  basic_stringbuf<_CharT, _Traits, _Alloc> _M_buf;
};


//----------------------------------------------------------------------
// Class basic_stringstream, a bidirectional stream that uses a stringbuf.

template <class _CharT, class _Traits, class _Alloc>
class basic_stringstream : public std::basic_iostream<_CharT, _Traits>
{
public:                         // Typedefs
  typedef typename _Traits::char_type char_type;
  typedef typename _Traits::int_type  int_type;
  typedef typename _Traits::pos_type  pos_type;
  typedef typename _Traits::off_type  off_type;
  typedef _Traits                     traits_type;

  typedef typename _Alloc_traits<_CharT, _Alloc>::allocator_type
                                      allocator_type;

  typedef std::basic_ios<_CharT, _Traits>                 _Basic_ios;
  typedef std::basic_iostream<_CharT, _Traits>            _Base;
  typedef basic_string<_CharT, _Traits, _Alloc>      _String;
  typedef basic_stringbuf<_CharT, _Traits, _Alloc>  _Buf;

  typedef std::ios_base::openmode openmode;

  /* TODO:
  BSLALG_DECLARE_NESTED_TRAITS(
                  basic_stringstream,
                  BloombergLP::bslalg::PassthroughTraitBslmaAllocator<_Alloc>);
                  */

public:                         // Constructors, destructor.
  basic_stringstream(openmode __mod = std::ios_base::in | std::ios_base::out);
  basic_stringstream(const _String& __str,
                     openmode __mod = std::ios_base::in | std::ios_base::out);
  // Constructors with allocators.
  basic_stringstream(const allocator_type& __a);
  basic_stringstream(openmode              __mod,
                     const allocator_type& __a);
  basic_stringstream(const _String&        __str,
                     const allocator_type& __a);
  basic_stringstream(const _String&        __str,
                     openmode              __mod,
                     const allocator_type& __a);
  ~basic_stringstream();

public:                         // Member functions.

  basic_stringbuf<_CharT, _Traits, _Alloc>* rdbuf() const
    { return const_cast<_Buf*>(&_M_buf); }

  _String str() const { return _M_buf.str(); }
    void str(const _String& __s) { _M_buf.str(__s); }

private:
  basic_stringbuf<_CharT, _Traits, _Alloc> _M_buf;
};

}  // close namespace bsl


// BEGIN FORMER CONTENTS OF bslstp_stl_sstream.c
/*
 * Copyright (c) 1999
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

namespace bsl {

//----------------------------------------------------------------------
// Non-inline stringbuf member functions.

// Constructors.  Note that the base class constructor sets all of the
// get and area pointers to null.

template <class _CharT, class _Traits, class _Alloc>
basic_stringbuf<_CharT, _Traits, _Alloc>
  ::basic_stringbuf(std::ios_base::openmode __mode)
    : ::std::basic_streambuf<_CharT, _Traits>(), _M_mode(__mode), _M_str()
{}

template <class _CharT, class _Traits, class _Alloc>
basic_stringbuf<_CharT, _Traits, _Alloc>
  ::basic_stringbuf(const basic_string<_CharT, _Traits, _Alloc>& __s, std::ios_base::openmode __mode)
    : ::std::basic_streambuf<_CharT, _Traits>(), _M_mode(__mode), _M_str(__s)
{
  _M_set_ptrs();
}

template <class _CharT, class _Traits, class _Alloc>
basic_stringbuf<_CharT, _Traits, _Alloc>
  ::basic_stringbuf(const allocator_type& __a)
    : ::std::basic_streambuf<_CharT, _Traits>(), _M_mode(std::ios_base::in | std::ios_base::out), _M_str(__a)
{}

template <class _CharT, class _Traits, class _Alloc>
basic_stringbuf<_CharT, _Traits, _Alloc>
  ::basic_stringbuf(std::ios_base::openmode __mode, const allocator_type& __a)
    : ::std::basic_streambuf<_CharT, _Traits>(), _M_mode(__mode), _M_str(__a)
{}

template <class _CharT, class _Traits, class _Alloc>
basic_stringbuf<_CharT, _Traits, _Alloc>
  ::basic_stringbuf(const basic_string<_CharT, _Traits, _Alloc>& __s,
                    const allocator_type& __a)
    : ::std::basic_streambuf<_CharT, _Traits>(), _M_mode(std::ios_base::in | std::ios_base::out), _M_str(__s, __a)
{
  _M_set_ptrs();
}

template <class _CharT, class _Traits, class _Alloc>
basic_stringbuf<_CharT, _Traits, _Alloc>
  ::basic_stringbuf(const basic_string<_CharT, _Traits, _Alloc>& __s,
                    std::ios_base::openmode __mode,  const allocator_type& __a)
 : ::std::basic_streambuf<_CharT, _Traits>(), _M_mode(__mode), _M_str(__s, __a)
{
  _M_set_ptrs();
}

template <class _CharT, class _Traits, class _Alloc>
basic_stringbuf<_CharT, _Traits, _Alloc>::~basic_stringbuf()
{}

// Set the underlying string to a new value.
template <class _CharT, class _Traits, class _Alloc>
void
basic_stringbuf<_CharT, _Traits, _Alloc>::str(const basic_string<_CharT, _Traits, _Alloc>& __s)
{
  _M_str = __s;
  _M_set_ptrs();
}

template <class _CharT, class _Traits, class _Alloc>
void
basic_stringbuf<_CharT, _Traits, _Alloc>::_M_set_ptrs() {
  _CharT* __data_ptr = const_cast<_CharT*>(_M_str.data());
  _CharT* __data_end = __data_ptr + _M_str.size();
  // The initial read position is the beginning of the string.
  if (_M_mode & std::ios_base::in) {
    if (_M_mode & std::ios_base::ate)
      this->setg(__data_ptr, __data_end, __data_end);
    else
      this->setg(__data_ptr, __data_ptr, __data_end);
  }

  // The initial write position is the beginning of the string.
  if (_M_mode & std::ios_base::out) {
    if (_M_mode & (std::ios_base::app | std::ios_base::ate))
      this->setp(__data_end, __data_end);
    else
      this->setp(__data_ptr, __data_end);
  }
}

// Precondition: gptr() >= egptr().  Returns a character, if one is available.
template <class _CharT, class _Traits, class _Alloc>
typename basic_stringbuf < _CharT , _Traits , _Alloc > :: int_type
basic_stringbuf<_CharT, _Traits, _Alloc>::underflow()
{
  return this->gptr() != this->egptr()
    ? _Traits::to_int_type(*this->gptr())
    : _Traits::eof();
}

// Precondition: gptr() >= egptr().
template <class _CharT, class _Traits, class _Alloc>
typename basic_stringbuf < _CharT , _Traits , _Alloc > :: int_type
basic_stringbuf<_CharT, _Traits, _Alloc>::uflow()
{
  if (this->gptr() != this->egptr()) {
    int_type __c = _Traits::to_int_type(*this->gptr());
    this->gbump(1);
    return __c;
  }
  else
    return _Traits::eof();
}

template <class _CharT, class _Traits, class _Alloc>
typename basic_stringbuf < _CharT , _Traits , _Alloc > :: int_type
basic_stringbuf<_CharT, _Traits, _Alloc>::pbackfail(int_type __c)
{
  if (this->gptr() != this->eback()) {
    if (!_Traits::eq_int_type(__c, _Traits::eof())) {
      if (_Traits::eq(_Traits::to_char_type(__c), this->gptr()[-1])) {
        this->gbump(-1);
        return __c;
      }
      else if (_M_mode & std::ios_base::out) {
        this->gbump(-1);
        *this->gptr() = _Traits::to_char_type(__c);
        return __c;
      }
      else
        return _Traits::eof();
    }
    else {
      this->gbump(-1);
      return _Traits::not_eof(__c);
    }
  }
  else
    return _Traits::eof();
}

template <class _CharT, class _Traits, class _Alloc>
typename basic_stringbuf < _CharT , _Traits , _Alloc > :: int_type
basic_stringbuf<_CharT, _Traits, _Alloc>::overflow(int_type __c)
{
  // fbp : reverse order of "ifs" to pass Dietmar's test.
  // Apparently, standard allows overflow with eof even for read-only streams.
  if (!_Traits::eq_int_type(__c, _Traits::eof())) {
    if (_M_mode & std::ios_base::out) {
      if (!(_M_mode & std::ios_base::in)) {
        // It's a write-only streambuf, so we can use special append buffer.
        if (this->pptr() == this->epptr())
          this->_M_append_buffer();

        if (this->pptr() != this->epptr()) {
          *this->pptr() = _Traits::to_char_type(__c);
          this->pbump(1);
          return __c;
        }
        else
          return _Traits::eof();
      }

      else {
        // We're not using a special append buffer, just the string itself.
        if (this->pptr() == this->epptr()) {
          ptrdiff_t __offset = this->gptr() - this->eback();
          _M_str.push_back(_Traits::to_char_type(__c));

          _CharT* __data_ptr = const_cast<_CharT*>(_M_str.data());
          size_t __data_size = _M_str.size();

          this->setg(__data_ptr, __data_ptr + __offset, __data_ptr+__data_size);
          this->setp(__data_ptr, __data_ptr + __data_size);
          this->pbump((int)__data_size);
          return __c;
        }
        else {
          *this->pptr() = _Traits::to_char_type(__c);
          this->pbump(1);
          return __c;
        }
      }
    }
    else                          // Overflow always fails if it's read-only
      return _Traits::eof();
  }
  else                        // __c is EOF, so we don't have to do anything
    return _Traits::not_eof(__c);
}

template <class _CharT, class _Traits, class _Alloc>
::std::streamsize
basic_stringbuf<_CharT, _Traits, _Alloc>::xsputn(const char_type* __s,
                                                 ::std::streamsize __n)
{
  ::std::streamsize __nwritten = 0;

  if ((_M_mode & std::ios_base::out) && __n > 0) {
    // If the put pointer is somewhere in the middle of the string,
    // then overwrite instead of append.
    if (this->pbase() == _M_str.data() ) {
      ptrdiff_t __avail = _M_str.data() + _M_str.size() - this->pptr();
      if (__avail > __n) {
        _Traits::copy(this->pptr(), __s, __n);
        this->pbump((int)__n);
        return __n;
      }
      else {
        _Traits::copy(this->pptr(), __s, __avail);
        __nwritten += __avail;
        __n -= __avail;
        __s += __avail;
        this->setp(_M_Buf, _M_Buf + static_cast<int>(_S_BufSiz));
      }
    }

    // At this point we know we're appending.
    if (_M_mode & std::ios_base::in) {
      ptrdiff_t __get_offset = this->gptr() - this->eback();
      _M_str.append(__s, __s + __n);

      _CharT* __data_ptr = const_cast<_CharT*>(_M_str.data());
      size_t __data_size = _M_str.size();

      this->setg(__data_ptr, __data_ptr + __get_offset, __data_ptr+__data_size);
      this->setp(__data_ptr, __data_ptr + __data_size);
      this->pbump((int)__data_size);
    }
    else {
      _M_append_buffer();
      _M_str.append(__s, __s + __n);
    }

    __nwritten += __n;
  }

  return __nwritten;
}

template <class _CharT, class _Traits, class _Alloc>
::std::streamsize
basic_stringbuf<_CharT, _Traits, _Alloc>::_M_xsputnc(char_type __c,
                                                     ::std::streamsize __n)
{
  ::std::streamsize __nwritten = 0;

  if ((_M_mode & std::ios_base::out) && __n > 0) {
    // If the put pointer is somewhere in the middle of the string,
    // then overwrite instead of append.
    if (this->pbase() == _M_str.data()) {
      ptrdiff_t __avail = _M_str.data() + _M_str.size() - this->pptr();
      if (__avail > __n) {
        _Traits::assign(this->pptr(), __n, __c);
        this->pbump((int)__n);
        return __n;
      }
      else {
        _Traits::assign(this->pptr(), __avail, __c);
        __nwritten += __avail;
        __n -= __avail;
        this->setp(_M_Buf, _M_Buf + static_cast<int>(_S_BufSiz));
      }
    }

    // At this point we know we're appending.
    if (this->_M_mode & std::ios_base::in) {
      ptrdiff_t __get_offset = this->gptr() - this->eback();
      _M_str.append(__n, __c);

      _CharT* __data_ptr = const_cast<_CharT*>(_M_str.data());
      size_t __data_size = _M_str.size();

      this->setg(__data_ptr, __data_ptr + __get_offset, __data_ptr+__data_size);
      this->setp(__data_ptr, __data_ptr + __data_size);
      this->pbump((int)__data_size);

    }
    else {
      _M_append_buffer();
      _M_str.append(__n, __c);
    }

    __nwritten += __n;
  }

  return __nwritten;
}

// According to the C++ standard the effects of setbuf are implementation
// defined, except that setbuf(0, 0) has no effect.  In this implementation,
// setbuf(<anything>, n), for n > 0, calls reserve(n) on the underlying
// string.
template <class _CharT, class _Traits, class _Alloc>
::std::basic_streambuf<_CharT, _Traits>*
basic_stringbuf<_CharT, _Traits, _Alloc>::setbuf(_CharT*, ::std::streamsize __n)
{
  if (__n > 0) {
    bool __do_get_area = false;
    bool __do_put_area = false;
    ptrdiff_t __offg = 0;
    ptrdiff_t __offp = 0;

    if (this->pbase() == _M_str.data()) {
      __do_put_area = true;
      __offp = this->pptr() - this->pbase();
    }

    if (this->eback() == _M_str.data()) {
      __do_get_area = true;
      __offg = this->gptr() - this->eback();
    }

    if ((_M_mode & std::ios_base::out) && !(_M_mode & std::ios_base::in))
      _M_append_buffer();

    _M_str.reserve(__n);

    _CharT* __data_ptr = const_cast<_CharT*>(_M_str.data());
    size_t __data_size = _M_str.size();

    if (__do_get_area) {
      this->setg(__data_ptr, __data_ptr + __offg, __data_ptr+__data_size);
    }

    if (__do_put_area) {
      this->setp(__data_ptr, __data_ptr+__data_size);
      this->pbump((int)__offp);
    }
  }

  return this;
}

template <class _CharT, class _Traits, class _Alloc>
typename basic_stringbuf < _CharT , _Traits , _Alloc > :: pos_type
basic_stringbuf<_CharT, _Traits, _Alloc>::seekoff(
                                                off_type __off,
                                                std::ios_base::seekdir __dir,
                                                std::ios_base::openmode __mode)
{
  bool __imode  = false;
  bool __omode = false;

  if ((__mode & (std::ios_base::in | std::ios_base::out)) == (std::ios_base::in | std::ios_base::out) ) {
    if (__dir == std::ios_base::beg || __dir == std::ios_base::end)
      __imode = __omode = true;
  }
  else if (__mode & std::ios_base::in)
    __imode = true;
  else if (__mode & std::ios_base::out)
    __omode = true;

  if (!__imode && !__omode)
    return pos_type(off_type(-1));
  else if ((__imode  && (!(_M_mode & std::ios_base::in) || this->gptr() == 0)) ||
           (__omode && (!(_M_mode & std::ios_base::out) || this->pptr() == 0)))
    return pos_type(off_type(-1));

  if ((_M_mode & std::ios_base::out) && !(_M_mode & std::ios_base::in))
    _M_append_buffer();

  ::std::streamoff __newoff;
  switch(__dir) {
  case std::ios_base::beg:
    __newoff = 0;
    break;
  case std::ios_base::end:
    __newoff = _M_str.size();
    break;
  case std::ios_base::cur:
    __newoff = __imode ? this->gptr() - this->eback()
                    : this->pptr() - this->pbase();
    break;
  default:
    return pos_type(off_type(-1));
  }

  __off += __newoff;

  if (__imode) {
    ptrdiff_t __n = this->egptr() - this->eback();

    if (__off < 0 || __off > __n)
      return pos_type(off_type(-1));
    else
      this->setg(this->eback(), this->eback() + __off, this->eback() + __n);
  }

  if (__omode) {
    ptrdiff_t __n = this->epptr() - this->pbase();

    if (__off < 0 || __off > __n)
      return pos_type(off_type(-1));
    else {
      this->setp(this->pbase(), this->pbase() + __n);
      this->pbump((int)__off);
    }
  }

  return pos_type(__off);
}

template <class _CharT, class _Traits, class _Alloc>
typename basic_stringbuf < _CharT , _Traits , _Alloc > :: pos_type
basic_stringbuf<_CharT, _Traits, _Alloc>
  ::seekpos(pos_type __pos, std::ios_base::openmode __mode)
{
  bool __imode  = (__mode & std::ios_base::in) != 0;
  bool __omode = (__mode & std::ios_base::out) != 0;

  if ((__imode  && (!(_M_mode & std::ios_base::in) || this->gptr() == 0)) ||
      (__omode && (!(_M_mode & std::ios_base::out) || this->pptr() == 0)))
    return pos_type(off_type(-1));

  const off_type __n = __pos - pos_type(off_type(0));
  if ((_M_mode & std::ios_base::out) && !(_M_mode & std::ios_base::in))
    _M_append_buffer();

  if (__imode) {
    if (__n < 0 || __n > this->egptr() - this->eback())
      return pos_type(off_type(-1));
    this->setg(this->eback(), this->eback() + __n, this->egptr());
  }

  if (__omode) {
    if (__n < 0 || size_t(__n) > _M_str.size())
      return pos_type(off_type(-1));

    _CharT* __data_ptr = const_cast<_CharT*>(_M_str.data());
    size_t __data_size = _M_str.size();

    this->setp(__data_ptr, __data_ptr+__data_size);
    this->pbump((int)__n);
  }

  return __pos;
}

// This is declared as a const member function because it is
// called by basic_stringbuf<>::str().  Precondition: this is a
// write-only stringbuf.  We can't use an output buffer for read-
// write stringbufs.  Postcondition: pptr is reset to the beginning
// of the buffer.
template <class _CharT, class _Traits, class _Alloc>
void basic_stringbuf<_CharT, _Traits, _Alloc>::_M_append_buffer() const

{
  // Do we have a buffer to append?
  if (this->pbase() == this->_M_Buf && this->pptr() != this->_M_Buf) {
    basic_stringbuf<_CharT, _Traits, _Alloc>* __this = const_cast<_Self*>(this);
    __this->_M_str.append((const _CharT*)this->pbase(), (const _CharT*)this->pptr());
    __this->setp(const_cast<_CharT*>(_M_Buf),
                 const_cast<_CharT*>(_M_Buf + static_cast<int>(_S_BufSiz)));
  }

  // Have we run off the end of the string?
  else if (this->pptr() == this->epptr()) {
    basic_stringbuf<_CharT, _Traits, _Alloc>* __this = const_cast<_Self*>(this);
    __this->setp(const_cast<_CharT*>(_M_Buf),
                 const_cast<_CharT*>(_M_Buf + static_cast<int>(_S_BufSiz)));
  }
}

//----------------------------------------------------------------------
// Non-inline istringstream member functions.

#if defined(BSLS_PLATFORM__CMP_MSVC)

// The ios::init() method in the MSVC++ implementation contains a bug causing
// a leak of an std::locale object if called explicitly.  But passing a pointer
// to the uninitialized _M_buf into a basic_istream constructor is fine,
// because this is what the MSVC++ implementation of the stringstreams does.

template <class _CharT, class _Traits, class _Alloc>
basic_istringstream<_CharT, _Traits, _Alloc>
  ::basic_istringstream(std::ios_base::openmode __mode)
    : ::std::basic_istream<_CharT, _Traits>(&_M_buf),
      _M_buf(__mode | std::ios_base::in)
{
}

template <class _CharT, class _Traits, class _Alloc>
basic_istringstream<_CharT, _Traits, _Alloc>
  ::basic_istringstream(const _String& __str,std::ios_base::openmode __mode)
    : ::std::basic_istream<_CharT, _Traits>(&_M_buf),
      _M_buf(__str, __mode | std::ios_base::in)
{
}

template <class _CharT, class _Traits, class _Alloc>
basic_istringstream<_CharT, _Traits, _Alloc>
  ::basic_istringstream(const allocator_type& __a)
    : ::std::basic_istream<_CharT, _Traits>(&_M_buf), _M_buf(std::ios_base::in, __a)
{
}

template <class _CharT, class _Traits, class _Alloc>
basic_istringstream<_CharT, _Traits, _Alloc>
  ::basic_istringstream(std::ios_base::openmode __mode,  const allocator_type& __a)
    : ::std::basic_istream<_CharT, _Traits>(&_M_buf),
      _M_buf(__mode | std::ios_base::in, __a)
{
}

template <class _CharT, class _Traits, class _Alloc>
basic_istringstream<_CharT, _Traits, _Alloc>
  ::basic_istringstream(const _String& __str, const allocator_type& __a)
    : ::std::basic_istream<_CharT, _Traits>(&_M_buf),
      _M_buf(__str, std::ios_base::in, __a)
{
}

template <class _CharT, class _Traits, class _Alloc>
basic_istringstream<_CharT, _Traits, _Alloc>
  ::basic_istringstream(const _String& __str, std::ios_base::openmode __mode,  const allocator_type& __a)
    : ::std::basic_istream<_CharT, _Traits>(&_M_buf),
      _M_buf(__str, __mode | std::ios_base::in, __a)
{
}

#else  // BSLS_PLATFORM__CMP_MSVC

template <class _CharT, class _Traits, class _Alloc>
basic_istringstream<_CharT, _Traits, _Alloc>
  ::basic_istringstream(std::ios_base::openmode __mode)
    : ::std::basic_istream<_CharT, _Traits>(0),
      _M_buf(__mode | std::ios_base::in)
{
  this->init(&_M_buf);
}

template <class _CharT, class _Traits, class _Alloc>
basic_istringstream<_CharT, _Traits, _Alloc>
  ::basic_istringstream(const _String& __str,std::ios_base::openmode __mode)
    : ::std::basic_istream<_CharT, _Traits>(0),
      _M_buf(__str, __mode | std::ios_base::in)
{
  this->init(&_M_buf);
}

template <class _CharT, class _Traits, class _Alloc>
basic_istringstream<_CharT, _Traits, _Alloc>
  ::basic_istringstream(const allocator_type& __a)
    : ::std::basic_istream<_CharT, _Traits>(0), _M_buf(std::ios_base::in, __a)
{
  this->init(&_M_buf);
}

template <class _CharT, class _Traits, class _Alloc>
basic_istringstream<_CharT, _Traits, _Alloc>
  ::basic_istringstream(std::ios_base::openmode __mode,  const allocator_type& __a)
    : ::std::basic_istream<_CharT, _Traits>(0),
      _M_buf(__mode | std::ios_base::in, __a)
{
  this->init(&_M_buf);
}

template <class _CharT, class _Traits, class _Alloc>
basic_istringstream<_CharT, _Traits, _Alloc>
  ::basic_istringstream(const _String& __str, const allocator_type& __a)
    : ::std::basic_istream<_CharT, _Traits>(0),
      _M_buf(__str, std::ios_base::in, __a)
{
  this->init(&_M_buf);
}

template <class _CharT, class _Traits, class _Alloc>
basic_istringstream<_CharT, _Traits, _Alloc>
  ::basic_istringstream(const _String& __str, std::ios_base::openmode __mode,  const allocator_type& __a)
    : ::std::basic_istream<_CharT, _Traits>(0),
      _M_buf(__str, __mode | std::ios_base::in, __a)
{
  this->init(&_M_buf);
}

#endif  // BSLS_PLATFORM__CMP_MSVC

template <class _CharT, class _Traits, class _Alloc>
basic_istringstream<_CharT, _Traits, _Alloc>::~basic_istringstream()
{}

//----------------------------------------------------------------------
// Non-inline ostringstream member functions.

#if defined(BSLS_PLATFORM__CMP_MSVC)

// The ios::init() method in the MSVC++ implementation contains a bug causing
// a leak of an std::locale object if called explicitly.  But passing a pointer
// to the uninitialized _M_buf into a basic_istream constructor is fine,
// because this is what the MSVC++ implementation of the stringstreams does.

template <class _CharT, class _Traits, class _Alloc>
basic_ostringstream<_CharT, _Traits, _Alloc>
  ::basic_ostringstream(std::ios_base::openmode __mode)
    : ::std::basic_ostream<_CharT, _Traits>(&_M_buf),
      _M_buf(__mode | std::ios_base::out)
{
}

template <class _CharT, class _Traits, class _Alloc>
basic_ostringstream<_CharT, _Traits, _Alloc>
  ::basic_ostringstream(const _String& __str, std::ios_base::openmode __mode)
    : ::std::basic_ostream<_CharT, _Traits>(&_M_buf),
      _M_buf(__str, __mode | std::ios_base::out)
{
}

template <class _CharT, class _Traits, class _Alloc>
basic_ostringstream<_CharT, _Traits, _Alloc>
  ::basic_ostringstream(const allocator_type& __a)
    : ::std::basic_ostream<_CharT, _Traits>(&_M_buf),
      _M_buf(std::ios_base::out, __a)
{
}

template <class _CharT, class _Traits, class _Alloc>
basic_ostringstream<_CharT, _Traits, _Alloc>
  ::basic_ostringstream(std::ios_base::openmode __mode,  const allocator_type& __a)
    : ::std::basic_ostream<_CharT, _Traits>(&_M_buf),
      _M_buf(__mode | std::ios_base::out, __a)
{
}

template <class _CharT, class _Traits, class _Alloc>
basic_ostringstream<_CharT, _Traits, _Alloc>
  ::basic_ostringstream(const _String& __str, const allocator_type& __a)
    : ::std::basic_ostream<_CharT, _Traits>(&_M_buf),
      _M_buf(__str, std::ios_base::out, __a)
{
}

template <class _CharT, class _Traits, class _Alloc>
basic_ostringstream<_CharT, _Traits, _Alloc>
  ::basic_ostringstream(const _String& __str, std::ios_base::openmode __mode,  const allocator_type& __a)
    : ::std::basic_ostream<_CharT, _Traits>(&_M_buf),
      _M_buf(__str, __mode | std::ios_base::out, __a)
{
}

#else  // BSLS_PLATFORM__CMP_MSVC

template <class _CharT, class _Traits, class _Alloc>
basic_ostringstream<_CharT, _Traits, _Alloc>
  ::basic_ostringstream(std::ios_base::openmode __mode)
    : ::std::basic_ostream<_CharT, _Traits>(0),
      _M_buf(__mode | std::ios_base::out)
{
  this->init(&_M_buf);
}

template <class _CharT, class _Traits, class _Alloc>
basic_ostringstream<_CharT, _Traits, _Alloc>
  ::basic_ostringstream(const _String& __str, std::ios_base::openmode __mode)
    : ::std::basic_ostream<_CharT, _Traits>(0),
      _M_buf(__str, __mode | std::ios_base::out)
{
  this->init(&_M_buf);
}

template <class _CharT, class _Traits, class _Alloc>
basic_ostringstream<_CharT, _Traits, _Alloc>
  ::basic_ostringstream(const allocator_type& __a)
    : ::std::basic_ostream<_CharT, _Traits>(0),
      _M_buf(std::ios_base::out, __a)
{
  this->init(&_M_buf);
}

template <class _CharT, class _Traits, class _Alloc>
basic_ostringstream<_CharT, _Traits, _Alloc>
  ::basic_ostringstream(std::ios_base::openmode __mode,  const allocator_type& __a)
    : ::std::basic_ostream<_CharT, _Traits>(0),
      _M_buf(__mode | std::ios_base::out, __a)
{
  this->init(&_M_buf);
}

template <class _CharT, class _Traits, class _Alloc>
basic_ostringstream<_CharT, _Traits, _Alloc>
  ::basic_ostringstream(const _String& __str, const allocator_type& __a)
    : ::std::basic_ostream<_CharT, _Traits>(0),
      _M_buf(__str, std::ios_base::out, __a)
{
  this->init(&_M_buf);
}

template <class _CharT, class _Traits, class _Alloc>
basic_ostringstream<_CharT, _Traits, _Alloc>
  ::basic_ostringstream(const _String& __str, std::ios_base::openmode __mode,  const allocator_type& __a)
    : ::std::basic_ostream<_CharT, _Traits>(0),
      _M_buf(__str, __mode | std::ios_base::out, __a)
{
  this->init(&_M_buf);
}

#endif  // BSLS_PLATFORM__CMP_MSVC

template <class _CharT, class _Traits, class _Alloc>
basic_ostringstream<_CharT, _Traits, _Alloc>::~basic_ostringstream()
{}

//----------------------------------------------------------------------
// Non-inline stringstream member functions.

#if defined(BSLS_PLATFORM__CMP_MSVC)

// The ios::init() method in the MSVC++ implementation contains a bug causing
// a leak of an std::locale object if called explicitly.  But passing a pointer
// to the uninitialized _M_buf into a basic_istream constructor is fine,
// because this is what the MSVC++ implementation of the stringstreams does.

template <class _CharT, class _Traits, class _Alloc>
basic_stringstream<_CharT, _Traits, _Alloc>
  ::basic_stringstream(std::ios_base::openmode __mode)
    : std::basic_iostream<_CharT, _Traits>(&_M_buf), _M_buf(__mode)
{
}

template <class _CharT, class _Traits, class _Alloc>
basic_stringstream<_CharT, _Traits, _Alloc>
  ::basic_stringstream(const _String& __str, std::ios_base::openmode __mode)
    : std::basic_iostream<_CharT, _Traits>(&_M_buf), _M_buf(__str, __mode)
{
}

template <class _CharT, class _Traits, class _Alloc>
basic_stringstream<_CharT, _Traits, _Alloc>
  ::basic_stringstream(const allocator_type& __a)
    : std::basic_iostream<_CharT, _Traits>(&_M_buf),
      _M_buf(std::ios_base::in | std::ios_base::out, __a)
{
}

template <class _CharT, class _Traits, class _Alloc>
basic_stringstream<_CharT, _Traits, _Alloc>
  ::basic_stringstream(std::ios_base::openmode __mode,  const allocator_type& __a)
    : std::basic_iostream<_CharT, _Traits>(&_M_buf), _M_buf(__mode, __a)
{
}

template <class _CharT, class _Traits, class _Alloc>
basic_stringstream<_CharT, _Traits, _Alloc>
  ::basic_stringstream(const _String& __str, const allocator_type& __a)
    : std::basic_iostream<_CharT, _Traits>(&_M_buf),
      _M_buf(__str, std::ios_base::in | std::ios_base::out, __a)
{
}

template <class _CharT, class _Traits, class _Alloc>
basic_stringstream<_CharT, _Traits, _Alloc>
  ::basic_stringstream(const _String& __str, std::ios_base::openmode __mode,  const allocator_type& __a)
    : std::basic_iostream<_CharT, _Traits>(&_M_buf), _M_buf(__str, __mode, __a)
{
}

#else  // BSLS_PLATFORM__CMP_MSVC

template <class _CharT, class _Traits, class _Alloc>
basic_stringstream<_CharT, _Traits, _Alloc>
  ::basic_stringstream(std::ios_base::openmode __mode)
    : std::basic_iostream<_CharT, _Traits>(0), _M_buf(__mode)
{
   this->init(&_M_buf);
}

template <class _CharT, class _Traits, class _Alloc>
basic_stringstream<_CharT, _Traits, _Alloc>
  ::basic_stringstream(const _String& __str, std::ios_base::openmode __mode)
    : std::basic_iostream<_CharT, _Traits>(0), _M_buf(__str, __mode)
{
  this->init(&_M_buf);
}

template <class _CharT, class _Traits, class _Alloc>
basic_stringstream<_CharT, _Traits, _Alloc>
  ::basic_stringstream(const allocator_type& __a)
    : std::basic_iostream<_CharT, _Traits>(0),
      _M_buf(std::ios_base::in | std::ios_base::out, __a)
{
   this->init(&_M_buf);
}

template <class _CharT, class _Traits, class _Alloc>
basic_stringstream<_CharT, _Traits, _Alloc>
  ::basic_stringstream(std::ios_base::openmode __mode,  const allocator_type& __a)
    : std::basic_iostream<_CharT, _Traits>(0), _M_buf(__mode, __a)
{
   this->init(&_M_buf);
}

template <class _CharT, class _Traits, class _Alloc>
basic_stringstream<_CharT, _Traits, _Alloc>
  ::basic_stringstream(const _String& __str, const allocator_type& __a)
    : std::basic_iostream<_CharT, _Traits>(0),
      _M_buf(__str, std::ios_base::in | std::ios_base::out, __a)
{
  this->init(&_M_buf);
}

template <class _CharT, class _Traits, class _Alloc>
basic_stringstream<_CharT, _Traits, _Alloc>
  ::basic_stringstream(const _String& __str, std::ios_base::openmode __mode,  const allocator_type& __a)
    : std::basic_iostream<_CharT, _Traits>(0), _M_buf(__str, __mode, __a)
{
  this->init(&_M_buf);
}

#endif  // BSLS_PLATFORM__CMP_MSVC

template <class _CharT, class _Traits, class _Alloc>
basic_stringstream<_CharT, _Traits, _Alloc>::~basic_stringstream()
{}

}  // close namespace bsl

#ifdef BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
extern template class bsl::basic_stringbuf<char>;
extern template class bsl::basic_istringstream<char>;
extern template class bsl::basic_ostringstream<char>;
extern template class bsl::basic_stringstream<char>;

extern template class bsl::basic_stringbuf<wchar_t>;
extern template class bsl::basic_istringstream<wchar_t>;
extern template class bsl::basic_ostringstream<wchar_t>;
extern template class bsl::basic_stringstream<wchar_t>;
#endif

//# undef __BSB_int_type__
//# undef __BSB_pos_type__

// END FORMER CONTENTS OF bslstp_stl_sstream.c

#endif /* INCLUDED_BSLSTP_SSTREAM */

// Local Variables:
// mode:C++
// End:
