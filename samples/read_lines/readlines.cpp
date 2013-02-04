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

    // Continue processing the read data
}
