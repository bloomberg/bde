// bsls_bslsourcenameparserutil.cpp                                   -*-C++-*-
#include <bsls_bslsourcenameparserutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_bslsourcenameparserutil.h>

#include <bsls_platform.h>

#include <string.h>

namespace {

// ============================================================================
//                         Generic Helper Functions
// ----------------------------------------------------------------------------

                            // Text Comparison

template <size_t CLEN>
inline
bool endsWith(const char *end, const char (&suffix)[CLEN])
    // Return 'true' if the 'CLEN - 1' characters leading up to the specified
    // 'end', 'end' not included, are equal to the specified null-terminated
    // 'suffix' without its null terminator.  The behavior is undefined unless
    // 'end' points after a buffer of at least 'CLEN - 1' characters.
{
    const size_t k_LEN = CLEN - 1;

    return 0 == memcmp(end - k_LEN, suffix, k_LEN);
}

template <size_t CLEN>
inline
bool startsWith(const char *begin, const char (&prefix)[CLEN])
    // Return 'true' if the first 'CLEN - 1' characters from the specified
    // 'begin', 'begin' included, are equal to the specified null-terminated
    // 'prefix' without its null terminator.  The behavior is undefined unless
    // 'begin' points to a buffer of at least 'CLEN - 1' characters.
{
    const size_t k_LEN = CLEN - 1;

    return 0 == memcmp(begin, prefix, k_LEN);
}

                    // Delimiter Counting in Text

static
inline
bool hasAtLeastCountChar(const char *       begin,
                         const char * const end,
                         size_t             count,
                         char               ch)
    // Return 'true' if the specified character 'ch' occurs at least the
    // specified 'count' times within the specified ['begin' .. 'end') range.
    // The behavior is undefined unless 'begin' is not less than 'end'.
{
    for (; begin != end && 0 != count; ++begin) {
        if (*begin == ch) {
            --count;
        }
    }
    return 0 == count;
}

                      // Conditional "Movement" in Text

static
inline
const char *onePastLastChr(const char *const begin, const char *end, char ch)
    // Find the last occurrence of the specified character 'ch', within the
    // specified ['begin' .. 'end') range and return a pointer to the character
    // right after it, or 'end' if it is the last character.  If 'ch' is not
    // found return 'begin'.  The behavior is undefined if 'begin' is greater
    // than 'end'.
{
    while (end != begin) {
        --end;
        if (*end == ch) {
            ++end;
            break;
        }
    }
    return end;
}

               // BDE-Style File Name Specific Helper Functions
static
inline
bool isAlphaTag(char tag)
    // Return 'true' if the specified 'tag' is a lowercase US alphabetic/letter
    // character or return 'false' if it is some other character.
{
#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#endif
    typedef char err_THIS_CODE_NEEDS_ASCII_RUNTIME_CHARACTER_SET[
        'z' - 'a' == 25 ? 42 : -1];  // We cannot use 'BSLMF_ASSERT' here.
#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
# pragma GCC diagnostic pop
#endif

    // 'isalpha' and 'islower' are locale dependent, and we cannot use our
    // locale-independent character categorizing functions because we cannot
    // depend on anything but standard C library at this low level.

    return tag >= 'a' && tag <= 'z';
}

static
inline
bool isTestDriverTag(char tag)
    // Return 'true' if the specified 'tag' is a valid test driver tag
    // character from a source name ('t' or 'g').
{
    return 't' == tag || 'g' == tag;
}

static
inline
bool isTestDriverType(unsigned sourceType)
    // Return 'true' if the specified 'sourceType' is a test driver type
    // according to 'SourceTypes'.
{
    using BloombergLP::bsls::BslSourceNameParserUtil;
    return 0 != (sourceType & BslSourceNameParserUtil::k_MASK_TEST);
}

static
const char *skipPath(const char *filename)
    // Return a pointer to the first character of the specified 'filename'
    // after the last path delimiter.
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    static const char pathSeparators[] = ":/\\";
#else
    static const char pathSeparators[] = "/";
#endif
    const char *delim_p;
    while ((delim_p = strpbrk(filename, pathSeparators))) {
        filename = delim_p + 1;
    }

    return filename;
}

}  // close unnamed namespace

namespace BloombergLP {
namespace bsls {

                      //-------------------------------
                      // struct BslSourceNameParserUtil
                      //-------------------------------

// CLASS METHODS
int BslSourceNameParserUtil::getComponentName(const char **componentNamePtr,
                                              size_t      *componentNameLength,
                                              const char  *sourceName,
                                              unsigned    *type_p)
{
    // Parse by skipping the optional leading path segments and use that
    // position for the 'componentName' pointer if the rest of the parsing
    // succeeds.  Going backwards from the end "cut back" known file extensions
    // by reducing the length.  If no valid file extension found fail parsing
    // (by setting the 3 attributes to zero).  Next, still going backwards look
    // for valid one-letter test driver tags ('.t' for traditional BDE test
    // drivers, and '.g' for Google test).  If a test driver tag is found save
    // its letter for the 'tag' attribute and reduce the length of the
    // component name to "cut it off".  Next look for a split test driver tag
    // that is a dot, followed by zero or more lowercase US English letters,
    // followed by one or two decimal numbers and cut them off if found.  Next
    // look for the generated C++03 variadic subordinate source file suffix
    // that is an underscore followed by "cpp03" and cut it off by reducing the
    // length.  Next look for the subordinate test component suffix that is an
    // underscore followed by the letters "test", followed by zero or more
    // alphanumeric characters (but not an underscore), and cut it off by
    // reducing the length.  If a subordinate test component has been found and
    // it is not a test driver (no tag was saved) set the test tag to '_' to
    // indicate that this file is not supposed to throw or catch assert
    // exceptions(*).  At this point the parsing is successful, set all three
    // attributes to the necessary values and return.
    //
    // (*) A subordinate test component is not allowed to have executable code
    //     in its header or implementation file, only in its test driver, so it
    //     is not a component that itself can throw assert exceptions.
    //
    // A BDE-standard component filename can be described with this
    // regular-expression-like syntax where dot '.' represents itself (so we
    // avoid a lot of backslashes), \d is decimal digit:
    //..
    // "grppkg_component[_test[0-9a-z]*](.h|(.[a-z]*\d\d?)?(.[tg])?.cpp)"
    //..
    // The file name may be preceded by a path that is irrelevant to parsing
    // and if present it ends with a (forward) slash '/' or backslash '\\'.

    sourceName = skipPath(sourceName);
    const char *end = sourceName + strlen(sourceName);

    // Shortest possibly valid file name is "abcx_a.h" (8 characters), with 3
    // mandatory characters for the package group, one character for the
    // package (e.g.: "bsls"), one character for the delimiting underscore, one
    // character for the component name (without the prefix), and 2 characters
    // for the shortest allowed extension (".h").
    if (8 > (end - sourceName)) {
        return -1;                                                    // RETURN
    }

    unsigned  dummyType = 0;
    unsigned& srcType   = (type_p ? *type_p : dummyType);  // Branch only once

    // We ensured earlier that the name is longer than 3 characters, so we can
    // safely check here if it ends in ".cpp":
    if (endsWith(end, ".cpp")) {
        end -= 4;  // Cut off the ".cpp"

        // See if there is a test driver tag, and cut it off if there is.  We
        // can safely read back 2 more characters as we have determined that
        // we have at least 8 (and used up only 4 above).
        const char *cursor = end;
        const char maybeTag = *--cursor;
        const char maybeDot = *--cursor;
        if (('.' == maybeDot) && isTestDriverTag(maybeTag)) {
            // Looks like a test driver
            srcType = ('t' == maybeTag ? k_TTEST : k_GTEST);
            end = cursor;  // Cut off the dot ('.') and the test driver tag
        }
        else {
            srcType = k_IMPL;  // Component implementation (.cpp) file
        }
    }
    else if (endsWith(end, ".h")) {  // A header file
        end -= 2;     // Cut off the ".h"

        srcType = k_HEADER;  // Component header (.h) file
    }
    else {
        // It was not a .cpp file, or a .h file
        return -2;                                                    // RETURN
    }

    // At this point ...
    // ... extension is verified and cut off
    // ... if a valid the test driver tag was present with a ".cpp" extension
    //     that was cut off as well.
    // ... we are looking at the base file name *unless* this is a split test
    //     driver, or a subordinate component.
    //
    // With split test drivers we have a ".D" or ".DD" still at the end of the
    // name (where 'D' is a decimal digit 0-9) to be removed to get to the
    // component name.
    //
    // With subordinate test components we have an "_test" followed by zero or
    // more decimal digits.  Once that is removed, we have the component name.

    if (isTestDriverType(srcType) &&
        hasAtLeastCountChar(sourceName, end, 1, '.'))
    {
        // This may be a numbered test driver part name of a multi-file test
        // driver.  Let's verify and off the .D[D](.t.cpp) part of the file
        // name, where D[D] is a one or two digits decimal number as ASCII.

        const char * const cursor = onePastLastChr(sourceName, end, '.');

        // We allow digits when the tag is only 1 character long.  The "system"
        // (build, CI, etc.) all work with *any* non-zero number of *any* type
        // of characters between the module name and the test driver extension.
        // We cannot make that work because test drivers for file names with
        // tags (such as "abcp_http2curl.m.cpp" would not match to their test
        // driver "abcp_http2curl.m.t.cpp" ("m" file have no test drivers but
        // BDE uses one-character tags to indicate non-component source files,
        // and "m" is well known).

        if ((end - cursor) < 1) {
            // Nothing between two dots, e.g., "abcx_name..t.cpp"
            return -3;                                                // RETURN
        }

        // We do not allow a single lowercase alphanumeric character as a
        // multi-file (split) test driver tag as those are dedicated to be
        // special by BDE naming to be able to differentiate component file
        // names from file names that aren't for components.  We make them look
        // different by parsing the single-letter "extension" tag as part of
        // the component name.  This way "abcx_a.m.cpp" is a different
        // component "abcx_a.m" than "abcx_a.cpp" ("abcx_a").  We need to do
        // the same when parsing test driver source file names, so
        // "abcx_a.m.t.cpp" *must* result in the component name "abcx_a.m", and
        // not "abcx_a" because we have mistaken ".m" for a multi-file (split)
        // test driver part tag.
        //
        // We also don't allow underscore character as the second character of
        // a multi-file test tag that starts with a single lowercase letter
        // (see 'isAlphaTag').  The reason is that we would end up cutting off
        // the whole ".b_cpp03" or ".b_test12" as multi-file test driver tag,
        // when in fact the starting single letter is part of the component
        // name.  Then we would parse say "abcx_a.b_cpp03.t.cpp" as "abcx_a"
        // component name instead of the correct "abcx_a.b", as
        // "abcx_a.b_cpp03.t.cpp" is the C++03 test driver source generated
        // from "abcx_a.b.t.cpp".  And "abcx_a.b.t.cpp" is the test driver for
        // "abcx_a.b' because single letters are special in BDE naming.  Same
        // thing would happen with '_test[^_.]*' suffixes in subordinate test
        // component test driver file names.  We do not verify if the
        // characters are the '_' are indeed a suffix to be removed, as that'd
        // duplicate too much code, and it is a pathological-enough case that
        // its not worth the complications.
        if (!isAlphaTag(*cursor) || ((end - cursor) > 1 && '_' != cursor[1])) {
            srcType |= k_IS_MULTIFILE_TEST;
            end = cursor - 1;
        }
    }

    // Skip "_cpp03" if present at the end of the file name.
    if (5 < (end - sourceName) && endsWith(end, "_cpp03")) {
        srcType |= k_IS_CPP03_GENERATED;
        end -= 6;  // Cut off the "_cpp03"
    }

    // A subordinate or private component, as it has at least 2 underscores.
    if (hasAtLeastCountChar(sourceName, end, 2, '_')) {

        // "Move back before" the last underscore
        const char * const cursor = onePastLastChr(sourceName, end, '_');

        // There are at least 4 characters after the last underscore, and those
        // are "test".  We do not verify that the characters following "_test"
        // are decimal digits or not, as the rest of the build, CI, and other
        // systems do not either.
        if (end - cursor >= 4 && startsWith(cursor, "test")) {
            // Old style subordinate test component
            // 'grppkg_compname_test[^_.]*'.

            // TBD -- Verify if there are dots in the part after "_test"?

            // A subordinate test component is really just a test driver for
            // its main component.
            end = cursor - 1; // Cut off '_test[^_.]*'

            srcType |= k_IS_SUBORDINATE_TEST;
        }
    }

    // Any further "_[a-z0-9]+" we may have left at the end of the string now
    // is part of a package-private component's name, and so it is not special
    // from an assert testing point of view, and don't need to be removed.

    *componentNamePtr    = sourceName;
    *componentNameLength = static_cast<size_t>(end - sourceName);

    return 0;
}

const char *BslSourceNameParserUtil::errorMessage(int errorCode)
{
    switch (errorCode) {
      case  0: return "success";
      case -1: return "filename is too short";
      case -2: return "extension is not recognized (not '.h', or '.cpp')";
      case -3: return "two consecutive dots in file name";
    }
    return "***unknown error code***";
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
