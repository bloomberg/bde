#include <bsls_platform.h>

//#ifdef (BSLS_PLATFORM_OS_LINUX && BSLS_PLATFORM_CMP_GNU) || \
       //(BSLS_PLATFORM_OS_SOLARIS)
#if 1
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wcast-qual"

#ifdef __GNUC__
#  define _WCHAR_T
#else
#  define __thread
#  define __QNX__
#endif


extern "C" {
  #include "LIBRARY/float128/dpml_four_over_pi.c"
}

#  pragma GCC diagnostic pop
#endif
