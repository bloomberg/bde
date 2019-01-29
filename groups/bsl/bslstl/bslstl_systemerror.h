// bslstl_systemerror.h                                               -*-C++-*-
#ifndef INCLUDED_BSLSTL_SYSTEMERROR
#define INCLUDED_BSLSTL_SYSTEMERROR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide C++11-defined error classes and functions for C++03.
//
//@CLASSES:
//  bsl::errc:                    C++03 version of std::errc
//  bsl::error_category:          C++03 version of std::error_category
//  bsl::error_code:              C++03 version of std::error_code
//  bsl::error_condition:         C++03 version of std::error_condition
//  bsl::system_error:            C++03 version of std::system_error
//  bsl::is_error_code_enum:      C++03 version of std::is_error_code_enum
//  bsl::is_error_condition_enum: C++03 version of std::is_error_condition_enum
//
//@DESCRIPTION: This component defines seven classes ('bsl::errc',
// 'bsl::error_category', 'bsl::error_code', 'bsl::error_condition',
// 'bsl::system_error', 'bsl::is_error_code_enum', and
// 'bsl::is_error_condition_enum'), four named functions
// ('bsl::generic_category', 'bsl::system_category', 'bsl::make_error_code',
// and 'bsl::make_error_condition'), and a variety of operators that provide
// implementations of the C++11 'system_error' facility in C++03 mode.

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_system_error.h> instead of <bslstl_systemerror.h> in \
BSL_OVERRIDES_STD mode"
#endif
#include <bslscm_version.h>

#include <bslh_hash.h>

#include <bslmf_enableif.h>
#include <bslmf_integralconstant.h>

#include <cerrno>
#include <cstring>
#include <functional>
#include <ostream>
#include <stdexcept>
#include <string>

namespace bsl {

// FORWARD DECLARATIONS

class error_code;
class error_condition;

                          // ========================
                          // class is_error_code_enum
                          // ========================

template <class TYPE>
class is_error_code_enum : public false_type {
    // This class template represents a trait defining whether the specified
    // enumeration type 'TYPE' is to be treated as an error code by the
    // 'error_code' template methods.
};

                       // =============================
                       // class is_error_condition_enum
                       // =============================

template <class TYPE>
class is_error_condition_enum : public false_type {
    // This class template represents a trait defining whether the specified
    // enumeration type 'TYPE' is to be treated as an error condition by the
    // 'error_condition' template methods.
};

                                 // =========
                                 // enum errc
                                 // =========

namespace errc {
enum errc {
    // This enumeration provides named literals for the 'errno' values defined
    // in the '<cerrno>' header.
    //
    // Note that in the C++11 standard, this is defined as an 'enum class', so
    // that literals can be qualified with 'errc::' and objects of the type can
    // be declared as 'errc'.  In C++03 mode, we cannot have both, and we have
    // chosen in favor of allowing the literals to be qualified, so objects of
    // the type must be declared as 'errc::errc'.

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
};

                       // -----------------------------
                       // class is_error_condition_enum
                       // -----------------------------
template <>
class is_error_condition_enum<errc::errc> : public true_type {
    // Mark the 'errc' enumeration as an error condition.
};

                            // ====================
                            // class error_category
                            // ====================

class error_category {
    // This class acts as a base for types that represent the source and
    // encoding of error categories.

  private:
    // PRIVATE CREATORS
    error_category(const error_category&);             // = delete

    // PRIVATE MANIPULATORS
    error_category& operator=(const error_category&);  // = delete

  public:
    // CREATORS
    error_category();
        // Create an object of this type.

    virtual ~error_category();
        // Destroy this object.

    // ACCESSORS
    virtual const char *name() const = 0;
        // Return the name of this error category.

    virtual error_condition default_error_condition(int value) const;
        // Return an 'error_condition' object initialized with the specified
        // 'value' and this object as the error category.

    virtual bool equivalent(int code, const error_condition& condition) const;
    virtual bool equivalent(const error_code& code, int condition) const;
        // Return, for the error category defined by this object, whether the
        // specified 'code' and 'condition' are considered equivalent.

    virtual std::string message(int value) const = 0;
        // Return a string describing the error condition denoted by the
        // specified 'value'.

    bool operator==(const error_category& other) const;
        // Return whether this object is the same as the specified 'other'.

    bool operator!=(const error_category& other) const;
        // Return whether this object is different than the specified 'other'.

    bool operator<(const error_category& other) const;
        // Return whether this object precedes the specified 'other' in a total
        // ordering of 'error_category' objects.
};

                              // ================
                              // class error_code
                              // ================

class error_code {
    // Objects of this class are intended to hold system-specific error values.

  public:
    // CREATORS
    error_code();
        // Create an object of this type initialized with value 0 and system
        // category.

    error_code(int value, const error_category& category);
        // Create an object of this type initialized with the specified 'value'
        // and 'category'.

    template <class ErrorCodeEnum>
    error_code(typename enable_if<is_error_code_enum<ErrorCodeEnum>::value,
                                  ErrorCodeEnum>::type value);
        // Construct an object of this type initialized with the specified
        // 'value' and generic category.  Note that this constructor exists
        // only for those types designated as error codes via the
        // 'is_error_code_enum' trait template.  Note that this object is
        // constructed with the generic rather than system category, because
        // that is what the standard specifies.

    // MANIPULATORS
    void assign(int value, const error_category& category);
        // Set this object to hold the specified 'value' and 'category'.

    template <class ErrorCodeEnum>
    error_code&
    operator=(typename enable_if<is_error_code_enum<ErrorCodeEnum>::value,
                                 ErrorCodeEnum>::type value);
        // Set this object to hold the specified 'value' and generic category.
        // Note that this operator exists only for those types designated as
        // error codes via the 'is_error_code_enum' trait template.  Note that
        // this object is set to the generic rather than system category,
        // because that is what the standard specifies.

    void clear();
        // Set this object to hold the value 0 and the system category.

    // ACCESSORS
    int value() const;
        // Return the value held by this object.

    const error_category& category() const;
        // Return a 'const' reference to the category held by this object.

    error_condition default_error_condition() const;
        // Return an 'error_condition' object initialized with the value and
        // category of this object.

    std::string message() const;
        // Return a string describing this object.

    operator bool() const;
        // Return whether the value held by this object is non-zero.

  private:
    // DATA
    int                   d_value;       // error code value
    const error_category *d_category_p;  // error category
};

                           // =====================
                           // class error_condition
                           // =====================

class error_condition {
    // Objects of this class are intended to hold portable error values.

  public:
    // CREATORS
    error_condition();
        // Create an object of this type initialized with value 0 and generic
        // category.

    error_condition(int value, const error_category& category);
        // Create an object of this type initialized with the specified 'value'
        // and 'category'.

    template <class ErrorConditionEnum>
    error_condition(
        typename enable_if<is_error_condition_enum<ErrorConditionEnum>::value,
                           ErrorConditionEnum>::type value);
        // Construct an object of this type initialized with the specified
        // 'value' and generic category.  Note that this constructor exists
        // only for those types designated as error conditions via the
        // 'is_error_condition_enum' trait template.

    // MANIPULATORS
    void assign(int value, const error_category& category);
        // Set this object to hold the specified 'value' and 'category'.

    template <class ErrorConditionEnum>
    error_condition& operator=(
        typename enable_if<is_error_condition_enum<ErrorConditionEnum>::value,
                           ErrorConditionEnum>::type value);
        // Set this object to hold the specified 'value' and generic category.
        // Note that this operator exists only for those types designated as
        // error conditions via the 'is_error_condition_enum' trait template.

    void clear();
        // Set this object to hold the value 0 and the generic category.

    // ACCESSORS
    int value() const;
        // Return the value held by this object.

    const error_category& category() const;
        // Return a 'const' reference to the category held by this object.

    std::string message() const;
        // Return a string describing this object.

    operator bool() const;
        // Return whether the value held by this object is non-zero.

  private:
    // DATA
    int                   d_value;       // error code value
    const error_category *d_category_p;  // error category
};

                             // ==================
                             // class system_error
                             // ==================

class system_error : public std::runtime_error {
    // This class represents exceptions that have an associated error code.

  public:
    // CREATORS
    system_error(error_code code, const std::string& what);
    system_error(error_code code, const char *what);
    system_error(error_code code);
        // Create an object of this type holding the specified 'code'.
        // Optionally specify a string 'what' to be added to the description of
        // this object.

    system_error(int                   value,
                 const error_category& category,
                 const std::string&    what);
    system_error(int code, const error_category& category, const char *what);
    system_error(int code, const error_category& category);
        // Create an object of this type holding an error code holding the
        // specified 'value' and 'category'.  Optionally specify a string
        // 'what' to be added to the description of this object.

    // ACCESSORS
    const error_code& code() const;
        // Return a 'const' reference to the error code held by this object.

  private:
    error_code d_code;  // error code
};

// FREE FUNCTIONS
const error_category& generic_category();
    // Return a 'const' reference to the unique generic category object.

const error_category& system_category();
    // Return a 'const' reference to the unique system category object.

error_code make_error_code(errc::errc value);
    // Return an 'error_code' object holding the specified 'value' and generic
    // category.  Note that the category is generic rather than system because
    // that is what the standard specifies.

error_condition make_error_condition(errc::errc value);
    // Return an 'error_condition' object holding the specified 'value' and
    // generic category.

template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const error_code& object)
{
    using ::BloombergLP::bslh::hashAppend;
    hashAppend(hashAlg, static_cast<const void *>(&object.category()));
    hashAppend(hashAlg, object.value());
}

template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const error_condition& object)
{
    using ::BloombergLP::bslh::hashAppend;
    hashAppend(hashAlg, static_cast<const void *>(&object.category()));
    hashAppend(hashAlg, object.value());
}

// FREE OPERATORS
bool operator==(const error_code& lhs, const error_code& rhs);
bool operator==(const error_code& lhs, const error_condition& rhs);
bool operator==(const error_condition& lhs, const error_code& rhs);
bool operator==(const error_condition& lhs, const error_condition& rhs);
    // Return whether the specfied 'lhs' and 'rhs' are equal or equivalent.

bool operator!=(const error_code&, const error_code&);
bool operator!=(const error_code&, const error_condition&);
bool operator!=(const error_condition&, const error_code&);
bool operator!=(const error_condition&, const error_condition&);
    // Return whether the specfied 'lhs' and 'rhs' are not equal or equivalent.

bool operator<(const error_code& lhs, const error_code& rhs);
bool operator<(const error_condition& lhs, const error_condition& rhs);
    // Return whether the specified 'lhs' is lexicographically less than the
    // specified 'rhs', ordered by category then value.

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                              // ----------------
                              // class error_code
                              // ----------------

// CREATORS
template <class ErrorCodeEnum>
inline
error_code::error_code(
    typename enable_if<is_error_code_enum<ErrorCodeEnum>::value,
                       ErrorCodeEnum>::type value)
: d_value(make_error_code(value).value())
, d_category_p(&make_error_code(value).category())
{
}

template <class ErrorCodeEnum>
inline
error_code&
error_code::
operator=(typename enable_if<is_error_code_enum<ErrorCodeEnum>::value,
                             ErrorCodeEnum>::type value)
{
    d_value = make_error_code(value).value();
    d_category_p = &make_error_code(value).category();
    return *this;
}

// FREE OPERATORS
template <class CHAR_TYPE, class CHAR_TRAITS>
inline
std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>& operator<<(
                            std::basic_ostream<CHAR_TYPE, CHAR_TRAITS>& stream,
                            const error_code&                           code)
{
    return stream << code.category().name() << ':' << code.value();
}

                           // ---------------------
                           // class error_condition
                           // ---------------------

// CREATORS
template <class ErrorConditionEnum>
inline
error_condition::error_condition(
    typename enable_if<is_error_condition_enum<ErrorConditionEnum>::value,
                       ErrorConditionEnum>::type value)
: d_value(make_error_condition(value).value())
, d_category_p(&make_error_condition(value).category())
{
}

template <class ErrorConditionEnum>
inline
error_condition&
error_condition::operator=(
    typename enable_if<is_error_condition_enum<ErrorConditionEnum>::value,
                       ErrorConditionEnum>::type value)
{
    d_value = make_error_condition(value).value();
    d_category_p = &make_error_condition(value).category();
    return *this;
}

}  // close namespace bsl

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
