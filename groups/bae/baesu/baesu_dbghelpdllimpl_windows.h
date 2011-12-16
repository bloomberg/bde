// baesu_dbghelpdllimpl_windows.h                                     -*-C++-*-
#ifndef INCLUDED_BAESU_DBGHELPDLLIMPL_WINDOWS
#define INCLUDED_BAESU_DBGHELPDLLIMPL_WINDOWS

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
//@DESCRIPTION: This component governs the use of the 'dbghelp.dll' shared
// library that is provided by the Windows operating system.  On that platform,
// the baesu stack trace facility is built on top of the functions provided by
// that dll, which is not linked with.  This component explicitly loads and
// initializes the shared library (once) when it is needed, and provides access
// to the desired functions inside it.  Upon process termination, this
// component cleans up datastructures and unloads the shared library to avoid
// purify reporting memory leaks.  The functions in 'dbghelp.dll' are not
// thread-safe, so this component has a built-in mutex (a 'bcemt_QLock') that
// is exposed in the interface and that must be locked anytime any of the
// functions in this component are called.
//
// Note that this component is entirely internal to the stack trace facility,
// it is not to be seen or accessed by end users of the stack trace facility.
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

class baesu_DbghelpDllImpl_Windows {
    // Namespace class used for accessing functions in the Windows
    // 'dbghelp.dll' shared library used by the stack trace facility.  Since
    // this shared library is not normally linked with, this component manages
    // the loading and initializing of the code.  The static functions
    // 'symSetOptions', 'symFromAddr', 'symGetSymFromAddr64',
    // 'symGetLineFromAddr64', and 'stackWalk64' simply call the corresponding
    // Windows functions 'SymSetOptions', 'SymFromAddr', 'SymGetSymFromAddr64',
    // 'SymGetLineFromAddr64', and 'StackWalk64', respectively.  The names,
    // function, and ordering of the arguments is the same as for their Windows
    // counterparts except that in most cases some arguments are elided and
    // automatically provided by this facility to the underlying Windows calls.

    // DATA
    static bcemt_QLock s_qLock;     // Mutex to prevent more than one of the
                                    // non thread-safe functions in the dll
                                    // from running at the same time.

  public:
    // CLASS METHODS
    static bool isLoaded();
        // Verify that the dll has been successfully loaded.  For testing only.

    static bcemt_QLock& qLock();
        // Return a reference to the qLock, a form of mutex that is used to
        // govern access to the 'dbghelp.dll' library, which is not thread
        // safe.  This qlock must be locked before any of the functions
        // corresponding to any of the 'dbghelp.dll' functions are called.

          // -----------------------------------------------------------
          // Functions corresponding to Windows 'dbghelp.dll' functions:
          // -----------------------------------------------------------

    static DWORD symSetOptions(DWORD symOptions);
        // Behavior is identical to Windows 'SymSetOptions'.  See msdn.com.
        // Set the various 'symOptions' flags affecting subsequent calls to the
        // 'dbghelp.dll' library, and return the options flags.  The
        // 'symOptions' arg consists of 'SYMOPT_*' flags bitwise-OR'ed
        // together.  See msdn.com for more information on 'SymSetOptions'.

#ifdef BSLS_PLATFORM__CPU_32_BIT
    // For some reason, 'symFromAddr' doesn't work right on 64 bit, so we
    // #ifdef everything and use 'SymGetSymFromAddr64'.

    static BOOL symFromAddr(DWORD64      address,
                            PDWORD64     displacement,
                            PSYMBOL_INFO symbol);
        // Behavior is identical to Windows 'SymFromAddr', except that the
        // 'hProcess' arg is elided and provided automatically to the
        // underlying 'SymFromAddr' call by this utility.  Return the symbol
        // for the function containing the code at location 'address',
        // 'displacement' is set to the displacement from the actual symbol,
        // and 'symbol' is a pointer to a struct set to contain the actual
        // symbol.  Return 'true' on success and 'false' otherwise.  Note that
        // 'MaxNameLen' in '*symbol' must be set, prior to the call, to the
        // maximum symbol length.  See msdn.com for more information on
        // 'symFromAddr'.
#else
    static BOOL symGetSymFromAddr64(DWORD64            address,
                                    PDWORD64           displacement,
                                    PIMAGEHLP_SYMBOL64 symbol);
        // Behavior is identical to Windows 'SymGetSymFromAddr64', except that
        // the 'hProcess' arg is elided and provided automatically to the
        // underlying 'SymGetSymFromAddr64' call by the underlying facility.
        // Return the name of the function containing the code at location
        // 'address'.  'displacement' is set by this function to the
        // displacement of 'address' from the symbol found, and 'symbol' is a
        // pointer to a struct that is set to the symbol that is found.  Return
        // 'true' on success and 'false' otherwise.  Note that 'MaxNameLength'
        // in '*symbol' must be set prior to this function being called.  See
        // msdn.com for more information on 'SymGetSymFromAddr64'.
#endif
    static BOOL symGetLineFromAddr64(DWORD64          dwAddr,
                                     PDWORD           pdwDisplacement,
                                     PIMAGEHLP_LINE64 line);
        // Behavior is identical to Windows 'SymGetLineFromAddr64', except that
        // the 'hProcess' arg is elided, provided automatically by this utility
        // and passed to the underlying 'SymGetLineFromAddr64' call.  Return
        // the source line number corresponding to the code at location
        // 'dwAddr'.  Return the displacement from the beginning of the line in
        // 'pdwDisplacement', and return the source file name and line number
        // in the struct pointed at by 'line'.  Return 'true' on success and
        // 'false' otherwise.  See msdn.com for more information on
        // 'SymGetLineFromAddr64'.

    static BOOL stackWalk64(DWORD          machineType,
                            HANDLE         hThread,
                            LPSTACKFRAME64 stackFrame,
                            PVOID          contextRecord);
        // Behavior is identical to Windows 'StackWalk64', except that the
        // 'hProcess', 'ReadMemoryRoutine', 'FunctionTableAccessRoutine',
        // 'GetModuleBaseRoutine', and 'TranslateAddressRoutine' args are
        // elided, provided automatically by this utility to the underlying
        // 'StackWalk64' call.  'machineType' is a constant indicating whether
        // the executable is 32 or 64 bit, 'hThread' is a handle for the
        // current thread, 'stackFrame' points to a struct that must be
        // properly initialized before the call and that will contain the
        // return program counter, and 'contextRecord' refers to a Windows
        // 'CONTEXT' that must be properly initialized before the call.
        // Consequetive calls to this function will return consequetive frames
        // off the stack.  Return 'true' on success and 'false' otherwise or if
        // the end of the stack is reached.
};

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

inline
bcemt_QLock& baesu_DbghelpDllImpl_Windows::qLock()
{
    return s_qLock;
}

}  // close namespace BloombergLP

// WINDOWS
#endif

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
