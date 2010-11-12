// bdesu_pathutil.cpp   -*-C++-*-
#include <bdesu_pathutil.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_algorithm.h>
#include <bsl_cctype.h>
#include <bsl_climits.h>
#include <bsl_cstddef.h>
#include <bsl_cstring.h>

namespace BloombergLP {

namespace {
    // Scopes private helper methods and data

const char SEPARATOR =
#ifdef BSLS_PLATFORM__OS_WINDOWS
'\\'
#else
'/'
#endif
    ;

void parse (size_t* rootEnd, const char* path, int length = -1)
{
   if (0 > length) {
      length = bsl::strlen(path);
   }
   *rootEnd = 0;

#ifdef BSLS_PLATFORM__OS_WINDOWS
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
       *rootEnd = rootNameEnd = 2;
    }
    else if (4 <= length && SEPARATOR == path[0] &&
             SEPARATOR == path[1] && '?' != path[2]) {
       //UNC (we require 4 because the hostname must be nonempty and there
       //must then be another separator; then at least a nonempty
       //share directory).  Root name ends after the separator that follows
       //the hostname; root directory ends after the separator that follows
       //the share name.
       const char* rootNameEndPtr = bsl::find(path + 3, path + length,
                                              SEPARATOR);

       if (rootNameEndPtr != path + length) {
          ++rootNameEndPtr;
       }
       rootNameEnd = (size_t)(rootNameEndPtr - path);

       const char* rootEndPtr = bsl::find(path + rootNameEnd,
                                           path + length,
                                           SEPARATOR);
       if (rootEndPtr != path + length) {
          ++rootEndPtr;
       }
       *rootEnd = (size_t)(rootEndPtr - path);
    }
    else if (UNCW_PREFIXLEN < length &&
             0 == strncmp(path, UNCW_PREFIX, UNCW_PREFIXLEN)) {
       //LUNC.  Now determine whether it is LFS or UNC
       if (UNCW_PREFIXLEN+2 <= length
           && bsl::isalpha(static_cast<unsigned char>(path[UNCW_PREFIXLEN]))
           && ':' == path[UNCW_PREFIXLEN+1]) {
          //LFS.
          *rootEnd = rootNameEnd = UNCW_PREFIXLEN+2;
       }
       else if (UNCW_UNCPREFIXLEN < length &&
                0 == strncmp(path, UNCW_UNCPREFIX, UNCW_UNCPREFIXLEN)) {
          //UNC.
          const char* rootNameEndPtr =
             bsl::find(path + UNCW_UNCPREFIXLEN, path + length, SEPARATOR);

          if (rootNameEndPtr != path + length) {
             ++rootNameEndPtr;
          }
          rootNameEnd = (unsigned)(rootNameEndPtr - path);

          const char* rootEndPtr = bsl::find(path + rootNameEnd,
                                             path + length,
                                             SEPARATOR);
          if (rootEndPtr != path + length) {
             ++rootEndPtr;
          }
          *rootEnd = (unsigned)(rootEndPtr - path);
       }
    }
#endif

    while((int) *rootEnd < length &&
          path[*rootEnd] == SEPARATOR) {
        ++*rootEnd;
    }
}

inline
void parse (int* rootEnd, const char* path, int length = -1)
{
    size_t rootEndOffset;
    parse(&rootEndOffset, path, length);
    BSLS_ASSERT(INT_MAX > rootEndOffset);
    *rootEnd = rootEndOffset;
}

const char *leafDelimiter(const char *path,
                          int rootEnd,
                          int length=-1)
{
    if (0 > length) {
        length = bsl::strlen(path);
    }

    while (0 < length && SEPARATOR == path[length-1]) {
        --length;
    }

    const char *position;
    for (position = path + length - 1;
         position > path + rootEnd && *position != SEPARATOR;
         --position) {
    }
    return position;
}

inline
const char *leafDelimiter(const bsl::string &path,
                          int rootEnd)
{
    return leafDelimiter(path.c_str(), rootEnd, path.length());
}

} // close unnamed namespace

int bdesu_PathUtil::appendIfValid(bsl::string *path,
                                  const bdeut_StringRef& origFilename)
{
    bdeut_StringRef filename = origFilename;
    int filenameLength = filename.length();

    size_t rootEnd;
    parse(&rootEnd, filename.data(), filenameLength);
    if (0 != rootEnd) { // absolute path
        return -1;
    }

    int aliasOffset = filename.data() - path->c_str();

    if (0 <= aliasOffset && aliasOffset < (int) path->length()) {
        // 'filename' is an alias of 'path'.  Resize and then
        // point 'filename' back into 'path'
        path->reserve(bsl::max(path->capacity(),
                               path->length() + filenameLength + 1));
        filename.assign(path->c_str() + aliasOffset, filenameLength);
    }

    // suppress trailing separators in filename and path
    while (0 < filenameLength && SEPARATOR == filename[filenameLength-1]) {
        filenameLength--;
    }
    int pathRootEnd;
    parse(&pathRootEnd, path->c_str(), path->length());

    while (!path->empty() && SEPARATOR == (*path)[path->length()-1] &&
           pathRootEnd < (int) path->length())
    {
        path->erase(path->length()-1);
    }

    appendRaw(path, filename.data(), filenameLength, pathRootEnd);
    return 0;
}

void bdesu_PathUtil::appendRaw(bsl::string *path,
                               const char *filename,
                               int length,
                               int rootEnd)
{
    if (0 > length) {
        length = bsl::strlen(filename);
    }

    if (0 < length) {
        if (0 > rootEnd) {
            parse(&rootEnd, path->c_str(), path->length());
        }
        if (hasLeaf(path->c_str(), rootEnd) ||
            (rootEnd > 0 && (*path)[rootEnd-1] != SEPARATOR)) {
            path->push_back(SEPARATOR);
        }
        path->append(filename, length);
    }
}

int bdesu_PathUtil::popLeaf(bsl::string *path, int rootEnd)
{
    if (0 > rootEnd) {
        parse(&rootEnd, path->c_str(), path->length());
    }

    if (!hasLeaf(path->c_str(), rootEnd)) {
        return -1;
    }

    path->erase(leafDelimiter(*path, rootEnd) - path->begin());
    return 0;
}

int bdesu_PathUtil::getLeaf(bsl::string *filename,
                            const bdeut_StringRef& path,
                            int rootEnd)
{
    int length = path.length();

    if (0 > rootEnd) {
        parse(&rootEnd, path.data(), length);
    }

    if (!hasLeaf(path, rootEnd)) {
        return -1;
    }
    filename->clear();
    const char *lastSeparator = leafDelimiter(path.data(),
                                              rootEnd, length);
    BSLS_ASSERT(lastSeparator != path.data() + length);

    while (0 < length && SEPARATOR == path[length-1]) {
        --length;
    }

    filename->append(*lastSeparator == SEPARATOR
                            ? lastSeparator+1
                            : lastSeparator,
                     path.data() + length);
    return 0;
}

int bdesu_PathUtil::getDirname(bsl::string *filename,
                               const bdeut_StringRef& path,
                               int rootEnd)
{
    if (0 > rootEnd) {
        parse(&rootEnd, path.data(), path.length());
    }

    if (!hasLeaf(path, rootEnd)) {
        return -1;
    }

    filename->clear();
    const char *lastSeparator = leafDelimiter(path.data(), rootEnd,
                                              path.length());
    if (lastSeparator == path.data()) {
        //nothing to do
        return 0;
    }
    filename->append(path.data(), lastSeparator);
    return 0;
}

int bdesu_PathUtil::getRoot(bsl::string *root,
                            const bdeut_StringRef& path,
                            int rootEnd)
{
    if (0 > rootEnd) {
        parse(&rootEnd, path.data(), path.length());
    }

    if (isRelative(path, rootEnd)) {
        return -1;
    }

    root->clear();
    root->append(path.data(), path.data() + rootEnd);
    return 0;
}

bool bdesu_PathUtil::isAbsolute(const bdeut_StringRef& path, int rootEnd)
{
    if (0 > rootEnd) {
        parse(&rootEnd, path.data(), path.length());
    }

    return rootEnd > 0;
}

bool bdesu_PathUtil::isRelative(const bdeut_StringRef& path, int rootEnd)
{
    if (0 > rootEnd) {
        parse(&rootEnd, path.data(), path.length());
    }

    return rootEnd == 0;
}

int bdesu_PathUtil::getRootEnd(const bdeut_StringRef& path)
{
    int result;
    parse(&result, path.data(), path.length());
    return result;
}

bool bdesu_PathUtil::hasLeaf(const bdeut_StringRef& path,
                             int rootEnd)
{
    int length = path.length();
    if (0 > rootEnd) {
        parse(&rootEnd, path.data(), length);
    }

    while (0 < length && SEPARATOR == path[length-1]) {
        --length;
    }

    return rootEnd < length;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.


//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
