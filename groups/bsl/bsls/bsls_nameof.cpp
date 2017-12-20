// bsls_nameof.cpp                                                    -*-C++-*-
#include <bsls_nameof.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_assert.h>

#include <cstring>

#include <ctype.h>

namespace {
namespace u {

void substitute(char       *buffer,
                const char *pattern,
                const char *replacement)
    // Do a global substitution replacing all instances of the specified
    // 'pattern' with the specified 'remainder' in the specified 'buffer'.  The
    // behavior is undefined unless the length of 'replacement' is less than or
    // equal to that of 'pattern'.  Note that this function contains special
    // logic so that it will only do substitutions at points where the
    // appropriate pattern is likely to start.
    //
    // Several types of patterns are to be matched:
    //: o namespace 'BloombergLP::'
    //: o 'std::...string...'
    //: o anonymous namespace (which may begin with '(', '{', or '<'
    //: o anonymous nameppace ("::" on Solaris CC).  This is the only pattern
    //:   that begins with ':', and it is important that we be careful
    //:   substituting them.
    //: o gap at end of nested template (" >") which is the only pattern that
    //:   begins with space, and also the only pattern that may immediately
    //:   follow an id.
{
    const char startPat = pattern[0];
    std::size_t patternLen;
    std::size_t replacementLen;
    bool firstTime = true;

    const char semicolon = ';';     // We're assuming ';' (semicolon) will
                                    // never occur in a type name.

    for (char *pc = buffer; (pc = std::strstr(pc, pattern)); ++pc) {
        const char prevChar = buffer < pc ? pc[-1] : semicolon;
        if (' ' != startPat && !std::strchr(";<([{, ", prevChar)) {
            continue;
        }

        if (firstTime) {
            firstTime = false;

            patternLen     = std::strlen(pattern);
            replacementLen = std::strlen(replacement);
            BSLS_ASSERT(patternLen >= replacementLen);
        }

        std::memmove(pc, replacement, replacementLen);
        const char *remainder = pc + patternLen;
        std::memmove(pc + replacementLen,
                     remainder,
                     std::strlen(remainder) + 1);
    }
}

}  // close namespace u
}  // close unnamed namespace

namespace BloombergLP {
namespace bsls {

// CLASS METHOD
const char *NameOf_Base::initBuffer(char       *buffer,
                                    const char *functionName)
    // Initialize the specified '*buffer' with the type name contained in the
    // specified 'functionName', where 'functionName' is the function name of
    // the 'NameOf' constructor.
{
    // On all platforms, 'functionName' will start with 'uselessPreamble',
    // which will not make it into the final buffer, so we know that the final
    // buffer can be trimmed of this length.

#if defined(BSLS_PLATFORM_CMP_MSVC)
# if defined(BSLS_PLATFORM_CPU_64_BIT)
    static
    char uselessPreamble[] = { "__cdecl BloombergLP::bsls::NameOf<" };
# else
    static
    char uselessPreamble[] = { "__thiscall BloombergLP::bsls::NameOf<" };
# endif
#else
    static
    char uselessPreamble[] = {            "BloombergLP::bsls::NameOf<" };
#endif

    static
    const char compileTimeAssertArray[
                sizeof(uselessPreamble) == k_USELESS_PREAMBLE_LEN + 1] = { 0 };
        // This will fail to compile if expression in '[]' is not 'true'.  We
        // do our compile-time assert this way as 'BSLMF_ASSERT' is not
        // accessible from this component.

    (void) compileTimeAssertArray;    // silence 'unused' warnings.

    // 'buffer' is guaranteed to be at least as long as 'functionName' minus
    // 'k_USELESS_PERAMBLE_LEN' including the '\0', except in the case of
    // Solaris CC, where we must truncate it to a buffer.

    if (std::strncmp(uselessPreamble,
                     functionName,
                     k_USELESS_PREAMBLE_LEN)) {
        // Preamble not present.  Result might not fit in 'buffer', and our
        // manipulation of the string probably won't work.

        return functionName;                                          // RETURN
    }

#if defined(BSLS_PLATFORM_CMP_SUN)
    // Must truncate 'functionName' to buffer size.

    std::strncpy(buffer,
                 functionName + k_USELESS_PREAMBLE_LEN,
                 k_BUF_SIZE_SOLARIS_CC);
    buffer[k_BUF_SIZE_SOLARIS_CC - 1] = 0;
    const char *pc = buffer;
#else
    const char *pc = functionName + k_USELESS_PREAMBLE_LEN;
#endif
    const char *end;

#if defined(BSLS_PLATFORM_CMP_GNU) && !defined(BSLS_PLATFORM_CMP_CLANG)
    // If typename == 'int',

    // functionName:
    //           "BloombergLP::bsls::NameOf<TYPE>::NameOf() [with TYPE = int]"
    //
    // pc:                                 "TYPE>::NameOf() [with TYPE = int]"

    static const char openMatch[] = { ">::NameOf() [with TYPE = " };
    enum { k_OPEN_MATCH_LEN = sizeof(openMatch) - 1 };

    const char *open = std::strstr(pc, openMatch);
    end = pc + std::strlen(pc);
    if (!open || end <= pc || ']' != *--end) {
        // 'functionName' is not what we expect.  Our manipulation probably
        // won't work.

        return functionName;                                          // RETURN
    }
    pc = open + k_OPEN_MATCH_LEN;

#elif defined(BSLS_PLATFORM_CMP_CLANG)
    // If typename = 'int'
    // functionName: "BloombergLP::bsls::NameOf<int>::NameOf() [TYPE = int]"
    // pc:                                     "int>::NameOf() [TYPE = int]"

    static const char openMatch[] = { ">::NameOf() [TYPE = " };
    enum { k_OPEN_MATCH_LEN = sizeof(openMatch) - 1 };

    const char *open = std::strstr(pc, openMatch);
    end = pc + std::strlen(pc);
    if (!open || end <= pc || ']' != *--end) {
        // 'functionName' is not what we expect.  Our manipulation probably
        // won't work.

        return functionName;                                          // RETURN
    }
    pc = open + k_OPEN_MATCH_LEN;

#elif defined(BSLS_PLATFORM_CMP_MSVC)
    // If typename = 'int'
    // functionName: "__thiscall BloombergLP::bsls::NameOf<int>::NameOf(void)"
    // pc:                                                "int>::NameOf(void)"

    static const char tail[] = { ">::NameOf(void)" };
    enum { k_TAIL_LEN = sizeof(tail) - 1 };

    end = pc + std::strlen(pc);
    if (!end || end-pc <= k_TAIL_LEN || std::strcmp(end -= k_TAIL_LEN, tail)) {
        // 'functionName' is not what we expect.  Our manipulation probably
        // won't work.

        return functionName;                                          // RETURN
    }

#else
    // Sun CC, Aix.

    // If typename == 'int',
    // functionName: "BloombergLP::bsls::NameOf<int>::NameOf()"
    // pc:                                     "int>::NameOf()"

    static const char tail[] = { ">::NameOf()" };
    enum { k_TAIL_LEN = sizeof(tail) - 1 };

    end = pc + std::strlen(pc);
    if (!end || end-pc <= k_TAIL_LEN || std::strcmp(end -= k_TAIL_LEN, tail)) {
        // 'functionName' is not what we expect.  Our manipulation probably
        // won't work.

        return functionName;                                          // RETURN
    }

#endif

    end = end > pc ? end : pc + std::strlen(pc);
    while (end > pc && ' ' == end[-1]) {
        --end;
    }

    std::memmove(buffer, pc, end - pc);    // Note that on Solaris CC, 'pc' and
    buffer[end - pc] = 0;                  // 'end' will point into 'buffer',
                                           // which shouldn't be a problem.
    BSLS_ASSERT(std::strlen(buffer) == static_cast<std::size_t>(end - pc));

#if   defined(BSLS_PLATFORM_CMP_SUN) && !defined(BSLS_PLATFORM_CMP_GNU)
    char stringName[] = { "std::basic_string<char, std::char_traits<char>,"
                                                    " std::allocator<char>>" };

    static const char anonymous[] = { "::" };
#elif defined(BSLS_PLATFORM_CMP_GNU) && !defined(BSLS_PLATFORM_CMP_CLANG)
# if BSLS_PLATFORM_CMP_VERSION < 40502
    char stringName[] = { "std::basic_string<char, std::char_traits<char>,"
                                                    " std::allocator<char>>" };

    static const char anonymous[] = { "<unnamed>::" };
# elif BSLS_PLATFORM_CMP_VERSION < 40601
    char stringName[] = { "std::basic_string<char>" };

    static const char anonymous[] = { "<unnamed>::" };
# elif BSLS_PLATFORM_CMP_VERSION < 50000
    char stringName[] = { "std::basic_string<char>" };

    static const char anonymous[] = { "{anonymous}::" };
# else
    char stringName[] = { "std::__cxx11::basic_string<char>" };

    u::substitute(buffer,     stringName,       "std::string");
    u::substitute(stringName, "std::__cxx11::", "std::");

    static const char anonymous[] = { "{anonymous}::" };
# endif
#elif defined(BSLS_PLATFORM_CMP_IBM)
    char stringName[] = { "std::basic_string<char,std::char_traits<char>,"
                                                     "std::allocator<char>>" };

    static const char anonymous[] = { "<unnamed>::" };
#elif defined(BSLS_PLATFORM_CMP_MSVC)
    u::substitute(buffer,    "struct ", "");
    u::substitute(buffer,    "class ",  "");
    u::substitute(buffer,    "union ",  "");

    char stringName[] = { "std::basic_string<char,std::char_traits<char>,"
                                                     "std::allocator<char>>" };

    static const char anonymous[] = { "`anonymous namespace'::" };
#else
    // Linux clang, and Darwin clang

# if BSLS_PLATFORM_OS_DARWIN
    char stringName[] = { "std::__1::basic_string<char>" };

    u::substitute(buffer,     stringName,   "std::string");
    u::substitute(stringName, "std::__1::",  "std::");
# else
    // Linux clang

    char stringName[] = { "std::basic_string<char>" };
# endif
    static const char anonymous[] = { "(anonymous namespace)::" };
#endif

    u::substitute(buffer,     " >",            ">");

    u::substitute(buffer,     stringName,      "std::string");
    u::substitute(stringName, "std::basic",    "bsl::basic");
    u::substitute(buffer,     stringName,      "bsl::string");
    u::substitute(stringName, "std::alloc",    "bsl::alloc");
    u::substitute(buffer,     stringName,      "bsl::string");
    u::substitute(stringName, "std::",         "bsl::");
    u::substitute(buffer,     stringName,      "bsl::string");
    u::substitute(stringName, "bsl::",         "");
    u::substitute(buffer,     stringName,      "string");

    u::substitute(buffer,     "BloombergLP::", "");
    u::substitute(buffer,     anonymous,       "");

    return buffer;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
