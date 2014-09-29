#include <bsl_istream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bufferallocator.h>

void readLines(bsl::istream& inputStream)
{
    const int BUFFER_SIZE = 1024;
    char      buffer[BUFFER_SIZE];

    BufferAllocator          allocator(buffer, BUFFER_SIZE);
    bsl::vector<bsl::string> lines(&allocator);

    while (inputStream) {
        bsl::string input(&allocator);

        bsl::getline(inputStream, input);

        lines.push_back(input);
    }

    // Continue processing the read data.
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
