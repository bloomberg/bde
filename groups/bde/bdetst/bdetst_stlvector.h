// Include header files of components that this component depends on. 
// This is required for testing, so build tool recompiles when there are 
// changes to these components. Build tool will not recompile if included 
// header files of these components only in the test files and not in the
// component header file.

#include <vector>

#include <bdema_strallocator.h>                 // for testing only
#include <bdema_bufferallocator.h>              // for testing only
#include <bdema_testallocator.h>                // for testing only
#include <bdema_testallocatorexception.h>       // for testing only

