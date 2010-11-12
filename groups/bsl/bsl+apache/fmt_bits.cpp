/************************************************************************
 *
 * fmt_bits.cpp - definitions of snprintfa helpers
 *
 * $Id: fmt_bits.cpp 648752 2008-04-16 17:01:56Z faridz $
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
 * Copyright 2005-2008 Rogue Wave Software. Inc.
 * 
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC
#include "fmt_defs.h"
#include <rw_printf.h>

#include <ctype.h>    // for isalnum(), ...
#include <errno.h>    // for errno, errno constants
#include <locale.h>
#include <signal.h>   // for signal constant
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>   // memcpy(), memmove(), strcat(), ...

#ifndef _RWSTD_NO_WCHAR_H
#  include <wchar.h>
#endif   // _RWSTD_NO_WCHAR_H

#ifndef _RWSTD_NO_WCTYPE_H
#  include <wctype.h>   // for iswalpha(), ...
#endif   // _RWSTD_NO_WCTYPE_H

#ifdef _WIN32
#  include <windows.h>   // for FormatMessage()
#endif   // _WIN32

#include <ios>
#include <iostream>
#include <locale>

/**************************************************************************/

struct Bitnames
{
    const char *longname;
    const char *name;
    int         bits;
};

#define BITNAME(qual, name)   { #qual "::" #name, #name, qual::name }

static int
_rw_bmpfmt (const FmtSpec &spec, Buffer &buf,
            const Bitnames  bmap[],
            size_t          size,
            int             bits)
{
    RW_ASSERT (0 != buf.pbuf);

    char buffer [1024];
    *buffer = '\0';

    // string to use when no bits are set
    const char* all_clear = "0";

    for (size_t i = 0; i != size; ++i) {
        if (bmap [i].bits) {
            if ((bits & bmap [i].bits) == bmap [i].bits) {

                const char* const name = spec.fl_pound ?
                    bmap [i].longname : bmap [i].name;

                strcat (*buffer ? strcat (buffer, " | ") : buffer, name);

                bits &= ~bmap [i].bits;
            }
        }
        else {
            // save the name of the constant to use for 0
            all_clear = spec.fl_pound ? bmap [i].longname : bmap [i].name;
        }
    }

    size_t buffersize;

    if ('\0' == *buffer) {
        // no constant matched, format teh value either as a number
        // or, when 0, using the all_clear name (see above)
        if (bits)
            sprintf (buffer, "%#x", bits);
        else
            strcpy (buffer, all_clear);

        buffersize = strlen (buffer);
    }
    else if (bits) {
        buffersize = strlen (buffer);

        // verify that buffer wasn't overflowed
        RW_ASSERT (buffersize <= sizeof buffer);

        char bitstr [32];
        const int n = sprintf (bitstr, "%#x | ", bits);

        RW_ASSERT (0 < n);

        memmove (buffer + n, buffer, buffersize);
        memcpy (buffer, bitstr, size_t (n));

        buffersize += n;
    }
    else {
        buffersize = strlen (buffer);
    }

    // verify that buffer wasn't overflowed
    RW_ASSERT (buffersize <= sizeof buffer);

    // NUL-terminate string so that it can be appended to using
    // the %{+} directive, taking care not to make the NUL a part
    // of the formatted string
    FmtSpec newspec (spec);
    newspec.fl_pound = 0;

    const int len = _rw_fmtstr (newspec, buf, buffer, buffersize + 1);
    --buf.endoff;

    return 0 < len ? len - 1 : len;
}

/********************************************************************/

/* extern */ int
_rw_fmtflags (const FmtSpec &spec, Buffer &buf, int bits)
{
    static const Bitnames names [] = {
        BITNAME (std::ios, adjustfield),
        BITNAME (std::ios, basefield),
        BITNAME (std::ios, boolalpha),
        BITNAME (std::ios, dec),
        BITNAME (std::ios, fixed),
        BITNAME (std::ios, hex),
        BITNAME (std::ios, internal),
        BITNAME (std::ios, left),
        BITNAME (std::ios, oct),
        BITNAME (std::ios, right),
        BITNAME (std::ios, scientific),
        BITNAME (std::ios, showbase),
        BITNAME (std::ios, showpoint),
        BITNAME (std::ios, showpos),
        BITNAME (std::ios, skipws),
        BITNAME (std::ios, unitbuf),
        BITNAME (std::ios, uppercase),

#ifndef _RWSTD_NO_EXT_BIN_IO

        // extension: produce binary output (similar to oct, dec, and hex)
        BITNAME (std::ios, bin),

#endif   // _RWSTD_NO_EXT_BIN_IO

#ifndef _RWSTD_NO_EXT_REENTRANT_IO

        // extension: allow unsychronized access to stream and/or its buffer
        BITNAME (std::ios, nolock),
        BITNAME (std::ios, nolockbuf),

#endif   // _RWSTD_NO_EXT_REENTRANT_IO

        { "std::ios::fmtflags(0)", "fmtflags(0)", std::ios::fmtflags () }

    };

    static const size_t count = sizeof names / sizeof *names;

#ifndef _RWSTD_IOS_BASEMASK
    // PGH: Make work for non-Apache/RW code
    int len = _rw_bmpfmt (spec, buf, names, count, bits);
#else
    const int base = (bits >> _RWSTD_IOS_BASEOFF) & _RWSTD_IOS_BASEMASK;

    // zero out bits representingthe numeric base
    bits &= ~(_RWSTD_IOS_BASEMASK << _RWSTD_IOS_BASEOFF);

    int len = _rw_bmpfmt (spec, buf, names, count, bits);

    if (0 < len && base && base != 8 && base != 10 && base != 16) {

        // for numeric bases other than those required by the standard,
        // use the text "base (%d)" to show the extended numeric base

#ifndef _RWSTD_NO_EXT_BIN_IO

        if (bits & std::ios::bin)
            return len;

#endif   // _RWSTD_NO_EXT_BIN_IO

        const int n = rw_asnprintf (buf.pbuf, buf.pbufsize,
                                    "%{+} | %{?}std::ios::%{;}base(%d)",
                                    spec.fl_pound, base);

        if (0 < n) {
            // adjust length and the end offset after appending above
            len        += n;
            buf.endoff += n;
        }
        else {
            // error (most likely ENOMEM)
            len = n;
        }
    }
#endif // _RWSTD_IOS_BASEMASK

    return len;
}

/********************************************************************/

/* extern */ int
_rw_fmtiostate (const FmtSpec &spec, Buffer &buf, int bits)
{
    static const Bitnames names [] = {
        BITNAME (std::ios, goodbit),
        BITNAME (std::ios, badbit),
        BITNAME (std::ios, eofbit),
        BITNAME (std::ios, failbit)
    };

    static const size_t count = sizeof names / sizeof *names;

    return _rw_bmpfmt (spec, buf, names, count, bits);
}

/********************************************************************/

/* extern */ int
_rw_fmtopenmode (const FmtSpec &spec, Buffer &buf, int bits)
{
    static const Bitnames names [] = {

#ifdef _RWSTD_IOS_NOCREATE
        // PGH: Don't define noreplace ... native for non-RW implementations.
#ifndef _RWSTD_NO_EXTENSIONS

        { "std::ios::nocreate", "nocreate", std::ios::nocreate },
        { "std::ios::noreplace", "noreplace", std::ios::noreplace },

#else   // if defined (_RWSTD_NO_EXTENSIONS)

        { "__rw:::__rw_nocreate", "__rw_nocreate", _RW::__rw_nocreate },
        { "__rw::__rw_noreplace", "__rw_noreplace", _RW::__rw_noreplace },

#endif   // _RWSTD_NO_EXTENSIONS
#endif // _RWSTD_IOS_NOCREATE

#ifdef _RWSTD_IOS_STDIO  // pgh
#ifndef _RWSTD_NO_EXT_STDIO

        { "std::ios::stdio", "stdio", std::ios::stdio },
        { "std::ios::native", "native", std::ios::native },

#else   // if defined (_RWSTD_NO_EXT_STDIO)

        { "__rw::__rw_stdio", "__rw_stdio", _RW::__rw_stdio },
        { "__rw::__rw_native", "__rw_native", _RW::__rw_native },

#endif   // _RWSTD_NO_EXT_STDIO
#endif // _RWSTD_IOS_STDIO

        BITNAME (std::ios, app),
        BITNAME (std::ios, binary),
        BITNAME (std::ios, in),
        BITNAME (std::ios, out),
        BITNAME (std::ios, trunc),
        BITNAME (std::ios, ate),

        { "std::ios::openmode(0)", "openmode(0)", std::ios::openmode () }
    };

    static const size_t count = sizeof names / sizeof *names;

    return _rw_bmpfmt (spec, buf, names, count, bits);
}

/********************************************************************/

/* extern */ int
_rw_fmtseekdir (const FmtSpec &spec, Buffer &buf, int bits)
{
    static const Bitnames names [] = {

        BITNAME (std::ios, beg),
        BITNAME (std::ios, cur),
        BITNAME (std::ios, end)
    };

    static const size_t count = sizeof names / sizeof *names;

    return _rw_bmpfmt (spec, buf, names, count, bits);
}

/********************************************************************/

/* extern */ int
_rw_fmtevent (const FmtSpec& spec, Buffer &buf, int event)
{
    const char* const str =
          std::ios::copyfmt_event == event ? "copyfmt_event"
        : std::ios::imbue_event   == event ? "imbue_event"
        : std::ios::erase_event   == event ? "erase_event"
        : 0;

    // NUL-terminate before appending below
    FmtSpec newspec (spec);
    newspec.fl_pound = 0;

    int len = _rw_fmtstr (newspec, buf, "", 1);

    if (1 == len) {
        // back up before the terminating NUL
        buf.endoff -= 1;

        // append name of event
        len = rw_asnprintf (buf.pbuf, buf.pbufsize,
                            "%{+}%{?}std::ios::%{;}%{?}%s%{:}event(%d)%{;}",
                            spec.fl_pound, 0 != str, str, event);

        if (0 < len)
            buf.endoff += len;
    }

    return len;
}

/********************************************************************/

/* extern */ int
_rw_fmtlc (const FmtSpec &spec, Buffer &buf, int val)
{
    const char *str = 0;

    switch (val) {
    case LC_ALL:      str = "LC_ALL"; break;
    case LC_COLLATE:  str = "LC_COLLATE"; break;
    case LC_CTYPE:    str = "LC_CTYPE"; break;
    case LC_MONETARY: str = "LC_MONETARY"; break;
    case LC_NUMERIC:  str = "LC_NUMERIC"; break;
    case LC_TIME:     str = "LC_TIME"; break;

#ifdef LC_MESSAGES
    case LC_MESSAGES: str = "LC_MESSAGES"; break;
#endif   // LC_MESSAGES

    }

    if (str) {
        // NUL-terminate before appending below
        FmtSpec newspec (spec);
        newspec.fl_pound = 0;

        int len = _rw_fmtstr (newspec, buf, "", 1);

        if (1 == len) {
            // back up before the terminating NUL
            buf.endoff -= 1;

            len = rw_asnprintf (buf.pbuf, buf.pbufsize, "%{+}%s", str);

            if (0 < len)
                buf.endoff += len;
        }

        return len;
    }

    static const Bitnames names [] = {
        BITNAME (std::locale, all),
        BITNAME (std::locale, none),
        BITNAME (std::locale, collate),
        BITNAME (std::locale, ctype),
        BITNAME (std::locale, monetary),
        BITNAME (std::locale, numeric),
        BITNAME (std::locale, messages),
        BITNAME (std::locale, time)
    };

    static const size_t count = sizeof names / sizeof *names;

    return _rw_bmpfmt (spec, buf, names, count, val);
}

/********************************************************************/

/* extern */ int
_rw_fmtmonpat (const FmtSpec &spec, Buffer &buf, const char pat [4])
{
    static const char qual[] = "std::money_base::";

    char buffer [256];

    buffer [0] = '\0';

    for (int i = 0; i != 4; ++i) {
        switch (pat [i]) {
        case std::money_base::symbol:
            if (spec.fl_pound)
                strcat (buffer, qual);
            strcat (buffer, "symbol ");
            break;

        case std::money_base::sign:
            if (spec.fl_pound)
                strcat (buffer, qual);
            strcat (buffer, "sign ");
            break;

        case std::money_base::none:
            if (spec.fl_pound)
                strcat (buffer, qual);
            strcat (buffer, "none ");
            break;

        case std::money_base::value:
            if (spec.fl_pound)
                strcat (buffer, qual);
            strcat (buffer, "value ");
            break;

        case std::money_base::space:
            if (spec.fl_pound)
                strcat (buffer, qual);
            strcat (buffer, "space ");
            break;

        default:
            sprintf (buffer + strlen (buffer), "\\%03o", pat [i]);
            break;
        }
    }

    FmtSpec newspec (spec);
    newspec.fl_pound = 0;

    return _rw_fmtstr (newspec, buf, buffer, _RWSTD_SIZE_MAX);
}

/********************************************************************/

/* extern */ int
_rw_fmtsignal (const FmtSpec &spec, Buffer &buf, int val)
{
    static const struct {
        int         val;
        const char* str;
    } names[] = {

#undef SIGNAL
#define SIGNAL(val)   { val, #val }

#ifdef SIGABRT
        SIGNAL (SIGABRT),
#endif   // SIGABRT
#ifdef SIGALRM
        SIGNAL (SIGALRM),
#endif   // SIGALRM
#ifdef SIGBUS
        SIGNAL (SIGBUS),
#endif   // SIGBUS
#ifdef SIGCANCEL
        SIGNAL (SIGCANCEL),
#endif   // SIGCANCEL
#ifdef SIGCHLD
        SIGNAL (SIGCHLD),
#endif   // SIGCHLD
#ifdef SIGCKPT
        SIGNAL (SIGCKPT),
#endif   // SIGCKPT
#ifdef SIGCLD
        SIGNAL (SIGCLD),
#endif   // SIGCLD
#ifdef SIGCONT
        SIGNAL (SIGCONT),
#endif   // SIGCONT
#ifdef SIGDIL
        SIGNAL (SIGDIL),
#endif   // SIGDIL
#ifdef SIGEMT
        SIGNAL (SIGEMT),
#endif   // SIGEMT
#ifdef SIGFPE
        SIGNAL (SIGFPE),
#endif   // SIGFPE
#ifdef SIGFREEZE
        SIGNAL (SIGFREEZE),
#endif   // SIGFREEZE
#ifdef SIGGFAULT
        SIGNAL (SIGGFAULT),
#endif   // SIGGFAULT
#ifdef SIGHUP
        SIGNAL (SIGHUP),
#endif   // SIGHUP
#ifdef SIGILL
        SIGNAL (SIGILL),
#endif   // SIGILL
#ifdef SIGINFO
        SIGNAL (SIGINFO),
#endif   // SIGINFO
#ifdef SIGINT
        SIGNAL (SIGINT),
#endif   // SIGINT
#ifdef SIGIO
        SIGNAL (SIGIO),
#endif   // SIGIO
#ifdef SIGIOT
        SIGNAL (SIGIOT),
#endif   // SIGIOT
#ifdef SIGK32
        SIGNAL (SIGK32),
#endif   // SIGK32
#ifdef SIGKILL
        SIGNAL (SIGKILL),
#endif   // SIGKILL
#ifdef SIGLOST
        SIGNAL (SIGLOST),
#endif   // SIGLOST
#ifdef SIGLWP
        SIGNAL (SIGLWP),
#endif   // SIGLWP
#ifdef SIGPIPE
        SIGNAL (SIGPIPE),
#endif   // SIGPIPE
#ifdef SIGPOLL
        SIGNAL (SIGPOLL),
#endif   // SIGPOLL
#ifdef SIGPROF
        SIGNAL (SIGPROF),
#endif   // SIGPROF
#ifdef SIGPTINTR
        SIGNAL (SIGPTINTR),
#endif   // SIGPTINTR
#ifdef SIGPTRESCHED
        SIGNAL (SIGPTRESCHED),
#endif   // SIGPTRESCHED
#ifdef SIGPWR
        SIGNAL (SIGPWR),
#endif   // SIGPWR
#ifdef SIGQUIT
        SIGNAL (SIGQUIT),
#endif   // SIGQUIT
#ifdef SIGRESTART
        SIGNAL (SIGRESTART),
#endif   // SIGRESTART
#ifdef SIGRESV
        SIGNAL (SIGRESV),
#endif   // SIGRESV
#ifdef SIGSEGV
        SIGNAL (SIGSEGV),
#endif   // SIGSEGV
#ifdef SIGSTKFLT
        SIGNAL (SIGSTKFLT),
#endif   // SIGSTKFLT
#ifdef SIGSTOP
        SIGNAL (SIGSTOP),
#endif   // SIGSTOP
#ifdef SIGSYS
        SIGNAL (SIGSYS),
#endif   // SIGSYS
#ifdef SIGTERM
        SIGNAL (SIGTERM),
#endif   // SIGTERM
#ifdef SIGTHAW
        SIGNAL (SIGTHAW),
#endif   // SIGTHAW
#ifdef SIGTRAP
        SIGNAL (SIGTRAP),
#endif   // SIGTRAP
#ifdef SIGTSTP
        SIGNAL (SIGTSTP),
#endif   // SIGTSTP
#ifdef SIGTTIN
        SIGNAL (SIGTTIN),
#endif   // SIGTTIN
#ifdef SIGTTOU
        SIGNAL (SIGTTOU),
#endif   // SIGTTOU
#ifdef SIGUNUSED
        SIGNAL (SIGUNUSED),
#endif   // SIGUNUSED
#ifdef SIGURG
        SIGNAL (SIGURG),
#endif   // SIGURG
#ifdef SIGUSR1
        SIGNAL (SIGUSR1),
#endif   // SIGUSR1
#ifdef SIGUSR2
        SIGNAL (SIGUSR2),
#endif   // SIGUSR2
#ifdef SIGVTALRM
        SIGNAL (SIGVTALRM),
#endif   // SIGVTALRM
#ifdef SIGWAITING
        SIGNAL (SIGWAITING),
#endif   // SIGWAITING
#ifdef SIGWINCH
        SIGNAL (SIGWINCH),
#endif   // SIGWINCH
#ifdef SIGWINDOW
        SIGNAL (SIGWINDOW),
#endif   // SIGWINDOW
#ifdef SIGXCPU
        SIGNAL (SIGXCPU),
#endif   // SIGXCPU
#ifdef SIGXFSZ
        SIGNAL (SIGXFSZ),
#endif   // SIGXFSZ
#ifdef SIGXRES
        SIGNAL (SIGXRES),
#endif   // SIGXRES
        { -1, 0 }
    };

    const char *name = 0;

    for (size_t i = 0; i != sizeof names / sizeof *names; ++i) {
        if (names [i].val == val) {
            name = names [i].str;
            break;
        }
    }

    char smallbuf [32];
    if (0 == name) {
        sprintf (smallbuf, "SIG#%d", val);
        name = smallbuf;
    }

    FmtSpec newspec (spec);
    newspec.fl_pound = 0;

    return _rw_fmtstr (newspec, buf, name, _RWSTD_SIZE_MAX);
}

/********************************************************************/

/* extern */ int
_rw_fmterrno (const FmtSpec &spec, Buffer &buf, int val)
{
    static const struct {
        int         val;
        const char* str;
    } names[] = {

#undef ERRNO
#define ERRNO(val)   { val, #val }

#ifdef EPERM
        ERRNO (EPERM),
#endif   // EPERM
#ifdef ENOENT
        ERRNO (ENOENT),
#endif   // ENOENT
#ifdef ESRCH
        ERRNO (ESRCH),
#endif   // ESRCH
#ifdef EINTR
        ERRNO (EINTR),
#endif   // EINTR
#ifdef EIO
        ERRNO (EIO),
#endif   // EIO
#ifdef ENXIO
        ERRNO (ENXIO),
#endif   // ENXIO
#ifdef E2BIG
        ERRNO (E2BIG),
#endif   // E2BIG
#ifdef ENOEXEC
        ERRNO (ENOEXEC),
#endif   // ENOEXEC
#ifdef EBADF
        ERRNO (EBADF),
#endif   // EBADF
#ifdef ECHILD
        ERRNO (ECHILD),
#endif   // ECHILD
#ifdef EAGAIN
        ERRNO (EAGAIN),
#endif   // EAGAIN
#ifdef ENOMEM
        ERRNO (ENOMEM),
#endif   // ENOMEM
#ifdef EACCES
        ERRNO (EACCES),
#endif   // EACCES
#ifdef EFAULT
        ERRNO (EFAULT),
#endif   // EFAULT
#ifdef ENOTBLK
        ERRNO (ENOTBLK),
#endif   // ENOTBLK
#ifdef EBUSY
        ERRNO (EBUSY),
#endif   // EBUSY
#ifdef EEXIST
        ERRNO (EEXIST),
#endif   // EEXIST
#ifdef EXDEV
        ERRNO (EXDEV),
#endif   // EXDEV
#ifdef ENODEV
        ERRNO (ENODEV),
#endif   // ENODEV
#ifdef ENOTDIR
        ERRNO (ENOTDIR),
#endif   // ENOTDIR
#ifdef EISDIR
        ERRNO (EISDIR),
#endif   // EISDIR
#ifdef EINVAL
        ERRNO (EINVAL),
#endif   // EINVAL
#ifdef ENFILE
        ERRNO (ENFILE),
#endif   // ENFILE
#ifdef EMFILE
        ERRNO (EMFILE),
#endif   // EMFILE
#ifdef ENOTTY
        ERRNO (ENOTTY),
#endif   // ENOTTY
#ifdef ETXTBSY
        ERRNO (ETXTBSY),
#endif   // ETXTBSY
#ifdef EFBIG
        ERRNO (EFBIG),
#endif   // EFBIG
#ifdef ENOSPC
        ERRNO (ENOSPC),
#endif   // ENOSPC
#ifdef ESPIPE
        ERRNO (ESPIPE),
#endif   // ESPIPE
#ifdef EROFS
        ERRNO (EROFS),
#endif   // EROFS
#ifdef EMLINK
        ERRNO (EMLINK),
#endif   // EMLINK
#ifdef EPIPE
        ERRNO (EPIPE),
#endif   // EPIPE
#ifdef EDOM
        ERRNO (EDOM),
#endif   // EDOM
#ifdef ERANGE
        ERRNO (ERANGE),
#endif   // ERANGE
#ifdef ENOMSG
        ERRNO (ENOMSG),
#endif   // ENOMSG
#ifdef EIDRM
        ERRNO (EIDRM),
#endif   // EIDRM
#ifdef ECHRNG
        ERRNO (ECHRNG),
#endif   // ECHRNG
#ifdef EL2NSYNC
        ERRNO (EL2NSYNC),
#endif   // EL2NSYNC
#ifdef EL3HLT
        ERRNO (EL3HLT),
#endif   // EL3HLT
#ifdef EL3RST
        ERRNO (EL3RST),
#endif   // EL3RST
#ifdef ELNRNG
        ERRNO (ELNRNG),
#endif   // ELNRNG
#ifdef EUNATCH
        ERRNO (EUNATCH),
#endif   // EUNATCH
#ifdef ENOCSI
        ERRNO (ENOCSI),
#endif   // ENOCSI
#ifdef EL2HLT
        ERRNO (EL2HLT),
#endif   // EL2HLT
#ifdef EDEADLK
        ERRNO (EDEADLK),
#endif   // EDEADLK
#ifdef ENOLCK
        ERRNO (ENOLCK),
#endif   // ENOLCK
#ifdef ECANCELED
        ERRNO (ECANCELED),
#endif   // ECANCELED
#ifdef ENOTSUP
        ERRNO (ENOTSUP),
#endif   // ENOTSUP
#ifdef EDQUOT
        ERRNO (EDQUOT),
#endif   // EDQUOT
#ifdef EBADE
        ERRNO (EBADE),
#endif   // EBADE
#ifdef EBADR
        ERRNO (EBADR),
#endif   // EBADR
#ifdef EXFULL
        ERRNO (EXFULL),
#endif   // EXFULL
#ifdef ENOANO
        ERRNO (ENOANO),
#endif   // ENOANO
#ifdef EBADRQC
        ERRNO (EBADRQC),
#endif   // EBADRQC
#ifdef EBADSLT
        ERRNO (EBADSLT),
#endif   // EBADSLT
#ifdef EDEADLOCK
        ERRNO (EDEADLOCK),
#endif   // EDEADLOCK
#ifdef EBFONT
        ERRNO (EBFONT),
#endif   // EBFONT
#ifdef EOWNERDEAD
        ERRNO (EOWNERDEAD),
#endif   // EOWNERDEAD
#ifdef ENOTRECOVERABLE
        ERRNO (ENOTRECOVERABLE),
#endif   // ENOTRECOVERABLE
#ifdef ENOSTR
        ERRNO (ENOSTR),
#endif   // ENOSTR
#ifdef ENODATA
        ERRNO (ENODATA),
#endif   // ENODATA
#ifdef ETIME
        ERRNO (ETIME),
#endif   // ETIME
#ifdef ENOSR
        ERRNO (ENOSR),
#endif   // ENOSR
#ifdef ENONET
        ERRNO (ENONET),
#endif   // ENONET
#ifdef ENOPKG
        ERRNO (ENOPKG),
#endif   // ENOPKG
#ifdef EREMOTE
        ERRNO (EREMOTE),
#endif   // EREMOTE
#ifdef ENOLINK
        ERRNO (ENOLINK),
#endif   // ENOLINK
#ifdef EADV
        ERRNO (EADV),
#endif   // EADV
#ifdef ESRMNT
        ERRNO (ESRMNT),
#endif   // ESRMNT
#ifdef ECOMM
        ERRNO (ECOMM),
#endif   // ECOMM
#ifdef ELOCKUNMAPPED
        ERRNO (ELOCKUNMAPPED),
#endif   // ELOCKUNMAPPED
#ifdef ENOTACTIVE
        ERRNO (ENOTACTIVE),
#endif   // ENOTACTIVE
#ifdef EMULTIHOP
        ERRNO (EMULTIHOP),
#endif   // EMULTIHOP
#ifdef EBADMSG
        ERRNO (EBADMSG),
#endif   // EBADMSG
#ifdef ENAMETOOLONG
        ERRNO (ENAMETOOLONG),
#endif   // ENAMETOOLONG
#ifdef EOVERFLOW
        ERRNO (EOVERFLOW),
#endif   // EOVERFLOW
#ifdef ENOTUNIQ
        ERRNO (ENOTUNIQ),
#endif   // ENOTUNIQ
#ifdef EBADFD
        ERRNO (EBADFD),
#endif   // EBADFD
#ifdef EREMCHG
        ERRNO (EREMCHG),
#endif   // EREMCHG
#ifdef ELIBACC
        ERRNO (ELIBACC),
#endif   // ELIBACC
#ifdef ELIBBAD
        ERRNO (ELIBBAD),
#endif   // ELIBBAD
#ifdef ELIBSCN
        ERRNO (ELIBSCN),
#endif   // ELIBSCN
#ifdef ELIBMAX
        ERRNO (ELIBMAX),
#endif   // ELIBMAX
#ifdef ELIBEXEC
        ERRNO (ELIBEXEC),
#endif   // ELIBEXEC
#ifdef EILSEQ
        ERRNO (EILSEQ),
#endif   // EILSEQ
#ifdef ENOSYS
        ERRNO (ENOSYS),
#endif   // ENOSYS
#ifdef ELOOP
        ERRNO (ELOOP),
#endif   // ELOOP
#ifdef ERESTART
        ERRNO (ERESTART),
#endif   // ERESTART
#ifdef ESTRPIPE
        ERRNO (ESTRPIPE),
#endif   // ESTRPIPE
#ifdef ENOTEMPTY
        ERRNO (ENOTEMPTY),
#endif   // ENOTEMPTY
#ifdef EUSERS
        ERRNO (EUSERS),
#endif   // EUSERS
#ifdef ENOTSOCK
        ERRNO (ENOTSOCK),
#endif   // ENOTSOCK
#ifdef EDESTADDRREQ
        ERRNO (EDESTADDRREQ),
#endif   // EDESTADDRREQ
#ifdef EMSGSIZE
        ERRNO (EMSGSIZE),
#endif   // EMSGSIZE
#ifdef EPROTOTYPE
        ERRNO (EPROTOTYPE),
#endif   // EPROTOTYPE
#ifdef ENOPROTOOPT
        ERRNO (ENOPROTOOPT),
#endif   // ENOPROTOOPT
#ifdef EPROTONOSUPPORT
        ERRNO (EPROTONOSUPPORT),
#endif   // EPROTONOSUPPORT
#ifdef ESOCKTNOSUPPORT
        ERRNO (ESOCKTNOSUPPORT),
#endif   // ESOCKTNOSUPPORT
#ifdef EOPNOTSUPP
        ERRNO (EOPNOTSUPP),
#endif   // EOPNOTSUPP
#ifdef EPFNOSUPPORT
        ERRNO (EPFNOSUPPORT),
#endif   // EPFNOSUPPORT
#ifdef EAFNOSUPPORT
        ERRNO (EAFNOSUPPORT),
#endif   // EAFNOSUPPORT
#ifdef EADDRINUSE
        ERRNO (EADDRINUSE),
#endif   // EADDRINUSE
#ifdef EADDRNOTAVAIL
        ERRNO (EADDRNOTAVAIL),
#endif   // EADDRNOTAVAIL
#ifdef ENETDOWN
        ERRNO (ENETDOWN),
#endif   // ENETDOWN
#ifdef ENETUNREACH
        ERRNO (ENETUNREACH),
#endif   // ENETUNREACH
#ifdef ENETRESET
        ERRNO (ENETRESET),
#endif   // ENETRESET
#ifdef ECONNABORTED
        ERRNO (ECONNABORTED),
#endif   // ECONNABORTED
#ifdef ECONNRESET
        ERRNO (ECONNRESET),
#endif   // ECONNRESET
#ifdef ENOBUFS
        ERRNO (ENOBUFS),
#endif   // ENOBUFS
#ifdef EISCONN
        ERRNO (EISCONN),
#endif   // EISCONN
#ifdef ENOTCONN
        ERRNO (ENOTCONN),
#endif   // ENOTCONN
#ifdef ESHUTDOWN
        ERRNO (ESHUTDOWN),
#endif   // ESHUTDOWN
#ifdef ETOOMANYREFS
        ERRNO (ETOOMANYREFS),
#endif   // ETOOMANYREFS
#ifdef ETIMEDOUT
        ERRNO (ETIMEDOUT),
#endif   // ETIMEDOUT
#ifdef ECONNREFUSED
        ERRNO (ECONNREFUSED),
#endif   // ECONNREFUSED
#ifdef EHOSTDOWN
        ERRNO (EHOSTDOWN),
#endif   // EHOSTDOWN
#ifdef EHOSTUNREACH
        ERRNO (EHOSTUNREACH),
#endif   // EHOSTUNREACH
#ifdef EWOULDBLOCK
        ERRNO (EWOULDBLOCK),
#endif   // EWOULDBLOCK
#ifdef EALREADY
        ERRNO (EALREADY),
#endif   // EALREADY
#ifdef EINPROGRESS
        ERRNO (EINPROGRESS),
#endif   // EINPROGRESS
#ifdef ESTALE
        ERRNO (ESTALE),
#endif   // ESTALE
        { -1, 0 }
    };

    const char* str = strerror (val);

    char smallbuf [32];

    if (spec.fl_pound || 0 == str) {

        const char *name = 0;

        for (size_t i = 0; i != sizeof names / sizeof *names; ++i) {
            if (names [i].val == val) {
                name = names [i].str;
                break;
            }
        }

        if (0 == name) {
            sprintf (smallbuf, "E#%d", val);
            str = smallbuf;
        }
        else
            str = name;
    }

    FmtSpec newspec (spec);
    newspec.width    = 0;
    newspec.fl_pound = 0;

    return _rw_fmtstr (newspec, buf, str, _RWSTD_SIZE_MAX);
}

/********************************************************************/

/* extern */ int
_rw_fmtlasterror (const FmtSpec &spec, Buffer &buf, int val)
{
#ifdef _WIN32

    LPVOID pmsg;
    FormatMessage (  FORMAT_MESSAGE_ALLOCATE_BUFFER
                   | FORMAT_MESSAGE_FROM_SYSTEM
                   | FORMAT_MESSAGE_IGNORE_INSERTS,
                   0, (DWORD)val,
                   MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
                   (LPTSTR)&pmsg,
                   0, 0);

    FmtSpec newspec (spec);
    newspec.width    = 0;
    newspec.fl_pound = 0;

    const int result = _rw_fmtstr (newspec, buf,
                                   _RWSTD_STATIC_CAST (const char*, pmsg),
                                   _RWSTD_SIZE_MAX);

    LocalFree (pmsg);

    return result;

#else   // if !defined (_WIN32)

    return _rw_fmterrno (spec, buf, val);

#endif   // _WIN32

}

/********************************************************************/

/* extern */ int
_rw_fmtmask (const FmtSpec &spec, Buffer &buf, int bits)
{
    static const Bitnames names [] = {
        BITNAME (std::ctype_base, alnum),
        BITNAME (std::ctype_base, alpha),
        BITNAME (std::ctype_base, cntrl),
        BITNAME (std::ctype_base, digit),
        BITNAME (std::ctype_base, graph),
        BITNAME (std::ctype_base, lower),
        BITNAME (std::ctype_base, print),
        BITNAME (std::ctype_base, punct),
        BITNAME (std::ctype_base, space),
        BITNAME (std::ctype_base, upper),
        BITNAME (std::ctype_base, xdigit)
    };

    static const size_t count = sizeof names / sizeof *names;

    return _rw_bmpfmt (spec, buf, names, count, bits);
}

/********************************************************************/

/* extern */ int
_rw_fmtcharmask (const FmtSpec &spec, Buffer &buf, int c)
{
    enum {
        bit_alnum  = 1,
        bit_alpha  = 1 << 1,
        bit_cntrl  = 1 << 2,
        bit_digit  = 1 << 3,
        bit_graph  = 1 << 4,
        bit_lower  = 1 << 5,
        bit_print  = 1 << 6,
        bit_punct  = 1 << 7,
        bit_space  = 1 << 8,
        bit_upper  = 1 << 9,
        bit_xdigit = 1 << 10
    };

    int mask = 0;

    if (spec.fl_pound) {

#ifndef _RWSTD_NO_WCHAR_H

        mask |= iswalnum (c) ? bit_alnum : 0;
        mask |= iswalpha (c) ? bit_alpha : 0;
        mask |= iswcntrl (c) ? bit_cntrl : 0;
        mask |= iswdigit (c) ? bit_digit : 0;
        mask |= iswgraph (c) ? bit_graph : 0;
        mask |= iswlower (c) ? bit_lower : 0;
        mask |= iswprint (c) ? bit_print : 0;
        mask |= iswpunct (c) ? bit_punct : 0;
        mask |= iswspace (c) ? bit_space : 0;
        mask |= iswupper (c) ? bit_upper : 0;
        mask |= iswxdigit (c) ? bit_xdigit : 0;

#endif   // _RWSTD_NO_WCHAR_H

    }
    else {
        const unsigned char uc = c;

        mask |= isalnum (uc) ? bit_alnum : 0;
        mask |= isalpha (uc) ? bit_alpha : 0;
        mask |= iscntrl (uc) ? bit_cntrl : 0;
        mask |= isdigit (uc) ? bit_digit : 0;
        mask |= isgraph (uc) ? bit_graph : 0;
        mask |= islower (uc) ? bit_lower : 0;
        mask |= isprint (uc) ? bit_print : 0;
        mask |= ispunct (uc) ? bit_punct : 0;
        mask |= isspace (uc) ? bit_space : 0;
        mask |= isupper (uc) ? bit_upper : 0;
        mask |= isxdigit (uc) ? bit_xdigit : 0;
    }

    char mask_str [80];
    char *str = mask_str;

    str [0] = '\0';

#define APPEND(bit)                             \
    if (mask & bit_ ## bit)                     \
        strcat (strcat (str, #bit), "|");       \
    else (void)0

    APPEND (alnum);
    APPEND (alpha);
    APPEND (cntrl);
    APPEND (digit);
    APPEND (graph);
    APPEND (lower);
    APPEND (print);
    APPEND (punct);
    APPEND (space);
    APPEND (upper);
    APPEND (xdigit);

    if (str == mask_str)
        *str = '\0';
    else
        str [-1] = '\0';

    FmtSpec newspec (spec);
    newspec.fl_pound = 0;

    return _rw_fmtstr (newspec, buf, str, _RWSTD_SIZE_MAX);
}

/********************************************************************/

/* extern */ int
_rw_fmtbits (const FmtSpec &spec, Buffer &buf,
             const void *pelems, size_t elemsize)
{
    const size_t nbits = spec.prec;

    char bitbuf [256];
    char *pbits = nbits < sizeof bitbuf ? bitbuf : new char [nbits + 1];

    for (size_t bitno = 0; bitno != nbits; ++bitno) {

        if (bitno && 0 == (bitno % (elemsize * _RWSTD_CHAR_BIT)))
            pelems = _RWSTD_STATIC_CAST (const UChar*, pelems) + elemsize;

        size_t bit = 1 << (bitno & (_RWSTD_CHAR_BIT * elemsize - 1));

        switch (elemsize) {
        case _RWSTD_CHAR_SIZE:
            bit = *_RWSTD_STATIC_CAST (const UChar*, pelems) & bit;
            break;

#if _RWSTD_CHAR_SIZE < _RWSTD_SHRT_SIZE
        case _RWSTD_SHRT_SIZE:
            bit = *_RWSTD_STATIC_CAST (const UShrt*, pelems) & bit;
            break;
#endif   // sizeof (char) < sizeof (short)

#if _RWSTD_SHRT_SIZE < _RWSTD_INT_SIZE
        case _RWSTD_INT_SIZE:
            bit = *_RWSTD_STATIC_CAST (const UInt*, pelems) & bit;
            break;
#endif   // sizeof (short) < sizeof (int)

#if _RWSTD_INT_SIZE < _RWSTD_LONG_SIZE
        case _RWSTD_LONG_SIZE:
            bit = *_RWSTD_STATIC_CAST (const ULong*, pelems) & bit;
            break;
#endif   // sizeof (int) < sizeof (long)

#if _RWSTD_LONG_SIZE < _RWSTD_LLONG_SIZE
        case _RWSTD_LLONG_SIZE:
            bit = *_RWSTD_STATIC_CAST (const ULLong*, pelems) & bit;
            break;
#endif   // sizeof (long) < sizeof (long long)

        default:
            RW_ASSERT (!"logic error: bad element size");
        }

        pbits [bitno] = bit ? '1' : '0';
    }

    const int res = _rw_fmtstr (spec, buf, pbits, nbits);

    if (pbits != bitbuf)
        delete[] pbits;

    return res;
}
