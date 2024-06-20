// bsls_nameof.cpp                                                    -*-C++-*-
#include <bsls_nameof.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>

#include <stdio.h>
#include <cstring>
#include <cctype>

namespace {
namespace u {

#define U_SAFE_COPY(dstBuf, srcBuf) do {                                      \
        enum { k_STATIC_ASSERT =                                              \
                           1 / (sizeof(srcBuf) <= sizeof(dstBuf) ? 1 : 0) };  \
        std::strcpy(dstBuf, srcBuf);                                          \
    } while (false)

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
    //:
    //: o 'std::...string<...'
    //:
    //: o 'bsl::...string<...'
    //:
    //: o 'std::...::pmr::string<...'
    //:
    //: o 'std::...string_view<...'
    //:
    //: o 'bsl::...string_view<...'
    //:
    //: o anonymous namespace (which may begin with '(', '{', or '<'
    //:
    //: o anonymous namespace ("::" on Solaris CC).  This is the only pattern
    //:   that begins with ':', and it is important that we be careful
    //:   substituting them.
    //:
    //: o in C++03, templates sometimes end with "> >" so we substitute ">" for
    //:   " >" and then all patterns after that assume nested templates end
    //:   with ">>".
{
    const std::size_t patternLen     = std::strlen(pattern);
    const std::size_t replacementLen = std::strlen(replacement);
    BSLS_ASSERT_OPT(patternLen >= replacementLen);

    static const unsigned char colon = ':';

    for (char *pc = buffer; (pc = std::strstr(pc, pattern)); ++pc) {
        if (buffer < pc) {
            // If we are translating a pattern beginning with 'std' or 'bsl',
            // make sure that the previous character wasn't an alphanum or ':',
            // in which case we are in the middle of an id and we should just
            // skp the match.  If we are translating " >" to ">", we expect
            // the previous character to be '>' so that will not be skipped.

            const unsigned char prevChar = pc[-1];
            if (std::isalnum(prevChar) || colon == prevChar) {
                continue;
            }
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
    const char uselessPreamble[] = {    "__cdecl BloombergLP::bsls::NameOf<" };
# else
    static
    const char uselessPreamble[] = { "__thiscall BloombergLP::bsls::NameOf<" };
# endif
#else
    static
    const char uselessPreamble[] = {            "BloombergLP::bsls::NameOf<" };
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

    u::substitute(buffer, " >", ">");

    // In this series of '#ifdef/#elif/#else', we set 'stringName' to be the
    // full template expression describing 'std::string', and we pass
    // 'stringName' to 'substitute' to match those expressions to translate
    // them to non-template shorter 'typedef's.  We arrange it so that when we
    // emerge from these '#ifdef's,
    //: 1 'stringName' is the expression to be translated to 'std::string'
    //:
    //: 2 'stringViewName' is the expression to be translated to
    //:   'std::string_view', and
    //:
    //: 3 'anonymous' is the name the compiler gives to the anonymous
    //:   namespace, which we will use 'substitute' to translate to nothing.

#if defined(BSLS_PLATFORM_CMP_SUN) && !defined(BSLS_PLATFORM_CMP_GNU)
    char stringName[] = { "std::basic_string<char, std::char_traits<char>,"
                                                    " std::allocator<char>>" };

    char stringViewName[] = "std::basic_string_view<char,"
                                                    " std::char_traits<char>>";
    static const char anonymous[] = { "::" };
#elif defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
# if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING)
    char stringName[] = { "std::__cxx11::basic_string<char,"
                                       " std::char_traits<char>, std::pmr::"
                                       "polymorphic_allocator<char>>" };

    u::substitute(buffer, stringName, "std::pmr::string");

    U_SAFE_COPY(stringName, "std::basic_string<char,"
                            " std::char_traits<char>, std::allocator<char>>");
# else
    char stringName[] = {   "std::basic_string<char,"
                            " std::char_traits<char>, std::allocator<char>>" };
# endif

    // We iterate to handle various combinations of parts of the definition
    // being in the "std::" or "bsl::" namespaces, and the closing of the
    // template being either "> >" or ">>".

    char typedefName[] = { "std::string" };

    u::substitute(buffer,      stringName,           typedefName);

    u::substitute(stringName,  "std::basic",         "bsl::basic");
    u::substitute(typedefName, "std::",              "bsl::");
    u::substitute(buffer,      stringName,           typedefName);

    u::substitute(stringName,  "std::alloc",         "bsl::alloc");
    u::substitute(buffer,      stringName,           typedefName);

    u::substitute(stringName,  "std::",              "bsl::");
    u::substitute(buffer,      stringName,           typedefName);

    U_SAFE_COPY(stringName,    "std::__cxx11::basic_string<char>");
    u::substitute(buffer,      stringName,           "std::string");
    u::substitute(stringName,  "std::__cxx11::",     "std::");

    char stringViewName[] = "std::__cxx11::basic_string_view<char>";
    u::substitute(buffer,     stringViewName,        "std::string_view");
    u::substitute(stringViewName,  "std::__cxx11::", "std::");

# if defined(BSLS_PLATFORM_CMP_CLANG)
    static const char anonymous[] = { "(anonymous namespace)::" };
# else
    static const char anonymous[] = { "{anonymous}::" };
# endif
#elif defined(BSLS_PLATFORM_CMP_IBM)
    char stringName[] = { "std::basic_string<char,std::char_traits<char>,"
                                                     "std::allocator<char>>" };

    char stringViewName[] = "std::basic_string_view<char,"
                                                     "std::char_traits<char>>";
    static const char anonymous[] = { "<unnamed>::" };
#elif defined(BSLS_PLATFORM_CMP_MSVC)
    u::substitute(buffer,    "struct ", "");
    u::substitute(buffer,    "class ",  "");
    u::substitute(buffer,    "union ",  "");

    char stringName[] = { "std::basic_string<char,std::char_traits<char>,"
                          "std::pmr::polymorphic_allocator<char>>" };
    u::substitute(buffer,     stringName,      "std::pmr::string");

    U_SAFE_COPY(stringName, "std::basic_string<char,std::char_traits<char>,"
                            "std::allocator<char>>");

    char stringViewName[] = "std::basic_string_view<char,"
                                                     "std::char_traits<char>>";

    static const char anonymous[] = { "`anonymous namespace'::" };
#elif BSLS_PLATFORM_OS_DARWIN
    char stringName[] = { "std::__1::basic_string<char>" };

    u::substitute(buffer,     stringName,   "std::string");
    u::substitute(stringName, "std::__1::",  "std::");

    char stringViewName[] = "std::__1::basic_string_view<char>";
    u::substitute(buffer,     stringViewName, "std::string_view");
    u::substitute(stringViewName, "std::__1::",  "std::");

    static const char anonymous[] = { "(anonymous namespace)::" };
#endif

    u::substitute(buffer,     "BloombergLP::", "");

    // ... At this point 'stringName' is the templated version of 'std::string'

    u::substitute(buffer,     stringName,      "std::string");
    u::substitute(stringName, "std::basic",    "bsl::basic");
    u::substitute(buffer,     stringName,      "bsl::string");
    u::substitute(stringName, "std::alloc",    "bsl::alloc");
    u::substitute(buffer,     stringName,      "bsl::string");
    u::substitute(stringName, "std::",         "bsl::");
    u::substitute(buffer,     stringName,      "bsl::string");
    u::substitute(stringName, "bsl::",         "");
    u::substitute(buffer,     stringName,      "string");

    // ... and 'stringViewName' is the templated version of 'std::string_view',
    // note that on platforms that don't support 'std::string_view', we go on
    // to translate 'bsl::string_view'

    u::substitute(buffer, stringViewName, "std::string_view");
    u::substitute(stringViewName, "std::basic", "bsl::basic");
    u::substitute(buffer, stringViewName, "bsl::string_view");
    u::substitute(stringViewName, "std::",      "bsl::");
    u::substitute(buffer, stringViewName, "bsl::string_view");

    // ... and 'anonymous' is prefix the compiler gives to types that are in
    // the anonymous namespace

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
