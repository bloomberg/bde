#ifndef BSL_OVERRIDES_STD
#define BSL_OVERRIDES_STD
#endif
// Include all standard headers in reverse alphabetical order

#include <vector>
#include <valarray>
#include <utility>
#include <unordered_set>
#include <unordered_map>
#include <typeinfo>
#include <strstream>
#include <string>
#include <streambuf>
#include <stdexcept>
#include <stack>
#include <sstream>
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

#ifdef BSL_TEST_CPP11_HEADERS

#include <typeindex>
#include <tuple>
#include <thread>
#include <system_error>
#include <scoped_allocator>
#include <regex>
#include <ratio>
#include <random>
#include <mutex>
#include <initializer_list>
#include <future>
#include <cuchar>
#include <ctgmath>
#include <cstdint>
#include <cstdbool>
#include <condition_variable>
#include <cinttypes>
#include <chrono>
#include <cfenv>
#include <atomic>
#include <array>

#endif


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
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
