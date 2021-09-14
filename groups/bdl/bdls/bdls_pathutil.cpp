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

namespace bdls {

namespace {

struct IsSeparator {
    // This functor is used to determine whether a character is any kind of
    // separator.

    // ACCESSOR
    bool operator()(char ch) const;
        // Return 'true' if the specified 'ch' character is a path separator
        // and return 'false' otherwise.
};

// ACCESSOR
inline
bool IsSeparator::operator()(char ch) const
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    return ch == '\\' || ch == '/';
#else
    return ch == '/';
#endif
}

IsSeparator isSeparator;

// LOCAL CONSTANTS
static
const char k_separators[] =
#ifdef BSLS_PLATFORM_OS_WINDOWS
    "\\/"
#else
    "/"
#endif
    ;

static
void findFirstNonSeparatorChar(size_t     *resultOffset,
                               const char *path,
                               int         length = -1)
    // Load into the specified 'resultOffset' the offset of the first *non*
    // path-separator character in the specified 'path' (e.g., the first
    // character not equal to '/' on unix platforms).  Optionally specify
    // 'length' indicating the length of 'path'.  If 'length' is not supplied,
    // call 'strlen' on 'path' to determine its length.
{
    BSLS_ASSERT(resultOffset);
    BSLS_ASSERT(path);

    if (0 > length) {
        length = static_cast<int>(bsl::strlen(path));
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
    //      "\\servername\sharefolder\output\test.t"
    //
    // LUNC: root starts with "\\?\".  Then follows either "UNC" followed by
    //       a UNC root, or an LFS root.  The "\\?\" is included as part of
    //       the root name.  e.g.,
    //      "\\?\UNC\servername\folder\hello" or "\\?\c:\windows\test"
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
          && isSeparator(path[0])
          && isSeparator(path[1]) && '?' != path[2]) {
        //UNC (we require 4 because the hostname must be nonempty and there
        //must then be another separator; then at least a nonempty
        //share directory).  Root name ends after the separator that follows
        //the hostname; root directory ends after the separator that follows
        //the share name.

        const char *rootNameEndPtr = bsl::find_if(path + 3,
                                                  path + length,
                                                  isSeparator);

        if (rootNameEndPtr != path + length) {
            ++rootNameEndPtr;
        }
        rootNameEnd = (size_t)(rootNameEndPtr - path);

        const char *resultOffsetPtr = bsl::find_if(path + rootNameEnd,
                                                   path + length,
                                                   isSeparator);
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
                 bsl::find_if(path + UNCW_UNCPREFIXLEN,
                              path + length,
                              isSeparator);

            if (rootNameEndPtr != path + length) {
                ++rootNameEndPtr;
            }
            rootNameEnd = (unsigned)(rootNameEndPtr - path);

            const char *resultOffsetPtr = bsl::find_if(path + rootNameEnd,
                                                       path + length,
                                                       isSeparator);
            if (resultOffsetPtr != path + length) {
                ++resultOffsetPtr;
            }
            *resultOffset = (unsigned)(resultOffsetPtr - path);
        }
    }
#endif

    while (static_cast<int>(*resultOffset) < length
           && isSeparator(path[*resultOffset])) {
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
    *result = static_cast<int>(resultOffset);
}

static
const char *leafDelimiter(const char *path, int rootEnd, int length = -1)
    // Return the position of the beginning of the basename of the specified
    // 'path'.  The basename will not be found within the specified 'rootEnd'
    // characters from the beginning of 'path'.  If the optionally specified
    // 'length' is not given, assume 'path' is null-terminated.  Note that this
    // file may be a directory.  Also note that trailing separators are
    // ignored.
{
    BSLS_ASSERT(path);

    if (0 > length) {
        length = static_cast<int>(bsl::strlen(path));
    }

    while (0 < length && isSeparator(path[length - 1])) {
        --length;
    }

    const char *position;
    for (position = path + length - 1;
         position > path + rootEnd && !isSeparator(*position);
         --position) {
    }
    return position;
}

template <class STR_TYPE>
static inline
const char *leafDelimiter(const STR_TYPE &path, int rootEnd)
{
    return leafDelimiter(path.c_str(),
                         rootEnd,
                         static_cast<int>(path.length()));
}

template <class STR_TYPE>
int u_appendIfValid(STR_TYPE *path, const bsl::string_view& filename)
{
    BSLS_ASSERT(path);

    // If 'filename' refers to characters in 'path', create a copy of
    // 'filename' and call 'appendIfValid' so 'path' may be safely resized.

    const char *pathEnd = path->c_str() + path->length();
    if (filename.data() >= path->c_str() && filename.data() < pathEnd) {
        bsl::string nonAliasedFilename(filename.data(), filename.length());
        return PathUtil::appendIfValid(path, nonAliasedFilename);     // RETURN
    }

    // If 'filename' is an absolute path, return an error status.

    size_t nonSeparatorOffset;
    findFirstNonSeparatorChar(&nonSeparatorOffset,
                              filename.data(),
                              static_cast<int>(filename.length()));
    if (0 != nonSeparatorOffset) { // absolute path
        return -1;                                                    // RETURN
    }

    // Create an 'adjustedFilenameLength' that suppresses trailing separators
    // in 'filename'.

    int adjustedFilenameLength = static_cast<int>(filename.length());
    for (; adjustedFilenameLength > 0; --adjustedFilenameLength) {
        if (!isSeparator(filename[adjustedFilenameLength - 1])) {
            break;
        }
    }

    // Erase trailing separators from 'path'.

    if (!path->empty()) {
        bsl::size_t lastChar = path->find_last_not_of(k_separators);

        // If 'path' is *all* separator characters (i.e., no non-seperator was
        // found), the resulting 'path' should be 1 separator character.

        lastChar = (lastChar == bsl::string::npos) ? 0 : lastChar;
        if (lastChar != path->length()) {
            path->erase(path->begin() + lastChar + 1, path->end());
        }
    }

    PathUtil::appendRaw(path, filename.data(), adjustedFilenameLength);
    return 0;
}

template <class STR_TYPE>
void u_appendRaw(STR_TYPE *path, const char *filename, int length, int rootEnd)
{
    BSLS_ASSERT(path);
    BSLS_ASSERT(filename);

    if (0 > length) {
        length = static_cast<int>(bsl::strlen(filename));
    }

    if (0 < length) {
        if (0 > rootEnd) {
            findFirstNonSeparatorChar(&rootEnd,
                                      path->c_str(),
                                      static_cast<int>(path->length()));
        }
        if (PathUtil::hasLeaf(path->c_str(), rootEnd)
         || (rootEnd > 0 && !isSeparator((*path)[rootEnd-1]))) {
            path->push_back(k_separators[0]);
        }
        path->append(filename, length);
    }
}

template <class STR_TYPE>
int u_popLeaf(STR_TYPE *path, int rootEnd)
{
    BSLS_ASSERT(path);

    if (0 > rootEnd) {
        findFirstNonSeparatorChar(&rootEnd,
                                  path->c_str(),
                                  static_cast<int>(path->length()));
    }

    if (!PathUtil::hasLeaf(path->c_str(), rootEnd)) {
        return -1;                                                    // RETURN
    }

    path->erase(leafDelimiter(*path, rootEnd) - path->data());
    return 0;
}

template <class STR_TYPE>
int u_getLeaf(STR_TYPE *leaf, const bsl::string_view& path, int rootEnd)
{
    BSLS_ASSERT(leaf);

    int length = static_cast<int>(path.length());

    if (0 > rootEnd) {
        findFirstNonSeparatorChar(&rootEnd, path.data(), length);
    }

    if (!PathUtil::hasLeaf(path, rootEnd)) {
        return -1;                                                    // RETURN
    }

    leaf->clear();
    const char *lastSeparator = leafDelimiter(path.data(), rootEnd, length);
    BSLS_ASSERT(lastSeparator != path.data() + length);

    while (0 < length && isSeparator(path[length-1])) {
        --length;
    }

    leaf->append(
               isSeparator(*lastSeparator) ? lastSeparator + 1 : lastSeparator,
               path.data() + length);
    return 0;
}

template <class STR_TYPE>
int u_getExtension(STR_TYPE                *extension,
                   const bsl::string_view&  path,
                   int                      rootEnd)
{
    BSLS_ASSERT(extension);
    STR_TYPE    leaf;
    int hasLeaf = PathUtil::getLeaf(&leaf, path, rootEnd);

    if (hasLeaf != 0) {
        return -1;                                                        // RETURN
    }

    if (leaf == "." || leaf == "..") {
        return -1;                                                        // RETURN
    }

    bsl::size_t lastDotIndex = leaf.find_last_of(".");

    if (lastDotIndex == 0 || lastDotIndex == bsl::string::npos) {
        return -1;                                                        // RETURN
    }

    extension->assign(&leaf[lastDotIndex], leaf.size() - lastDotIndex);
    return 0;                                                             // RETURN
}

template <class STR_TYPE>
int u_getDirname(STR_TYPE *dirname, const bsl::string_view& path, int rootEnd)
{
    BSLS_ASSERT(dirname);

    if (0 > rootEnd) {
        findFirstNonSeparatorChar(&rootEnd,
                                  path.data(),
                                  static_cast<int>(path.length()));
    }

    if (!PathUtil::hasLeaf(path, rootEnd)) {
        return -1;                                                    // RETURN
    }

    dirname->clear();
    const char *lastSeparator = leafDelimiter(path.data(),
                                              rootEnd,
                                              static_cast<int>(path.length()));
    if (lastSeparator == path.data()) {
        //nothing to do

        return 0;                                                     // RETURN
    }
    dirname->append(path.data(), lastSeparator);
    return 0;
}

template <class STR_TYPE>
int u_getRoot(STR_TYPE *root, const bsl::string_view& path, int rootEnd)
{
    BSLS_ASSERT(root);

    if (0 > rootEnd) {
        findFirstNonSeparatorChar(&rootEnd,
                                  path.data(),
                                  static_cast<int>(path.length()));
    }

    if (PathUtil::isRelative(path, rootEnd)) {
        return -1;                                                    // RETURN
    }

    root->clear();
    root->append(path.data(), path.data() + rootEnd);
    return 0;
}

}  // close unnamed namespace

                              // ===============
                              // struct PathUtil
                              // ===============

// CLASS METHODS
int PathUtil::appendIfValid(bsl::string             *path,
                            const bsl::string_view&  filename)
{
    return u_appendIfValid(path, filename);
}

int PathUtil::appendIfValid(std::string             *path,
                            const bsl::string_view&  filename)
{
    return u_appendIfValid(path, filename);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
int PathUtil::appendIfValid(std::pmr::string        *path,
                            const bsl::string_view&  filename)
{
    return u_appendIfValid(path, filename);
}
#endif

void PathUtil::appendRaw(bsl::string *path,
                         const char  *filename,
                         int          length,
                         int          rootEnd)
{
    u_appendRaw(path, filename, length, rootEnd);
}

void PathUtil::appendRaw(std::string *path,
                         const char  *filename,
                         int          length,
                         int          rootEnd)
{
    u_appendRaw(path, filename, length, rootEnd);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
void PathUtil::appendRaw(std::pmr::string *path,
                         const char  *filename,
                         int          length,
                         int          rootEnd)
{
    u_appendRaw(path, filename, length, rootEnd);
}
#endif

int PathUtil::popLeaf(bsl::string *path, int rootEnd)
{
    return u_popLeaf(path, rootEnd);
}

int PathUtil::popLeaf(std::string *path, int rootEnd)
{
    return u_popLeaf(path, rootEnd);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
int PathUtil::popLeaf(std::pmr::string *path, int rootEnd)
{
    return u_popLeaf(path, rootEnd);
}
#endif

int PathUtil::getLeaf(bsl::string             *leaf,
                      const bsl::string_view&  path,
                      int                      rootEnd)
{
    return u_getLeaf(leaf, path, rootEnd);
}

int PathUtil::getLeaf(std::string             *leaf,
                      const bsl::string_view&  path,
                      int                      rootEnd)
{
    return u_getLeaf(leaf, path, rootEnd);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
int PathUtil::getLeaf(std::pmr::string        *leaf,
                      const bsl::string_view&  path,
                      int                      rootEnd)
{
    return u_getLeaf(leaf, path, rootEnd);
}
#endif

int PathUtil::getExtension(bsl::string             *extension,
                           const bsl::string_view&  path,
                           int                      rootEnd)
{
    return u_getExtension(extension, path, rootEnd);
}

int PathUtil::getExtension(std::string             *extension,
                           const bsl::string_view&  path,
                           int                      rootEnd)
{
    return u_getExtension(extension, path, rootEnd);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
int PathUtil::getExtension(std::pmr::string        *extension,
                           const bsl::string_view&  path,
                           int                      rootEnd)
{
    return u_getExtension(extension, path, rootEnd);
}
#endif

int PathUtil::getDirname(bsl::string             *dirname,
                         const bsl::string_view&  path,
                         int                      rootEnd)
{
    return u_getDirname(dirname, path, rootEnd);
}

int PathUtil::getDirname(std::string             *dirname,
                         const bsl::string_view&  path,
                         int                      rootEnd)
{
    return u_getDirname(dirname, path, rootEnd);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
int PathUtil::getDirname(std::pmr::string             *dirname,
                         const bsl::string_view&  path,
                         int                      rootEnd)
{
    return u_getDirname(dirname, path, rootEnd);
}
#endif

int PathUtil::getRoot(bsl::string             *root,
                      const bsl::string_view&  path,
                      int                      rootEnd)
{
    return u_getRoot(root, path, rootEnd);
}

int PathUtil::getRoot(std::string             *root,
                      const bsl::string_view&  path,
                      int                      rootEnd)
{
    return u_getRoot(root, path, rootEnd);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
int PathUtil::getRoot(std::pmr::string             *root,
                      const bsl::string_view&  path,
                      int                      rootEnd)
{
    return u_getRoot(root, path, rootEnd);
}
#endif

void PathUtil::splitFilename(bsl::string_view        *head,
                             bsl::string_view        *tail,
                             const bsl::string_view&  path,
                             int                      rootEnd)
{
    BSLS_ASSERT(head);
    BSLS_ASSERT(tail);
    BSLS_ASSERT(head != tail);

    bsl::string_view default_sv;

    if (path.empty()) {
        *head = default_sv;
        *tail = default_sv;
        return;                                                       // RETURN
    }

    BSLS_ASSERT(INT_MAX >= path.length());
    int numCharsToHandle = static_cast<int>(path.length());

    const char *pathBegin     = path.data();
    const char *pathEnd       = pathBegin + numCharsToHandle;
    const char *lastSeparator = pathEnd - 1;

    if (0 > rootEnd) {
        findFirstNonSeparatorChar(&rootEnd, pathBegin, numCharsToHandle);
    }

    // Tail detection.

    while (numCharsToHandle > rootEnd && (!isSeparator(*lastSeparator))) {
        --numCharsToHandle;
        --lastSeparator;
    }

    if (pathEnd - 1 != lastSeparator) {
        *tail =
            bsl::string_view(lastSeparator + 1, pathEnd - lastSeparator - 1);
    } else {
        *tail = default_sv;
    }

    // Skip trailing delimiters between head and tail.

    while (numCharsToHandle > rootEnd && (isSeparator(*(lastSeparator - 1)))) {
        --numCharsToHandle;
        --lastSeparator;
    }

    // Head detection.
    *head = bsl::string_view(pathBegin,
                             lastSeparator > pathBegin + rootEnd
                                 ? (lastSeparator - pathBegin)
                                 : rootEnd);
}

bool PathUtil::isAbsolute(const bsl::string_view& path, int rootEnd)
{
    if (0 > rootEnd) {
        findFirstNonSeparatorChar(&rootEnd,
                                  path.data(),
                                  static_cast<int>(path.length()));
    }

    return rootEnd > 0;
}

bool PathUtil::isRelative(const bsl::string_view& path, int rootEnd)
{
    if (0 > rootEnd) {
        findFirstNonSeparatorChar(&rootEnd,
                                  path.data(),
                                  static_cast<int>(path.length()));
    }

    return 0 == rootEnd;
}

bool PathUtil::hasLeaf(const bsl::string_view& path, int rootEnd)
{
    int length = static_cast<int>(path.length());
    if (0 > rootEnd) {
        findFirstNonSeparatorChar(&rootEnd, path.data(), length);
    }

    while (0 < length && isSeparator(path[length-1])) {
        --length;
    }

    return rootEnd < length;
}

int PathUtil::getRootEnd(const bsl::string_view& path)
{
    int result;
    findFirstNonSeparatorChar(&result,
                              path.data(),
                              static_cast<int>(path.length()));
    return result;
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
