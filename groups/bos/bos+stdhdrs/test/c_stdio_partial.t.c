#include <features.h> // needed because it is assumed to already be there when
                      // errno.h is partially included

#define __need_FILE
#include <stdio.h>

#define __need___FILE
#include <stdio.h>

#include <stdio.h>

int main()
{
    // verify that a symbol from 'stdio.h' is actually available.
    ssize_t i = 17;
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
