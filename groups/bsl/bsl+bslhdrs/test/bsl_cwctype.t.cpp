#include <bsl_cwctype.h>
#include <cwctype>
#ifdef std
#   error std was not expected to be a macro
#endif
namespace std { }
int main() { return 0; }
