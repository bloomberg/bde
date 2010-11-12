/************************************************************************
 *
 * valcmp.cpp - definitions of the rw_valcmp() family of helper functions
 *
 * $Id: valcmp.cpp 525985 2007-04-05 22:31:12Z sebor $
 *
 ************************************************************************
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

#include <valcmp.h>

#include <stdio.h>    // for fprintf, stderr
#include <stdlib.h>   // for abort
#include <string.h>   // for memcmp

/**************************************************************************/

typedef unsigned char UChar;


static const UChar
_rw_upper [] = {

#if 'A' == 0x41

    // basic ASCII:
    //          .0  .1  .2  .3  .4  .5  .6  .7  .8  .9  .a  .b  .c  .d  .e  .f
    //         --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
    //         NUL SOH STX ETX EOT ENQ ACK BEL BS  TAB  LF  VT  FF  CR  SO  SI
    /* 0. */ "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
    //         DLE DC1 DC2 DC3 DC4 NAK SYN ETB CAN  EM SUB ESC  FS  GS  RS  US
    /* 1. */ "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"
    //         SPC   !   "   #   $   %   '   '   (   )   *   +   ,   -   .   /
    /* 2. */ "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f"
    //           0   1   2   3   4   5   6   7   8   9   :   ;   <   =   >   ?
    /* 3. */ "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f"
    //               A   B   C   D   E   F   G   H   I   J   K   L   M   N   O
    /* 4. */ "\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f"
    //           P   Q   R   S   T   U   V   W   X   Y   Z   [   \   ]   ^   _
    /* 5. */ "\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f"
    //           `   a   b   c   d   e   f   g   h   i   j   k   l   m   n   o
    /* 6. */ "\x60\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f"
    //           p   q   r   s   t   u   v   w   x   y   z   {   |   }   ~ DEL
    /* 7. */ "\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x7b\x7c\x7d\x7e\x7f"

    // extended ASCII:
    /* 8. */ "\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f"
    /* 9. */ "\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f"
    /* a. */ "\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf"
    /* b. */ "\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf"
    /* c. */ "\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf"
    /* d. */ "\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf"
    /* e. */ "\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef"
    /* f. */ "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff"

#elif 'A' == 0xc1

    // EBCDIC:
    //          .0  .1  .2  .3  .4  .5  .6  .7  .8  .9  .a  .b  .c  .d  .e  .f
    //         --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
    //         NUL SOH STX ETX  PF  HT  LC DEL         SMM  VT  FF  CR  SO  SI
    /* 0. */ "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
    //         DLE DC1 DC2  TM RES  NL  BS  IL CAN  EM  CC CU1 IFS IGS IRS IUS
    /* 1. */ "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"
    //          DS SOS  FS     BYP  LF ETB ESC          SM CU2     ENQ ACK BEL
    /* 2. */ "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f"
    //                 SYN      PN  RS  UC EOT             CU3 DC4 NAK     SUB
    /* 3. */ "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f"
    //          SP                                     ct.  .   <   (   +   |
    /* 4. */ "\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f"
    //          &                                       !   $   *   )   ;   ~
    /* 5. */ "\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f"
    //          -   /                                       ,   %   _   >   ?
    /* 6. */ "\x60\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f"
    //                                                  :   #   @   '   =   "
    /* 7. */ "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f"
    //              a   b   c   d   e   f   g   h   i
    /* 8. */ "\x80\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\x8a\x8b\x8c\x8d\x8e\x8f"
    //              j   k   l   m   n   o   p   q   r
    /* 9. */ "\x90\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\x9a\x9b\x9c\x9d\x9e\x9f"
    //                  s   t   u   v   w   x   y   z
    /* a. */ "\xa0\xa1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xaa\xab\xac\xad\xae\xaf"
    //                                            `
    /* b. */ "\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf"
    //              A   B   C   D   E   F   G   H   I
    /* c. */ "\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf"
    //              J   K   L   M   N   O   P   Q   R
    /* d. */ "\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf"
    //                  S   T   U   V   W   X   Y   Z
    /* e. */ "\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef"
    //          0   1   2   3   4   5   6   7   8   9
    /* f. */ "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff"

#else   // 'A' != 0x41 && 'A' != 0xc1
#  error unknown character set (neither ASCII nor EBCDIC)
#endif   // ASCII or EBCDIC

};


// returns 1 iff all size bytes of the object pointed to by buf are 0
static int
_rw_iszero (const void *buf, size_t size)
{
    for (size_t i = 0; i != size; ++i)
        if (_RWSTD_STATIC_CAST (const UChar*, buf)[i])
            return 0;

    return 1;
}


// compares two objects of equal size
static int
_rw_cmpx (const void *buf1,
          const void *buf2,
          size_t      nelems,
          size_t      size,
          int         flags = 0)
{
    const UChar *p1 = _RWSTD_STATIC_CAST (const UChar*, buf1);
    const UChar *p2 = _RWSTD_STATIC_CAST (const UChar*, buf2);

    int ret = 0;

    for ( ; nelems; p1 += size, p2 += size, --nelems) {

        ret = memcmp (p1, p2, size);

        if (flags & CMP_NULTERM) {

            const int zero1 = _rw_iszero (p1, size);
            const int zero2 = _rw_iszero (p2, size);

            if (zero1 || zero2) {
                ret = zero1 - zero2;
                break;
            }
        }

        if (ret && (flags & CMP_NOCASE)) {
            // FIXME: implement
            fprintf (stderr,
                     "%s:%d: case insensitive comparison not implemented\n",
                     __FILE__, __LINE__);
            abort ();
        }
    }

    if (flags & CMP_RETOFF) {
        // FIXME: implement
        fprintf (stderr,
                 "%s:%d: returning offset not implemented\n",
                 __FILE__, __LINE__);
        abort ();
    }

    return ret < 0 ? -1 : ret ? +1 : 0;
}


// compares two byte arrays
static int
_rw_cmp1 (const void *buf1,
          const void *buf2,
          size_t      nelems,
          int         flags = 0)
{
#ifdef _RWSTD_UINT8_T

    typedef _RWSTD_UINT8_T UI8T;

    if (0 == flags) {
        const int ret = memcmp (buf1, buf2, nelems);
        return ret < 0 ? -1 : ret ? +1 : 0;
    }

    if (CMP_NULTERM == flags) {
        const int ret =
            strncmp (_RWSTD_STATIC_CAST (const char*, buf1),
                     _RWSTD_STATIC_CAST (const char*, buf2),
                     nelems);
        return ret < 0 ? -1 : ret ? +1 : 0;
    }

    size_t inx = 0;
    int    ret = 0;

    const UI8T *pi1 = _RWSTD_STATIC_CAST (const UI8T*, buf1);
    const UI8T *pi2 = _RWSTD_STATIC_CAST (const UI8T*, buf2);

    for (; inx < nelems; ++pi1, ++pi2, ++inx) {

        const UI8T ui1 = *pi1;
        const UI8T ui2 = *pi2;

        if (flags & CMP_NULTERM) {

            if (0 == ui1) {
                ret = ui2 ? -1 : 0;
                break;
            }

            if (0 == ui2) {
                ret = +1;
                break;
            }
        }

        if (ui1 != ui2) {
            if (flags & CMP_NOCASE) {
                if (_rw_upper [ui1] != _rw_upper [ui2]) {
                    ret = _rw_upper [ui1] < _rw_upper [ui2] ? -1 : +1;
                    break;
                }
            }
            else {
                ret = ui1 < ui2 ? -1 : +1;
                break;
            }
        }
    }

    if (CMP_RETOFF & flags) {
        ret = ret ? int (inx) : -1;
    }
            
    return ret;

#else // if !defined (_RWSTD_UINT8_T)
#  error _RWSTD_UINT8_T not #defined (configuration problem?)
#endif   // _RWSTD_UINT8_T

}


// compares two arrays of objects each 16 bits in size
static int
_rw_cmp2 (const void *buf1,
          const void *buf2,
          size_t      nelems,
          int         flags = 0)
{
#ifdef _RWSTD_UINT16_T

    typedef _RWSTD_UINT16_T UI16T;

    size_t inx = 0;
    int    ret = 0;

    const UI16T *pi1 = _RWSTD_STATIC_CAST (const UI16T*, buf1);
    const UI16T *pi2 = _RWSTD_STATIC_CAST (const UI16T*, buf2);

    for (; inx < nelems; ++pi1, ++pi2, ++inx) {

        const UI16T ui1 = *pi1;
        const UI16T ui2 = *pi2;

        if (flags & CMP_NULTERM) {

            if (0 == ui1) {
                ret = ui2 ? -1 : 0;
                break;
            }

            if (0 == ui2) {
                ret = +1;
                break;
            }
        }

        if (ui1 != ui2) {
            if (flags & CMP_NOCASE) {
                if (_RWSTD_UCHAR_MAX < ui1 || _RWSTD_UCHAR_MAX < ui2) {
                    ret = ui1 < ui2 ? -1 : +1;
                    break;
                }
                else if (_rw_upper [ui1] != _rw_upper [ui2]) {
                    ret = _rw_upper [ui1] < _rw_upper [ui2] ? -1 : +1;
                    break;
                }
            }
            else {
                ret = ui1 < ui2 ? -1 : +1;
                break;
            }
        }
    }

    if (CMP_RETOFF & flags) {
        ret = ret ? int (inx) : -1;
    }
            
    return ret;

#else   // if !defined (_RWSTD_UINT16_T)

    return _rw_cmpx (buf1, buf2, nelems, 2, flags);

#endif   // _RWSTD_UINT16_T
}


// compares two arrays of objects each 32 bits in size
static int
_rw_cmp4 (const void *buf1,
          const void *buf2,
          size_t      nelems,
          int         flags = 0)
{
#ifdef _RWSTD_UINT32_T

    typedef _RWSTD_UINT32_T UI32T;

    size_t inx = 0;
    int    ret = 0;

    const UI32T *pi1 = _RWSTD_STATIC_CAST (const UI32T*, buf1);
    const UI32T *pi2 = _RWSTD_STATIC_CAST (const UI32T*, buf2);

    for (; inx < nelems; ++pi1, ++pi2, ++inx) {

        const UI32T ui1 = *pi1;
        const UI32T ui2 = *pi2;

        if (flags & CMP_NULTERM) {

            if (0 == ui1) {
                ret = ui2 ? -1 : 0;
                break;
            }

            if (0 == ui2) {
                ret = +1;
                break;
            }
        }

        if (ui1 != ui2) {
            if (flags & CMP_NOCASE) {
                if (_RWSTD_UCHAR_MAX < ui1 || _RWSTD_UCHAR_MAX < ui2) {
                    ret = ui1 < ui2 ? -1 : +1;
                    break;
                }
                else if (_rw_upper [ui1] != _rw_upper [ui2]) {
                    ret = _rw_upper [ui1] < _rw_upper [ui2] ? -1 : +1;
                    break;
                }
            }
            else {
                ret = ui1 < ui2 ? -1 : +1;
                break;
            }
        }
    }

    if (CMP_RETOFF & flags) {
        ret = ret ? int (inx) : -1;
    }
            
    return ret;

#else   // if !defined (_RWSTD_UINT32_T)

    return _rw_cmpx (buf1, buf2, nelems, 4, flags);

#endif   // _RWSTD_UINT32_T
}


// compares two arrays of objects each 64 bits in size
static int
_rw_cmp8 (const void *buf1,
          const void *buf2,
          size_t      nelems,
          int         flags = 0)
{
#ifdef _RWSTD_UINT64_T

    typedef _RWSTD_UINT64_T UI64T;

    size_t inx = 0;
    int    ret = 0;

    const UI64T *pi1 = _RWSTD_STATIC_CAST (const UI64T*, buf1);
    const UI64T *pi2 = _RWSTD_STATIC_CAST (const UI64T*, buf2);

    for (; nelems; ++pi1, ++pi2, ++inx) {

        const UI64T ui1 = *pi1;
        const UI64T ui2 = *pi2;

        if (flags & CMP_NULTERM) {

            if (0 == ui1) {
                ret = ui2 ? -1 : 0;
                break;
            }

            if (0 == ui2) {
                ret = +1;
                break;
            }
        }

        if (ui1 != ui2) {
            if (flags & CMP_NOCASE) {
                if (_RWSTD_UCHAR_MAX < ui1 || _RWSTD_UCHAR_MAX < ui2) {
                    ret = ui1 < ui2 ? -1 : +1;
                    break;
                }
                else if (_rw_upper [ui1] != _rw_upper [ui2]) {
                    ret = _rw_upper [ui1] < _rw_upper [ui2] ? -1 : +1;
                    break;
                }
            }
            else {
                ret = ui1 < ui2 ? -1 : +1;
                break;
            }
        }
    }

    if (CMP_RETOFF & flags) {
        ret = ret ? int (inx) : -1;
    }
            
    return ret;

#else   // if !defined (_RWSTD_UINT64_T)

    return _rw_cmpx (buf1, buf2, nelems, 8, flags);

#endif   // _RWSTD_UINT64_T
}


// compares two arrays of objects of unequal size
static int
_rw_cmpxx (const void* buf1,
           const void* buf2,
           size_t      nelems,
           size_t      size1,
           size_t      size2,
           int         flags)
{
    size_t inx = 0;
    int    ret = 0;

    for (; inx < nelems; ++inx) {

#ifdef _RWSTD_UINT64_T

        _RWSTD_UINT64_T ui1;
        _RWSTD_UINT64_T ui2;

#else   // if !defined (_RWSTD_UINT64_T)

        size_t ui1;
        size_t ui2;

#endif   // _RWSTD_UINT64_T

        switch (size1) {

#ifdef _RWSTD_UINT8_T

        case 1: {
            const _RWSTD_UINT8_T* ptr =
                _RWSTD_STATIC_CAST (const _RWSTD_UINT8_T*, buf1);
            ui1 = *ptr++;
            buf1 = ptr;
            break;
        }

#endif   // _RWSTD_UINT8_T

#ifdef _RWSTD_UINT16_T

        case 2: {
            const _RWSTD_UINT16_T* ptr =
                _RWSTD_STATIC_CAST (const _RWSTD_UINT16_T*, buf1);
            ui1 = *ptr++;
            buf1 = ptr;
            break;
        }

#endif   // _RWSTD_UINT16_T

#ifdef _RWSTD_UINT32_T

        case 4: {
            const _RWSTD_UINT32_T* ptr =
                _RWSTD_STATIC_CAST (const _RWSTD_UINT32_T*, buf1);
            ui1 = *ptr++;
            buf1 = ptr;
            break;
        }

#endif   // _RWSTD_UINT32_T

#ifdef _RWSTD_UINT64_T

        case 8: {
            const _RWSTD_UINT64_T* ptr =
                _RWSTD_STATIC_CAST (const _RWSTD_UINT64_T*, buf1);
            ui1 = *ptr++;
            buf1 = ptr;
            break;
        }

#endif   // _RWSTD_UINT64_T

        default:
            fprintf (stderr,
                     "%s:%d: comparison of objects %u and %u bytes in size "
                     "not implemented\n", __FILE__, __LINE__,
                     unsigned (size1), unsigned (size2));
            abort ();
        }

        switch (size2) {

#ifdef _RWSTD_UINT8_T

        case 1: {
            const _RWSTD_UINT8_T* ptr =
                _RWSTD_STATIC_CAST (const _RWSTD_UINT8_T*, buf2);
            ui2 = *ptr++;
            buf2 = ptr;
            break;
        }

#endif   // _RWSTD_UINT8_T

#ifdef _RWSTD_UINT16_T

        case 2: {
            const _RWSTD_UINT16_T* ptr =
                _RWSTD_STATIC_CAST (const _RWSTD_UINT16_T*, buf2);
            ui2 = *ptr++;
            buf2 = ptr;
            break;
        }

#endif   // _RWSTD_UINT16_T

#ifdef _RWSTD_UINT32_T

        case 4: {
            const _RWSTD_UINT32_T* ptr =
                _RWSTD_STATIC_CAST (const _RWSTD_UINT32_T*, buf2);
            ui2 = *ptr++;
            buf2 = ptr;
            break;
        }

#endif   // _RWSTD_UINT32_T

#ifdef _RWSTD_UINT64_T

        case 8: {
            const _RWSTD_UINT64_T* ptr =
                _RWSTD_STATIC_CAST (const _RWSTD_UINT64_T*, buf2);
            ui2 = *ptr++;
            buf2 = ptr;
            break;
        }

#endif   // _RWSTD_UINT64_T

        default:
            fprintf (stderr,
                     "%s:%d: comparison of objects %u and %u bytes in size "
                     "not implemented\n", __FILE__, __LINE__,
                     unsigned (size1), unsigned (size2));
            abort ();
        }

        if (flags & CMP_NULTERM) {

            if (0 == ui1) {
                ret = ui2 ? -1 : 0;
                break;
            }

            if (0 == ui2) {
                ret = +1;
                break;
            }
        }

        if (ui1 != ui2) {
            if (flags & CMP_NOCASE) {
                if (_RWSTD_UCHAR_MAX < ui1 || _RWSTD_UCHAR_MAX < ui2) {
                    ret = ui1 < ui2 ? -1 : +1;
                    break;
                }
                else if (_rw_upper [ui1] != _rw_upper [ui2]) {
                    ret = _rw_upper [ui1] < _rw_upper [ui2] ? -1 : +1;
                    break;
                }
            }
            else {
                ret = ui1 < ui2 ? -1 : +1;
                break;
            }
        }
    }

    if (CMP_RETOFF & flags) {
        ret = ret ? int (inx) : -1;
    }

    return ret;
}


_TEST_EXPORT int
rw_valcmp (const void* buf1,
           const void* buf2,
           size_t      nelems,
           size_t      size1,
           size_t      size2,
           int         flags /* = 0 */)
{
    if (size1 == size2) {

        switch (size1) {
        case 1: return _rw_cmp1 (buf1, buf2, nelems, flags);
        case 2: return _rw_cmp2 (buf1, buf2, nelems, flags);
        case 4: return _rw_cmp4 (buf1, buf2, nelems, flags);
        case 8: return _rw_cmp8 (buf1, buf2, nelems, flags);
        }

        return _rw_cmpx (buf1, buf2, nelems, size1, flags);
    }

    return _rw_cmpxx (buf1, buf2, nelems, size1, size2, flags);
}


_TEST_EXPORT int
rw_strncmp (const char* str1,
            const char* str2,
            size_t      nelems /* = _RWSTD_SIZE_MAX */,
           int          flags /* = CMP_NULTERM */)
{
    return rw_valcmp (str1, str2, nelems, 1, 1, flags);
}


#ifndef _RWSTD_NO_WCHAR_T

_TEST_EXPORT int
rw_strncmp (const char*    str1,
            const wchar_t* str2,
            size_t         nelems /* = _RWSTD_SIZE_MAX */,
           int             flags /* = CMP_NULTERM */)
{
    return rw_valcmp (str1, str2, nelems, 1, sizeof (wchar_t), flags);
}


_TEST_EXPORT int
rw_strncmp (const wchar_t* str1,
            const char*    str2,
            size_t         nelems /* = _RWSTD_SIZE_MAX */,
            int            flags /* = CMP_NULTERM */)
{
    return rw_valcmp (str1, str2, nelems, sizeof (wchar_t), 1, flags);
}


_TEST_EXPORT int
rw_strncmp (const wchar_t* str1,
            const wchar_t* str2,
            size_t         nelems /* = _RWSTD_SIZE_MAX */,
           int             flags /* = CMP_NULTERM */)
{
    return rw_valcmp (str1, str2, nelems,
                      sizeof (wchar_t), sizeof (wchar_t), flags);
}

#endif   // _RWSTD_NO_WCHAR_T


// floating point comparison helpers based on
// http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm

_TEST_EXPORT int
rw_fltcmp (float x, float y)
{
#if _RWSTD_FLT_SIZE == _RWSTD_SHRT_SIZE
    typedef short IntT;
    const IntT imin = _RWSTD_SHRT_MIN;
#elif _RWSTD_FLT_SIZE == _RWSTD_INT_SIZE
    typedef int IntT;
    const IntT imin = _RWSTD_INT_MIN;
#elif _RWSTD_FLT_SIZE == _RWSTD_LONG_SIZE
    typedef long IntT;
    const IntT imin = _RWSTD_LONG_MIN;
#elif _RWSTD_FLT_SIZE == _RWSTD_LLONG_SIZE
    typedef _RWSTD_LONG_LONG IntT;
    const IntT imin = _RWSTD_LLONG_MIN;
#else
    // ???
#  error no integral type of the same size as float exists
#endif

    if (x == y)
        return 0;

    // make both arguments lexicographically ordered as twos-complement ints
    IntT x_int = *(IntT*)&x;
    if (x_int < 0)
        x_int = imin - x_int;

    IntT y_int = *(IntT*)&y;
    if (y_int < 0)
        y_int = imin - y_int;

    const IntT int_diff = x_int - y_int;

    return int_diff;
}


#define Abs(x) ((x) < 0 ? -(x) : (x))


_TEST_EXPORT int
rw_dblcmp (double x, double y)
{
#if _RWSTD_DBL_SIZE == _RWSTD_INT_SIZE
    typedef int IntT;
    const IntT imin = _RWSTD_INT_MIN;
#elif _RWSTD_DBL_SIZE == _RWSTD_LONG_SIZE
    typedef long IntT;
    const IntT imin = _RWSTD_LONG_MIN;
#elif _RWSTD_DBL_SIZE == _RWSTD_LLONG_SIZE
    typedef _RWSTD_LONG_LONG IntT;
    const IntT imin = _RWSTD_LLONG_MIN;
#endif

#if _RWSTD_LLONG_SIZE < _RWSTD_DBL_SIZE

    if (x == y)
        return 0;

    // FIXME: use integer math as in the functions above

    const double diff = x - y;

    // check absolute error
    if (Abs (diff) < _RWSTD_DBL_EPSILON)
        return 0;

    // check relative error
    const double relerr =
        Abs (x) < Abs (y) ? Abs (diff / y) : Abs (diff / x);

    if (relerr <= 0.0000001)
        return 0;

    return x < y ? -1 : +1;

#else   // if !(_RWSTD_LLONG_SIZE < _RWSTD_DBL_SIZE)

    if (x == y)
        return 0;

    IntT x_int = *(IntT*)&x;
    if (x_int < 0)
        x_int = imin - x_int;

    IntT y_int = *(IntT*)&y;
    if (y_int < 0)
        y_int = imin - y_int;

    const IntT int_diff = x_int - y_int;

    // cast a possibly wider type to int to avoid compiler warnings
    // this should be safe since the function should not be used to
    // compare numbers that are that far apart
    return int (int_diff);

#endif   // _RWSTD_LLONG_SIZE < _RWSTD_DBL_SIZE

}


#ifndef _RWSTD_NO_LONG_DOUBLE

_TEST_EXPORT int
rw_ldblcmp (long double x, long double y)
{
    if (sizeof (long double) == sizeof (double))
        return rw_dblcmp (double (x), double (y));

    if (x == y)
        return 0;

    // FIXME: use integer math as in the functions above

    const long double diff = x - y;

    // check absolute error
    if (Abs (diff) < _RWSTD_LDBL_EPSILON)
        return 0;

    // check relative error
    const long double relerr =
        Abs (x) < Abs (y) ? Abs (diff / y) : Abs (diff / x);

    if (relerr <= 0.0000001L)
        return 0;

    return x < y ? -1 : +1;
}

#endif   // _RWSTD_NO_LONG_DOUBLE
