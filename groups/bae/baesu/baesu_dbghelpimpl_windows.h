// baesu_dbghelpimpl_windows.h                                        -*-C++-*-
#ifndef INCLUDED_BAESU_DBGHELPIMPL_WINDOWS
#define INCLUDED_BAESU_DBGHELPIMPL_WINDOWS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide access to the 'dbghelp.dll' shared library on Windows.
//
//@CLASSES:
//   baesu_Dbghelp
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@SEE_ALSO:
//
//@DESCRIPTION: On Windows, the baesu stack trace facility uses the windows
// shared library "dbghelp.dll".  This dll is not linked with, so it must
// be manually loaded at run-time when needed, and pointers to the wanted
// functions in it saved.  The library is not thread-safe, so all access to
// it must go through a mutex owned by this component.
//
// All data in this class is static.
//
// This class does not exist and is not used on any platform other than
// Windows.

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifdef BSLS_PLATFORM__OS_WINDOWS

#ifndef INCLUDED_BCEMT_QLOCK
#include <bcemt_qlock.h>
#endif

#ifndef INCLUDED_WINDOWS
#include <windows.h>
#endif

#ifndef INCLUDED_INTRIN
#include <intrin.h>
#endif

#ifndef INCLUDED_DBGHELP
#include <dbghelp.h>
#endif

namespace BloombergLP {

                            // ===================
                            // class baesu_Dbghelp
                            // ===================

class baesu_Dbghelp {
    // Namespace class used for accessing functions in the Windows
    // 'dbghelp.dll' shared library used by the stack trace facility.

  public:
    struct NullArg {
        // An object of this type is passed to args that are ignored where
        // corresponding values are passed automatically to the underlying
        // Windows functions.  When an arg is of type 'NullArg', a
        // default-constructed object of this type is to be passed to that
        // arg.  This makes it easier to read the calls and look them up in
        // the windows doc, since the number and types of active arguments is
        // unchanged.
    };

  private:
    // DATA
    static HMODULE s_moduleHandle;

  public:
    // CLASS METHODS
    static bcemt_QLock& qLock();
        // Return a reference to the qLock, a form of mutex that is used to
        // govern access to the 'dbghelp.dll' library, which is not thread
        // safe.  This object must be locked before any other functions in this
        // class are called.

    // The following functions are all Windows functions, except that on
    // Windows the first letter of their names are upper case.  See
    // http:://msdn.com for detailed doc.  All these functions do is make sure
    // the dbghelp.dll library is loaded and call the corresponding Windows
    // function, guarded by a static mutex that is shared by the whole
    // component.  In most cases some arguments are omitted, provided
    // automatically with cached values held in private statics by this
    // facility.

    static DWORD symSetOptions(DWORD arg1);
        // Behavior is identical to Windows 'SymSetOptions'.  See msdn.com.

#ifdef BSLS_PLATFORM__CPU_32_BIT
    // For some reason, 'symFromAddr' doesn't work right on 64 bit, so we
    // #ifdef everything and use 'SymGetSymFromAddr64'.

    static BOOL symFromAddr(NullArg,
                            DWORD64      arg2,
                            PDWORD64     arg3,
                            PSYMBOL_INFO arg4);
        // Behavior is identical to Windows 'SymFromAddr', except that
        // the first arg is ignored provided automatically by this utility.
        // See msdn.com.

#else
    static BOOL symGetSymFromAddr64(NullArg,
                                    DWORD64            arg2,
                                    PDWORD64           arg3,
                                    PIMAGEHLP_SYMBOL64 arg4);
        // Behavior is identical to Windows 'SymGetSymFromAddr64', except that
        // the first arg is ignored provided automatically by this utility.
        // See msdn.com.

#endif
    static BOOL symGetLineFromAddr64(NullArg,
                                     DWORD64           arg2,
                                     PDWORD            arg3,
                                     PIMAGEHLP_LINE64  arg4);
        // Behavior is identical to Windows 'SymGetLineFromAddr64', except that
        // the first arg is ignored provided automatically by this utility.
        // See msdn.com.

    static BOOL stackWalk64(DWORD                            arg1,
                            NullArg,
                            HANDLE                           arg3,
                            LPSTACKFRAME64                   arg4,
                            PVOID                            arg5,
                            NullArg,
                            NullArg,
                            NullArg,
                            NullArg);
        // Behavior is identical to Windows 'StackWalk64', except that the 2nd,
        // 6th, 7th, 8th, and 9th of the 9 arguments of that function are
        // ignored and provided automatically by this facility.  See msdn.com.

    static bool loadedOK();
        // Verify that the dll has been successfully loaded.  For testing only.
};

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

inline
bcemt_QLock& baesu_Dbghelp::qLock()
{
    static bcemt_QLock qLockInstance = BCEMT_QLOCK_INITIALIZER;

    return s_qLock;
}

}  // close namespace BloombergLP

// WINDOWS
#endif

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
