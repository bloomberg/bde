#include <bsl_string.h>
#include <string>

#ifdef std
#   error std was not expected to be a macro
#endif

namespace std { }

int main()
{
    // make sure 'bslstl_StringArgument' is included by 'bsl_string.h'
    BloombergLP::bslstl_StringArgument *p = NULL;
    (void) p;
    return 0;
}

