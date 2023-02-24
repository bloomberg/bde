// bsl_filesystem.h                                                   -*-C++-*-
#ifndef INCLUDED_BSL_FILESYSTEM
#define INCLUDED_BSL_FILESYSTEM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Provide
// 'bsl::hash' and 'bslh::hashAppend' support.  Finally, place the included
// symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_nativestd.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM
#include <filesystem>

#include <bslh_filesystem.h>

namespace bsl {

namespace filesystem {
    // CLASSES
    using std::filesystem::path;
    using std::filesystem::filesystem_error;
    using std::filesystem::directory_entry;
    using std::filesystem::directory_iterator;
    using std::filesystem::recursive_directory_iterator;
    using std::filesystem::file_status;
    using std::filesystem::space_info;
    using std::filesystem::file_type;
    using std::filesystem::perms;
    using std::filesystem::perm_options;
    using std::filesystem::copy_options;
    using std::filesystem::directory_options;
    using std::filesystem::file_time_type;

    // FUNCTIONS
    using std::filesystem::absolute;
    using std::filesystem::canonical;
    using std::filesystem::weakly_canonical;
    using std::filesystem::relative;
    using std::filesystem::proximate;
    using std::filesystem::copy;
    using std::filesystem::copy_file;
    using std::filesystem::copy_symlink;
    using std::filesystem::create_directory;
    using std::filesystem::create_directories;
    using std::filesystem::create_hard_link;
    using std::filesystem::create_symlink;
    using std::filesystem::create_directory_symlink;
    using std::filesystem::current_path;
    using std::filesystem::exists;
    using std::filesystem::equivalent;
    using std::filesystem::file_size;
    using std::filesystem::hard_link_count;
    using std::filesystem::last_write_time;
    using std::filesystem::permissions;
    using std::filesystem::read_symlink;
    using std::filesystem::remove;
    using std::filesystem::remove_all;
    using std::filesystem::rename;
    using std::filesystem::resize_file;
    using std::filesystem::space;
    using std::filesystem::status;
    using std::filesystem::symlink_status;
    using std::filesystem::temp_directory_path;

    // CLASSIFICATION FUNCTIONS
    using std::filesystem::is_block_file;
    using std::filesystem::is_character_file;
    using std::filesystem::is_directory;
    using std::filesystem::is_empty;
    using std::filesystem::is_fifo;
    using std::filesystem::is_other;
    using std::filesystem::is_regular_file;
    using std::filesystem::is_socket;
    using std::filesystem::is_symlink;
    using std::filesystem::status_known;
}  // close namespace filesystem
}  // close package namespace

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

#endif

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
