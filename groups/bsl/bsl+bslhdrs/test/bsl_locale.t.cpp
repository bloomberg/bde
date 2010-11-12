#include <bsl_locale.h>
#include <locale>
#ifdef std
#   error std was not expected to be a macro
#endif
namespace std { }
int main() { return 0; }
