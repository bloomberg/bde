/***********************************************************************
 *
 * 18.numeric.special.float.cpp
 *
 *   test to exercise floating point specializations of the numeric_limits
 *   class template
 *
 * $Id: 18.numeric.special.float.cpp 648752 2008-04-16 17:01:56Z faridz $
 *
 ***********************************************************************
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
 * Copyright 2000-2008 Rogue Wave Software, Inc.
 *
 ***********************************************************************/

#include <bsls_platform.h>

#include <rw/_defs.h>

#if defined (__IBMCPP__) && !defined (_RWSTD_NO_IMPLICIT_INCLUSION)
   // disable implicit inclusion to work around
   // a limitation in IBM's VisualAge 5.0.2.0 (see PR#26959)
#  define _RWSTD_NO_IMPLICIT_INCLUSION
#endif


#include <limits>

#include <climits>
#include <cstdlib>   // for atof
#include <cstdio>    // for sprintf

#include <float.h>   // for constants, MSVC _fpclass, _isinf, _isnan
#include <math.h>    // for fpclassify, isinf, isnan

#ifndef _RWSTD_NO_IEEEFP_H
#  include <ieeefp.h>   // for fpclass, isnan
#endif   // _RWSTD_NO_IEEEFP_H

#include <driver.h>

// use FLT_ROUNDS if available, otherwise (e.g., when <ieeefp.h> must
// be #included to bring the macro in), fall back on our own macro
#ifndef FLT_ROUNDS
#  define FLT_ROUNDS   _RWSTD_FLT_ROUNDS
#endif   // FLT_ROUNDS

/***********************************************************************/

// determine whether the architecture is little-endian or big-endian
static const int endian_test = 1;

// extern works around a SunPro 5.4 bug (PR #28124)
extern const bool big_endian = !*(const char*)&endian_test;

/***********************************************************************/

volatile float  flt_zero  = 0;
volatile double dbl_zero  = 0;

#ifndef _RWSTD_NO_LONG_DOUBLE

volatile long double ldbl_zero = 0;

#endif   // _RWSTD_NO_LONG_DOUBLE

/***********************************************************************/

#if defined (_MSC_VER)

const char* fpclass_name (int fpc)
{
    switch (fpc) {
    case _FPCLASS_SNAN: return "_FPCLASS_SNAN";
    case _FPCLASS_QNAN: return "_FPCLASS_QNAN";
    case _FPCLASS_NINF: return "_FPCLASS_NINF";
    case _FPCLASS_NN:   return "_FPCLASS_NN";
    case _FPCLASS_ND:   return "_FPCLASS_ND";
    case _FPCLASS_NZ:   return "_FPCLASS_NZ";
    case _FPCLASS_PZ:   return "_FPCLASS_PZ";
    case _FPCLASS_PD:   return "_FPCLASS_PD";
    case _FPCLASS_PN:   return "_FPCLASS_PN";
    case _FPCLASS_PINF: return "_FPCLASS_PINF";
    }

    static char buf [16];
    std::sprintf (buf, "%#x", fpc);
    return buf;
}

#elif defined (_RWSTD_OS_SUNOS)

const char* fpclass_name (fpclass_t fpc)
{
    switch (fpc) {
    case FP_SNAN:    return "FP_SNAN";
    case FP_QNAN:    return "FP_QNAN";
    case FP_NINF:    return "FP_NINF";
    case FP_PINF:    return "FP_PINF";
    case FP_NDENORM: return "FP_NDENORM";
    case FP_PDENORM: return "FP_PDENORM";
    case FP_NZERO:   return "FP_NZERO";
    case FP_PZERO:   return "FP_PZERO";
    case FP_NNORM:   return "FP_NNORM";
    case FP_PNORM:   return "FP_PNORM";

    }

    static char buf [16];
    std::sprintf (buf, "%#x", fpc);
    return buf;
}

#elif defined (fpclassify)   // C99 classification

const char* fpclass_name (int fpc)
{
    switch (fpc) {
#  ifdef FP_INFINITE
    case FP_INFINITE:  return "FP_INFINITE";
#  endif
#  ifdef FP_NAN
    case FP_NAN:       return "FP_NAN";
#  endif
#  ifdef FP_NORMAL
    case FP_NORMAL:    return "FP_NORMAL";
#  endif
#  ifdef FP_SUBNORMAL
    case FP_SUBNORMAL: return "FP_SUBNORMAL";
#  endif
#  ifdef FP_ZERO
    case FP_ZERO:      return "FP_ZERO";
#  endif
    default: break;
    }

    static char buf [16];
    std::sprintf (buf, "%#x", fpc);
    return buf;
}

#endif

/***********************************************************************/

template <class FloatT>
void test_infinity (FloatT inf, FloatT max, const char *tname)
{
    if (!std::numeric_limits<FloatT>::traps) {

        // infinity must be even greater than the maximum value
        rw_assert (inf > max, 0, __LINE__,
                   "numeric_limits<%s>::infinity() > "
                   "numeric_limits<%s>::max()", tname, tname);

        // multiplying infinity by anything other than 0.0 yields infinity
        rw_assert (inf == inf * inf, 0, __LINE__,
                   "numeric_limits<%s>::infinity()", tname);
    }

#ifdef _MSC_VER

    const int fpc = _fpclass (inf);
    rw_assert (_FPCLASS_PINF == fpc, 0, __LINE__,
               "_fpclass (numeric_limits<%s>::infinity()) == "
               "%d (_FPCLASS_PINF), got %d (%s)",
               tname, _FPCLASS_PINF, fpc, fpclass_name (fpc));

#elif defined (_RWSTD_OS_SUNOS)

    rw_assert (!finite (inf), 0, __LINE__,
               "finite (numeric_limits<%s>::infinity()) == 0, "
               "got non-zero", tname);

    const fpclass_t fpc = fpclass (inf);
    rw_assert (FP_PINF == fpclass (inf), 0, __LINE__,
               "fpclass (numeric_limits<%s>::infinity()) == %d (FP_PINF), "
               "got %d (%s)", tname, FP_PINF, fpc, fpclass_name (fpc));

#else

#  ifdef isinf

    rw_assert (isinf (inf), 0, __LINE__,
               "isinf (numeric_limits<%s>::infinity()) != 0, got 0",
               tname);

#  endif   // isinf

#  ifdef fpclassify

    const int fpc = fpclassify (inf);
    rw_assert (FP_INFINITE == fpc, 0, __LINE__,
               "fpclassify(numeric_limits<%s>::infinity()) == "
               "%d (FP_INFINITE), got %d (%s)", tname,
               FP_INFINITE, fpc, fpclass_name (fpc));

#  endif   // fpclassify

#endif

}

/***********************************************************************/

template <class FloatT>
void test_quiet_NaN (FloatT qnan, FloatT qnan2, const char *tname)
{
    // NAN never compares equal to self or any other number
    rw_assert (!(qnan == qnan2), 0, __LINE__,
               "numeric_limits<%s>::quiet_NaN() != "
               "numeric_limits<%1$s>::quiet_NaN()",
               tname);

    const FloatT inf = std::numeric_limits<FloatT>::infinity ();

    rw_assert (!(qnan == inf), 0, __LINE__,
               "numeric_limits<%s>::quiet_NaN() != "
               "numeric_limits<%1$s>::infinity()",
               tname);

    rw_assert (!(qnan == -inf), 0, __LINE__,
               "numeric_limits<%s>::quiet_NaN() != "
               "-numeric_limits<%1$s>::infinity()",
               tname);

#ifdef _MSC_VER

    rw_assert (0 != _isnan (qnan), 0, __LINE__,
               "_isnan(numeric_limits<%s>::quiet_NaN()) != 0, got 0",
               tname);

    const int fpc = _fpclass (qnan);
    rw_assert (_FPCLASS_QNAN == fpc, 0, __LINE__,
               "_fpclass(numeric_limits<%s>::quiet_NaN()) == "
               "%d (_FPCLASS_QNAN), got %d (%s)",
               tname, _FPCLASS_QNAN, fpc, fpclass_name (fpc));

#elif defined (_RWSTD_OS_SUNOS)

    rw_assert (!finite (qnan), 0, __LINE__,
               "finite(numeric_limits<%s>::quiet_NaN()) == 0, "
               "got non-zero", tname);

    const fpclass_t fpc = fpclass (qnan);
    rw_assert (FP_QNAN == fpc, 0, __LINE__,
               "fpclass(numeric_limits<%s>::infinity()) == %d (FP_QNAN), "
               "got %d (%s)", tname, FP_QNAN, fpc, fpclass_name (fpc));

#else

#  ifdef isnan

    rw_assert (0 != isnan (qnan), 0, __LINE__,
               "isnan(numeric_limits<%s>::quiet_NaN()) != 0, got 0",
               tname);

#  endif   // isnan

#  ifdef fpclassify

    const int fpc = fpclassify (qnan);
    rw_assert (FP_NAN == fpc, 0, __LINE__,
               "fpclassify(numeric_limits<%s>::quiet_NaN()) == "
               "%d (FP_NAN), got %d (%s)", tname,
               FP_NAN, fpc, fpclass_name (fpc));

#  endif   // fpclassify

#endif

}

/***********************************************************************/

template <class FloatT>
void test_signaling_NaN (FloatT snan, FloatT snan2, const char *tname)
{
    // NAN never compares equal to self or any other number
    rw_assert (!(snan == snan2), 0, __LINE__,
               "numeric_limits<%s>::signaling_NaN() != "
               "numeric_limits<%1$s>::signaling_NaN()",
               tname);

    const FloatT inf = std::numeric_limits<FloatT>::infinity ();

    rw_assert (!(snan == inf), 0, __LINE__,
               "numeric_limits<%s>::signaling_NaN() != "
               "numeric_limits<%1$s>::infinity()",
                tname);

    rw_assert (!(snan == -inf), 0, __LINE__,
               "numeric_limits<%s>::signaling_NaN() != "
               "-numeric_limits<%1$s>::infinity()",
               tname);

#ifdef _MSC_VER

    rw_assert (0 != _isnan (snan), 0, __LINE__,
               "_isnan (numeric_limits<%s>::signaling_NaN()) != 0, got 0",
               tname);

    const int fpc = _fpclass (snan);
    rw_assert (_FPCLASS_SNAN == fpc, 0, __LINE__,
               "_fpclass(numeric_limits<%s>::signaling_NaN()) == "
               "%d (_FPCLASS_SNAN), got %d (%s)",
               tname, _FPCLASS_SNAN, fpc, fpclass_name (fpc));

#elif defined (_RWSTD_OS_SUNOS)

    rw_assert (!finite (snan), 0, __LINE__,
               "finite(numeric_limits<%s>::signaling_NaN()) == 0, "
               "got non-zero", tname);

    const fpclass_t fpc = fpclass (snan);
    rw_assert (FP_SNAN == fpc, 0, __LINE__,
               "fpclass(numeric_limits<%s>::signaling_NaN()) == %d "
               "(FP_SNAN), got %d (%s)", tname,
               FP_SNAN, fpc, fpclass_name (fpc));
#else

#  ifdef isnan

    rw_assert (0 != isnan (snan), 0, __LINE__,
               "isnan(numeric_limits<%s>::signaling_NaN()) != 0, got 0",
               tname);

#  endif   // isnan

#  ifdef fpclassify

    const int fpc = fpclassify (snan);
    rw_assert (FP_NAN == fpc, 0, __LINE__,
               "fpclassify(numeric_limits<%s>::signaling_NaN()) == "
               "%d (FP_NAN), got %d (%s)", tname,
               FP_NAN, fpc, fpclass_name (fpc));

#  endif   // fpclassify

#endif

}

/***********************************************************************/

template <class T>
struct limits_values;

#if    defined (__alpha) && !defined (__linux__) \
    && !defined (__VMS) && defined (__DECCXX) && defined (__PURE_CNAME)

extern "C" unsigned int read_rnd ();
#define read_rnd   read_rnd

#endif   // __alpha && !__linux__ && !__VMS && __DECCXX && __PURE_CNAME

/***********************************************************************/

_RWSTD_SPECIALIZED_CLASS
struct limits_values<float>
{
    static bool is_specialized () { return true; }

    static float (min) () { return FLT_MIN; }
    static float (max) () { return FLT_MAX; }

    static int digits () { return FLT_MANT_DIG; }
    static int digits10 () { return FLT_DIG; }
    static bool is_signed () { return  0.0f - 1.0f < 0.0f; }
    static bool is_integer () { return false; }
    static bool is_exact () { return false; }
    static int radix () { return FLT_RADIX; }

    static float epsilon () { return FLT_EPSILON; }

    static float round_error () {

#ifdef __osf__
        int rounding = FLT_ROUNDS;
#  ifdef read_rnd
        if (rounding == std::round_indeterminate)
            rounding = _RWSTD_STATIC_CAST (int, read_rnd ());
#  endif   //  read_rnd

        switch (rounding) {
        case std::round_toward_zero:
        case std::round_toward_infinity:
        case std::round_toward_neg_infinity:
            return 1.0f;
        default:
            return 0.5f;
        }
#else
        return 0.5f;
#endif

    }

    static int min_exponent () { return FLT_MIN_EXP; }
    static int min_exponent10 () { return FLT_MIN_10_EXP; }
    static int max_exponent () { return FLT_MAX_EXP; }
    static int max_exponent10 () { return FLT_MAX_10_EXP; }

    static bool has_infinity () {
        return true;
    }

    static bool has_quiet_NaN () {

#if defined (NAN)
        // 7.12, p5 of C99:
        // The macro NAN is defined if and only if the implementation
        // supports quiet NaNs for the float type. It expands to a
        // constant expression of type float representing a quiet NaN.
        return true;
#else
        return true;
#endif

    }


    static bool has_signaling_NaN () {

#if defined (FLT_SNAN)
        return true;
#else
#  ifndef _RWSTD_NO_SIGNALING_NAN
        return true;
#  else
        return false;
#  endif   // _RWSTD_NO_SIGNALING_NAN
#endif

    }


    static std::float_denorm_style has_denorm () {

#if defined (_AIX) \
    || defined (__hpux) \
    || defined (__osf__)
        return std::denorm_present;
#else
        return std::denorm_indeterminate;
#endif
    }


    static bool has_denorm_loss () { return false; }


    static float infinity () {

#if defined (FLT_INFINITY)
        return FLT_INFINITY;
#elif defined (_RWSTD_NO_DBL_TRAPS)

        float inf = float (std::atof ("inf"));
        if (inf > FLT_MAX)
            return inf;

        inf = 1.0f / flt_zero;

        return inf;

#else
        const union {
            char bits [sizeof (float)];
            float val;
        } val = {
            _RWSTD_FLT_INF_BITS
        };

        return val.val;
#endif

    }


    static float quiet_NaN () {

#if defined (NAN)
        return NAN;
#elif defined (FLT_QNAN)
        return FLT_QNAN;
#elif defined (_RWSTD_NO_DBL_TRAPS)

        float nan = float (std::atof ("nan"));

        if (nan != float (std::atof ("nan")))
            return nan;

        nan = float (dbl_zero / flt_zero);

        return nan;

#else
        const union {
            char bits [sizeof (float)];
            float val;
        } val = {
            _RWSTD_FLT_QNAN_BITS
        };

        return val.val;

#endif

    }


    static float signaling_NaN () {

#if defined (FLT_SNAN)
        return FLT_SNAN;
#elif defined (_RWSTD_NO_DBL_TRAPS)

        union {
            float val;
            char bits [sizeof (float)];
        } snan;

        snan.val = infinity ();

#  ifdef __hpux

        if (big_endian) {
            snan.bits [sizeof snan.val - 2] = '\xc0';
        }
        else {
            snan.bits [1] = '\xc0';
        }

#  else   // if !defined (__hpux)

        // convert infinity into a signaling NAN
        // (toggle any bit in signifcand)
        if (big_endian) {
            snan.bits [sizeof snan.val - 1] |= 1;
        }
        else {
            snan.bits [0] |= 1;
        }

#  endif   // __hpux

        return snan.val;

#else

        const union {
            char bits [sizeof (float)];
            float val;
        } val = {
            _RWSTD_FLT_SNAN_BITS
        };

        return val.val;

#endif

    }


    static float denorm_min () {

#if defined (__FLT_DENORM_MIN__)
        // gcc 3.x predefined macro
        return __FLT_DENORM_MIN__;
#elif defined (__osf__)
        const unsigned short pos_denorm [] = { 0x0001, 0x0000 };
        return *_RWSTD_REINTERPRET_CAST (const float*, pos_denorm);
#else
        // assume IEEE 754

        static union {
            float denorm_min;
            char denorm_min_bits [sizeof (float)];
        } u;

        if (big_endian)
            u.denorm_min_bits [sizeof (float) - 1] = '\001';
        else
            u.denorm_min_bits [0] = '\001';

        return u.denorm_min;

#endif

    }


    static bool is_iec559 () {

#if defined (__osf__)

        // Tru64 UNIX with Compaq C++: IEC 559 support is enabled
        // by specifying the -ieee flag on the compiler command line

#  ifdef _IEEE_FP
        return true;
#  else
        return false;
#  endif   //  _IEEE_FP
#else
        return true;
#endif

    }


    static bool is_bounded () { return true; }
    static bool is_modulo () { return false; }

    static bool traps () { return true; }
    static bool tinyness_before () { return false; }


    static std::float_round_style round_style () {

#if defined (_AIX)
        return std::round_to_nearest;
#else
        return _RWSTD_STATIC_CAST (std::float_round_style, FLT_ROUNDS);
#endif
    }

};

/***********************************************************************/


_RWSTD_SPECIALIZED_CLASS
struct limits_values<double>
{
    static bool is_specialized () { return true; }

    static double (min)() { return DBL_MIN; }
    static double (max)() { return DBL_MAX; }

    static int digits () { return DBL_MANT_DIG; }
    static int digits10 () { return DBL_DIG; }
    static bool is_signed () { return  0.0 - 1.0 < 0.0; }
    static bool is_integer () { return false; }
    static bool is_exact () { return false; }
    static int radix () { return FLT_RADIX; }

    static double epsilon () { return DBL_EPSILON; }


    static double round_error () {

#ifdef __osf__
        int rounding = FLT_ROUNDS;
#  ifdef read_rnd
        if (rounding == std::round_indeterminate)
            rounding = _RWSTD_STATIC_CAST (int, read_rnd ());
#  endif   //  read_rnd
        switch (rounding) {
        case std::round_toward_zero:
        case std::round_toward_infinity:
        case std::round_toward_neg_infinity:
            return 1.0;
        default:
            return 0.5;
        }
#else
        return 0.5;
#endif

    }


    static int min_exponent () { return DBL_MIN_EXP; }
    static int min_exponent10 () { return DBL_MIN_10_EXP; }
    static int max_exponent () { return DBL_MAX_EXP; }
    static int max_exponent10 () { return DBL_MAX_10_EXP; }


    static bool has_infinity () {
        return true;
    }


    static bool has_quiet_NaN () {
        return true;
    }


    static bool has_signaling_NaN () {

#if defined DBL_SNAN
        return true;
#else
#  ifndef _RWSTD_NO_SIGNALING_NAN
        return true;
#  else
        return false;
#  endif   // _RWSTD_NO_SIGNALING_NAN
#endif

    }


    static std::float_denorm_style has_denorm () {
#if defined (_AIX) \
    || defined (__hpux) \
    || defined (__osf__)
        return std::denorm_present;
#else
        return std::denorm_indeterminate;
#endif
    }


    static bool has_denorm_loss () { return false; }


    static double infinity () {

#if defined (DBL_INFINITY)
        return DBL_INFINITY;
#elif defined (_RWSTD_NO_DBL_TRAPS)

        double inf = std::atof ("inf");
        if (inf > DBL_MAX)
            return inf;

        inf = 1.0 / dbl_zero;

        return inf;

#else
        const union {
            char bits [sizeof (double)];
            double val;
        } val = {
            _RWSTD_DBL_INF_BITS
        };

        return val.val;
#endif

    }


    static double quiet_NaN () {

#if defined (NAN)
        return NAN;
#elif defined (DBL_QNAN)
        return DBL_QNAN;
#elif defined (_RWSTD_NO_DBL_TRAPS)

        double nan = std::atof ("nan");

        if (nan != std::atof ("nan"))
            return nan;

        nan = flt_zero / dbl_zero;

        return nan;

#else
        const union {
            char bits [sizeof (double)];
            double val;
        } val = {
            _RWSTD_DBL_QNAN_BITS
        };

        return val.val;

#endif

    }


    static double signaling_NaN () {

#if defined (DBL_SNAN)
        return DBL_SNAN;
#elif defined (_RWSTD_NO_DBL_TRAPS)

        union {
            double val;
            char bits [sizeof (double)];
        } snan;

        snan.val = infinity ();

#  ifdef __hpux

        if (big_endian) {
            snan.bits [sizeof snan.val - 2] = '\xf8';
        }
        else {
            snan.bits [1] = '\xf8';
        }

#  else   // if !defined (__hpux)

        // convert infinity into a signaling NAN
        // (toggle any bit in signifcand)
        if (big_endian) {
            snan.bits [sizeof snan.val - 1] |= 1;
        }
        else {
            snan.bits [0] |= 1;
        }

#  endif   // __hpux

        return snan.val;

#else

        const union {
            char bits [sizeof (double)];
            double val;
        } val = {
            _RWSTD_DBL_SNAN_BITS
        };

        return val.val;

#endif

    }


    static double denorm_min () {

#if defined (__DBL_DENORM_MIN__)
        // gcc 3.x predefined macro
        return __DBL_DENORM_MIN__;
#elif defined (__osf__)
        const unsigned short pos_denorm [] = { 0x0001, 0x0000 };
        return *_RWSTD_REINTERPRET_CAST (const double*, pos_denorm);
#else
        // assume IEEE 754

        static union {
            double denorm_min;
            char denorm_min_bits [sizeof (double)];
        } u;

        if (big_endian)
            u.denorm_min_bits [sizeof (double) - 1] = '\001';
        else
            u.denorm_min_bits [0] = '\001';

        return u.denorm_min;

#endif

    }


    static bool is_iec559 () {

#if defined (__osf__)
#  ifdef _IEEE_FP
        return true;
#  else
        return false;
#  endif   //  _IEEE_FP
#else
        return true;
#endif

    }


    static bool is_bounded () { return true; }
    static bool is_modulo () { return false; }

    static bool traps () { return true; }
    static bool tinyness_before () { return false; }


    static std::float_round_style round_style () {

#if defined (_AIX)
        return std::round_to_nearest;
#else
        return _RWSTD_STATIC_CAST (std::float_round_style, FLT_ROUNDS);
#endif

    }

};

/***********************************************************************/


_RWSTD_SPECIALIZED_CLASS
struct limits_values<long double>
{
    static bool is_specialized () { return true; }

    static long double (min)() { return LDBL_MIN; }
    static long double (max)() { return LDBL_MAX; }

    static int digits () { return LDBL_MANT_DIG; }
    static int digits10 () { return LDBL_DIG; }
    static bool is_signed () { return  0.0 - 1.0 < 0.0; }
    static bool is_integer () { return false; }
    static bool is_exact () { return false; }
    static int radix () { return FLT_RADIX; }

    static long double epsilon () { return LDBL_EPSILON; }

    static long double round_error () {

#ifdef __osf__
        int rounding = FLT_ROUNDS;
#  ifdef read_rnd
        if (rounding == std::round_indeterminate)
            rounding = _RWSTD_STATIC_CAST (int, read_rnd ());
#  endif   //  read_rnd
        switch (rounding) {
        case std::round_toward_zero:
        case std::round_toward_infinity:
        case std::round_toward_neg_infinity:
            return 1.0;
        default:
            return 0.5;
        }
#else
        return 0.5;
#endif

    }

    static int min_exponent () { return LDBL_MIN_EXP; }
    static int min_exponent10 () { return LDBL_MIN_10_EXP; }
    static int max_exponent () { return LDBL_MAX_EXP; }
    static int max_exponent10 () { return LDBL_MAX_10_EXP; }

    static bool has_infinity () {
        return true;
    }

    static bool has_quiet_NaN () {
        return true;
    }


    static bool has_signaling_NaN () {

#if defined (LDBL_SNAN)
        return true;
#else
#  ifndef _RWSTD_NO_SIGNALING_NAN
        return true;
#  else
        return false;
#  endif   // _RWSTD_NO_SIGNALING_NAN
#endif

    }


    static std::float_denorm_style has_denorm () {
#if defined (_AIX) \
    || defined (__hpux) \
    || defined (__osf__)
        return std::denorm_present;
#else
        return std::denorm_indeterminate;
#endif
    }

    static bool has_denorm_loss () { return false; }


    static long double infinity () {

#if defined (LDBL_INFINITY)
        return LDBL_INFINITY;
#elif defined (_RWSTD_NO_DBL_TRAPS)

        long double inf = std::atof ("inf");
        if (inf > LDBL_MAX)
            return inf;

        inf = (long double)1.0 / ldbl_zero;

        return inf;

#else
        const union {
            char bits [sizeof (long double)];
            long double val;
        } val = {
            _RWSTD_LDBL_INF_BITS
        };

        return val.val;
#endif

    }


    static long double quiet_NaN () {

#if defined (NAN)
        return NAN;
#elif defined (LDBL_QNAN)
        return LDBL_QNAN;
#elif defined (_RWSTD_NO_DBL_TRAPS)

        long double nan = std::atof ("nan");

        if (nan != (long double)std::atof ("nan"))
            return nan;

        nan = ldbl_zero / ldbl_zero;

        return nan;

#else
        const union {
            char bits [sizeof (long double)];
            long double val;
        } val = {
            _RWSTD_LDBL_QNAN_BITS
        };

        return val.val;

#endif

    }


    static long double signaling_NaN () {

#if defined (LDBL_SNAN)
        return LDBL_SNAN;
#elif defined (_RWSTD_NO_DBL_TRAPS)

        union {
            long double val;
            char bits [sizeof (long double)];
        } snan;

        snan.val = infinity ();

#  ifdef __hpux

        if (big_endian) {
            snan.bits [sizeof snan.val - 3] = '\x80';
        }
        else {
            snan.bits [2] = '\x80';
        }

#  else   // if !defined (__hpux)

        // convert infinity into a signaling NAN
        // (toggle any bit in signifcand)
        if (big_endian) {
            snan.bits [sizeof snan.val - 1] |= 1;
        }
        else {
            snan.bits [0] |= 1;
        }

#  endif   // __hpux

        return snan.val;

#else

        const union {
            char bits [sizeof (long double)];
            long double val;
        } val = {
            _RWSTD_LDBL_SNAN_BITS
        };

        return val.val;

#endif

    }


    static long double denorm_min () {

#if defined (__LDBL_DENORM_MIN__)
        // gcc 3.x predefined macro
        return __LDBL_DENORM_MIN__;
#elif defined (__osf__)
        const unsigned short pos_denorm [] = { 0x0001, 0x0000 };
        return *_RWSTD_REINTERPRET_CAST (const long double*, pos_denorm);
#else
        // assume IEEE 754

        static union {
            long double denorm_min;
            char denorm_min_bits [sizeof (long double)];
        } u;

        if (big_endian)
            u.denorm_min_bits [sizeof (long double) - 1] = '\001';
        else
            u.denorm_min_bits [0] = '\001';

        return u.denorm_min;
#endif

    }


    static bool is_iec559 () {
#if defined (__osf__)
#  ifdef _IEEE_FP
        return true;
#  else
        return false;
#  endif   //  _IEEE_FP
#else
        return true;
#endif
    }

    static bool is_bounded () { return true; }
    static bool is_modulo () { return false; }

    static bool traps () { return true; }
    static bool tinyness_before () { return false; }

    static std::float_round_style round_style () {
#if defined (_AIX)
        return std::round_to_nearest;
#else
        return _RWSTD_STATIC_CAST (std::float_round_style, FLT_ROUNDS);
#endif
    }
};

/***********************************************************************/

template <class FloatT>
void test_limits (FloatT, const char *tname, const char *fmt)
{
    typedef std::numeric_limits<FloatT> FLim;
    typedef limits_values<FloatT>       FVal;

    // self-test: assert that endianness is correctly computed
#if defined (__alpha)
    rw_warn (!big_endian, 0, __LINE__, "alpha is typically little-endian");
#elif defined (_AIX)
    rw_warn (big_endian, 0, __LINE__, "AIX is big-endian");
#elif defined (__hpux) && defined (_BIG_ENDIAN)
    rw_warn (big_endian, 0, __LINE__, "HP-UX is big-endian");
#elif defined (__i386__)
    rw_warn (!big_endian, 0, __LINE__, "Intel x86 is little-endian");
#elif defined (__sparc)
    rw_warn (big_endian, 0, __LINE__, "SPARC is big-endian");
#elif defined (_WIN64)
    rw_warn (!big_endian, 0, __LINE__, "WIN64 is little-endian");
#elif defined (_WIN32)
    rw_warn (!big_endian, 0, __LINE__, "WIN32 is little-endian");
#endif

#ifndef _RWSTD_NO_STATIC_CONST_MEMBER_INIT

#  if !defined (__EDG__) || __EDG_VERSION__ > 245

#if defined(BSLS_PLATFORM__CMP_SUN)

#    define CHECK_CONST(const_int)   \
         enum { e = const_int };

#else  // BSLS_PLATFORM__CMP_SUN

#    define CHECK_CONST(const_int)   \
         enum { e = const_int };     \
         (void)&const_int

#endif  // BSLS_PLATFORM__CMP_SUN

#  else   // if EDG eccp < 3.0
    // working around an EDG eccp 2.4x ICE (not in 3.0)
#    define CHECK_CONST(const_int)                        \
         switch (const_int) { case const_int: break; };   \
         (void)&const_int
#  endif   // __EDG__
#else
   // static const integral members must be usable
   //  as constant integral expressions
#  define CHECK_CONST(const_int)      enum { e = const_int }
#endif   // _RWSTD_NO_STATIC_CONST_MEMBER_INIT


// compare against a computed value
#define VERIFY_DATA(member) do {                                        \
        /* verify that member is a constant integral expression */      \
        CHECK_CONST (FLim::member);                                     \
        /* verify value */                                              \
        rw_assert (FLim::member == FVal::member (), 0, __LINE__,        \
                   "numeric_limits<%s>::" #member " == %i, got %i",     \
                   tname, FVal::member (), FLim::member);               \
    } while (0)

// compare against a constant
#define VERIFY_CONST(member, value) do {                                 \
        /* verify that member if a constant integral expression */       \
        CHECK_CONST (FLim::member);                                      \
        /* verify value */                                               \
        rw_assert (FLim::member == value, 0, __LINE__,                   \
                   "numeric_limits<%s>::" #member " == %i, got %i",      \
                   tname, FVal::member (), value);                       \
    } while (0)

// account for NaN != NaN
#define VERIFY_FUNCTION(member) do {                                       \
        /* verify function signature */                                    \
        FloatT (*pf)() _PTR_THROWS(()) = &FLim::member;                    \
        _RWSTD_UNUSED (pf);                                                \
        /* verify value */                                                 \
        rw_assert (   FLim::member () == FVal::member ()                   \
                   || FLim::member () != FLim::member ()                   \
                   && FVal::member () != FVal::member (),                  \
                   0, __LINE__,                                            \
                   "numeric_limits<%s>::" #member "() == %{@}, got %{@}",  \
                   tname, fmt, FVal::member (), fmt, FLim::member ());     \
    } while (0)

#define VERIFY_SIGNATURE(member) do {                   \
        /* verify function signature */                 \
        FloatT (*pf)() _PTR_THROWS(()) = &FLim::member; \
        _RWSTD_UNUSED (pf);                             \
    } while (0)

#undef min
#undef max

    VERIFY_FUNCTION (min);           // 18.2.1.2, p1
    VERIFY_FUNCTION (max);           //           p4
    VERIFY_DATA (digits);            //           p6
    VERIFY_DATA (digits10);          //           p9
    VERIFY_DATA (is_signed);         //           p11
    VERIFY_DATA (is_integer);        //           p13
    VERIFY_DATA (is_exact);          //           p15
    VERIFY_DATA (radix);             //           p17

    VERIFY_FUNCTION (epsilon);       // 18.2.1.2, p19
    VERIFY_FUNCTION (round_error);   //           p22

    VERIFY_DATA (min_exponent);      // 18.2.1.2, p23
    VERIFY_DATA (min_exponent10);    //           p25
    VERIFY_DATA (max_exponent);      //           p27
    VERIFY_DATA (max_exponent10);    //           p29

    VERIFY_FUNCTION (infinity);      // 18.2.1.2, p42

    if (FLim::has_infinity)
        test_infinity (FLim::infinity (), FLim::max (), tname);

    if (std::numeric_limits<FloatT>::traps)   //  p45
        VERIFY_SIGNATURE (quiet_NaN);
    else {
        VERIFY_FUNCTION (quiet_NaN);

        if (FLim::has_quiet_NaN)
            test_quiet_NaN (FLim::quiet_NaN (),
                            FLim::quiet_NaN (), tname);
    }

    if (std::numeric_limits<FloatT>::traps)   //  p47
        VERIFY_SIGNATURE (signaling_NaN);
    else {
        VERIFY_FUNCTION (signaling_NaN);

        if (FLim::has_signaling_NaN)
            test_signaling_NaN (FLim::signaling_NaN (),
                                FLim::signaling_NaN (), tname);
    }

    if (std::numeric_limits<FloatT>::traps) { //  p49
        // denorm min may trap (e.g., Tru64 UNIX on Alpha
        // with Compaq C++ without the -ieee compiler flag)
        VERIFY_SIGNATURE (denorm_min);
    }
    else {
        VERIFY_FUNCTION (denorm_min);
    }

#ifdef _MSC_VER

    if (sizeof (FloatT) > sizeof (float)) {
        // for doubles and long doubles only, verify that denorm_min
        // is properly classified

        const FloatT denorm_min = FLim::denorm_min ();

        const int fpc = _fpclass (double (denorm_min));

        rw_assert (_FPCLASS_PD == fpc, 0, __LINE__,
                   "_fpclass(numeric_limits<%s>::denorm_min()) == "
                   "%d (_FPCLASS_PD), got %d (%s)",
                   tname, _FPCLASS_PD, fpc, fpclass_name (fpc));
    }

#endif   // _MSC_VER

    VERIFY_DATA (is_iec559);         // 18.2.1.2, p52
    VERIFY_DATA (is_bounded);        //           p54
    VERIFY_DATA (is_modulo);         //           p56

    VERIFY_DATA (tinyness_before);   // 18.2.1.2, p61
    VERIFY_DATA (round_style);       //           p63

    if (!FLim::traps) {              // 18.2.1.2, p59
        // invalid floating point operations must not trap

        // IEEE 754 specifies that the following operations
        // return NaN unless they trap:
        //     1.   sqrt (n); n < 0
        //     2.   rem (x, 0.0), rem (inf, x)
        //     3.   0 * inf
        //     4.   0.0 / 0.0, inf / inf
        //     5.   inf - inf (when both inf have the same sign)

        // use a variable (not a literal) to prevent warnings
        volatile FloatT zero = 0;

        // compute infinity (division may trap)
        const FloatT inf = FLim::infinity ();

        FloatT nan = zero * inf;   // #3
        nan = zero / zero;         // #4a
        nan = inf / inf;           // #4b
        nan = inf - inf;           // #5

        _RWSTD_UNUSED (inf);
        _RWSTD_UNUSED (nan);
    }

    if (FLim::is_iec559) {
        // 18.2.1.2, p33
        VERIFY_CONST (has_infinity, true);

        // 18.2.1.2, p36
        VERIFY_CONST (has_quiet_NaN, true);

        // 18.2.1.2, p39
        VERIFY_CONST (has_signaling_NaN, true);

        // 18.2.1.2, p40 - 42
        VERIFY_DATA (has_denorm);
        VERIFY_DATA (has_denorm_loss);
    }
    else {
        // 18.2.1.2, p34 - 42
        VERIFY_DATA (has_quiet_NaN);
        VERIFY_DATA (has_signaling_NaN);
        VERIFY_DATA (has_denorm);
        VERIFY_DATA (has_denorm_loss);
    }
}

/***********************************************************************/

static int
run_test (int, char**)
{
#undef VERIFY_CONST
#define VERIFY_CONST(x, value) do {                     \
        enum { e = x };                                 \
        rw_assert (value == x, 0, __LINE__,             \
                   #x " == %d, got %d", value, x);      \
    } while (0)

    // verify values of constants in 18.2.1.3
    VERIFY_CONST (std::round_indeterminate,      -1);
    VERIFY_CONST (std::round_toward_zero,         0);
    VERIFY_CONST (std::round_to_nearest,          1);
    VERIFY_CONST (std::round_toward_infinity,     2);
    VERIFY_CONST (std::round_toward_neg_infinity, 3);

    // verify values of constants in 18.2.1.4
    VERIFY_CONST (std::denorm_indeterminate, std::float_denorm_style (-1));
    VERIFY_CONST (std::denorm_absent,        std::float_denorm_style (0));
    VERIFY_CONST (std::denorm_present,       std::float_denorm_style (1));

    test_limits (float (), "float", "%g");
    test_limits (double (), "double", "%g");

#ifndef _RWSTD_NO_LONG_DOUBLE

   // working around a SunPro 5.3 ICE (PR #25968)
#  if !defined (__SUNPRO_CC) || __SUNPRO_CC > 0x530

    const char fmt[] = "%" _RWSTD_LDBL_PRINTF_PREFIX "g";
    test_limits ((long double)0.0, "long double", fmt);

#  endif   // SunPro > 5.3

#endif   //_RWSTD_NO_LONG_DOUBLE

    return 0;
}

/***********************************************************************/

int main (int argc, char *argv[])
{
    // working around a bogus EDG eccp warning (see PR #25679)
    _RWSTD_UNUSED (big_endian);

    return rw_test (argc, argv, __FILE__,
                    "numeric.special",
                    "floating specializations",
                    run_test,
                    "",
                    (void*)0);
}
