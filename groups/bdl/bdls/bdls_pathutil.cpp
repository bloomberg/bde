// bdls_pathutil.cpp                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdls_pathutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdls_pathutil_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_algorithm.h>
#include <bsl_cctype.h>
#include <bsl_climits.h>
#include <bsl_cstddef.h>
#include <bsl_cstring.h>

namespace BloombergLP {

// LOCAL CONSTANTS
const char SEPARATOR =
#ifdef BSLS_PLATFORM_OS_WINDOWS
    '\\'
#else
    '/'
#endif
    ;

// STATIC HELPER FUNCTIONS
static
void findFirstNonSeparatorChar(size_t     *resultOffset,
                               const char *path,
                               int         length = -1)
    // Load into the specified 'result' the offset of the first *non*
    // path-separator character in the specified 'path' (e.g., the first
    // character not equal to '/' on unix platforms).  Optionally specify
    // 'length' indicating the length of 'path'.  If 'length' is not supplied,
    // call 'strlen' on 'path' to determine its length.

{
    BSLS_ASSERT(resultOffset);
    BSLS_ASSERT(path);

    if (0 > length) {
        length = bsl::strlen(path);
    }
    *resultOffset = 0;

#ifdef BSLS_PLATFORM_OS_WINDOWS
    // Windows paths come in three flavors: local (LFS), UNC, and Long
    // UNC (LUNC).
    // LFS: root consists of a drive letter followed by a colon (the name part)
    //      and then zero or more separators (the directory part).  E.g.,
    //      "c:\hello.txt" or "c:tmp"
    // UNC: root consists of two separators followed by a hostname and
    //      separator (the name part), and then a shared folder followed by one
    //      or more separators (the directory part).  e.g.,
    //      \\servername\sharefolder\output\test.t
    //
    // LUNC: root consists of "\\?\".  Then follows either "UNC" followed by
    //       a UNC root, or an LFS root.  The "\\?\" is included as part of
    //       the root name.  e.g.,
    //      \\?\UNC\servername\folder\hello or \\?\c:\windows\test
#define UNCW_PREFIX "\\\\?\\"
#define UNCW_PREFIXLEN 4
#define UNCW_UNCPREFIX UNCW_PREFIX "UNC\\"
#define UNCW_UNCPREFIXLEN (UNCW_PREFIXLEN + 4)
    int rootNameEnd = 0;

    if (2 <= length && bsl::isalpha(static_cast<unsigned char>(path[0]))
                                                           && ':' == path[1]) {
        // LFS.  Root name ends after the ':'.

        *resultOffset = rootNameEnd = 2;
    }
    else if (4 <= length
          && SEPARATOR == path[0]
          && SEPARATOR == path[1] && '?' != path[2]) {
        //UNC (we require 4 because the hostname must be nonempty and there
        //must then be another separator; then at least a nonempty
        //share directory).  Root name ends after the separator that follows
        //the hostname; root directory ends after the separator that follows
        //the share name.

        const char *rootNameEndPtr = bsl::find(path + 3,
                                               path + length,
                                               SEPARATOR);

        if (rootNameEndPtr != path + length) {
            ++rootNameEndPtr;
        }
        rootNameEnd = (size_t)(rootNameEndPtr - path);

        const char *resultOffsetPtr = bsl::find(path + rootNameEnd,
                                                path + length,
                                                SEPARATOR);
        if (resultOffsetPtr != path + length) {
            ++resultOffsetPtr;
        }
        *resultOffset = (size_t)(resultOffsetPtr - path);
    }
    else if (UNCW_PREFIXLEN < length
           && 0 == strncmp(path, UNCW_PREFIX, UNCW_PREFIXLEN)) {
        //LUNC.  Now determine whether it is LFS or UNC

        if (UNCW_PREFIXLEN+2 <= length
           && bsl::isalpha(static_cast<unsigned char>(path[UNCW_PREFIXLEN]))
           && ':' == path[UNCW_PREFIXLEN+1]) {
            //LFS.

            *resultOffset = rootNameEnd = UNCW_PREFIXLEN+2;
        }
        else if (UNCW_UNCPREFIXLEN < length
                && 0 == strncmp(path, UNCW_UNCPREFIX, UNCW_UNCPREFIXLEN)) {
            //UNC.

            const char *rootNameEndPtr =
                 bsl::find(path + UNCW_UNCPREFIXLEN, path + length, SEPARATOR);

            if (rootNameEndPtr != path + length) {
                ++rootNameEndPtr;
            }
            rootNameEnd = (unsigned)(rootNameEndPtr - path);

            const char *resultOffsetPtr = bsl::find(path + rootNameEnd,
                                               path + length,
                                               SEPARATOR);
            if (resultOffsetPtr != path + length) {
                ++resultOffsetPtr;
            }
            *resultOffset = (unsigned)(resultOffsetPtr - path);
        }
    }
#endif

    while ((int) *resultOffset < length && SEPARATOR == path[*resultOffset]) {
        ++*resultOffset;
    }
}

static
void findFirstNonSeparatorChar(int *result, const char *path, int length = -1)
    // Load into the specified 'result' the offset of the first *non*
    // path-separator character in the specified 'path' (e.g., the first
    // character not equal to '/' on unix platforms).  Optionally specify
    // 'length' indicating the length of 'path'.  If 'length' is not supplied,
    // call 'strlen' on 'path' to determine its length.  Note that the
    // behavior of this routine is identical to that of the above
    // 'findFirstNonSeparatorChar' routine, except this one takes an 'int *'
    // instead of a 'size_t *'.
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(path);

    size_t resultOffset;
    findFirstNonSeparatorChar(&resultOffset, path, length);
    BSLS_ASSERT(INT_MAX > resultOffset);
    *result = resultOffset;
}

static
const char *leafDelimiter(const char *path, int rootEnd, int length = -1)
    // Return the position of the beginning of the basename of the specified
    // 'path'.  The basename will not be found within the specified 'rootEnd'
    // characters from teh beginning of 'path'.  If the optionally specified
    // 'length' is not given, assume 'path' is null-terminated.  Note that this
    // file may be a directory.  Also note that trailing separators are
    // ignored.
{
    BSLS_ASSERT(path);

    if (0 > length) {
        length = bsl::strlen(path);
    }

    while (0 < length && SEPARATOR == path[length - 1]) {
        --length;
    }

    const char *position;
    for (position = path + length - 1;
         position > path + rootEnd && *position != SEPARATOR;
         --position) {
    }
    return position;
}

static inline
const char *leafDelimiter(const bsl::string &path, int rootEnd)
{
    return leafDelimiter(path.c_str(), rootEnd, path.length());
}

namespace bdls {
                              // ===============
                              // struct PathUtil
                              // ===============

// CLASS METHODS
int PathUtil::appendIfValid(bsl::string              *path,
                            const bslstl::StringRef&  filename)
{
    BSLS_ASSERT(path);

    // If 'filename' refers to characters in 'path', create a copy of
    // 'filename' and call 'appendIfValid' so 'path' may be safely resized.

    const char *pathEnd = path->c_str() + path->length();
    if (filename.data() >= path->c_str() && filename.data() < pathEnd) {
        bsl::string nonAliasedFilename(filename.data(), filename.length());
        return appendIfValid(path, nonAliasedFilename);               // RETURN
    }

    // If 'filename' is an absolute path, return an error status.

    size_t nonSeparatorOffset;
    findFirstNonSeparatorChar(&nonSeparatorOffset,
                              filename.data(),
                              filename.length());
    if (0 != nonSeparatorOffset) { // absolute path
        return -1;                                                    // RETURN
    }

    // Create an 'adjustedFilenameLength' that suppresses trailing separators
    // in 'filename'.

    bsl::size_t adjustedFilenameLength = filename.length();
    for (; adjustedFilenameLength > 0; --adjustedFilenameLength) {
        if (SEPARATOR != filename[adjustedFilenameLength - 1]) {
            break;
        }
    }

    // Erase trailing separators from 'path'.

    if (!path->empty()) {
        bsl::size_t lastChar = path->find_last_not_of(SEPARATOR);

        // If 'path' is *all* separator characters (i.e., no non-seperator was
        // found), the resulting 'path' should be 1 separator character.

        lastChar = (lastChar == bsl::string::npos) ? 0 : lastChar;
        if (lastChar != path->length()) {
            path->erase(path->begin() + lastChar + 1, path->end());
        }
    }

    appendRaw(path, filename.data(), adjustedFilenameLength);
    return 0;
}

void PathUtil::appendRaw(bsl::string *path,
                         const char  *filename,
                         int          length,
                         int          rootEnd)
{
    BSLS_ASSERT(path);
    BSLS_ASSERT(filename);

    if (0 > length) {
        length = bsl::strlen(filename);
    }

    if (0 < length) {
        if (0 > rootEnd) {
            findFirstNonSeparatorChar(&rootEnd, path->c_str(), path->length());
        }
        if (hasLeaf(path->c_str(), rootEnd)
         || (rootEnd > 0 && (*path)[rootEnd-1] != SEPARATOR)) {
            path->push_back(SEPARATOR);
        }
        path->append(filename, length);
    }
}

int PathUtil::popLeaf(bsl::string *path, int rootEnd)
{
    BSLS_ASSERT(path);

    if (0 > rootEnd) {
        findFirstNonSeparatorChar(&rootEnd, path->c_str(), path->length());
    }

    if (!hasLeaf(path->c_str(), rootEnd)) {
        return -1;                                                    // RETURN
    }

    path->erase(leafDelimiter(*path, rootEnd) - path->begin());
    return 0;
}

int PathUtil::getLeaf(bsl::string              *leaf,
                      const bslstl::StringRef&  path,
                      int                       rootEnd)
{
    BSLS_ASSERT(leaf);

    int length = path.length();

    if (0 > rootEnd) {
        findFirstNonSeparatorChar(&rootEnd, path.data(), length);
    }

    if (!hasLeaf(path, rootEnd)) {
        return -1;                                                    // RETURN
    }
    leaf->clear();
    const char *lastSeparator = leafDelimiter(path.data(), rootEnd, length);
    BSLS_ASSERT(lastSeparator != path.data() + length);

    while (0 < length && SEPARATOR == path[length-1]) {
        --length;
    }

    leaf->append(
               *lastSeparator == SEPARATOR ? lastSeparator + 1 : lastSeparator,
               path.data() + length);
    return 0;
}

int PathUtil::getDirname(bsl::string              *dirname,
                         const bslstl::StringRef&  path,
                         int                       rootEnd)
{
    BSLS_ASSERT(dirname);

    if (0 > rootEnd) {
        findFirstNonSeparatorChar(&rootEnd, path.data(), path.length());
    }

    if (!hasLeaf(path, rootEnd)) {
        return -1;                                                    // RETURN
    }

    dirname->clear();
    const char *lastSeparator = leafDelimiter(path.data(),
                                              rootEnd,
                                              path.length());
    if (lastSeparator == path.data()) {
        //nothing to do

        return 0;                                                     // RETURN
    }
    dirname->append(path.data(), lastSeparator);
    return 0;
}

int PathUtil::getRoot(bsl::string              *root,
                      const bslstl::StringRef&  path,
                      int                       rootEnd)
{
    BSLS_ASSERT(root);

    if (0 > rootEnd) {
        findFirstNonSeparatorChar(&rootEnd, path.data(), path.length());
    }

    if (isRelative(path, rootEnd)) {
        return -1;                                                    // RETURN
    }

    root->clear();
    root->append(path.data(), path.data() + rootEnd);
    return 0;
}

bool PathUtil::isAbsolute(const bslstl::StringRef& path, int rootEnd)
{
    if (0 > rootEnd) {
        findFirstNonSeparatorChar(&rootEnd, path.data(), path.length());
    }

    return rootEnd > 0;
}

bool PathUtil::isRelative(const bslstl::StringRef& path, int rootEnd)
{
    if (0 > rootEnd) {
        findFirstNonSeparatorChar(&rootEnd, path.data(), path.length());
    }

    return 0 == rootEnd;
}

int PathUtil::getRootEnd(const bslstl::StringRef& path)
{
    int result;
    findFirstNonSeparatorChar(&result, path.data(), path.length());
    return result;
}

bool PathUtil::hasLeaf(const bslstl::StringRef& path, int rootEnd)
{
    int length = path.length();
    if (0 > rootEnd) {
        findFirstNonSeparatorChar(&rootEnd, path.data(), length);
    }

    while (0 < length && SEPARATOR == path[length-1]) {
        --length;
    }

    return rootEnd < length;
}
}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
