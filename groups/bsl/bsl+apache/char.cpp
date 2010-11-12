/**************************************************************************
 *
 * char.cpp - helpers for user-defined character type and its traits
 *
 * $Id: char.cpp 648752 2008-04-16 17:01:56Z faridz $
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
 * except in  compliance with the License.   You may obtain  a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the  License is distributed on an  "AS IS" BASIS,
 * WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
 * implied.   See  the License  for  the  specific language  governing
 * permissions and limitations under the License.
 * 
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC
#include <rw_char.h>

#include <rw_printf.h>   // for rw_snprintfa()

#include <ctype.h>       // for isdigit()
#include <stdarg.h>      // for va_arg(), va_list, ...
#include <stdlib.h>      // for strtol()
#include <string.h>      // for memcpy(), strlen(), ...

/**************************************************************************/
// for convenience
typedef unsigned char UChar;


void UserTraits<char>::
assign (char_type &dst, const char_type &src)
{
    ++n_calls_ [MemFun::assign];
    Base::assign (dst, src);
}


bool UserTraits<char>::
eq (const char_type &ch1, const char_type &ch2)
{
    ++n_calls_ [MemFun::eq];
    return Base::eq (ch1, ch2);
}


bool UserTraits<char>::
lt (const char_type &ch1, const char_type &ch2)
{
    ++n_calls_ [MemFun::lt];
    return Base::lt (ch1, ch2);
}


int UserTraits<char>::
compare (const char_type *s1, const char_type *s2, size_t n)
{
    ++n_calls_ [MemFun::compare];
    return Base::compare (s1, s2, n);
}

        
size_t UserTraits<char>::
length (const char_type *s)
{
    ++n_calls_ [MemFun::length];
    return Base::length (s);
}
 

const UserTraits<char>::char_type*
UserTraits<char>::
find (const char_type *s, size_t n, const char_type &ch)
{
    ++n_calls_ [MemFun::find];
    return Base::find (s, n, ch);
}


UserTraits<char>::char_type*
UserTraits<char>::
copy (char_type *dst, const char_type *src, size_t n)
{
    ++n_calls_ [MemFun::copy];
    return Base::copy (dst, src, n);
}


UserTraits<char>::char_type*
UserTraits<char>::
move (char_type *dst, const char_type *src, size_t n)
{
    ++n_calls_ [MemFun::move];
    return Base::move (dst, src, n);
}


UserTraits<char>::char_type*
UserTraits<char>::
assign (char_type *s, size_t n, char_type ch)
{
    ++n_calls_ [MemFun::assign];
    return Base::assign (s, n, ch);
}


UserTraits<char>::int_type
UserTraits<char>::
not_eof (const int_type &i)
{
    ++n_calls_ [MemFun::not_eof];
    return eof () == i ? ~i : i;
}


UserTraits<char>::char_type
UserTraits<char>::
to_char_type (const int_type &i)
{
    ++n_calls_ [MemFun::to_char_type];
    return Base::to_char_type (i);
}

      
UserTraits<char>::int_type
UserTraits<char>::
to_int_type (const char_type &ch)
{
    ++n_calls_ [MemFun::to_int_type];
    return Base::to_int_type (ch);
}


bool UserTraits<char>::
eq_int_type (const int_type &i1, const int_type &i2)
{
    ++n_calls_ [MemFun::eq_int_type];
    return Base::eq_int_type (i1, i2);
}


UserTraits<char>::int_type
UserTraits<char>::
eof ()
{
    return eof_;
}


size_t UserTraits<char>::
n_calls_ [UserTraits<char>::MemFun::n_funs];


UserTraits<char>::int_type
UserTraits<char>::
eof_ = std::char_traits<char>::eof ();


#ifndef _RWSTD_NO_WCHAR_T

void UserTraits<wchar_t>::
assign (char_type &dst, const char_type &src)
{
    ++n_calls_ [MemFun::assign];
    Base::assign (dst, src);
}


bool UserTraits<wchar_t>::
eq (const char_type &ch1, const char_type &ch2)
{
    ++n_calls_ [MemFun::eq];
    return Base::eq (ch1, ch2);
}


bool UserTraits<wchar_t>::
lt (const char_type &ch1, const char_type &ch2)
{
    ++n_calls_ [MemFun::lt];
    return Base::lt (ch1, ch2);
}


int UserTraits<wchar_t>::
compare (const char_type *s1, const char_type *s2, size_t n)
{
    ++n_calls_ [MemFun::compare];
    return Base::compare (s1, s2, n);
}

        
size_t UserTraits<wchar_t>::
length (const char_type *s)
{
    ++n_calls_ [MemFun::length];
    return Base::length (s);
}
 

const UserTraits<wchar_t>::char_type*
UserTraits<wchar_t>::
find (const char_type *s, size_t n, const char_type &ch)
{
    ++n_calls_ [MemFun::find];
    return Base::find (s, n, ch);
}


UserTraits<wchar_t>::char_type*
UserTraits<wchar_t>::
copy (char_type *dst, const char_type *src, size_t n)
{
    ++n_calls_ [MemFun::copy];
    return Base::copy (dst, src, n);
}


UserTraits<wchar_t>::char_type*
UserTraits<wchar_t>::
move (char_type *dst, const char_type *src, size_t n)
{
    ++n_calls_ [MemFun::move];
    return Base::move (dst, src, n);
}


UserTraits<wchar_t>::char_type*
UserTraits<wchar_t>::
assign (char_type *s, size_t n, char_type ch)
{
    ++n_calls_ [MemFun::assign];
    return Base::assign (s, n, ch);
}


UserTraits<wchar_t>::int_type
UserTraits<wchar_t>::
not_eof (const int_type &i)
{
    ++n_calls_ [MemFun::not_eof];
    return eof () == i ? ~i : i;
}


UserTraits<wchar_t>::char_type
UserTraits<wchar_t>::
to_char_type (const int_type &i)
{
    ++n_calls_ [MemFun::to_char_type];
    return Base::to_char_type (i);
}

      
UserTraits<wchar_t>::int_type
UserTraits<wchar_t>::
to_int_type (const char_type &ch)
{
    ++n_calls_ [MemFun::to_int_type];
    return Base::to_int_type (ch);
}


bool UserTraits<wchar_t>::
eq_int_type (const int_type &i1, const int_type &i2)
{
    ++n_calls_ [MemFun::eq_int_type];
    return Base::eq_int_type (i1, i2);
}


UserTraits<wchar_t>::int_type
UserTraits<wchar_t>::
eof ()
{
    return eof_;
}


size_t UserTraits<wchar_t>::
n_calls_ [UserTraits<wchar_t>::MemFun::n_funs];


UserTraits<wchar_t>::int_type
UserTraits<wchar_t>::
eof_ = std::char_traits<wchar_t>::eof ();

#endif   // _RWSTD_NO_WCHAR_T


size_t
UserTraits<UserChar>::
n_calls_ [UserTraits<UserChar>::MemFun::n_funs];


void
UserTraits<UserChar>::
assign (char_type &c1, const char_type &c2)
{
    ++n_calls_ [MemFun::assign];

    c1.f = c2.f;
    c1.c = c2.c;
}


bool
UserTraits<UserChar>::
eq (const char_type &c1, const char_type &c2)
{
    ++n_calls_ [MemFun::eq];

    return c1.f == c2.f && c1.c == c2.c;
}


bool
UserTraits<UserChar>::
lt (const char_type &c1, const char_type &c2)
{
    ++n_calls_ [MemFun::lt];

    return c1.f < c2.f || c1.f == c2.f && c1.c < c2.c;
}


int UserTraits<UserChar>::
compare (const char_type *s1, const char_type *s2, size_t n)
{
    RW_ASSERT (0 == n || s1 && s2);

    ++n_calls_ [MemFun::compare];

    for (size_t i = 0; i != n; ++i) {
        if (s1 [i].f != s2 [i].f || s1 [i].c != s2 [i].c) {
            if (   s1 [i].f < s2 [i].f
                || s1 [i].f == s2 [i].f && s1 [i].c < s2 [i].c)
                return -1;

            return 1;
        }
    }

    return 0;
}
        

size_t UserTraits<UserChar>::
length (const char_type *s)
{
    RW_ASSERT (0 != s);

    ++n_calls_ [MemFun::length];

    size_t len = 0;

    for (; s [len].f || s [len].c; ++len);

    return len;
}
 

const UserTraits<UserChar>::char_type*
UserTraits<UserChar>::
find (const char_type *s, size_t n, const char_type &c)
{
    RW_ASSERT (0 == n || s);

    ++n_calls_ [MemFun::find];

    for (; n--; ++s) {
        if (s->f == c.f && s->c == c.c)
            return s;
    }

    return 0;
}


UserTraits<UserChar>::char_type*
UserTraits<UserChar>::
copy (char_type *dst, const char_type *src, size_t n)
{
    RW_ASSERT (0 == n || dst && src);

    ++n_calls_ [MemFun::copy];

    for (size_t i = 0; i != n; i++)
        dst [i] = src [i];

    return dst;
}


UserTraits<UserChar>::char_type*
UserTraits<UserChar>::
move (char_type *dst, const char_type *src, size_t n)
{
    RW_ASSERT (0 == n || dst && src);

    ++n_calls_ [MemFun::move];

    if (dst < src) {
        for (size_t i = 0; i != n; ++i)
            dst [i] = src [i];
    }
    else {
        while (n--)
            dst [n] = src [n];
    }

    return dst;
}


UserTraits<UserChar>::char_type*
UserTraits<UserChar>::
assign (char_type *s, size_t n, char_type c)
{
    RW_ASSERT (0 == n || s);

    ++n_calls_ [MemFun::assign2];

    for (; n--; s [n] = c);

    return s;
}


UserTraits<UserChar>::int_type
UserTraits<UserChar>::
not_eof (const int_type &i)
{
    ++n_calls_ [MemFun::not_eof];

    if (i.equal (int_type::eof ())) {
        const char_type c = { 0, 0 };

        return int_type::from_char (c);
    }

    return i;
}


UserTraits<UserChar>::char_type
UserTraits<UserChar>::
to_char_type (const int_type &i)
{
    ++n_calls_ [MemFun::to_char_type];

    return i.to_char ();
}
      

UserTraits<UserChar>::int_type
UserTraits<UserChar>::
to_int_type (const char_type &c)
{
    ++n_calls_ [MemFun::to_int_type];

    return int_type::from_char (c);
}


bool
UserTraits<UserChar>::
eq_int_type (const int_type &i1, const int_type &i2)
{
    ++n_calls_ [MemFun::eq_int_type];

    return i1.equal (i2);
}


UserTraits<UserChar>::int_type
UserTraits<UserChar>::
eof ()
{
    ++n_calls_ [MemFun::eof];

    return int_type::eof ();
}

/**************************************************************************/

_TEST_EXPORT
char*
rw_widen (char *dst, const char *src, size_t len /* = SIZE_MAX */)
{
    // compute the length of src if not specified
    if (_RWSTD_SIZE_MAX == len)
        len = src ? strlen (src) : 0;

    if (len) {
        RW_ASSERT (0 != dst);

        if (src) {
            // copy src into dst
            memcpy (dst, src, len);
            dst [len] = '\0';
        }
        else {
            // set dst to all NUL
            memset (dst, 0, len);
        }
    }
    else if (dst)
        *dst = '\0';

    return dst;
}

/**************************************************************************/

static unsigned long
_rw_get_char (const char *src, const char** end, size_t *count)
{
    RW_ASSERT (0 != src);
    RW_ASSERT (0 != end);
    RW_ASSERT (0 != count);

    if (0 == *count) {
        *end = src;
        return 0;
    }

    unsigned long ch = UChar (*src++);

    if (0 == ch && _RWSTD_SIZE_MAX == *count) {
        // stop at the terminating NUL
        *end   = src;
        *count = 1;
        return ch;
    }

    if ('<' == char (ch) && 'U' == src [0] && isxdigit (src [1])) {
        // this looks like the beginning of a <Unnn...>
        // sequence encoding a Unicode character, look
        // for a sequence of digits followed by the
        // closing '>'

        char *tmp_end;
        const unsigned long val = strtoul (src + 1, &tmp_end, 16);
        if ('>' == *tmp_end) {
            ch  = val;
            src = tmp_end + 1;
        }
    }

    if ('@' == src [0] && isdigit (src [1])) {
        // <char>@<count> denotes a repeat directive representing
        // <count> consecutive occurrences of the character <char>

        char* tmp_end;
        *count = strtoul (src + 1, &tmp_end, 10);
        src    = tmp_end;
    }
    else
        *count = 1;

    *end = src;

    return ch;
}


static void*
_rw_expand (void *dst, size_t elemsize,
            const char *src, size_t src_len /* = SIZE_MAX */,
            size_t *dst_len /* = 0 */)
{
    // create a typedef for "genericity"
#ifndef _RWSTD_NO_WCHAR_T
    typedef wchar_t WChar;
#else   // if defined (_RWSTD_NO_WCHAR_T)
    // dummy, will never be used
    typedef int WChar;
#endif   // _RWSTD_NO_WCHAR_T

    RW_ASSERT (   sizeof (char) == elemsize
               || sizeof (WChar) == elemsize
               || sizeof (UserChar) == elemsize);

    if (0 == src) {
        src     = "";
        src_len = 0;
    }

    // save the original value of dst and avoid deallocating
    // it when a large buffer is needed
    const void* const dst_save = dst;

    // when both dst and dst_size are non-null use *dst_len
    // as the size of the destination buffer
    size_t bufsize = dst ? dst_len ? *dst_len : _RWSTD_SIZE_MAX : 0;
    size_t buflen  = 0;

    if (_RWSTD_SIZE_MAX == src_len)
        src_len = strlen (src);

    void *pnext = dst;

    for (const char *psrc = src; ; ) {

        size_t count = src_len;
        const char *end = 0;
        const unsigned long ch = _rw_get_char (psrc, &end, &count);

        const size_t nchars = size_t (end - psrc);
        if (nchars <= src_len)
            src_len -= nchars;
        else
            src_len = 0;

        psrc = end;

        if (bufsize - buflen <= count) {
            // increase the size of the buffer
            bufsize = (bufsize + count) * 2;
            if (bufsize < 128)
                bufsize = 128;

            // allocate larger buffer
            void* tmp;

            switch (elemsize) {
            case sizeof (WChar):    tmp = new WChar [bufsize]; break;
            case sizeof (UserChar): tmp = new UserChar [bufsize]; break;
            default:                tmp = new char [bufsize]; break;
            }

            // copy old buffer into new one
            memcpy (tmp, dst, buflen * elemsize);

            // dispose of previously allocated buffer
            if (dst != dst_save) {
                switch (elemsize) {
                case sizeof (WChar):    delete[] (WChar*)dst; break;
                case sizeof (UserChar): delete[] (UserChar*)dst; break;
                default:                delete[] (char*)dst; break;
                }
            }

            dst = tmp;

            pnext = (char*)dst + buflen * elemsize;
        }

        if (sizeof (WChar) == elemsize) {
            for (size_t i = 0; i != count; ++i)
                ((WChar*)pnext)[i] = WChar (ch);
        }
        else if (sizeof (UserChar) == elemsize) {
            for (size_t i = 0; i != count; ++i) {
                ((UserChar*)pnext)[i].f = 0;
                ((UserChar*)pnext)[i].c = UChar (ch);
            }
        }
        else if (ch < 0x80U) {
            memset (pnext, UChar (ch), count);
        }
        else {
#if 1
            // narrow the wide character to char
            memset (pnext, UChar (ch), count);

            pnext   = (char*)pnext + count * elemsize;
            buflen += count;
            
#else   // disabled

            // FIXME: enable UCS to UTF-8 conversion
            // (need to allocate sufficient storage above)

            const char* const pnext_start = pnext;

            // count the number of UTF-8 bytes
            size_t nbytes = 0;

            for (size_t i = 0; i != count; ++i) {
                if (ch < 0x800U) {
                    *pnext++ = UChar (0xc0U | (ch >> 6));
                    *pnext++ = UChar (0x80U | (ch & 0x3fU));
                    nbytes  += 2;
                }
                else if (ch < 0x10000U) {
                    *pnext++ = UChar (0xe0U | (ch >> 12));
                    *pnext++ = UChar (0x80U | (ch >>  6 & 0x3fU));
                    *pnext++ = UChar (0x80U | (ch       & 0x3fU));
                    nbytes  += 3;
                }
                else if (ch < 0x200000U) {
                    *pnext++ = UChar (0xf0U | (ch >> 18));
                    *pnext++ = UChar (0x80U | (ch >> 12 & 0x3fU));
                    *pnext++ = UChar (0x80U | (ch >>  6 & 0x3fU));
                    *pnext++ = UChar (0x80U | (ch       & 0x3fU));
                    nbytes  += 4;
                }
                else if (ch < 0x4000000U) {
                    *pnext++ = UChar (0xf8U | (ch >> 24));
                    *pnext++ = UChar (0x80U | (ch >> 18 & 0x3fU));
                    *pnext++ = UChar (0x80U | (ch >> 12 & 0x3fU));
                    *pnext++ = UChar (0x80U | (ch >>  6 & 0x3fU));
                    *pnext++ = UChar (0x80U | (ch       & 0x3fU));
                    nbytes  += 5;
                }
                else {
                    *pnext++ = UChar (0xfcU | (ch >> 30));
                    *pnext++ = UChar (0x80U | (ch >> 24 & 0x3fU));
                    *pnext++ = UChar (0x80U | (ch >> 18 & 0x3fU));
                    *pnext++ = UChar (0x80U | (ch >> 12 & 0x3fU));
                    *pnext++ = UChar (0x80U | (ch >>  6 & 0x3fU));
                    *pnext++ = UChar (0x80U | (ch       & 0x3fU));
                    nbytes  += 6;
                }
            }

#endif   // 0/1

        }

        pnext   = (char*)pnext + count * elemsize;
        buflen += count;

        if (0 == src_len) {
            memset (pnext, 0, elemsize);
            break;
        }
    }

    if (dst_len)
        *dst_len = buflen;

    return dst;
}

/**************************************************************************/

_TEST_EXPORT
char*
rw_expand (char *dst, const char *src, size_t src_len /* = SIZE_MAX */,
           size_t *dst_len /* = 0 */)
{
    void* const result = _rw_expand (dst, sizeof *dst, src, src_len, dst_len);

    return _RWSTD_STATIC_CAST (char*, result);
}


_TEST_EXPORT
char*
rw_narrow (char *dst, const char *src, size_t len /* = SIZE_MAX */)
{
    return rw_widen (dst, src, len);
}


_TEST_EXPORT
size_t
rw_match (const char *s1, const char *s2, size_t len /* = SIZE_MAX */)
{
    // the length of the initial subsequence of s1 and s2
    // consisting solely of characters that compare equal
    size_t count = 0;

    if (0 == s1) {
        s1 = s2;
        s2 = 0;
    }

    const char* p1 = s1;

    if (0 == s2) {
        // when one of the strings is null, compute the length
        // of the other string when all directives are expanded
        if (0 == s1 || 0 == *s1)
            return 0;

        do {
            size_t n = _RWSTD_SIZE_MAX == len ? len : len - count;

            _rw_get_char (p1, &p1, &n);

            count += n;
        } while (p1 && *p1);

        return count;
    }
    
    const char* p2 = s2;

    size_t n1 = 0;
    size_t n2 = 0;

    for (unsigned long ch1, ch2; count < len; ) {

        while (0 == n1) {
            n1  = _RWSTD_SIZE_MAX == len ? len : len - count;
            ch1 = _rw_get_char (p1, &p1, &n1);
        }

        while (0 == n2) {
            n2  = _RWSTD_SIZE_MAX == len ? len : len - count;
            ch2 = _rw_get_char (p2, &p2, &n2);
        }

        if (ch1 != ch2)
            break;

        if (n1 < n2) {
            // the repeat count specified by the first directive
            // is less than the repeat count given by the second
            count += n1;
            n2    -= n1;
            n1     = 0;

        }
        else if (n2 <= n1) {
            // the repeat count specified by the second directive
            // is less than or equal than that given by the first
            count += n2;
            n1    -= n2;
            n2     = 0;
        }

        if (_RWSTD_SIZE_MAX == len && 0 == ch1)
            break;
    }

    return len < count ? len : count;
}


#ifndef _RWSTD_NO_WCHAR_T

_TEST_EXPORT
wchar_t*
rw_widen (wchar_t *dst, const char *src, size_t len /* = SIZE_MAX */)
{
    // compute the length of src if not specified
    if (_RWSTD_SIZE_MAX == len)
        len = src ? strlen (src) : 0;

    // if len is non-zero dst must be non-0 as well
    RW_ASSERT (0 == len || 0 != dst);

    if (dst) {
        if (src) {
            // widen src into dst one element at a time
            for (size_t i = 0; ; ++i) {
                if (i == len) {
                    dst [i] = L'\0';
                    break;
                }

                dst [i] = wchar_t (UChar (src [i]));
            }
        }
        else {
            // set dst to all NUL
            memset (dst, 0, len * sizeof *dst);
        }
    }

    return dst;
}


_TEST_EXPORT
wchar_t*
rw_expand (wchar_t *dst, const char *src, size_t src_len /* = SIZE_MAX */,
           size_t *dst_len /* = 0 */)
{
    void* const result = _rw_expand (dst, sizeof *dst, src, src_len, dst_len);

    return _RWSTD_STATIC_CAST (wchar_t*, result);
}


_TEST_EXPORT
char*
rw_narrow (char *dst, const wchar_t *src, size_t len /* = SIZE_MAX */)
{
    // compute the length of src if not specified
    if (_RWSTD_SIZE_MAX == len) {
        if (src) {
            for (len = 0; src [len]; ++len);
        }
        else
            len = 0;
    }

    // if len is non-zero dst must be non-0 as well
    RW_ASSERT (0 == len || 0 != dst);

    if (dst) {
        if (src) {
            // narrow src into dst one element at a time
            for (size_t i = 0; ; ++i) {
                if (i == len) {
                    dst [i] = '\0';
                    break;
                }

                dst [i] = char (UChar (src [i]));
            }
        }
        else {
            // set dst to all NUL
            memset (dst, 0, len);
        }
    }

    return dst;
}


_TEST_EXPORT
size_t
rw_match (const char *s1, const wchar_t *s2, size_t len /* = SIZE_MAX */)
{
    if (0 == s1) {
        if (s2) {
            // return the length of s2
            for (len = 0; s2 [len]; ++len);
            return len;
        }

        return 0;
    }

    if (0 == s2)
        return rw_match (s1, (char*)0, len);

    const char*    p1 = s1;
    const wchar_t* p2 = s2;

    // the length of the initial subsequence of s1 and s2
    // consisting solely of characters that compare equal
    size_t count = 0;

    size_t n1 = 0;
    size_t n2 = 0;

    for (unsigned long ch1, ch2; count < len; ) {

        while (0 == n1) {
            n1  = _RWSTD_SIZE_MAX == len ? len : len - count;
            ch1 = _rw_get_char (p1, &p1, &n1);
        }

        ch2 = _RWSTD_STATIC_CAST (unsigned long, *p2++);
        n2  = 1;

        if (ch1 != ch2)
            break;

        if (n1 < n2) {
            // the repeat count specified by the first directive
            // is less than the repeat count given by the second
            count += n1;
            n2    -= n1;
            n1     = 0;

        }
        else if (n2 <= n1) {
            // the repeat count specified by the second directive
            // is less than or equal than that given by the first
            count += n2;
            n1    -= n2;
            n2     = 0;
        }

        if (_RWSTD_SIZE_MAX == len && L'\0' == ch1)
            break;
    }

    return len < count ? len : count;
}

#endif   // _RWSTD_NO_WCHAR_T


_TEST_EXPORT
UserChar*
rw_widen (UserChar *dst, const char *src, size_t len /* = SIZE_MAX */)
{
    // compute the length of src if not specified
    if (_RWSTD_SIZE_MAX == len)
        len = src ? strlen (src) : 0;

    // if len is non-zero dst must be non-0 as well
    RW_ASSERT (0 == len || 0 != dst);

    if (dst) {
        if (src) {
            // widen src into dst one element at a time
            for (size_t i = 0; ; ++i) {
                if (i == len) {
                    dst [i] = UserChar::eos ();
                    break;
                }

                dst [i].f = 0;
                dst [i].c = UChar (src [i]);
            }
        }
        else {
            // set dst to all NUL
            memset (dst, 0, len * sizeof *dst);
        }
    }

    return dst;
}


_TEST_EXPORT
UserChar*
rw_expand (UserChar *dst, const char *src, size_t src_len /* = SIZE_MAX */,
           size_t *dst_len /* = 0 */)
{
    void* const result = _rw_expand (dst, sizeof *dst, src, src_len, dst_len);

    return _RWSTD_STATIC_CAST (UserChar*, result);
}


_TEST_EXPORT
char*
rw_narrow (char *dst, const UserChar *src, size_t len /* = SIZE_MAX */)
{
    // compute the length of src if not specified
    if (_RWSTD_SIZE_MAX == len) {
        if (src) {
            for (len = 0; src [len].f || src [len].c; ++len);
        }
        else
            len = 0;
    }

    // if len is non-zero dst must be non-0 as well
    RW_ASSERT (0 == len || 0 != dst);

    if (dst) {
        if (src) {
            // narrow src into dst one element at a time
            for (size_t i = 0; ; ++i) {
                if (i == len) {
                    dst [i] = '\0';
                    break;
                }

                dst [i] = char (src [i].c);
            }
        }
        else {
            // set dst to all NUL
            memset (dst, 0, len);
        }
    }

    return dst;
}


_TEST_EXPORT
size_t
rw_match (const char *s1, const UserChar *s2, size_t len /* = SIZE_MAX */)
{
    if (0 == s1) {
        if (s2) {
            // return the length of s2
            for (len = 0; s2 [len].f || s2 [len].c; ++len);
            return len;
        }

        return 0;
    }

    if (0 == s2)
        return rw_match (s1, (char*)0, len);

    const char*     p1 = s1;
    const UserChar* p2 = s2;

    // the length of the initial subsequence of s1 and s2
    // consisting solely of characters that compare equal
    size_t count = 0;

    size_t n1 = 0;
    size_t n2 = 0;

    unsigned long ch1;

    for (UserChar ch2; count < len; ) {

        while (0 == n1) {
            n1  = _RWSTD_SIZE_MAX == len ? len : len - count;
            ch1 = _rw_get_char (p1, &p1, &n1);
        }

        ch2 = *p2++;
        n2  = 1;

        if (ch1 != ch2.c)
            break;

        if (n1 < n2) {
            // the repeat count specified by the first directive
            // is less than the repeat count given by the second
            count += n1;
            n2    -= n1;
            n1     = 0;

        }
        else if (n2 <= n1) {
            // the repeat count specified by the second directive
            // is less than or equal than that given by the first
            count += n2;
            n1    -= n2;
            n2     = 0;
        }

        if (_RWSTD_SIZE_MAX == len && L'\0' == ch1)
            break;
    }

    return len < count ? len : count;
}

/**************************************************************************/

static int
_rw_fmtstringv (char **pbuf, size_t *pbufsize, const char *fmt, va_list va)
{
    RW_ASSERT (0 != pbuf);
    RW_ASSERT (0 != pbufsize);
    RW_ASSERT (0 != fmt);

    // directive syntax:
    // '/' [ '#' ] { '*' | <n> } [ '.' '*' | <n> ] "Gs"

    // NOTE:
    // leading slash (or any other violation of the "standard" directive
    // syntax) prevents the caller from extracting width and precision
    // (etc.) from its variable argument list and allows us to do so

    static int nested_call;

    if (nested_call || '/' != fmt [0])
        return _RWSTD_INT_MIN;

    ++nested_call;
    ++fmt;

    va_list* pva      =  0;
    bool     fl_pound = false;
    int      nelems   = -1;
    int      paramno  = -1;
    int      elemsize = -1;

    union UPtr {
        const char     *pc;
        const wchar_t  *pwc;
        const UserChar *puc;
    };

    if ('#' == *fmt) {
        fl_pound = true;
        ++fmt;
    }

    // saved caller's va_list in case it needs to be restored
    // to its orginal state after extracting argument from it
    va_list va_save;

    if ('*' == *fmt) {
        // process element width (i.e., sizeof(charT))
        pva = va_arg (va, va_list*);

        RW_ASSERT (0 != pva);

        _RWSTD_VA_COPY (va_save, *pva);

        // extract the width from rw_snprintfa's variable argument
        // list pass through to us by the caller
        elemsize = va_arg (*pva, int);
        ++fmt;
    }
    else if (isdigit (UChar (*fmt))) {
        // process positional parameter or width
        char* end = 0;
        const int arg = strtol (fmt, &end, 10);
        if ('$' == *end)
            paramno = arg;
        else
            elemsize = arg;

        fmt = end;
    }

    if ('.' == *fmt) {
        // process precision (the length of the array in elements)
        if ('*' == *++fmt) {
            if (0 == pva) {
                pva = va_arg (va, va_list*);

                RW_ASSERT (0 != pva);

                _RWSTD_VA_COPY (va_save, *pva);
            }

            // extract the width from rw_snprintfa's variable argument
            // list passed through to us by the caller
            nelems = va_arg (*pva, int);
            ++fmt;
        }
        else if (isdigit (UChar (*fmt))) {
            char* end = 0;
            nelems = int (strtol (fmt, &end, 10));
            fmt    = end;
        }
    }

    // extract the address of the caller's variable argument list
    if (0 == pva) {
        pva = va_arg (va, va_list*);

        RW_ASSERT (0 != pva);

        _RWSTD_VA_COPY (va_save, *pva);
    }

    if ('G' != fmt [0] || 's' != fmt [1] || '\0' != fmt [2]) {

        // restore caller's (potentially modified) va_list
        _RWSTD_VA_COPY (*pva, va_save);

        --nested_call;

        // uknown directive, let caller process
        return _RWSTD_INT_MIN;
    }

    // extract a pointer to the first character from rw_snprintfa's
    // variable argument list pass through to us by the caller 
    const UPtr beg = { va_arg (*pva, char*) };

    {
        // extract the address where to store the extracted argument
        // for use by any subsequent positional paramaters
        const char** const pparam = va_arg (va, const char**);

        RW_ASSERT (0 != pparam);

        // store the extracted argument
        *pparam = beg.pc;
    }

    // compute the length of the buffer formatted so far
    const size_t buflen_0 = *pbuf ? strlen (*pbuf) : 0;

    int nbytes = 0;

    //////////////////////////////////////////////////////////////////
    // invoke rw_asnprintf() recursively to format our arguments
    // and append the result to the end of the buffer; pass the
    // value returned from rw_asnprintf() (i.e., the number of
    // bytes appended) back to the caller

    if (-1 == elemsize || 1 == elemsize) {
        if (nelems < 0)
            nelems = beg.pc ? int (strlen (beg.pc)) : 0;

        nbytes = rw_asnprintf (pbuf, pbufsize, "%{+}%{#*s}", nelems, beg.pc);
    }
    else if (_RWSTD_WCHAR_SIZE == elemsize) {
        if (nelems < 0)
            nbytes = rw_asnprintf (pbuf, pbufsize, "%{+}%{#ls}", beg.pwc);
        else
            nbytes = rw_asnprintf (pbuf, pbufsize, "%{+}%{#*ls}",
                                   nelems, beg.pwc);
    }
    else if (sizeof (UserChar) == size_t (elemsize)) {

        // narrow the argument into a local buffer of sufficient size
        // (dynamically allocating memory only when the length of the
        // string exceeds the size of the buffer for efficiency) and formt
        // the resulting narrow string
        char smallbuf [256];
        const size_t len = nelems < 0 ? rw_match (0, beg.puc) : size_t (nelems);
        char* const pb = len < sizeof smallbuf ? smallbuf : new char [len + 1];
        rw_narrow (pb, beg.puc, len);

        if (nelems < 0)
            nelems = int (len);

        nbytes = rw_asnprintf (pbuf, pbufsize, "%{+}%{#*s}",
                               nelems, beg.pc ? pb : beg.pc);

        if (pb != smallbuf)
            delete[] pb;
    }
    else {
        nbytes = rw_asnprintf (pbuf, pbufsize,
                               "*** %%{/Gs}: bad element size: %d ***",
                               elemsize);
    }

    //////////////////////////////////////////////////////////////////

    // compute the new length of the buffer
    const size_t buflen_1 = *pbuf ? strlen (*pbuf) : 0;

    // assert that the function really appended as many characters
    // as it said it did (assumes no NULs embedded in the output)
    // and that it didn't write past the end of the buffer
    RW_ASSERT (buflen_1 == buflen_0 + nbytes);
    RW_ASSERT (buflen_1 < *pbufsize);

    --nested_call;

    return nbytes;
}


static int
_rw_fmtstring (char **pbuf, size_t *pbufsize, const char *fmt, ...)
{
    va_list va;
    va_start (va, fmt);

    const int nbytes = _rw_fmtstringv (pbuf, pbufsize, fmt, va);

    va_end (va);

    return nbytes;
}


UserCharFmatInit::
UserCharFmatInit ()
{
    // install the formatting callback function
    static int format_init = rw_printf ("%{+!}", _rw_fmtstring);

    _RWSTD_UNUSED (format_init);
}
