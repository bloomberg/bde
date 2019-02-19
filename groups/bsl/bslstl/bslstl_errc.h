// bslstl_errc.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLSTL_ERRC
#define INCLUDED_BSLSTL_ERRC

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide C++11-defined error classes and functions for C++03.
//
//@MACROS:
//  BSL_ERRC_ENUM:  type designating the 'errc' enumeration

//@DESCRIPTION: This component defines an enumeration 'errc', and marks it
// elgible to participate as an error condition in the 'system_error' facility.
// In C++11 mode, the vendor-supplied '<system_error>' implementation is used
// instead, and the corresponding names from 'std' are imported into 'bsl'.
//
// This component also defines the macro 'BSL_ERRC_ENUM' that can be used as
// the type of the 'errc' enumeration in both C++03 and C++11 mode.  Because
// 'errc' is defined as an 'enum class', we cannot emulate use of 'std::errc'
// in C++03 mode - that tag can either be used to qualify the enumeration
// literals or name the type, but not both.  This component chooses to have
// 'bsl::errc' be the literal qualifier and 'BSL_ERRC_ENUM' be the type, so to
// use this component portably, you would, for example, write
//..
//  BSL_ERRC_ENUM error_value = bsl::errc::io_error;
//..
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Descriptive System Error Value Names
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we would like to have more descriptive and non-macro names for
// 'errno' system error values.  We can use the 'errc' from the 'system_error'
// facility of the C++ standard for this.
//
// First, we do something that will set an error value.
//..
//  strtod("1e2000", 0);
//..
// Finally, we check for that error using the descriptive name.
//..
//  BSL_ERRC_ENUM expected = bsl::errc::result_out_of_range;
//  assert(static_cast<int>(expected) == errno);
//..

#include <bslscm_version.h>

#include <bsls_libraryfeatures.h>

#include <bslstl_iserrorconditionenum.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

#include <bsls_nativestd.h>

#include <system_error>

#define BSL_ERRC_ENUM bsl::errc

namespace bsl {
    using native_std::errc;
};

#else

#include <errno.h>

#define BSL_ERRC_ENUM bsl::errc::errc

namespace bsl {

                                // ===========
                                // enum 'errc'
                                // ===========

namespace errc {
enum errc {
    // This enumeration provides named literals for the 'errno' values defined
    // in the '<cerrno>' header.
    //
    // Note that in the C++11 standard, 'errc' is defined as an 'enum class',
    // so that literals can be qualified with 'errc::' and objects of the type
    // can be declared as 'errc'.  In C++03 mode, we cannot have both, and we
    // have chosen in favor of allowing the literals to be qualified, so
    // objects of the type must be declared as 'errc::errc'.  For portability
    // between versions, declare objects of the enumeration type as
    // 'BSL_ERRC_ENUM'.

    address_family_not_supported       = EAFNOSUPPORT,
    address_in_use                     = EADDRINUSE,
    address_not_available              = EADDRNOTAVAIL,
    already_connected                  = EISCONN,
    argument_list_too_long             = E2BIG,
    argument_out_of_domain             = EDOM,
    bad_address                        = EFAULT,
    bad_file_descriptor                = EBADF,
    bad_message                        = EBADMSG,
    broken_pipe                        = EPIPE,
    connection_aborted                 = ECONNABORTED,
    connection_already_in_progress     = EALREADY,
    connection_refused                 = ECONNREFUSED,
    connection_reset                   = ECONNRESET,
    cross_device_link                  = EXDEV,
    destination_address_required       = EDESTADDRREQ,
    device_or_resource_busy            = EBUSY,
    directory_not_empty                = ENOTEMPTY,
    executable_format_error            = ENOEXEC,
    file_exists                        = EEXIST,
    file_too_large                     = EFBIG,
    filename_too_long                  = ENAMETOOLONG,
    function_not_supported             = ENOSYS,
    host_unreachable                   = EHOSTUNREACH,
    identifier_removed                 = EIDRM,
    illegal_byte_sequence              = EILSEQ,
    inappropriate_io_control_operation = ENOTTY,
    interrupted                        = EINTR,
    invalid_argument                   = EINVAL,
    invalid_seek                       = ESPIPE,
    io_error                           = EIO,
    is_a_directory                     = EISDIR,
    message_size                       = EMSGSIZE,
    network_down                       = ENETDOWN,
    network_reset                      = ENETRESET,
    network_unreachable                = ENETUNREACH,
    no_buffer_space                    = ENOBUFS,
    no_child_process                   = ECHILD,
    no_link                            = ENOLINK,
    no_lock_available                  = ENOLCK,
    no_message_available               = ENODATA,
    no_message                         = ENOMSG,
    no_protocol_option                 = ENOPROTOOPT,
    no_space_on_device                 = ENOSPC,
    no_stream_resources                = ENOSR,
    no_such_device_or_address          = ENXIO,
    no_such_device                     = ENODEV,
    no_such_file_or_directory          = ENOENT,
    no_such_process                    = ESRCH,
    not_a_directory                    = ENOTDIR,
    not_a_socket                       = ENOTSOCK,
    not_a_stream                       = ENOSTR,
    not_connected                      = ENOTCONN,
    not_enough_memory                  = ENOMEM,
    not_supported                      = ENOTSUP,
    operation_canceled                 = ECANCELED,
    operation_in_progress              = EINPROGRESS,
    operation_not_permitted            = EPERM,
    operation_not_supported            = EOPNOTSUPP,
    operation_would_block              = EWOULDBLOCK,
    owner_dead                         = EOWNERDEAD,
    permission_denied                  = EACCES,
    protocol_error                     = EPROTO,
    protocol_not_supported             = EPROTONOSUPPORT,
    read_only_file_system              = EROFS,
    resource_deadlock_would_occur      = EDEADLK,
    resource_unavailable_try_again     = EAGAIN,
    result_out_of_range                = ERANGE,
    state_not_recoverable              = ENOTRECOVERABLE,
    stream_timeout                     = ETIME,
    text_file_busy                     = ETXTBSY,
    timed_out                          = ETIMEDOUT,
    too_many_files_open_in_system      = ENFILE,
    too_many_files_open                = EMFILE,
    too_many_links                     = EMLINK,
    too_many_symbolic_link_levels      = ELOOP,
    value_too_large                    = EOVERFLOW,
    wrong_protocol_type                = EPROTOTYPE
};
}  // close namespace errc

                       // ------------------------------
                       // struct is_error_condition_enum
                       // ------------------------------
template <>
struct is_error_condition_enum<errc::errc> : public true_type {
    // Mark the 'errc' enumeration as an error condition.
};

}  // close namespace bsl

#endif
#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
