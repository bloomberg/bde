#include <bsl_ctime.h>
#include <ctime>
#ifdef std
#   error std was not expected to be a macro
#endif
namespace std { }
int main() { return 0; }
