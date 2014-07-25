#include <bsls_platform.h>

#if BSLS_PLATFORM_OS_LINUX && BSLS_PLATFORM_CMP_GNU
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include "LIBRARY/float128/dpml_ux_log.c"
#pragma GCC diagnostic pop
#endif
