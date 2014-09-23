#include <bsls_platform.h>

#if 1

#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wcast-qual"

#ifdef __GNUC__
#  define _WCHAR_T
#else
#  define __thread
#  define __QNX__
#endif

#ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
#define BID_BIG_ENDIAN 1
#else
#define BID_BIG_ENDIAN 0
#endif


extern "C" {
  #include "LIBRARY/src/bid128_round_integral.c"
}

#  pragma GCC diagnostic pop

#endif
