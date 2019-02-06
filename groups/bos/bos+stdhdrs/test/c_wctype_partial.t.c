#define __need_wint_t // needed first when partially including 'wctype.h'
#include <wchar.h>

#define __need_iswxxx
#include <wctype.h>

#include <wctype.h>

int main()
{
    // verify that a symbol from 'wctype.h' is actually available.
    int i = iswalpha((wint_t)0);
    (void)i;

    return 0;
}

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
