/************************************************************************
 *
 * opt_types.cpp - definitions of type option handlers
 *
 * $Id: opt_types.cpp 550991 2007-06-26 23:58:07Z sebor $
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
 * Copyright 1994-2006 Rogue Wave Software.
 *
 **************************************************************************/

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include "opt_types.h"

#include <cmdopt.h>
#include <driver.h>

#include <string.h>

enum {
    rw_bool, rw_char, rw_uchar, rw_schar, rw_ushrt, rw_shrt,
    rw_uint, rw_int, rw_ulong, rw_long, rw_ullong, rw_llong,
    rw_flt, rw_dbl, rw_ldbl, rw_wchar, rw_pvoid
};


static int
_rw_disabled_types = 0;

static int
_rw_enabled_types = 0;


static int
_rw_enable_type (int bit, int enable)
{
    const int set_mask = 1 << bit;

    if (enable)
        _rw_enabled_types |= set_mask;
    else
        _rw_disabled_types |= set_mask;

    return 0;
}


#define DEFINE_HANDLERS(T)                               \
        static int _rw_no_ ## T (int, char *[]) {        \
            return _rw_enable_type (rw_ ## T, false);    \
        }                                                \
        static int _rw_en_ ## T (int, char *[]) {        \
            return _rw_enable_type (rw_ ## T, true);     \
        }                                                \
        typedef void unused_type /* allow a semicolon */

DEFINE_HANDLERS (bool);
DEFINE_HANDLERS (char);
DEFINE_HANDLERS (schar);
DEFINE_HANDLERS (uchar);
DEFINE_HANDLERS (shrt);
DEFINE_HANDLERS (ushrt);
DEFINE_HANDLERS (int);
DEFINE_HANDLERS (uint);
DEFINE_HANDLERS (long);
DEFINE_HANDLERS (ulong);
DEFINE_HANDLERS (llong);
DEFINE_HANDLERS (ullong);
DEFINE_HANDLERS (flt);
DEFINE_HANDLERS (dbl);
DEFINE_HANDLERS (ldbl);
DEFINE_HANDLERS (wchar);
DEFINE_HANDLERS (pvoid);

/**************************************************************************/

int _rw_setopts_types ()
{
    const int result =
        rw_setopts (// options to disable fundamental types
            "|-no-bool |-no-char |-no-schar |-no-uchar "
            "|-no-shrt |-no-ushrt "
            "|-no-int |-no-uint "
            "|-no-long |-no-ulong "
            "|-no-llong |-no-ullong "
            "|-no-flt |-no-dbl |-no-ldbl "
            "|-no-wchar |-no-pvoid "
            // options to enable fundamental types
            "|-enable-bool |-enable-char |-enable-schar |-enable-uchar "
            "|-enable-shrt |-enable-ushrt "
            "|-enable-int |-enable-uint "
            "|-enable-long |-enable-ulong "
            "|-enable-llong |-enable-ullong "
            "|-enable-flt |-enable-dbl |-enable-ldbl "
            "|-enable-wchar |-enable-pvoid",
            // handlers to disable fundamental types
            _rw_no_bool, _rw_no_char, _rw_no_schar, _rw_no_uchar,
            _rw_no_shrt, _rw_no_ushrt,
            _rw_no_int, _rw_no_uint,
            _rw_no_long, _rw_no_ulong,
            _rw_no_llong, _rw_no_ullong,
            _rw_no_flt, _rw_no_dbl, _rw_no_ldbl,
            _rw_no_wchar, _rw_no_pvoid,
            // handlers to enable fundamental types
            _rw_en_bool, _rw_en_char, _rw_en_schar, _rw_en_uchar,
            _rw_en_shrt, _rw_en_ushrt,
            _rw_en_int, _rw_en_uint,
            _rw_en_long, _rw_en_ulong,
            _rw_en_llong, _rw_en_ullong,
            _rw_en_flt, _rw_en_dbl, _rw_en_ldbl,
            _rw_en_wchar, _rw_en_pvoid);

    return result;
}

/**************************************************************************/

_TEST_EXPORT int
rw_enabled (const char *name)
{
    int enabled = -1;

    static const struct {
        int bit;
        const char *name;
    } types[] = {
        { rw_bool, "bool" },

        { rw_char, "char" },

        { rw_schar, "schar" },
        { rw_schar, "signed char" },
        { rw_uchar, "uchar" },
        { rw_uchar, "unsigned char" },

        { rw_shrt, "shrt" },
        { rw_shrt, "short" },
        { rw_shrt, "signed short" },
        { rw_ushrt, "ushrt" },
        { rw_ushrt, "unsigned short" },

        { rw_int, "int" },
        { rw_int, "signed int" },
        { rw_uint, "uint" },
        { rw_uint, "unsigned int" },

        { rw_long, "long" },
        { rw_long, "signed long" },
        { rw_ulong, "ulong" },
        { rw_ulong, "unsigned long" },

        { rw_llong, "llong" },
        { rw_llong, "long long" },
        { rw_llong, "signed long long" },
        { rw_ullong, "ullong" },
        { rw_ullong, "unsigned long long" },

        { rw_flt, "flt" },
        { rw_flt, "float" },

        { rw_dbl, "dbl" },
        { rw_dbl, "double" },

        { rw_ldbl, "ldbl" },
        { rw_ldbl, "long double" },

        { rw_wchar, "wchar" },
        { rw_wchar, "wchar_t" },

        { rw_pvoid, "pvoid" },
        { rw_pvoid, "void*" }
    };

    for (size_t i = 0; i != sizeof types / sizeof *types; ++i) {
        if (0 == strcmp (types [i].name, name)) {
            const int mask = 1 << types [i].bit;

            enabled = !(_rw_disabled_types & mask);

            if (_rw_enabled_types)
                enabled = enabled && (_rw_enabled_types & mask);

            break;
        }
    }

    return enabled;
}
