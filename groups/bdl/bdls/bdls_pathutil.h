// bdls_pathutil.h                                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLS_PATHUTIL
#define INCLUDED_BDLS_PATHUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide portable file path manipulation.
//
//@CLASSES:
//  bdls::PathUtil: Portable utility methods for manipulating paths
//
//@AUTHOR: David Schumann (dschumann1)
//
//@SEE_ALSO: bdls_filesystemutil
//
//@DESCRIPTION: This component provides utility methods for manipulating
// strings that represent paths in the filesystem.  Class methods of
// 'bdls::PathUtil' include platform-independent operations to add or remove
// filenames or relative paths at the end of a path string (by "filenames" we
// are referring to the names of any filesystem item, including regular files
// and directories).  There are also methods to parse the path to delimit the
// "root" as defined for the current platform; see "Parsing and Performance"
// below.
//
// Paths that have a root are called *absolute* paths, whereas paths that do
// not have a root are *relative* paths.
//
// Note that this component does not perform filesystem operations.  In
// particular, no effort is made to verify the existence or accessibility of
// any segment of any path.
//
///Terminology
///-----------
// To introduce the terminology explored in this section, lets start with a
// Unix example:
//..
//  "/foo/bar/myfile.txt"
//..
// The elements of this path would be:
//..
//            Path: "/foo/bar/myfile.txt"
//            Root: "/"                       # the starting separator(s)
//  Leaf(Basename): "myfile.txt"
//         Dirname: "/foo/bar/"
//..
//
///Separator
///- - - - -
// A platform dependent character that separates elements of a path, such as
// directory names from each other and file names.  The separator character is
// the '/' (slash) on Unix (and the like) systems and '\' (backslash) on
// Windows systems.
//
///Path
/// - -
// An optional root, followed by optional directories, followed by an optional
// filename.
//
///Root
/// - -
// The root, if present, is at the beginning of a path and its presence
// determines if a path is absolute (the root is present) or relative (the root
// is not present).  The textual rules for what a root is are platform
// dependent.  See {'Unix Root'} and {'Window Root'}.
//
// See also {'Parsing and Performance (rootEnd argument)'} for important notes
// about speeding up functions (especially on Windows) by not reparsing roots
// every time a function is called.
//
///Unix Root
///  -  -  -
// The Unix root consists of the separator characters at the beginning of a
// path, so the root of "/one" is "/", the root of "//two" is "//", while the
// root of "somefile" is "" (there is no root, relative path).
//
///Windows Root
///  -  -  -  -
// The Windows root is much more complicated than the Unix root, because
// Windows has three different flavors of paths: local (LFS), UNC, and Long UNC
// UNC (LUNC):
//
//: LFS: root consists of a drive letter followed by a colon (the name part)
//:      and then zero or more separators (the directory part).  E.g.,
//:      "c:\hello.txt", root is "c:\"; "c:tmp" root is "c:"
//:
//: UNC: root consists of two separators followed by a hostname and
//:      separator (the name part), and then a shared folder followed by one
//:      or more separators (the directory part).  e.g.,
//:      "\\servername\sharefolder\output\test.t" root is
//:      "\\servername\sharefolder\"
//:
//: LUNC: root starts with "\\?\".  Then follows either "UNC" followed by
//:       a UNC root, or an LFS root.  The "\\?\" is included as part of
//:       the root name.  e.g.,
//:      "\\?\UNC\servername\folder\hello" root is "\\?\UNC\servername\dir\"
//:      while "\\?\c:\windows\test" root is "\\?\\c:\"
//
///Leaf (a.k.a. Basename)
/// - - - - - - - - - - -
// The leaf is the rightmost name following the root, in other words: the last
// element of the path.  Note that several methods in this utility require a
// leaf to be present to function (such as 'getDirname').  Note that a relative
// path may contain a leaf only.  Examples:
//..
//  Path                            Leaf
//  ----                            ----
//  "/tmp/foo/bar.txt"              "bar.txt"
//  "c:\tmp\foo\bar.txt"            "bar.txt"
//  "\\server\share\tmp\foo.txt"    "foo.txt"
//  "/tmp/foo/"                     "foo"
//  "/tmp/"                         "tmp"
//  "/"                             Not Present
//..
//
///Dirname
///- - - -
// Dirname is the part of the path that contains the root but not the leaf.
// Note that the 'getDirname' utility method requires a leaf to be present to
// function.  Examples:
//..
//  Path                            Dirname
//  ----                            -------
//  "/tmp/foo/bar.txt"              "/tmp/foo/"
//  "c:\tmp\foo\bar.txt"            "c:\tmp\foo\"
//  "\\server\share\tmp\foo.txt"    "\\server\share\tmp\"
//  "/tmp/foo/"                     "/tmp"
//  "/tmp/"                         "/"
//  "/"                             no leaf -> error
//  "foo.txt"                       empty
//..
//
///Parsing and Performance (rootEnd argument)
///------------------------------------------
// Most methods of this component will perform basic parsing of the beginning
// part of the path to determine what part of it is the "root" as defined for
// the current platform.  This parsing is trivial on Unix platforms but is
// slightly more involved for the Windows operating system.  To accommodate
// client code which is willing to store parsing results in order to maximize
// performance, all methods which parse the "root" of the path accept an
// optional argument delimiting the "root"; if this argument is specified,
// parsing is skipped.
//
///Usage
///-----
// We start with strings representing an absolute native path and a relative
// native path, respectively:
//..
//  #ifdef BSLS_PLATFORM_OS_WINDOWS
//      bsl::string tempPath  = "c:\\windows\\temp";
//      bsl::string otherPath = "22jan08\\log.txt";
//  #else
//      bsl::string tempPath  = "/var/tmp";
//      bsl::string otherPath = "22jan08/log.txt";
//  #endif
//..
// 'tempPath' is an absolute path, since it has a root.  It also has a leaf
// element ("temp"):
//..
//  assert(false == bdls::PathUtil::isRelative(tempPath));
//  assert(true  == bdls::PathUtil::isAbsolute(tempPath));
//  assert(true  == bdls::PathUtil::hasLeaf(tempPath));
//..
// We can add filenames to the path one at a time, or we can add another path
// if is relative.  We can also remove filenames from the end of the path one
// at a time:
//..
//  bdls::PathUtil::appendRaw(&tempPath, "myApp");
//  bdls::PathUtil::appendRaw(&tempPath, "logs");
//  assert(true == bdls::PathUtil::isRelative(otherPath));
//  assert(0 == bdls::PathUtil::appendIfValid(&tempPath, otherPath));
//  assert(true == bdls::PathUtil::hasLeaf(tempPath));
//  bdls::PathUtil::popLeaf(tempPath);
//  bdls::PathUtil::appendRaw(&tempPath, "log2.txt");
//
//  #ifdef BSLS_PLATFORM_OS_WINDOWS
//      assert("c:\\windows\\temp\\myApp\\logs\\22jan08\\log2.txt" ==
//                                                                   tempPath);
//  #else
//      assert("/var/tmp/myApp/logs/22jan08/log2.txt" == tempPath);
//  #endif
//..
// A relative path may be appended to any other path, even itself.  An absolute
// path may not be appended to any path, or undefined behavior will result:
//..
//   assert(0 == bdls::PathUtil::appendIfValid(&otherPath, otherPath));  // OK
//   /* bdls::PathUtil::append(&otherPath, tempPath); */ // UNDEFINED BEHAVIOR!
//..
// Note that there is no attempt to distinguish filenames that are regular
// files from filenames that are directories, or to verify the existence of
// paths in the filesystem.  On POSIX:
//..
//   assert("22jan08/log.txt/22jan08/log.txt" == otherPath);
//..

#include <bdlscm_version.h>

#include <bsls_assert.h>

#include <bsl_string.h>

namespace BloombergLP {

namespace bdls {
                              // ===============
                              // struct PathUtil
                              // ===============

struct PathUtil {
    // This struct contains utility methods for platform-independent
    // manipulation of filesystem paths.  No method of this struct provides
    // any filesystem operations or accesses the filesystem as part of its
    // implementation.

    // CLASS METHODS
    static int appendIfValid(bsl::string              *path,
                             const bslstl::StringRef&  filename);
        // Append the specified 'filename' to the end of the specified 'path'
        // if 'filename' represents a relative path.  Return 0 on success, and
        // a non-zero value otherwise.  Note that any filesystem separator
        // characters at the end of 'filename' or 'path' will be discarded.
        // See {'Terminology'} for the definition of separator.

    static void appendRaw(bsl::string *path,
                          const char  *filename,
                          int          length = -1,
                          int          rootEnd = -1);
        // Append the specified 'filename' up to the optionally specified
        // 'length' to the end of the specified 'path'.  If 'length' is
        // negative, append the entire string.  If the optionally specified
        // 'rootEnd' offset is non-negative, it is taken as the position in
        // 'path' of the character following the root.  The behavior is
        // undefined if 'filename' represents an absolute path or if either
        // 'filename' or 'path' ends with the filesystem separator character.
        // The behavior is also undefined if 'filename' points to any part of
        // 'path' (i.e., 'filename' may not be an alias for 'path').  See
        // {'Parsing and Performance (rootEnd argument)'}.

    static int popLeaf(bsl::string *path, int rootEnd = -1);
        // Remove from the specified 'path' the rightmost name following the
        // root; that is, remove the leaf element.  If the optionally specified
        // 'rootEnd' offset is non-negative, it is taken as the position in
        // 'path' of the character following the root.  Return 0 on success,
        // and a nonzero value otherwise; in particular, return a nonzero value
        // if 'path' does not have a leaf.  See
        // {'Parsing and Performance (rootEnd argument)'}.  See also
        // {'Terminology'} for the definition of leaf and root.

    static int getBasename(bsl::string              *leaf,
                           const bslstl::StringRef&  path,
                           int                       rootEnd = -1);
        // Load into the specified 'leaf' the value of the rightmost name in
        // the specified 'path' that follows the root; that is, the leaf
        // element.  If the optionally specified 'rootEnd' offset is
        // non-negative, it is taken as the position in 'path' of the character
        // following the root.  Return 0 on success, and a non-zero value
        // otherwise; in particular, return nonzero if 'path' does not have a
        // leaf.  Note that 'getBasename' is a synonym for 'getLeaf'.  See
        // {'Parsing and Performance (rootEnd argument)'}.  See also
        // {'Terminology'} for the definition of leaf and root.

    static int getDirname(bsl::string              *dirname,
                          const bslstl::StringRef&  path,
                          int                       rootEnd = -1);
        // Load into the specified 'dirname' the value of the directory part
        // of the specified 'path', that is, the root if it exists and all the
        // names except the last one (the leaf).  If the optionally specified
        // 'rootEnd' offset is non-negative, it is taken as the position in
        // 'path' of the character following the root.  Return 0 on success,
        // and a non-zero value otherwise; in particular, return a nonzero
        // value if 'path' does not have a leaf.  Note that in the case of a
        // relative path with a single filename, the function will succeed and
        // 'dirname' will be the empty string.  See
        // {'Parsing and Performance (rootEnd argument)'}.  See also
        // {'Terminology'} for the definition of directories and root.

    static int getLeaf(bsl::string              *leaf,
                       const bslstl::StringRef&  path,
                       int                       rootEnd = -1);
        // Load into the specified 'leaf' the value of the rightmost name in
        // the specified 'path' that follows the root; that is, the leaf
        // element.  If the optionally specified 'rootEnd' offset is
        // non-negative, it is taken as the position in 'path' of the character
        // following the root.  Return 0 on success, and a non-zero value
        // otherwise; in particular, return nonzero if 'path' does not have a
        // leaf.  Note that 'getBasename' is a synonym for 'getLeaf'.  See
        // {'Parsing and Performance (rootEnd argument)'}.  See also
        // {'Terminology'} for the definition of leaf and root.

    static int getRoot(bsl::string              *root,
                       const bslstl::StringRef&  path,
                       int                       rootEnd = -1);
        // Load into the specified 'root' the value of the root part of the
        // specified 'path'.  If the optionally specified 'rootEnd' offset is
        // non-negative, it is taken as the position in 'path' of the character
        // following the root.  Return 0 on success, and a non-zero value
        // otherwise; in particular, return a nonzero value if 'path' is
        // relative.  Note that the meaning of the root part is
        // platform-dependent.  See
        // {'Parsing and Performance (rootEnd argument)'}.  See also
        // {'Terminology'} for the definition of root.

    static void splitFilename(bslstl::StringRef        *head,
                              bslstl::StringRef        *tail,
                              const bslstl::StringRef&  path,
                              int                       rootEnd = -1);
        // Load the last pathname component from the specified 'path' into the
        // specified 'tail' and everything leading up to that to the specified
        // 'head'.  If the optionally specified 'rootEnd' offset is
        // non-negative, it is taken as the position in 'path' of the character
        // following the root.  The 'tail' part never contains a slash; if
        // 'path' ends in a slash, 'tail' is empty.  If there is no slash in
        // 'path', 'head' is empty.  If 'path' is empty, both 'head' and 'tail'
        // are empty.  Trailing slashes are stripped from 'head' unless it is
        // the root.  See {'Terminology'} for the definition of root.


    static bool isAbsolute(const bslstl::StringRef& path, int rootEnd = -1);
        // Return 'true' if the specified 'path' is absolute (has a root), and
        // 'false' otherwise.  If the optionally specified 'rootEnd' offset is
        // non-negative, it is taken as the position in 'path' of the character
        // following the root.  See
        // {'Parsing and Performance (rootEnd argument)'}.  See also
        // {'Terminology'} for the definition of root.

    static bool isRelative(const bslstl::StringRef& path, int rootEnd = -1);
        // Return 'true' if the specified 'path' is relative (lacks a root),
        // and 'false' otherwise.  If the optionally specified 'rootEnd' offset
        // is non-negative, it is taken as the position in 'path' of the
        // character following the root.  See
        // {'Parsing and Performance (rootEnd argument)'}.  See also
        // {'Terminology'} for the definition of root.

    static bool hasLeaf(const bslstl::StringRef& path, int rootEnd = -1);
        // Return 'true' if the specified 'path' has a name following the root,
        // and 'false' otherwise.  If the optionally specified 'rootEnd' offset
        // is non-negative, it is taken as the position in 'path' of the
        // character following the root.  See
        // {'Parsing and Performance (rootEnd argument)'}.  See also
        // {'Terminology'} for the definition of leaf.

    static int getRootEnd(const bslstl::StringRef& path);
        // Return the 0-based position in the specified 'path' of the character
        // following the root.  Note that a return value of 0 indicates a
        // relative path.  See
        // {'Parsing and Performance (rootEnd argument)'}.  See also
        // {'Terminology'} for the definition of root.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                               // --------------
                               // class PathUtil
                               // --------------

// CLASS METHODS
inline
int PathUtil::getBasename(bsl::string              *leaf,
                          const bslstl::StringRef&  path,
                          int                       rootEnd)
{
    BSLS_ASSERT_SAFE(leaf);

    return getLeaf(leaf, path, rootEnd);
}
}  // close package namespace

}  // close enterprise namespace

#endif

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
