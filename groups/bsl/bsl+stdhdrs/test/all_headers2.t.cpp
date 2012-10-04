#ifndef BSL_OVERRIDES_STD
#define BSL_OVERRIDES_STD
#endif
// Include all standard headers in reverse alphabetical order

#include <vector>
#include <valarray>
#include <utility>
#include <typeinfo>
#include <strstream>
#include <string>
#include <streambuf>
#include <stdexcept>
#include <stack>
#include <sstream>
#include <slist>
#include <set>
#include <queue>
#include <ostream>
#include <numeric>
#include <new>
#include <memory>
#include <map>
#include <locale>
#include <list>
#include <limits>
#include <iterator>
#include <istream>
#include <iostream>
#include <iosfwd>
#include <ios>
#include <iomanip>
#include <hash_set>
#include <hash_map>
#include <functional>
#include <fstream>
#include <exception>
#include <deque>
#include <cwctype>
#include <cwchar>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstddef>
#include <cstdarg>
#include <csignal>
#include <csetjmp>
//#include <complex>
#include <cmath>
#include <clocale>
#include <climits>
#include <ciso646>
#include <cfloat>
#include <cerrno>
#include <cctype>
#include <cassert>
#include <bitset>
#include <algorithm>

#ifndef std
# error std was expected to be a macro
#endif
int main()
{
    std::size_t a = 0;
    std::pair<std::size_t,int> b(a, 0);
    return 0;
}

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
